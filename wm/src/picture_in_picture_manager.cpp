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

#include "picture_in_picture_manager.h"

#include <refbase.h>
#include "picture_in_picture_controller.h"
#include "window.h"
#include "window_manager_hilog.h"
#include "window_scene_session_impl.h"
#include "wm_common.h"
#include "scene_board_judgement.h"

namespace OHOS {
namespace Rosen {
namespace {
    const std::string ACTION_CLOSE = "close";
    const std::string ACTION_RESTORE = "restore";
}

sptr<PictureInPictureController> PictureInPictureManager::activeController_ = nullptr;
wptr<PictureInPictureController> PictureInPictureManager::autoStartController_ = nullptr;
std::map<int32_t, wptr<PictureInPictureController>> PictureInPictureManager::autoStartControllerMap_ = {};
std::map<int32_t, sptr<PictureInPictureController>> PictureInPictureManager::windowToControllerMap_ = {};
sptr<IWindowLifeCycle> PictureInPictureManager::mainWindowLifeCycleImpl_;
std::shared_mutex PictureInPictureManager::controllerMapMutex_;
std::recursive_mutex PictureInPictureManager::mutex_;

PictureInPictureManager::PictureInPictureManager()
{
}

PictureInPictureManager::~PictureInPictureManager()
{
}

bool PictureInPictureManager::IsSupportPiP()
{
    return SceneBoardJudgement::IsSceneBoardEnabled();
}

bool PictureInPictureManager::ShouldAbortPipStart()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return activeController_ != nullptr && activeController_->GetControllerState() == PiPWindowState::STATE_STARTING;
}

void PictureInPictureManager::PutPipControllerInfo(int32_t windowId, sptr<PictureInPictureController> pipController)
{
    TLOGD(WmsLogTag::WMS_PIP, "PutPipControllerInfo called, windowId %{public}u", windowId);
    std::unique_lock<std::shared_mutex> lock(controllerMapMutex_);
    windowToControllerMap_.insert(std::make_pair(windowId, pipController));
}

void PictureInPictureManager::RemovePipControllerInfo(int32_t windowId)
{
    TLOGD(WmsLogTag::WMS_PIP, "RemovePipControllerInfo called, windowId %{public}u", windowId);
    std::unique_lock<std::shared_mutex> lock(controllerMapMutex_);
    windowToControllerMap_.erase(windowId);
}

sptr<PictureInPictureController> PictureInPictureManager::GetPipControllerInfo(int32_t windowId)
{
    std::shared_lock<std::shared_mutex> lock(controllerMapMutex_);
    if (windowToControllerMap_.empty() || windowToControllerMap_.find(windowId) == windowToControllerMap_.end()) {
        TLOGE(WmsLogTag::WMS_PIP, "GetPipControllerInfo error, %{public}d not registered!", windowId);
        return nullptr;
    }
    return windowToControllerMap_[windowId];
}

bool PictureInPictureManager::HasActiveController()
{
    return activeController_ != nullptr;
}

bool PictureInPictureManager::IsActiveController(wptr<PictureInPictureController> pipController)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!HasActiveController()) {
        return false;
    }
    bool res = pipController.GetRefPtr() == activeController_.GetRefPtr();
    TLOGD(WmsLogTag::WMS_PIP, "IsActiveController %{public}u", res);
    return res;
}

void PictureInPictureManager::SetActiveController(sptr<PictureInPictureController> pipController)
{
    TLOGD(WmsLogTag::WMS_PIP, "SetActiveController called");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    activeController_ = pipController;
}

void PictureInPictureManager::RemoveActiveController(wptr<PictureInPictureController> pipController)
{
    TLOGD(WmsLogTag::WMS_PIP, "RemoveActiveController called");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!IsActiveController(pipController)) {
        return;
    }
    activeController_ = nullptr;
}

