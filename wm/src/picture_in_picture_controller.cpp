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
#include <transaction/rs_sync_transaction_controller.h>
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
    constexpr int32_t DELAY_RESET = 100;
    constexpr int32_t PIP_DESTROY_TIMEOUT = 600;
    constexpr int32_t PIP_SUCCESS = 1;
    constexpr int32_t FAILED = 0;
    constexpr uint32_t PIP_LOW_PRIORITY = 0;
    constexpr uint32_t PIP_HIGH_PRIORITY = 1;
    const std::string PIP_CONTENT_PATH = "/system/etc/window/resources/pip_content.abc";
    const std::string KEY = "auto_start_pip_status";
    const std::string SETTING_COLUMN_KEYWORD = "KEYWORD";
    const std::string SETTING_COLUMN_VALUE = "VALUE";
    const std::string DESTROY_TIMEOUT_TASK = "PipDestroyTimeout";
    const std::string SETTING_URI_PROXY = "datashare:///com.ohos.settingsdata/entry/"
        "settingsdata/SETTINGSDATA?Proxy=true";
    constexpr const char *SETTINGS_DATA_EXT_URI = "datashare:///com.ohos.settingsdata.DataAbility";
}

uint32_t PictureInPictureController::GetPipPriority(uint32_t pipTemplateType)
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

WMError PictureInPictureController::CreatePictureInPictureWindow(StartPipType startType)
{
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
    TLOGI(WmsLogTag::WMS_PIP, "mainWindow:%{public}u, mainWindowState:%{public}u",
        mainWindowId_, mainWindow_->GetWindowState());
    if (startType != StartPipType::AUTO_START && mainWindow_->GetWindowState() != WindowState::STATE_SHOWN) {
        TLOGE(WmsLogTag::WMS_PIP, "mainWindow is not shown. create failed.");
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
    pipTemplateInfo.pipControlStatusInfoList = pipOption_->GetControlStatus();
    pipTemplateInfo.pipControlEnableInfoList = pipOption_->GetControlEnable();
    sptr<Window> window = Window::CreatePiP(windowOption, pipTemplateInfo, context->lock(), errCode);
    if (window == nullptr || errCode != WMError::WM_OK) {
        TLOGW(WmsLogTag::WMS_PIP, "Window create failed, reason: %{public}d", errCode);
        return WMError::WM_ERROR_PIP_CREATE_FAILED;
    }
    window_ = window;
    window_->UpdatePiPRect(windowRect_, WindowSizeChangeReason::PIP_START);
    PictureInPictureManager::PutPipControllerInfo(window_->GetWindowId(), this);
    return WMError::WM_OK;
}

WMError PictureInPictureController::ShowPictureInPictureWindow(StartPipType startType)
{
    TLOGI(WmsLogTag::WMS_PIP, "startType:%{public}u", startType);
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
    for (auto& listener : pipLifeCycleListeners_) {
        listener->OnPreparePictureInPictureStart();
    }
    window_->SetUIContentByAbc(PIP_CONTENT_PATH, env_, nullptr, nullptr);
    WMError errCode = window_->Show(0, false);
    if (errCode != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "window show failed, err: %{public}u", errCode);
        for (auto& listener : pipLifeCycleListeners_) {
            listener->OnPictureInPictureOperationError(static_cast<int32_t>(errCode));
        }
        SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
            pipOption_->GetPipTemplate(), FAILED, "window show failed");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
    uint32_t requestWidth = 0;
    uint32_t requestHeight = 0;
    pipOption_->GetContentSize(requestWidth, requestHeight);
    WindowSizeChangeReason reason = WindowSizeChangeReason::PIP_SHOW;
    if (startType == StartPipType::AUTO_START) {
        reason = WindowSizeChangeReason::PIP_AUTO_START;
    }
    if (requestWidth > 0 && requestHeight > 0) {
        Rect requestRect = {0, 0, requestWidth, requestHeight};
        window_->UpdatePiPRect(requestRect, reason);
    } else {
        window_->UpdatePiPRect(windowRect_, reason);
    }
    PictureInPictureManager::SetActiveController(this);
    SingletonContainer::Get<PiPReporter>().ReportPiPStartWindow(static_cast<int32_t>(startType),
        pipOption_->GetPipTemplate(), PIP_SUCCESS, "show pip success");
    isStoppedFromClient_ = false;
    return WMError::WM_OK;
}

