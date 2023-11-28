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
#include "parameters.h"
#include <power_mgr_client.h>
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
    constexpr int32_t DELAY_ANIM = 500;
    const std::string PROP_DEFAULT_PAGE_NAME = "const.window.pip.debug.useDefaultPage";
    const std::string DEFAULT_PAGE_PATH = "pages/pipwindow/PiPWindow";
}

PictureInPictureController::PictureInPictureController(sptr<PipOption> pipOption, uint32_t windowId, napi_env env)
    : weakRef_(this), pipOption_(pipOption), mainWindowId_(windowId), env_(env)
{
    this->handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
    curState_ = PipWindowState::STATE_UNDEFINED;
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
    mainWindow_ = callWindow;
    sptr<WindowOption> windowOption = new(std::nothrow) WindowOption();
    if (windowOption == nullptr) {
        WLOGFE("Get WindowOption failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    mainWindowXComponentController_ = pipOption_->GetXComponentController();
    if (mainWindowXComponentController_ == nullptr || mainWindow_ == nullptr) {
        WLOGFE("mainWindowXComponentController or main window is nullptr");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    UpdateXComponentPositionAndSize();
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
    if (pipLifeCycleListener_ != nullptr) {
        pipLifeCycleListener_->OnPreparePictureInPictureStart();
    }
    bool useDefaultPage = system::GetParameter(PROP_DEFAULT_PAGE_NAME, "") != "";
    if (useDefaultPage && env_ != nullptr) {
        window_->NapiSetUIContent(DEFAULT_PAGE_PATH, env_, nullptr, false, nullptr);
    }
    WMError errCode = window_->Show(0, false);
    if (errCode != WMError::WM_OK) {
        WLOGFD("window_ show failed");
        int32_t err = static_cast<int32_t>(errCode);
        if (pipLifeCycleListener_ != nullptr) {
            pipLifeCycleListener_->OnPictureInPictureOperationError(err);
        }
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
    PictureInPictureManager::SetActiveController(this);
    return WMError::WM_OK;
}

WMError PictureInPictureController::StartPictureInPicture()
{
    WLOGI("StartPictureInPicture is called");
    std::lock_guard<std::mutex> lock(mutex_);
    if (curState_ == PipWindowState::STATE_STARTING || curState_ == PipWindowState::STATE_STARTED) {
        WLOGFW("pip window is starting");
        return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    }
    curState_ = PipWindowState::STATE_STARTING;
    if (PictureInPictureManager::HasActiveController() && !PictureInPictureManager::IsActiveController(weakRef_)) {
        // if current controller is not the active one, but belongs to the same mainWindow, reserve pipWindow
        if (PictureInPictureManager::IsAttachedToSameWindow(mainWindowId_)) {
            window_ = PictureInPictureManager::GetCurrentWindow();
            PictureInPictureManager::DoClose(false, false);
            PictureInPictureManager::PutPipControllerInfo(window_->GetWindowId(), this);
            WMError err = ShowPictureInPictureWindow();
            if (err != WMError::WM_OK) {
                curState_ = PipWindowState::STATE_UNDEFINED;
            } else {
                curState_ = PipWindowState::STATE_STARTED;
            }
            return err;
        }
        // otherwise, stop the previous one
        PictureInPictureManager::DoClose(true, false);
    }
    WMError errCode = CreatePictureInPictureWindow();
    if (errCode != WMError::WM_OK) {
        curState_ = PipWindowState::STATE_UNDEFINED;
        WLOGFE("Create pip window failed");
        return errCode;
    }
    errCode = ShowPictureInPictureWindow();
    if (errCode != WMError::WM_OK) {
        curState_ = PipWindowState::STATE_UNDEFINED;
        WLOGFE("Show pip window failed");
        return errCode;
    }
    curState_ = PipWindowState::STATE_STARTED;
    return WMError::WM_OK;
}

WMError PictureInPictureController::StopPictureInPicture(bool destroyWindow, bool needAnim)
{
    WLOGI("StopPictureInPicture is called, destroyWindow: %{public}u, needAnim: %{public}u", destroyWindow, needAnim);
    std::lock_guard<std::mutex> lock(mutex_);
    if (curState_ == PipWindowState::STATE_STOPPING || curState_ == PipWindowState::STATE_STOPPED) {
        WLOGFE("Repeat stop request");
        return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    }
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    curState_ = PipWindowState::STATE_STOPPING;
    if (pipLifeCycleListener_ != nullptr) {
        pipLifeCycleListener_->OnPreparePictureInPictureStop();
    }
    window_->NotifyPrepareClosePiPWindow();
    if (!destroyWindow) {
        curState_ = PipWindowState::STATE_STOPPED;
        if (pipLifeCycleListener_) {
            pipLifeCycleListener_->OnPictureInPictureStop();
        }
        PictureInPictureManager::RemoveActiveController();
        PictureInPictureManager::RemovePipControllerInfo(window_->GetWindowId());
        return WMError::WM_OK;
    }
    auto task = [weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
        }
        session->ResetExtController();
        WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(session->window_->Destroy());
        if (ret != WmErrorCode::WM_OK) {
            session->curState_ = PipWindowState::STATE_UNDEFINED;
            WLOGFE("Window destroy failed");
            int32_t err = static_cast<int32_t>(ret);
            if (session->pipLifeCycleListener_ != nullptr) {
                session->pipLifeCycleListener_->OnPictureInPictureOperationError(err);
            }
            return WMError::WM_ERROR_PIP_DESTROY_FAILED;
        }
        if (session->pipLifeCycleListener_ != nullptr) {
            session->pipLifeCycleListener_->OnPictureInPictureStop();
        }
        PictureInPictureManager::RemoveActiveController();
        PictureInPictureManager::RemovePipControllerInfo(session->window_->GetWindowId());
        session->window_ = nullptr;
        session->curState_ = PipWindowState::STATE_STOPPED;
        return WMError::WM_OK;
    };
    if (handler_ && needAnim) {
        handler_->PostTask(task, "StopPictureInPicture", DEFAULT_TIME_DELAY);
    } else {
        return task();
    }
    return WMError::WM_OK;
}

sptr<Window> PictureInPictureController::GetPipWindow()
{
    WLOGFD("GetPipWindow is called");
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
    if (isAutoStartEnabled_) {
        PictureInPictureManager::AttachAutoStartController(pipOption_->GetNavigationId(), this);
    } else {
        PictureInPictureManager::DetachAutoStartController(pipOption_->GetNavigationId(), this);
    }
}

void PictureInPictureController::IsAutoStartEnabled(bool& enable) const
{
    enable = isAutoStartEnabled_;
}

PipWindowState PictureInPictureController::GetControllerState() {
    return curState_;
}

void PictureInPictureController::UpdateContentSize(uint32_t width, uint32_t height)
{
    WLOGI("UpdateContentSize is called, state: %{public}u", curState_);
    if (curState_ != PipWindowState::STATE_STARTED) {
        return;
    }
    if (window_ == nullptr) {
        WLOGFE("PiPWindow is not exist");
        return;
    }
    window_->UpdatePiPRect(width, height, PiPRectUpdateReason::REASON_PIP_VIDEO_RATIO_CHANGE);
}

void PictureInPictureController::StartMove()
{
    WLOGI("StartMove is called");
    if (window_ == nullptr) {
        WLOGFE("PiPWindow is not exist");
        return;
    }
    window_->StartMove();
}

void PictureInPictureController::DoScale()
{
    WLOGI("DoScale is called");
    if (window_ == nullptr) {
        WLOGFE("PiPWindow is not exist");
        return;
    }
    window_->UpdatePiPRect(0, 0, PiPRectUpdateReason::REASON_PIP_SCALE_CHANGE);
}

void PictureInPictureController::PipMainWindowLifeCycleImpl::AfterBackground()
{
    WLOGI("PipMainWindowLifeCycleImpl AfterBackground is called");
    if (!PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        WLOGFE("screen is off");
        return;
    }
    PictureInPictureManager::AutoStartPipWindow();
}

void PictureInPictureController::PipMainWindowLifeCycleImpl::BackgroundFailed(int32_t type)
{
    WLOGI("PipMainWindowLifeCycleImpl BackgroundFailed is called");
}

void PictureInPictureController::DoActionEvent(std::string& actionName)
{
    WLOGFD("actionName: %{public}s", actionName.c_str());
    if (pipActionObserver_ == nullptr) {
        WLOGFE("pipActionObserver is not registered");
        return;
    }
    pipActionObserver_->OnActionEvent(actionName);
}

void PictureInPictureController::RestorePictureInPictureWindow()
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return;
    }
    window_->RecoveryPullPiPMainWindow(windowRect_);
    auto stopPipTask = [weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return;
        }
        session->StopPictureInPicture(true, false);
    };
    if (handler_ == nullptr) {
        WLOGFE("handler is nullptr");
        return;
    }
    handler_->PostTask(stopPipTask, DELAY_ANIM);
    WLOGFI("restore pip main window finished");
}

