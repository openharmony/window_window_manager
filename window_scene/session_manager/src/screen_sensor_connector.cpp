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

#include "screen_sensor_connector.h"

#include <chrono>
#include <securec.h>

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenSensorConnector"};
#ifdef WM_SUBSCRIBE_MOTION_ENABLE
    constexpr int32_t MOTION_ACTION_PORTRAIT = 0;
    constexpr int32_t MOTION_ACTION_LEFT_LANDSCAPE = 1;
    constexpr int32_t MOTION_ACTION_PORTRAIT_INVERTED = 2;
    constexpr int32_t MOTION_ACTION_RIGHT_LANDSCAPE = 3;
#endif
}

#ifdef WM_SUBSCRIBE_MOTION_ENABLE
bool MotionSubscriber::isMotionSensorSubscribed_ = false;
sptr<RotationMotionEventCallback> MotionSubscriber::motionEventCallback_ = nullptr;
#endif

void ScreenSensorConnector::SubscribeRotationSensor()
{
    WLOGFD("dms: subscribe rotation-related sensor");
#ifdef WM_SUBSCRIBE_MOTION_ENABLE
    MotionSubscriber::SubscribeMotionSensor();
    if (MotionSubscriber::isMotionSensorSubscribed_) {
        return;
    }
#endif
}

void ScreenSensorConnector::UnsubscribeRotationSensor()
{
#ifdef WM_SUBSCRIBE_MOTION_ENABLE
    MotionSubscriber::UnsubscribeMotionSensor();
#endif
}

// Motion
#ifdef WM_SUBSCRIBE_MOTION_ENABLE
void MotionSubscriber::SubscribeMotionSensor()
{
    WLOGFI("dms: Subscribe motion Sensor");
    if (isMotionSensorSubscribed_) {
        WLOGFE("dms: motion sensor's already subscribed");
        return;
    }
    sptr<RotationMotionEventCallback> callback = new (std::nothrow) RotationMotionEventCallback();
    if (callback == nullptr) {
        return;
    }
    int32_t ret = OHOS::Msdp::SubscribeCallback(OHOS::Msdp::MOTION_TYPE_ROTATION, callback);
    if (ret != 0) {
        return;
    }
    motionEventCallback_ = callback;
    isMotionSensorSubscribed_ = true;
}

void MotionSubscriber::UnsubscribeMotionSensor()
{
    if (!isMotionSensorSubscribed_) {
        WLOGFI("dms: Unsubscribe motion sensor");
        return;
    }
    int32_t ret = OHOS::Msdp::UnsubscribeCallback(OHOS::Msdp::MOTION_TYPE_ROTATION, motionEventCallback_);
    if (ret != 0) {
        return;
    }
    isMotionSensorSubscribed_ = false;
}

void RotationMotionEventCallback::OnMotionChanged(const MotionEvent& motionData)
{
    DeviceRotation motionRotation = DeviceRotation::INVALID;
    switch (motionData.status) {
        case MOTION_ACTION_PORTRAIT: {
            motionRotation = DeviceRotation::ROTATION_PORTRAIT;
            break;
        }
        case MOTION_ACTION_LEFT_LANDSCAPE: {
            motionRotation = ScreenRotationProperty::IsDefaultDisplayRotationPortrait() ?
                DeviceRotation::ROTATION_LANDSCAPE_INVERTED : DeviceRotation::ROTATION_LANDSCAPE;
            break;
        }
        case MOTION_ACTION_PORTRAIT_INVERTED: {
            motionRotation = DeviceRotation::ROTATION_PORTRAIT_INVERTED;
            break;
        }
        case MOTION_ACTION_RIGHT_LANDSCAPE: {
            motionRotation = ScreenRotationProperty::IsDefaultDisplayRotationPortrait() ?
                DeviceRotation::ROTATION_LANDSCAPE : DeviceRotation::ROTATION_LANDSCAPE_INVERTED;
            break;
        }
        default: {
            break;
        }
    }
    ScreenRotationProperty::HandleSensorEventInput(motionRotation);
}
#endif
} // Rosen
} // OHOS