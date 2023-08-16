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
#include <cmath>

#include "fold_screen_controller/fold_screen_sensor_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "FoldScreenSensorManager"};
    constexpr float ANGLE_MIN_VAL = 0.0F;
} // namespace
WM_IMPLEMENT_SINGLE_INSTANCE(FoldScreenSensorManager);

FoldScreenSensorManager::FoldScreenSensorManager() : mState_(FoldState::FOLD_STATE_UNKNOWN)
{
}

void FoldScreenSensorManager::RegisterFoldScreenStateCallback(FoldScreenStateCallback& callback)
{
    if (mCallback_) {
        return;
    }
    WLOGFI("device register FoldScreenStateCallback success.");
    mCallback_ = callback;
}

void FoldScreenSensorManager::HandleSensorData(float angle, int hall)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    FoldState nextState = TransferAngleToScreenState(angle, hall);
    if (nextState == FoldState::FOLD_STATE_UNKNOWN) {
        if (mState_ == FoldState::FOLD_STATE_UNKNOWN) {
            mState_ = nextState;
        }
        return;
    }
    mState_ = nextState;
    if (mCallback_) {
        mCallback_(mState_);
    }
}

FoldState FoldScreenSensorManager::TransferAngleToScreenState(float angle, int hall) const
{
    if (std::isless(angle, ANGLE_MIN_VAL)) {
        return mState_;
    }
    FoldState state;
    if (std::isgreaterequal(angle, HALF_FOLDED_MAX_THRESHOLD)) {
        return FoldState::FOLD_STATE_EXPAND;
    }
    if (hall == HALL_THRESHOLD) {
        if (std::islessequal(angle, OPEN_HALF_FOLDED_MIN_THRESHOLD)) {
            state = FoldState::FOLD_STATE_FOLDED;
        } else if (std::islessequal(angle, HALF_FOLDED_MAX_THRESHOLD - HALF_FOLDED_BUFFER) &&
            std::isgreater(angle, OPEN_HALF_FOLDED_MIN_THRESHOLD + HALF_FOLDED_BUFFER)) {
            state = FoldState::FOLD_STATE_HALF_FOLDED;
        } else {
            state = mState_;
            if (state == FoldState::FOLD_STATE_UNKNOWN) {
                state = FoldState::FOLD_STATE_HALF_FOLDED;
            }
        }
        return state;
    }
    if (std::islessequal(angle, CLOSE_HALF_FOLDED_MIN_THRESHOLD)) {
        state = FoldState::FOLD_STATE_FOLDED;
    } else if (std::islessequal(angle, HALF_FOLDED_MAX_THRESHOLD - HALF_FOLDED_BUFFER) &&
        std::isgreater(angle, CLOSE_HALF_FOLDED_MIN_THRESHOLD + HALF_FOLDED_BUFFER)) {
        state = FoldState::FOLD_STATE_HALF_FOLDED;
    } else {
        state = mState_;
        if (state == FoldState::FOLD_STATE_UNKNOWN) {
            state = FoldState::FOLD_STATE_HALF_FOLDED;
        }
    }
    return state;
}
} // Rosen
} // OHOS