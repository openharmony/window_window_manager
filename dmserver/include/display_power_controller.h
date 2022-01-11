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

#ifndef OHOS_ROSEN_DISPLAY_POWER_CONTROLLER_H
#define OHOS_ROSEN_DISPLAY_POWER_CONTROLLER_H

#include <map>
#include <mutex>
#include "dm_common.h"

namespace OHOS {
namespace Rosen {
class DisplayPowerController {
public:
    DisplayPowerController() = default;
    virtual ~DisplayPowerController() = default;

    bool SuspendBegin(PowerStateChangeReason reason);
    bool SetDisplayState(DisplayState state);
    DisplayState GetDisplayState(uint64_t displayId);
    void NotifyDisplayEvent(DisplayEvent event);

private:
    std::recursive_mutex mutex_;
    DisplayState displayState_ { DisplayState::ON };
};
}
}
#endif // OHOS_ROSEN_DISPLAY_POWER_CONTROLLER_H