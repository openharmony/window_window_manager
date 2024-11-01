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
#include <vector>
 
#include "dm_common.h"
 
#include "fold_screen_controller/super_fold_sensor_manager.h"
#include "fold_screen_controller/super_fold_state_manager.h"
#include "window_manager_hilog.h"
#include "screen_session_manager.h"
 
namespace OHOS {
 
namespace Rosen {
namespace {
constexpr float ANGLE_MIN_VAL = 30.0F;
constexpr float ANGLE_MAX_VAL = 180.0F;
constexpr float ANGLE_FLAT_THRESHOLD = 150.0F;
constexpr float ANGLE_HALF_FOLD_THRESHOLD = 135.0F;
constexpr int32_t HALL_HAVE_KEYBOARD_THRESHOLD = 0x0100;
constexpr int32_t HALL_REMOVE_KEYBOARD_THRESHOLD = 0;
constexpr int32_t HALL_ACTIVE = 1 << 2;
constexpr int32_t SENSOR_SUCCESS = 0;
constexpr int32_t POSTURE_INTERVAL = 100000000;
constexpr uint16_t SENSOR_EVENT_FIRST_DATA = 0;
constexpr float ACCURACY_ERROR_FOR_PC = 0.0001F;
} // namespace

SuperFoldSensorManager &SuperFoldSensorManager::GetInstance()
{
    static SuperFoldSensorManager SuperFoldSensorManager;
    return SuperFoldSensorManager;
}

static void SensorPostureDataCallback(SensorEvent *event)
{
    OHOS::Rosen::SuperFoldSensorManager::GetInstance().HandlePostureData(event);
}

static void SensorHallDataCallback(SensorEvent *event)
{
    OHOS::Rosen::SuperFoldSensorManager::GetInstance().HandleHallData(event);
}

void SuperFoldSensorManager::RegisterPostureCallback()
{
    postureUser.callback = SensorPostureDataCallback;
    int32_t subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_POSTURE, &postureUser);
    int32_t setBatchRet = SetBatch(SENSOR_TYPE_ID_POSTURE, &postureUser, POSTURE_INTERVAL, POSTURE_INTERVAL);
    int32_t activateRet = ActivateSensor(SENSOR_TYPE_ID_POSTURE, &postureUser);
    TLOGI(WmsLogTag::DMS,
        "subscribeRet: %{public}d, setBatchRet: %{public}d, activateRet: %{public}d",
        subscribeRet, setBatchRet, activateRet);
    if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
        TLOGI(WmsLogTag::DMS, "RegisterPostureCallback failed.");
    } else {
        TLOGI(WmsLogTag::DMS, "RegisterPostureCallback success.");
    }
}

void SuperFoldSensorManager::UnregisterPostureCallback()
{
    int32_t deactivateRet = DeactivateSensor(SENSOR_TYPE_ID_POSTURE, &postureUser);
    int32_t unsubscribeRet = UnsubscribeSensor(SENSOR_TYPE_ID_POSTURE, &postureUser);
    TLOGI(WmsLogTag::DMS, "deactivateRet: %{public}d, unsubscribeRet: %{public}d",
        deactivateRet, unsubscribeRet);
    if (deactivateRet == SENSOR_SUCCESS && unsubscribeRet == SENSOR_SUCCESS) {
        TLOGI(WmsLogTag::DMS, "FoldScreenSensorManager.UnRegisterPostureCallback success.");
    }
}

void SuperFoldSensorManager::RegisterHallCallback()
{
    hallUser.callback = SensorHallDataCallback;
    int32_t subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_HALL, &hallUser);
    TLOGI(WmsLogTag::DMS, "subscribeRet: %{public}d", subscribeRet);
    int32_t setBatchRet = SetBatch(SENSOR_TYPE_ID_HALL, &hallUser, POSTURE_INTERVAL, POSTURE_INTERVAL);
    TLOGI(WmsLogTag::DMS, "setBatchRet: %{public}d", setBatchRet);
    int32_t activateRet = ActivateSensor(SENSOR_TYPE_ID_HALL, &hallUser);
    TLOGI(WmsLogTag::DMS, "activateRet: %{public}d", activateRet);
    if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
        TLOGI(WmsLogTag::DMS, "RegisterHallCallback failed.");
    }
}

void SuperFoldSensorManager::UnregisterHallCallback()
{
    int32_t deactivateRet = DeactivateSensor(SENSOR_TYPE_ID_HALL_EXT, &hallUser);
    int32_t unsubscribeRet = UnsubscribeSensor(SENSOR_TYPE_ID_HALL_EXT, &hallUser);
    if (deactivateRet == SENSOR_SUCCESS && unsubscribeRet == SENSOR_SUCCESS) {
        TLOGI(WmsLogTag::DMS, "UnRegisterHallCallback success.");
    }
}

