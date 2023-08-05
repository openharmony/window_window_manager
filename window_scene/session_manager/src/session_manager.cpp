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

#include "session_manager.h"

#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "window_manager_hilog.h"
#include "mock_session_manager_service_interface.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "SessionManager" };
}

WM_IMPLEMENT_SINGLE_INSTANCE(SessionManager)

void SessionManager::ClearSessionManagerProxy()
{
    mockSessionManagerServiceProxy_ = nullptr;
    sessionManagerServiceProxy_ = nullptr;
    sceneSessionManagerProxy_ = nullptr;
    screenSessionManagerProxy_ = nullptr;
    screenLockManagerProxy_ = nullptr;
}

sptr<ScreenLock::ScreenLockManagerInterface> SessionManager::GetScreenLockManagerProxy()
{
    InitSessionManagerServiceProxy();
    InitScreenLockManagerProxy();
    return screenLockManagerProxy_;
}

sptr<IScreenSessionManager> SessionManager::GetScreenSessionManagerProxy()
{
    InitSessionManagerServiceProxy();
    InitScreenSessionManagerProxy();
    return screenSessionManagerProxy_;
}

sptr<ISceneSessionManager> SessionManager::GetSceneSessionManagerProxy()
{
    InitSessionManagerServiceProxy();
    InitSceneSessionManagerProxy();
    return sceneSessionManagerProxy_;
}

void SessionManager::InitSessionManagerServiceProxy()
{
    if (sessionManagerServiceProxy_) {
        return;
    }
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
            WLOGFE("Failed to get system ability mgr.");
            return;
        }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
    if (!remoteObject) {
        WLOGFE("Remote object is nullptr");
        return;
    }
    mockSessionManagerServiceProxy_ = iface_cast<IMockSessionManagerInterface>(remoteObject);
    sessionManagerServiceProxy_ = iface_cast<ISessionManagerService>(
            mockSessionManagerServiceProxy_->GetSessionManagerService());
    if (!sessionManagerServiceProxy_) {
        WLOGFE("sessionManagerServiceProxy_ is nullptr");
    }
}

void SessionManager::InitScreenSessionManagerProxy()
{
    if (screenSessionManagerProxy_) {
        return;
    }
    if (!sessionManagerServiceProxy_) {
        WLOGFW("Get screen session manager proxy failed, sessionManagerServiceProxy_ is nullptr");
        return;
    }
    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy_->GetScreenSessionManagerService();
    if (!remoteObject) {
        WLOGFW("Get screen session manager proxy failed, screen session manager service is null");
        return;
    }
    screenSessionManagerProxy_ = iface_cast<IScreenSessionManager>(remoteObject);
    if (!screenSessionManagerProxy_) {
        WLOGFW("Get screen session manager proxy failed, nullptr");
    }
}

void SessionManager::InitSceneSessionManagerProxy()
{
    if (sceneSessionManagerProxy_) {
        return;
    }
    if (!sessionManagerServiceProxy_) {
        WLOGFE("sessionManagerServiceProxy_ is nullptr");
        return;
    }

    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy_->GetSceneSessionManager();
    if (!remoteObject) {
        WLOGFW("Get scene session manager proxy failed, scene session manager service is null");
        return;
    }
    sceneSessionManagerProxy_ = iface_cast<ISceneSessionManager>(remoteObject);
    if (!sceneSessionManagerProxy_) {
        WLOGFW("Get scene session manager proxy failed, nullptr");
    }
}

void SessionManager::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session, sptr<IRemoteObject> token)
{
    WLOGFD("CreateAndConnectSpecificSession");
    GetSceneSessionManagerProxy();
    if (!sceneSessionManagerProxy_) {
        WLOGFE("sceneSessionManagerProxy_ is nullptr");
        return;
    }
    sceneSessionManagerProxy_->CreateAndConnectSpecificSession(sessionStage, eventChannel,
        surfaceNode, property, persistentId, session, token);
}

void SessionManager::DestroyAndDisconnectSpecificSession(const int32_t& persistentId)
{
    WLOGFD("DestroyAndDisconnectSpecificSession");
    GetSceneSessionManagerProxy();
    if (!sceneSessionManagerProxy_) {
        WLOGFE("sceneSessionManagerProxy_ is nullptr");
        return;
    }
    sceneSessionManagerProxy_->DestroyAndDisconnectSpecificSession(persistentId);
}

WMError SessionManager::UpdateProperty(sptr<WindowSessionProperty>& property, WSPropertyChangeAction action)
{
    WLOGFD("UpdateProperty");
    InitSceneSessionManagerProxy();
    if (!sceneSessionManagerProxy_) {
        WLOGFE("sceneSessionManagerProxy_ is nullptr");
        return WMError::WM_DO_NOTHING;
    }
    return static_cast<WMError>(sceneSessionManagerProxy_->UpdateProperty(property, action));
}

WMError SessionManager::SetSessionGravity(int32_t persistentId, SessionGravity gravity, uint32_t percent)
{
    WLOGFD("SetWindowGravity");
    InitSceneSessionManagerProxy();
    if (!sceneSessionManagerProxy_) {
        WLOGFE("sceneSessionManagerProxy_ is nullptr");
        return WMError::WM_DO_NOTHING;
    }
    return static_cast<WMError>(sceneSessionManagerProxy_->SetSessionGravity(persistentId, gravity, percent));
}

WMError SessionManager::BindDialogTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken)
{
    WLOGFD("BindDialogTarget");
    InitSceneSessionManagerProxy();
    if (!sceneSessionManagerProxy_) {
        WLOGFE("sceneSessionManagerProxy_ is nullptr");
        return WMError::WM_DO_NOTHING;
    }
    return static_cast<WMError>(sceneSessionManagerProxy_->BindDialogTarget(persistentId, targetToken));
}

void SessionManager::InitScreenLockManagerProxy()
{
    if (screenLockManagerProxy_) {
        return;
    }
    if (!sessionManagerServiceProxy_) {
        WLOGFE("Get screen session manager proxy failed, sessionManagerServiceProxy_ is nullptr");
        return;
    }
    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy_->GetScreenLockManagerService();
    if (!remoteObject) {
        WLOGFE("Get screenlock manager proxy failed, screenlock manager service is null");
        return;
    }

    screenLockManagerProxy_ = iface_cast<ScreenLock::ScreenLockManagerInterface>(remoteObject);
    if (!screenLockManagerProxy_) {
        WLOGFW("Get screenlock manager proxy failed, nullptr");
    }
}
} // namespace OHOS::Rosen
