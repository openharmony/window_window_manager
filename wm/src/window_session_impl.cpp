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

#include <common/rs_common_def.h>
#include <filesystem>
#include <fstream>
#include <ipc_skeleton.h>
#include <hisysevent.h>
#include <parameters.h>
#include <int_wrapper.h>
#ifdef IMF_ENABLE
#include <input_method_controller.h>
#endif // IMF_ENABLE
#include <transaction/rs_interfaces.h>
#include <transaction/rs_transaction.h>

#include "color_parser.h"
#include "common/include/fold_screen_state_internel.h"
#include "common/include/fold_screen_common.h"
#include "display_info.h"
#include "display_manager.h"
#include "extension/extension_business_info.h"
#include "hitrace_meter.h"
#include "scene_board_judgement.h"
#include "session_permission.h"
#include "key_event.h"
#include "session/container/include/window_event_channel.h"
#include "session_manager/include/session_manager.h"
#include "vsync_station.h"
#include "window_adapter.h"
#include "window_inspector.h"
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
constexpr int32_t FORCE_SPLIT_MODE = 5;
constexpr int32_t API_VERSION_18 = 18;
constexpr uint32_t API_VERSION_MOD = 1000;
constexpr uint32_t LIFECYCLE_ISOLATE_VERSION = 18;
constexpr uint32_t INVALID_TARGET_API_VERSION = 0;
/*
 * DFX
 */
const std::string SET_UIEXTENSION_DESTROY_TIMEOUT_LISTENER_TASK_NAME = "SetUIExtDestroyTimeoutListener";
constexpr int64_t SET_UIEXTENSION_DESTROY_TIMEOUT_TIME_MS = 4000;

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

Ace::ViewportConfig FillViewportConfig(
    Rect rect, float density, int32_t orientation, uint32_t transformHint, uint64_t displayId)
{
    Ace::ViewportConfig config;
    config.SetSize(rect.width_, rect.height_);
    config.SetPosition(rect.posX_, rect.posY_);
    config.SetDensity(density);
    config.SetOrientation(orientation);
    config.SetTransformHint(transformHint);
    config.SetDisplayId(displayId);
    return config;
}
}

std::map<int32_t, std::vector<sptr<IWindowLifeCycle>>> WindowSessionImpl::lifecycleListeners_;
std::map<int32_t, std::vector<sptr<IDisplayMoveListener>>> WindowSessionImpl::displayMoveListeners_;
std::map<int32_t, std::vector<sptr<IWindowChangeListener>>> WindowSessionImpl::windowChangeListeners_;
std::map<int32_t, std::vector<sptr<IWindowCrossAxisListener>>> WindowSessionImpl::windowCrossAxisListeners_;
std::map<int32_t, std::vector<sptr<IAvoidAreaChangedListener>>> WindowSessionImpl::avoidAreaChangeListeners_;
std::map<int32_t, std::vector<sptr<IDialogDeathRecipientListener>>> WindowSessionImpl::dialogDeathRecipientListeners_;
std::map<int32_t, std::vector<sptr<IDialogTargetTouchListener>>> WindowSessionImpl::dialogTargetTouchListener_;
std::map<int32_t, std::vector<sptr<IOccupiedAreaChangeListener>>> WindowSessionImpl::occupiedAreaChangeListeners_;
std::map<int32_t, std::vector<sptr<IKeyboardDidShowListener>>> WindowSessionImpl::keyboardDidShowListeners_;
std::map<int32_t, std::vector<sptr<IKeyboardDidHideListener>>> WindowSessionImpl::keyboardDidHideListeners_;
std::map<int32_t, std::vector<sptr<IScreenshotListener>>> WindowSessionImpl::screenshotListeners_;
std::map<int32_t, std::vector<sptr<ITouchOutsideListener>>> WindowSessionImpl::touchOutsideListeners_;
std::map<int32_t, std::vector<IWindowVisibilityListenerSptr>> WindowSessionImpl::windowVisibilityChangeListeners_;
std::mutex WindowSessionImpl::displayIdChangeListenerMutex_;
std::map<int32_t, std::vector<IDisplayIdChangeListenerSptr>> WindowSessionImpl::displayIdChangeListeners_;
std::mutex WindowSessionImpl::systemDensityChangeListenerMutex_;
std::unordered_map<int32_t, std::vector<ISystemDensityChangeListenerSptr>>
    WindowSessionImpl::systemDensityChangeListeners_;
std::map<int32_t, std::vector<IWindowNoInteractionListenerSptr>> WindowSessionImpl::windowNoInteractionListeners_;
std::map<int32_t, std::vector<sptr<IWindowTitleButtonRectChangedListener>>>
    WindowSessionImpl::windowTitleButtonRectChangeListeners_;
std::map<int32_t, std::vector<sptr<IWindowRectChangeListener>>> WindowSessionImpl::windowRectChangeListeners_;
std::map<int32_t, sptr<ISubWindowCloseListener>> WindowSessionImpl::subWindowCloseListeners_;
std::map<int32_t, sptr<IMainWindowCloseListener>> WindowSessionImpl::mainWindowCloseListeners_;
std::unordered_map<int32_t, std::vector<sptr<IWindowWillCloseListener>>> WindowSessionImpl::windowWillCloseListeners_;
std::map<int32_t, std::vector<sptr<ISwitchFreeMultiWindowListener>>> WindowSessionImpl::switchFreeMultiWindowListeners_;
std::map<int32_t, std::vector<sptr<IWindowHighlightChangeListener>>> WindowSessionImpl::highlightChangeListeners_;
std::recursive_mutex WindowSessionImpl::lifeCycleListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowCrossAxisListenerMutex_;
std::recursive_mutex WindowSessionImpl::avoidAreaChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::dialogDeathRecipientListenerMutex_;
std::recursive_mutex WindowSessionImpl::dialogTargetTouchListenerMutex_;
std::recursive_mutex WindowSessionImpl::occupiedAreaChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::keyboardDidShowListenerMutex_;
std::recursive_mutex WindowSessionImpl::keyboardDidHideListenerMutex_;
std::recursive_mutex WindowSessionImpl::screenshotListenerMutex_;
std::recursive_mutex WindowSessionImpl::touchOutsideListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowVisibilityChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowNoInteractionListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowStatusChangeListenerMutex_;
std::recursive_mutex WindowSessionImpl::windowTitleButtonRectChangeListenerMutex_;
std::mutex WindowSessionImpl::displayMoveListenerMutex_;
std::mutex WindowSessionImpl::windowRectChangeListenerMutex_;
std::mutex WindowSessionImpl::subWindowCloseListenersMutex_;
std::mutex WindowSessionImpl::mainWindowCloseListenersMutex_;
std::mutex WindowSessionImpl::windowWillCloseListenersMutex_;
std::mutex WindowSessionImpl::switchFreeMultiWindowListenerMutex_;
std::mutex WindowSessionImpl::highlightChangeListenerMutex_;
std::map<std::string, std::pair<int32_t, sptr<WindowSessionImpl>>> WindowSessionImpl::windowSessionMap_;
std::shared_mutex WindowSessionImpl::windowSessionMutex_;
std::set<sptr<WindowSessionImpl>> WindowSessionImpl::windowExtensionSessionSet_;
std::shared_mutex WindowSessionImpl::windowExtensionSessionMutex_;
std::recursive_mutex WindowSessionImpl::subWindowSessionMutex_;
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

#define CHECK_UI_CONTENT_RETURN_IF_NULL(uiContent)                             \
    do {                                                                       \
        if ((uiContent) == nullptr) {                                          \
            TLOGE(WmsLogTag::WMS_LIFE, "uiContent is null");                   \
            return;                                                            \
        }                                                                      \
    } while (false)

WindowSessionImpl::WindowSessionImpl(const sptr<WindowOption>& option)
{
    WLOGFD("[WMSCom] Constructor");
    property_ = sptr<WindowSessionProperty>::MakeSptr();
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
    property_->SetViewKeepScreenOn(option->IsViewKeepScreenOn());
    property_->SetWindowMode(option->GetWindowMode());
    property_->SetWindowFlags(option->GetWindowFlags());
    property_->SetCallingSessionId(option->GetCallingWindow());
    property_->SetIsUIExtFirstSubWindow(option->GetIsUIExtFirstSubWindow());
    property_->SetTopmost(option->GetWindowTopmost());
    property_->SetSubWindowZLevel(option->GetSubWindowZLevel());
    property_->SetRealParentId(option->GetRealParentId());
    property_->SetParentWindowType(option->GetParentWindowType());
    property_->SetUIExtensionUsage(static_cast<UIExtensionUsage>(option->GetUIExtensionUsage()));
    property_->SetIsUIExtAnySubWindow(option->GetIsUIExtAnySubWindow());
    property_->SetIsSystemKeyboard(option->IsSystemKeyboard());
    property_->SetConstrainedModal(option->IsConstrainedModal());
    layoutCallback_ = sptr<FutureCallback>::MakeSptr();
    isMainHandlerAvailable_ = option->GetMainHandlerAvailable();
    isIgnoreSafeArea_ = WindowHelper::IsSubWindow(optionWindowType);
    windowOption_ = option;
    handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
    surfaceNode_ = CreateSurfaceNode(property_->GetWindowName(), optionWindowType);
    if (surfaceNode_ != nullptr) {
        vsyncStation_ = std::make_shared<VsyncStation>(surfaceNode_->GetId());
        if (WindowHelper::IsSubWindow(GetType())) {
            surfaceNode_->SetFrameGravity(Gravity::TOP_LEFT);
        }
    }
}

bool WindowSessionImpl::IsPcWindow() const
{
    return windowSystemConfig_.IsPcWindow();
}

bool WindowSessionImpl::IsPcOrPadCapabilityEnabled() const
{
    return WindowSessionImpl::IsPcOrPadFreeMultiWindowMode() || property_->GetIsPcAppInPad();
}

bool WindowSessionImpl::IsPcOrPadFreeMultiWindowMode() const
{
    return windowSystemConfig_.IsPcWindow() || IsFreeMultiWindowMode();
}

bool WindowSessionImpl::IsSceneBoardEnabled() const
{
    return SceneBoardJudgement::IsSceneBoardEnabled();
}

bool WindowSessionImpl::GetCompatibleModeInPc() const
{
    return property_->GetCompatibleModeInPc();
}

void WindowSessionImpl::MakeSubOrDialogWindowDragableAndMoveble()
{
    TLOGI(WmsLogTag::WMS_PC, "Called %{public}d.", GetPersistentId());
    if (IsPcOrPadCapabilityEnabled() && windowOption_ != nullptr) {
        if (WindowHelper::IsSubWindow(property_->GetWindowType())) {
            TLOGI(WmsLogTag::WMS_PC, "create subwindow, title: %{public}s, decorEnable: %{public}d",
                windowOption_->GetSubWindowTitle().c_str(), windowOption_->GetSubWindowDecorEnable());
            property_->SetDecorEnable(windowOption_->GetSubWindowDecorEnable());
            property_->SetDragEnabled(windowOption_->GetSubWindowDecorEnable());
            UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED);
            subWindowTitle_ = windowOption_->GetSubWindowTitle();
        }
        bool isDialog = WindowHelper::IsDialogWindow(property_->GetWindowType());
        if (isDialog) {
            bool dialogDecorEnable = windowOption_->GetDialogDecorEnable();
            property_->SetDecorEnable(dialogDecorEnable);
            property_->SetDragEnabled(dialogDecorEnable);
            UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED);
            dialogTitle_ = windowOption_->GetDialogTitle();
            TLOGI(WmsLogTag::WMS_PC, "create dialogWindow, title: %{public}s, decorEnable: %{public}d",
                dialogTitle_.c_str(), dialogDecorEnable);
        }
    }
}

void WindowSessionImpl::SetSubWindowZLevelToProperty()
{
    if (WindowHelper::IsNormalSubWindow(property_->GetWindowType(), property_->GetWindowFlags())) {
        property_->SetSubWindowZLevel(windowOption_->GetSubWindowZLevel());
    }
}

RSSurfaceNode::SharedPtr WindowSessionImpl::CreateSurfaceNode(const std::string& name, WindowType type)
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
            TLOGI(WmsLogTag::WMS_UIEXT, "uiExtensionUsage=%{public}u", property_->GetUIExtensionUsage());
            if (property_->GetUIExtensionUsage() == UIExtensionUsage::CONSTRAINED_EMBEDDED) {
                rsSurfaceNodeType = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE;
            } else {
                rsSurfaceNodeType = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
            }
            break;
        case WindowType::WINDOW_TYPE_PIP:
            rsSurfaceNodeType = RSSurfaceNodeType::APP_WINDOW_NODE;
            break;
        default:
            rsSurfaceNodeType = RSSurfaceNodeType::DEFAULT;
            break;
    }
    return RSSurfaceNode::Create(rsSurfaceNodeConfig, rsSurfaceNodeType, true, property_->IsConstrainedModal());
}

WindowSessionImpl::~WindowSessionImpl()
{
    WLOGFD("[WMSCom] id: %{public}d", GetPersistentId());
    Destroy(true, false);
}

uint32_t WindowSessionImpl::GetWindowId() const
{
    return static_cast<uint32_t>(GetPersistentId()) & 0xffffffff; // 0xffffffff: to get low 32 bits
}

uint64_t WindowSessionImpl::GetDisplayId() const
{
    return property_->GetDisplayId();
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
        TLOGW(WmsLogTag::WMS_LIFE, "already destroyed or not created! id: %{public}d state_: %{public}u",
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
    return property_->GetPersistentId();
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
    if (IsWindowSessionInvalid() || surfaceNode_ == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "session is invalid");
        return;
    }
    auto colorGamut = GetSurfaceGamutFromColorSpace(colorSpace);
    surfaceNode_->SetColorSpace(colorGamut);
}

ColorSpace WindowSessionImpl::GetColorSpace()
{
    if (IsWindowSessionInvalid() || surfaceNode_ == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "session is invalid");
        return ColorSpace::COLOR_SPACE_DEFAULT;
    }
    GraphicColorGamut colorGamut = surfaceNode_->GetColorSpace();
    return GetColorSpaceFromSurfaceGamut(colorGamut);
}

WMError WindowSessionImpl::WindowSessionCreateCheck()
{
    if (vsyncStation_ == nullptr || !vsyncStation_->IsVsyncReceiverCreated()) {
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
        TLOGI(WmsLogTag::DEFAULT, "Create camera float window, TokenId=%{private}u", accessTokenId);
    }
    return WMError::WM_OK;
}

void WindowSessionImpl::SetDefaultDisplayIdIfNeed()
{
    TLOGD(WmsLogTag::DEFAULT, "in");
    auto displayId = property_->GetDisplayId();
    if (displayId == DISPLAY_ID_INVALID) {
        auto defaultDisplayId = SingletonContainer::IsDestroyed() ? DISPLAY_ID_INVALID :
            SingletonContainer::Get<DisplayManager>().GetDefaultDisplayId();
        defaultDisplayId = (defaultDisplayId == DISPLAY_ID_INVALID) ? 0 : defaultDisplayId;
        property_->SetDisplayId(defaultDisplayId);
        TLOGI(WmsLogTag::DEFAULT, "Reset displayId to %{public}" PRIu64, defaultDisplayId);
    }
}

/** @note @window.hierarchy */
int32_t WindowSessionImpl::GetSubWindowZLevelByFlags(WindowType type, uint32_t windowFlags, bool isTopmost)
{
    if (WindowHelper::IsApplicationModalSubWindow(type, windowFlags)) {
        if (isTopmost) {
            return APPLICATION_MODALITY_SUB_WINDOW_Z_LEVEL + TOPMOST_SUB_WINDOW_Z_LEVEL;
        }
        return APPLICATION_MODALITY_SUB_WINDOW_Z_LEVEL;
    } else if (WindowHelper::IsModalSubWindow(type, windowFlags)) {
        if (isTopmost) {
            return MODALITY_SUB_WINDOW_Z_LEVEL + TOPMOST_SUB_WINDOW_Z_LEVEL;
        }
        return MODALITY_SUB_WINDOW_Z_LEVEL;
    } else if (WindowHelper::IsToastSubWindow(type, windowFlags)) {
        return TOAST_SUB_WINDOW_Z_LEVEL;
    } else if (WindowHelper::IsTextMenuSubWindow(type, windowFlags)) {
        return TEXT_MENU_SUB_WINDOW_Z_LEVEL;
    } else if (WindowHelper::IsDialogWindow(type)) {
        return DIALOG_SUB_WINDOW_Z_LEVEL;
    }
    return NORMAL_SUB_WINDOW_Z_LEVEL;
}

WMError WindowSessionImpl::Connect()
{
    TLOGI(WmsLogTag::WMS_LIFE, "in");
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_NULLPTR);
    sptr<ISessionStage> iSessionStage(this);
    auto windowEventChannel = sptr<WindowEventChannel>::MakeSptr(iSessionStage);
    windowEventChannel->SetIsUIExtension(property_->GetWindowType() == WindowType::WINDOW_TYPE_UI_EXTENSION);
    windowEventChannel->SetUIExtensionUsage(property_->GetUIExtensionUsage());
    sptr<IWindowEventChannel> iWindowEventChannel(windowEventChannel);
    sptr<IRemoteObject> token = context_ ? context_->GetToken() : nullptr;
    if (token) {
        property_->SetTokenState(true);
    }
    property_->SetApiVersion(GetTargetAPIVersion());
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
    TLOGI(WmsLogTag::WMS_EVENT, "id: %{public}d", keyEvent->GetId());
    if (auto uiContent = GetUIContentSharedPtr()) {
        return uiContent->ProcessKeyEvent(keyEvent, true);
    }
    return false;
}

bool WindowSessionImpl::NotifyOnKeyPreImeEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    return PreNotifyKeyEvent(keyEvent);
}

void WindowSessionImpl::GetSubWidnows(int32_t parentPersistentId, std::vector<sptr<WindowSessionImpl>>& subWindows)
{
    std::lock_guard<std::recursive_mutex> lock(subWindowSessionMutex_);
    auto iter = subWindowSessionMap_.find(parentPersistentId);
    if (iter == subWindowSessionMap_.end()) {
        TLOGD(WmsLogTag::WMS_SUB, "parent window: %{public}d has no child node", parentPersistentId);
        return;
    }
    subWindows = iter->second;
}

void WindowSessionImpl::UpdateSubWindowStateAndNotify(int32_t parentPersistentId, const WindowState newState)
{
    std::vector<sptr<WindowSessionImpl>> subWindows;
    GetSubWidnows(parentPersistentId, subWindows);
    if (subWindows.empty()) {
        TLOGD(WmsLogTag::WMS_SUB, "parent window: %{public}d, its subWindowMap is empty", parentPersistentId);
        return;
    }

    // when parent window hide and subwindow whose state is shown should hide and notify user
    if (newState == WindowState::STATE_HIDDEN) {
        for (auto subwindow : subWindows) {
            if (subwindow != nullptr && subwindow->GetWindowState() == WindowState::STATE_SHOWN) {
                subwindow->state_ = WindowState::STATE_HIDDEN;
                subwindow->NotifyAfterBackground();
                TLOGD(WmsLogTag::WMS_SUB, "Notify subWindow background, id:%{public}d", subwindow->GetPersistentId());
                UpdateSubWindowStateAndNotify(subwindow->GetPersistentId(), newState);
            }
        }
    // when parent window show and subwindow whose state is shown should show and notify user
    } else if (newState == WindowState::STATE_SHOWN) {
        for (auto subwindow : subWindows) {
            if (subwindow != nullptr && subwindow->GetWindowState() == WindowState::STATE_HIDDEN &&
                subwindow->GetRequestWindowState() == WindowState::STATE_SHOWN) {
                subwindow->state_ = WindowState::STATE_SHOWN;
                subwindow->NotifyAfterForeground();
                TLOGD(WmsLogTag::WMS_SUB, "Notify subWindow foreground, id:%{public}d", subwindow->GetPersistentId());
                UpdateSubWindowStateAndNotify(subwindow->GetPersistentId(), newState);
            }
        }
    }
}

