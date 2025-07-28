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
DisplayAni::DisplayAni(const sptr<Display>& display) : display_(display)
{
}

void DisplayAni::GetCutoutInfo(ani_env* env, ani_object obj, ani_object cutoutInfoObj)
{
    auto display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplay();
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
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
        DisplayAniUtils::ConvertRect(rects[i], static_cast<ani_object>(currentCutoutInfo), env);
    }
    // waterfall area
    ani_ref waterfallObj = nullptr;
    env->Object_GetFieldByName_Ref(cutoutInfoObj, "<property>waterfallDisplayAreaRects", &waterfallObj);
    auto waterfallDisplayAreaRects = cutoutInfo->GetWaterfallDisplayAreaRects();
    DisplayAniUtils::ConvertWaterArea(waterfallDisplayAreaRects, static_cast<ani_object>(waterfallObj), env);
}

void DisplayAni::GetAvailableArea(ani_env* env, ani_object obj, ani_object availableAreaObj)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    ani_double id;
    env->Object_GetFieldByName_Double(obj, "<property>id", &id);
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(id);
    if (display == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI] can not find display.");
        AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_SCREEN,
            "JsDisplay::GetAvailableArea failed, can not find display.");
        return;
    }
    DMRect area;
    DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(display->GetAvailableArea(area));
    if (ret != DmErrorCode::DM_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] Display get available area failed.");
        AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_SCREEN,
            "JsDisplay::GetAvailableArea failed.");
        return;
    }
    DisplayAniUtils::ConvertRect(area, availableAreaObj, env);
}

ani_boolean DisplayAni::HasImmersiveWindow(ani_env* env, ani_object obj)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    ani_double id;
    env->Object_GetFieldByName_Double(obj, "<property>id", &id);
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(id);
    if (display == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI]can not find display.");
        AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_SCREEN,
            "can not find display.");
        return false;
    }
    bool immersive = false;
    DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(display->HasImmersiveWindow(immersive));
    if (ret != DmErrorCode::DM_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] get display has immersive window failed.");
        AniErrUtils::ThrowBusinessError(env, ret, "JsDisplay::HasImmersiveWindow failed.");
        return false;
    }
    return immersive;
}

bool DisplayAni::IsCallbackRegistered(ani_env* env, const std::string& type, ani_ref callback)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        TLOGI(WmsLogTag::DMS, "method %{public}s not registered.", type.c_str());
        return false;
    }
    for (const auto& iter : jsCbMap_[type]) {
        ani_boolean isEquals = false;
        env->Reference_StrictEquals(callback, iter.first, &isEquals);
        if (isEquals) {
            TLOGE(WmsLogTag::DMS, "callback is already registered!");
            return true;
        }
    }
    return false;
}

void DisplayAni::RegisterCallback(ani_env* env, ani_object obj, ani_string type, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    ani_long displayRef;
    env->Object_GetFieldByName_Long(obj, "displayRef", &displayRef);
    DisplayAni* displayAni = reinterpret_cast<DisplayAni*>(displayRef);
    if (displayAni != nullptr) {
        displayAni->OnRegisterCallback(env, obj, type, callback);
    } else {
        TLOGE(WmsLogTag::DMS, "[ANI] displayAni not found");
    }
}

void DisplayAni::OnRegisterCallback(ani_env* env, ani_object obj, ani_string type, ani_ref callback)
{
    std::string typeString;
    DisplayAniUtils::GetStdString(env, type, typeString);
    ani_boolean callbackUndefined = 0;
    env->Reference_IsUndefined(callback, &callbackUndefined);
    if (callbackUndefined) {
        std::string errMsg = "[ANI] failed to register display listener with type, cbk null or undefined";
        TLOGE(WmsLogTag::DMS, "callback undef");
        AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_PARAM, errMsg);
        return;
    }
    if (IsCallbackRegistered(env, typeString, callback)) {
        TLOGI(WmsLogTag::DMS, "Callback has already been registered!");
        return;
    }
    
    sptr<DisplayAniListener> displayAniListener = new(std::nothrow) DisplayAniListener(env);
    if (displayAniListener == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI]displayListener is nullptr");
        AniErrUtils::ThrowBusinessError(env, DMError::DM_ERROR_INVALID_PARAM, "displayListener is nullptr");
        return;
    }
    DMError ret = DMError::DM_OK;
    std::lock_guard<std::mutex> lock(mtx_);
    if (typeString == EVENT_AVAILABLE_AREA_CHANGED) {
        auto displayId = display_->GetId();
        TLOGI(WmsLogTag::DMS, "[ANI] availableAreaChange begin");
        ret = SingletonContainer::Get<DisplayManager>().RegisterAvailableAreaListener(displayAniListener, displayId);
    } else {
        ret = DMError::DM_ERROR_INVALID_PARAM;
    }
    DmErrorCode retCode = DM_JS_TO_ERROR_CODE_MAP.at(ret);
    if (retCode != DmErrorCode::DM_OK) {
        TLOGI(WmsLogTag::DMS, "Failed to register display listener with type");
        std::string errMsg = "Failed to register display listener with type";
        AniErrUtils::ThrowBusinessError(env, DMError::DM_ERROR_INVALID_PARAM, errMsg);
        return;
    }
    ani_ref cbRef{};
    if (ANI_OK != env->GlobalReference_Create(callback, &cbRef)) {
        TLOGE(WmsLogTag::DMS, "[ANI]create global ref fail");
        return;
    }
    displayAniListener->AddCallback(typeString, cbRef);
    jsCbMap_[typeString][callback] = displayAniListener;
}

