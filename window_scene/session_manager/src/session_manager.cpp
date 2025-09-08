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

#include "window_manager_hilog.h"
#include "session_manager_lite.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "SessionManager" };
}
std::unordered_map<int32_t, sptr<SessionManager>> SessionManager::sessionManagerMap_ = {};
std::mutex SessionManager::sessionManagerMapMutex_;

class SessionManagerServiceRecoverListener : public IRemoteStub<ISessionManagerServiceRecoverListener> {
public:
    explicit SessionManagerServiceRecoverListener(sptr<SessionManager> sessionManager)
        : sessionManager_(sessionManager) {}

    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
        MessageOption& option) override
    {
        if (data.ReadInterfaceToken() != GetDescriptor()) {
            WLOGFE("InterfaceToken check failed");
            return ERR_TRANSACTION_FAILED;
        }
        auto msgId = static_cast<SessionManagerServiceRecoverMessage>(code);
        switch (msgId) {
            case SessionManagerServiceRecoverMessage::TRANS_ID_ON_SESSION_MANAGER_SERVICE_RECOVER: {
                auto sessionManagerService = data.ReadRemoteObject();
                // Even if sessionManagerService is null, the recovery process is still required.
                OnSessionManagerServiceRecover(sessionManagerService);
                break;
            }
            case SessionManagerServiceRecoverMessage::TRANS_ID_ON_WMS_CONNECTION_CHANGED: {
                int32_t userId = INVALID_USER_ID;
                int32_t screenId = DEFAULT_SCREEN_ID;
                bool isConnected = false;
                if (!data.ReadInt32(userId) || !data.ReadInt32(screenId) || !data.ReadBool(isConnected)) {
                    TLOGE(WmsLogTag::WMS_MULTI_USER, "Read data failed!");
                    return ERR_TRANSACTION_FAILED;
                }
                if (isConnected) {
                    // Even if data.ReadRemoteObject() is null, the WMS connection still needs to be notified.
                    OnWMSConnectionChanged(userId, screenId, isConnected, data.ReadRemoteObject());
                } else {
                    OnWMSConnectionChanged(userId, screenId, isConnected, nullptr);
                }
                break;
            }
            default:
                WLOGFW("unknown transaction code %{public}d", code);
                return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
        return ERR_NONE;
    }

    void OnSessionManagerServiceRecover(const sptr<IRemoteObject>& sessionManagerService) override
    {
        if (sessionManager_ == nullptr) {
            TLOGE(WmsLogTag::WMS_RECOVER, "sessionManager_ is null");
            return;
        }
        sessionManager_->RemoveSSMDeathRecipient();
        sessionManager_->ClearSessionManagerProxy();

        auto sms = iface_cast<ISessionManagerService>(sessionManagerService);
        sessionManager_->RecoverSessionManagerService(sms);
    }

    void OnWMSConnectionChanged(
        int32_t userId, int32_t screenId, bool isConnected, const sptr<IRemoteObject>& sessionManagerService) override
    {
        if (sessionManager_ == nullptr) {
            TLOGE(WmsLogTag::WMS_RECOVER, "failed to get sessionManager_");
            return;
        }
        auto sms = iface_cast<ISessionManagerService>(sessionManagerService);
        sessionManager_->OnWMSConnectionChanged(userId, screenId, isConnected, sms);
    }

private:
    sptr<SessionManager> sessionManager_ = nullptr;
};

SessionManager::~SessionManager()
{
    sptr<IRemoteObject> remoteObject = nullptr;
    if (mockSessionManagerServiceProxy_) {
        remoteObject = mockSessionManagerServiceProxy_->AsObject();
    }
    if (remoteObject) {
        remoteObject->RemoveDeathRecipient(foundationDeath_);
    }
    TLOGI(WmsLogTag::WMS_SCB, "destroyed userId: %{public}d", userId_);
}

SessionManager::SessionManager(const int32_t userId) : userId_(userId) {}

