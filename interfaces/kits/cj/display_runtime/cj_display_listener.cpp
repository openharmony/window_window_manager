/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cj_display_listener.h"

#include <functional>
#include <hitrace_meter.h>
#include <mutex>

#include "cj_lambda.h"
#include "display_ffi.h"
#include "display_utils.h"
#include "dm_common.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

CJDisplayListener::CJDisplayListener()
{
    TLOGD(WmsLogTag::DMS, "Constructor execution");
}

CJDisplayListener::~CJDisplayListener()
{
    TLOGD(WmsLogTag::DMS, "Destructor execution");
}

void CJDisplayListener::AddCallback(const std::string& type, int64_t callbackObject)
{
    std::lock_guard<std::mutex> lock(mtx_);
    TLOGI(WmsLogTag::DMS, "CJDisplayListener::AddCallback is called with type %{public}s", type.c_str());
    auto callback = CJLambda::Create(reinterpret_cast<void (*)(void*)>(callbackObject));
    cjCallBack_[type][callbackObject] = callback;
}

void CJDisplayListener::RemoveAllCallback()
{
    std::lock_guard<std::mutex> lock(mtx_);
    TLOGD(WmsLogTag::DMS, "CJDisplayListener::RemoveAllCallback is called");
    cjCallBack_.clear();
}

void CJDisplayListener::RemoveCallback(const std::string& type, int64_t callbackObject)
{
    std::lock_guard<std::mutex> lock(mtx_);
    TLOGI(WmsLogTag::DMS, "CJDisplayListener::RemoveCallback is called with type %{public}s", type.c_str());
    auto it = cjCallBack_.find(type);
    if (it == cjCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "CJDisplayListener::RemoveCallback no callback to remove");
        return;
    }
    auto& callbacks = it->second;
    for (auto iter = callbacks.begin(); iter != callbacks.end();) {
        if (iter->first == callbackObject) {
            iter = callbacks.erase(iter);
        } else {
            iter++;
        }
    }
    TLOGI(WmsLogTag::DMS, "CJDisplayListener::RemoveCallback succ cjCallBack_ size %{public}u!",
        static_cast<uint32_t>(callbacks.size()));
}

void CJDisplayListener::OnCreate(DisplayId id) {}

void CJDisplayListener::OnDestroy(DisplayId id) {}

void CJDisplayListener::OnChange(DisplayId id)
{
    std::lock_guard<std::mutex> lock(mtx_);
    TLOGI(WmsLogTag::DMS, "CJDisplayListener::OnChange is called, displayId: %{public}d", static_cast<uint32_t>(id));
    if (cjCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "CJDispalyListener::OnChange not register!");
        return;
    }
    if (cjCallBack_.find(EVENT_CHANGE) == cjCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "CJDisplayListener::OnChange not this event, return");
        return;
    }
    CallCJMethod(EVENT_CHANGE, &id);
}

void CJDisplayListener::OnPrivateWindow(bool hasPrivate)
{
    std::lock_guard<std::mutex> lock(mtx_);
    TLOGI(WmsLogTag::DMS, "CJDisplayListener::OnPrivateWindow is called");
    if (cjCallBack_.empty()) {
        TLOGI(WmsLogTag::DMS, "CJDispalyListener::OnPrivateWindow not register!");
        return;
    }
    if (cjCallBack_.find(EVENT_PRIVATE_MODE_CHANGE) == cjCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "CJDisplayListener::OnPrivateWindow not this event, return");
        return;
    }
    CallCJMethod(EVENT_PRIVATE_MODE_CHANGE, &hasPrivate);
}

void CJDisplayListener::OnFoldStatusChanged(FoldStatus foldStatus)
{
    std::lock_guard<std::mutex> lock(mtx_);
    TLOGI(WmsLogTag::DMS, "CJDisplayListener::OnFoldStatusChanged is called");
    if (cjCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "CJDispalyListener::OnFoldStatusChanged not register!");
        return;
    }
    if (cjCallBack_.find(EVENT_FOLD_STATUS_CHANGED) == cjCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "CJDisplayListener::OnFoldStatusChanged not this event, return");
        return;
    }
    CallCJMethod(EVENT_FOLD_STATUS_CHANGED, &foldStatus);
}

void CJDisplayListener::OnFoldAngleChanged(std::vector<float> foldAngles)
{
    std::lock_guard<std::mutex> lock(mtx_);
    TLOGI(WmsLogTag::DMS, "CJDisplayListener::OnFoldAngleChanged is called");
    if (cjCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "CJDispalyListener::OnFoldAngleChanged not register!");
        return;
    }
    if (cjCallBack_.find(EVENT_FOLD_ANGLE_CHANGED) == cjCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "CJDisplayListener::OnFoldAngleChanged not this event, return");
        return;
    }
    CallCJMethod(EVENT_FOLD_ANGLE_CHANGED, &foldAngles);
}

void CJDisplayListener::OnCaptureStatusChanged(bool isCapture)
{
    std::lock_guard<std::mutex> lock(mtx_);
    TLOGI(WmsLogTag::DMS, "CJDispalyListener::OnCaptureStatusChanged is called!");
    if (cjCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "CJDispalyListener::OnCaptureStatusChanged not register!");
        return;
    }
    if (cjCallBack_.find(EVENT_CAPTURE_STATUS_CHANGED) == cjCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "CJDisplayListener::OnCaptureStatusChanged not this event, return");
        return;
    }
    CallCJMethod(EVENT_CAPTURE_STATUS_CHANGED, &isCapture);
}

void CJDisplayListener::OnDisplayModeChanged(FoldDisplayMode displayMode)
{
    std::lock_guard<std::mutex> lock(mtx_);
    TLOGI(WmsLogTag::DMS, "CJDispalyListener::OnDisplayModeChanged is called!");
    if (cjCallBack_.empty()) {
        TLOGE(WmsLogTag::DMS, "CJDispalyListener::OnDisplayModeChanged not register!");
        return;
    }
    if (cjCallBack_.find(EVENT_DISPLAY_MODE_CHANGED) == cjCallBack_.end()) {
        TLOGE(WmsLogTag::DMS, "CJDisplayListener::OnDisplayModeChanged not this event, return");
        return;
    }
    CallCJMethod(EVENT_DISPLAY_MODE_CHANGED, &displayMode);
}

void CJDisplayListener::OnAvailableAreaChanged(DMRect area) {}

void CJDisplayListener::CallCJMethod(const std::string& methodName, void* argv)
{
    if (methodName.empty()) {
        TLOGE(WmsLogTag::DMS, "empty method name str, call method failed");
        return;
    }
    TLOGD(WmsLogTag::DMS, "CallCJMethod methodName = %{public}s", methodName.c_str());
    for (auto callback : cjCallBack_[methodName]) {
        callback.second(argv);
    }
}
} // namespace Rosen
} // namespace OHOS