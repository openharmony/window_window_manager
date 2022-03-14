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

#ifndef FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_CONTROLLER_H
#define FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_CONTROLLER_H

#include "abstract_screen_controller.h"

#include <map>
#include <pixel_map.h>
#include <surface.h>
#include <transaction/rs_interfaces.h>

#include "screen.h"
#include "abstract_display.h"
#include "display_change_listener.h"
#include "transaction/rs_interfaces.h"
#include "future.h"

namespace OHOS::Rosen {
class AbstractDisplayController : public RefBase {
using DisplayStateChangeListener = std::function<void(DisplayId, DisplayStateChangeType)>;
public:
    AbstractDisplayController(std::recursive_mutex& mutex, DisplayStateChangeListener);
    ~AbstractDisplayController();
    WM_DISALLOW_COPY_AND_MOVE(AbstractDisplayController);

    void Init(sptr<AbstractScreenController> abstractScreenController);
    ScreenId GetDefaultScreenId();
    RSScreenModeInfo GetScreenActiveMode(ScreenId id);

    std::shared_ptr<Media::PixelMap> GetScreenSnapshot(DisplayId displayId);
    sptr<AbstractDisplay> GetAbstractDisplay(DisplayId displayId) const;
    sptr<AbstractDisplay> GetAbstractDisplayByScreen(ScreenId screenId) const;
    std::vector<DisplayId> GetAllDisplayIds() const;
    void SetFreeze(std::vector<DisplayId> displayIds, bool isFreeze);

private:
    void OnAbstractScreenConnect(sptr<AbstractScreen> absScreen);
    void OnAbstractScreenDisconnect(sptr<AbstractScreen> absScreen);
    void OnAbstractScreenChange(sptr<AbstractScreen> absScreen, DisplayChangeEvent event);
    void ProcessDisplayUpdateOrientation(sptr<AbstractScreen> absScreen);
    void ProcessDisplaySizeChange(sptr<AbstractScreen> absScreen);
    void BindAloneScreenLocked(sptr<AbstractScreen> absScreen);
    void AddScreenToMirrorLocked(sptr<AbstractScreen> absScreen);
    void AddScreenToExpandLocked(sptr<AbstractScreen> absScreen);
    DisplayId ProcessNormalScreenDisconnected(sptr<AbstractScreen> absScreen, sptr<AbstractScreenGroup> screenGroup);
    DisplayId ProcessExpandScreenDisconnected(sptr<AbstractScreen> absScreen, sptr<AbstractScreenGroup> screenGroup);
    bool UpdateDisplaySize(sptr<AbstractDisplay> absDisplay, sptr<SupportedScreenModes> info);

    std::recursive_mutex& mutex_;
    std::atomic<DisplayId> displayCount_ { 0 };
    sptr<AbstractDisplay> dummyDisplay_;
    std::map<DisplayId, sptr<AbstractDisplay>> abstractDisplayMap_;
    sptr<AbstractScreenController> abstractScreenController_;
    sptr<AbstractScreenController::AbstractScreenCallback> abstractScreenCallback_;
    OHOS::Rosen::RSInterfaces& rsInterface_;
    DisplayStateChangeListener displayStateChangeListener_;

    class ScreenshotCallback : public SurfaceCaptureCallback, public Future<std::shared_ptr<Media::PixelMap>> {
    public:
        ScreenshotCallback() = default;
        ~ScreenshotCallback() {};
        void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
        {
            FutureCall(pixelmap);
        }

    protected:
        void Call(std::shared_ptr<Media::PixelMap> pixelmap) override
        {
            if (!flag_) {
                flag_ = true;
                pixelMap_ = pixelmap;
            }
        }
        bool IsReady() override
        {
            return flag_;
        }
        std::shared_ptr<Media::PixelMap> FetchResult() override
        {
            return pixelMap_;
        }
    private:
        bool flag_ = false;
        std::shared_ptr<Media::PixelMap> pixelMap_ = nullptr;
    };
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_DISPLAY_CONTROLLER_H