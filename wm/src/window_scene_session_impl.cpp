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

#include <ability_manager_client.h>
#include <parameters.h>
#include <transaction/rs_transaction.h>

#include "anr_handler.h"
#include "color_parser.h"
#include "display_info.h"
#include "singleton_container.h"
#include "display_manager.h"
#include "perform_reporter.h"
#include "session_permission.h"
#include "session/container/include/window_event_channel.h"
#include "session_manager/include/session_manager.h"
#include "singleton_container.h"
#include "window_adapter.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "window_prepare_terminate.h"
#include "wm_common.h"
#include "wm_math.h"
#include "session_manager_agent_controller.h"
#include <transaction/rs_interfaces.h>
#include "surface_capture_future.h"

#include "window_session_impl.h"

namespace OHOS {
namespace Rosen {
union WSColorParam {
#if defined(BIG_ENDIANNESS) && BIG_ENDIANNESS
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
const std::string PARAM_DUMP_HELP = "-h";
}
uint32_t WindowSceneSessionImpl::maxFloatingWindowSize_ = 1920;

WindowSceneSessionImpl::WindowSceneSessionImpl(const sptr<WindowOption>& option) : WindowSessionImpl(option)
{
    WLOGFI("[WMSCom] Constructor");
}

WindowSceneSessionImpl::~WindowSceneSessionImpl()
{
    WLOGFI("[WMSCom] Destructor");
}

bool WindowSceneSessionImpl::IsValidSystemWindowType(const WindowType& type)
{
    if (!(type == WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW || type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT ||
        type == WindowType::WINDOW_TYPE_FLOAT_CAMERA || type == WindowType::WINDOW_TYPE_DIALOG ||
        type == WindowType::WINDOW_TYPE_FLOAT || type == WindowType::WINDOW_TYPE_SCREENSHOT ||
        type == WindowType::WINDOW_TYPE_VOICE_INTERACTION || type == WindowType::WINDOW_TYPE_POINTER ||
        type == WindowType::WINDOW_TYPE_TOAST || type == WindowType::WINDOW_TYPE_DRAGGING_EFFECT ||
        type == WindowType::WINDOW_TYPE_SEARCHING_BAR || type == WindowType::WINDOW_TYPE_PANEL ||
        type == WindowType::WINDOW_TYPE_VOLUME_OVERLAY || type == WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR ||
        type == WindowType::WINDOW_TYPE_SYSTEM_TOAST || type == WindowType::WINDOW_TYPE_SYSTEM_FLOAT ||
        type == WindowType::WINDOW_TYPE_PIP)) {
        WLOGFW("[WMSSystem] Invalid type: %{public}u", GetType());
        return false;
    }
    return true;
}

sptr<WindowSessionImpl> WindowSceneSessionImpl::FindParentSessionByParentId(uint32_t parentId)
{
    std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
    for (const auto& item : windowSessionMap_) {
        if (item.second.second && item.second.second->GetProperty() &&
            item.second.second->GetWindowId() == parentId &&
            WindowHelper::IsMainWindow(item.second.second->GetType())) {
            WLOGFD("Find parent, [parentName: %{public}s, parentId:%{public}u, selfPersistentId: %{public}d]",
                item.second.second->GetProperty()->GetWindowName().c_str(), parentId, GetProperty()->GetPersistentId());
            return item.second.second;
        }
    }
    WLOGFD("[WMSCom] Can not find parent window, id: %{public}d", parentId);
    return nullptr;
}

sptr<WindowSessionImpl> WindowSceneSessionImpl::FindMainWindowWithContext()
{
    std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
    for (const auto& winPair : windowSessionMap_) {
        auto win = winPair.second.second;
        if (win && win->GetType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
            context_.get() == win->GetContext().get()) {
            return win;
        }
    }
    WLOGFW("Can not find main window, not app type");
    return nullptr;
}

WMError WindowSceneSessionImpl::CreateAndConnectSpecificSession()
{
    sptr<ISessionStage> iSessionStage(this);
    sptr<WindowEventChannel> channel = new (std::nothrow) WindowEventChannel(iSessionStage);
    if (channel == nullptr || property_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    sptr<IWindowEventChannel> eventChannel(channel);
    auto persistentId = INVALID_SESSION_ID;
    sptr<Rosen::ISession> session;
    sptr<IRemoteObject> token = context_ ? context_->GetToken() : nullptr;
    if (token) {
        property_->SetTokenState(true);
    }
    const WindowType type = GetType();
    if (WindowHelper::IsSubWindow(type)) { // sub window
        auto parentSession = FindParentSessionByParentId(property_->GetParentId());
        if (parentSession == nullptr || parentSession->GetHostSession() == nullptr) {
            return WMError::WM_ERROR_NULLPTR;
        }
        // set parent persistentId
        property_->SetParentPersistentId(parentSession->GetPersistentId());
        windowSystemConfig_ = parentSession->GetSystemSessionConfig();
        // creat sub session by parent session
        SingletonContainer::Get<WindowAdapter>().CreateAndConnectSpecificSession(iSessionStage, eventChannel,
            surfaceNode_, property_, persistentId, session, token);
        // update subWindowSessionMap_
        subWindowSessionMap_[parentSession->GetPersistentId()].push_back(this);
    } else { // system window
        if (WindowHelper::IsAppFloatingWindow(type) || WindowHelper::IsPipWindow(type)) {
            property_->SetParentPersistentId(GetFloatingWindowParentId());
            WLOGFI("property set parentPersistentId: %{public}d", property_->GetParentPersistentId());
            auto mainWindow = FindMainWindowWithContext();
            property_->SetFloatingWindowAppType(mainWindow != nullptr ? true : false);
        } else if (type == WindowType::WINDOW_TYPE_DIALOG) {
            auto mainWindow = FindMainWindowWithContext();
            if (mainWindow != nullptr) {
                property_->SetParentPersistentId(mainWindow->GetPersistentId());
                WLOGFD("[WMSDialog] Set parentId for dialog, parentId:%{public}d", mainWindow->GetPersistentId());
            }
        }
        PreProcessCreate();
        SingletonContainer::Get<WindowAdapter>().CreateAndConnectSpecificSession(iSessionStage, eventChannel,
            surfaceNode_, property_, persistentId, session, token);
    }
    property_->SetPersistentId(persistentId);
    if (session == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    hostSession_ = session;
    WLOGFI("[WMSSystem][WMSSub] CreateAndConnectSpecificSession [name:%{public}s, id:%{public}d, type: %{public}u]",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), GetType());
    return WMError::WM_OK;
}

void WindowSceneSessionImpl::UpdateWindowState()
{
    {
        std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
        windowSessionMap_.insert(std::make_pair(property_->GetWindowName(),
        std::pair<uint64_t, sptr<WindowSessionImpl>>(property_->GetPersistentId(), this)));
    }
    state_ = WindowState::STATE_CREATED;
    requestState_ = WindowState::STATE_CREATED;
    if (WindowHelper::IsMainWindow(GetType())) {
        maxFloatingWindowSize_ = windowSystemConfig_.maxFloatingWindowSize_;
        if (property_->GetIsNeedUpdateWindowMode()) {
            WLOGFI("UpdateWindowMode %{public}u mode %{public}u", GetWindowId(), static_cast<uint32_t>(property_->GetWindowMode()));
            UpdateWindowModeImmediately(property_->GetWindowMode());
            property_->SetIsNeedUpdateWindowMode(false);
        } else {
            SetWindowMode(windowSystemConfig_.defaultWindowMode_);
        }
        NotifyWindowNeedAvoid(
            (property_->GetWindowFlags()) & (static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID)));
        GetConfigurationFromAbilityInfo();
    } else {
        UpdateWindowSizeLimits();
    }
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
    AdjustWindowAnimationFlag();
    if (context && context->GetApplicationInfo() &&
        context->GetApplicationInfo()->apiCompatibleVersion >= 9 && // 9: api version
        !SessionPermission::IsSystemCalling()) {
        WLOGI("Remove window flag WINDOW_FLAG_SHOW_WHEN_LOCKED");
        property_->SetWindowFlags(property_->GetWindowFlags() &
            (~(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED))));
    }
    if (hostSession_) { // main window
        ret = Connect();
    } else { // system or sub window
        WLOGFI("Create system or sub window");
        if (WindowHelper::IsSystemWindow(GetType())) {
            if (GetType() == WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW) {
                WLOGFI("System sub window is not support");
                return WMError::WM_ERROR_INVALID_TYPE;
            }
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
        UpdateWindowState();
    }
    WLOGFD("[WMSCom] Window Create [name:%{public}s, id:%{public}d], state:%{pubic}u, windowmode:%{public}u",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), state_, GetMode());
    return ret;
}

void WindowSceneSessionImpl::GetConfigurationFromAbilityInfo()
{
    auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context_);
    if (abilityContext == nullptr) {
        WLOGFE("abilityContext is nullptr");
        return;
    }
    auto abilityInfo = abilityContext->GetAbilityInfo();
    if (abilityInfo != nullptr) {
        property_->SetWindowLimits({
            abilityInfo->maxWindowWidth, abilityInfo->maxWindowHeight,
            abilityInfo->minWindowWidth, abilityInfo->minWindowHeight,
            static_cast<float>(abilityInfo->maxWindowRatio), static_cast<float>(abilityInfo->minWindowRatio)
        });
        UpdateWindowSizeLimits();
        UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS);
        // get support modes configuration
        uint32_t modeSupportInfo = WindowHelper::ConvertSupportModesToSupportInfo(abilityInfo->windowModes);
        if (modeSupportInfo == 0) {
            WLOGFI("mode config param is 0, all modes is supported");
            modeSupportInfo = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
        }
        WLOGFI("winId: %{public}u, modeSupportInfo: %{public}u", GetWindowId(), modeSupportInfo);
        property_->SetModeSupportInfo(modeSupportInfo);
        auto isPhone = system::GetParameter("const.product.devicetype", "unknown") == "phone";
        if (modeSupportInfo == WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN && !isPhone) {
            SetFullScreen(true);
        }
        // get orientation configuration
        OHOS::AppExecFwk::DisplayOrientation displayOrientation =
            static_cast<OHOS::AppExecFwk::DisplayOrientation>(
                static_cast<uint32_t>(abilityInfo->orientation));
        if (ABILITY_TO_SESSION_ORIENTATION_MAP.count(displayOrientation) == 0) {
            WLOGFE("id:%{public}u Do not support this Orientation type", GetWindowId());
            return;
        }
        Orientation orientation = ABILITY_TO_SESSION_ORIENTATION_MAP.at(displayOrientation);
        if (orientation < Orientation::BEGIN || orientation > Orientation::END) {
            WLOGFE("Set orientation from ability failed");
            return;
        }
        property_->SetRequestedOrientation(orientation);
    }
}

