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

#include "screen_rotation_controller.h"

#include <chrono>
#include <securec.h>

#include "display_manager_service_inner.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenRotationController"};
    constexpr int64_t ORIENTATION_SENSOR_SAMPLING_RATE = 200000000; // 200ms
    constexpr int64_t ORIENTATION_SENSOR_REPORTING_RATE = 0;
    constexpr long ORIENTATION_SENSOR_CALLBACK_TIME_INTERVAL = 200; // 200ms
    constexpr int VALID_INCLINATION_ANGLE_THRESHOLD_COEFFICIENT = 3;
}

enum class ROTATION_DEGREE_RANGE { };

DisplayId ScreenRotationController::defaultDisplayId_ = 0;
bool ScreenRotationController::isGravitySensorSubscribed_ = false;
SensorUser ScreenRotationController::user_;
Rotation ScreenRotationController::currentDisplayRotation_;
bool ScreenRotationController::isScreenRotationLocked_ = false;
long ScreenRotationController::lastCallbackTime_ = 0;
uint32_t ScreenRotationController::defaultDeviceRotationOffset_ = 0;
Orientation ScreenRotationController::lastOrientationType_ = Orientation::UNSPECIFIED;
Rotation ScreenRotationController::defaultLandscapeDisplayRotation_ = Rotation::ROTATION_90;

void ScreenRotationController::SubscribeGravitySensor()
{
    WLOGFI("dms: Subscribe gravity Sensor");
    if (isGravitySensorSubscribed_) {
        WLOGFE("dms: gravity sensor's already subscribed");
        return;
    }
    if (strcpy_s(user_.name, sizeof(user_.name), "ScreenRotationController") != EOK) {
        WLOGFE("dms strcpy_s error");
        return;
    }
    user_.userData = nullptr;
    user_.callback = &HandleGravitySensorEventCallback;
    SubscribeSensor(SENSOR_TYPE_ID_GRAVITY, &user_);
    SetBatch(SENSOR_TYPE_ID_GRAVITY, &user_, ORIENTATION_SENSOR_SAMPLING_RATE, ORIENTATION_SENSOR_REPORTING_RATE);
    SetMode(SENSOR_TYPE_ID_GRAVITY, &user_, SENSOR_ON_CHANGE);
    ActivateSensor(SENSOR_TYPE_ID_GRAVITY, &user_);
    isGravitySensorSubscribed_ = true;
}

void ScreenRotationController::UnsubscribeGravitySensor()
{
    WLOGFI("dms: Unsubscribe gravity Sensor");
    if (!isGravitySensorSubscribed_) {
        WLOGFE("dms: Orientation Sensor is not subscribed");
        return;
    }
    DeactivateSensor(SENSOR_TYPE_ID_GRAVITY, &user_);
    UnsubscribeSensor(SENSOR_TYPE_ID_GRAVITY, &user_);
    isGravitySensorSubscribed_ = false;
}

bool ScreenRotationController::IsScreenRotationLocked()
{
    return isScreenRotationLocked_;
}

void ScreenRotationController::SetScreenRotationLocked(bool isLocked)
{
    isScreenRotationLocked_ = isLocked;
}

void ScreenRotationController::SetDefaultDeviceRotationOffset(uint32_t defaultDeviceRotationOffset)
{
    // Available options for defaultDeviceRotationOffset: {0, 90, 180, 270}
    if (defaultDeviceRotationOffset < 0 || defaultDeviceRotationOffset > 270 || defaultDeviceRotationOffset % 90 != 0) {
        return;
    }
    defaultDeviceRotationOffset_ = defaultDeviceRotationOffset;
    defaultLandscapeDisplayRotation_ = static_cast<Rotation>(
        // divided by 90 to get bias, %4 to normalize the values into the range 0~3
        (static_cast<uint32_t>(Rotation::ROTATION_90) + (defaultDeviceRotationOffset_ / 90)) % 4);
}

