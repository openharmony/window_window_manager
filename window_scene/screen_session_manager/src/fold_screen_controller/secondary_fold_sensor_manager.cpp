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

#ifdef SENSOR_ENABLE
#include <cmath>
#include <hisysevent.h>
#include <parameters.h>

#include "fold_screen_controller/secondary_fold_sensor_manager.h"
#include "fold_screen_state_internel.h"
#include "window_manager_hilog.h"
#include "screen_session_manager.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS {
namespace Rosen {
namespace {
constexpr float ANGLE_MIN_VAL = 0.0F;
constexpr float ANGLE_MAX_VAL = 180.0F;
constexpr int32_t SENSOR_SUCCESS = 0;
constexpr int32_t POSTURE_INTERVAL = 100000000;
constexpr uint16_t SENSOR_EVENT_FIRST_DATA = 0;
constexpr uint16_t SENSOR_EVENT_SECOND_DATA = 1;
constexpr uint16_t HALL_B_C_COLUMN_ORDER = 1;
constexpr uint16_t HALL_A_B_COLUMN_ORDER = 4;
constexpr float ACCURACY_ERROR_FOR_ALTA = 0.0001F;
constexpr float MINI_NOTIFY_FOLD_ANGLE = 0.5F;
std::vector<float> oldFoldAngle = {0.0F, 0.0F};
constexpr size_t SECONDARY_FOLDING_AXIS_SIZE = 2;
} // namespace
WM_IMPLEMENT_SINGLE_INSTANCE(SecondaryFoldSensorManager);

void SecondaryFoldSensorManager::RegisterApplicationStateObserver()
{
    if (sensorFoldStateManager_ == nullptr) {
        return;
    }
    sensorFoldStateManager_->RegisterApplicationStateObserver();
}

void SecondaryFoldSensorManager::SetFoldScreenPolicy(sptr<FoldScreenPolicy> foldScreenPolicy)
{
    foldScreenPolicy_ = foldScreenPolicy;
}

void SecondaryFoldSensorManager::SetSensorFoldStateManager(sptr<SensorFoldStateManager> sensorFoldStateManager)
{
    sensorFoldStateManager_ = sensorFoldStateManager;
}

void SecondaryFoldSensorManager::SecondarySensorPostureDataCallback(SensorEvent *event)
{
    OHOS::Rosen::SecondaryFoldSensorManager::GetInstance().HandlePostureData(event);
}

void SecondaryFoldSensorManager::SecondarySensorHallDataCallbackExt(SensorEvent *event)
{
    OHOS::Rosen::SecondaryFoldSensorManager::GetInstance().HandleHallDataExt(event);
}

void SecondaryFoldSensorManager::RegisterPostureCallback()
{
    postureUser.callback = SecondarySensorPostureDataCallback;
    int32_t subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_POSTURE, &postureUser);
    int32_t setBatchRet = SetBatch(SENSOR_TYPE_ID_POSTURE, &postureUser, POSTURE_INTERVAL, POSTURE_INTERVAL);
    int32_t activateRet = ActivateSensor(SENSOR_TYPE_ID_POSTURE, &postureUser);
    TLOGI(WmsLogTag::DMS,
        "subscribeRet: %{public}d, setBatchRet: %{public}d, activateRet: %{public}d",
        subscribeRet, setBatchRet, activateRet);
    if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
        TLOGE(WmsLogTag::DMS, "failed.");
    } else {
        registerPosture_ = true;
        TLOGI(WmsLogTag::DMS, "success.");
    }
}

void SecondaryFoldSensorManager::UnRegisterPostureCallback()
{
    int32_t deactivateRet = DeactivateSensor(SENSOR_TYPE_ID_POSTURE, &postureUser);
    int32_t unsubscribeRet = UnsubscribeSensor(SENSOR_TYPE_ID_POSTURE, &postureUser);
    TLOGI(WmsLogTag::DMS, "deactivateRet: %{public}d, unsubscribeRet: %{public}d",
        deactivateRet, unsubscribeRet);
    if (deactivateRet == SENSOR_SUCCESS && unsubscribeRet == SENSOR_SUCCESS) {
        registerPosture_ = false;
        TLOGI(WmsLogTag::DMS, "success.");
    }
}

