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
#include "picture_in_picture_manager.h"
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
    sptr<PictureInPictureController> thisController = this;

    if (pipOption_ == nullptr || pipOption_->GetContext() == nullptr) {
        WLOGFE("Get PictureInPictureOption failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(pipOption_->GetContext());
    sptr<Window> callWindow = Window::GetTopWindowWithContext(context->lock());
    if (callWindow == nullptr) {
        WLOGFE("Get call Window failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    mainWindowId_ = callWindow->GetWindowId();

    sptr<WindowOption> windowOption = new(std::nothrow) WindowOption();
    if (windowOption == nullptr) {
        WLOGFE("Get WindowOption failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    windowOption->SetWindowName(PIP_WINDOW_NAME);
    windowOption->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    windowOption->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    windowOption->SetWindowRect(windowRect_);
    WMError errCode;
    sptr<Window> window = Window::Create(windowOption->GetWindowName(), windowOption, context->lock(), errCode);
    if (window == nullptr || errCode != WMError::WM_OK) {
        WLOGFE("Window create failed, reason: %{public}d", errCode);
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    window_ = window;
    PictureInPictureManager::PutPipControllerInfo(window_->GetWindowId(), thisController);
    return WMError::WM_OK;
}

WMError PictureInPictureController::ShowPictureInPictureWindow()
{
    WLOGI("ShowPictureInPictureWindow is called");
    if (window_ == nullptr) {
        WLOGFD("window_ is nullptr");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    WMError errCode = window_->Show(0, false);
    if (errCode != WMError::WM_OK) {
        WLOGFD("window_ show failed");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
    PictureInPictureManager::SetCurrentPipController(this);
    return WMError::WM_OK;
}

WMError PictureInPictureController::StartPictureInPicture()
{
    WLOGI("StartPictureInPicture is called");
    sptr<PictureInPictureController> thisController = this;
    if (PictureInPictureManager::GetPipWindowState() == PipWindowState::STATE_STARTING) {
        WLOGFE("Pip window is starting");
        return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    }
    if (PictureInPictureManager::IsCurrentPipControllerExist()) {
        // pip window exists
        if (PictureInPictureManager::IsCurrentPipController(weakRef_)) {
            WLOGFE("Repeat start request");
            return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
        }
        if (PictureInPictureManager::IsAttachedPipWindow(mainWindowId_)) {
            window_ = PictureInPictureManager::GetCurrentWindow();
            PictureInPictureManager::RemoveCurrentPipController();
            PictureInPictureManager::SetCurrentPipController(thisController);
            return ShowPictureInPictureWindow();
        }
        PictureInPictureManager::DoClose(false);
    }
    PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STARTING);
    WMError errCode = CreatePictureInPictureWindow();
    if (errCode != WMError::WM_OK) {
        PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_UNDEFINED);
        WLOGFE("Create pip window failed");
        return errCode;
    }
    errCode = ShowPictureInPictureWindow();
    if (errCode != WMError::WM_OK) {
        PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_UNDEFINED);
        WLOGFE("Show pip window failed");
        return errCode;
    }
    PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STARTED);
    return WMError::WM_OK;
}

WMError PictureInPictureController::StopPictureInPicture(bool needAnim)
{
    WLOGI("StopPictureInPicture is called, needAnim: %{public}u", needAnim);
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    if (PictureInPictureManager::GetPipWindowState() == PipWindowState::STATE_STOPPING) {
        WLOGFE("Repeat stop request");
        return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    }
    PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STOPPING);
    auto task = [this]() {
            if (!window_) {
                WLOGFE("StopPictureInPicture failed, window is null");
                return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
            }
            if (window_->Destroy() != WMError::WM_OK) {
                PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_UNDEFINED);
                WLOGFE("Window destroy failed");
                return WMError::WM_ERROR_PIP_DESTROY_FAILED;
            }
            PictureInPictureManager::RemoveCurrentPipController();
            PictureInPictureManager::RemovePipControllerInfo(window_->GetWindowId());
            window_ = nullptr;
            PictureInPictureManager::SetPipWindowState(PipWindowState::STATE_STOPPED);
            return WSError::WS_OK;
        };
    if (handler_ && needAnim) {
        WLOGFD("Window destroy async");
        handler_->PostTask(task, "StopPictureInPicture", DEFAULT_TIME_DELAY);
    } else {
        WLOGFD("Window destroy sync");
        task();
    }
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