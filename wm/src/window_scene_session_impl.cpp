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

#include <chrono>
#include <limits>
#include <ability_manager_client.h>
#include <parameters.h>
#include <transaction/rs_transaction.h>
#include <hitrace_meter.h>

#include "anr_handler.h"
#include "color_parser.h"
#include "display_info.h"
#include "singleton_container.h"
#include "display_manager.h"
#include "display_manager_adapter.h"
#include "input_transfer_station.h"
#include "perform_reporter.h"
#include "session_helper.h"
#include "session_permission.h"
#include "session/container/include/window_event_channel.h"
#include "session_manager/include/session_manager.h"
#include "window_adapter.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "window_prepare_terminate.h"
#include "wm_common.h"
#include "wm_common_inner.h"
#include "wm_math.h"
#include "session_manager_agent_controller.h"
#include <transaction/rs_interfaces.h>
#include "surface_capture_future.h"
#include "pattern_detach_callback.h"
#include "window_session_impl.h"
#include "sys_cap_util.h"

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
constexpr int32_t WINDOW_DETACH_TIMEOUT = 300;
const std::string PARAM_DUMP_HELP = "-h";
constexpr float MIN_GRAY_SCALE = 0.0f;
constexpr float MAX_GRAY_SCALE = 1.0f;
}
uint32_t WindowSceneSessionImpl::maxFloatingWindowSize_ = 1920;
std::mutex WindowSceneSessionImpl::keyboardPanelInfoChangeListenerMutex_;

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
        type == WindowType::WINDOW_TYPE_PIP || type == WindowType::WINDOW_TYPE_GLOBAL_SEARCH ||
        type == WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW || type == WindowType::WINDOW_TYPE_HANDWRITE)) {
        TLOGI(WmsLogTag::WMS_SYSTEM, "Invalid type: %{public}u", type);
        return false;
    }
    TLOGI(WmsLogTag::WMS_SYSTEM, "Valid type: %{public}u", type);
    return true;
}

sptr<WindowSessionImpl> WindowSceneSessionImpl::FindParentSessionByParentId(uint32_t parentId)
{
    std::shared_lock<std::shared_mutex> lock(windowSessionMutex_);
    for (const auto& item : windowSessionMap_) {
        if (item.second.second && item.second.second->GetProperty() &&
            item.second.second->GetWindowId() == parentId) {
                if (WindowHelper::IsMainWindow(item.second.second->GetType()) ||
                    WindowHelper::IsSystemWindow(item.second.second->GetType())) {
                    WLOGFD("Find parent, [parentName: %{public}s, parentId:%{public}u, selfPersistentId: %{public}d]",
                        item.second.second->GetProperty()->GetWindowName().c_str(), parentId,
                        GetProperty()->GetPersistentId());
                    return item.second.second;
                } else if (WindowHelper::IsSubWindow(item.second.second->GetType()) &&
                    isSessionMainWindow(item.second.second->GetParentId())) {
                    // subwindow grandparent is mainwindow
                    return item.second.second;
                }
        }
    }
    WLOGFD("[WMSCom] Can not find parent window, id: %{public}d", parentId);
    return nullptr;
}

bool WindowSceneSessionImpl::isSessionMainWindow(uint32_t parentId)
{
    std::shared_lock<std::shared_mutex> lock(windowSessionMutex_);
    for (const auto& item : windowSessionMap_) {
        if (item.second.second && item.second.second->GetProperty() &&
            item.second.second->GetWindowId() == parentId &&
            WindowHelper::IsMainWindow(item.second.second->GetType())) {
                return true;
        }
    }
    return false;
}

sptr<WindowSessionImpl> WindowSceneSessionImpl::FindMainWindowWithContext()
{
    std::shared_lock<std::shared_mutex> lock(windowSessionMutex_);
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
        TLOGE(WmsLogTag::WMS_LIFE, "inputChannel or property is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    sptr<IWindowEventChannel> eventChannel(channel);
    auto persistentId = INVALID_SESSION_ID;
    sptr<Rosen::ISession> session;
    sptr<IRemoteObject> token = context_ ? context_->GetToken() : nullptr;
    if (token) {
        property_->SetTokenState(true);
    }
    const WindowType& type = GetType();
    auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context_);
    if (property_ && abilityContext && abilityContext->GetAbilityInfo()) {
        auto info = property_->GetSessionInfo();
        info.abilityName_ = abilityContext->GetAbilityInfo()->name;
        info.moduleName_ = context_->GetHapModuleInfo()->moduleName;
        info.bundleName_ = abilityContext->GetAbilityInfo()->bundleName;
        property_->SetSessionInfo(info);
    }
    if (WindowHelper::IsSubWindow(type) && (property_->GetExtensionFlag() == false)) { // sub window
        auto parentSession = FindParentSessionByParentId(property_->GetParentId());
        if (parentSession == nullptr || parentSession->GetHostSession() == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "parent of sub window is nullptr, name: %{public}s, type: %{public}d",
                property_->GetWindowName().c_str(), type);
            return WMError::WM_ERROR_NULLPTR;
        }
        // set parent persistentId
        property_->SetParentPersistentId(parentSession->GetPersistentId());
        // creat sub session by parent session
        SingletonContainer::Get<WindowAdapter>().CreateAndConnectSpecificSession(iSessionStage, eventChannel,
            surfaceNode_, property_, persistentId, session, windowSystemConfig_, token);
        // update subWindowSessionMap_
        subWindowSessionMap_[parentSession->GetPersistentId()].push_back(this);
    } else if (property_->GetExtensionFlag() == true) {
        property_->SetParentPersistentId(property_->GetParentId());
        // creat sub session by parent session
        SingletonContainer::Get<WindowAdapter>().CreateAndConnectSpecificSession(iSessionStage, eventChannel,
            surfaceNode_, property_, persistentId, session, windowSystemConfig_, token);
    } else { // system window
        WMError createSystemWindowRet = CreateSystemWindow(type);
        if (createSystemWindowRet != WMError::WM_OK) {
            return createSystemWindowRet;
        }
        PreProcessCreate();
        SingletonContainer::Get<WindowAdapter>().CreateAndConnectSpecificSession(iSessionStage, eventChannel,
            surfaceNode_, property_, persistentId, session, windowSystemConfig_, token);
        if (windowSystemConfig_.maxFloatingWindowSize_ != UINT32_MAX) {
            maxFloatingWindowSize_ = windowSystemConfig_.maxFloatingWindowSize_;
        }
    }
    property_->SetPersistentId(persistentId);
    if (session == nullptr) {
        TLOGI(WmsLogTag::WMS_LIFE, "create specific failed, session is nullptr, name: %{public}s",
            property_->GetWindowName().c_str());
        return WMError::WM_ERROR_NULLPTR;
    }
    hostSession_ = session;
    TLOGI(WmsLogTag::WMS_LIFE, "CreateAndConnectSpecificSession [name:%{public}s, id:%{public}d, parentId: %{public}d, "
        "type: %{public}u]", property_->GetWindowName().c_str(), property_->GetPersistentId(),
        property_->GetParentPersistentId(), GetType());
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::CreateSystemWindow(WindowType type)
{
    if (WindowHelper::IsAppFloatingWindow(type) || WindowHelper::IsPipWindow(type) ||
        (type == WindowType::WINDOW_TYPE_TOAST)) {
        property_->SetParentPersistentId(GetFloatingWindowParentId());
        TLOGI(WmsLogTag::WMS_SYSTEM, "set parentId: %{public}d, type: %{public}d",
            property_->GetParentPersistentId(), type);
        auto mainWindow = FindMainWindowWithContext();
        property_->SetFloatingWindowAppType(mainWindow != nullptr ? true : false);
    } else if (type == WindowType::WINDOW_TYPE_DIALOG) {
        auto mainWindow = FindMainWindowWithContext();
        if (mainWindow != nullptr) {
            property_->SetParentPersistentId(mainWindow->GetPersistentId());
            TLOGI(WmsLogTag::WMS_DIALOG, "Set parentId, parentId:%{public}d", mainWindow->GetPersistentId());
        }
    } else if (WindowHelper::IsSystemSubWindow(type)) {
        auto parentSession = FindParentSessionByParentId(property_->GetParentId());
        if (parentSession == nullptr || parentSession->GetHostSession() == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "parent of system sub window, name: %{public}s, type: %{public}d",
                property_->GetWindowName().c_str(), type);
            return WMError::WM_ERROR_NULLPTR;
        }
        if (WindowHelper::IsSystemSubWindow(parentSession->GetType())) {
            TLOGE(WmsLogTag::WMS_LIFE, "parent is system sub window, name: %{public}s, type: %{public}d",
                property_->GetWindowName().c_str(), type);
            return WMError::WM_ERROR_INVALID_TYPE;
        }
        // set parent persistentId
        property_->SetParentPersistentId(parentSession->GetPersistentId());
    }
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::RecoverAndConnectSpecificSession()
{
    if (property_ == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "property_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto persistentId = property_->GetPersistentId();

    TLOGI(WmsLogTag::WMS_RECOVER, "windowName = %{public}s, windowMode = %{public}u, "
        "windowType = %{public}u, persistentId = %{public}d, windowState = %{public}d", GetWindowName().c_str(),
        property_->GetWindowMode(), property_->GetWindowType(), persistentId, state_);

    property_->SetWindowState(state_);

    sptr<ISessionStage> iSessionStage(this);
    sptr<WindowEventChannel> channel = new (std::nothrow) WindowEventChannel(iSessionStage);
    sptr<IWindowEventChannel> eventChannel(channel);
    sptr<Rosen::ISession> session;
    sptr<IRemoteObject> token = context_ ? context_->GetToken() : nullptr;
    if (token) {
        property_->SetTokenState(true);
    }

    const WindowType type = GetType();
    if (WindowHelper::IsSubWindow(type)) { // sub window
        TLOGD(WmsLogTag::WMS_RECOVER, "SubWindow");
        auto parentSession = FindParentSessionByParentId(property_->GetParentId());
        if (parentSession == nullptr || parentSession->GetHostSession() == nullptr) {
            TLOGE(WmsLogTag::WMS_RECOVER, "parentSession is null");
            return WMError::WM_ERROR_NULLPTR;
        }
    }
    SingletonContainer::Get<WindowAdapter>().RecoverAndConnectSpecificSession(
        iSessionStage, eventChannel, surfaceNode_, property_, session, token);

    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "Recover failed, session is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    hostSession_ = session;

    TLOGI(WmsLogTag::WMS_RECOVER,
        "over, windowName = %{public}s, persistentId = %{public}d",
        GetWindowName().c_str(), GetPersistentId());
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::RecoverAndReconnectSceneSession()
{
    if (isFocused_) {
        UpdateFocus(false);
    }
    SessionInfo info;
    auto abilityContext = AbilityRuntime::Context::ConvertTo<AbilityRuntime::AbilityContext>(context_);
    if (property_ && context_ && context_->GetHapModuleInfo() && abilityContext && abilityContext->GetAbilityInfo()) {
        info.abilityName_ = abilityContext->GetAbilityInfo()->name;
        info.moduleName_ = context_->GetHapModuleInfo()->moduleName;
        info.bundleName_ = property_->GetSessionInfo().bundleName_;
    } else {
        TLOGE(WmsLogTag::WMS_RECOVER, "property_ or context_ or abilityContext is null, recovered session failed");
        return WMError::WM_ERROR_NULLPTR;
    }
    property_->SetSessionInfo(info);
    property_->SetWindowState(state_);
    TLOGI(WmsLogTag::WMS_RECOVER,
        "Recover and reconnect sceneSession with: bundleName=%{public}s, moduleName=%{public}s, "
        "abilityName=%{public}s, appIndex=%{public}d, type=%{public}u, persistentId=%{public}d, windowState=%{public}d",
        info.bundleName_.c_str(), info.moduleName_.c_str(), info.abilityName_.c_str(), info.appIndex_, info.windowType_,
        GetPersistentId(), state_);
    sptr<ISessionStage> iSessionStage(this);
    auto windowEventChannel = new (std::nothrow) WindowEventChannel(iSessionStage);
    sptr<IWindowEventChannel> iWindowEventChannel(windowEventChannel);
    sptr<IRemoteObject> token = context_ ? context_->GetToken() : nullptr;
    sptr<Rosen::ISession> session;
    auto ret = SingletonContainer::Get<WindowAdapter>().RecoverAndReconnectSceneSession(
        iSessionStage, iWindowEventChannel, surfaceNode_, session, property_, token);
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "session is null, recovered session failed");
        return WMError::WM_ERROR_NULLPTR;
    }
    RecoverSessionListener();
    TLOGI(WmsLogTag::WMS_RECOVER, "Recover and reconnect sceneSession successful");
    hostSession_ = session;
    return static_cast<WMError>(ret);
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
    WindowType windowType = GetType();
    if (WindowHelper::IsMainWindow(windowType)) {
        if (windowSystemConfig_.maxFloatingWindowSize_ != UINT32_MAX) {
            maxFloatingWindowSize_ = windowSystemConfig_.maxFloatingWindowSize_;
        }
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
        bool isSubWindow = WindowHelper::IsSubWindow(windowType);
        bool isDialogWindow = WindowHelper::IsDialogWindow(windowType);
        UpdateWindowSizeLimits();
        if ((isSubWindow || isDialogWindow) && property_->GetDragEnabled()) {
            WLOGFD("sync window limits to server side in order to make size limits work while resizing");
            UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS);
        }
    }
}