void PictureInPictureController::UpdateXComponentPositionAndSize()
{
    float posX = 0;
    float posY = 0;
    float width = 0;
    float height = 0;
    mainWindowXComponentController_->GetGlobalPosition(posX, posY);
    mainWindowXComponentController_->GetSize(width, height);
    windowRect_.width_ = static_cast<uint32_t>(width);
    windowRect_.height_ = static_cast<uint32_t>(height);
    windowRect_.posX_ = static_cast<uint32_t>(posX);
    windowRect_.posY_ = static_cast<uint32_t>(posY);

    bool isFullScreen = mainWindow_->IsLayoutFullScreen();
    if (!isFullScreen) {
        // calculate status bar height as offset
        WLOGFI("not full screen");
        AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_SYSTEM;
        AvoidArea avoidArea;
        mainWindow_->GetAvoidAreaByType(avoidAreaType, avoidArea);

        uint32_t offset = avoidArea.topRect_.height_;
        windowRect_.posY_ += offset;
        WLOGFD("status bar height = %{public}d", offset);
    }
    WLOGFD("position width: %{public}u, height: %{public}u, posX: %{public}d, posY: %{public}d",
        windowRect_.width_, windowRect_.height_, windowRect_.posX_, windowRect_.posY_);
}

void PictureInPictureController::ResetExtController()
{
    if (mainWindowXComponentController_ != nullptr && pipXComponentController_ != nullptr) {
        XComponentControllerErrorCode errorCode =
                mainWindowXComponentController_->ResetExtController(pipXComponentController_);
        if (errorCode != XComponentControllerErrorCode::XCOMPONENT_CONTROLLER_NO_ERROR) {
            WLOGFE("swap xComponent failed, errorCode: %{public}u", errorCode);
        }
    }
}

