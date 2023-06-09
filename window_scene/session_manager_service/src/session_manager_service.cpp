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

#include "session_manager_service.h"

#include <system_ability_definition.h>
#include "session_manager/include/scene_session_manager.h"
#include "screenlock_system_ability.h"
#include "window_manager_hilog.h"

#include "screen_session_manager.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionManagerService"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(SessionManagerService)

SessionManagerService::SessionManagerService()
{
}

int SessionManagerService::GetValueById(int id)
{
    return id + 1;
}

sptr<IRemoteObject> SessionManagerService::GetSceneSessionManager()
{
    WLOGFD("GetSceneSessionManager success");
    if (sceneSessionManagerObj_) {
        return sceneSessionManagerObj_;
    }
    sceneSessionManagerObj_ = SceneSessionManager::GetInstance().AsObject();
    return sceneSessionManagerObj_;
}

sptr<IRemoteObject> SessionManagerService::GetScreenLockManagerService()
{
    if (screenLockManager_) {
        return screenLockManager_;
    }
    screenLockManager_ = ScreenLock::ScreenLockSystemAbility::GetInstance()->AsObject();
    return screenLockManager_;
}

sptr<IRemoteObject> SessionManagerService::GetRemoteObject()
{
    sessionManagerServiceObj_ = this->AsObject();
    return sessionManagerServiceObj_;
}

sptr<IRemoteObject> SessionManagerService::GetScreenSessionManagerService()
{
    if (screenSessionManagerObj_) {
        return screenSessionManagerObj_;
    }
    screenSessionManagerObj_ = ScreenSessionManager::GetInstance().AsObject();
    return screenSessionManagerObj_;
}
} // namesapce OHOS::Rosen