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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H
#define OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H

#include "interfaces/include/ws_common.h"
#include "session/host/include/root_scene_session.h"
#include "wm_single_instance.h"

#include "session_manager_base.h"
#include "session_manager/include/zidl/scene_session_manager_stub.h"

namespace OHOS::Ace::NG {
class UIWindow;
}

namespace OHOS::AAFwk {
class SessionInfo;
}

namespace OHOS::Rosen {
class SceneSession;
using NotifyCreateSpecificSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
class SceneSessionManager : public SceneSessionManagerStub,
                            public SessionManagerBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(SceneSessionManager)
public:
    sptr<SceneSession> RequestSceneSession(const SessionInfo& sessionInfo);
    WSError RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession);

    sptr<RootSceneSession> GetRootSceneSession();
    sptr<SceneSession> GetSceneSession(uint64_t persistentId);
    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session);
    WSError DestroyAndDisconnectSpecificSession(const uint64_t& persistentId);
    void SetCreateSpecificSessionListener(const NotifyCreateSpecificSessionFunc& func);

protected:
    SceneSessionManager();
    virtual ~SceneSessionManager() = default;

private:
    void Init();
    sptr<AAFwk::SessionInfo> SetAbilitySessionInfo(const sptr<SceneSession>& scnSession);
    std::map<uint64_t, sptr<SceneSession>> abilitySceneMap_;
    sptr<RootSceneSession> rootSceneSession_;
    std::shared_ptr<Ace::NG::UIWindow> rootScene_;
    NotifyCreateSpecificSessionFunc createSpecificSessionFunc_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H
