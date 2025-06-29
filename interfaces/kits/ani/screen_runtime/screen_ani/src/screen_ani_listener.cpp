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
#include "screen_ani_listener.h"
#include "window_manager_hilog.h"
#include "screen_ani_utils.h"
 
namespace OHOS {
namespace Rosen {
 
ScreenAniListener::~ScreenAniListener()
{
    TLOGI(WmsLogTag::DMS, "[ANI]~ScreenAniListener");
}
 
void ScreenAniListener::AddCallback(const std::string& type, ani_ref callback)
{
    TLOGI(WmsLogTag::DMS, "[ANI] AddCallback is called, type = %{public}s", type.c_str());
    if (env_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "env_ nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mtx_);
    ani_ref cbRef{};
    if (env_->GlobalReference_Create(callback, &cbRef) != ANI_OK) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI]create global ref fail");
        return;
    };
    aniCallBack_[type].emplace_back(cbRef);
    TLOGI(WmsLogTag::DMS, "[ANI] AddCallback success aniCallBack_ size: %{public}u!",
        static_cast<uint32_t>(aniCallBack_[type].size()));
}
void ScreenAniListener::RemoveAllCallback()
{
    std::lock_guard<std::mutex> lock(mtx_);
    aniCallBack_.clear();
}
void ScreenAniListener::RemoveCallback(ani_env* env, const std::string& type, ani_ref callback)
{
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = aniCallBack_.find(type);
    if (it == aniCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "[ANI] Listener no callback to remove");
        return;
    }
    auto& listeners = it->second;
    for (auto iter = listeners.begin(); iter != listeners.end();) {
        ani_boolean isEquals = 0;
        env->Reference_StrictEquals(callback, *iter, &isEquals);
        if (isEquals) {
            listeners.erase(iter);
        } else {
            iter++;
        }
    }
    TLOGI(WmsLogTag::DMS, "[ANI] Success remove callback, jsCallBack_ size: %{public}u!",
        static_cast<uint32_t>(listeners.size()));
}
void ScreenAniListener::OnConnect(ScreenId id)
{
    TLOGI(WmsLogTag::DMS, "[ANI] OnConnect begin");
    std::lock_guard<std::mutex> lock(mtx_);
    auto thisListener = weakRef_.promote();
    if (thisListener == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] this listener is nullptr");
        return;
    }
    TLOGI(WmsLogTag::DMS, "[ANI] OnConnect is called, displayId: %{public}d", static_cast<uint32_t>(id));
    if (aniCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnConnect not register!");
        return;
    }
    auto it = aniCallBack_.find(EVENT_CONNECT);
    if (it == aniCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnConnect not this event, return");
        return;
    }
    std::vector<ani_ref> vec = it->second;
    TLOGI(WmsLogTag::DMS, "vec_callback size: %{public}d", (int)vec.size());
    // find callbacks in vector
    for (auto oneAniCallback : vec) {
        if (env_ == nullptr) {
            TLOGI(WmsLogTag::DMS, "OnDestroy: null env_");
            return;
        }
        ani_boolean undefRes;
        ani_boolean nullRes;
        env_->Reference_IsUndefined(oneAniCallback, &undefRes);
        env_->Reference_IsNull(oneAniCallback, &nullRes);
        // judge is null or undefined
        if (undefRes == 1) {
            TLOGE(WmsLogTag::DMS, "[ANI] oneAniCallback undefRes, return");
            return;
        }
        if (nullRes == 1) {
            TLOGE(WmsLogTag::DMS, "[ANI] oneAniCallback null, return");
            return;
        }
        
        ScreenAniUtils::CallAniFunctionVoid(env_, "L@ohos/screen/screen;", "screenEventCallBack",
            "Lstd/core/Object;D:V", oneAniCallback, static_cast<ani_double>(id));
    }
}
void ScreenAniListener::OnDisconnect(ScreenId id)
{
    TLOGI(WmsLogTag::DMS, "[ANI] OnDisconnect begin");
    std::lock_guard<std::mutex> lock(mtx_);
    auto thisListener = weakRef_.promote();
    if (thisListener == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] this listener is nullptr");
        return;
    }
    TLOGI(WmsLogTag::DMS, "[ANI] OnDisconnect is called, displayId: %{public}d", static_cast<uint32_t>(id));
    if (aniCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnDisconnect not register!");
        return;
    }
    auto it = aniCallBack_.find(EVENT_CHANGE);
    if (it == aniCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnDisconnect not this event, return");
        return;
    }
    std::vector<ani_ref> vec = it->second;
    TLOGI(WmsLogTag::DMS, "vec_callback size: %{public}d", (int)vec.size());
    // find callbacks in vector
    for (auto oneAniCallback : vec) {
        if (env_ == nullptr) {
            TLOGI(WmsLogTag::DMS, "OnDestroy: null env_");
            return;
        }
        ani_boolean undefRes;
        ani_boolean nullRes;
        env_->Reference_IsUndefined(oneAniCallback, &undefRes);
        env_->Reference_IsNull(oneAniCallback, &nullRes);
        // judge is null or undefined
        if (undefRes == 1) {
            TLOGE(WmsLogTag::DMS, "[ANI] oneAniCallback undefRes, return");
            return;
        }
        if (nullRes == 1) {
            TLOGE(WmsLogTag::DMS, "[ANI] oneAniCallback null, return");
            return;
        }
        
        ScreenAniUtils::CallAniFunctionVoid(env_, "L@ohos/screen/screen;", "screenEventCallBack",
            "Lstd/core/Object;D:V", oneAniCallback, static_cast<ani_double>(id));
    }
}
 