SessionManager& SessionManager::GetInstance()
{
    static sptr<SessionManager> instance = new SessionManager();
    return *instance;
}

sptr<SessionManager> SessionManager::GetInstance(const int32_t userId)
{
    sptr<SessionManager> instance = nullptr;
    if (userId <= INVALID_USER_ID) {
        instance = &SessionManager::GetInstance();
        return instance;
    }
    // multi-instance mode
    std::lock_guard<std::mutex> lock(sessionManagerMapMutex_);
    auto iter = sessionManagerMap_.find(userId);
    if (iter != sessionManagerMap_.end()) {
        return iter->second;
    }
    TLOGI(WmsLogTag::WMS_MULTI_USER, "create new instance userId: %{public}d", userId);
    instance = new SessionManager(userId);
    sessionManagerMap_.insert({ userId, instance });
    return instance;
}

void SessionManager::OnWMSConnectionChangedCallback(
    int32_t userId, int32_t screenId, bool isConnected, bool isCallbackRegistered)
{
    if (isCallbackRegistered) {
        TLOGI(WmsLogTag::WMS_MULTI_USER,
            "WMS connection changed with userId=%{public}d, screenId=%{public}d, isConnected=%{public}d", userId,
            screenId, isConnected);
        wmsConnectionChangedFunc_(userId, screenId, isConnected);
    } else {
        TLOGD(WmsLogTag::WMS_MULTI_USER, "WMS CallbackFunc is null.");
    }
}

void SessionManager::OnWMSConnectionChanged(
    int32_t userId, int32_t screenId, bool isConnected, const sptr<ISessionManagerService>& sessionManagerService)
{
    bool isCallbackRegistered = false;
    int32_t lastUserId = INVALID_USER_ID;
    int32_t lastScreenId = DEFAULT_SCREEN_ID;
    {
        // The mutex ensures the timing of the following variable states in multiple threads
        std::lock_guard<std::mutex> lock(wmsConnectionMutex_);
        lastUserId = currentWMSUserId_;
        lastScreenId = currentScreenId_;
        isCallbackRegistered = wmsConnectionChangedFunc_ != nullptr;
        if (isConnected) {
            currentWMSUserId_ = userId;
            currentScreenId_ = screenId;
        }
        // isWMSConnected_ only represents the wms connection status of the active user
        if (currentWMSUserId_ == userId) {
            isWMSConnected_ = isConnected;
        }
    }
    TLOGD(WmsLogTag::WMS_MULTI_USER,
          "curUserId=%{public}d, oldUserId=%{public}d, screenId=%{public}d, isConnected=%{public}d",
          userId, lastUserId, screenId, isConnected);
    if (isConnected && lastUserId > INVALID_USER_ID && lastUserId != userId) {
        // Notify the user that the old wms has been disconnected.
        OnWMSConnectionChangedCallback(lastUserId, lastScreenId, false, isCallbackRegistered);
        OnUserSwitch(sessionManagerService);
    }
    // Notify the user that the current wms connection has changed.
    OnWMSConnectionChangedCallback(userId, screenId, isConnected, isCallbackRegistered);
}

void SessionManager::ClearSessionManagerProxy()
{
    TLOGI(WmsLogTag::WMS_SCB, "begin clear proxy");
    {
        std::lock_guard<std::mutex> lock(sessionManagerServiceMutex_);
        if (sessionManagerServiceProxy_ != nullptr) {
            int refCount = sessionManagerServiceProxy_->GetSptrRefCount();
            TLOGI(WmsLogTag::WMS_SCB, "ref count: %{public}d", refCount);
            sessionManagerServiceProxy_ = nullptr;
        }
    }
    std::lock_guard<std::mutex> lock(sceneSessionManagerMutex_);
    sceneSessionManagerProxy_ = nullptr;
}

