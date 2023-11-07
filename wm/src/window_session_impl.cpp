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
#include <ipc_skeleton.h>
#include <hisysevent.h>
#ifdef IMF_ENABLE
#include <input_method_controller.h>
#endif // IMF_ENABLE
#include <transaction/rs_interfaces.h>
#include <transaction/rs_transaction.h>

#include "anr_handler.h"
#include "color_parser.h"
#include "display_manager.h"
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

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowSessionImpl"};
}

std::map<int32_t, std::vector<sptr<IWindowLifeCycle>>> WindowSessionImpl::lifecycleListeners_;
std::map<int32_t, std::vector<sptr<IWindowChangeListener>>> WindowSessionImpl::windowChangeListeners_;
std::map<int32_t, std::vector<sptr<IAvoidAreaChangedListener>>> WindowSessionImpl::avoidAreaChangeListeners_;
std::map<int32_t, std::vector<sptr<IDialogDeathRecipientListener>>> WindowSessionImpl::dialogDeathRecipientListeners_;
std::map<int32_t, std::vector<sptr<IDialogTargetTouchListener>>> WindowSessionImpl::dialogTargetTouchListener_;
std::map<int32_t, std::vector<sptr<IOccupiedAreaChangeListener>>> WindowSessionImpl::occupiedAreaChangeListeners_;
std::map<int32_t, std::vector<sptr<IScreenshotListener>>> WindowSessionImpl::screenshotListeners_;
std::map<int32_t, std::vector<sptr<ITouchOutsideListener>>> WindowSessionImpl::touchOutsideListeners_;
std::recursive_mutex WindowSessionImpl::globalMutex_;
std::map<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>> WindowSessionImpl::windowSessionMap_;
std::map<int32_t, std::vector<sptr<WindowSessionImpl>>> WindowSessionImpl::subWindowSessionMap_;

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
        if (uiContent_ != nullptr) {                          \
            uiContent_->uiContentCb();                        \
        }                                                     \
    } while (0)

