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
#include <hisysevent.h>

#include "fold_screen_controller/fold_screen_sensor_manager.h"
#include "window_manager_hilog.h"
#include "screen_session_manager.h"

static void SensorPostureDataCallback(SensorEvent *event)
{
    OHOS::Rosen::FoldScreenSensorManager::GetInstance().HandlePostureData(event);
}

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "FoldScreenSensorManager"};
    constexpr float ANGLE_MIN_VAL = 0.0F;
    constexpr float ANGLE_MAX_VAL = 180.0F;
    constexpr int32_t SENSOR_SUCCESS = 0;
    constexpr uint16_t DEFAULT_HALL = 1;
    constexpr int32_t POSTURE_INTERVAL = 1000000;
    constexpr uint16_t SENSOR_EVENT_FIRST_DATA = 0;
    constexpr uint16_t HALL_THRESHOLD = 1;
    constexpr float HALF_FOLDED_MAX_THRESHOLD = 140.0F;
    constexpr float CLOSE_HALF_FOLDED_MIN_THRESHOLD = 90.0F;
    constexpr float OPEN_HALF_FOLDED_MIN_THRESHOLD = 25.0F;
    constexpr float HALF_FOLDED_BUFFER = 10.0F;
} // namespace
WM_IMPLEMENT_SINGLE_INSTANCE(FoldScreenSensorManager);

FoldScreenSensorManager::FoldScreenSensorManager()
{
    RegisterSensorCallback();
}

void FoldScreenSensorManager::SetFoldScreenPolicy(sptr<FoldScreenPolicy> foldScreenPolicy)
{
    foldScreenPolicy_ = foldScreenPolicy;
}

void FoldScreenSensorManager::RegisterSensorCallback()
{
    user.callback = SensorPostureDataCallback;
    int32_t subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_POSTURE, &user);
    WLOGFI("RegisterSensorCallback, subscribeRet: %{public}d", subscribeRet);
    int32_t setBatchRet = SetBatch(SENSOR_TYPE_ID_POSTURE, &user, POSTURE_INTERVAL, POSTURE_INTERVAL);
    WLOGFI("RegisterSensorCallback, setBatchRet: %{public}d", setBatchRet);
    int32_t activateRet = ActivateSensor(SENSOR_TYPE_ID_POSTURE, &user);
    WLOGFI("RegisterSensorCallback, activateRet: %{public}d", activateRet);
    if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
        WLOGFE("RegisterSensorCallback failed.");
    }
}

void FoldScreenSensorManager::UnRegisterSensorCallback()
{
    int32_t deactivateRet = DeactivateSensor(SENSOR_TYPE_ID_POSTURE, &user);
    int32_t unsubscribeRet = UnsubscribeSensor(SENSOR_TYPE_ID_POSTURE, &user);
    if (deactivateRet == SENSOR_SUCCESS && unsubscribeRet == SENSOR_SUCCESS) {
        WLOGFI("FoldScreenSensorManager.UnRegisterSensorCallback success.");
    }
}

void FoldScreenSensorManager::HandlePostureData(const SensorEvent * const event)
{
    if (event == nullptr) {
        WLOGFI("SensorEvent is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].data == nullptr) {
        WLOGFI("SensorEvent[0].data is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].dataLen < sizeof(PostureData)) {
        WLOGFI("SensorEvent dataLen less than posture data size.");
        return;
    }
    if (foldScreenPolicy_ != nullptr && foldScreenPolicy_->lockDisplayStatus_ == true) {
        WLOGFI("SensorEvent display status is locked.");
        return;
    }
    PostureData *postureData = reinterpret_cast<PostureData *>(event[SENSOR_EVENT_FIRST_DATA].data);
    float angle = (*postureData).angle;
    if (std::isless(angle, ANGLE_MIN_VAL) || std::isgreater(angle, ANGLE_MAX_VAL)) {
        WLOGFE("Invalid angle value, angle is %{public}f.", angle);
        return;
    }
    WLOGFD("angle vlaue in PostureData is: %{public}f.", angle);
    HandleSensorData(angle, DEFAULT_HALL);
}

void FoldScreenSensorManager::HandleSensorData(float angle, int hall)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    FoldStatus nextState = TransferAngleToScreenState(angle, hall);
    if (nextState == FoldStatus::UNKNOWN) {
        if (mState_ == FoldStatus::UNKNOWN) {
            mState_ = nextState;
        }
        return;
    }
    if (mState_ != nextState) {
        WLOGFI("current state: %{public}d, next state: %{public}d.", mState_, nextState);
        ReportNotifyFoldStatusChange((int32_t)mState_, (int32_t)nextState, angle);
        mState_ = nextState;
        ScreenSessionManager::GetInstance().NotifyFoldStatusChanged(mState_);
        if (foldScreenPolicy_ != nullptr) {
            foldScreenPolicy_->SendSensorResult(mState_);
        }
    }
}

FoldStatus FoldScreenSensorManager::TransferAngleToScreenState(float angle, int hall) const
{
    if (std::isless(angle, ANGLE_MIN_VAL)) {
        return mState_;
    }
    FoldStatus state;
    if (std::isgreaterequal(angle, HALF_FOLDED_MAX_THRESHOLD)) {
        return FoldStatus::EXPAND;
    }
    if (hall == HALL_THRESHOLD) {
        if (std::islessequal(angle, OPEN_HALF_FOLDED_MIN_THRESHOLD)) {
            state = FoldStatus::FOLDED;
        } else if (std::islessequal(angle, HALF_FOLDED_MAX_THRESHOLD - HALF_FOLDED_BUFFER) &&
            std::isgreater(angle, OPEN_HALF_FOLDED_MIN_THRESHOLD + HALF_FOLDED_BUFFER)) {
            state = FoldStatus::HALF_FOLD;
        } else {
            state = mState_;
            if (state == FoldStatus::UNKNOWN) {
                state = FoldStatus::HALF_FOLD;
            }
        }
        return state;
    }
    if (std::islessequal(angle, CLOSE_HALF_FOLDED_MIN_THRESHOLD)) {
        state = FoldStatus::FOLDED;
    } else if (std::islessequal(angle, HALF_FOLDED_MAX_THRESHOLD - HALF_FOLDED_BUFFER) &&
        std::isgreater(angle, CLOSE_HALF_FOLDED_MIN_THRESHOLD + HALF_FOLDED_BUFFER)) {
        state = FoldStatus::HALF_FOLD;
    } else {
        state = mState_;
        if (state == FoldStatus::UNKNOWN) {
            state = FoldStatus::HALF_FOLD;
        }
    }
    return state;
}

void FoldScreenSensorManager::ReportNotifyFoldStatusChange(int32_t currentStatus, int32_t nextStatus, float postureAngle)
{
    WLOGI("ReportNotifyFoldStatusChange currentStatus: %{public}d, nextStatus: %{public}d, postureAngle: %{public}f",
            currentStatus, nextStatus, postureAngle);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "NOTIFY_FOLD_STATE_CHANGE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "CURRENT_FOLD_STATUS", currentStatus,
        "NEXT_FOLD_STATUS", nextStatus,
        "SENSOR_POSTURE", postureAngle);

    if (ret != 0) {
        WLOGE("ReportNotifyFoldStatusChange Write HiSysEvent error, ret: %{public}d", ret);
    }
}
} // Rosen
} // OHOS