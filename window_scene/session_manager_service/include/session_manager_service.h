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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_SERVICE_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_SERVICE_H

#include "session_manager_service_stub.h"

#include "wm_single_instance.h"

namespace OHOS::Rosen {
class SessionManagerService : public SessionManagerServiceStub {
WM_DECLARE_SINGLE_INSTANCE(SessionManagerService)
public:
    void Init();
    void NotifySceneBoardAvailable();
    sptr<IRemoteObject> GetSceneSessionManager() override;
    sptr<IRemoteObject> GetSceneSessionManagerLite() override;

private:
    std::recursive_mutex mutex_;
    sptr<IRemoteObject> sceneSessionManagerObj_;
    sptr<IRemoteObject> sceneSessionManagerLiteObj_;
};
} // namesapce OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_SERVICE_H
