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
    std::lock_guard<std::mutex> lock(aniCallBackMtx_);
    aniCallBack_[type].emplace_back(callback);
    TLOGI(WmsLogTag::DMS, "[ANI] AddCallback success aniCallBack_ size: %{public}u!",
        static_cast<uint32_t>(aniCallBack_[type].size()));
}
bool DisplayAniListener::IsAniCallBackExist(ani_env* env, const std::string& type, ani_ref callback)
{
    std::lock_guard<std::mutex> lock(aniCallBackMtx_);
    auto it = aniCallBack_.find(type);
    if (it == aniCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "[ANI] Listener is not found");
        return false;
    }
    auto& listeners = it->second;
    for (auto iter = listeners.begin(); iter != listeners.end(); iter++) {
        ani_boolean isEquals = 0;
        env->Reference_StrictEquals(callback, *iter, &isEquals);
        if (isEquals) {
            return true;
        }
    }
    return false;
}

void DisplayAniListener::RemoveCallback(ani_env* env, const std::string& type, ani_ref callback)
{
    std::lock_guard<std::mutex> lock(aniCallBackMtx_);
    auto it = aniCallBack_.find(type);
    if (it == aniCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "[ANI] Listener is not found");
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
    std::lock_guard<std::mutex> lock(aniCallBackMtx_);
    aniCallBack_.clear();
}

void DisplayAniListener::OnCreate(DisplayId id)
{
    TLOGI(WmsLogTag::DMS, "[ANI] OnCreate begin");
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
    std::lock_guard<std::mutex> lock(aniCallBackMtx_);
    auto it = aniCallBack_.find(EVENT_ADD);
    if (it == aniCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnCreate not this event, return");
        return;
    }
    std::vector<ani_ref> vec = it->second;
    for (ani_ref oneAniCallback : vec) {
        ani_boolean result;
        if (ANI_OK != env_->Reference_IsUndefined(oneAniCallback, &result)) {
            TLOGE(WmsLogTag::DMS, "[ANI] OnCreate Callback is undefined, return");
            return;
        }
        auto task = [env = env_, oneAniCallback, id] () {
            DisplayAniUtils::CallAniFunctionVoid(env, "L@ohos/display/display;", "displayEventCallBack",
                nullptr, oneAniCallback, static_cast<ani_double>(id));
        };
        if (!eventHandler_) {
            TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
            return;
        }
        eventHandler_->PostTask(task, "dms:AniDisplayListener::CreateCallBack", 0,
            AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void DisplayAniListener::OnDestroy(DisplayId id)
{
}
void DisplayAniListener::OnChange(DisplayId id)
{
    TLOGI(WmsLogTag::DMS, "[ANI] OnChange begin");
    auto thisListener = weakRef_.promote();
    if (thisListener == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "[ANI] this listener is nullptr");
        return;
    }
    TLOGI(WmsLogTag::DMS, "[ANI] OnChange is called, displayId: %{public}d", static_cast<uint32_t>(id));
    std::lock_guard<std::mutex> lock(aniCallBackMtx_);
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
    for (auto oneAniCallback : vec) {
        if (env_ == nullptr) {
            TLOGI(WmsLogTag::DMS, "OnDestroy: null env_");
            return;
        }
        ani_boolean undefRes;
        env_->Reference_IsUndefined(oneAniCallback, &undefRes);
        if (undefRes) {
            TLOGE(WmsLogTag::DMS, "[ANI] oneAniCallback undefRes, return");
            continue;
        }
        auto task = [env = env_, oneAniCallback, id] () {
            DisplayAniUtils::CallAniFunctionVoid(env, "L@ohos/display/display;", "displayEventCallBack",
                nullptr, oneAniCallback, static_cast<ani_double>(id));
        };
        if (!eventHandler_) {
            TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
            return;
        }
        eventHandler_->PostTask(task, "dms:AniWindowListener::SizeChangeCallBack", 0,
            AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}
void DisplayAniListener::OnPrivateWindow(bool hasPrivate)
{
}
void DisplayAniListener::OnFoldStatusChanged(FoldStatus foldStatus)
{
    TLOGI(WmsLogTag::DMS, "[ANI] OnFoldStatusChanged is called, foldStatus: %{public}u",
        static_cast<uint32_t>(foldStatus));
    auto thisListener = weakRef_.promote();
    if (thisListener == nullptr || env_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI] this listener or env is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(aniCallBackMtx_);
    if (aniCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnFoldStatusChanged not register!");
        return;
    }
    if (aniCallBack_.find(EVENT_FOLD_STATUS_CHANGED) == aniCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnFoldStatusChanged not this event, return");
        return;
    }
    if (env_ != nullptr) {
        auto it = aniCallBack_.find(EVENT_FOLD_STATUS_CHANGED);
        for (auto oneAniCallback : it->second) {
            auto task = [env = env_, oneAniCallback, foldStatus] () {
                DisplayAniUtils::CallAniFunctionVoid(env, "L@ohos/display/display;", "foldStatusCallback",
                    nullptr, oneAniCallback, static_cast<ani_int>(foldStatus));
            };
            if (!eventHandler_) {
                TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
                return;
            }
            eventHandler_->PostTask(task, "dms:AniDisplayListener::FoldStatusChangedCallback", 0,
                AppExecFwk::EventQueue::Priority::IMMEDIATE);
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
void DisplayAniListener::OnDisplayModeChanged(FoldDisplayMode foldDisplayMode)
{
    TLOGI(WmsLogTag::DMS, "[ANI] OnDisplayModeChanged is called, foldDisplayMode: %{public}u",
        static_cast<uint32_t>(foldDisplayMode));
    auto thisListener = weakRef_.promote();
    if (thisListener == nullptr || env_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "[ANI] this listener or env is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(aniCallBackMtx_);
    if (aniCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnDisplayModeChanged not register!");
        return;
    }
    if (aniCallBack_.find(EVENT_DISPLAY_MODE_CHANGED) == aniCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "[ANI] OnDisplayModeChanged not this event, return");
        return;
    }
    if (env_ != nullptr) {
        auto it = aniCallBack_.find(EVENT_DISPLAY_MODE_CHANGED);
        for (auto oneAniCallback : it->second) {
            auto task = [env = env_, oneAniCallback, foldDisplayMode] () {
                DisplayAniUtils::CallAniFunctionVoid(env, "L@ohos/display/display;", "foldDisplayModeCallback",
                    nullptr, oneAniCallback, static_cast<ani_int>(foldDisplayMode));
            };
            if (!eventHandler_) {
                TLOGE(WmsLogTag::DEFAULT, "get main event handler failed!");
                return;
            }
            eventHandler_->PostTask(task, "dms:AniDisplayListener::DisplayModeChangedCallback", 0,
                AppExecFwk::EventQueue::Priority::IMMEDIATE);
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