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

#ifndef OHOS_ORSEN_WINDOW_SCENE_SCENE_SESSION_STAGE_H
#define OHOS_ORSEN_WINDOW_SCENE_SCENE_SESSION_STAGE_H

#include "session/scene/host/include/zidl/scene_session_interface.h"
#include "session_stage.h"
#include "utils/include/window_scene_common.h"
#include "zidl/scene_session_stage_stub.h"
#include "zidl/window_event_channel_interface.h"

namespace OHOS::Rosen {
class SceneSessionStage : public SceneSessionStageStub, public SessionStage {
public:
    explicit SceneSessionStage(const sptr<ISceneSession>& sceneSession);
    ~SceneSessionStage() = default;

    virtual WSError SetActive(bool active) override;
    virtual WSError UpdateSessionRect(const WSRect& rect, SessionSizeChangeReason reason) override;

    virtual WSError Connect() override;
    virtual WSError Foreground() override;
    virtual WSError Background() override;
    virtual WSError Disconnect() override;

    WSError StartScene(const SceneAbilityInfo& info, SessionOption sessionOption);
    WSError Minimize();
    WSError Close();
    WSError Recover();
    WSError Maximum();

private:
    sptr<ISceneSession> sceneSession_;
};
}
#endif // OHOS_ORSEN_WINDOW_SCENE_SCENE_SESSION_STAGE_H