WindowSessionImpl::WindowSessionImpl(const sptr<WindowOption>& option)
{
    WLOGFD("WindowSessionImpl");
    property_ = new (std::nothrow) WindowSessionProperty();
    if (property_ == nullptr) {
        WLOGFE("Property is null");
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
    WLOGFD("~WindowSessionImpl, id: %{public}d", GetPersistentId());
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
        WLOGW("already destroyed or not created! id: %{public}d state_: %{public}u", GetPersistentId(), state_);
    }
    return res;
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

WMError WindowSessionImpl::Create(const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<Rosen::ISession>& iSession)
{
    return WMError::WM_OK;
}

WMError WindowSessionImpl::Connect()
{
    if (hostSession_ == nullptr) {
        WLOGFE("Session is null!");
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
    WLOGFI("Window Connect [name:%{public}s, id:%{public}d, type:%{public}u], ret:%{public}u",
        property_->GetWindowName().c_str(), GetPersistentId(), property_->GetWindowType(), ret);
    return static_cast<WMError>(ret);
}

WMError WindowSessionImpl::Show(uint32_t reason, bool withAnimation)
{
    WLOGFD("Window Show [name:%{public}s, id:%{public}d, type:%{public}u], reason:%{public}u state:%{public}u",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), GetType(), reason, state_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_SHOWN) {
        WLOGFD("window session is alreay shown [name:%{public}s, id:%{public}d, type: %{public}u]",
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
    WLOGFD("id:%{public}d Hide, reason:%{public}u, state:%{public}u",
        GetPersistentId(), reason, state_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        WLOGFD("window session is alreay hidden [name:%{public}s, id:%{public}d, type: %{public}u]",
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
    WLOGFI("Id: %{public}d Destroy, state_:%{public}u, needNotifyServer: %{public}d, needClearListener: %{public}d",
        GetPersistentId(), state_, needNotifyServer, needClearListener);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (hostSession_ != nullptr) {
        hostSession_->Disconnect();
    }
    NotifyBeforeDestroy(GetWindowName());
    if (needClearListener) {
        ClearListenersById(GetPersistentId());
    }
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        state_ = WindowState::STATE_DESTROYED;
        requestState_ = WindowState::STATE_DESTROYED;
    }
    hostSession_ = nullptr;
    windowSessionMap_.erase(property_->GetWindowName());
    DelayedSingleton<ANRHandler>::GetInstance()->OnWindowDestroyed(GetPersistentId());
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
    if (GetRect().width_ != 0 && GetRect().height_ != 0 && WindowHelper::IsMainWindow(GetType())) {
        // 50 session动画阈值
        int widthRange = 50;
        int heightRange = 50;
        if (std::abs((int)(GetRect().width_) - (int)(wmRect.width_)) > widthRange ||
            std::abs((int)(GetRect().height_) - (int)(wmRect.height_)) > heightRange) {
            wmReason = wmReason == WindowSizeChangeReason::UNDEFINED ?
                WindowSizeChangeReason::MAXIMIZE : wmReason;
        }
    }
    auto preRect = GetRect();
    if (preRect.width_ == wmRect.height_ && preRect.height_ == wmRect.width_ &&
            (preRect.width_ != wmRect.width_ || preRect.height_ != wmRect.height_)) {
        wmReason = WindowSizeChangeReason::ROTATION;
    }
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
    WLOGFI("update rect [%{public}d, %{public}d, %{public}u, %{public}u], reason:%{public}u"
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
        protocol.SetDuration(400);
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
    WLOGFD("Report update focus: %{public}u", isFocused);
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
        WLOGFE("uiContent_ is null!");
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
    float density = display->GetDisplayInfo()->GetVirtualPixelRatio();
    config.SetDensity(density);
    uiContent_->UpdateViewportConfig(config, reason, rsTransaction);
    WLOGFD("Id:%{public}d, windowRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
        GetPersistentId(), rect.posX_, rect.posY_, rect.width_, rect.height_);
}

int32_t WindowSessionImpl::GetFloatingWindowParentId()
{
    if (context_.get() == nullptr) {
        return INVALID_SESSION_ID;
    }

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
    return SetUIContentInner(contentInfo, env, storage, isdistributed, false, ability);
}

WMError WindowSessionImpl::SetUIContentByName(
    const std::string& contentInfo, napi_env env, napi_value storage, AppExecFwk::Ability* ability)
{
    return SetUIContentInner(contentInfo, env, storage, false, true, ability);
}

WMError WindowSessionImpl::SetUIContentInner(const std::string& contentInfo, napi_env env, napi_value storage,
    bool isdistributed, bool isLoadedByName, AppExecFwk::Ability* ability)
{
    WLOGFD("NapiSetUIContent: %{public}s state:%{public}u", contentInfo.c_str(), state_);
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
        WLOGFE("fail to NapiSetUIContent id: %{public}d", GetPersistentId());
        return WMError::WM_ERROR_NULLPTR;
    }
    if (isdistributed) {
        uiContent->Restore(this, contentInfo, storage);
    } else if (isLoadedByName) {
        uiContent->InitializeByName(this, contentInfo, storage);
    } else {
        uiContent->Initialize(this, contentInfo, storage);
    }
    // make uiContent available after Initialize/Restore
    uiContent_ = std::move(uiContent);

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
    WLOGFD("notify uiContent window size change end");
    return WMError::WM_OK;
}

void WindowSessionImpl::UpdateDecorEnable(bool needNotify)
{
    if (needNotify) {
        if (uiContent_ != nullptr) {
            uiContent_->UpdateWindowMode(GetMode(), IsDecorEnable());
            WLOGFD("Notify uiContent window mode change end");
        }
        NotifyModeChange(GetMode(), IsDecorEnable());
    }
}

void WindowSessionImpl::NotifyModeChange(WindowMode mode, bool hasDeco)
{
    auto windowChangeListeners = GetListeners<IWindowChangeListener>();
    for (auto& listener : windowChangeListeners) {
        if (listener.GetRefPtr() != nullptr) {
            listener.GetRefPtr()->OnModeChange(mode, hasDeco);
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
    WLOGFI("lastReqOrientation: %{public}u target:%{public}u state_:%{public}u",
        property_->GetRequestedOrientation(), orientation, state_);
    if (property_->GetRequestedOrientation() == orientation) {
        return;
    }
    property_->SetRequestedOrientation(orientation);
    if (state_ == WindowState::STATE_SHOWN) {
        UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION);
    }
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
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    return RegisterListener(lifecycleListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    WLOGFD("Start register");
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    return RegisterListener(occupiedAreaChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    WLOGFD("Start unregister");
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    return UnregisterListener(occupiedAreaChangeListeners_[GetPersistentId()], listener);
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
EnableIfSame<T, IWindowLifeCycle, std::vector<sptr<IWindowLifeCycle>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IWindowLifeCycle>> lifecycleListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : lifecycleListeners_[GetPersistentId()]) {
            lifecycleListeners.push_back(listener);
        }
    }
    return lifecycleListeners;
}

template<typename T>
EnableIfSame<T, IWindowChangeListener, std::vector<sptr<IWindowChangeListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IWindowChangeListener>> windowChangeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : windowChangeListeners_[GetPersistentId()]) {
            windowChangeListeners.push_back(listener);
        }
    }
    return windowChangeListeners;
}

template<typename T>
EnableIfSame<T, IOccupiedAreaChangeListener, std::vector<sptr<IOccupiedAreaChangeListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IOccupiedAreaChangeListener>> occupiedAreaChangeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : occupiedAreaChangeListeners_[GetPersistentId()]) {
            occupiedAreaChangeListeners.push_back(listener);
        }
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

void WindowSessionImpl::ClearListenersById(int32_t persistentId)
{
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    ClearUselessListeners(lifecycleListeners_, persistentId);
    ClearUselessListeners(windowChangeListeners_, persistentId);
    ClearUselessListeners(avoidAreaChangeListeners_, persistentId);
    ClearUselessListeners(dialogDeathRecipientListeners_, persistentId);
    ClearUselessListeners(dialogTargetTouchListener_, persistentId);
    ClearUselessListeners(screenshotListeners_, persistentId);
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
    std::shared_ptr<Ace::UIContent> uiContent = std::move(uiContent_);
    auto task = [uiContent]() {
        if (uiContent != nullptr) {
            uiContent->Destroy();
            WLOGFD("NotifyBeforeDestroy: uiContent destroy success");
        }
    };
    if (handler_) {
        handler_->PostTask(task);
    } else {
        task();
    }
    uiContent_ = nullptr;

    if (notifyNativeFunc_) {
        notifyNativeFunc_(windowName);
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

void WindowSessionImpl::NotifyBackgroundFailed(WMError ret)
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER_WITH_PARAM(BackgroundFailed, lifecycleListeners, static_cast<int32_t>(ret));
}

void WindowSessionImpl::NotifyAfterResumed()
{
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterResumed, lifecycleListeners);
}

void WindowSessionImpl::NotifyAfterPaused()
{
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
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    RegisterListener(dialogDeathRecipientListeners_[GetPersistentId()], listener);
}

void WindowSessionImpl::UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener)
{
    WLOGFD("Start unregister DialogDeathRecipientListener");
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    UnregisterListener(dialogDeathRecipientListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    WLOGFD("Start register DialogTargetTouchListener");
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    return RegisterListener(dialogTargetTouchListener_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    WLOGFD("Start unregister DialogTargetTouchListener");
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    return UnregisterListener(dialogTargetTouchListener_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterScreenshotListener(const sptr<IScreenshotListener>& listener)
{
    WLOGFD("Start register ScreenshotListener");
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    return RegisterListener(screenshotListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener)
{
    WLOGFD("Start unregister ScreenshotListener");
    std::lock_guard<std::recursive_mutex> lock(globalMutex_);
    return UnregisterListener(screenshotListeners_[GetPersistentId()], listener);
}

template<typename T>
EnableIfSame<T, IDialogDeathRecipientListener, std::vector<sptr<IDialogDeathRecipientListener>>> WindowSessionImpl::
    GetListeners()
{
    std::vector<sptr<IDialogDeathRecipientListener>> dialogDeathRecipientListener;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : dialogDeathRecipientListeners_[GetPersistentId()]) {
            dialogDeathRecipientListener.push_back(listener);
        }
    }
    return dialogDeathRecipientListener;
}

template<typename T>
EnableIfSame<T, IDialogTargetTouchListener,
    std::vector<sptr<IDialogTargetTouchListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IDialogTargetTouchListener>> dialogTargetTouchListener;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : dialogTargetTouchListener_[GetPersistentId()]) {
            dialogTargetTouchListener.push_back(listener);
        }
    }
    return dialogTargetTouchListener;
}