WMError WindowSessionImpl::Show(uint32_t reason, bool withAnimation, bool withFocus)
{
    TLOGI(WmsLogTag::WMS_LIFE, "name:%{public}s, id:%{public}d, type:%{public}u, reason:%{public}u, state:%{public}u",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), GetType(), reason, state_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (state_ == WindowState::STATE_SHOWN) {
        TLOGD(WmsLogTag::WMS_LIFE, "alreay shown [name:%{public}s, id:%{public}d, type: %{public}u]",
            property_->GetWindowName().c_str(), GetPersistentId(), property_->GetWindowType());
        NotifyAfterForeground(true, false);
        return WMError::WM_OK;
    }

    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
    WSError ret = hostSession->Foreground(property_);
    // delete after replace WSError with WMError
    WMError res = static_cast<WMError>(ret);
    if (res == WMError::WM_OK) {
        UpdateSubWindowStateAndNotify(GetPersistentId(), WindowState::STATE_SHOWN);
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
        TLOGD(WmsLogTag::WMS_LIFE, "alreay hidden [name:%{public}s, id:%{public}d, type: %{public}u]",
            property_->GetWindowName().c_str(), GetPersistentId(), property_->GetWindowType());
        NotifyBackgroundFailed(WMError::WM_DO_NOTHING);
        return WMError::WM_OK;
    }
    UpdateSubWindowStateAndNotify(GetPersistentId(), WindowState::STATE_HIDDEN);
    state_ = WindowState::STATE_HIDDEN;
    requestState_ = WindowState::STATE_HIDDEN;
    NotifyAfterBackground();
    return WMError::WM_OK;
}

void WindowSessionImpl::RemoveSubWindow(int32_t parentPersistentId)
{
    const int32_t persistentId = GetPersistentId();
    TLOGI(WmsLogTag::WMS_SUB, "Id: %{public}d, parentId: %{public}d", persistentId, parentPersistentId);
    std::vector<sptr<WindowSessionImpl>> subWindows;
    GetSubWidnows(parentPersistentId, subWindows);
    for (auto iter = subWindows.begin(); iter != subWindows.end(); iter++) {
        auto subWindow = *iter;
        if (subWindow != nullptr && subWindow->GetPersistentId() == persistentId) {
            TLOGD(WmsLogTag::WMS_SUB, "Destroy sub window, persistentId: %{public}d", persistentId);
            subWindows.erase(iter);
            break;
        } else {
            TLOGD(WmsLogTag::WMS_SUB, "Exists other sub window, persistentId: %{public}d", persistentId);
        }
    }
}

void WindowSessionImpl::DestroySubWindow()
{
    int32_t parentPersistentId = property_->GetParentPersistentId();
    const int32_t persistentId = GetPersistentId();
    if (property_->GetIsUIExtFirstSubWindow()) {
        auto extensionWindow = FindExtensionWindowWithContext();
        if (extensionWindow != nullptr) {
            parentPersistentId = extensionWindow->GetPersistentId();
        }
    }
    // remove from subWindowMap_ when destroy sub window
    RemoveSubWindow(parentPersistentId);
    {
        std::lock_guard<std::recursive_mutex> lock(subWindowSessionMutex_);
        auto subIter = subWindowSessionMap_.find(parentPersistentId);
        if (subIter != subWindowSessionMap_.end() && property_->GetIsUIExtFirstSubWindow() &&
            subWindowSessionMap_.empty()) {
            auto extensionWindow = FindExtensionWindowWithContext();
            if (extensionWindow != nullptr && extensionWindow->GetUIContentSharedPtr() == nullptr) {
                extensionWindow->AddSetUIExtensionDestroyTimeoutCheck();
            }
        }
    }
    // remove from subWindowMap_ when destroy parent window
    std::vector<sptr<WindowSessionImpl>> subWindows;
    GetSubWidnows(persistentId, subWindows);
    for (auto iter = subWindows.begin(); iter != subWindows.end(); iter = subWindows.begin()) {
        auto subWindow = *iter;
        if (subWindow == nullptr) {
            TLOGW(WmsLogTag::WMS_SUB, "Destroy sub window which is nullptr");
            subWindows.erase(iter);
            continue;
        }
        bool isExtDestroyed = subWindow->property_->GetIsUIExtFirstSubWindow();
        TLOGD(WmsLogTag::WMS_SUB, "Destroy sub window, persistentId: %{public}d, isExtDestroyed: %{public}d",
            subWindow->GetPersistentId(), isExtDestroyed);
        auto ret = subWindow->Destroy(isExtDestroyed);
        if (ret != WMError::WM_OK) {
            TLOGE(WmsLogTag::WMS_SUB, "Destroy failed. persistentId: %{public}d", subWindow->GetPersistentId());
            subWindows.erase(iter);
        }
    }
    {
        std::lock_guard<std::recursive_mutex> lock(subWindowSessionMutex_);
        auto mainIter = subWindowSessionMap_.find(persistentId);
        if (mainIter != subWindowSessionMap_.end()) {
            mainIter->second.clear();
            subWindowSessionMap_.erase(mainIter);
        }
    }
}

WMError WindowSessionImpl::Destroy(bool needNotifyServer, bool needClearListener)
{
    TLOGI(WmsLogTag::WMS_LIFE, "id:%{public}d Destroy, state:%{public}u, needNotifyServer:%{public}d, "
        "needClearListener:%{public}d", GetPersistentId(), state_, needNotifyServer, needClearListener);
    if (IsWindowSessionInvalid()) {
        WLOGFW("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (auto hostSession = GetHostSession()) {
        hostSession->Disconnect();
    }
    NotifyBeforeDestroy(GetWindowName());
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        state_ = WindowState::STATE_DESTROYED;
        requestState_ = WindowState::STATE_DESTROYED;
    }
    DestroySubWindow();
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

/** @note @window.layout */
WSError WindowSessionImpl::UpdateRect(const WSRect& rect, SizeChangeReason reason,
    const SceneAnimationConfig& config, const std::map<AvoidAreaType, AvoidArea>& avoidAreas)
{
    // delete after replace ws_common.h with wm_common.h
    auto wmReason = static_cast<WindowSizeChangeReason>(reason);
    Rect wmRect = { rect.posX_, rect.posY_, rect.width_, rect.height_ };
    auto preRect = GetRect();
    if (preRect.width_ != wmRect.width_ || preRect.height_ != wmRect.height_) {
        windowSizeChanged_ = true;
    }
    property_->SetWindowRect(wmRect);
    property_->SetRequestRect(wmRect);

    TLOGI(WmsLogTag::WMS_LAYOUT, "%{public}s, preRect:%{public}s, reason:%{public}u, hasRSTransaction:%{public}d"
        ", duration:%{public}d, [name:%{public}s, id:%{public}d], clientDisplayId: %{public}" PRIu64,
        rect.ToString().c_str(), preRect.ToString().c_str(), wmReason, config.rsTransaction_ != nullptr,
        config.animationDuration_, GetWindowName().c_str(), GetPersistentId(), property_->GetDisplayId());
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
        "WindowSessionImpl::UpdateRect id: %d [%d, %d, %u, %u] reason: %u hasRSTransaction: %u", GetPersistentId(),
        wmRect.posX_, wmRect.posY_, wmRect.width_, wmRect.height_, wmReason, config.rsTransaction_ != nullptr);
    if (handler_ != nullptr && wmReason == WindowSizeChangeReason::ROTATION) {
        postTaskDone_ = false;
        UpdateRectForRotation(wmRect, preRect, wmReason, config, avoidAreas);
    } else if (handler_ != nullptr && wmReason == WindowSizeChangeReason::RESIZE_WITH_ANIMATION) {
        RectAnimationConfig rectAnimationConfig = property_->GetRectAnimationConfig();
        TLOGI(WmsLogTag::WMS_LAYOUT, "rectAnimationConfig.duration: %{public}d", rectAnimationConfig.duration);
        postTaskDone_ = false;
        UpdateRectForResizeWithAnimation(wmRect, preRect, wmReason, rectAnimationConfig,
            config.rsTransaction_, avoidAreas);
    } else {
        UpdateRectForOtherReason(wmRect, preRect, wmReason, config.rsTransaction_, avoidAreas);
    }

    if (wmReason == WindowSizeChangeReason::MOVE || wmReason == WindowSizeChangeReason::RESIZE ||
        wmReason == WindowSizeChangeReason::MOVE_WITH_ANIMATION ||
        wmReason == WindowSizeChangeReason::RESIZE_WITH_ANIMATION) {
        layoutCallback_->OnUpdateSessionRect(wmRect, wmReason, GetPersistentId());
    }

    return WSError::WS_OK;
}

/** @note @window.layout */
void WindowSessionImpl::UpdateVirtualPixelRatio(const sptr<Display>& display)
{
    if (display == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "display is null when rotation!");
        return;
    }
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "displayInfo is null when rotation!");
        return;
    }
    virtualPixelRatio_ = GetVirtualPixelRatio(displayInfo);
    TLOGD(WmsLogTag::WMS_LAYOUT, "virtualPixelRatio: %{public}f", virtualPixelRatio_);
}

void WindowSessionImpl::UpdateRectForRotation(const Rect& wmRect, const Rect& preRect,
    WindowSizeChangeReason wmReason, const SceneAnimationConfig& config,
    const std::map<AvoidAreaType, AvoidArea>& avoidAreas)
{
    handler_->PostTask([weak = wptr(this), wmReason, wmRect, preRect, config, avoidAreas]() mutable {
        HITRACE_METER_NAME(HITRACE_TAG_WINDOW_MANAGER, "WindowSessionImpl::UpdateRectForRotation");
        auto window = weak.promote();
        if (!window) {
            return;
        }
        auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(window->property_->GetDisplayId());
        sptr<DisplayInfo> displayInfo = display ? display->GetDisplayInfo() : nullptr;
        window->UpdateVirtualPixelRatio(display);
        const std::shared_ptr<RSTransaction>& rsTransaction = config.rsTransaction_;
        if (rsTransaction) {
            RSTransaction::FlushImplicitTransaction();
            rsTransaction->Begin();
        }
        window->rotationAnimationCount_++;
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(config.animationDuration_);
        // animation curve: cubic [0.2, 0.0, 0.2, 1.0]
        auto curve = RSAnimationTimingCurve::CreateCubicCurve(0.2, 0.0, 0.2, 1.0);
        RSNode::OpenImplicitAnimation(protocol, curve, [weak]() {
            auto window = weak.promote();
            if (!window) {
                return;
            }
            window->rotationAnimationCount_--;
            if (window->rotationAnimationCount_ == 0) {
                window->NotifyRotationAnimationEnd();
            }
        });
        if ((wmRect != preRect) || (wmReason != window->lastSizeChangeReason_)) {
            window->NotifySizeChange(wmRect, wmReason);
            window->lastSizeChangeReason_ = wmReason;
        }
        window->UpdateViewportConfig(wmRect, wmReason, rsTransaction, displayInfo, avoidAreas);
        RSNode::CloseImplicitAnimation();
        if (rsTransaction) {
            rsTransaction->Commit();
        } else {
            RSTransaction::FlushImplicitTransaction();
        }
        window->postTaskDone_ = true;
    }, "WMS_WindowSessionImpl_UpdateRectForRotation");
}

void WindowSessionImpl::UpdateRectForOtherReasonTask(const Rect& wmRect, const Rect& preRect,
    WindowSizeChangeReason wmReason, const std::shared_ptr<RSTransaction>& rsTransaction,
    const std::map<AvoidAreaType, AvoidArea>& avoidAreas)
{
    if ((wmRect != preRect) || (wmReason != lastSizeChangeReason_) || !postTaskDone_) {
        NotifySizeChange(wmRect, wmReason);
        lastSizeChangeReason_ = wmReason;
        postTaskDone_ = true;
    }
    UpdateViewportConfig(wmRect, wmReason, rsTransaction, nullptr, avoidAreas);
    UpdateFrameLayoutCallbackIfNeeded(wmReason);
}

bool WindowSessionImpl::CheckIfNeedCommitRsTransaction(WindowSizeChangeReason wmReason)
{
    if (wmReason == WindowSizeChangeReason::FULL_TO_SPLIT ||
        wmReason == WindowSizeChangeReason::FULL_TO_FLOATING || wmReason == WindowSizeChangeReason::RECOVER ||
        wmReason == WindowSizeChangeReason::MAXIMIZE) {
        return false;
    }
    return true;
}

void WindowSessionImpl::UpdateRectForOtherReason(const Rect& wmRect, const Rect& preRect,
    WindowSizeChangeReason wmReason, const std::shared_ptr<RSTransaction>& rsTransaction,
    const std::map<AvoidAreaType, AvoidArea>& avoidAreas)
{
    if (handler_ == nullptr) {
        UpdateRectForOtherReasonTask(wmRect, preRect, wmReason, rsTransaction, avoidAreas);
        return;
    }

    auto task = [weak = wptr(this), wmReason, wmRect, preRect, rsTransaction, avoidAreas] {
        auto window = weak.promote();
        if (!window) {
            TLOGE(WmsLogTag::WMS_LAYOUT, "window is null, updateViewPortConfig failed");
            return;
        }
        bool ifNeedCommitRsTransaction = window->CheckIfNeedCommitRsTransaction(wmReason);
        if (rsTransaction && ifNeedCommitRsTransaction) {
            RSTransaction::FlushImplicitTransaction();
            rsTransaction->Begin();
        }
        if (wmReason == WindowSizeChangeReason::DRAG) {
            window->UpdateRectForOtherReasonTask(window->GetRect(), preRect, wmReason, rsTransaction, avoidAreas);
            window->isDragTaskPostDone_.store(true);
        } else {
            window->UpdateRectForOtherReasonTask(wmRect, preRect, wmReason, rsTransaction, avoidAreas);
        }
        if (rsTransaction && ifNeedCommitRsTransaction) {
            rsTransaction->Commit();
        }
    };
    if (wmReason == WindowSizeChangeReason::DRAG) {
        bool isDragTaskPostDone = true;
        if (isDragTaskPostDone_.compare_exchange_strong(isDragTaskPostDone, false)) {
            handler_->PostTask(task, "WMS_WindowSessionImpl_UpdateRectForOtherReason");
        }
    } else {
        handler_->PostTask(task, "WMS_WindowSessionImpl_UpdateRectForOtherReason");
    }
}

void WindowSessionImpl::UpdateRectForResizeWithAnimation(const Rect& wmRect, const Rect& preRect,
    WindowSizeChangeReason wmReason, const RectAnimationConfig& rectAnimationConfig,
    const std::shared_ptr<RSTransaction>& rsTransaction,
    const std::map<AvoidAreaType, AvoidArea>& avoidAreas)
{
    if (handler_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "handler is null!");
        return;
    }
    auto task = [weakThis = wptr(this), wmReason, wmRect, preRect, rectAnimationConfig,
        rsTransaction, avoidAreas]() mutable {
        HITRACE_METER_NAME(HITRACE_TAG_WINDOW_MANAGER, "WindowSessionImpl::UpdateRectForResizeWithAnimation");
        auto window = weakThis.promote();
        if (!window) {
            TLOGNE(WmsLogTag::WMS_LAYOUT, "window is null, updateRectForResizeWithAnimation failed");
            return;
        }
        if (rsTransaction) {
            RSTransaction::FlushImplicitTransaction();
            rsTransaction->Begin();
        }
        RSAnimationTimingProtocol protocol;
        protocol.SetDuration(rectAnimationConfig.duration);
        auto curve = RSAnimationTimingCurve::CreateCubicCurve(rectAnimationConfig.x1,
            rectAnimationConfig.y1, rectAnimationConfig.x2, rectAnimationConfig.y2);
        RSNode::OpenImplicitAnimation(protocol, curve, nullptr);
        if (wmRect != preRect || wmReason != window->lastSizeChangeReason_) {
            window->NotifySizeChange(wmRect, wmReason);
            window->lastSizeChangeReason_ = wmReason;
        }
        window->UpdateViewportConfig(wmRect, wmReason, rsTransaction, nullptr, avoidAreas);
        RSNode::CloseImplicitAnimation();
        if (rsTransaction) {
            rsTransaction->Commit();
        } else {
            RSTransaction::FlushImplicitTransaction();
        }
        window->postTaskDone_ = true;
    };
    handler_->PostTask(task, "WMS_WindowSessionImpl_UpdateRectForResizeWithAnimation");
}

void WindowSessionImpl::NotifyAfterUIContentReady()
{
    auto uiContent = GetUIContentSharedPtr();
    CHECK_UI_CONTENT_RETURN_IF_NULL(uiContent);
    if (IsNeedRenotifyTransform()) {
        auto transform = GetCurrentTransform();
        TLOGI(WmsLogTag::WMS_LAYOUT, "Renotify transform, id:%{public}d, scaleX:%{public}f, scaleY:%{public}f",
            GetPersistentId(), transform.scaleX_, transform.scaleY_);
        uiContent->UpdateTransform(transform);
        SetNeedRenotifyTransform(false);
    }
}

void WindowSessionImpl::NotifyRotationAnimationEnd()
{
    auto task = [weak = wptr(this)] {
        auto window = weak.promote();
        if (!window) {
            TLOGE(WmsLogTag::WMS_DIALOG, "window is null");
            return;
        }
        std::shared_ptr<Ace::UIContent> uiContent = window->GetUIContentSharedPtr();
        if (uiContent == nullptr) {
            WLOGFW("uiContent is null!");
            return;
        }
        uiContent->NotifyRotationAnimationEnd();
    };
    if (handler_ == nullptr) {
        TLOGW(WmsLogTag::WMS_DIALOG, "handler is null!");
        task();
    } else {
        handler_->PostTask(task, "WMS_WindowSessionImpl_NotifyRotationAnimationEnd");
    }
}

void WindowSessionImpl::FlushLayoutSize(int32_t width, int32_t height)
{
    if (!WindowHelper::IsMainWindow(GetType())) {
        return;
    }
    WSRect rect = { 0, 0, width, height };
    bool windowSizeChanged = true;
    bool enableFrameLayoutFinishCb = true;
    if (windowSizeChanged_.compare_exchange_strong(windowSizeChanged, false) ||
        enableFrameLayoutFinishCb_.compare_exchange_strong(enableFrameLayoutFinishCb, false) || layoutRect_ != rect) {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER,
            "NotifyFrameLayoutFinishFromApp, id: %u, rect: %s, notifyListener: %d",
            GetWindowId(), rect.ToString().c_str(), enableFrameLayoutFinishCb_.load());
        TLOGI(WmsLogTag::WMS_PATTERN,
            "NotifyFrameLayoutFinishFromApp, id: %{public}u, rect: %{public}s, notifyListener: %{public}d",
            GetWindowId(), rect.ToString().c_str(), enableFrameLayoutFinishCb_.load());
        if (auto session = GetHostSession()) {
            session->NotifyFrameLayoutFinishFromApp(enableFrameLayoutFinishCb_, rect);
        }
        layoutRect_ = rect;
        enableFrameLayoutFinishCb_ = false;
    }
}

