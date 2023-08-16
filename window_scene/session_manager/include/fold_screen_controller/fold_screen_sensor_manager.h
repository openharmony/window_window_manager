/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef OHOS_ROSEN_FOLD_SCREEN_SENSOR_MANAGER_H
#define OHOS_ROSEN_FOLD_SCREEN_SENSOR_MANAGER_H

#include <functional>
#include <mutex>

#include "refbase.h"
#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {

enum class FoldState : uint16_t {
    FOLD_STATE_UNKNOWN,
    FOLD_STATE_EXPAND,
    FOLD_STATE_FOLDED,
    FOLD_STATE_HALF_FOLDED
};

class FoldScreenSensorManager : public RefBase {
    WM_DECLARE_SINGLE_INSTANCE_BASE(FoldScreenSensorManager);
    using FoldScreenStateCallback = std::function<void(FoldState)>;

public:
    void RegisterFoldScreenStateCallback(FoldScreenStateCallback&);

private:
    FoldScreenStateCallback mCallback_;

    std::recursive_mutex mutex_;

    FoldState mState_;

    void HandleSensorData(float, int);

    FoldState TransferAngleToScreenState(float, int) const;

    FoldScreenSensorManager();

    ~FoldScreenSensorManager() = default;
};

namespace {
    constexpr uint16_t HALL_THRESHOLD = 1;
    constexpr float HALF_FOLDED_MAX_THRESHOLD = 140.0F;
    constexpr float CLOSE_HALF_FOLDED_MIN_THRESHOLD = 90.0F;
    constexpr float OPEN_HALF_FOLDED_MIN_THRESHOLD = 25.0F;
    constexpr float HALF_FOLDED_BUFFER = 10.0F;
}
} // namespace Rosen
} // namespace OHOS
#endif