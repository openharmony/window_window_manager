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

#include <cstdlib>
#include <optional>

#include <common/rs_common_def.h>
#include <filesystem>
#include <fstream>
#include <ipc_skeleton.h>
#include <hisysevent.h>
#include <parameters.h>
#ifdef IMF_ENABLE
#include <input_method_controller.h>
#endif // IMF_ENABLE
#include <transaction/rs_interfaces.h>
#include <transaction/rs_transaction.h>

#include "anr_handler.h"
#include "color_parser.h"
#include "display_info.h"
#include "display_manager.h"
#include "hitrace_meter.h"
#include "interfaces/include/ws_common.h"
#include "session_permission.h"
#include "key_event.h"
#include "session/container/include/window_event_channel.h"
#include "session_manager/include/session_manager.h"
#include "vsync_station.h"
#include "window_adapter.h"
#include "window_manager_hilog.h"
#include "window_helper.h"
#include "color_parser.h"
#include "singleton_container.h"
#include "perform_reporter.h"
#include "picture_in_picture_manager.h"
#include "parameters.h"

namespace OHOS::Accessibility {
class AccessibilityEventInfo;
}
namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowSessionImpl"};
constexpr int32_t ANIMATION_TIME = 400;
}

std::map<int32_t, std::vector<sptr<IWindowLifeCycle>>> WindowSessionImpl::lifecycleListeners_;
std::map<int32_t, std::vector<sptr<IWindowChangeListener>>> WindowSessionImpl::windowChangeListeners_;
std::map<int32_t, std::vector<sptr<IAvoidAreaChangedListener>>> WindowSessionImpl::avoidAreaChangeListeners_;
std::map<int32_t, std::vector<sptr<IDialogDeathRecipientListener>>> WindowSessionImpl::dialogDeathRecipientListeners_;
std::map<int32_t, std::vector<sptr<IDialogTargetTouchListener>>> WindowSessionImpl::dialogTargetTouchListener_;
std::map<int32_t, std::vector<sptr<IOccupiedAreaChangeListener>>> WindowSessionImpl::occupiedAreaChangeListeners_;
std::map<int32_t, std::vector<sptr<IScreenshotListener>>> WindowSessionImpl::screenshotListeners_;
std::map<int32_t, std::vector<sptr<ITouchOutsideListener>>> WindowSessionImpl::touchOutsideListeners_;
std::map<int32_t, std::vector<IWindowVisibilityListenerSptr>> WindowSessionImpl::windowVisibilityChangeListeners_;
std::map<int32_t, std::vector<sptr<IWindowTitleButtonRectChangedListener>>>
    WindowSessionImpl::windowTitleButtonRectChangeListeners_;
std::recursive_mutex WindowSessionImpl::lifeCycleListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::avoidAreaChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::dialogDeathRecipientListenerMutex_;
std::recursive_mutex WindowSessionImpl::dialogTargetTouchListenerMutex_;
std::recursive_mutex WindowSessionImpl::occupiedAreaChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::screenshotListenerMutex_;
std::recursive_mutex WindowSessionImpl::touchOutsideListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowVisibilityChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowStatusChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowTitleButtonRectChangeListenerMutex_;
std::map<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>> WindowSessionImpl::windowSessionMap_;
std::shared_mutex WindowSessionImpl::windowSessionMutex_;
std::map<int32_t, std::vector<sptr<WindowSessionImpl>>> WindowSessionImpl::subWindowSessionMap_;
std::map<int32_t, std::vector<sptr<IWindowStatusChangeListener>>> WindowSessionImpl::windowStatusChangeListeners_;

#define CALL_LIFECYCLE_LISTENER(windowLifecycleCb, listeners) \
    do {                                                      \
        for (auto& listener : (listeners)) {                  \
            if (listener != nullptr) {            \
                listener->windowLifecycleCb();    \
            }                                                 \
        }                                                     \
    } while (0)

#define CALL_LIFECYCLE_LISTENER_WITH_PARAM(windowLifecycleCb, listeners, param) \
    do {                                                                        \
        for (auto& listener : (listeners)) {                                    \
            if (listener != nullptr) {                                         \
                listener->windowLifecycleCb(param);                 \
            }                                                                   \
        }                                                                       \
    } while (0)

#define CALL_UI_CONTENT(uiContentCb)                          \
    do {                                                      \
        std::lock_guard<std::recursive_mutex> lock(mutex_);   \
        if (uiContent_ != nullptr) {                          \
            uiContent_->uiContentCb();                        \
        }                                                     \
    } while (0)

WindowSessionImpl::WindowSessionImpl(const sptr<WindowOption>& option)
{
    WLOGFD("[WMSCom]WindowSessionImpl");
    property_ = new (std::nothrow) WindowSessionProperty();
    if (property_ == nullptr) {
        WLOGFE("[WMSCom]Property is null");
        return;
    }
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = option->GetBundleName();
    property_->SetSessionInfo(sessionInfo);
    property_->SetWindowName(option->GetWindowName());
    property_->SetRequestRect(option->GetWindowRect());
    property_->SetWindowType(option->GetWindowType());
    property_->SetFocusable(option->GetFocusable());
    property_->SetTouchable(option->GetTouchable());
    property_->SetDisplayId(option->GetDisplayId());
    property_->SetParentId(option->GetParentId());
    property_->SetTurnScreenOn(option->IsTurnScreenOn());
    property_->SetKeepScreenOn(option->IsKeepScreenOn());
    property_->SetWindowMode(option->GetWindowMode());
    property_->SetWindowFlags(option->GetWindowFlags());
    property_->SetCallingWindow(option->GetCallingWindow());
    isMainHandlerAvailable_ = option->GetMainHandlerAvailable();

    auto isPC = system::GetParameter("const.product.devicetype", "unknown") == "2in1";
    if (isPC && WindowHelper::IsSubWindow(option->GetWindowType())) {
        WLOGFD("create subwindow, title: %{public}s, decorEnable: %{public}d",
            option->GetSubWindowTitle().c_str(), option->GetSubWindowDecorEnable());
        property_->SetDecorEnable(option->GetSubWindowDecorEnable());
        property_->SetDragEnabled(option->GetSubWindowDecorEnable());
        subWindowTitle_ = option->GetSubWindowTitle();
    }

    surfaceNode_ = CreateSurfaceNode(property_->GetWindowName(), option->GetWindowType());
    handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
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
    WLOGFD("[WMSCom]~WindowSessionImpl, id: %{public}d", GetPersistentId());
    Destroy(true, false);
}

uint32_t WindowSessionImpl::GetWindowId() const
{
    return static_cast<uint32_t>(GetPersistentId()) & 0xffffffff; // 0xffffffff: to get low 32 bits
}

int32_t WindowSessionImpl::GetParentId() const
{
    // 0xffffffff: to get low 32 bits
    uint32_t parentID = static_cast<uint32_t>(property_->GetParentPersistentId()) & 0x7fffffff;
    return static_cast<int32_t>(parentID);
}

bool WindowSessionImpl::IsWindowSessionInvalid() const
{
    bool res = ((hostSession_ == nullptr) || (GetPersistentId() == INVALID_SESSION_ID) ||
        (state_ == WindowState::STATE_DESTROYED));
    if (res) {
        WLOGW("[WMSLife] already destroyed or not created! id: %{public}d state_: %{public}u",
            GetPersistentId(), state_);
    }
    return res;
}

bool WindowSessionImpl::IsMainHandlerAvailable() const
{
    return isMainHandlerAvailable_;
}

int32_t WindowSessionImpl::GetPersistentId() const
{
    if (property_) {
        return property_->GetPersistentId();
    }
    return INVALID_SESSION_ID;
}

sptr<WindowSessionProperty> WindowSessionImpl::GetProperty() const
{
    return property_;
}

SystemSessionConfig WindowSessionImpl::GetSystemSessionConfig() const
{
    return windowSystemConfig_;
}

sptr<ISession> WindowSessionImpl::GetHostSession() const
{
    return hostSession_;
}

ColorSpace WindowSessionImpl::GetColorSpaceFromSurfaceGamut(GraphicColorGamut colorGamut)
{
    if (colorGamut == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB) {
        return ColorSpace::COLOR_SPACE_DEFAULT;
    } else if (colorGamut == GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3) {
        return ColorSpace::COLOR_SPACE_WIDE_GAMUT;
    } else {
        WLOGFE("try to get not exist ColorSpace");
        return ColorSpace::COLOR_SPACE_DEFAULT;
    }
}

GraphicColorGamut WindowSessionImpl::GetSurfaceGamutFromColorSpace(ColorSpace colorSpace)
{
    if (colorSpace == ColorSpace::COLOR_SPACE_DEFAULT) {
        return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    } else if (colorSpace == ColorSpace::COLOR_SPACE_WIDE_GAMUT) {
        return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_DCI_P3;
    } else {
        WLOGFE("try to get not exist colorGamut");
        return GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    }
}

bool WindowSessionImpl::IsSupportWideGamut()
{
    return true;
}

void WindowSessionImpl::SetColorSpace(ColorSpace colorSpace)
{
    auto colorGamut = GetSurfaceGamutFromColorSpace(colorSpace);
    surfaceNode_->SetColorSpace(colorGamut);
}

ColorSpace WindowSessionImpl::GetColorSpace()
{
    GraphicColorGamut colorGamut = surfaceNode_->GetColorSpace();
    return GetColorSpaceFromSurfaceGamut(colorGamut);
}

WMError WindowSessionImpl::WindowSessionCreateCheck()
{
    if (!property_) {
        return WMError::WM_ERROR_NULLPTR;
    }
    const auto& name = property_->GetWindowName();
    std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
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
        uint32_t accessTokenId = static_cast<uint32_t>(IPCSkeleton::GetCallingTokenID());
        property_->SetAccessTokenId(accessTokenId);
        WLOGI("Create camera float window, TokenId = %{public}u", accessTokenId);
    }
    return WMError::WM_OK;
}