uint32_t WindowSceneSessionImpl::UpdateConfigVal(uint32_t minVal, uint32_t maxVal, uint32_t configVal,
                                                 uint32_t defaultVal, float vpr)
{
    bool validConfig = minVal < (configVal * vpr) && (configVal * vpr) < maxVal;
    return validConfig ? static_cast<uint32_t>(configVal * vpr) : static_cast<uint32_t>(defaultVal * vpr);
}

WindowLimits WindowSceneSessionImpl::GetSystemSizeLimits(uint32_t displayWidth,
    uint32_t displayHeight, float vpr)
{
    WindowLimits systemLimits;
    systemLimits.maxWidth_ = static_cast<uint32_t>(maxFloatingWindowSize_ * vpr);
    systemLimits.maxHeight_ = static_cast<uint32_t>(maxFloatingWindowSize_ * vpr);

    if (WindowHelper::IsMainWindow(GetType())) {
        systemLimits.minWidth_ = UpdateConfigVal(0, displayWidth, windowSystemConfig_.miniWidthOfMainWindow_,
                                                 MIN_FLOATING_WIDTH, vpr);
        systemLimits.minHeight_ = UpdateConfigVal(0, displayHeight, windowSystemConfig_.miniHeightOfMainWindow_,
                                                  MIN_FLOATING_HEIGHT, vpr);
    } else if (WindowHelper::IsSubWindow(GetType())) {
        systemLimits.minWidth_ = UpdateConfigVal(0, displayWidth, windowSystemConfig_.miniWidthOfSubWindow_,
                                                 MIN_FLOATING_WIDTH, vpr);
        systemLimits.minHeight_ = UpdateConfigVal(0, displayHeight, windowSystemConfig_.miniHeightOfSubWindow_,
                                                  MIN_FLOATING_HEIGHT, vpr);
    } else {
        systemLimits.minWidth_ = static_cast<uint32_t>(MIN_FLOATING_WIDTH * vpr);
        systemLimits.minHeight_ = static_cast<uint32_t>(MIN_FLOATING_HEIGHT * vpr);
    }
    WLOGFI("[System SizeLimits] [maxWidth: %{public}u, minWidth: %{public}u, maxHeight: %{public}u, "
        "minHeight: %{public}u]", systemLimits.maxWidth_, systemLimits.minWidth_,
        systemLimits.maxHeight_, systemLimits.minHeight_);
    return systemLimits;
}

void WindowSceneSessionImpl::UpdateWindowSizeLimits()
{
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr || display->GetDisplayInfo() == nullptr) {
        return;
    }
    uint32_t displayWidth = static_cast<uint32_t>(display->GetWidth());
    uint32_t displayHeight = static_cast<uint32_t>(display->GetHeight());
    if (displayWidth == 0 || displayHeight == 0) {
        return;
    }

    float virtualPixelRatio = display->GetDisplayInfo()->GetVirtualPixelRatio();
    const auto& systemLimits = GetSystemSizeLimits(displayWidth, displayHeight, virtualPixelRatio);
    const auto& customizedLimits = property_->GetWindowLimits();

    WindowLimits newLimits = systemLimits;

    // configured limits of floating window
    uint32_t configuredMaxWidth = static_cast<uint32_t>(customizedLimits.maxWidth_ * virtualPixelRatio);
    uint32_t configuredMaxHeight = static_cast<uint32_t>(customizedLimits.maxHeight_ * virtualPixelRatio);
    uint32_t configuredMinWidth = static_cast<uint32_t>(customizedLimits.minWidth_ * virtualPixelRatio);
    uint32_t configuredMinHeight = static_cast<uint32_t>(customizedLimits.minHeight_ * virtualPixelRatio);

    // calculate new limit size
    if (systemLimits.minWidth_ <= configuredMaxWidth && configuredMaxWidth <= systemLimits.maxWidth_) {
        newLimits.maxWidth_ = configuredMaxWidth;
    }
    if (systemLimits.minHeight_ <= configuredMaxHeight && configuredMaxHeight <= systemLimits.maxHeight_) {
        newLimits.maxHeight_ = configuredMaxHeight;
    }
    if (systemLimits.minWidth_ <= configuredMinWidth && configuredMinWidth <= newLimits.maxWidth_) {
        newLimits.minWidth_ = configuredMinWidth;
    }
    if (systemLimits.minHeight_ <= configuredMinHeight && configuredMinHeight <= newLimits.maxHeight_) {
        newLimits.minHeight_ = configuredMinHeight;
    }

    // calculate new limit ratio
    newLimits.maxRatio_ = static_cast<float>(newLimits.maxWidth_) / static_cast<float>(newLimits.minHeight_);
    newLimits.minRatio_ = static_cast<float>(newLimits.minWidth_) / static_cast<float>(newLimits.maxHeight_);
    if (!MathHelper::GreatNotEqual(newLimits.minRatio_, customizedLimits.maxRatio_) &&
        !MathHelper::GreatNotEqual(customizedLimits.maxRatio_, newLimits.maxRatio_)) {
        newLimits.maxRatio_ = customizedLimits.maxRatio_;
    }
    if (!MathHelper::GreatNotEqual(newLimits.minRatio_, customizedLimits.minRatio_) &&
        !MathHelper::GreatNotEqual(customizedLimits.minRatio_, newLimits.maxRatio_)) {
        newLimits.minRatio_ = customizedLimits.minRatio_;
    }

    // recalculate limit size by new ratio
    uint32_t newMaxWidth = static_cast<uint32_t>(static_cast<float>(newLimits.maxHeight_) * newLimits.maxRatio_);
    newLimits.maxWidth_ = std::min(newMaxWidth, newLimits.maxWidth_);
    uint32_t newMinWidth = static_cast<uint32_t>(static_cast<float>(newLimits.minHeight_) * newLimits.minRatio_);
    newLimits.minWidth_ = std::max(newMinWidth, newLimits.minWidth_);
    uint32_t newMaxHeight = static_cast<uint32_t>(static_cast<float>(newLimits.maxWidth_) / newLimits.minRatio_);
    newLimits.maxHeight_ = std::min(newMaxHeight, newLimits.maxHeight_);
    uint32_t newMinHeight = static_cast<uint32_t>(static_cast<float>(newLimits.minWidth_) / newLimits.maxRatio_);
    newLimits.minHeight_ = std::max(newMinHeight, newLimits.minHeight_);

    property_->SetWindowLimits(newLimits);
}

void WindowSceneSessionImpl::UpdateSubWindowStateAndNotify(int32_t parentPersistentId, const WindowState& newState)
{
    auto iter = subWindowSessionMap_.find(parentPersistentId);
    if (iter == subWindowSessionMap_.end()) {
        WLOGFD("[WMSSub] main window: %{public}d has no child node", parentPersistentId);
        return;
    }
    const auto& subWindows = iter->second;
    if (subWindows.empty()) {
        WLOGFD("[WMSSub] main window: %{public}d, its subWindowMap is empty", parentPersistentId);
        return;
    }

    // when main window hide and subwindow whose state is shown should hide and notify user
    if (newState == WindowState::STATE_HIDDEN) {
        for (auto subwindow : subWindows) {
            if (subwindow != nullptr && subwindow->GetWindowState() == WindowState::STATE_SHOWN) {
                subwindow->state_ = WindowState::STATE_HIDDEN;
                subwindow->NotifyAfterBackground();
                WLOGFD("[WMSSub] Notify subWindow background, id:%{public}d", subwindow->GetPersistentId());
            }
        }
    // when main window show and subwindow whose state is shown should show and notify user
    } else if (newState == WindowState::STATE_SHOWN) {
        for (auto subwindow : subWindows) {
            if (subwindow != nullptr && subwindow->GetWindowState() == WindowState::STATE_HIDDEN &&
                subwindow->GetRequestWindowState() == WindowState::STATE_SHOWN) {
                subwindow->state_ = WindowState::STATE_SHOWN;
                subwindow->NotifyAfterForeground();
                WLOGFD("[WMSSub] Notify subWindow foreground, id:%{public}d", subwindow->GetPersistentId());
            }
        }
    }
}

