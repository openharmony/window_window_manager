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
#include <transaction/rs_transaction.h>

#include "color_parser.h"
#include "display_manager.h"
#include "permission.h"
#include "session/container/include/window_event_channel.h"
#include "session_manager/include/session_manager.h"
#include "singleton_container.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_math.h"
#include "session_manager_agent_controller.h"

#include "window_session_impl.h"

namespace OHOS {
namespace Rosen {
union WSColorParam {
#if BIG_ENDIANNESS
    struct {
        uint8_t alpha;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } argb;
#else
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    } argb;
#endif
    uint32_t value;
};
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
        type == WindowType::WINDOW_TYPE_VOICE_INTERACTION || type == WindowType::WINDOW_TYPE_POINTER ||
        type == WindowType::WINDOW_TYPE_TOAST || type == WindowType::WINDOW_TYPE_DRAGGING_EFFECT)) {
        WLOGFW("Invalid type: %{public}u", GetType());
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

sptr<WindowSessionImpl> WindowSceneSessionImpl::FindMainWindowWithContext()
{
    if (GetType() != WindowType::WINDOW_TYPE_DIALOG) {
        return nullptr;
    }

    for (const auto& winPair : windowSessionMap_) {
        auto win = winPair.second.second;
        if (win && win->GetType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
            context_.get() == win->GetContext().get()) {
            return win;
        }
    }
    WLOGFE("Can not find main window to bind dialog!");
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
    if (WindowHelper::IsSubWindow(GetType())) { // sub window
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
            WLOGFI("property_ set parentPersistentId: %{public}" PRIu64 "", property_->GetParentPersistentId());
        }
        if (GetType() == WindowType::WINDOW_TYPE_DIALOG) {
            auto mainWindow = FindMainWindowWithContext();
            property_->SetParentPersistentId(mainWindow->GetPersistentId());
            WLOGFD("Bind dialog to main window");
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
        property_->GetWindowName().c_str(), property_->GetPersistentId(), GetType());
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::Create(const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<Rosen::ISession>& iSession)
{
    // allow iSession is nullptr when create window by innerkits
    if (!context) {
        WLOGFW("context is nullptr!");
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
        if (WindowHelper::IsSystemWindow(GetType())) {
            // Not valid system window type for session should return WMError::WM_OK;
            if (!IsValidSystemWindowType(GetType())) {
                return WMError::WM_OK;
            }
        } else if (!WindowHelper::IsSubWindow(GetType())) {
            return WMError::WM_ERROR_INVALID_TYPE;
        }
        ret = CreateAndConnectSpecificSession();
    }
    if (ret == WMError::WM_OK) {
        windowSessionMap_.insert(std::make_pair(property_->GetWindowName(),
            std::pair<uint64_t, sptr<WindowSessionImpl>>(property_->GetPersistentId(), this)));
        state_ = WindowState::STATE_CREATED;
        if (WindowHelper::IsMainWindow(GetType())) {
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
}

WMError WindowSceneSessionImpl::Show(uint32_t reason, bool withAnimation)
{
    WLOGFI("Window Show [name:%{public}s, id:%{public}" PRIu64 ", type: %{public}u], reason:%{public}u state:%{pubic}u",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), GetType(), reason, state_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_SHOWN) {
        WLOGFD("window session is alreay shown [name:%{public}s, id:%{public}" PRIu64 ", type: %{public}u]",
            property_->GetWindowName().c_str(), property_->GetPersistentId(), GetType());
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
            property_->GetWindowName().c_str(), property_->GetPersistentId(), GetType());
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
    if (state_ == WindowState::STATE_HIDDEN || state_ < WindowState::STATE_CREATED) {
        WLOGFD("Window is hidden or not created! id: %{public}" PRIu64 ", oriPos: [%{public}d, %{public}d, "
            "movePos: [%{public}d, %{public}d]", property_->GetPersistentId(), rect.posX_, rect.posY_, x, y);
        return WMError::WM_OK;
    }

    WSRect wsRect = { newRect.posX_, newRect.posY_, newRect.width_, newRect.height_ };
    const WSError& ret = hostSession_->UpdateSessionRect(wsRect, SizeChangeReason::MOVE);
    return static_cast<WMError>(ret);
}

void WindowSceneSessionImpl::LimitCameraFloatWindowMininumSize(uint32_t& width, uint32_t& height)
{
    // Float camera window has a special limit:
    // if display sw <= 600dp, portrait: min width = display sw * 30%, landscape: min width = sw * 50%
    // if display sw > 600dp, portrait: min width = display sw * 12%, landscape: min width = sw * 30%
    if (property_->GetWindowType() != WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
        return;
    }

    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64"", property_->GetDisplayId());
        return;
    }
    uint32_t displayWidth = static_cast<uint32_t>(display->GetWidth());
    uint32_t displayHeight = static_cast<uint32_t>(display->GetHeight());
    if (displayWidth == 0 || displayHeight == 0) {
        return;
    }
    float vpr = (displayWidth > 2700) ? 3.5 : 1.5; // display width: 2770; vpr: 3.5 / 1.5
    uint32_t smallWidth = displayHeight <= displayWidth ? displayHeight : displayWidth;
    float hwRatio = static_cast<float>(displayHeight) / static_cast<float>(displayWidth);
    uint32_t minWidth;
    if (smallWidth <= static_cast<uint32_t>(600 * vpr)) { // sw <= 600dp
        if (displayWidth <= displayHeight) {
            minWidth = static_cast<uint32_t>(smallWidth * 0.3); // ratio : 0.3
        } else {
            minWidth = static_cast<uint32_t>(smallWidth * 0.5); // ratio : 0.5
        }
    } else {
        if (displayWidth <= displayHeight) {
            minWidth = static_cast<uint32_t>(smallWidth * 0.12); // ratio : 0.12
        } else {
            minWidth = static_cast<uint32_t>(smallWidth * 0.3); // ratio : 0.3
        }
    }
    width = (width < minWidth) ? minWidth : width;
    height = static_cast<uint32_t>(width * hwRatio);
}

WMError WindowSceneSessionImpl::Resize(uint32_t width, uint32_t height)
{
    WLOGFD("Id:%{public}" PRIu64 " Resize %{public}u %{public}u", property_->GetPersistentId(), width, height);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    // Float camera window has special limits
    LimitCameraFloatWindowMininumSize(width, height);

    const auto& rect = property_->GetWindowRect();
    Rect newRect = { rect.posX_, rect.posY_, width, height }; // must keep w/h
    property_->SetRequestRect(newRect);
    if (state_ == WindowState::STATE_HIDDEN || state_ < WindowState::STATE_CREATED) {
        WLOGFD("Window is hidden or not created! id: %{public}" PRIu64 ", oriSize: [%{public}u, %{public}u, "
            "newSize [%{public}u, %{public}u], state: %{public}u", property_->GetPersistentId(), rect.width_,
            rect.height_, width, height, static_cast<uint32_t>(state_));
        return WMError::WM_OK;
    }

    WSRect wsRect = { newRect.posX_, newRect.posY_, newRect.width_, newRect.height_ };
    const WSError& ret = hostSession_->UpdateSessionRect(wsRect, SizeChangeReason::RESIZE);
    return static_cast<WMError>(ret);
}

WMError WindowSceneSessionImpl::SetAspectRatio(float ratio)
{
    if (property_ == nullptr) {
        WLOGFE("SetAspectRatio failed because of property is null");
        return  WMError::WM_ERROR_NULLPTR;
    }

    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64"", property_->GetDisplayId());
        return  WMError::WM_ERROR_NULLPTR;
    }
    float vpr = display->GetVirtualPixelRatio();
    auto limits = property_->GetWindowLimits();

    auto ToLayoutWidth = [&, vpr](int32_t winWidth) -> int32_t {
        return IsDecorEnable() ? (winWidth - WINDOW_FRAME_WIDTH * vpr * 2) : winWidth; // 2: left and right edge
    };

    auto ToLayoutHeight = [&, vpr](int32_t winHeight) -> int32_t {
        return IsDecorEnable() ? (winHeight - WINDOW_FRAME_WIDTH * vpr - WINDOW_TITLE_BAR_HEIGHT * vpr) : winHeight;
    };
    if (limits.minWidth_ != 0 && MathHelper::LessNotEqual(ratio,
        static_cast<float>(ToLayoutWidth(limits.minWidth_)) / ToLayoutHeight(limits.maxHeight_))) {
        WLOGE("Failed, because aspectRation is smaller than minWidth/maxHeight");
        return WMError::WM_ERROR_INVALID_PARAM;
    } else if (limits.minHeight_ != 0 && MathHelper::GreatNotEqual(ratio,
        static_cast<float>(ToLayoutWidth(limits.maxWidth_)) / ToLayoutHeight(limits.minHeight_))) {
        WLOGE("Failed, because aspectRation is bigger than maxWidth/minHeight");
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    Rect rect = property_->GetWindowRect();
    if (hostSession_) {
        float currentRatio = static_cast<float>(ToLayoutWidth(rect.width_)) / ToLayoutHeight(rect.height_);
        if (MathHelper::GreatNotEqual(currentRatio, ratio)) {
            rect.height_ = ToLayoutWidth(rect.width_) / ratio + WINDOW_FRAME_WIDTH * vpr +
                           WINDOW_TITLE_BAR_HEIGHT * vpr;
        } else if (MathHelper::LessNotEqual(currentRatio, ratio)) {
            rect.width_ = ToLayoutHeight(rect.height_) * ratio + WINDOW_FRAME_WIDTH * vpr * 2; // 2: left and right edge
        } else {
            return WMError::WM_DO_NOTHING;
        }
        Resize(rect.width_, rect.height_);
        hostSession_->SetAspectRatio(ratio);
        return WMError::WM_OK;
    } else {
        WLOGE("no host session found");
        return WMError::WM_ERROR_NULLPTR;
    }
}

