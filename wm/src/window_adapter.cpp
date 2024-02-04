/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "window_adapter.h"
#include <iservice_registry.h>
#include <key_event.h>
#include <system_ability_definition.h>
#include <rs_window_animation_target.h>
#include "window_manager.h"
#include "window_manager_proxy.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "scene_board_judgement.h"
#include "session_manager.h"
#include "focus_change_info.h"
#include <unistd.h>
#include "window_session_impl.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowAdapter"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowAdapter)

#define INIT_PROXY_CHECK_RETURN(ret) \
    do { \
        if (Rosen::SceneBoardJudgement::IsSceneBoardEnabled()) { \
            if (!InitSSMProxy()) { \
                WLOGFE("InitSSMProxy failed!"); \
                return ret; \
            } \
        } else { \
            if (!InitWMSProxy()) { \
                WLOGFE("InitWMSProxy failed!"); \
                return ret; \
            } \
        } \
    } while (false)

WMError WindowAdapter::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& windowProperty,
    std::shared_ptr<RSSurfaceNode> surfaceNode, uint32_t& windowId, const sptr<IRemoteObject>& token)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->CreateWindow(window, windowProperty, surfaceNode, windowId, token);
}

WMError WindowAdapter::AddWindow(sptr<WindowProperty>& windowProperty)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->AddWindow(windowProperty);
}

WMError WindowAdapter::RemoveWindow(uint32_t windowId, bool isFromInnerkits)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->RemoveWindow(windowId, isFromInnerkits);
}

WMError WindowAdapter::DestroyWindow(uint32_t windowId)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->DestroyWindow(windowId);
}

WMError WindowAdapter::RequestFocus(uint32_t windowId)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->RequestFocus(windowId);
}

WMError WindowAdapter::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);

    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (windowManagerAgentMap_.find(type) == windowManagerAgentMap_.end()) {
            windowManagerAgentMap_[type] = std::set<sptr<IWindowManagerAgent>>();
        }
        windowManagerAgentMap_[type].insert(windowManagerAgent);
    }

    return windowManagerServiceProxy_->RegisterWindowManagerAgent(type, windowManagerAgent);
}

WMError WindowAdapter::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    auto ret = windowManagerServiceProxy_->UnregisterWindowManagerAgent(type, windowManagerAgent);

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (windowManagerAgentMap_.find(type) == windowManagerAgentMap_.end()) {
        WLOGFW("WindowManagerAgentType = %{public}d not found", type);
        return ret;
    }

    auto& agentSet = windowManagerAgentMap_[type];
    auto agent = std::find(agentSet.begin(), agentSet.end(), windowManagerAgent);
    if (agent == agentSet.end()) {
        WLOGFW("Cannot find agent,  type = %{public}d", type);
        return ret;
    }
    agentSet.erase(agent);

    return ret;
}

WMError WindowAdapter::CheckWindowId(int32_t windowId, int32_t &pid)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->CheckWindowId(windowId, pid);
}

WMError WindowAdapter::GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->GetAccessibilityWindowInfo(infos);
}

WMError WindowAdapter::GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->GetVisibilityWindowInfo(infos);
}

WMError WindowAdapter::SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->SetWindowAnimationController(controller);
}

WMError WindowAdapter::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type, AvoidArea& avoidArea)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    avoidArea = windowManagerServiceProxy_->GetAvoidAreaByType(windowId, type);
    return WMError::WM_OK;
}

void WindowAdapter::NotifyServerReadyToMoveOrDrag(uint32_t windowId, sptr<WindowProperty>& windowProperty,
    sptr<MoveDragProperty>& moveDragProperty)
{
    INIT_PROXY_CHECK_RETURN();
    return windowManagerServiceProxy_->NotifyServerReadyToMoveOrDrag(windowId, windowProperty, moveDragProperty);
}

void WindowAdapter::ProcessPointDown(uint32_t windowId, bool isPointDown)
{
    INIT_PROXY_CHECK_RETURN();
    return windowManagerServiceProxy_->ProcessPointDown(windowId, isPointDown);
}

void WindowAdapter::ProcessPointUp(uint32_t windowId)
{
    INIT_PROXY_CHECK_RETURN();
    return windowManagerServiceProxy_->ProcessPointUp(windowId);
}

WMError WindowAdapter::MinimizeAllAppWindows(DisplayId displayId)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->MinimizeAllAppWindows(displayId);
}

WMError WindowAdapter::ToggleShownStateForAllAppWindows()
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->ToggleShownStateForAllAppWindows();
}

