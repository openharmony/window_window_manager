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

#ifdef SENSOR_ENABLE
#include <cmath>
#include <hisysevent.h>
#include <parameters.h>

#include "fold_screen_controller/fold_screen_sensor_manager.h"
#include "fold_screen_state_internel.h"
#include "window_manager_hilog.h"
#include "screen_session_manager.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

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
    constexpr float ANGLE_MIN_VAL = 0.0F;
    constexpr float ANGLE_MAX_VAL = 180.0F;
    constexpr int32_t SENSOR_SUCCESS = 0;
    constexpr int32_t POSTURE_INTERVAL = 100000000;
    constexpr uint16_t SENSOR_EVENT_FIRST_DATA = 0;
    constexpr int32_t HALL_FOLDED_THRESHOLD = 0;
    constexpr float ACCURACY_ERROR_FOR_ALTA = 0.0001F;
    static float INWARD_HALF_FOLDED_MIN_THRESHOLD = static_cast<float>(system::GetIntParameter<int32_t>
        ("const.fold.half_folded_min_threshold", 85));
    static float LARGE_FOLD_HALF_FOLDED_MIN_THRESHOLD = static_cast<float>(system::GetIntParameter<int32_t>
        ("const.large_fold.half_folded_min_threshold", 25));
    constexpr float MINI_NOTIFY_FOLD_ANGLE = 0.5F;
    float oldFoldAngle = 0.0F;
} // namespace
WM_IMPLEMENT_SINGLE_INSTANCE(FoldScreenSensorManager);

FoldScreenSensorManager::FoldScreenSensorManager()
{
}

void FoldScreenSensorManager::SetFoldScreenPolicy(sptr<FoldScreenPolicy> foldScreenPolicy)
{
    foldScreenPolicy_ = foldScreenPolicy;
}

void FoldScreenSensorManager::SetSensorFoldStateManager(sptr<SensorFoldStateManager> sensorFoldStateManager)
{
    sensorFoldStateManager_ = sensorFoldStateManager;
}

void FoldScreenSensorManager::RegisterPostureCallback()
{
    postureUser.callback = SensorPostureDataCallback;
    int32_t subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_POSTURE, &postureUser);
    int32_t setBatchRet = SetBatch(SENSOR_TYPE_ID_POSTURE, &postureUser, POSTURE_INTERVAL, POSTURE_INTERVAL);
    int32_t activateRet = ActivateSensor(SENSOR_TYPE_ID_POSTURE, &postureUser);
    TLOGI(WmsLogTag::DMS,
        "RegisterPostureCallback, subscribeRet: %{public}d, setBatchRet: %{public}d, activateRet: %{public}d",
        subscribeRet, setBatchRet, activateRet);
    if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
        TLOGE(WmsLogTag::DMS, "RegisterPostureCallback failed.");
    } else {
        TLOGI(WmsLogTag::DMS, "FoldScreenSensorManager.RegisterPostureCallback success.");
    }
}

void FoldScreenSensorManager::UnRegisterPostureCallback()
{
    int32_t deactivateRet = DeactivateSensor(SENSOR_TYPE_ID_POSTURE, &postureUser);
    int32_t unsubscribeRet = UnsubscribeSensor(SENSOR_TYPE_ID_POSTURE, &postureUser);
    TLOGI(WmsLogTag::DMS, "UnRegisterPostureCallback, deactivateRet: %{public}d, unsubscribeRet: %{public}d",
        deactivateRet, unsubscribeRet);
    if (deactivateRet == SENSOR_SUCCESS && unsubscribeRet == SENSOR_SUCCESS) {
        TLOGI(WmsLogTag::DMS, "FoldScreenSensorManager.UnRegisterPostureCallback success.");
        sensorFoldStateManager_->ClearState(foldScreenPolicy_);
    }
}

void FoldScreenSensorManager::RegisterHallCallback()
{
    hallUser.callback = SensorHallDataCallback;
    int32_t subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_HALL_EXT, &hallUser);
    TLOGI(WmsLogTag::DMS, "RegisterHallCallback, subscribeRet: %{public}d", subscribeRet);
    int32_t setBatchRet = SetBatch(SENSOR_TYPE_ID_HALL_EXT, &hallUser, POSTURE_INTERVAL, POSTURE_INTERVAL);
    TLOGI(WmsLogTag::DMS, "RegisterHallCallback, setBatchRet: %{public}d", setBatchRet);
    int32_t activateRet = ActivateSensor(SENSOR_TYPE_ID_HALL_EXT, &hallUser);
    TLOGI(WmsLogTag::DMS, "RegisterHallCallback, activateRet: %{public}d", activateRet);
    if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
        TLOGE(WmsLogTag::DMS, "RegisterHallCallback failed.");
    }
}