WMError WindowSceneSessionImpl::Create(const std::shared_ptr<AbilityRuntime::Context>& context,
    const sptr<Rosen::ISession>& iSession, const std::string& identityToken)
{
    if (property_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "Window Create failed, property is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    TLOGI(WmsLogTag::WMS_LIFE, "Window Create name:%{public}s, state:%{public}u, windowmode:%{public}u",
        property_->GetWindowName().c_str(), state_, GetMode());
    // allow iSession is nullptr when create window by innerkits
    if (!context) {
        WLOGFW("[WMSLife] context is nullptr, name:%{public}s", property_->GetWindowName().c_str());
    }
    WMError ret = WindowSessionCreateCheck();
    if (ret != WMError::WM_OK) {
        return ret;
    }
    SetDefaultDisplayIdIfNeed();
    hostSession_ = iSession;
    context_ = context;
    identityToken_ = identityToken;
    AdjustWindowAnimationFlag();
    if (context && context->GetApplicationInfo() &&
        context->GetApplicationInfo()->apiCompatibleVersion >= 9 && // 9: api version
        !SessionPermission::IsSystemCalling()) {
        WLOGI("Remove window flag WINDOW_FLAG_SHOW_WHEN_LOCKED");
        property_->SetWindowFlags(property_->GetWindowFlags() &
            (~(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED))));
    }

    bool isSpecificSession = false;
    if (hostSession_) { // main window
        ret = Connect();
    } else { // system or sub window
        TLOGI(WmsLogTag::WMS_LIFE, "Create system or sub window");
        isSpecificSession = true;
        const auto& type = GetType();
        if (WindowHelper::IsSystemWindow(type)) {
            // Not valid system window type for session should return WMError::WM_OK;
            if (!IsValidSystemWindowType(type)) {
                return WMError::WM_OK;
            }
        } else if (!WindowHelper::IsSubWindow(type)) {
            TLOGI(WmsLogTag::WMS_LIFE, "create failed not system or sub type, type: %{public}d", type);
            return WMError::WM_ERROR_INVALID_TYPE;
        }
        ret = CreateAndConnectSpecificSession();
    }
    if (ret == WMError::WM_OK) {
        MakeSubOrDialogWindowDragableAndMoveble();
        UpdateWindowState();
        RegisterSessionRecoverListener(isSpecificSession);
    }
    TLOGD(WmsLogTag::WMS_LIFE, "Window Create success [name:%{public}s, \
        id:%{public}d], state:%{public}u, windowmode:%{public}u",
        property_->GetWindowName().c_str(), property_->GetPersistentId(), state_, GetMode());
    sptr<Window> self(this);
    InputTransferStation::GetInstance().AddInputWindow(self);
    needRemoveWindowInputChannel_ = true;
    return ret;
}

bool WindowSceneSessionImpl::HandlePointDownEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    const MMI::PointerEvent::PointerItem& pointerItem, int32_t sourceType, float vpr, const WSRect& rect)
{
    bool needNotifyEvent = true;
    uint32_t titleBarHeight = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * vpr);
    int32_t winX = pointerItem.GetWindowX();
    int32_t winY = pointerItem.GetWindowY();
    bool isMoveArea = (0 <= winX && winX <= static_cast<int32_t>(rect.width_)) &&
        (0 <= winY && winY <= static_cast<int32_t>(titleBarHeight));
    int outside = (sourceType == MMI::PointerEvent::SOURCE_TYPE_MOUSE) ? static_cast<int>(HOTZONE_POINTER * vpr) :
        static_cast<int>(HOTZONE_TOUCH * vpr);
    auto dragType = SessionHelper::GetAreaType(winX, winY, sourceType, outside, vpr, rect);
    WindowType windowType = property_->GetWindowType();
    bool isDecorDialog = windowType == WindowType::WINDOW_TYPE_DIALOG && property_->IsDecorEnable();
    bool isFixedSubWin = WindowHelper::IsSubWindow(windowType) && !property_->GetDragEnabled();
    if ((WindowHelper::IsSystemWindow(windowType) || isFixedSubWin) && !isDecorDialog) {
        hostSession_->ProcessPointDownSession(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
    } else {
        if (dragType != AreaType::UNDEFINED) {
            hostSession_->SendPointEventForMoveDrag(pointerEvent);
            needNotifyEvent = false;
        } else if (isMoveArea) {
            hostSession_->SendPointEventForMoveDrag(pointerEvent);
        } else {
            hostSession_->ProcessPointDownSession(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
        }
    }
    return needNotifyEvent;
}

void WindowSceneSessionImpl::ConsumePointerEventInner(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    const MMI::PointerEvent::PointerItem& pointerItem)
{
    const int32_t& action = pointerEvent->GetPointerAction();
    const auto& sourceType = pointerEvent->GetSourceType();
    const auto& rect = SessionHelper::TransferToWSRect(GetRect());
    bool isPointDown = (action == MMI::PointerEvent::POINTER_ACTION_DOWN ||
        action == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    bool needNotifyEvent = true;
    if (isPointDown) {
        auto displayInfo = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayInfo(property_->GetDisplayId());
        if (displayInfo == nullptr) {
            WLOGFE("The display or display info is nullptr");
            pointerEvent->MarkProcessed();
            return;
        }
        float vpr = GetVirtualPixelRatio(displayInfo);
        if (MathHelper::NearZero(vpr)) {
            WLOGFW("vpr is zero");
            pointerEvent->MarkProcessed();
            return;
        }
        needNotifyEvent = HandlePointDownEvent(pointerEvent, pointerItem, sourceType, vpr, rect);
        RefreshNoInteractionTimeoutMonitor();
    }
    bool isPointUp = (action == MMI::PointerEvent::POINTER_ACTION_UP ||
        action == MMI::PointerEvent::POINTER_ACTION_BUTTON_UP ||
        action == MMI::PointerEvent::POINTER_ACTION_CANCEL);
    if (isPointUp) {
        hostSession_->SendPointEventForMoveDrag(pointerEvent);
    }

    if (needNotifyEvent) {
        NotifyPointerEvent(pointerEvent);
    } else {
        pointerEvent->MarkProcessed();
    }
    if (isPointDown || isPointUp) {
        TLOGI(WmsLogTag::WMS_EVENT, "InputTracking id:%{public}d, windowId:%{public}u, pointId:%{public}d, "
            "sourceType:%{public}d, pointPos:[%{public}d, %{public}d], "
            "winRect:[%{public}d, %{public}d, %{public}u, %{public}u], "
            "needNotifyEvent:%{public}d",
            pointerEvent->GetId(), GetWindowId(), pointerEvent->GetPointerId(),
            sourceType, pointerItem.GetDisplayX(), pointerItem.GetDisplayY(),
            rect.posX_, rect.posY_, rect.width_, rect.height_,
            needNotifyEvent);
    }
}

void WindowSceneSessionImpl::ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (pointerEvent == nullptr) {
        WLOGFE("PointerEvent is nullptr, windowId: %{public}d", GetWindowId());
        return;
    }

    if (hostSession_ == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "hostSession is nullptr, windowId: %{public}d", GetWindowId());
        pointerEvent->MarkProcessed();
        return;
    }
    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
        TLOGW(WmsLogTag::WMS_EVENT, "invalid pointerEvent, windowId: %{public}d", GetWindowId());
        pointerEvent->MarkProcessed();
        return;
    }

    ConsumePointerEventInner(pointerEvent, pointerItem);
}

bool WindowSceneSessionImpl::PreNotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    bool ret = false;
    if (uiContent_ != nullptr) {
        ret = uiContent_->ProcessKeyEvent(keyEvent, true);
    }
    RefreshNoInteractionTimeoutMonitor();
    return ret;
}

