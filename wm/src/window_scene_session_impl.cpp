/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "window_scene_session_impl.h"

#include <parameters.h>

#include "permission.h"
#include "session/container/include/window_event_channel.h"
#include "session_manager/include/session_manager.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

#include "window_session_impl.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowSceneSessionImpl"};
}

WindowSceneSessionImpl::WindowSceneSessionImpl(const sptr<WindowOption>& option) : WindowSessionImpl(option)
{
}

WindowSceneSessionImpl::~WindowSceneSessionImpl()
{
}

bool WindowSceneSessionImpl::IsValidSystemWindowType(const WindowType& type)
{
    if (!(type == WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW || type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT ||
        type == WindowType::WINDOW_TYPE_FLOAT_CAMERA || type == WindowType::WINDOW_TYPE_DIALOG ||
        type == WindowType::WINDOW_TYPE_FLOAT || type == WindowType::WINDOW_TYPE_SCREENSHOT ||
        type == WindowType::WINDOW_TYPE_VOICE_INTERACTION)) {
        return false;
    }
    return true;
}

sptr<WindowSessionImpl> WindowSceneSessionImpl::FindParentSessionByParentId(uint32_t parentId)
{
    for (const auto& item : windowSessionMap_) {
        if (item.second.second && item.second.second->GetProperty() &&
            item.second.second->GetWindowId() == parentId &&
            WindowHelper::IsMainWindow(item.second.second->GetType())) {
            WLOGFD("Find parent, [parentName: %{public}s, parentId:%{public}u, selfPersistentId: %{public}" PRIu64"]",
                item.second.second->GetProperty()->GetWindowName().c_str(), parentId, GetProperty()->GetPersistentId());
            return item.second.second;
        }
    }
    WLOGFD("Can not find parent window");
    return nullptr;
}

