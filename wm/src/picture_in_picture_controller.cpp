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
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "datashare_helper.h"
#include "iservice_registry.h"
#include "result_set.h"
#include "system_ability_definition.h"
#include "uri.h"

namespace OHOS {
namespace Rosen {
    sptr<IRemoteObject> PictureInPictureController::remoteObj_;
namespace {
    constexpr int32_t DELAY_ANIM = 500;
    constexpr int32_t SUCCESS = 1;
    constexpr int32_t FAILED = 0;
    constexpr uint32_t PIP_LOW_PRIORITY = 0;
    constexpr uint32_t PIP_HIGH_PRIORITY = 1;
    const std::string PIP_CONTENT_PATH = "/system/etc/window/resources/pip_content.abc";
    const std::string KEY = "auto_start_pip_status";
    const std::string SETTING_COLUMN_KEYWORD = "KEYWORD";
    const std::string SETTING_COLUMN_VALUE = "VALUE";
    const std::string SETTING_URI_PROXY = "datashare:///com.ohos.settingsdata/entry/"
        "settingsdata/SETTINGSDATA?Proxy=true";
    constexpr const char *SETTINGS_DATA_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
}
static uint32_t GetPipPriority(uint32_t pipTemplateType)
{
    if (pipTemplateType >= static_cast<uint32_t>(PiPTemplateType::END)) {
        TLOGE(WmsLogTag::WMS_PIP, "param invalid, pipTemplateType is %{public}d", pipTemplateType);
        return PIP_LOW_PRIORITY;
    }
    if (pipTemplateType == static_cast<uint32_t>(PiPTemplateType::VIDEO_PLAY) ||
        pipTemplateType == static_cast<uint32_t>(PiPTemplateType::VIDEO_LIVE)) {
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
    curState_ = PiPWindowState::STATE_UNDEFINED;

    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "GetSystemAbilityManager return nullptr");
        return;
    }
    auto remoteObj = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
    if (remoteObj == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "GetSystemAbility return nullptr, systemAbilityId=%{public}d",
            WINDOW_MANAGER_SERVICE_ID);
        return;
    }
    remoteObj_ = remoteObj;
}

PictureInPictureController::~PictureInPictureController()
{
    PictureInPictureManager::DetachAutoStartController(handleId_, weakRef_);
    remoteObj_ = nullptr;
}

