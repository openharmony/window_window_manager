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

#include "interfaces/include/ws_common.h"
#include "iremote_object.h"
#include "session/host/include/zidl/session_interface.h"
#include "session_manager_service/include/session_manager_service_interface.h"
#include "wm_single_instance.h"
#include "zidl/scene_session_manager_interface.h"

namespace OHOS::Rosen {
class AbilityConnection;
class SessionManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(SessionManager);
public:
    SessionManager();

    ~SessionManager();

    void Init();

    sptr<IRemoteObject> GetRemoteObject();

    void CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session);
    void DestroyAndDisconnectSpecificSession(const uint64_t& persistentId);

private:
    void ConnectToService();
    void InitSceneSessionManagerProxy();
    void CreateSessionManagerServiceProxy();
    void GetSceneSessionManagerProxy();

    sptr<AbilityConnection> abilityConnection_;
    sptr<IRemoteObject> remoteObject_ = nullptr;
    sptr<ISessionManagerService> sessionManagerServiceProxy_ = nullptr;
    sptr<ISceneSessionManager> sceneSessionManagerProxy_ = nullptr;
    std::recursive_mutex mutex_;
};

} // namespace OHOS::Rosen