void WindowSessionImpl::SetDefaultDisplayIdIfNeed()
{
    auto displayId = property_->GetDisplayId();
    if (displayId == DISPLAY_ID_INVALID) {
        auto defaultDisplayId = SingletonContainer::IsDestroyed() ? DISPLAY_ID_INVALID :
            SingletonContainer::Get<DisplayManager>().GetDefaultDisplayId();
        defaultDisplayId = (defaultDisplayId == DISPLAY_ID_INVALID)? 0 : defaultDisplayId;
        property_->SetDisplayId(defaultDisplayId);
        WLOGFI("Reset displayId to %{public}llu", defaultDisplayId);
    }
}

WMError WindowSessionImpl::Create(const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<Rosen::ISession>& iSession)
{
    return WMError::WM_OK;
}

WMError WindowSessionImpl::Connect()
{
    if (hostSession_ == nullptr) {
        WLOGFE("[WMSLife]Session is null!");
        return WMError::WM_ERROR_NULLPTR;
    }
    sptr<ISessionStage> iSessionStage(this);
    auto windowEventChannel = new (std::nothrow) WindowEventChannel(iSessionStage);
    sptr<IWindowEventChannel> iWindowEventChannel(windowEventChannel);
    sptr<IRemoteObject> token = context_ ? context_->GetToken() : nullptr;
    if (token) {
        property_->SetTokenState(true);
    }
    auto ret = hostSession_->Connect(
        iSessionStage, iWindowEventChannel, surfaceNode_, windowSystemConfig_, property_, token);
    WLOGFI("[WMSLife]Window Connect [name:%{public}s, id:%{public}d, type:%{public}u], ret:%{public}u",
        property_->GetWindowName().c_str(), GetPersistentId(), property_->GetWindowType(), ret);
    return static_cast<WMError>(ret);
}

void WindowSessionImpl::ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    NotifyPointerEvent(pointerEvent);
}

void WindowSessionImpl::ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    bool isConsumed = false;
    NotifyKeyEvent(keyEvent, isConsumed);
}

WMError WindowSessionImpl::Show(uint32_t reason, bool withAnimation)
{
    WLOGFI("[WMSLife]Window Show [name:%{public}s, id:%{public}d, type:%{public}u], reason:%{public}u state:%{public}u",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), GetType(), reason, state_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_SHOWN) {
        WLOGFD("[WMSLife]window session is alreay shown [name:%{public}s, id:%{public}d, type: %{public}u]",
            property_->GetWindowName().c_str(), GetPersistentId(), property_->GetWindowType());
        NotifyAfterForeground(true, false);
        return WMError::WM_OK;
    }

    WSError ret = hostSession_->Foreground(property_);
    // delete after replace WSError with WMError
    WMError res = static_cast<WMError>(ret);
    if (res == WMError::WM_OK) {
        state_ = WindowState::STATE_SHOWN;
        requestState_ = WindowState::STATE_SHOWN;
        NotifyAfterForeground();
    } else {
        NotifyForegroundFailed(res);
    }
    return res;
}

WMError WindowSessionImpl::Hide(uint32_t reason, bool withAnimation, bool isFromInnerkits)
{
    WLOGFI("[WMSLife]id:%{public}d Hide, reason:%{public}u, state:%{public}u",
        GetPersistentId(), reason, state_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        WLOGFD("[WMSLife]window session is alreay hidden [name:%{public}s, id:%{public}d, type: %{public}u]",
            property_->GetWindowName().c_str(), GetPersistentId(), property_->GetWindowType());
        NotifyBackgroundFailed(WMError::WM_DO_NOTHING);
        return WMError::WM_OK;
    }
    state_ = WindowState::STATE_HIDDEN;
    requestState_ = WindowState::STATE_HIDDEN;
    NotifyAfterBackground();
    return WMError::WM_OK;
}

WMError WindowSessionImpl::Destroy(bool needNotifyServer, bool needClearListener)
{
    WLOGFI("[WMSLife]Id: %{public}d Destroy, state_:%{public}u, needNotifyServer: %{public}d, "
        "needClearListener: %{public}d", GetPersistentId(), state_, needNotifyServer, needClearListener);
    if (IsWindowSessionInvalid()) {
        WLOGFW("[WMSLife]session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (hostSession_ != nullptr) {
        hostSession_->Disconnect();
    }
    NotifyBeforeDestroy(GetWindowName());
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        state_ = WindowState::STATE_DESTROYED;
        requestState_ = WindowState::STATE_DESTROYED;
    }
    hostSession_ = nullptr;
    {
        std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
        windowSessionMap_.erase(property_->GetWindowName());
    }
    DelayedSingleton<ANRHandler>::GetInstance()->OnWindowDestroyed(GetPersistentId());
    NotifyAfterDestroy();
    if (needClearListener) {
        ClearListenersById(GetPersistentId());
    }
    return WMError::WM_OK;
}

WMError WindowSessionImpl::Destroy()
{
    return Destroy(true);
}

WSError WindowSessionImpl::SetActive(bool active)
{
    WLOGFD("active status: %{public}d", active);
    if (active) {
        NotifyAfterActive();
    } else {
        NotifyAfterInactive();
    }
    return WSError::WS_OK;
}

WSError WindowSessionImpl::UpdateRect(const WSRect& rect, SizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    // delete after replace ws_common.h with wm_common.h
    auto wmReason = static_cast<WindowSizeChangeReason>(reason);
    Rect wmRect = { rect.posX_, rect.posY_, rect.width_, rect.height_ };
    auto preRect = GetRect();
    property_->SetWindowRect(wmRect);
    if (handler_ != nullptr && wmReason == WindowSizeChangeReason::ROTATION) {
        postTaskDone_ = false;
        UpdateRectForRotation(wmRect, preRect, wmReason, rsTransaction);
    } else {
        if ((wmRect != preRect) || (wmReason != lastSizeChangeReason_) || !postTaskDone_) {
            NotifySizeChange(wmRect, wmReason);
            lastSizeChangeReason_ = wmReason;
            postTaskDone_ = true;
        }
        UpdateViewportConfig(wmRect, wmReason, rsTransaction);
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
        "WindowSessionImpl::UpdateRect%d [%d, %d, %u, %u] reason:%u",
        GetPersistentId(), wmRect.posX_, wmRect.posY_, wmRect.width_, wmRect.height_, wmReason);
    WLOGFI("[WMSLayout] updateRect [%{public}d, %{public}d, %{public}u, %{public}u], reason:%{public}u"
        "WindowInfo:[name: %{public}s, persistentId:%{public}d]", rect.posX_, rect.posY_,
        rect.width_, rect.height_, wmReason, GetWindowName().c_str(), GetPersistentId());
    return WSError::WS_OK;
}

void WindowSessionImpl::UpdateRectForRotation(const Rect& wmRect, const Rect& preRect,
    WindowSizeChangeReason wmReason, const std::shared_ptr<RSTransaction>& rsTransaction)
{
    handler_->PostTask([weak = wptr(this), wmReason, wmRect, preRect, rsTransaction]() mutable {
        auto window = weak.promote();
        if (!window) {
            return;
        }
        if (rsTransaction) {
            RSTransaction::FlushImplicitTransaction();
            rsTransaction->Begin();
        }
        RSSystemProperties::SetDrawTextAsBitmap(true);
        RSInterfaces::GetInstance().EnableCacheForRotation();
        window->rotationAnimationCount_++;
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(ANIMATION_TIME);
        auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.2, 0.0, 0.2, 1.0);
        RSNode::OpenImplicitAnimation(protocol, curve, [weak]() {
            auto window = weak.promote();
            if (!window) {
                return;
            }
            window->rotationAnimationCount_--;
            if (window->rotationAnimationCount_ == 0) {
                RSSystemProperties::SetDrawTextAsBitmap(false);
                RSInterfaces::GetInstance().DisableCacheForRotation();
            }
        });
        if ((wmRect != preRect) || (wmReason != window->lastSizeChangeReason_)) {
            window->NotifySizeChange(wmRect, wmReason);
            window->lastSizeChangeReason_ = wmReason;
        }
        window->UpdateViewportConfig(wmRect, wmReason, rsTransaction);
        RSNode::CloseImplicitAnimation();
        if (rsTransaction) {
            rsTransaction->Commit();
        } else {
            RSTransaction::FlushImplicitTransaction();
        }
        window->postTaskDone_ = true;
    }, "WMS_WindowSessionImpl_UpdateRectForRotation");
}

void WindowSessionImpl::UpdateDensity()
{
    auto preRect = GetRect();
    UpdateViewportConfig(preRect, WindowSizeChangeReason::UNDEFINED);
    WLOGFI("WindowSessionImpl::UpdateDensity [%{public}d, %{public}d, %{public}u, %{public}u]",
        preRect.posX_, preRect.posY_, preRect.width_, preRect.height_);
}

WSError WindowSessionImpl::UpdateFocus(bool isFocused)
{
    WLOGFI("[WMSFocus]Report update focus: %{public}u, id: %{public}d", isFocused, GetPersistentId());
    isFocused_ = isFocused;
    if (isFocused) {
        HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
            "FOCUS_WINDOW",
            OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "PID", getpid(),
            "UID", getuid(),
            "BUNDLE_NAME", property_->GetSessionInfo().bundleName_);
        NotifyAfterFocused();
    } else {
        NotifyAfterUnfocused();
    }
    return WSError::WS_OK;
}

bool WindowSessionImpl::IsFocused() const
{
    return isFocused_;
}

