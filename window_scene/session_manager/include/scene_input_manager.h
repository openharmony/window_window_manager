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

#ifndef OHOS_SESSION_MANAGER_SCENE_INPUT_MANAGER_H
#define OHOS_SESSION_MANAGER_SCENE_INPUT_MANAGER_H

#include <string>
#include <vector>

#include <ui_content.h>
#include "input_manager.h"
#include "session/host/include/scene_session.h"
#include "wm_common.h"
#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {
class SceneSessionDirtyManager;

class SceneInputManager : public std::enable_shared_from_this<SceneInputManager> {
WM_DECLARE_SINGLE_INSTANCE_BASE(SceneInputManager)
public:
    void FlushDisplayInfoToMMI();
    void NotifyWindowInfoChange(const sptr<SceneSession>& scenenSession, const WindowUpdateType& type);
    void NotifyWindowInfoChangeFromSession(const sptr<SceneSession>& sceneSession);
    void NotifyMMIWindowPidChange(const sptr<SceneSession>& sceneSession, const bool startMoving);

protected:
    SceneInputManager() = default;
    virtual ~SceneInputManager() = default;

private:
    void Init();
    void FlushFullInfoToMMI(const std::vector<MMI::WindowInfo>& windowInfoList);
    void FlushChangeInfoToMMI(const std::map<uint64_t, std::vector<MMI::WindowInfo>>& screenId2Windows);
    void ConstructDisplayInfos(std::vector<MMI::DisplayInfo>& displayInfos);

    std::shared_ptr<SceneSessionDirtyManager> sceneSessionDirty_;
};
}//Rosen
}//OHOS
#endif //OHOS_SESSION_MANAGER_SCENE_INPUT_MANAGER_H