WMError WindowSceneSessionImpl::CreateAndConnectSpecificSession()
{
    sptr<ISessionStage> iSessionStage(this);
    sptr<WindowEventChannel> channel = new (std::nothrow) WindowEventChannel(iSessionStage);
    if (channel == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    sptr<IWindowEventChannel> eventChannel(channel);
    uint64_t persistentId = INVALID_SESSION_ID;
    sptr<Rosen::ISession> session;
    if (WindowHelper::IsSubWindow(property_->GetWindowType())) { // sub window
        auto parentSession = FindParentSessionByParentId(property_->GetParentId());
        if (parentSession == nullptr || parentSession->GetHostSession() == nullptr) {
            return WMError::WM_ERROR_NULLPTR;
        }
        // set parent persistentId
        property_->SetParentPersistentId(parentSession->GetPersistentId());
        // creat sub session by parent session
        parentSession->GetHostSession()->CreateAndConnectSpecificSession(iSessionStage, eventChannel, surfaceNode_,
            property_, persistentId, session);
        // update subWindowSessionMap_
        subWindowSessionMap_[parentSession->GetPersistentId()].push_back(this);
    } else { // system window
        if (WindowHelper::IsAppFloatingWindow(GetType())) {
            property_->SetParentPersistentId(GetFloatingWindowParentId());
            WLOGFI("WindowSessionImpl set SetParentPersistentId: %{public}d", property_->GetParentPersistentId());
        }
        SessionManager::GetInstance().CreateAndConnectSpecificSession(iSessionStage, eventChannel, surfaceNode_,
            property_, persistentId, session);
    }
    property_->SetPersistentId(persistentId);
    if (session != nullptr) {
        hostSession_ = session;
    } else {
        return WMError::WM_ERROR_NULLPTR;
    }
    WLOGFI("CreateAndConnectSpecificSession [name:%{public}s, id:%{public}" PRIu64 ", type: %{public}u]",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), property_->GetWindowType());
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::Create(const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<Rosen::ISession>& iSession)
{
    // allow iSession is nullptr when create from window manager
    if (!context) {
        WLOGFE("context is nullptr!");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    WMError ret = WindowSessionCreateCheck();
    if (ret != WMError::WM_OK) {
        return ret;
    }
    hostSession_ = iSession;
    context_ = context;
    if (hostSession_) { // main window
        ret = Connect();
    } else { // system or sub window
        if (WindowHelper::IsSystemWindow(property_->GetWindowType())) {
            // Not valid system window type for session should return WMError::WM_OK;
            if (!IsValidSystemWindowType(property_->GetWindowType())) {
                return WMError::WM_OK;
            }
        } else if (!WindowHelper::IsSubWindow(property_->GetWindowType())) {
            return WMError::WM_ERROR_INVALID_TYPE;
        }
        ret = CreateAndConnectSpecificSession();
    }
    if (ret == WMError::WM_OK) {
        windowSessionMap_.insert(std::make_pair(property_->GetWindowName(),
            std::pair<uint64_t, sptr<WindowSessionImpl>>(property_->GetPersistentId(), this)));
        state_ = WindowState::STATE_CREATED;
        if (WindowHelper::IsMainWindow(property_->GetWindowType())) {
            windowMode_ = windowSystemConfig_.defaultWindowMode_;
        }
    }
    WLOGFD("Window Create [name:%{public}s, id:%{public}" PRIu64 "], state:%{pubic}u, windowmode:%{public}u",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), state_, windowMode_);
    return ret;
}

void WindowSceneSessionImpl::UpdateSubWindowStateAndNotify(uint64_t parentPersistentId, const WindowState& newState)
{
    auto iter = subWindowSessionMap_.find(parentPersistentId);
    if (iter == subWindowSessionMap_.end()) {
        WLOGFD("main window: %{public}" PRIu64" has no child node", parentPersistentId);
        return;
    }
    const auto& subWindows = iter->second;
    if (subWindows.empty()) {
        WLOGFD("main window: %{public}" PRIu64", its subWindowMap is empty", parentPersistentId);
        return;
    }

    // when main window hide and subwindow whose state is shown should hide and notify user
    if (newState == WindowState::STATE_HIDDEN) {
        for (auto subwindow : subWindows) {
            if (subwindow != nullptr && subwindow->GetWindowState() == WindowState::STATE_SHOWN) {
                subwindow->NotifyAfterBackground();
                subwindow->state_ = WindowState::STATE_HIDDEN;
            }
        }
    // when main window show and subwindow whose state is shown should show and notify user
    } else if (newState == WindowState::STATE_SHOWN) {
        for (auto subwindow : subWindows) {
            if (subwindow != nullptr && subwindow->GetWindowState() == WindowState::STATE_HIDDEN) {
                subwindow->NotifyAfterForeground();
                subwindow->state_ = WindowState::STATE_SHOWN;
            }
        }
    }
    return;
}

WMError WindowSceneSessionImpl::Show(uint32_t reason, bool withAnimation)
{
    WLOGFI("Window Show [name:%{public}s, id:%{public}" PRIu64 ", type: %{public}u], reason:%{public}u state:%{pubic}u",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), property_->GetWindowType(), reason, state_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_SHOWN) {
        WLOGFD("window session is alreay shown [name:%{public}s, id:%{public}" PRIu64 ", type: %{public}u]",
            property_->GetWindowName().c_str(), property_->GetPersistentId(), property_->GetWindowType());
        return WMError::WM_OK;
    }
    if (hostSession_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }

    WSError ret = hostSession_->Foreground();
    // delete after replace WSError with WMError
    WMError res = static_cast<WMError>(ret);
    if (res == WMError::WM_OK) {
        // update sub window state if this is main window
        if (WindowHelper::IsMainWindow(GetType())) {
            UpdateSubWindowStateAndNotify(GetPersistentId(), WindowState::STATE_SHOWN);
        }
        NotifyAfterForeground();
        state_ = WindowState::STATE_SHOWN;
    } else {
        NotifyForegroundFailed(res);
    }
    return res;
}

WMError WindowSceneSessionImpl::Hide(uint32_t reason, bool withAnimation, bool isFromInnerkits)
{
    WLOGFI("id:%{public}" PRIu64 " Hide, reason:%{public}u, state:%{public}u",
        property_->GetPersistentId(), reason, state_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        WLOGFD("window session is alreay hidden [name:%{public}s, id:%{public}" PRIu64 ", type: %{public}u]",
            property_->GetWindowName().c_str(), property_->GetPersistentId(), property_->GetWindowType());
        return WMError::WM_OK;
    }
    WSError ret = WSError::WS_OK;
    if (!WindowHelper::IsMainWindow(GetType())) {
        // main window no need to notify host, since host knows hide first
        // need to SetActive(false) for host session before background
        ret = SetActive(false);
        if (ret != WSError::WS_OK) {
            return static_cast<WMError>(ret);
        }
        ret = hostSession_->Background();
    }

    // delete after replace WSError with WMError
    WMError res = static_cast<WMError>(ret);
    if (res == WMError::WM_OK) {
        // update sub window state if this is main window
        if (WindowHelper::IsMainWindow(GetType())) {
            UpdateSubWindowStateAndNotify(GetPersistentId(), WindowState::STATE_HIDDEN);
        }
        NotifyAfterBackground();
        state_ = WindowState::STATE_HIDDEN;
    }
    return res;
}