template<typename T>
EnableIfSame<T, IScreenshotListener, std::vector<sptr<IScreenshotListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IScreenshotListener>> screenshotListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : screenshotListeners_[GetPersistentId()]) {
            screenshotListeners.push_back(listener);
        }
    }
    return screenshotListeners;
}

WSError WindowSessionImpl::NotifyDestroy()
{
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
    PictureInPictureManager::DoClose(false);
    return WSError::WS_OK;
}

void WindowSessionImpl::NotifyTouchDialogTarget()
{
    auto dialogTargetTouchListener = GetListeners<IDialogTargetTouchListener>();
    for (auto& listener : dialogTargetTouchListener) {
        if (listener != nullptr) {
            listener->OnDialogTargetTouch();
        }
    }
}

void WindowSessionImpl::NotifyScreenshot()
{
    auto screenshotListeners = GetListeners<IScreenshotListener>();
    for (auto& listener : screenshotListeners) {
        if (listener != nullptr) {
            listener->OnScreenshot();
        }
    }
}

void WindowSessionImpl::NotifySizeChange(Rect rect, WindowSizeChangeReason reason)
{
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
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
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
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
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

template<typename T>
EnableIfSame<T, IAvoidAreaChangedListener,
    std::vector<sptr<IAvoidAreaChangedListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IAvoidAreaChangedListener>> windowChangeListeners;
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : avoidAreaChangeListeners_[GetPersistentId()]) {
            windowChangeListeners.push_back(listener);
        }
    }
    return windowChangeListeners;
}