WMError WindowAdapter::GetSystemConfig(SystemConfig& systemConfig)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->GetSystemConfig(systemConfig);
}

WMError WindowAdapter::GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->GetModeChangeHotZones(displayId, hotZones);
}

bool WindowAdapter::InitWMSProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isProxyValid_) {
        sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            WLOGFE("Failed to get system ability mgr.");
            return false;
        }

        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
        if (!remoteObject) {
            WLOGFE("Failed to get window manager service.");
            return false;
        }

        windowManagerServiceProxy_ = iface_cast<IWindowManager>(remoteObject);
        if ((!windowManagerServiceProxy_) || (!windowManagerServiceProxy_->AsObject())) {
            WLOGFE("Failed to get system window manager services");
            return false;
        }

        wmsDeath_ = new WMSDeathRecipient();
        if (!wmsDeath_) {
            WLOGFE("Failed to create death Recipient ptr WMSDeathRecipient");
            return false;
        }
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(wmsDeath_)) {
            WLOGFE("Failed to add death recipient");
            return false;
        }
        isProxyValid_ = true;
    }
    return true;
}

void WindowAdapter::RegisterSessionRecoverCallbackFunc(
    int32_t persistentId, const SessionRecoverCallbackFunc& callbackFunc)
{
    WLOGFI("[WMSRecover] RegisterSessionRecoverCallbackFunc persistentId = %{public}d", persistentId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    sessionRecoverCallbackFuncMap_[persistentId] = callbackFunc;
}

void WindowAdapter::UnregisterSessionRecoverCallbackFunc(int32_t persistentId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto it = sessionRecoverCallbackFuncMap_.find(persistentId);
    if (it != sessionRecoverCallbackFuncMap_.end()) {
        sessionRecoverCallbackFuncMap_.erase(it);
    }
}

WMError WindowAdapter::RegisterWMSConnectionChangedListener(const WMSConnectionChangedCallbackFunc& callbackFunc)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    WLOGFI("RegisterWMSConnectionChangedListener in");
    SessionManager::GetInstance().RegisterWMSConnectionChangedListener(callbackFunc);
    return WMError::WM_OK;
}

void WindowAdapter::WindowManagerAndSessionRecover()
{
    ClearWindowAdapter();
    if (!InitSSMProxy()) {
        WLOGFE("[WMSRecover] InitSSMProxy failed");
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (const auto& it : windowManagerAgentMap_) {
        WLOGFI("[WMSRecover] RecoverWindowManagerAgents type = %{public}" PRIu32 ", size = %{public}" PRIu64, it.first,
            static_cast<uint64_t>(it.second.size()));
        for (auto& agent : it.second) {
            if (windowManagerServiceProxy_->RegisterWindowManagerAgent(it.first, agent) != WMError::WM_OK) {
                WLOGFE("[WMSRecover] RecoverWindowManagerAgent failed");
            }
        }
    }

    for (const auto& it : sessionRecoverCallbackFuncMap_) {
        WLOGFD("[WMSRecover] Session recover callback, persistentId = %{public}" PRId32, it.first);
        it.second();
    }
}

bool WindowAdapter::InitSSMProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isProxyValid_) {
        windowManagerServiceProxy_ = SessionManager::GetInstance().GetSceneSessionManagerProxy();
        if ((!windowManagerServiceProxy_) || (!windowManagerServiceProxy_->AsObject())) {
            WLOGFE("Failed to get system scene session manager services");
            return false;
        }

        wmsDeath_ = new (std::nothrow) WMSDeathRecipient();
        if (!wmsDeath_) {
            WLOGFE("Failed to create death Recipient ptr WMSDeathRecipient");
            return false;
        }
        sptr<IRemoteObject> remoteObject = windowManagerServiceProxy_->AsObject();
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(wmsDeath_)) {
            WLOGFE("Failed to add death recipient");
            return false;
        }
        if (!recoverInitialized) {
            SessionManager::GetInstance().RegisterWindowManagerRecoverCallbackFunc(
                std::bind(&WindowAdapter::WindowManagerAndSessionRecover, this));
            recoverInitialized = true;
        }
        isProxyValid_ = true;
    }
    return true;
}

void WindowAdapter::ClearWindowAdapter()
{
    if ((windowManagerServiceProxy_ != nullptr) && (windowManagerServiceProxy_->AsObject() != nullptr)) {
        windowManagerServiceProxy_->AsObject()->RemoveDeathRecipient(wmsDeath_);
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    isProxyValid_ = false;
}

void WMSDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("object is null");
        return;
    }
    WLOGI("wms OnRemoteDied");
    SingletonContainer::Get<WindowAdapter>().ClearWindowAdapter();
}

