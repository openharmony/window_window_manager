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

#include "ability_manager_client.h"
#include "scene_session_manager_interface.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "SessionManager" };
}

WM_IMPLEMENT_SINGLE_INSTANCE(SessionManager)

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
    auto remoteObject = AAFwk::AbilityManagerClient::GetInstance()->GetSessionManagerService();
    if (!remoteObject) {
        WLOGFE("Remote object is nullptr");
        return;
    }
    sessionManagerServiceProxy_ = iface_cast<ISessionManagerService>(remoteObject);
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
    sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session)
{
    WLOGFD("CreateAndConnectSpecificSession");
    GetSceneSessionManagerProxy();
    if (!sceneSessionManagerProxy_) {
        WLOGFE("sceneSessionManagerProxy_ is nullptr");
        return;
    }
    sceneSessionManagerProxy_->CreateAndConnectSpecificSession(sessionStage, eventChannel,
        surfaceNode, property, persistentId, session);
}

void SessionManager::DestroyAndDisconnectSpecificSession(const uint64_t& persistentId)
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

WMError GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos)
{
    WLOGFD("GetAccessibilityWindowInfo");
    GetSceneSessionManagerProxy();
    if (!sceneSessionManagerProxy_) {
        WLOGFE("sceneSessionManagerProxy_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    return static_cast<WMError>(sceneSessionManagerProxy_->GetAccessibilityWindowInfo(infos));
}
} // namespace OHOS::Rosen
