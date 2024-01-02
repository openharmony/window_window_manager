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

static void SensorHallDataCallback(SensorEvent *event)
{
    OHOS::Rosen::FoldScreenSensorManager::GetInstance().HandleHallData(event);
}

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "FoldScreenSensorManager"};
    constexpr float ANGLE_MIN_VAL = 0.0F;
    constexpr float ANGLE_MAX_VAL = 180.0F;
    constexpr int32_t SENSOR_SUCCESS = 0;
    constexpr int32_t POSTURE_INTERVAL = 100000000;
    constexpr uint16_t SENSOR_EVENT_FIRST_DATA = 0;
    constexpr float HALF_FOLDED_MAX_THRESHOLD = 140.0F;
    constexpr float CLOSE_HALF_FOLDED_MIN_THRESHOLD = 90.0F;
    constexpr float OPEN_HALF_FOLDED_MIN_THRESHOLD = 25.0F;
    constexpr float HALF_FOLDED_BUFFER = 10.0F;

    float SWITCH_ANGLE = 0.0F;
    uint16_t SWITCH_HALL = 1;
    typedef struct EXTHALLData {
        float flag = 0.0;
        float hall = 0.0;
    } ExtHallData;
} // namespace
WM_IMPLEMENT_SINGLE_INSTANCE(FoldScreenSensorManager);

FoldScreenSensorManager::FoldScreenSensorManager()
{
    RegisterPostureCallback();
    RegisterHallCallback();
}

void FoldScreenSensorManager::SetFoldScreenPolicy(sptr<FoldScreenPolicy> foldScreenPolicy)
{
    foldScreenPolicy_ = foldScreenPolicy;
}

void FoldScreenSensorManager::RegisterPostureCallback()
{
    postureuser.callback = SensorPostureDataCallback;
    int32_t subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_POSTURE, &postureuser);
    WLOGFI("RegisterPostureCallback, subscribeRet: %{public}d", subscribeRet);
    int32_t setBatchRet = SetBatch(SENSOR_TYPE_ID_POSTURE, &postureuser, POSTURE_INTERVAL, POSTURE_INTERVAL);
    WLOGFI("RegisterPostureCallback, setBatchRet: %{public}d", setBatchRet);
    int32_t activateRet = ActivateSensor(SENSOR_TYPE_ID_POSTURE, &postureuser);
    WLOGFI("RegisterPostureCallback, activateRet: %{public}d", activateRet);
    if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
        WLOGFE("RegisterPostureCallback failed.");
    }
}

void FoldScreenSensorManager::UnRegisterPostureCallback()
{
    int32_t deactivateRet = DeactivateSensor(SENSOR_TYPE_ID_POSTURE, &postureuser);
    int32_t unsubscribeRet = UnsubscribeSensor(SENSOR_TYPE_ID_POSTURE, &postureuser);
    if (deactivateRet == SENSOR_SUCCESS && unsubscribeRet == SENSOR_SUCCESS) {
        WLOGFI("FoldScreenSensorManager.UnRegisterPostureCallback success.");
    }
}

void FoldScreenSensorManager::RegisterHallCallback()
{
    halluser.callback = SensorHallDataCallback;
    int32_t subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_HALL_EXT, &halluser);
    WLOGFI("RegisterHallCallback, subscribeRet: %{public}d", subscribeRet);
    int32_t setBatchRet = SetBatch(SENSOR_TYPE_ID_HALL_EXT, &halluser, POSTURE_INTERVAL, POSTURE_INTERVAL);
    WLOGFI("RegisterHallCallback, setBatchRet: %{public}d", setBatchRet);
    int32_t activateRet = ActivateSensor(SENSOR_TYPE_ID_HALL_EXT, &halluser);
    WLOGFI("RegisterHallCallback, activateRet: %{public}d", activateRet);
    if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
        WLOGFE("RegisterHallCallback failed.");
    }
}

void FoldScreenSensorManager::UnRegisterHallCallback()
{
    int32_t deactivateRet1 = DeactivateSensor(SENSOR_TYPE_ID_HALL_EXT, &halluser);
    int32_t unsubscribeRet1 = UnsubscribeSensor(SENSOR_TYPE_ID_HALL_EXT, &halluser);
    if (deactivateRet1 == SENSOR_SUCCESS && unsubscribeRet1 == SENSOR_SUCCESS) {
        WLOGFI("FoldScreenSensorManager.UnRegisterHallCallback success.");
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
    SWITCH_ANGLE = (*postureData).angle;
    if (std::isless(SWITCH_ANGLE, ANGLE_MIN_VAL) || std::isgreater(SWITCH_ANGLE, ANGLE_MAX_VAL)) {
        WLOGFE("Invalid angle value, angle is %{public}f.", SWITCH_ANGLE);
        return;
    }
    WLOGFD("angle value in PostureData is: %{public}f.", SWITCH_ANGLE);
    HandleSensorData(SWITCH_ANGLE, SWITCH_HALL);
}

void FoldScreenSensorManager::HandleHallData(const SensorEvent * const event)
{
    if (event == nullptr) {
        WLOGFI("SensorEvent is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].data == nullptr) {
        WLOGFI("SensorEvent[0].data is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].dataLen < sizeof(ExtHallData)) {
        WLOGFI("SensorEvent dataLen less than hall data size.");
        return;
    }
    if (foldScreenPolicy_ != nullptr && foldScreenPolicy_->lockDisplayStatus_ == true) {
        WLOGFI("SensorEvent display status is locked.");
        return;
    }

    ExtHallData *extHallData = reinterpret_cast<ExtHallData *>(event[SENSOR_EVENT_FIRST_DATA].data);
    uint16_t flag = (uint16_t)(*extHallData).flag;
    if (!(flag & (1 << 1))) {
        WLOGFI("NOT Support Extend Hall.");
        return;
    }
    SWITCH_HALL = (uint16_t)(*extHallData).hall;
    WLOGFI("hall value in HallData is: %{public}u.", SWITCH_HALL);
    HandleSensorData(SWITCH_ANGLE, SWITCH_HALL);
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
    if (hall == SWITCH_HALL) {
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