WMError WindowSceneSessionImpl::Show(uint32_t reason, bool withAnimation)
{
    const auto& type = GetType();
    WLOGFI("[WMSCom] Window show [name:%{public}s, id:%{public}d, type:%{public}u], reason:%{public}u,"
        " state:%{public}u, requestState:%{public}u", property_->GetWindowName().c_str(),
        property_->GetPersistentId(), type, reason, state_, requestState_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("[WMSCom] session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    UpdateDecorEnable(true);
    if (state_ == WindowState::STATE_SHOWN) {
        WLOGFD("[WMSCom] window session is alreay shown [name:%{public}s, id:%{public}d, type: %{public}u]",
            property_->GetWindowName().c_str(), property_->GetPersistentId(), type);
        NotifyAfterForeground(true, false);
        return WMError::WM_OK;
    }
    if (hostSession_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    WMError ret = UpdateAnimationFlagProperty(withAnimation);
    if (ret != WMError::WM_OK) {
        WLOGFE("[WMSCom] UpdateProperty failed with errCode:%{public}d", static_cast<int32_t>(ret));
        return ret;
    }
    UpdateTitleButtonVisibility();
    if (WindowHelper::IsMainWindow(type)) {
        ret = static_cast<WMError>(hostSession_->Foreground(property_));
    } else if (WindowHelper::IsSubWindow(type) || WindowHelper::IsSystemWindow(type)) {
        ret = static_cast<WMError>(hostSession_->Show(property_));
    } else {
        ret = WMError::WM_ERROR_INVALID_WINDOW;
    }

    if (ret == WMError::WM_OK) {
        // update sub window state if this is main window
        if (WindowHelper::IsMainWindow(type)) {
            UpdateSubWindowStateAndNotify(GetPersistentId(), WindowState::STATE_SHOWN);
        }
        state_ = WindowState::STATE_SHOWN;
        requestState_ = WindowState::STATE_SHOWN;
        NotifyAfterForeground();
    } else {
        NotifyForegroundFailed(ret);
    }
    NotifyWindowStatusChange(GetMode());
    return ret;
}

WMError WindowSceneSessionImpl::Hide(uint32_t reason, bool withAnimation, bool isFromInnerkits)
{
    const auto& type = GetType();
    WLOGFI("[WMSCom] Window hide [id:%{public}d, type: %{public}d, reason:%{public}u, state:%{public}u, "
        "requestState:%{public}u", property_->GetPersistentId(), type, reason, state_, requestState_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    WindowState validState = WindowHelper::IsSubWindow(type) ? requestState_ : state_;
    if (validState == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        WLOGFD("[WMSCom] window session is alreay hidden, id:%{public}d", property_->GetPersistentId());
        return WMError::WM_OK;
    }

    WMError res = UpdateAnimationFlagProperty(withAnimation);
    if (res != WMError::WM_OK) {
        WLOGFE("[WMSCom] UpdateProperty failed with errCode:%{public}d", static_cast<int32_t>(res));
        return res;
    }

    uint32_t animationFlag = property_->GetAnimationFlag();
    if (animationFlag == static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
        animationTransitionController_->AnimationForHidden();
        RSTransaction::FlushImplicitTransaction();
    }

    /*
     * delete after replace WSError with WMError
     * main window no need to notify host, since host knows hide first
     * main window notify host temporarily, since host background may failed
     * need to SetActive(false) for host session before background
     */

    if (WindowHelper::IsMainWindow(type)) {
        res = static_cast<WMError>(SetActive(false));
        if (res != WMError::WM_OK) {
            return res;
        }
        res = static_cast<WMError>(hostSession_->Background());
    } else if (WindowHelper::IsSubWindow(type) || WindowHelper::IsSystemWindow(type)) {
        res = static_cast<WMError>(hostSession_->Hide());
    } else {
        res = WMError::WM_ERROR_INVALID_WINDOW;
    }

    if (res == WMError::WM_OK) {
        // update sub window state if this is main window
        UpdateSubWindowState(type);
        state_ = WindowState::STATE_HIDDEN;
        requestState_ = WindowState::STATE_HIDDEN;
    }
    NotifyWindowStatusChange(GetMode());
    return res;
}

void WindowSceneSessionImpl::UpdateSubWindowState(const WindowType& type)
{
    if (WindowHelper::IsMainWindow(type)) {
        UpdateSubWindowStateAndNotify(GetPersistentId(), WindowState::STATE_HIDDEN);
    }
    if (WindowHelper::IsSubWindow(type)) {
        if (state_ == WindowState::STATE_SHOWN) {
            NotifyAfterBackground();
        }
    } else {
        NotifyAfterBackground();
    }
}

void WindowSceneSessionImpl::PreProcessCreate()
{
    SetDefaultProperty();
}

void WindowSceneSessionImpl::SetDefaultProperty()
{
    switch (property_->GetWindowType()) {
        case WindowType::WINDOW_TYPE_TOAST:
        case WindowType::WINDOW_TYPE_FLOAT:
        case WindowType::WINDOW_TYPE_SYSTEM_FLOAT:
        case WindowType::WINDOW_TYPE_FLOAT_CAMERA:
        case WindowType::WINDOW_TYPE_VOICE_INTERACTION:
        case WindowType::WINDOW_TYPE_SEARCHING_BAR:
        case WindowType::WINDOW_TYPE_SCREENSHOT:
        case WindowType::WINDOW_TYPE_DIALOG:
        case WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW: {
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            break;
        }
        case WindowType::WINDOW_TYPE_VOLUME_OVERLAY:
        case WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT:
        case WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR: {
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            property_->SetFocusable(false);
            break;
        }
        case WindowType::WINDOW_TYPE_SYSTEM_TOAST: {
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            property_->SetTouchable(false);
            property_->SetFocusable(false);
            break;
        }
        default:
            break;
    }
}

WSError WindowSceneSessionImpl::SetActive(bool active)
{
    WLOGFI("[WMSCom] active status: %{public}d", active);
    if (!WindowHelper::IsMainWindow(GetType())) {
        if (hostSession_ == nullptr) {
            WLOGFD("hostSession_ nullptr");
            return WSError::WS_ERROR_INVALID_WINDOW;
        }
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
    const int32_t& parentPersistentId = property_->GetParentPersistentId();
    const int32_t& persistentId = GetPersistentId();

    WLOGFI("[WMSSub] Id: %{public}d, parentId: %{public}d", persistentId, parentPersistentId);

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
                WLOGFD("[WMSSub] Destroy sub window, persistentId: %{public}d", persistentId);
                subWindows.erase(iter);
                break;
            } else {
                WLOGFD("[WMSSub] Exists other sub window, persistentId: %{public}d", persistentId);
                iter++;
            }
        }
    }

    // remove from subWindowMap_ when destroy main window
    auto mainIter = subWindowSessionMap_.find(persistentId);
    if (mainIter != subWindowSessionMap_.end()) {
        auto& subWindows = mainIter->second;
        for (auto iter = subWindows.begin(); iter != subWindows.end(); iter = subWindows.begin()) {
            if ((*iter) == nullptr) {
                WLOGFD("[WMSSub] Destroy sub window which is nullptr");
                subWindows.erase(iter);
                continue;
            }
            WLOGFD("[WMSSub] Destroy sub window, persistentId: %{public}d", (*iter)->GetPersistentId());
            (*iter)->Destroy(false);
        }
        mainIter->second.clear();
        subWindowSessionMap_.erase(mainIter);
    }
}

WMError WindowSceneSessionImpl::Destroy(bool needNotifyServer, bool needClearListener)
{
    WLOGFI("[WMSCom] Id: %{public}d Destroy, state_:%{public}u, needNotifyServer: %{public}d, "
        "needClearListener: %{public}d", property_->GetPersistentId(), state_, needNotifyServer, needClearListener);
    if (IsWindowSessionInvalid()) {
        WLOGFE("[WMSCom] session is invalid");
        return WMError::WM_OK;
    }
    WSError ret = WSError::WS_OK;
    if (!WindowHelper::IsMainWindow(GetType()) && needNotifyServer) {
        if (WindowHelper::IsSystemWindow(GetType())) {
            // main window no need to notify host, since host knows hide first
            SingletonContainer::Get<WindowAdapter>().DestroyAndDisconnectSpecificSession(property_->GetPersistentId());
        } else if (WindowHelper::IsSubWindow(GetType())) {
            auto parentSession = FindParentSessionByParentId(GetParentId());
            if (parentSession == nullptr || parentSession->GetHostSession() == nullptr) {
                return WMError::WM_ERROR_NULLPTR;
            }
            SingletonContainer::Get<WindowAdapter>().DestroyAndDisconnectSpecificSession(property_->GetPersistentId());
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
        requestState_ = WindowState::STATE_DESTROYED;
    }

    DestroySubWindow();
	{
        std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
        windowSessionMap_.erase(property_->GetWindowName());
	}
    DelayedSingleton<ANRHandler>::GetInstance()->OnWindowDestroyed(property_->GetPersistentId());
    hostSession_ = nullptr;
    return res;
}

WMError WindowSceneSessionImpl::MoveTo(int32_t x, int32_t y)
{
    WLOGFI("[WMSCom] Id:%{public}d MoveTo %{public}d %{public}d", property_->GetPersistentId(), x, y);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    const auto& windowRect = GetRect();
    const auto& requestRect = GetRequestRect();
    Rect newRect = { x, y, requestRect.width_, requestRect.height_ }; // must keep x/y
    WLOGFI("[WMSCom] Id:%{public}d, state: %{public}d, type: %{public}d, mode: %{public}d, requestRect: "
        "[%{public}d, %{public}d, %{public}d, %{public}d], windowRect: [%{public}d, %{public}d, "
        "%{public}d, %{public}d], newRect: [%{public}d, %{public}d, %{public}d, %{public}d]",
        property_->GetPersistentId(), state_, GetType(), GetMode(), requestRect.posX_, requestRect.posY_,
        requestRect.width_, requestRect.height_, windowRect.posX_, windowRect.posY_,
        windowRect.width_, windowRect.height_, newRect.posX_, newRect.posY_,
        newRect.width_, newRect.height_);

    property_->SetRequestRect(newRect);

    WSRect wsRect = { newRect.posX_, newRect.posY_, newRect.width_, newRect.height_ };
    auto ret = hostSession_->UpdateSessionRect(wsRect, SizeChangeReason::MOVE);
    return static_cast<WMError>(ret);
}

void WindowSceneSessionImpl::LimitCameraFloatWindowMininumSize(uint32_t& width, uint32_t& height)
{
    // Float camera window has a special limit:
    // if display sw <= 600dp, portrait: min width = display sw * 30%, landscape: min width = sw * 50%
    // if display sw > 600dp, portrait: min width = display sw * 12%, landscape: min width = sw * 30%
    if (GetType() != WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
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
    float vpr = display->GetVirtualPixelRatio();
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

void WindowSceneSessionImpl::UpdateFloatingWindowSizeBySizeLimits(uint32_t& width, uint32_t& height) const
{
    if (property_->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
        return;
    }
    // get new limit config with the settings of system and app
    const auto& sizeLimits = property_->GetWindowLimits();
    // limit minimum size of floating (not system type) window
    if (!WindowHelper::IsSystemWindow(property_->GetWindowType())) {
        width = std::max(sizeLimits.minWidth_, width);
        height = std::max(sizeLimits.minHeight_, height);
    }
    width = std::min(sizeLimits.maxWidth_, width);
    height = std::min(sizeLimits.maxHeight_, height);
    if (height == 0) {
        return;
    }
    float curRatio = static_cast<float>(width) / static_cast<float>(height);
    // there is no need to fix size by ratio if this is not main floating window
    if (!WindowHelper::IsMainFloatingWindow(property_->GetWindowType(), GetMode()) ||
        (!MathHelper::GreatNotEqual(sizeLimits.minRatio_, curRatio) &&
         !MathHelper::GreatNotEqual(curRatio, sizeLimits.maxRatio_))) {
        return;
    }

    float newRatio = curRatio < sizeLimits.minRatio_ ? sizeLimits.minRatio_ : sizeLimits.maxRatio_;
    if (MathHelper::NearZero(newRatio)) {
        return;
    }
    if (sizeLimits.maxWidth_ == sizeLimits.minWidth_) {
        height = static_cast<uint32_t>(static_cast<float>(width) / newRatio);
        return;
    }
    if (sizeLimits.maxHeight_ == sizeLimits.minHeight_) {
        width = static_cast<uint32_t>(static_cast<float>(height) * newRatio);
        return;
    }
    WLOGFD("After limit by customize config: %{public}u %{public}u", width, height);
}

WMError WindowSceneSessionImpl::Resize(uint32_t width, uint32_t height)
{
    WLOGFI("[WMSCom] Id:%{public}d Resize %{public}u %{public}u", property_->GetPersistentId(), width, height);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    // Float camera window has special limits
    LimitCameraFloatWindowMininumSize(width, height);

    UpdateFloatingWindowSizeBySizeLimits(width, height);

    const auto& windowRect = GetRect();
    const auto& requestRect = GetRequestRect();
    Rect newRect = { requestRect.posX_, requestRect.posY_, width, height }; // must keep w/h
    WLOGFI("[WMSCom] Id:%{public}d, state: %{public}d, type: %{public}d, mode: %{public}d, requestRect: "
        "[%{public}d, %{public}d, %{public}d, %{public}d], windowRect: [%{public}d, %{public}d, "
        "%{public}d, %{public}d], newRect: [%{public}d, %{public}d, %{public}d, %{public}d]",
        property_->GetPersistentId(), state_, GetType(), GetMode(), requestRect.posX_, requestRect.posY_,
        requestRect.width_, requestRect.height_, windowRect.posX_, windowRect.posY_,
        windowRect.width_, windowRect.height_, newRect.posX_, newRect.posY_,
        newRect.width_, newRect.height_);

    property_->SetRequestRect(newRect);

    WSRect wsRect = { newRect.posX_, newRect.posY_, newRect.width_, newRect.height_ };
    auto ret = hostSession_->UpdateSessionRect(wsRect, SizeChangeReason::RESIZE);
    return static_cast<WMError>(ret);
}

WMError WindowSceneSessionImpl::SetAspectRatio(float ratio)
{
    if (property_ == nullptr || hostSession_ == nullptr) {
        WLOGFE("SetAspectRatio failed because of nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (ratio == MathHelper::INF || ratio == MathHelper::NAG_INF || std::isnan(ratio) || MathHelper::NearZero(ratio)) {
        WLOGFE("SetAspectRatio failed, because of wrong value: %{public}f", ratio);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (hostSession_->SetAspectRatio(ratio) != WSError::WS_OK) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::ResetAspectRatio()
{
    if (!hostSession_) {
        WLOGFE("no host session found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return static_cast<WMError>(hostSession_->SetAspectRatio(0.0f));
}

WmErrorCode WindowSceneSessionImpl::RaiseToAppTop()
{
    WLOGFI("[WMSCom] id: %{public}d", GetPersistentId());
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
    if (!hostSession_) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    const WSError& ret = hostSession_->RaiseToAppTop();
    return static_cast<WmErrorCode>(ret);
}

WmErrorCode WindowSceneSessionImpl::RaiseAboveTarget(int32_t subWindowId)
{
    auto parentId = GetParentId();
    auto currentWindowId = GetWindowId();

    if (parentId == INVALID_SESSION_ID) {
        WLOGFE("Only the children of the main window can be raised!");
        return WmErrorCode::WM_ERROR_INVALID_PARENT;
    }

    auto subWindows = Window::GetSubWindow(parentId);
    auto targetWindow = find_if(subWindows.begin(), subWindows.end(), [subWindowId](sptr<Window>& window) {
        return static_cast<uint32_t>(subWindowId) == window->GetWindowId();
    });
    if (targetWindow == subWindows.end()) {
        return WmErrorCode::WM_ERROR_INVALID_PARAM;
    }

    if (!WindowHelper::IsSubWindow(GetType())) {
        WLOGFE("Must be app sub window window!");
        return WmErrorCode::WM_ERROR_INVALID_CALLING;
    }

    if ((state_ != WindowState::STATE_SHOWN) ||
        ((*targetWindow)->GetWindowState() != WindowState::STATE_SHOWN)) {
        WLOGFE("The sub window must be shown!");
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (!hostSession_) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    if (currentWindowId == static_cast<uint32_t>(subWindowId)) {
        return WmErrorCode::WM_OK;
    }
    WSError ret = hostSession_->RaiseAboveTarget(subWindowId);
    return WM_JS_TO_ERROR_CODE_MAP.at(static_cast<WMError>(ret));
}

WMError WindowSceneSessionImpl::GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea)
{
    uint32_t windowId = GetWindowId();
    WLOGFD("GetAvoidAreaByType windowId:%{public}u type:%{public}u", windowId, static_cast<uint32_t>(type));
    WindowMode mode = GetMode();
    if (type != AvoidAreaType::TYPE_KEYBOARD &&
        mode != WindowMode::WINDOW_MODE_FULLSCREEN &&
        mode != WindowMode::WINDOW_MODE_SPLIT_PRIMARY &&
        mode != WindowMode::WINDOW_MODE_SPLIT_SECONDARY &&
        !(mode == WindowMode::WINDOW_MODE_FLOATING &&
          system::GetParameter("const.product.devicetype", "unknown") == "phone")) {
        WLOGI("avoidAreaType:%{public}u, windowMode:%{public}u, return default avoid area.",
            static_cast<uint32_t>(type), static_cast<uint32_t>(mode));
        return WMError::WM_OK;
    }
    if (hostSession_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    avoidArea = hostSession_->GetAvoidAreaByType(type);
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::NotifyWindowNeedAvoid(bool status)
{
    WLOGFD("NotifyWindowNeedAvoid called windowId:%{public}u status:%{public}d",
        GetWindowId(), static_cast<int32_t>(status));
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (hostSession_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    hostSession_->OnNeedAvoid(status);
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetLayoutFullScreenByApiVersion(bool status)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    uint32_t version = 0;
    if ((context_ != nullptr) && (context_->GetApplicationInfo() != nullptr)) {
        version = context_->GetApplicationInfo()->apiCompatibleVersion;
    }
    isIgnoreSafeArea_ = status;
    // 10 ArkUI new framework support after API10
    if (version >= 10) {
        if (uiContent_ != nullptr) {
            uiContent_->SetIgnoreViewSafeArea(status);
        }
        isIgnoreSafeAreaNeedNotify_ = true;
    } else {
        WMError ret = WMError::WM_OK;
        if (status) {
            RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
            if (ret != WMError::WM_OK) {
                WLOGFE("RemoveWindowFlag errCode:%{public}d winId:%{public}u",
                    static_cast<int32_t>(ret), GetWindowId());
                return ret;
            }
        } else {
            AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
            if (ret != WMError::WM_OK) {
                WLOGFE("RemoveWindowFlag errCode:%{public}d winId:%{public}u",
                    static_cast<int32_t>(ret), GetWindowId());
                return ret;
            }
        }
        ret = NotifyWindowNeedAvoid(!status);
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
    if (hostSession_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!WindowHelper::IsWindowModeSupported(property_->GetModeSupportInfo(), WindowMode::WINDOW_MODE_FULLSCREEN)) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    WindowMode mode = GetMode();
    if (!((mode == WindowMode::WINDOW_MODE_FLOATING ||
            mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
            mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY)  &&
            WindowHelper::IsMainWindow(GetType()) &&
          system::GetParameter("const.product.devicetype", "unknown") == "phone")) {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_MAXIMIZE);
        SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    }

    WMError ret = SetLayoutFullScreenByApiVersion(status);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetLayoutFullScreenByApiVersion errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), GetWindowId());
    }
    return ret;
}

bool WindowSceneSessionImpl::IsLayoutFullScreen() const
{
    WindowMode mode = GetMode();
    return (mode == WindowMode::WINDOW_MODE_FULLSCREEN && isIgnoreSafeArea_);
}

SystemBarProperty WindowSceneSessionImpl::GetSystemBarPropertyByType(WindowType type) const
{
    WLOGFD("GetSystemBarPropertyByType windowId:%{public}u type:%{public}u",
        GetWindowId(), static_cast<uint32_t>(type));
    if (property_ == nullptr) {
        return SystemBarProperty();
    }
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
    if ((state_ == WindowState::STATE_CREATED &&
         property_->GetModeSupportInfo() != WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN) ||
         state_ == WindowState::STATE_HIDDEN) {
        return WMError::WM_OK;
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

    if (property_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    isSystembarPropertiesSet_ = true;
    property_->SetSystemBarProperty(type, property);
    WMError ret = NotifyWindowSessionProperty();
    if (ret != WMError::WM_OK) {
        WLOGFE("NotifyWindowSessionProperty winId:%{public}u errCode:%{public}d",
            GetWindowId(), static_cast<int32_t>(ret));
    }
    return ret;
}

WMError WindowSceneSessionImpl::SetFullScreen(bool status)
{
    WLOGFI("winId:%{public}u status:%{public}d", GetWindowId(), static_cast<int32_t>(status));
    if (hostSession_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!WindowHelper::IsWindowModeSupported(property_->GetModeSupportInfo(), WindowMode::WINDOW_MODE_FULLSCREEN)) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    WindowMode mode = GetMode();
    if (!((mode == WindowMode::WINDOW_MODE_FLOATING ||
           mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
           mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY)  &&
            WindowHelper::IsMainWindow(GetType()) &&
          system::GetParameter("const.product.devicetype", "unknown") == "phone")) {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_MAXIMIZE);
        SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    };

    WMError ret = SetLayoutFullScreenByApiVersion(status);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetLayoutFullScreenByApiVersion errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), GetWindowId());
    }
    SystemBarProperty statusProperty = GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    UpdateDecorEnable(true);
    statusProperty.enable_ = !status;
    ret = SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, statusProperty);
    if (ret != WMError::WM_OK) {
        WLOGFE("SetSystemBarProperty errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), GetWindowId());
    }
    return ret;
}

bool WindowSceneSessionImpl::IsFullScreen() const
{
    SystemBarProperty statusProperty = GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    return (IsLayoutFullScreen() && !statusProperty.enable_);
}

bool WindowSceneSessionImpl::IsDecorEnable() const
{
    if (GetMode() == WindowMode::WINDOW_MODE_FLOATING
        && system::GetParameter("const.product.devicetype", "unknown") == "phone") {
        /* FloatingWindow skip for Phone*/
        return false;
    }
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
    if (WindowHelper::IsMainWindow(GetType()) && hostSession_) {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_MINIMIZE);
    } else {
        WLOGFE("This window state is abnormal.");
        return WMError::WM_DO_NOTHING;
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
    if (!WindowHelper::IsWindowModeSupported(property_->GetModeSupportInfo(),
        WindowMode::WINDOW_MODE_FULLSCREEN)) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (GetGlobalMaximizeMode() != MaximizeMode::MODE_AVOID_SYSTEM_BAR) {
        SetFullScreen(true);
        property_->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    } else {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_MAXIMIZE_FLOATING);
        SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
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
    if (WindowHelper::IsMainWindow(GetType()) && hostSession_) {
        if (property_->GetMaximizeMode() == MaximizeMode::MODE_RECOVER &&
            property_->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING) {
            WLOGFW("Recover fail, already MODE_RECOVER");
            return WMError::WM_ERROR_REPEAT_OPERATION;
        }
        hostSession_->OnSessionEvent(SessionEvent::EVENT_RECOVER);
        SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        property_->SetMaximizeMode(MaximizeMode::MODE_RECOVER);
        UpdateDecorEnable(true);
        UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE);
    } else {
        WLOGFE("recovery is invalid on sub window");
        return WMError::WM_ERROR_INVALID_OPERATION;
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
    if ((WindowHelper::IsMainWindow(GetType()) || WindowHelper::IsPipWindow(GetType())) && hostSession_) {
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
        auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context_);
        if (!abilityContext) {
            return Destroy(true);
        }
        WindowPrepareTerminateHandler* handler = new(std::nothrow) WindowPrepareTerminateHandler();
        if (handler == nullptr) {
            WLOGFW("new WindowPrepareTerminateHandler failed, do close window");
            hostSession_->OnSessionEvent(SessionEvent::EVENT_CLOSE);
            return WMError::WM_OK;
        }
        wptr<ISession> hostSessionWptr = hostSession_;
        PrepareTerminateFunc func = [hostSessionWptr]() {
            auto weakSession = hostSessionWptr.promote();
            if (weakSession == nullptr) {
                WLOGFW("this session wptr is nullptr");
                return;
            }
            weakSession->OnSessionEvent(SessionEvent::EVENT_CLOSE);
        };
        handler->SetPrepareTerminateFun(func);
        sptr<AAFwk::IPrepareTerminateCallback> callback = handler;
        if (AAFwk::AbilityManagerClient::GetInstance()->PrepareTerminateAbility(abilityContext->GetToken(),
            callback) != ERR_OK) {
            WLOGFW("RegisterWindowManagerServiceHandler failed, do close window");
            hostSession_->OnSessionEvent(SessionEvent::EVENT_CLOSE);
            return WMError::WM_OK;
        }
    }

    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::DisableAppWindowDecor()
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
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
    } else if (uiContent_) {
        WLOGFD("Transfer back event to uiContent");
        isConsumed = uiContent_->ProcessBackPressed();
    } else {
        WLOGFE("There is no back event consumer");
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
    wptr<WindowSceneSessionImpl> weak = this;
    auto task = [weak]() {
        auto weakSession = weak.promote();
        if (weakSession == nullptr) {
            WLOGFE("HandleBackEvent session wptr is nullptr");
            return;
        }
        weakSession->PerformBack();
    };
    if (!handler_->PostTask(task)) {
        WLOGFE("Failed to post PerformBack");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    return WSError::WS_OK;
}

void WindowSceneSessionImpl::PerformBack()
{
    if (!WindowHelper::IsMainWindow(GetType())) {
        WLOGFI("PerformBack is not MainWindow, return");
        return;
    }
    if (hostSession_) {
        bool needMoveToBackground = false;
        auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context_);
        if (abilityContext != nullptr) {
            abilityContext->OnBackPressedCallBack(needMoveToBackground);
        }
        WLOGFI("Transfer back event to host session needMoveToBackground %{public}d", needMoveToBackground);
        hostSession_->RequestSessionBack(needMoveToBackground);
    }
}

WMError WindowSceneSessionImpl::SetGlobalMaximizeMode(MaximizeMode mode)
{
    WLOGFD("WindowSceneSessionImpl::SetGlobalMaximizeMode %{public}u", static_cast<uint32_t>(mode));
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(GetType()) && hostSession_) {
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
    if (!WindowHelper::IsWindowModeSupported(property_->GetModeSupportInfo(),
        WindowMode::WINDOW_MODE_FULLSCREEN)) {
        return mode;
    }
    if (hostSession_) {
        hostSession_->GetGlobalMaximizeMode(mode);
    }
    return mode;
}

WMError WindowSceneSessionImpl::SetWindowMode(WindowMode mode)
{
    WLOGFD("SetWindowMode %{public}u mode %{public}u", GetWindowId(), static_cast<uint32_t>(mode));
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!WindowHelper::IsWindowModeSupported(property_->GetModeSupportInfo(), mode)) {
        WLOGFE("window %{public}u do not support mode: %{public}u",
            GetWindowId(), static_cast<uint32_t>(mode));
        return WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE;
    }
    WMError ret = UpdateWindowModeImmediately(mode);
    if (ret != WMError::WM_OK) {
        return ret;
    }

    if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_SPLIT_PRIMARY);
    } else if (mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_SPLIT_SECONDARY);
    }
    return WMError::WM_OK;
}

WindowMode WindowSceneSessionImpl::GetMode() const
{
    return property_->GetWindowMode();
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
    if (flag == WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED && context_ && context_->GetApplicationInfo() &&
        context_->GetApplicationInfo()->apiCompatibleVersion >= 9 && // 9: api version
        !SessionPermission::IsSystemCalling()) {
        WLOGI("Can not add window flag WINDOW_FLAG_SHOW_WHEN_LOCKED");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
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
    std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
    for (const auto& winPair : windowSessionMap_) {
        auto window = winPair.second.second;
        window->UpdateConfiguration(configuration);
    }
}

sptr<Window> WindowSceneSessionImpl::GetTopWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context)
{
    std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
    if (windowSessionMap_.empty()) {
        WLOGFE("[GetTopWin] Please create mainWindow First!");
        return nullptr;
    }
    uint32_t mainWinId = INVALID_WINDOW_ID;
    for (const auto& winPair : windowSessionMap_) {
        auto win = winPair.second.second;
        if (win && WindowHelper::IsMainWindow(win->GetType()) && context.get() == win->GetContext().get()) {
            mainWinId = win->GetWindowId();
            WLOGI("[GetTopWin] Find MainWinId:%{public}u.", mainWinId);
            break;
        }
    }
    WLOGFI("[GetTopWin] mainId: %{public}u!", mainWinId);
    if (mainWinId == INVALID_WINDOW_ID) {
        WLOGFE("[GetTopWin] Cannot find topWindow!");
        return nullptr;
    }
    uint32_t topWinId = INVALID_WINDOW_ID;
    WMError ret = SingletonContainer::Get<WindowAdapter>().GetTopWindowId(mainWinId, topWinId);
    if (ret != WMError::WM_OK) {
        WLOGFE("[GetTopWin] failed with errCode:%{public}d", static_cast<int32_t>(ret));
        return nullptr;
    }
    return FindWindowById(topWinId);
}

sptr<Window> WindowSceneSessionImpl::GetTopWindowWithId(uint32_t mainWinId)
{
    WLOGFI("[GetTopWin] mainId: %{public}u!", mainWinId);
    uint32_t topWinId = INVALID_WINDOW_ID;
    WMError ret = SingletonContainer::Get<WindowAdapter>().GetTopWindowId(mainWinId, topWinId);
    if (ret != WMError::WM_OK) {
        WLOGFE("[GetTopWin] failed with errCode:%{public}d", static_cast<int32_t>(ret));
        return nullptr;
    }
    return FindWindowById(topWinId);
}

sptr<WindowSessionImpl> WindowSceneSessionImpl::GetMainWindowWithId(uint32_t mainWinId)
{
    std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
    if (windowSessionMap_.empty()) {
        WLOGFE("Please create mainWindow First!");
        return nullptr;
    }
    for (const auto& winPair : windowSessionMap_) {
        auto win = winPair.second.second;
        if (win && WindowHelper::IsMainWindow(win->GetType()) && mainWinId == win->GetWindowId()) {
            WLOGI("GetTopWindow Find MainWinId:%{public}u.", mainWinId);
            return win;
        }
    }
    WLOGFE("Cannot find Window!");
    return nullptr;
}

void WindowSceneSessionImpl::SetNeedDefaultAnimation(bool needDefaultAnimation)
{
    enableDefaultAnimation_= needDefaultAnimation;
    hostSession_->UpdateWindowAnimationFlag(needDefaultAnimation);
    return;
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

    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("Check failed, permission denied");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }

    return WMError::WM_OK;
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
        auto display = SingletonContainer::IsDestroyed() ? nullptr :
            SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
        if (display == nullptr) {
            WLOGFE("get display failed displayId:%{public}" PRIu64"", property_->GetDisplayId());
            return WMError::WM_ERROR_INVALID_PARAM;
        }
        surfaceNode_->SetBackgroundFilter(RSFilter::CreateMaterialFilter(static_cast<int>(blurStyle),
                                                                         display->GetVirtualPixelRatio()));
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
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_SYSTEM_PRIVACY_MODE);
}

WMError WindowSceneSessionImpl::SetSnapshotSkip(bool isSkip)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("set snapshot skip permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    surfaceNode_->SetSkipLayer(isSkip || property_->GetSystemPrivacyMode());
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

std::shared_ptr<Media::PixelMap> WindowSceneSessionImpl::Snapshot()
{
    std::shared_ptr<SurfaceCaptureFuture> callback = std::make_shared<SurfaceCaptureFuture>();
    auto isSucceeded = RSInterfaces::GetInstance().TakeSurfaceCapture(surfaceNode_, callback);
    std::shared_ptr<Media::PixelMap> pixelMap;
    if (!isSucceeded) {
        WLOGFE("Failed to TakeSurfaceCapture!");
        return nullptr;
    }
    pixelMap = callback->GetResult(2000); // wait for <= 2000ms
    if (pixelMap != nullptr) {
        WLOGFD("Snapshot succeed, save WxH = %{public}dx%{public}d", pixelMap->GetWidth(), pixelMap->GetHeight());
    } else {
        WLOGFE("Failed to get pixelmap, return nullptr!");
    }
    return pixelMap;
}

WMError WindowSceneSessionImpl::NotifyMemoryLevel(int32_t level)
{
    WLOGFD("id: %{public}u, notify memory level: %{public}d", GetWindowId(), level);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (uiContent_ == nullptr) {
        WLOGFE("Window %{public}s notify memory level failed, ace is null.", GetWindowName().c_str());
        return WMError::WM_ERROR_NULLPTR;
    }
    // notify memory level
    uiContent_->NotifyMemoryLevel(level);
    WLOGFD("WindowSceneSessionImpl::NotifyMemoryLevel End!");
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetTurnScreenOn(bool turnScreenOn)
{
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetTurnScreenOn(turnScreenOn);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON);
}

bool WindowSceneSessionImpl::IsTurnScreenOn() const
{
    return property_->IsTurnScreenOn();
}

WMError WindowSceneSessionImpl::SetKeepScreenOn(bool keepScreenOn)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetKeepScreenOn(keepScreenOn);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON);
}