WMError WindowAdapter::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->GetTopWindowId(mainWinId, topWinId);
}

WMError WindowAdapter::SetWindowLayoutMode(WindowLayoutMode mode)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->SetWindowLayoutMode(mode);
}

WMError WindowAdapter::UpdateProperty(sptr<WindowProperty>& windowProperty, PropertyChangeAction action)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->UpdateProperty(windowProperty, action);
}

WMError WindowAdapter::SetWindowGravity(uint32_t windowId, WindowGravity gravity, uint32_t percent)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->SetWindowGravity(windowId, gravity, percent);
}

WMError WindowAdapter::NotifyWindowTransition(sptr<WindowTransitionInfo> from, sptr<WindowTransitionInfo> to)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->NotifyWindowTransition(from, to, true);
}

void WindowAdapter::MinimizeWindowsByLauncher(std::vector<uint32_t> windowIds, bool isAnimated,
    sptr<RSIWindowAnimationFinishedCallback>& finishCallback)
{
    INIT_PROXY_CHECK_RETURN();
    windowManagerServiceProxy_->MinimizeWindowsByLauncher(windowIds, isAnimated, finishCallback);
}

WMError WindowAdapter::UpdateAvoidAreaListener(uint32_t windowId, bool haveListener)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->UpdateAvoidAreaListener(windowId, haveListener);
}

WMError WindowAdapter::UpdateRsTree(uint32_t windowId, bool isAdd)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->UpdateRsTree(windowId, isAdd);
}

WMError WindowAdapter::BindDialogTarget(uint32_t& windowId, sptr<IRemoteObject> targetToken)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->BindDialogTarget(windowId, targetToken);
}

void WindowAdapter::SetAnchorAndScale(int32_t x, int32_t y, float scale)
{
    INIT_PROXY_CHECK_RETURN();
    windowManagerServiceProxy_->SetAnchorAndScale(x, y, scale);
}

void WindowAdapter::SetAnchorOffset(int32_t deltaX, int32_t deltaY)
{
    INIT_PROXY_CHECK_RETURN();
    windowManagerServiceProxy_->SetAnchorOffset(deltaX, deltaY);
}

void WindowAdapter::OffWindowZoom()
{
    INIT_PROXY_CHECK_RETURN();
    windowManagerServiceProxy_->OffWindowZoom();
}

WmErrorCode WindowAdapter::RaiseToAppTop(uint32_t windowId)
{
    INIT_PROXY_CHECK_RETURN(WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY);
    return windowManagerServiceProxy_->RaiseToAppTop(windowId);
}

std::shared_ptr<Media::PixelMap> WindowAdapter::GetSnapshot(int32_t windowId)
{
    INIT_PROXY_CHECK_RETURN(nullptr);
    return windowManagerServiceProxy_->GetSnapshot(windowId);
}

WMError WindowAdapter::SetGestureNavigaionEnabled(bool enable)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->SetGestureNavigaionEnabled(enable);
}

void WindowAdapter::DispatchKeyEvent(uint32_t windowId, std::shared_ptr<MMI::KeyEvent> event)
{
    INIT_PROXY_CHECK_RETURN();
    windowManagerServiceProxy_->DispatchKeyEvent(windowId, event);
}

void WindowAdapter::NotifyDumpInfoResult(const std::vector<std::string>& info)
{
    INIT_PROXY_CHECK_RETURN();
    windowManagerServiceProxy_->NotifyDumpInfoResult(info);
}

WMError WindowAdapter::DumpSessionAll(std::vector<std::string> &infos)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return static_cast<WMError>(windowManagerServiceProxy_->DumpSessionAll(infos));
}

WMError WindowAdapter::DumpSessionWithId(int32_t persistentId, std::vector<std::string> &infos)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return static_cast<WMError>(windowManagerServiceProxy_->DumpSessionWithId(persistentId, infos));
}

WMError WindowAdapter::GetWindowAnimationTargets(std::vector<uint32_t> missionIds,
    std::vector<sptr<RSWindowAnimationTarget>>& targets)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->GetWindowAnimationTargets(missionIds, targets);
}
void WindowAdapter::SetMaximizeMode(MaximizeMode maximizeMode)
{
    INIT_PROXY_CHECK_RETURN();
    windowManagerServiceProxy_->SetMaximizeMode(maximizeMode);
}