__attribute__((no_sanitize("cfi"))) sptr<ISceneSessionManager> SessionManager::GetSceneSessionManagerProxy()
{
    InitSessionManagerServiceProxy();
    InitSceneSessionManagerProxy();
    return sceneSessionManagerProxy_;
}

void SessionManager::InitSessionManagerServiceProxy()
{
    {
        std::lock_guard<std::mutex> lock(sessionManagerServiceMutex_);
        if (sessionManagerServiceProxy_) {
            return;
        }
    }
    if (InitMockSMSProxy() != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_SCB, "init failed");
        return;
    }

    RegisterSMSRecoverListener();

    auto proxy = SessionManagerLite::GetInstance(userId_)->GetSessionManagerServiceProxy();
    std::lock_guard<std::mutex> lock(sessionManagerServiceMutex_);
    sessionManagerServiceProxy_ = proxy;
    if (!sessionManagerServiceProxy_) {
        TLOGE(WmsLogTag::WMS_SCB, "failed to get sms proxy");
    }
}

WMError SessionManager::InitMockSMSProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        TLOGE(WmsLogTag::WMS_SCB, "get mgr failed");
        return WMError::WM_ERROR_NULLPTR;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
    if (!remoteObject) {
        TLOGE(WmsLogTag::WMS_SCB, "Remote object is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    {
        std::lock_guard<std::mutex> lock(mockSessionManagerServiceMutex_);
        mockSessionManagerServiceProxy_ = iface_cast<IMockSessionManagerInterface>(remoteObject);
        if (!mockSessionManagerServiceProxy_) {
            TLOGE(WmsLogTag::WMS_SCB, "get mock proxy failed");
            return WMError::WM_ERROR_NULLPTR;
        }
    }

    if (GetUserIdByUid(getuid()) != SYSTEM_USERID) {
        return WMError::WM_OK;
    }
    {
        std::lock_guard<std::mutex> lock(foundationListenerRegisterdMutex_);
        if (isFoundationListenerRegistered_) {
            return WMError::WM_OK;
        }
    }
    if (!foundationDeath_) {
        foundationDeath_ = sptr<FoundationDeathRecipient>::MakeSptr(userId_);
    }
    if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(foundationDeath_)) {
        TLOGE(WmsLogTag::WMS_SCB, "Failed to add death recipient");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    std::lock_guard<std::mutex> lock(foundationListenerRegisterdMutex_);
    isFoundationListenerRegistered_ = true;
    return WMError::WM_OK;
}

__attribute__((no_sanitize("cfi"))) void SessionManager::InitSceneSessionManagerProxy()
{
    TLOGI(WmsLogTag::WMS_SCB, "init ssm proxy");
    {
        std::lock_guard<std::mutex> lock(sceneSessionManagerMutex_);
        if (sceneSessionManagerProxy_) {
            return;
        }
    }
    sptr<IRemoteObject> remoteObject = nullptr;
    {
        std::lock_guard<std::mutex> lock(sessionManagerServiceMutex_);
        if (sessionManagerServiceProxy_ == nullptr) {
            TLOGE(WmsLogTag::WMS_SCB, "get sms proxy failed");
            return;
        }
        remoteObject = sessionManagerServiceProxy_->GetSceneSessionManager();
        if (remoteObject == nullptr) {
            TLOGE(WmsLogTag::WMS_SCB, "get sms proxy failed");
            return;
        }
    }
    sceneSessionManagerDeath_ = sptr<SSMDeathRecipient>::MakeSptr(userId_);
    {
        std::lock_guard<std::mutex> lock(sceneSessionManagerMutex_);
        sceneSessionManagerProxy_ = iface_cast<ISceneSessionManager>(remoteObject);
        if (sceneSessionManagerProxy_ == nullptr) {
            TLOGE(WmsLogTag::WMS_SCB, "Get scene session manager proxy failed");
            return;
        }
    }
    if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(sceneSessionManagerDeath_)) {
        TLOGE(WmsLogTag::WMS_SCB, "failed to add death recipient");
        return;
    }
}