void WindowSessionImpl::NotifyAvoidAreaChange(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)
{
    auto avoidAreaChangeListeners = GetListeners<IAvoidAreaChangedListener>();
    for (auto& listener : avoidAreaChangeListeners) {
        if (listener != nullptr) {
            listener->OnAvoidAreaChanged(*avoidArea, type);
        }
    }
}

WSError WindowSessionImpl::UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)
{
    WLOGI("UpdateAvoidArea, id:%{public}d", GetPersistentId());
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
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
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
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        ret = UnregisterListener(touchOutsideListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            return ret;
        }
        if (avoidAreaChangeListeners_[persistentId].empty()) {
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
    {
        std::lock_guard<std::recursive_mutex> lock(globalMutex_);
        for (auto& listener : touchOutsideListeners_[GetPersistentId()]) {
            windowChangeListeners.push_back(listener);
        }
    }
    return windowChangeListeners;
}

WSError WindowSessionImpl::NotifyTouchOutside()
{
    auto touchOutsideListeners = GetListeners<ITouchOutsideListener>();
    for (auto& listener : touchOutsideListeners) {
        if (listener != nullptr) {
            listener->OnTouchOutside();
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

void WindowSessionImpl::NotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed)
{
    if (keyEvent == nullptr) {
        WLOGFE("keyEvent is nullptr");
        return;
    }

    bool inputMethodHasProcessed = false;
#ifdef IMF_ENABLE
    bool isKeyboardEvent = IsKeyboardEvent(keyEvent);
    if (isKeyboardEvent) {
        WLOGD("dispatch keyEvent to input method");
        inputMethodHasProcessed = MiscServices::InputMethodController::GetInstance()->DispatchKeyEvent(keyEvent);
    }
#endif // IMF_ENABLE
    if (inputMethodHasProcessed) {
        WLOGFD("input method has processed key event");
        return;
    }
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
        isConsumed = uiContent_->ProcessKeyEvent(keyEvent);
        if (!isConsumed && keyEvent->GetKeyCode() == MMI::KeyEvent::KEYCODE_ESCAPE &&
            property_->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
            property_->GetMaximizeMode() == MaximizeMode::MODE_FULL_FILL) {
            WLOGI("recover from fullscreen cause KEYCODE_ESCAPE");
            Recover();
        }
    }
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

WMError WindowSessionImpl::UpdateProperty(WSPropertyChangeAction action)
{
    WLOGFD("UpdateProperty, action:%{public}u", action);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SessionManager::GetInstance().UpdateProperty(property_, action);
}

sptr<Window> WindowSessionImpl::Find(const std::string& name)
{
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
    WLOGFE("FA mode could not set bg color: %{public}u", GetWindowId());
    return WMError::WM_ERROR_INVALID_OPERATION;
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
    WLOGFE("FA mode does not get bg color: %{public}u", GetWindowId());
    return 0xffffffff; // means no background color been set, default color is white
}

WMError WindowSessionImpl::SetLayoutFullScreenByApiVersion(bool status)
{
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetWindowGravity(WindowGravity gravity, uint32_t percent)
{
    return SessionManager::GetInstance().SetSessionGravity(GetPersistentId(),
        static_cast<SessionGravity>(gravity), percent);
}

WMError WindowSessionImpl::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    return WMError::WM_OK;
}

void WindowSessionImpl::NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info)
{
    WLOGFD("NotifyOccupiedAreaChangeInfo, safeHeight: %{public}u "
           "occupied rect: x %{public}u, y %{public}u, w %{public}u, h %{public}u",
           info->safeHeight_, info->rect_.posX_, info->rect_.posY_, info->rect_.width_, info->rect_.height_);
    auto occupiedAreaChangeListeners = GetListeners<IOccupiedAreaChangeListener>();
    for (auto& listener : occupiedAreaChangeListeners) {
        if (listener != nullptr) {
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
} // namespace Rosen
} // namespace OHOS