void WindowSceneSessionImpl::RegisterSessionRecoverListener(bool isSpecificSession)
{
    TLOGD(WmsLogTag::WMS_RECOVER, "persistentId = %{public}d, isSpecificSession = %{public}s",
        GetPersistentId(), isSpecificSession ? "true" : "false");

    if (GetType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        TLOGI(WmsLogTag::WMS_RECOVER, "input method window does not need to recover");
        return;
    }
    if (property_ != nullptr && property_->GetCollaboratorType() != CollaboratorType::DEFAULT_TYPE) {
        TLOGI(WmsLogTag::WMS_RECOVER, "collaboratorType is %{public}" PRId32 ", not need to recover",
            property_->GetCollaboratorType());
        return;
    }

    wptr<WindowSceneSessionImpl> weakThis = this;
    auto callbackFunc = [weakThis, isSpecificSession] {
        auto promoteThis = weakThis.promote();
        if (promoteThis == nullptr) {
            TLOGW(WmsLogTag::WMS_RECOVER, "promoteThis is nullptr");
            return WMError::WM_ERROR_NULLPTR;
        }
        if (promoteThis->state_ == WindowState::STATE_DESTROYED) {
            TLOGW(WmsLogTag::WMS_RECOVER, "windowState is STATE_DESTROYED, no need to recover");
            return WMError::WM_ERROR_DESTROYED_OBJECT;
        }

        auto ret = isSpecificSession ? promoteThis->RecoverAndConnectSpecificSession() :
			promoteThis->RecoverAndReconnectSceneSession();

        TLOGD(WmsLogTag::WMS_RECOVER, "Recover session over, ret = %{public}d", ret);
        return ret;
    };
    SingletonContainer::Get<WindowAdapter>().RegisterSessionRecoverCallbackFunc(GetPersistentId(), callbackFunc);
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
        property_->SetConfigWindowLimitsVP({
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
    } else if (WindowHelper::IsSystemWindow(GetType()) && GetType() != WindowType::WINDOW_TYPE_DIALOG) {
        systemLimits.minWidth_ = 0;
        systemLimits.minHeight_ = 0;
    } else {
        systemLimits.minWidth_ = static_cast<uint32_t>(MIN_FLOATING_WIDTH * vpr);
        systemLimits.minHeight_ = static_cast<uint32_t>(MIN_FLOATING_HEIGHT * vpr);
    }
    WLOGFI("[System SizeLimits] [maxWidth: %{public}u, minWidth: %{public}u, maxHeight: %{public}u, "
        "minHeight: %{public}u]", systemLimits.maxWidth_, systemLimits.minWidth_,
        systemLimits.maxHeight_, systemLimits.minHeight_);
    return systemLimits;
}

void WindowSceneSessionImpl::CalculateNewLimitsByLimits(
    WindowLimits& newLimits, WindowLimits& customizedLimits, float& virtualPixelRatio)
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

    virtualPixelRatio = GetVirtualPixelRatio(display->GetDisplayInfo());
    const auto& systemLimits = GetSystemSizeLimits(displayWidth, displayHeight, virtualPixelRatio);

    if (userLimitsSet_) {
        customizedLimits = property_->GetUserWindowLimits();
    } else {
        customizedLimits = property_->GetConfigWindowLimitsVP();
        customizedLimits.maxWidth_ = static_cast<uint32_t>(customizedLimits.maxWidth_ * virtualPixelRatio);
        customizedLimits.maxHeight_ = static_cast<uint32_t>(customizedLimits.maxHeight_ * virtualPixelRatio);
        customizedLimits.minWidth_ = static_cast<uint32_t>(customizedLimits.minWidth_ * virtualPixelRatio);
        customizedLimits.minHeight_ = static_cast<uint32_t>(customizedLimits.minHeight_ * virtualPixelRatio);
    }
    newLimits = systemLimits;

    // calculate new limit size
    if (systemLimits.minWidth_ <= customizedLimits.maxWidth_ &&
        customizedLimits.maxWidth_ <= systemLimits.maxWidth_) {
        newLimits.maxWidth_ = customizedLimits.maxWidth_;
    }
    if (systemLimits.minHeight_ <= customizedLimits.maxHeight_ &&
        customizedLimits.maxHeight_ <= systemLimits.maxHeight_) {
        newLimits.maxHeight_ = customizedLimits.maxHeight_;
    }
    if (systemLimits.minWidth_ <= customizedLimits.minWidth_ &&
        customizedLimits.minWidth_ <= newLimits.maxWidth_) {
        newLimits.minWidth_ = customizedLimits.minWidth_;
    }
    if (systemLimits.minHeight_ <= customizedLimits.minHeight_ &&
        customizedLimits.minHeight_ <= newLimits.maxHeight_) {
        newLimits.minHeight_ = customizedLimits.minHeight_;
    }
}

void WindowSceneSessionImpl::CalculateNewLimitsByRatio(WindowLimits& newLimits, WindowLimits& customizedLimits)
{
    newLimits.maxRatio_ = customizedLimits.maxRatio_;
    newLimits.minRatio_ = customizedLimits.minRatio_;

    // calculate new limit ratio
    float maxRatio = FLT_MAX;
    float minRatio = 0.0f;
    if (newLimits.minHeight_ != 0) {
        maxRatio = static_cast<float>(newLimits.maxWidth_) / static_cast<float>(newLimits.minHeight_);
    }
    if (newLimits.maxHeight_ != 0) {
        minRatio = static_cast<float>(newLimits.minWidth_) / static_cast<float>(newLimits.maxHeight_);
    }
    if (!MathHelper::GreatNotEqual(minRatio, customizedLimits.maxRatio_) &&
        !MathHelper::GreatNotEqual(customizedLimits.maxRatio_, maxRatio)) {
        maxRatio = customizedLimits.maxRatio_;
    }
    if (!MathHelper::GreatNotEqual(minRatio, customizedLimits.minRatio_) &&
        !MathHelper::GreatNotEqual(customizedLimits.minRatio_, maxRatio)) {
        minRatio = customizedLimits.minRatio_;
    }

    // recalculate limit size by new ratio
    double newMaxWidthFloat = static_cast<double>(newLimits.maxHeight_) * maxRatio;
    uint32_t newMaxWidth = (newMaxWidthFloat > static_cast<double>(UINT32_MAX)) ? UINT32_MAX :
        static_cast<uint32_t>(newMaxWidthFloat);
    newLimits.maxWidth_ = std::min(newMaxWidth, newLimits.maxWidth_);

    double newMinWidthFloat = static_cast<double>(newLimits.minHeight_) * minRatio;
    uint32_t newMinWidth = (newMinWidthFloat > static_cast<double>(UINT32_MAX)) ? UINT32_MAX :
        static_cast<uint32_t>(newMinWidthFloat);
    newLimits.minWidth_ = std::max(newMinWidth, newLimits.minWidth_);

    uint32_t newMaxHeight = MathHelper::NearZero(minRatio) ? UINT32_MAX :
        static_cast<uint32_t>(static_cast<double>(newLimits.maxWidth_) / minRatio);
    newLimits.maxHeight_ = std::min(newMaxHeight, newLimits.maxHeight_);

    uint32_t newMinHeight = MathHelper::NearZero(maxRatio) ? UINT32_MAX :
        static_cast<uint32_t>(static_cast<double>(newLimits.minWidth_) / maxRatio);
    newLimits.minHeight_ = std::max(newMinHeight, newLimits.minHeight_);
}

void WindowSceneSessionImpl::UpdateWindowSizeLimits()
{
    WindowLimits customizedLimits;
    WindowLimits newLimits;
    float virtualPixelRatio = 0.0f;

    CalculateNewLimitsByLimits(newLimits, customizedLimits, virtualPixelRatio);
    if (MathHelper::NearZero(virtualPixelRatio)) {
        return;
    }

    CalculateNewLimitsByRatio(newLimits, customizedLimits);

    property_->SetWindowLimits(newLimits);
    property_->SetLastLimitsVpr(virtualPixelRatio);
}

void WindowSceneSessionImpl::UpdateSubWindowStateAndNotify(int32_t parentPersistentId, const WindowState& newState)
{
    auto iter = subWindowSessionMap_.find(parentPersistentId);
    if (iter == subWindowSessionMap_.end()) {
        TLOGD(WmsLogTag::WMS_SUB, "main window: %{public}d has no child node", parentPersistentId);
        return;
    }
    const auto& subWindows = iter->second;
    if (subWindows.empty()) {
        TLOGD(WmsLogTag::WMS_SUB, "main window: %{public}d, its subWindowMap is empty", parentPersistentId);
        return;
    }

    // when main window hide and subwindow whose state is shown should hide and notify user
    if (newState == WindowState::STATE_HIDDEN) {
        for (auto subwindow : subWindows) {
            if (subwindow != nullptr && subwindow->GetWindowState() == WindowState::STATE_SHOWN) {
                subwindow->state_ = WindowState::STATE_HIDDEN;
                subwindow->NotifyAfterBackground();
                TLOGD(WmsLogTag::WMS_SUB, "Notify subWindow background, id:%{public}d", subwindow->GetPersistentId());
            }
        }
    // when main window show and subwindow whose state is shown should show and notify user
    } else if (newState == WindowState::STATE_SHOWN) {
        for (auto subwindow : subWindows) {
            if (subwindow != nullptr && subwindow->GetWindowState() == WindowState::STATE_HIDDEN &&
                subwindow->GetRequestWindowState() == WindowState::STATE_SHOWN) {
                subwindow->state_ = WindowState::STATE_SHOWN;
                subwindow->NotifyAfterForeground();
                TLOGD(WmsLogTag::WMS_SUB, "Notify subWindow foreground, id:%{public}d", subwindow->GetPersistentId());
            }
        }
    }
}

