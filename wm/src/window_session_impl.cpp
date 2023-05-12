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

#include "window_session_impl.h"

#include <common/rs_common_def.h>
#include <refbase.h>
#include <transaction/rs_interfaces.h>
#include <unistd.h>
#include "window_manager_hilog.h"
#include "window_helper.h"
#include "session/container/include/window_event_channel.h"
#include "session_manager/include/session_manager.h"
#include "vsync_station.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowSessionImpl"};
}

std::map<uint64_t, std::vector<sptr<IWindowLifeCycle>>> WindowSessionImpl::lifecycleListeners_;
std::map<uint64_t, std::vector<sptr<IWindowChangeListener>>> WindowSessionImpl::windowChangeListeners_;
std::recursive_mutex WindowSessionImpl::globalMutex_;
std::map<std::string, std::pair<uint64_t, sptr<WindowSessionImpl>>> WindowSessionImpl::windowSessionMap_;

#define CALL_LIFECYCLE_LISTENER(windowLifecycleCb, listeners) \
    do {                                                      \
        for (auto& listener : (listeners)) {                  \
            if (listener.GetRefPtr() != nullptr) {            \
                listener.GetRefPtr()->windowLifecycleCb();    \
            }                                                 \
        }                                                     \
    } while (0)

#define CALL_LIFECYCLE_LISTENER_WITH_PARAM(windowLifecycleCb, listeners, param) \
    do {                                                                        \
        for (auto& listener : (listeners)) {                                    \
            if (listener.GetRefPtr()) {                                         \
                listener.GetRefPtr()->windowLifecycleCb(param);                 \
            }                                                                   \
        }                                                                       \
    } while (0)

#define CALL_UI_CONTENT(uiContentCb)                          \
    do {                                                      \
        if (uiContent_ != nullptr) {                          \
            uiContent_->uiContentCb();                        \
        }                                                     \
    } while (0)

WindowSessionImpl::WindowSessionImpl(const sptr<WindowOption>& option)
{
    property_ = new (std::nothrow) WindowSessionProperty();
    if (property_ == nullptr) {
        WLOGFE("Property is null");
        return;
    }
    property_->SetWindowName(option->GetWindowName());
    property_->SetRequestRect(option->GetWindowRect());
    property_->SetWindowType(option->GetWindowType());
    property_->SetFocusable(option->GetFocusable());
    property_->SetTouchable(option->GetTouchable());
    property_->SetDisplayId(option->GetDisplayId());
    property_->SetWindowName(option->GetWindowName());
    surfaceNode_ = CreateSurfaceNode(property_->GetWindowName(), option->GetWindowType());
}

RSSurfaceNode::SharedPtr WindowSessionImpl::CreateSurfaceNode(std::string name, WindowType type)
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = name;
    RSSurfaceNodeType rsSurfaceNodeType = RSSurfaceNodeType::DEFAULT;
    switch (type) {
        case WindowType::WINDOW_TYPE_BOOT_ANIMATION:
        case WindowType::WINDOW_TYPE_POINTER:
            rsSurfaceNodeType = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
            break;
        case WindowType::WINDOW_TYPE_APP_MAIN_WINDOW:
            rsSurfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
            break;
        default:
            rsSurfaceNodeType = RSSurfaceNodeType::DEFAULT;
            break;
    }
    return RSSurfaceNode::Create(rsSurfaceNodeConfig, rsSurfaceNodeType);
}

WindowSessionImpl::~WindowSessionImpl()
{
    Destroy(false);
}

uint32_t WindowSessionImpl::GetWindowId() const
{
    // for get window property
    return static_cast<uint32_t>((property_->GetPersistentId()) & 0xffffffff); // 0xffffffff: to get lower 32 bits
}

bool WindowSessionImpl::IsWindowSessionInvalid() const
{
    bool res = ((hostSession_ == nullptr) || (GetPersistentId() == INVALID_SESSION_ID) ||
        (state_ == WindowState::STATE_DESTROYED));
    if (res) {
        WLOGW("already destroyed or not created! id: %{public}" PRIu64 "", GetPersistentId());
    }
    return res;
}