WMError WindowSessionImpl::RequestFocus() const
{
    if (IsWindowSessionInvalid()) {
        WLOGFD("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SingletonContainer::Get<WindowAdapter>().RequestFocusStatus(GetPersistentId(), true);
}

void WindowSessionImpl::NotifyForegroundInteractiveStatus(bool interactive)
{
    WLOGFI("NotifyForegroundInteractiveStatus %{public}d", interactive);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return;
    }
    if (state_ == WindowState::STATE_SHOWN) {
        if (interactive) {
            NotifyAfterResumed();
        } else {
            NotifyAfterPaused();
        }
    }
}

WSError WindowSessionImpl::UpdateWindowMode(WindowMode mode)
{
    return WSError::WS_OK;
}

void WindowSessionImpl::UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (uiContent_ == nullptr) {
        WLOGFW("uiContent_ is null!");
        return;
    }
    Ace::ViewportConfig config;
    config.SetSize(rect.width_, rect.height_);
    config.SetPosition(rect.posX_, rect.posY_);
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr || display->GetDisplayInfo() == nullptr) {
        WLOGFE("display is null!");
        return;
    }
    auto displayInfo = display->GetDisplayInfo();
    float density = displayInfo->GetVirtualPixelRatio();
    int32_t orientation = static_cast<int32_t>(displayInfo->GetDisplayOrientation());
    virtualPixelRatio_ = density;
    config.SetDensity(density);
    config.SetOrientation(orientation);
    uiContent_->UpdateViewportConfig(config, reason, rsTransaction);
    WLOGFI("[WMSLayout] Id:%{public}d, reason:%{public}d, windowRect:[%{public}d, %{public}d, %{public}u, %{public}u], "
        "orientation: %{public}d", GetPersistentId(), reason, rect.posX_, rect.posY_,
        rect.width_, rect.height_, orientation);
}

int32_t WindowSessionImpl::GetFloatingWindowParentId()
{
    if (context_.get() == nullptr) {
        return INVALID_SESSION_ID;
    }
    std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
    for (const auto& winPair : windowSessionMap_) {
        if (winPair.second.second && WindowHelper::IsMainWindow(winPair.second.second->GetType()) &&
            winPair.second.second->GetProperty() &&
            context_.get() == winPair.second.second->GetContext().get()) {
            WLOGFD("Find parent, [parentName: %{public}s, selfPersistentId: %{public}d]",
                winPair.second.second->GetProperty()->GetWindowName().c_str(), GetPersistentId());
            return winPair.second.second->GetProperty()->GetPersistentId();
        }
    }
    return INVALID_SESSION_ID;
}

Rect WindowSessionImpl::GetRect() const
{
    return property_->GetWindowRect();
}

void WindowSessionImpl::UpdateTitleButtonVisibility()
{
    if (uiContent_ == nullptr || !IsDecorEnable()) {
        return;
    }
    auto isPC = system::GetParameter("const.product.devicetype", "unknown") == "2in1";
    if (isPC && WindowHelper::IsSubWindow(GetType())) {
        WLOGFD("hide other buttons except close");
        uiContent_->HideWindowTitleButton(true, true, true);
        return;
    }
    auto modeSupportInfo = property_->GetModeSupportInfo();
    bool hideSplitButton = !(modeSupportInfo & WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY);
    // not support fullscreen in split and floating mode, or not support float in fullscreen mode
    bool hideMaximizeButton = (!(modeSupportInfo & WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN) &&
        (GetMode() == WindowMode::WINDOW_MODE_FLOATING || WindowHelper::IsSplitWindowMode(GetMode()))) ||
        (!(modeSupportInfo & WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING) &&
        GetMode() == WindowMode::WINDOW_MODE_FULLSCREEN);
    WLOGFI("[hideSplit, hideMaximize]: [%{public}d, %{public}d]", hideSplitButton, hideMaximizeButton);
    uiContent_->HideWindowTitleButton(hideSplitButton, hideMaximizeButton, false);
}

WMError WindowSessionImpl::NapiSetUIContent(const std::string& contentInfo, napi_env env, napi_value storage,
    bool isdistributed, sptr<IRemoteObject> token, AppExecFwk::Ability* ability)
{
    return SetUIContentInner(contentInfo, env, storage,
        isdistributed ? WindowSetUIContentType::DISTRIBUTE : WindowSetUIContentType::DEFAULT, ability);
}

WMError WindowSessionImpl::SetUIContentByName(
    const std::string& contentInfo, napi_env env, napi_value storage, AppExecFwk::Ability* ability)
{
    return SetUIContentInner(contentInfo, env, storage, WindowSetUIContentType::BY_NAME, ability);
}

WMError WindowSessionImpl::SetUIContentByAbc(
    const std::string& contentInfo, napi_env env, napi_value storage, AppExecFwk::Ability* ability)
{
    return SetUIContentInner(contentInfo, env, storage, WindowSetUIContentType::BY_ABC, ability);
}

WMError WindowSessionImpl::SetUIContentInner(const std::string& contentInfo, napi_env env, napi_value storage,
    WindowSetUIContentType type, AppExecFwk::Ability* ability)
{
    WLOGFD("[WMSLife]NapiSetUIContent: %{public}s state:%{public}u", contentInfo.c_str(), state_);
    if (uiContent_) {
        uiContent_->Destroy();
    }
    std::unique_ptr<Ace::UIContent> uiContent;
    if (ability != nullptr) {
        uiContent = Ace::UIContent::Create(ability);
    } else {
        uiContent = Ace::UIContent::Create(context_.get(), reinterpret_cast<NativeEngine*>(env));
    }
    if (uiContent == nullptr) {
        WLOGFE("[WMSLife]fail to NapiSetUIContent id: %{public}d", GetPersistentId());
        return WMError::WM_ERROR_NULLPTR;
    }
    switch (type) {
        default:
        case WindowSetUIContentType::DEFAULT:
            uiContent->Initialize(this, contentInfo, storage);
            break;
        case WindowSetUIContentType::DISTRIBUTE:
            uiContent->Restore(this, contentInfo, storage);
            break;
        case WindowSetUIContentType::BY_NAME:
            uiContent->InitializeByName(this, contentInfo, storage);
            break;
        case WindowSetUIContentType::BY_ABC:
            auto abcContent = GetAbcContent(contentInfo);
            uiContent->Initialize(this, abcContent, storage);
            break;
    }
    // make uiContent available after Initialize/Restore
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        uiContent_ = std::move(uiContent);
    }

    if (WindowHelper::IsSubWindow(GetType()) && IsDecorEnable()) {
        SetAPPWindowLabel(subWindowTitle_);
    }

    uint32_t version = 0;
    if ((context_ != nullptr) && (context_->GetApplicationInfo() != nullptr)) {
        version = context_->GetApplicationInfo()->apiCompatibleVersion;
    }
    // 10 ArkUI new framework support after API10
    if (version < 10) {
        SetLayoutFullScreenByApiVersion(isIgnoreSafeArea_);
        if (!isSystembarPropertiesSet_) {
            SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, SystemBarProperty());
        }
    } else if (isIgnoreSafeAreaNeedNotify_) {
        SetLayoutFullScreenByApiVersion(isIgnoreSafeArea_);
    }

    UpdateDecorEnable(true);
    if (state_ == WindowState::STATE_SHOWN) {
        // UIContent may be nullptr when show window, need to notify again when window is shown
        uiContent_->Foreground();
        UpdateTitleButtonVisibility();
    }
    UpdateViewportConfig(GetRect(), WindowSizeChangeReason::UNDEFINED);
    if (shouldReNotifyFocus_) {
        // uiContent may be nullptr when notify focus status, need to notify again when uiContent is not empty.
        NotifyUIContentFocusStatus();
        shouldReNotifyFocus_ = false;
    }
    WLOGFD("[WMSLife]notify uiContent window size change end");
    return WMError::WM_OK;
}

std::shared_ptr<std::vector<uint8_t>> WindowSessionImpl::GetAbcContent(const std::string& abcPath)
{
    std::filesystem::path abcFile { abcPath };
    if (abcFile.empty() || !abcFile.is_absolute() || !std::filesystem::exists(abcFile)) {
        WLOGFE("abc file path is not valid");
        return nullptr;
    }
    int begin, end;
    std::fstream file(abcFile, std::ios::in | std::ios::binary);
    if (!file) {
        WLOGFE("abc file is not valid");
        return nullptr;
    }
    begin = file.tellg();
    file.seekg(0, std::ios::end);
    end = file.tellg();
    size_t len = end - begin;
    WLOGFD("abc file: %{public}s, size: %{public}u", abcPath.c_str(), static_cast<uint32_t>(len));

    if (len <= 0) {
        WLOGFE("abc file size is 0");
        return nullptr;
    }
    std::vector<uint8_t> abcBytes(len);
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char *>(abcBytes.data()), len);
    return std::make_shared<std::vector<uint8_t>>(abcBytes);
}

void WindowSessionImpl::UpdateDecorEnableToAce(bool isDecorEnable)
{
    if (uiContent_ != nullptr) {
        uiContent_->UpdateWindowMode(GetMode(), isDecorEnable);
        WLOGFD("Notify uiContent window mode change end");
    }
}

void WindowSessionImpl::UpdateDecorEnable(bool needNotify, WindowMode mode)
{
    if (mode == WindowMode::WINDOW_MODE_UNDEFINED){
		mode = GetMode();
	}
	if (needNotify) {
        if (uiContent_ != nullptr) {
            uiContent_->UpdateWindowMode(mode, IsDecorEnable());
            WLOGFD("Notify uiContent window mode change end");
        }
        NotifyModeChange(mode, IsDecorEnable());
    }
}