WMError WindowSceneSessionImpl::Show(uint32_t reason, bool withAnimation)
{
    if (property_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "Window show failed, property is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    const auto& type = GetType();
    TLOGI(WmsLogTag::WMS_LIFE, "Window show [name: %{public}s, id: %{public}d, type: %{public}u], reason: %{public}u,"
        " state:%{public}u, requestState:%{public}u", property_->GetWindowName().c_str(),
        GetPersistentId(), type, reason, state_, requestState_);
    if (IsWindowSessionInvalid()) {
        TLOGI(WmsLogTag::WMS_LIFE, "Window show failed, session is invalid, name: %{public}s, id: %{public}d",
            property_->GetWindowName().c_str(), GetPersistentId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    auto isDecorEnable = IsDecorEnable();
    UpdateDecorEnableToAce(isDecorEnable);
    property_->SetDecorEnable(isDecorEnable);

    if (state_ == WindowState::STATE_SHOWN) {
        TLOGD(WmsLogTag::WMS_LIFE, "window session is alreay shown [name:%{public}s, id:%{public}d, type: %{public}u]",
            property_->GetWindowName().c_str(), property_->GetPersistentId(), type);
        if (WindowHelper::IsMainWindow(type) && hostSession_ != nullptr) {
            hostSession_->RaiseAppMainWindowToTop();
        }
        NotifyAfterForeground(true, false);
        RefreshNoInteractionTimeoutMonitor();
        return WMError::WM_OK;
    }

    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr || display->GetDisplayInfo() == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "Window show failed, display is null, name: %{public}s, id: %{public}d",
            property_->GetWindowName().c_str(), GetPersistentId());
        return WMError::WM_ERROR_NULLPTR;
    }
    auto displayInfo = display->GetDisplayInfo();
    float density = GetVirtualPixelRatio(displayInfo);
    if (!MathHelper::NearZero(virtualPixelRatio_ - density) ||
        !MathHelper::NearZero(property_->GetLastLimitsVpr() - density)) {
        UpdateDensity();
    }

    if (hostSession_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    WMError ret = UpdateAnimationFlagProperty(withAnimation);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_LIFE, "Window show failed, UpdateProperty failed, ret: %{public}d, name: %{public}s"
            ", id: %{public}d", static_cast<int32_t>(ret), property_->GetWindowName().c_str(), GetPersistentId());
        return ret;
    }
    UpdateTitleButtonVisibility();
    if (WindowHelper::IsMainWindow(type)) {
        ret = static_cast<WMError>(hostSession_->Foreground(property_, true));
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
        RefreshNoInteractionTimeoutMonitor();
        TLOGI(WmsLogTag::WMS_LIFE, "Window show success [name:%{public}s, id:%{public}d, type:%{public}u]",
            property_->GetWindowName().c_str(), GetPersistentId(), type);
    } else {
        NotifyForegroundFailed(ret);
        TLOGI(WmsLogTag::WMS_LIFE, "Window show failed with errcode: %{public}d, name:%{public}s, id:%{public}d",
            static_cast<int32_t>(ret), property_->GetWindowName().c_str(), GetPersistentId());
    }
    NotifyWindowStatusChange(GetMode());
    return ret;
}

WMError WindowSceneSessionImpl::Hide(uint32_t reason, bool withAnimation, bool isFromInnerkits)
{
    if (property_ == nullptr || hostSession_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE,
            "Window hide failed, because of nullptr, property: %{public}d, id: %{public}d",
            property_ == nullptr, GetPersistentId());
        return WMError::WM_ERROR_NULLPTR;
    }

    const auto& type = GetType();
    TLOGI(WmsLogTag::WMS_LIFE, "Window hide [id:%{public}d, type: %{public}d, reason:%{public}u, state:%{public}u, "
        "requestState:%{public}u", GetPersistentId(), type, reason, state_, requestState_);
    if (IsWindowSessionInvalid()) {
        TLOGI(WmsLogTag::WMS_LIFE, "session is invalid, id:%{public}d", GetPersistentId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    WindowState validState = WindowHelper::IsSubWindow(type) ? requestState_ : state_;
    if (validState == WindowState::STATE_HIDDEN || state_ == WindowState::STATE_CREATED) {
        TLOGD(WmsLogTag::WMS_LIFE, "window session is alreay hidden, id:%{public}d", property_->GetPersistentId());
        NotifyBackgroundFailed(WMError::WM_DO_NOTHING);
        return WMError::WM_OK;
    }

    WMError res = UpdateAnimationFlagProperty(withAnimation);
    if (res != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_LIFE, "UpdateProperty failed with errCode:%{public}d", static_cast<int32_t>(res));
        return res;
    }

    /*
     * main window no need to notify host, since host knows hide first
     * main window notify host temporarily, since host background may failed
     * need to SetActive(false) for host session before background
     */

    if (WindowHelper::IsMainWindow(type)) {
        res = static_cast<WMError>(SetActive(false));
        if (res != WMError::WM_OK) {
            return res;
        }
        res = static_cast<WMError>(hostSession_->Background(true));
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
    uint32_t animationFlag = property_->GetAnimationFlag();
    if (animationFlag == static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
        animationTransitionController_->AnimationForHidden();
        RSTransaction::FlushImplicitTransaction();
    }
    NotifyWindowStatusChange(GetMode());
    escKeyEventTriggered_ = false;
    TLOGI(WmsLogTag::WMS_LIFE, "Window hide success [id:%{public}d, type: %{public}d",
        property_->GetPersistentId(), type);
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
        case WindowType::WINDOW_TYPE_GLOBAL_SEARCH:
        case WindowType::WINDOW_TYPE_DIALOG:
        case WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW:
        case WindowType::WINDOW_TYPE_PANEL:
        case WindowType::WINDOW_TYPE_LAUNCHER_DOCK: {
            property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            break;
        }
        case WindowType::WINDOW_TYPE_VOLUME_OVERLAY:
        case WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT:
        case WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR:
        case WindowType::WINDOW_TYPE_DOCK_SLICE:
        case WindowType::WINDOW_TYPE_STATUS_BAR:
        case WindowType::WINDOW_TYPE_NAVIGATION_BAR: {
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
        case WindowType::WINDOW_TYPE_POINTER: {
            property_->SetFocusable(false);
            break;
        }
        default:
            break;
    }
}

void WindowSceneSessionImpl::DestroySubWindow()
{
    const int32_t& parentPersistentId = property_->GetParentPersistentId();
    const int32_t& persistentId = GetPersistentId();

    TLOGI(WmsLogTag::WMS_SUB, "Id: %{public}d, parentId: %{public}d", persistentId, parentPersistentId);

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
                TLOGD(WmsLogTag::WMS_SUB, "Destroy sub window, persistentId: %{public}d", persistentId);
                subWindows.erase(iter);
                break;
            } else {
                TLOGD(WmsLogTag::WMS_SUB, "Exists other sub window, persistentId: %{public}d", persistentId);
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
                TLOGW(WmsLogTag::WMS_SUB, "Destroy sub window which is nullptr");
                subWindows.erase(iter);
                continue;
            }
            TLOGD(WmsLogTag::WMS_SUB, "Destroy sub window, persistentId: %{public}d", (*iter)->GetPersistentId());
            auto ret = (*iter)->Destroy(false);
            if (ret != WMError::WM_OK) {
                TLOGE(WmsLogTag::WMS_SUB, "Destroy sub window failed. persistentId: %{public}d",
                    (*iter)->GetPersistentId());
                subWindows.erase(iter);
            }
        }
        mainIter->second.clear();
        subWindowSessionMap_.erase(mainIter);
    }
}

WMError WindowSceneSessionImpl::DestroyInner(bool needNotifyServer)
{
    WMError ret = WMError::WM_OK;
    if (!WindowHelper::IsMainWindow(GetType()) && needNotifyServer) {
        if (WindowHelper::IsSystemWindow(GetType())) {
            // main window no need to notify host, since host knows hide first
            ret = SyncDestroyAndDisconnectSpecificSession(property_->GetPersistentId());
        } else if (WindowHelper::IsSubWindow(GetType()) && (property_->GetExtensionFlag() == false)) {
            auto parentSession = FindParentSessionByParentId(GetParentId());
            if (parentSession == nullptr || parentSession->GetHostSession() == nullptr) {
                return WMError::WM_ERROR_NULLPTR;
            }
            ret = SyncDestroyAndDisconnectSpecificSession(property_->GetPersistentId());
        } else if (property_->GetExtensionFlag() == true) {
            ret = SyncDestroyAndDisconnectSpecificSession(property_->GetPersistentId());
        }
    }
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_LIFE, "DestroyInner fail, ret:%{public}d", ret);
        return ret;
    }

    if (WindowHelper::IsMainWindow(GetType())) {
        if (hostSession_ != nullptr) {
            ret = static_cast<WMError>(hostSession_->Disconnect(true));
        }
    }
    return ret;
}

WMError WindowSceneSessionImpl::SyncDestroyAndDisconnectSpecificSession(int32_t persistentId)
{
    WMError ret = WMError::WM_OK;
    if (SysCapUtil::GetBundleName() == AppExecFwk::Constants::SCENE_BOARD_BUNDLE_NAME) {
        TLOGI(WmsLogTag::WMS_LIFE, "Destroy window is scb window");
        ret = SingletonContainer::Get<WindowAdapter>().DestroyAndDisconnectSpecificSession(persistentId);
        return ret;
    }
    sptr<PatternDetachCallback> callback = new PatternDetachCallback();
    ret = SingletonContainer::Get<WindowAdapter>().DestroyAndDisconnectSpecificSessionWithDetachCallback(persistentId,
        callback->AsObject());
    if (ret != WMError::WM_OK) {
        return ret;
    }
    auto startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    callback->GetResult(WINDOW_DETACH_TIMEOUT);
    auto endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    auto waitTime = endTime - startTime;
    if (waitTime >= WINDOW_DETACH_TIMEOUT) {
        TLOGW(WmsLogTag::WMS_LIFE, "Destroy window timeout, persistentId:%{public}d", persistentId);
        callback->GetResult(std::numeric_limits<int>::max());
    }
    TLOGI(WmsLogTag::WMS_LIFE, "Destroy window persistentId:%{public}d waitTime:%{public}lld", persistentId, waitTime);
    return ret;
}

