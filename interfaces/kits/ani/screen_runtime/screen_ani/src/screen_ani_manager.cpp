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
#include "singleton_container.h"
#include "window_manager_hilog.h"
#include "dm_common.h"
#include "refbase.h"
#include "screen_ani_manager.h"
#include "screen_ani_utils.h"
#include "screen_ani_listener.h"
#include "ani_err_utils.h"

namespace OHOS {
namespace Rosen {

ScreenManagerAni::ScreenManagerAni()
{
}

void ScreenManagerAni::registerCallback(ani_env* env, ani_string type, ani_ref callback, ani_long nativeObj)
{
    TLOGI(WmsLogTag::DMS, "[ANI] start to register screen callback: %{public}lld", nativeObj);
    ScreenManagerAni* screenManagerAni = reinterpret_cast<ScreenManagerAni*>(nativeObj);
    if (screenManagerAni != nullptr) {
        screenManagerAni->onRegisterCallback(env, type, callback);
    } else {
        TLOGE(WmsLogTag::DMS, "[ANI] screenManagerAni null ptr");
    }
}

void ScreenManagerAni::unRegisterCallback(ani_env* env, ani_string type, ani_long nativeObj, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] unRegisterCallback begin");
    ScreenManagerAni* screenManagerAni = reinterpret_cast<ScreenManagerAni*>(nativeObj);
    if (screenManagerAni != nullptr) {
        screenManagerAni->onUnRegisterCallback(env, type, callback);
    } else {
        TLOGI(WmsLogTag::DMS, "[ANI] null ptr");
    }
}

void ScreenManagerAni::onRegisterCallback(ani_env* env, ani_string type, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] onRegisterCallback begin");
    std::lock_guard<std::mutex> lock(mtx_);
    std::string typeString;
    ScreenAniUtils::GetStdString(env, type, typeString);
    ani_boolean callbackUndefined = 0;
    ani_boolean callbackIsNull = 0;
    env->Reference_IsUndefined(callback, &callbackUndefined);
    env->Reference_IsNull(callback, &callbackIsNull);
    DmErrorCode ret;
    if (callbackUndefined || callbackIsNull) {
        std::string errMsg = "[ANI] failed to register screen listener with type, cbk null or undefined";
        TLOGE(WmsLogTag::DMS, "callbackNull or undefined");
        AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_PARAM, errMsg);
        return;
    }
    ani_ref cbRef{};
    if (env->GlobalReference_Create(callback, &cbRef) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]create global ref fail");
    };
    TLOGI(WmsLogTag::DMS, "create listener");
    sptr<ScreenAniListener> screenAniListener = new(std::nothrow) ScreenAniListener(env);
    if (screenAniListener == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI] screenListener is nullptr");
        AniErrUtils::ThrowBusinessError(env, DMError::DM_ERROR_INVALID_PARAM, "screenListener is nullptr");
        return;
    }
    screenAniListener->AddCallback(typeString, cbRef);
    ret = processRegisterCallback(env, typeString, screenAniListener);
    if (ret != DmErrorCode::DM_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] register screen listener with type, errcode: %{public}d", ret);
        std::string errMsg = "Failed to register screen listener with type";
        AniErrUtils::ThrowBusinessError(env, DmErrorCode::DM_ERROR_INVALID_PARAM, errMsg);
        return;
    }
    // add listener to map
    jsCbMap_[typeString][callback] = screenAniListener;
    if (typeString == EVENT_CHANGE) {
        screenAniListener->OnChange(0);
    }
}

DmErrorCode ScreenManagerAni::processRegisterCallback(ani_env* env, std::string& typeStr,
    sptr<ScreenAniListener> screenAniListener)
{
    DmErrorCode ret = DmErrorCode::DM_ERROR_INVALID_PARAM;
    if (typeStr == EVENT_CHANGE) {
        TLOGI(WmsLogTag::DMS, "processRegisterCallback %{public}s", typeStr.c_str());
        ret = DM_JS_TO_ERROR_CODE_MAP.at(
            SingletonContainer::Get<ScreenManager>().RegisterScreenListener(screenAniListener));
    }
    return ret;
}

