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
#include "session_manager_service_recover_interface.h"
#include "zidl/scene_session_manager_interface.h"
#include "wm_single_instance.h"
#include "common/include/task_scheduler.h"

namespace OHOS::Rosen {
class SSMDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class SessionManager : public IRemoteStub<ISessionManagerServiceRecoverListener> {
WM_DECLARE_SINGLE_INSTANCE_BASE(SessionManager);
public:
    using SessionRecoverCallbackFunc = std::function<void()>;
    using WindowManagerRecoverCallbackFunc = std::function<void()>;
    void ClearSessionManagerProxy();
    void Clear();
    void RecoverAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, sptr<ISession>& session,
        sptr<IRemoteObject> token = nullptr);
    WMError RecoverAndReconnectSceneSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        SystemSessionConfig& systemConfig, sptr<ISession>& session, sptr<WindowSessionProperty> property = nullptr,
        sptr<IRemoteObject> token = nullptr, int32_t pid = -1, int32_t uid = -1);
    void RegisterSessionRecoverCallbackFunc(int32_t persistentId, const SessionRecoverCallbackFunc& callbackFunc);
    void UnRegisterSessionRecoverCallbackFunc(int32_t persistentId);
    void RegisterWindowManagerRecoverCallbackFuc(const WindowManagerRecoverCallbackFunc& callbackFunc);
    void UnRegisterWindowManagerRecoverCallbackFuc();

    sptr<ISceneSessionManager> GetSceneSessionManagerProxy();

    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;
protected:
    SessionManager() = default;
    virtual ~SessionManager();

private:
    void InitSessionManagerServiceProxy();
    void InitSceneSessionManagerProxy();

    void OnSessionManagerServiceRecover(const sptr<IRemoteObject>& sessionManagerService) override;

    std::shared_ptr<TaskScheduler> taskScheduler_ = std::make_shared<TaskScheduler>("SessionManager");

    sptr<IMockSessionManagerInterface> mockSessionManagerServiceProxy_ = nullptr;
    sptr<ISessionManagerService> sessionManagerServiceProxy_ = nullptr;
    sptr<ISceneSessionManager> sceneSessionManagerProxy_ = nullptr;
    sptr<SSMDeathRecipient> ssmDeath_ = nullptr;
    std::map<int32_t, SessionRecoverCallbackFunc> sessionRecoverCallbackFuncMap_;
    WindowManagerRecoverCallbackFunc windowManagerRecoverFunc_ = nullptr;
    std::recursive_mutex mutex_;
    bool destroyed_ = false;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_H
