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
constexpr int32_t FULL_CIRCLE_DEGREE = 360;
constexpr int32_t ONE_FOURTH_FULL_CIRCLE_DEGREE = 90;
constexpr int32_t FORCE_SPLIT_MODE = 5;

Ace::ContentInfoType GetAceContentInfoType(BackupAndRestoreType type)
{
    auto contentInfoType = Ace::ContentInfoType::NONE;
    switch (type) {
        case BackupAndRestoreType::CONTINUATION:
            contentInfoType = Ace::ContentInfoType::CONTINUATION;
            break;
        case BackupAndRestoreType::APP_RECOVERY:
            contentInfoType = Ace::ContentInfoType::APP_RECOVERY;
            break;
        case BackupAndRestoreType::RESOURCESCHEDULE_RECOVERY:
            contentInfoType = Ace::ContentInfoType::RESOURCESCHEDULE_RECOVERY;
            break;
        case BackupAndRestoreType::NONE:
            [[fallthrough]];
        default:
            break;
    }
    return contentInfoType;
}
}

std::map<int32_t, std::vector<sptr<IWindowLifeCycle>>> WindowSessionImpl::lifecycleListeners_;
std::map<int32_t, std::vector<sptr<IDisplayMoveListener>>> WindowSessionImpl::displayMoveListeners_;
std::map<int32_t, std::vector<sptr<IWindowChangeListener>>> WindowSessionImpl::windowChangeListeners_;
std::map<int32_t, std::vector<sptr<IAvoidAreaChangedListener>>> WindowSessionImpl::avoidAreaChangeListeners_;
std::map<int32_t, std::vector<sptr<IDialogDeathRecipientListener>>> WindowSessionImpl::dialogDeathRecipientListeners_;
std::map<int32_t, std::vector<sptr<IDialogTargetTouchListener>>> WindowSessionImpl::dialogTargetTouchListener_;
std::map<int32_t, std::vector<sptr<IOccupiedAreaChangeListener>>> WindowSessionImpl::occupiedAreaChangeListeners_;
std::map<int32_t, std::vector<sptr<IScreenshotListener>>> WindowSessionImpl::screenshotListeners_;
std::map<int32_t, std::vector<sptr<ITouchOutsideListener>>> WindowSessionImpl::touchOutsideListeners_;
std::map<int32_t, std::vector<IWindowVisibilityListenerSptr>> WindowSessionImpl::windowVisibilityChangeListeners_;
std::map<int32_t, std::vector<IWindowNoInteractionListenerSptr>> WindowSessionImpl::windowNoInteractionListeners_;
std::map<int32_t, std::vector<sptr<IWindowTitleButtonRectChangedListener>>>
    WindowSessionImpl::windowTitleButtonRectChangeListeners_;
std::map<int32_t, std::vector<sptr<IWindowRectChangeListener>>> WindowSessionImpl::windowRectChangeListeners_;
std::map<int32_t, sptr<ISubWindowCloseListener>> WindowSessionImpl::subWindowCloseListeners_;
std::recursive_mutex WindowSessionImpl::lifeCycleListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::avoidAreaChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::dialogDeathRecipientListenerMutex_;
std::recursive_mutex WindowSessionImpl::dialogTargetTouchListenerMutex_;
std::recursive_mutex WindowSessionImpl::occupiedAreaChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::screenshotListenerMutex_;
std::recursive_mutex WindowSessionImpl::touchOutsideListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowVisibilityChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowNoInteractionListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowStatusChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowTitleButtonRectChangeListenerMutex_;
std::mutex WindowSessionImpl::displayMoveListenerMutex_;
std::mutex WindowSessionImpl::windowRectChangeListenerMutex_;
std::mutex WindowSessionImpl::subWindowCloseListenersMutex_;
std::map<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>> WindowSessionImpl::windowSessionMap_;
std::shared_mutex WindowSessionImpl::windowSessionMutex_;
std::map<int32_t, std::vector<sptr<WindowSessionImpl>>> WindowSessionImpl::subWindowSessionMap_;
std::map<int32_t, std::vector<sptr<IWindowStatusChangeListener>>> WindowSessionImpl::windowStatusChangeListeners_;
bool WindowSessionImpl::isUIExtensionAbilityProcess_ = false;

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

#define CALL_UI_CONTENT(uiContentCb)                                           \
    do {                                                                       \
        std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();   \
        if (uiContent != nullptr) {                                            \
            uiContent->uiContentCb();                                          \
        }                                                                      \
    } while (0)

#define CHECK_HOST_SESSION_RETURN_IF_NULL(hostSession)                         \
    do {                                                                       \
        if ((hostSession) == nullptr) {                                        \
            TLOGE(WmsLogTag::DEFAULT, "hostSession is null");                  \
            return;                                                            \
        }                                                                      \
    } while (false)

#define CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, ret)              \
    do {                                                                       \
        if ((hostSession) == nullptr) {                                        \
            TLOGE(WmsLogTag::DEFAULT, "hostSession is null");                  \
            return ret;                                                        \
        }                                                                      \
    } while (false)

WindowSessionImpl::WindowSessionImpl(const sptr<WindowOption>& option)
{
    WLOGFD("[WMSCom]WindowSessionImpl");
    property_ = new (std::nothrow) WindowSessionProperty();
    if (property_ == nullptr) {
        WLOGFE("[WMSCom]Property is null");
        return;
    }
    WindowType optionWindowType = option->GetWindowType();
    SessionInfo sessionInfo;
    sessionInfo.bundleName_ = option->GetBundleName();
    property_->SetSessionInfo(sessionInfo);
    property_->SetWindowName(option->GetWindowName());
    property_->SetRequestRect(option->GetWindowRect());
    property_->SetWindowType(optionWindowType);
    property_->SetFocusable(option->GetFocusable());
    property_->SetTouchable(option->GetTouchable());
    property_->SetDisplayId(option->GetDisplayId());
    property_->SetParentId(option->GetParentId());
    property_->SetTurnScreenOn(option->IsTurnScreenOn());
    property_->SetKeepScreenOn(option->IsKeepScreenOn());
    property_->SetWindowMode(option->GetWindowMode());
    property_->SetWindowFlags(option->GetWindowFlags());
    property_->SetCallingSessionId(option->GetCallingWindow());
    property_->SetExtensionFlag(option->GetExtensionTag());
    property_->SetTopmost(option->GetWindowTopmost());
    property_->SetUIExtensionUsage(static_cast<UIExtensionUsage>(option->GetUIExtensionUsage()));
    isMainHandlerAvailable_ = option->GetMainHandlerAvailable();
    isIgnoreSafeArea_ = WindowHelper::IsSubWindow(optionWindowType);
    windowOption_ = option;
    surfaceNode_ = CreateSurfaceNode(property_->GetWindowName(), optionWindowType);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
    if (surfaceNode_ != nullptr) {
        vsyncStation_ = std::make_shared<VsyncStation>(surfaceNode_->GetId());
    }
}

void WindowSessionImpl::MakeSubOrDialogWindowDragableAndMoveble()
{
    TLOGI(WmsLogTag::WMS_LIFE, "Called %{public}d.", GetPersistentId());
    auto isPC = windowSystemConfig_.uiType_ == "pc";
    bool isPcAppInPad = property_->GetIsPcAppInPad();
    if ((isPC || IsFreeMultiWindowMode() || isPcAppInPad) && windowOption_ != nullptr) {
        if (WindowHelper::IsSubWindow(property_->GetWindowType())) {
            TLOGI(WmsLogTag::WMS_LIFE, "create subwindow, title: %{public}s, decorEnable: %{public}d",
                windowOption_->GetSubWindowTitle().c_str(), windowOption_->GetSubWindowDecorEnable());
            property_->SetDecorEnable(windowOption_->GetSubWindowDecorEnable());
            property_->SetDragEnabled(windowOption_->GetSubWindowDecorEnable());
            subWindowTitle_ = windowOption_->GetSubWindowTitle();
        }
        bool isDialog = WindowHelper::IsDialogWindow(property_->GetWindowType());
        if (isDialog) {
            bool dialogDecorEnable = windowOption_->GetDialogDecorEnable();
            property_->SetDecorEnable(dialogDecorEnable);
            property_->SetDragEnabled(dialogDecorEnable);
            dialogTitle_ = windowOption_->GetDialogTitle();
            TLOGI(WmsLogTag::WMS_LIFE, "create dialogWindow, title: %{public}s, decorEnable: %{public}d",
                dialogTitle_.c_str(), dialogDecorEnable);
        }
    }
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
        case WindowType::WINDOW_TYPE_UI_EXTENSION:
            TLOGI(WmsLogTag::WMS_LIFE, "uiExtensionUsage = %{public}u", property_->GetUIExtensionUsage());
            if (property_->GetUIExtensionUsage() == UIExtensionUsage::CONSTRAINED_EMBEDDED) {
                rsSurfaceNodeType = RSSurfaceNodeType::UI_EXTENSION_NODE;
            } else {
                rsSurfaceNodeType = RSSurfaceNodeType::DEFAULT;
            }
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
    bool res = ((GetHostSession() == nullptr) || (GetPersistentId() == INVALID_SESSION_ID) ||
        (state_ == WindowState::STATE_DESTROYED));
    if (res) {
        WLOGW("[WMSLife] already destroyed or not created! id: %{public}d state_: %{public}u",
            GetPersistentId(), state_);
    }
    return res;
}