uint64_t WindowSessionImpl::GetPersistentId() const
{
    return property_->GetPersistentId();
}

WMError WindowSessionImpl::CreateAndConnectSpecificSession()
{
    sptr<ISessionStage> iSessionStage(this);
    sptr<WindowEventChannel> channel = new (std::nothrow) WindowEventChannel(iSessionStage);
    if (channel == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    sptr<IWindowEventChannel> eventChannel(channel);
    uint64_t persistentId = INVALID_SESSION_ID;
    sptr<Rosen::ISession> session;
    SessionManager::GetInstance().CreateAndConnectSpecificSession(iSessionStage, eventChannel, surfaceNode_,
        property_, persistentId, session);
    property_->SetPersistentId(persistentId);
    if (session != nullptr) {
        hostSession_ = session;
    } else {
        return WMError::WM_ERROR_NULLPTR;
    }
    WLOGFI("CreateAndConnectSpecificSession [name:%{public}s, id:%{public}" PRIu64 ", type: %{public}u], ",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), property_->GetWindowType());
    return WMError::WM_OK;
}

bool WindowSessionImpl::IsValidSystemWindowType(const WindowType& type)
{
    if (!(type == WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW || type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT ||
        type == WindowType::WINDOW_TYPE_FLOAT_CAMERA || type == WindowType::WINDOW_TYPE_DIALOG ||
        type == WindowType::WINDOW_TYPE_FLOAT || type == WindowType::WINDOW_TYPE_SCREENSHOT ||
        type == WindowType::WINDOW_TYPE_VOICE_INTERACTION)) {
        return false;
    }
    return true;
}

WMError WindowSessionImpl::WindowSessionCreateCheck()
{
    const auto& name = property_->GetWindowName();
    // check window name, same window names are forbidden
    if (windowSessionMap_.find(name) != windowSessionMap_.end()) {
        WLOGFE("WindowName(%{public}s) already exists.", name.c_str());
        return WMError::WM_ERROR_REPEAT_OPERATION;
    }

    // check if camera floating window is already exists
    if (property_->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
        for (const auto& item : windowSessionMap_) {
            if (item.second.second && item.second.second->property_ &&
                item.second.second->property_->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
                    WLOGFE("Camera floating window is already exists.");
                return WMError::WM_ERROR_REPEAT_OPERATION;
            }
        }
    }
    return WMError::WM_OK;
}

WMError WindowSessionImpl::Create(const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<Rosen::ISession>& iSession)
{
    // allow iSession is nullptr when create from window manager
    if (!context) {
        WLOGFE("context is nullptr: %{public}u", context == nullptr);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    hostSession_ = iSession;
    context_ = context;
    WMError ret = WMError::WM_OK;
    if (hostSession_) {
        ret = Connect();
        state_ = WindowState::STATE_CREATED;
    } else {
        // Not valid system window type for session should return WMError::WM_OK;
        if (!IsValidSystemWindowType(property_->GetWindowType())) {
            return WMError::WM_OK;
        }
        ret = WindowSessionCreateCheck();
        if (ret != WMError::WM_OK) {
            return ret;
        }
        ret = CreateAndConnectSpecificSession();
    }

    windowSessionMap_.insert(std::make_pair(property_->GetWindowName(),
        std::pair<uint64_t, sptr<WindowSessionImpl>>(property_->GetPersistentId(), this)));
    return ret;
}

WMError WindowSessionImpl::Connect()
{
    if (hostSession_ == nullptr) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    sptr<ISessionStage> iSessionStage(this);
    sptr<WindowEventChannel> channel = new (std::nothrow) WindowEventChannel(iSessionStage);
    if (channel == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    sptr<IWindowEventChannel> eventChannel(channel);
    uint64_t persistentId = INVALID_SESSION_ID;
    WSError ret = hostSession_->Connect(iSessionStage, eventChannel, surfaceNode_, persistentId, property_);
    property_->SetPersistentId(persistentId);
    // replace WSError with WMError
    WMError res = static_cast<WMError>(ret);
    WLOGFI("Window Connect [name:%{public}s, id:%{public}" PRIu64 ", type: %{public}u], ret:%{public}u",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), property_->GetWindowType(), res);
    return res;
}

WMError WindowSessionImpl::Show(uint32_t reason, bool withAnimation)
{
    WLOGFI("Window Show [name:%{public}s, id:%{public}" PRIu64 ", type: %{public}u], reason:%{public}u",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), property_->GetWindowType(), reason);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_SHOWN) {
        WLOGFD("window session is alreay shown [name:%{public}s, id:%{public}" PRIu64 ", type: %{public}u]",
            property_->GetWindowName().c_str(), property_->GetPersistentId(), property_->GetWindowType());
        return WMError::WM_OK;
    }

    WSError ret = hostSession_->Foreground();
    // delete after replace WSError with WMError
    WMError res = static_cast<WMError>(ret);
    if (res == WMError::WM_OK) {
        NotifyAfterForeground();
        state_ = WindowState::STATE_SHOWN;
    } else {
        NotifyForegroundFailed(res);
    }
    return res;
}

