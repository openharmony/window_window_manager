/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_DM_COMMON_H
#define OHOS_ROSEN_DM_COMMON_H

#include <refbase.h>

namespace OHOS {
namespace Rosen {
constexpr int32_t INVALID_DISPLAY_ID = -1;
enum class PowerStateChangeReason : uint32_t {
    POWER_BUTTON
};

enum class DisplayPowerState : uint32_t {
    POWER_ON,
    POWER_STAND_BY,
    POWER_SUSPEND,
    POWER_OFF,
    POWER_BUTT,
    INVALID_STATE
};

enum class DisplayState : uint32_t {
    ON,
    OFF,
    UNKNOWN
};

enum class DisplayEvent : uint32_t {
    UNLOCK
};
using DisplayStateCallback = std::function<void(DisplayState)>;

enum class DisplayPowerEvent : uint32_t {
    WAKE_UP,
    SLEEP,
    DISPLAY_ON,
    DISPLAY_OFF,
    DESKTOP_READY
};

enum class EventStatus : uint32_t {
    BEGIN,
    END
};

class IDisplayPowerEventListener : public RefBase {
public:
    virtual void OnDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) = 0;
};
}
}
#endif // OHOS_ROSEN_DM_COMMON_H