bool WindowSessionImpl::IsMainHandlerAvailable() const
{
    TLOGI(WmsLogTag::DEFAULT, "id:%{public}d, isAvailable:%{public}u",
        GetPersistentId(), isMainHandlerAvailable_);
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
    std::lock_guard<std::mutex> lock(hostSessionMutex_);
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
    std::shared_lock<std::shared_mutex> lock(windowSessionMutex_);
    // check window name, same window names are forbidden
    if (windowSessionMap_.find(name) != windowSessionMap_.end()) {
        WLOGFE("WindowName(%{public}s) already exists.", name.c_str());
        return WMError::WM_ERROR_REPEAT_OPERATION;
    }

    // check if camera floating window is already exists
    if (property_->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA ||
        property_->GetWindowType() == WindowType::WINDOW_TYPE_PIP) {
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
    TLOGI(WmsLogTag::WMS_LIFE, "Called");
    auto displayId = property_->GetDisplayId();
    if (displayId == DISPLAY_ID_INVALID) {
        auto defaultDisplayId = SingletonContainer::IsDestroyed() ? DISPLAY_ID_INVALID :
            SingletonContainer::Get<DisplayManager>().GetDefaultDisplayId();
        defaultDisplayId = (defaultDisplayId == DISPLAY_ID_INVALID)? 0 : defaultDisplayId;
        property_->SetDisplayId(defaultDisplayId);
        WLOGFI("Reset displayId to %{public}" PRIu64, defaultDisplayId);
    }
}

WMError WindowSessionImpl::Create(const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<Rosen::ISession>& iSession, const std::string& identityToken)
{
    return WMError::WM_OK;
}

WMError WindowSessionImpl::Connect()
{
    TLOGI(WmsLogTag::WMS_LIFE, "Called");
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_NULLPTR);
    sptr<ISessionStage> iSessionStage(this);
    auto windowEventChannel = new (std::nothrow) WindowEventChannel(iSessionStage);
    if (windowEventChannel && property_) {
        windowEventChannel->SetIsUIExtension(property_->GetWindowType() == WindowType::WINDOW_TYPE_UI_EXTENSION);
        windowEventChannel->SetUIExtensionUsage(property_->GetUIExtensionUsage());
    }
    sptr<IWindowEventChannel> iWindowEventChannel(windowEventChannel);
    sptr<IRemoteObject> token = context_ ? context_->GetToken() : nullptr;
    if (token) {
        property_->SetTokenState(true);
    }
    auto ret = hostSession->Connect(
        iSessionStage, iWindowEventChannel, surfaceNode_, windowSystemConfig_, property_,
        token, identityToken_);
    TLOGI(WmsLogTag::WMS_LIFE, "Window Connect [name:%{public}s, id:%{public}d, type:%{public}u], ret:%{public}u",
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
    NotifyKeyEvent(keyEvent, isConsumed, false);
}

bool WindowSessionImpl::PreNotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent != nullptr) {
        return uiContent->ProcessKeyEvent(keyEvent, true);
    }
    return false;
}

bool WindowSessionImpl::NotifyOnKeyPreImeEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    return PreNotifyKeyEvent(keyEvent);
}

WMError WindowSessionImpl::Show(uint32_t reason, bool withAnimation)
{
    TLOGI(WmsLogTag::WMS_LIFE, "name:%{public}s, id:%{public}d, type:%{public}u, reason:%{public}u, state:%{public}u",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), GetType(), reason, state_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_SHOWN) {
        TLOGD(WmsLogTag::WMS_LIFE, "window session is alreay shown [name:%{public}s, id:%{public}d, type: %{public}u]",
            property_->GetWindowName().c_str(), GetPersistentId(), property_->GetWindowType());
        NotifyAfterForeground(true, true);
        return WMError::WM_OK;
    }

    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
    WSError ret = hostSession->Foreground(property_);
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
    TLOGI(WmsLogTag::WMS_LIFE, "id:%{public}d Hide, reason:%{public}u, state:%{public}u",
        GetPersistentId(), reason, state_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        TLOGD(WmsLogTag::WMS_LIFE, "window session is alreay hidden [name:%{public}s, id:%{public}d, type: %{public}u]",
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
    TLOGI(WmsLogTag::WMS_LIFE, "Id: %{public}d Destroy, state_:%{public}u, needNotifyServer: %{public}d, "
        "needClearListener: %{public}d", GetPersistentId(), state_, needNotifyServer, needClearListener);
    if (IsWindowSessionInvalid()) {
        WLOGFW("[WMSLife]session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    {
        auto hostSession = GetHostSession();
        if (hostSession != nullptr) {
            hostSession->Disconnect();
        }
    }
    NotifyBeforeDestroy(GetWindowName());
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        state_ = WindowState::STATE_DESTROYED;
        requestState_ = WindowState::STATE_DESTROYED;
    }
    {
        std::lock_guard<std::mutex> lock(hostSessionMutex_);
        hostSession_ = nullptr;
    }
    {
        std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
        windowSessionMap_.erase(property_->GetWindowName());
    }
    NotifyAfterDestroy();
    if (needClearListener) {
        ClearListenersById(GetPersistentId());
    }
    if (context_) {
        context_.reset();
    }
    ClearVsyncStation();
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
    if (reason == SizeChangeReason::DRAG_END) {
        property_->SetRequestRect(wmRect);
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "updateRect %{public}s, reason:%{public}u"
        "WindowInfo:[name: %{public}s, persistentId:%{public}d]", rect.ToString().c_str(),
        wmReason, GetWindowName().c_str(), GetPersistentId());
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
        "WindowSessionImpl::UpdateRect%d [%d, %d, %u, %u] reason:%u",
        GetPersistentId(), wmRect.posX_, wmRect.posY_, wmRect.width_, wmRect.height_, wmReason);
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
    return WSError::WS_OK;
}

void WindowSessionImpl::UpdateRectForRotation(const Rect& wmRect, const Rect& preRect,
    WindowSizeChangeReason wmReason, const std::shared_ptr<RSTransaction>& rsTransaction)
{
    handler_->PostTask([weak = wptr(this), wmReason, wmRect, preRect, rsTransaction]() mutable {
        HITRACE_METER_NAME(HITRACE_TAG_WINDOW_MANAGER, "WindowSessionImpl::UpdateRectForRotation");
        auto window = weak.promote();
        if (!window) {
            return;
        }
        if (rsTransaction) {
            RSTransaction::FlushImplicitTransaction();
            rsTransaction->Begin();
        }
        RSInterfaces::GetInstance().EnableCacheForRotation();
        window->rotationAnimationCount_++;
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(ANIMATION_TIME);
        // animation curve: cubic [0.2, 0.0, 0.2, 1.0]
        auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.2, 0.0, 0.2, 1.0);
        RSNode::OpenImplicitAnimation(protocol, curve, [weak]() {
            auto window = weak.promote();
            if (!window) {
                return;
            }
            window->rotationAnimationCount_--;
            if (window->rotationAnimationCount_ == 0) {
                RSInterfaces::GetInstance().DisableCacheForRotation();
                window->NotifyRotationAnimationEnd();
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

void WindowSessionImpl::NotifyRotationAnimationEnd()
{
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        WLOGFW("uiContent is null!");
        return;
    }
    uiContent->NotifyRotationAnimationEnd();
}

void WindowSessionImpl::GetTitleButtonVisible(bool isPC, bool &hideMaximizeButton, bool &hideMinimizeButton,
    bool &hideSplitButton)
{
    if (!isPC) {
        return;
    }
    if (hideMaximizeButton > !windowTitleVisibleFlags_.isMaximizeVisible) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "isMaximizeVisible param INVALID");
    }
    hideMaximizeButton = hideMaximizeButton || (!windowTitleVisibleFlags_.isMaximizeVisible);
    if (hideMinimizeButton > !windowTitleVisibleFlags_.isMinimizeVisible) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "isMinimizeVisible param INVALID");
    }
    hideMinimizeButton = hideMinimizeButton || (!windowTitleVisibleFlags_.isMinimizeVisible);
    if (hideSplitButton > !windowTitleVisibleFlags_.isSplitVisible) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "isSplitVisible param INVALID");
    }
    hideSplitButton = hideSplitButton || (!windowTitleVisibleFlags_.isSplitVisible);
}

void WindowSessionImpl::UpdateDensity()
{
    auto preRect = GetRect();
    UpdateViewportConfig(preRect, WindowSizeChangeReason::UNDEFINED);
    WLOGFI("WindowSessionImpl::UpdateDensity [%{public}d, %{public}d, %{public}u, %{public}u]",
        preRect.posX_, preRect.posY_, preRect.width_, preRect.height_);
}

WSError WindowSessionImpl::UpdateOrientation()
{
    TLOGD(WmsLogTag::DMS, "UpdateOrientation, wid: %{public}d", GetPersistentId());
    return WSError::WS_OK;
}

WSError WindowSessionImpl::UpdateDisplayId(uint64_t displayId)
{
    property_->SetDisplayId(displayId);
    return WSError::WS_OK;
}

WSError WindowSessionImpl::UpdateFocus(bool isFocused)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "Report update focus: %{public}u, id: %{public}d", isFocused, GetPersistentId());
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
    TLOGD(WmsLogTag::WMS_FOCUS, "window id = %{public}d, isFocused = %{public}d", GetPersistentId(), isFocused_);
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

bool WindowSessionImpl::IsNotifyInteractiveDuplicative(bool interactive)
{
    if (interactive == interactive_ && hasFirstNotifyInteractive_) {
        return true;
    }
    hasFirstNotifyInteractive_ = true;
    if (interactive_ != interactive) {
        interactive_ = interactive;
    }
    return false;
}

void WindowSessionImpl::NotifyForegroundInteractiveStatus(bool interactive)
{
    WLOGFI("NotifyForegroundInteractiveStatus %{public}d", interactive);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return;
    }
    if (IsNotifyInteractiveDuplicative(interactive)) {
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

float WindowSessionImpl::GetVirtualPixelRatio(sptr<DisplayInfo> displayInfo)
{
    return displayInfo->GetVirtualPixelRatio();
}

void WindowSessionImpl::UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction)
{
    if (rect.width_ == 0 || rect.height_ == 0) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "window rect width: %{public}d, height: %{public}d", rect.width_, rect.height_);
        return;
    }
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("display is null!");
        return;
    }
    auto displayInfo = display->GetDisplayInfo();
    if (displayInfo == nullptr) {
        WLOGFE("displayInfo is null!");
        return;
    }
    auto rotation =  ONE_FOURTH_FULL_CIRCLE_DEGREE * static_cast<uint32_t>(displayInfo->GetRotation());
    auto deviceRotation = static_cast<uint32_t>(displayInfo->GetDefaultDeviceRotationOffset());
    uint32_t transformHint = (rotation + deviceRotation) % FULL_CIRCLE_DEGREE;
    float density = GetVirtualPixelRatio(displayInfo);
    int32_t orientation = static_cast<int32_t>(displayInfo->GetDisplayOrientation());

    virtualPixelRatio_ = density;
    TLOGI(WmsLogTag::WMS_LAYOUT, "[rotation,deviceRotation,transformHint,virtualPixelRatio]:[%{public}u,"
        "%{public}u,%{public}u,%{public}f]", rotation, deviceRotation, transformHint, virtualPixelRatio_);
    Ace::ViewportConfig config;
    config.SetSize(rect.width_, rect.height_);
    config.SetPosition(rect.posX_, rect.posY_);
    config.SetDensity(density);
    config.SetOrientation(orientation);
    config.SetTransformHint(transformHint);
    {
        std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
        if (uiContent == nullptr) {
            WLOGFW("uiContent is null!");
            return;
        }
        uiContent->UpdateViewportConfig(config, reason, rsTransaction);
    }
    if (WindowHelper::IsUIExtensionWindow(GetType())) {
        TLOGD(WmsLogTag::WMS_LAYOUT, "Id:%{public}d reason:%{public}d windowRect:[%{public}d,%{public}d,\
            %{public}u,%{public}u] displayOrientation:%{public}d", GetPersistentId(), reason, rect.posX_, rect.posY_,
            rect.width_, rect.height_, orientation);
    } else {
        TLOGI(WmsLogTag::WMS_LAYOUT, "Id:%{public}d reason:%{public}d windowRect:[%{public}d,%{public}d,\
            %{public}u,%{public}u] displayOrientation:%{public}d", GetPersistentId(), reason, rect.posX_, rect.posY_,
            rect.width_, rect.height_, orientation);
    }
}

