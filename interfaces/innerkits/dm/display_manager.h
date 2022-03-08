/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DM_DISPLAY_MANAGER_H
#define FOUNDATION_DM_DISPLAY_MANAGER_H

#include <vector>
#include <mutex>
#include <pixel_map.h>
#include <surface.h>

#include "display.h"
#include "dm_common.h"
#include "wm_single_instance.h"

namespace OHOS::Rosen {
class DisplayManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(DisplayManager);
public:
    class IDisplayListener : public virtual RefBase {
    public:
        virtual void OnCreate(DisplayId) = 0;
        virtual void OnDestroy(DisplayId) = 0;
        virtual void OnChange(DisplayId) = 0;
    };

    std::vector<sptr<Display>> GetAllDisplays();
    DisplayId GetDefaultDisplayId();
    sptr<Display> GetDefaultDisplay();
    sptr<Display> GetDisplayById(DisplayId displayId);
    sptr<Display> GetDisplayByScreen(ScreenId screenId);
    std::vector<DisplayId> GetAllDisplayIds();
    bool RegisterDisplayListener(sptr<IDisplayListener> listener);
    bool UnregisterDisplayListener(sptr<IDisplayListener> listener);

    std::shared_ptr<Media::PixelMap> GetScreenshot(DisplayId displayId);
    std::shared_ptr<Media::PixelMap> GetScreenshot(DisplayId displayId, const Media::Rect &rect,
                                        const Media::Size &size, int rotation);

    bool RegisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener);
    bool UnregisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener);
    bool WakeUpBegin(PowerStateChangeReason reason);
    bool WakeUpEnd();
    bool SuspendBegin(PowerStateChangeReason reason);
    bool SuspendEnd();
    bool SetDisplayState(DisplayState state, DisplayStateCallback callback);
    DisplayState GetDisplayState(DisplayId displayId);
    bool SetScreenBrightness(uint64_t screenId, uint32_t level);
    uint32_t GetScreenBrightness(uint64_t screenId) const;
    void NotifyDisplayEvent(DisplayEvent event);
    bool Freeze(std::vector<DisplayId> displayIds);
    bool Unfreeze(std::vector<DisplayId> displayIds);
    constexpr static int32_t MAX_RESOLUTION_SIZE_SCREENSHOT = 3840; // max resolution, 4K

private:
    DisplayManager();
    ~DisplayManager();

    class Impl;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_DISPLAY_MANAGER_H