void ScreenManagerAni::onUnRegisterCallback(ani_env* env, ani_string type, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] onUnRegisterCallback begin");
    std::string typeString;
    ScreenAniUtils::GetStdString(env, type, typeString);
    std::lock_guard<std::mutex> lock(mtx_);
    ani_boolean callbackNull = 0;
    env->Reference_IsUndefined(callback, &callbackNull);
    DmErrorCode ret;
    if (callbackNull) {
        TLOGI(WmsLogTag::DMS, "[ANI] onUnRegisterCallback for all");
        ret = DM_JS_TO_ERROR_CODE_MAP.at(UnRegisterAllScreenListenerWithType(typeString));
    } else {
        TLOGI(WmsLogTag::DMS, "[ANI] onUnRegisterCallback with type");
        ret = DM_JS_TO_ERROR_CODE_MAP.at(UnRegisterScreenListenerWithType(typeString, env, callback));
    }

    if (ret != DmErrorCode::DM_OK) {
        DmErrorCode errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
        if (ret == DmErrorCode::DM_ERROR_NOT_SYSTEM_APP) {
            errCode = ret;
        }
        std::string errMsg = "[ANI] failed to unregister screen listener with type";
        TLOGE(WmsLogTag::DMS, "[ANI] failed to unregister screen listener with type");
        AniErrUtils::ThrowBusinessError(env, DMError::DM_ERROR_INVALID_PARAM, errMsg);
    }
}

DMError ScreenManagerAni::UnRegisterScreenListenerWithType(std::string type, ani_env* env, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] UnRegisterScreenListenerWithType begin");
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        TLOGI(WmsLogTag::DMS, "[ANI] methodName %{public}s not registered!", type.c_str());
        return DMError::DM_OK;
    }
    DMError ret = DMError::DM_OK;
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end(); it++) {
        ani_boolean isEquals = 0;
        env->Reference_StrictEquals(callback, it->first, &isEquals);
        if (isEquals) {
            it->second->RemoveCallback(env, type, callback);
            if (type == EVENT_CHANGE) {
                TLOGI(WmsLogTag::DMS, "[ANI] start to unregis screen event listener! event = %{public}s",
                    type.c_str());
                sptr<ScreenManager::IScreenListener> thisListener(it->second);
                ret = SingletonContainer::Get<ScreenManager>().UnregisterScreenListener(thisListener);
            }
        }
    }
    return ret;
}

DMError ScreenManagerAni::UnRegisterAllScreenListenerWithType(std::string type)
{
    TLOGI(WmsLogTag::DMS, "[ANI] UnregisterAllScreenListenerWithType begin");
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        TLOGI(WmsLogTag::DMS, "[ANI] UnregisterAllScreenListenerWithType methodName %{public}s not registered!",
            type.c_str());
        return DMError::DM_OK;
    }
    DMError ret = DMError::DM_OK;
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        it->second->RemoveAllCallback();
        if (type == EVENT_CHANGE) {
            sptr<ScreenManager::IScreenListener> thisListener(it->second);
            ret = SingletonContainer::Get<ScreenManager>().UnregisterScreenListener(thisListener);
        }
    }
    return ret;
}


ani_status ScreenManagerAni::initScreenManagerAni(ani_namespace screenNameSpace, ani_env* env)
{
    TLOGI(WmsLogTag::DEFAULT, "[ANI]");
    ani_function setObjFunc = nullptr;
    ani_status ret = env->Namespace_FindFunction(screenNameSpace, "setScreenMgrRef", "J:V", &setObjFunc);
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
    if ((ret = env->FindNamespace("L@ohos/screen/screen;", &nsp)) != ANI_OK) {
        TLOGE(WmsLogTag::DMS, "[ANI] null env %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    ScreenManagerAni::initScreenManagerAni(nsp, env);
    std::array funcs = {
        ani_native_function {"syncOn", nullptr,
            reinterpret_cast<void *>(ScreenManagerAni::registerCallback)},
        ani_native_function {"syncOff", nullptr,
            reinterpret_cast<void *>(ScreenManagerAni::unRegisterCallback)}
    };
    if ((ret = env->Namespace_BindNativeFunctions(nsp, funcs.data(), funcs.size()))) {
        TLOGE(WmsLogTag::DMS, "[ANI] bind namespace fail %{public}u", ret);
        return ANI_NOT_FOUND;
    }
    *result = ANI_VERSION_1;
    return ANI_OK;
}
}

}
}