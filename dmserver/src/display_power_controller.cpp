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
#include "window_manager_service_inner.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayPowerController"};
}

bool DisplayPowerController::SuspendBegin(PowerStateChangeReason reason)
{
    WLOGFI("reason:%{public}u", reason);
    return WindowManagerServiceInner::GetInstance().NotifyDisplaySuspend();
}

bool DisplayPowerController::SetDisplayState(DisplayState state)
{
    WLOGFI("state:%{public}u", state);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (displayState_ == state) {
        WLOGFI("state is already set");
        return true;
    }
    switch (state) {
        case DisplayState::ON: {
            // TODO: open vsync and SendSystemEvent to keyguard
            break;
        }
        case DisplayState::OFF: {
            // TODO: SendSystemEvent to keyguard
            displayState_ = state;
            break;
        }
        default:
            WLOGFW("unknown DisplayState!");
    }
    return true;
}

DisplayState DisplayPowerController::GetDisplayState(uint64_t displayId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return displayState_;
}

void DisplayPowerController::NotifyDisplayEvent(DisplayEvent event)
{
    if (event == DisplayEvent::UNLOCK) {
        WLOGFI("DisplayEvent UNLOCK");
        WindowManagerServiceInner::GetInstance().RestoreSuspendedWindows();
        return;
    }
    // TODO: set displayState_ ON when keyguard is drawn
}
}
}