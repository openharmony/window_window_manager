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

#include "screen_ani_manager.h"

#include <algorithm>
#include <hitrace_meter.h>

#include "ani.h"
#include "ani_err_utils.h"
#include "dm_common.h"
#include "refbase.h"
#include "screen_ani.h"
#include "screen_ani_listener.h"
#include "screen_ani_utils.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

ScreenManagerAni::ScreenManagerAni()
{
}

void ScreenManagerAni::RegisterCallback(ani_env* env, ani_string type, ani_ref callback, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DMS, "[ANI] start to register screen callback: %{public}ld", (long)nativeObj);
    ScreenManagerAni* screenManagerAni = reinterpret_cast<ScreenManagerAni*>(nativeObj);
    if (screenManagerAni != nullptr) {
        screenManagerAni->OnRegisterCallback(env, type, callback);
    } else {
        TLOGE(WmsLogTag::DMS, "[ANI] screenManagerAni null ptr");
    }
}

void ScreenManagerAni::UnRegisterCallback(ani_env* env, ani_string type, ani_long nativeObj, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    ScreenManagerAni* screenManagerAni = reinterpret_cast<ScreenManagerAni*>(nativeObj);
    if (screenManagerAni != nullptr) {
        screenManagerAni->OnUnRegisterCallback(env, type, callback);
    } else {
        TLOGI(WmsLogTag::DMS, "[ANI] null ptr");
    }
}

void ScreenManagerAni::OnRegisterCallback(ani_env* env, ani_string type, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    ani_ref cbRef{};
    std::string typeString;
    ScreenAniUtils::GetStdString(env, type, typeString);
    if (env->GlobalReference_Create(callback, &cbRef) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] create global ref fail");
        env->GlobalReference_Delete(cbRef);
        return;
    }
    std::lock_guard<std::mutex> lock(mtx_);
    if (IsCallbackRegistered(env, typeString, cbRef)) {
        TLOGI(WmsLogTag::DMS, "[ANI] type %{public}s callback already registered!", typeString.c_str());
        env->GlobalReference_Delete(cbRef);
        return;
    }
    ani_boolean callbackUndefined = 0;
    env->Reference_IsUndefined(cbRef, &callbackUndefined);
    if (callbackUndefined) {
        TLOGE(WmsLogTag::DMS, "undefined");
        env->GlobalReference_Delete(cbRef);
        std::string errMsg = "[ANI] failed to register screen listener with type, cbk null or undefined";
        AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_PARAM, errMsg);
        return;
    }
    TLOGI(WmsLogTag::DMS, "create listener");
    sptr<ScreenAniListener> screenAniListener = new(std::nothrow) ScreenAniListener(env);
    if (screenAniListener == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI] screenListener is nullptr");
        env->GlobalReference_Delete(cbRef);
        AniErrUtils::ThrowBusinessError(env, DMError::DM_ERROR_INVALID_PARAM, "screenListener is nullptr");
        return;
    }
    screenAniListener->AddCallback(typeString, cbRef);
    screenAniListener->SetMainEventHandler();
    DmErrorCode ret = ProcessRegisterCallback(env, typeString, screenAniListener);
    if (ret != DmErrorCode::DM_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] register screen listener with type, errcode: %{public}d", ret);
        env->GlobalReference_Delete(cbRef);
        std::string errMsg = "Failed to register screen listener with type";
        AniErrUtils::ThrowBusinessError(env, ret, errMsg);
        return;
    }
    // add listener to map
    jsCbMap_[typeString][cbRef] = screenAniListener;
}

DmErrorCode ScreenManagerAni::ProcessRegisterCallback(ani_env* env, std::string& typeStr,
    sptr<ScreenAniListener> screenAniListener)
{
    DmErrorCode ret = DmErrorCode::DM_ERROR_INVALID_PARAM;
    if (typeStr == ANI_EVENT_CHANGE || typeStr == ANI_EVENT_CONNECT || typeStr == ANI_EVENT_DISCONNECT) {
        TLOGI(WmsLogTag::DMS, "ProcessRegisterCallback %{public}s", typeStr.c_str());
        ret = DM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<ScreenManager>().RegisterScreenListener(screenAniListener));
    }
    return ret;
}