WMError PictureInPictureController::CreatePictureInPictureWindow()
{
    TLOGI(WmsLogTag::WMS_PIP, "CreatePictureInPictureWindow is called, mainWindow:%{public}u", mainWindowId_);
    sptr<PictureInPictureController> thisController = this;
    if (pipOption_ == nullptr || pipOption_->GetContext() == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "Create pip failed, invalid pipOption");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    auto context = static_cast<std::weak_ptr<AbilityRuntime::Context>*>(pipOption_->GetContext());
    const std::shared_ptr<AbilityRuntime::Context>& abilityContext = context->lock();
    SingletonContainer::Get<PiPReporter>().SetCurrentPackageName(abilityContext->GetApplicationInfo()->name);
    sptr<WindowOption> windowOption = new(std::nothrow) WindowOption();
    if (windowOption == nullptr) {
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    mainWindowXComponentController_ = pipOption_->GetXComponentController();
    if (mainWindowXComponentController_ == nullptr || mainWindow_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "mainWindowXComponentController or mainWindow is nullptr");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    UpdateXComponentPositionAndSize();
    windowOption->SetWindowName(PIP_WINDOW_NAME);
    windowOption->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    windowOption->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    windowOption->SetWindowRect(windowRect_);
    windowOption->SetKeepScreenOn(true);
    windowOption->SetTouchable(false);
    WMError errCode;
    PiPTemplateInfo pipTemplateInfo;
    pipTemplateInfo.pipTemplateType = pipOption_->GetPipTemplate();
    pipTemplateInfo.controlGroup = pipOption_->GetControlGroup();
    pipTemplateInfo.priority = GetPipPriority(pipOption_->GetPipTemplate());
    sptr<Window> window = Window::CreatePiP(windowOption, pipTemplateInfo, context->lock(), errCode);
    if (window == nullptr || errCode != WMError::WM_OK) {
        TLOGW(WmsLogTag::WMS_PIP, "Window create failed, reason: %{public}d", errCode);
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    window_ = window;
    window_->UpdatePiPRect(windowRect_, WindowSizeChangeReason::PIP_START);
    PictureInPictureManager::PutPipControllerInfo(window_->GetWindowId(), thisController);
    return WMError::WM_OK;
}

WMError PictureInPictureController::ShowPictureInPictureWindow(StartPipType startType)
{
    TLOGD(WmsLogTag::WMS_PIP, "ShowPictureInPictureWindow is called");
    if (pipOption_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "Get PictureInPicture option failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    if (window_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "window is null when show pip");
        SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
            pipOption_->GetPipTemplate(), FAILED, "window is nullptr");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    if (pipLifeCycleListener_ != nullptr) {
        pipLifeCycleListener_->OnPreparePictureInPictureStart();
    }
    window_->SetUIContentByAbc(PIP_CONTENT_PATH, env_, nullptr, nullptr);
    WMError errCode = window_->Show(0, false);
    if (errCode != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "window show failed, err: %{public}u", errCode);
        int32_t err = static_cast<int32_t>(errCode);
        if (pipLifeCycleListener_ != nullptr) {
            pipLifeCycleListener_->OnPictureInPictureOperationError(err);
        }
        SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
            pipOption_->GetPipTemplate(), FAILED, "window show failed");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
    uint32_t requestWidth = 0;
    uint32_t requestHeight = 0;
    pipOption_->GetContentSize(requestWidth, requestHeight);
    if (requestWidth > 0 && requestHeight > 0) {
        Rect requestRect = {0, 0, requestWidth, requestHeight};
        window_->UpdatePiPRect(requestRect, WindowSizeChangeReason::PIP_SHOW);
    } else {
        window_->UpdatePiPRect(windowRect_, WindowSizeChangeReason::PIP_SHOW);
    }
    PictureInPictureManager::SetActiveController(this);
    SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
        pipOption_->GetPipTemplate(), SUCCESS, "show pip success");
    return WMError::WM_OK;
}

WMError PictureInPictureController::StartPictureInPicture(StartPipType startType)
{
    TLOGI(WmsLogTag::WMS_PIP, "StartPictureInPicture called");
    std::lock_guard<std::mutex> lock(mutex_);
    if (pipOption_ == nullptr || pipOption_->GetContext() == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "pipOption is null or Get PictureInPictureOption failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    if (curState_ == PiPWindowState::STATE_STARTING || curState_ == PiPWindowState::STATE_STARTED) {
        TLOGW(WmsLogTag::WMS_PIP, "pipWindow is starting, state: %{public}u, id: %{public}u, mainWindow: %{public}u",
            curState_, (window_ == nullptr) ? INVALID_WINDOW_ID : window_->GetWindowId(), mainWindowId_);
        SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
            pipOption_->GetPipTemplate(), FAILED, "Pip window is starting");
        return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    }
    if (mainWindow_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "Init main window failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    if (!IsPullPiPAndHandleNavigation()) {
        TLOGE(WmsLogTag::WMS_PIP, "Navigation operate failed");
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    curState_ = PiPWindowState::STATE_STARTING;
    if (PictureInPictureManager::HasActiveController() && !PictureInPictureManager::IsActiveController(weakRef_)) {
        // if current controller is not the active one, but belongs to the same mainWindow, reserve pipWindow
        if (PictureInPictureManager::IsAttachedToSameWindow(mainWindowId_)) {
            window_ = PictureInPictureManager::GetCurrentWindow();
            if (window_ == nullptr) {
                TLOGE(WmsLogTag::WMS_PIP, "Reuse pipWindow failed");
                curState_ = PiPWindowState::STATE_UNDEFINED;
                return WMError::WM_ERROR_PIP_CREATE_FAILED;
            }
            TLOGI(WmsLogTag::WMS_PIP, "Reuse pipWindow: %{public}u as attached to the same mainWindow: %{public}u",
                window_->GetWindowId(), mainWindowId_);
            PictureInPictureManager::DoClose(false, false);
            mainWindowXComponentController_ = pipOption_->GetXComponentController();
            UpdateXComponentPositionAndSize();
            UpdateContentSize(windowRect_.width_, windowRect_.height_);
            PictureInPictureManager::PutPipControllerInfo(window_->GetWindowId(), this);
            WMError err = ShowPictureInPictureWindow(startType);
            if (err != WMError::WM_OK) {
                curState_ = PiPWindowState::STATE_UNDEFINED;
            } else {
                curState_ = PiPWindowState::STATE_STARTED;
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
        curState_ = PiPWindowState::STATE_UNDEFINED;
        TLOGE(WmsLogTag::WMS_PIP, "Create pip window failed, err: %{public}u", errCode);
        SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
            pipOption_->GetPipTemplate(), FAILED, "Create pip window failed");
        return errCode;
    }
    errCode = ShowPictureInPictureWindow(startType);
    if (errCode != WMError::WM_OK) {
        curState_ = PiPWindowState::STATE_UNDEFINED;
        TLOGE(WmsLogTag::WMS_PIP, "Show pip window failed, err: %{public}u", errCode);
        SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
            pipOption_->GetPipTemplate(), FAILED, "Show pip window failed");
        return errCode;
    }
    curState_ = PiPWindowState::STATE_STARTED;
    SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
        pipOption_->GetPipTemplate(), SUCCESS, "start pip success");
    return WMError::WM_OK;
}