WMError WindowSessionImpl::Hide(uint32_t reason, bool withAnimation, bool isFromInnerkits)
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
        NotifyAfterBackground();
        state_ = WindowState::STATE_HIDDEN;
    }
    return res;
}

WMError WindowSessionImpl::Destroy(bool needClearListener)
{
    WLOGFI("id:%{public}" PRIu64 " Destroy", property_->GetPersistentId());
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_OK;
    }
    WSError ret = WSError::WS_OK;
    if (!WindowHelper::IsMainWindow(GetType())) {
        // main window no need to notify host, since host knows hide first
        SessionManager::GetInstance().DestroyAndDisconnectSpecificSession(property_->GetPersistentId());
    }
    // delete after replace WSError with WMError
    WMError res = static_cast<WMError>(ret);
    if (res == WMError::WM_OK) {
        NotifyBeforeDestroy(GetWindowName());
        state_ = WindowState::STATE_DESTROYED;
    }
    if (needClearListener) {
        ClearListenersById(GetPersistentId());
    }
    windowSessionMap_.erase(property_->GetWindowName());
    return res;
}

WMError WindowSessionImpl::Destroy()
{
    return Destroy(true);
}

WSError WindowSessionImpl::SetActive(bool active)
{
    WLOGFD("active status: %{public}d", active);
    WSError ret = hostSession_->UpdateActiveStatus(active);
    if (ret != WSError::WS_OK) {
        return ret;
    }
    if (active) {
        NotifyAfterActive();
    } else {
        NotifyAfterInactive();
    }
    return WSError::WS_OK;
}

WSError WindowSessionImpl::UpdateRect(const WSRect& rect, SizeChangeReason reason)
{
    WLOGFI("update rect [%{public}d, %{public}d, %{public}u, %{public}u], reason:%{public}u", rect.posX_, rect.posY_,
        rect.width_, rect.height_, reason);

    // delete after replace ws_common.h with wm_common.h
    auto wmReason = static_cast<WindowSizeChangeReason>(reason);
    Rect wmRect = { rect.posX_, rect.posY_, rect.width_, rect.height_ };
    property_->SetWindowRect(wmRect);
    NotifySizeChange(wmRect, wmReason);
    UpdateViewportConfig(wmRect, wmReason);
    return WSError::WS_OK;
}

void WindowSessionImpl::UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (uiContent_ == nullptr) {
        return;
    }
    Ace::ViewportConfig config;
    config.SetSize(rect.width_, rect.height_);
    config.SetPosition(rect.posX_, rect.posY_);
    float density = rect.height_ > 2700 ? 3.5f : 1.5f; // 2700: phone height; 3.5f/1.5f: normal desity
    config.SetDensity(density);
    uiContent_->UpdateViewportConfig(config, reason);
    WLOGFD("Id:%{public}" PRIu64 ", windowRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
        property_->GetPersistentId(), rect.posX_, rect.posY_, rect.width_, rect.height_);
}

Rect WindowSessionImpl::GetRect() const
{
    return property_->GetWindowRect();
}

