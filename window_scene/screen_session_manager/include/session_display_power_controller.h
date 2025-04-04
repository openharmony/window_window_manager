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

#ifndef OHOS_ROSEN_SESSION_DISPLAY_POWER_CONTROLLER_H
#define OHOS_ROSEN_SESSION_DISPLAY_POWER_CONTROLLER_H

#include <atomic>
#include <map>
#include <mutex>
#include <refbase.h>
#include "display.h"
#include "display_change_listener.h"
#include "dm_common.h"

namespace OHOS {
namespace Rosen {
class SessionDisplayPowerController : public RefBase {
using SessionDisplayStateChangeListener = std::function<void(DisplayId, sptr<DisplayInfo>,
    const std::map<DisplayId, sptr<DisplayInfo>>&, DisplayStateChangeType)>;
public:
    SessionDisplayPowerController(SessionDisplayStateChangeListener listener)
        : displayStateChangeListener_(listener)
    {
    }
    virtual ~SessionDisplayPowerController() = default;

    bool SuspendBegin(PowerStateChangeReason reason);
    bool SetDisplayState(DisplayState state);
    DisplayState GetDisplayState(DisplayId displayId);
    void NotifyDisplayEvent(DisplayEvent event);

    bool needCancelNotify_ { false };
    bool canceledSuspend_ { false };
    bool canCancelSuspendNotify_ { false };
    bool skipScreenOffBlock_ { false };
    bool isSuspendBegin_ { false };
    std::mutex notifyMutex_;

private:
    void WaitScreenOffNotify(DisplayState& state);
    void SetDisplayStateToOn(DisplayState& state);
    bool HandleSetDisplayStateOff(DisplayState& state);
    std::atomic<DisplayState> displayState_ { DisplayState::UNKNOWN };
    SessionDisplayStateChangeListener displayStateChangeListener_;
};
}
}
#endif // OHOS_ROSEN_DISPLAY_POWER_CONTROLLER_H