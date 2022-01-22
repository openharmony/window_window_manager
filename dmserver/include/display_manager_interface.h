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

#ifndef FOUNDATION_DMSERVER_DISPLAY_MANAGER_INTERFACE_H
#define FOUNDATION_DMSERVER_DISPLAY_MANAGER_INTERFACE_H

#include <iremote_broker.h>
#include <pixel_map.h>
#include <surface.h>

#include "dm_common.h"
#include "screen.h"
#include "display_info.h"
#include "zidl/display_manager_agent_interface.h"

namespace OHOS::Rosen {
class IDisplayManager : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IDisplayManager");

    enum {
        TRANS_ID_GET_DEFAULT_DISPLAY_ID = 0,
        TRANS_ID_GET_DISPLAY_BY_ID,
        TRANS_ID_GET_DISPLAY_SNAPSHOT,
        TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT,
        TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT,
        TRANS_ID_WAKE_UP_BEGIN,
        TRANS_ID_WAKE_UP_END,
        TRANS_ID_SUSPEND_BEGIN,
        TRANS_ID_SUSPEND_END,
        TRANS_ID_SET_SCREEN_POWER_FOR_ALL,
        TRANS_ID_SET_DISPLAY_STATE,
        TRANS_ID_GET_DISPLAY_STATE,
        TRANS_ID_NOTIFY_DISPLAY_EVENT,
        TRANS_ID_CREATE_VIRTUAL_SCREEN = 1000,
        TRANS_ID_DESTROY_VIRTUAL_SCREEN,
        TRANS_ID_SCREENGROUP_BASE = 1100,
        TRANS_ID_SCREEN_MAKE_MIRROR = TRANS_ID_SCREENGROUP_BASE,
    };

    virtual DisplayId GetDefaultDisplayId() = 0;
    virtual DisplayInfo GetDisplayInfoById(DisplayId displayId) = 0;

    virtual ScreenId CreateVirtualScreen(VirtualScreenOption option) = 0;
    virtual DMError DestroyVirtualScreen(ScreenId screenId) = 0;
    virtual std::shared_ptr<Media::PixelMap> GetDispalySnapshot(DisplayId displayId) = 0;

    virtual bool RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) = 0;
    virtual bool UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) = 0;
    virtual bool WakeUpBegin(PowerStateChangeReason reason) = 0;
    virtual bool WakeUpEnd() = 0;
    virtual bool SuspendBegin(PowerStateChangeReason reason) = 0;
    virtual bool SuspendEnd() = 0;
    virtual bool SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason) = 0;
    virtual bool SetDisplayState(DisplayState state) = 0;
    virtual DisplayState GetDisplayState(uint64_t displayId) = 0;
    virtual void NotifyDisplayEvent(DisplayEvent event) = 0;
    virtual DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId) = 0;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_INTERFACE_H