bool WindowSceneSessionImpl::IsKeepScreenOn() const
{
    return property_->IsKeepScreenOn();
}

WMError WindowSceneSessionImpl::SetTransform(const Transform& trans)
{
    WLOGFI("property_ persistentId: %{public}d", property_->GetPersistentId());
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    Transform oriTrans = property_->GetTransform();
    property_->SetTransform(trans);
    TransformSurfaceNode(trans);
    return WMError::WM_OK;
}

const Transform& WindowSceneSessionImpl::GetTransform() const
{
    return property_->GetTransform();
}

void WindowSceneSessionImpl::TransformSurfaceNode(const Transform& trans)
{
    if (surfaceNode_ == nullptr) {
        return;
    }
    surfaceNode_->SetPivotX(trans.pivotX_);
    surfaceNode_->SetPivotY(trans.pivotY_);
    surfaceNode_->SetScaleX(trans.scaleX_);
    surfaceNode_->SetScaleY(trans.scaleY_);
    surfaceNode_->SetTranslateX(trans.translateX_);
    surfaceNode_->SetTranslateY(trans.translateY_);
    surfaceNode_->SetTranslateZ(trans.translateZ_);
    surfaceNode_->SetRotationX(trans.rotationX_);
    surfaceNode_->SetRotationY(trans.rotationY_);
    surfaceNode_->SetRotation(trans.rotationZ_);
    uint32_t animationFlag = property_->GetAnimationFlag();
    if (animationFlag != static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
        RSTransaction::FlushImplicitTransaction();
    }
}