WMError PictureInPictureController::StopPictureInPictureFromClient()
{
    if (!window_) {
        TLOGE(WmsLogTag::WMS_PIP, "window is null");
        SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(StopPipType::USER_STOP),
            pipOption_->GetPipTemplate(), FAILED, "window is null");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    if (curState_ == PiPWindowState::STATE_STOPPING || curState_ == PiPWindowState::STATE_STOPPED) {
        TLOGE(WmsLogTag::WMS_PIP, "Repeat stop request, curState: %{public}u", curState_);
        SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(StopPipType::USER_STOP),
            pipOption_->GetPipTemplate(), FAILED, "Repeat stop request");
        return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    }
    WMError res = window_->NotifyPrepareClosePiPWindow();
    if (res != WMError::WM_OK) {
        SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(StopPipType::USER_STOP),
            pipOption_->GetPipTemplate(), FAILED, "window destroy failed");
        return WMError::WM_ERROR_PIP_DESTROY_FAILED;
    }
    return res;
}

WMError PictureInPictureController::StopPictureInPicture(bool destroyWindow, StopPipType stopPipType)
{
    TLOGD(WmsLogTag::WMS_PIP, "StopPictureInPicture is called, destroyWindow: %{public}u", destroyWindow);
    std::lock_guard<std::mutex> lock(mutex_);
    if (curState_ == PiPWindowState::STATE_STOPPING || curState_ == PiPWindowState::STATE_STOPPED) {
        TLOGE(WmsLogTag::WMS_PIP, "Repeat stop request, curState: %{public}u", curState_);
        SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(stopPipType),
            pipOption_->GetPipTemplate(), FAILED, "Repeat stop request");
        return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    }
    if (window_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "window is nullptr when stop pip");
        SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(stopPipType),
            pipOption_->GetPipTemplate(), FAILED, "window_ is nullptr");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    curState_ = PiPWindowState::STATE_STOPPING;
    if (pipLifeCycleListener_ != nullptr) {
        pipLifeCycleListener_->OnPreparePictureInPictureStop();
    }
    if (!destroyWindow) {
        ResetExtController();
        curState_ = PiPWindowState::STATE_STOPPED;
        if (pipLifeCycleListener_) {
            pipLifeCycleListener_->OnPictureInPictureStop();
        }
        PictureInPictureManager::RemoveActiveController(weakRef_);
        PictureInPictureManager::RemovePipControllerInfo(window_->GetWindowId());
        return WMError::WM_OK;
    }
    return StopPictureInPictureInner(stopPipType);
}