WMError WindowSceneSessionImpl::ResetAspectRatio()
{
    if (hostSession_) {
        hostSession_->SetAspectRatio(0.0f);
        return WMError::WM_OK;
    } else {
        WLOGE("no host session found");
        return WMError::WM_ERROR_NULLPTR;
    }
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

WMError WindowSceneSessionImpl::GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea)
{
    uint32_t windowId = GetWindowId();
    WLOGFI("GetAvoidAreaByType windowId:%{public}u type:%{public}u", windowId, static_cast<uint32_t>(type));
    if (type != AvoidAreaType::TYPE_KEYBOARD &&
        windowMode_ != WindowMode::WINDOW_MODE_FULLSCREEN &&
        windowMode_ != WindowMode::WINDOW_MODE_SPLIT_PRIMARY &&
        windowMode_ != WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        WLOGI("avoidAreaType:%{public}u, windowMode:%{public}u, return default avoid area.",
            static_cast<uint32_t>(type), static_cast<uint32_t>(windowMode_));
        return WMError::WM_OK;
    }
    avoidArea = hostSession_->GetAvoidAreaByType(type);
    return WMError::WM_OK;
}

SystemBarProperty WindowSceneSessionImpl::GetSystemBarPropertyByType(WindowType type) const
{
    WLOGFI("GetSystemBarPropertyByType windowId:%{public}u type:%{public}u",
        GetWindowId(), static_cast<uint32_t>(type));
    auto curProperties = property_->GetSystemBarProperty();
    return curProperties[type];
}

