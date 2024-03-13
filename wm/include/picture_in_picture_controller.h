/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_PICTURE_IN_PICTURE_CONTROLLER_H
#define OHOS_PICTURE_IN_PICTURE_CONTROLLER_H

#define PIP_WINDOW_NAME "pip_window"

#include <event_handler.h>
#include <refbase.h>
#include <mutex>
#include <ability_context.h>
#include "picture_in_picture_option.h"
#include "window.h"
#include "wm_common.h"
#include "picture_in_picture_interface.h"
#include "xcomponent_controller.h"
#include "pip_report.h"
#include "navigation_controller.h"
#include "display_manager.h"

namespace OHOS {
namespace Rosen {

enum class StartPipType : uint32_t {
    NULL_START = 0,
    USER_START = 1,
    AUTO_START = 2,
    ONE_STEP_START = 3,
};

enum class StopPipType : uint32_t {
    NULL_STOP = 0,
    USER_STOP = 1,
    OTHER_PACKAGE_STOP = 2,
    PACKAGE_STOP = 3,
};

using namespace Ace;
class PictureInPictureController : virtual public RefBase {
public:
    PictureInPictureController(sptr<PipOption> pipOption, sptr<Window> mainWindow, uint32_t mainWindowId, napi_env env);
    ~PictureInPictureController();
    WMError StartPictureInPicture(StartPipType startType);
    WMError StopPictureInPicture(bool destroyWindow, StopPipType stopPipType);
    WMError StopPictureInPictureFromClient();
    sptr<Window> GetPipWindow();
    uint32_t GetMainWindowId();
    void SetPipWindow(sptr<Window> window);
    void SetAutoStartEnabled(bool enable);
    void IsAutoStartEnabled(bool& enable) const;
    void UpdateContentSize(int32_t width, int32_t height);
    void StartMove();
    void DoScale();
    void DoActionEvent(const std::string& actionName, int32_t status);
    void RestorePictureInPictureWindow();
    void SetPictureInPictureLifecycle(sptr<IPiPLifeCycle> listener);
    void SetPictureInPictureActionObserver(sptr<IPiPActionObserver> listener);
    sptr<IPiPLifeCycle> GetPictureInPictureLifecycle() const;
    sptr<IPiPActionObserver> GetPictureInPictureActionObserver() const;
    WMError SetXComponentController(std::shared_ptr<XComponentController> xComponentController);
    PiPWindowState GetControllerState();
    std::string GetPiPNavigationId();

    class PipMainWindowLifeCycleImpl : public Rosen::IWindowLifeCycle {
    public:
        PipMainWindowLifeCycleImpl(const std::string& navigationId)
        {
            navigationId_ = navigationId;
        };
        ~PipMainWindowLifeCycleImpl() {};
        void AfterBackground() override;
        void BackgroundFailed(int32_t type) override;
    private:
        std::string navigationId_ = "";
    };

private:
    WMError CreatePictureInPictureWindow();
    WMError ShowPictureInPictureWindow(StartPipType startType);
    WMError StartPictureInPictureInner(StartPipType startType);
    WMError StopPictureInPictureInner(StopPipType stopType);
    void UpdateXComponentPositionAndSize();
    void ResetExtController();
    bool IsPullPiPAndHandleNavigation();
    wptr<PictureInPictureController> weakRef_ = nullptr;
    sptr<PipOption> pipOption_;
    sptr<IPiPLifeCycle> pipLifeCycleListener_;
    sptr<IPiPActionObserver> pipActionObserver_;
    sptr<Window> window_;
    sptr<Window> mainWindow_;
    uint32_t mainWindowId_;
    Rect windowRect_ = {0, 0, 0, 0};
    bool isAutoStartEnabled_ = false;
    PiPWindowState curState_ = PiPWindowState::STATE_UNDEFINED;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::shared_ptr<XComponentController> pipXComponentController_;
    std::shared_ptr<XComponentController> mainWindowXComponentController_;
    napi_env env_;
    std::mutex mutex_;
    int32_t handleId_ = -1;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_PICTURE_IN_PICTURE_CONTROLLER_H
