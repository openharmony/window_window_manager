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

#include "screen_rotation_property.h"

#include <chrono>
#include <securec.h>

#include "screen_session_manager.h"
#include "parameters.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenRotationProperty"};
}

DisplayId ScreenRotationProperty::defaultDisplayId_ = 0;
Rotation ScreenRotationProperty::currentDisplayRotation_;
bool ScreenRotationProperty::isScreenRotationLocked_ = true;
uint32_t ScreenRotationProperty::defaultDeviceRotationOffset_ = 0;
Orientation ScreenRotationProperty::lastOrientationType_ = Orientation::UNSPECIFIED;
Rotation ScreenRotationProperty::lastSensorDecidedRotation_;
Rotation ScreenRotationProperty::rotationLockedRotation_;
uint32_t ScreenRotationProperty::defaultDeviceRotation_ = 0;
std::map<SensorRotation, DeviceRotation> ScreenRotationProperty::sensorToDeviceRotationMap_;
std::map<DeviceRotation, Rotation> ScreenRotationProperty::deviceToDisplayRotationMap_;
std::map<Rotation, DisplayOrientation> ScreenRotationProperty::displayToDisplayOrientationMap_;
DeviceRotation ScreenRotationProperty::lastSensorRotationConverted_ = DeviceRotation::INVALID;

void ScreenRotationProperty::Init()
{
    ProcessRotationMapping();
    currentDisplayRotation_ = GetCurrentDisplayRotation();
    defaultDisplayId_ = GetDefaultDisplayId();
    lastSensorDecidedRotation_ = currentDisplayRotation_;
    rotationLockedRotation_ = currentDisplayRotation_;
}

DisplayId ScreenRotationProperty::GetDefaultDisplayId()
{
    sptr<DisplayInfo> defaultDisplayInfo = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    if (defaultDisplayInfo == nullptr) {
        WLOGFW("GetDefaultDisplayId, defaultDisplayInfo is nullptr.");
        return DISPLAY_ID_INVALID;
    }
    return defaultDisplayInfo->GetDisplayId();
}

bool ScreenRotationProperty::IsScreenRotationLocked()
{
    return isScreenRotationLocked_;
}

DMError ScreenRotationProperty::SetScreenRotationLocked(bool isLocked)
{
    isScreenRotationLocked_ = isLocked;
    if (isLocked) {
        rotationLockedRotation_ = GetCurrentDisplayRotation();
        return DMError::DM_OK;
    }
    if (GetCurrentDisplayRotation() == ConvertDeviceToDisplayRotation(lastSensorRotationConverted_)) {
        return DMError::DM_OK;
    }
    Orientation currentOrientation = GetPreferredOrientation();
    if (IsSensorRelatedOrientation(currentOrientation)) {
        ProcessSwitchToSensorRelatedOrientation(currentOrientation, lastSensorRotationConverted_);
    }
    return DMError::DM_OK;
}

void ScreenRotationProperty::SetDefaultDeviceRotationOffset(uint32_t defaultDeviceRotationOffset)
{
    // Available options for defaultDeviceRotationOffset: {0, 90, 180, 270}
    if (defaultDeviceRotationOffset < 0 || defaultDeviceRotationOffset > 270 || defaultDeviceRotationOffset % 90 != 0) {
        return;
    }
    defaultDeviceRotationOffset_ = defaultDeviceRotationOffset;
}

void ScreenRotationProperty::HandleSensorEventInput(DeviceRotation deviceRotation)
{
    WLOGFI("ScreenRotationProperty::HandleSensorEventInput deviceRotation: %{public}d", deviceRotation);
    auto isPhone = system::GetParameter("const.product.devicetype", "unknown") == "phone";
    auto isPad = system::GetParameter("const.product.devicetype", "unknown") == "tablet";
    if (!isPhone && !isPad) {
        WLOGFW("device is not phone or pad, return.");
        return;
    }
    auto screenSession = ScreenSessionManager::GetInstance().GetDefaultScreenSession();
    if (!screenSession) {
        WLOGFW("screenSession is null.");
        return;
    }
    screenSession->SetSensorRotation(deviceRotation);
    if (deviceRotation == DeviceRotation::INVALID) {
        WLOGFW("deviceRotation is invalid, return.");
        return;
    }
    Rotation targetSensorRotation = ConvertDeviceToDisplayRotation(deviceRotation);
    if (lastSensorRotationConverted_ == deviceRotation &&
        screenSession->ConvertRotationToFloat(targetSensorRotation) == screenSession->GetCurrentSensorRotation()) {
        return;
    }
    lastSensorRotationConverted_ = deviceRotation;
    screenSession->SensorRotationChange(targetSensorRotation);
}