WMError WindowSceneSessionImpl::NotifyWindowSessionProperty()
{
    WLOGFD("NotifyWindowSessionProperty called windowId:%{public}u", GetWindowId());
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS);
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    WLOGFI("SetSystemBarProperty windowId:%{public}u type:%{public}u"
        "enable:%{public}u bgColor:%{public}x Color:%{public}x",
        GetWindowId(), static_cast<uint32_t>(type),
        property.enable_, property.backgroundColor_, property.contentColor_);
    if (!((state_ > WindowState::STATE_INITIAL) && (state_ < WindowState::STATE_BOTTOM))) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    } else if (GetSystemBarPropertyByType(type) == property) {
        return WMError::WM_OK;
    }

    property_->SetSystemBarProperty(type, property);
    if (state_ == WindowState::STATE_CREATED || state_ == WindowState::STATE_HIDDEN) {
        return WMError::WM_OK;
    }
    WMError ret = NotifyWindowSessionProperty();
    if (ret != WMError::WM_OK) {
        WLOGFE("SetSystemBarProperty winId:%{public}u errCode:%{public}d",
            GetWindowId(), static_cast<int32_t>(ret));
    }
    return ret;
}

WMError WindowSceneSessionImpl::NotifyWindowNeedAvoid(bool status)
{
    WLOGFD("NotifyWindowNeedAvoid called windowId:%{public}u status:%{public}d",
        GetWindowId(), static_cast<int32_t>(status));
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    hostSession_->OnNeedAvoid(status);
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetLayoutFullScreenByApiVersion(bool status)
{
    uint32_t version = 0;
    if ((context_ != nullptr) && (context_->GetApplicationInfo() != nullptr)) {
        version = context_->GetApplicationInfo()->apiCompatibleVersion;
    }
    hostSession_->OnSessionEvent(SessionEvent::EVENT_MAXIMIZE);
    windowMode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
    // 10 ArkUI new framework support after API10
    if (version >= 10) {
        if (uiContent_ != nullptr) {
            uiContent_->SetIgnoreViewSafeArea(status);
        } else {
            isIgnoreSafeAreaNeedNotify_ = true;
            isIgnoreSafeArea_ = status;
        }
    } else {
        WMError ret = NotifyWindowNeedAvoid(!status);
        if (ret != WMError::WM_OK) {
            WLOGFE("NotifyWindowNeedAvoid errCode:%{public}d winId:%{public}u",
                static_cast<int32_t>(ret), GetWindowId());
            return ret;
        }
    }
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetLayoutFullScreen(bool status)
{
    WLOGFI("winId:%{public}u status:%{public}d", GetWindowId(), static_cast<int32_t>(status));
    WMError ret = SetLayoutFullScreenByApiVersion(status);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetLayoutFullScreenByApiVersion errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), GetWindowId());
    }
    return ret;
}

