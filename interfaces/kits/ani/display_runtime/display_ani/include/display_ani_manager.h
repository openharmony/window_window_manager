/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef DISPLAY_MANAGER_ANI_H
#define DISPLAY_MANAGER_ANI_H

#include "ani.h"
#include "display.h"
#include "display_ani_register_manager.h"

namespace OHOS {
namespace Rosen {

class DisplayManagerAni {
public:
    explicit DisplayManagerAni();
    static void setAniEnv(ani_env* env);

    static ani_int getFoldDisplayModeAni(ani_env* env, ani_object obj);
    static ani_boolean isFoldableAni(ani_env* env, ani_object obj);
    static ani_int getFoldStatus(ani_env* env, ani_object obj);
    static ani_object getCurrentFoldCreaseRegion(ani_env* env, ani_object obj);

    static ani_array_ref getAllDisplaysAni(ani_env* env);
    static ani_status getDisplayByIdSyncAni(ani_env* env, ani_object obj, ani_int displayId);
    static ani_status getDefaultDisplaySyncAni(ani_env* env, ani_object obj);

    static ani_object registerCallback(ani_env* env, ani_object obj,
        ani_string type, ani_ref callback, ani_long nativeObj);
    static ani_object unRegisterCallback(ani_env* env, ani_object obj,
        ani_string type, ani_ref callback, ani_long nativeObj);

private:
    ani_object onRegisterCallback(ani_env* env, ani_string type, ani_ref callback);
    ani_object onUnRegisterCallback(ani_env* env, ani_string type, ani_ref callback);
    std::mutex mtx_;
    std::unique_ptr<OHOS::Rosen::DisplayAniRegisterManager> registerManager_ = nullptr;
    static ani_env* env_;
};
}
}
#endif