/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "window_rate_manager.h"

#include "vsync_station.h"
#include "window_frame_trace.h"
#include "window_manager_hilog.h"
#include "ui/rs_frame_rate_linker.h"

using namespace FRAME_TRACE;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowRateManager"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowRateManager)

void WindowRateManager::FlushFrameRate(int32_t windowId, uint32_t rate)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (auto iter = windowRateMap_.find(windowId); iter != windowRateMap_.end() && iter->second != rate) {
        iter->second = rate;
        expectedRate_ = GetExpectedRate();
        VsyncStation::GetInstance().FlushFrameRate(expectedRate_, false);
    }
}

void WindowRateManager::FlushFrameRateForRootWindow(uint32_t rate)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (rate != rootWindowRate_) {
        rootWindowRate_ = rate;
        expectedRate_ = GetExpectedRate();
        VsyncStation::GetInstance().FlushFrameRate(expectedRate_, false);
    }
}

void WindowRateManager::AddWindowRate(int32_t windowId, uint32_t rate)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (windowRateMap_.count(windowId)) {
        return;
    }
    windowRateMap_.emplace(windowId, rate);
    WLOGD("WindowRateManager::AddWindowRate id: %{public}d", windowId);
}

void WindowRateManager::RemoveWindowRate(int32_t windowId)
{
    std::lock_guard<std::mutex> lock(mtx_);
    if (auto iter = windowRateMap_.find(windowId); iter != windowRateMap_.end()) {
        auto rate = iter->second;
        windowRateMap_.erase(iter);
        WLOGD("WindowRateManager::RemoveWindowRate id: %{public}d", windowId);
        if (rate == expectedRate_) {
            expectedRate_ = GetExpectedRate();
            VsyncStation::GetInstance().FlushFrameRate(expectedRate_, true);
        }
    }
}

uint32_t WindowRateManager::GetExpectedRate()
{
    uint32_t expectedRate = 0;
    if (!windowRateMap_.empty()) {
        auto iter = std::max_element(
            windowRateMap_.begin(), windowRateMap_.end(), [](auto a, auto b) { return a.second < b.second; });
        expectedRate = iter->second;
    }
    expectedRate = std::max(expectedRate, rootWindowRate_);
    return expectedRate;
}
} // namespace Rosen
} // namespace OHOS