WMError WindowSceneSessionImpl::RegisterAnimationTransitionController(
    const sptr<IAnimationTransitionController>& listener)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("register animation transition controller permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    animationTransitionController_ = listener;
    wptr<WindowSessionProperty> propertyToken(property_);
    wptr<IAnimationTransitionController> animationTransitionControllerToken(animationTransitionController_);
    if (uiContent_) {
        uiContent_->SetNextFrameLayoutCallback([propertyToken, animationTransitionControllerToken]() {
            auto property = propertyToken.promote();
            auto animationTransitionController = animationTransitionControllerToken.promote();
            if (!property || !animationTransitionController) {
                return;
            }
            uint32_t animationFlag = property->GetAnimationFlag();
            if (animationFlag == static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
                // CustomAnimation is enabled when animationTransitionController_ exists
                animationTransitionController->AnimationForShown();
            }
            WLOGFI("AnimationForShown excute sucess  %{public}d!", property->GetPersistentId());
        });
    }
    WLOGI("RegisterAnimationTransitionController %{public}d!", property_->GetPersistentId());
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::UpdateSurfaceNodeAfterCustomAnimation(bool isAdd)
{
    WLOGFI("id: %{public}d , isAdd:%{public}u", property_->GetPersistentId(), isAdd);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!WindowHelper::IsSystemWindow(property_->GetWindowType())) {
        WLOGFE("only system window can set");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    // set no custom after customAnimation
    WMError ret = UpdateAnimationFlagProperty(false);
    if (ret != WMError::WM_OK) {
        WLOGFE("UpdateAnimationFlagProperty failed!");
        return ret;
    }
    ret = static_cast<WMError>(hostSession_->UpdateWindowSceneAfterCustomAnimation(isAdd));
    return ret;
}

void WindowSceneSessionImpl::AdjustWindowAnimationFlag(bool withAnimation)
{
    if (IsWindowSessionInvalid()) {
        WLOGE("AdjustWindowAnimationFlag failed since session invalid!");
        return;
    }
    // when show/hide with animation
    // use custom animation when transitionController exists; else use default animation
    WindowType winType = property_->GetWindowType();
    bool isAppWindow = WindowHelper::IsAppWindow(winType);
    if (withAnimation && !isAppWindow && animationTransitionController_) {
        // use custom animation
        property_->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
    } else if ((isAppWindow && enableDefaultAnimation_) || (withAnimation && !animationTransitionController_)) {
        // use default animation
        property_->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::DEFAULT));
    } else {
        // with no animation
        property_->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::NONE));
    }
}

