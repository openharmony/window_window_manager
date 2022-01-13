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

    bool SuspendBegin(PowerStateChangeReason reason) override;
    bool SetDisplayState(DisplayState state) override;
    DisplayState GetDisplayState(uint64_t displayId) override;
    void NotifyDisplayEvent(DisplayEvent event) override;

private:
    DisplayManagerService();
    ~DisplayManagerService() = default;
    bool Init();
    DisplayId GetDisplayIdFromScreenId(ScreenId screenId);
    ScreenId GetScreenIdFromDisplayId(DisplayId displayId);

    std::recursive_mutex mutex_;
    static inline SingletonDelegator<DisplayManagerService> delegator_;
    std::map<int32_t, sptr<AbstractDisplay>> abstractDisplayMap_;
    AbstractScreenController screenController_ = AbstractScreenController(mutex_);
    DisplayPowerController displayPowerController_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H