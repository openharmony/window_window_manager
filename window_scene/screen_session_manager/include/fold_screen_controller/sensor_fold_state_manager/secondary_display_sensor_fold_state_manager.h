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

#ifndef OHOS_ROSEN_SECONDARY_DISPLAY_FOLD_STATE_SENSOR_MANAGER_H
#define OHOS_ROSEN_SECONDARY_DISPLAY_FOLD_STATE_SENSOR_MANAGER_H

#include "dm_common.h"
#include <transaction/rs_interfaces.h>
#include "fold_screen_controller/fold_screen_policy.h"
#include "fold_screen_controller/sensor_fold_state_manager/sensor_fold_state_manager.h"

namespace OHOS {
namespace Rosen {
class SecondaryDisplaySensorFoldStateManager : public SensorFoldStateManager {
public:
    SecondaryDisplaySensorFoldStateManager();
    virtual ~SecondaryDisplaySensorFoldStateManager();
    void HandleAngleOrHallChange(const std::vector<float>& angles, const std::vector<uint16_t>& halls,
        sptr<FoldScreenPolicy> foldScreenPolicy, bool isPostureRegistered) override;
private:
    FoldStatus GetNextFoldState(const std::vector<float> &angles, const std::vector<uint16_t> &halls,
        bool isPostureRegistered, bool isFoldScreenOn);
    FoldStatus GetNextFoldStateHalf(float angle, uint16_t hall, FoldStatus myNextStatus,
        int32_t allowUserSensorForLargeFoldDevice);
    FoldStatus GetGlobalFoldState(FoldStatus PrimaryFoldState, FoldStatus SecondaryFoldState);
    FoldStatus UpdateSwitchScreenBoundaryForLargeFoldDeviceAB(float angle, uint16_t hall, FoldStatus state);
    FoldStatus UpdateSwitchScreenBoundaryForLargeFoldDeviceBC(float angle, uint16_t hall, FoldStatus state);
    FoldStatus GetFoldStateUnpower(const std::vector<uint16_t> &halls);
    void ReportSecondaryReflexion(int32_t currentStatus, int32_t nextStatus, bool isSecondaryReflexion);
    void SendReflexionResult(bool isSecondaryReflexion);
    FoldStatus HandleSecondaryOneStep(FoldStatus currentStatus, FoldStatus nextStatus,
        const std::vector<float>& angles, const std::vector<uint16_t>& halls) override;
    FoldStatus CalculateNewABFoldStatus(float previousAngle, uint16_t previousHall, float angle, uint16_t hall);

    int32_t allowUserSensorForLargeFoldDeviceAB = 0;
    int32_t allowUserSensorForLargeFoldDeviceBC = 0;
    bool isHasReflexioned = false;
    FoldStatus mNextStateAB = FoldStatus::UNKNOWN;
    FoldStatus mNextStateBC = FoldStatus::UNKNOWN;
    // used to record the current hall value between the A and B screens.
    uint16_t curHallAB_ = 2;
    // used to record the current hall value between the B and C screens.
    uint16_t curHallBC_ = 2;
    std::mutex secondaryFoldStatusMutex_;
};
} // namespace Rosen
} // namespace OHOS
#endif