void WindowSessionImpl::GetTitleButtonVisible(bool& hideMaximizeButton, bool& hideMinimizeButton,
    bool& hideSplitButton, bool& hideCloseButton)
{
    if (!IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_DECOR, "device not support");
        return;
    }
    if (hideMaximizeButton > !windowTitleVisibleFlags_.isMaximizeVisible) {
        TLOGW(WmsLogTag::WMS_DECOR, "isMaximizeVisible param INVALID");
    }
    hideMaximizeButton = hideMaximizeButton || (!windowTitleVisibleFlags_.isMaximizeVisible);
    if (hideMinimizeButton > !windowTitleVisibleFlags_.isMinimizeVisible) {
        TLOGW(WmsLogTag::WMS_DECOR, "isMinimizeVisible param INVALID");
    }
    hideMinimizeButton = hideMinimizeButton || (!windowTitleVisibleFlags_.isMinimizeVisible);
    if (hideSplitButton > !windowTitleVisibleFlags_.isSplitVisible) {
        TLOGW(WmsLogTag::WMS_DECOR, "isSplitVisible param INVALID");
    }
    hideSplitButton = hideSplitButton || (!windowTitleVisibleFlags_.isSplitVisible) || !isSplitButtonVisible_;
    if (hideCloseButton > !windowTitleVisibleFlags_.isCloseVisible) {
        TLOGW(WmsLogTag::WMS_DECOR, "isCloseVisible param INVALID");
    }
    hideCloseButton = hideCloseButton || (!windowTitleVisibleFlags_.isCloseVisible);
}

void WindowSessionImpl::UpdateDensity()
{
    auto preRect = GetRect();
    UpdateViewportConfig(preRect, WindowSizeChangeReason::UNDEFINED);
    WLOGFI("[%{public}d, %{public}d, %{public}u, %{public}u]",
        preRect.posX_, preRect.posY_, preRect.width_, preRect.height_);
}

void WindowSessionImpl::SetUniqueVirtualPixelRatio(bool useUniqueDensity, float virtualPixelRatio)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "old {useUniqueDensity: %{public}d, virtualPixelRatio: %{public}f}, "
        "new {useUniqueDensity: %{public}d, virtualPixelRatio: %{public}f}",
        useUniqueDensity_, virtualPixelRatio_, useUniqueDensity, virtualPixelRatio);
    bool oldUseUniqueDensity = useUniqueDensity_;
    useUniqueDensity_ = useUniqueDensity;
    if (useUniqueDensity_) {
        float oldVirtualPixelRatio = virtualPixelRatio_;
        virtualPixelRatio_ = virtualPixelRatio;
        if (!MathHelper::NearZero(oldVirtualPixelRatio - virtualPixelRatio)) {
            UpdateDensity();
            SetUniqueVirtualPixelRatioForSub(useUniqueDensity, virtualPixelRatio);
        }
    } else {
        if (oldUseUniqueDensity) {
            UpdateDensity();
            SetUniqueVirtualPixelRatioForSub(useUniqueDensity, virtualPixelRatio);
        }
    }
}

void WindowSessionImpl::CopyUniqueDensityParameter(sptr<WindowSessionImpl> parentWindow)
{
    if (parentWindow) {
        useUniqueDensity_ = parentWindow->useUniqueDensity_;
        virtualPixelRatio_ = parentWindow->virtualPixelRatio_;
    }
}

sptr<WindowSessionImpl> WindowSessionImpl::FindMainWindowWithContext()
{
    if (context_ == nullptr) {
        return nullptr;
    }
    std::shared_lock<std::shared_mutex> lock(windowSessionMutex_);
    for (const auto& winPair : windowSessionMap_) {
        auto win = winPair.second.second;
        if (win && win->GetType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
            context_.get() == win->GetContext().get()) {
            return win;
        }
    }
    TLOGD(WmsLogTag::WMS_MAIN, "Can not find main window, not app type");
    return nullptr;
}

sptr<WindowSessionImpl> WindowSessionImpl::FindExtensionWindowWithContext()
{
    if (context_ == nullptr) {
        return nullptr;
    }
    std::shared_lock<std::shared_mutex> lock(windowExtensionSessionMutex_);
    for (const auto& window : windowExtensionSessionSet_) {
        if (window && context_.get() == window->GetContext().get()) {
            return window;
        }
    }
    return nullptr;
}

void WindowSessionImpl::SetUniqueVirtualPixelRatioForSub(bool useUniqueDensity, float virtualPixelRatio)
{
    std::vector<sptr<WindowSessionImpl>> subWindows;
    GetSubWidnows(GetPersistentId(), subWindows);
    for (auto& subWindowSession : subWindows) {
        if (subWindowSession != nullptr) {
            subWindowSession->SetUniqueVirtualPixelRatio(useUniqueDensity, virtualPixelRatio);
        }
    }
}

WSError WindowSessionImpl::UpdateOrientation()
{
    TLOGD(WmsLogTag::DMS, "wid: %{public}d", GetPersistentId());
    return WSError::WS_OK;
}

WSError WindowSessionImpl::UpdateDisplayId(uint64_t displayId)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "wid: %{public}d, displayId: %{public}" PRIu64, GetPersistentId(), displayId);
    property_->SetDisplayId(displayId);
    return WSError::WS_OK;
}

WSError WindowSessionImpl::UpdateFocus(bool isFocused)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "focus: %{public}u, id: %{public}d", isFocused, GetPersistentId());
    isFocused_ = isFocused;
    if (isFocused) {
        HiSysEventWrite(
            OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
            "FOCUS_WINDOW",
            OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "PID", getpid(),
            "UID", getuid(),
            "BUNDLE_NAME", property_->GetSessionInfo().bundleName_,
            "WINDOW_TYPE", static_cast<uint32_t>(GetType()));
        NotifyAfterFocused();
    } else {
        NotifyAfterUnfocused();
    }
    return WSError::WS_OK;
}

bool WindowSessionImpl::IsFocused() const
{
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_FOCUS, "Session is invalid");
        return false;
    }

    TLOGD(WmsLogTag::WMS_FOCUS, "window id=%{public}d, isFocused=%{public}d", GetPersistentId(), isFocused_.load());
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

/** @note @window.focus */
WMError WindowSessionImpl::RequestFocusByClient(bool isFocused) const
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_FOCUS, "permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    if (IsWindowSessionInvalid()) {
        TLOGD(WmsLogTag::WMS_FOCUS, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
    auto ret = hostSession->RequestFocus(isFocused);
    return static_cast<WMError>(ret);
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
    WLOGFI("%{public}d", interactive);
    if (IsWindowSessionInvalid()) {
        return;
    }
    if (IsNotifyInteractiveDuplicative(interactive)) {
        return;
    }
    if (state_ != WindowState::STATE_SHOWN) {
        TLOGI(WmsLogTag::WMS_LIFE, "window state %{public}d, is not shown", state_);
        return;
    }
    if (interactive) {
        if (GetTargetAPIVersion() >= LIFECYCLE_ISOLATE_VERSION && !isDidForeground_) {
            TLOGI(WmsLogTag::WMS_LIFE, "api version %{public}u, isDidForeground %{public}d", GetTargetAPIVersion(),
                isDidForeground_);
            return;
        }
        NotifyAfterResumed();
    } else {
        NotifyAfterPaused();
    }
}

WSError WindowSessionImpl::UpdateWindowMode(WindowMode mode)
{
    return WSError::WS_OK;
}

/** @note @window.layout */
float WindowSessionImpl::GetVirtualPixelRatio()
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "virtualPixelRatio: %{public}f", virtualPixelRatio_);
    return virtualPixelRatio_;
}

float WindowSessionImpl::GetVirtualPixelRatio(const sptr<DisplayInfo>& displayInfo)
{
    if (useUniqueDensity_) {
        return virtualPixelRatio_;
    }
    return displayInfo->GetVirtualPixelRatio();
}

WMError WindowSessionImpl::GetVirtualPixelRatio(float& vpr)
{
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "get display failed displayId: %{public}" PRIu64, property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    auto displayInfo = display->GetDisplayInfo();
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "get display info failed displayId: %{public}" PRIu64, property_->GetDisplayId());
        return WMError::WM_ERROR_NULLPTR;
    }
    vpr = GetVirtualPixelRatio(displayInfo);
    if (MathHelper::NearZero(vpr)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "get decor height failed, because of wrong vpr: %{public}f", vpr);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return WMError::WM_OK;
}

/** @note @window.layout */
void WindowSessionImpl::UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason,
    const std::shared_ptr<RSTransaction>& rsTransaction, const sptr<DisplayInfo>& info,
    const std::map<AvoidAreaType, AvoidArea>& avoidAreas)
{
    // update avoid areas to listeners
    for (const auto& [type, avoidArea] : avoidAreas) {
        TLOGD(WmsLogTag::WMS_IMMS, "avoid type %{public}u area %{public}s",
            type, avoidArea.ToString().c_str());
        if (lastAvoidAreaMap_.find(type) == lastAvoidAreaMap_.end() ||
            lastAvoidAreaMap_[type] != avoidArea) {
            lastAvoidAreaMap_[type] = avoidArea;
            NotifyAvoidAreaChange(new AvoidArea(avoidArea), type);
        }
    }

    sptr<DisplayInfo> displayInfo;
    if (info == nullptr) {
        auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
        if (display == nullptr) {
            WLOGFE("display is null!");
            return;
        }
        displayInfo = display->GetDisplayInfo();
    } else {
        displayInfo = info;
    }
    if (displayInfo == nullptr) {
        WLOGFE("displayInfo is null!");
        return;
    }
    if (rect.width_ <= 0 || rect.height_ <= 0) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "invalid width: %{public}d, height: %{public}d, id: %{public}d",
              rect.width_, rect.height_, GetPersistentId());
        return;
    }
    auto rotation =  ONE_FOURTH_FULL_CIRCLE_DEGREE * static_cast<uint32_t>(displayInfo->GetOriginRotation());
    auto deviceRotation = static_cast<uint32_t>(displayInfo->GetDefaultDeviceRotationOffset());
    uint32_t transformHint = (rotation + deviceRotation) % FULL_CIRCLE_DEGREE;
    float density = GetVirtualPixelRatio(displayInfo);
    int32_t orientation = static_cast<int32_t>(displayInfo->GetDisplayOrientation());
    virtualPixelRatio_ = density;
    const auto& config = FillViewportConfig(rect, density, orientation, transformHint, GetDisplayId());
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        WLOGFW("uiContent is null!");
        return;
    }
    uiContent->UpdateViewportConfig(config, reason, rsTransaction, lastAvoidAreaMap_);

    if (WindowHelper::IsUIExtensionWindow(GetType())) {
        TLOGD(WmsLogTag::WMS_LAYOUT, "Id: %{public}d, reason: %{public}d, windowRect: %{public}s, "
            "displayOrientation: %{public}d, config[%{public}u, %{public}u, %{public}u, "
            "%{public}f]", GetPersistentId(), reason, rect.ToString().c_str(), orientation,
            rotation, deviceRotation, transformHint, virtualPixelRatio_);
    } else {
        TLOGI(WmsLogTag::WMS_LAYOUT, "Id: %{public}d, reason: %{public}d, windowRect: %{public}s, "
            "displayOrientation: %{public}d, config[%{public}u, %{public}u, %{public}u, "
            "%{public}f]", GetPersistentId(), reason, rect.ToString().c_str(), orientation,
            rotation, deviceRotation, transformHint, virtualPixelRatio_);
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
    WindowType windowType = GetType();
    bool isSubWindow = WindowHelper::IsSubWindow(windowType);
    bool isDialogWindow = WindowHelper::IsDialogWindow(windowType);
    if (IsPcOrPadCapabilityEnabled() && (isSubWindow || isDialogWindow)) {
        uiContent->HideWindowTitleButton(true, !windowOption_->GetSubWindowMaximizeSupported(), true, false);
        return;
    }
    auto windowModeSupportType = property_->GetWindowModeSupportType();
    bool hideSplitButton = !(windowModeSupportType & WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY);
    // not support fullscreen in split and floating mode, or not support float in fullscreen mode
    bool hideMaximizeButton = (!(windowModeSupportType & WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN) &&
        (GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING || WindowHelper::IsSplitWindowMode(GetWindowMode()))) ||
        (!(windowModeSupportType & WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING) &&
         GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN);
    bool hideMinimizeButton = false;
    bool hideCloseButton = false;
    GetTitleButtonVisible(hideMaximizeButton, hideMinimizeButton, hideSplitButton, hideCloseButton);
    TLOGI(WmsLogTag::WMS_DECOR, "[hideSplit, hideMaximize, hideMinimizeButton, hideCloseButton]:"
        "[%{public}d, %{public}d, %{public}d, %{public}d]",
        hideSplitButton, hideMaximizeButton, hideMinimizeButton, hideCloseButton);
    bool isSuperFoldDisplayDevice = FoldScreenStateInternel::IsSuperFoldDisplayDevice();
    if (property_->GetCompatibleModeInPc() && !property_->GetIsAtomicService()) {
        uiContent->HideWindowTitleButton(true, isSuperFoldDisplayDevice, hideMinimizeButton, hideCloseButton);
        uiContent->OnContainerModalEvent("scb_back_visibility", "true");
    } else {
        uiContent->HideWindowTitleButton(hideSplitButton, hideMaximizeButton, hideMinimizeButton, hideCloseButton);
        uiContent->OnContainerModalEvent("scb_back_visibility", "false");
    }
    if (isSuperFoldDisplayDevice) {
        handler_->PostTask([weakThis = wptr(this), where = __func__] {
            auto window = weakThis.promote();
            if (window == nullptr) {
                TLOGNE(WmsLogTag::WMS_LAYOUT_PC, "%{public}s window is null", where);
                return;
            }
            std::shared_ptr<Ace::UIContent> uiContent = window->GetUIContentSharedPtr();
            if (uiContent == nullptr || !window->IsDecorEnable()) {
                TLOGND(WmsLogTag::WMS_LAYOUT_PC, "%{public}s uiContent unavailable", where);
                return;
            }
            uiContent->OnContainerModalEvent(WINDOW_WATERFALL_VISIBILITY_EVENT,
                window->supportEnterWaterfallMode_ ? "true" : "false");
        }, "UIContentOnContainerModalEvent");
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
    if (auto uiContent = GetUIContentSharedPtr()) {
        uiContent->Destroy();
    }
}

WMError WindowSessionImpl::InitUIContent(const std::string& contentInfo, napi_env env, napi_value storage,
    WindowSetUIContentType setUIContentType, BackupAndRestoreType restoreType, AppExecFwk::Ability* ability,
    OHOS::Ace::UIContentErrorCode& aceRet)
{
    DestroyExistUIContent();
    std::unique_ptr<Ace::UIContent> uiContent = ability != nullptr ? Ace::UIContent::Create(ability) :
        Ace::UIContent::Create(context_.get(), reinterpret_cast<NativeEngine*>(env));
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "uiContent nullptr id: %{public}d", GetPersistentId());
        return WMError::WM_ERROR_NULLPTR;
    }
    switch (setUIContentType) {
        default:
        case WindowSetUIContentType::DEFAULT: {
            if (isUIExtensionAbilityProcess_ && property_->GetIsUIExtFirstSubWindow()) {
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
            aceRet = uiContent->Initialize(this, abcContent, storage, contentInfo);
            break;
    }
    // make uiContent available after Initialize/Restore
    {
        std::unique_lock<std::shared_mutex> lock(uiContentMutex_);
        uiContent_ = std::move(uiContent);
        // compatibleMode app in pc, need change decor title to float title bar
        if (property_->GetCompatibleModeInPc()) {
            uiContent_->SetContainerModalTitleVisible(false, true);
            uiContent_->EnableContainerModalCustomGesture(true);
        }
        WLOGFI("Initialized, isUIExtensionSubWindow:%{public}d, isUIExtensionAbilityProcess:%{public}d",
            uiContent_->IsUIExtensionSubWindow(), uiContent_->IsUIExtensionAbilityProcess());
    }
    RegisterWatchFocusActiveChangeCallback();
    return WMError::WM_OK;
}

void WindowSessionImpl::RegisterWatchFocusActiveChangeCallback()
{
    if (auto uiContent = GetUIContentSharedPtr()) {
        uiContent->AddFocusActiveChangeCallback([this](bool isFocusActive) {
            NotifyWatchFocusActiveChange(isFocusActive);
        });
    } else {
        TLOGE(WmsLogTag::WMS_EVENT, "uiContent is nullptr");
    }
}

WMError WindowSessionImpl::NotifyWatchFocusActiveChange(bool isActive)
{
    TLOGD(WmsLogTag::WMS_EVENT, "isActive:%{public}d", isActive);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SingletonContainer::Get<WindowAdapter>().NotifyWatchFocusActiveChange(isActive);
}

WMError WindowSessionImpl::SetUIContentInner(const std::string& contentInfo, napi_env env, napi_value storage,
    WindowSetUIContentType setUIContentType, BackupAndRestoreType restoreType, AppExecFwk::Ability* ability)
{
    TLOGI(WmsLogTag::WMS_LIFE, "%{public}s, state:%{public}u, persistentId: %{public}d",
        contentInfo.c_str(), state_, GetPersistentId());
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_LIFE, "window is invalid! window state: %{public}d",
            state_);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    NotifySetUIContentComplete();
    OHOS::Ace::UIContentErrorCode aceRet = OHOS::Ace::UIContentErrorCode::NO_ERRORS;
    WMError initUIContentRet = InitUIContent(contentInfo, env, storage, setUIContentType, restoreType, ability, aceRet);
    if (initUIContentRet != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_LIFE, "Init UIContent fail, ret:%{public}u", initUIContentRet);
        return initUIContentRet;
    }
    if (auto uiContent = GetUIContentSharedPtr()) {
        TLOGI(WmsLogTag::WMS_LAYOUT, "id:%{public}d, posX:%{public}d, posY:%{public}d, "
              "scaleX:%{public}f, scaleY:%{public}f", GetPersistentId(),
              singleHandTransform_.posX, singleHandTransform_.posY,
              singleHandTransform_.scaleX, singleHandTransform_.scaleY);
        uiContent->UpdateSingleHandTransform(singleHandTransform_);
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

    // UIContent may be nullptr on setting system bar properties, need to set menubar color on UIContent init.
    if (auto uiContent = GetUIContentSharedPtr()) {
        auto property = GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
        uiContent->SetStatusBarItemColor(property.contentColor_);
    }

    UpdateDecorEnable(true);
    if (state_ == WindowState::STATE_SHOWN) {
        // UIContent may be nullptr when show window, need to notify again when window is shown
        if (auto uiContent = GetUIContentSharedPtr()) {
            uiContent->Foreground();
        }
        UpdateTitleButtonVisibility();
    }
    UpdateViewportConfig(GetRect(), WindowSizeChangeReason::UNDEFINED);
    if (shouldReNotifyFocus_) {
        // uiContent may be nullptr when notify focus status, need to notify again when uiContent is not empty.
        NotifyUIContentFocusStatus();
        shouldReNotifyFocus_ = false;
    }
    if (shouldReNotifyHighlight_) {
        // uiContent may be nullptr when notify highlight status, need to notify again when uiContent is not empty.
        NotifyUIContentHighlightStatus(isHighlighted_);
        shouldReNotifyHighlight_ = false;
    }
    if (aceRet != OHOS::Ace::UIContentErrorCode::NO_ERRORS) {
        WLOGFE("failed to init or restore uicontent with file %{public}s. errorCode: %{public}d",
            contentInfo.c_str(), static_cast<uint16_t>(aceRet));
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    NotifyAfterUIContentReady();
    TLOGD(WmsLogTag::WMS_LIFE, "end");
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
    if (auto uiContent = GetUIContentSharedPtr()) {
        WindowMode mode = GetWindowMode();
        bool decorVisible = mode == WindowMode::WINDOW_MODE_FLOATING ||
            mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY ||
            (mode == WindowMode::WINDOW_MODE_FULLSCREEN && !property_->IsLayoutFullScreen());
        TLOGD(WmsLogTag::WMS_DECOR, "decorVisible:%{public}d", decorVisible);
        if (windowSystemConfig_.freeMultiWindowSupport_) {
            auto isSubWindow = WindowHelper::IsSubWindow(GetType());
            decorVisible = decorVisible && (windowSystemConfig_.freeMultiWindowEnable_ ||
                (property_->GetIsPcAppInPad() && isSubWindow));
        }
        uiContent->UpdateDecorVisible(decorVisible, isDecorEnable);
        return;
    }

    std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
    auto windowChangeListeners = GetListeners<IWindowChangeListener>();
    for (auto& listener : windowChangeListeners) {
        if (listener.GetRefPtr() != nullptr) {
            listener.GetRefPtr()->OnModeChange(GetWindowMode(), isDecorEnable);
        }
    }
}

void WindowSessionImpl::UpdateDecorEnable(bool needNotify, WindowMode mode)
{
    if (mode == WindowMode::WINDOW_MODE_UNDEFINED) {
        mode = GetWindowMode();
    }
    if (needNotify) {
        if (auto uiContent = GetUIContentSharedPtr()) {
            bool decorVisible = mode == WindowMode::WINDOW_MODE_FLOATING ||
                mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY ||
                (mode == WindowMode::WINDOW_MODE_FULLSCREEN && !property_->IsLayoutFullScreen());
            if (windowSystemConfig_.freeMultiWindowSupport_) {
                auto isSubWindow = WindowHelper::IsSubWindow(GetType());
                decorVisible = decorVisible && (windowSystemConfig_.freeMultiWindowEnable_ ||
                    (property_->GetIsPcAppInPad() && isSubWindow));
            }
            TLOGD(WmsLogTag::WMS_DECOR, "decorVisible:%{public}d", decorVisible);
            uiContent->UpdateDecorVisible(decorVisible, IsDecorEnable());
            uiContent->NotifyWindowMode(mode);
        }
        NotifyModeChange(mode, IsDecorEnable());
    }
}

/** @note @window.layout */
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

/** @note @window.focus */
WMError WindowSessionImpl::SetExclusivelyHighlighted(bool isExclusivelyHighlighted)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    TLOGI(WmsLogTag::WMS_FOCUS, "windowId: %{public}d, isExclusivelyHighlighted: %{public}d",
        property_->GetPersistentId(), isExclusivelyHighlighted);
    if (WindowHelper::IsMainWindow(GetType()) || WindowHelper::IsDialogWindow(GetType()) ||
        WindowHelper::IsModalWindow(property_->GetWindowFlags())) {
        TLOGE(WmsLogTag::WMS_FOCUS, "unsupport window, type: %{public}u, windowFlags: %{public}u ",
            GetType(), property_->GetWindowFlags());
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    if (property_->GetExclusivelyHighlighted() == isExclusivelyHighlighted) {
        TLOGD(WmsLogTag::WMS_FOCUS, "already exclusivelyHighlighted");
        return WMError::WM_OK;
    }
    property_->SetExclusivelyHighlighted(isExclusivelyHighlighted);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_EXCLUSIVE_HIGHLIGHTED);
}