WMError WindowSceneSessionImpl::Destroy(bool needNotifyServer, bool needClearListener)
{
    if (property_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "Window destroy failed, property is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    TLOGI(WmsLogTag::WMS_LIFE, "Destroy start, id: %{public}d, state_:%{public}u, needNotifyServer: %{public}d, "
        "needClearListener: %{public}d", GetPersistentId(), state_, needNotifyServer, needClearListener);
    if (needRemoveWindowInputChannel_) {
        InputTransferStation::GetInstance().RemoveInputWindow(GetPersistentId());
        needRemoveWindowInputChannel_ = false;
    }
    if (IsWindowSessionInvalid()) {
        TLOGI(WmsLogTag::WMS_LIFE, "session is invalid, id: %{public}d", GetPersistentId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    SingletonContainer::Get<WindowAdapter>().UnregisterSessionRecoverCallbackFunc(property_->GetPersistentId());

    auto ret = DestroyInner(needNotifyServer);
    if (ret != WMError::WM_OK && ret != WMError::WM_ERROR_NULLPTR) { //nullptr means no session in server
        WLOGFW("[WMSLife] Destroy window failed, id: %{public}d", GetPersistentId());
        return ret;
    }

    // delete after replace WSError with WMError
    NotifyBeforeDestroy(GetWindowName());
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
    NotifyAfterDestroy();
    if (needClearListener) {
        ClearListenersById(GetPersistentId());
    }
    if (context_) {
        context_.reset();
    }
    TLOGI(WmsLogTag::WMS_LIFE, "Destroy success, id: %{public}d", property_->GetPersistentId());
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::MoveTo(int32_t x, int32_t y)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "Id:%{public}d MoveTo %{public}d %{public}d", property_->GetPersistentId(), x, y);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (property_->GetWindowType() == WindowType::WINDOW_TYPE_PIP) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "Unsupported operation for pip window");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    const auto& windowRect = GetRect();
    const auto& requestRect = GetRequestRect();
    if (WindowHelper::IsSubWindow(GetType())) {
        auto mainWindow = FindMainWindowWithContext();
        if (mainWindow != nullptr && (mainWindow->GetMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY ||
                                      mainWindow->GetMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY)) {
            if (requestRect.posX_ == x && requestRect.posX_ == y) {
                TLOGW(WmsLogTag::WMS_LAYOUT, "Request same position in multiWindow will not update");
                return WMError::WM_OK;
            }
        }
    }
    Rect newRect = { x, y, requestRect.width_, requestRect.height_ }; // must keep x/y
    TLOGI(WmsLogTag::WMS_LAYOUT, "Id:%{public}d, state: %{public}d, type: %{public}d, mode: %{public}d, requestRect: "
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

void WindowSceneSessionImpl::LimitCameraFloatWindowMininumSize(uint32_t& width, uint32_t& height, float& vpr)
{
    // Float camera window has a special limit:
    // if display sw <= 600dp, portrait: min width = display sw * 30%, landscape: min width = sw * 50%
    // if display sw > 600dp, portrait: min width = display sw * 12%, landscape: min width = sw * 30%
    if (GetType() != WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
        return;
    }

    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(property_->GetDisplayId());
    if (display == nullptr || display->GetDisplayInfo() == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64"", property_->GetDisplayId());
        return;
    }
    uint32_t displayWidth = static_cast<uint32_t>(display->GetWidth());
    uint32_t displayHeight = static_cast<uint32_t>(display->GetHeight());
    if (displayWidth == 0 || displayHeight == 0) {
        return;
    }
    vpr = GetVirtualPixelRatio(display->GetDisplayInfo());
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
    if ((!WindowHelper::IsSystemWindow(property_->GetWindowType())) ||
        (property_->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG)) {
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

void WindowSceneSessionImpl::LimitWindowSize(uint32_t& width, uint32_t& height)
{
    float vpr = 0.0f;

    // Float camera window has special limits
    LimitCameraFloatWindowMininumSize(width, height, vpr);

    if (!MathHelper::NearZero(vpr) || !MathHelper::NearZero(property_->GetLastLimitsVpr() - vpr)) {
        UpdateWindowSizeLimits();
    }
    UpdateFloatingWindowSizeBySizeLimits(width, height);
}

WMError WindowSceneSessionImpl::Resize(uint32_t width, uint32_t height)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "Id:%{public}d Resize %{public}u %{public}u",
        property_->GetPersistentId(), width, height);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (property_->GetWindowType() == WindowType::WINDOW_TYPE_PIP) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "Unsupported operation for pip window");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    if (GetMode() != WindowMode::WINDOW_MODE_FLOATING) {
        TLOGW(WmsLogTag::WMS_LAYOUT, "Fullscreen window could not resize, winId: %{public}u", GetWindowId());
        return WMError::WM_ERROR_INVALID_OPERATION;
    }

    LimitWindowSize(width, height);

    const auto& windowRect = GetRect();
    const auto& requestRect = GetRequestRect();

    if (WindowHelper::IsSubWindow(GetType())) {
        auto mainWindow = FindMainWindowWithContext();
        if (mainWindow != nullptr && (mainWindow->GetMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY ||
                                      mainWindow->GetMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY)) {
            if (width == requestRect.width_ && height == requestRect.height_) {
                TLOGW(WmsLogTag::WMS_LAYOUT, "Request same size in multiWindow will not update, return");
                return WMError::WM_OK;
            }
        }
    }

    Rect newRect = { requestRect.posX_, requestRect.posY_, width, height }; // must keep w/h
    TLOGI(WmsLogTag::WMS_LAYOUT, "Id:%{public}d, state: %{public}d, type: %{public}d, mode: %{public}d, requestRect: "
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

bool WindowSceneSessionImpl::CheckGetAvoidAreaAvailable(AvoidAreaType type)
{
    WindowMode mode = GetMode();
    WindowType winType = GetType();

    if (type == AvoidAreaType::TYPE_KEYBOARD) {
        return true;
    }
    if (WindowHelper::IsMainWindow(winType)) {
        if (mode != WindowMode::WINDOW_MODE_FLOATING ||
            system::GetParameter("const.product.devicetype", "unknown") == "phone" ||
            system::GetParameter("const.product.devicetype", "unknown") == "tablet") {
            return true;
        }
    }
    if (WindowHelper::IsSubWindow(winType)) {
        auto parentSessoin = FindParentSessionByParentId(property_->GetParentId());
        if (parentSessoin->GetRect() == GetRect()) {
            return true;
        }
    }
    TLOGI(WmsLogTag::WMS_IMMS, "Window [%{public}u, %{public}s] type %{public}d "
        "avoidAreaType %{public}u windowMode %{public}u, return default avoid area.",
        GetWindowId(), GetWindowName().c_str(), static_cast<uint32_t>(winType),
        static_cast<uint32_t>(type), static_cast<uint32_t>(mode));
    return false;
}

WMError WindowSceneSessionImpl::GetAvoidAreaByType(AvoidAreaType type, AvoidArea& avoidArea)
{
    uint32_t windowId = GetWindowId();
    if (!CheckGetAvoidAreaAvailable(type)) {
        return WMError::WM_OK;
    }
    if (hostSession_ == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "hostSession_ is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    avoidArea = hostSession_->GetAvoidAreaByType(type);
    getAvoidAreaCnt_++;
    TLOGI(WmsLogTag::WMS_IMMS, "Window [%{public}u, %{public}s] type %{public}d %{public}u times, "
          "top{%{public}d, %{public}d, %{public}d, %{public}d}, down{%{public}d, %{public}d, %{public}d, %{public}d}",
          windowId, GetWindowName().c_str(), type, getAvoidAreaCnt_,
          avoidArea.topRect_.posX_, avoidArea.topRect_.posY_, avoidArea.topRect_.width_, avoidArea.topRect_.height_,
          avoidArea.bottomRect_.posX_, avoidArea.bottomRect_.posY_, avoidArea.bottomRect_.width_,
          avoidArea.bottomRect_.height_);
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::NotifyWindowNeedAvoid(bool status)
{
    TLOGD(WmsLogTag::WMS_IMMS, "NotifyWindowNeedAvoid called windowId:%{public}u status:%{public}d",
        GetWindowId(), static_cast<int32_t>(status));
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_IMMS, "session is invalid");
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
        TLOGE(WmsLogTag::WMS_IMMS, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    uint32_t version = 0;
    if ((context_ != nullptr) && (context_->GetApplicationInfo() != nullptr)) {
        version = context_->GetApplicationInfo()->apiCompatibleVersion;
    }
    isIgnoreSafeArea_ = status;
    // 10 ArkUI new framework support after API10
    if (version >= 10) {
        TLOGI(WmsLogTag::WMS_IMMS, "SetIgnoreViewSafeArea winId:%{public}u status:%{public}d",
            GetWindowId(), static_cast<int32_t>(status));
        if (uiContent_ != nullptr) {
            uiContent_->SetIgnoreViewSafeArea(status);
        }
        isIgnoreSafeAreaNeedNotify_ = true;
    } else {
        TLOGI(WmsLogTag::WMS_IMMS, "SetWindowNeedAvoidFlag winId:%{public}u status:%{public}d",
            GetWindowId(), static_cast<int32_t>(status));
        WMError ret = WMError::WM_OK;
        if (status) {
            RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
            if (ret != WMError::WM_OK) {
                TLOGE(WmsLogTag::WMS_IMMS, "RemoveWindowFlag errCode:%{public}d winId:%{public}u",
                    static_cast<int32_t>(ret), GetWindowId());
                return ret;
            }
        } else {
            AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
            if (ret != WMError::WM_OK) {
                TLOGE(WmsLogTag::WMS_IMMS, "RemoveWindowFlag errCode:%{public}d winId:%{public}u",
                    static_cast<int32_t>(ret), GetWindowId());
                return ret;
            }
        }
        ret = NotifyWindowNeedAvoid(!status);
        if (ret != WMError::WM_OK) {
            TLOGE(WmsLogTag::WMS_IMMS, "NotifyWindowNeedAvoid errCode:%{public}d winId:%{public}u",
                static_cast<int32_t>(ret), GetWindowId());
            return ret;
        }
    }
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetLayoutFullScreen(bool status)
{
    TLOGI(WmsLogTag::WMS_IMMS, "winId:%{public}u %{public}s status:%{public}d",
        GetWindowId(), GetWindowName().c_str(), static_cast<int32_t>(status));
    if (WindowHelper::IsSystemWindow(GetType())) {
        TLOGI(WmsLogTag::WMS_IMMS, "system window is not supported");
        return WMError::WM_OK;
    }
    
    bool preStatus = property_->IsLayoutFullScreen();
    property_->SetIsLayoutFullScreen(status);
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE);

    if (WindowHelper::IsMainWindow(GetType()) &&
        system::GetParameter("const.product.devicetype", "unknown") != "phone" &&
        system::GetParameter("const.product.devicetype", "unknown") != "tablet") {
        if (hostSession_ == nullptr) {
            TLOGE(WmsLogTag::WMS_IMMS, "hostSession is null");
            return WMError::WM_ERROR_NULLPTR;
        }
        if (!WindowHelper::IsWindowModeSupported(property_->GetModeSupportInfo(), WindowMode::WINDOW_MODE_FULLSCREEN)) {
            TLOGE(WmsLogTag::WMS_IMMS, "fullscreen window mode is not supported");
            return WMError::WM_ERROR_INVALID_WINDOW;
        }
        hostSession_->OnSessionEvent(SessionEvent::EVENT_MAXIMIZE);
        SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    }

    WMError ret = SetLayoutFullScreenByApiVersion(status);
    if (ret != WMError::WM_OK) {
        property_->SetIsLayoutFullScreen(preStatus);
        TLOGE(WmsLogTag::WMS_IMMS, "SetLayoutFullScreenByApiVersion errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), GetWindowId());
    }

    return ret;
}

bool WindowSceneSessionImpl::IsLayoutFullScreen() const
{
    return isIgnoreSafeArea_;
}

SystemBarProperty WindowSceneSessionImpl::GetSystemBarPropertyByType(WindowType type) const
{
    TLOGD(WmsLogTag::WMS_IMMS, "GetSystemBarPropertyByType windowId:%{public}u type:%{public}u",
        GetWindowId(), static_cast<uint32_t>(type));
    if (property_ == nullptr) {
        return SystemBarProperty();
    }
    auto curProperties = property_->GetSystemBarProperty();
    return curProperties[type];
}

WMError WindowSceneSessionImpl::NotifyWindowSessionProperty()
{
    TLOGD(WmsLogTag::WMS_IMMS, "windowId:%{public}u", GetWindowId());
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_IMMS, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS);
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::NotifySpecificWindowSessionProperty(WindowType type, const SystemBarProperty& property)
{
    TLOGD(WmsLogTag::WMS_IMMS, "windowId:%{public}u", GetWindowId());
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_IMMS, "session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (type == WindowType::WINDOW_TYPE_STATUS_BAR) {
        UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_STATUS_PROPS);
    } else if (type == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_PROPS);
    } else if (type == WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR) {
        UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_INDICATOR_PROPS);
    }
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::SetSpecificBarProperty(WindowType type, const SystemBarProperty& property)
{
    if (!WindowHelper::IsMainWindow(GetType())) {
        TLOGI(WmsLogTag::WMS_IMMS, "only main window support");
        return WMError::WM_OK;
    }
    if (!((state_ > WindowState::STATE_INITIAL) && (state_ < WindowState::STATE_BOTTOM))) {
        TLOGE(WmsLogTag::WMS_IMMS, "windowId:%{public}u state is invalid", GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW;
    } else if (GetSystemBarPropertyByType(type) == property) {
        setSameSystembarPropertyCnt_++;
        TLOGI(WmsLogTag::WMS_IMMS, "windowId:%{public}u %{public}s set same property %{public}u times, "
            "type:%{public}u, enable:%{public}u bgColor:%{public}x Color:%{public}x enableAnim:%{public}u",
            GetWindowId(), GetWindowName().c_str(), setSameSystembarPropertyCnt_,
            static_cast<uint32_t>(type), property.enable_, property.backgroundColor_, property.contentColor_,
            property.enableAnimation_);
        return WMError::WM_OK;
    }
    setSameSystembarPropertyCnt_ = 0;
    TLOGI(WmsLogTag::WMS_IMMS, "windowId:%{public}u %{public}s type:%{public}u, "
        "enable:%{public}u bgColor:%{public}x Color:%{public}x enableAnim:%{public}u settingFlag:%{public}u",
        GetWindowId(), GetWindowName().c_str(), static_cast<uint32_t>(type), property.enable_,
        property.backgroundColor_, property.contentColor_, property.enableAnimation_, property.settingFlag_);

    if (property_ == nullptr) {
        TLOGE(WmsLogTag::WMS_IMMS, "property_ is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    isSystembarPropertiesSet_ = true;
    property_->SetSystemBarProperty(type, property);
    WMError ret = NotifySpecificWindowSessionProperty(type, property);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "winId:%{public}u errCode:%{public}d",
            GetWindowId(), static_cast<int32_t>(ret));
    }
    return ret;
}

WMError WindowSceneSessionImpl::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    return SetSpecificBarProperty(type, property);
}

WMError WindowSceneSessionImpl::SetFullScreen(bool status)
{
    TLOGI(WmsLogTag::WMS_IMMS,
        "winId:%{public}u %{public}s status:%{public}d",
        GetWindowId(), GetWindowName().c_str(), static_cast<int32_t>(status));
    if (WindowHelper::IsSystemWindow(GetType())) {
        TLOGI(WmsLogTag::WMS_IMMS, "system window is not supported");
        return WMError::WM_OK;
    }

    bool isSwitchFreeMultiWindow = windowSystemConfig_.freeMultiWindowEnable_ &&
        windowSystemConfig_.freeMultiWindowSupport_;
    if (isSwitchFreeMultiWindow || (WindowHelper::IsMainWindow(GetType()) &&
        system::GetParameter("const.product.devicetype", "unknown") != "phone" &&
        system::GetParameter("const.product.devicetype", "unknown") != "tablet")) {
        if (hostSession_ == nullptr) {
            TLOGE(WmsLogTag::WMS_IMMS, "hostSession is null");
            return WMError::WM_ERROR_NULLPTR;
        }
        if (!WindowHelper::IsWindowModeSupported(property_->GetModeSupportInfo(), WindowMode::WINDOW_MODE_FULLSCREEN)) {
            TLOGE(WmsLogTag::WMS_IMMS, "fullscreen window mode is not supported");
            return WMError::WM_ERROR_INVALID_WINDOW;
        }
        hostSession_->OnSessionEvent(SessionEvent::EVENT_MAXIMIZE);
        SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    };

    WMError ret = SetLayoutFullScreenByApiVersion(status);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "SetLayoutFullScreenByApiVersion errCode:%{public}d winId:%{public}u",
            static_cast<int32_t>(ret), GetWindowId());
    }

    UpdateDecorEnable(true);
    SystemBarProperty statusProperty = GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    statusProperty.enable_ = !status;
    ret = SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, statusProperty);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_IMMS, "SetSystemBarProperty errCode:%{public}d winId:%{public}u",
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
    WindowType windowType = GetType();
    bool isMainWindow = WindowHelper::IsMainWindow(windowType);
    bool isSubWindow = WindowHelper::IsSubWindow(windowType);
    bool isDialogWindow = WindowHelper::IsDialogWindow(windowType);
    bool isValidWindow = isMainWindow ||
        ((isSubWindow || isDialogWindow) && property_->IsDecorEnable());
    bool isWindowModeSupported = WindowHelper::IsWindowModeSupported(
        windowSystemConfig_.decorModeSupportInfo_, GetMode());
    if (windowSystemConfig_.freeMultiWindowSupport_) {
        return isValidWindow && windowSystemConfig_.isSystemDecorEnable_;
    }
    bool enable = isValidWindow && windowSystemConfig_.isSystemDecorEnable_ &&
        isWindowModeSupported;
    WLOGFD("get decor enable %{public}d", enable);
    return enable;
}

