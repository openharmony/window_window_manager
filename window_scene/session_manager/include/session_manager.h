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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_H

#include <iremote_stub.h>
#include <shared_mutex>

#include "mock_session_manager_service_interface.h"
#include "session_manager_service_interface.h"
#include "wm_single_instance.h"
#include "zidl/scene_session_manager_interface.h"

namespace OHOS::Rosen {
class SSMDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};
class FoundationDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class SessionManager {
    WM_DECLARE_SINGLE_INSTANCE_BASE(SessionManager);
    
public:
    using SessionRecoverCallbackFunc = std::function<void()>;
    using WindowManagerRecoverCallbackFunc = std::function<void()>;
    using WMSConnectionChangedCallbackFunc = std::function<void(int32_t, int32_t, bool)>;
    using UserSwitchCallbackFunc = std::function<void()>;
    void RegisterWindowManagerRecoverCallbackFunc(const WindowManagerRecoverCallbackFunc& callbackFunc);
    void RecoverSessionManagerService(const sptr<ISessionManagerService>& sessionManagerService);
    void OnWMSConnectionChanged(
        int32_t userId, int32_t screenId, bool isConnected, const sptr<ISessionManagerService>& sessionManagerService);
    void ClearSessionManagerProxy();
    void Clear();
    WMError RegisterWMSConnectionChangedListener(const WMSConnectionChangedCallbackFunc& callbackFunc);
    void RegisterUserSwitchListener(const UserSwitchCallbackFunc& callbackFunc);

    sptr<ISceneSessionManager> GetSceneSessionManagerProxy();
    void OnFoundationDied();

protected:
    SessionManager() = default;
    virtual ~SessionManager();

private:
    void InitSessionManagerServiceProxy();
    WMError InitMockSMSProxy();
    void InitSceneSessionManagerProxy();
    void OnWMSConnectionChangedCallback(int32_t userId, int32_t screenId, bool isConnected, bool isCallbackRegistered);
    void OnUserSwitch(const sptr<ISessionManagerService>& sessionManagerService);
    void RegisterSMSRecoverListener();
    UserSwitchCallbackFunc userSwitchCallbackFunc_ = nullptr;

    std::recursive_mutex mutex_;
    sptr<IMockSessionManagerInterface> mockSessionManagerServiceProxy_ = nullptr;
    sptr<ISessionManagerService> sessionManagerServiceProxy_ = nullptr;
    sptr<ISceneSessionManager> sceneSessionManagerProxy_ = nullptr;
    bool isRecoverListenerRegistered_ = false;
    sptr<IRemoteObject> smsRecoverListener_ = nullptr;
    sptr<SSMDeathRecipient> ssmDeath_ = nullptr;
    sptr<FoundationDeathRecipient> foundationDeath_ = nullptr;
    bool isFoundationListenerRegistered_ = false;
    // above guarded by mutex_

    std::recursive_mutex recoverMutex_;
    WindowManagerRecoverCallbackFunc windowManagerRecoverFunc_ = nullptr;
    // above guarded by recoverMutex_

    std::mutex wmsConnectionMutex_;
    bool isWMSConnected_ = false;
    int32_t currentWMSUserId_ = INVALID_USER_ID;
    int32_t currentScreenId_ = DEFAULT_SCREEN_ID;
    WMSConnectionChangedCallbackFunc wmsConnectionChangedFunc_ = nullptr;
    // above guarded by wmsConnectionMutex_, among OnWMSConnectionChanged for wms connection event, user switched,
    // register WMSConnectionChangedListener.
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_H