/** @note @window.focus */
bool WindowSessionImpl::GetExclusivelyHighlighted() const
{
    bool isExclusivelyHighlighted = property_->GetExclusivelyHighlighted();
    TLOGD(WmsLogTag::WMS_FOCUS, "windowId: %{public}d, isExclusivelyHighlighted: %{public}d",
        property_->GetPersistentId(), isExclusivelyHighlighted);
    return isExclusivelyHighlighted;
}

/** @note @window.focus */
WSError WindowSessionImpl::NotifyHighlightChange(bool isHighlight)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "windowId: %{public}d, isHighlight: %{public}u,", GetPersistentId(), isHighlight);
    isHighlighted_ = isHighlight;
    if (GetUIContentSharedPtr() != nullptr) {
        NotifyUIContentHighlightStatus(isHighlighted_);
    } else {
        shouldReNotifyHighlight_ = true;
    }
    std::lock_guard<std::mutex> lockListener(highlightChangeListenerMutex_);
    auto highlightChangeListeners = GetListeners<IWindowHighlightChangeListener>();
    for (const auto& listener : highlightChangeListeners) {
        if (listener != nullptr) {
            listener->OnWindowHighlightChange(isHighlight);
        }
    }
    return WSError::WS_OK;
}

/** @note @window.focus */
WMError WindowSessionImpl::IsWindowHighlighted(bool& highlighted) const
{
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_FOCUS, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto hostSession = GetHostSession();
    hostSession->GetIsHighlighted(highlighted);
    TLOGD(WmsLogTag::WMS_FOCUS, "windowId: %{public}d, isWindowHighlighted: %{public}d",
        GetPersistentId(), highlighted);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetFocusable(bool isFocusable)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    TLOGI(WmsLogTag::WMS_FOCUS, "set focusable: windowId=%{public}d, isFocusable=%{public}d",
        property_->GetPersistentId(), isFocusable);
    property_->SetFocusable(isFocusable);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE);
}

bool WindowSessionImpl::GetFocusable() const
{
    bool isFocusable = property_->GetFocusable();
    TLOGD(WmsLogTag::WMS_FOCUS, "get focusable: windowId=%{public}d, isFocusable=%{public}d",
        property_->GetPersistentId(), isFocusable);
    return isFocusable;
}

WMError WindowSessionImpl::SetTouchable(bool isTouchable)
{
    WLOGFD("%{public}d", isTouchable);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetTouchable(isTouchable);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE);
}

/** @note @window.hierarchy */
WMError WindowSessionImpl::SetTopmost(bool topmost)
{
    TLOGD(WmsLogTag::WMS_HIERARCHY, "%{public}d", topmost);
    if (!windowSystemConfig_.IsPcWindow()) {
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetTopmost(topmost);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_TOPMOST);
}

/** @note @window.hierarchy */
bool WindowSessionImpl::IsTopmost() const
{
    return property_->IsTopmost();
}

/** @note @window.hierarchy */
WMError WindowSessionImpl::SetMainWindowTopmost(bool isTopmost)
{
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "device not support");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (!WindowHelper::IsMainWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "window type is not supported");
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    property_->SetMainWindowTopmost(isTopmost);
    uint32_t accessTokenId = static_cast<uint32_t>(IPCSkeleton::GetCallingTokenID());
    property_->SetAccessTokenId(accessTokenId);
    TLOGD(WmsLogTag::WMS_HIERARCHY, "tokenId=%{private}u, isTopmost=%{public}d", accessTokenId, isTopmost);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_MAIN_WINDOW_TOPMOST);
}

bool WindowSessionImpl::IsMainWindowTopmost() const
{
    return property_->IsMainWindowTopmost();
}

WMError WindowSessionImpl::SetWindowDelayRaiseEnabled(bool isEnabled)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_FOCUS, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (!WindowHelper::IsAppWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_FOCUS, "window type is not supported");
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    property_->SetWindowDelayRaiseEnabled(isEnabled);
    TLOGI(WmsLogTag::WMS_FOCUS, "isEnabled: %{public}d", isEnabled);
    return WMError::WM_OK;
}

bool WindowSessionImpl::IsWindowDelayRaiseEnabled() const
{
    return property_->IsWindowDelayRaiseEnabled();
}

WMError WindowSessionImpl::SetResizeByDragEnabled(bool dragEnabled)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "%{public}d", dragEnabled);
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(GetType()) ||
        (WindowHelper::IsSubWindow(GetType()) && windowOption_->GetSubWindowDecorEnable())) {
        property_->SetDragEnabled(dragEnabled);
    } else {
        TLOGE(WmsLogTag::WMS_LAYOUT, "This is not main window or decor enabled sub window.");
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED);
}

/** @note @window.hierarchy */
WMError WindowSessionImpl::SetRaiseByClickEnabled(bool raiseEnabled)
{
    WLOGFD("%{public}d", raiseEnabled);
    auto parentId = GetParentId();
    if (parentId == INVALID_SESSION_ID) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "Window id: %{public}d Parent id is invalid!",
              GetPersistentId());
        return WMError::WM_ERROR_INVALID_PARENT;
    }

    if (!WindowHelper::IsSubWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "Window id: %{public}d Must be app sub window!",
              GetPersistentId());
        return WMError::WM_ERROR_INVALID_CALLING;
    }

    if (state_ != WindowState::STATE_SHOWN) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "Window id: %{public}d The sub window must be shown!",
              GetPersistentId());
        return WMError::WM_DO_NOTHING;
    }
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    property_->SetRaiseEnabled(raiseEnabled);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED);
}

/** @note @window.immersive */
WMError WindowSessionImpl::SetAvoidAreaOption(uint32_t avoidAreaOption)
{
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_IMMS, "Session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetAvoidAreaOption(avoidAreaOption);
    TLOGI(WmsLogTag::WMS_IMMS, "win %{public}d, set option %{public}d",
        GetPersistentId(), avoidAreaOption);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_AVOID_AREA_OPTION);
}

/** @note @window.immersive */
WMError WindowSessionImpl::GetAvoidAreaOption(uint32_t& avoidAreaOption)
{
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_IMMS, "Session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    avoidAreaOption = property_->GetAvoidAreaOption();
    return WMError::WM_OK;
}

WMError WindowSessionImpl::HideNonSystemFloatingWindows(bool shouldHide)
{
    WLOGFD("%{public}d", shouldHide);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetHideNonSystemFloatingWindows(shouldHide);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS);
}

WMError WindowSessionImpl::SetLandscapeMultiWindow(bool isLandscapeMultiWindow)
{
    TLOGI(WmsLogTag::WMS_MULTI_WINDOW, "isLandscapeMultiWindow:%{public}d",
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
    WLOGFD("enable %{public}d", enable);
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
    if (IsWindowSessionInvalid()) {
        return false;
    }
    return property_->IsFloatingWindowAppType();
}

bool WindowSessionImpl::GetTouchable() const
{
    return property_->GetTouchable();
}

WMError WindowSessionImpl::SetWindowType(WindowType type)
{
    TLOGD(WmsLogTag::DEFAULT, "%{public}u type %{public}u", GetWindowId(), static_cast<uint32_t>(type));
    if (type != WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW && !SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DEFAULT, "permission denied!");
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
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "invalid brightness value: %{public}f", brightness);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (!(WindowHelper::IsAppWindow(GetType()) || GetType() == WindowType::WINDOW_TYPE_WALLET_SWIPE_CARD)) {
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "non app window does not support set brightness, type: %{public}u", GetType());
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    property_->SetBrightness(brightness);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS);
}

float WindowSessionImpl::GetBrightness() const
{
    return property_->GetBrightness();
}

void WindowSessionImpl::SetRequestedOrientation(Orientation orientation, bool needAnimation)
{
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::DEFAULT, "windowSession is invalid");
        return;
    }
    TLOGI(WmsLogTag::WMS_MAIN, "id:%{public}u lastReqOrientation:%{public}u target:%{public}u state:%{public}u",
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
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::DEFAULT, "windowSession is invalid");
        return Orientation::UNSPECIFIED;
    }
    return property_->GetRequestedOrientation();
}

std::string WindowSessionImpl::GetContentInfo(BackupAndRestoreType type)
{
    WLOGFD("in");
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
    TLOGD(WmsLogTag::WMS_LIFE, "in");
    restoredRouterStack_ = routerStack;
    return WMError::WM_OK;
}

std::string WindowSessionImpl::GetRestoredRouterStack()
{
    TLOGD(WmsLogTag::WMS_LIFE, "in");
    return std::move(restoredRouterStack_);
}

Ace::UIContent* WindowSessionImpl::GetUIContent() const
{
    std::shared_lock<std::shared_mutex> lock(uiContentMutex_);
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
    WLOGFI("[name:%{public}s, id:%{public}d]",
        property_->GetWindowName().c_str(), GetPersistentId());
    if (auto uiContent = GetUIContentSharedPtr()) {
        uiContent->OnNewWant(want);
    }
}

