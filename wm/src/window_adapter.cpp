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
    return windowManagerServiceProxy_->RegisterWindowManagerAgent(type, windowManagerAgent);
}

WMError WindowAdapter::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    INIT_PROXY_CHECK_RETURN(WMError::WM_ERROR_SAMGR);
    return windowManagerServiceProxy_->UnregisterWindowManagerAgent(type, windowManagerAgent);
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
    SingletonContainer::Get<WindowManager>().OnRemoteDied();
    SingletonContainer::Get<SessionManager>().ClearSessionManagerProxy();
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
    return static_cast<WMError>(windowManagerServiceProxy_->UpdateSessionTouchOutsideListener(persistentId, haveListener));
}
} // namespace Rosen
} // namespace OHOS