void DisplayAni::UnRegisterCallback(ani_env* env, ani_object obj, ani_string type, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    ani_long displayRef;
    env->Object_GetFieldByName_Long(obj, "displayRef", &displayRef);
    DisplayAni* displayAni = reinterpret_cast<DisplayAni*>(displayRef);
    if (displayAni != nullptr) {
        displayAni->OnUnRegisterCallback(env, obj, type, callback);
    } else {
        TLOGE(WmsLogTag::DMS, "[ANI] DisplayAni null");
    }
}

void DisplayAni::OnUnRegisterCallback(ani_env* env, ani_object obj, ani_string type, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    std::string typeString;
    DisplayAniUtils::GetStdString(env, type, typeString);
    ani_boolean callbackUndefined = 0;
    env->Reference_IsUndefined(callback, &callbackUndefined);
    DmErrorCode ret;
    if (callbackUndefined) {
        TLOGI(WmsLogTag::DMS, "[ANI] for all");
        ret = DM_JS_TO_ERROR_CODE_MAP.at(UnregisterAllDisplayListenerWithType(typeString));
    } else {
        TLOGI(WmsLogTag::DMS, "[ANI] with type");
        ret = DM_JS_TO_ERROR_CODE_MAP.at(UnregisterDisplayListenerWithType(typeString, env, callback));
    }

    if (ret != DmErrorCode::DM_OK) {
        std::string errMsg = "[ANI] failed to unregister display listener with type";
        TLOGE(WmsLogTag::DMS, "[ANI] failed to unregister display listener with type");
        AniErrUtils::ThrowBusinessError(env, DMError::DM_ERROR_INVALID_PARAM, errMsg);
    }
}

DMError DisplayAni::UnregisterAllDisplayListenerWithType(std::string type)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    std::lock_guard<std::mutex> lock(mtx_);
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        TLOGI(WmsLogTag::DMS, "[ANI] methodName %{public}s not registered!",
            type.c_str());
        return DMError::DM_OK;
    }
    DMError ret = DMError::DM_OK;
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        if (it->second == nullptr) {
            TLOGE(WmsLogTag::DMS, "listener is null");
            continue;
        }
        it->second->RemoveAllCallback();
        if (type == EVENT_AVAILABLE_AREA_CHANGED) {
            auto displayId = display_->GetId();
            sptr<DisplayManager::IAvailableAreaListener> thisListener(it->second);
            ret = SingletonContainer::Get<DisplayManager>().UnregisterAvailableAreaListener(thisListener, displayId);
        } else {
            ret = DMError::DM_ERROR_INVALID_PARAM;
        }
        jsCbMap_[type].erase(it++);
    }
    jsCbMap_.erase(type);
    return ret;
}

