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

#include "mock_session_manager_service_interface.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "SessionManager" };
}

WM_IMPLEMENT_SINGLE_INSTANCE(SessionManager)

SessionManager::~SessionManager()
{
    WLOGFI("SessionManager destory!");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    destroyed_ = true;
    if (mockSessionManagerServiceProxy_ != nullptr) {
        mockSessionManagerServiceProxy_->UnRegisterSessionManagerServiceRecoverListener(getpid());
        mockSessionManagerServiceProxy_ = nullptr;
    }
}

int32_t SessionManager::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    auto msgId = static_cast<SessionManagerServiceRecoverMessage>(code);
    switch (msgId) {
        case SessionManagerServiceRecoverMessage::TRANS_ID_ON_SESSION_MANAGER_SERVICE_RECOVER: {
            auto sessionManagerService = data.ReadRemoteObject();
            OnSessionManagerServiceRecover(sessionManagerService);
            break;
        }
        default:
            WLOGFW("unknown transaction code %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}

void SessionManager::OnSessionManagerServiceRecover(const sptr<IRemoteObject>& sessionManagerService)
{
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        sessionManagerServiceProxy_ = iface_cast<ISessionManagerService>(sessionManagerService);
        sceneSessionManagerProxy_ = nullptr;
    }

    taskScheduler_->PostAsyncTask([this] () {
        WLOGFI("[RECOVER] Run recover task");
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (windowManagerRecoverFunc_ != nullptr) {
            WLOGFI("[RECOVER] windowManagerRecover");
            windowManagerRecoverFunc_();
        }

        for (const auto& it: sessionRecoverCallbackFuncMap_) {
            WLOGFI("[RECOVER] Session recover callback, persistentId = %{public}d", it.first);
            it.second();
        }
    });
}

void SessionManager::ClearSessionManagerProxy()
{
    WLOGFI("ClearSessionManagerProxy enter!");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (destroyed_) {
        WLOGFE("Already destroyed");
        return;
    }

    if (sessionManagerServiceProxy_ != nullptr) {
        int refCount = sessionManagerServiceProxy_->GetSptrRefCount();
        WLOGFI("sessionManagerServiceProxy_ GetSptrRefCount : %{public}d", refCount);
        sessionManagerServiceProxy_ = nullptr;
    }
    sceneSessionManagerProxy_ = nullptr;
}

sptr<ISceneSessionManager> SessionManager::GetSceneSessionManagerProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
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
        WLOGFI("Remote object is nullptr");
        return;
    }
    mockSessionManagerServiceProxy_ = iface_cast<IMockSessionManagerInterface>(remoteObject);
    if (!mockSessionManagerServiceProxy_) {
        WLOGFW("Get mock session manager service proxy failed, nullptr");
        return;
    }

    sptr<IRemoteObject> listener = this;
    mockSessionManagerServiceProxy_->RegisterSessionManagerServiceRecoverListener(getpid(), listener);

    sptr<IRemoteObject> remoteObject2 = mockSessionManagerServiceProxy_->GetSessionManagerService();
    if (!remoteObject2) {
        WLOGFE("Remote object2 is nullptr");
        return;
    }
    sessionManagerServiceProxy_ = iface_cast<ISessionManagerService>(remoteObject2);
    if (!sessionManagerServiceProxy_) {
        WLOGFE("sessionManagerServiceProxy_ is nullptr");
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
    if (sceneSessionManagerProxy_) {
        ssmDeath_ = new (std::nothrow) SSMDeathRecipient();
        if (!ssmDeath_) {
            WLOGFE("Failed to create death Recipient ptr WMSDeathRecipient");
            return;
        }
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(ssmDeath_)) {
            WLOGFE("Failed to add death recipient");
            return;
        }
    }
    if (!sceneSessionManagerProxy_) {
        WLOGFW("Get scene session manager proxy failed, nullptr");
    }
}

void SessionManager::Clear()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((sceneSessionManagerProxy_ != nullptr) && (sceneSessionManagerProxy_->AsObject() != nullptr)) {
        sceneSessionManagerProxy_->AsObject()->RemoveDeathRecipient(ssmDeath_);
    }
}

void SSMDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("SSMDeathRecipient wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("SSMDeathRecipient object is null");
        return;
    }
    WLOGI("ssm OnRemoteDied");
    SessionManager::GetInstance().Clear();
    SessionManager::GetInstance().ClearSessionManagerProxy();
}

void SessionManager::RecoverAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, sptr<ISession>& session, sptr<IRemoteObject> token)
{
    WLOGFI("[RECOVER] RecoverAndConnectSpecificSession");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    GetSceneSessionManagerProxy();
    if (!sceneSessionManagerProxy_) {
        WLOGFE("[RECOVER] sceneSessionManagerProxy_ is nullptr");
        return;
    }
    auto ret = sceneSessionManagerProxy_->RecoverAndConnectSpecificSession(sessionStage, eventChannel,
        surfaceNode, property, session, token);
    if (ret != WSError::WS_OK) {
        WLOGFE("[RECOVER] call ipc failed");
        return;
    }
}

WMError SessionManager::RecoverAndReconnectSceneSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    SystemSessionConfig& systemConfig, sptr<ISession>& session, sptr<WindowSessionProperty> property,
    sptr<IRemoteObject> token, int32_t pid, int32_t uid)
{
    WLOGFD("RecoverAndReconnectSceneSession");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    GetSceneSessionManagerProxy();
    if (!sceneSessionManagerProxy_) {
        WLOGFE("sceneSessionManagerProxy_ is nullptr");
        return WMError::WM_DO_NOTHING;
    }
    auto ret = sceneSessionManagerProxy_->RecoverAndReconnectSceneSession(
        sessionStage, eventChannel, surfaceNode, systemConfig, session, property, token);
    if (ret != WSError::WS_OK) {
        WLOGFE("RecoverAndReconnectSceneSession failed, ret = %{public}d", ret);
        return WMError::WM_DO_NOTHING;
    }

    return WMError::WM_OK;
}

void SessionManager::RegisterSessionRecoverCallbackFunc(
    int32_t persistentId, const SessionRecoverCallbackFunc& callbackFunc)
{
    WLOGFI("[RECOVER] RegisterSessionRecoverCallbackFunc persistentId = %{public}d", persistentId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    sessionRecoverCallbackFuncMap_[persistentId] = callbackFunc;
}

void SessionManager::UnRegisterSessionRecoverCallbackFunc(int32_t persistentId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto it = sessionRecoverCallbackFuncMap_.find(persistentId);
    if (it != sessionRecoverCallbackFuncMap_.end()) {
        sessionRecoverCallbackFuncMap_.erase(it);
    }
}

void SessionManager::RegisterWindowManagerRecoverCallbackFuc(const WindowManagerRecoverCallbackFunc& callbackFunc)
{
    windowManagerRecoverFunc_ = callbackFunc;
}

void SessionManager::UnRegisterWindowManagerRecoverCallbackFuc()
{
    windowManagerRecoverFunc_ = nullptr;
}
} // namespace OHOS::Rosen
