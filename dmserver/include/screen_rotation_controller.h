/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_SCREEN_ROTATION_CONTROLLER_H
#define OHOS_ROSEN_SCREEN_ROTATION_CONTROLLER_H

#include <refbase.h>

#include "sensor_agent.h"

#include "dm_common.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
class ScreenRotationController : public RefBase {
public:
    ScreenRotationController() = delete;
    ~ScreenRotationController() = default;
    static void SubscribeGravitySensor();
    static void UnsubscribeGravitySensor();
    static bool IsScreenRotationLocked();
    static void SetScreenRotationLocked(bool isLocked);
    static void SetDefaultDeviceRotationOffset(uint32_t defaultDeviceRotationOffset);
private:
    static void HandleGravitySensorEventCallback(SensorEvent *event);
    static Rotation GetCurrentDisplayRotation();
    static Orientation GetDisplayOrientation();
    static int CalcRotationDegree(GravityData* gravityData);
    static Rotation CalcTargetDisplayRotation(Orientation requestedOrientation, Rotation sensorRotation);
    static Rotation ProcessAutoRotationPortraitOrientation(Rotation sensorRotation);
    static Rotation ProcessAutoRotationLandscapeOrientation(Rotation sensorRotation);
    static void SetScreenRotation(Rotation targetRotation);
    static bool CheckCallbackTimeInterval();
    static int CalcSensorDisplayRotation(int orientationDegree);
    static Rotation ConvertToDeviceRotation(Rotation sensorRotation);
    static bool IsSensorRelatedOrientation(Orientation orientation);

    static DisplayId defaultDisplayId_;
    static bool isGravitySensorSubscribed_;
    static bool isScreenRotationLocked_;
    static SensorUser user_;
    static Rotation currentDisplayRotation_;
    static long lastCallbackTime_;
    static uint32_t defaultDeviceRotationOffset_;
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_SCREEN_ROTATION_CONTROLLER_H