int32_t WindowSessionImpl::GetFloatingWindowParentId()
{
    if (context_.get() == nullptr) {
        return INVALID_SESSION_ID;
    }
    std::shared_lock<std::shared_mutex> lock(windowSessionMutex_);
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
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr || !IsDecorEnable()) {
        return;
    }
    auto isPC = windowSystemConfig_.uiType_ == "pc";
    bool isPcAppInPad = property_->GetIsPcAppInPad();
    WindowType windowType = GetType();
    bool isSubWindow = WindowHelper::IsSubWindow(windowType);
    bool isDialogWindow = WindowHelper::IsDialogWindow(windowType);
    if ((isPC || IsFreeMultiWindowMode() || isPcAppInPad) && (isSubWindow || isDialogWindow)) {
        WLOGFD("hide other buttons except close");
        uiContent->HideWindowTitleButton(true, true, true);
        return;
    }
    auto modeSupportInfo = property_->GetModeSupportInfo();
    bool hideSplitButton = !(modeSupportInfo & WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY);
    // not support fullscreen in split and floating mode, or not support float in fullscreen mode
    bool hideMaximizeButton = (!(modeSupportInfo & WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN) &&
        (GetMode() == WindowMode::WINDOW_MODE_FLOATING || WindowHelper::IsSplitWindowMode(GetMode()))) ||
        (!(modeSupportInfo & WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING) &&
        GetMode() == WindowMode::WINDOW_MODE_FULLSCREEN);
    bool hideMinimizeButton = false;
    GetTitleButtonVisible(isPC, hideMaximizeButton, hideMinimizeButton, hideSplitButton);
    TLOGI(WmsLogTag::WMS_LAYOUT, "[hideSplit, hideMaximize, hideMinimizeButton]: [%{public}d, %{public}d, %{public}d]",
        hideSplitButton, hideMaximizeButton, hideMinimizeButton);
    if (property_->GetCompatibleModeInPc()) {
        uiContent->HideWindowTitleButton(hideSplitButton, true, hideMinimizeButton);
    } else {
        uiContent->HideWindowTitleButton(hideSplitButton, hideMaximizeButton, hideMinimizeButton);
    }
}

WMError WindowSessionImpl::NapiSetUIContent(const std::string& contentInfo, napi_env env, napi_value storage,
    BackupAndRestoreType type, sptr<IRemoteObject> token, AppExecFwk::Ability* ability)
{
    return SetUIContentInner(contentInfo, env, storage,
        type == BackupAndRestoreType::NONE ? WindowSetUIContentType::DEFAULT : WindowSetUIContentType::RESTORE,
        type, ability);
}

WMError WindowSessionImpl::SetUIContentByName(
    const std::string& contentInfo, napi_env env, napi_value storage, AppExecFwk::Ability* ability)
{
    return SetUIContentInner(contentInfo, env, storage, WindowSetUIContentType::BY_NAME,
        BackupAndRestoreType::NONE, ability);
}

WMError WindowSessionImpl::SetUIContentByAbc(
    const std::string& contentInfo, napi_env env, napi_value storage, AppExecFwk::Ability* ability)
{
    return SetUIContentInner(contentInfo, env, storage, WindowSetUIContentType::BY_ABC,
        BackupAndRestoreType::NONE, ability);
}

void WindowSessionImpl::DestroyExistUIContent()
{
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent) {
        uiContent->Destroy();
    }
}

WMError WindowSessionImpl::InitUIContent(const std::string& contentInfo, napi_env env, napi_value storage,
    WindowSetUIContentType setUIContentType, BackupAndRestoreType restoreType, AppExecFwk::Ability* ability,
    OHOS::Ace::UIContentErrorCode& aceRet)
{
    DestroyExistUIContent();
    std::unique_ptr<Ace::UIContent> uiContent;
    if (ability != nullptr) {
        uiContent = Ace::UIContent::Create(ability);
    } else {
        uiContent = Ace::UIContent::Create(context_.get(), reinterpret_cast<NativeEngine*>(env));
    }
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "fail to NapiSetUIContent id: %{public}d", GetPersistentId());
        return WMError::WM_ERROR_NULLPTR;
    }
    switch (setUIContentType) {
        default:
        case WindowSetUIContentType::DEFAULT: {
            if (isUIExtensionAbilityProcess_ && property_->GetExtensionFlag() == true) {
                // subWindow created by UIExtensionAbility
                uiContent->SetUIExtensionSubWindow(true);
                uiContent->SetUIExtensionAbilityProcess(true);
            } else {
                auto routerStack = GetRestoredRouterStack();
                auto type = GetAceContentInfoType(BackupAndRestoreType::RESOURCESCHEDULE_RECOVERY);
                if (!routerStack.empty() &&
                    uiContent->Restore(this, routerStack, storage, type) == Ace::UIContentErrorCode::NO_ERRORS) {
                    TLOGI(WmsLogTag::WMS_LIFE, "Restore router stack succeed.");
                    break;
                }
            }
            aceRet = uiContent->Initialize(this, contentInfo, storage);
            break;
        }
        case WindowSetUIContentType::RESTORE:
            aceRet = uiContent->Restore(this, contentInfo, storage, GetAceContentInfoType(restoreType));
            break;
        case WindowSetUIContentType::BY_NAME:
            aceRet = uiContent->InitializeByName(this, contentInfo, storage);
            break;
        case WindowSetUIContentType::BY_ABC:
            auto abcContent = GetAbcContent(contentInfo);
            aceRet = uiContent->Initialize(this, abcContent, storage);
            break;
    }
    // make uiContent available after Initialize/Restore
    {
        std::unique_lock<std::shared_mutex> lock(uiContentMutex_);
        uiContent_ = std::move(uiContent);
        RegisterFrameLayoutCallback();
        WLOGFI("UIContent Initialize, isUIExtensionSubWindow:%{public}d, isUIExtensionAbilityProcess:%{public}d",
            uiContent_->IsUIExtensionSubWindow(), uiContent_->IsUIExtensionAbilityProcess());
    }
    return WMError::WM_OK;
}

void WindowSessionImpl::RegisterFrameLayoutCallback()
{
    uiContent_->SetFrameLayoutFinishCallback([weakThis = wptr(this)]() {
        auto promoteThis = weakThis.promote();
        if (promoteThis != nullptr && promoteThis->surfaceNode_ != nullptr) {
            bool setCallBackEnable = true;
            if (promoteThis->enableSetBufferAvailableCallback_.compare_exchange_strong(setCallBackEnable, false)) {
                // false: Make the function callable
                promoteThis->surfaceNode_->SetIsNotifyUIBufferAvailable(false);
            }
        }
    });
}

WMError WindowSessionImpl::SetUIContentInner(const std::string& contentInfo, napi_env env, napi_value storage,
    WindowSetUIContentType setUIContentType, BackupAndRestoreType restoreType, AppExecFwk::Ability* ability)
{
    TLOGI(WmsLogTag::WMS_LIFE, "NapiSetUIContent: %{public}s state:%{public}u", contentInfo.c_str(), state_);
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_LIFE, "interrupt set uicontent because window is invalid! window state: %{public}d",
            state_);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    OHOS::Ace::UIContentErrorCode aceRet = OHOS::Ace::UIContentErrorCode::NO_ERRORS;
    WMError initUIContentRet = InitUIContent(contentInfo, env, storage, setUIContentType, restoreType, ability, aceRet);
    if (initUIContentRet != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_LIFE, "Init UIContent fail, ret:%{public}u", initUIContentRet);
        return initUIContentRet;
    }
    WindowType winType = GetType();
    bool isSubWindow = WindowHelper::IsSubWindow(winType);
    bool isDialogWindow = WindowHelper::IsDialogWindow(winType);
    if (IsDecorEnable()) {
        if (isSubWindow) {
            SetAPPWindowLabel(subWindowTitle_);
        } else if (isDialogWindow) {
            SetAPPWindowLabel(dialogTitle_);
        }
    }

    AppForceLandscapeConfig config = {};
    if (WindowHelper::IsMainWindow(winType) && GetAppForceLandscapeConfig(config) == WMError::WM_OK &&
        config.mode_ == FORCE_SPLIT_MODE) {
        SetForceSplitEnable(true, config.homePage_);
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
    } else if (isSubWindow) {
        SetLayoutFullScreenByApiVersion(isIgnoreSafeArea_);
        isIgnoreSafeAreaNeedNotify_ = false;
    }

    UpdateDecorEnable(true);
    if (state_ == WindowState::STATE_SHOWN) {
        // UIContent may be nullptr when show window, need to notify again when window is shown
        {
            std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
            if (uiContent != nullptr) {
                uiContent->Foreground();
            }
        }
        UpdateTitleButtonVisibility();
    }
    UpdateViewportConfig(GetRect(), WindowSizeChangeReason::UNDEFINED);
    if (shouldReNotifyFocus_) {
        // uiContent may be nullptr when notify focus status, need to notify again when uiContent is not empty.
        NotifyUIContentFocusStatus();
        shouldReNotifyFocus_ = false;
    }
    if (aceRet != OHOS::Ace::UIContentErrorCode::NO_ERRORS) {
        WLOGFE("failed to init or restore uicontent with file %{public}s. errorCode: %{public}d",
            contentInfo.c_str(), static_cast<uint16_t>(aceRet));
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    TLOGD(WmsLogTag::WMS_LIFE, "notify uiContent window size change end");
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
    int len = end - begin;
    WLOGFD("abc file: %{public}s, size: %{public}d", abcPath.c_str(), len);

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
    {
        std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
        if (uiContent != nullptr) {
            WindowMode mode = GetMode();
            bool decorVisible = mode == WindowMode::WINDOW_MODE_FLOATING ||
                mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY ||
                (mode == WindowMode::WINDOW_MODE_FULLSCREEN && !property_->IsLayoutFullScreen());
            WLOGFD("[WSLayout]Notify uiContent window mode change end,decorVisible:%{public}d", decorVisible);
            if (windowSystemConfig_.freeMultiWindowSupport_) {
                auto isSubWindow = WindowHelper::IsSubWindow(GetType());
                decorVisible = decorVisible && (windowSystemConfig_.freeMultiWindowEnable_ ||
                        (property_->GetIsPcAppInPad() && isSubWindow));
            }
            uiContent->UpdateDecorVisible(decorVisible, isDecorEnable);
            return;
        }
    }
    std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
    auto windowChangeListeners = GetListeners<IWindowChangeListener>();
    for (auto& listener : windowChangeListeners) {
        if (listener.GetRefPtr() != nullptr) {
            listener.GetRefPtr()->OnModeChange(GetMode(), isDecorEnable);
        }
    }
}

