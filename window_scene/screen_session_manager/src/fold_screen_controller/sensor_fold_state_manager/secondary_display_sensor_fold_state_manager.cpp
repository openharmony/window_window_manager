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

#include "fold_screen_controller/sensor_fold_state_manager/secondary_display_sensor_fold_state_manager.h"
#include <parameters.h>

#include "fold_screen_controller/fold_screen_policy.h"
#include "fold_screen_controller/sensor_fold_state_manager/sensor_fold_state_manager.h"
#include "session/screen/include/screen_session.h"
#include "fold_screen_state_internel.h"

#include "window_manager_hilog.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {
namespace {
constexpr uint8_t FOLDING_AXIS_SIZE = 2;
constexpr float ANGLE_MIN_VAL = 0.0F;
constexpr float ALTA_HALF_FOLDED_MAX_THRESHOLD = 140.0F;
constexpr float CLOSE_ALTA_HALF_FOLDED_MIN_THRESHOLD = 70.0F;
constexpr float OPEN_ALTA_HALF_FOLDED_MIN_THRESHOLD = 25.0F;
constexpr float ALTA_HALF_FOLDED_BUFFER = 10.0F;
constexpr float LARGER_BOUNDARY_FOR_ALTA_THRESHOLD = 90.0F;
constexpr int32_t LARGER_BOUNDARY_FLAG = 1;
constexpr int32_t SMALLER_BOUNDARY_FLAG = 0;
constexpr int32_t HALL_THRESHOLD = 1;
constexpr int32_t HALL_FOLDED_THRESHOLD = 0;

} // namespace

SecondaryDisplaySensorFoldStateManager::SecondaryDisplaySensorFoldStateManager() {}
SecondaryDisplaySensorFoldStateManager::~SecondaryDisplaySensorFoldStateManager() {}

void SecondaryDisplaySensorFoldStateManager::HandleAngleChange(std::vector<float> angles, std::vector<uint16_t> halls,
    sptr<FoldScreenPolicy> foldScreenPolicy)
{
    FoldStatus nextState = GetNextFoldState(angles, halls);
    HandleSensorChange(nextState, angles, foldScreenPolicy);
}

void SecondaryDisplaySensorFoldStateManager::HandleHallChange(std::vector<float> angles, std::vector<uint16_t> halls,
    sptr<FoldScreenPolicy> foldScreenPolicy)
{
    FoldStatus nextState = GetNextFoldState(angles, halls);
    HandleSensorChange(nextState, angles, foldScreenPolicy);
}

FoldStatus SecondaryDisplaySensorFoldStateManager::GetNextFoldState(std::vector<float> angles,
    std::vector<uint16_t> halls)
{
    TLOGD(WmsLogTag::DMS, "%{public}s, %{public}s",
        FoldScreenStateInternel::TransVec2Str(angles, "angles").c_str(),
        FoldScreenStateInternel::TransVec2Str(halls, "halls").c_str());

    FoldStatus state = FoldStatus::UNKNOWN;
    if (angles.size() != FOLDING_AXIS_SIZE || halls.size() != FOLDING_AXIS_SIZE) {
        TLOGE(WmsLogTag::DMS, "angles or halls size is not equals %{public}u.", FOLDING_AXIS_SIZE);
        return state;
    }

    float angleAB = angles[1];
    int hallAB = halls[1];
    float angleBC = angles[0];
    int hallBC = halls[0];

    FoldStatus nextStateSecondary = GetNextFoldStateHalf(angleAB, hallAB, mNextStateAB);
    mNextStateAB = nextStateSecondary;

    FoldStatus nextStatePrimary = GetNextFoldStateHalf(angleBC, hallBC, mNextStateBC);
    mNextStateBC = nextStatePrimary;

    state = GetGlobalFoldState(nextStatePrimary, nextStateSecondary);
    TLOGW(WmsLogTag::DMS, "State: %{public}d.", static_cast<int>(state));
    return state;
}


