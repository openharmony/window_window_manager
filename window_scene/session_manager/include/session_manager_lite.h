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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_LITE_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_LITE_H

#include <functional>
#include <shared_mutex>

#include "session_manager_service_interface.h"
#include "mock_session_manager_service_interface.h"
#include "zidl/scene_session_manager_lite_interface.h"
#include "zidl/screen_session_manager_lite_interface.h"
#include "wm_single_instance.h"
#include "wm_common.h"

namespace OHOS::Rosen {
class SSMDeathRecipientLite : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class FoundationDeathRecipientLite : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class SessionManagerLite {
WM_DECLARE_SINGLE_INSTANCE_BASE(SessionManagerLite);
public:
    using UserSwitchCallbackFunc = std::function<void()>;
    using WMSConnectionChangedCallbackFunc = std::function<void(int32_t, int32_t, bool)>;
    void ClearSessionManagerProxy();
    void Clear();

    sptr<ISceneSessionManagerLite> GetSceneSessionManagerLiteProxy();
    sptr<IScreenSessionManagerLite> GetScreenSessionManagerLiteProxy();

    sptr<ISessionManagerService> GetSessionManagerServiceProxy();

    void SaveSessionListener(const sptr<ISessionListener>& listener);
    void DeleteSessionListener(const sptr<ISessionListener>& listener);
    void RecoverSessionManagerService(const sptr<ISessionManagerService>& sessionManagerService);
    WMError GetSessionVerificationInfo(int32_t persistentId, SessionVerificationInfo& verificationInfo);
    void RegisterUserSwitchListener(const UserSwitchCallbackFunc& callbackFunc);
    void OnWMSConnectionChanged(
        int32_t userId, int32_t screenId, bool isConnected, const sptr<ISessionManagerService>& sessionManagerService);
    void OnFoundationDied();

    WMError RegisterWMSConnectionChangedListener(const WMSConnectionChangedCallbackFunc& callbackFunc);

protected:
    SessionManagerLite() = default;
    virtual ~SessionManagerLite();

private:
    void InitSessionManagerServiceProxy();
    void InitSceneSessionManagerLiteProxy();
    void InitScreenSessionManagerLiteProxy();
    void OnUserSwitch(const sptr<ISessionManagerService> &sessionManagerService);
    void DeleteAllSessionListeners();
    void ReregisterSessionListener() const;
    void RegisterSMSRecoverListener();
    void OnWMSConnectionChangedCallback(int32_t userId, int32_t screenId, bool isConnected, bool isCallbackRegistered);
    WMError InitMockSMSProxy();
    UserSwitchCallbackFunc userSwitchCallbackFunc_ = nullptr;

    std::recursive_mutex mutex_;
    sptr<IMockSessionManagerInterface> mockSessionManagerServiceProxy_ = nullptr;
    sptr<ISessionManagerService> sessionManagerServiceProxy_ = nullptr;
    sptr<ISceneSessionManagerLite> sceneSessionManagerLiteProxy_ = nullptr;
    sptr<IScreenSessionManagerLite> screenSessionManagerLiteProxy_ = nullptr;
    sptr<SSMDeathRecipientLite> ssmDeath_ = nullptr;
    sptr<IRemoteObject> smsRecoverListener_ = nullptr;
    sptr<FoundationDeathRecipientLite> foundationDeath_ = nullptr;
    bool recoverListenerRegistered_ = false;
    bool destroyed_ = false;
    bool isFoundationListenerRegistered_ = false;
    // above guarded by mutex_

    std::recursive_mutex listenerLock_;
    std::vector<sptr<ISessionListener>> sessionListeners_;
    // above guarded by listenerLock_

    std::mutex wmsConnectionMutex_;
    int32_t currentWMSUserId_ = INVALID_USER_ID;
    int32_t currentScreenId_ = DEFAULT_SCREEN_ID;
    bool isWMSConnected_ = false;
    WMSConnectionChangedCallbackFunc wmsConnectionChangedFunc_ = nullptr;
    // above guarded by wmsConnectionMutex_, among OnWMSConnectionChanged for wms connection event, user switched,
    // register WMSConnectionChangedListener.
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_LITE_H