void WindowSessionImpl::UpdateDecorEnable(bool needNotify, WindowMode mode)
{
    if (mode == WindowMode::WINDOW_MODE_UNDEFINED){
        mode = GetMode();
    }
    if (needNotify) {
        {
            std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
            if (uiContent != nullptr) {
                bool decorVisible = mode == WindowMode::WINDOW_MODE_FLOATING ||
                    mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY ||
                    (mode == WindowMode::WINDOW_MODE_FULLSCREEN && !property_->IsLayoutFullScreen());
                if (windowSystemConfig_.freeMultiWindowSupport_) {
                    auto isSubWindow = WindowHelper::IsSubWindow(GetType());
                    decorVisible = decorVisible && (windowSystemConfig_.freeMultiWindowEnable_ ||
                            (property_->GetIsPcAppInPad() && isSubWindow));
                }
                WLOGFD("[WSLayout]Notify uiContent window mode change end,decorVisible:%{public}d", decorVisible);
                uiContent->UpdateDecorVisible(decorVisible, IsDecorEnable());
            }
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

    if (GetHostSession()) {
        property_->SetWindowMode(mode);
        property_->SetDecorEnable(hasDeco);
    }
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_MODE);
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE);
}

std::shared_ptr<RSSurfaceNode> WindowSessionImpl::GetSurfaceNode() const
{
    TLOGI(WmsLogTag::DEFAULT, "name:%{public}s, id:%{public}d",
        property_->GetWindowName().c_str(), GetPersistentId());
    return surfaceNode_;
}

const std::shared_ptr<AbilityRuntime::Context> WindowSessionImpl::GetContext() const
{
    TLOGI(WmsLogTag::DEFAULT, "name:%{public}s, id:%{public}d",
        property_->GetWindowName().c_str(), GetPersistentId());
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
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    TLOGI(WmsLogTag::WMS_FOCUS, "set focusable: windowId = %{public}d, isFocusable = %{public}d",
        property_->GetPersistentId(), isFocusable);
    property_->SetFocusable(isFocusable);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE);
}

bool WindowSessionImpl::GetFocusable() const
{
    bool isFocusable = property_->GetFocusable();
    TLOGD(WmsLogTag::WMS_FOCUS, "get focusable: windowId = %{public}d, isFocusable = %{public}d",
        property_->GetPersistentId(), isFocusable);
    return isFocusable;
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

WMError WindowSessionImpl::SetTopmost(bool topmost)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "set topmost");
    auto isPC = windowSystemConfig_.uiType_ == "pc";
    if (!isPC) {
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetTopmost(topmost);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_TOPMOST);
}

