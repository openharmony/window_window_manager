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
#include <power_mgr_client.h>
#include "picture_in_picture_manager.h"
#include "picture_in_picture_option.h"
#include "window_manager_hilog.h"
#include "window_option.h"
#include "window.h"
#include "wm_common.h"
#include "singleton_container.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "PictureInPictureController"};
    constexpr int32_t DELAY_ANIM = 500;
    constexpr int32_t SUCCESS = 1;
    constexpr int32_t FAILED = 0;
    constexpr uint32_t PIP_LOW_PRIORITY = 0;
    constexpr uint32_t PIP_HIGH_PRIORITY = 1;
    const std::string VIDEO_PAGE_PATH = "/system/etc/window/resources/pip_video.abc";
    const std::string CALL_PAGE_PATH = "/system/etc/window/resources/pip_call.abc";
    const std::string MEETING_PAGE_PATH = "/system/etc/window/resources/pip_meeting.abc";
    const std::string LIVE_PAGE_PATH = "/system/etc/window/resources/pip_live.abc";
}
static uint32_t GetPipPriority(uint32_t pipTemplateType)
{
    if (pipTemplateType < 0 || pipTemplateType >= static_cast<uint32_t>(PipTemplateType::END)) {
        WLOGFE("param invalid, pipTemplateType is %{public}d", pipTemplateType);
        return PIP_LOW_PRIORITY;
    }
    if (pipTemplateType == static_cast<uint32_t>(PipTemplateType::VIDEO_PLAY) ||
        pipTemplateType == static_cast<uint32_t>(PipTemplateType::VIDEO_LIVE)) {
        return PIP_LOW_PRIORITY;
    } else {
        return PIP_HIGH_PRIORITY;
    }
}

PictureInPictureController::PictureInPictureController(sptr<PipOption> pipOption, sptr<Window> mainWindow,
    uint32_t windowId, napi_env env)
    : weakRef_(this), pipOption_(pipOption), mainWindow_(mainWindow), mainWindowId_(windowId), env_(env)
{
    this->handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
    curState_ = PipWindowState::STATE_UNDEFINED;
    pipDisplayListener_ = new PictureInPictureController::PipDisplayListener(weakRef_);
}

PictureInPictureController::~PictureInPictureController()
{
    PictureInPictureManager::DetachAutoStartController(handleId_, weakRef_);
}

