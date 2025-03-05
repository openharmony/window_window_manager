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

namespace OHOS {
namespace Rosen {
 
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsDisplayManager"};
}

// construct, set registerManager.
DisplayAni::DisplayAni(const std::shared_ptr<OHOS::Rosen::Display>& display)
    : registerManager_(std::make_unique<DisplayAniRegisterManager>())
{
}

ani_object DisplayAni::getCutoutInfo(ani_env* env, ani_object obj)
{
    auto display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplay();
    sptr<CutoutInfo> cutoutInfo = display->GetCutoutInfo();
    ani_object objRet = nullptr;
    ani_class cls;
    if (ANI_OK != env->FindClass("L@ohos/display/display/CutoutInfoImpl", &cls)) {
        WLOGFE("[ANI] null class CutoutInfoImpl");
    }
    if (ANI_OK != DisplayAniUtils::NewAniObjectNoParams(env, cls, &objRet)) {
        WLOGFE("[ANI] create object fail");
    }
    ani_array_ref arrayRef = DisplayAniUtils::convertRects(cutoutInfo->GetBoundingRects(), env);
    env->Object_SetFieldByName_Ref(objRet, "boundingRects", arrayRef);
    ani_object waterfallObj = nullptr;

    if (ANI_OK != env->FindClass("L@ohos/display/display/WaterfallDisplayAreaRectsImpl", &cls)) {
        WLOGFE("[ANI] null class WaterfallDisplayAreaRectsImpl");
    }
    if (ANI_OK != DisplayAniUtils::NewAniObjectNoParams(env, cls, &waterfallObj)) {
        WLOGFE("[ANI] create object fail");
    }
    auto WaterfallDisplayAreaRects = cutoutInfo->GetWaterfallDisplayAreaRects();
    DMRect left = WaterfallDisplayAreaRects.left;
    DMRect top = WaterfallDisplayAreaRects.top;
    DMRect right = WaterfallDisplayAreaRects.right;
    DMRect bottom = WaterfallDisplayAreaRects.bottom;
    env->Object_SetFieldByName_Ref(waterfallObj, "left", DisplayAniUtils::convertRect(left, env));
    env->Object_SetFieldByName_Ref(waterfallObj, "top", DisplayAniUtils::convertRect(top, env));
    env->Object_SetFieldByName_Ref(waterfallObj, "right", DisplayAniUtils::convertRect(right, env));
    env->Object_SetFieldByName_Ref(waterfallObj, "bottom", DisplayAniUtils::convertRect(bottom, env));
    env->Object_SetFieldByName_Ref(objRet, "waterfallDisplayAreaRects", waterfallObj);
    return objRet;
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, OHOS::Rosen::HILOG_DOMAIN_WINDOW, "AniDisplay"};
    ani_status ret;
    ani_env *env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        WLOGFE("[ANI] null env");
        return ANI_NOT_FOUND;
    }
    DisplayManagerAni::setAniEnv(env);
    ani_namespace nsp;
    if ((ret = env->FindNamespace("L@ohos/display/display;", &nsp)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array funcs = {
        ani_native_function {"isFoldable", ":Z", reinterpret_cast<void *>(DisplayManagerAni::isFoldableAni)},
        ani_native_function {"getFoldDisplayMode", ":I",
            reinterpret_cast<void *>(DisplayManagerAni::getFoldDisplayModeAni)},
        ani_native_function {"getFoldStatus", ":I", reinterpret_cast<void *>(DisplayManagerAni::getFoldStatus)},
        ani_native_function {"getCurrentFoldCreaseRegion", ":L@ohos/display/display/FoldCreaseRegion;",
            reinterpret_cast<void *>(DisplayManagerAni::getCurrentFoldCreaseRegion)},
        ani_native_function {"getDisplayByIdSyncNative", "Lstd/core/Object;I:I",
            reinterpret_cast<void *>(DisplayManagerAni::getDisplayByIdSyncAni)},
        ani_native_function {"getDefaultDisplaySyncNative", "Lstd/core/Object;:I",
            reinterpret_cast<void *>(DisplayManagerAni::getDefaultDisplaySyncAni)},
        ani_native_function {"getAllDisplays", ":Lescompat/Array;",
            reinterpret_cast<void *>(DisplayManagerAni::getAllDisplaysAni)},
        ani_native_function {"syncOn", "Lstd/core/String;Lstd/core/Object;J:V",
            reinterpret_cast<void *>(DisplayManagerAni::registerCallback)},
        ani_native_function {"syncOff", "Lstd/core/String;Lstd/core/Object;J:V",
            reinterpret_cast<void *>(DisplayManagerAni::unRegisterCallback)}
    };
    WLOGFE("[ANI] func len %{public}u", funcs.size());
    if ((ret = env->Namespace_BindNativeFunctions(nsp, funcs.data(), funcs.size()))) {
        WLOGFE("[ANI] bind namespace fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    ani_class displayCls = nullptr;
    if ((ret = env->FindClass("L@ohos/display/display/DisplayImpl;", &displayCls)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array methods = {
        ani_native_function {"getCutoutInfoInternal", ":L@ohos/display/display/CutoutInfo;",
            reinterpret_cast<void *>(DisplayAni::getCutoutInfo)},
    };
    if ((ret = env->Class_BindNativeMethods(displayCls, methods.data(), methods.size())) != ANI_OK) {
        WLOGFE("[ANI] bind fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}
 
}  // namespace Rosen
}  // namespace OHOS