WMError PictureInPictureController::StartPictureInPicture(StartPipType startType)
{
    TLOGI(WmsLogTag::WMS_PIP, "called");
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
    WMError errCode = CreatePictureInPictureWindow(startType);
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
        pipOption_->GetPipTemplate(), PIP_SUCCESS, "start pip success");
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
    if (curState_ == PiPWindowState::STATE_STOPPING || curState_ == PiPWindowState::STATE_STOPPED ||
        curState_ == PiPWindowState::STATE_RESTORING) {
        TLOGE(WmsLogTag::WMS_PIP, "Repeat stop request, curState: %{public}u", curState_);
        SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(StopPipType::USER_STOP),
            pipOption_->GetPipTemplate(), FAILED, "Repeat stop request");
        return WMError::WM_ERROR_PIP_REPEAT_OPERATION;
    }
    isStoppedFromClient_ = true;
    WMError res = window_->NotifyPrepareClosePiPWindow();
    if (res != WMError::WM_OK) {
        SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(StopPipType::USER_STOP),
            pipOption_->GetPipTemplate(), FAILED, "window destroy failed");
        return WMError::WM_ERROR_PIP_DESTROY_FAILED;
    }
    curState_ = PiPWindowState::STATE_STOPPING;
    for (auto& listener : pipLifeCycleListeners_) {
        listener->OnPreparePictureInPictureStop();
    }
    return res;
}

WMError PictureInPictureController::StopPictureInPicture(bool destroyWindow, StopPipType stopPipType, bool withAnim)
{
    TLOGD(WmsLogTag::WMS_PIP, "destroyWindow: %{public}u anim: %{public}d", destroyWindow, withAnim);
    std::lock_guard<std::mutex> lock(mutex_);
    if ((!isStoppedFromClient_ && curState_ == PiPWindowState::STATE_STOPPING) ||
        curState_ == PiPWindowState::STATE_STOPPED) {
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
    if (curState_ != PiPWindowState::STATE_STOPPING) {
        curState_ = PiPWindowState::STATE_STOPPING;
        for (auto& listener : pipLifeCycleListeners_) {
            listener->OnPreparePictureInPictureStop();
        }
    }
    if (!destroyWindow) {
        ResetExtController();
        curState_ = PiPWindowState::STATE_STOPPED;
        for (auto& listener : pipLifeCycleListeners_) {
            listener->OnPictureInPictureStop();
        }
        PictureInPictureManager::RemoveActiveController(weakRef_);
        PictureInPictureManager::RemovePipControllerInfo(window_->GetWindowId());
        return WMError::WM_OK;
    }
    return StopPictureInPictureInner(stopPipType, withAnim);
}

WMError PictureInPictureController::StopPictureInPictureInner(StopPipType stopType, bool withAnim)
{
    uint32_t templateType = 0;
    if (pipOption_ != nullptr) {
        templateType = pipOption_->GetPipTemplate();
    }
    if (window_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "window is nullptr in stop pip inner");
        SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(stopType),
            templateType, FAILED, "pipController is null");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
    auto syncTransactionController = RSSyncTransactionController::GetInstance();
    if (syncTransactionController) {
        syncTransactionController->OpenSyncTransaction();
    }
    ResetExtController();
    if (!withAnim) {
        DestroyPictureInPictureWindow();
    } else {
        // handle destroy timeout
        if (handler_ == nullptr) {
            handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
        }
        auto timeoutTask = [weakThis = wptr(this)]() {
            auto pipController = weakThis.promote();
            if (pipController == nullptr) {
                TLOGE(WmsLogTag::WMS_PIP, "execute destroy timeout task failed, pipController is null");
                return;
            }
            if (pipController->curState_ == PiPWindowState::STATE_STOPPED) {
                TLOGI(WmsLogTag::WMS_PIP, "pip window already destroyed");
                return;
            }
            TLOGI(WmsLogTag::WMS_PIP, "DestroyPictureInPictureWindow timeout");
            pipController->DestroyPictureInPictureWindow();
        };
        handler_->PostTask(timeoutTask, DESTROY_TIMEOUT_TASK, PIP_DESTROY_TIMEOUT);
    }
    if (syncTransactionController) {
        syncTransactionController->CloseSyncTransaction();
    }

    SingletonContainer::Get<PiPReporter>().ReportPiPStopWindow(static_cast<int32_t>(stopType),
        templateType, PIP_SUCCESS, "pip window stop success");
    return WMError::WM_OK;
}

