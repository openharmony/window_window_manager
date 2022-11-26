/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_SCENE_SESSION_STAGE_H
#define OHOS_SCENE_SESSION_STAGE_H

#include "zidl/scene_session_stage_stub.h"
#include "session_stage.h"
#include "zidl/scene_session_interface.h"
#include "window_scene_common.h"

namespace OHOS::Rosen {
class SceneSessionStage : public SceneSessionStageStub, public SessionStage{
public:
    explicit SceneSessionStage() = default;
    ~SceneSessionStage() = default;

    virtual WSError SetActive(bool active) override;

    WSError RequestSceneSessionActivation(const AbilityInfo& info) override;
    WSError Minimize() override;
    WSError Close() override;
    WSError Recover() override;
    WSError Maximum() override;

private:
    sptr<ISceneSession> sceneSession_;
};
}
#endif // OHOS_SCENE_SESSION_STAGE_H