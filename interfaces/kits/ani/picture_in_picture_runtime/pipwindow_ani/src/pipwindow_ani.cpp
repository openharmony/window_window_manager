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
#include <hitrace_meter.h>

#include "ani.h"
#include "pipwindow_ani.h"
#include "pipwindow_ani_manager.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"
#include "dm_common.h"
#include "refbase.h"
#include "picture_in_picture_manager.h"

namespace OHOS {
namespace Rosen {

// construct, set registerManager.
PiPWindowAni::PiPWindowAni(const sptr<PictureInPictureController>& pipController)
{
}

void PiPWindowAni::startPiPAni(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::WMS_PIP, "[ANI] startPiPAni start");
    PiPWindowAni* aniPiPWindow = reinterpret_cast<PiPWindowAni*>(nativeObj);
    if (aniPiPWindow != nullptr) {
        aniPiPWindow->OnstartPiPAni(env);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] aniPiPWindow is nullptr");
    }
}

void PiPWindowAni::OnstartPiPAni(ani_env* env)
{
    auto pipController = GetPiPController();
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] pipController is nullptr");
        return;
    }
    pipController->StartPictureInPicture(StartPipType::USER_START);
    TLOGI(WmsLogTag::WMS_PIP, "[ANI] startPiPAni end");
}

void PiPWindowAni::stopPiPAni(ani_env* env, ani_object obj, ani_long nativeObj)
{
    TLOGI(WmsLogTag::WMS_PIP, "[ANI] stopPiPAni start");
    PiPWindowAni* aniPiPWindow = reinterpret_cast<PiPWindowAni*>(nativeObj);
    if (aniPiPWindow != nullptr) {
        aniPiPWindow->OnstopPiPAni(env);
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] aniPiPWindow is nullptr");
    }
}

void PiPWindowAni::OnstopPiPAni(ani_env* env)
{
    auto pipController = GetPiPController();
    if (pipController == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] pipController is nullptr");
        return;
    }
    pipController->StopPictureInPictureFromClient();
    TLOGI(WmsLogTag::WMS_PIP, "[ANI] stopPiPAni end");
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    ani_status ret;
    ani_env *env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] null env");
        return ANI_NOT_FOUND;
    }
    ani_namespace nsp;
    if ((ret = env->FindNamespace("L@ohos/PiPWindow/PiPWindow;", &nsp)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    PiPWindowManagerAni::initPiPWindowManagerAni(nsp, env);
    std::array funcs = {
        ani_native_function {"isPiPEnabled", ":Z", reinterpret_cast<void *>(PiPWindowManagerAni::isPiPEnabledAni)},
        ani_native_function {"startPiP", ":V", reinterpret_cast<void *>(PiPWindowAni::startPiPAni)},
        ani_native_function {"stopPiP", ":V", reinterpret_cast<void *>(PiPWindowAni::stopPiPAni)},
    };
    if ((ret = env->Namespace_BindNativeFunctions(nsp, funcs.data(), funcs.size()))) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] bind namespace fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/PiPWindow/PiPWindow/PiPControllerInternal;", &cls)) != ANI_OK) {
        TLOGD(WmsLogTag::WMS_PIP, "[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}

}  // namespace Rosen
}  // namespace OHOS