WMError PictureInPictureController::DestroyPictureInPictureWindow()
{
    TLOGI(WmsLogTag::WMS_PIP, "called");
    handler_->RemoveTask(DESTROY_TIMEOUT_TASK);
    if (window_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "window is nullptr when destroy pip");
        return WMError::WM_ERROR_PIP_INTERNAL_ERROR;
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window_->Destroy());
    if (ret != WmErrorCode::WM_OK) {
        curState_ = PiPWindowState::STATE_UNDEFINED;
        TLOGE(WmsLogTag::WMS_PIP, "window destroy failed, err:%{public}u", ret);
        for (auto& listener : pipLifeCycleListeners_) {
            listener->OnPictureInPictureOperationError(static_cast<int32_t>(ret));
        }
        return WMError::WM_ERROR_PIP_DESTROY_FAILED;
    }
    PictureInPictureManager::RemoveActiveController(this);
    PictureInPictureManager::RemovePipControllerInfo(window_->GetWindowId());
    window_ = nullptr;

    for (auto& listener : pipLifeCycleListeners_) {
        listener->OnPictureInPictureStop();
    }
    curState_ = PiPWindowState::STATE_STOPPED;
    std::string navId = pipOption_ == nullptr ? "" : pipOption_->GetNavigationId();
    if (!navId.empty() && mainWindow_) {
        auto navController = NavigationController::GetNavigationController(mainWindow_->GetUIContent(), navId);
        if (navController) {
            navController->DeletePIPMode(handleId_);
            TLOGI(WmsLogTag::WMS_PIP, "Delete pip mode id: %{public}d", handleId_);
        }
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
    TLOGI(WmsLogTag::WMS_PIP, "enable: %{public}u, mainWindow: %{public}u", enable, mainWindowId_);
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
            TLOGE(WmsLogTag::WMS_PIP, "pipOption is null");
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
    pipOption_->SetContentSize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
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
        bool isSizeChange = IsContentSizeChanged(newWidth, newHeight, posX, posY);
        if (isSizeChange) {
            Rect r = {static_cast<int32_t>(posX), static_cast<int32_t>(posY),
                static_cast<uint32_t>(newWidth), static_cast<uint32_t>(newHeight)};
            window_->UpdatePiPRect(r, WindowSizeChangeReason::TRANSFORM);
        }
    }
    TLOGI(WmsLogTag::WMS_PIP, "UpdateContentSize window: %{public}u width:%{public}u height:%{public}u",
        window_->GetWindowId(), width, height);
    Rect rect = {0, 0, width, height};
    window_->UpdatePiPRect(rect, WindowSizeChangeReason::PIP_RATIO_CHANGE);
    SingletonContainer::Get<PiPReporter>().ReportPiPRatio(width, height);
}


void PictureInPictureController::UpdatePiPControlStatus(PiPControlType controlType, PiPControlStatus status)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlType:%{public}u, status:%{public}d", controlType, status);
    if (static_cast<int32_t>(status) < -1) {
        pipOption_->SetPiPControlEnabled(controlType, status);
    } else {
        pipOption_->SetPiPControlStatus(controlType, status);
    }
    if (window_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "pipWindow not exist");
        return;
    }
    window_->UpdatePiPControlStatus(controlType, status);
}

bool PictureInPictureController::IsContentSizeChanged(float width, float height, float posX, float posY)
{
    return windowRect_.width_ != static_cast<uint32_t>(width) ||
        windowRect_.height_ != static_cast<uint32_t>(height) ||
        windowRect_.posX_ != static_cast<int32_t>(posX) || windowRect_.posY_ != static_cast<int32_t>(posY);
}

void PictureInPictureController::PipMainWindowLifeCycleImpl::AfterBackground()
{
    std::string value;
    ErrCode ret = getSettingsAutoStartStatus(KEY, value);
    TLOGI(WmsLogTag::WMS_PIP, "getSettingsAutoStartStatus, value=%{public}s", value.c_str());
    if (ret != ERR_OK) {
        TLOGE(WmsLogTag::WMS_PIP, "get setting auto pip failed, ret=%{public}d", ret);
    }
    if (value == "false") {
        return;
    }
    if (windowListener_ != nullptr) {
        auto curMode = windowListener_->GetMode();
        if (curMode == WindowMode::WINDOW_MODE_FLOATING) {
            TLOGI(WmsLogTag::WMS_PIP, "floating not process");
            return;
        }
        if (!windowListener_->IsValid()) {
            TLOGI(WmsLogTag::WMS_PIP, "split close not process");
            return;
        }
    }
    PictureInPictureManager::AutoStartPipWindow(navigationId_);
    TLOGI(WmsLogTag::WMS_PIP, "called success");
}

void PictureInPictureController::PipMainWindowLifeCycleImpl::BackgroundFailed(int32_t type)
{
    TLOGD(WmsLogTag::WMS_PIP, "PipMainWindowLifeCycleImpl BackgroundFailed");
}