WMError WindowSessionImpl::SetUIContent(const std::string& contentInfo,
    NativeEngine* engine, NativeValue* storage, bool isdistributed, AppExecFwk::Ability* ability)
{
    WLOGFD("SetUIContent: %{public}s state:%{public}u", contentInfo.c_str(), state_);
    if (uiContent_) {
        uiContent_->Destroy();
    }
    std::unique_ptr<Ace::UIContent> uiContent;
    if (ability != nullptr) {
        uiContent = Ace::UIContent::Create(ability);
    } else {
        uiContent = Ace::UIContent::Create(context_.get(), engine);
    }
    if (uiContent == nullptr) {
        WLOGFE("fail to SetUIContent id: %{public}" PRIu64 "", property_->GetPersistentId());
        return WMError::WM_ERROR_NULLPTR;
    }
    if (isdistributed) {
        uiContent->Restore(this, contentInfo, storage);
    } else {
        uiContent->Initialize(this, contentInfo, storage);
    }
    // make uiContent available after Initialize/Restore
    uiContent_ = std::move(uiContent);
    if (state_ == WindowState::STATE_SHOWN) {
        // UIContent may be nullptr when show window, need to notify again when window is shown
        uiContent_->Foreground();
    }
    UpdateViewportConfig(GetRect(), WindowSizeChangeReason::UNDEFINED);
    WLOGFD("notify uiContent window size change end");
    return WMError::WM_OK;
}

std::shared_ptr<RSSurfaceNode> WindowSessionImpl::GetSurfaceNode() const
{
    return surfaceNode_;
}

const std::shared_ptr<AbilityRuntime::Context> WindowSessionImpl::GetContext() const
{
    return context_;
}

Rect WindowSessionImpl::GetRequestRect() const
{
    return property_->GetRequestRect();
}

WindowType WindowSessionImpl::GetType() const
{
    return property_->GetWindowType();
}

const std::string& WindowSessionImpl::GetWindowName() const
{
    return property_->GetWindowName();
}

WindowState WindowSessionImpl::GetWindowState() const
{
    return state_;
}

WMError WindowSessionImpl::SetFocusable(bool isFocusable)
{
    property_->SetFocusable(isFocusable);
    return WMError::WM_OK;
}

bool WindowSessionImpl::GetFocusable() const
{
    return property_->GetFocusable();
}

WMError WindowSessionImpl::SetTouchable(bool isTouchable)
{
    property_->SetTouchable(isTouchable);
    return WMError::WM_OK;
}

bool WindowSessionImpl::GetTouchable() const
{
    return property_->GetTouchable();
}

WMError WindowSessionImpl::SetWindowType(WindowType type)
{
    property_->SetWindowType(type);
    return WMError::WM_OK;
}

std::string WindowSessionImpl::GetContentInfo()
{
    WLOGFD("GetContentInfo");
    if (uiContent_ == nullptr) {
        WLOGFE("fail to GetContentInfo id: %{public}" PRIu64 "", property_->GetPersistentId());
        return "";
    }
    return uiContent_->GetContentInfo();
}

Ace::UIContent* WindowSessionImpl::GetUIContent() const
{
    return uiContent_.get();
}

void WindowSessionImpl::OnNewWant(const AAFwk::Want& want)
{
    WLOGFI("Window [name:%{public}s, id:%{public}" PRIu64 "]",
        property_->GetWindowName().c_str(), property_->GetPersistentId());
    if (uiContent_ != nullptr) {
        uiContent_->OnNewWant(want);
    }
}

WMError WindowSessionImpl::RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    WLOGFD("Start register");
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    return RegisterListener(lifecycleListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    WLOGFD("Start unregister");
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    return UnregisterListener(lifecycleListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
{
    WLOGFD("Start register");
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    return RegisterListener(windowChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
{
    WLOGFD("Start register");
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    return UnregisterListener(windowChangeListeners_[GetPersistentId()], listener);
}

template<typename T>
EnableIfSame<T, IWindowLifeCycle, std::vector<wptr<IWindowLifeCycle>>> WindowSessionImpl::GetListeners()
{
    std::vector<wptr<IWindowLifeCycle>> lifecycleListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : lifecycleListeners_[GetPersistentId()]) {
            lifecycleListeners.push_back(listener);
        }
    }
    return lifecycleListeners;
}

template<typename T>
EnableIfSame<T, IWindowChangeListener, std::vector<wptr<IWindowChangeListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<wptr<IWindowChangeListener>> windowChangeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : windowChangeListeners_[GetPersistentId()]) {
            windowChangeListeners.push_back(listener);
        }
    }
    return windowChangeListeners;
}

