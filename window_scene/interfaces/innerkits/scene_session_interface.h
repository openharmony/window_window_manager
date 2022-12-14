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

#ifndef OHOS_SCENE_SESSION_INTERFACE_H
#define OHOS_SCENE_SESSION_INTERFACE_H

#include "zidl/session_interface.h"
#include "scene_session_stage_interface.h"
#include "window_scene_common.h"
#include "window_event_channel_interface.h"

namespace OHOS::Rosen {
class ISceneSession : public ISession {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ISceneSession");

    enum class SceneSessionMessage : uint32_t {
        TRANS_ID_CONNECT,
        TRANS_ID_START_SCENE,
        TRANS_ID_MINIMIZE,
        TRANS_ID_CLOSE,
        TRANS_ID_RECOVER,
        TRANS_ID_MAXIMUM,
    };

    virtual WSError Connect(const sptr<ISceneSessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel) = 0;
    virtual WSError Minimize() = 0;
    virtual WSError Close() = 0;
    virtual WSError Recover() = 0;
    virtual WSError Maximum() = 0;
    virtual WSError StartScene(const SceneAbilityInfo& info, SessionOption sessionOption) = 0;
};
}
#endif // OHOS_SCENE_SESSION_INTERFACE_H