Rotation ScreenRotationProperty::GetCurrentDisplayRotation()
{
    sptr<DisplayInfo> defaultDisplayInfo = ScreenSessionManager::GetInstance().GetDefaultDisplayInfo();
    if (defaultDisplayInfo == nullptr) {
        WLOGFW("Cannot get default display info");
        return defaultDeviceRotation_ == 0 ? ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT) :
            ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE);
    }
    return defaultDisplayInfo->GetRotation();
}

Orientation ScreenRotationProperty::GetPreferredOrientation()
{
    return Orientation::SENSOR;
}

Rotation ScreenRotationProperty::CalcTargetDisplayRotation(
    Orientation requestedOrientation, DeviceRotation sensorRotationConverted)
{
    switch (requestedOrientation) {
        case Orientation::SENSOR: {
            lastSensorDecidedRotation_ = ConvertDeviceToDisplayRotation(sensorRotationConverted);
            return lastSensorDecidedRotation_;
        }
        case Orientation::SENSOR_VERTICAL: {
            return ProcessAutoRotationPortraitOrientation(sensorRotationConverted);
        }
        case Orientation::SENSOR_HORIZONTAL: {
            return ProcessAutoRotationLandscapeOrientation(sensorRotationConverted);
        }
        case Orientation::AUTO_ROTATION_RESTRICTED: {
            if (isScreenRotationLocked_) {
                return currentDisplayRotation_;
            }
            lastSensorDecidedRotation_ = ConvertDeviceToDisplayRotation(sensorRotationConverted);
            return lastSensorDecidedRotation_;
        }
        case Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED: {
            if (isScreenRotationLocked_) {
                return currentDisplayRotation_;
            }
            return ProcessAutoRotationPortraitOrientation(sensorRotationConverted);
        }
        case Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED: {
            if (isScreenRotationLocked_) {
                return currentDisplayRotation_;
            }
            return ProcessAutoRotationLandscapeOrientation(sensorRotationConverted);
        }
        default: {
            return currentDisplayRotation_;
        }
    }
}

Rotation ScreenRotationProperty::ProcessAutoRotationPortraitOrientation(DeviceRotation sensorRotationConverted)
{
    if (IsDeviceRotationHorizontal(sensorRotationConverted)) {
        return currentDisplayRotation_;
    }
    lastSensorDecidedRotation_ = ConvertDeviceToDisplayRotation(sensorRotationConverted);
    return lastSensorDecidedRotation_;
}

Rotation ScreenRotationProperty::ProcessAutoRotationLandscapeOrientation(DeviceRotation sensorRotationConverted)
{
    if (IsDeviceRotationVertical(sensorRotationConverted)) {
        return currentDisplayRotation_;
    }
    lastSensorDecidedRotation_ = ConvertDeviceToDisplayRotation(sensorRotationConverted);
    return lastSensorDecidedRotation_;
}

void ScreenRotationProperty::SetScreenRotation(Rotation targetRotation)
{
    if (targetRotation == GetCurrentDisplayRotation()) {
        return;
    }
    ScreenSessionManager::GetInstance().SetRotationFromWindow(targetRotation);
    WLOGFI("dms: Set screen rotation: %{public}u", targetRotation);
}

DeviceRotation ScreenRotationProperty::CalcDeviceRotation(SensorRotation sensorRotation)
{
    if (sensorRotation == SensorRotation::INVALID) {
        return DeviceRotation::INVALID;
    }
    // offset(in degree) divided by 90 to get rotation bias
    int32_t bias = static_cast<int32_t>(defaultDeviceRotationOffset_ / 90);
    int32_t deviceRotationValue = static_cast<int32_t>(sensorRotation) - bias;
    while (deviceRotationValue < 0) {
        // +4 is used to normalize the values into the range 0~3, corresponding to the four rotations.
        deviceRotationValue += 4;
    }
    if (defaultDeviceRotation_ == 1) {
        deviceRotationValue += static_cast<int32_t>(defaultDeviceRotation_);
        // %2 to determine whether the rotation is horizontal or vertical.
        if (deviceRotationValue % 2 == 0) {
            // if device's default rotation is landscape, use -2 to swap 0 and 90, 180 and 270.
            deviceRotationValue -= 2;
        }
    }
    return static_cast<DeviceRotation>(deviceRotationValue);
}

bool ScreenRotationProperty::IsSensorRelatedOrientation(Orientation orientation)
{
    if ((orientation >= Orientation::UNSPECIFIED && orientation <= Orientation::REVERSE_HORIZONTAL) ||
        orientation == Orientation::LOCKED) {
        return false;
    }
    return true;
}