void WindowSessionImpl::NotifyModeChange(WindowMode mode, bool hasDeco)
{
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
        auto windowChangeListeners = GetListeners<IWindowChangeListener>();
        for (auto& listener : windowChangeListeners) {
            if (listener.GetRefPtr() != nullptr) {
                listener.GetRefPtr()->OnModeChange(mode, hasDeco);
            }
        }
    }

    if (hostSession_) {
        property_->SetWindowMode(mode);
        property_->SetDecorEnable(hasDeco);
    }
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_MODE);
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE);
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

WindowState WindowSessionImpl::GetRequestWindowState() const
{
    return requestState_;
}

WMError WindowSessionImpl::SetFocusable(bool isFocusable)
{
    WLOGFD("set focusable");
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetFocusable(isFocusable);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE);
}

bool WindowSessionImpl::GetFocusable() const
{
    return property_->GetFocusable();
}

WMError WindowSessionImpl::SetTouchable(bool isTouchable)
{
    WLOGFD("set touchable");
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetTouchable(isTouchable);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE);
}

WMError WindowSessionImpl::SetResizeByDragEnabled(bool dragEnabled)
{
    WLOGFD("set dragEnabled");
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    if (WindowHelper::IsMainWindow(GetType())) {
        property_->SetDragEnabled(dragEnabled);
    } else {
        WLOGFE("This is not main window.");
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED);
}

WMError WindowSessionImpl::SetRaiseByClickEnabled(bool raiseEnabled)
{
    WLOGFD("set raiseEnabled");
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    property_->SetRaiseEnabled(raiseEnabled);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED);
}

WMError WindowSessionImpl::HideNonSystemFloatingWindows(bool shouldHide)
{
    WLOGFD("hide non-system floating windows");
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetHideNonSystemFloatingWindows(shouldHide);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS);
}

