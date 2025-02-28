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

#ifndef OHOS_JS_DISPLAY_MANAGER_H
#define OHOS_JS_DISPLAY_MANAGER_H

#include <hitrace_meter.h>
 
#include "../../common/ani.h"
#include "display_ani.h"
#include "display_info.h"
#include "display.h"
#include "singleton_container.h"
#include "display_manager.h"
#include "window_manager_hilog.h"
#include "dm_common.h"
#include "display_ani_utils.h"
#include "refbase.h"



namespace OHOS {
namespace Rosen {
 
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsDisplayManager"};
}
 
ani_int DisplayAni::getFoldDisplayModeAni(ani_env* env)
{
    auto mode = SingletonContainer::Get<DisplayManager>().GetFoldDisplayMode();
    WLOGD("[NAPI]" PRIu64", getFoldDisplayMode = %{public}u", mode);
    return static_cast<ani_int>(mode);
}
 
ani_boolean DisplayAni::isFoldableAni()
{
    bool foldable = SingletonContainer::Get<DisplayManager>().IsFoldable();
    WLOGD("[NAPI]" PRIu64", isFoldable = %{public}u", foldable);
    return static_cast<ani_boolean>(foldable);
}
 
ani_int DisplayAni::getFoldStatus()
{
    auto status = SingletonContainer::Get<DisplayManager>().GetFoldStatus();
    WLOGD("[NAPI]" PRIu64", getFoldStatus = %{public}u", status);
    return static_cast<ani_int>(status);
}
 
ani_object DisplayAni::getCurrentFoldCreaseRegion(ani_env *env)
{
    ani_object obj = nullptr;
    auto region = SingletonContainer::Get<DisplayManager>().GetCurrentFoldCreaseRegion();
    WLOGI("[NAPI]" PRIu64", getCurrentFoldCreaseRegion");
    ani_class cls;
    ani_class rectCls;
    if (ANI_OK != env->FindClass("L@ohos/display/display/FoldCreaseRegionImpl", &cls)) {
        WLOGFE("[ANI] null class FoldCreaseRegionImpl");
        return obj;
    }
    if (ANI_OK != env->FindClass("L@ohos/display/display/RectImpl", &rectCls)) {
        WLOGFE("[ANI] null class RectImpl");
        return obj;
    }
    ani_field displayIdFld;
    ani_field creaseRectsFld;
    if (ANI_OK != env->Class_GetField(cls, "displayId", &displayIdFld)) {
        WLOGFE("[ANI] null field displayIdFld");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "creaseRects", &creaseRectsFld)) {
        WLOGFE("[ANI] null field creaseRectsFld");
        return obj;
    }
    ani_int displayId = static_cast<ani_int>(region->GetDisplayId());
    std::vector<DMRect> rects = region->GetCreaseRects();
    ani_fixedarray_ref aniRects = DisplayAniUtils::convertRects(rects, env);
    env->Object_SetField_Int(obj, displayIdFld, displayId);
    env->Object_SetField_Ref(obj, creaseRectsFld, aniRects);
    return obj;
}