WMError PictureInPictureController::SetXComponentController(std::shared_ptr<XComponentController> xComponentController)
{
    pipXComponentController_ = xComponentController;
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    if (mainWindowXComponentController_ == nullptr || pipXComponentController_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    XComponentControllerErrorCode errorCode =
        mainWindowXComponentController_->SetExtController(pipXComponentController_);
    if (errorCode != XComponentControllerErrorCode::XCOMPONENT_CONTROLLER_NO_ERROR) {
        WLOGFE("swap xComponent failed, errorCode: %{public}u", errorCode);
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
    if (pipLifeCycleListener_ != nullptr) {
        pipLifeCycleListener_->OnPictureInPictureStart();
    }
    return WMError::WM_OK;
}

void PictureInPictureController::SetPictureInPictureLifecycle(sptr<IPiPLifeCycle> listener)
{
    WLOGFD("SetPictureInPictureLifecycle is called");
    pipLifeCycleListener_ = listener;
}

void PictureInPictureController::SetPictureInPictureActionObserver(sptr<IPiPActionObserver> listener)
{
    WLOGFD("SetPictureInPictureActionObserver is called");
    pipActionObserver_ = listener;
}

sptr<IPiPLifeCycle> PictureInPictureController::GetPictureInPictureLifecycle() const
{
    return pipLifeCycleListener_;
}

sptr<IPiPActionObserver> PictureInPictureController::GetPictureInPictureActionObserver() const
{
    return pipActionObserver_;
}
} // namespace Rosen
} // namespace OHOS