void PictureInPictureManager::AttachAutoStartController(int32_t handleId,
    wptr<PictureInPictureController> pipController)
{
    TLOGD(WmsLogTag::WMS_PIP, "AttachAutoStartController, %{public}u", handleId);
    if (pipController == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (autoStartController_ != nullptr && mainWindowLifeCycleImpl_ != nullptr) {
        sptr<WindowSessionImpl> previousMainWindow = WindowSceneSessionImpl::GetMainWindowWithId(
            autoStartController_->GetMainWindowId());
        if (previousMainWindow != nullptr) {
            previousMainWindow->UnregisterLifeCycleListener(mainWindowLifeCycleImpl_);
        }
    }
    autoStartController_ = pipController;
    sptr<WindowSessionImpl> mainWindow = WindowSceneSessionImpl::GetMainWindowWithId(
        autoStartController_->GetMainWindowId());
    if (mainWindow != nullptr) {
        mainWindowLifeCycleImpl_ = new PictureInPictureController::PipMainWindowLifeCycleImpl(
            pipController->GetPiPNavigationId());
        mainWindow->RegisterLifeCycleListener(mainWindowLifeCycleImpl_);
    }
    autoStartControllerMap_[handleId] = pipController;
}

void PictureInPictureManager::DetachAutoStartController(int32_t handleId,
    wptr<PictureInPictureController> pipController)
{
    TLOGD(WmsLogTag::WMS_PIP, "Detach active pipController, %{public}u", handleId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    autoStartControllerMap_.erase(handleId);
    if (autoStartController_ == nullptr) {
        return;
    }
    if (pipController != nullptr &&
        pipController.GetRefPtr() != autoStartController_.GetRefPtr()) {
        TLOGE(WmsLogTag::WMS_PIP, "not same pip controller or no active pip controller");
        return;
    }
    sptr<WindowSessionImpl> mainWindow = WindowSceneSessionImpl::GetMainWindowWithId(
        autoStartController_->GetMainWindowId());
    if (mainWindow != nullptr && mainWindowLifeCycleImpl_ != nullptr) {
        mainWindow->UnregisterLifeCycleListener(mainWindowLifeCycleImpl_);
    }
    autoStartController_ = nullptr;
}

bool PictureInPictureManager::IsAttachedToSameWindow(uint32_t windowId)
{
    TLOGD(WmsLogTag::WMS_PIP, "IsAttachedToSameWindow called %{public}u", windowId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!HasActiveController()) {
        return false;
    }
    return activeController_->GetMainWindowId() == windowId;
}

sptr<Window> PictureInPictureManager::GetCurrentWindow()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!HasActiveController()) {
        return nullptr;
    }
    return activeController_->GetPipWindow();
}

void PictureInPictureManager::DoRestore()
{
    TLOGD(WmsLogTag::WMS_PIP, "DoRestore is called");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!HasActiveController()) {
        return;
    }
    activeController_->RestorePictureInPictureWindow();
}

void PictureInPictureManager::DoClose(bool destroyWindow, bool byPriority)
{
    TLOGD(WmsLogTag::WMS_PIP, "DoClose is called");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!HasActiveController()) {
        return;
    }
    StopPipType currentStopType = StopPipType::NULL_STOP;
    if (!byPriority) {
        currentStopType = StopPipType::USER_STOP;
    } else {
        currentStopType = StopPipType::OTHER_PACKAGE_STOP;
    }
    activeController_->StopPictureInPicture(destroyWindow, currentStopType);
}

void PictureInPictureManager::DoActionEvent(const std::string& actionName, int32_t status)
{
    TLOGD(WmsLogTag::WMS_PIP, "DoActionEvent is called");
    if (!HasActiveController()) {
        return;
    }
    if (actionName.c_str() == ACTION_CLOSE) {
        DoClose(true, false);
    } else if (actionName.c_str() == ACTION_RESTORE) {
        DoRestore();
    } else {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        activeController_->DoActionEvent(actionName, status);
    }
}

void PictureInPictureManager::AutoStartPipWindow(std::string navigationId)
{
    TLOGD(WmsLogTag::WMS_PIP, "AutoStartPipWindow is called, navId: %{public}s", navigationId.c_str());
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (autoStartController_ == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "autoStartController_ is null");
        return;
    }
    if (navigationId == "") {
        TLOGI(WmsLogTag::WMS_PIP, "No use navigationId for auto start");
        autoStartController_->StartPictureInPicture(StartPipType::AUTO_START);
        return;
    }
    sptr<WindowSessionImpl> mainWindow = WindowSceneSessionImpl::GetMainWindowWithId(
        autoStartController_->GetMainWindowId());
    if (mainWindow) {
        auto navController = NavigationController::GetNavigationController(mainWindow->GetUIContent(), navigationId);
        if (!navController) {
            TLOGE(WmsLogTag::WMS_PIP, "navController is nullptr");
            return;
        }
        if (navController->IsNavDestinationInTopStack()) {
            int handleId = navController->GetTopHandle();
            if (autoStartControllerMap_.empty() ||
                autoStartControllerMap_.find(handleId) == autoStartControllerMap_.end()) {
                TLOGE(WmsLogTag::WMS_PIP, "GetNAvController info error, %{public}d not registered", handleId);
                return;
            }
            auto pipController = autoStartControllerMap_[handleId];
            pipController->StartPictureInPicture(StartPipType::AUTO_START);
        } else {
            TLOGE(WmsLogTag::WMS_PIP, "Top is not navDestination");
        }
    }
}
}
}