void ScreenRotationController::HandleGravitySensorEventCallback(SensorEvent *event)
{
    if (!CheckCallbackTimeInterval()) {
        return;
    }
    if (event->sensorTypeId != SENSOR_TYPE_ID_GRAVITY) {
        WLOGE("dms: Orientation Sensor Callback is not SENSOR_TYPE_ID_GRAVITY");
        return;
    }
    Orientation orientation = GetRequestedOrientation();
    currentDisplayRotation_ = GetCurrentDisplayRotation();
    HandleUnspecifiedOrientation(orientation);
    if (!IsSensorRelatedOrientation(orientation)) {
        lastOrientationType_ = orientation;
        return;
    }
    
    GravityData* gravityData = reinterpret_cast<GravityData*>(event->data);
    int sensorDegree = CalcRotationDegree(gravityData);
    Rotation currentSensorRotation;
    // Use ROTATION_0 when degree range is [0, 30]∪[330, 359]
    if (sensorDegree >= 0 && (sensorDegree <= 30 || sensorDegree >= 330)) {
        currentSensorRotation = Rotation::ROTATION_0;
    } else if (sensorDegree >= 60 && sensorDegree <= 120) { // Use ROTATION_90 when degree range is [60, 120]
        currentSensorRotation = Rotation::ROTATION_90;
    } else if (sensorDegree >= 150 && sensorDegree <= 210) { // Use ROTATION_180 when degree range is [150, 210]
        currentSensorRotation = Rotation::ROTATION_180;
    } else if (sensorDegree >= 240 && sensorDegree <= 300) { // Use ROTATION_270 when degree range is [240, 300]
        currentSensorRotation = Rotation::ROTATION_270;
    } else {
        ProcessRotationWhenSensorDataNotValid(orientation);
        return;
    }
    lastOrientationType_ = orientation;
    if ((ConvertToDeviceRotation(currentSensorRotation) == currentDisplayRotation_) &&
        (orientation == Orientation::SENSOR || orientation == Orientation::AUTO_ROTATION_RESTRICTED)) {
        return;
    }

    Rotation targetDisplayRotation = CalcTargetDisplayRotation(orientation, currentSensorRotation);
    if (targetDisplayRotation == currentDisplayRotation_) {
        return;
    }
    
    SetScreenRotation(targetDisplayRotation);
}

int ScreenRotationController::CalcRotationDegree(GravityData* gravityData)
{
    float x = gravityData->x;
    float y = gravityData->y;
    float z = gravityData->z;
    int degree = -1;
    if ((x * x + y * y) * VALID_INCLINATION_ANGLE_THRESHOLD_COEFFICIENT < z * z) {
        return degree;
    }
    // arccotx = pi / 2 - arctanx, 90 is used to calculate acot(in degree); degree = rad / pi * 180
    degree = 90 - static_cast<int>(round(atan2(y, -x) / M_PI * 180));
    // Normalize the degree to the range of 0~360
    return degree >= 0 ? degree % 360 : degree % 360 + 360;
}

Rotation ScreenRotationController::GetCurrentDisplayRotation()
{
    return DisplayManagerServiceInner::GetInstance().GetDisplayById(defaultDisplayId_)->GetRotation();
}

Orientation ScreenRotationController::GetRequestedOrientation()
{
    Orientation orientation = Orientation::UNSPECIFIED;
    DisplayManagerServiceInner::GetInstance().GetFullScreenWindowRequestedOrientation(defaultDisplayId_, orientation);
    return orientation;
}

Rotation ScreenRotationController::CalcTargetDisplayRotation(
    Orientation requestedOrientation, Rotation sensorRotation)
{
    switch (requestedOrientation) {
        case Orientation::SENSOR: {
            return ConvertToDeviceRotation(sensorRotation);
        }
        case Orientation::SENSOR_VERTICAL: {
            return ProcessAutoRotationPortraitOrientation(sensorRotation);
        }
        case Orientation::SENSOR_HORIZONTAL: {
            return ProcessAutoRotationLandscapeOrientation(sensorRotation);
        }
        case Orientation::AUTO_ROTATION_RESTRICTED: {
            if (isScreenRotationLocked_) {
                return currentDisplayRotation_;
            }
            return ConvertToDeviceRotation(sensorRotation);
        }
        case Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED: {
            if (isScreenRotationLocked_) {
                return currentDisplayRotation_;
            }
            return ProcessAutoRotationPortraitOrientation(sensorRotation);
        }
        case Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED: {
            if (isScreenRotationLocked_) {
                return currentDisplayRotation_;
            }
            return ProcessAutoRotationLandscapeOrientation(sensorRotation);
        }
        default: {
            return currentDisplayRotation_;
        }
    }
}

