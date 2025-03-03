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
#include "display_ani_register_manager.h"
#include "display_ani_listener.h"
#include "singleton_container.h"
#include "display_manager.h"
#include "window_manager_hilog.h"
#include "display_ani_util.h"

namespace OHOS {
namespace Rosen {

namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsDisplayManager"};
}

DisplayAniRegisterManager::DisplayAniRegisterManager(){

}

DisplayAniRegisterManager::~DisplayAniRegisterManager(){

}

ani_object DisplayAniRegisterManager::RegisterListener(std::string type, ani_env* env, ani_ref callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    ani_boolean callbackNotNull = 0;
    env->Reference_IsNull(callback, &callbackNotNull);
    DmErrorCode ret;
    if (callbackNotNull) {
        sptr<DisplayAniListener> displayAniListener = new(std::nothrow) DisplayAniListener(env); 
        if (type == EVENT_ADD) {
            // add callback to listener
            displayAniListener->AddCallback(type, callback);
            // add listener to map
            jsCbMap_[type][callback] = displayAniListener;
            ret = DM_JS_TO_ERROR_CODE_MAP.at(SingletonContainer::Get<DisplayManager>().RegisterDisplayListener(displayAniListener));
            DmErrorCode errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
            if (ret == DmErrorCode::DM_ERROR_NOT_SYSTEM_APP) {
                errCode = ret;
            }
            WLOGFW("failed to unregister display listener with type");
            std::string errMsg = "failed to register display listener with type";
            ani_object aniErr = AniErrUtils::CreateAniError(env, errCode, errMsg);
            env->ThrowError(static_cast<ani_error>(aniErr));
        }
    }
    return DisplayAniUtils::CreateAniUndefined(env);
}

ani_object DisplayAniRegisterManager::UnregisterListener(std::string type, ani_env* env, ani_ref callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    ani_boolean callbackNotNull = 0;
    env->Reference_IsNull(callback, &callbackNotNull);
    DmErrorCode ret;
    if (callbackNotNull) {
        ret = DM_JS_TO_ERROR_CODE_MAP.at(UnRegisterDisplayListenerWithType(type, env, callback));
    } else {
        ret = DM_JS_TO_ERROR_CODE_MAP.at(UnregisterAllDisplayListenerWithType(type));
    }
    if (ret != DmErrorCode::DM_OK) {
        DmErrorCode errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
        if (ret == DmErrorCode::DM_ERROR_NOT_SYSTEM_APP) {
            errCode = ret;
        }
        WLOGFW("failed to unregister display listener with type");
        std::string errMsg = "failed to unregister display listener with type";
        ani_object aniErr = AniErrUtils::CreateAniError(env, errCode, errMsg);
        env->ThrowError(aniErr);
    }
    return DisplayAniUtils::CreateAniUndefined(env);
}

DMError DisplayAniRegisterManager::UnRegisterDisplayListenerWithType(std::string type, ani_env* env, ani_ref callback)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("UnRegisterDisplayListenerWithType methodName %{public}s not registered!", type.c_str());
        return DMError::DM_OK;
    }
    DMError ret = DMError::DM_OK;
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end(); it++) {
        ani_boolean isEquals = 0;
        env->Reference_StrictEquals(callback, it->first(), &isEquals);
        if (isEquals) {
            it->second->RemoveCallback(env, type, callback);
            if (type == EVENT_ADD || type == EVENT_REMOVE || type == EVENT_CHANGE) {
                sptr<DisplayManager::IDisplayListener> thisListener(it->second);
                ret = SingletonContainer::Get<DisplayManager>().UnregisterDisplayListener(thisListener);
            } else if (type == EVENT_FOLD_STATUS_CHANGED) {
                sptr<DisplayManager::IFoldStatusListener> thisListener(it->second);
                ret = SingletonContainer::Get<DisplayManager>().UnregisterFoldStatusListener(thisListener);
            } else if (type == EVENT_DISPLAY_MODE_CHANGED) {
                sptr<DisplayManager::IDisplayModeListener> thisListener(it->second);
                ret = SingletonContainer::Get<DisplayManager>().UnregisterDisplayModeListener(thisListener);
            }
        }
    }
    return ret;
}

DMError DisplayAniRegisterManager::UnregisterAllDisplayListenerWithType(std::string type)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGI("UnregisterAllDisplayListenerWithType methodName %{public}s not registered!",
            type.c_str());
        return DMError::DM_OK;
    }
    DMError ret = DMError::DM_OK;
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        it->second->RemoveAllCallback();
        if (type == EVENT_ADD || type == EVENT_REMOVE || type == EVENT_CHANGE) {
            sptr<DisplayManager::IDisplayListener> thisListener(it->second);
            ret = SingletonContainer::Get<DisplayManager>().UnregisterDisplayListener(thisListener);
        } else if (type == EVENT_FOLD_STATUS_CHANGED) {
            sptr<DisplayManager::IFoldStatusListener> thisListener(it->second);
            ret = SingletonContainer::Get<DisplayManager>().UnregisterFoldStatusListener(thisListener);
        } else if (type == EVENT_DISPLAY_MODE_CHANGED) {
            sptr<DisplayManager::IDisplayModeListener> thisListener(it->second);
            ret = SingletonContainer::Get<DisplayManager>().UnregisterDisplayModeListener(thisListener);
        }
    }
    return ret;
}
}
}