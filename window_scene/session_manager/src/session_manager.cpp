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
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "SessionManager" };
}

class SessionManagerServiceRecoverListener : public IRemoteStub<ISessionManagerServiceRecoverListener> {
public:
    explicit SessionManagerServiceRecoverListener() = default;

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
        SessionManager::GetInstance().Clear();
        SessionManager::GetInstance().ClearSessionManagerProxy();

        auto sms = iface_cast<ISessionManagerService>(sessionManagerService);
        SessionManager::GetInstance().RecoverSessionManagerService(sms);
    }

    void OnWMSConnectionChanged(
        int32_t userId, int32_t screenId, bool isConnected, const sptr<IRemoteObject>& sessionManagerService) override
    {
        auto sms = iface_cast<ISessionManagerService>(sessionManagerService);
        SessionManager::GetInstance().OnWMSConnectionChanged(userId, screenId, isConnected, sms);
    }
};

WM_IMPLEMENT_SINGLE_INSTANCE(SessionManager)

SessionManager::~SessionManager()
{
    WLOGFI("destroyed!");
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
        TLOGE(WmsLogTag::WMS_MULTI_USER, "WMS CallbackFunc is null.");
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
        isWMSConnected_ = isConnected;
        isCallbackRegistered = wmsConnectionChangedFunc_ != nullptr;
        if (isConnected) {
            currentWMSUserId_ = userId;
            currentScreenId_ = screenId;
        }
    }
    TLOGI(WmsLogTag::WMS_MULTI_USER,
        "curUserId=%{public}d, oldUserId=%{public}d, screenId=%{public}d, isConnected=%{public}d", userId, lastUserId,
        screenId, isConnected);
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
    WLOGFI("enter!");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (sessionManagerServiceProxy_ != nullptr) {
        int refCount = sessionManagerServiceProxy_->GetSptrRefCount();
        WLOGFI("sessionManagerServiceProxy GetSptrRefCount : %{public}d", refCount);
        sessionManagerServiceProxy_ = nullptr;
    }
    sceneSessionManagerProxy_ = nullptr;
}

__attribute__((no_sanitize("cfi"))) sptr<ISceneSessionManager> SessionManager::GetSceneSessionManagerProxy()
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
        WLOGFE("sessionManagerServiceProxy is nullptr");
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
        foundationDeath_ = sptr<FoundationDeathRecipient>::MakeSptr();
    }
    if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(foundationDeath_)) {
        WLOGFE("Failed to add death recipient");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    isFoundationListenerRegistered_ = true;
    return WMError::WM_OK;
}

__attribute__((no_sanitize("cfi"))) void SessionManager::InitSceneSessionManagerProxy()
{
    if (sceneSessionManagerProxy_) {
        return;
    }
    if (!sessionManagerServiceProxy_) {
        WLOGFE("sessionManagerServiceProxy is nullptr");
        return;
    }

    sptr<IRemoteObject> remoteObject = sessionManagerServiceProxy_->GetSceneSessionManager();
    if (!remoteObject) {
        WLOGFW("Get scene session manager proxy failed, null");
        return;
    }
    sceneSessionManagerProxy_ = iface_cast<ISceneSessionManager>(remoteObject);
    if (sceneSessionManagerProxy_) {
        ssmDeath_ = sptr<SSMDeathRecipient>::MakeSptr();
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(ssmDeath_)) {
            WLOGFE("Failed to add death recipient");
            return;
        }
    } else {
        WLOGFW("Get scene session manager proxy failed, iface_cast null");
    }
}

void SessionManager::RegisterSMSRecoverListener()
{
    if (!isRecoverListenerRegistered_) {
        if (!mockSessionManagerServiceProxy_) {
            TLOGE(WmsLogTag::WMS_RECOVER, "mockSessionManagerServiceProxy is null");
            return;
        }
        isRecoverListenerRegistered_ = true;
        TLOGI(WmsLogTag::WMS_RECOVER, "Register recover listener");
        smsRecoverListener_ = sptr<SessionManagerServiceRecoverListener>::MakeSptr();
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

void SessionManager::OnUserSwitch(const sptr<ISessionManagerService>& sessionManagerService)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "User switched");
    Clear();
    ClearSessionManagerProxy();
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        sessionManagerServiceProxy_ = sessionManagerService;
        InitSceneSessionManagerProxy();
        if (!sceneSessionManagerProxy_) {
            TLOGE(WmsLogTag::WMS_MULTI_USER, "sceneSessionManagerProxy is null");
            return;
        }
    }
    if (userSwitchCallbackFunc_) {
        TLOGI(WmsLogTag::WMS_MULTI_USER, "User switch callback");
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
    TLOGI(WmsLogTag::WMS_MULTI_USER, "in");
    if (callbackFunc == nullptr) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "callbackFunc is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    bool isWMSAlreadyConnected = false;
    int32_t userId = INVALID_USER_ID;
    int32_t screenId = DEFAULT_SCREEN_ID;
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
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        auto ret = InitMockSMSProxy();
        if (ret != WMError::WM_OK) {
            TLOGE(WmsLogTag::WMS_MULTI_USER, "Init mock session manager service failed");
            return ret;
        }
        RegisterSMSRecoverListener();
    }
    return WMError::WM_OK;
}

void SessionManager::RegisterUserSwitchListener(const UserSwitchCallbackFunc& callbackFunc)
{
    TLOGI(WmsLogTag::WMS_MULTI_USER, "enter");
    userSwitchCallbackFunc_ = callbackFunc;
}

void SessionManager::OnFoundationDied()
{
    TLOGI(WmsLogTag::WMS_RECOVER, "enter");
    {
        std::lock_guard<std::mutex> lock(wmsConnectionMutex_);
        isWMSConnected_ = false;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    isFoundationListenerRegistered_ = false;
    isRecoverListenerRegistered_ = false;
    mockSessionManagerServiceProxy_ = nullptr;
    sessionManagerServiceProxy_ = nullptr;
    sceneSessionManagerProxy_ = nullptr;
}

void FoundationDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    TLOGI(WmsLogTag::WMS_RECOVER, "Foundation died");
    SessionManager::GetInstance().OnFoundationDied();
}

void SSMDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    WLOGI("ssm OnRemoteDied");
    SessionManager::GetInstance().Clear();
    SessionManager::GetInstance().ClearSessionManagerProxy();
}
} // namespace OHOS::Rosen
