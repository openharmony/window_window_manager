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

#include "session_manager_lite.h"
#include <iremote_stub.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "mock_session_manager_service_interface.h"
#include "session_manager_service_recover_interface.h"
#include "scene_session_manager_lite_proxy.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionManagerLite" };
}

class SessionManagerServiceLiteRecoverListener : public IRemoteStub<ISessionManagerServiceRecoverListener> {
public:
    explicit SessionManagerServiceLiteRecoverListener() = default;

    virtual int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        if (data.ReadInterfaceToken() != GetDescriptor()) {
            WLOGFE("[WMSRecover] InterfaceToken check failed");
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
                WLOGFW("[WMSRecover] unknown transaction code %{public}d", code);
                return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
        return 0;
    }

    void OnSessionManagerServiceRecover(const sptr<IRemoteObject>& sessionManagerService) override
    {
        SessionManagerLite::GetInstance().Clear();
        SessionManagerLite::GetInstance().ClearSessionManagerProxy();

        auto sms = iface_cast<ISessionManagerService>(sessionManagerService);
        SessionManagerLite::GetInstance().RecoverSessionManagerService(sms);
    }

    void OnWMSConnectionChanged(int32_t userId, int32_t screenId, bool isConnected) override
    {
        WLOGFD("OnWMSConnectionChanged lite: %{public}d, %{public}d, %{public}d", userId, screenId, isConnected);
    }
};

class SceneSessionManagerLiteProxyMock : public SceneSessionManagerLiteProxy {
public:
    explicit SceneSessionManagerLiteProxyMock(const sptr<IRemoteObject>& impl)
        : SceneSessionManagerLiteProxy(impl) {}
    virtual ~SceneSessionManagerLiteProxyMock() = default;

    WSError RegisterSessionListener(const sptr<ISessionListener>& listener) override
    {
        WLOGFI("[WMSRecover] RegisterSessionListener");
        auto ret = SceneSessionManagerLiteProxy::RegisterSessionListener(listener);
        if (ret != WSError::WS_OK) {
            return ret;
        }
        SessionManagerLite::GetInstance().SaveSessionListener(listener);
        return WSError::WS_OK;
    }
    WSError UnRegisterSessionListener(const sptr<ISessionListener>& listener) override
    {
        WLOGFI("[WMSRecover] UnRegisterSessionListener");
        auto ret = SceneSessionManagerLiteProxy::UnRegisterSessionListener(listener);
        SessionManagerLite::GetInstance().DeleteSessionListener(listener);
        return ret;
    }
private:
    static inline BrokerDelegator<SceneSessionManagerLiteProxyMock> delegator_;
};

WM_IMPLEMENT_SINGLE_INSTANCE(SessionManagerLite)

SessionManagerLite::~SessionManagerLite()
{
    WLOGFD("SessionManagerLite destroy");
    DeleteAllSessionListeners();
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    destroyed_ = true;
    if (recoverListenerRegistered_ && mockSessionManagerServiceProxy_ != nullptr) {
        mockSessionManagerServiceProxy_->UnregisterSMSLiteRecoverListener();
    }
}

void SessionManagerLite::ClearSessionManagerProxy()
{
    WLOGFD("ClearSessionManagerProxy enter");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (destroyed_) {
        WLOGFI("Already destroyed");
        return;
    }

    sessionManagerServiceProxy_ = nullptr;
    sceneSessionManagerLiteProxy_ = nullptr;
    screenSessionManagerLiteProxy_ = nullptr;
}

sptr<ISceneSessionManagerLite> SessionManagerLite::GetSceneSessionManagerLiteProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    InitSessionManagerServiceProxy();
    InitSceneSessionManagerLiteProxy();
    return sceneSessionManagerLiteProxy_;
}

sptr<IScreenSessionManagerLite> SessionManagerLite::GetScreenSessionManagerLiteProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    InitSessionManagerServiceProxy();
    InitScreenSessionManagerLiteProxy();
    return screenSessionManagerLiteProxy_;
}

void SessionManagerLite::SaveSessionListener(const sptr<ISessionListener>& listener)
{
    std::lock_guard<std::recursive_mutex> guard(listenerLock_);
    auto it = std::find_if(sessionListeners_.begin(), sessionListeners_.end(),
        [&listener](const sptr<ISessionListener>& item) {
            return (item && item->AsObject() == listener->AsObject());
        });
    if (it != sessionListeners_.end()) {
        WLOGFW("[WMSRecover] listener was already added, do not add again");
        return;
    }
    sessionListeners_.emplace_back(listener);
}

