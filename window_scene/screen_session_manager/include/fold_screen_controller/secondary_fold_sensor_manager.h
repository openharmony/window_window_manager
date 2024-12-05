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

#ifndef OHOS_ROSEN_SECONDARY_FOLD_SENSOR_MANAGER_H
#define OHOS_ROSEN_SECONDARY_FOLD_SENSOR_MANAGER_H

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

class SecondaryFoldSensorManager : public RefBase {
    WM_DECLARE_SINGLE_INSTANCE_BASE(SecondaryFoldSensorManager);
public:
    void RegisterApplicationStateObserver();
    void SetFoldScreenPolicy(sptr<FoldScreenPolicy> foldScreenPolicy);
    void SetSensorFoldStateManager(sptr<SensorFoldStateManager> sensorFoldStateManager);
    void SecondarySensorPostureDataCallback();
    void SecondarySensorHallDataCallbackExt();
    void RegisterPostureCallback();
    void RegisterHallCallback();
    void UnRegisterPostureCallback();
    void UnRegisterHallCallback();
    void HandlePostureData(const SensorEvent * const event);
    void HandleHallDataExt(const SensorEvent * const event);
private:
    sptr<FoldScreenPolicy> foldScreenPolicy_;
    sptr<SensorFoldStateManager> sensorFoldStateManager_;

    FoldStatus mState_ = FoldStatus::UNKNOWN;
    SensorUser postureUser {};
    SensorUser hallUser {};
    std::vector<float> globalAngle = {-1.0F, -1.0F};
    std::vector<uint16_t> globalHall = {USHRT_MAX, USHRT_MAX};
    bool registerPosture_ = false;
    typedef struct EXTHALLData {
        float flag = 0.0F;
        float hallBc = 0.0F;
        float hallNfc = 0.0F;
        float hallProtectiveCase = 0.0F;
        float hallAb = 0.0F;
    } ExtHallData;

    typedef struct PostureData {
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
    } PostureData;

    SecondaryFoldSensorManager() = default;
    ~SecondaryFoldSensorManager() = default;
    void NotifyFoldAngleChanged(const std::vector<float> &angles);
    bool GetPostureInner(const SensorEvent * const event, float &valueBc, float &valueAb,
        float &valueAbAnti);
    bool GetHallInner(const SensorEvent * const event, uint16_t &valueBc, uint16_t &valueAb);
    bool IsDataBeyondBoundary();
};
} // namespace Rosen
} // namespace OHOS
#endif
#endif