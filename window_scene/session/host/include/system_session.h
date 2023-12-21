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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SYSTEM_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_SYSTEM_SESSION_H

#include "session/host/include/scene_session.h"

namespace OHOS::Rosen {
class SystemSession : public SceneSession {
public:
    SystemSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback);
    ~SystemSession();

    WSError Show(sptr<WindowSessionProperty> property) override;
    WSError Hide() override;
    WSError Disconnect() override;
    WSError ProcessPointDownSession(int32_t posX, int32_t posY) override;

private:
    void UpdateCameraFloatWindowStatus(bool isShowing);
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SYSTEM_SESSION_H
