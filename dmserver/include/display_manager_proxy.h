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

#ifndef FOUNDATION_DMSERVER_DISPLAY_MANAGER_PROXY_H
#define FOUNDATION_DMSERVER_DISPLAY_MANAGER_PROXY_H

#include "display_manager_interface.h"

#include <iremote_proxy.h>

namespace OHOS::Rosen {
class DisplayManagerProxy : public IRemoteProxy<IDisplayManager> {
public:
    explicit DisplayManagerProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<IDisplayManager>(impl) {};
    ~DisplayManagerProxy() {};

    DisplayId GetDefaultDisplayId() override;
    DisplayInfo GetDisplayInfoById(DisplayId displayId) override;

    DisplayId CreateVirtualDisplay(const VirtualDisplayInfo &virtualDisplayInfo,
        sptr<Surface> surface) override;
    bool DestroyVirtualDisplay(DisplayId displayId) override;
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

private:
    static inline BrokerDelegator<DisplayManagerProxy> delegator_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_PROXY_H