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
 
#ifndef OHOS_ROSEN_SUPER_FOLD_SENSOR_MANAGER_H
#define OHOS_ROSEN_SUPER_FOLD_SENSOR_MANAGER_H
 
#ifdef SENSOR_ENABLE
 
#include <deque>
#include <memory>
#include <mutex>
#include <sstream>
#include <functional>
#include <climits>
 
#include "refbase.h"
#include "wm_single_instance.h"
#include "ws_common.h"
#include "sensor_agent.h"
#include "sensor_agent_type.h"
#include "session/screen/include/screen_property.h"
#include "dm_common.h"
 
namespace OHOS {

namespace Rosen {
 
class SuperFoldSensorManager : public RefBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(SuperFoldSensorManager);
 
public:
 
    void RegisterPostureCallback(); //折叠角度注册回调
 
    void RegisterHallCallback(); // 磁吸键盘
 
    void RegisterSoftKeyboardCallback();
 
    void UnregisterPostureCallback();
 
    void UnregisterHallCallback();
 
    void HandlePostureData(const SensorEvent * const event);
 
    void HandleSoftKeyboardData();
 
    void HandleHallData(const SensorEvent * const event);

    void HandleSuperSensorChange();

    void SetHasKeyboard(bool flag);
    
private:
    
    std::recursive_mutex mutex_;

    bool hasKeyboard_ = false;

    SuperFoldStatusChangeEvents events_ = SuperFoldStatusChangeEvents::UNDEFINED;
 
    SensorUser postureUser {};
 
    SensorUser hallUser {};
 
    float globalAngle = 170.0F;

    uint16_t globalHall = USHRT_MAX;
 
    void NotifyFoldAngleChanged(float foldAngle);
 
    void NotifyHallChanged(uint16_t hall);

    void NotifySoftKeyboardChanged();
 
    SuperFoldSensorManager();
 
    ~SuperFoldSensorManager();
};
}
}
 
#endif // SENSOR_ENABLE
#endif // OHOS_ROSEN_SUPER_FOLD_SENSOR_MANAGER_H