void SuperFoldSensorManager::RegisterSoftKeyboardCallback() {}

void SuperFoldSensorManager::HandlePostureData(const SensorEvent * const event)
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
    if (std::isless(globalAngle, ANGLE_MIN_VAL) ||
        std::isgreater(globalAngle, ANGLE_MAX_VAL + ACCURACY_ERROR_FOR_PC)) {
        TLOGI(WmsLogTag::DMS, "Invalid value, angle value is: %{public}f.", globalAngle);
        return;
    }
    TLOGI(WmsLogTag::DMS, "angle value is: %{public}f.", globalAngle);
    NotifyFoldAngleChanged(globalAngle);
}

void SuperFoldSensorManager::NotifyFoldAngleChanged(float foldAngle)
{
    if (std::isgreater(foldAngle, ANGLE_FLAT_THRESHOLD)) {
        TLOGI(WmsLogTag::DMS, "NotifyFoldAngleChanged is not Folded");
        events_ = SuperFoldStatusChangeEvents::ANGLE_CHANGE_EXPANDED;
    } else if (std::isless(foldAngle, ANGLE_HALF_FOLD_THRESHOLD) &&
        std::isgreater(foldAngle, ANGLE_MIN_VAL)) {
        TLOGI(WmsLogTag::DMS, "NotifyFoldAngleChanged is folded");
        events_ = SuperFoldStatusChangeEvents::ANGLE_CHANGE_HALF_FOLDED;
    } else {
        TLOGI(WmsLogTag::DMS, "NotifyFoldAngleChanged");
        events_ = SuperFoldStatusChangeEvents::ANGLE_CHANGE_FOLDED;
        return;
    }
    // notify
    std::vector<float> foldAngles;
    foldAngles.push_back(foldAngle);
    ScreenSessionManager::GetInstance().NotifyFoldAngleChanged(foldAngles);
    HandleSuperSensorChange();
}

void SuperFoldSensorManager::HandleHallData(const SensorEvent * const event)
{
    if (event == nullptr) {
        TLOGI(WmsLogTag::DMS, "SensorEvent is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].data == nullptr) {
        TLOGI(WmsLogTag::DMS, "SensorEvent[0].data is nullptr.");
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].dataLen < sizeof(HallData)) {
        TLOGI(WmsLogTag::DMS, "SensorEvent[0].dataLen is nullptr.");
        return;
    }
    auto data = reinterpret_cast<HallData*>(event->data);
    auto status = static_cast<uint32_t>(data->status);
    TLOGI(WmsLogTag::DMS, "HallData status is: %{public}u.", status);
    
    if (globalHall == (status & HALL_ACTIVE)) {
        TLOGI(WmsLogTag::DMS, "Hall don't change, hall = %{public}u", globalHall);
        return;
    }
    globalHall = (status & HALL_ACTIVE);
    TLOGI(WmsLogTag::DMS, "Hall change, hall = %{public}u", globalHall);
    NotifyHallChanged(globalHall);
}

void SuperFoldSensorManager::NotifyHallChanged(uint16_t Hall)
{
    if (Hall == HALL_REMOVE_KEYBOARD_THRESHOLD) {
        TLOGI(WmsLogTag::DMS, "NotifyHallChanged is not hasPhysicalKeyboard");
        hasKeyboard_ = false;
        events_ = SuperFoldStatusChangeEvents::KEYBOARD_OFF;
    } else if (Hall == HALL_HAVE_KEYBOARD_THRESHOLD) {
        TLOGI(WmsLogTag::DMS, "NotifyHallChanged is hasPhysicalKeyboard");
        hasKeyboard_ = true;
        events_ = SuperFoldStatusChangeEvents::KEYBOARD_ON;
    } else {
        TLOGI(WmsLogTag::DMS, "NotifyHallChanged invalide hall value");
        return;
    }
    // notify
    HandleSuperSensorChange();
}

void SuperFoldSensorManager::HandleSoftKeyboardData() {}

void SuperFoldSensorManager::NotifySoftKeyboardChanged() {}

void SuperFoldSensorManager::HandleSuperSensorChange()
{
    // trigger events
    if (hasKeyboard_ && events_ != SuperFoldStatusChangeEvents::KEYBOARD_OFF) {
        TLOGI(WmsLogTag::DMS, "Don't Change!");
        return;
    }
    SuperFoldStateManager::GetInstance().HandleSuperFoldStatusChange(events_);
}

void SuperFoldSensorManager::SetHasKeyboard(bool flag)
{
    hasKeyboard_ = flag;
}

SuperFoldSensorManager::SuperFoldSensorManager() {}

SuperFoldSensorManager::~SuperFoldSensorManager() {}
} // Rosen
} // OHOS
#endif