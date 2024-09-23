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
#include "errors.h"

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

static std::map<std::string, PiPControlType> CONTROL_TYPE_MAP = {
    {"playbackStateChanged", PiPControlType::VIDEO_PLAY_PAUSE},
    {"nextVideo", PiPControlType::VIDEO_NEXT},
    {"previousVideo", PiPControlType::VIDEO_PREVIOUS},
    {"hangUp", PiPControlType::HANG_UP_BUTTON},
    {"micStateChanged", PiPControlType::MICROPHONE_SWITCH},
    {"videoStateChanged", PiPControlType::CAMERA_SWITCH},
    {"voiceStateChanged", PiPControlType::MUTE_SWITCH},
    {"fastForward", PiPControlType::FAST_FORWARD},
    {"fastBackward", PiPControlType::FAST_BACKWARD}
};

using namespace Ace;
class PictureInPictureController : virtual public RefBase {
public:
    PictureInPictureController(sptr<PipOption> pipOption, sptr<Window> mainWindow, uint32_t mainWindowId, napi_env env);
    ~PictureInPictureController();
    WMError StartPictureInPicture(StartPipType startType);
    WMError StopPictureInPicture(bool destroyWindow, StopPipType stopPipType, bool withAnim = true);
    WMError StopPictureInPictureFromClient();
    WMError DestroyPictureInPictureWindow();
    sptr<Window> GetPipWindow() const;
    uint32_t GetMainWindowId();
    void SetPipWindow(sptr<Window> window);
    void SetAutoStartEnabled(bool enable);
    void IsAutoStartEnabled(bool& enable) const;
    void UpdateContentSize(int32_t width, int32_t height);
    void UpdatePiPControlStatus(PiPControlType controlType, PiPControlStatus status);
    bool IsContentSizeChanged(float width, float height, float posX, float posY);
    void DoActionEvent(const std::string& actionName, int32_t status);
    void DoControlEvent(PiPControlType controlType, PiPControlStatus status);
    void PreRestorePictureInPicture();
    void RestorePictureInPictureWindow();
    void PrepareSource();
    void LocateSource();
    WMError RegisterPiPLifecycle(const sptr<IPiPLifeCycle>& listener);
    WMError RegisterPiPActionObserver(const sptr<IPiPActionObserver>& listener);
    WMError RegisterPiPControlObserver(const sptr<IPiPControlObserver>& listener);
    WMError UnregisterPiPLifecycle(const sptr<IPiPLifeCycle>& listener);
    WMError UnregisterPiPActionObserver(const sptr<IPiPActionObserver>& listener);
    WMError UnregisterPiPControlObserver(const sptr<IPiPControlObserver>& listener);
    sptr<IPiPLifeCycle> GetPictureInPictureLifecycle() const;
    sptr<IPiPActionObserver> GetPictureInPictureActionObserver() const;
    sptr<IPiPControlObserver> GetPictureInPictureControlObserver() const;
    WMError SetXComponentController(std::shared_ptr<XComponentController> xComponentController);
    PiPWindowState GetControllerState();
    std::string GetPiPNavigationId();
    napi_ref GetCustomNodeController();
    napi_ref GetTypeNode() const;
    void OnPictureInPictureStart();
    bool IsTypeNodeEnabled() const;

private:
    class WindowLifeCycleListener : public IWindowLifeCycle {
    public:
        void AfterDestroyed() override;
    };

private:
    uint32_t GetPipPriority(uint32_t pipTemplateType);
    WMError CreatePictureInPictureWindow(StartPipType startType);
    WMError ShowPictureInPictureWindow(StartPipType startType);
    WMError StartPictureInPictureInner(StartPipType startType);
    WMError StopPictureInPictureInner(StopPipType stopType, bool withAnim);
    void UpdateWinRectByComponent();
    void UpdatePiPSourceRect() const;
    void ResetExtController();
    bool IsPullPiPAndHandleNavigation();
    template<typename T> WMError RegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    template<typename T> WMError UnregisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener);
    wptr<PictureInPictureController> weakRef_ = nullptr;
    sptr<PipOption> pipOption_ = nullptr;
    std::vector<sptr<IPiPLifeCycle>> pipLifeCycleListeners_;
    std::vector<sptr<IPiPActionObserver>> pipActionObservers_;
    std::vector<sptr<IPiPControlObserver>> pipControlObservers_;
    sptr<Window> window_ = nullptr;
    sptr<Window> mainWindow_ = nullptr;
    sptr<IWindowLifeCycle> mainWindowLifeCycleListener_ = nullptr;
    uint32_t mainWindowId_ = 0;
    Rect windowRect_ = {0, 0, 0, 0};
    bool isAutoStartEnabled_ = false;
    PiPWindowState curState_ = PiPWindowState::STATE_UNDEFINED;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    std::shared_ptr<XComponentController> pipXComponentController_ = nullptr;
    std::shared_ptr<XComponentController> mainWindowXComponentController_ = nullptr;
    napi_env env_ = nullptr;
    std::mutex mutex_;
    int32_t handleId_ = -1;
    bool isStoppedFromClient_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_PICTURE_IN_PICTURE_CONTROLLER_H