void ScreenManagerAni::OnUnRegisterCallback(ani_env* env, ani_string type, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    std::string typeString;
    ScreenAniUtils::GetStdString(env, type, typeString);
    std::lock_guard<std::mutex> lock(mtx_);
    ani_boolean callbackUndefined = 0;
    env->Reference_IsUndefined(callback, &callbackUndefined);
    DmErrorCode ret;
    if (callbackUndefined) {
        TLOGI(WmsLogTag::DMS, "[ANI] OnUnRegisterCallback for all");
        ret = DM_JS_TO_ERROR_CODE_MAP.at(UnRegisterAllScreenListenerWithType(typeString));
    } else {
        TLOGI(WmsLogTag::DMS, "[ANI] OnUnRegisterCallback with type");
        ret = DM_JS_TO_ERROR_CODE_MAP.at(UnRegisterScreenListenerWithType(typeString, env, callback));
    }

    if (ret != DmErrorCode::DM_OK) {
        std::string errMsg = "[ANI] failed to unregister screen listener with type";
        TLOGE(WmsLogTag::DMS, "[ANI] failed to unregister screen listener with type");
        AniErrUtils::ThrowBusinessError(env, ret, errMsg);
    }
}

bool ScreenManagerAni::IsCallbackRegistered(ani_env* env, const std::string& type, ani_ref callback)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        TLOGI(WmsLogTag::DMS, "method %{public}s not registered!", type.c_str());
        return false;
    }
    for (const auto& iter : jsCbMap_[type]) {
        ani_boolean isEquals = false;
        env->Reference_StrictEquals(callback, iter.first, &isEquals);
        if (isEquals) {
            TLOGE(WmsLogTag::DMS, "callback already registered!");
            return true;
        }
    }
    return false;
}

DMError ScreenManagerAni::UnRegisterScreenListenerWithType(std::string type, ani_env* env, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] UnRegisterScreenListenerWithType begin");
    DMError ret = DMError::DM_OK;
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        TLOGI(WmsLogTag::DMS, "[ANI] methodName %{public}s not registered!", type.c_str());
        return ret;
    }
    ani_ref cbRef{};
    if (env->GlobalReference_Create(callback, &cbRef) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI]create global ref fail");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end(); it++) {
        ani_boolean isEquals = 0;
        env->Reference_StrictEquals(cbRef, it->first, &isEquals);
        if (isEquals) {
            it->second->RemoveCallback(env, type, callback);
            if (type == ANI_EVENT_CONNECT || type == ANI_EVENT_DISCONNECT || type == ANI_EVENT_CHANGE) {
                TLOGI(WmsLogTag::DMS, "[ANI] start to unregis screen event listener! event = %{public}s",
                    type.c_str());
                sptr<ScreenManager::IScreenListener> thisListener(it->second);
                ret = SingletonContainer::Get<ScreenManager>().UnregisterScreenListener(thisListener);
            }
            jsCbMap_[type].erase(it);
            break;
        }
    }
    if (jsCbMap_[type].empty()) {
        jsCbMap_.erase(type);
    }
    env->GlobalReference_Delete(cbRef);
    return ret;
}

DMError ScreenManagerAni::UnRegisterAllScreenListenerWithType(std::string type)
{
    TLOGI(WmsLogTag::DMS, "[ANI] begin");
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        TLOGI(WmsLogTag::DMS, "[ANI] UnregisterAllScreenListenerWithType methodName %{public}s not registered!",
            type.c_str());
        return DMError::DM_OK;
    }
    DMError ret = DMError::DM_OK;
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        it->second->RemoveAllCallback();
        if (type == ANI_EVENT_CONNECT || type == ANI_EVENT_DISCONNECT || type == ANI_EVENT_CHANGE) {
            sptr<ScreenManager::IScreenListener> thisListener(it->second);
            ret = SingletonContainer::Get<ScreenManager>().UnregisterScreenListener(thisListener);
        }
    }
    return ret;
}

ani_long ScreenManagerAni::MakeMirror(ani_env* env, ani_long mainScreen, ani_object mirrorScreen)
{
    ani_int length = 0;
    std::vector<ScreenId> screenIds;
    env->Object_GetPropertyByName_Int(mirrorScreen, "length", &length);
    TLOGI(WmsLogTag::DMS, "[ANI] length %{public}d", (ani_int)length);
    for (int32_t i = 0; i < length; i++) {
        ani_ref screenIdRef;
        auto ret = env->Object_CallMethodByName_Ref(mirrorScreen, "$_get", "i:C{std.core.Object}",
            &screenIdRef, (ani_int)i);
        if (ANI_OK != ret) {
            TLOGE(WmsLogTag::DMS, "[ANI] get ani_array index %{public}u fail, ret: %{public}u", (ani_int)i, ret);
            AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_PARAM, "Failed to get screenId");
            return static_cast<ani_long>(INVALID_SCREEN_ID);
        }
        ani_long screenId;
        ret = env->Object_CallMethodByName_Long(static_cast<ani_object>(screenIdRef), "unboxed", ":J", &screenId);
        if (ANI_OK != ret) {
            TLOGE(WmsLogTag::DMS, "[ANI] unboxed screenId failed, ret: %{public}u", ret);
            AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_PARAM, "Failed to unboxed screenId");
            return static_cast<ani_long>(INVALID_SCREEN_ID);
        }
        screenIds.emplace_back(static_cast<ScreenId>(screenId));
    }
    ScreenId screenGroupId = INVALID_SCREEN_ID;
    DmErrorCode ret = DM_JS_TO_ERROR_CODE_MAP.at(
        SingletonContainer::Get<ScreenManager>().MakeMirror(static_cast<ScreenId>(mainScreen),
            screenIds, screenGroupId));
    if (ret != DmErrorCode::DM_OK) {
        AniErrUtils::ThrowBusinessError(env, ret, "JsScreenManager::OnMakeMirror failed.");
        return static_cast<ani_long>(INVALID_SCREEN_ID);
    }
    return static_cast<ani_long>(screenGroupId);
}

