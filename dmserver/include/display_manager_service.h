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

#ifndef FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H
#define FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H

#include <map>
#include <mutex>

#include <system_ability.h>
#include <surface.h>

#include "abstract_display.h"
#include "abstract_display_controller.h"
#include "abstract_screen_controller.h"
#include "display_manager_stub.h"
#include "display_power_controller.h"
#include "wm_single_instance.h"
#include "singleton_delegator.h"

namespace OHOS::Rosen {
class DMAgentDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    DMAgentDeathRecipient(std::function<void(sptr<IRemoteObject>&)> callback) : callback_(callback) {}
    ~DMAgentDeathRecipient() = default;

    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;

private:
    std::function<void(sptr<IRemoteObject>&)> callback_;
};

class DisplayManagerService : public SystemAbility, public DisplayManagerStub {
DECLARE_SYSTEM_ABILITY(DisplayManagerService);

WM_DECLARE_SINGLE_INSTANCE_BASE(DisplayManagerService);

public:
    void OnStart() override;
    void OnStop() override;
    DisplayId CreateVirtualDisplay(const VirtualDisplayInfo &virtualDisplayInfo,
        sptr<Surface> surface) override;
    bool DestroyVirtualDisplay(DisplayId displayId) override;

    DisplayId GetDefaultDisplayId() override;
    DisplayInfo GetDisplayInfoById(DisplayId displayId) override;
    std::shared_ptr<Media::PixelMap> GetDispalySnapshot(DisplayId displayId) override;

    void RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;
    void UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;
    bool WakeUpBegin(PowerStateChangeReason reason) override;
    bool WakeUpEnd() override;
    bool SuspendBegin(PowerStateChangeReason reason) override;
    bool SuspendEnd() override;
    bool SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason) override;
    bool SetDisplayState(DisplayState state) override;
    DisplayState GetDisplayState(uint64_t displayId) override;
    void NotifyDisplayEvent(DisplayEvent event) override;
    bool NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status);

    sptr<AbstractScreenController> GetAbstractScreenController();

private:
    DisplayManagerService();
    ~DisplayManagerService() = default;
    bool Init();
    DisplayId GetDisplayIdFromScreenId(ScreenId screenId);
    ScreenId GetScreenIdFromDisplayId(DisplayId displayId);
    void RemoveDisplayManagerAgent(const sptr<IRemoteObject>& remoteObject);
    bool UnregisterDisplayManagerAgent(std::vector<sptr<IDisplayManagerAgent>>& displayManagerAgents,
        const sptr<IRemoteObject>& displayManagerAgent);

    struct finder_t {
        finder_t(sptr<IRemoteObject> remoteObject) : remoteObject_(remoteObject) {}
        bool operator()(sptr<IDisplayManagerAgent> displayManagerAgent)
        {
            return displayManagerAgent->AsObject() == remoteObject_;
        }
        sptr<IRemoteObject> remoteObject_;
    };
    std::recursive_mutex mutex_;
    static inline SingletonDelegator<DisplayManagerService> delegator_;
    std::map<int32_t, sptr<AbstractDisplay>> abstractDisplayMap_;
    sptr<AbstractScreenController> abstractScreenController_;
    sptr<AbstractDisplayController> abstractDisplayController_;
    DisplayPowerController displayPowerController_;
    std::map<DisplayManagerAgentType, std::vector<sptr<IDisplayManagerAgent>> > displayManagerAgentMap_;
    sptr<DMAgentDeathRecipient> dmAgentDeath_ = new DMAgentDeathRecipient(
        std::bind(&DisplayManagerService::RemoveDisplayManagerAgent, this, std::placeholders::_1));
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H