void ScreenRotationProperty::ProcessSwitchToSensorRelatedOrientation(
    Orientation orientation, DeviceRotation sensorRotationConverted)
{
    lastOrientationType_ = orientation;
    switch (orientation) {
        case Orientation::AUTO_ROTATION_RESTRICTED: {
            if (isScreenRotationLocked_) {
                SetScreenRotation(rotationLockedRotation_);
                return;
            }
            [[fallthrough]];
        }
        case Orientation::SENSOR: {
            ProcessSwitchToAutoRotation(sensorRotationConverted);
            return;
        }
        case Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED: {
            if (isScreenRotationLocked_) {
                ProcessSwitchToAutoRotationPortraitRestricted();
                return;
            }
            [[fallthrough]];
        }
        case Orientation::SENSOR_VERTICAL: {
            ProcessSwitchToAutoRotationPortrait(sensorRotationConverted);
            return;
        }
        case Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED: {
            if (isScreenRotationLocked_) {
                ProcessSwitchToAutoRotationLandscapeRestricted();
                return;
            }
            [[fallthrough]];
        }
        case Orientation::SENSOR_HORIZONTAL: {
            ProcessSwitchToAutoRotationLandscape(sensorRotationConverted);
            return;
        }
        default: {
            return;
        }
    }
}

void ScreenRotationProperty::ProcessSwitchToAutoRotation(DeviceRotation rotation)
{
    if (rotation != DeviceRotation::INVALID) {
        SetScreenRotation(ConvertDeviceToDisplayRotation(rotation));
    }
}

void ScreenRotationProperty::ProcessSwitchToAutoRotationPortrait(DeviceRotation rotation)
{
    if (IsDeviceRotationVertical(rotation)) {
        SetScreenRotation(ConvertDeviceToDisplayRotation(rotation));
        return;
    }
    SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT));
}

void ScreenRotationProperty::ProcessSwitchToAutoRotationLandscape(DeviceRotation rotation)
{
    if (IsDeviceRotationHorizontal(rotation)) {
        SetScreenRotation(ConvertDeviceToDisplayRotation(rotation));
        return;
    }
    SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE));
}

void ScreenRotationProperty::ProcessSwitchToAutoRotationPortraitRestricted()
{
    if (IsCurrentDisplayVertical()) {
        return;
    }
    if (IsDisplayRotationVertical(rotationLockedRotation_)) {
        SetScreenRotation(rotationLockedRotation_);
        return;
    }
    SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT));
}

void ScreenRotationProperty::ProcessSwitchToAutoRotationLandscapeRestricted()
{
    if (IsCurrentDisplayHorizontal()) {
        return;
    }
    if (IsDisplayRotationHorizontal(rotationLockedRotation_)) {
        SetScreenRotation(rotationLockedRotation_);
        return;
    }
    SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE));
}

DeviceRotation ScreenRotationProperty::ConvertSensorToDeviceRotation(SensorRotation sensorRotation)
{
    if (sensorToDeviceRotationMap_.empty()) {
        ProcessRotationMapping();
    }
    return sensorToDeviceRotationMap_.at(sensorRotation);
}

DisplayOrientation ScreenRotationProperty::ConvertRotationToDisplayOrientation(Rotation rotation)
{
    if (displayToDisplayOrientationMap_.empty()) {
        ProcessRotationMapping();
    }
    return displayToDisplayOrientationMap_.at(rotation);
}

Rotation ScreenRotationProperty::ConvertDeviceToDisplayRotation(DeviceRotation deviceRotation)
{
    if (deviceRotation == DeviceRotation::INVALID) {
        return GetCurrentDisplayRotation();
    }
    if (deviceToDisplayRotationMap_.empty()) {
        ProcessRotationMapping();
    }
    return deviceToDisplayRotationMap_.at(deviceRotation);
}

