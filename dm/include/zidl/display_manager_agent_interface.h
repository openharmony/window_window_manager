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

#ifndef OHOS_ROSEN_DISPLAY_MANAGER_AGENT_INTERFACE_H
#define OHOS_ROSEN_DISPLAY_MANAGER_AGENT_INTERFACE_H

#include <iremote_broker.h>
#include "dm_common.h"

namespace OHOS {
namespace Rosen {
enum class DisplayManagerAgentType : uint32_t {
    DISPLAY_POWER_EVENT_LISTENER,
    DISPLAY_STATE_LISTENER,
};

class IDisplayManagerAgent : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IDisplayManagerAgent");

    enum {
        TRANS_ID_NOTIFY_DISPLAY_POWER_EVENT = 1,
        TRANS_ID_NOTIFY_DISPLAY_STATE_CHANGED,
    };
    virtual void NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) = 0;
    virtual void NotifyDisplayStateChanged(DisplayState state) = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_DISPLAY_MANAGER_AGENT_INTERFACE_H