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

#include "display.h"
#include "dm_common.h"
#include "wm_single_instance.h"
#include "screenshot_info.h"

namespace OHOS::Rosen {
class DisplayManager {
WM_DECLARE_SINGLE_INSTANCE_BASE(DisplayManager);
friend class DMSDeathRecipient;
public:
    class IDisplayListener : public virtual RefBase {
    public:
        /**
         * @brief Notify when a new display is created.
         */
        virtual void OnCreate(DisplayId) = 0;

        /**
         * @brief Notify when the display is destroyed.
         */
        virtual void OnDestroy(DisplayId) = 0;

        /**
         * @brief Notify when the state of a display changes
         */
        virtual void OnChange(DisplayId) = 0;
    };

    class IScreenshotListener : public virtual RefBase {
    public:
        /**
         * @brief Notify when a screenshot event occurs.
         */
        virtual void OnScreenshot(const ScreenshotInfo info) {}
    };

    class IPrivateWindowListener : public virtual RefBase {
    public:
        /**
         * @brief Monitor whether the existence of privacy window has changed
         */
        virtual void OnPrivateWindow(bool hasPrivate) {};
    };

    /**
     * @brief Obtain the All Displays object
     */
    std::vector<sptr<Display>> GetAllDisplays();

    /**
     * @brief Obtain the Id of the default display.
     */
    DisplayId GetDefaultDisplayId();

    /**
     * @brief Get the Default Display object.
     */
    sptr<Display> GetDefaultDisplay();

    /**
     * @brief Get the Default Display object by means of sync.
     */
    sptr<Display> GetDefaultDisplaySync();

    /**
     * @brief Get the Display object By Id.
     *
     * @param displayId Id of the target display.
     */
    sptr<Display> GetDisplayById(DisplayId displayId);

    /**
     * @brief Get the Display Object By corresponding screenId.
     *
     * @param screenId Id the target screen.
     */
    sptr<Display> GetDisplayByScreen(ScreenId screenId);

    /**
     * @brief Get the All Displays' Id.
     */
    std::vector<DisplayId> GetAllDisplayIds();

    /**
     * @brief Get whether the target display contains privacy windows.
     */
    DMError HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow);

    /**
     * @brief Get screenshot of the target display.
     */
    std::shared_ptr<Media::PixelMap> GetScreenshot(DisplayId displayId);
    std::shared_ptr<Media::PixelMap> GetScreenshot(DisplayId displayId, const Media::Rect &rect,
                                        const Media::Size &size, int rotation);

    /**
     * @brief Begin to wake screen.
     */
    bool WakeUpBegin(PowerStateChangeReason reason);

    /**
     * @brief Wake up screen end.
     */
    bool WakeUpEnd();

    /**
     * @brief Begin to suspend the screen.
     */
    bool SuspendBegin(PowerStateChangeReason reason);

    /**
     * @brief Suspend screen end.
     *
     * @return true
     * @return false
     */
    bool SuspendEnd();

    /**
     * @brief Set the Display State object
     */
    bool SetDisplayState(DisplayState state, DisplayStateCallback callback);

    /**
     * @brief Get the state of the target display.
     */
    DisplayState GetDisplayState(DisplayId displayId);

    /**
     * @brief Set the brightness level of the target screen.
     *
     * @param screenId Target screen.
     * @param level Brightness level.
     */
    bool SetScreenBrightness(uint64_t screenId, uint32_t level);

    /**
     * @brief Get the brightness level of the target screen.
     */
    uint32_t GetScreenBrightness(uint64_t screenId) const;

    /**
     * @brief Notify when events of certain types occur.
     */
    void NotifyDisplayEvent(DisplayEvent event);

    /**
     * @brief Freeze target displays.
     */
    bool Freeze(std::vector<DisplayId> displayIds);

    /**
     * @brief Unfreeze target displays.
     */
    bool Unfreeze(std::vector<DisplayId> displayIds);

    /**
     * @brief Resgister a display listener.
     */
    DMError RegisterDisplayListener(sptr<IDisplayListener> listener);

    /**
     * @brief Unregister an existed display listener.
     */
    DMError UnregisterDisplayListener(sptr<IDisplayListener> listener);

    /**
     * @brief Register a listener for display power events.
     */
    DMError RegisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener);

    /**
     * @brief Unregiste an existed listener for display power events.
     */
    DMError UnregisterDisplayPowerEventListener(sptr<IDisplayPowerEventListener> listener);

    /**
     * @brief Register a listener for screenshot event.
     */
    DMError RegisterScreenshotListener(sptr<IScreenshotListener> listener);

    /**
     * @brief Unregister an existed listener for screenshot event.
     */
    DMError UnregisterScreenshotListener(sptr<IScreenshotListener> listener);

    /**
     * @brief Register a listener for the event of private window.
     */
    DMError RegisterPrivateWindowListener(sptr<IPrivateWindowListener> listener);

    /**
     * @brief Unregister an existed listener for the event of private window.
     */
    DMError UnregisterPrivateWindowListener(sptr<IPrivateWindowListener> listener);

    /**
     * @brief Add a surface node to the target display.
     *
     * @param displayId Target display.
     * @param surfaceNode SurfaceNode object.
     */
    DMError AddSurfaceNodeToDisplay(DisplayId displayId, std::shared_ptr<class RSSurfaceNode>& surfaceNode);
    
    /**
     * @brief Remove a surface node from the target display.
     *
     * @param displayId Target display.
     * @param surfaceNode SurfaceNode object.
     */
    DMError RemoveSurfaceNodeFromDisplay(DisplayId displayId, std::shared_ptr<class RSSurfaceNode>& surfaceNode);

    constexpr static int32_t MAX_RESOLUTION_SIZE_SCREENSHOT = 3840; // max resolution, 4K

private:
    DisplayManager();
    ~DisplayManager();
    void OnRemoteDied();

    class Impl;
    std::recursive_mutex mutex_;
    bool destroyed_ = false;
    sptr<Impl> pImpl_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DM_DISPLAY_MANAGER_H