WMError WindowSceneSessionImpl::SetFullScreen(bool status)
{
    WLOGFI("winId:%{public}u status:%{public}d", GetWindowId(), static_cast<int32_t>(status));
    WMError ret = SetLayoutFullScreenByApiVersion(status);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetLayoutFullScreenByApiVersion errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), GetWindowId());
    }
    SystemBarProperty statusProperty = GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    statusProperty.enable_ = status;
    ret = SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, statusProperty);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetSystemBarProperty errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), GetWindowId());
    }
    return ret;
}

bool WindowSceneSessionImpl::IsDecorEnable() const
{
    bool enable = WindowHelper::IsMainWindow(GetType()) &&
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
    if (WindowHelper::IsMainWindow(GetType())) {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_MINIMIZE);
    }
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::Maximize()
{
    WLOGFD("WindowSceneSessionImpl::Maximize called");
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(GetType())) {
        SetFullScreen(true);
        UpdateDecorEnable(true);
    }
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::MaximizeFloating()
{
    WLOGFD("WindowSceneSessionImpl::MaximizeFloating called");
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!WindowHelper::IsMainWindow(property_->GetWindowType())) {
        WLOGFW("SetGlobalMaximizeMode fail, not main window");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (GetGlobalMaximizeMode() != MaximizeMode::MODE_AVOID_SYSTEM_BAR) {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_MAXIMIZE);
        SetFullScreen(true);
        windowMode_ = WindowMode::WINDOW_MODE_FULLSCREEN;
        UpdateDecorEnable(true);
        property_->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    } else {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_MAXIMIZE_FLOATING);
        windowMode_ = WindowMode::WINDOW_MODE_FLOATING;
        property_->SetMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR);
        UpdateDecorEnable(true);
    }
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE);

    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::Recover()
{
    WLOGFD("WindowSceneSessionImpl::Recover called");
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(GetType())) {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_RECOVER);
        windowMode_ = WindowMode::WINDOW_MODE_FLOATING;
        property_->SetMaximizeMode(MaximizeMode::MODE_RECOVER);
        UpdateDecorEnable(true);
        UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE);
    }
    return WMError::WM_OK;
}