void SessionManager::RegisterSMSRecoverListener()
{
    {
        std::lock_guard<std::mutex> lock(recoverListenerMutex_);
        if (isRecoverListenerRegistered_) {
            TLOGI(WmsLogTag::WMS_RECOVER, "listener has been registered");
            return;
        }
    }
    {
        std::lock_guard<std::mutex> lock(mockSessionManagerServiceMutex_);
        if (mockSessionManagerServiceProxy_ == nullptr) {
            TLOGE(WmsLogTag::WMS_SCB, "Get mock sms proxy failed");
            return;
        }
        smsRecoverListener_ = sptr<SessionManagerServiceRecoverListener>::MakeSptr(this);
        TLOGD(WmsLogTag::WMS_RECOVER, "Register recover listener, userId_: %{public}d", userId_);
        mockSessionManagerServiceProxy_->RegisterSMSRecoverListener(smsRecoverListener_, userId_);
    }
    {
        std::lock_guard<std::mutex> lock(recoverListenerMutex_);
        isRecoverListenerRegistered_ = true;
    }
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    IPCSkeleton::SetCallingIdentity(identity);
}

void SessionManager::UnregisterSMSRecoverListener()
{
    TLOGI(WmsLogTag::WMS_RECOVER, "begin unregister listener");
    {
        std::lock_guard<std::mutex> lock(recoverListenerMutex_);
        isRecoverListenerRegistered_ = false;
    }
    TLOGD(WmsLogTag::WMS_RECOVER, "UnRegister recover listener, userId_: %{public}d", userId_);
    mockSessionManagerServiceProxy_->UnregisterSMSRecoverListener(userId_);
}

void SessionManager::RegisterWindowManagerRecoverCallbackFunc(const WindowManagerRecoverCallbackFunc& callbackFunc)
{
    std::lock_guard<std::mutex> lock(recoverCallbackMutex_);
    windowManagerRecoverFunc_ = callbackFunc;
}

void SessionManager::RecoverSessionManagerService(const sptr<ISessionManagerService>& sessionManagerService)
{
    TLOGI(WmsLogTag::WMS_RECOVER, "recover sms proxy");
    {
        std::lock_guard<std::mutex> lock(sessionManagerServiceMutex_);
        sessionManagerServiceProxy_ = sessionManagerService;
    }

    {
        std::lock_guard<std::mutex> lock(recoverCallbackMutex_);
        if (windowManagerRecoverFunc_) {
            TLOGD(WmsLogTag::WMS_RECOVER, "begin recover");
            windowManagerRecoverFunc_();
        }
    }
}

void SessionManager::OnUserSwitch(const sptr<ISessionManagerService>& sessionManagerService)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "begin user switch");
    RemoveSSMDeathRecipient();
    ClearSessionManagerProxy();
    {
        std::lock_guard<std::mutex> lock(sessionManagerServiceMutex_);
        sessionManagerServiceProxy_ = sessionManagerService;
    }
        InitSceneSessionManagerProxy();
    {
        std::lock_guard<std::mutex> lock(sceneSessionManagerMutex_);
        if (sceneSessionManagerProxy_ == nullptr) {
            TLOGE(WmsLogTag::WMS_MULTI_USER, "init ssm proxy failed");
            return;
        }
    }
    if (userSwitchCallbackFunc_) {
        TLOGD(WmsLogTag::WMS_MULTI_USER, "run user switch callback");
        userSwitchCallbackFunc_();
    }
}

void SessionManager::RemoveSSMDeathRecipient()
{
    TLOGI(WmsLogTag::WMS_SCB, "begin remove ssm death recipient");
    sptr<IRemoteObject> remoteObject = nullptr;
    {
        std::lock_guard<std::mutex> lock(sceneSessionManagerMutex_);
        if (sceneSessionManagerProxy_) {
            remoteObject = sceneSessionManagerProxy_->AsObject();
        }
    }
    if (remoteObject) {
        remoteObject->RemoveDeathRecipient(sceneSessionManagerDeath_);
    }
}