bool WindowSessionImpl::IsTopmost() const
{
    return property_->IsTopmost();
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

WMError WindowSessionImpl::SetLandscapeMultiWindow(bool isLandscapeMultiWindow)
{
    TLOGI(WmsLogTag::WMS_MULTI_WINDOW, "SetLandscapeMultiWindow, isLandscapeMultiWindow:%{public}d",
        isLandscapeMultiWindow);
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "Session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
    hostSession->SetLandscapeMultiWindow(isLandscapeMultiWindow);
    return WMError::WM_OK;
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
    TLOGD(WmsLogTag::DEFAULT, "SetWindowType %{public}u type %{public}u", GetWindowId(), static_cast<uint32_t>(type));
    if (type != WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW && !SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DEFAULT, "set window type permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::DEFAULT, "Session is invalid");
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
        TLOGE(WmsLogTag::DEFAULT, "invalid brightness value: %{public}f", brightness);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (!WindowHelper::IsAppWindow(GetType())) {
        TLOGE(WmsLogTag::DEFAULT, "non app window does not support set brightness, type: %{public}u", GetType());
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    if (!property_) {
        TLOGE(WmsLogTag::DEFAULT, "window property is not existed");
        return WMError::WM_ERROR_NULLPTR;
    }
    property_->SetBrightness(brightness);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS);
}

float WindowSessionImpl::GetBrightness() const
{
    return property_->GetBrightness();
}

void WindowSessionImpl::SetRequestedOrientation(Orientation orientation)
{
    TLOGI(WmsLogTag::WMS_MAIN, "id:%{public}u lastReqOrientation: %{public}u target:%{public}u state_:%{public}u",
        GetPersistentId(), property_->GetRequestedOrientation(), orientation, state_);
    bool isUserOrientation = IsUserOrientation(orientation);
    if (property_->GetRequestedOrientation() == orientation && !isUserOrientation) {
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

std::string WindowSessionImpl::GetContentInfo(BackupAndRestoreType type)
{
    WLOGFD("GetContentInfo");
    if (type == BackupAndRestoreType::NONE) {
        return "";
    }
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        WLOGFE("fail to GetContentInfo id: %{public}d", GetPersistentId());
        return "";
    }
    return uiContent->GetContentInfo(GetAceContentInfoType(type));
}

WMError WindowSessionImpl::SetRestoredRouterStack(const std::string& routerStack)
{
    TLOGD(WmsLogTag::WMS_LIFE, "Set restored router stack.");
    restoredRouterStack_ = routerStack;
    return WMError::WM_OK;
}

std::string WindowSessionImpl::GetRestoredRouterStack()
{
    TLOGD(WmsLogTag::WMS_LIFE, "Get restored router stack.");
    return std::move(restoredRouterStack_);
}

Ace::UIContent* WindowSessionImpl::GetUIContent() const
{
    return uiContent_.get();
}

std::shared_ptr<Ace::UIContent> WindowSessionImpl::GetUIContentSharedPtr() const
{
    std::shared_lock<std::shared_mutex> lock(uiContentMutex_);
    return uiContent_;
}

Ace::UIContent* WindowSessionImpl::GetUIContentWithId(uint32_t winId) const
{
    sptr<Window> targetWindow = FindWindowById(winId);
    if (targetWindow == nullptr) {
        WLOGE("target window is null");
        return nullptr;
    }
    return targetWindow->GetUIContent();
}

void WindowSessionImpl::OnNewWant(const AAFwk::Want& want)
{
    WLOGFI("Window [name:%{public}s, id:%{public}d]",
        property_->GetWindowName().c_str(), GetPersistentId());
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent != nullptr) {
        uiContent->OnNewWant(want);
    }
}

WMError WindowSessionImpl::SetAPPWindowLabel(const std::string& label)
{
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent->SetAppWindowTitle(label);
    WLOGI("Set app window label success, label : %{public}s", label.c_str());
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon)
{
    if (icon == nullptr) {
        WLOGFE("window icon is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent->SetAppWindowIcon(icon);
    WLOGI("Set app window icon success");
    return WMError::WM_OK;
}

WMError WindowSessionImpl::RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    WLOGFD("Start register");
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    return RegisterListener(lifecycleListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener)
{
    WLOGFD("start register");
    std::lock_guard<std::mutex> lockListener(displayMoveListenerMutex_);
    return RegisterListener(displayMoveListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener)
{
    WLOGFD("Start unregister");
    std::lock_guard<std::mutex> lockListener(displayMoveListenerMutex_);
    return UnregisterListener(displayMoveListeners_[GetPersistentId()], listener);
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
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent->SetContainerModalTitleVisible(isVisible, true);
    WLOGI("Change the visibility of decor success");
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetSubWindowModal(bool isModal)
{
    if (!WindowHelper::IsSubWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_SUB, "called by invalid window type, type:%{public}d", GetType());
        return WMError::WM_ERROR_INVALID_CALLING;
    }

    return isModal ? AddWindowFlag(WindowFlag::WINDOW_FLAG_IS_MODAL) :
        RemoveWindowFlag(WindowFlag::WINDOW_FLAG_IS_MODAL);
}

WMError WindowSessionImpl::SetDecorHeight(int32_t decorHeight)
{
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64, property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    auto displayInfo = display->GetDisplayInfo();
    if (displayInfo == nullptr) {
        WLOGFE("get display info failed displayId:%{public}" PRIu64, property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    float vpr = GetVirtualPixelRatio(displayInfo);
    int32_t decorHeightWithPx = static_cast<int32_t>(decorHeight * vpr);
    {
        std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
        if (uiContent == nullptr) {
            WLOGFE("uicontent is empty");
            return WMError::WM_ERROR_NULLPTR;
        }
        uiContent->SetContainerModalTitleHeight(decorHeightWithPx);
    }
    auto hostSession = GetHostSession();
    if (hostSession != nullptr) {
        hostSession->SetCustomDecorHeight(decorHeight);
    }
    WLOGI("Set app window decor height success, height : %{public}d", decorHeight);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::GetDecorHeight(int32_t& height)
{
    {
        std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
        if (uiContent == nullptr) {
            WLOGFE("uiContent is nullptr, windowId: %{public}u", GetWindowId());
            return WMError::WM_ERROR_NULLPTR;
        }
        height = uiContent->GetContainerModalTitleHeight();
    }
    if (height == -1) {
        height = 0;
        WLOGFE("Get app window decor height failed");
        return WMError::WM_OK;
    }
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64, property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    auto displayInfo = display->GetDisplayInfo();
    if (displayInfo == nullptr) {
        WLOGFE("get display info failed displayId:%{public}" PRIu64, property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    float vpr = GetVirtualPixelRatio(displayInfo);
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
    Rect decorRect;
    Rect titleButtonLeftRect;
    bool res = false;
    {
        std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
        if (uiContent == nullptr) {
            WLOGFE("uicontent is empty");
            return WMError::WM_ERROR_NULLPTR;
        }
        res = uiContent->GetContainerModalButtonsRect(decorRect, titleButtonLeftRect);
    }
    if (!res) {
        WLOGFE("get window title buttons area failed");
        titleButtonRect.IsUninitializedRect();
        return WMError::WM_OK;
    }
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64, property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    auto displayInfo = display->GetDisplayInfo();
    if (displayInfo == nullptr) {
        WLOGFE("get display info failed displayId:%{public}" PRIu64, property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    float vpr = GetVirtualPixelRatio(displayInfo);
    if (MathHelper::NearZero(vpr)) {
        WLOGFE("get title buttons area failed, because of wrong vpr: %{public}f", vpr);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    titleButtonRect.posX_ = static_cast<int32_t>(decorRect.width_) -
        static_cast<int32_t>(titleButtonLeftRect.width_) - titleButtonLeftRect.posX_;
    titleButtonRect.posX_ = static_cast<int32_t>(titleButtonRect.posX_ / vpr);
    titleButtonRect.posY_ = static_cast<int32_t>(titleButtonLeftRect.posY_ / vpr);
    titleButtonRect.width_ = static_cast<uint32_t>(titleButtonLeftRect.width_ / vpr);
    titleButtonRect.height_ = static_cast<uint32_t>(titleButtonLeftRect.height_ / vpr);
    return WMError::WM_OK;
}

WSError WindowSessionImpl::GetUIContentRemoteObj(sptr<IRemoteObject>& uiContentRemoteObj)
{
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "uiContent is nullptr. Failed to get uiContentRemoteObj");
        return WSError::WS_ERROR_NULLPTR;
    }
    uiContentRemoteObj = uiContent->GetRemoteObj();
    return WSError::WS_OK;
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
    if (display == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64, property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    auto displayInfo = display->GetDisplayInfo();
    if (displayInfo == nullptr) {
        WLOGFE("get display info failed displayId:%{public}" PRIu64, property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    float vpr = GetVirtualPixelRatio(displayInfo);
    if (MathHelper::NearZero(vpr)) {
        WLOGFE("register title button rect change listener failed, because of wrong vpr: %{public}f", vpr);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent != nullptr) {
        uiContent->SubscribeContainerModalButtonsRectChange([vpr, this](Rect& decorRect, Rect& titleButtonLeftRect) {
            TitleButtonRect titleButtonRect;
            titleButtonRect.posX_ = static_cast<int32_t>(decorRect.width_) -
                static_cast<int32_t>(titleButtonLeftRect.width_) - titleButtonLeftRect.posX_;
            titleButtonRect.posX_ = static_cast<int32_t>(titleButtonRect.posX_ / vpr);
            titleButtonRect.posY_ = static_cast<int32_t>(titleButtonLeftRect.posY_ / vpr);
            titleButtonRect.width_ = static_cast<uint32_t>(titleButtonLeftRect.width_ / vpr);
            titleButtonRect.height_ = static_cast<uint32_t>(titleButtonLeftRect.height_ / vpr);
            NotifyWindowTitleButtonRectChange(titleButtonRect);
        });
    }
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
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent != nullptr) {
        uiContent->SubscribeContainerModalButtonsRectChange(nullptr);
    }
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

template<typename T>
EnableIfSame<T, IWindowRectChangeListener,
    std::vector<sptr<IWindowRectChangeListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IWindowRectChangeListener>> windowRectChangeListeners;
    for (auto& listener : windowRectChangeListeners_[GetPersistentId()]) {
        windowRectChangeListeners.push_back(listener);
    }
    return windowRectChangeListeners;
}

WMError WindowSessionImpl::RegisterWindowRectChangeListener(const sptr<IWindowRectChangeListener>& listener)
{
    WMError ret = WMError::WM_OK;
    {
        std::lock_guard<std::mutex> lockListener(windowRectChangeListenerMutex_);
        ret = RegisterListener(windowRectChangeListeners_[GetPersistentId()], listener);
    }
    auto hostSession = GetHostSession();
    if (hostSession != nullptr && ret == WMError::WM_OK) {
        hostSession->UpdateRectChangeListenerRegistered(true);
    }
    return ret;
}

WMError WindowSessionImpl::UnregisterWindowRectChangeListener(const sptr<IWindowRectChangeListener>& listener)
{
    WMError ret = WMError::WM_OK;
    bool windowRectChangeListenersEmpty = false;
    {
        std::lock_guard<std::mutex> lockListener(windowRectChangeListenerMutex_);
        ret = UnregisterListener(windowRectChangeListeners_[GetPersistentId()], listener);
        windowRectChangeListenersEmpty = windowRectChangeListeners_.count(GetPersistentId()) == 0 ||
                                         windowRectChangeListeners_[GetPersistentId()].empty();
    }
    auto hostSession = GetHostSession();
    if (hostSession != nullptr && windowRectChangeListenersEmpty) {
        hostSession->UpdateRectChangeListenerRegistered(false);
    }
    return ret;
}

template<typename T>
EnableIfSame<T, ISubWindowCloseListener, sptr<ISubWindowCloseListener>> WindowSessionImpl::GetListeners()
{
    sptr<ISubWindowCloseListener> subWindowCloseListeners;
    subWindowCloseListeners = subWindowCloseListeners_[GetPersistentId()];
    return subWindowCloseListeners;
}

WMError WindowSessionImpl::RegisterSubWindowCloseListeners(const sptr<ISubWindowCloseListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!WindowHelper::IsSubWindow(GetType()) && !WindowHelper::IsSystemSubWindow(GetType())) {
        WLOGFE("window type is not supported");
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    std::lock_guard<std::mutex> lockListener(subWindowCloseListenersMutex_);
    subWindowCloseListeners_[GetPersistentId()] = listener;
    return WMError::WM_OK;
}

WMError WindowSessionImpl::UnregisterSubWindowCloseListeners(const sptr<ISubWindowCloseListener>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!WindowHelper::IsSubWindow(GetType()) && !WindowHelper::IsSystemSubWindow(GetType())) {
        WLOGFE("window type is not supported");
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    std::lock_guard<std::mutex> lockListener(subWindowCloseListenersMutex_);
    subWindowCloseListeners_[GetPersistentId()] = nullptr;
    return WMError::WM_OK;
}

void WindowSessionImpl::RecoverSessionListener()
{
    auto persistentId = GetPersistentId();
    TLOGI(WmsLogTag::WMS_RECOVER, "with persistentId=%{public}d", persistentId);
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
EnableIfSame<T, IOccupiedAreaChangeListener,
    std::vector<sptr<IOccupiedAreaChangeListener>>> WindowSessionImpl::GetListeners()
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
    TLOGI(WmsLogTag::WMS_LIFE, "Called id: %{public}d.", GetPersistentId());
    {
        std::lock_guard<std::mutex> lockListener(displayMoveListenerMutex_);
        ClearUselessListeners(displayMoveListeners_, persistentId);
    }
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
    {
        std::lock_guard<std::mutex> lockListener(windowRectChangeListenerMutex_);
        ClearUselessListeners(windowRectChangeListeners_, persistentId);
    }
    {
        std::lock_guard<std::mutex> lockListener(subWindowCloseListenersMutex_);
        subWindowCloseListeners_[GetPersistentId()] = nullptr;
    }
    TLOGI(WmsLogTag::WMS_LIFE, "Clear successfully, id: %{public}d.", GetPersistentId());
}

void WindowSessionImpl::RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func)
{
    notifyNativeFunc_ = std::move(func);
}

void WindowSessionImpl::ClearVsyncStation()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (vsyncStation_ != nullptr) {
        vsyncStation_.reset();
    }
}

void WindowSessionImpl::SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer)
{
    TLOGI(WmsLogTag::WMS_EVENT, "called");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    inputEventConsumer_ = inputEventConsumer;
}

WMError WindowSessionImpl::SetTitleButtonVisible(bool isMaximizeVisible, bool isMinimizeVisible, bool isSplitVisible)
{
    if (!WindowHelper::IsMainWindow(GetType())) {
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    if (GetUIContentSharedPtr() == nullptr || !IsDecorEnable()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto isPC = windowSystemConfig_.uiType_ == "pc";
    bool isPcAppInPad = property_->GetIsPcAppInPad();
    if (!(isPC || IsFreeMultiWindowMode() || isPcAppInPad)) {
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    windowTitleVisibleFlags_ = { isMaximizeVisible, isMinimizeVisible, isSplitVisible };
    UpdateTitleButtonVisibility();
    return WMError::WM_OK;
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
    if (vsyncStation_ == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "SetFrameRateLinkerEnable ture failed, vsyncStation is nullptr");
        return;
    }
    TLOGD(WmsLogTag::WMS_MAIN, "SetFrameRateLinkerEnable: true, linkerId = %{public}" PRIu64,
        vsyncStation_->GetFrameRateLinkerId());
    vsyncStation_->SetFrameRateLinkerEnable(true);
    if (WindowHelper::IsMainWindow(GetType())) {
        TLOGD(WmsLogTag::WMS_MAIN, "IsMainWindow: %{public}d, WindowType: %{public}d",
            WindowHelper::IsMainWindow(GetType()), GetType());
        vsyncStation_->SetDisplaySoloistFrameRateLinkerEnable(true);
    }
}

void WindowSessionImpl::NotifyAfterBackground(bool needNotifyListeners, bool needNotifyUiContent)
{
    bool isPcAppInPad = property_->GetIsPcAppInPad();
    if (needNotifyListeners && !isPcAppInPad) {
        std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
        auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterBackground, lifecycleListeners);
    }
    if (needNotifyUiContent) {
        CALL_UI_CONTENT(Background);
    }
    if (vsyncStation_ == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "SetFrameRateLinkerEnable false failed, vsyncStation is nullptr");
        return;
    }
    TLOGD(WmsLogTag::WMS_MAIN, "SetFrameRateLinkerEnable: false, linkerId = %{public}" PRIu64,
        vsyncStation_->GetFrameRateLinkerId());
    vsyncStation_->SetFrameRateLinkerEnable(false);
    if (WindowHelper::IsMainWindow(GetType())) {
        TLOGD(WmsLogTag::WMS_MAIN, "IsMainWindow: %{public}d, WindowType: %{public}d",
            WindowHelper::IsMainWindow(GetType()), GetType());
        vsyncStation_->SetDisplaySoloistFrameRateLinkerEnable(false);
    }
}

static void RequestInputMethodCloseKeyboard(bool isNeedKeyboard, bool keepKeyboardFlag)
{
    if (!isNeedKeyboard && !keepKeyboardFlag) {
#ifdef IMF_ENABLE
        if (MiscServices::InputMethodController::GetInstance()) {
            MiscServices::InputMethodController::GetInstance()->RequestHideInput();
        } else {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "Get instance failed, Notify Input framework close keyboard failed.");
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
            TLOGE(WmsLogTag::WMS_KEYBOARD, "window is null, Notify input framework close keyboard failed.");
            return;
        }
        bool isNeedKeyboard = false;
        {
            std::shared_ptr<Ace::UIContent> uiContent = window->GetUIContentSharedPtr();
            if (uiContent != nullptr) {
                // isNeedKeyboard is set by arkui and indicates whether the window needs a keyboard or not.
                isNeedKeyboard = uiContent->NeedSoftKeyboard();
            }
        }
        // whether keep the keyboard created by other windows, support system window and app subwindow.
        bool keepKeyboardFlag = (window->property_) ? window->property_->GetKeepKeyboardFlag() : false;
        TLOGI(WmsLogTag::WMS_KEYBOARD, "id: %{public}d, isNeedKeyboard: %{public}d, keepKeyboardFlag: %{public}d",
            window->GetPersistentId(), isNeedKeyboard, keepKeyboardFlag);
        RequestInputMethodCloseKeyboard(isNeedKeyboard, keepKeyboardFlag);
    };
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent != nullptr) {
        uiContent->SetOnWindowFocused(task);
    }
}

void WindowSessionImpl::NotifyAfterFocused()
{
    NotifyWindowAfterFocused();
    if (GetUIContentSharedPtr() != nullptr) {
        NotifyUIContentFocusStatus();
    } else {
        shouldReNotifyFocus_ = true;
    }
}

void WindowSessionImpl::NotifyAfterUnfocused(bool needNotifyUiContent)
{
    NotifyWindowAfterUnfocused();
    if (needNotifyUiContent) {
        if (GetUIContentSharedPtr() == nullptr) {
            shouldReNotifyFocus_ = true;
        } else {
            CALL_UI_CONTENT(UnFocus);
        }
    }
}

void WindowSessionImpl::NotifyWindowAfterFocused()
{
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    CALL_LIFECYCLE_LISTENER(AfterFocused, lifecycleListeners);
}

void WindowSessionImpl::NotifyWindowAfterUnfocused()
{
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    auto lifecycleListeners = GetListeners<IWindowLifeCycle>();
    // use needNotifyUinContent to separate ui content callbacks
    CALL_LIFECYCLE_LISTENER(AfterUnfocused, lifecycleListeners);
}

void WindowSessionImpl::NotifyBeforeDestroy(std::string windowName)
{
    auto task = [this]() {
        {
            std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
            if (uiContent != nullptr) {
                uiContent->Destroy();
            }
        }
        std::unique_lock<std::shared_mutex> lock(uiContentMutex_);
        if (uiContent_ != nullptr) {
            uiContent_ = nullptr;
            TLOGD(WmsLogTag::WMS_LIFE, "NotifyBeforeDestroy: uiContent destroy success, persistentId:%{public}d",
                GetPersistentId());
        }
    };
    if (handler_) {
        handler_->PostSyncTask(task, "wms:NotifyBeforeDestroy");
    } else {
        task();
    }
    TLOGI(WmsLogTag::WMS_LIFE, "Release uicontent successfully, id: %{public}d.", GetPersistentId());
    if (notifyNativeFunc_) {
        notifyNativeFunc_(windowName);
    }
    TLOGI(WmsLogTag::WMS_LIFE, "successed with id: %{public}d.", GetPersistentId());
}

void WindowSessionImpl::NotifyAfterDestroy()
{
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
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
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WSError::WS_DO_NOTHING);
    return hostSession->MarkProcessed(eventId);
}

void WindowSessionImpl::RegisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "window %{public}s id %{public}d register DialogDeathRecipientListener",
        GetWindowName().c_str(), GetPersistentId());
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return;
    }
    std::lock_guard<std::recursive_mutex> lockListener(dialogDeathRecipientListenerMutex_);
    RegisterListener(dialogDeathRecipientListeners_[GetPersistentId()], listener);
}

void WindowSessionImpl::UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "window %{public}s id %{public}d unregister DialogDeathRecipientListener",
        GetWindowName().c_str(), GetPersistentId());
    std::lock_guard<std::recursive_mutex> lockListener(dialogDeathRecipientListenerMutex_);
    UnregisterListener(dialogDeathRecipientListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "window %{public}s id %{public}d register DialogTargetTouchListener",
        GetWindowName().c_str(), GetPersistentId());
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::lock_guard<std::recursive_mutex> lockListener(dialogTargetTouchListenerMutex_);
    return RegisterListener(dialogTargetTouchListener_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "window %{public}s id %{public}d unregister DialogTargetTouchListener",
        GetWindowName().c_str(), GetPersistentId());
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

template<typename T>
EnableIfSame<T, IDisplayMoveListener, std::vector<sptr<IDisplayMoveListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IDisplayMoveListener>> displayMoveListeners;
    for (auto& listener : displayMoveListeners_[GetPersistentId()]) {
        displayMoveListeners.push_back(listener);
    }
    return displayMoveListeners;
}

void WindowSessionImpl::NotifyDisplayMove(DisplayId from, DisplayId to)
{
    WLOGFD("Notify display move from %{public}" PRIu64 " to %{public}" PRIu64, from, to);
    std::lock_guard<std::mutex> lockListener(displayMoveListenerMutex_);
    auto displayMoveListeners = GetListeners<IDisplayMoveListener>();
    for (auto& listener : displayMoveListeners) {
        if (listener != nullptr) {
            listener->OnDisplayMove(from, to);
        }
    }
}

WSError WindowSessionImpl::NotifyCloseExistPipWindow()
{
    TLOGI(WmsLogTag::WMS_PIP, "WindowSessionImpl::NotifyCloseExistPipWindow");
    auto task = []() {
        PictureInPictureManager::DoClose(true, true);
    };
    handler_->PostTask(task, "WMS_WindowSessionImpl_NotifyCloseExistPipWindow");
    return WSError::WS_OK;
}

void WindowSessionImpl::NotifyTouchDialogTarget(int32_t posX, int32_t posY)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "window %{public}s id %{public}d", GetWindowName().c_str(), GetPersistentId());
    auto hostSession = GetHostSession();
    if (hostSession != nullptr) {
        hostSession->ProcessPointDownSession(posX, posY);
    }
    std::lock_guard<std::recursive_mutex> lockListener(dialogTargetTouchListenerMutex_);
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
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
        auto windowChangeListeners = GetListeners<IWindowChangeListener>();
        for (auto& listener : windowChangeListeners) {
            if (listener != nullptr) {
                listener->OnSizeChange(rect, reason);
            }
        }
    }
    {
        std::lock_guard<std::mutex> lockRectListener(windowRectChangeListenerMutex_);
        auto windowRectChangeListeners = GetListeners<IWindowRectChangeListener>();
        for (auto& listener : windowRectChangeListeners) {
            if (listener != nullptr) {
                listener->OnRectChange(rect, reason);
            }
        }
    }
}

