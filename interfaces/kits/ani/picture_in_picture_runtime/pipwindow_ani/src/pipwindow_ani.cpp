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
#include "pipwindow_ani.h"
#include <hitrace_meter.h>
#include "ani.h"
#include "window_manager_hilog.h"
#include "picture_in_picture_manager.h"
#include "picture_in_picture_controller.h"

namespace OHOS {
namespace Rosen {

ani_status PiPWindowAni::Init(ani_env* env, ani_namespace nsp)
{
    TLOGI(WmsLogTag::WMS_PIP, "[ANI] Init");
    if (env == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] null env");
        return ANI_ERROR;
    }
    ani_function setObjFunc = nullptr;
    ani_status ret = env->Namespace_FindFunction(nsp, "setNativeObj", "J:V", &setObjFunc);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] find setNativeObj func fail %{public}u", ret);
        return ret;
    }
    std::unique_ptr<PiPWindowAni> aniPiPWindowAni = std::make_unique<PiPWindowAni>();
    ret = env->Function_Call_Void(setObjFunc, aniPiPWindowAni.get());
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] find setNativeObj func fail %{public}u", ret);
        return ret;
    }
    return ret;
}

sptr<PictureInPictureController> PiPWindowAni::GetPiPController()
{
    return pipControllerToken_;
}

void PiPWindowAni::SetPiPController(sptr<PictureInPictureController> pipControllerToken)
{
    pipControllerToken_ = pipControllerToken;
}

ani_boolean PiPWindowAni::IsPiPEnabledAni(ani_env* env)
{
    bool isPiPEnabled = true;
    TLOGI(WmsLogTag::WMS_PIP, "[ANI] isPiPEnabled = %{public}u", isPiPEnabled);
    return static_cast<ani_boolean>(isPiPEnabled);
}

void PiPWindowAni::StartPiPAni(ani_env* env, ani_object obj, ani_long nativeObj)
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
    if (pipControllerToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] pipController is nullptr");
        return;
    }
    pipControllerToken_->StartPictureInPicture(StartPipType::USER_START);
    TLOGI(WmsLogTag::WMS_PIP, "[ANI] startPiPAni end");
}

void PiPWindowAni::StopPiPAni(ani_env* env, ani_object obj, ani_long nativeObj)
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
    if (pipControllerToken_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] pipController is nullptr");
        return;
    }
    pipControllerToken_->StopPictureInPictureFromClient();
    TLOGI(WmsLogTag::WMS_PIP, "[ANI] stopPiPAni end");
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    using namespace OHOS::Rosen;
    ani_status ret;
    ani_env* env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] null env");
        return ANI_NOT_FOUND;
    }
    ani_namespace nsp;
    if ((ret = env->FindNamespace("L@ohos/PiPWindow/PiPWindow;", &nsp)) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/PiPWindow/PiPWindow/PiPControllerInternal;", &cls)) != ANI_OK) {
        TLOGD(WmsLogTag::WMS_PIP, "[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    std::array funcs = {
        ani_native_function {"isPiPEnabled", ":Z", reinterpret_cast<void*>(PiPWindowAni::IsPiPEnabledAni)},
    };

    if ((ret = env->Namespace_BindNativeFunctions(nsp, funcs.data(), funcs.size()))) {
        TLOGE(WmsLogTag::WMS_PIP, "taoye [ANI] bind namespace fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    std::array methods = {
        ani_native_function {"startPiPNative", "J:V", reinterpret_cast<void*>(PiPWindowAni::StartPiPAni)},
        ani_native_function {"stopPiPNative", "J:V", reinterpret_cast<void*>(PiPWindowAni::StopPiPAni)},
    };

    if ((ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size())) != ANI_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "[ANI] bind pip method fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    PiPWindowAni::Init(env, nsp);
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}
}  // namespace Rosen
}  // namespace OHOS