Rotation ScreenRotationController::ProcessAutoRotationPortraitOrientation(Rotation sensorRotation)
{
    if (sensorRotation == Rotation::ROTATION_0 || sensorRotation == Rotation::ROTATION_180) {
        return ConvertToDeviceRotation(sensorRotation);
    }
    if (currentDisplayRotation_ == ConvertToDeviceRotation(Rotation::ROTATION_0) ||
        currentDisplayRotation_ == ConvertToDeviceRotation(Rotation::ROTATION_180)) {
        return currentDisplayRotation_;
    }
    return ConvertToDeviceRotation(Rotation::ROTATION_0);
}

Rotation ScreenRotationController::ProcessAutoRotationLandscapeOrientation(Rotation sensorRotation)
{
    if (sensorRotation == Rotation::ROTATION_90 || sensorRotation == Rotation::ROTATION_270) {
        return ConvertToDeviceRotation(sensorRotation);
    }
    if (currentDisplayRotation_ == ConvertToDeviceRotation(Rotation::ROTATION_90) ||
        currentDisplayRotation_ == ConvertToDeviceRotation(Rotation::ROTATION_270)) {
        return currentDisplayRotation_;
    }
    return defaultLandscapeDisplayRotation_;
}

void ScreenRotationController::SetScreenRotation(Rotation targetRotation)
{
    DisplayManagerServiceInner::GetInstance().GetDefaultDisplay()->SetRotation(targetRotation);
    DisplayManagerServiceInner::GetInstance().SetRotationFromWindow(defaultDisplayId_, targetRotation);
}

bool ScreenRotationController::CheckCallbackTimeInterval()
{
    std::chrono::milliseconds ms = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch();
    long currentTimeInMillitm = ms.count();
    if (currentTimeInMillitm - lastCallbackTime_ < ORIENTATION_SENSOR_CALLBACK_TIME_INTERVAL) {
        return false;
    }
    lastCallbackTime_ = currentTimeInMillitm;
    return true;
}

Rotation ScreenRotationController::ConvertToDeviceRotation(Rotation sensorRotation)
{
    int32_t bias = defaultDeviceRotationOffset_ / 90; // offset(in degree) divided by 90 to get rotation bias
    int32_t deviceRotationValue = static_cast<int32_t>(sensorRotation) - bias;
    while (deviceRotationValue < 0) {
        // +4 is used to normalize the values into the range 0~3, corresponding to the four rotations.
        deviceRotationValue += 4;
    }
    return static_cast<Rotation>(deviceRotationValue);
}

bool ScreenRotationController::IsSensorRelatedOrientation(Orientation orientation)
{
    if ((orientation >= Orientation::BEGIN && orientation <= Orientation::REVERSE_HORIZONTAL) ||
        orientation == Orientation::LOCKED) {
        return false;
    }
    return true;
}

void ScreenRotationController::HandleUnspecifiedOrientation(Orientation orientation)
{
    if (lastOrientationType_ == orientation) {
        return;
    }
    if (orientation == Orientation::UNSPECIFIED) {
        SetScreenRotation(Rotation::ROTATION_0);
        lastOrientationType_ = orientation;
    }
}

void ScreenRotationController::ProcessRotationWhenSensorDataNotValid(Orientation orientation)
{
    if (lastOrientationType_ == orientation) {
        return;
    }
    lastOrientationType_ = orientation;
    switch (orientation) {
        case Orientation::SENSOR_VERTICAL:
        case Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED: {
            if (currentDisplayRotation_ == ConvertToDeviceRotation(Rotation::ROTATION_90) ||
                    currentDisplayRotation_ == ConvertToDeviceRotation(Rotation::ROTATION_270)) {
                SetScreenRotation(ConvertToDeviceRotation(Rotation::ROTATION_0));
            }
            break;
        }
        case Orientation::SENSOR_HORIZONTAL:
        case Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED: {
            if (currentDisplayRotation_ == ConvertToDeviceRotation(Rotation::ROTATION_0) ||
                    currentDisplayRotation_ == ConvertToDeviceRotation(Rotation::ROTATION_180)) {
                SetScreenRotation(defaultLandscapeDisplayRotation_);
            }
            break;
        }
        default: {
            return;
        }
    }
}
} // Rosen
} // OHOS