void ScreenRotationProperty::ProcessRotationMapping()
{
    sptr<SupportedScreenModes> modes =
        ScreenSessionManager::GetInstance().GetScreenModesByDisplayId(defaultDisplayId_);

    // 0 means PORTRAIT, 1 means LANDSCAPE.
    defaultDeviceRotation_ = (modes == nullptr || modes->width_ < modes->height_) ? 0 : 1;
    if (deviceToDisplayRotationMap_.empty()) {
        deviceToDisplayRotationMap_ = {
            {DeviceRotation::ROTATION_PORTRAIT,
                defaultDeviceRotation_ == 0 ? Rotation::ROTATION_0 : Rotation::ROTATION_90},
            {DeviceRotation::ROTATION_LANDSCAPE,
                defaultDeviceRotation_ == 1 ? Rotation::ROTATION_0 : Rotation::ROTATION_90},
            {DeviceRotation::ROTATION_PORTRAIT_INVERTED,
                defaultDeviceRotation_ == 0 ? Rotation::ROTATION_180 : Rotation::ROTATION_270},
            {DeviceRotation::ROTATION_LANDSCAPE_INVERTED,
                defaultDeviceRotation_ == 1 ? Rotation::ROTATION_180 : Rotation::ROTATION_270},
        };
    }
    if (displayToDisplayOrientationMap_.empty()) {
        displayToDisplayOrientationMap_ = {
            {defaultDeviceRotation_ == 0 ? Rotation::ROTATION_0 : Rotation::ROTATION_90,
                DisplayOrientation::PORTRAIT},
            {defaultDeviceRotation_ == 1 ? Rotation::ROTATION_0 : Rotation::ROTATION_90,
                DisplayOrientation::LANDSCAPE},
            {defaultDeviceRotation_ == 0 ? Rotation::ROTATION_180 : Rotation::ROTATION_270,
                DisplayOrientation::PORTRAIT_INVERTED},
            {defaultDeviceRotation_ == 1 ? Rotation::ROTATION_180 : Rotation::ROTATION_270,
                DisplayOrientation::LANDSCAPE_INVERTED},
        };
    }
    if (sensorToDeviceRotationMap_.empty()) {
        sensorToDeviceRotationMap_ = {
            {SensorRotation::ROTATION_0, CalcDeviceRotation(SensorRotation::ROTATION_0)},
            {SensorRotation::ROTATION_90, CalcDeviceRotation(SensorRotation::ROTATION_90)},
            {SensorRotation::ROTATION_180, CalcDeviceRotation(SensorRotation::ROTATION_180)},
            {SensorRotation::ROTATION_270, CalcDeviceRotation(SensorRotation::ROTATION_270)},
            {SensorRotation::INVALID, DeviceRotation::INVALID},
        };
    }
}

bool ScreenRotationProperty::IsDeviceRotationVertical(DeviceRotation deviceRotation)
{
    return (deviceRotation == DeviceRotation::ROTATION_PORTRAIT) ||
        (deviceRotation == DeviceRotation::ROTATION_PORTRAIT_INVERTED);
}

bool ScreenRotationProperty::IsDeviceRotationHorizontal(DeviceRotation deviceRotation)
{
    return (deviceRotation == DeviceRotation::ROTATION_LANDSCAPE) ||
        (deviceRotation == DeviceRotation::ROTATION_LANDSCAPE_INVERTED);
}

bool ScreenRotationProperty::IsCurrentDisplayVertical()
{
    return IsDisplayRotationVertical(GetCurrentDisplayRotation());
}

bool ScreenRotationProperty::IsCurrentDisplayHorizontal()
{
    return IsDisplayRotationHorizontal(GetCurrentDisplayRotation());
}

bool ScreenRotationProperty::IsDefaultDisplayRotationPortrait()
{
    return Rotation::ROTATION_0 == ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT);
}

bool ScreenRotationProperty::IsDisplayRotationVertical(Rotation rotation)
{
    return (rotation == ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT)) ||
        (rotation == ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT_INVERTED));
}

bool ScreenRotationProperty::IsDisplayRotationHorizontal(Rotation rotation)
{
    return (rotation == ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE)) ||
        (rotation == ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE_INVERTED));
}

void ScreenRotationProperty::ProcessSwitchToSensorUnrelatedOrientation(Orientation orientation)
{
    if (lastOrientationType_ == orientation) {
        return;
    }
    lastOrientationType_ = orientation;
    switch (orientation) {
        case Orientation::UNSPECIFIED: {
            SetScreenRotation(Rotation::ROTATION_0);
            break;
        }
        case Orientation::VERTICAL: {
            SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT));
            break;
        }
        case Orientation::REVERSE_VERTICAL: {
            SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT_INVERTED));
            break;
        }
        case Orientation::HORIZONTAL: {
            SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE));
            break;
        }
        case Orientation::REVERSE_HORIZONTAL: {
            SetScreenRotation(ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE_INVERTED));
            break;
        }
        default: {
            return;
        }
    }
}

void ScreenRotationProperty::ProcessOrientationSwitch(Orientation orientation)
{
    if (!IsSensorRelatedOrientation(orientation)) {
        ProcessSwitchToSensorUnrelatedOrientation(orientation);
    } else {
        ProcessSwitchToSensorRelatedOrientation(orientation, lastSensorRotationConverted_);
    }
}
} // Rosen
} // OHOS