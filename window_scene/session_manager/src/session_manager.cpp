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
#include <ipc_skeleton.h>

#include "session_manager_service_recover_interface.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"
#include "session_manager_lite.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionManager" };
}

class SessionManagerServiceRecoverListener : public IRemoteStub<ISessionManagerServiceRecoverListener> {
public:
    explicit SessionManagerServiceRecoverListener() = default;

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
            case SessionManagerServiceRecoverMessage::TRANS_ID_ON_WMS_CONNECTION_CHANGED: {
                int32_t userId = data.ReadInt32();
                int32_t screenId = data.ReadInt32();
                bool isConnected = data.ReadBool();
                OnWMSConnectionChanged(userId, screenId, isConnected);
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
    }

    void OnWMSConnectionChanged(int32_t userId, int32_t screenId, bool isConnected) override
    {
        TLOGI(WmsLogTag::WMS_MULTI_USER, "userId=%{public}d, screenId=%{public}d, isConnected=%{public}d", userId,
            screenId, isConnected);
        SessionManager::GetInstance().OnWMSConnectionChanged(userId, screenId, isConnected);
    }
};

WM_IMPLEMENT_SINGLE_INSTANCE(SessionManager)

SessionManager::~SessionManager()
{
    WLOGFI("SessionManager destroy!");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    destroyed_ = true;
    if (mockSessionManagerServiceProxy_ != nullptr) {
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        mockSessionManagerServiceProxy_->UnregisterSMSRecoverListener();
        IPCSkeleton::SetCallingIdentity(identity);
        mockSessionManagerServiceProxy_ = nullptr;
    }
}

void SessionManager::OnWMSConnectionChangedCallback(int32_t userId, int32_t screenId, bool isConnected)
{
    if (wmsConnectionChangedFunc_ != nullptr) {
        TLOGI(WmsLogTag::WMS_MULTI_USER,
            "WMS connection changed with userId=%{public}d, screenId=%{public}d, isConnected=%{public}d", userId,
            screenId, isConnected);
        wmsConnectionChangedFunc_(userId, screenId, isConnected);
    }
}

void SessionManager::OnWMSConnectionChanged(int32_t userId, int32_t screenId, bool isConnected)
{
    TLOGD(WmsLogTag::WMS_MULTI_USER, "WMS connection changed enter");
    if (isConnected && currentWMSUserId_ > INVALID_UID && currentWMSUserId_ != userId) {
        OnUserSwitch();
        // Notify the user that the old wms has been disconnected.
        OnWMSConnectionChangedCallback(currentWMSUserId_, currentScreenId_, false);
    }
    isWMSConnected_ = isConnected;
    currentWMSUserId_ = userId;
    currentScreenId_ = screenId;
    // Notify the user that the current wms connection has changed.
    OnWMSConnectionChangedCallback(userId, screenId, isConnected);
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
    if (InitMockSMSProxy() != WMError::WM_OK) {
        WLOGFE("Init mock session manager service proxy failed");
        return;
    }

    RegisterSMSRecoverListener();

    sessionManagerServiceProxy_ = SessionManagerLite::GetInstance().GetSessionManagerServiceProxy();
    if (!sessionManagerServiceProxy_) {
        WLOGFE("sessionManagerServiceProxy_ is nullptr");
    }
}