WSError WindowSceneSessionImpl::SetActive(bool active)
{
    WLOGFD("active status: %{public}d", active);
    if (!WindowHelper::IsMainWindow(GetType())) {
        WSError ret = hostSession_->UpdateActiveStatus(active);
        if (ret != WSError::WS_OK) {
            return ret;
        }
    }
    if (active) {
        NotifyAfterActive();
    } else {
        NotifyAfterInactive();
    }
    return WSError::WS_OK;
}

void WindowSceneSessionImpl::DestroySubWindow()
{
    for (auto elem : subWindowSessionMap_) {
        WLOGFE("Id: %{public}" PRIu64 ", size: %{public}zu", elem.first, subWindowSessionMap_.size());
    }

    const uint64_t& parentPersistentId = property_->GetParentPersistentId();
    const uint64_t& persistentId = GetPersistentId();

    WLOGFD("Id: %{public}" PRIu64 ", parentId: %{public}" PRIu64 "", persistentId, parentPersistentId);

    // remove from subWindowMap_ when destroy sub window
    auto subIter = subWindowSessionMap_.find(parentPersistentId);
    if (subIter != subWindowSessionMap_.end()) {
        auto& subWindows = subIter->second;
        for (auto iter = subWindows.begin(); iter != subWindows.end();) {
            if ((*iter) == nullptr) {
                iter++;
                continue;
            }
            if ((*iter)->GetPersistentId() == persistentId) {
                WLOGFD("Destroy sub window, persistentId: %{public}" PRIu64 "", persistentId);
                iter = subWindows.erase(iter);
                break;
            }
        }
    }

    // remove from subWindowMap_ when destroy main window
    auto mainIter = subWindowSessionMap_.find(persistentId);
    if (mainIter != subWindowSessionMap_.end()) {
        auto& subWindows = mainIter->second;
        for (auto iter = subWindows.begin(); iter != subWindows.end();) {
            if ((*iter) == nullptr) {
                WLOGFD("Destroy sub window which is nullptr");
                iter = subWindows.erase(iter);
                continue;
            }
            WLOGFD("Destroy sub window, persistentId: %{public}" PRIu64 "", (*iter)->GetPersistentId());
            (*iter)->Destroy(false);
            iter++;
        }
        mainIter->second.clear();
        subWindowSessionMap_.erase(mainIter);
    }
}

WMError WindowSceneSessionImpl::Destroy(bool needClearListener)
{
    WLOGFI("Id:%{public}" PRIu64 " Destroy, state_:%{public}u", property_->GetPersistentId(), state_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_OK;
    }
    WSError ret = WSError::WS_OK;
    if (!WindowHelper::IsMainWindow(GetType())) {
        if (WindowHelper::IsSystemWindow(GetType())) {
            // main window no need to notify host, since host knows hide first
            SessionManager::GetInstance().DestroyAndDisconnectSpecificSession(property_->GetPersistentId());
        } else if (WindowHelper::IsSubWindow(GetType())) {
            auto parentSession = FindParentSessionByParentId(GetParentId());
            if (parentSession == nullptr || parentSession->GetHostSession() == nullptr) {
                return WMError::WM_ERROR_NULLPTR;
            }
            parentSession->GetHostSession()->DestroyAndDisconnectSpecificSession(property_->GetPersistentId());
        }
    }
    // delete after replace WSError with WMError
    WMError res = static_cast<WMError>(ret);
    NotifyBeforeDestroy(GetWindowName());
    if (needClearListener) {
        ClearListenersById(GetPersistentId());
    }
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        state_ = WindowState::STATE_DESTROYED;
    }

    DestroySubWindow();
    windowSessionMap_.erase(property_->GetWindowName());
    hostSession_ = nullptr;
    return res;
}

