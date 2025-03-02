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

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AniListener"};
}
DisplayAniListener::~DisplayAniListener()
{
    WLOGI("[ANI]~AniWindowListener");
}

void DisplayAniListener::AddCallback(const std::string& type, ani_ref callback)
{
    WLOGD("[ANI] AddCallback is called");
    if (env_ == nullptr) {
        WLOGFE("env_ nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mtx_);
    aniCallBack_[type].emplace_back(callback);
    WLOGD("[ANI] AddCallback success aniCallBack_ size: %{public}u!",
        static_cast<uint32_t>(aniCallBack_[type].size()));
}

void DisplayAniListener::RemoveCallback(ani_env* env, const std::string& type, ani_ref callback){
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = aniCallBack_.find(type);
    if (it == aniCallBack_.end()) {
        WLOGE("JsDisplayListener::RemoveCallback no callback to remove");
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
    WLOGD("JsDisplayListener::RemoveCallback success jsCallBack_ size: %{public}u!",
        static_cast<uint32_t>(listeners.size()));
}

void DisplayAniListener::OnCreate(DisplayId id){
    
}
void DisplayAniListener::OnDestroy(DisplayId id){

}
void DisplayAniListener::OnChange(DisplayId id){
    
}
void DisplayAniListener::OnPrivateWindow(bool hasPrivate){
    
}
void DisplayAniListener::OnFoldStatusChanged(FoldStatus foldStatus){
    
}
void DisplayAniListener::OnFoldAngleChanged(std::vector<float> foldAngles){
    
}
void DisplayAniListener::OnCaptureStatusChanged(bool isCapture){
    
}
void DisplayAniListener::OnDisplayModeChanged(FoldDisplayMode displayMode){
    
}
void DisplayAniListener::OnAvailableAreaChanged(DMRect area){
    
}

void DisplayAniListener::RemoveAllCallback(){

}

}
}