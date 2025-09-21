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
#include "dms_global_mutex.h"

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
constexpr int32_t SENSOR_FAILURE = 1;
constexpr int32_t POSTURE_INTERVAL = 10000000;
constexpr uint16_t SENSOR_EVENT_FIRST_DATA = 0;
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

void FoldScreenSensorManager::RegisterHallCallback()
{
    int ret = OHOS::Rosen::FoldScreenSensorManager::GetInstance().SubscribeSensorCallback(
        SENSOR_TYPE_ID_HALL_EXT, POSTURE_INTERVAL, SensorHallDataCallback);
    if (ret == SENSOR_SUCCESS) {
        registerHall_ = true;
        TLOGI(WmsLogTag::DMS, "success.");
    } else {
        registerHall_ = false;
        TLOGE(WmsLogTag::DMS, "failed.");
    }
}

void FoldScreenSensorManager::UnRegisterPostureCallback()
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

void FoldScreenSensorManager::RegisterPostureCallback()
{
    int ret = OHOS::Rosen::FoldScreenSensorManager::GetInstance().SubscribeSensorCallback(
        SENSOR_TYPE_ID_POSTURE, POSTURE_INTERVAL, SensorPostureDataCallback);
    if (ret == SENSOR_SUCCESS) {
        registerPosture_ = true;
        TLOGI(WmsLogTag::DMS, "FoldScreenSensorManager.RegisterPostureCallback success.");
    } else {
        registerPosture_ = false;
        TLOGE(WmsLogTag::DMS, "RegisterPostureCallback failed.");
    }
}

void FoldScreenSensorManager::UnRegisterHallCallback()
{
    int32_t deactivateRet1 = DeactivateSensor(SENSOR_TYPE_ID_HALL_EXT, &hallUser);
    int32_t unsubscribeRet1 = UnsubscribeSensor(SENSOR_TYPE_ID_HALL_EXT, &hallUser);
    if (deactivateRet1 == SENSOR_SUCCESS && unsubscribeRet1 == SENSOR_SUCCESS) {
        TLOGI(WmsLogTag::DMS, "UnRegisterHallCallback success.");
        registerHall_ = false;
    }
}

void FoldScreenSensorManager::HandlePostureData(const SensorEvent* const event)
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
    NotifyFoldAngleChanged(globalAngle);
}

void FoldScreenSensorManager::NotifyFoldAngleChanged(float foldAngle)
{
    if (fabs(foldAngle - oldFoldAngle) < MINI_NOTIFY_FOLD_ANGLE) {
        return;
    }
    oldFoldAngle = foldAngle;
    std::vector<float> foldAngles;
    foldAngles.push_back(foldAngle);
    ScreenSessionManager::GetInstance().NotifyFoldAngleChanged(foldAngles);
}

static void GlobalSensorCallback(SensorEvent* event)
{
    DmUtils::HoldLock callback_lock;
    OHOS::Rosen::FoldScreenSensorManager::GetInstance().HandleSensorData(event);
}

int32_t FoldScreenSensorManager::SubscribeSensorCallback(int32_t sensorTypeId,
                                                         int64_t interval, const RecordSensorCallback taskCallback)
{
    sensorCallbacks_[sensorTypeId] = {taskCallback, interval };
    auto& user = users_[sensorTypeId];
    user.callback = GlobalSensorCallback;
    int32_t subscribeRet = SubscribeSensor(sensorTypeId, &user);
    int32_t setBatchRet = SetBatch(sensorTypeId, &user, interval, interval);
    int32_t activateRet = ActivateSensor(sensorTypeId, &user);
    TLOGI(WmsLogTag::DMS,
        "subscribeRet: %{public}d, setBatchRet: %{public}d, activateRet: %{public}d",
        subscribeRet, setBatchRet, activateRet);
    if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
        TLOGE(WmsLogTag::DMS, "failed.");
        return SENSOR_FAILURE;
    } else {
        TLOGI(WmsLogTag::DMS, "success.");
        return SENSOR_SUCCESS;
    }
}