WMError WindowSceneSessionImpl::Minimize()
{
    WLOGFI("WindowSceneSessionImpl::Minimize id: %{public}d", GetPersistentId());
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
    WLOGFI("WindowSceneSessionImpl::Maximize id: %{public}d", GetPersistentId());
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (WindowHelper::IsMainWindow(GetType())) {
        SetFullScreen(true);
    }
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::Maximize(MaximizeLayoutOption option)
{
    if (option.dock != ShowType::HIDE || option.decor == ShowType::FORBIDDEN) {
        WLOGE("[WMLayout] dock cannot be hide always! dock is not hide: %{public}d", option.dock != ShowType::HIDE);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (!WindowHelper::IsMainWindow(GetType())) {
        WLOGFE("maximize fail, not main window");
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    if (!WindowHelper::IsWindowModeSupported(property_->GetModeSupportInfo(), WindowMode::WINDOW_MODE_FULLSCREEN)) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    property_->SetIsLayoutFullScreen(option.decor == ShowType::HIDE);
    return SetFullScreen(true);
}

WMError WindowSceneSessionImpl::MaximizeFloating()
{
    WLOGFI("WindowSceneSessionImpl::MaximizeFloating id: %{public}d", GetPersistentId());
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
        NotifyWindowStatusChange(GetMode());
    }
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE);

    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::Recover()
{
    WLOGFI("WindowSceneSessionImpl::Recover id: %{public}d", GetPersistentId());
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
        NotifyWindowStatusChange(GetMode());
    } else {
        WLOGFE("recovery is invalid on sub window");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::Recover(uint32_t reason)
{
    WLOGFI("WindowSceneSessionImpl::Recover id: %{public}d, reason:%{public}u", GetPersistentId(), reason);
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto isPC = system::GetParameter("const.product.devicetype", "unknown") == "2in1";
    bool isFreeMutiWindowMode = windowSystemConfig_.freeMultiWindowSupport_ &&
        windowSystemConfig_.freeMultiWindowEnable_;
    if (!(isPC || isFreeMutiWindowMode)) {
        WLOGFE("The device is not supported");
        return WMError::WM_ERROR_DEVICE_NOT_SUPPORT;
    }
    if (WindowHelper::IsMainWindow(GetType()) && hostSession_) {
        if (property_->GetMaximizeMode() == MaximizeMode::MODE_RECOVER &&
            property_->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING) {
            WLOGFW("Recover fail, already MODE_RECOVER");
            return WMError::WM_ERROR_REPEAT_OPERATION;
        }
        hostSession_->OnSessionEvent(SessionEvent::EVENT_RECOVER);
        // need notify arkui maximize mode change
        if (reason == 1 && property_->GetMaximizeMode() == MaximizeMode::MODE_AVOID_SYSTEM_BAR) {
            UpdateMaximizeMode(MaximizeMode::MODE_RECOVER);
        }
        SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        property_->SetMaximizeMode(MaximizeMode::MODE_RECOVER);
        UpdateDecorEnable(true);
        UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE);
        NotifyWindowStatusChange(GetMode());
    } else {
        WLOGFE("recovery is invalid on sub window");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    return WMError::WM_OK;
}

void WindowSceneSessionImpl::StartMove()
{
    WLOGFI("WindowSceneSessionImpl::StartMove id：%{public}d", GetPersistentId());
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return;
    }
    WindowType windowType = GetType();
    bool isMainWindow = WindowHelper::IsMainWindow(windowType);
    bool isSubWindow = WindowHelper::IsSubWindow(windowType);
    bool isDialogWindow = WindowHelper::IsDialogWindow(windowType);
    bool isDecorDialog = isDialogWindow && property_->IsDecorEnable();
    bool isFreeMutiWindowMode = windowSystemConfig_.freeMultiWindowSupport_ &&
        windowSystemConfig_.freeMultiWindowEnable_;
    auto isPC = system::GetParameter("const.product.devicetype", "unknown") == "2in1";
    bool isValidWindow = isMainWindow || ((isPC || isFreeMutiWindowMode) && (isSubWindow || isDecorDialog));
    if (isValidWindow && hostSession_) {
        hostSession_->OnSessionEvent(SessionEvent::EVENT_START_MOVE);
    }
    return;
}

WMError WindowSceneSessionImpl::Close()
{
    WLOGFI("WindowSceneSessionImpl::Close id: %{public}d", GetPersistentId());
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    WindowType windowType = GetType();
    bool isSubWindow = WindowHelper::IsSubWindow(windowType);
    bool isSystemSubWindow = WindowHelper::IsSystemSubWindow(windowType);
    bool isDialogWindow = WindowHelper::IsDialogWindow(windowType);
    if (WindowHelper::IsMainWindow(windowType)) {
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
    } else if (isSubWindow || isSystemSubWindow || isDialogWindow) {
        WLOGFI("WindowSceneSessionImpl::Close subwindow or dialog");
        return Destroy(true);
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
    } else {
        if (uiContent_ != nullptr) {
            WLOGFD("Transfer back event to uiContent");
            isConsumed = uiContent_->ProcessBackPressed();
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
    wptr<WindowSceneSessionImpl> weak = this;
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
    WLOGFI("WindowSceneSessionImpl::SetGlobalMaximizeMode %{public}u", static_cast<uint32_t>(mode));
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

WMError WindowSceneSessionImpl::SetGrayScale(float grayScale)
{
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    constexpr float eps = 1e-6f;
    if (grayScale < (MIN_GRAY_SCALE - eps) || grayScale > (MAX_GRAY_SCALE + eps)) {
        WLOGFE("invalid grayScale value: %{public}f", grayScale);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (uiContent_ == nullptr) {
        WLOGFE("uicontent is empty");
        return WMError::WM_ERROR_NULLPTR;
    }

    uiContent_->SetContentNodeGrayScale(grayScale);

    WLOGI("Set window gray scale success, grayScale: %{public}f", grayScale);
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
    if (flag == WindowFlag::WINDOW_FLAG_HANDWRITING && !SessionPermission::IsSystemCalling()) {
        WLOGI("Can not add window flag WINDOW_FLAG_HANDWRITING");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
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
            WLOGD("[GetTopWin] Find MainWinId:%{public}u.", mainWinId);
            break;
        }
    }
    WLOGFD("[GetTopWin] mainId: %{public}u!", mainWinId);
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

sptr<Window> WindowSceneSessionImpl::GetMainWindowWithContext(const std::shared_ptr<AbilityRuntime::Context>& context)
{
    std::shared_lock<std::shared_mutex> lock(windowSessionMutex_);
    if (windowSessionMap_.empty()) {
        TLOGE(WmsLogTag::WMS_MAIN, "Please create mainWindow First!");
        return nullptr;
    }
    for (const auto& winPair : windowSessionMap_) {
        auto win = winPair.second.second;
        if (win && WindowHelper::IsMainWindow(win->GetType()) && context.get() == win->GetContext().get()) {
            TLOGI(WmsLogTag::WMS_MAIN, "Find MainWinId:%{public}u.", win->GetWindowId());
            return win;
        }
    }
    TLOGE(WmsLogTag::WMS_MAIN, "Cannot find Window!");
    return nullptr;
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


sptr<WindowSessionImpl> WindowSceneSessionImpl::GetWindowWithId(uint32_t winId)
{
    std::shared_lock<std::shared_mutex> lock(windowSessionMutex_);
    if (windowSessionMap_.empty()) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "Please create mainWindow First!");
        return nullptr;
    }
    for (const auto& winPair : windowSessionMap_) {
        auto win = winPair.second.second;
        if (win && winId == win->GetWindowId()) {
            return win;
        }
    }
    TLOGE(WmsLogTag::WMS_SYSTEM, "Cannot find Window!");
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
        if (display == nullptr || display->GetDisplayInfo() == nullptr) {
            WLOGFE("get display failed displayId:%{public}" PRIu64"", property_->GetDisplayId());
            return WMError::WM_ERROR_INVALID_PARAM;
        }
        surfaceNode_->SetBackgroundFilter(RSFilter::CreateMaterialFilter(
            static_cast<int>(blurStyle), GetVirtualPixelRatio(display->GetDisplayInfo())));
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
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ArkUI:NotifyMemoryLevel");
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
        TLOGE(WmsLogTag::WMS_SYSTEM, "register animation transition controller permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "listener is nullptr");
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
            TLOGI(WmsLogTag::WMS_SYSTEM, "AnimationForShown excute sucess  %{public}d!", property->GetPersistentId());
        });
    }
    TLOGI(WmsLogTag::WMS_SYSTEM, "RegisterAnimationTransitionController %{public}d!", property_->GetPersistentId());
    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::UpdateSurfaceNodeAfterCustomAnimation(bool isAdd)
{
    TLOGI(WmsLogTag::WMS_SYSTEM, "id: %{public}d, isAdd:%{public}u", property_->GetPersistentId(), isAdd);
    if (IsWindowSessionInvalid()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!WindowHelper::IsSystemWindow(property_->GetWindowType())) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "only system window can set");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    // set no custom after customAnimation
    WMError ret = UpdateAnimationFlagProperty(false);
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "UpdateAnimationFlagProperty failed!");
        return ret;
    }
    ret = static_cast<WMError>(hostSession_->UpdateWindowSceneAfterCustomAnimation(isAdd));
    return ret;
}

void WindowSceneSessionImpl::AdjustWindowAnimationFlag(bool withAnimation)
{
    if (IsWindowSessionInvalid()) {
        WLOGW("[WMSCom]AdjustWindowAnimationFlag failed since session invalid!");
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
    TLOGI(WmsLogTag::WMS_DIALOG, "id: %{public}d", persistentId);
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

WmErrorCode WindowSceneSessionImpl::KeepKeyboardOnFocus(bool keepKeyboardFlag)
{
    if (property_ == nullptr) {
        return WmErrorCode::WM_ERROR_STATE_ABNORMALLY;
    }
    property_->KeepKeyboardOnFocus(keepKeyboardFlag);

    return WmErrorCode::WM_OK;
}

WMError WindowSceneSessionImpl::SetCallingWindow(uint32_t callingSessionId)
{
    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Set calling session id failed, window session is invalid!");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    if (property_ == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Set calling session id failed, property_ is nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (callingSessionId != property_->GetCallingSessionId()) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Set calling session id form %{public}d to: %{public}d",
            property_->GetCallingSessionId(), callingSessionId);
    }
    if (hostSession_) {
        hostSession_->SetCallingSessionId(callingSessionId);
    }
    property_->SetCallingSessionId(callingSessionId);
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
    if (system::GetParameter("const.product.devicetype", "unknown") == "2in1") {
        if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) {
            surfaceNode_->SetFrameGravity(Gravity::LEFT);
        } else if (mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
            surfaceNode_->SetFrameGravity(Gravity::RIGHT);
        } else if (mode == WindowMode::WINDOW_MODE_FLOATING) {
            surfaceNode_->SetFrameGravity(Gravity::TOP_LEFT);
        } else if (mode == WindowMode::WINDOW_MODE_FULLSCREEN) {
            ret = SetLayoutFullScreenByApiVersion(true);
            if (ret != WMError::WM_OK) {
                TLOGE(WmsLogTag::WMS_IMMS, "SetLayoutFullScreenByApiVersion errCode:%{public}d winId:%{public}u",
                    static_cast<int32_t>(ret), GetWindowId());
            }
            SystemBarProperty statusProperty = GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
            statusProperty.enable_ = false;
            ret = SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, statusProperty);
            if (ret != WMError::WM_OK) {
                WLOGFE("SetSystemBarProperty errCode:%{public}d winId:%{public}u",
                    static_cast<int32_t>(ret), GetWindowId());
            }
        }
    }
    return static_cast<WSError>(ret);
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
        UpdateDecorEnable(true, mode);
        if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
            property_->SetMaximizeMode(MaximizeMode::MODE_RECOVER);
        }
    }
    NotifyWindowStatusChange(mode);
    return WMError::WM_OK;
}