void SecondaryFoldSensorManager::RegisterHallCallback()
{
    hallUser.callback = SecondarySensorHallDataCallbackExt;
    int32_t subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_HALL_EXT, &hallUser);
    TLOGI(WmsLogTag::DMS, "subscribeRet: %{public}d", subscribeRet);
    int32_t setBatchRet = SetBatch(SENSOR_TYPE_ID_HALL_EXT, &hallUser, POSTURE_INTERVAL, POSTURE_INTERVAL);
    TLOGI(WmsLogTag::DMS, "setBatchRet: %{public}d", setBatchRet);
    int32_t activateRet = ActivateSensor(SENSOR_TYPE_ID_HALL_EXT, &hallUser);
    TLOGI(WmsLogTag::DMS, "activateRet: %{public}d", activateRet);
    if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
        TLOGE(WmsLogTag::DMS, "failed.");
    } else {
        TLOGI(WmsLogTag::DMS, "success.");
    }
}

void SecondaryFoldSensorManager::UnRegisterHallCallback()
{
    int32_t deactivateRet = DeactivateSensor(SENSOR_TYPE_ID_HALL_EXT, &hallUser);
    int32_t unsubscribeRet = UnsubscribeSensor(SENSOR_TYPE_ID_HALL_EXT, &hallUser);
    if (deactivateRet == SENSOR_SUCCESS && unsubscribeRet == SENSOR_SUCCESS) {
        TLOGW(WmsLogTag::DMS, "success.");
    }
}

void SecondaryFoldSensorManager::HandlePostureData(const SensorEvent * const event)
{
    float postureBc = 0.0F;
    float postureAb = 0.0F;
    float valueAbAnti = 0.0F;
    if (!GetPostureInner(event, postureBc, postureAb, valueAbAnti)) {
        return;
    }
    globalAngle[SENSOR_EVENT_FIRST_DATA] = postureBc;
    globalAngle[SENSOR_EVENT_SECOND_DATA] = postureAb;
    if (isDataBeyondBoundary()) {
        return;
    }
    TLOGD(WmsLogTag::DMS, "%{public}s, %{public}s",
        FoldScreenStateInternel::TransVec2Str(globalAngle, "angle").c_str(),
        FoldScreenStateInternel::TransVec2Str(globalHall, "hall").c_str());
    sensorFoldStateManager_->HandleAngleChange(globalAngle, globalHall, foldScreenPolicy_);
    NotifyFoldAngleChanged(globalAngle);
}

void SecondaryFoldSensorManager::NotifyFoldAngleChanged(const std::vector<float> &angles)
{
    size_t size = angles.size();
    std::vector<bool> flags(true, size);
    for (size_t i = 0; i < size; i++) {
        if (fabs(angles[i] - oldFoldAngle[i]) < MINI_NOTIFY_FOLD_ANGLE) {
            flags[i] = false;
        }
        oldFoldAngle[i] = angles[i];
    }
    bool flag = false;
    for (bool f : flags) {
        flag = flag | f;
    }
    if (!flag) {
        return;
    }
    ScreenSessionManager::GetInstance().NotifyFoldAngleChanged(angles);
}

void SecondaryFoldSensorManager::HandleHallDataExt(const SensorEvent * const event)
{
    uint16_t hallBc = 0;
    uint16_t hallAb = 0;
    if (!GetHallInner(event, hallBc, hallAb)) {
        return;
    }
    globalHall[SENSOR_EVENT_FIRST_DATA] = hallBc;
    globalHall[SENSOR_EVENT_SECOND_DATA] = hallAb;
    if (isDataBeyondBoundary()) {
        return;
    }
    TLOGI(WmsLogTag::DMS, "%{public}s, %{public}s",
        FoldScreenStateInternel::TransVec2Str(globalAngle, "angle").c_str(),
        FoldScreenStateInternel::TransVec2Str(globalHall, "hall").c_str());
    if (!registerPosture_) {
        globalAngle[SENSOR_EVENT_FIRST_DATA] = ANGLE_MIN_VAL;
        globalAngle[SENSOR_EVENT_SECOND_DATA] = ANGLE_MIN_VAL;
    }
    sensorFoldStateManager_->HandleHallChange(globalAngle, globalHall, foldScreenPolicy_);
    return;
}