WMError WindowSessionImpl::SetSingleFrameComposerEnabled(bool enable)
{
    WLOGFD("Set the enable flag of single frame composer.");
    if (IsWindowSessionInvalid()) {
        WLOGE("The window state is invalid ");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    if (surfaceNode_ == nullptr) {
        WLOGE("The surface node is nullptr");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    surfaceNode_->MarkNodeSingleFrameComposer(enable);
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

bool WindowSessionImpl::IsFloatingWindowAppType() const
{
    return property_ != nullptr && property_->IsFloatingWindowAppType();
}

bool WindowSessionImpl::GetTouchable() const
{
    return property_->GetTouchable();
}

WMError WindowSessionImpl::SetWindowType(WindowType type)
{
    if (type != WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW && !SessionPermission::IsSystemCalling()) {
        WLOGFE("set window type permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetWindowType(type);
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetBrightness(float brightness)
{
    if ((brightness < MINIMUM_BRIGHTNESS &&
        std::fabs(brightness - UNDEFINED_BRIGHTNESS) >= std::numeric_limits<float>::min()) ||
        brightness > MAXIMUM_BRIGHTNESS) {
        WLOGFE("invalid brightness value: %{public}f", brightness);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (!WindowHelper::IsAppWindow(GetType())) {
        WLOGFE("non app window does not support set brightness, type: %{public}u", GetType());
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    if (!property_) {
        return WMError::WM_ERROR_NULLPTR;
    }
    property_->SetBrightness(brightness);
    if (state_ == WindowState::STATE_SHOWN) {
        return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS);
    }
    return WMError::WM_OK;
}

float WindowSessionImpl::GetBrightness() const
{
    return property_->GetBrightness();
}

void WindowSessionImpl::SetRequestedOrientation(Orientation orientation)
{
    WLOGFI("[WMSMain]id:%{public}u lastReqOrientation: %{public}u target:%{public}u state_:%{public}u",
        GetPersistentId(), property_->GetRequestedOrientation(), orientation, state_);
    if (property_->GetRequestedOrientation() == orientation) {
        return;
    }
    property_->SetRequestedOrientation(orientation);
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION);
}

Orientation WindowSessionImpl::GetRequestedOrientation()
{
    if (!property_) {
        WLOGFE("property_ is nullptr id: %{public}d", GetPersistentId());
        return Orientation::UNSPECIFIED;
    }
    return property_->GetRequestedOrientation();
}

std::string WindowSessionImpl::GetContentInfo()
{
    WLOGFD("GetContentInfo");
    if (uiContent_ == nullptr) {
        WLOGFE("fail to GetContentInfo id: %{public}d", GetPersistentId());
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
    WLOGFI("Window [name:%{public}s, id:%{public}d]",
        property_->GetWindowName().c_str(), GetPersistentId());
    if (uiContent_ != nullptr) {
        uiContent_->OnNewWant(want);
    }
}

WMError WindowSessionImpl::SetAPPWindowLabel(const std::string& label)
{
    if (uiContent_ == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent_->SetAppWindowTitle(label);
    WLOGI("Set app window label success, label : %{public}s", label.c_str());
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon)
{
    if (icon == nullptr) {
        WLOGFE("window icon is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (uiContent_ == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent_->SetAppWindowIcon(icon);
    WLOGI("Set app window icon success");
    return WMError::WM_OK;
}

WMError WindowSessionImpl::RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    WLOGFD("Start register");
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    return RegisterListener(lifecycleListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    WLOGFD("Start register");
    std::lock_guard<std::recursive_mutex> lockListener(occupiedAreaChangeListenerMutex_);
    return RegisterListener(occupiedAreaChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    WLOGFD("Start unregister");
    std::lock_guard<std::recursive_mutex> lockListener(occupiedAreaChangeListenerMutex_);
    return UnregisterListener(occupiedAreaChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    WLOGFD("Start unregister");
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    return UnregisterListener(lifecycleListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
{
    WLOGFD("Start register");
    std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
    return RegisterListener(windowChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
{
    WLOGFD("Start register");
    std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
    return UnregisterListener(windowChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterWindowStatusChangeListener(const sptr<IWindowStatusChangeListener>& listener)
{
    WLOGFD("Start register");
    std::lock_guard<std::recursive_mutex> lockListener(windowStatusChangeListenerMutex_);
    return RegisterListener(windowStatusChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterWindowStatusChangeListener(const sptr<IWindowStatusChangeListener>& listener)
{
    WLOGFD("Start register");
    std::lock_guard<std::recursive_mutex> lockListener(windowStatusChangeListenerMutex_);
    return UnregisterListener(windowStatusChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::SetDecorVisible(bool isVisible)
{
    if (uiContent_ == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent_->SetContainerModalTitleVisible(isVisible, true);
    WLOGI("Change the visibility of decor success");
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetDecorHeight(int32_t decorHeight)
{
    if (uiContent_ == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr || display->GetDisplayInfo() == nullptr) {
        WLOGFE("get display or get display info failed displayId:%{public}" PRIu64"", property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    float vpr = display->GetDisplayInfo()->GetVirtualPixelRatio();
    int32_t decorHeightWithPx = static_cast<int32_t>(decorHeight * vpr);
    uiContent_->SetContainerModalTitleHeight(decorHeightWithPx);
    WLOGI("Set app window decor height success, height : %{public}d", decorHeight);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::GetDecorHeight(int32_t& height)
{
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent is nullptr, windowId: %{public}u", GetWindowId());
        return WMError::WM_ERROR_NULLPTR;
    }
    height = uiContent_->GetContainerModalTitleHeight();
    if (height == -1) {
        WLOGFE("Get app window decor height failed");
        return WMError::WM_DO_NOTHING;
    }
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr || display->GetDisplayInfo() == nullptr) {
        WLOGFE("get display or get display info failed displayId:%{public}" PRIu64"", property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    float vpr = display->GetDisplayInfo()->GetVirtualPixelRatio();
    if (MathHelper::NearZero(vpr)) {
        WLOGFE("get decor height failed, because of wrong vpr: %{public}f", vpr);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    height = static_cast<int32_t>(height / vpr);
    WLOGI("Get app window decor height success, height : %{public}d", height);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::GetTitleButtonArea(TitleButtonRect& titleButtonRect)
{
    if (uiContent_ == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    Rect decorRect;
    Rect titleButtonLeftRect;
    bool res = uiContent_->GetContainerModalButtonsRect(decorRect, titleButtonLeftRect);
    if (!res) {
        WLOGFE("get window title buttons area failed");
        titleButtonRect.IsUninitializedRect();
        return WMError::WM_DO_NOTHING;
    }
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr || display->GetDisplayInfo() == nullptr) {
        WLOGFE("get display or get display info failed displayId:%{public}" PRIu64"", property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    float vpr = display->GetDisplayInfo()->GetVirtualPixelRatio();
    if (MathHelper::NearZero(vpr)) {
        WLOGFE("get title buttons area failed, because of wrong vpr: %{public}f", vpr);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    titleButtonRect.posX_ = decorRect.width_ - titleButtonLeftRect.width_ - titleButtonLeftRect.posX_;
    titleButtonRect.posX_ = static_cast<int32_t>(titleButtonRect.posX_ / vpr);
    titleButtonRect.posY_ = static_cast<int32_t>(titleButtonLeftRect.posY_ / vpr);
    titleButtonRect.width_ = static_cast<uint32_t>(titleButtonLeftRect.width_ / vpr);
    titleButtonRect.height_ = static_cast<uint32_t>(titleButtonLeftRect.height_ / vpr);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::RegisterWindowTitleButtonRectChangeListener(
    const sptr<IWindowTitleButtonRectChangedListener>& listener)
{
    WMError ret = WMError::WM_OK;
    auto persistentId = GetPersistentId();
    WLOGFD("Start register windowTitleButtonRectChange listener, id:%{public}d", persistentId);
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    {
        std::lock_guard<std::recursive_mutex> lockListener(windowTitleButtonRectChangeListenerMutex_);
        ret = RegisterListener(windowTitleButtonRectChangeListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            WLOGFE("register the listener of window title button rect change failed");
            return ret;
        }
    }
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr || display->GetDisplayInfo() == nullptr) {
        WLOGFE("get display or get display info failed displayId:%{public}" PRIu64"", property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    float vpr = display->GetDisplayInfo()->GetVirtualPixelRatio();
    if (MathHelper::NearZero(vpr)) {
        WLOGFE("register title button rect change listener failed, because of wrong vpr: %{public}f", vpr);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    uiContent_->SubscribeContainerModalButtonsRectChange([vpr, this](Rect& decorRect, Rect& titleButtonLeftRect) {
        TitleButtonRect titleButtonRect;
        titleButtonRect.posX_ = decorRect.width_ - titleButtonLeftRect.width_ - titleButtonLeftRect.posX_;
        titleButtonRect.posX_ = static_cast<int32_t>(titleButtonRect.posX_ / vpr);
        titleButtonRect.posY_ = static_cast<int32_t>(titleButtonLeftRect.posY_ / vpr);
        titleButtonRect.width_ = static_cast<uint32_t>(titleButtonLeftRect.width_ / vpr);
        titleButtonRect.height_ = static_cast<uint32_t>(titleButtonLeftRect.height_ / vpr);
        NotifyWindowTitleButtonRectChange(titleButtonRect);
    });
    return ret;
}

WMError WindowSessionImpl::UnregisterWindowTitleButtonRectChangeListener(
    const sptr<IWindowTitleButtonRectChangedListener>& listener)
{
    WMError ret = WMError::WM_OK;
    auto persistentId = GetPersistentId();
    WLOGFD("Start unregister windowTitleButtonRectChange listener, id:%{public}d", persistentId);
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    {
        std::lock_guard<std::recursive_mutex> lockListener(windowTitleButtonRectChangeListenerMutex_);
        ret = UnregisterListener(windowTitleButtonRectChangeListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            WLOGFE("unregister the listener of window title button rect change failed");
            return ret;
        }
    }
    uiContent_->SubscribeContainerModalButtonsRectChange(nullptr);
    return ret;
}

template<typename T>
EnableIfSame<T, IWindowTitleButtonRectChangedListener,
    std::vector<sptr<IWindowTitleButtonRectChangedListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IWindowTitleButtonRectChangedListener>> windowTitleButtonRectListeners;
        for (auto& listener : windowTitleButtonRectChangeListeners_[GetPersistentId()]) {
            windowTitleButtonRectListeners.push_back(listener);
        }
    return windowTitleButtonRectListeners;
}

void WindowSessionImpl::NotifyWindowTitleButtonRectChange(TitleButtonRect titleButtonRect)
{
    std::lock_guard<std::recursive_mutex> lockListener(windowTitleButtonRectChangeListenerMutex_);
    auto windowTitleButtonRectListeners = GetListeners<IWindowTitleButtonRectChangedListener>();
    for (auto& listener : windowTitleButtonRectListeners) {
        if (listener != nullptr) {
            listener->OnWindowTitleButtonRectChanged(titleButtonRect);
        }
    }
}

void WindowSessionImpl::RecoverSessionListener()
{
    auto persistentId = GetPersistentId();
    WLOGI("[WMSRecover] RecoverSessionListener with persistentId=%{public}d", persistentId);
    if (avoidAreaChangeListeners_.find(persistentId) != avoidAreaChangeListeners_.end() &&
        !avoidAreaChangeListeners_[persistentId].empty()) {
        SingletonContainer::Get<WindowAdapter>().UpdateSessionAvoidAreaListener(persistentId, true);
    }
    if (touchOutsideListeners_.find(persistentId) != touchOutsideListeners_.end() &&
        !touchOutsideListeners_[persistentId].empty()) {
        SingletonContainer::Get<WindowAdapter>().UpdateSessionTouchOutsideListener(persistentId, true);
    }
}

template<typename T>
EnableIfSame<T, IWindowLifeCycle, std::vector<sptr<IWindowLifeCycle>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IWindowLifeCycle>> lifecycleListeners;
    for (auto& listener : lifecycleListeners_[GetPersistentId()]) {
        lifecycleListeners.push_back(listener);
    }
    return lifecycleListeners;
}

template<typename T>
EnableIfSame<T, IWindowChangeListener, std::vector<sptr<IWindowChangeListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IWindowChangeListener>> windowChangeListeners;
    for (auto& listener : windowChangeListeners_[GetPersistentId()]) {
        windowChangeListeners.push_back(listener);
    }
    return windowChangeListeners;
}

template<typename T>
EnableIfSame<T, IOccupiedAreaChangeListener, std::vector<sptr<IOccupiedAreaChangeListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IOccupiedAreaChangeListener>> occupiedAreaChangeListeners;
    for (auto& listener : occupiedAreaChangeListeners_[GetPersistentId()]) {
        occupiedAreaChangeListeners.push_back(listener);
    }
    return occupiedAreaChangeListeners;
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
void WindowSessionImpl::ClearUselessListeners(std::map<int32_t, T>& listeners, int32_t persistentId)
{
    listeners.erase(persistentId);
}

template<typename T>
EnableIfSame<T, IWindowStatusChangeListener, std::vector<sptr<IWindowStatusChangeListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IWindowStatusChangeListener>> windowStatusChangeListeners;
    for (auto& listener : windowStatusChangeListeners_[GetPersistentId()]) {
        windowStatusChangeListeners.push_back(listener);
    }
    return windowStatusChangeListeners;
}

void WindowSessionImpl::ClearListenersById(int32_t persistentId)
{
    {
        std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
        ClearUselessListeners(lifecycleListeners_, persistentId);
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
        ClearUselessListeners(windowChangeListeners_, persistentId);
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(avoidAreaChangeListenerMutex_);
        ClearUselessListeners(avoidAreaChangeListeners_, persistentId);
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(dialogDeathRecipientListenerMutex_);
        ClearUselessListeners(dialogDeathRecipientListeners_, persistentId);
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(dialogTargetTouchListenerMutex_);
        ClearUselessListeners(dialogTargetTouchListener_, persistentId);
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(screenshotListenerMutex_);
        ClearUselessListeners(screenshotListeners_, persistentId);
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowStatusChangeListenerMutex_);
        ClearUselessListeners(windowStatusChangeListeners_, persistentId);
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowTitleButtonRectChangeListenerMutex_);
        ClearUselessListeners(windowTitleButtonRectChangeListeners_, persistentId);
    }
}

void WindowSessionImpl::RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func)
{
    notifyNativeFunc_ = std::move(func);
}

void WindowSessionImpl::SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    inputEventConsumer_ = inputEventConsumer;
}

void WindowSessionImpl::NotifyAfterForeground(bool needNotifyListeners, bool needNotifyUiContent)
{
    if (needNotifyListeners) {
        std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterForeground, lifecycleListeners);
    }
    if (needNotifyUiContent) {
        CALL_UI_CONTENT(Foreground);
    }
    if (WindowHelper::IsMainWindow(GetType()) || WindowHelper::IsUIExtensionWindow(GetType())) {
        VsyncStation::GetInstance().SetFrameRateLinkerEnable(true);
    }
}

void WindowSessionImpl::NotifyAfterBackground(bool needNotifyListeners, bool needNotifyUiContent)
{
    if (needNotifyListeners) {
        std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterBackground, lifecycleListeners);
    }
    if (needNotifyUiContent) {
        CALL_UI_CONTENT(Background);
    }
    if (WindowHelper::IsMainWindow(GetType()) || WindowHelper::IsUIExtensionWindow(GetType())) {
        VsyncStation::GetInstance().SetFrameRateLinkerEnable(false);
    }
}

static void RequestInputMethodCloseKeyboard(bool isNeedKeyboard, bool keepKeyboardFlag)
{
    if (!isNeedKeyboard && !keepKeyboardFlag) {
#ifdef IMF_ENABLE
        WLOGFI("[WMSInput] Notify InputMethod framework close keyboard");
        if (MiscServices::InputMethodController::GetInstance()) {
            int32_t ret = MiscServices::InputMethodController::GetInstance()->RequestHideInput();
            if (ret != 0) { // 0 - NO_ERROR
                WLOGFE("[WMSInput] InputMethod framework close keyboard failed, ret: %{public}d", ret);
            }
        }
#endif
    }
}

void WindowSessionImpl::NotifyUIContentFocusStatus()
{
    if (!isFocused_) {
        CALL_UI_CONTENT(UnFocus);
        return;
    }
    CALL_UI_CONTENT(Focus);
    auto task = [weak = wptr(this)]() {
        auto window = weak.promote();
        if (!window) {
            return;
        }
        bool isNeedKeyboard = false;
        {
            std::lock_guard<std::recursive_mutex> lock(window->mutex_);
            if (window->uiContent_ != nullptr) {
                // isNeedKeyboard is set by arkui and indicates whether the window needs a keyboard or not.
                isNeedKeyboard = window->uiContent_->NeedSoftKeyboard();
            }
        }
        // whether keep the keyboard created by other windows, support system window and app subwindow.
        bool keepKeyboardFlag = (window->property_) ? window->property_->GetKeepKeyboardFlag() : false;
        WLOGFI("[WMSInput] isNeedKeyboard: %{public}d, keepKeyboardFlag: %{public}d",
            isNeedKeyboard, keepKeyboardFlag);
        RequestInputMethodCloseKeyboard(isNeedKeyboard, keepKeyboardFlag);
    };
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (uiContent_ != nullptr) {
        uiContent_->SetOnWindowFocused(task);
        WLOGFI("[WMSInput] set need soft keyboard callback success!");
    }
}

void WindowSessionImpl::NotifyAfterFocused()
{
    {
        std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterFocused, lifecycleListeners);
    }
    if (uiContent_ != nullptr) {
        NotifyUIContentFocusStatus();
    } else {
        shouldReNotifyFocus_ = true;
    }
}

void WindowSessionImpl::NotifyAfterUnfocused(bool needNotifyUiContent)
{
    {
        std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        // use needNotifyUinContent to separate ui content callbacks
        CALL_LIFECYCLE_LISTENER(AfterUnfocused, lifecycleListeners);
    }
    if (needNotifyUiContent) {
        if (uiContent_ == nullptr) {
            shouldReNotifyFocus_ = true;
        }
        CALL_UI_CONTENT(UnFocus);
    }
}

void WindowSessionImpl::NotifyBeforeDestroy(std::string windowName)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    std::shared_ptr<Ace::UIContent> uiContent = std::move(uiContent_);
    auto task = [uiContent]() {
        if (uiContent != nullptr) {
            uiContent->Destroy();
            WLOGFD("[WMSLife]NotifyBeforeDestroy: uiContent destroy success");
        }
    };
    if (handler_) {
        handler_->PostSyncTask(task, "wms:NotifyBeforeDestroy");
    } else {
        task();
    }
    uiContent_ = nullptr;

    if (notifyNativeFunc_) {
        notifyNativeFunc_(windowName);
    }
}

void WindowSessionImpl::NotifyAfterDestroy()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterDestroyed, lifecycleListeners);
}

void WindowSessionImpl::NotifyAfterActive()
{
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterActive, lifecycleListeners);
}

void WindowSessionImpl::NotifyAfterInactive()
{
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterInactive, lifecycleListeners);
}

void WindowSessionImpl::NotifyForegroundFailed(WMError ret)
{
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER_WITH_PARAM(ForegroundFailed, lifecycleListeners, static_cast<int32_t>(ret));
}

void WindowSessionImpl::NotifyBackgroundFailed(WMError ret)
{
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER_WITH_PARAM(BackgroundFailed, lifecycleListeners, static_cast<int32_t>(ret));
}

void WindowSessionImpl::NotifyAfterResumed()
{
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterResumed, lifecycleListeners);
}

void WindowSessionImpl::NotifyAfterPaused()
{
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterPaused, lifecycleListeners);
}

WSError WindowSessionImpl::MarkProcessed(int32_t eventId)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("HostSession is invalid");
        return WSError::WS_DO_NOTHING;
    }
    return hostSession_->MarkProcessed(eventId);
}

void WindowSessionImpl::RegisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener)
{
    WLOGFD("Start register DialogDeathRecipientListener");
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return;
    }
    std::lock_guard<std::recursive_mutex> lockListener(dialogDeathRecipientListenerMutex_);
    RegisterListener(dialogDeathRecipientListeners_[GetPersistentId()], listener);
}

void WindowSessionImpl::UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener)
{
    WLOGFD("Start unregister DialogDeathRecipientListener");
    std::lock_guard<std::recursive_mutex> lockListener(dialogDeathRecipientListenerMutex_);
    UnregisterListener(dialogDeathRecipientListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    WLOGFD("Start register DialogTargetTouchListener");
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::lock_guard<std::recursive_mutex> lockListener(dialogTargetTouchListenerMutex_);
    return RegisterListener(dialogTargetTouchListener_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    WLOGFD("Start unregister DialogTargetTouchListener");
    std::lock_guard<std::recursive_mutex> lockListener(dialogTargetTouchListenerMutex_);
    return UnregisterListener(dialogTargetTouchListener_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterScreenshotListener(const sptr<IScreenshotListener>& listener)
{
    WLOGFD("Start register ScreenshotListener");
    std::lock_guard<std::recursive_mutex> lockListener(screenshotListenerMutex_);
    return RegisterListener(screenshotListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener)
{
    WLOGFD("Start unregister ScreenshotListener");
    std::lock_guard<std::recursive_mutex> lockListener(screenshotListenerMutex_);
    return UnregisterListener(screenshotListeners_[GetPersistentId()], listener);
}

template<typename T>
EnableIfSame<T, IDialogDeathRecipientListener, std::vector<sptr<IDialogDeathRecipientListener>>> WindowSessionImpl::
    GetListeners()
{
    std::vector<sptr<IDialogDeathRecipientListener>> dialogDeathRecipientListener;
    for (auto& listener : dialogDeathRecipientListeners_[GetPersistentId()]) {
        dialogDeathRecipientListener.push_back(listener);
    }
    return dialogDeathRecipientListener;
}

template<typename T>
EnableIfSame<T, IDialogTargetTouchListener,
    std::vector<sptr<IDialogTargetTouchListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IDialogTargetTouchListener>> dialogTargetTouchListener;
    for (auto& listener : dialogTargetTouchListener_[GetPersistentId()]) {
        dialogTargetTouchListener.push_back(listener);
    }
    return dialogTargetTouchListener;
}

template<typename T>
EnableIfSame<T, IScreenshotListener, std::vector<sptr<IScreenshotListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IScreenshotListener>> screenshotListeners;
    for (auto& listener : screenshotListeners_[GetPersistentId()]) {
        screenshotListeners.push_back(listener);
    }
    return screenshotListeners;
}

WSError WindowSessionImpl::NotifyDestroy()
{
    std::lock_guard<std::recursive_mutex> lockListener(dialogDeathRecipientListenerMutex_);
    auto dialogDeathRecipientListener = GetListeners<IDialogDeathRecipientListener>();
    for (auto& listener : dialogDeathRecipientListener) {
        if (listener != nullptr) {
            listener->OnDialogDeathRecipient();
        }
    }
    return WSError::WS_OK;
}

WSError WindowSessionImpl::NotifyCloseExistPipWindow()
{
    WLOGFD("WindowSessionImpl::NotifyCloseExistPipWindow");
    PictureInPictureManager::DoClose(true, false);
    return WSError::WS_OK;
}

void WindowSessionImpl::NotifyTouchDialogTarget(int32_t posX, int32_t posY)
{
    std::lock_guard<std::recursive_mutex> lockListener(dialogTargetTouchListenerMutex_);
    if (hostSession_ != nullptr) {
        hostSession_->ProcessPointDownSession(posX, posY);
    }
    auto dialogTargetTouchListener = GetListeners<IDialogTargetTouchListener>();
    for (auto& listener : dialogTargetTouchListener) {
        if (listener != nullptr) {
            listener->OnDialogTargetTouch();
        }
    }
}

void WindowSessionImpl::NotifyScreenshot()
{
    std::lock_guard<std::recursive_mutex> lockListener(screenshotListenerMutex_);
    auto screenshotListeners = GetListeners<IScreenshotListener>();
    for (auto& listener : screenshotListeners) {
        if (listener != nullptr) {
            listener->OnScreenshot();
        }
    }
}

void WindowSessionImpl::NotifySizeChange(Rect rect, WindowSizeChangeReason reason)
{
    std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
    auto windowChangeListeners = GetListeners<IWindowChangeListener>();
    for (auto& listener : windowChangeListeners) {
        if (listener != nullptr) {
            listener->OnSizeChange(rect, reason);
        }
    }
}

WMError WindowSessionImpl::RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    bool isUpdate = false;
    WMError ret = WMError::WM_OK;
    auto persistentId = GetPersistentId();
    WLOGFD("Start register avoidAreaChange listener, id:%{public}d", persistentId);
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    {
        std::lock_guard<std::recursive_mutex> lockListener(avoidAreaChangeListenerMutex_);
        ret = RegisterListener(avoidAreaChangeListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            return ret;
        }
        if (avoidAreaChangeListeners_[persistentId].size() == 1) {
            isUpdate = true;
        }
    }
    if (isUpdate) {
        ret = SingletonContainer::Get<WindowAdapter>().UpdateSessionAvoidAreaListener(persistentId, true);
    }
    return ret;
}

WMError WindowSessionImpl::UnregisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    bool isUpdate = false;
    WMError ret = WMError::WM_OK;
    auto persistentId = GetPersistentId();
    WLOGFD("Start unregister avoidAreaChange listener, id:%{public}d", persistentId);
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    {
        std::lock_guard<std::recursive_mutex> lockListener(avoidAreaChangeListenerMutex_);
        ret = UnregisterListener(avoidAreaChangeListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            return ret;
        }
        if (avoidAreaChangeListeners_[persistentId].empty()) {
            isUpdate = true;
        }
    }
    if (isUpdate) {
        ret = SingletonContainer::Get<WindowAdapter>().UpdateSessionAvoidAreaListener(persistentId, false);
    }
    return ret;
}

WMError WindowSessionImpl::RegisterExtensionAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    auto persistentId = GetPersistentId();
    WLOGI("Start register extension avoidAreaChange listener, id:%{public}d", persistentId);
    std::lock_guard<std::recursive_mutex> lockListener(avoidAreaChangeListenerMutex_);
    return RegisterListener(avoidAreaChangeListeners_[persistentId], listener);
}

WMError WindowSessionImpl::UnregisterExtensionAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    auto persistentId = GetPersistentId();
    WLOGI("Start unregister extension avoidAreaChange listener, id:%{public}d", persistentId);
    std::lock_guard<std::recursive_mutex> lockListener(avoidAreaChangeListenerMutex_);
    return UnregisterListener(avoidAreaChangeListeners_[persistentId], listener);
}

template<typename T>
EnableIfSame<T, IAvoidAreaChangedListener,
    std::vector<sptr<IAvoidAreaChangedListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IAvoidAreaChangedListener>> windowChangeListeners;
    for (auto& listener : avoidAreaChangeListeners_[GetPersistentId()]) {
        windowChangeListeners.push_back(listener);
    }
    return windowChangeListeners;
}

void WindowSessionImpl::NotifyAvoidAreaChange(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)
{
    std::lock_guard<std::recursive_mutex> lockListener(avoidAreaChangeListenerMutex_);
    auto avoidAreaChangeListeners = GetListeners<IAvoidAreaChangedListener>();
    for (auto& listener : avoidAreaChangeListeners) {
        if (listener != nullptr) {
            listener->OnAvoidAreaChanged(*avoidArea, type);
        }
    }
}

WSError WindowSessionImpl::UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)
{
    WLOGI("UpdateAvoidArea: id:%{public}d, type:%{public}d, top{%{public}d,%{public}d,%{public}d,%{public}d}, "
        "down{%{public}d,%{public}d,%{public}d,%{public}d}", GetPersistentId(), type,
        avoidArea->topRect_.posX_, avoidArea->topRect_.posY_, avoidArea->topRect_.width_, avoidArea->topRect_.height_,
        avoidArea->bottomRect_.posX_, avoidArea->bottomRect_.posY_, avoidArea->bottomRect_.width_,
        avoidArea->bottomRect_.height_);
    NotifyAvoidAreaChange(avoidArea, type);
    return WSError::WS_OK;
}

WMError WindowSessionImpl::RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener)
{
    bool isUpdate = false;
    WMError ret = WMError::WM_OK;
    auto persistentId = GetPersistentId();
    WLOGFD("Start register touchOutside listener, id:%{public}d", persistentId);
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    {
        std::lock_guard<std::recursive_mutex> lockListener(touchOutsideListenerMutex_);
        ret = RegisterListener(touchOutsideListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            return ret;
        }
        if (touchOutsideListeners_[persistentId].size() == 1) {
            isUpdate = true;
        }
    }
    if (isUpdate) {
        ret = SingletonContainer::Get<WindowAdapter>().UpdateSessionTouchOutsideListener(persistentId, true);
    }
    return ret;
}

WMError WindowSessionImpl::UnregisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener)
{
    bool isUpdate = false;
    WMError ret = WMError::WM_OK;
    auto persistentId = GetPersistentId();
    WLOGFD("Start unregister touchOutside listener, id:%{public}d", persistentId);
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    {
        std::lock_guard<std::recursive_mutex> lockListener(touchOutsideListenerMutex_);
        ret = UnregisterListener(touchOutsideListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            return ret;
        }
        if (touchOutsideListeners_[persistentId].empty()) {
            isUpdate = true;
        }
    }
    if (isUpdate) {
        ret = SingletonContainer::Get<WindowAdapter>().UpdateSessionTouchOutsideListener(persistentId, false);
    }
    return ret;
}

template<typename T>
EnableIfSame<T, ITouchOutsideListener, std::vector<sptr<ITouchOutsideListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<ITouchOutsideListener>> windowChangeListeners;
    for (auto& listener : touchOutsideListeners_[GetPersistentId()]) {
        windowChangeListeners.push_back(listener);
    }
    return windowChangeListeners;
}

WSError WindowSessionImpl::NotifyTouchOutside()
{
    std::lock_guard<std::recursive_mutex> lockListener(touchOutsideListenerMutex_);
    auto touchOutsideListeners = GetListeners<ITouchOutsideListener>();
    for (auto& listener : touchOutsideListeners) {
        if (listener != nullptr) {
            listener->OnTouchOutside();
        }
    }
    return WSError::WS_OK;
}

WMError WindowSessionImpl::RegisterWindowVisibilityChangeListener(const IWindowVisibilityListenerSptr& listener)
{
    auto persistentId = GetPersistentId();
    WLOGFD("Start to register window visibility change listener, persistentId=%{public}d.", persistentId);
    WMError ret = WMError::WM_OK;
    bool isFirstRegister = false;
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowVisibilityChangeListenerMutex_);
        ret = RegisterListener(windowVisibilityChangeListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            return ret;
        }
        isFirstRegister = windowVisibilityChangeListeners_[persistentId].size() == 1;
    }

    if (isFirstRegister) {
        ret = SingletonContainer::Get<WindowAdapter>().UpdateSessionWindowVisibilityListener(persistentId, true);
    }
    return ret;
}

WMError WindowSessionImpl::UnregisterWindowVisibilityChangeListener(const IWindowVisibilityListenerSptr& listener)
{
    auto persistentId = GetPersistentId();
    WLOGFD("Start to unregister window visibility change listener, persistentId=%{public}d.", persistentId);
    WMError ret = WMError::WM_OK;
    bool isFirstUnregister = false;
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowVisibilityChangeListenerMutex_);
        ret = UnregisterListener(windowVisibilityChangeListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            return ret;
        }
        isFirstUnregister = windowVisibilityChangeListeners_[persistentId].empty();
    }

    if (isFirstUnregister) {
        ret = SingletonContainer::Get<WindowAdapter>().UpdateSessionWindowVisibilityListener(persistentId, false);
    }
    return ret;
}

template<typename T>
EnableIfSame<T, IWindowVisibilityChangedListener, std::vector<IWindowVisibilityListenerSptr>> WindowSessionImpl::GetListeners()
{
    std::vector<IWindowVisibilityListenerSptr> windowVisibilityChangeListeners;
    for (auto& listener : windowVisibilityChangeListeners_[GetPersistentId()]) {
        windowVisibilityChangeListeners.push_back(listener);
    }
    return windowVisibilityChangeListeners;
}

WSError WindowSessionImpl::NotifyWindowVisibility(bool isVisible)
{
    WLOGFD("Notify window visibility Change, window: name=%{public}s, id=%{public}u, isVisible:%{public}d",
        GetWindowName().c_str(), GetPersistentId(), isVisible);
    std::lock_guard<std::recursive_mutex> lockListener(windowVisibilityChangeListenerMutex_);
    auto windowVisibilityListeners = GetListeners<IWindowVisibilityChangedListener>();
    for (auto& listener : windowVisibilityListeners) {
        if (listener != nullptr) {
            listener->OnWindowVisibilityChangedCallback(isVisible);
        }
    }
    return WSError::WS_OK;
}

void WindowSessionImpl::NotifyPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (!pointerEvent) {
        WLOGFD("Pointer event is nullptr");
        return;
    }

    std::shared_ptr<IInputEventConsumer> inputEventConsumer;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        inputEventConsumer = inputEventConsumer_;
    }
    if (inputEventConsumer != nullptr) {
        WLOGFD("Transfer pointer event to inputEventConsumer");
        (void)inputEventConsumer->OnInputEvent(pointerEvent);
        return;
    }

    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (uiContent_ != nullptr) {
            if (pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_MOVE) {
                WLOGFI("InputTracking id:%{public}d, WindowSessionImpl::NotifyPointerEvent",
                    pointerEvent->GetId());
            }
            (void)uiContent_->ProcessPointerEvent(pointerEvent);
        } else {
            WLOGFW("pointerEvent is not consumed, windowId: %{public}u", GetWindowId());
            pointerEvent->MarkProcessed();
        }
    }
}