WMError PictureInPictureController::StopPictureInPictureInner(StopPipType stopType)
{
    auto task = [weakThis = wptr(this), currentStopType = stopType, currentPipOption = pipOption_]() {
        auto session = weakThis.promote();
        if (!session || !session->window_) {
            TLOGE(WmsLogTag::WMS_PIP, "pipController is null in stopping task");
            SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(currentStopType),
                currentPipOption->GetPipTemplate(), FAILED, "pipController is null");
            return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
        }
        session->ResetExtController();
        WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(session->window_->Destroy());
        if (ret != WmErrorCode::WM_OK) {
            session->curState_ = PiPWindowState::STATE_UNDEFINED;
            TLOGE(WmsLogTag::WMS_PIP, "Window destroy failed, err:%{public}u", ret);
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
        session->curState_ = PiPWindowState::STATE_STOPPED;
        std::string navId = session->pipOption_->GetNavigationId();
        if (navId != "" && session->mainWindow_) {
            auto navController = NavigationController::GetNavigationController(
                session->mainWindow_->GetUIContent(), navId);
            if (navController) {
                navController->DeletePIPMode(session->handleId_);
                TLOGI(WmsLogTag::WMS_PIP, "Delete pip mode id: %{public}d", session->handleId_);
            }
        }
        SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(currentStopType),
            currentPipOption->GetPipTemplate(), SUCCESS, "pip window stop success");
        return WMError::WM_OK;
    };
    if (handler_) {
        handler_->PostTask(task, "wms:StopPictureInPicture", 0);
    } else {
        return task();
    }
    return WMError::WM_OK;
}

sptr<Window> PictureInPictureController::GetPipWindow() const
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
    TLOGI(WmsLogTag::WMS_PIP,
        "SetAutoStartEnabled called, enable: %{public}u, mainWindow: %{public}u", enable, mainWindowId_);
    isAutoStartEnabled_ = enable;
    if (isAutoStartEnabled_) {
        // cache navigation here as we cannot get containerId while BG
        if (!IsPullPiPAndHandleNavigation()) {
            TLOGE(WmsLogTag::WMS_PIP, "Navigation operate failed");
            return;
        }
        PictureInPictureManager::AttachAutoStartController(handleId_, weakRef_);
    } else {
        PictureInPictureManager::DetachAutoStartController(handleId_, weakRef_);
        if (!pipOption_) {
            return;
        }
        std::string navId = pipOption_->GetNavigationId();
        if (navId != "" && mainWindow_) {
            auto navController = NavigationController::GetNavigationController(mainWindow_->GetUIContent(), navId);
            if (navController) {
                navController->DeletePIPMode(handleId_);
                TLOGI(WmsLogTag::WMS_PIP, "Delete pip mode id: %{public}d", handleId_);
            }
        }
    }
}

void PictureInPictureController::IsAutoStartEnabled(bool& enable) const
{
    enable = isAutoStartEnabled_;
}

PiPWindowState PictureInPictureController::GetControllerState()
{
    return curState_;
}

void PictureInPictureController::UpdateContentSize(int32_t width, int32_t height)
{
    if (width <= 0 || height <= 0) {
        TLOGE(WmsLogTag::WMS_PIP, "invalid size");
        return;
    }
    if (curState_ != PiPWindowState::STATE_STARTED) {
        TLOGD(WmsLogTag::WMS_PIP, "UpdateContentSize is disabled when state: %{public}u", curState_);
        return;
    }
    if (window_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "pipWindow not exist");
        return;
    }
    if (mainWindowXComponentController_) {
        float posX = 0;
        float posY = 0;
        float newWidth = 0;
        float newHeight = 0;
        mainWindowXComponentController_->GetGlobalPosition(posX, posY);
        mainWindowXComponentController_->GetSize(newWidth, newHeight);
        if (windowRect_.width_ != static_cast<uint32_t>(newWidth) ||
            windowRect_.height_ != static_cast<uint32_t>(newHeight) ||
            windowRect_.posX_ != static_cast<int32_t>(posX) || windowRect_.posY_ != static_cast<int32_t>(posY)) {
            Rect r = {posX, posY, newWidth, newHeight};
            window_->UpdatePiPRect(r, WindowSizeChangeReason::TRANSFORM);
        }
    }
    TLOGI(WmsLogTag::WMS_PIP, "UpdateContentSize window: %{public}u width:%{public}u height:%{public}u",
        window_->GetWindowId(), width, height);
    pipOption_->SetContentSize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    Rect rect = {0, 0, width, height};
    window_->UpdatePiPRect(rect, WindowSizeChangeReason::PIP_RATIO_CHANGE);
    SingletonContainer::Get<PiPReporter>().ReportPiPRatio(width, height);
}