void WindowSceneSessionImpl::StartMove()
{
    WLOGFD("WindowSceneSessionImpl::StartMove called");
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return;
    }
    if (WindowHelper::IsMainWindow(GetType())) {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_START_MOVE);
    }
    return;
}

WMError WindowSceneSessionImpl::Close()
{
    WLOGFD("WindowSceneSessionImpl::Close called");
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(GetType())) {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_CLOSE);
    }

    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::DisableAppWindowDecor()
{
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        WLOGFE("disable app window decor permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    if (!WindowHelper::IsMainWindow(GetType())) {
        WLOGFE("window decoration is invalid on sub window");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    WLOGI("disable app window decoration.");
    windowSystemConfig_.isSystemDecorEnable_ = false;
    UpdateDecorEnable(true);
    return WMError::WM_OK;
}

WSError WindowSceneSessionImpl::HandleBackEvent()
{
    bool isConsumed = false;
    if (uiContent_) {
        WLOGFD("Transfer back event to uiContent");
        isConsumed = uiContent_->ProcessBackPressed();
    } else {
        WLOGFE("There is no back event consumer");
    }
    if (isConsumed) {
        WLOGD("Back key event is consumed");
        return WSError::WS_OK;
    }
    // notify back event to host session
    if (hostSession_) {
        WLOGFD("Transfer back event to host session");
        hostSession_->RequestSessionBack();
    }
    return WSError::WS_OK;
}

WMError WindowSceneSessionImpl::SetGlobalMaximizeMode(MaximizeMode mode)
{
    WLOGFD("WindowSceneSessionImpl::SetGlobalMaximizeMode %{public}u", static_cast<uint32_t>(mode));
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(property_->GetWindowType())) {
        hostSession_->SetGlobalMaximizeMode(mode);
        return WMError::WM_OK;
    } else {
        WLOGFW("SetGlobalMaximizeMode fail, not main window");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
}

MaximizeMode WindowSceneSessionImpl::GetGlobalMaximizeMode() const
{
    WLOGFD("WindowSceneSessionImpl::GetGlobalMaximizeMode");
    MaximizeMode mode = MaximizeMode::MODE_RECOVER;
    hostSession_->GetGlobalMaximizeMode(mode);
    return mode;
}

WindowMode WindowSceneSessionImpl::GetMode() const
{
    return windowMode_;
}

bool WindowSceneSessionImpl::IsTransparent() const
{
    WSColorParam backgroundColor;
    backgroundColor.value = GetBackgroundColor();
    WLOGFD("color: %{public}u, alpha: %{public}u", backgroundColor.value, backgroundColor.argb.alpha);
    return backgroundColor.argb.alpha == 0x00; // 0x00: completely transparent
}

WMError WindowSceneSessionImpl::SetTransparent(bool isTransparent)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    WSColorParam backgroundColor;
    backgroundColor.value = GetBackgroundColor();
    if (isTransparent) {
        backgroundColor.argb.alpha = 0x00; // 0x00: completely transparent
        return SetBackgroundColor(backgroundColor.value);
    } else {
        backgroundColor.value = GetBackgroundColor();
        if (backgroundColor.argb.alpha == 0x00) {
            backgroundColor.argb.alpha = 0xff; // 0xff: completely opaque
            return SetBackgroundColor(backgroundColor.value);
        }
    }
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::AddWindowFlag(WindowFlag flag)
{
    uint32_t updateFlags = property_->GetWindowFlags() | (static_cast<uint32_t>(flag));
    return SetWindowFlags(updateFlags);
}

WMError WindowSceneSessionImpl::RemoveWindowFlag(WindowFlag flag)
{
    uint32_t updateFlags = property_->GetWindowFlags() & (~(static_cast<uint32_t>(flag)));
    return SetWindowFlags(updateFlags);
}

WMError WindowSceneSessionImpl::SetWindowFlags(uint32_t flags)
{
    WLOGI("Session %{public}u flags %{public}u", GetWindowId(), flags);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (property_->GetWindowFlags() == flags) {
        return WMError::WM_OK;
    }
    auto oriFlags = property_->GetWindowFlags();
    property_->SetWindowFlags(flags);
    if (state_ == WindowState::STATE_CREATED || state_ == WindowState::STATE_HIDDEN) {
        return WMError::WM_OK;
    }
    WMError ret = UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_FLAGS);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetWindowFlags errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), GetWindowId());
        property_->SetWindowFlags(oriFlags);
    }
    return ret;
}