WSError WindowSceneSessionImpl::UpdateMaximizeMode(MaximizeMode mode)
{
    WLOGFI("UpdateMaximizeMode %{public}u mode %{public}u", GetWindowId(), static_cast<uint32_t>(mode));
    if (uiContent_ == nullptr) {
        WLOGFE("UpdateMaximizeMode uiContent_ is null");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    property_->SetMaximizeMode(mode);
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

WSError WindowSceneSessionImpl::SwitchFreeMultiWindow(bool enable)
{
    if (IsWindowSessionInvalid()) {
        return WSError::WS_ERROR_INVALID_WINDOW;
    }
    windowSystemConfig_.freeMultiWindowEnable_ = enable;
    return WSError::WS_OK;
}

WMError WindowSceneSessionImpl::NotifyPrepareClosePiPWindow()
{
    TLOGD(WmsLogTag::WMS_PIP, "NotifyPrepareClosePiPWindow type: %{public}u", GetType());
    if (!WindowHelper::IsPipWindow(GetType())) {
        return WMError::WM_DO_NOTHING;
    }
    hostSession_->NotifyPiPWindowPrepareClose();
    return WMError::WM_OK;
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
    if (GetMode() != WindowMode::WINDOW_MODE_FLOATING) {
        TLOGI(WmsLogTag::WMS_LAYOUT, "Fullscreen window could not update new size, winId: %{public}u", GetWindowId());
        return;
    }
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
    WLOGFI("SetWindowLimits WinId:%{public}u, minWidth:%{public}u, minHeight:%{public}u, "
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
        WLOGFE("windowType not support. WinId:%{public}u, WindowType:%{public}u",
            GetWindowId(), static_cast<uint32_t>(windowType));
        return WMError::WM_ERROR_INVALID_CALLING;
    }

    if (property_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }

    const auto& customizedLimits = property_->GetWindowLimits();
    uint32_t minWidth = windowLimits.minWidth_ ? windowLimits.minWidth_ : customizedLimits.minWidth_;
    uint32_t minHeight = windowLimits.minHeight_ ? windowLimits.minHeight_ : customizedLimits.minHeight_;
    uint32_t maxWidth = windowLimits.maxWidth_ ? windowLimits.maxWidth_ : customizedLimits.maxWidth_;
    uint32_t maxHeight = windowLimits.maxHeight_ ? windowLimits.maxHeight_ : customizedLimits.maxHeight_;

    property_->SetUserWindowLimits({
        maxWidth, maxHeight, minWidth, minHeight, customizedLimits.maxRatio_, customizedLimits.minRatio_
    });
    userLimitsSet_ = true;
    UpdateWindowSizeLimits();
    WMError ret = UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS);
    if (ret != WMError::WM_OK) {
        WLOGFE("update window proeprty failed! id: %{public}u.", GetWindowId());
        return ret;
    }
    UpdateNewSize();

    fillWindowLimits(windowLimits);
    return WMError::WM_OK;
}

void WindowSceneSessionImpl::fillWindowLimits(WindowLimits& windowLimits)
{
    const auto& newLimits = property_->GetWindowLimits();
    windowLimits.minWidth_ = newLimits.minWidth_;
    windowLimits.minHeight_ = newLimits.minHeight_;
    windowLimits.maxWidth_ = newLimits.maxWidth_;
    windowLimits.maxHeight_ = newLimits.maxHeight_;
    WLOGFI("SetWindowLimits success! WinId:%{public}u, minWidth:%{public}u, minHeight:%{public}u, "
        "maxWidth:%{public}u, maxHeight:%{public}u", GetWindowId(), windowLimits.minWidth_,
        windowLimits.minHeight_, windowLimits.maxWidth_, windowLimits.maxHeight_);
}

WSError WindowSceneSessionImpl::NotifyDialogStateChange(bool isForeground)
{
    if (property_ == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "property is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    const auto& type = GetType();
    TLOGI(WmsLogTag::WMS_DIALOG, "state change [name:%{public}s, id:%{public}d, type:%{public}u], state:%{public}u,"
        " requestState:%{public}u, isForeground:%{public}d", property_->GetWindowName().c_str(), GetPersistentId(),
        type, state_, requestState_, static_cast<int32_t>(isForeground));
    if (IsWindowSessionInvalid()) {
        TLOGI(WmsLogTag::WMS_DIALOG, "session is invalid, id:%{public}d", GetPersistentId());
        return WSError::WS_ERROR_INVALID_WINDOW;
    }

    if (isForeground) {
        if (state_ == WindowState::STATE_SHOWN) {
            return WSError::WS_OK;
        }
        if (state_ == WindowState::STATE_HIDDEN) {
            state_ = WindowState::STATE_SHOWN;
            requestState_ = WindowState::STATE_SHOWN;
            NotifyAfterForeground();
        }
    } else {
        if (state_ == WindowState::STATE_HIDDEN) {
            return WSError::WS_OK;
        }
        if (state_ == WindowState::STATE_SHOWN) {
            state_ = WindowState::STATE_HIDDEN;
            requestState_ = WindowState::STATE_HIDDEN;
            NotifyAfterBackground();
        }
    }
    TLOGI(WmsLogTag::WMS_DIALOG, "dialog state change success [name:%{public}s, id:%{public}d, type:%{public}u],"
        " state:%{public}u, requestState:%{public}u", property_->GetWindowName().c_str(), property_->GetPersistentId(),
        type, state_, requestState_);
    return WSError::WS_OK;
}

WMError WindowSceneSessionImpl::SetDefaultDensityEnabled(bool enabled)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "windowId=%{public}d set default density enabled=%{public}d", GetWindowId(), enabled);

    if (IsWindowSessionInvalid()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "window session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    if (!WindowHelper::IsMainWindow(GetType())) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "must be app main window");
        return WMError::WM_ERROR_INVALID_CALLING;
    }

    if (isDefaultDensityEnabled_ == enabled) {
        TLOGI(WmsLogTag::WMS_LAYOUT, "isDefaultDensityEnabled_ not change");
        return WMError::WM_OK;
    }
    isDefaultDensityEnabled_ = enabled;

    std::shared_lock<std::shared_mutex> lock(windowSessionMutex_);
    for (const auto& winPair : windowSessionMap_) {
        auto window = winPair.second.second;
        if (window == nullptr) {
            TLOGE(WmsLogTag::WMS_LAYOUT, "window is nullptr");
            continue;
        }
        TLOGD(WmsLogTag::WMS_LAYOUT, "windowId=%{public}d UpdateDensity", window->GetWindowId());
        window->UpdateDensity();
    }
    return WMError::WM_OK;
}