WMError PictureInPictureController::CreatePictureInPictureWindow()
{
    WLOGI("CreatePictureInPictureWindow is called, mainWindow:%{public}u", mainWindowId_);
    sptr<PictureInPictureController> thisController = this;
    if (pipOption_ == nullptr || pipOption_->GetContext() == nullptr) {
        WLOGFE("Get PictureInPictureOption failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(pipOption_->GetContext());
    const std::shared_ptr<AbilityRuntime::Context>& abilityContext = context->lock();
    SingletonContainer::Get<PiPReporter>().SetCurrentPackageName(abilityContext->GetApplicationInfo()->name);
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
    windowOption->SetKeepScreenOn(true);
    WMError errCode;
    PiPTemplateInfo pipTemplateInfo;
    pipTemplateInfo.pipTemplateType = pipOption_->GetPipTemplate();
    pipTemplateInfo.controlGroup = pipOption_->GetControlGroup();
    pipTemplateInfo.priority = GetPipPriority(pipOption_->GetPipTemplate());
    sptr<Window> window = Window::CreatePiP(windowOption, pipTemplateInfo, context->lock(), errCode);
    if (window == nullptr || errCode != WMError::WM_OK) {
        WLOGFE("Window create failed, reason: %{public}d", errCode);
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    window_ = window;
    PictureInPictureManager::PutPipControllerInfo(window_->GetWindowId(), thisController);
    return WMError::WM_OK;
}

WMError PictureInPictureController::ShowPictureInPictureWindow(StartPipType startType)
{
    WLOGD("ShowPictureInPictureWindow is called");
    if (window_ == nullptr) {
        WLOGFD("window_ is nullptr");
        SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
            pipOption_->GetPipTemplate(), FAILED, "window_ is nullptr");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    if (pipLifeCycleListener_ != nullptr) {
        pipLifeCycleListener_->OnPreparePictureInPictureStart();
    }
    if (pipOption_ == nullptr) {
        WLOGFE("Get PictureInPicture option failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    auto pipTemplateType = static_cast<PipTemplateType>(pipOption_->GetPipTemplate());
    WLOGFI("mainWindow: %{public}u, pipWindow: %{public}u, template:%{public}u", mainWindowId_,
        window_->GetWindowId(), pipTemplateType);
    switch (pipTemplateType) {
        default:
        case PipTemplateType::VIDEO_PLAY:
            window_->SetUIContentByAbc(VIDEO_PAGE_PATH, env_, nullptr, nullptr);
            break;
        case PipTemplateType::VIDEO_CALL:
            window_->SetUIContentByAbc(CALL_PAGE_PATH, env_, nullptr, nullptr);
            break;
        case PipTemplateType::VIDEO_MEETING:
            window_->SetUIContentByAbc(MEETING_PAGE_PATH, env_, nullptr, nullptr);
            break;
        case PipTemplateType::VIDEO_LIVE:
            window_->SetUIContentByAbc(LIVE_PAGE_PATH, env_, nullptr, nullptr);
            break;
    }
    WMError errCode = window_->Show(0, false);
    if (errCode != WMError::WM_OK) {
        WLOGFD("window_ show failed, err: %{public}u", errCode);
        int32_t err = static_cast<int32_t>(errCode);
        if (pipLifeCycleListener_ != nullptr) {
            pipLifeCycleListener_->OnPictureInPictureOperationError(err);
        }
        SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
            pipOption_->GetPipTemplate(), FAILED, "window_ show failed");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
    PictureInPictureManager::SetActiveController(this);
    SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
        pipOption_->GetPipTemplate(), SUCCESS, "show pip success");
    return WMError::WM_OK;
}

WMError PictureInPictureController::StartPictureInPicture(StartPipType startType)
{
    WLOGI("StartPictureInPicture called");
    std::lock_guard<std::mutex> lock(mutex_);
    if (curState_ == PipWindowState::STATE_STARTING || curState_ == PipWindowState::STATE_STARTED) {
        WLOGFW("pip window is starting, state: %{public}u, pipWindow: %{public}u, mainWindow: %{public}u",
            curState_, window_->GetWindowId(), mainWindowId_);
        SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
            pipOption_->GetPipTemplate(), FAILED, "Pip window is starting");
        return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    }
    if (pipOption_ == nullptr || pipOption_->GetContext() == nullptr) {
        WLOGFE("Get PictureInPictureOption failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    if (mainWindow_ == nullptr) {
        WLOGFE("Init main window failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    if (!IsPullPiPAndHandleNavigation()) {
        WLOGFE("Navigation operate failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    Rosen::DisplayManager::GetInstance().RegisterDisplayListener(pipDisplayListener_);
    curState_ = PipWindowState::STATE_STARTING;
    if (PictureInPictureManager::HasActiveController() && !PictureInPictureManager::IsActiveController(weakRef_)) {
        // if current controller is not the active one, but belongs to the same mainWindow, reserve pipWindow
        if (PictureInPictureManager::IsAttachedToSameWindow(mainWindowId_)) {
            window_ = PictureInPictureManager::GetCurrentWindow();
            WLOGFD("Reuse pipWindow: %{public}u as attached to the same mainWindow: %{public}u",
                window_->GetWindowId(), mainWindowId_);
            PictureInPictureManager::DoClose(false, false);
            mainWindowXComponentController_ = pipOption_->GetXComponentController();
            UpdateXComponentPositionAndSize();
            UpdateContentSize(windowRect_.width_, windowRect_.height_);
            PictureInPictureManager::PutPipControllerInfo(window_->GetWindowId(), this);
            WMError err = ShowPictureInPictureWindow(startType);
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
    return StartPictureInPictureInner(startType);
}

WMError PictureInPictureController::StartPictureInPictureInner(StartPipType startType)
{
    WMError errCode = CreatePictureInPictureWindow();
    if (errCode != WMError::WM_OK) {
        curState_ = PipWindowState::STATE_UNDEFINED;
        WLOGFE("Create pip window failed, err: %{public}u", errCode);
        SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
            pipOption_->GetPipTemplate(), FAILED, "Create pip window failed");
        return errCode;
    }
    errCode = ShowPictureInPictureWindow(startType);
    if (errCode != WMError::WM_OK) {
        curState_ = PipWindowState::STATE_UNDEFINED;
        WLOGFE("Show pip window failed, err: %{public}u", errCode);
        SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
            pipOption_->GetPipTemplate(), FAILED, "Show pip window failed");
        return errCode;
    }
    curState_ = PipWindowState::STATE_STARTED;
    SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
        pipOption_->GetPipTemplate(), SUCCESS, "start pip success");
    return WMError::WM_OK;
}

WMError PictureInPictureController::StopPictureInPicture(bool destroyWindow, bool needAnim, StopPipType stopPipType)
{
    WLOGI("StopPictureInPicture is called, destroyWindow: %{public}u, needAnim: %{public}u", destroyWindow, needAnim);
    std::lock_guard<std::mutex> lock(mutex_);
    if (curState_ == PipWindowState::STATE_STOPPING || curState_ == PipWindowState::STATE_STOPPED) {
        WLOGFE("Repeat stop request, curState: %{public}u", curState_);
        SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(stopPipType),
            pipOption_->GetPipTemplate(), FAILED, "Repeat stop request");
        return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    }
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(stopPipType),
            pipOption_->GetPipTemplate(), FAILED, "window_ is nullptr");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    curState_ = PipWindowState::STATE_STOPPING;
    if (pipLifeCycleListener_ != nullptr) {
        pipLifeCycleListener_->OnPreparePictureInPictureStop();
    }
    (void)Rosen::DisplayManager::GetInstance().UnregisterDisplayListener(pipDisplayListener_);
    if (!destroyWindow) {
        ResetExtController();
        curState_ = PipWindowState::STATE_STOPPED;
        if (pipLifeCycleListener_) {
            pipLifeCycleListener_->OnPictureInPictureStop();
        }
        PictureInPictureManager::RemoveActiveController(weakRef_);
        PictureInPictureManager::RemovePipControllerInfo(window_->GetWindowId());
        return WMError::WM_OK;
    }
    return StopPictureInPictureInner(needAnim, stopPipType);
}

WMError PictureInPictureController::StopPictureInPictureInner(bool needAnim, StopPipType stopType)
{
    window_->NotifyPrepareClosePiPWindow();
    auto task = [weakThis = wptr(this), currentStopType = stopType, currentPipOption = pipOption_]() {
        auto session = weakThis.promote();
        if (!session || !session->window_) {
            WLOGFE("session is null");
            SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(currentStopType),
                currentPipOption->GetPipTemplate(), FAILED, "session is null");
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
            SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(currentStopType),
                currentPipOption->GetPipTemplate(), FAILED, "Window destroy failed");
            return WMError::WM_ERROR_PIP_DESTROY_FAILED;
        }
        if (session->pipLifeCycleListener_ != nullptr) {
            session->pipLifeCycleListener_->OnPictureInPictureStop();
        }
        PictureInPictureManager::RemoveActiveController(session);
        PictureInPictureManager::RemovePipControllerInfo(session->window_->GetWindowId());
        session->window_ = nullptr;
        session->curState_ = PipWindowState::STATE_STOPPED;
        std::string navId = session->pipOption_->GetNavigationId();
        if (navId != "" && session->mainWindow_) {
            auto navController = NavigationController::GetNavigationController(
                session->mainWindow_->GetUIContent(), navId);
            if (navController) {
                navController->DeletePIPMode(session->handleId_);
                WLOGFI("Delete pip mode id: %{public}d", session->handleId_);
            }
        }
        SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(currentStopType),
            currentPipOption->GetPipTemplate(), SUCCESS, "pip window stop success");
        return WMError::WM_OK;
    };
    if (handler_ && needAnim) {
        handler_->PostTask(task, "wms:StopPictureInPicture", DEFAULT_TIME_DELAY);
    } else {
        return task();
    }
    return WMError::WM_OK;
}

sptr<Window> PictureInPictureController::GetPipWindow()
{
    return window_;
}

uint32_t PictureInPictureController::GetMainWindowId()
{
    return mainWindowId_;
}

void PictureInPictureController::SetPipWindow(sptr<Window> window)
{
    window_ = window;
}

void PictureInPictureController::SetAutoStartEnabled(bool enable)
{
    WLOGI("SetAutoStartEnabled called, enable: %{public}u, mainWindow: %{public}u", enable, mainWindowId_);
    isAutoStartEnabled_ = enable;
    if (isAutoStartEnabled_) {
        if (mainWindow_ == nullptr) {
            WLOGFE("Init main window failed");
            return;
        }
        if (!IsPullPiPAndHandleNavigation()) {
            WLOGFE("Navigation operate failed");
            return;
        }
        PictureInPictureManager::AttachAutoStartController(handleId_, weakRef_);
    } else {
        PictureInPictureManager::DetachAutoStartController(handleId_, weakRef_);
    }
}

void PictureInPictureController::IsAutoStartEnabled(bool& enable) const
{
    enable = isAutoStartEnabled_;
}

PipWindowState PictureInPictureController::GetControllerState()
{
    return curState_;
}

void PictureInPictureController::UpdateContentSize(int32_t width, int32_t height)
{
    if (width <= 0 || height <= 0) {
        WLOGFE("invalid size");
        return;
    }
    if (curState_ != PipWindowState::STATE_STARTED) {
        WLOGFD("UpdateContentSize is disabled when state: %{public}u", curState_);
        return;
    }
    if (window_ == nullptr) {
        WLOGFE("pipWindow not exist");
        return;
    }
    WLOGI("UpdateContentSize window: %{public}u width:%{public}u height:%{public}u",
        window_->GetWindowId(), width, height);
    window_->UpdatePiPRect(width, height, PiPRectUpdateReason::REASON_PIP_VIDEO_RATIO_CHANGE);
}

void PictureInPictureController::StartMove()
{
    if (window_ == nullptr) {
        WLOGFE("pipWindow not exist");
        return;
    }
    WLOGI("StartMove is called, window: %{public}u", window_->GetWindowId());
    SingletonContainer::Get<PiPReporter>().ReportPiPMove();
    window_->StartMove();
}

void PictureInPictureController::DoScale()
{
    if (window_ == nullptr) {
        WLOGFE("PiPWindow is not exist");
        return;
    }
    WLOGI("DoScale is called, window: %{public}u", window_->GetWindowId());
    window_->UpdatePiPRect(0, 0, PiPRectUpdateReason::REASON_PIP_SCALE_CHANGE);
}

void PictureInPictureController::PipMainWindowLifeCycleImpl::AfterBackground()
{
    WLOGI("PipMainWindowLifeCycleImpl AfterBackground is called");
    if (!PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        WLOGFI("disable auto start as screen is off");
        return;
    }
    PictureInPictureManager::AutoStartPipWindow(navigationId_);
}

void PictureInPictureController::PipMainWindowLifeCycleImpl::BackgroundFailed(int32_t type)
{
    WLOGI("PipMainWindowLifeCycleImpl BackgroundFailed");
}

void PictureInPictureController::PipDisplayListener::OnCreate(DisplayId displayId)
{
    WLOGD("PipDisplayListener OnCreate");
}

void PictureInPictureController::PipDisplayListener::OnDestroy(DisplayId displayId)
{
    WLOGD("PipDisplayListener OnDestroy");
}

void PictureInPictureController::PipDisplayListener::OnChange(DisplayId displayId)
{
    if (displayId != DisplayManager::GetInstance().GetDefaultDisplay()->GetId()) {
        return;
    }
    Rotation rotation = DisplayManager::GetInstance().GetDisplayById(displayId)->GetRotation();
    if (((preRotation_ == Rotation::ROTATION_0 || preRotation_ == Rotation::ROTATION_180) &&
        (rotation == Rotation::ROTATION_90 || rotation == Rotation::ROTATION_270)) ||
        ((preRotation_ == Rotation::ROTATION_90 || preRotation_ == Rotation::ROTATION_270) &&
        (rotation == Rotation::ROTATION_0 || rotation == Rotation::ROTATION_180))) {
        WLOGFI("display rotation changed from %{public}d to %{public}d", static_cast<int32_t>(preRotation_),
            static_cast<int32_t>(rotation));
        preRotation_ = rotation;
        if (pipController_ != nullptr && pipController_->GetPipWindow() != nullptr) {
            pipController_->GetPipWindow()->UpdatePiPRect(pipController_->windowRect_.width_,
                pipController_->windowRect_.height_, PiPRectUpdateReason::REASON_DISPLAY_ROTATION_CHANGE);
        }
    }
}

void PictureInPictureController::DoActionEvent(std::string& actionName)
{
    WLOGFD("actionName: %{public}s", actionName.c_str());
    if (pipActionObserver_ == nullptr) {
        WLOGFE("pipActionObserver is not registered");
        return;
    }
    SingletonContainer::Get<PiPReporter>().ReportPiPActionEvent(pipOption_->GetPipTemplate(), actionName);
    pipActionObserver_->OnActionEvent(actionName);
}

void PictureInPictureController::RestorePictureInPictureWindow()
{
    if (window_ == nullptr || mainWindow_ == nullptr) {
        WLOGFE("window or main window is nullptr");
        return;
    }
    WLOGFI("restore pipWindow %{public}u to [%{public}u, %{public}u, %{public}u, %{public}u]", window_->GetWindowId(),
        windowRect_.posX_, windowRect_.posY_, windowRect_.width_, windowRect_.height_);
    if (pipLifeCycleListener_) {
        pipLifeCycleListener_->OnRestoreUserInterface();
    }
    window_->RecoveryPullPiPMainWindow(windowRect_);
    std::string navId = pipOption_->GetNavigationId();
    if (navId != "") {
        auto navController = NavigationController::GetNavigationController(mainWindow_->GetUIContent(), navId);
        if (navController) {
            navController->PushInPIP(handleId_);
            WLOGFI("Push in pip handleId: %{public}d", handleId_);
        } else {
            WLOGFE("navController is nullptr");
        }
    }
    auto stopPipTask = [weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return;
        }
        session->StopPictureInPicture(true, false, StopPipType::NULL_STOP);
    };
    if (handler_ == nullptr) {
        WLOGFE("handler is nullptr");
        return;
    }
    handler_->PostTask(stopPipTask, "wms:RestorePictureInPictureWindow", DELAY_ANIM);
    SingletonContainer::Get<PiPReporter>().ReportPiPRestore();
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
    WLOGI("ResetExtController is called");
    if (mainWindowXComponentController_ == nullptr || pipXComponentController_ == nullptr) {
        WLOGFE("error when reset XComponentController");
        return;
    }
    XComponentControllerErrorCode errorCode =
        mainWindowXComponentController_->ResetExtController(pipXComponentController_);
    if (errorCode != XComponentControllerErrorCode::XCOMPONENT_CONTROLLER_NO_ERROR) {
        WLOGFE("swap xComponent failed, errorCode: %{public}u", errorCode);
    }
}

