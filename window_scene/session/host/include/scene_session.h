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

#ifndef OHOS_SCENE_SESSION_H
#define OHOS_SCENE_SESSION_H

#include "zidl/scene_session_stub.h"
#include "foundation/window/window_manager/window_scene/session/host/include/session.h"
#include "zidl/scene_session_stage_interface.h"
#include "window_scene_common.h"

namespace OHOS::Rosen {
class SceneSession;
using NotifyStartSceneFunc = std::function<void(const SceneAbilityInfo& info, SessionOption sessionOption)>;
class SceneSession : public SceneSessionStub, public Session {
public:
    explicit SceneSession(const SceneAbilityInfo& info);
    ~SceneSession() = default;

    virtual WSError Connect(const sptr<ISceneSessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel) override;
    virtual WSError Foreground() override;
    virtual WSError Background() override;
    virtual WSError Disconnect() override;
    virtual WSError Minimize() override;
    virtual WSError Close() override;
    virtual WSError Recover() override;
    virtual WSError Maximum() override;
    virtual WSError StartScene(const SceneAbilityInfo& info, SessionOption sessionOption) override;

    virtual WSError SetActive(bool active) override;

    const SceneAbilityInfo& GetAbilityInfo() const;
    void RegisterStartSceneEventListener(const NotifyStartSceneFunc& func);
private:
    sptr<ISceneSessionStage> sceneSessionStage_;
    sptr<IWindowEventChannel> windowEventChannel_;
    SceneAbilityInfo abilityInfo_;
    std::string sessionType_ = "";
    bool isActive_ = false;
    NotifyStartSceneFunc startSceneFunc_;
};
}
#endif // OHOS_SCENE_SESSION_H