template<typename T>
WMError WindowSessionImpl::RegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (std::find(holder.begin(), holder.end(), listener) != holder.end()) {
        WLOGFE("Listener already registered");
        return WMError::WM_OK;
    }
    holder.emplace_back(listener);
    return WMError::WM_OK;
}

template<typename T>
WMError WindowSessionImpl::UnregisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    holder.erase(std::remove_if(holder.begin(), holder.end(),
        [listener](sptr<T> registeredListener) {
            return registeredListener == listener;
        }), holder.end());
    return WMError::WM_OK;
}

template<typename T>
void WindowSessionImpl::ClearUselessListeners(std::map<uint64_t, T>& listeners, uint64_t persistentId)
{
    listeners.erase(persistentId);
}

void WindowSessionImpl::ClearListenersById(uint64_t persistentId)
{
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    ClearUselessListeners(lifecycleListeners_, persistentId);
    ClearUselessListeners(windowChangeListeners_, persistentId);
}

void WindowSessionImpl::RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func)
{
    notifyNativefunc_ = std::move(func);
}

void WindowSessionImpl::NotifyAfterForeground(bool needNotifyListeners, bool needNotifyUiContent)
{
    if (needNotifyListeners) {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterForeground, lifecycleListeners);
    }
    if (needNotifyUiContent) {
        CALL_UI_CONTENT(Foreground);
    }
}

void WindowSessionImpl::NotifyAfterBackground(bool needNotifyListeners, bool needNotifyUiContent)
{
    if (needNotifyListeners) {
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterBackground, lifecycleListeners);
    }
    if (needNotifyUiContent) {
        CALL_UI_CONTENT(Background);
    }
}

void WindowSessionImpl::NotifyAfterFocused()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterFocused, lifecycleListeners);
    CALL_UI_CONTENT(Focus);
}

void WindowSessionImpl::NotifyAfterUnfocused(bool needNotifyUiContent)
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    // use needNotifyUinContent to separate ui content callbacks
    CALL_LIFECYCLE_LISTENER(AfterUnfocused, lifecycleListeners);
    if (needNotifyUiContent) {
        CALL_UI_CONTENT(UnFocus);
    }
}

void WindowSessionImpl::NotifyBeforeDestroy(std::string windowName)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (uiContent_ != nullptr) {
        auto uiContent = std::move(uiContent_);
        uiContent_ = nullptr;
        uiContent->Destroy();
    }
    if (notifyNativefunc_) {
        notifyNativefunc_(windowName);
    }
}

void WindowSessionImpl::NotifyAfterActive()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterActive, lifecycleListeners);
}

void WindowSessionImpl::NotifyAfterInactive()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterInactive, lifecycleListeners);
}

void WindowSessionImpl::NotifyForegroundFailed(WMError ret)
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER_WITH_PARAM(ForegroundFailed, lifecycleListeners, static_cast<int32_t>(ret));
}

void WindowSessionImpl::NotifySizeChange(Rect rect, WindowSizeChangeReason reason)
{
    auto windowChangeListeners = GetListeners<IWindowChangeListener>();
    for (auto& listener : windowChangeListeners) {
        if (listener.GetRefPtr() != nullptr) {
            listener.GetRefPtr()->OnSizeChange(rect, reason);
        }
    }
}

void WindowSessionImpl::NotifyPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (uiContent_) {
        uiContent_->ProcessPointerEvent(pointerEvent);
    }
}

void WindowSessionImpl::NotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (uiContent_) {
        uiContent_->ProcessKeyEvent(keyEvent);
    }
}

void WindowSessionImpl::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (state_ == WindowState::STATE_DESTROYED) {
        WLOGFE("Receive vsync request failed, window is destroyed");
        return;
    }
    VsyncStation::GetInstance().RequestVsync(vsyncCallback);
}
} // namespace Rosen
} // namespace OHOS
