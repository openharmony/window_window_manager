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

#include <mutex>
#include <vector>
#include <map>
#include <unistd.h>

#include <refbase.h>
#include <iremote_object.h>

#include "session/scene/host/include/root_scene_session.h"
#include "utils/include/ws_single_instance.h"
#include "utils/include/window_scene_common.h"

namespace OHOS::Ace::NG {
class UIWindow;
}

namespace OHOS::Rosen {
class SceneSession;

class SceneSessionManager {
WS_DECLARE_SINGLE_INSTANCE(SceneSessionManager)
public:
    sptr<RootSceneSession> GetRootSceneSession();
    sptr<SceneSession> RequestSceneSession(const SceneAbilityInfo& abilityInfo);
    WSError RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession);

private:
    uint32_t GenSessionId();

    std::recursive_mutex mutex_;
    std::vector<sptr<SceneSession>> sessions_;
    int pid_ = getpid();
    std::atomic<uint32_t> sessionId_ = INVALID_SESSION_ID;
    std::map<uint32_t, sptr<IRemoteObject>> abilitySceneMap_;
    sptr<RootSceneSession> rootSceneSession_;
    std::shared_ptr<Ace::NG::UIWindow> rootScene_;
};
}
#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H
