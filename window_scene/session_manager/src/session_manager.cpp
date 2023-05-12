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

#include <unistd.h>
#include "ability_manager_client.h"
#include "ability_connect_callback_stub.h"
#include "session_manager_service/include/session_manager_service_proxy.h"
#include "window_manager_hilog.h"
#include "zidl/scene_session_manager_proxy.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionManager"};
constexpr int CONNECT_COUNT = 10;
constexpr int SLEEP_US = 100;
}

WM_IMPLEMENT_SINGLE_INSTANCE(SessionManager)

class AbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    void OnAbilityConnectDone(
        const AppExecFwk::ElementName& element, const sptr<IRemoteObject>& remoteObject, int resultCode) override
    {
        remoteObject_ = remoteObject;
        if (remoteObject_ == nullptr) {
            WLOGFW("RemoteObject_ is nullptr");
        }
    }

    void OnAbilityDisconnectDone(const AppExecFwk::ElementName& element, int resultCode) override
    {
        remoteObject_ = nullptr;
    }

    sptr<IRemoteObject> GetRemoteObject() const
    {
        return remoteObject_;
    }
private:
    sptr<IRemoteObject> remoteObject_;
};

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{
}

void SessionManager::Init()
{
    if (!abilityConnection_) {
        ConnectToService();
    }
}

sptr<IRemoteObject> SessionManager::GetRemoteObject()
{
    if (remoteObject_) {
        return remoteObject_;
    }
    if (abilityConnection_) {
        remoteObject_ = abilityConnection_->GetRemoteObject();
        if (remoteObject_) {
            return remoteObject_;
        }
    }

    int count = 0;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    while (remoteObject_ == nullptr && count < CONNECT_COUNT) {
        usleep(SLEEP_US);
        if (abilityConnection_) {
            remoteObject_ = abilityConnection_->GetRemoteObject();
        }
    }
    return remoteObject_;
}

void SessionManager::ConnectToService()
{
    if (!abilityConnection_) {
        abilityConnection_ = new AbilityConnection();
    }

    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard", "com.ohos.sceneboard.MainAbility");
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, abilityConnection_, nullptr);
    if (ret != ERR_OK) {
        WLOGFE("ConnectToService failed, errorcode: %{public}d", ret);
    }
}

void SessionManager::CreateSessionManagerServiceProxy()
{
    sptr<IRemoteObject> remoteObject = GetRemoteObject();
    if (remoteObject) {
        sessionManagerServiceProxy_ = iface_cast<ISessionManagerService>(remoteObject);
        if (!sessionManagerServiceProxy_) {
            WLOGFE("sessionManagerServiceProxy_ is nullptr");
        }
    } else {
        WLOGFE("GetRemoteObject null");
    }
}

void SessionManager::GetSceneSessionManagerProxy()
{
    if (sceneSessionManagerProxy_) {
        return;
    }
    if (!sessionManagerServiceProxy_) {
        WLOGFE("sessionManagerServiceProxy_ is nullptr");
    }

    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy_->GetSceneSessionManager();
    if (!remoteObject) {
        WLOGFW("Get screen session manager proxy failed, screen session manager service is null");
        return;
    }
    sceneSessionManagerProxy_ = iface_cast<ISceneSessionManager>(remoteObject);
    if (!sceneSessionManagerProxy_) {
        WLOGFW("Get screen session manager proxy failed, nullptr");
    }
}

void SessionManager::InitSceneSessionManagerProxy()
{
    WLOGFD("InitSceneSessionManagerProxy");
    Init();
    CreateSessionManagerServiceProxy();
    GetSceneSessionManagerProxy();
}

void SessionManager::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session)
{
    WLOGFD("CreateAndConnectSpecificSession");
    InitSceneSessionManagerProxy();
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
    InitSceneSessionManagerProxy();
    if (!sceneSessionManagerProxy_) {
        WLOGFE("sceneSessionManagerProxy_ is nullptr");
        return;
    }
    sceneSessionManagerProxy_->DestroyAndDisconnectSpecificSession(persistentId);
}
} // namespace OHOS::Rosen