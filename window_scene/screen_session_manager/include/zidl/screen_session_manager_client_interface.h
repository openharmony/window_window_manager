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

#ifndef OHOS_ROSEN_SCREEN_SESSION_MANAGER_CLIENT_INTERFACE_H
#define OHOS_ROSEN_SCREEN_SESSION_MANAGER_CLIENT_INTERFACE_H

#include <iremote_broker.h>
#include <screen_manager/screen_types.h>

#include "display_info.h"
#include "session/screen/include/screen_property.h"

namespace OHOS::Rosen {
class IScreenSessionManagerClient : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IScreenSessionManagerClient");

    enum class ScreenSessionManagerClientMessage : uint32_t {
        TRANS_ID_ON_SCREEN_CONNECTION_CHANGED,
        TRANS_ID_ON_PROPERTY_CHANGED,
        TRANS_ID_ON_SENSOR_ROTATION_CHANGED,
        TRANS_ID_ON_SCREEN_ORIENTATION_CHANGED,
        TRANS_ID_ON_SCREEN_ROTATION_LOCKED_CHANGED,
        TRANS_ID_ON_DISPLAY_STATE_CHANGED,
        TRANS_ID_ON_SCREEN_SHOT,
        TRANS_ID_ON_IMMERSIVE_STATE_CHANGED,
    };

    virtual void OnScreenConnectionChanged(ScreenId screenId, ScreenEvent screenEvent,
        ScreenId rsId, const std::string& name) = 0;
    virtual void OnPropertyChanged(ScreenId screenId,
        const ScreenProperty& property, ScreenPropertyChangeReason reason) = 0;
    virtual void OnSensorRotationChanged(ScreenId screenId, float sensorRotation) = 0;
    virtual void OnScreenOrientationChanged(ScreenId screenId, float screenOrientation) = 0;
    virtual void OnScreenRotationLockedChanged(ScreenId screenId, bool isLocked) = 0;

    virtual void OnDisplayStateChanged(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type) = 0;
    virtual void OnScreenshot(DisplayId displayId) = 0;
    virtual void OnImmersiveStateChanged(bool& immersive) = 0;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_CLIENT_INTERFACE_H