void WindowSessionImpl::DispatchKeyEventCallback(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    std::shared_ptr<IInputEventConsumer> inputEventConsumer;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        inputEventConsumer = inputEventConsumer_;
    }
    int32_t keyCode = keyEvent->GetKeyCode();
    int32_t keyAction = keyEvent->GetKeyAction();
    if (keyCode == MMI::KeyEvent::KEYCODE_BACK && keyAction == MMI::KeyEvent::KEY_ACTION_UP) {
        WLOGFI("input event is consumed by back, return");
        if (inputEventConsumer != nullptr) {
            WLOGFD("Transfer key event to inputEventConsumer");
            if (inputEventConsumer->OnInputEvent(keyEvent)) {
                return;
            }
            PerformBack();
            return;
        }
        HandleBackEvent();
        return;
    }
    if (inputEventConsumer != nullptr) {
        WLOGD("Transfer key event to inputEventConsumer");
        (void)inputEventConsumer->OnInputEvent(keyEvent);
    } else if (uiContent_) {
        auto isConsumed = uiContent_->ProcessKeyEvent(keyEvent);
        if (!isConsumed && keyEvent->GetKeyCode() == MMI::KeyEvent::KEYCODE_ESCAPE &&
            property_->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
            property_->GetMaximizeMode() == MaximizeMode::MODE_FULL_FILL) {
            WLOGI("recover from fullscreen cause KEYCODE_ESCAPE");
            Recover();
        }
    }
}