void FoldScreenSensorManager::UnSubscribeSensorCallback()
{
    for (auto it = sensorCallbacks_.begin(); it != sensorCallbacks_.end();) {
        int32_t sensorTypeId = it->first;
        auto& callbackInfo = it->second;

        auto userIt = users_.find(sensorTypeId);
        if (userIt == users_.end()) {
            TLOGI(WmsLogTag::DMS, "User data not found for sensor type %{public}d", sensorTypeId);
            ++it;
            continue;
        }

        auto& user = userIt->second;

        int32_t deactivateRet = DeactivateSensor(sensorTypeId, &user);
        int32_t unsubscribeRet = UnsubscribeSensor(sensorTypeId, &user);

        TLOGI(WmsLogTag::DMS,
              "Unsubscribe sensor type: sensorTypeId=%{public}d, deactivateRet=%{public}d, unsubscribeRet=%{public}d",
              sensorTypeId,
              deactivateRet,
              unsubscribeRet);

        it = sensorCallbacks_.erase(it);
        users_.erase(userIt);

        if (deactivateRet != SENSOR_SUCCESS || unsubscribeRet != SENSOR_SUCCESS) {
            TLOGW(WmsLogTag::DMS, "Failed to unsubscribe sensor type %{public}d", sensorTypeId);
        }
    }
}

void FoldScreenSensorManager::HandleSensorData(const SensorEvent* const event)
{
    if (event == nullptr || event->data == nullptr) {
        TLOGE(WmsLogTag::DMS, "Received invalid sensor event or data is null.");
        return;
    }

    if (event->dataLen == 0) {
        TLOGW(WmsLogTag::DMS, "Sensor event data length is zero, skipping processing.");
        return;
    }

    auto it = sensorCallbacks_.find(event->sensorTypeId);
    if (it == sensorCallbacks_.end()) {
        TLOGI(WmsLogTag::DMS, "No callback registered for sensorTypeId: %{public}d", event->sensorTypeId);
        return;
    }
    const auto& entry = it->second;
    entry.taskCallback(const_cast<SensorEvent*>(event));
}

void FoldScreenSensorManager::HandleHallData(const SensorEvent* const event)
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
    uint16_t flag = static_cast<uint16_t>((*extHallData).flag);
    if (!(flag & (1 << 1))) {
        TLOGI(WmsLogTag::DMS, "NOT Support Extend Hall.");
        return;
    }
    if (globalHall == static_cast<uint16_t>((*extHallData).hall)) {
        TLOGI(WmsLogTag::DMS, "Hall don't change, hall = %{public}u", globalHall);
        return;
    }
    globalHall = static_cast<uint16_t>((*extHallData).hall);
    if (globalHall == USHRT_MAX || std::isless(globalAngle, ANGLE_MIN_VAL) ||
        std::isgreater(globalAngle, ANGLE_MAX_VAL + ACCURACY_ERROR_FOR_ALTA)) {
        TLOGE(WmsLogTag::DMS, "Invalid value, hall value is: %{public}u, angle value is: %{public}f.",
            globalHall, globalAngle);
        return;
    }
    TLOGI(WmsLogTag::DMS, "hall value is: %{public}u, angle value is: %{public}f", globalHall, globalAngle);
    if (!registerPosture_) {
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
    if (!registerPosture_) {
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

bool FoldScreenSensorManager::GetSensorRegisterStatus()
{
    return registerHall_ || registerPosture_;
}

float FoldScreenSensorManager::GetGlobalAngle() const
{
    return globalAngle;
}

void FoldScreenSensorManager::SetGlobalAngle(float angle)
{
    if (std::isless(angle, ANGLE_MIN_VAL) ||
        std::isgreater(angle, ANGLE_MAX_VAL + ACCURACY_ERROR_FOR_ALTA)) {
        TLOGI(WmsLogTag::DMS, "Invalid angle: %{public}f", angle);
        return;
    }
    globalAngle = angle;
}

uint16_t FoldScreenSensorManager::GetGlobalHall() const
{
    return globalHall;
}

void FoldScreenSensorManager::SetGlobalHall(uint16_t hall)
{
    if (hall == USHRT_MAX) {
        TLOGI(WmsLogTag::DMS, "Invalid hall: %{public}u", hall);
        return;
    }
    globalHall = hall;
}
} // Rosen
} // OHOS
#endif