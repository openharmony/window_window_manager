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

#ifndef OHOS_WINDOW_RATE_MANAGER_H
#define OHOS_WINDOW_RATE_MANAGER_H

#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {
class WindowRateManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowRateManager);
public:
    WindowRateManager() = default;
    ~WindowRateManager() = default;
    void FlushFrameRate(int32_t id, uint32_t rate);
    void FlushFrameRateForRootWindow(uint32_t rate);
    void AddWindowRate(int32_t windowId, uint32_t rate = 0);
    void RemoveWindowRate(int32_t windowId);
    uint32_t GetExpectedRate();

private:
    std::mutex mtx_;
    std::unordered_map<int32_t, uint32_t> windowRateMap_;
    uint32_t expectedRate_ = 0;
    uint32_t rootWindowRate_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_RATE_MANAGER_H