WMError WindowSessionImpl::SetAPPWindowLabel(const std::string& label)
{
    TLOGI(WmsLogTag::WMS_DECOR, "Enter");
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        TLOGI(WmsLogTag::WMS_DECOR, "uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent->SetAppWindowTitle(label);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetAPPWindowIcon(const std::shared_ptr<Media::PixelMap>& icon)
{
    if (icon == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "icon is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent->SetAppWindowIcon(icon);
    TLOGD(WmsLogTag::WMS_DECOR, "end");
    return WMError::WM_OK;
}

WMError WindowSessionImpl::RegisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    return RegisterListener(lifecycleListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::mutex> lockListener(displayMoveListenerMutex_);
    return RegisterListener(displayMoveListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterDisplayMoveListener(sptr<IDisplayMoveListener>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::mutex> lockListener(displayMoveListenerMutex_);
    return UnregisterListener(displayMoveListeners_[GetPersistentId()], listener);
}

/**
 * Currently only supports system windows.
 */
WMError WindowSessionImpl::EnableDrag(bool enableDrag)
{
    if (!IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    property_->SetDragEnabled(enableDrag);
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
    WMError errorCode = hostSession->SetSystemWindowEnableDrag(enableDrag);
    TLOGI(WmsLogTag::WMS_LAYOUT, "Id:%{public}d, enableDrag:%{public}d, errcode:%{public}d",
        GetPersistentId(), enableDrag, static_cast<int>(errorCode));
    return static_cast<WMError>(errorCode);
}

WMError WindowSessionImpl::RegisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::recursive_mutex> lockListener(occupiedAreaChangeListenerMutex_);
    return RegisterListener(occupiedAreaChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterOccupiedAreaChangeListener(const sptr<IOccupiedAreaChangeListener>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::recursive_mutex> lockListener(occupiedAreaChangeListenerMutex_);
    return UnregisterListener(occupiedAreaChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterKeyboardDidShowListener(const sptr<IKeyboardDidShowListener>& listener)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "in");
    std::lock_guard<std::recursive_mutex> lockListener(keyboardDidShowListenerMutex_);
    WMError ret = RegisterListener(keyboardDidShowListeners_[GetPersistentId()], listener);
    if (ret == WMError::WM_OK && isKeyboardDidShowRegistered_ == false) {
        auto hostSession = GetHostSession();
        CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
        hostSession->NotifyKeyboardDidShowRegistered(true);
        isKeyboardDidShowRegistered_ = true;
    }
    return ret;
}

WMError WindowSessionImpl::UnregisterKeyboardDidShowListener(const sptr<IKeyboardDidShowListener>& listener)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "in");
    std::lock_guard<std::recursive_mutex> lockListener(keyboardDidShowListenerMutex_);
    WMError ret = UnregisterListener(keyboardDidShowListeners_[GetPersistentId()], listener);
    if (keyboardDidShowListeners_[GetPersistentId()].empty()) {
        auto hostSession = GetHostSession();
        CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
        hostSession->NotifyKeyboardDidShowRegistered(false);
        isKeyboardDidShowRegistered_ = false;
    }
    return ret;
}

WMError WindowSessionImpl::RegisterKeyboardDidHideListener(const sptr<IKeyboardDidHideListener>& listener)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "in");
    std::lock_guard<std::recursive_mutex> lockListener(keyboardDidHideListenerMutex_);
    WMError ret = RegisterListener(keyboardDidHideListeners_[GetPersistentId()], listener);
    if (ret == WMError::WM_OK && isKeyboardDidHideRegistered_ == false) {
        auto hostSession = GetHostSession();
        CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
        hostSession->NotifyKeyboardDidHideRegistered(true);
        isKeyboardDidHideRegistered_ = true;
    }
    return ret;
}

WMError WindowSessionImpl::UnregisterKeyboardDidHideListener(const sptr<IKeyboardDidHideListener>& listener)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "in");
    std::lock_guard<std::recursive_mutex> lockListener(keyboardDidHideListenerMutex_);
    WMError ret = UnregisterListener(keyboardDidHideListeners_[GetPersistentId()], listener);
    if (keyboardDidHideListeners_[GetPersistentId()].empty()) {
        auto hostSession = GetHostSession();
        CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
        hostSession->NotifyKeyboardDidHideRegistered(false);
        isKeyboardDidHideRegistered_ = false;
    }
    return ret;
}

WMError WindowSessionImpl::UnregisterLifeCycleListener(const sptr<IWindowLifeCycle>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::recursive_mutex> lockListener(lifeCycleListenerMutex_);
    return UnregisterListener(lifecycleListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
    return RegisterListener(windowChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterWindowChangeListener(const sptr<IWindowChangeListener>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
    return UnregisterListener(windowChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterWindowCrossAxisListener(const sptr<IWindowCrossAxisListener>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
    return RegisterListener(windowCrossAxisListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterWindowCrossAxisListener(const sptr<IWindowCrossAxisListener>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
    return UnregisterListener(windowCrossAxisListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterWindowStatusChangeListener(const sptr<IWindowStatusChangeListener>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::recursive_mutex> lockListener(windowStatusChangeListenerMutex_);
    return RegisterListener(windowStatusChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterWindowStatusChangeListener(const sptr<IWindowStatusChangeListener>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::recursive_mutex> lockListener(windowStatusChangeListenerMutex_);
    return UnregisterListener(windowStatusChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::SetDecorVisible(bool isVisible)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "uicontent is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent->SetContainerModalTitleVisible(isVisible, true);
    TLOGD(WmsLogTag::WMS_DECOR, "end");
    return WMError::WM_OK;
}

WMError WindowSessionImpl::GetDecorVisible(bool& isVisible)
{
    TLOGD(WmsLogTag::WMS_DECOR, "%{public}u in", GetWindowId());
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!IsPcOrPadCapabilityEnabled()) {
        TLOGE(WmsLogTag::WMS_DECOR, "device not support");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "uicontent is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    isVisible = uiContent->GetContainerModalTitleVisible(GetImmersiveModeEnabledState());
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetWindowTitleMoveEnabled(bool enable)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_DECOR, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (!WindowHelper::IsMainWindow(GetType()) && !WindowHelper::IsSubWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_DECOR, "called by invalid window type, type:%{public}d", GetType());
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "uicontent is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent->EnableContainerModalGesture(enable);
    TLOGI(WmsLogTag::WMS_DECOR, "enable:%{public}d end", enable);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetSubWindowModal(bool isModal, ModalityType modalityType)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!WindowHelper::IsSubWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_SUB, "called by invalid window type, type:%{public}d", GetType());
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    if (modalityType == ModalityType::APPLICATION_MODALITY && !IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_SUB, "device not support");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }

    WMError modalRet = isModal ?
        AddWindowFlag(WindowFlag::WINDOW_FLAG_IS_MODAL) :
        RemoveWindowFlag(WindowFlag::WINDOW_FLAG_IS_MODAL);
    if (modalRet != WMError::WM_OK) {
        return modalRet;
    }
    modalRet = isModal && modalityType == ModalityType::APPLICATION_MODALITY ?
        AddWindowFlag(WindowFlag::WINDOW_FLAG_IS_APPLICATION_MODAL) :
        RemoveWindowFlag(WindowFlag::WINDOW_FLAG_IS_APPLICATION_MODAL);
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
    SubWindowModalType subWindowModalType = SubWindowModalType::TYPE_NORMAL;
    if (isModal) {
        subWindowModalType = modalityType == ModalityType::WINDOW_MODALITY ?
            SubWindowModalType::TYPE_WINDOW_MODALITY :
            SubWindowModalType::TYPE_APPLICATION_MODALITY;
    }
    if (!(property_->GetSubWindowZLevel() <= MAXIMUM_Z_LEVEL && !isModal)) {
        int32_t zLevel = GetSubWindowZLevelByFlags(GetType(), GetWindowFlags(), IsTopmost());
        property_->SetSubWindowZLevel(zLevel);
        UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_SUB_WINDOW_Z_LEVEL);
    }
    hostSession->NotifySubModalTypeChange(subWindowModalType);
    return modalRet;
}

WMError WindowSessionImpl::SetWindowModal(bool isModal)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!IsPcOrPadFreeMultiWindowMode()) {
        TLOGE(WmsLogTag::WMS_MAIN, "device not support");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (!WindowHelper::IsMainWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_MAIN, "called by invalid window type, type:%{public}d", GetType());
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    WMError modalRet = isModal ?
        AddWindowFlag(WindowFlag::WINDOW_FLAG_IS_MODAL) :
        RemoveWindowFlag(WindowFlag::WINDOW_FLAG_IS_MODAL);
    if (modalRet != WMError::WM_OK) {
        return modalRet;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_SYSTEM_ABNORMALLY);
    hostSession->NotifyMainModalTypeChange(isModal);
    return modalRet;
}

WMError WindowSessionImpl::SetDecorHeight(int32_t decorHeight)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    float vpr = 0.f;
    auto err = GetVirtualPixelRatio(vpr);
    if (err != WMError::WM_OK) {
        return err;
    }
    int32_t decorHeightWithPx = static_cast<int32_t>(decorHeight * vpr);
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent->SetContainerModalTitleHeight(decorHeightWithPx);

    if (auto hostSession = GetHostSession()) {
        hostSession->SetCustomDecorHeight(decorHeight);
    }
    decorHeight_ = decorHeight;
    TLOGD(WmsLogTag::WMS_DECOR, "end, height: %{public}d", decorHeight);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::GetDecorHeight(int32_t& height)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "uiContent is null, windowId: %{public}u", GetWindowId());
        return WMError::WM_ERROR_NULLPTR;
    }
    height = uiContent->GetContainerModalTitleHeight();
    if (height == -1) {
        height = 0;
        TLOGD(WmsLogTag::WMS_DECOR, "Get app window decor height failed");
        return WMError::WM_OK;
    }
    float vpr = 0.f;
    auto err = GetVirtualPixelRatio(vpr);
    if (err != WMError::WM_OK) {
        return err;
    }
    height = static_cast<int32_t>(height / vpr);
    TLOGD(WmsLogTag::WMS_DECOR, "end, height: %{public}d", height);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetDecorButtonStyle(const DecorButtonStyle& decorButtonStyle)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    if (!WindowHelper::CheckButtonStyleValid(decorButtonStyle)) {
        TLOGE(WmsLogTag::WMS_DECOR, "set decor button style param invalid");
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    if (!WindowHelper::IsMainWindow(GetType()) && !WindowHelper::IsSubWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_DECOR, "called by invalid window type, type:%{public}d", GetType());
        return WMError::WM_ERROR_INVALID_CALLING;
    }

    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "uiContent is null, windowId: %{public}u", GetWindowId());
        return WMError::WM_ERROR_NULLPTR;
    }
    uiContent->SetContainerButtonStyle(decorButtonStyle);
    decorButtonStyle_ = decorButtonStyle;
    return WMError::WM_OK;
}

WMError WindowSessionImpl::GetDecorButtonStyle(DecorButtonStyle& decorButtonStyle)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    if (!WindowHelper::IsMainWindow(GetType()) && !WindowHelper::IsSubWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_DECOR, "called by invalid window type, type:%{public}d", GetType());
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    decorButtonStyle = decorButtonStyle_;
    return WMError::WM_OK;
}

WMError WindowSessionImpl::GetTitleButtonArea(TitleButtonRect& titleButtonRect)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    Rect decorRect;
    Rect titleButtonLeftRect;
    bool res = false;
    std::shared_ptr<Ace::UIContent> uiContent = GetUIContentSharedPtr();
    if (uiContent == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }
    res = uiContent->GetContainerModalButtonsRect(decorRect, titleButtonLeftRect);
    if (!res) {
        TLOGE(WmsLogTag::WMS_DECOR, "GetContainerModalButtonsRect failed");
        titleButtonRect.IsUninitializedRect();
        return WMError::WM_OK;
    }
    float vpr = 0.f;
    auto err = GetVirtualPixelRatio(vpr);
    if (err != WMError::WM_OK) {
        return err;
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
        TLOGE(WmsLogTag::DEFAULT, "uiContent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    uiContentRemoteObj = uiContent->GetRemoteObj();
    return WSError::WS_OK;
}

WMError WindowSessionImpl::RegisterWindowTitleButtonRectChangeListener(
    const sptr<IWindowTitleButtonRectChangedListener>& listener)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto persistentId = GetPersistentId();
    TLOGD(WmsLogTag::WMS_DECOR, "Start, id:%{public}d", persistentId);
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "listener is null");
        return WMError::WM_ERROR_NULLPTR;
    }

    {
        std::lock_guard<std::recursive_mutex> lockListener(windowTitleButtonRectChangeListenerMutex_);
        WMError ret = RegisterListener(windowTitleButtonRectChangeListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            TLOGE(WmsLogTag::WMS_DECOR, "register failed");
            return ret;
        }
    }
    if (auto uiContent = GetUIContentSharedPtr()) {
        const char* const where = __func__;
        uiContent->SubscribeContainerModalButtonsRectChange(
            [where, weakThis = wptr(this)](Rect& decorRect, Rect& titleButtonLeftRect) {
            auto window = weakThis.promote();
            if (!window) {
                TLOGNE(WmsLogTag::WMS_DECOR, "%{public}s window is null", where);
                return;
            }
            float vpr = 0.f;
            auto err = window->GetVirtualPixelRatio(vpr);
            if (err != WMError::WM_OK) {
                return;
            }
            TitleButtonRect titleButtonRect;
            titleButtonRect.posX_ = static_cast<int32_t>(decorRect.width_) -
                static_cast<int32_t>(titleButtonLeftRect.width_) - titleButtonLeftRect.posX_;
            titleButtonRect.posX_ = static_cast<int32_t>(titleButtonRect.posX_ / vpr);
            titleButtonRect.posY_ = static_cast<int32_t>(titleButtonLeftRect.posY_ / vpr);
            titleButtonRect.width_ = static_cast<uint32_t>(titleButtonLeftRect.width_ / vpr);
            titleButtonRect.height_ = static_cast<uint32_t>(titleButtonLeftRect.height_ / vpr);
            window->NotifyWindowTitleButtonRectChange(titleButtonRect);
        });
    }
    return WMError::WM_OK;
}

WMError WindowSessionImpl::UnregisterWindowTitleButtonRectChangeListener(
    const sptr<IWindowTitleButtonRectChangedListener>& listener)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    WMError ret = WMError::WM_OK;
    auto persistentId = GetPersistentId();
    TLOGD(WmsLogTag::WMS_DECOR, "Start, id:%{public}d", persistentId);
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowTitleButtonRectChangeListenerMutex_);
        ret = UnregisterListener(windowTitleButtonRectChangeListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            TLOGE(WmsLogTag::WMS_DECOR, "failed");
            return ret;
        }
    }
    if (auto uiContent = GetUIContentSharedPtr()) {
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
        WLOGFE("listener is null");
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

template<typename T>
EnableIfSame<T, IWindowHighlightChangeListener, std::vector<sptr<IWindowHighlightChangeListener>>>
    WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IWindowHighlightChangeListener>> highlightChangeListeners;
    for (auto& listener : highlightChangeListeners_[GetPersistentId()]) {
        highlightChangeListeners.push_back(listener);
    }
    return highlightChangeListeners;
}

WMError WindowSessionImpl::RegisterWindowHighlightChangeListeners(const sptr<IWindowHighlightChangeListener>& listener)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "name=%{public}s, id=%{public}u", GetWindowName().c_str(), GetPersistentId());
    std::lock_guard<std::mutex> lockListener(highlightChangeListenerMutex_);
    return RegisterListener(highlightChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterWindowHighlightChangeListeners(
    const sptr<IWindowHighlightChangeListener>& listener)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "name=%{public}s, id=%{public}u", GetWindowName().c_str(), GetPersistentId());
    std::lock_guard<std::mutex> lockListener(highlightChangeListenerMutex_);
    return UnregisterListener(highlightChangeListeners_[GetPersistentId()], listener);
}

template<typename T>
EnableIfSame<T, IMainWindowCloseListener, sptr<IMainWindowCloseListener>> WindowSessionImpl::GetListeners()
{
    return mainWindowCloseListeners_[GetPersistentId()];
}

WMError WindowSessionImpl::RegisterMainWindowCloseListeners(const sptr<IMainWindowCloseListener>& listener)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_PC, "listener is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!WindowHelper::IsMainWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_PC, "window type is not supported");
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    if (!IsPcOrPadCapabilityEnabled()) {
        TLOGE(WmsLogTag::WMS_PC, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    std::lock_guard<std::mutex> lockListener(mainWindowCloseListenersMutex_);
    mainWindowCloseListeners_[GetPersistentId()] = listener;
    return WMError::WM_OK;
}

WMError WindowSessionImpl::UnregisterMainWindowCloseListeners(const sptr<IMainWindowCloseListener>& listener)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_PC, "listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!IsPcOrPadCapabilityEnabled()) {
        TLOGE(WmsLogTag::WMS_PC, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (!WindowHelper::IsMainWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_PC, "window type is not supported");
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    std::lock_guard<std::mutex> lockListener(mainWindowCloseListenersMutex_);
    mainWindowCloseListeners_[GetPersistentId()] = nullptr;
    return WMError::WM_OK;
}

template<typename T>
EnableIfSame<T, IWindowWillCloseListener, std::vector<sptr<IWindowWillCloseListener>>> WindowSessionImpl::GetListeners()
{
    return windowWillCloseListeners_[GetPersistentId()];
}

WMError WindowSessionImpl::RegisterWindowWillCloseListeners(const sptr<IWindowWillCloseListener>& listener)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "listener is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!IsPcOrPadCapabilityEnabled()) {
        TLOGE(WmsLogTag::WMS_DECOR, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (!WindowHelper::IsAppWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_DECOR, "window type is not supported");
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    std::lock_guard<std::mutex> lockListener(windowWillCloseListenersMutex_);
    return RegisterListener(windowWillCloseListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnRegisterWindowWillCloseListeners(const sptr<IWindowWillCloseListener>& listener)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_DECOR, "listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!IsPcOrPadCapabilityEnabled()) {
        TLOGE(WmsLogTag::WMS_DECOR, "The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (!WindowHelper::IsAppWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_DECOR, "window type is not supported");
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    std::lock_guard<std::mutex> lockListener(windowWillCloseListenersMutex_);
    return UnregisterListener(windowWillCloseListeners_[GetPersistentId()], listener);
}

template<typename T>
EnableIfSame<T, ISwitchFreeMultiWindowListener,
    std::vector<sptr<ISwitchFreeMultiWindowListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<ISwitchFreeMultiWindowListener>> switchFreeMultiWindowListeners;
    for (auto& listener : switchFreeMultiWindowListeners_[GetPersistentId()]) {
        switchFreeMultiWindowListeners.push_back(listener);
    }
    return switchFreeMultiWindowListeners;
}

WMError WindowSessionImpl::RegisterSwitchFreeMultiWindowListener(const sptr<ISwitchFreeMultiWindowListener>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!WindowHelper::IsMainWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "window type is not supported");
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "Start register");
    std::lock_guard<std::mutex> lockListener(switchFreeMultiWindowListenerMutex_);
    return RegisterListener(switchFreeMultiWindowListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterSwitchFreeMultiWindowListener(const sptr<ISwitchFreeMultiWindowListener>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!WindowHelper::IsMainWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_LAYOUT_PC, "window type is not supported");
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    TLOGD(WmsLogTag::WMS_LAYOUT_PC, "Start unregister");
    std::lock_guard<std::mutex> lockListener(switchFreeMultiWindowListenerMutex_);
    return UnregisterListener(switchFreeMultiWindowListeners_[GetPersistentId()], listener);
}

void WindowSessionImpl::RecoverSessionListener()
{
    auto persistentId = GetPersistentId();
    TLOGI(WmsLogTag::WMS_RECOVER, "with persistentId=%{public}d", persistentId);
    {
        std::lock_guard<std::recursive_mutex> lockListener(avoidAreaChangeListenerMutex_);
        if (avoidAreaChangeListeners_.find(persistentId) != avoidAreaChangeListeners_.end() &&
            !avoidAreaChangeListeners_[persistentId].empty()) {
            SingletonContainer::Get<WindowAdapter>().UpdateSessionAvoidAreaListener(persistentId, true);
        }
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(touchOutsideListenerMutex_);
        if (touchOutsideListeners_.find(persistentId) != touchOutsideListeners_.end() &&
            !touchOutsideListeners_[persistentId].empty()) {
            SingletonContainer::Get<WindowAdapter>().UpdateSessionTouchOutsideListener(persistentId, true);
        }
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowVisibilityChangeListenerMutex_);
        if (windowVisibilityChangeListeners_.find(persistentId) != windowVisibilityChangeListeners_.end() &&
            !windowVisibilityChangeListeners_[persistentId].empty()) {
            SingletonContainer::Get<WindowAdapter>().UpdateSessionWindowVisibilityListener(persistentId, true);
        }
    }
    {
        std::lock_guard<std::mutex> lockListener(windowRectChangeListenerMutex_);
        if (windowRectChangeListeners_.find(persistentId) != windowRectChangeListeners_.end() &&
            !windowRectChangeListeners_[persistentId].empty()) {
            if (auto hostSession = GetHostSession()) {
                hostSession->UpdateRectChangeListenerRegistered(true);
            }
        }
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
EnableIfSame<T, IWindowCrossAxisListener, std::vector<sptr<IWindowCrossAxisListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IWindowCrossAxisListener>> windowCrossAxisListeners;
    for (const auto& listener : windowCrossAxisListeners_[GetPersistentId()]) {
        windowCrossAxisListeners.push_back(listener);
    }
    return windowCrossAxisListeners;
}

void WindowSessionImpl::NotifyWindowCrossAxisChange(CrossAxisState state)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "id: %{public}d, cross axis state %{public}d", GetPersistentId(),
        static_cast<uint32_t>(state));
    crossAxisState_ = state;
    AAFwk::Want want;
    want.SetParam(Extension::CROSS_AXIS_FIELD, static_cast<int32_t>(state));
    if (auto uiContent = GetUIContentSharedPtr()) {
        uiContent->SendUIExtProprty(static_cast<uint32_t>(Extension::Businesscode::SYNC_CROSS_AXIS_STATE),
            want, static_cast<uint8_t>(SubSystemId::WM_UIEXT));
    }
    std::lock_guard<std::recursive_mutex> lockListener(windowTitleButtonRectChangeListenerMutex_);
    auto windowCrossAxisListeners = GetListeners<IWindowCrossAxisListener>();
    for (const auto& listener : windowCrossAxisListeners) {
        if (listener != nullptr) {
            listener->OnCrossAxisChange(state);
        }
    }
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
EnableIfSame<T, IKeyboardDidShowListener, std::vector<sptr<IKeyboardDidShowListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IKeyboardDidShowListener>> keyboardDidShowListeners;
    for (auto& listener : keyboardDidShowListeners_[GetPersistentId()]) {
        keyboardDidShowListeners.push_back(listener);
    }
    return keyboardDidShowListeners;
}

template<typename T>
EnableIfSame<T, IKeyboardDidHideListener, std::vector<sptr<IKeyboardDidHideListener>>> WindowSessionImpl::GetListeners()
{
    std::vector<sptr<IKeyboardDidHideListener>> keyboardDidHideListeners;
    for (auto& listener : keyboardDidHideListeners_[GetPersistentId()]) {
        keyboardDidHideListeners.push_back(listener);
    }
    return keyboardDidHideListeners;
}

template<typename T>
WMError WindowSessionImpl::RegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (std::find(holder.begin(), holder.end(), listener) != holder.end()) {
        WLOGFE("already registered");
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
void WindowSessionImpl::ClearUselessListeners(std::unordered_map<int32_t, T>& listeners, int32_t persistentId)
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
        std::lock_guard<std::mutex> lockListener(displayIdChangeListenerMutex_);
        ClearUselessListeners(displayIdChangeListeners_, persistentId);
    }
    {
        std::lock_guard<std::mutex> lockListener(systemDensityChangeListenerMutex_);
        ClearUselessListeners(systemDensityChangeListeners_, persistentId);
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowNoInteractionListenerMutex_);
        ClearUselessListeners(windowNoInteractionListeners_, persistentId);
    }
    {
        std::lock_guard<std::mutex> lockListener(windowRectChangeListenerMutex_);
        ClearUselessListeners(windowRectChangeListeners_, persistentId);
    }
    {
        std::lock_guard<std::mutex> lockListener(subWindowCloseListenersMutex_);
        ClearUselessListeners(subWindowCloseListeners_, persistentId);
    }
    {
        std::lock_guard<std::mutex> lockListener(mainWindowCloseListenersMutex_);
        ClearUselessListeners(mainWindowCloseListeners_, persistentId);
    }
    {
        std::lock_guard<std::mutex> lockListener(windowWillCloseListenersMutex_);
        ClearUselessListeners(windowWillCloseListeners_, persistentId);
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(occupiedAreaChangeListenerMutex_);
        ClearUselessListeners(occupiedAreaChangeListeners_, persistentId);
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(keyboardDidShowListenerMutex_);
        ClearUselessListeners(keyboardDidShowListeners_, persistentId);
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(keyboardDidHideListenerMutex_);
        ClearUselessListeners(keyboardDidHideListeners_, persistentId);
    }
    {
        std::lock_guard<std::mutex> lockListener(highlightChangeListenerMutex_);
        ClearUselessListeners(highlightChangeListeners_, persistentId);
    }
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowCrossAxisListenerMutex_);
        ClearUselessListeners(windowCrossAxisListeners_, persistentId);
    }
    ClearSwitchFreeMultiWindowListenersById(persistentId);
    TLOGI(WmsLogTag::WMS_LIFE, "Clear success, id: %{public}d.", GetPersistentId());
}

void WindowSessionImpl::ClearSwitchFreeMultiWindowListenersById(int32_t persistentId)
{
    std::lock_guard<std::mutex> lockListener(switchFreeMultiWindowListenerMutex_);
    ClearUselessListeners(switchFreeMultiWindowListeners_, persistentId);
}

void WindowSessionImpl::RegisterWindowDestroyedListener(const NotifyNativeWinDestroyFunc& func)
{
    notifyNativeFunc_ = std::move(func);
}

void WindowSessionImpl::ClearVsyncStation()
{
    if (vsyncStation_ != nullptr) {
        vsyncStation_->Destroy();
    }
}

void WindowSessionImpl::SetInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer)
{
    TLOGI(WmsLogTag::WMS_EVENT, "in");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    inputEventConsumer_ = inputEventConsumer;
}