WMError SessionManager::InitMockSMSProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        WLOGFE("Failed to get system ability mgr.");
        return WMError::WM_ERROR_NULLPTR;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
    if (!remoteObject) {
        WLOGFE("Remote object is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    mockSessionManagerServiceProxy_ = iface_cast<IMockSessionManagerInterface>(remoteObject);
    if (!mockSessionManagerServiceProxy_) {
        WLOGFW("Get mock session manager service proxy failed, nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (GetUserIdByUid(getuid()) != SYSTEM_USERID || isFoundationListenerRegistered_) {
        return WMError::WM_OK;
    }
    if (!foundationDeath_) {
        foundationDeath_ = new (std::nothrow) FoundationDeathRecipient();
        if (!foundationDeath_) {
            WLOGFE("Failed to create death Recipient ptr FoundationDeathRecipient");
            return WMError::WM_ERROR_NO_MEM;
        }
    }
    if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(foundationDeath_)) {
        WLOGFE("Failed to add death recipient");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    isFoundationListenerRegistered_ = true;
    return WMError::WM_OK;
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

void SessionManager::RegisterSMSRecoverListener()
{
    if (!isRecoverListenerRegistered_) {
        if (!mockSessionManagerServiceProxy_) {
            TLOGE(WmsLogTag::WMS_RECOVER, "mockSessionManagerServiceProxy_ is null");
            return;
        }
        isRecoverListenerRegistered_ = true;
        TLOGI(WmsLogTag::WMS_RECOVER, "Register recover listener");
        smsRecoverListener_ = new SessionManagerServiceRecoverListener();
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        mockSessionManagerServiceProxy_->RegisterSMSRecoverListener(smsRecoverListener_);
        IPCSkeleton::SetCallingIdentity(identity);
    }
}

void SessionManager::RegisterWindowManagerRecoverCallbackFunc(const WindowManagerRecoverCallbackFunc& callbackFunc)
{
    std::lock_guard<std::recursive_mutex> lock(recoverMutex_);
    windowManagerRecoverFunc_ = callbackFunc;
}

void SessionManager::RecoverSessionManagerService(const sptr<ISessionManagerService>& sessionManagerService)
{
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        sessionManagerServiceProxy_ = sessionManagerService;
    }
    
    {
        std::lock_guard<std::recursive_mutex> lock(recoverMutex_);
        TLOGI(WmsLogTag::WMS_RECOVER, "Run recover");
        if (windowManagerRecoverFunc_ != nullptr) {
            TLOGD(WmsLogTag::WMS_RECOVER, "windowManagerRecover");
            windowManagerRecoverFunc_();
        }
    }
}

void SessionManager::OnUserSwitch()
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "User switched");
    Clear();
    ClearSessionManagerProxy();
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        InitSessionManagerServiceProxy();
        InitSceneSessionManagerProxy();
        if (!sceneSessionManagerProxy_) {
            TLOGE(WmsLogTag::WMS_MULTI_USER, "sceneSessionManagerProxy_ is null");
            return;
        }
    }
    if (userSwitchCallbackFunc_) {
        TLOGI(WmsLogTag::WMS_MULTI_USER, "User switch callback.");
        userSwitchCallbackFunc_();
    }
}

void SessionManager::Clear()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((sceneSessionManagerProxy_ != nullptr) && (sceneSessionManagerProxy_->AsObject() != nullptr)) {
        sceneSessionManagerProxy_->AsObject()->RemoveDeathRecipient(ssmDeath_);
    }
}

WMError SessionManager::RegisterWMSConnectionChangedListener(const WMSConnectionChangedCallbackFunc& callbackFunc)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "RegisterWMSConnectionChangedListener in");
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        auto ret = InitMockSMSProxy();
        if (ret != WMError::WM_OK) {
            TLOGE(WmsLogTag::WMS_MULTI_USER, "Init mock session manager service failed");
            return ret;
        }
        RegisterSMSRecoverListener();
    }
    wmsConnectionChangedFunc_ = callbackFunc;
    if (isWMSConnected_ && currentWMSUserId_ > INVALID_USER_ID) {
        OnWMSConnectionChangedCallback(currentWMSUserId_, currentScreenId_, true);
    }
    return WMError::WM_OK;
}

void SessionManager::RegisterUserSwitchListener(const UserSwitchCallbackFunc& callbackFunc)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "Register user switch listener enter");
    userSwitchCallbackFunc_ = callbackFunc;
}

void SessionManager::OnFoundationDied()
{
    TLOGI(WmsLogTag::WMS_RECOVER, "On foundation died enter");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    isFoundationListenerRegistered_ = false;
    isRecoverListenerRegistered_ = false;
    isWMSConnected_ = false;
    mockSessionManagerServiceProxy_ = nullptr;
    sessionManagerServiceProxy_ = nullptr;
    sceneSessionManagerProxy_ = nullptr;
}

void FoundationDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("FoundationDeathRecipient wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("FoundationDeathRecipient object is null");
        return;
    }
    TLOGI(WmsLogTag::WMS_RECOVER, "Foundation died");
    SessionManager::GetInstance().OnFoundationDied();
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