// need to implement
void ScreenAniListener::OnChange(ScreenId id)
{
    TLOGI(WmsLogTag::DMS, "[ANI] OnChange begin");
    std::lock_guard<std::mutex> lock(mtx_);
    auto thisListener = weakRef_.promote();
    if (thisListener == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] this listener is nullptr");
        return;
    }
    TLOGI(WmsLogTag::DMS, "[ANI] OnChange is called, displayId: %{public}d", static_cast<uint32_t>(id));
    if (aniCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnChange not register!");
        return;
    }
    auto it = aniCallBack_.find(EVENT_CHANGE);
    if (it == aniCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnChange not this event, return");
        return;
    }
    std::vector<ani_ref> vec = it->second;
    TLOGI(WmsLogTag::DMS, "vec_callback size: %{public}d", (int)vec.size());
    // find callbacks in vector
    for (auto oneAniCallback : vec) {
        if (env_ == nullptr) {
            TLOGI(WmsLogTag::DMS, "OnDestroy: null env_");
            return;
        }
        ani_boolean undefRes;
        ani_boolean nullRes;
        env_->Reference_IsUndefined(oneAniCallback, &undefRes);
        env_->Reference_IsNull(oneAniCallback, &nullRes);
        // judge is null or undefined
        if (undefRes == 1) {
            TLOGE(WmsLogTag::DMS, "[ANI] oneAniCallback undefRes, return");
            return;
        }
        if (nullRes == 1) {
            TLOGE(WmsLogTag::DMS, "[ANI] oneAniCallback null, return");
            return;
        }
        
        ScreenAniUtils::CallAniFunctionVoid(env_, "L@ohos/screen/screen;", "screenEventCallBack",
            "Lstd/core/Object;D:V", oneAniCallback, static_cast<ani_double>(id));
    }
}
 
ani_status ScreenAniListener::CallAniMethodVoid(ani_object object, const char* cls,
    const char* method, const char* signature, ...)
{
    ani_class aniClass;
    ani_status ret = env_->FindClass(cls, &aniClass);
    if (ret != ANI_OK) {
        return ret;
    }
    ani_method aniMethod;
    ret = env_->Class_FindMethod(aniClass, method, signature, &aniMethod);
    if (ret != ANI_OK) {
        return ret;
    }
    va_list args;
    va_start(args, signature);
    ret = env_->Object_CallMethod_Void(object, aniMethod, args);
    va_end(args);
    return ret;
}
}
}
