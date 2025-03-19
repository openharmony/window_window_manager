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

#include "display_ani_listener.h"
#include "window_manager_hilog.h"
#include "display_ani_utils.h"

namespace OHOS {
namespace Rosen {

DisplayAniListener::~DisplayAniListener()
{
    TLOGI(WmsLogTag::DMS, "[ANI]~AniDisplayListener");
}

void DisplayAniListener::SetMainEventHandler()
{
    auto mainRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    if (mainRunner == nullptr) {
        return;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(mainRunner);
}

void DisplayAniListener::AddCallback(const std::string& type, ani_ref callback)
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

void DisplayAniListener::RemoveCallback(ani_env* env, const std::string& type, ani_ref callback)
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
    TLOGD(WmsLogTag::DMS, "[ANI] Success remove callback, jsCallBack_ size: %{public}u!",
        static_cast<uint32_t>(listeners.size()));
}

void DisplayAniListener::RemoveAllCallback()
{
    std::lock_guard<std::mutex> lock(mtx_);
    aniCallBack_.clear();
}

void DisplayAniListener::OnCreate(DisplayId id)
{
    TLOGI(WmsLogTag::DMS, "[ANI] OnCreate begin");
    std::lock_guard<std::mutex> lock(mtx_);
    auto thisListener = weakRef_.promote();
    if (thisListener == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] this listener is nullptr");
        return;
    }
    TLOGI(WmsLogTag::DMS, "[ANI] OnCreate is called, displayId: %{public}d", static_cast<uint32_t>(id));
    if (aniCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnCreate not register!");
        return;
    }
    auto it = aniCallBack_.find(EVENT_ADD);
    if (it == aniCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnCreate not this event, return");
        return;
    }
    std::vector<ani_ref> vec = it->second;
    TLOGI(WmsLogTag::DMS, "vec_callback size: %{public}lu", vec.size());
    // find callbacks in vector
    for (ani_ref oneAniCallback : vec) {
        ani_boolean result;
        if (ANI_OK != env_->Reference_IsUndefined(oneAniCallback ,&result)) {
            TLOGE(WmsLogTag::DMS, "[ANI] OnCreate Callback is undefined, return");
            return;
        }
        if (ANI_OK != env_->Reference_IsNull(oneAniCallback, &result)) {
            TLOGE(WmsLogTag::DMS, "[ANI] OnCreate Callback is null, return");
            return;
        }
        DisplayAniUtils::CallAniFunctionVoid(env_, "L@ohos/display/display;", "displayEventCallBack",
            "Lstd/core/Object;D:V", oneAniCallback, static_cast<ani_double>(id));
    }
}

void DisplayAniListener::OnDestroy(DisplayId id)
{
}
void DisplayAniListener::OnChange(DisplayId id)
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
    TLOGI(WmsLogTag::DMS, "vec_callback size: %{public}lu", vec.size());
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
        if (undefRes == 0) {
            TLOGE(WmsLogTag::DMS, "[ANI] oneAniCallback undefRes, return");
            return;
        }
        if (nullRes == 0) {
            TLOGE(WmsLogTag::DMS, "[ANI] oneAniCallback null, return");
            return;
        }
        DisplayAniUtils::CallAniFunctionVoid(env_, "L@ohos/display/display;", "displayEventCallBack",
            "Lstd/core/Object;D:V", oneAniCallback, static_cast<ani_double>(id));
    }
}
void DisplayAniListener::OnPrivateWindow(bool hasPrivate)
{
}
void DisplayAniListener::OnFoldStatusChanged(FoldStatus foldStatus)
{
    std::lock_guard<std::mutex> lock(mtx_);
    TLOGI(WmsLogTag::DMS, "[ANI] OnFoldStatusChanged is called, foldStatus: %{public}u",
        static_cast<uint32_t>(foldStatus));
    if (aniCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnFoldStatusChanged not register!");
        return;
    }
    if (aniCallBack_.find(EVENT_FOLD_STATUS_CHANGED) == aniCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnFoldStatusChanged not this event, return");
        return;
    }
    auto thisListener = weakRef_.promote();
    if (thisListener == nullptr || env_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI] this listener or env is nullptr");
        return;
    }
    if (env_ != nullptr) {
        auto it = aniCallBack_.find(EVENT_ADD);
        for (auto oneAniCallback : it->second) {
            // 修改实现方法（"invoke"在的这个字段）
            ani_status ret = thisListener->CallAniMethodVoid(static_cast<ani_object>(oneAniCallback),
                "L@ohos/display/display/Callback", "invoke", "I:V", static_cast<uint32_t>(foldStatus));
            if (ret != ANI_OK) {
                TLOGE(WmsLogTag::DMS, "[ANI] OnCreate: Failed to SendEvent.");
                break;
            }
        }
    } else {
        TLOGE(WmsLogTag::DMS, "[ANI] OnCreate: env is nullptr");
    }
}
void DisplayAniListener::OnFoldAngleChanged(std::vector<float> foldAngles)
{
}
void DisplayAniListener::OnCaptureStatusChanged(bool isCapture)
{
}
void DisplayAniListener::OnDisplayModeChanged(FoldDisplayMode displayMode)
{
    std::lock_guard<std::mutex> lock(mtx_);
    TLOGI(WmsLogTag::DMS, "[ANI] OnFoldStatusChanged is called, foldStatus: %{public}u",
        static_cast<uint32_t>(displayMode));
    if (aniCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnFoldStatusChanged not register!");
        return;
    }
    if (aniCallBack_.find(EVENT_DISPLAY_MODE_CHANGED) == aniCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnFoldStatusChanged not this event, return");
        return;
    }
    auto thisListener = weakRef_.promote();
    if (thisListener == nullptr || env_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI] this listener or env is nullptr");
        return;
    }
    if (env_ != nullptr) {
        auto it = aniCallBack_.find(EVENT_ADD);
        for (auto oneAniCallback : it->second) {
            // 修改实现方法（"invoke"在的这个字段）
            ani_status ret = thisListener->CallAniMethodVoid(static_cast<ani_object>(oneAniCallback),
                "L@ohos/display/display/Callback", "invoke", "I:V", static_cast<ani_int>(displayMode));
            if (ret != ANI_OK) {
                TLOGE(WmsLogTag::DMS, "[ANI] OnDisplayModeChanged: Failed to SendEvent.");
                break;
            }
        }
    } else {
        TLOGE(WmsLogTag::DMS, "[ANI] OnDisplayModeChanged: env is nullptr");
    }
}
void DisplayAniListener::OnAvailableAreaChanged(DMRect area)
{
}

ani_status DisplayAniListener::CallAniMethodVoid(ani_object object, const char* cls,
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