WMError SessionManager::RegisterWMSConnectionChangedListener(const WMSConnectionChangedCallbackFunc& callbackFunc)
{
    TLOGD(WmsLogTag::WMS_MULTI_USER, "register wms connection changed listener");
    auto ret = WMError::WM_OK;
    bool isWMSAlreadyConnected = false;
    int32_t userId = INVALID_USER_ID;
    int32_t screenId = DEFAULT_SCREEN_ID;

    if (callbackFunc == nullptr) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "callbackFunc is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    {
        // The mutex ensures the timing of the following variable states in multiple threads
        std::lock_guard<std::mutex> lock(wmsConnectionMutex_);
        wmsConnectionChangedFunc_ = callbackFunc;
        isWMSAlreadyConnected = isWMSConnected_ && (currentWMSUserId_ > INVALID_USER_ID);
        userId = currentWMSUserId_;
        screenId = currentScreenId_;
    }
    if (isWMSAlreadyConnected) {
        TLOGI(WmsLogTag::WMS_MULTI_USER, "WMS already connected, notify immediately");
        OnWMSConnectionChangedCallback(userId, screenId, true, true);
    }

    ret = InitMockSMSProxy();
    if (ret != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "Init mock session manager service failed");
        return ret;
    }
    RegisterSMSRecoverListener();
    return WMError::WM_OK;
}

WMError SessionManager::UnregisterWMSConnectionChangedListener()
{
    {
        std::lock_guard<std::mutex> lock(wmsConnectionMutex_);
        wmsConnectionChangedFunc_ = nullptr;
    }
    UnregisterSMSRecoverListener();
    return WMError::WM_OK;
}

void SessionManager::RegisterUserSwitchListener(const UserSwitchCallbackFunc& callbackFunc)
{
    TLOGD(WmsLogTag::WMS_MULTI_USER, "enter");
    userSwitchCallbackFunc_ = callbackFunc;
}

void SessionManager::OnFoundationDied()
{
    TLOGI(WmsLogTag::WMS_SCB, "begin clear");
    {
        std::lock_guard<std::mutex> lock(foundationListenerRegisterdMutex_);
        isFoundationListenerRegistered_ = false;
    }
    {
        std::lock_guard<std::mutex> lock(wmsConnectionMutex_);
        isWMSConnected_ = false;
    }
    {
        std::lock_guard<std::mutex> lock(recoverListenerMutex_);
        isRecoverListenerRegistered_ = false;
    }
    {
        std::lock_guard<std::mutex> lock(sessionManagerServiceMutex_);
        sessionManagerServiceProxy_ = nullptr;
    }
    {
        std::lock_guard<std::mutex> lock(sceneSessionManagerMutex_);
        sceneSessionManagerProxy_ = nullptr;
    }
    std::lock_guard<std::mutex> lock(mockSessionManagerServiceMutex_);
    mockSessionManagerServiceProxy_ = nullptr;
}

FoundationDeathRecipient::FoundationDeathRecipient(int32_t userId) : userId_(userId) {}

void FoundationDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    TLOGI(WmsLogTag::WMS_RECOVER, "mock sms proxy died, userId_: %{public}d", userId_);
    SessionManager::GetInstance(userId_)->OnFoundationDied();
}

SSMDeathRecipient::SSMDeathRecipient(int32_t userId) : userId_(userId) {}

void SSMDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    TLOGI(WmsLogTag::WMS_RECOVER, "ssm proxy died, userId_: %{public}d", userId_);
    SessionManager::GetInstance(userId_)->RemoveSSMDeathRecipient();
    SessionManager::GetInstance(userId_)->ClearSessionManagerProxy();
}
} // namespace OHOS::Rosen