void WindowSessionImpl::NotifySubWindowClose(bool& terminateCloseProcess)
{
    WLOGFD("WindowSessionImpl::NotifySubWindowClose");
    std::lock_guard<std::mutex> lockListener(subWindowCloseListenersMutex_);
    auto subWindowCloseListeners = GetListeners<ISubWindowCloseListener>();
    if (subWindowCloseListeners != nullptr) {
        subWindowCloseListeners->OnSubWindowClose(terminateCloseProcess);
    }
}

WMError WindowSessionImpl::RegisterAvoidAreaChangeListener(sptr<IAvoidAreaChangedListener>& listener)
{
    bool isUpdate = false;
    WMError ret = WMError::WM_OK;
    auto persistentId = GetPersistentId();
    TLOGI(WmsLogTag::WMS_IMMS, "Start register avoidAreaChange listener, id:%{public}d", persistentId);
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "listener is nullptr");
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
    TLOGI(WmsLogTag::WMS_IMMS, "Start unregister avoidAreaChange listener, id:%{public}d", persistentId);
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
    TLOGI(WmsLogTag::WMS_IMMS,
          "id: %{public}d, type: %{public}d, "
          "top %{public}s, bottom %{public}s, left %{public}s, right %{public}s",
          GetPersistentId(), type,
          avoidArea->topRect_.ToString().c_str(), avoidArea->bottomRect_.ToString().c_str(),
          avoidArea->leftRect_.ToString().c_str(), avoidArea->rightRect_.ToString().c_str());
    std::lock_guard<std::recursive_mutex> lockListener(avoidAreaChangeListenerMutex_);
    auto avoidAreaChangeListeners = GetListeners<IAvoidAreaChangedListener>();
    for (auto& listener : avoidAreaChangeListeners) {
        if (listener != nullptr) {
            listener->OnAvoidAreaChanged(*avoidArea, type);
        }
    }
}

WSError WindowSessionImpl::NotifyTransferComponentData(const AAFwk::WantParams& wantParams)
{
    return WSError::WS_OK;
}

WSErrorCode WindowSessionImpl::NotifyTransferComponentDataSync(const AAFwk::WantParams& wantParams,
    AAFwk::WantParams& reWantParams)
{
    return WSErrorCode::WS_OK;
}

WSError WindowSessionImpl::UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)
{
    NotifyAvoidAreaChange(avoidArea, type);
    return WSError::WS_OK;
}

WSError WindowSessionImpl::SetPipActionEvent(const std::string& action, int32_t status)
{
    TLOGI(WmsLogTag::WMS_PIP, "action: %{public}s, status: %{public}d", action.c_str(), status);
    auto task = [action, status]() {
        PictureInPictureManager::DoActionEvent(action, status);
    };
    handler_->PostTask(task, "WMS_WindowSessionImpl_SetPipActionEvent");
    return WSError::WS_OK;
}

WSError WindowSessionImpl::SetPiPControlEvent(WsPiPControlType controlType, WsPiPControlStatus status)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlType:%{public}u, enabled:%{public}d", controlType, status);
    auto task = [controlType, status]() {
        PictureInPictureManager::DoControlEvent(static_cast<PiPControlType>(controlType),
            static_cast<PiPControlStatus>(status));
    };
    handler_->PostTask(task, "WMS_WindowSessionImpl_SetPiPControlEvent");
    return WSError::WS_OK;
}