void PictureInPictureController::DoActionEvent(const std::string& actionName, int32_t status)
{
    TLOGI(WmsLogTag::WMS_PIP, "actionName: %{public}s", actionName.c_str());
    SingletonContainer::Get<PiPReporter>().ReportPiPActionEvent(pipOption_->GetPipTemplate(), actionName);
    for (auto& listener : pipActionObservers_) {
        listener->OnActionEvent(actionName, status);
    }
    if (CONTROL_TYPE_MAP.find(actionName) != CONTROL_TYPE_MAP.end()) {
        pipOption_->SetPiPControlStatus(CONTROL_TYPE_MAP[actionName], static_cast<PiPControlStatus>(status));
    }
}

void PictureInPictureController::PreRestorePictureInPicture()
{
    TLOGI(WmsLogTag::WMS_PIP, "called");
    curState_ = PiPWindowState::STATE_RESTORING;
    for (auto& listener : pipLifeCycleListeners_) {
        listener->OnRestoreUserInterface();
    }
}

void PictureInPictureController::DoControlEvent(PiPControlType controlType, PiPControlStatus status)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlType:%{public}u, enabled:%{public}d", controlType, status);
    if (pipOption_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "pipOption_ is nullptr");
        return;
    }
    SingletonContainer::Get<PiPReporter>().ReportPiPControlEvent(pipOption_->GetPipTemplate(), controlType);
    for (auto& listener : pipControlObservers_) {
        listener->OnControlEvent(controlType, status);
    }
    pipOption_->SetPiPControlStatus(controlType, status);
}

void PictureInPictureController::RestorePictureInPictureWindow()
{
    StopPictureInPicture(true, StopPipType::NULL_STOP, true);
    SingletonContainer::Get<PiPReporter>().ReportPiPRestore();
    TLOGI(WmsLogTag::WMS_PIP, "restore pip main window finished");
}

void PictureInPictureController::LocateSource()
{
    TLOGI(WmsLogTag::WMS_PIP, "called");
    if (mainWindow_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "main window is nullptr");
        return;
    }
    window_->SetTransparent(true);
    UpdatePiPSourceRect();
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
    if (windowRect_.width_ == 0 || windowRect_.height_ == 0) {
        uint32_t contentWidth = 0;
        uint32_t contentHeight = 0;
        pipOption_->GetContentSize(contentWidth, contentHeight);
        windowRect_.width_ = contentWidth;
        windowRect_.height_ = contentHeight;
    }
    windowRect_.posX_ = static_cast<int32_t>(posX);
    windowRect_.posY_ = static_cast<int32_t>(posY);
    TLOGD(WmsLogTag::WMS_PIP, "position width: %{public}u, height: %{public}u, posX: %{public}d, posY: %{public}d",
        windowRect_.width_, windowRect_.height_, windowRect_.posX_, windowRect_.posY_);
}

void PictureInPictureController::UpdatePiPSourceRect() const
{
    if (useTypeNode_) {
        Rect rect = {0, 0, 0, 0};
        TLOGI(WmsLogTag::WMS_PIP, "use typeNode, unable to locate source rect");
        window_->UpdatePiPRect(rect, WindowSizeChangeReason::PIP_RESTORE);
        return;
    }
    if (mainWindowXComponentController_ == nullptr || window_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "xcomponent controller not valid");
        return;
    }
    float posX = 0;
    float posY = 0;
    float width = 0;
    float height = 0;
    mainWindowXComponentController_->GetGlobalPosition(posX, posY);
    mainWindowXComponentController_->GetSize(width, height);
    Rect rect = { posX, posY, width, height };
    TLOGI(WmsLogTag::WMS_PIP, "result rect: [%{public}d, %{public}d, %{public}u, %{public}u]",
        rect.posX_, rect.posY_, rect.width_, rect.height_);
    window_->UpdatePiPRect(rect, WindowSizeChangeReason::PIP_RESTORE);
}

void PictureInPictureController::ResetExtController()
{
    TLOGI(WmsLogTag::WMS_PIP, "called");
    if (useTypeNode_) {
        TLOGI(WmsLogTag::WMS_PIP, "skip resetExtController as nodeController enabled");
        return;
    }
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
    TLOGI(WmsLogTag::WMS_PIP, "called");
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
    for (auto& listener : pipLifeCycleListeners_) {
        listener->OnPictureInPictureStart();
    }
    return WMError::WM_OK;
}