WMError WindowSceneSessionImpl::UpdateAnimationFlagProperty(bool withAnimation)
{
    if (!WindowHelper::IsSystemWindow(GetType())) {
        return WMError::WM_OK;
    }
    AdjustWindowAnimationFlag(withAnimation);
    // when show(true) with default, hide() with None, to adjust animationFlag to disabled default animation
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG);
}

WMError WindowSceneSessionImpl::SetAlpha(float alpha)
{
    WLOGI("Window %{public}d alpha %{public}f", property_->GetPersistentId(), alpha);
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("set alpha permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    surfaceNode_->SetAlpha(alpha);
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::BindDialogTarget(sptr<IRemoteObject> targetToken)
{
    auto persistentId = property_->GetPersistentId();
    WMError ret = SingletonContainer::Get<WindowAdapter>().BindDialogSessionTarget(persistentId, targetToken);
    if (ret != WMError::WM_OK) {
        WLOGFE("bind window failed with errCode:%{public}d", static_cast<int32_t>(ret));
    }
    return ret;
}

WMError WindowSceneSessionImpl::SetTouchHotAreas(const std::vector<Rect>& rects)
{
    if (property_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    std::vector<Rect> lastTouchHotAreas;
    property_->GetTouchHotAreas(lastTouchHotAreas);
    property_->SetTouchHotAreas(rects);
    WMError result = UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA);
    if (result != WMError::WM_OK) {
        property_->SetTouchHotAreas(lastTouchHotAreas);
        WLOGFE("SetTouchHotAreas with errCode:%{public}d", static_cast<int32_t>(result));
        return result;
    }
    for (uint32_t i = 0; i < rects.size(); i++) {
        WLOGFI("Set areas: %{public}u [x: %{public}d y:%{public}d w:%{public}u h:%{public}u]",
            i, rects[i].posX_, rects[i].posY_, rects[i].width_, rects[i].height_);
    }
    return result;
}

WMError WindowSceneSessionImpl::SetNeedKeepKeyboard(bool isNeedKeepKeyboard)
{
    if (property_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    property_->SetNeedKeepKeyboard(isNeedKeepKeyboard);

    return WMError::WM_OK;
}

void WindowSceneSessionImpl::DumpSessionElementInfo(const std::vector<std::string>& params)
{
    WLOGFD("DumpSessionElementInfo");
    std::vector<std::string> info;
    if (params.size() == 1 && params[0] == PARAM_DUMP_HELP) { // 1: params num
        WLOGFD("Dump ArkUI help Info");
        Ace::UIContent::ShowDumpHelp(info);
        SingletonContainer::Get<WindowAdapter>().NotifyDumpInfoResult(info);
        return;
    }
    WLOGFD("ArkUI:DumpInfo");
    if (uiContent_ != nullptr) {
        uiContent_->DumpInfo(params, info);
    }

    for (auto iter = info.begin(); iter != info.end();) {
        if ((*iter).size() == 0) {
            iter = info.erase(iter);
            continue;
        }
        WLOGFD("ElementInfo size: %{public}u", static_cast<uint32_t>((*iter).size()));
        iter++;
    }
    if (info.size() == 0) {
        WLOGFD("ElementInfo is empty");
        return;
    }
    SingletonContainer::Get<WindowAdapter>().NotifyDumpInfoResult(info);
}

WSError WindowSceneSessionImpl::UpdateWindowMode(WindowMode mode)
{
    WLOGFI("UpdateWindowMode %{public}u mode %{public}u", GetWindowId(), static_cast<uint32_t>(mode));
    if (IsWindowSessionInvalid()) {
        return WSError::WS_ERROR_INVALID_WINDOW;
    }
    if (!WindowHelper::IsWindowModeSupported(property_->GetModeSupportInfo(), mode)) {
        WLOGFE("window %{public}u do not support mode: %{public}u",
            GetWindowId(), static_cast<uint32_t>(mode));
        return WSError::WS_ERROR_INVALID_WINDOW_MODE_OR_SIZE;
    }
    WMError ret = UpdateWindowModeImmediately(mode);

    if (mode == WindowMode::WINDOW_MODE_FULLSCREEN
        && system::GetParameter("const.product.devicetype", "unknown") == "pc") {
        SystemBarProperty statusProperty = GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
        statusProperty.enable_ = false;
        ret = SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, statusProperty);
        if (ret != WMError::WM_OK) {
            WLOGFE("SetSystemBarProperty errCode:%{public}d winId:%{public}u",
                static_cast<int32_t>(ret), GetWindowId());
        }
    }
    return static_cast<WSError>(ret);
}

WMError WindowSceneSessionImpl::RecoveryPullPiPMainWindow(const Rect& rect)
{
    WLOGFI("RecoveryPullPiPMainWindow");
    if (hostSession_ && property_->GetWindowType() == WindowType::WINDOW_TYPE_PIP) {
        hostSession_->RecoveryPullPiPMainWindow(GetPersistentId(), rect);
        return WMError::WM_OK;
    }
    WLOGFW("not pip window, nothing to do");
    return WMError::WM_DO_NOTHING;
}

WMError WindowSceneSessionImpl::UpdateWindowModeImmediately(WindowMode mode)
{
    if (state_ == WindowState::STATE_CREATED || state_ == WindowState::STATE_HIDDEN) {
        property_->SetWindowMode(mode);
        UpdateTitleButtonVisibility();
        UpdateDecorEnable(true);
    } else if (state_ == WindowState::STATE_SHOWN) {
        WMError ret = UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_MODE);
        if (ret != WMError::WM_OK) {
            WLOGFE("update window mode filed! id: %{public}u, mode: %{public}u.", GetWindowId(),
                static_cast<uint32_t>(mode));
            return ret;
        }
        // set client window mode if success.
        property_->SetWindowMode(mode);
        UpdateTitleButtonVisibility();
        UpdateDecorEnable(true);
        if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
            property_->SetMaximizeMode(MaximizeMode::MODE_RECOVER);
        }
    }
    return WMError::WM_OK;
}

