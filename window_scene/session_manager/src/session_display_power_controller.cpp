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

#include "session_display_power_controller.h"
#include "screen_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SessionDisplayPowerController"};
}

bool SessionDisplayPowerController::SuspendBegin(PowerStateChangeReason reason)
{
    WLOGFI("reason:%{public}u", reason);
    std::map<DisplayId, sptr<DisplayInfo>> emptyMap;
    displayStateChangeListener_(DISPLAY_ID_INVALID, nullptr, emptyMap, DisplayStateChangeType::BEFORE_SUSPEND);
    return true;
}

bool SessionDisplayPowerController::SetDisplayState(DisplayState state)
{
    WLOGFI("state:%{public}u", state);
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (displayState_ == state) {
            WLOGFE("state is already set");
            return false;
        }
    }
    switch (state) {
        case DisplayState::ON: {
            {
                std::lock_guard<std::recursive_mutex> lock(mutex_);
                displayState_ = state;
            }
            if (!ScreenSessionManager::GetInstance().IsMultiScreenCollaboration()) {
                ScreenSessionManager::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::DISPLAY_ON,
                    EventStatus::BEGIN, PowerStateChangeReason::STATE_CHANGE_REASON_INIT);
            }
            break;
        }
        case DisplayState::OFF: {
            {
                std::lock_guard<std::recursive_mutex> lock(mutex_);
                displayState_ = state;
            }
            if (!ScreenSessionManager::GetInstance().IsMultiScreenCollaboration()) {
                ScreenSessionManager::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::DISPLAY_OFF,
                    EventStatus::BEGIN, PowerStateChangeReason::STATE_CHANGE_REASON_INIT);
            }
            break;
        }
        default: {
            WLOGFW("unknown DisplayState!");
            return false;
        }
    }
    ScreenSessionManager::GetInstance().NotifyDisplayStateChanged(DISPLAY_ID_INVALID, state);
    return true;
}

DisplayState SessionDisplayPowerController::GetDisplayState(DisplayId displayId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return displayState_;
}

void SessionDisplayPowerController::NotifyDisplayEvent(DisplayEvent event)
{
    WLOGFI("DisplayEvent:%{public}u", event);
    if (event == DisplayEvent::UNLOCK) {
        std::map<DisplayId, sptr<DisplayInfo>> emptyMap;
        displayStateChangeListener_(DISPLAY_ID_INVALID, nullptr, emptyMap, DisplayStateChangeType::BEFORE_UNLOCK);
        ScreenSessionManager::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::DESKTOP_READY,
            EventStatus::BEGIN, PowerStateChangeReason::STATE_CHANGE_REASON_INIT);
        return;
    }
}
}
}