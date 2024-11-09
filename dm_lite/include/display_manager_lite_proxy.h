/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_DISPLAY_MANAGER_LITE_PROXY_H
#define OHOS_ROSEN_DISPLAY_MANAGER_LITE_PROXY_H

#include "display_manager_interface_code.h"

#include <iremote_broker.h>
#include <iremote_proxy.h>
#include <cinttypes>
#include "dm_common.h"
#include "zidl/display_manager_agent_interface.h"
#include "display_info.h"
#include "fold_screen_info.h"
#include "screen_group_info.h"

namespace OHOS {
namespace Rosen {

class DisplayManagerLiteProxy : public IRemoteBroker {
public:
#ifdef SCENE_BOARD_ENABLED
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IScreenSessionManager");
#else
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IDisplayManager");
#endif

    explicit DisplayManagerLiteProxy(const sptr<IRemoteObject>& impl) : remoteObject(impl) {}

    ~DisplayManagerLiteProxy() = default;

    sptr<IRemoteObject> AsObject() { return remoteObject; };
    virtual DMError RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type);
    virtual DMError UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type);

    virtual FoldDisplayMode GetFoldDisplayMode();
    virtual void SetFoldDisplayMode(const FoldDisplayMode displayMode);
    virtual bool IsFoldable();
    FoldStatus GetFoldStatus();
    virtual sptr<DisplayInfo> GetDefaultDisplayInfo();
    virtual sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId);
    virtual sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId);
    virtual VirtualScreenFlag GetVirtualScreenFlag(ScreenId screenId);
    /*
     * used by powermgr
     */
    virtual bool WakeUpBegin(PowerStateChangeReason reason);
    virtual bool WakeUpEnd();
    virtual bool SuspendBegin(PowerStateChangeReason reason);
    virtual bool SuspendEnd();
    virtual ScreenId GetInternalScreenId();
    virtual bool SetScreenPowerById(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason);
    virtual bool SetSpecifiedScreenPower(ScreenId, ScreenPowerState, PowerStateChangeReason);
    virtual bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason);
    virtual ScreenPowerState GetScreenPower(ScreenId dmsScreenId);
    virtual bool SetDisplayState(DisplayState state);
    virtual DisplayState GetDisplayState(DisplayId displayId);
    virtual bool TryToCancelScreenOff();
    virtual bool SetScreenBrightness(uint64_t screenId, uint32_t level);
    virtual uint32_t GetScreenBrightness(uint64_t screenId);
    virtual std::vector<DisplayId> GetAllDisplayIds();
private:
    sptr<IRemoteObject> Remote() { return remoteObject; };
    sptr<IRemoteObject> remoteObject = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_SCREEN_SESSION_MANAGER_LITE_PROXY_H