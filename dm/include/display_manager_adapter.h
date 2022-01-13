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
#include <surface.h>

#include "display.h"
#include "display_manager_interface.h"
#include "singleton_delegator.h"

namespace OHOS::Rosen {
class DMSDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class DisplayManagerAdapter {
WM_DECLARE_SINGLE_INSTANCE_BASE(DisplayManagerAdapter);
public:
    DisplayId GetDefaultDisplayId();
    sptr<Display> GetDisplayById(DisplayId displayId);
    DisplayId CreateVirtualDisplay(const VirtualDisplayInfo &virtualDisplayInfo,
        sptr<Surface> surface);
    bool DestroyVirtualDisplay(DisplayId displayId);
    std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId displayId);

    bool SuspendBegin(PowerStateChangeReason reason);
    bool SetDisplayState(DisplayState state, DisplayStateCallback callback);
    DisplayState GetDisplayState(uint64_t displayId);
    void NotifyDisplayEvent(DisplayEvent event);

    void Clear();

private:
    DisplayManagerAdapter() = default;
    ~DisplayManagerAdapter() = default;
    bool InitDMSProxyLocked();
    void NotifyDisplayChange(DisplayState state);

    static inline SingletonDelegator<DisplayManagerAdapter> delegator;

    std::mutex mutex_;
    sptr<IDisplayManager> displayManagerServiceProxy_ = nullptr;
    sptr<DMSDeathRecipient> dmsDeath_ = nullptr;
    std::map<DisplayId, sptr<Display>> displayMap_;
    DisplayId defaultDisplayId_;
    DisplayStateCallback callback_;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DM_DISPLAY_MANAGER_ADAPTER_H