WMError WindowSessionImpl::SetTitleButtonVisible(bool isMaximizeVisible, bool isMinimizeVisible, bool isSplitVisible,
    bool isCloseVisible)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!WindowHelper::IsMainWindow(GetType())) {
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    if (!IsPcOrPadCapabilityEnabled()) {
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (GetUIContentSharedPtr() == nullptr || !IsDecorEnable()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    windowTitleVisibleFlags_ = { isMaximizeVisible, isMinimizeVisible, isSplitVisible, isCloseVisible };
    UpdateTitleButtonVisibility();
    return WMError::WM_OK;
}

WSError WindowSessionImpl::SetSplitButtonVisible(bool isVisible)
{
    TLOGI(WmsLogTag::WMS_DECOR, "isVisible: %{public}d", isVisible);
    auto task = [weakThis = wptr(this), isVisible] {
        auto window = weakThis.promote();
        if (!window) {
            return;
        }
        window->isSplitButtonVisible_ = isVisible;
        window->UpdateTitleButtonVisibility();
    };
    handler_->PostTask(task, "WMS_WindowSessionImpl_SetSplitButtonVisible");
    return WSError::WS_OK;
}

WMError WindowSessionImpl::GetIsMidScene(bool& isMidScene)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, WMError::WM_ERROR_INVALID_WINDOW);
    hostSession->GetIsMidScene(isMidScene);
    return WMError::WM_OK;
}

CrossAxisState WindowSessionImpl::GetCrossAxisState()
{
    if (crossAxisState_ != CrossAxisState::STATE_INVALID) {
        return crossAxisState_;
    }
    if (IsWindowSessionInvalid()) {
        return CrossAxisState::STATE_INVALID;
    }
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, CrossAxisState::STATE_INVALID);
    CrossAxisState state = CrossAxisState::STATE_INVALID;
    if (hostSession->GetCrossAxisState(state) != WSError::WS_OK) {
        return CrossAxisState::STATE_INVALID;
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "window id is %{public}d, state is %{public}d", GetPersistentId(),
        static_cast<uint32_t>(state));
    return state;
}

WSError WindowSessionImpl::SendContainerModalEvent(const std::string& eventName, const std::string& eventValue)
{
    TLOGI(WmsLogTag::WMS_EVENT, "in");
    auto task = [weakThis = wptr(this), eventName, eventValue] {
        auto window = weakThis.promote();
        if (!window) {
            return;
        }
        std::shared_ptr<Ace::UIContent> uiContent = window->GetUIContentSharedPtr();
        if (uiContent == nullptr) {
            TLOGNE(WmsLogTag::WMS_EVENT, "uiContent is null!");
            return;
        }
        TLOGNI(WmsLogTag::WMS_EVENT, "name: %{public}s, value: %{public}s", eventName.c_str(), eventValue.c_str());
        uiContent->OnContainerModalEvent(eventName, eventValue);
    };
    handler_->PostTask(task, "WMS_WindowSessionImpl_SendContainerModalEvent");
    return WSError::WS_OK;
}