MaximizeMode WindowAdapter::GetMaximizeMode()
{
    INIT_PROXY_CHECK_RETURN(MaximizeMode::MODE_FULL_FILL);
    return windowManagerServiceProxy_->GetMaximizeMode();
}

void WindowAdapter::GetFocusWindowInfo(FocusChangeInfo& focusInfo)
{
    INIT_PROXY_CHECK_RETURN();
    return windowManagerServiceProxy_->GetFocusWindowInfo(focusInfo);
}

WMError WindowAdapter::UpdateSessionAvoidAreaListener(int32_t& persistentId, bool haveListener)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_DO_NOTHING);
    return static_cast<WMError>(windowManagerServiceProxy_->UpdateSessionAvoidAreaListener(persistentId, haveListener));
}

WMError WindowAdapter::UpdateSessionTouchOutsideListener(int32_t& persistentId, bool haveListener)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_DO_NOTHING);
    return static_cast<WMError>(
        windowManagerServiceProxy_->UpdateSessionTouchOutsideListener(persistentId, haveListener));
}

void WindowAdapter::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session, sptr<IRemoteObject> token)
{
    INIT_PROXY_CHECK_RETURN();
    windowManagerServiceProxy_->CreateAndConnectSpecificSession(sessionStage, eventChannel,
        surfaceNode, property, persistentId, session, token);
}

void WindowAdapter::RecoverAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, sptr<ISession>& session, sptr<IRemoteObject> token)
{
    INIT_PROXY_CHECK_RETURN();
    windowManagerServiceProxy_->RecoverAndConnectSpecificSession(
        sessionStage, eventChannel, surfaceNode, property, session, token);
}

void WindowAdapter::DestroyAndDisconnectSpecificSession(const int32_t& persistentId)
{
    INIT_PROXY_CHECK_RETURN();
    windowManagerServiceProxy_->DestroyAndDisconnectSpecificSession(persistentId);
}

WMError WindowAdapter::RecoverAndReconnectSceneSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<ISession>& session, sptr<WindowSessionProperty> property, sptr<IRemoteObject> token)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_DO_NOTHING);
    WLOGFD("RecoverAndReconnectSceneSession");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto ret = windowManagerServiceProxy_->RecoverAndReconnectSceneSession(
        sessionStage, eventChannel, surfaceNode, session, property, token);
    if (ret != WSError::WS_OK) {
        WLOGFE("RecoverAndReconnectSceneSession failed, ret = %{public}d", ret);
        return WMError::WM_DO_NOTHING;
    }
    return WMError::WM_OK;
}

WMError WindowAdapter::UpdateSessionProperty(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_DO_NOTHING);
    return windowManagerServiceProxy_->UpdateSessionProperty(property, action);
}

WMError WindowAdapter::SetSessionGravity(int32_t persistentId, SessionGravity gravity, uint32_t percent)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_DO_NOTHING);
    return static_cast<WMError>(windowManagerServiceProxy_->SetSessionGravity(persistentId, gravity, percent));
}

WMError WindowAdapter::BindDialogSessionTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_DO_NOTHING);
    return static_cast<WMError>(windowManagerServiceProxy_->BindDialogSessionTarget(persistentId, targetToken));
}

WMError WindowAdapter::RequestFocusStatus(int32_t persistentId, bool isFocused)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_DO_NOTHING);
    return static_cast<WMError>(windowManagerServiceProxy_->RequestFocusStatus(persistentId, isFocused));
}

WMError WindowAdapter::RaiseWindowToTop(int32_t persistentId)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_DO_NOTHING);
    return static_cast<WMError>(windowManagerServiceProxy_->RaiseWindowToTop(persistentId));
}

WMError WindowAdapter::NotifyWindowExtensionVisibilityChange(int32_t pid, int32_t uid, bool visible)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_DO_NOTHING);
    return static_cast<WMError>(windowManagerServiceProxy_->NotifyWindowExtensionVisibilityChange(pid, uid, visible));
}

WMError WindowAdapter::UpdateSessionWindowVisibilityListener(int32_t persistentId, bool haveListener)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_DO_NOTHING);
    WSError ret = windowManagerServiceProxy_->UpdateSessionWindowVisibilityListener(persistentId, haveListener);
    return static_cast<WMError>(ret);
}

WMError WindowAdapter::ShiftAppWindowFocus(int32_t sourcePersistentId, int32_t targetPersistentId)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_DO_NOTHING);
    return static_cast<WMError>(
        windowManagerServiceProxy_->ShiftAppWindowFocus(sourcePersistentId, targetPersistentId));
}
} // namespace Rosen
} // namespace OHOS
