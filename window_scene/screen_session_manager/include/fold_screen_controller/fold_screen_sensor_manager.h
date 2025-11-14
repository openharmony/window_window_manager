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

#ifdef SENSOR_ENABLE
#include <functional>
#include <mutex>
#include <climits>

#include "fold_screen_controller.h"
#include "fold_screen_controller/sensor_fold_state_manager/sensor_fold_state_manager.h"
#include "refbase.h"
#include "wm_single_instance.h"
#include "sensor_agent.h"
#include "sensor_agent_type.h"

namespace OHOS {
namespace Rosen {

class FoldScreenSensorManager : public RefBase {
    WM_DECLARE_SINGLE_INSTANCE_BASE(FoldScreenSensorManager);

public:
    void RegisterPostureCallback();

    void RegisterApplicationStateObserver();

    void RegisterHallCallback();

    void UnRegisterPostureCallback();

    void UnRegisterHallCallback();

    void SetFoldScreenPolicy(sptr<FoldScreenPolicy> foldScreenPolicy);

    void SetSensorFoldStateManager(sptr<SensorFoldStateManager> sensorFoldStateManager);

    void HandlePostureData(const SensorEvent* const event);

    void HandleHallData(const SensorEvent* const event);

    void HandleSensorData(const SensorEvent* const event);

    int32_t  UnSubscribeSensorCallback(int32_t sensorTypeId);

    void CleanupCallback(int32_t sensorTypeId);

    typedef bool (*IsFwdSensorEvent)(const SensorEvent* event);

    int32_t SubscribeSensorCallback(int32_t sensorTypeId, int64_t interval, const RecordSensorCallback taskCallback);

    typedef struct SensorCallbackEntry {
        RecordSensorCallback taskCallback;
        int64_t interval;
    } SensorCallbackEntry;
    
    std::map<int32_t, SensorCallbackEntry> sensorCallbacks_;

    std::unordered_map<int32_t, SensorUser> users_;

    void TriggerDisplaySwitch();

    bool GetSensorRegisterStatus(void);

    typedef struct EXTHALLData {
        float flag = 0.0F;
        float hall = 0.0F; // BC axis angle
        float hallNfc = 0.0F;
        float hallProtectiveCase = 0.0F;
        float hallAb = 0.0F;
    } ExtHallData;

    typedef struct PostureDataSecondary {
        float postureAccxc = 0.0F;
        float postureAccyc = 0.0F;
        float postureAcczc = 0.0F;
        float postureAccxb = 0.0F;
        float postureAccyb = 0.0F;
        float postureAcczb = 0.0F;
        float postureBc = 0.0F;
        float postureScreen = 0.0F;
        float postureAccxx = 0.0F;
        float postureAccyx = 0.0F;
        float postureAcczx = 0.0F;
        float postureAb = 0.0F;
        float postureAbAnti = 0.0F;
    } PostureDataSecondary;

    float GetGlobalAngle() const;
    void SetGlobalAngle(float angle);
    uint16_t GetGlobalHall() const;
    void SetGlobalHall(uint16_t hall);

protected:
    FoldStatus GetCurrentState();

private:
    sptr<FoldScreenPolicy> foldScreenPolicy_;

    sptr<SensorFoldStateManager> sensorFoldStateManager_;

    std::recursive_mutex mutex_;

    FoldStatus mState_ = FoldStatus::UNKNOWN;

    void NotifyFoldAngleChanged(float foldAngle);
    bool HandleAbnormalAngle();

    FoldScreenSensorManager();

    ~FoldScreenSensorManager() = default;

    float globalAngle = -1.0F;

    uint16_t globalHall = USHRT_MAX;

    bool registerPosture_ = false;

    bool registerHall_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif
#endif