WMError WindowSessionImpl::SetWindowContainerColor(const std::string& activeColor, const std::string& inactiveColor)
{
    if (!WindowHelper::IsMainWindow(GetType())) {
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    if (!IsDecorEnable()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!(windowSystemConfig_.IsPcWindow() || windowSystemConfig_.IsPadWindow())) {
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    uint32_t activeColorValue;
    if (!ColorParser::Parse(activeColor, activeColorValue)) {
        TLOGW(WmsLogTag::WMS_DECOR, "window: %{public}s, value: [%{public}s, %{public}u]",
            GetWindowName().c_str(), activeColor.c_str(), activeColorValue);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    uint32_t inactiveColorValue;
    if (!ColorParser::Parse(inactiveColor, inactiveColorValue)) {
        TLOGW(WmsLogTag::WMS_DECOR, "window: %{public}s, value: [%{public}s, %{public}u]",
            GetWindowName().c_str(), inactiveColor.c_str(), inactiveColorValue);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (auto uiContent = GetUIContentSharedPtr()) {
        uiContent->SetWindowContainerColor(activeColorValue, inactiveColorValue);
    }
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

    if (vsyncStation_ != nullptr) {
        TLOGD(WmsLogTag::WMS_MAIN, "enable FrameRateLinker, linkerId=%{public}" PRIu64,
            vsyncStation_->GetFrameRateLinkerId());
        vsyncStation_->SetFrameRateLinkerEnable(true);
        if (WindowHelper::IsMainWindow(GetType())) {
            TLOGD(WmsLogTag::WMS_MAIN, "IsMainWindow: enable soloist linker");
            vsyncStation_->SetDisplaySoloistFrameRateLinkerEnable(true);
        }
    } else {
        TLOGW(WmsLogTag::WMS_MAIN, "vsyncStation is null");
    }
}

void WindowSessionImpl::NotifyAfterDidForeground(uint32_t reason)
{
    TLOGI(WmsLogTag::WMS_LIFE, "reason: %{public}d", reason);
    if (reason != static_cast<uint32_t>(WindowStateChangeReason::USER_SWITCH) &&
        reason != static_cast<uint32_t>(WindowStateChangeReason::ABILITY_CALL)) {
        TLOGI(WmsLogTag::WMS_LIFE, "reason: %{public}d no need notify did foreground", reason);
        return;
    }
    if (handler_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "handler is nullptr");
        return;
    }
    const char* const where = __func__;
    handler_->PostTask([weak = wptr(this), where] {
        auto window = weak.promote();
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s window is nullptr", where);
            return;
        }
        TLOGNI(WmsLogTag::WMS_LIFE, "%{public}s execute", where);
        auto lifecycleListeners = window->GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterDidForeground, lifecycleListeners);
    }, where, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
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

    if (vsyncStation_ != nullptr) {
        TLOGD(WmsLogTag::WMS_MAIN, "disable FrameRateLinker, linkerId=%{public}" PRIu64,
            vsyncStation_->GetFrameRateLinkerId());
        vsyncStation_->SetFrameRateLinkerEnable(false);
        if (WindowHelper::IsMainWindow(GetType())) {
            TLOGD(WmsLogTag::WMS_MAIN, "IsMainWindow: disable soloist linker");
            vsyncStation_->SetDisplaySoloistFrameRateLinkerEnable(false);
        }
    } else {
        TLOGW(WmsLogTag::WMS_MAIN, "vsyncStation is null");
    }
}

void WindowSessionImpl::NotifyAfterDidBackground(uint32_t reason)
{
    TLOGI(WmsLogTag::WMS_LIFE, "reason: %{public}d", reason);
    if (reason != static_cast<uint32_t>(WindowStateChangeReason::USER_SWITCH) &&
        reason != static_cast<uint32_t>(WindowStateChangeReason::ABILITY_CALL)) {
        TLOGI(WmsLogTag::WMS_LIFE, "reason: %{public}d no need notify did background", reason);
        return;
    }
    if (handler_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "handler is nullptr");
        return;
    }
    const char* const where = __func__;
    handler_->PostTask([weak = wptr(this), where] {
        auto window = weak.promote();
        if (window == nullptr) {
            TLOGNI(WmsLogTag::WMS_LIFE, "%{public}s window is nullptr", where);
            return;
        }
        TLOGNI(WmsLogTag::WMS_LIFE, "%{public}s execute", where);
        auto lifecycleListeners = window->GetListeners<IWindowLifeCycle>();
        CALL_LIFECYCLE_LISTENER(AfterDidBackground, lifecycleListeners);
    }, where, 0, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

static void RequestInputMethodCloseKeyboard(bool isNeedKeyboard, bool keepKeyboardFlag)
{
    if (!isNeedKeyboard && !keepKeyboardFlag) {
#ifdef IMF_ENABLE
        if (MiscServices::InputMethodController::GetInstance()) {
            MiscServices::InputMethodController::GetInstance()->RequestHideInput();
            TLOGI(WmsLogTag::WMS_KEYBOARD, "Notify InputMethod framework close keyboard end.");
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
        if (auto uiContent = window->GetUIContentSharedPtr()) {
            // isNeedKeyboard is set by arkui and indicates whether the window needs a keyboard or not.
            isNeedKeyboard = uiContent->NeedSoftKeyboard();
        }
        // whether keep the keyboard created by other windows, support system window and app subwindow.
        bool keepKeyboardFlag = window->property_->GetKeepKeyboardFlag();
        TLOGNI(WmsLogTag::WMS_KEYBOARD, "id: %{public}d, isNeedKeyboard: %{public}d, keepKeyboardFlag: %{public}d",
            window->GetPersistentId(), isNeedKeyboard, keepKeyboardFlag);
        RequestInputMethodCloseKeyboard(isNeedKeyboard, keepKeyboardFlag);
    };
    if (auto uiContent = GetUIContentSharedPtr()) {
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

void WindowSessionImpl::NotifyUIContentHighlightStatus(bool isHighlighted)
{
    if (isHighlighted) {
        CALL_UI_CONTENT(ActiveWindow);
    } else {
        CALL_UI_CONTENT(UnActiveWindow);
    }
}

void WindowSessionImpl::NotifyBeforeDestroy(std::string windowName)
{
    auto task = [this]() {
        if (auto uiContent = GetUIContentSharedPtr()) {
            uiContent->Destroy();
        }
        {
            std::unique_lock<std::shared_mutex> lock(uiContentMutex_);
            if (uiContent_ != nullptr) {
                uiContent_ = nullptr;
                TLOGND(WmsLogTag::WMS_LIFE, "NotifyBeforeDestroy: uiContent destroy success, persistentId:%{public}d",
                    GetPersistentId());
            }
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
        WLOGFE("session is invalid");
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
        WLOGFE("listener is null");
        return;
    }
    std::lock_guard<std::recursive_mutex> lockListener(dialogDeathRecipientListenerMutex_);
    RegisterListener(dialogDeathRecipientListeners_[GetPersistentId()], listener);
}

void WindowSessionImpl::UnregisterDialogDeathRecipientListener(const sptr<IDialogDeathRecipientListener>& listener)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "window %{public}s id %{public}d",
        GetWindowName().c_str(), GetPersistentId());
    std::lock_guard<std::recursive_mutex> lockListener(dialogDeathRecipientListenerMutex_);
    UnregisterListener(dialogDeathRecipientListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterDialogTargetTouchListener(const sptr<IDialogTargetTouchListener>& listener)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "window %{public}s id %{public}d",
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
    TLOGI(WmsLogTag::WMS_DIALOG, "window %{public}s id %{public}d",
        GetWindowName().c_str(), GetPersistentId());
    std::lock_guard<std::recursive_mutex> lockListener(dialogTargetTouchListenerMutex_);
    return UnregisterListener(dialogTargetTouchListener_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterScreenshotListener(const sptr<IScreenshotListener>& listener)
{
    WLOGFD("in");
    std::lock_guard<std::recursive_mutex> lockListener(screenshotListenerMutex_);
    return RegisterListener(screenshotListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterScreenshotListener(const sptr<IScreenshotListener>& listener)
{
    WLOGFD("in");
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
    if (WindowHelper::IsDialogWindow(property_->GetWindowType())) {
        std::lock_guard<std::recursive_mutex> lockListener(dialogDeathRecipientListenerMutex_);
        auto dialogDeathRecipientListener = GetListeners<IDialogDeathRecipientListener>();
        for (auto& listener : dialogDeathRecipientListener) {
            if (listener != nullptr) {
                listener->OnDialogDeathRecipient();
            }
        }
    } else if (WindowHelper::IsSubWindow(property_->GetWindowType())) {
        if (property_->GetIsUIExtFirstSubWindow() && !isUIExtensionAbilityProcess_) {
            Destroy();
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
    WLOGFD("from %{public}" PRIu64 " to %{public}" PRIu64, from, to);
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
    TLOGI(WmsLogTag::WMS_PIP, "in");
    auto task = []() {
        PictureInPictureManager::DoClose(true, true);
    };
    handler_->PostTask(task, "WMS_WindowSessionImpl_NotifyCloseExistPipWindow");
    return WSError::WS_OK;
}

void WindowSessionImpl::NotifyTouchDialogTarget(int32_t posX, int32_t posY)
{
    TLOGI(WmsLogTag::WMS_DIALOG, "window %{public}s id %{public}d", GetWindowName().c_str(), GetPersistentId());
    if (auto hostSession = GetHostSession()) {
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

/** @note @window.layout */
void WindowSessionImpl::NotifySizeChange(Rect rect, WindowSizeChangeReason reason)
{
    {
        std::lock_guard<std::recursive_mutex> lockListener(windowChangeListenerMutex_);
        auto windowChangeListeners = GetListeners<IWindowChangeListener>();
        TLOGD(WmsLogTag::WMS_LAYOUT, "Id:%{public}d, sizeChange listenerSize:%{public}zu",
            GetPersistentId(), windowChangeListeners.size());
        for (auto& listener : windowChangeListeners) {
            if (listener != nullptr) {
                listener->OnSizeChange(rect, reason);
            }
        }
    }
    {
        std::lock_guard<std::mutex> lockRectListener(windowRectChangeListenerMutex_);
        auto windowRectChangeListeners = GetListeners<IWindowRectChangeListener>();
        TLOGD(WmsLogTag::WMS_LAYOUT, "Id:%{public}d, rectChange listenerSize:%{public}zu",
            GetPersistentId(), windowRectChangeListeners.size());
        for (auto& listener : windowRectChangeListeners) {
            if (listener != nullptr) {
                listener->OnRectChange(rect, reason);
            }
        }
    }
}

void WindowSessionImpl::NotifySubWindowClose(bool& terminateCloseProcess)
{
    WLOGFD("in");
    std::lock_guard<std::mutex> lockListener(subWindowCloseListenersMutex_);
    auto subWindowCloseListeners = GetListeners<ISubWindowCloseListener>();
    if (subWindowCloseListeners != nullptr) {
        subWindowCloseListeners->OnSubWindowClose(terminateCloseProcess);
    }
}

WMError WindowSessionImpl::NotifyMainWindowClose(bool& terminateCloseProcess)
{
    std::lock_guard<std::mutex> lockListener(mainWindowCloseListenersMutex_);
    auto mainWindowCloseListener = GetListeners<IMainWindowCloseListener>();
    if (mainWindowCloseListener != nullptr) {
        mainWindowCloseListener->OnMainWindowClose(terminateCloseProcess);
        return WMError::WM_OK;
    }
    return WMError::WM_ERROR_NULLPTR;
}

WMError WindowSessionImpl::NotifyWindowWillClose(sptr<Window> window)
{
    std::lock_guard<std::mutex> lockListener(windowWillCloseListenersMutex_);
    const auto& windowWillCloseListeners = GetListeners<IWindowWillCloseListener>();
    auto res = WMError::WM_ERROR_NULLPTR;
    for (const auto& listener : windowWillCloseListeners) {
        if (listener != nullptr) {
            listener->OnWindowWillClose(window);
            res = WMError::WM_OK;
        }
    }
    return res;
}

void WindowSessionImpl::NotifySwitchFreeMultiWindow(bool enable)
{
    std::lock_guard<std::mutex> lockListener(switchFreeMultiWindowListenerMutex_);
    auto switchFreeMultiWindowListeners = GetListeners<ISwitchFreeMultiWindowListener>();
    for (auto& listener : switchFreeMultiWindowListeners) {
        if (listener != nullptr) {
            listener->OnSwitchFreeMultiWindow(enable);
        }
    }
}

WMError WindowSessionImpl::RegisterAvoidAreaChangeListener(const sptr<IAvoidAreaChangedListener>& listener)
{
    auto persistentId = GetPersistentId();
    TLOGI(WmsLogTag::WMS_IMMS, "win %{public}d", persistentId);
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "listener is null");
        return WMError::WM_ERROR_NULLPTR;
    }

    WMError ret = WMError::WM_OK;
    bool isUpdate = false;
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

WMError WindowSessionImpl::UnregisterAvoidAreaChangeListener(const sptr<IAvoidAreaChangedListener>& listener)
{
    auto persistentId = GetPersistentId();
    TLOGI(WmsLogTag::WMS_IMMS, "win %{public}d", persistentId);
    if (listener == nullptr) {
        WLOGFE("listener is null");
        return WMError::WM_ERROR_NULLPTR;
    }

    WMError ret = WMError::WM_OK;
    bool isUpdate = false;
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

WMError WindowSessionImpl::RegisterExtensionAvoidAreaChangeListener(const sptr<IAvoidAreaChangedListener>& listener)
{
    auto persistentId = GetPersistentId();
    WLOGFI("Start, id:%{public}d", persistentId);
    std::lock_guard<std::recursive_mutex> lockListener(avoidAreaChangeListenerMutex_);
    return RegisterListener(avoidAreaChangeListeners_[persistentId], listener);
}

WMError WindowSessionImpl::UnregisterExtensionAvoidAreaChangeListener(const sptr<IAvoidAreaChangedListener>& listener)
{
    auto persistentId = GetPersistentId();
    WLOGFI("Start, id:%{public}d", persistentId);
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
    bool isUIExtensionWithSystemHost =
        WindowHelper::IsUIExtensionWindow(GetType()) && WindowHelper::IsSystemWindow(GetRootHostWindowType());
    bool isSystemWindow = WindowHelper::IsSystemWindow(GetType());
    uint32_t currentApiVersion = GetTargetAPIVersionByApplicationInfo();
    AvoidArea newAvoidArea;
    // api 18 isolation for UEC with system host window
    if ((isUIExtensionWithSystemHost || isSystemWindow) && currentApiVersion < static_cast<uint32_t>(API_VERSION_18)) {
        TLOGI(WmsLogTag::WMS_IMMS, "win %{public}d api %{public}u type %{public}d not supported",
            GetPersistentId(), currentApiVersion, type);
    } else {
        newAvoidArea = *avoidArea;
        TLOGI(WmsLogTag::WMS_IMMS, "win %{public}d api %{public}u type %{public}d area %{public}s",
            GetPersistentId(), currentApiVersion, type, newAvoidArea.ToString().c_str());
    }

    for (auto& listener : avoidAreaChangeListeners) {
        if (listener != nullptr) {
            listener->OnAvoidAreaChanged(newAvoidArea, type);
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
    auto task = [weak = wptr(this), avoidArea, type] {
        auto window = weak.promote();
        if (!window) {
            return;
        }
        if (window->lastAvoidAreaMap_.find(type) == window->lastAvoidAreaMap_.end() ||
            window->lastAvoidAreaMap_[type] != *avoidArea) {
            window->lastAvoidAreaMap_[type] = *avoidArea;
            window->NotifyAvoidAreaChange(avoidArea, type);
            window->UpdateViewportConfig(window->GetRect(), WindowSizeChangeReason::AVOID_AREA_CHANGE);
        }
    };
    handler_->PostTask(std::move(task), __func__);
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

WSError WindowSessionImpl::NotifyPipWindowSizeChange(double width, double height, double scale)
{
    TLOGI(WmsLogTag::WMS_PIP, "width: %{public}f, height: %{public}f scale: %{public}f", width, height, scale);
    auto task = [width, height, scale]() {
        PictureInPictureManager::PipSizeChange(width, height, scale);
    };
    handler_->PostTask(task, "WMS_WindowSessionImpl_NotifyPipWindowSizeChange");
    return WSError::WS_OK;
}

WMError WindowSessionImpl::RegisterTouchOutsideListener(const sptr<ITouchOutsideListener>& listener)
{
    bool isUpdate = false;
    WMError ret = WMError::WM_OK;
    auto persistentId = GetPersistentId();
    TLOGI(WmsLogTag::WMS_EVENT, "name=%{public}s, id=%{public}u",
        GetWindowName().c_str(), GetPersistentId());
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "listener is null");
        return WMError::WM_ERROR_NULLPTR;
    }

    {
        std::lock_guard<std::recursive_mutex> lockListener(touchOutsideListenerMutex_);
        ret = RegisterListener(touchOutsideListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            TLOGE(WmsLogTag::WMS_EVENT, "fail, ret:%{public}u", ret);
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
    TLOGI(WmsLogTag::WMS_EVENT, "name=%{public}s, id=%{public}u",
        GetWindowName().c_str(), GetPersistentId());
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "listener is null");
        return WMError::WM_ERROR_NULLPTR;
    }

    {
        std::lock_guard<std::recursive_mutex> lockListener(touchOutsideListenerMutex_);
        ret = UnregisterListener(touchOutsideListeners_[persistentId], listener);
        if (ret != WMError::WM_OK) {
            TLOGE(WmsLogTag::WMS_EVENT, "fail, ret:%{public}u", ret);
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
    TLOGD(WmsLogTag::WMS_EVENT, "window: name=%{public}s, id=%{public}u",
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
    WLOGFD("Start, persistentId=%{public}d.", persistentId);
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
    WLOGFD("Start, persistentId=%{public}d.", persistentId);
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

WMError WindowSessionImpl::RegisterDisplayIdChangeListener(const IDisplayIdChangeListenerSptr& listener)
{
    TLOGD(WmsLogTag::WMS_ATTRIBUTE, "name=%{public}s, id=%{public}u", GetWindowName().c_str(), GetPersistentId());
    std::lock_guard<std::mutex> lockListener(displayIdChangeListenerMutex_);
    return RegisterListener(displayIdChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterDisplayIdChangeListener(const IDisplayIdChangeListenerSptr& listener)
{
    TLOGD(WmsLogTag::WMS_ATTRIBUTE, "name=%{public}s, id=%{public}u", GetWindowName().c_str(), GetPersistentId());
    std::lock_guard<std::mutex> lockListener(displayIdChangeListenerMutex_);
    return UnregisterListener(displayIdChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterSystemDensityChangeListener(const ISystemDensityChangeListenerSptr& listener)
{
    TLOGD(WmsLogTag::WMS_ATTRIBUTE, "name=%{public}s, id=%{public}d", GetWindowName().c_str(), GetPersistentId());
    std::lock_guard<std::mutex> lockListener(systemDensityChangeListenerMutex_);
    return RegisterListener(systemDensityChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::UnregisterSystemDensityChangeListener(const ISystemDensityChangeListenerSptr& listener)
{
    TLOGD(WmsLogTag::WMS_ATTRIBUTE, "name=%{public}s, id=%{public}d", GetWindowName().c_str(), GetPersistentId());
    std::lock_guard<std::mutex> lockListener(systemDensityChangeListenerMutex_);
    return UnregisterListener(systemDensityChangeListeners_[GetPersistentId()], listener);
}

WMError WindowSessionImpl::RegisterWindowNoInteractionListener(const IWindowNoInteractionListenerSptr& listener)
{
    WLOGFD("in");
    std::lock_guard<std::recursive_mutex> lockListener(windowNoInteractionListenerMutex_);
    WMError ret = RegisterListener(windowNoInteractionListeners_[GetPersistentId()], listener);
    if (ret != WMError::WM_OK) {
        WLOGFE("register failed.");
    } else {
        SubmitNoInteractionMonitorTask(this->lastInteractionEventId_.load(), listener);
    }
    return ret;
}

WMError WindowSessionImpl::UnregisterWindowNoInteractionListener(const IWindowNoInteractionListenerSptr& listener)
{
    WLOGFD("in");
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
EnableIfSame<T, IDisplayIdChangeListener,
    std::vector<IDisplayIdChangeListenerSptr>> WindowSessionImpl::GetListeners()
{
    return displayIdChangeListeners_[GetPersistentId()];
}

template<typename T>
EnableIfSame<T, ISystemDensityChangeListener,
    std::vector<ISystemDensityChangeListenerSptr>> WindowSessionImpl::GetListeners()
{
    return systemDensityChangeListeners_[GetPersistentId()];
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

WSError WindowSessionImpl::NotifyDisplayIdChange(DisplayId displayId)
{
    TLOGD(WmsLogTag::WMS_ATTRIBUTE, "id=%{public}u, displayId=%{public}" PRIu64, GetPersistentId(), displayId);
    std::lock_guard<std::mutex> lock(displayIdChangeListenerMutex_);
    auto displayIdChangeListeners = GetListeners<IDisplayIdChangeListener>();
    for (auto& listener : displayIdChangeListeners) {
        if (listener != nullptr) {
            listener->OnDisplayIdChanged(displayId);
        }
    }
    return WSError::WS_OK;
}

WMError WindowSessionImpl::NotifyWatchGestureConsumeResult(int32_t keyCode, bool isConsumed)
{
    TLOGD(WmsLogTag::WMS_EVENT, "isConsumed:%{public}d", isConsumed);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SingletonContainer::Get<WindowAdapter>().NotifyWatchGestureConsumeResult(keyCode, isConsumed);
}

bool WindowSessionImpl::GetWatchGestureConsumed() const
{
    return isWatchGestureConsumed_;
}

void WindowSessionImpl::SetWatchGestureConsumed(bool isWatchGestureConsumed)
{
    TLOGD(WmsLogTag::WMS_EVENT, "wid:%{public}d, isWatchGestureConsumed:%{public}d",
        GetPersistentId(), isWatchGestureConsumed);
    isWatchGestureConsumed_ = isWatchGestureConsumed;
}

WSError WindowSessionImpl::NotifySystemDensityChange(float density)
{
    std::lock_guard<std::mutex> lock(systemDensityChangeListenerMutex_);
    const auto& systemDensityChangeListeners = GetListeners<ISystemDensityChangeListener>();
    for (const auto& listener : systemDensityChangeListeners) {
        if (listener != nullptr) {
            listener->OnSystemDensityChanged(density);
        }
    }
    return WSError::WS_OK;
}

WSError WindowSessionImpl::NotifyWindowVisibility(bool isVisible)
{
    TLOGD(WmsLogTag::WMS_ATTRIBUTE, "window: name=%{public}s, id=%{public}u, isVisible=%{public}d",
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
        WLOGFE("invalid listener");
        return WSError::WS_ERROR_NULLPTR;
    }
    WLOGFD("name=%{public}s, id=%{public}u, timeout=%{public}" PRId64,
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
                "Transfer to inputEventConsumer InputTracking id:%{public}d",
                pointerEvent->GetId());
        }
        if (!(inputEventConsumer->OnInputEvent(pointerEvent))) {
            pointerEvent->MarkProcessed();
        }
        return;
    }
    if (FilterPointerEvent(pointerEvent)) {
        return;
    }
    if (auto uiContent = GetUIContentSharedPtr()) {
        if (pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_MOVE) {
            TLOGD(WmsLogTag::WMS_EVENT, "eid:%{public}d", pointerEvent->GetId());
        }
        if (IsWindowDelayRaiseEnabled()) {
            pointerEvent->MarkProcessed();
            return;
        }
        TLOGD(WmsLogTag::WMS_EVENT, "Start to process pointerEvent, id: %{public}d", pointerEvent->GetId());
        if (!uiContent->ProcessPointerEvent(pointerEvent)) {
            TLOGI(WmsLogTag::WMS_INPUT_KEY_FLOW, "UI content dose not consume");
            pointerEvent->MarkProcessed();
        }
    } else {
        if (pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_MOVE) {
            TLOGW(WmsLogTag::WMS_INPUT_KEY_FLOW, "pointerEvent not consumed, windowId:%{public}u", GetWindowId());
        }
        pointerEvent->MarkProcessed();
    }
}

WMError WindowSessionImpl::SetKeyEventFilter(KeyEventFilterFunc filter)
{
    std::unique_lock<std::mutex> lock(keyEventFilterMutex_);
    keyEventFilter_ = std::move(filter);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::ClearKeyEventFilter()
{
    std::unique_lock<std::mutex> lock(keyEventFilterMutex_);
    keyEventFilter_ = nullptr;
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetMouseEventFilter(MouseEventFilterFunc filter)
{
    std::unique_lock<std::mutex> lock(mouseEventFilterMutex_);
    mouseEventFilter_ = std::move(filter);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::ClearMouseEventFilter()
{
    std::unique_lock<std::mutex> lock(mouseEventFilterMutex_);
    mouseEventFilter_ = nullptr;
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetTouchEventFilter(TouchEventFilterFunc filter)
{
    std::unique_lock<std::mutex> lock(touchEventFilterMutex_);
    touchEventFilter_ = std::move(filter);
    return WMError::WM_OK;
}

WMError WindowSessionImpl::ClearTouchEventFilter()
{
    std::unique_lock<std::mutex> lock(touchEventFilterMutex_);
    touchEventFilter_ = nullptr;
    return WMError::WM_OK;
}

bool WindowSessionImpl::FilterKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    std::lock_guard<std::mutex> lock(keyEventFilterMutex_);
    if (keyEventFilter_ != nullptr) {
        bool isFilter = keyEventFilter_(*keyEvent.get());
        TLOGE(WmsLogTag::WMS_SYSTEM, "isFilter:%{public}d", isFilter);
        if (isFilter) {
            keyEvent->MarkProcessed();
            return true;
        }
    }
    return false;
}

void WindowSessionImpl::NotifyConsumeResultToFloatWindow
    (const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool isConsumed)
{
    if ((keyEvent->GetKeyCode() == MMI::KeyEvent::KEYCODE_TAB ||
         keyEvent->GetKeyCode() == MMI::KeyEvent::KEYCODE_ENTER) && !GetWatchGestureConsumed() &&
        keyEvent->GetKeyAction() == MMI::KeyEvent::KEY_ACTION_DOWN) {
        TLOGD(WmsLogTag::WMS_EVENT, "wid:%{public}u, isConsumed:%{public}d", GetWindowId(), isConsumed);
        NotifyWatchGestureConsumeResult(keyEvent->GetKeyCode(), isConsumed);
    }
}

bool WindowSessionImpl::FilterPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    bool isFiltered = false;
    auto sourceType = pointerEvent->GetSourceType();
    auto action = pointerEvent->GetPointerAction();
    if (sourceType == OHOS::MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN) {
        std::lock_guard<std::mutex> lock(touchEventFilterMutex_);
        if (touchEventFilter_ == nullptr) {
            return false;
        }
        isFiltered = touchEventFilter_(*pointerEvent.get());
    } else if (sourceType == OHOS::MMI::PointerEvent::SOURCE_TYPE_MOUSE &&
               (action != OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_BEGIN &&
                action != OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_UPDATE &&
                action != OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_END)) {
        std::lock_guard<std::mutex> lock(mouseEventFilterMutex_);
        if (mouseEventFilter_ == nullptr) {
            return false;
        }
        isFiltered = mouseEventFilter_(*pointerEvent.get());
    }
    if (isFiltered) {
        pointerEvent->MarkProcessed();
    }
    return isFiltered;
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
        WLOGFI("event consumed by back");
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

    if (auto uiContent = GetUIContentSharedPtr()) {
        if (FilterKeyEvent(keyEvent)) return;
        TLOGI(WmsLogTag::WMS_EVENT, "Start to process keyEvent, id: %{public}d", keyEvent->GetId());
        isConsumed = uiContent->ProcessKeyEvent(keyEvent);
        if (!isConsumed && keyEvent->GetKeyCode() == MMI::KeyEvent::KEYCODE_ESCAPE &&
            IsPcOrPadFreeMultiWindowMode() &&
            property_->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
            GetImmersiveModeEnabledState() &&
            keyAction == MMI::KeyEvent::KEY_ACTION_DOWN && !escKeyEventTriggered_) {
            WLOGI("recover from fullscreen cause KEYCODE_ESCAPE");
            Recover();
        }
        NotifyConsumeResultToFloatWindow(keyEvent, isConsumed);
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
    TLOGI(WmsLogTag::WMS_EVENT, "in");
    bool isConsumed = false;
    std::shared_ptr<IInputEventConsumer> inputEventConsumer;
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        inputEventConsumer = inputEventConsumer_;
    }
    if (inputEventConsumer != nullptr) {
        WLOGFD("Transfer back event to inputEventConsumer");
        std::shared_ptr<MMI::KeyEvent> backKeyEvent = MMI::KeyEvent::Create();
        if (backKeyEvent == nullptr) {
            WLOGFE("backKeyEvent is null");
            return WSError::WS_ERROR_NULLPTR;
        }
        backKeyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_BACK);
        backKeyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
        isConsumed = inputEventConsumer->OnInputEvent(backKeyEvent);
    } else {
        if (auto uiContent = GetUIContentSharedPtr()) {
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
    bool isKeySound = (keyCode == MMI::KeyEvent::KEYCODE_SOUND);
    TLOGD(WmsLogTag::WMS_EVENT, "isKeyFN:%{public}d, isKeyboard:%{public}d", isKeyFN, isKeyboard);
    return (isKeyFN || isKeyboard || isKeyBack || isKeySound);
}

void WindowSessionImpl::RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback)
{
    if (vsyncStation_ == nullptr) {
        TLOGW(WmsLogTag::WMS_MAIN, "vsyncStation is null");
        return;
    }
    vsyncStation_->RequestVsync(vsyncCallback);
}

int64_t WindowSessionImpl::GetVSyncPeriod()
{
    if (vsyncStation_ == nullptr) {
        TLOGW(WmsLogTag::WMS_MAIN, "vsyncStation is null");
        return 0;
    }
    return vsyncStation_->GetVSyncPeriod();
}

void WindowSessionImpl::FlushFrameRate(uint32_t rate, int32_t animatorExpectedFrameRate, uint32_t rateType)
{
    if (vsyncStation_ == nullptr) {
        TLOGW(WmsLogTag::WMS_MAIN, "vsyncStation is null");
        return;
    }
    vsyncStation_->FlushFrameRate(rate, animatorExpectedFrameRate, rateType);
}

WMError WindowSessionImpl::UpdateProperty(WSPropertyChangeAction action)
{
    TLOGD(WmsLogTag::DEFAULT, "action:%{public}" PRIu64, action);
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
        TLOGE(WmsLogTag::WMS_ATTRIBUTE, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    uint32_t colorValue;
    if (ColorParser::Parse(color, colorValue)) {
        TLOGD(WmsLogTag::WMS_ATTRIBUTE, "window: %{public}s, value: [%{public}s, %{public}u]",
            GetWindowName().c_str(), color.c_str(), colorValue);
        return SetBackgroundColor(colorValue);
    }
    TLOGE(WmsLogTag::WMS_ATTRIBUTE, "invalid color string: %{public}s", color.c_str());
    return WMError::WM_ERROR_INVALID_PARAM;
}

WMError WindowSessionImpl::SetBackgroundColor(uint32_t color)
{
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "window: %{public}s, value:%{public}u", GetWindowName().c_str(), color);

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

    if (auto uiContent = GetUIContentSharedPtr()) {
        uiContent->SetBackgroundColor(color);
        // 24: Shift right by 24 bits to move the alpha channel to the lowest 8 bits.
        uint8_t alpha = static_cast<uint8_t>((color >> 24) & 0xff);
        property_->SetBackgroundAlpha(alpha);
        UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_BACKGROUND_ALPHA);
        return WMError::WM_OK;
    }

    if (aceAbilityHandler_ != nullptr) {
        aceAbilityHandler_->SetBackgroundColor(color);
        return WMError::WM_OK;
    }
    TLOGD(WmsLogTag::WMS_ATTRIBUTE, "FA mode could not set bg color: %{public}u", GetWindowId());
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
    std::lock_guard<std::recursive_mutex> lock(subWindowSessionMutex_);
    auto iter = subWindowSessionMap_.find(parentId);
    if (iter == subWindowSessionMap_.end()) {
        return std::vector<sptr<Window>>();
    }
    return std::vector<sptr<Window>>(subWindowSessionMap_[parentId].begin(), subWindowSessionMap_[parentId].end());
}

uint32_t WindowSessionImpl::GetBackgroundColor() const
{
    if (auto uiContent = GetUIContentSharedPtr()) {
        return uiContent->GetBackgroundColor();
    }
    WLOGD("uiContent is null, windowId: %{public}u, use FA mode", GetWindowId());
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

WMError WindowSessionImpl::UpdateSystemBarProperties(
    const std::unordered_map<WindowType, SystemBarProperty>& systemBarProperties,
    const std::unordered_map<WindowType, SystemBarPropertyFlag>& systemBarPropertyFlags)
{
    return WMError::WM_OK;
}

WMError WindowSessionImpl::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    return WMError::WM_OK;
}

void WindowSessionImpl::UpdateSpecificSystemBarEnabled(bool systemBarEnable, bool systemBarEnableAnimation,
    SystemBarProperty& property)
{
    property.enable_ = systemBarEnable;
    property.enableAnimation_ = systemBarEnableAnimation;
    // isolate on api 18
    if (GetTargetAPIVersion() >= API_VERSION_18) {
        property.settingFlag_ |= SystemBarSettingFlag::ENABLE_SETTING;
    }
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
            listener->OnSizeChange(info);
        }
    }
}

void WindowSessionImpl::NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info,
                                                     const std::shared_ptr<RSTransaction>& rsTransaction)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "transaction: %{public}d, safeHeight: %{public}u"
        ", occupied rect: x %{public}u, y %{public}u, w %{public}u, h %{public}u", rsTransaction != nullptr,
        info->safeHeight_, info->rect_.posX_, info->rect_.posY_, info->rect_.width_, info->rect_.height_);
    if (handler_ == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "handler is nullptr");
        return;
    }
    auto task = [weak = wptr(this), info, rsTransaction]() {
        auto window = weak.promote();
        if (!window) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "window is nullptr, notify occupied area change info failed");
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
    handler_->PostTask(task, "WMS_WindowSessionImpl_NotifyOccupiedAreaChangeInfo");
}

void WindowSessionImpl::NotifyKeyboardDidShow(const KeyboardPanelInfo& keyboardPanelInfo)
{
    std::lock_guard<std::recursive_mutex> lockListener(keyboardDidShowListenerMutex_);
    auto keyboardDidShowListeners = GetListeners<IKeyboardDidShowListener>();
    for (const auto& listener : keyboardDidShowListeners) {
        if (listener != nullptr) {
            listener->OnKeyboardDidShow(keyboardPanelInfo);
        }
    }
}

void WindowSessionImpl::NotifyKeyboardDidHide(const KeyboardPanelInfo& keyboardPanelInfo)
{
    std::lock_guard<std::recursive_mutex> lockListener(keyboardDidHideListenerMutex_);
    auto keyboardDidHideListeners = GetListeners<IKeyboardDidHideListener>();
    for (const auto& listener : keyboardDidHideListeners) {
        if (listener != nullptr) {
            listener->OnKeyboardDidHide(keyboardPanelInfo);
        }
    }
}

void WindowSessionImpl::NotifyKeyboardAnimationCompleted(const KeyboardPanelInfo& keyboardPanelInfo)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "isShowAnimation: %{public}d, beginRect: %{public}s, endRect: %{public}s",
        keyboardPanelInfo.isShowing_, keyboardPanelInfo.beginRect_.ToString().c_str(),
        keyboardPanelInfo.endRect_.ToString().c_str());
    if (handler_ == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "handler is nullptr");
        return;
    }
    auto task = [weak = wptr(this), keyboardPanelInfo]() {
        auto window = weak.promote();
        if (!window) {
            TLOGNE(WmsLogTag::WMS_KEYBOARD, "window is nullptr, notify keyboard animation completed failed.");
            return;
        }
        if (keyboardPanelInfo.isShowing_) {
            window->NotifyKeyboardDidShow(keyboardPanelInfo);
        } else {
            window->NotifyKeyboardDidHide(keyboardPanelInfo);
        }
    };
    handler_->PostTask(task, __func__);
}

KeyboardAnimationConfig WindowSessionImpl::GetKeyboardAnimationConfig()
{
    return { windowSystemConfig_.animationIn_, windowSystemConfig_.animationOut_ };
}

void WindowSessionImpl::DumpSessionElementInfo(const std::vector<std::string>& params)
{
    WLOGFD("in");
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
    WLOGFD("in");
}

void WindowSessionImpl::NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits)
{
    WLOGFD("in");
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
        WLOGFE("session is invalid");
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

void WindowSessionImpl::SetAutoStartPiP(bool isAutoStart, uint32_t priority, uint32_t width, uint32_t height)
{
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_PIP, "session is invalid");
        return;
    }
    if (auto hostSession = GetHostSession()) {
        hostSession->SetAutoStartPiP(isAutoStart, priority, width, height);
    }
}

WindowStatus WindowSessionImpl::GetWindowStatusInner(WindowMode mode)
{
    auto windowStatus = WindowStatus::WINDOW_STATUS_UNDEFINED;
    if (mode == WindowMode::WINDOW_MODE_FLOATING) {
        windowStatus = WindowStatus::WINDOW_STATUS_FLOATING;
        if (property_->GetMaximizeMode() == MaximizeMode::MODE_AVOID_SYSTEM_BAR) {
            windowStatus = WindowStatus::WINDOW_STATUS_MAXIMIZE;
        }
    } else if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        windowStatus = WindowStatus::WINDOW_STATUS_SPLITSCREEN;
    }
    if (mode == WindowMode::WINDOW_MODE_FULLSCREEN) {
        if (IsPcOrPadFreeMultiWindowMode() && GetTargetAPIVersion() >= 14) { // 14: isolated version
            windowStatus = GetImmersiveModeEnabledState() ? WindowStatus::WINDOW_STATUS_FULLSCREEN :
                WindowStatus::WINDOW_STATUS_MAXIMIZE;
        } else {
            windowStatus = WindowStatus::WINDOW_STATUS_FULLSCREEN;
        }
    }
    if (state_ == WindowState::STATE_HIDDEN) {
        windowStatus = WindowStatus::WINDOW_STATUS_MINIMIZE;
    }
    return windowStatus;
}

uint32_t WindowSessionImpl::GetStatusBarHeight() const
{
    uint32_t height = 0;
    auto hostSession = GetHostSession();
    CHECK_HOST_SESSION_RETURN_ERROR_IF_NULL(hostSession, height);
    height = static_cast<uint32_t>(hostSession->GetStatusBarHeight());
    TLOGI(WmsLogTag::WMS_IMMS, "win %{public}u height %{public}u", GetPersistentId(), height);
    return height;
}

/** @note @window.layout */
void WindowSessionImpl::NotifyWindowStatusChange(WindowMode mode)
{
    auto windowStatus = GetWindowStatusInner(mode);
    TLOGD(WmsLogTag::WMS_LAYOUT, "WindowMode: %{public}d, windowStatus: %{public}d", mode, windowStatus);
    std::lock_guard<std::recursive_mutex> lockListener(windowStatusChangeListenerMutex_);
    auto windowStatusChangeListeners = GetListeners<IWindowStatusChangeListener>();
    for (auto& listener : windowStatusChangeListeners) {
        if (listener != nullptr) {
            listener->OnWindowStatusChange(windowStatus);
        }
    }
    if (state_ != WindowState::STATE_HIDDEN) {
        auto ret = HiSysEventWrite(
            HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
            "WINDOW_STATUS_CHANGE",
            HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
            "BUNDLE_NAME", property_->GetSessionInfo().bundleName_,
            "WINDOW_MODE", static_cast<int32_t>(mode));
        if (ret) {
            TLOGW(WmsLogTag::WMS_FOCUS, "write event fail, WINDOW_STATUS_CHANGE, ret=%{public}d", ret);
        }
    }
}

void WindowSessionImpl::NotifyTransformChange(const Transform& transform)
{
    WLOGFI("in");
    SetCurrentTransform(transform);
    if (auto uiContent = GetUIContentSharedPtr()) {
        uiContent->UpdateTransform(transform);
        SetLayoutTransform(transform);
        SetNeedRenotifyTransform(false);
    } else {
        SetNeedRenotifyTransform(true);
    }
}

void WindowSessionImpl::NotifySingleHandTransformChange(const SingleHandTransform& singleHandTransform)
{
    singleHandTransform_ = singleHandTransform;
    if (auto uiContent = GetUIContentSharedPtr()) {
        TLOGI(WmsLogTag::WMS_LAYOUT, "id:%{public}d, posX:%{public}d, posY:%{public}d, "
              "scaleX:%{public}f, scaleY:%{public}f", GetPersistentId(),
              singleHandTransform.posX, singleHandTransform.posY,
              singleHandTransform.scaleX, singleHandTransform.scaleY);
        uiContent->UpdateSingleHandTransform(singleHandTransform);
    } else {
        TLOGW(WmsLogTag::WMS_LAYOUT, "id:%{public}d, uiContent is nullptr", GetPersistentId());
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

bool WindowSessionImpl::IsVerticalOrientation(Orientation orientation) const
{
    if (orientation == Orientation::VERTICAL ||
        orientation == Orientation::REVERSE_VERTICAL ||
        orientation == Orientation::SENSOR_VERTICAL ||
        orientation == Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED ||
        orientation == Orientation::USER_ROTATION_PORTRAIT ||
        orientation == Orientation::USER_ROTATION_PORTRAIT_INVERTED) {
        return true;
    }
    return false;
}

WMError WindowSessionImpl::GetCallingWindowWindowStatus(WindowStatus& windowStatus) const
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "id: %{public}d", GetPersistentId());
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SingletonContainer::Get<WindowAdapter>().GetCallingWindowWindowStatus(GetPersistentId(), windowStatus);
}

WMError WindowSessionImpl::GetCallingWindowRect(Rect& rect) const
{
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    return SingletonContainer::Get<WindowAdapter>().GetCallingWindowRect(GetPersistentId(), rect);
}

void WindowSessionImpl::SetUiDvsyncSwitch(bool dvsyncSwitch)
{
    if (vsyncStation_ == nullptr) {
        TLOGW(WmsLogTag::WMS_MAIN, "vsyncStation is null");
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

void WindowSessionImpl::SetFrameLayoutCallbackEnable(bool enable)
{
    enableFrameLayoutFinishCb_ = enable;
}

void WindowSessionImpl::UpdateFrameLayoutCallbackIfNeeded(WindowSizeChangeReason wmReason)
{
    bool isDragInPcmode = IsFreeMultiWindowMode() && (wmReason == WindowSizeChangeReason::DRAG_END);
    if (wmReason == WindowSizeChangeReason::FULL_TO_SPLIT || wmReason == WindowSizeChangeReason::SPLIT_TO_FULL ||
        wmReason == WindowSizeChangeReason::FULL_TO_FLOATING || wmReason == WindowSizeChangeReason::FLOATING_TO_FULL ||
        isDragInPcmode) {
        TLOGI(WmsLogTag::WMS_MULTI_WINDOW, "enable framelayoutfinish callback reason:%{public}u", wmReason);
        SetFrameLayoutCallbackEnable(true);
    }
}

WMError WindowSessionImpl::SetContinueState(int32_t continueState)
{
    if (continueState > ContinueState::CONTINUESTATE_MAX || continueState < ContinueState::CONTINUESTATE_UNKNOWN) {
        TLOGE(WmsLogTag::WMS_MAIN, "continueState is invalid: %{public}d", continueState);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    property_->EditSessionInfo().continueState = static_cast<ContinueState>(continueState);
    return WMError::WM_OK;
}

void WindowSessionImpl::SetUIContentComplete()
{
    bool setUIContentCompleted = false;
    if (setUIContentCompleted_.compare_exchange_strong(setUIContentCompleted, true)) {
        TLOGI(WmsLogTag::WMS_LIFE, "persistentId=%{public}d", GetPersistentId());
        handler_->RemoveTask(SET_UICONTENT_TIMEOUT_LISTENER_TASK_NAME + std::to_string(GetPersistentId()));
    } else {
        TLOGI(WmsLogTag::WMS_LIFE, "already SetUIContent, persistentId=%{public}d", GetPersistentId());
    }
}

void WindowSessionImpl::AddSetUIContentTimeoutCheck()
{
    const auto checkBeginTime =
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())
            .time_since_epoch().count();
    auto task = [weakThis = wptr(this), checkBeginTime] {
        auto window = weakThis.promote();
        if (window == nullptr) {
            TLOGNI(WmsLogTag::WMS_LIFE, "window is nullptr");
            return;
        }
        if (window->setUIContentCompleted_.load()) {
            TLOGNI(WmsLogTag::WMS_LIFE, "already SetUIContent, persistentId=%{public}d", window->GetPersistentId());
            return;
        }

        const auto checkEndTime =
            std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now())
                .time_since_epoch().count();
        if (checkEndTime - checkBeginTime > SET_UICONTENT_TIMEOUT_TIME_AFTER_FREEZE_MS) {
            TLOGNI(WmsLogTag::WMS_LIFE, "will start re-check after freeze, persistentId=%{public}d",
                window->GetPersistentId());
            window->AddSetUIContentTimeoutCheck();
            return;
        }

        TLOGNI(WmsLogTag::WMS_LIFE, "SetUIContent timeout, persistentId=%{public}d", window->GetPersistentId());
        std::ostringstream oss;
        oss << "SetUIContent timeout uid: " << getuid();
        oss << ", windowName: " << window->GetWindowName();
        if (window->context_) {
            oss << ", bundleName: " << window->context_->GetBundleName();
            if (window->context_->GetApplicationInfo()) {
                oss << ", abilityName: " << window->context_->GetApplicationInfo()->name;
            }
        }
        SingletonContainer::Get<WindowInfoReporter>().ReportWindowException(
            static_cast<int32_t>(WindowDFXHelperType::WINDOW_TRANSPARENT_CHECK), getpid(), oss.str());

        if (WindowHelper::IsUIExtensionWindow(window->GetType())) {
            window->NotifyExtensionTimeout(TimeoutErrorCode::SET_UICONTENT_TIMEOUT);
        }
    };
    handler_->PostTask(task, SET_UICONTENT_TIMEOUT_LISTENER_TASK_NAME + std::to_string(GetPersistentId()),
        SET_UICONTENT_TIMEOUT_TIME_MS, AppExecFwk::EventQueue::Priority::HIGH);
}

void WindowSessionImpl::NotifySetUIContentComplete()
{
    if (WindowHelper::IsSubWindow(GetType()) || WindowHelper::IsSystemWindow(GetType())) {
        // created by UIExtension
        auto extWindow = FindExtensionWindowWithContext();
        if (extWindow != nullptr) {
            extWindow->SetUIContentComplete();
        }
    }
    SetUIContentComplete();
}

void WindowSessionImpl::SetUIExtensionDestroyComplete()
{
    bool setUIExtensionDestroyCompleted = false;
    if (setUIExtensionDestroyCompleted_.compare_exchange_strong(setUIExtensionDestroyCompleted, true)) {
        TLOGI(WmsLogTag::WMS_LIFE, "persistentId=%{public}d", GetPersistentId());
        handler_->RemoveTask(SET_UIEXTENSION_DESTROY_TIMEOUT_LISTENER_TASK_NAME + std::to_string(GetPersistentId()));
    } else {
        TLOGI(WmsLogTag::WMS_LIFE, "already, persistentId=%{public}d", GetPersistentId());
    }
}

void WindowSessionImpl::SetUIExtensionDestroyCompleteInSubWindow()
{
    if (WindowHelper::IsSubWindow(GetType()) || WindowHelper::IsSystemWindow(GetType())) {
        bool startUIExtensionDestroyTimer = true;
        auto extensionWindow = FindExtensionWindowWithContext();
        if (extensionWindow != nullptr && extensionWindow->startUIExtensionDestroyTimer_.compare_exchange_strong(
            startUIExtensionDestroyTimer, false)) {
            TLOGI(WmsLogTag::WMS_LIFE, "called");
            extensionWindow->SetUIExtensionDestroyComplete();
            extensionWindow->setUIExtensionDestroyCompleted_.store(false);
        }
    }
}

void WindowSessionImpl::AddSetUIExtensionDestroyTimeoutCheck()
{
    const char* const where = __func__;
    auto task = [weakThis = wptr(this), where] {
        auto window = weakThis.promote();
        if (window == nullptr) {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s: window is nullptr", where);
            return;
        }
        if (window->setUIExtensionDestroyCompleted_.load()) {
            TLOGNI(WmsLogTag::WMS_LIFE, "%{public}s: already, persistentId=%{public}d", where,
                window->GetPersistentId());
            return;
        }

        TLOGNI(WmsLogTag::WMS_LIFE, "%{public}s: timeout, persistentId=%{public}d", where, window->GetPersistentId());
        std::ostringstream oss;
        oss << "SetUIExtDestroy timeout uid: " << getuid();
        oss << ", windowName: " << window->GetWindowName();
        if (window->context_) {
            oss << ", bundleName: " << window->context_->GetBundleName();
            if (window->context_->GetApplicationInfo()) {
                oss << ", abilityName: " << window->context_->GetApplicationInfo()->name;
            }
        }
        SingletonContainer::Get<WindowInfoReporter>().ReportWindowException(
            static_cast<int32_t>(WindowDFXHelperType::WINDOW_TRANSPARENT_CHECK), getpid(), oss.str());

        if (WindowHelper::IsUIExtensionWindow(window->GetType())) {
            window->NotifyExtensionTimeout(TimeoutErrorCode::SET_UIEXTENSION_DESTROY_TIMEOUT);
        }
    };
    handler_->PostTask(task, SET_UIEXTENSION_DESTROY_TIMEOUT_LISTENER_TASK_NAME + std::to_string(GetPersistentId()),
        SET_UIEXTENSION_DESTROY_TIMEOUT_TIME_MS, AppExecFwk::EventQueue::Priority::HIGH);
    startUIExtensionDestroyTimer_.store(true);
}

WSError WindowSessionImpl::SetEnableDragBySystem(bool enableDrag)
{
    TLOGE(WmsLogTag::WMS_LAYOUT, "enableDrag: %{public}d", enableDrag);
    property_->SetDragEnabled(enableDrag);
    return WSError::WS_OK;
}

WSError WindowSessionImpl::SetDragActivated(bool dragActivated)
{
    dragActivated_ = dragActivated;
    return WSError::WS_OK;
}

bool WindowSessionImpl::IsWindowDraggable()
{
    bool isDragEnabled = GetProperty()->GetDragEnabled();
    TLOGD(WmsLogTag::WMS_LAYOUT, "PersistentId: %{public}d, dragEnabled: %{public}d, dragActivate: %{public}d",
        GetPersistentId(), isDragEnabled, dragActivated_.load());
    return isDragEnabled && dragActivated_.load();
}

void WindowSessionImpl::SetTargetAPIVersion(uint32_t targetAPIVersion)
{
    targetAPIVersion_ = targetAPIVersion;
}

uint32_t WindowSessionImpl::GetTargetAPIVersion() const
{
    return targetAPIVersion_;
}

uint32_t WindowSessionImpl::GetTargetAPIVersionByApplicationInfo() const
{
    return (context_ != nullptr && context_->GetApplicationInfo() != nullptr) ?
        static_cast<uint32_t>(context_->GetApplicationInfo()->apiTargetVersion) % API_VERSION_MOD :
        INVALID_TARGET_API_VERSION;
}

void WindowSessionImpl::SetLayoutTransform(const Transform& transform)
{
    std::lock_guard<std::recursive_mutex> lock(transformMutex_);
    layoutTransform_ = transform;
}

Transform WindowSessionImpl::GetLayoutTransform() const
{
    std::lock_guard<std::recursive_mutex> lock(transformMutex_);
    return layoutTransform_;
}

void WindowSessionImpl::SetCurrentTransform(const Transform& transform)
{
    std::lock_guard<std::mutex> lock(currentTransformMutex_);
    currentTransform_ = transform;
}

Transform WindowSessionImpl::GetCurrentTransform() const
{
    std::lock_guard<std::mutex> lock(currentTransformMutex_);
    return currentTransform_;
}

void WindowSessionImpl::RegisterWindowInspectorCallback()
{
    auto getWMSWindowListCallback = [weakThis = wptr(this)]() -> std::optional<WindowListInfo> {
        if (auto window = weakThis.promote()) {
            return std::make_optional<WindowListInfo>({
                window->GetWindowName(), window->GetWindowId(),
                static_cast<uint32_t>(window->GetType()), window->GetRect()
            });
        } else {
            return std::nullopt;
        }
    };
    WindowInspector::GetInstance().RegisterGetWMSWindowListCallback(GetWindowId(), std::move(getWMSWindowListCallback));
}

void WindowSessionImpl::GetExtensionConfig(AAFwk::WantParams& want) const
{
    want.SetParam(Extension::CROSS_AXIS_FIELD, AAFwk::Integer::Box(static_cast<int32_t>(crossAxisState_.load())));
    WindowType rootHostWindowType = (GetType() == WindowType::WINDOW_TYPE_UI_EXTENSION) ?
                                    GetRootHostWindowType() : GetType();
    want.SetParam(Extension::ROOT_HOST_WINDOW_TYPE_FIELD,
                  AAFwk::Integer::Box(static_cast<int32_t>(rootHostWindowType)));
}

void WindowSessionImpl::UpdateExtensionConfig(const std::shared_ptr<AAFwk::Want>& want)
{
    if (want == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "null want ptr");
        return;
    }

    const auto& configParam = want->GetParams().GetWantParams(Extension::UIEXTENSION_CONFIG_FIELD);
    auto state = configParam.GetIntParam(Extension::CROSS_AXIS_FIELD, 0);
    if (IsValidCrossState(state)) {
        crossAxisState_ = static_cast<CrossAxisState>(state);
    }
    want->RemoveParam(Extension::UIEXTENSION_CONFIG_FIELD);
    auto rootHostWindowType =
        static_cast<WindowType>(configParam.GetIntParam(Extension::ROOT_HOST_WINDOW_TYPE_FIELD, 0));
    SetRootHostWindowType(rootHostWindowType);
    TLOGI(WmsLogTag::WMS_ATTRIBUTE, "CrossAxisState: %{public}d, rootHostWindowType: %{public}u, winId: %{public}u",
        state, rootHostWindowType, GetWindowId());
}

bool WindowSessionImpl::IsValidCrossState(int32_t state) const
{
    return state >= static_cast<int32_t>(CrossAxisState::STATE_INVALID) &&
        state < static_cast<int32_t>(CrossAxisState::STATE_END);
}

void WindowSessionImpl::UpdateSubWindowLevel(uint32_t subWindowLevel)
{
    std::vector<sptr<WindowSessionImpl>> subWindows;
    GetSubWidnows(GetPersistentId(), subWindows);
    for (auto& subWindow : subWindows) {
        if (subWindow != nullptr) {
            subWindow->UpdateSubWindowLevel(subWindowLevel + 1);
        }
    }
}

bool WindowSessionImpl::IsSubWindowMaximizeSupported() const
{
    if (!WindowHelper::IsSubWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Maximize fail not subwindow");
        return false;
    }
    if (windowOption_ != nullptr) {
        return windowOption_->GetSubWindowMaximizeSupported();
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS
