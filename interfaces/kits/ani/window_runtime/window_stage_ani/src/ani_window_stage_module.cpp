/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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


#include "ani_window_stage.h"

#include <memory>

#include "ani.h"
#include "ani_window.h"
#include "ani_window_manager.h"
#include "ani_window_utils.h"
#include "window_manager_hilog.h"
#include "permission.h"
#include "window_scene.h"

using OHOS::Rosen::WindowScene;

static ani_ref WindowGetMainWindow(ani_env* env, ani_object obj, ani_long nativeObj)
{
    using namespace OHOS::Rosen;
    TLOGD(WmsLogTag::DEFAULT, "[ANI]");
    AniWindowStage* windowStage = reinterpret_cast<AniWindowStage*>(nativeObj);
    if (windowStage == nullptr || windowStage->GetWindowScene().lock() == nullptr) {
        TLOGD(WmsLogTag::DEFAULT, "[ANI] windowStage is nullptr");
        return AniWindowUtils::CreateAniUndefined(env);
    }
    return windowStage->GetMainWindow(env);
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    ani_status ret;
    ani_env* env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] null env");
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/window/window/WindowStageInternal;", &cls)) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] can't find class %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array methods = {
        ani_native_function {"loadContent", "JLstd/core/String;:I",
            reinterpret_cast<void *>(AniWindowStage::LoadContent)},
        ani_native_function {"loadContentSync",
            "JLstd/core/String;Larkui/stateManagement/storages/localStorage/LocalStorage;:V",
            reinterpret_cast<void *>(AniWindowStage::LoadContentWithStorage)},
        ani_native_function {"disableWindowDecorSync", nullptr,
            reinterpret_cast<void *>(AniWindowStage::DisableWindowDecor)},
        ani_native_function {"setShowOnLockScreenSync",
            nullptr, reinterpret_cast<void *>(AniWindowStage::SetShowOnLockScreen)},
        ani_native_function {"getMainWindowSync", "J:L@ohos/window/window/Window;",
            reinterpret_cast<void *>(WindowGetMainWindow)},
        ani_native_function {"onSync", nullptr,
            reinterpret_cast<void *>(AniWindowStage::RegisterWindowCallback)},
        ani_native_function {"offSync", nullptr,
            reinterpret_cast<void *>(AniWindowStage::UnregisterWindowCallback)},
    };
    if ((ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size())) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] bind fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;

    AniWindowManager::AniWindowManagerInit(env);
    OHOS::Rosen::ANI_Window_Constructor(vm, result);
    return ANI_OK;
}
}