void PictureInPictureController::PipMainWindowLifeCycleImpl::AfterBackground()
{
    TLOGI(WmsLogTag::WMS_PIP, "PipMainWindowLifeCycleImpl AfterBackground is called");
    if (!PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        TLOGI(WmsLogTag::WMS_PIP, "disable auto start as screen is off");
        return;
    }
    std::string value;
    ErrCode ret = getSettingsAutoStartStatus(KEY, value);
    TLOGI(WmsLogTag::WMS_PIP, "getSettingsAutoStartStatus, value=%{public}s", value.c_str());
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "get setting auto pip failed, ret=%{public}d", ret);
        return;
    }
    if (value == "false") {
        return;
    }
    TLOGI(WmsLogTag::WMS_PIP, "PipMainWindowLifeCycleImpl AfterBackground is called success");
    PictureInPictureManager::AutoStartPipWindow(navigationId_);
}

void PictureInPictureController::PipMainWindowLifeCycleImpl::BackgroundFailed(int32_t type)
{
    TLOGD(WmsLogTag::WMS_PIP, "PipMainWindowLifeCycleImpl BackgroundFailed");
}

void PictureInPictureController::DoActionEvent(const std::string& actionName, int32_t status)
{
    TLOGD(WmsLogTag::WMS_PIP, "actionName: %{public}s", actionName.c_str());
    if (pipActionObserver_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "pipActionObserver is not registered");
        return;
    }
    SingletonContainer::Get<PiPReporter>().ReportPiPActionEvent(pipOption_->GetPipTemplate(), actionName);
    pipActionObserver_->OnActionEvent(actionName, status);
}

void PictureInPictureController::RestorePictureInPictureWindow()
{
    if (pipLifeCycleListener_) {
        pipLifeCycleListener_->OnRestoreUserInterface();
    }
    if (mainWindow_ == nullptr) {
        TLOGI(WmsLogTag::WMS_PIP, "main window is nullptr");
        return;
    }
    std::string navId = pipOption_->GetNavigationId();
    if (navId != "") {
        auto navController = NavigationController::GetNavigationController(mainWindow_->GetUIContent(), navId);
        if (navController) {
            navController->PushInPIP(handleId_);
            TLOGI(WmsLogTag::WMS_PIP, "Push in pip handleId: %{public}d", handleId_);
        } else {
            TLOGE(WmsLogTag::WMS_PIP, "navController is nullptr");
        }
    }
    if (handler_) {
        auto stopTask = [weakThis = wptr(this)]() {
            auto controller = weakThis.promote();
            if (!controller) {
                TLOGE(WmsLogTag::WMS_PIP, "controller is nullptr");
                return;
            }
            controller->StopPictureInPicture(true, StopPipType::NULL_STOP);
        };
        handler_->PostTask(stopTask, "wms:StopPictureInPicture_restore", DELAY_ANIM);
    } else {
        TLOGW(WmsLogTag::WMS_PIP, "StopPictureInPicture no delay while restore");
        StopPictureInPicture(true, StopPipType::NULL_STOP);
    }
    SingletonContainer::Get<PiPReporter>().ReportPiPRestore();
    TLOGI(WmsLogTag::WMS_PIP, "restore pip main window finished");
}

void PictureInPictureController::UpdateXComponentPositionAndSize()
{
    if (!mainWindowXComponentController_) {
        TLOGE(WmsLogTag::WMS_PIP, "main window xComponent not set");
        return;
    }
    float posX = 0;
    float posY = 0;
    float width = 0;
    float height = 0;
    mainWindowXComponentController_->GetGlobalPosition(posX, posY);
    mainWindowXComponentController_->GetSize(width, height);
    windowRect_.width_ = static_cast<uint32_t>(width);
    windowRect_.height_ = static_cast<uint32_t>(height);
    windowRect_.posX_ = static_cast<int32_t>(posX);
    windowRect_.posY_ = static_cast<int32_t>(posY);
    TLOGD(WmsLogTag::WMS_PIP, "position width: %{public}u, height: %{public}u, posX: %{public}d, posY: %{public}d",
        windowRect_.width_, windowRect_.height_, windowRect_.posX_, windowRect_.posY_);
}

