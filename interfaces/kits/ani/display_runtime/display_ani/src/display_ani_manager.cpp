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
#include <algorithm>

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
#include "ani_err_utils.h"

namespace OHOS {
namespace Rosen {

ani_env* DisplayManagerAni::env_ = nullptr;
DisplayManagerAni::DisplayManagerAni()
    : registerManager_(std::make_unique<DisplayAniRegisterManager>())
{
    ani_namespace nsp;
    if (env_ != nullptr) {
        if (env_->FindNamespace("L@ohos/display/display;", &nsp) != ANI_OK) {
            TLOGE(WmsLogTag::DMS, "[ANI] null env");
        }
        ani_function aniFunc;
        if (env_->Namespace_FindFunction(nsp, "setDisplayMgrRef", "J:V", &aniFunc) != ANI_OK) {
            TLOGE(WmsLogTag::DMS, "[ANI] null aniFunc");
        }
        // 向ts侧注册this指针
        if (env_->Function_Call_Void(aniFunc, (long)this) != ANI_OK) {
            TLOGE(WmsLogTag::DMS, "[ANI] call ts func failed");
        }
    }
}

void DisplayManagerAni::setAniEnv(ani_env* env)
{
    env_ = env;
}

ani_int DisplayManagerAni::getFoldDisplayModeAni(ani_env* env)
{
    auto mode = SingletonContainer::Get<DisplayManager>().GetFoldDisplayMode();
    TLOGI(WmsLogTag::DMS, "[ANI]" PRIu64", getFoldDisplayMode = %{public}u", mode);
    return static_cast<ani_int>(mode);
}

ani_boolean DisplayManagerAni::isFoldableAni(ani_env* env)
{
    bool foldable = SingletonContainer::Get<DisplayManager>().IsFoldable();
    TLOGI(WmsLogTag::DMS, "[ANI]" PRIu64", isFoldable = %{public}u", foldable);
    return static_cast<ani_boolean>(foldable);
}

ani_int DisplayManagerAni::getFoldStatus(ani_env* env)
{
    auto status = SingletonContainer::Get<DisplayManager>().GetFoldStatus();
    TLOGI(WmsLogTag::DMS, "[ANI]" PRIu64", getFoldStatus = %{public}u", status);
    return static_cast<ani_int>(status);
}

ani_object DisplayManagerAni::getCurrentFoldCreaseRegion(ani_env* env, ani_object obj)
{
    auto region = SingletonContainer::Get<DisplayManager>().GetCurrentFoldCreaseRegion();
    ani_class rectCls;
    if (ANI_OK != env->FindClass("L@ohos/display/display/RectImpl;", &rectCls)) {
        TLOGE(WmsLogTag::DMS, "[ANI] null class RectImpl");
        return obj;
    }
    ani_int displayId = static_cast<ani_int>(region->GetDisplayId());
    std::vector<DMRect> rects = region->GetCreaseRects();
    ani_array_ref aniRects = nullptr;
    if (ANI_OK != env->Object_SetFieldByName_Int(obj, "<property>displayId", displayId)) {
        TLOGE(WmsLogTag::DMS, "[ANI] set displayId field fail");
    }
    if (ANI_OK != env->Object_SetFieldByName_Ref(obj, "<property>creaseRects", aniRects)) {
        TLOGE(WmsLogTag::DMS, "[ANI] set creaseRects field fail");
    }
    return obj;
}

void DisplayManagerAni::getAllDisplaysAni(ani_env* env, ani_object arrayObj)
{
    std::vector<sptr<Display>> displays = SingletonContainer::Get<DisplayManager>().GetAllDisplays();
    if (displays.empty()) {
        AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_SCREEN, "");
    }
    ani_double length;
    if(ANI_OK != env->Object_GetPropertyByName_Double(arrayObj, "length", &length)){
        TLOGE(WmsLogTag::DMS, "[ANI] get ani_array len fail");
    }

    for(int i = 0; i < std::min(int(length), static_cast<int>(displays.size())); i++){
        ani_ref currentDisplay;
        if(ANI_OK != env->Object_CallMethodByName_Ref(arrayObj, "$_get", "I:Lstd/core/Object;", &currentDisplay, (ani_int)i)){
            TLOGE(WmsLogTag::DMS, "[ANI] get ani_array index %{public}u fail", (ani_int)i);
        }
        TLOGI(WmsLogTag::DMS, "current i: %{public}d", i);
        DisplayAniUtils::cvtDisplay(displays[i], env, static_cast<ani_object>(currentDisplay));
    }
    TLOGI(WmsLogTag::DMS, "[ANI] getAllDisplaysAni end");
}

ani_status DisplayManagerAni::getDisplayByIdSyncAni(ani_env* env, ani_object obj, ani_int displayId)
{
    if (displayId < 0) {
        TLOGE(WmsLogTag::DMS, "[ANI] Invalid displayId, less than 0");
        return ANI_ERROR;
    }
    sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDisplayById(static_cast<DisplayId>(displayId));
    if (display == nullptr) {
        AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_SYSTEM_INNORMAL, "");
    }
    if (display == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI] Display null");
        return ANI_ERROR;
    }
    ani_status ret = DisplayAniUtils::cvtDisplay(display, env, obj);
    return ret;
}

ani_status DisplayManagerAni::getDefaultDisplaySyncAni(ani_env* env, ani_object obj)
{
    sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplaySync(true);
    if (display == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI] Display null");
        return ANI_ERROR;
    }
    TLOGI(WmsLogTag::DMS, "[ANI] getDefaultDisplaySyncAni");
    ani_status ret = DisplayAniUtils::cvtDisplay(display, env, obj);
    return ret;
}

ani_object DisplayManagerAni::registerCallback(ani_env* env, ani_string type,
    ani_ref callbackInternal, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DMS, "[ANI] start to register");
    DisplayManagerAni* displayManagerAni = reinterpret_cast<DisplayManagerAni*>(nativeObj);
    return displayManagerAni != nullptr ? displayManagerAni->onRegisterCallback(env, type, static_cast<ani_object>(callbackInternal)) : nullptr;
}

ani_object DisplayManagerAni::onRegisterCallback(ani_env* env, ani_string type, ani_object callbackInternal)
{
    std::string typeString;
    DisplayAniUtils::GetStdString(env, type, typeString);
    registerManager_->RegisterListener(typeString, env, callbackInternal);
    return DisplayAniUtils::CreateAniUndefined(env);
}

ani_object DisplayManagerAni::unRegisterCallback(ani_env* env, ani_string type,
    ani_ref callbackInternal, ani_long nativeObj)
{
    DisplayManagerAni* displayManagerAni = reinterpret_cast<DisplayManagerAni*>(nativeObj);
    return displayManagerAni != nullptr ? displayManagerAni->onUnRegisterCallback(env, type, callbackInternal) : nullptr;
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