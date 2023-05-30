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
#include <condition_variable>

#include "zidl/screen_session_manager_proxy.h"

#include "ability_manager_client.h"
#include "ability_connect_callback_stub.h"
#include "session_manager_service/include/session_manager_service_proxy.h"
#include "window_manager_hilog.h"
#include "zidl/scene_session_manager_proxy.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SessionManager"};
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
        cv_.notify_all();
    }

    void OnAbilityDisconnectDone(const AppExecFwk::ElementName& element, int resultCode) override
    {
        remoteObject_ = nullptr;
    }

    sptr<IRemoteObject> GetRemoteObject()
    {
        if (!remoteObject_) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (cv_.wait_for(lock, std::chrono::seconds(1)) == std::cv_status::timeout) {
                WLOGFW("Get remote object timeout.");
            }
        }

        return remoteObject_;
    }
private:
    sptr<IRemoteObject> remoteObject_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{
}

void SessionManager::Init()
{
    if (!serviceConnected_) {
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
    } else {
        Init();
    }

    if (!remoteObject_) {
        WLOGFE("Get session manager service remote object nullptr");
    }
    return remoteObject_;
}

sptr<IScreenSessionManager> SessionManager::GetScreenSessionManagerProxy()
{
    Init();
    InitSessionManagerServiceProxy();
    InitScreenSessionManagerProxy();

    return screenSessionManagerProxy_;
}

void SessionManager::ConnectToService()
{
    if (!abilityConnection_) {
        abilityConnection_ = new(std::nothrow) AbilityConnection();
    }

    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard", "com.ohos.sceneboard.MainAbility");
    ErrCode ret = AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, abilityConnection_, nullptr);
    if (ret != ERR_OK) {
        WLOGFE("ConnectToService failed, errorcode: %{public}d", ret);
    }
    serviceConnected_ = (ret == ERR_OK);
}

void SessionManager::InitSessionManagerServiceProxy()
{
    if (sessionManagerServiceProxy_) {
        return;
    }
    sptr<IRemoteObject> remoteObject = GetRemoteObject();
    if (remoteObject) {
        sessionManagerServiceProxy_ = iface_cast<ISessionManagerService>(remoteObject);
        if (!sessionManagerServiceProxy_) {
            WLOGFE("sessionManagerServiceProxy_ is nullptr");
        }
    }
}

void SessionManager::GetSceneSessionManagerProxy()
{
    if (sceneSessionManagerProxy_) {
        return;
    }
    if (!sessionManagerServiceProxy_) {
        WLOGFE("sessionManagerServiceProxy_ is nullptr");
        return;
    }

    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy_->GetSceneSessionManager();
    sceneSessionManagerProxy_ = iface_cast<ISceneSessionManager>(remoteObject);
    if (!sceneSessionManagerProxy_) {
        WLOGFW("Get scene session manager proxy failed, nullptr");
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
    WLOGFD("InitSceneSessionManagerProxy");
    Init();
    InitSessionManagerServiceProxy();
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