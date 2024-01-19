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

#include <shared_mutex>
#include <iremote_stub.h>

#include "session_manager_service_interface.h"
#include "mock_session_manager_service_interface.h"
#include "zidl/scene_session_manager_interface.h"
#include "wm_single_instance.h"

namespace OHOS::Rosen {
class SSMDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class SessionManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(SessionManager);
public:
    using SessionRecoverCallbackFunc = std::function<void()>;
    using WindowManagerRecoverCallbackFunc = std::function<void()>;
    using WMSConnectionChangedCallbackFunc = std::function<void(int32_t userId, int32_t screenId, bool isConnected)>;
    void RegisterWindowManagerRecoverCallbackFunc(const WindowManagerRecoverCallbackFunc& callbackFunc);
    void RecoverSessionManagerService(const sptr<ISessionManagerService>& sessionManagerService);
    void ClearSessionManagerProxy();
    void Clear();
    WMError RegisterWMSConnectionChangedListener(const WMSConnectionChangedCallbackFunc& callbackFunc);
    WMError UnregisterWMSConnectionChangedListener();

    sptr<ISceneSessionManager> GetSceneSessionManagerProxy();

protected:
    SessionManager() = default;
    virtual ~SessionManager();

private:
    void InitSessionManagerServiceProxy();
    void InitSceneSessionManagerProxy();
    void OnWMSConnectionChanged(int32_t userId, int32_t screenId, bool isConnected);
    sptr<IMockSessionManagerInterface> mockSessionManagerServiceProxy_ = nullptr;
    sptr<ISessionManagerService> sessionManagerServiceProxy_ = nullptr;
    sptr<ISceneSessionManager> sceneSessionManagerProxy_ = nullptr;
    sptr<IRemoteObject> smsRecoverListener_ = nullptr;
    sptr<SSMDeathRecipient> ssmDeath_ = nullptr;
    WMSConnectionChangedCallbackFunc wmsConnectionChangedFunc_ = nullptr;
    std::recursive_mutex mutex_;
    bool destroyed_ = false;
    int32_t currentUserId_ = 0;
    int32_t currentScreenId_ = 0;
    bool isWMSConnected_ = false;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_H