void FoldScreenSensorManager::UnRegisterHallCallback()
{
    int32_t deactivateRet1 = DeactivateSensor(SENSOR_TYPE_ID_HALL_EXT, &hallUser);
    int32_t unsubscribeRet1 = UnsubscribeSensor(SENSOR_TYPE_ID_HALL_EXT, &hallUser);
    if (deactivateRet1 == SENSOR_SUCCESS && unsubscribeRet1 == SENSOR_SUCCESS) {
        TLOGI(WmsLogTag::DMS, "FoldScreenSensorManager.UnRegisterHallCallback success.");
    }
}

void FoldScreenSensorManager::HandlePostureData(const SensorEvent * const event)
{
    if (event == nullptr) {
        TLOGI(WmsLogTag::DMS, "SensorEvent is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].data == nullptr) {
        TLOGI(WmsLogTag::DMS, "SensorEvent[0].data is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].dataLen < sizeof(PostureData)) {
        TLOGI(WmsLogTag::DMS, "SensorEvent dataLen less than posture data size.");
        return;
    }
    PostureData *postureData = reinterpret_cast<PostureData *>(event[SENSOR_EVENT_FIRST_DATA].data);
    globalAngle = (*postureData).angle;
    if (globalHall == USHRT_MAX || std::isless(globalAngle, ANGLE_MIN_VAL) ||
        std::isgreater(globalAngle, ANGLE_MAX_VAL + ACCURACY_ERROR_FOR_ALTA)) {
        TLOGE(WmsLogTag::DMS, "Invalid value, hall value is: %{public}u, angle value is: %{public}f.",
            globalHall, globalAngle);
        return;
    }
    TLOGD(WmsLogTag::DMS, "angle value in PostureData is: %{public}f.", globalAngle);
    sensorFoldStateManager_->HandleAngleChange(globalAngle, globalHall, foldScreenPolicy_);
    notifyFoldAngleChanged(globalAngle);
}

void FoldScreenSensorManager::notifyFoldAngleChanged(float foldAngle)
{
    if (fabs(foldAngle - oldFoldAngle) < MINI_NOTIFY_FOLD_ANGLE) {
        return;
    }
    oldFoldAngle = foldAngle;
    std::vector<float> foldAngles;
    foldAngles.push_back(foldAngle);
    ScreenSessionManager::GetInstance().NotifyFoldAngleChanged(foldAngles);
}

void FoldScreenSensorManager::HandleHallData(const SensorEvent * const event)
{
    if (event == nullptr) {
        TLOGI(WmsLogTag::DMS, "SensorEvent is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].data == nullptr) {
        TLOGI(WmsLogTag::DMS, "SensorEvent[0].data is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].dataLen < sizeof(ExtHallData)) {
        TLOGI(WmsLogTag::DMS, "SensorEvent dataLen less than hall data size.");
        return;
    }
    ExtHallData *extHallData = reinterpret_cast<ExtHallData *>(event[SENSOR_EVENT_FIRST_DATA].data);
    uint16_t flag = (uint16_t)(*extHallData).flag;
    if (!(flag & (1 << 1))) {
        TLOGI(WmsLogTag::DMS, "NOT Support Extend Hall.");
        return;
    }
    if (globalHall == (uint16_t)(*extHallData).hall) {
        TLOGI(WmsLogTag::DMS, "Hall don't change, hall = %{public}u", globalHall);
        return;
    }
    globalHall = (uint16_t)(*extHallData).hall;
    if (globalHall == USHRT_MAX || std::isless(globalAngle, ANGLE_MIN_VAL) ||
        std::isgreater(globalAngle, ANGLE_MAX_VAL + ACCURACY_ERROR_FOR_ALTA)) {
        TLOGE(WmsLogTag::DMS, "Invalid value, hall value is: %{public}u, angle value is: %{public}f.",
            globalHall, globalAngle);
        return;
    }
    TLOGI(WmsLogTag::DMS, "hall value is: %{public}u, angle value is: %{public}f", globalHall, globalAngle);
    if (globalHall == HALL_FOLDED_THRESHOLD) {
        globalAngle = ANGLE_MIN_VAL;
    }
    sensorFoldStateManager_->HandleHallChange(globalAngle, globalHall, foldScreenPolicy_);
}

void FoldScreenSensorManager::RegisterApplicationStateObserver()
{
    sensorFoldStateManager_->RegisterApplicationStateObserver();
}

void FoldScreenSensorManager::TriggerDisplaySwitch()
{
    TLOGI(WmsLogTag::DMS, "TriggerDisplaySwitch hall value is: %{public}u, angle value is: %{public}f",
        globalHall, globalAngle);
    if (globalHall == HALL_FOLDED_THRESHOLD) {
        globalAngle = ANGLE_MIN_VAL;
    } else {
        if (FoldScreenStateInternel::IsDualDisplayFoldDevice()) {
            globalAngle = INWARD_HALF_FOLDED_MIN_THRESHOLD;
        } else if (FoldScreenStateInternel::IsSingleDisplayFoldDevice()) {
            globalAngle = LARGE_FOLD_HALF_FOLDED_MIN_THRESHOLD;
        }
    }
    sensorFoldStateManager_->HandleAngleChange(globalAngle, globalHall, foldScreenPolicy_);
}
} // Rosen
} // OHOS
#endif