void PictureInPictureController::ResetExtController()
{
    TLOGI(WmsLogTag::WMS_PIP, "ResetExtController is called");
    if (mainWindowXComponentController_ == nullptr || pipXComponentController_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "error when resetExtController, one of the xComponentController is null");
        return;
    }
    XComponentControllerErrorCode errorCode =
        mainWindowXComponentController_->ResetExtController(pipXComponentController_);
    if (errorCode != XComponentControllerErrorCode::XCOMPONENT_CONTROLLER_NO_ERROR) {
        TLOGE(WmsLogTag::WMS_PIP, "swap xComponent failed, errorCode: %{public}u", errorCode);
    }
}

WMError PictureInPictureController::SetXComponentController(std::shared_ptr<XComponentController> xComponentController)
{
    TLOGD(WmsLogTag::WMS_PIP, "SetXComponentController is called");
    pipXComponentController_ = xComponentController;
    if (window_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "window is nullptr when set XComponentController");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    if (mainWindowXComponentController_ == nullptr || pipXComponentController_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "error when setXController, one of the xComponentController is null");
        return WMError::WM_ERROR_PIP_STATE_ABNORMALLY;
    }
    XComponentControllerErrorCode errorCode =
        mainWindowXComponentController_->SetExtController(pipXComponentController_);
    if (errorCode != XComponentControllerErrorCode::XCOMPONENT_CONTROLLER_NO_ERROR) {
        TLOGE(WmsLogTag::WMS_PIP, "swap xComponent failed, errorCode: %{public}u", errorCode);
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
    if (pipLifeCycleListener_ != nullptr) {
        pipLifeCycleListener_->OnPictureInPictureStart();
    }
    return WMError::WM_OK;
}

void PictureInPictureController::SetPictureInPictureLifecycle(sptr<IPiPLifeCycle> listener)
{
    pipLifeCycleListener_ = listener;
}

void PictureInPictureController::SetPictureInPictureActionObserver(sptr<IPiPActionObserver> listener)
{
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
        TLOGI(WmsLogTag::WMS_PIP, "App not use navigation");
        return true;
    }
    if (mainWindow_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "Main window init error");
        return false;
    }
    std::string navId = pipOption_->GetNavigationId();
    auto navController = NavigationController::GetNavigationController(mainWindow_->GetUIContent(), navId);
    if (navController) {
        if (navController->IsNavDestinationInTopStack()) {
            handleId_ = navController->GetTopHandle();
            if (handleId_ != -1) {
                TLOGD(WmsLogTag::WMS_PIP, "Top handle id : %{public}d", handleId_);
                navController->SetInPIPMode(handleId_);
                return true;
            } else {
                TLOGE(WmsLogTag::WMS_PIP, "Get top handle error");
                return false;
            }
        } else {
            TLOGE(WmsLogTag::WMS_PIP, "Top is not navDestination");
            return false;
        }
    } else {
        TLOGE(WmsLogTag::WMS_PIP, "Get navController error");
    }
    return false;
}

ErrCode PictureInPictureController::getSettingsAutoStartStatus(const std::string& key, std::string& value)
{
    auto helper = DataShare::DataShareHelper::Creator(remoteObj_, SETTING_URI_PROXY, SETTINGS_DATA_EXT_URI);
    if (helper == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "create helper is nullptr");
        return ERR_NO_INIT;
    }
    std::vector<std::string> columns = {SETTING_COLUMN_VALUE};
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(SETTING_COLUMN_KEYWORD, key);
    Uri uri(SETTING_URI_PROXY + "&key=" + key);
    auto resultSet = helper->Query(uri, predicates, columns);
    if (resultSet == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "Query return nullptr");
        return ERR_INVALID_OPERATION;
    }
    int32_t count;
    resultSet->GetRowCount(count);
    if (count == 0) {
        TLOGE(WmsLogTag::WMS_PIP, "not found value, key=%{public}s, count=%{public}d", key.c_str(), count);
        resultSet->Close();
        return ERR_NAME_NOT_FOUND;
    }
    const int32_t INDEX = 0;
    resultSet->GoToRow(INDEX);
    int32_t ret = resultSet->GetString(INDEX, value);
    if (ret != NativeRdb::E_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "resultSet->GetString return not ok, ret=%{public}d", ret);
        resultSet->Close();
        return ERR_INVALID_VALUE;
    }
    resultSet->Close();
    return ERR_OK;
}

std::string PictureInPictureController::GetPiPNavigationId()
{
    return pipOption_? pipOption_->GetNavigationId() : "";
}
} // namespace Rosen
} // namespace OHOS