ani_object DisplayAni::getCutoutInfo(ani_env *env)
{
    auto display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplay();
    sptr<CutoutInfo> cutoutInfo = display->GetCutoutInfo();
    ani_object obj = nullptr;
    ani_field boundingRectsFld;
    ani_class cls;
    if (ANI_OK != env->FindClass("L@ohos/display/display/CutoutInfoImpl", &cls)) {
        WLOGFE("[ANI] null class CutoutInfoImpl");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "boundingRects", &boundingRectsFld)) {
        WLOGFE("[ANI] null field boundingRectsFld");
        return obj;
    }
    // set boundingRects field
    ani_fixedarray_ref arrayRef = DisplayAniUtils::convertRects(cutoutInfo->GetBoundingRects(), env);
    env->Object_SetField_Ref(obj, boundingRectsFld, arrayRef);
    
    ani_object waterfallObj = nullptr;
    ani_field waterfallFld;
    ani_field leftFld;
    ani_field rightFld;
    ani_field topFld;
    ani_field bottomFld;
    if (ANI_OK != env->Class_GetField(cls, "waterfallDisplayAreaRects", &waterfallFld)) {
        WLOGFE("[ANI] null field waterfallFld");
        return obj;
    }
    if (ANI_OK != env->FindClass("L@ohos/display/display/WaterfallDisplayAreaRectsImpl", &cls)) {
        WLOGFE("[ANI] null class WaterfallDisplayAreaRectsImpl");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "left", &leftFld)) {
        WLOGFE("[ANI] null field left");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "right", &rightFld)) {
        WLOGFE("[ANI] null field right");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "top", &topFld)) {
        WLOGFE("[ANI] null field top");
        return obj;
    }
    if (ANI_OK != env->Class_GetField(cls, "bottom", &bottomFld)) {
        WLOGFE("[ANI] null field bottom");
        return obj;
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
    env->Object_SetField_Ref(obj, waterfallFld, waterfallObj);
    return obj;
}
 
ani_fixedarray_ref DisplayAni::getAllDisplaysAni(ani_env *env) {
    std::vector<sptr<Display>> displays = SingletonContainer::Get<DisplayManager>().GetAllDisplays();
    if (displays.size() == 0) {
        WLOGE("[ANI] Invalid displays, size equals 0");
        return nullptr;
    }
    ani_fixedarray_ref obj = DisplayAniUtils::convertDisplays(displays, env);
    return obj;
}
 
ani_object DisplayAni::getDisplayByIdSyncAni(ani_env *env, ani_int displayId) {
    if (displayId < 0) {
        WLOGE("[ANI] Invalid displayId, less than 0");
        return nullptr;
    }
    sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDisplayById(static_cast<DisplayId>(displayId));
    if (display == nullptr) {
        WLOGE("[ANI] Display null");
        return nullptr;
    }
    ani_object obj = DisplayAniUtils::convertDisplay(display, env);
    return obj;
}
 
ani_object DisplayAni::getDefaultDisplaySyncAni(ani_env *env) {
    sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplaySync(true);
    if (display == nullptr) {
        WLOGE("[ANI] Display null");
        return nullptr;
    }
    ani_object obj = DisplayAniUtils::convertDisplay(display, env);
    return obj;
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

    // namespace函数绑定
    ani_namespace nsp;
    // 找namespace
    if ((ret = env->FindNamespace("L@ohos/display/display;", &nsp)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    // ts->c++的映射
    std::array funcs = {
        ani_native_function {"isFoldable", ":Z", reinterpret_cast<void *>(DisplayAni::isFoldableAni)},
        ani_native_function {"getFoldDisplayModeAni", ":I", reinterpret_cast<void *>(DisplayAni::getFoldDisplayModeAni)},
        ani_native_function {"getFoldDisplayModeAni", ":I", reinterpret_cast<void *>(DisplayAni::getFoldStatus)},
    };
    // 绑定（指定nsp和func
    if ((ret = env->Namespace_BindNativeFunctions(nsp, funcs.data(), funcs.size()))) {
        WLOGFE("[ANI] bind namespace fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    // class函数绑定
    ani_class cls = nullptr;
    if ((ret = env->FindClass("L@ohos/display/display/Display;", &cls)) != ANI_OK) {
        WLOGFE("[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array methods = {
        ani_native_function {"getCutoutInfo", ":L@ohos/display/display/CutoutInfo", reinterpret_cast<void *>(DisplayAni::getCutoutInfo)},
    };
    // 通过class和methodname绑定到特定的class.mtd上
    if ((ret = env->Class_BindNativeMethods(cls, methods.data(), methods.size())) != ANI_OK) {
        WLOGFE("[ANI] bind fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }

    *result = ANI_VERSION_1;

    return ANI_OK;
}
}
 
}  // namespace Rosen
}  // namespace OHOS
 
#endif