void SessionManagerLite::DeleteSessionListener(const sptr<ISessionListener>& listener)
{
    std::lock_guard<std::recursive_mutex> guard(listenerLock_);
    auto it = std::find_if(sessionListeners_.begin(), sessionListeners_.end(),
        [&listener](const sptr<ISessionListener>& item) {
            return (item && item->AsObject() == listener->AsObject());
        });
    if (it != sessionListeners_.end()) {
        sessionListeners_.erase(it);
    }
}

void SessionManagerLite::DeleteAllSessionListeners()
{
    std::lock_guard<std::recursive_mutex> guard(listenerLock_);
    sessionListeners_.clear();
}

void SessionManagerLite::RecoverSessionManagerService(const sptr<ISessionManagerService>& sessionManagerService)
{
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        sessionManagerServiceProxy_ = sessionManagerService;
    }
    GetSceneSessionManagerLiteProxy();
    if (sceneSessionManagerLiteProxy_ == nullptr) {
        WLOGFE("[WMSRecover] sceneSessionManagerLiteProxy_ is null");
        return;
    }

    WLOGFI("[WMSRecover] RecoverSessionListeners, listener count = %{public}" PRIu64,
        static_cast<int64_t>(sessionListeners_.size()));
    for (const auto& listener: sessionListeners_) {
        auto ret = sceneSessionManagerLiteProxy_->RegisterSessionListener(listener);
        if (ret != WSError::WS_OK) {
            WLOGFW("[WMSRecover] RegisterSessionListener failed, ret = %{public}" PRId32, ret);
        }
    }
}

void SessionManagerLite::InitSessionManagerServiceProxy()
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
    if (!mockSessionManagerServiceProxy_) {
        WLOGFW("Get mock session manager service proxy failed, nullptr");
        return;
    }
    if (!recoverListenerRegistered_) {
        recoverListenerRegistered_ = true;
        smsRecoverListener_ = new SessionManagerServiceLiteRecoverListener();
        mockSessionManagerServiceProxy_->RegisterSMSLiteRecoverListener(smsRecoverListener_);
    }
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

void SessionManagerLite::InitScreenSessionManagerLiteProxy()
{
    if (screenSessionManagerLiteProxy_) {
        return;
    }
    if (!mockSessionManagerServiceProxy_) {
        WLOGFE("mockSessionManagerServiceProxy_ is nullptr");
        return;
    }

    sptr<IRemoteObject> remoteObject = mockSessionManagerServiceProxy_->GetScreenSessionManagerLite();
    if (!remoteObject) {
        WLOGFW("Get screen session manager lite proxy failed, scene session manager service is null");
        return;
    }
    screenSessionManagerLiteProxy_ = iface_cast<IScreenSessionManagerLite>(remoteObject);
    if (!screenSessionManagerLiteProxy_) {
        WLOGFW("Get screen session manager lite proxy failed, nullptr");
    }
}

void SessionManagerLite::InitSceneSessionManagerLiteProxy()
{
    if (sceneSessionManagerLiteProxy_) {
        return;
    }
    if (!sessionManagerServiceProxy_) {
        WLOGFE("sessionManagerServiceProxy_ is nullptr");
        return;
    }

    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy_->GetSceneSessionManagerLite();
    if (!remoteObject) {
        WLOGFW("Get scene session manager proxy failed, scene session manager service is null");
        return;
    }
    sceneSessionManagerLiteProxy_ = iface_cast<ISceneSessionManagerLite>(remoteObject);
    if (sceneSessionManagerLiteProxy_) {
        ssmDeath_ = new (std::nothrow) SSMDeathRecipientLite();
        if (!ssmDeath_) {
            WLOGFE("Failed to create death Recipient ptr WMSDeathRecipient");
            return;
        }
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(ssmDeath_)) {
            WLOGFE("Failed to add death recipient");
            return;
        }
    }
    if (!sceneSessionManagerLiteProxy_) {
        WLOGFW("Get scene session manager proxy failed, nullptr");
    }
}

void SessionManagerLite::Clear()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((sceneSessionManagerLiteProxy_ != nullptr) && (sceneSessionManagerLiteProxy_->AsObject() != nullptr)) {
        sceneSessionManagerLiteProxy_->AsObject()->RemoveDeathRecipient(ssmDeath_);
    }
}

void SSMDeathRecipientLite::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("SSMDeathRecipientLite wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("SSMDeathRecipientLite object is null");
        return;
    }
    WLOGI("ssm OnRemoteDied");
    SessionManagerLite::GetInstance().Clear();
    SessionManagerLite::GetInstance().ClearSessionManagerProxy();
}
} // namespace OHOS::Rosen