ani_status ScreenManagerAni::InitScreenManagerAni(ani_namespace screenNameSpace, ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI] begin");
    ani_function setObjFunc = nullptr;
    ani_status ret = env->Namespace_FindFunction(screenNameSpace, "setScreenMgrRef", "l:", &setObjFunc);
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] find setNativeObj func fail %{public}u", ret);
        return ret;
    }
    std::unique_ptr<ScreenManagerAni> aniScreenManager = std::make_unique<ScreenManagerAni>();
    ret = env->Function_Call_Void(setObjFunc, aniScreenManager.release());
    if (ret != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] find setNativeObj func fail %{public}u", ret);
        return ret;
    }
    return ret;
}

void ScreenManagerAni::GetAllScreens(ani_env* env, ani_object screensAni)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "JsScreenManager::OnGetAllScreens");
    TLOGI(WmsLogTag::DMS, "[ANI] start");
    std::vector<sptr<Screen>> screens;
    auto res = DM_JS_TO_ERROR_CODE_MAP.at(SingletonContainer::Get<ScreenManager>().GetAllScreens(screens));
    if (res != DmErrorCode::DM_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] JsScreenManager::OnGetAllScreens failed.");
        AniErrUtils::ThrowBusinessError(env, res, "JsScreenManager::OnGetAllScreens failed.");
    } else if (!screens.empty()) {
        TLOGI(WmsLogTag::DMS, "[ANI] JsScreenManager::OnGetAllScreens succeed. size = %{public}u",
            static_cast<uint32_t>(screens.size()));
        if (ANI_OK != ScreenAniUtils::ConvertScreens(env, screens, screensAni)) {
            TLOGE(WmsLogTag::DMS, "[ANI] ConvertScreens fail");
        }
    } else {
        TLOGE(WmsLogTag::DMS, "[ANI] JsScreenManager::OnGetAllScreens null.");
        AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_SCREEN,
            "JsScreenManager::OnGetAllScreens failed.");
    }
}

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    using namespace OHOS::Rosen;
    TLOGI(WmsLogTag::DMS, "[ANI] start to ANI_Constructor");
    ani_status ret;
    ani_env *env;
    if ((ret = vm->GetEnv(ANI_VERSION_1, &env)) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] null env");
        return ANI_NOT_FOUND;
    }
    ani_namespace nsp;
    if ((ret = env->FindNamespace("@ohos.screen.screen", &nsp)) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    ScreenManagerAni::InitScreenManagerAni(nsp, env);
    std::array funcs = {
        ani_native_function {"syncOn", nullptr,
            reinterpret_cast<void *>(ScreenManagerAni::RegisterCallback)},
        ani_native_function {"syncOff", nullptr,
            reinterpret_cast<void *>(ScreenManagerAni::UnRegisterCallback)},
        ani_native_function {"makeMirrorInternal", nullptr,
            reinterpret_cast<void *>(ScreenManagerAni::MakeMirror)},
        ani_native_function {"getAllScreensInternal", nullptr,
            reinterpret_cast<void *>(ScreenManagerAni::GetAllScreens)}
    };
    if ((ret = env->Namespace_BindNativeFunctions(nsp, funcs.data(), funcs.size()))) {
        TLOGE(WmsLogTag::DMS, "[ANI] bind namespace fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    ani_class screenCls = nullptr;
    if ((ret = env->FindClass("@ohos.screen.screen.ScreenImpl", &screenCls)) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    std::array methods = {
        ani_native_function {"setDensityDpiInternal", "d:",
            reinterpret_cast<void *>(ScreenAni::SetDensityDpi)},
        ani_native_function {"nativeTransferStatic", "C{std.interop.ESValue}C{std.core.Object}:z",
            reinterpret_cast<void *>(ScreenAni::TransferStatic)},
        ani_native_function {"nativeTransferDynamic", "l:C{std.interop.ESValue}",
            reinterpret_cast<void *>(ScreenAni::TransferDynamic)},
    };
    if ((ret = env->Class_BindNativeMethods(screenCls, methods.data(), methods.size())) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] bind screen class fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}

}
}