WMError PictureInPictureController::SetXComponentController(std::shared_ptr<XComponentController> xComponentController)
{
    WLOGI("SetXComponentController is called");
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

bool PictureInPictureController::IsPullPiPAndHandleNavigation()
{
    if (pipOption_->GetNavigationId() == "") {
        WLOGFI("App not use navigation");
        return true;
    }
    if (mainWindow_ == nullptr) {
        WLOGFE("Main window init error");
        return false;
    }
    std::string navId = pipOption_->GetNavigationId();
    auto navController = NavigationController::GetNavigationController(mainWindow_->GetUIContent(), navId);
    if (navController) {
        if (navController->IsNavDestinationInTopStack()) {
            handleId_ = navController->GetTopHandle();
            if (handleId_ != -1) {
                WLOGFD("Top handle id : %{public}d", handleId_);
                navController->SetInPIPMode(handleId_);
                return true;
            } else {
                WLOGFE("Get top handle error");
                return false;
            }
        } else {
            WLOGFE("Top is not navDestination");
            return false;
        }
    } else {
        WLOGFE("Get navController error");
    }
    return false;
}

std::string PictureInPictureController::GetPiPNavigationId()
{
    return pipOption_? pipOption_->GetNavigationId() : "";
}
} // namespace Rosen
} // namespace OHOS