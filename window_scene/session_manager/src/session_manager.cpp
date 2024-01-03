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

#include "session_manager_service_recover_interface.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "SessionManager" };
}

class SessionManagerServiceRecoverListener : public IRemoteStub<ISessionManagerServiceRecoverListener> {
public:
    explicit SessionManagerServiceRecoverListener(SessionManager::WindowManagerRecoverCallbackFunc callbackFunc)
    {
        windowManagerRecoverFunc_ = callbackFunc;
    }

    virtual int32_t OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
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

    void OnSessionManagerServiceRecover(const sptr<IRemoteObject>& sessionManagerService) override
    {
        SessionManager::GetInstance().Clear();
        SessionManager::GetInstance().ClearSessionManagerProxy();

        auto sms = iface_cast<ISessionManagerService>(sessionManagerService);
        SessionManager::GetInstance().RecoverSessionManagerService(sms);

        WLOGFI("[WMSRecover] Run recover");
        if (windowManagerRecoverFunc_ != nullptr) {
            WLOGFD("[WMSRecover] windowManagerRecover");
            windowManagerRecoverFunc_();
        }
    }

private:
    SessionManager::WindowManagerRecoverCallbackFunc windowManagerRecoverFunc_;
};

WM_IMPLEMENT_SINGLE_INSTANCE(SessionManager)

SessionManager::~SessionManager()
{
    WLOGFI("SessionManager destory!");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    destroyed_ = true;
    if (mockSessionManagerServiceProxy_ != nullptr) {
        mockSessionManagerServiceProxy_->UnregisterSessionManagerServiceRecoverListener();
        mockSessionManagerServiceProxy_ = nullptr;
    }
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

__attribute__((no_sanitize("cfi")))
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

__attribute__((no_sanitize("cfi")))
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

void SessionManager::RegisterWindowManagerRecoverCallbackFunc(const WindowManagerRecoverCallbackFunc& callbackFunc)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    InitSessionManagerServiceProxy();
    if (mockSessionManagerServiceProxy_ == nullptr) {
        WLOGFW("MockSessionManagerServiceProxy is nullptr");
        return;
    }

    if (callbackFunc != nullptr) {
        smsRecoverListener_ = new SessionManagerServiceRecoverListener(callbackFunc);
        mockSessionManagerServiceProxy_->RegisterSessionManagerServiceRecoverListener(smsRecoverListener_);
    } else {
        mockSessionManagerServiceProxy_->UnregisterSessionManagerServiceRecoverListener();
    }
}

void SessionManager::RecoverSessionManagerService(const sptr<ISessionManagerService>& sessionManagerService)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    sessionManagerServiceProxy_ = sessionManagerService;
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
} // namespace OHOS::Rosen
