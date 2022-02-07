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

#ifndef FOUNDATION_DM_DISPLAY_MANAGER_ADAPTER_H
#define FOUNDATION_DM_DISPLAY_MANAGER_ADAPTER_H

#include <map>
#include <mutex>
#include <surface.h>

#include "display.h"
#include "screen.h"
#include "screen_group.h"
#include "dm_common.h"
#include "display_manager_interface.h"
#include "singleton_delegator.h"

namespace OHOS::Rosen {
class DMSDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class DisplayManagerAdapter {
WM_DECLARE_SINGLE_INSTANCE(DisplayManagerAdapter);
public:
    virtual DisplayId GetDefaultDisplayId();
    virtual sptr<Display> GetDisplayById(DisplayId displayId);

    virtual ScreenId CreateVirtualScreen(VirtualScreenOption option);
    virtual DMError DestroyVirtualScreen(ScreenId screenId);
    virtual std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId displayId);

    virtual bool RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type);
    virtual bool UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type);
    virtual bool WakeUpBegin(PowerStateChangeReason reason);
    virtual bool WakeUpEnd();
    virtual bool SuspendBegin(PowerStateChangeReason reason);
    virtual bool SuspendEnd();
    virtual bool SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason);
    virtual bool SetDisplayState(DisplayState state);
    virtual DisplayState GetDisplayState(DisplayId displayId);
    virtual void NotifyDisplayEvent(DisplayEvent event);
    virtual DMError MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId);
    virtual void Clear();
    virtual sptr<Screen> GetScreenById(ScreenId screenId);
    virtual sptr<ScreenGroup> GetScreenGroupById(ScreenId screenId);
    virtual std::vector<sptr<Screen>> GetAllScreens();
    virtual DMError MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint);
    virtual bool SetScreenActiveMode(ScreenId screenId, uint32_t modeId);

private:
    bool InitDMSProxyLocked();

    static inline SingletonDelegator<DisplayManagerAdapter> delegator;

    std::recursive_mutex mutex_;
    sptr<IDisplayManager> displayManagerServiceProxy_ = nullptr;
    sptr<DMSDeathRecipient> dmsDeath_ = nullptr;
    std::map<DisplayId, sptr<Display>> displayMap_;
    std::map<ScreenId, sptr<Screen>> screenMap_;
    std::map<ScreenId, sptr<ScreenGroup>> screenGroupMap_;
    DisplayId defaultDisplayId_;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DM_DISPLAY_MANAGER_ADAPTER_H
