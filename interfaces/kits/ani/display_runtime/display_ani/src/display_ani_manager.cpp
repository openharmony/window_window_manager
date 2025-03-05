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
#include "display_ani_manager.h"
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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "DisplayAniManager"};
}
ani_env* DisplayManagerAni::env_ = nullptr;
DisplayManagerAni::DisplayManagerAni()
    : registerManager_(std::make_unique<DisplayAniRegisterManager>())
{
    ani_namespace nsp;
    if (env_ != nullptr) {
        if (env_->FindNamespace("L@ohos/display/display;", &nsp) != ANI_OK) {
            WLOGFE("[ANI] null env");
        }
        ani_function aniFunc;
        if (env_->Namespace_FindFunction(nsp, "setDisplayMgrRef", "J:V", &aniFunc) != ANI_OK) {
            WLOGFE("[ANI] null aniFunc");
        }
        // 向ts侧注册this指针
        if (env_->Function_Call_Void(aniFunc, (long)this) != ANI_OK) {
            WLOGFE("[ANI] call ts func failed");
        }
    }
}

void DisplayManagerAni::setAniEnv(ani_env* env)
{
    env_ = env;
}

ani_int DisplayManagerAni::getFoldDisplayModeAni(ani_env* env, ani_object obj)
{
    auto mode = SingletonContainer::Get<DisplayManager>().GetFoldDisplayMode();
    WLOGI("[ANI]" PRIu64", getFoldDisplayMode = %{public}u", mode);
    return static_cast<ani_int>(mode);
}
 
ani_boolean DisplayManagerAni::isFoldableAni(ani_env* env, ani_object obj)
{
    bool foldable = SingletonContainer::Get<DisplayManager>().IsFoldable();
    WLOGI("[ANI]" PRIu64", isFoldable = %{public}u", foldable);
    return static_cast<ani_boolean>(foldable);
}
 
ani_int DisplayManagerAni::getFoldStatus(ani_env* env, ani_object obj)
{
    auto status = SingletonContainer::Get<DisplayManager>().GetFoldStatus();
    WLOGI("[ANI]" PRIu64", getFoldStatus = %{public}u", status);
    return static_cast<ani_int>(status);
}
 
ani_object DisplayManagerAni::getCurrentFoldCreaseRegion(ani_env* env, ani_object obj)
{
    ani_object objRet = nullptr;
    auto region = SingletonContainer::Get<DisplayManager>().GetCurrentFoldCreaseRegion();
    ani_class cls;
    ani_class rectCls;
    if (ANI_OK != env->FindClass("L@ohos/display/display/FoldCreaseRegionImpl;", &cls)) {
        WLOGFE("[ANI] null class FoldCreaseRegionImpl");
        return objRet;
    }
    if (ANI_OK != DisplayAniUtils::NewAniObjectNoParams(env, cls, &objRet)) {
        WLOGFE("[ANI] create object fail");
        return objRet;
    }
    if (ANI_OK != env->FindClass("L@ohos/display/display/RectImpl;", &rectCls)) {
        WLOGFE("[ANI] null class RectImpl");
        return objRet;
    }
    ani_int displayId = static_cast<ani_int>(region->GetDisplayId());
    std::vector<DMRect> rects = region->GetCreaseRects();
    ani_array_ref aniRects = DisplayAniUtils::convertRects(rects, env);
    if (ANI_OK != env->Object_SetFieldByName_Int(objRet, "displayId", displayId)) {
        WLOGFE("[ANI] set displayId field fail");
    }
    if (ANI_OK != env->Object_SetFieldByName_Ref(objRet, "creaseRects", aniRects)) {
        WLOGFE("[ANI] set creaseRects field fail");
    }
    return objRet;
}

ani_array_ref DisplayManagerAni::getAllDisplaysAni(ani_env* env)
{
    std::vector<sptr<Display>> displays = SingletonContainer::Get<DisplayManager>().GetAllDisplays();
    ani_class cls;
    ani_array_ref arrayRef = nullptr;
    if (ANI_OK == env->FindClass("L@ohos/display/display/DisplayImpl;", &cls)) {
        env->Array_New_Ref(cls, displays.size(), nullptr, &arrayRef);
        for (int i = 0; i < displays.size(); i++) {
            ani_namespace nsp;
            if (env->FindNamespace("L@ohos/display/display;", &nsp) != ANI_OK) {
                WLOGFE("[ANI] null env");
            }
            ani_function fn;
            if (ANI_OK != env->Namespace_FindFunction(nsp, "getNewDisplay",
                ":L@ohos/display/display/DisplayImpl;", &fn)) {
                WLOGFE("[ANI] find nsp fail");
            }
            ani_ref displayObj;
            if (ANI_OK != env->Function_Call_Ref(fn, &displayObj)) {
                WLOGFE("[ANI] function call fail");
            }
            DisplayAniUtils::cvtDisplay(displays[i], env, static_cast<ani_object>(displayObj));
            if (ANI_OK != env->Array_Set_Ref(arrayRef, i, displayObj)) {
                WLOGFE("[ANI] null field displayIdFld");
            }
        }
    }
    return arrayRef;
}
 
ani_status DisplayManagerAni::getDisplayByIdSyncAni(ani_env* env, ani_object obj, ani_int displayId)
{
    if (displayId < 0) {
        WLOGE("[ANI] Invalid displayId, less than 0");
        return ANI_ERROR;
    }
    sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDisplayById(static_cast<DisplayId>(displayId));
    if (display == nullptr) {
        WLOGE("[ANI] Display null");
        return ANI_ERROR;
    }
    ani_status ret = DisplayAniUtils::cvtDisplay(display, env, obj);
    return ret;
}
 
ani_status DisplayManagerAni::getDefaultDisplaySyncAni(ani_env* env, ani_object obj)
{
    sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplaySync(true);
    if (display == nullptr) {
        WLOGE("[ANI] Display null");
        return ANI_ERROR;
    }
    ani_status ret = DisplayAniUtils::cvtDisplay(display, env, obj);
    return ret;
}

ani_object DisplayManagerAni::registerCallback(ani_env* env, ani_object obj,
    ani_string type, ani_ref callback, ani_long nativeObj)
{
    DisplayManagerAni* displayManagerAni = reinterpret_cast<DisplayManagerAni*>(nativeObj);
    return displayManagerAni != nullptr ? displayManagerAni->onRegisterCallback(env, type, callback) : nullptr;
}

ani_object DisplayManagerAni::onRegisterCallback(ani_env* env, ani_string type, ani_ref callback)
{
    std::string typeString;
    DisplayAniUtils::GetStdString(env, type, typeString);
    registerManager_->RegisterListener(typeString, env, callback);
    return DisplayAniUtils::CreateAniUndefined(env);
}

ani_object DisplayManagerAni::unRegisterCallback(ani_env* env, ani_object obj,
    ani_string type, ani_ref callback, ani_long nativeObj)
{
    DisplayManagerAni* displayManagerAni = reinterpret_cast<DisplayManagerAni*>(nativeObj);
    return displayManagerAni != nullptr ? displayManagerAni->onUnRegisterCallback(env, type, callback) : nullptr;
}

ani_object DisplayManagerAni::onUnRegisterCallback(ani_env* env, ani_string type, ani_ref callback)
{
    std::string typeString;
    DisplayAniUtils::GetStdString(env, type, typeString);
    registerManager_->UnregisterListener(typeString, env, callback);
    return DisplayAniUtils::CreateAniUndefined(env);
}
}
}