WSError WindowSceneSessionImpl::UpdateMaximizeMode(MaximizeMode mode)
{
    WLOGFI("UpdateMaximizeMode %{public}u mode %{public}u", GetWindowId(), static_cast<uint32_t>(mode));
    if (uiContent_ == nullptr) {
        WLOGFE("UpdateMaximizeMode uiContent_ is null");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    uiContent_->UpdateMaximizeMode(mode);
    return WSError::WS_OK;
}

void WindowSceneSessionImpl::NotifySessionForeground(uint32_t reason, bool withAnimation)
{
    WLOGFI("NotifySessionForeground");
    Show(reason, withAnimation);
}

void WindowSceneSessionImpl::NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits)
{
    WLOGFI("NotifySessionBackground");
    Hide(reason, withAnimation, isFromInnerkits);
}

WSError WindowSceneSessionImpl::UpdateTitleInTargetPos(bool isShow, int32_t height)
{
    WLOGFI("UpdateTitleInTargetPos %{public}u isShow %{public}u, height %{public}u", GetWindowId(), isShow, height);
    if (IsWindowSessionInvalid()) {
        return WSError::WS_ERROR_INVALID_WINDOW;
    }
    if (uiContent_ == nullptr) {
        WLOGFE("UpdateTitleInTargetPos uiContent_ is null");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    uiContent_->UpdateTitleInTargetPos(isShow, height);
    return WSError::WS_OK;
}

WMError WindowSceneSessionImpl::NotifyPrepareClosePiPWindow()
{
    if (!WindowHelper::IsPipWindow(GetType())) {
        return WMError::WM_DO_NOTHING;
    }
    WLOGFD("NotifyPrepareClosePiPWindow start");
    hostSession_->NotifyPiPWindowPrepareClose();
    WLOGFD("NotifyPrepareClosePiPWindow end");
    return WMError::WM_OK;
}

void WindowSceneSessionImpl::GetWindowDrawingContentChangeInfo(WindowDrawingContentInfo info)
{
    WindowDrawingContentInfo windowDrawingInfo(info);
    uint32_t windowId = info.windowId_;
    bool currentDrawingContentState = info.drawingContentState_;
    bool currentProcessContentState = lastProcessContentState_;
    if (currentDrawingContentState) {
        std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
        if (windowSessionMap_.empty()) {
            WLOGFE("Please create mainWindow First!");
            return;
        }
        for (auto& item : windowSessionMap_) {
            if (item.second.second->GetWindowId() == windowId) {
                item.second.second->SetDrawingContentState(currentDrawingContentState);
                break;
            }
        }
        currentProcessContentState = true;
    } else {
        std::unique_lock<std::shared_mutex> lock(windowSessionMutex_);
        if (windowSessionMap_.empty()) {
            WLOGFE("Please create mainWindow First!");
            return;
        }
        for (auto& item : windowSessionMap_) {
            WLOGFD("Show all WindowDrawingContentInfo:pid%{public}d, windowId%{public}d,"
                "currentDrawingContentState%{public}d", windowDrawingInfo.pid_, windowDrawingInfo.windowId_,
                currentDrawingContentState);
            if (item.second.second->GetWindowId() == windowId) {
                item.second.second->SetDrawingContentState(currentDrawingContentState);
                continue;
            }
            if (item.second.second->GetDrawingContentState()) {
                currentProcessContentState = true;
                break;
            }
            currentProcessContentState = false;
        }
    }
    WLOGFD("compare processContentState:pid%{public}d, currentProcessContentState%{public}d,"
        "lastProcessContentState%{public}d", windowDrawingInfo.pid_, currentProcessContentState,
        lastProcessContentState_);
    if (lastProcessContentState_ != currentProcessContentState) {
        windowDrawingInfo.drawingContentState_ = currentProcessContentState;
        SingletonContainer::Get<WindowManager>().UpdateWindowDrawingContentInfo(windowDrawingInfo);
        lastProcessContentState_ = currentProcessContentState;
    }
    return;
}

void WindowSceneSessionImpl::UpdateWindowDrawingContentInfo(const WindowDrawingContentInfo& info)
{
    WLOGFD("UpdateWindowDrawingContentInfo:pid%{public}d, windowId:%{public}u, drawingContentState:%{public}d",
        info.pid_, info.windowId_, info.drawingContentState_);
    GetWindowDrawingContentChangeInfo(info);
}

WMError WindowSceneSessionImpl::GetWindowLimits(WindowLimits& windowLimits)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (property_ == nullptr) {
        WLOGFE("GetWindowLimits property_ is null, WinId:%{public}u", GetWindowId());
        return WMError::WM_ERROR_NULLPTR;
    }
    const auto& customizedLimits = property_->GetWindowLimits();
    windowLimits.minWidth_ = customizedLimits.minWidth_;
    windowLimits.minHeight_ = customizedLimits.minHeight_;
    windowLimits.maxWidth_ = customizedLimits.maxWidth_;
    windowLimits.maxHeight_ = customizedLimits.maxHeight_;
    WLOGFI("GetWindowLimits WinId:%{public}u, minWidth:%{public}u, minHeight:%{public}u"
        "maxWidth:%{public}u, maxHeight:%{public}u", GetWindowId(), windowLimits.minWidth_,
        windowLimits.minHeight_, windowLimits.maxWidth_, windowLimits.maxHeight_);
    return WMError::WM_OK;
}

