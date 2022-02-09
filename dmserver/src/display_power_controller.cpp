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

#include "display_power_controller.h"
#include "display_manager_service.h"
#include "display_manager_agent_controller.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayPowerController"};
}

bool DisplayPowerController::SuspendBegin(PowerStateChangeReason reason)
{
    WLOGFI("reason:%{public}u", reason);
    DisplayManagerService::GetInstance().NotifyDisplayStateChange(DisplayStateChangeType::BEFORE_SUSPEND);
    return true;
}

bool DisplayPowerController::SetDisplayState(DisplayState state)
{
    WLOGFI("state:%{public}u", state);
    if (displayState_ == state) {
        WLOGFE("state is already set");
        return false;
    }
    switch (state) {
        case DisplayState::ON: {
            // TODO: open vsync and SendSystemEvent to keyguard
            displayState_ = state;
            DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::DISPLAY_ON,
                EventStatus::BEGIN);
            break;
        }
        case DisplayState::OFF: {
            displayState_ = state;
            DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::DISPLAY_OFF,
                EventStatus::BEGIN);
            break;
        }
        default: {
            WLOGFW("unknown DisplayState!");
            return false;
        }
    }
    DisplayManagerAgentController::GetInstance().NotifyDisplayStateChanged(state);
    return true;
}

DisplayState DisplayPowerController::GetDisplayState(DisplayId displayId)
{
    return displayState_;
}

void DisplayPowerController::NotifyDisplayEvent(DisplayEvent event)
{
    WLOGFI("DisplayEvent:%{public}u", event);
    if (event == DisplayEvent::UNLOCK) {
        DisplayManagerService::GetInstance().NotifyDisplayStateChange(DisplayStateChangeType::BEFORE_UNLOCK);
        DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::DESKTOP_READY,
            EventStatus::BEGIN);
        return;
    }
    // TODO: set displayState_ ON when keyguard is drawn
}
}
}