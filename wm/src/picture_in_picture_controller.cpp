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

#include "picture_in_picture_controller.h"

#include <event_handler.h>
#include <refbase.h>
#include "picture_in_picture_option.h"
#include "window_manager_hilog.h"
#include "window_option.h"
#include "window.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "PictureInPictureController"};
}

PictureInPictureController::PictureInPictureController(sptr<PipOption> pipOption, uint32_t windowId)
    : weakRef_(this), pipOption_(pipOption), mainWindowId_(windowId)
{
    this->handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
}

PictureInPictureController::~PictureInPictureController()
{
}

WMError PictureInPictureController::CreatePictureInPictureWindow()
{
    WLOGI("CreatePictureInPictureWindow is called");
    return WMError::WM_OK;
}

WMError PictureInPictureController::StartPictureInPicture()
{
    WLOGI("StartPictureInPicture is called");
    return WMError::WM_OK;
}

WMError PictureInPictureController::StopPictureInPicture(bool needAnim)
{
    WLOGI("StopPictureInPicture is called, needAnim: %{public}u", needAnim);
    return WMError::WM_OK;
}

sptr<Window> PictureInPictureController::GetPipWindow()
{
    WLOGFD("GetWindow is called");
    return window_;
}

uint32_t PictureInPictureController::GetMainWindowId()
{
    WLOGFD("GetMainWindowId is called");
    return mainWindowId_;
}

void PictureInPictureController::SetPipWindow(sptr<Window> window)
{
    WLOGFD("SetPipWindow is called");
    window_ = window;
}

void PictureInPictureController::SetAutoStartEnabled(bool enable)
{
    isAutoStartEnabled_ = enable;
}

void PictureInPictureController::IsAutoStartEnabled(bool& enable) const
{
    enable = isAutoStartEnabled_;
}

void PictureInPictureController::UpdateContentSize(uint32_t width, uint32_t height)
{
    WLOGI("UpdateDisplaySize is called");
    return;
}
} // namespace Rosen
} // namespace OHOS