void WindowSceneSessionImpl::UpdateNewSize()
{
    bool needResize = false;
    const Rect& windowRect = GetRect();
    uint32_t width = windowRect.width_;
    uint32_t height = windowRect.height_;
    const auto& newLimits = property_->GetWindowLimits();
    if (width < newLimits.minWidth_) {
        width = newLimits.minWidth_;
        needResize = true;
    }
    if (height < newLimits.minHeight_) {
        height = newLimits.minHeight_;
        needResize = true;
    }
    if (width > newLimits.maxWidth_) {
        width = newLimits.maxWidth_;
        needResize = true;
    }
    if (height > newLimits.maxHeight_) {
        height = newLimits.maxHeight_;
        needResize = true;
    }
    if (needResize) {
        Resize(width, height);
    }
}

WMError WindowSceneSessionImpl::SetWindowLimits(WindowLimits& windowLimits)
{
    WLOGFI("SetWindowLimits WinId:%{public}u, minWidth:%{public}u, minHeight:%{public}u"
        "maxWidth:%{public}u, maxHeight:%{public}u", GetWindowId(), windowLimits.minWidth_,
        windowLimits.minHeight_, windowLimits.maxWidth_, windowLimits.maxHeight_);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    WindowType windowType = GetType();
    if (!WindowHelper::IsMainWindow(windowType)
        && !WindowHelper::IsSubWindow(windowType)
        && windowType != WindowType::WINDOW_TYPE_DIALOG) {
        WLOGFE("windowType not support");
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    if (property_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr || display->GetDisplayInfo() == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    float vpr = display->GetDisplayInfo()->GetVirtualPixelRatio();
    if (MathHelper::NearZero(vpr)) {
        WLOGFE("SetWindowLimits failed, because of wrong vpr: %{public}f", vpr);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    const auto& customizedLimits = property_->GetWindowLimits();
    uint32_t minWidth = windowLimits.minWidth_ ? windowLimits.minWidth_ : customizedLimits.minWidth_;
    uint32_t minHeight = windowLimits.minHeight_ ? windowLimits.minHeight_ : customizedLimits.minHeight_;
    uint32_t maxWidth = windowLimits.maxWidth_ ? windowLimits.maxWidth_ : customizedLimits.maxWidth_;
    uint32_t maxHeight = windowLimits.maxHeight_ ? windowLimits.maxHeight_ : customizedLimits.maxHeight_;
    // px to vp
    minWidth = static_cast<uint32_t>(ceil(minWidth / vpr));
    minHeight = static_cast<uint32_t>(ceil(minHeight / vpr));
    maxWidth = static_cast<uint32_t>(ceil(maxWidth / vpr));
    maxHeight = static_cast<uint32_t>(ceil(maxHeight / vpr));

    property_->SetWindowLimits({
        maxWidth, maxHeight, minWidth, minHeight, customizedLimits.maxRatio_, customizedLimits.minRatio_
    });
    UpdateWindowSizeLimits();
    WMError ret = UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS);
    if (ret != WMError::WM_OK) {
        WLOGFE("update window proeprty failed! id: %{public}u.", GetWindowId());
        return ret;
    }
    UpdateNewSize();

    const auto& newLimits = property_->GetWindowLimits();
    windowLimits.minWidth_ = newLimits.minWidth_;
    windowLimits.minHeight_ = newLimits.minHeight_;
    windowLimits.maxWidth_ = newLimits.maxWidth_;
    windowLimits.maxHeight_ = newLimits.maxHeight_;
    WLOGFI("SetWindowLimits success! WinId:%{public}u, minWidth:%{public}u, minHeight:%{public}u"
        "maxWidth:%{public}u, maxHeight:%{public}u", GetWindowId(), windowLimits.minWidth_,
        windowLimits.minHeight_, windowLimits.maxWidth_, windowLimits.maxHeight_);
    return WMError::WM_OK;
}
} // namespace Rosen
} // namespace OHOS