WMError WindowSessionImpl::RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener)
{
    bool isUpdate = false;
    WMError ret = WMError::WM_OK;
    auto persistentId = GetPersistentId();
    TLOGI(WmsLogTag::WMS_EVENT, "Start register touchOutside listener, window: name=%{public}s, id=%{public}u",
        GetWindowName().c_str(), GetPersistentId());
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    {
        std::lock_guard<std::recursive_mutex> lockListener(touchOutsideListenerMutex_);
        ret = RegisterListener(touchOutsideListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            TLOGE(WmsLogTag::WMS_EVENT, "Register touchOutside listener fail, ret:%{public}u", ret);
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
    TLOGI(WmsLogTag::WMS_EVENT, "Start unregister touchOutside listener, window: name=%{public}s, id=%{public}u",
        GetWindowName().c_str(), GetPersistentId());
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    {
        std::lock_guard<std::recursive_mutex> lockListener(touchOutsideListenerMutex_);
        ret = UnregisterListener(touchOutsideListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            TLOGE(WmsLogTag::WMS_EVENT, "Unregister touchOutside listener fail, ret:%{public}u", ret);
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
    TLOGI(WmsLogTag::WMS_EVENT, "Notify touch outside, window: name=%{public}s, id=%{public}u",
        GetWindowName().c_str(), GetPersistentId());
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
    bool isLastUnregister = false;
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowVisibilityChangeListenerMutex_);
        ret = UnregisterListener(windowVisibilityChangeListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            return ret;
        }
        isLastUnregister = windowVisibilityChangeListeners_[persistentId].empty();
    }

    if (isLastUnregister) {
        ret = SingletonContainer::Get<WindowAdapter>().UpdateSessionWindowVisibilityListener(persistentId, false);
    }
    return ret;
}

WMError WindowSessionImpl::RegisterWindowNoInteractionListener(const IWindowNoInteractionListenerSptr& listener)
{
    WLOGFD("Start to register window no interaction listener.");
    std::lock_guard<std::recursive_mutex> lockListener(windowNoInteractionListenerMutex_);
    WMError ret = RegisterListener(windowNoInteractionListeners_[GetPersistentId()], listener);
    if (ret != WMError::WM_OK) {
        WLOGFE("register no interaction listener failed.");
    } else {
        SubmitNoInteractionMonitorTask(this->lastInteractionEventId_.load(), listener);
    }
    return ret;
}

WMError WindowSessionImpl::UnregisterWindowNoInteractionListener(const IWindowNoInteractionListenerSptr& listener)
{
    WLOGFD("Start to unregister window no interaction listener.");
    std::lock_guard<std::recursive_mutex> lockListener(windowNoInteractionListenerMutex_);
    WMError ret = UnregisterListener(windowNoInteractionListeners_[GetPersistentId()], listener);
    if (windowNoInteractionListeners_[GetPersistentId()].empty()) {
        lastInteractionEventId_.store(-1);
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

template<typename T>
EnableIfSame<T, IWindowNoInteractionListener, std::vector<IWindowNoInteractionListenerSptr>> WindowSessionImpl::GetListeners()
{
    std::vector<IWindowNoInteractionListenerSptr> noInteractionListeners;
    for (auto& listener : windowNoInteractionListeners_[GetPersistentId()]) {
        noInteractionListeners.push_back(listener);
    }
    return noInteractionListeners;
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

WSError WindowSessionImpl::NotifyNoInteractionTimeout(const IWindowNoInteractionListenerSptr& listener)
{
    if (listener == nullptr) {
        WLOGFE("invalid listener: nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    WLOGFD("Notify window no interaction timeout, window: name=%{public}s, id=%{public}u, timeout=%{public}" PRId64,
        GetWindowName().c_str(), GetPersistentId(), listener->GetTimeout());

    listener->OnWindowNoInteractionCallback();
    return WSError::WS_OK;
}

void WindowSessionImpl::NotifyPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (!pointerEvent) {
        TLOGE(WmsLogTag::WMS_INPUT_KEY_FLOW, "Pointer event is nullptr");
        return;
    }

    std::shared_ptr<IInputEventConsumer> inputEventConsumer;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        inputEventConsumer = inputEventConsumer_;
    }
    if (inputEventConsumer != nullptr) {
        WLOGFD("Transfer pointer event to inputEventConsumer");
        if (pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_MOVE) {
            TLOGI(WmsLogTag::WMS_INPUT_KEY_FLOW,
                "Transfer pointer event to inputEventConsumer InputTracking id:%{public}d",
                pointerEvent->GetId());
        }
        if (!(inputEventConsumer->OnInputEvent(pointerEvent))) {
            pointerEvent->MarkProcessed();
        }
        return;
    }

    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent != nullptr) {
        if (pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_MOVE) {
            TLOGI(WmsLogTag::WMS_EVENT, "Input id:%{public}d",
                pointerEvent->GetId());
        }
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "WindowSessionImpl:pointerEvent Send id:%d action:%d",
            pointerEvent->GetId(), pointerEvent->GetPointerAction());
        if (!(uiContent->ProcessPointerEvent(pointerEvent))) {
            TLOGI(WmsLogTag::WMS_EVENT, "UI content dose not consume this pointer event");
            pointerEvent->MarkProcessed();
        }
    } else {
        TLOGW(WmsLogTag::WMS_EVENT, "pointerEvent is not consumed, windowId: %{public}u", GetWindowId());
        pointerEvent->MarkProcessed();
    }
}

WMError WindowSessionImpl::SetKeyEventFilter(KeyEventFilterFunc filter)
{
    std::unique_lock<std::shared_mutex> lock(keyEventFilterMutex_);
    keyEventFilter_ = std::move(filter);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::ClearKeyEventFilter()
{
    std::unique_lock<std::shared_mutex> lock(keyEventFilterMutex_);
    keyEventFilter_ = nullptr;
    return WMError::WM_OK;
}

bool WindowSessionImpl::FilterKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    std::shared_lock<std::shared_mutex> lock(keyEventFilterMutex_);
    if (keyEventFilter_ != nullptr) {
        bool isFilter = keyEventFilter_(*keyEvent.get());
        TLOGE(WmsLogTag::WMS_SYSTEM, "keyCode:%{public}d isFilter:%{public}d",
            keyEvent->GetKeyCode(), isFilter);
        if (isFilter) {
            keyEvent->MarkProcessed();
            return true;
        }
    }
    return false;
}

void WindowSessionImpl::DispatchKeyEventCallback(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed)
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
            keyEvent->MarkProcessed();
            return;
        }
        HandleBackEvent();
        keyEvent->MarkProcessed();
        return;
    }

    if (inputEventConsumer != nullptr) {
        WLOGD("Transfer key event to inputEventConsumer");
        if (!(inputEventConsumer->OnInputEvent(keyEvent))) {
            keyEvent->MarkProcessed();
        }
        return;
    }

    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent) {
        if (FilterKeyEvent(keyEvent)) return;
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "WindowSessionImpl:keyEvent Send id:%d", keyEvent->GetId());
        isConsumed = uiContent->ProcessKeyEvent(keyEvent);
        if (!isConsumed && keyEvent->GetKeyCode() == MMI::KeyEvent::KEYCODE_ESCAPE &&
            property_->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
            property_->GetMaximizeMode() == MaximizeMode::MODE_FULL_FILL &&
            keyAction == MMI::KeyEvent::KEY_ACTION_DOWN && !escKeyEventTriggered_) {
            WLOGI("recover from fullscreen cause KEYCODE_ESCAPE");
            Recover();
        }
        if (!isConsumed) {
            keyEvent->MarkProcessed();
        }
        if (keyEvent->GetKeyCode() == MMI::KeyEvent::KEYCODE_ESCAPE) {
            escKeyEventTriggered_ = (keyAction == MMI::KeyEvent::KEY_ACTION_UP) ? false : true;
        }
    }
}

WSError WindowSessionImpl::HandleBackEvent()
{
    TLOGI(WmsLogTag::WMS_EVENT, "called");
    bool isConsumed = false;
    std::shared_ptr<IInputEventConsumer> inputEventConsumer;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        inputEventConsumer = inputEventConsumer_;
    }
    if (inputEventConsumer != nullptr) {
        WLOGFD("Transfer back event to inputEventConsumer");
        std::shared_ptr<MMI::KeyEvent> backKeyEvent = MMI::KeyEvent::Create();
        backKeyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_BACK);
        backKeyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
        isConsumed = inputEventConsumer->OnInputEvent(backKeyEvent);
    } else {
        std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
        if (uiContent != nullptr) {
            WLOGFD("Transfer back event to uiContent");
            isConsumed = uiContent->ProcessBackPressed();
        } else {
            WLOGFE("There is no back event consumer");
        }
    }

    if (isConsumed) {
        WLOGD("Back key event is consumed");
        return WSError::WS_OK;
    }
    WLOGFD("report Back");
    SingletonContainer::Get<WindowInfoReporter>().ReportBackButtonInfoImmediately();
    if (handler_ == nullptr) {
        WLOGFE("HandleBackEvent handler_ is nullptr!");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    // notify back event to host session
    wptr<WindowSessionImpl> weak = this;
    auto task = [weak]() {
        auto weakSession = weak.promote();
        if (weakSession == nullptr) {
            WLOGFE("HandleBackEvent session wptr is nullptr");
            return;
        }
        weakSession->PerformBack();
    };
    if (!handler_->PostTask(task, "wms:PerformBack")) {
        WLOGFE("Failed to post PerformBack");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    return WSError::WS_OK;
}

void WindowSessionImpl::NotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed,
    bool notifyInputMethod)
{
    if (keyEvent == nullptr) {
        WLOGFE("keyEvent is nullptr");
        return;
    }

#ifdef IMF_ENABLE
    bool isKeyboardEvent = IsKeyboardEvent(keyEvent);
    if (isKeyboardEvent && notifyInputMethod) {
        WLOGD("Async dispatch keyEvent to input method");
        auto callback = [weakThis = wptr(this)] (std::shared_ptr<MMI::KeyEvent>& keyEvent, bool consumed) {
            if (keyEvent == nullptr) {
                WLOGFW("keyEvent is null, consumed:%{public}" PRId32, consumed);
                return;
            }

            if (consumed) {
                WLOGD("Input method has processed key event, id:%{public}" PRId32, keyEvent->GetId());
                return;
            }

            auto promoteThis = weakThis.promote();
            if (promoteThis == nullptr) {
                WLOGFW("promoteThis is nullptr");
                keyEvent->MarkProcessed();
                return;
            }
            bool isConsumed = false;
            promoteThis->DispatchKeyEventCallback(keyEvent, isConsumed);
        };
        auto ret = MiscServices::InputMethodController::GetInstance()->DispatchKeyEvent(
            const_cast<std::shared_ptr<MMI::KeyEvent>&>(keyEvent), callback);
        if (ret != 0) {
            WLOGFE("DispatchKeyEvent failed, ret:%{public}" PRId32 ", id:%{public}" PRId32, ret, keyEvent->GetId());
            DispatchKeyEventCallback(keyEvent, isConsumed);
        }
        return;
    }
#endif // IMF_ENABLE
    DispatchKeyEventCallback(keyEvent, isConsumed);
}

bool WindowSessionImpl::IsKeyboardEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const
{
    int32_t keyCode = keyEvent->GetKeyCode();
    bool isKeyFN = (keyCode == MMI::KeyEvent::KEYCODE_FN);
    bool isKeyBack = (keyCode == MMI::KeyEvent::KEYCODE_BACK);
    bool isKeyboard = (keyCode >= MMI::KeyEvent::KEYCODE_0 && keyCode <= MMI::KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN);
    WLOGD("isKeyFN: %{public}d, isKeyboard: %{public}d", isKeyFN, isKeyboard);
    return (isKeyFN || isKeyboard || isKeyBack);
}

void WindowSessionImpl::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (state_ == WindowState::STATE_DESTROYED) {
        WLOGFE("Receive vsync request failed, window is destroyed");
        return;
    }

    if (vsyncStation_ == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "Receive vsync request failed, vsyncStation is nullptr");
        return;
    }
    vsyncStation_->RequestVsync(vsyncCallback);
}

int64_t WindowSessionImpl::GetVSyncPeriod()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (vsyncStation_ == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "Get vsync period failed, vsyncStation is nullptr");
        return 0;
    }
    return vsyncStation_->GetVSyncPeriod();
}