void SecondaryDisplaySensorFoldStateManager::UpdateSwitchScreenBoundaryForLargeFoldDevice(float angle, uint16_t hall)
{
    if (hall == HALL_FOLDED_THRESHOLD) {
        allowUserSensorForLargeFoldDevice = SMALLER_BOUNDARY_FLAG;
    } else if (angle >= LARGER_BOUNDARY_FOR_ALTA_THRESHOLD) {
        allowUserSensorForLargeFoldDevice = LARGER_BOUNDARY_FLAG;
    }
}


FoldStatus SecondaryDisplaySensorFoldStateManager::GetNextFoldStateHalf(float angle, int hall, FoldStatus CurrentState)
{
    UpdateSwitchScreenBoundaryForLargeFoldDevice(angle, hall);
    if (std::isless(angle, ANGLE_MIN_VAL)) {
        return CurrentState;
    }
    FoldStatus state = FoldStatus::UNKNOWN;

    if (allowUserSensorForLargeFoldDevice == SMALLER_BOUNDARY_FLAG) {
        if (std::islessequal(angle, OPEN_ALTA_HALF_FOLDED_MIN_THRESHOLD) && hall == HALL_FOLDED_THRESHOLD) {
            state = FoldStatus::FOLDED;
        } else if (std::isgreaterequal(angle, OPEN_ALTA_HALF_FOLDED_MIN_THRESHOLD + ALTA_HALF_FOLDED_BUFFER) &&
            hall == HALL_FOLDED_THRESHOLD) {
            state = FoldStatus::HALF_FOLD;
        } else if (std::islessequal(angle, ALTA_HALF_FOLDED_MAX_THRESHOLD - ALTA_HALF_FOLDED_BUFFER) &&
            hall == HALL_THRESHOLD) {
            state = FoldStatus::HALF_FOLD;
        } else if (std::isgreaterequal(angle, ALTA_HALF_FOLDED_MAX_THRESHOLD)) {
            state = FoldStatus::EXPAND;
        } else {
            state = CurrentState;
            if (state == FoldStatus::UNKNOWN) {
                state = FoldStatus::HALF_FOLD;
            }
        }
        return state;
    }

    if (hall == HALL_THRESHOLD && angle == OPEN_ALTA_HALF_FOLDED_MIN_THRESHOLD) {
        state = CurrentState;
    } else if (std::islessequal(angle, CLOSE_ALTA_HALF_FOLDED_MIN_THRESHOLD)) {
        state = FoldStatus::FOLDED;
    } else if (std::islessequal(angle, ALTA_HALF_FOLDED_MAX_THRESHOLD - ALTA_HALF_FOLDED_BUFFER) &&
        std::isgreater(angle, CLOSE_ALTA_HALF_FOLDED_MIN_THRESHOLD + ALTA_HALF_FOLDED_BUFFER)) {
        state = FoldStatus::HALF_FOLD;
    } else if (std::isgreaterequal(angle, ALTA_HALF_FOLDED_MAX_THRESHOLD)) {
        state = FoldStatus::EXPAND;
    } else {
        state = CurrentState;
        if (state == FoldStatus::UNKNOWN) {
            state = FoldStatus::HALF_FOLD;
        }
    }
    return state;
}

FoldStatus SecondaryDisplaySensorFoldStateManager::GetGlobalFoldState (FoldStatus mPrimaryFoldState,
    FoldStatus mSecondaryFoldState)
{
    FoldStatus defaultState = FoldStatus::FOLDED;
    if (mSecondaryFoldState == FoldStatus::UNKNOWN || mSecondaryFoldState == defaultState) {
        return mPrimaryFoldState;
    }
    int globalFoldState = (int)mPrimaryFoldState + (int)mSecondaryFoldState * 10;
    FoldStatus globalFoldStatus = static_cast<FoldStatus>(globalFoldState);
    return globalFoldStatus;
}

void SecondaryDisplaySensorFoldStateManager::RegisterApplicationStateObserver() {}
} // namespace OHOS::Rosen