void PictureInPictureController::OnPictureInPictureStart()
{
    useTypeNode_ = true;
    for (auto& listener : pipLifeCycleListeners_) {
        listener->OnPictureInPictureStart();
    }
}

WMError PictureInPictureController::RegisterPiPLifecycle(const sptr<IPiPLifeCycle>& listener)
{
    return RegisterListener(pipLifeCycleListeners_, listener);
}

WMError PictureInPictureController::RegisterPiPActionObserver(const sptr<IPiPActionObserver>& listener)
{
    return RegisterListener(pipActionObservers_, listener);
}

WMError PictureInPictureController::RegisterPiPControlObserver(const sptr<IPiPControlObserver>& listener)
{
    return RegisterListener(pipControlObservers_, listener);
}

WMError PictureInPictureController::UnregisterPiPLifecycle(const sptr<IPiPLifeCycle>& listener)
{
    return UnregisterListener(pipLifeCycleListeners_, listener);
}

WMError PictureInPictureController::UnregisterPiPActionObserver(const sptr<IPiPActionObserver>& listener)
{
    return UnregisterListener(pipActionObservers_, listener);
}

WMError PictureInPictureController::UnregisterPiPControlObserver(const sptr<IPiPControlObserver>& listener)
{
    return UnregisterListener(pipControlObservers_, listener);
}

template<typename T>
WMError PictureInPictureController::RegisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "listener is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (std::find(holder.begin(), holder.end(), listener) != holder.end()) {
        TLOGE(WmsLogTag::WMS_PIP, "Listener already registered");
        return WMError::WM_OK;
    }
    holder.emplace_back(listener);
    return WMError::WM_OK;
}

template<typename T>
WMError PictureInPictureController::UnregisterListener(std::vector<sptr<T>>& holder, const sptr<T>& listener)
{
    if (listener == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "listener could not be null");
        return WMError::WM_ERROR_NULLPTR;
    }
    holder.erase(std::remove_if(holder.begin(), holder.end(),
        [listener](const sptr<T>& registeredListener) {
            return registeredListener == listener;
        }), holder.end());
    return WMError::WM_OK;
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
    if (remoteObj_ == nullptr) {
        auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityManager == nullptr) {
            TLOGE(WmsLogTag::WMS_PIP, "failed to get registry");
            return ERR_NO_INIT;
        }
        remoteObj_ = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
    }
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

napi_ref PictureInPictureController::GetCustomNodeController()
{
    return pipOption_ == nullptr ? nullptr : pipOption_->GetNodeControllerRef();
}

napi_ref PictureInPictureController::GetTypeNode() const
{
    return pipOption_ == nullptr ? nullptr : pipOption_->GetTypeNodeRef();
}

PictureInPictureController::PiPMainWindowListenerImpl::PiPMainWindowListenerImpl(const sptr<Window> window)
{
    mode_ = window ? window->GetMode() : WindowMode::WINDOW_MODE_UNDEFINED;
    handler_ = std::make_shared<AppExecFwk::EventHandler>(AppExecFwk::EventRunner::GetMainEventRunner());
}

void PictureInPictureController::PiPMainWindowListenerImpl::OnModeChange(WindowMode mode, bool hasDeco)
{
    TLOGI(WmsLogTag::WMS_PIP, "called");
    if (mode == WindowMode::WINDOW_MODE_FULLSCREEN &&
        (mode_ == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode_ == WindowMode::WINDOW_MODE_SPLIT_SECONDARY)) {
        isValid_ = false;
        DelayReset();
    } else {
        isValid_ = true;
    }
    mode_ = mode;
}

void PictureInPictureController::PiPMainWindowListenerImpl::DelayReset()
{
    TLOGI(WmsLogTag::WMS_PIP, "called");
    if (handler_ == nullptr) {
        TLOGW(WmsLogTag::WMS_PIP, "task is not append to queue");
        isValid_ = true;
        return;
    }
    auto delayTask = [weakThis = wptr(this)]() {
        auto listener = weakThis.promote();
        if (!listener) {
            TLOGE(WmsLogTag::WMS_PIP, "listener is nullptr");
            return;
        }
        listener->isValid_ = true;
        TLOGI(WmsLogTag::WMS_PIP, "reset to valid");
    };
    handler_->PostTask(delayTask, "wms:PiPMainWindowListenerImpl_DelayReset", DELAY_RESET);
}

WindowMode PictureInPictureController::PiPMainWindowListenerImpl::GetMode()
{
    return mode_;
}

bool PictureInPictureController::PiPMainWindowListenerImpl::IsValid()
{
    return isValid_;
}
} // namespace Rosen
} // namespace OHOS