uint32_t WindowSceneSessionImpl::GetWindowFlags() const
{
    return property_->GetWindowFlags();
}

static float ConvertRadiusToSigma(float radius)
{
    return radius > 0.0f ? 0.57735f * radius + SK_ScalarHalf : 0.0f; // 0.57735f is blur sigma scale
}

WMError WindowSceneSessionImpl::CheckParmAndPermission()
{
    if (surfaceNode_ == nullptr) {
        WLOGFE("RSSurface node is null");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        WLOGFE("Check failed, permission denied");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }

    return WMError::WM_OK;
}

void WindowSceneSessionImpl::UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    if (uiContent_ != nullptr) {
        WLOGFD("notify ace winId:%{public}u", GetWindowId());
        uiContent_->UpdateConfiguration(configuration);
    }
    if (subWindowSessionMap_.count(GetPersistentId()) == 0) {
        return;
    }
    for (auto& subWindowSession : subWindowSessionMap_.at(GetPersistentId())) {
        subWindowSession->UpdateConfiguration(configuration);
    }
    
}
void WindowSceneSessionImpl::UpdateConfigurationForAll(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    WLOGD("notify scene ace update config");
    for (const auto& winPair : windowSessionMap_) {
        auto window = winPair.second.second;
        window->UpdateConfiguration(configuration);
    }
}