DMError DisplayAni::UnregisterDisplayListenerWithType(std::string type, ani_env* env, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    std::lock_guard<std::mutex> lock(mtx_);
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        TLOGI(WmsLogTag::DMS, "[ANI] methodName %{public}s not registered!", type.c_str());
        return DMError::DM_OK;
    }
    DMError ret = DMError::DM_OK;
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        ani_boolean isEquals = 0;
        env->Reference_StrictEquals(callback, it->first, &isEquals);
        if (isEquals) {
            if (it->second == nullptr) {
                TLOGE(WmsLogTag::DMS, "listener is null");
                continue;
            }
            it->second->RemoveCallback(env, type, callback);
            if (type == EVENT_AVAILABLE_AREA_CHANGED) {
                TLOGI(WmsLogTag::DMS, "[ANI] start to unregis display event listener! event = %{public}s",
                    type.c_str());
                auto displayId = display_->GetId();
                sptr<DisplayManager::IAvailableAreaListener> thisListener(it->second);
                ret = SingletonContainer::Get<DisplayManager>().UnregisterAvailableAreaListener(thisListener,
                    displayId);
            } else {
                ret = DMError::DM_ERROR_INVALID_PARAM;
            }
            jsCbMap_[type].erase(it++);
            TLOGI(WmsLogTag::DMS, "[ANI] unregister display listener with type: %{public}s ret: %{public}u",
                type.c_str(), ret);
            break;
        } else {
            it++;
        }
    }
    if (jsCbMap_[type].empty()) {
        jsCbMap_.erase(type);
    }
    return ret;
}

void DisplayAni::CreateDisplayAni(sptr<Display> display, ani_object displayObj, ani_env* env)
{
    std::unique_ptr<DisplayAni> displayAni = std::make_unique<DisplayAni>(display);
    if (ANI_OK != env->Object_SetFieldByName_Long(static_cast<ani_object>(displayObj),
        "displayRef", reinterpret_cast<ani_long>(displayAni.release()))) {
        TLOGE(WmsLogTag::DMS, "[ANI] set displayAni ref fail");
    }
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
    DisplayManagerAni::InitDisplayManagerAni(nsp, env);
    std::array funcs = {
        ani_native_function {"isFoldable", ":Z", reinterpret_cast<void *>(DisplayManagerAni::IsFoldableAni)},
        ani_native_function {"getFoldDisplayModeNative", ":I",
            reinterpret_cast<void *>(DisplayManagerAni::GetFoldDisplayModeAni)},
        ani_native_function {"getFoldStatusNative", ":I", reinterpret_cast<void *>(DisplayManagerAni::GetFoldStatus)},
        ani_native_function {"getCurrentFoldCreaseRegionNative", "Lstd/core/Object;J:V",
            reinterpret_cast<void *>(DisplayManagerAni::GetCurrentFoldCreaseRegion)},
        ani_native_function {"getDisplayByIdSyncNative", "Lstd/core/Object;D:V",
            reinterpret_cast<void *>(DisplayManagerAni::GetDisplayByIdSyncAni)},
        ani_native_function {"getDefaultDisplaySyncNative", "Lstd/core/Object;:V",
            reinterpret_cast<void *>(DisplayManagerAni::GetDefaultDisplaySyncAni)},
        ani_native_function {"getAllDisplaysSyncNative", "Lescompat/Array;:V",
            reinterpret_cast<void *>(DisplayManagerAni::GetAllDisplaysAni)},
        ani_native_function {"syncOn", nullptr,
            reinterpret_cast<void *>(DisplayManagerAni::RegisterCallback)},
        ani_native_function {"syncOff", nullptr,
            reinterpret_cast<void *>(DisplayManagerAni::UnRegisterCallback)},
        ani_native_function {"hasPrivateWindow", nullptr,
            reinterpret_cast<void *>(DisplayManagerAni::HasPrivateWindow)},
        ani_native_function {"getAllDisplayPhysicalResolutionNative", nullptr,
            reinterpret_cast<void *>(DisplayManagerAni::GetAllDisplayPhysicalResolution)},
        ani_native_function {"isCaptured", nullptr, reinterpret_cast<void *>(DisplayManagerAni::IsCaptured)},
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
            reinterpret_cast<void *>(DisplayAni::GetCutoutInfo)},
        ani_native_function {"getAvailableAreaInternal", "L@ohos/display/display/Rect;:V",
            reinterpret_cast<void *>(DisplayAni::GetAvailableArea)},
        ani_native_function {"hasImmersiveWindowInternal", ":Z",
            reinterpret_cast<void *>(DisplayAni::HasImmersiveWindow)},
        ani_native_function {"syncOn", nullptr,
            reinterpret_cast<void *>(DisplayAni::RegisterCallback)},
        ani_native_function {"syncOff", nullptr,
            reinterpret_cast<void *>(DisplayAni::UnRegisterCallback)},
    };
    if ((ret = env->Class_BindNativeMethods(displayCls, methods.data(), methods.size())) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] bind class fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}

}  // namespace Rosen
}  // namespace OHOS