bool SecondaryFoldSensorManager::IsDataBeyondBoundary()
{
    if (globalAngle.size() < SECONDARY_FOLDING_AXIS_SIZE || globalHall.size() < SECONDARY_FOLDING_AXIS_SIZE) {
        TLOGW(WmsLogTag::DMS, "global angles and halls size is less than %{public}lu.", SECONDARY_FOLDING_AXIS_SIZE);
        return true;
    }
    for (size_t i = 0; i < SECONDARY_FOLDING_AXIS_SIZE; i++) {
        uint16_t hall = globalHall[i];
        float angle = globalAngle[i];
        if (hall == USHRT_MAX ||
            std::isless(angle, ANGLE_MIN_VAL) ||
            std::isgreater(angle, ANGLE_MAX_VAL + ACCURACY_ERROR_FOR_ALTA)) {
            TLOGW(WmsLogTag::DMS, "i = %{public}lu, angle = %{public}f, hall = %{public}u", i, angle, hall);
            return true;
        }
    }
    return false;
}

bool SecondaryFoldSensorManager::GetPostureInner(const SensorEvent * const event, float &valueBc, float &valueAb,
    float &valueAbAnti)
{
    if (event == nullptr) {
        TLOGW(WmsLogTag::DMS, "SensorEvent is nullptr.");
        return false;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].data == nullptr) {
        TLOGW(WmsLogTag::DMS, "SensorEvent[0].data is nullptr.");
        return false;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].dataLen < sizeof(PostureData)) {
        TLOGW(WmsLogTag::DMS, "SensorEvent dataLen less than posture data size.");
        return false;
    }
    PostureData *postureData = reinterpret_cast<PostureData *>(event[SENSOR_EVENT_FIRST_DATA].data);
    valueBc = (*postureData).postureBc;
    valueAb = (*postureData).postureAb;
    valueAbAnti = (*postureData).postureAbAnti;
    TLOGD(WmsLogTag::DMS, "PostureData postureBc: %{public}f, postureAb: %{public}f, postureAbAnti: %{public}f.",
        valueBc, valueAb, postureAbAnti);
    return true;
}

bool SecondaryFoldSensorManager::GetHallInner(const SensorEvent * const event, uint16_t &valueBc, uint16_t &valueAb)
{
    if (event == nullptr) {
        TLOGW(WmsLogTag::DMS, "SensorEvent is nullptr.");
        return false;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].data == nullptr) {
        TLOGW(WmsLogTag::DMS, "SensorEvent[0].data is nullptr.");
        return false;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].dataLen < sizeof(ExtHallData)) {
        TLOGW(WmsLogTag::DMS, "SensorEvent dataLen less than hall data size.");
        return false;
    }
    ExtHallData *extHallData = reinterpret_cast<ExtHallData *>(event[SENSOR_EVENT_FIRST_DATA].data);
    uint16_t flag = (uint16_t)(*extHallData).flag;
    if (!(flag & (1 << HALL_B_C_COLUMN_ORDER)) || !(flag & (1 << HALL_A_B_COLUMN_ORDER))) {
        TLOGW(WmsLogTag::DMS, "not support Extend Hall.");
        return false;
    }
    valueBc = (uint16_t)(*extHallData).hallBc; // 0: hall closed, 1: hall expaned
    valueAb = (uint16_t)(*extHallData).hallAb;
    TLOGI(WmsLogTag::DMS, "HallData hallBc: %{public}u, hallAb: %{public}u.", valueBc, valueAb);
    return true;
}
} // Rosen
} // OHOS
#endif