bool WindowSceneSessionImpl::GetDefaultDensityEnabled()
{
    return isDefaultDensityEnabled_.load();
}

float WindowSceneSessionImpl::GetVirtualPixelRatio(sptr<DisplayInfo> displayInfo)
{
    float vpr = 1.0f;
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "displayInfo is nullptr");
        return vpr;
    }
    bool isDefaultDensityEnabled = false;
    if (WindowHelper::IsMainWindow(GetType())) {
        isDefaultDensityEnabled = GetDefaultDensityEnabled();
    } else {
        auto mainWindow = FindMainWindowWithContext();
        if (mainWindow) {
            isDefaultDensityEnabled = mainWindow->GetDefaultDensityEnabled();
        }
    }
    if (isDefaultDensityEnabled) {
        vpr = displayInfo->GetDefaultVirtualPixelRatio();
    } else {
        vpr = displayInfo->GetVirtualPixelRatio();
    }
    return vpr;
}

WMError WindowSceneSessionImpl::HideNonSecureWindows(bool shouldHide)
{
    return SingletonContainer::Get<WindowAdapter>().AddOrRemoveSecureSession(property_->GetPersistentId(), shouldHide);
}

WMError WindowSceneSessionImpl::SetTextFieldAvoidInfo(double textFieldPositionY, double textFieldHeight)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Set textFieldPositionY: %{public}f, textFieldHeight:%{public}f",
        textFieldPositionY, textFieldHeight);
    property_->SetTextFieldPositionY(textFieldPositionY);
    property_->SetTextFieldHeight(textFieldHeight);
    UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_TEXTFIELD_AVOID_INFO);
    return WMError::WM_OK;
}

std::unique_ptr<Media::PixelMap> WindowSceneSessionImpl::HandleWindowMask(
    const std::vector<std::vector<uint32_t>>& windowMask)
{
    const Rect& windowRect = GetRequestRect();
    uint32_t maskHeight = windowMask.size();
    if (maskHeight == 0) {
        WLOGFE("WindowMask is invalid");
        return nullptr;
    }
    uint32_t maskWidth = windowMask[0].size();
    if ((windowRect.height_ > 0 && windowRect.height_ != maskHeight) ||
        (windowRect.width_ > 0 && windowRect.width_ != maskWidth)) {
        WLOGFE("WindowMask is invalid");
        return nullptr;
    }
    const uint32_t bgraChannel = 4;
    Media::InitializationOptions opts;
    opts.size.width = static_cast<int32_t>(maskWidth);
    opts.size.height = static_cast<int32_t>(maskHeight);
    uint32_t length = maskWidth * maskHeight * bgraChannel;
    uint8_t* data = static_cast<uint8_t*>(malloc(length));
    if (data == nullptr) {
        WLOGFE("data is nullptr");
        return nullptr;
    }
    const uint32_t fullChannel = 255;
    const uint32_t greenChannel = 1;
    const uint32_t redChannel = 2;
    const uint32_t alphaChannel = 3;
    for (uint32_t i = 0; i < maskHeight; i++) {
        for (uint32_t j = 0; j < maskWidth; j++) {
            uint32_t idx = i * maskWidth + j;
            uint32_t channel = windowMask[i][j] > 0 ? fullChannel : 0;
            uint32_t channelIndex = idx * bgraChannel;
            data[channelIndex] = channel; // blue channel
            data[channelIndex + greenChannel] = channel;
            data[channelIndex + redChannel] = fullChannel;
            data[channelIndex + alphaChannel] = channel;
        }
    }
    std::unique_ptr<Media::PixelMap> mask = Media::PixelMap::Create(reinterpret_cast<uint32_t*>(data), length, opts);
    free(data);
    return mask;
}

WMError WindowSceneSessionImpl::SetWindowMask(const std::vector<std::vector<uint32_t>>& windowMask)
{
    WLOGFI("SetWindowMask, WindowId: %{public}u", GetWindowId());
    if (IsWindowSessionInvalid()) {
        WLOGFE("session is invalid");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    sptr<Media::PixelMap> mask = sptr<Media::PixelMap>(HandleWindowMask(windowMask).release());
    if (mask == nullptr) {
        WLOGFE("Failed to create pixelMap of window mask");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    auto rsMask = RSMask::CreatePixelMapMask(std::make_shared<Media::PixelMap>(*mask));
    surfaceNode_->SetCornerRadius(0.0f);
    surfaceNode_->SetShadowRadius(0.0f);
    surfaceNode_->SetAbilityBGAlpha(0);
    surfaceNode_->SetMask(rsMask); // RS interface to set mask
    RSTransaction::FlushImplicitTransaction();

    property_->SetWindowMask(mask);
    property_->SetIsShaped(true);
    return UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_WINDOW_MASK);
}

WMError WindowSceneSessionImpl::RegisterKeyboardPanelInfoChangeListener(
    const sptr<IKeyboardPanelInfoChangeListener>& listener)
{
    std::lock_guard<std::mutex> lockListener(keyboardPanelInfoChangeListenerMutex_);
    if (keyboardPanelInfoChangeListeners_ == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Register keyboard Panel info change listener id: %{public}d",
            GetPersistentId());
        keyboardPanelInfoChangeListeners_ = listener;
    } else {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Keyboard Panel info change, listener already registered");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }

    return WMError::WM_OK;
}

WMError WindowSceneSessionImpl::UnregisterKeyboardPanelInfoChangeListener(
    const sptr<IKeyboardPanelInfoChangeListener>& listener)
{
    std::lock_guard<std::mutex> lockListener(keyboardPanelInfoChangeListenerMutex_);
    keyboardPanelInfoChangeListeners_ = nullptr;
    TLOGI(WmsLogTag::WMS_KEYBOARD, "UnRegister keyboard Panel info change listener id: %{public}d", GetPersistentId());

    return WMError::WM_OK;
}

void WindowSceneSessionImpl::NotifyKeyboardPanelInfoChange(const KeyboardPanelInfo& keyboardPanelInfo)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "isKeyboardPanelShown: %{public}d, gravity: %{public}d"
        ", rect_: [%{public}d, %{public}d, %{public}d, %{public}d]", keyboardPanelInfo.isShowing_,
        keyboardPanelInfo.gravity_, keyboardPanelInfo.rect_.posX_, keyboardPanelInfo.rect_.posY_,
        keyboardPanelInfo.rect_.width_, keyboardPanelInfo.rect_.height_);
    std::lock_guard<std::mutex> lockListener(keyboardPanelInfoChangeListenerMutex_);
    if (keyboardPanelInfoChangeListeners_ && keyboardPanelInfoChangeListeners_.GetRefPtr()) {
        keyboardPanelInfoChangeListeners_.GetRefPtr()->OnKeyboardPanelInfoChanged(keyboardPanelInfo);
    } else {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "keyboardPanelInfoChangeListeners_ is unRegistered");
    }
}

void WindowSceneSessionImpl::UpdateDensity()
{
    if (!userLimitsSet_) {
        UpdateWindowSizeLimits();
        UpdateNewSize();
        WMError ret = UpdateProperty(WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS);
        if (ret != WMError::WM_OK) {
            WLOGFE("update window proeprty failed! id: %{public}u.", GetWindowId());
            return;
        }
    }

    auto preRect = GetRect();
    UpdateViewportConfig(preRect, WindowSizeChangeReason::UNDEFINED);
    WLOGFI("WindowSceneSessionImpl::UpdateDensity [%{public}d, %{public}d, %{public}u, %{public}u]",
        preRect.posX_, preRect.posY_, preRect.width_, preRect.height_);
}

WMError WindowSceneSessionImpl::AdjustKeyboardLayout(const KeyboardLayoutParams& params)
{
    TLOGI(WmsLogTag::WMS_KEYBOARD, "adjust keyboard layout, gravity: %{public}u, LandscapeKeyboardRect: %{public}s, "
        "PortraitKeyboardRect: %{public}s, LandscapePanelRect: %{public}s, PortraitPanelRect: %{public}s",
        static_cast<uint32_t>(params.gravity_), params.LandscapeKeyboardRect_.ToString().c_str(),
        params.PortraitKeyboardRect_.ToString().c_str(), params.LandscapePanelRect_.ToString().c_str(),
        params.PortraitPanelRect_.ToString().c_str());
    if (property_ != nullptr) {
        property_->SetKeyboardSessionGravity(static_cast<SessionGravity>(params.gravity_), 0);
        property_->SetKeyboardLayoutParams(params);
    }
    if (hostSession_ != nullptr) {
        return static_cast<WMError>(hostSession_->AdjustKeyboardLayout(params));
    }
    return WMError::WM_OK;
}
} // namespace Rosen
} // namespace OHOS