void WindowSessionImpl::FlushFrameRate(uint32_t rate, int32_t animatorExpectedFrameRate, uint32_t rateType)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (vsyncStation_ == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "FlushFrameRate failed, vsyncStation is nullptr");
        return;
    }
    vsyncStation_->FlushFrameRate(rate, animatorExpectedFrameRate, rateType);
}

WMError WindowSessionImpl::UpdateProperty(WSPropertyChangeAction action)
{
    TLOGD(WmsLogTag::DEFAULT, "action:%{public}u", action);
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::DEFAULT, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
    return hostSession->UpdateSessionPropertyByAction(property_, action);
}

sptr<Window> WindowSessionImpl::Find(const std::string& name)
{
    std::shared_lock<std::shared_mutex> lock(windowSessionMutex_);
    TLOGI(WmsLogTag::DEFAULT, "Try to find window %{public}s", name.c_str());
    auto iter = windowSessionMap_.find(name);
    if (iter == windowSessionMap_.end()) {
        TLOGE(WmsLogTag::DEFAULT, "Can not find window %{public}s", name.c_str());
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
        TLOGE(WmsLogTag::DEFAULT, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    uint32_t colorValue;
    if (ColorParser::Parse(color, colorValue)) {
        TLOGD(WmsLogTag::DEFAULT, "SetBackgroundColor: window: %{public}s, value: [%{public}s, %{public}u]",
            GetWindowName().c_str(), color.c_str(), colorValue);
        return SetBackgroundColor(colorValue);
    }
    TLOGE(WmsLogTag::DEFAULT, "invalid color string: %{public}s", color.c_str());
    return WMError::WM_ERROR_INVALID_PARAM;
}

WMError WindowSessionImpl::SetBackgroundColor(uint32_t color)
{
    TLOGI(WmsLogTag::DEFAULT, "window: %{public}s, value:%{public}u", GetWindowName().c_str(), color);

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
    {
        std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
        if (uiContent != nullptr) {
            uiContent->SetBackgroundColor(color);
            return WMError::WM_OK;
        }
    }
    if (aceAbilityHandler_ != nullptr) {
        aceAbilityHandler_->SetBackgroundColor(color);
        return WMError::WM_OK;
    }
    TLOGD(WmsLogTag::DEFAULT, "FA mode could not set bg color: %{public}u", GetWindowId());
    return WMError::WM_ERROR_INVALID_OPERATION;
}

sptr<Window> WindowSessionImpl::FindWindowById(uint32_t winId)
{
    std::shared_lock<std::shared_mutex> lock(windowSessionMutex_);
    if (windowSessionMap_.empty()) {
        WLOGFE("Please create mainWindow First!");
        return nullptr;
    }
    for (auto iter = windowSessionMap_.begin(); iter != windowSessionMap_.end(); iter++) {
        if (static_cast<int32_t>(winId) == iter->second.first) {
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
    {
        std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
        if (uiContent != nullptr) {
            return uiContent->GetBackgroundColor();
        }
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
    auto sessionGravity = static_cast<SessionGravity>(gravity);
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Set window gravity: %{public}u, percent: %{public}u", sessionGravity, percent);
    if (property_ != nullptr) {
        property_->SetKeyboardSessionGravity(sessionGravity, percent);
    }

    auto hostSession = GetHostSession();
    if (hostSession != nullptr) {
        return static_cast<WMError>(hostSession->SetKeyboardSessionGravity(
            static_cast<SessionGravity>(gravity), percent));
    }
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetSpecificBarProperty(WindowType type, const SystemBarProperty& property)
{
    return WMError::WM_OK;
}

void WindowSessionImpl::NotifyOccupiedAreaChangeInfoInner(sptr<OccupiedAreaChangeInfo> info)
{
    std::lock_guard<std::recursive_mutex> lockListener(occupiedAreaChangeListenerMutex_);
    auto occupiedAreaChangeListeners = GetListeners<IOccupiedAreaChangeListener>();
    for (auto& listener : occupiedAreaChangeListeners) {
        if (listener != nullptr) {
            if (((property_->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING &&
                  WindowHelper::IsMainWindow(GetType())) ||
                 (WindowHelper::IsSubWindow(GetType()) && FindWindowById(GetParentId()) != nullptr &&
                  FindWindowById(GetParentId())->GetMode() == WindowMode::WINDOW_MODE_FLOATING)) &&
                (windowSystemConfig_.uiType_ == "phone" ||
                 (windowSystemConfig_.uiType_ == "pad" && !IsFreeMultiWindowMode()))) {
                sptr<OccupiedAreaChangeInfo> occupiedAreaChangeInfo = new OccupiedAreaChangeInfo();
                listener->OnSizeChange(occupiedAreaChangeInfo);
                continue;
            }
            listener->OnSizeChange(info);
        }
    }
}

void WindowSessionImpl::NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info,
                                                     const std::shared_ptr<RSTransaction>& rsTransaction)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "hasRSTransaction: %{public}d, safeHeight: %{public}u"
        ", occupied rect: x %{public}u, y %{public}u, w %{public}u, h %{public}u", rsTransaction != nullptr,
        info->safeHeight_, info->rect_.posX_, info->rect_.posY_, info->rect_.width_, info->rect_.height_);
    if (handler_ == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "handler_ is nullptr, notify occupied area change info failed.");
    }
    auto task = [weak = wptr(this), info, rsTransaction]() {
        auto window = weak.promote();
        if (!window) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "window is nullptr, notify occupied area change info failed.");
            return;
        }
        if (rsTransaction) {
            RSTransaction::FlushImplicitTransaction();
            rsTransaction->Begin();
        }
        window->NotifyOccupiedAreaChangeInfoInner(info);
        if (rsTransaction) {
            rsTransaction->Commit();
        }
    };
    handler_->PostTask(task, "WindowSessionImpl_NotifyOccupiedAreaChangeInfo");
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

WSError WindowSessionImpl::SwitchFreeMultiWindow(bool enable)
{
    return WSError::WS_OK;
}

WSError WindowSessionImpl::NotifyDialogStateChange(bool isForeground)
{
    return WSError::WS_OK;
}

void WindowSessionImpl::UpdatePiPRect(const Rect& rect, WindowSizeChangeReason reason)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("HostSession is invalid");
        return;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_IF_NULL(hostSession);
    hostSession->UpdatePiPRect(rect, static_cast<SizeChangeReason>(reason));
}

void WindowSessionImpl::UpdatePiPControlStatus(PiPControlType controlType, PiPControlStatus status)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlType:%{public}u, status:%{public}d", controlType, status);
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_PIP, "HostSession is invalid");
        return;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_IF_NULL(hostSession);
    hostSession->UpdatePiPControlStatus(static_cast<WsPiPControlType>(controlType),
        static_cast<WsPiPControlStatus>(status));
}

void WindowSessionImpl::NotifyWindowStatusChange(WindowMode mode)
{
    WLOGFD("NotifyWindowStatusChange");
    auto WindowStatus = WindowStatus::WINDOW_STATUS_UNDEFINED;
    if (mode == WindowMode::WINDOW_MODE_FLOATING) {
        WindowStatus = WindowStatus::WINDOW_STATUS_FLOATING;
        if (property_->GetMaximizeMode() == MaximizeMode::MODE_AVOID_SYSTEM_BAR) {
            WindowStatus = WindowStatus::WINDOW_STATUS_MAXIMIZE;
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
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent != nullptr) {
        uiContent->UpdateTransform(transform);
    }
}

void WindowSessionImpl::SubmitNoInteractionMonitorTask(int32_t eventId,
    const IWindowNoInteractionListenerSptr& listener)
{
    auto task = [sessionWptr = wptr(this), eventId, listenerWptr = wptr(listener)]() {
        auto session = sessionWptr.promote();
        if (session == nullptr) {
            WLOGFE("windowInteractionMonitor task running failed, window session is null");
            return;
        }
        if (eventId != session->lastInteractionEventId_.load()) {
            WLOGFD("event id of windowInteractionMonitor has been changed, need not notify!");
            return;
        }
        if (session->state_ != WindowState::STATE_SHOWN) {
            WLOGFD("window state is not show, need not notify!");
            return;
        }
        session->NotifyNoInteractionTimeout(listenerWptr.promote());
    };
    handler_->PostTask(task, listener->GetTimeout());
}

void WindowSessionImpl::RefreshNoInteractionTimeoutMonitor()
{
    std::lock_guard<std::recursive_mutex> lockListener(windowNoInteractionListenerMutex_);
    if (windowNoInteractionListeners_[GetPersistentId()].empty()) {
        return;
    }
    this->lastInteractionEventId_.fetch_add(1);
    int32_t eventId = lastInteractionEventId_.load();
    auto noInteractionListeners = GetListeners<IWindowNoInteractionListener>();
    for (const auto& listenerItem : noInteractionListeners) {
        SubmitNoInteractionMonitorTask(eventId, listenerItem);
    }
}

bool WindowSessionImpl::IsUserOrientation(Orientation orientation) const
{
    if (orientation == Orientation::USER_ROTATION_PORTRAIT ||
        orientation == Orientation::USER_ROTATION_LANDSCAPE ||
        orientation == Orientation::USER_ROTATION_PORTRAIT_INVERTED ||
        orientation == Orientation::USER_ROTATION_LANDSCAPE_INVERTED) {
        return true;
    }
    return false;
}

WMError WindowSessionImpl::GetCallingWindowWindowStatus(WindowStatus& windowStatus) const
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "id: %{public}d", GetPersistentId());
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SingletonContainer::Get<WindowAdapter>().GetCallingWindowWindowStatus(GetPersistentId(), windowStatus);
}

WMError WindowSessionImpl::GetCallingWindowRect(Rect& rect) const
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Get CallingWindow Rect");
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SingletonContainer::Get<WindowAdapter>().GetCallingWindowRect(GetPersistentId(), rect);
}

void WindowSessionImpl::SetUiDvsyncSwitch(bool dvsyncSwitch)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (vsyncStation_ == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "vsyncStation is nullptr");
        return;
    }
    vsyncStation_->SetUiDvsyncSwitch(dvsyncSwitch);
}

WMError WindowSessionImpl::GetAppForceLandscapeConfig(AppForceLandscapeConfig& config)
{
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::DEFAULT, "HostSession is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_NULLPTR);
    return hostSession->GetAppForceLandscapeConfig(config);
}

void WindowSessionImpl::SetForceSplitEnable(bool isForceSplit, const std::string& homePage)
{
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "uiContent is null!");
        return;
    }
    TLOGI(WmsLogTag::DEFAULT, "isForceSplit: %{public}u, homePage: %{public}s",
        isForceSplit, homePage.c_str());
    uiContent->SetForceSplitEnable(isForceSplit, homePage);
}
} // namespace Rosen
} // namespace OHOS