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
#include "display_manager_ani.h"



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
        return objRet;
    }
    if (ANI_OK != DisplayAniUtils::NewAniObjectNoParams(env, cls, &objRet)) {
        WLOGFE("[ANI] create object fail");
        return objRet;
    }
    ani_field boundingRectsFld;
    if (ANI_OK != env->Class_FindField(cls, "boundingRects", &boundingRectsFld)) {
        WLOGFE("[ANI] null field boundingRectsFld");
        return objRet;
    }
    // set boundingRects field
    ani_array_ref arrayRef = DisplayAniUtils::convertRects(cutoutInfo->GetBoundingRects(), env);
    env->Object_SetField_Ref(objRet, boundingRectsFld, arrayRef);
    
    ani_object waterfallObj = nullptr;
    ani_field waterfallFld;

    if (ANI_OK != env->Class_FindField(cls, "waterfallDisplayAreaRects", &waterfallFld)) {
        WLOGFE("[ANI] null field waterfallFld");
        return objRet;
    }
    if (ANI_OK != env->FindClass("L@ohos/display/display/WaterfallDisplayAreaRectsImpl", &cls)) {
        WLOGFE("[ANI] null class WaterfallDisplayAreaRectsImpl");
        return objRet;
    }
    if (ANI_OK != DisplayAniUtils::NewAniObjectNoParams(env, cls, &waterfallObj)) {
        WLOGFE("[ANI] create object fail");
        return objRet;
    }
    ani_class rectCls;
    ani_field leftFld;
    ani_field rightFld;
    ani_field topFld;
    ani_field bottomFld;
    if (ANI_OK != env->FindClass("L@ohos/display/display/RectImpl", &rectCls)) {
        WLOGFE("[ANI] find class fail");
        return objRet;
    }
    if (ANI_OK != env->Class_FindField(cls, "left", &leftFld)) {
        WLOGFE("[ANI] null field left");
        return objRet;
    }
    if (ANI_OK != env->Class_FindField(cls, "right", &rightFld)) {
        WLOGFE("[ANI] null field right");
        return objRet;
    }
    if (ANI_OK != env->Class_FindField(cls, "top", &topFld)) {
        WLOGFE("[ANI] null field top");
        return objRet;
    }
    if (ANI_OK != env->Class_FindField(cls, "bottom", &bottomFld)) {
        WLOGFE("[ANI] null field bottom");
        return objRet;
    }
 
    // set WaterfallDisplayAreaRects field
    auto WaterfallDisplayAreaRects = cutoutInfo->GetWaterfallDisplayAreaRects();
    DMRect left = WaterfallDisplayAreaRects.left;
    DMRect top = WaterfallDisplayAreaRects.top;
    DMRect right = WaterfallDisplayAreaRects.right;
    DMRect bottom = WaterfallDisplayAreaRects.bottom;
    env->Object_SetField_Ref(waterfallObj, leftFld, DisplayAniUtils::convertRect(left, env));
    env->Object_SetField_Ref(waterfallObj, topFld, DisplayAniUtils::convertRect(top, env));
    env->Object_SetField_Ref(waterfallObj, rightFld, DisplayAniUtils::convertRect(right, env));
    env->Object_SetField_Ref(waterfallObj, bottomFld, DisplayAniUtils::convertRect(bottom, env));
    env->Object_SetField_Ref(objRet, waterfallFld, waterfallObj);
    return objRet;
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, OHOS::Rosen::HILOG_DOMAIN_WINDOW, "JsDisplayManager"};
    ani_status ret;
    ani_env *env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        WLOGFE("[ANI] null env");
        return ANI_NOT_FOUND;
    }
    DisplayManagerAni::setAniEnv(env);
    // namespace函数绑定
    ani_namespace nsp;
    // 找namespace
    if ((ret = env->FindNamespace("L@ohos/display/display;", &nsp)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    // ts->c++的映射
    std::array funcs = {
        ani_native_function {"isFoldable", ":Z", reinterpret_cast<void *>(DisplayManagerAni::isFoldableAni)},
        ani_native_function {"getFoldDisplayModeAni", ":I", reinterpret_cast<void *>(DisplayManagerAni::getFoldDisplayModeAni)},
        ani_native_function {"getFoldStatus", ":I", reinterpret_cast<void *>(DisplayManagerAni::getFoldStatus)},

        ani_native_function {"getCurrentFoldCreaseRegion", ":L@ohos/display/display/FoldCreaseRegionImpl;", reinterpret_cast<void *>(DisplayManagerAni::getFoldStatus)},
        ani_native_function {"getDisplayByIdSync", "I:L@ohos/display/display/DisplayImpl;", reinterpret_cast<void *>(DisplayManagerAni::getFoldStatus)},
        ani_native_function {"getDefaultDisplaySync", ":L@ohos/display/display/DisplayImpl;", reinterpret_cast<void *>(DisplayManagerAni::getFoldStatus)},
        // 函数签名问题
        ani_native_function {"getAllDisplays", ":", reinterpret_cast<void *>(DisplayManagerAni::getFoldStatus)},

        ani_native_function {"syncOn", "Lstd/core/String;L@ohos/display/display/Callback;J:V", reinterpret_cast<void *>(DisplayManagerAni::registerCallback)},
        ani_native_function {"syncOff", "Lstd/core/String;L@ohos/display/display/Callback;J:V", reinterpret_cast<void *>(DisplayManagerAni::unRegisterCallback)}
    };
    // 绑定（指定nsp和func
    if ((ret = env->Namespace_BindNativeFunctions(nsp, funcs.data(), funcs.size()))) {
        WLOGFE("[ANI] bind namespace fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    // class函数绑定
    ani_class cutoutCls = nullptr;
    if ((ret = env->FindClass("L@ohos/display/display/CoutoutImpl;", &cutoutCls)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    ani_class displayCls = nullptr;
    if ((ret = env->FindClass("L@ohos/display/display/DisplayImpl;", &displayCls)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array methods = {
        ani_native_function {"getCutoutInfo", ":L@ohos/display/display/CutoutInfoImpl;", reinterpret_cast<void *>(DisplayAni::getCutoutInfo)},
    };
    // 通过class和methodname绑定到特定的class.mtd上
    if ((ret = env->Class_BindNativeMethods(cutoutCls, methods.data(), methods.size())) != ANI_OK) {
        WLOGFE("[ANI] bind fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    *result = ANI_VERSION_1;

    return ANI_OK;
}
}
 
}  // namespace Rosen
}  // namespace OHOS