WMError WindowSceneSessionImpl::MoveTo(int32_t x, int32_t y)
{
    WLOGFD("Id:%{public}" PRIu64 " MoveTo %{public}d %{public}d", property_->GetPersistentId(), x, y);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    const auto& rect = property_->GetWindowRect();
    Rect newRect = { x, y, rect.width_, rect.height_ }; // must keep x/y
    property_->SetRequestRect(newRect);
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        WLOGFD("Window is hidden or created! id: %{public}" PRIu64 ", oriPos: [%{public}d, %{public}d, "
            "movePos: [%{public}d, %{public}d]", property_->GetPersistentId(), rect.posX_, rect.posY_, x, y);
        return WMError::WM_OK;
    }

    WSRect wsRect = { newRect.posX_, newRect.posY_, newRect.width_, newRect.height_ };
    const WSError& ret = hostSession_->UpdateSessionRect(wsRect, SizeChangeReason::MOVE);
    return static_cast<WMError>(ret);
}

WMError WindowSceneSessionImpl::Resize(uint32_t width, uint32_t height)
{
    WLOGFD("Id:%{public}" PRIu64 " Resize %{public}u %{public}u", property_->GetPersistentId(), width, height);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    const auto& rect = property_->GetWindowRect();
    Rect newRect = { rect.posX_, rect.posY_, width, height }; // must keep w/h
    property_->SetRequestRect(newRect);
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        WLOGFD("Window is hidden or created! id: %{public}" PRIu64 ", oriSize: [%{public}u, %{public}u, "
            "newSize [%{public}u, %{public}u]", property_->GetPersistentId(), rect.width_, rect.height_, width, height);
        return WMError::WM_OK;
    }

    WSRect wsRect = { newRect.posX_, newRect.posY_, newRect.width_, newRect.height_ };
    const WSError& ret = hostSession_->UpdateSessionRect(wsRect, SizeChangeReason::RESIZE);
    return static_cast<WMError>(ret);
}

WmErrorCode WindowSceneSessionImpl::RaiseToAppTop()
{
    auto parentId = GetParentId();
    if (parentId == INVALID_SESSION_ID) {
        WLOGFE("Only the children of the main window can be raised!");
        return WmErrorCode::WM_ERROR_INVALID_PARENT;
    }

    if (!WindowHelper::IsSubWindow(GetType())) {
        WLOGFE("Must be app sub window window!");
        return WmErrorCode::WM_ERROR_INVALID_CALLING;
    }

    if (state_ != WindowState::STATE_SHOWN) {
        WLOGFE("The sub window must be shown!");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }

    const WSError& ret = hostSession_->RaiseToAppTop();
    return static_cast<WmErrorCode>(ret);
}

bool WindowSceneSessionImpl::IsDecorEnable() const
{
    bool enable = WindowHelper::IsMainWindow(property_->GetWindowType()) &&
        windowSystemConfig_.isSystemDecorEnable_ &&
        WindowHelper::IsWindowModeSupported(windowSystemConfig_.decorModeSupportInfo_, GetMode());
    WLOGFD("get decor enable %{public}d", enable);
    return enable;
}

WMError WindowSceneSessionImpl::Minimize()
{
    WLOGFD("WindowSceneSessionImpl::Minimize called");
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    hostSession_->OnSessionEvent(SessionEvent::EVENT_MINIMIZE);
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::Maximize()
{
    WLOGFD("WindowSceneSessionImpl::Maximize called");
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    hostSession_->OnSessionEvent(SessionEvent::EVENT_MAXIMIZE);
    windowMode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
    UpdateDecorEnable(true);
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::Recover()
{
    WLOGFD("WindowSceneSessionImpl::Recover called");
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    hostSession_->OnSessionEvent(SessionEvent::EVENT_RECOVER);
    windowMode_ = WindowMode::WINDOW_MODE_FLOATING;
    UpdateDecorEnable(true);
    return WMError::WM_OK;
}

void WindowSceneSessionImpl::StartMove()
{
    WLOGFD("WindowSceneSessionImpl::StartMove called");
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return;
    }
    hostSession_->OnSessionEvent(SessionEvent::EVENT_START_MOVE);
    return;
}

WMError WindowSceneSessionImpl::Close()
{
    WLOGFD("WindowSceneSessionImpl::Close called");
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    hostSession_->OnSessionEvent(SessionEvent::EVENT_CLOSE);
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::DisableAppWindowDecor()
{
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        WLOGFE("disable app window decor permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    if (!WindowHelper::IsMainWindow(property_->GetWindowType())) {
        WLOGFE("window decoration is invalid on sub window");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    WLOGI("disable app window decoration.");
    windowSystemConfig_.isSystemDecorEnable_ = false;
    UpdateDecorEnable(true);
    return WMError::WM_OK;
}

WindowMode WindowSceneSessionImpl::GetMode() const
{
    return windowMode_;
}
} // namespace Rosen
} // namespace OHOS