void WindowSessionImpl::NotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed)
{
    if (keyEvent == nullptr) {
        WLOGFE("keyEvent is nullptr");
        return;
    }

    auto dispatchFunc = [weakThis = wptr(this), keyEvent] () {
        auto promoteThis = weakThis.promote();
        if (promoteThis == nullptr) {
            WLOGFW("promoteThis is nullptr");
            return;
        }
        promoteThis->DispatchKeyEventCallback(const_cast<std::shared_ptr<MMI::KeyEvent>&>(keyEvent));
    };
#ifdef IMF_ENABLE
    bool isKeyboardEvent = IsKeyboardEvent(keyEvent);
    if (isKeyboardEvent) {
        WLOGD("Async dispatch keyEvent to input method");
        auto callback = [dispatchFunc] (std::shared_ptr<MMI::KeyEvent>& keyEvent, bool consumed) {
            if (keyEvent == nullptr) {
                WLOGFW("keyEvent is null, consumed:%{public}" PRId32, consumed);
                return;
            }

            if (consumed) {
                WLOGD("Input method has processed key event, id:%{public}" PRId32, keyEvent->GetId());
                return;
            }

            dispatchFunc();
        };
        auto ret = MiscServices::InputMethodController::GetInstance()->DispatchKeyEvent(
            const_cast<std::shared_ptr<MMI::KeyEvent>&>(keyEvent), callback);
        if (ret != 0) {
            WLOGFE("DispatchKeyEvent failed, ret:%{public}" PRId32 ", id:%{public}" PRId32, ret, keyEvent->GetId());
        }
        return;
    }
#endif // IMF_ENABLE
    dispatchFunc();
}

