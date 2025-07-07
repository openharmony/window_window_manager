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
#include "display_ani.h"
#include "display_info.h"
#include "display.h"
#include "singleton_container.h"
#include "display_manager.h"
#include "window_manager_hilog.h"
#include "dm_common.h"
#include "display_ani_utils.h"
#include "refbase.h"
#include "display_ani_manager.h"
#include "ani_err_utils.h"

namespace OHOS {
namespace Rosen {

// construct, set registerManager.
DisplayAni::DisplayAni(const std::shared_ptr<OHOS::Rosen::Display>& display)
{
}

void DisplayAni::getCutoutInfo(ani_env* env, ani_object obj, ani_object cutoutInfoObj)
{
    auto display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplay();
    TLOGI(WmsLogTag::DMS, "[ANI] getCutoutInfo begin");
    sptr<CutoutInfo> cutoutInfo = display->GetCutoutInfo();
    if (cutoutInfo == nullptr) {
        AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_SCREEN, "");
    }
    std::vector<DMRect> rects = cutoutInfo->GetBoundingRects();
    // bounding rects
    ani_ref boundingRects = nullptr;
    ani_status status = env->Object_GetFieldByName_Ref(cutoutInfoObj, "<property>boundingRects", &boundingRects);
    if (ANI_OK != status) {
        TLOGE(WmsLogTag::DMS, "[ANI] get field bounding rects fail, ani_status = %{public}d", status);
    }
    ani_double length;
    if (ANI_OK != env->Object_GetPropertyByName_Double(static_cast<ani_object>(boundingRects), "length", &length)) {
        TLOGE(WmsLogTag::DMS, "[ANI] get ani_array len fail");
    }
    for (int i = 0; i < std::min(int(length), static_cast<int>(rects.size())); i++) {
        ani_ref currentCutoutInfo;
        if (ANI_OK != env->Object_CallMethodByName_Ref(static_cast<ani_object>(boundingRects), "$_get",
            "I:Lstd/core/Object;", &currentCutoutInfo, (ani_int)i)) {
            TLOGE(WmsLogTag::DMS, "[ANI] get ani_array index %{public}u fail", (ani_int)i);
        }
        TLOGI(WmsLogTag::DMS, "current i: %{public}d", i);
        DisplayAniUtils::convertRect(rects[i], static_cast<ani_object>(currentCutoutInfo), env);
    }
    // waterfall area
    ani_ref waterfallObj = nullptr;
    env->Object_GetFieldByName_Ref(cutoutInfoObj, "<property>waterfallDisplayAreaRects", &waterfallObj);
    auto waterfallDisplayAreaRects = cutoutInfo->GetWaterfallDisplayAreaRects();
    DisplayAniUtils::convertWaterArea(waterfallDisplayAreaRects, static_cast<ani_object>(waterfallObj), env);
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    ani_status ret;
    ani_env *env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] null env");
        return ANI_NOT_FOUND;
    }
    ani_namespace nsp;
    if ((ret = env->FindNamespace("L@ohos/display/display;", &nsp)) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    DisplayManagerAni::initDisplayManagerAni(nsp, env);
    std::array funcs = {
        ani_native_function {"isFoldable", ":Z", reinterpret_cast<void *>(DisplayManagerAni::isFoldableAni)},
        ani_native_function {"getFoldDisplayModeNative", ":I",
            reinterpret_cast<void *>(DisplayManagerAni::getFoldDisplayModeAni)},
        ani_native_function {"getFoldStatusNative", ":I", reinterpret_cast<void *>(DisplayManagerAni::getFoldStatus)},
        ani_native_function {"getCurrentFoldCreaseRegionNative", "Lstd/core/Object;J:V",
            reinterpret_cast<void *>(DisplayManagerAni::getCurrentFoldCreaseRegion)},
        ani_native_function {"getDisplayByIdSyncNative", "Lstd/core/Object;D:V",
            reinterpret_cast<void *>(DisplayManagerAni::getDisplayByIdSyncAni)},
        ani_native_function {"getDefaultDisplaySyncNative", "Lstd/core/Object;:V",
            reinterpret_cast<void *>(DisplayManagerAni::getDefaultDisplaySyncAni)},
        ani_native_function {"getAllDisplaysSyncNative", "Lescompat/Array;:V",
            reinterpret_cast<void *>(DisplayManagerAni::getAllDisplaysAni)},
        ani_native_function {"syncOn", nullptr,
            reinterpret_cast<void *>(DisplayManagerAni::registerCallback)},
        ani_native_function {"syncOff", nullptr,
            reinterpret_cast<void *>(DisplayManagerAni::unRegisterCallback)}
    };
    if ((ret = env->Namespace_BindNativeFunctions(nsp, funcs.data(), funcs.size()))) {
        TLOGE(WmsLogTag::DMS, "[ANI] bind namespace fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    ani_class displayCls = nullptr;
    if ((ret = env->FindClass("L@ohos/display/display/DisplayImpl;", &displayCls)) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array methods = {
        ani_native_function {"getCutoutInfoInternal", "L@ohos/display/display/CutoutInfo;:V",
            reinterpret_cast<void *>(DisplayAni::getCutoutInfo)},
    };
    if ((ret = env->Class_BindNativeMethods(displayCls, methods.data(), methods.size())) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] bind fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}

}  // namespace Rosen
}  // namespace OHOS