WMError WindowSceneSessionImpl::SetCornerRadius(float cornerRadius)
{
    if (surfaceNode_ == nullptr) {
        WLOGFE("RSSurface node is null");
        return WMError::WM_ERROR_NULLPTR;
    }

    WLOGFI("Set window %{public}s corner radius %{public}f", GetWindowName().c_str(), cornerRadius);
    surfaceNode_->SetCornerRadius(cornerRadius);
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetShadowRadius(float radius)
{
    WMError ret = CheckParmAndPermission();
    if (ret != WMError::WM_OK) {
        return ret;
    }

    WLOGFI("Set window %{public}s shadow radius %{public}f", GetWindowName().c_str(), radius);
    if (MathHelper::LessNotEqual(radius, 0.0)) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    surfaceNode_->SetShadowRadius(radius);
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetShadowColor(std::string color)
{
    WMError ret = CheckParmAndPermission();
    if (ret != WMError::WM_OK) {
        return ret;
    }

    WLOGFI("Set window %{public}s shadow color %{public}s", GetWindowName().c_str(), color.c_str());
    uint32_t colorValue = 0;
    if (!ColorParser::Parse(color, colorValue)) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    surfaceNode_->SetShadowColor(colorValue);
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetShadowOffsetX(float offsetX)
{
    WMError ret = CheckParmAndPermission();
    if (ret != WMError::WM_OK) {
        return ret;
    }

    WLOGFI("Set window %{public}s shadow offsetX %{public}f", GetWindowName().c_str(), offsetX);
    surfaceNode_->SetShadowOffsetX(offsetX);
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetShadowOffsetY(float offsetY)
{
    WMError ret = CheckParmAndPermission();
    if (ret != WMError::WM_OK) {
        return ret;
    }

    WLOGFI("Set window %{public}s shadow offsetY %{public}f", GetWindowName().c_str(), offsetY);
    surfaceNode_->SetShadowOffsetY(offsetY);
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetBlur(float radius)
{
    WMError ret = CheckParmAndPermission();
    if (ret != WMError::WM_OK) {
        return ret;
    }

    WLOGFI("Set window %{public}s blur radius %{public}f", GetWindowName().c_str(), radius);
    if (MathHelper::LessNotEqual(radius, 0.0)) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    radius = ConvertRadiusToSigma(radius);
    WLOGFI("Set window %{public}s blur radius after conversion %{public}f", GetWindowName().c_str(), radius);
    surfaceNode_->SetFilter(RSFilter::CreateBlurFilter(radius, radius));
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetBackdropBlur(float radius)
{
    WMError ret = CheckParmAndPermission();
    if (ret != WMError::WM_OK) {
        return ret;
    }

    WLOGFI("Set window %{public}s backdrop blur radius %{public}f", GetWindowName().c_str(), radius);
    if (MathHelper::LessNotEqual(radius, 0.0)) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    radius = ConvertRadiusToSigma(radius);
    WLOGFI("Set window %{public}s backdrop blur radius after conversion %{public}f", GetWindowName().c_str(), radius);
    surfaceNode_->SetBackgroundFilter(RSFilter::CreateBlurFilter(radius, radius));
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetBackdropBlurStyle(WindowBlurStyle blurStyle)
{
    WMError ret = CheckParmAndPermission();
    if (ret != WMError::WM_OK) {
        return ret;
    }

    WLOGFI("Set window %{public}s backdrop blur style %{public}u", GetWindowName().c_str(), blurStyle);
    if (blurStyle < WindowBlurStyle::WINDOW_BLUR_OFF || blurStyle > WindowBlurStyle::WINDOW_BLUR_THICK) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    if (blurStyle == WindowBlurStyle::WINDOW_BLUR_OFF) {
        surfaceNode_->SetBackgroundFilter(nullptr);
    } else {
        float density = 3.5f; // get density from screen property
        surfaceNode_->SetBackgroundFilter(RSFilter::CreateMaterialFilter(static_cast<int>(blurStyle), density));
    }

    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}


WMError WindowSceneSessionImpl::SetPrivacyMode(bool isPrivacyMode)
{
    WLOGFD("id : %{public}u, SetPrivacyMode, %{public}u", GetWindowId(), isPrivacyMode);
    property_->SetPrivacyMode(isPrivacyMode);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE);
}

bool WindowSceneSessionImpl::IsPrivacyMode() const
{
    return property_->GetPrivacyMode();
}

void WindowSceneSessionImpl::SetSystemPrivacyMode(bool isSystemPrivacyMode)
{
    WLOGFD("id : %{public}u, SetSystemPrivacyMode, %{public}u", GetWindowId(), isSystemPrivacyMode);
    property_->SetSystemPrivacyMode(isSystemPrivacyMode);
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE);
}

WMError WindowSceneSessionImpl::SetSnapshotSkip(bool isSkip)
{
    if (!Permission::IsSystemCalling() && !Permission::IsStartByHdcd()) {
        WLOGFE("set snapshot skip permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    surfaceNode_->SetSecurityLayer(isSkip || property_->GetSystemPrivacyMode());
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}
} // namespace Rosen
} // namespace OHOS
