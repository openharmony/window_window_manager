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

#ifndef FOUNDATION_DM_DISPLAY_MANAGER_H
#define FOUNDATION_DM_DISPLAY_MANAGER_H

#include <vector>
#include <mutex>
#include <pixel_map.h>
#include <surface.h>

#include "display.h"
#include "dm_common.h"
#include "singleton_delegator.h"
// #include "wm_common.h"


namespace OHOS::Rosen {
class DisplayManagerAdapter;
class DisplayManagerAgent;
class DisplayManager {
friend class DisplayManagerAgent;
WM_DECLARE_SINGLE_INSTANCE_BASE(DisplayManager);
public:
    std::vector<const sptr<Display>> GetAllDisplays();

    DisplayId GetDefaultDisplayId();
    const sptr<Display> GetDefaultDisplay();

    const sptr<Display> GetDisplayById(DisplayId displayId);

    std::vector<DisplayId> GetAllDisplayIds();

    std::shared_ptr<Media::PixelMap> GetScreenshot(DisplayId displayId);
    std::shared_ptr<Media::PixelMap> GetScreenshot(DisplayId displayId, const Media::Rect &rect,
                                        const Media::Size &size, int rotation);

    void RegisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener);
    void UnregisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener);
    bool WakeUpBegin(PowerStateChangeReason reason);
    bool WakeUpEnd();
    bool SuspendBegin(PowerStateChangeReason reason);
    bool SuspendEnd();
    bool SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason);
    DisplayPowerState GetScreenPower(uint64_t screenId);
    bool SetDisplayState(DisplayState state, DisplayStateCallback callback);
    DisplayState GetDisplayState(uint64_t displayId);
    bool SetScreenBrightness(uint64_t screenId, uint32_t level);
    uint32_t GetScreenBrightness(uint64_t screenId) const;
    void NotifyDisplayEvent(DisplayEvent event);

private:
    DisplayManager();
    ~DisplayManager();
    bool CheckRectValid(const Media::Rect &rect, int32_t oriHeight, int32_t oriWidth) const;
    bool CheckSizeValid(const Media::Size &size, int32_t oriHeight, int32_t oriWidth) const;
    void NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status);
    void NotifyDisplayStateChanged(DisplayState state);

    static inline SingletonDelegator<DisplayManager> delegator;
    const int32_t MAX_RESOLUTION_SIZE_SCREENSHOT = 15360; // max resolution, 16K
    std::mutex mutex_;
    std::vector<sptr<IDisplayPowerEventListener>> powerEventListeners_;
    sptr<DisplayManagerAgent> powerEventListenerAgent_;
    sptr<DisplayManagerAgent> displayStateAgent_;
    DisplayStateCallback displayStateCallback_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_DISPLAY_MANAGER_H