bool WindowSessionImpl::IsKeyboardEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const
{
    int32_t keyCode = keyEvent->GetKeyCode();
    bool isKeyFN = (keyCode == MMI::KeyEvent::KEYCODE_FN);
    bool isKeyBack = (keyCode == MMI::KeyEvent::KEYCODE_BACK);
    bool isKeyboard = (keyCode >= MMI::KeyEvent::KEYCODE_0 && keyCode <= MMI::KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN);
    WLOGI("isKeyFN: %{public}d, isKeyboard: %{public}d", isKeyFN, isKeyboard);
    return (isKeyFN || isKeyboard || isKeyBack);
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

int64_t WindowSessionImpl::GetVSyncPeriod()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return VsyncStation::GetInstance().GetVSyncPeriod();
}

void WindowSessionImpl::FlushFrameRate(uint32_t rate)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (WindowHelper::IsMainWindow(GetType()) || WindowHelper::IsUIExtensionWindow(GetType())) {
        VsyncStation::GetInstance().FlushFrameRate(rate);
    }
}

WMError WindowSessionImpl::UpdateProperty(WSPropertyChangeAction action)
{
    WLOGFD("UpdateProperty, action:%{public}u", action);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SingletonContainer::Get<WindowAdapter>().UpdateSessionProperty(property_, action);
}

sptr<Window> WindowSessionImpl::Find(const std::string& name)
{
    std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
    auto iter = windowSessionMap_.find(name);
    if (iter == windowSessionMap_.end()) {
        return nullptr;
    }
    return iter->second.second;
}

void WindowSessionImpl::SetAceAbilityHandler(const sptr<IAceAbilityHandler>& handler)
{
    if (handler == nullptr) {
        WLOGE("ace ability handler is nullptr");
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    aceAbilityHandler_ = handler;
}

WMError WindowSessionImpl::SetBackgroundColor(const std::string& color)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    uint32_t colorValue;
    if (ColorParser::Parse(color, colorValue)) {
        WLOGD("SetBackgroundColor: window: %{public}s, value: [%{public}s, %{public}u]",
            GetWindowName().c_str(), color.c_str(), colorValue);
        return SetBackgroundColor(colorValue);
    }
    WLOGFE("invalid color string: %{public}s", color.c_str());
    return WMError::WM_ERROR_INVALID_PARAM;
}

WMError WindowSessionImpl::SetBackgroundColor(uint32_t color)
{
    WLOGFD("Report set bg color: %{public}u", GetWindowId());

    // 0xff000000: ARGB style, means Opaque color.
    const bool isAlphaZero = !(color & 0xff000000);
    std::string bundleName;
    std::string abilityName;
    if ((context_ != nullptr) && (context_->GetApplicationInfo() != nullptr)) {
        bundleName = context_->GetBundleName();
        abilityName = context_->GetApplicationInfo()->name;
    }

    if (isAlphaZero && WindowHelper::IsMainWindow(GetType())) {
        auto& reportInstance = SingletonContainer::Get<WindowInfoReporter>();
        reportInstance.ReportZeroOpacityInfoImmediately(bundleName, abilityName);
    }

    if (uiContent_ != nullptr) {
        uiContent_->SetBackgroundColor(color);
        return WMError::WM_OK;
    }
    if (aceAbilityHandler_ != nullptr) {
        aceAbilityHandler_->SetBackgroundColor(color);
        return WMError::WM_OK;
    }
    WLOGFD("FA mode could not set bg color: %{public}u", GetWindowId());
    return WMError::WM_ERROR_INVALID_OPERATION;
}

sptr<Window> WindowSessionImpl::FindWindowById(uint32_t winId)
{
    if (windowSessionMap_.empty()) {
        WLOGFE("Please create mainWindow First!");
        return nullptr;
    }
    for (auto iter = windowSessionMap_.begin(); iter != windowSessionMap_.end(); iter++) {
        if (winId == iter->second.first) {
            WLOGD("FindWindow id: %{public}u", winId);
            return iter->second.second;
        }
    }
    WLOGFE("Cannot find Window, id: %{public}d", winId);
    return nullptr;
}

std::vector<sptr<Window>> WindowSessionImpl::GetSubWindow(int parentId)
{
    auto iter = subWindowSessionMap_.find(parentId);
    if (iter == subWindowSessionMap_.end()) {
        return std::vector<sptr<Window>>();
    }
    return std::vector<sptr<Window>>(subWindowSessionMap_[parentId].begin(), subWindowSessionMap_[parentId].end());
}

uint32_t WindowSessionImpl::GetBackgroundColor() const
{
    if (uiContent_ != nullptr) {
        return uiContent_->GetBackgroundColor();
    }
    WLOGD("uiContent is nullptr, windowId: %{public}u, use FA mode", GetWindowId());
    if (aceAbilityHandler_ != nullptr) {
        return aceAbilityHandler_->GetBackgroundColor();
    }
    WLOGFD("FA mode does not get bg color: %{public}u", GetWindowId());
    return 0xffffffff; // means no background color been set, default color is white
}

WMError WindowSessionImpl::SetLayoutFullScreenByApiVersion(bool status)
{
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetWindowGravity(WindowGravity gravity, uint32_t percent)
{
    return SingletonContainer::Get<WindowAdapter>().SetSessionGravity(GetPersistentId(),
        static_cast<SessionGravity>(gravity), percent);
}

WMError WindowSessionImpl::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetSpecificBarProperty(WindowType type, const SystemBarProperty& property)
{
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetTextFieldAvoidInfo(double textFieldPositionY, double textFieldHeight)
{
    property_->SetTextFieldPositionY(textFieldPositionY);
    property_->SetTextFieldHeight(textFieldHeight);
    return WMError::WM_OK;
}

void WindowSessionImpl::NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info)
{
    WLOGFD("NotifyOccupiedAreaChangeInfo, safeHeight: %{public}u "
           "occupied rect: x %{public}u, y %{public}u, w %{public}u, h %{public}u",
           info->safeHeight_, info->rect_.posX_, info->rect_.posY_, info->rect_.width_, info->rect_.height_);
    std::lock_guard<std::recursive_mutex> lockListener(occupiedAreaChangeListenerMutex_);
    auto occupiedAreaChangeListeners = GetListeners<IOccupiedAreaChangeListener>();
    for (auto& listener : occupiedAreaChangeListeners) {
        if (listener != nullptr) {
            if (property_->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING &&
                system::GetParameter("const.product.devicetype", "unknown") == "phone") {
                sptr<OccupiedAreaChangeInfo> occupiedAreaChangeInfo = new OccupiedAreaChangeInfo();
                listener->OnSizeChange(occupiedAreaChangeInfo);
                continue;
            }
            listener->OnSizeChange(info);
        }
    }
}

KeyboardAnimationConfig WindowSessionImpl::GetKeyboardAnimationConfig()
{
    return windowSystemConfig_.keyboardAnimationConfig_;
}

void WindowSessionImpl::DumpSessionElementInfo(const std::vector<std::string>& params)
{
    WLOGFD("DumpSessionElementInfo");
}

WSError WindowSessionImpl::UpdateMaximizeMode(MaximizeMode mode)
{
    return WSError::WS_OK;
}

WMError WindowSessionImpl::TransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
    int64_t uiExtensionIdLevel)
{
    return WMError::WM_OK;
}

void WindowSessionImpl::NotifySessionForeground(uint32_t reason, bool withAnimation)
{
    WLOGFD("NotifySessionForeground");
}

void WindowSessionImpl::NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits)
{
    WLOGFD("NotifySessionBackground");
}

WSError WindowSessionImpl::UpdateTitleInTargetPos(bool isShow, int32_t height)
{
    return WSError::WS_OK;
}

void WindowSessionImpl::UpdatePiPRect(const uint32_t width, const uint32_t height, PiPRectUpdateReason reason)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("HostSession is invalid");
        return;
    }
    hostSession_->UpdatePiPRect(width, height, reason);
}

void WindowSessionImpl::NotifyWindowStatusChange(WindowMode mode)
{
    WLOGFD("NotifyWindowStatusChange");
    auto WindowStatus = WindowStatus::WINDOW_STATUS_UNDEFINED;
    if (mode == WindowMode::WINDOW_MODE_FLOATING) {
        WindowStatus = WindowStatus::WINDOW_STATUS_FLOATING;
        if (property_->GetMaximizeMode() == MaximizeMode::MODE_AVOID_SYSTEM_BAR) {
            WindowStatus = WindowStatus::WINDOW_STATUS_MAXMIZE;
        }
    } else if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        WindowStatus = WindowStatus::WINDOW_STATUS_SPLITSCREEN;
    }
    if (mode == WindowMode::WINDOW_MODE_FULLSCREEN) {
        WindowStatus = WindowStatus::WINDOW_STATUS_FULLSCREEN;
    }
    if (state_ == WindowState::STATE_HIDDEN) {
        WindowStatus = WindowStatus::WINDOW_STATUS_MINIMIZE;
    }

    std::lock_guard<std::recursive_mutex> lockListener(windowStatusChangeListenerMutex_);
    auto windowStatusChangeListeners = GetListeners<IWindowStatusChangeListener>();
    for (auto& listener : windowStatusChangeListeners) {
        if (listener != nullptr) {
            listener->OnWindowStatusChange(WindowStatus);
        }
    }
}

void WindowSessionImpl::NotifyTransformChange(const Transform& transform)
{
    WLOGFI("NotifyWindowStatusChange");
    if (uiContent_ != nullptr) {
        uiContent_->UpdateTransform(transform);
    }
}
} // namespace Rosen
} // namespace OHOS
