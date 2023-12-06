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

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "PictureInPictureManager"};
}

sptr<PictureInPictureController> PictureInPictureManager::activeController_ = nullptr;
sptr<PictureInPictureController> PictureInPictureManager::autoStartController_ = nullptr; // todo delete
std::map<std::string, sptr<PictureInPictureController>> PictureInPictureManager::autoStartControllerMap_ = {};
std::map<int32_t, sptr<PictureInPictureController>> PictureInPictureManager::windowToControllerMap_ = {}; // todo delete
sptr<IWindowLifeCycle> PictureInPictureManager::mainWindowLifeCycleImpl_;

PictureInPictureManager::PictureInPictureManager()
{
}

PictureInPictureManager::~PictureInPictureManager()
{
}

bool PictureInPictureManager::ShouldAbortPipStart()
{
    return activeController_ != nullptr && activeController_->GetControllerState() == PipWindowState::STATE_STARTING;
}

void PictureInPictureManager::PutPipControllerInfo(int32_t windowId, sptr<PictureInPictureController> pipController)
{
    WLOGD("PutPipControllerInfo called, windowId %{public}u", windowId);
    windowToControllerMap_.insert(std::make_pair(windowId, pipController));
}

void PictureInPictureManager::RemovePipControllerInfo(int32_t windowId)
{
    WLOGD("RemovePipControllerInfo called, windowId %{public}u", windowId);
    windowToControllerMap_.erase(windowId);
}

sptr<PictureInPictureController> PictureInPictureManager::GetPipControllerInfo(int32_t windowId)
{
    WLOGD("GetPipControllerInfo called");
    if (windowToControllerMap_.empty() || windowToControllerMap_.find(windowId) == windowToControllerMap_.end()) {
        WLOGE("GetPipControllerInfo error, %{public}d not registered!", windowId);
        return nullptr;
    }
    return windowToControllerMap_[windowId];
}

bool PictureInPictureManager::HasActiveController()
{
    WLOGD("HasActiveController called");
    return activeController_ != nullptr;
}

bool PictureInPictureManager::IsActiveController(wptr<PictureInPictureController> pipController)
{
    WLOGD("IsActiveController called");
    if (!HasActiveController()) {
        return false;
    }
    return pipController.GetRefPtr() == activeController_.GetRefPtr();
}

void PictureInPictureManager::SetActiveController(sptr<PictureInPictureController> pipController)
{
    WLOGD("SetActiveController called");
    activeController_ = pipController;
}

void PictureInPictureManager::RemoveActiveController()
{
    WLOGD("RemoveActiveController called");
    activeController_ = nullptr;
}

void PictureInPictureManager::RemoveActiveControllerSafe()
{
    WLOGD("RemoveActiveControllerSafe called");
    if (!HasActiveController()) {
        return;
    }
    activeController_->SetPipWindow(nullptr);
    RemoveActiveController();
}

void PictureInPictureManager::AttachAutoStartController(std::string pageName, sptr<PictureInPictureController> pipController)
{
    WLOGD("AttachAutoStartController called");
    if (pipController == nullptr) {
        return;
    }
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
        mainWindowLifeCycleImpl_ = new PictureInPictureController::PipMainWindowLifeCycleImpl();
        mainWindow -> RegisterLifeCycleListener(mainWindowLifeCycleImpl_);
    }
    autoStartControllerMap_.insert(std::make_pair(pageName, pipController));
}

void PictureInPictureManager::DetachAutoStartController(std::string pageName, sptr<PictureInPictureController> pipController)
{
    WLOGD("Detach active pipController");
    if (pipController != nullptr &&
        pipController.GetRefPtr() != autoStartController_.GetRefPtr()) {
        WLOGFE("not same pip controller or no active pip controller");
        return;
    }
    sptr<WindowSessionImpl> mainWindow = WindowSceneSessionImpl::GetMainWindowWithId(
        autoStartController_->GetMainWindowId());
    if (mainWindow != nullptr && mainWindowLifeCycleImpl_ != nullptr) {
        mainWindow->UnregisterLifeCycleListener(mainWindowLifeCycleImpl_);
    }
    autoStartControllerMap_.erase(pageName);
    autoStartController_ = nullptr;
}

bool PictureInPictureManager::IsAttachedToSameWindow(uint32_t windowId)
{
    WLOGD("IsAttachedToSameWindow called");
    if (!HasActiveController()) {
        return false;
    }
    return activeController_->GetMainWindowId() == windowId;
}

sptr<Window> PictureInPictureManager::GetCurrentWindow()
{
    WLOGD("GetCurrentWindow is called");
    if (!HasActiveController()) {
        return nullptr;
    }
    return activeController_->GetPipWindow();
}

void PictureInPictureManager::DoRestore()
{
    WLOGD("DoRestore is called");
    if (!HasActiveController()) {
        return;
    }
    activeController_->RestorePictureInPictureWindow();
}

void PictureInPictureManager::DoClose(bool destroyWindow, bool needAnim)
{
    WLOGD("DoClose is called");
    if (!HasActiveController()) {
        return;
    }
    StopPipType currentStopType = StopPipType::NULL_STOP;
    if (needAnim) {
        currentStopType = StopPipType::USER_STOP;
    } else {
        currentStopType = StopPipType::OTHER_PACKAGE_STOP;
    }
    activeController_->StopPictureInPicture(destroyWindow, needAnim, currentStopType);
}

void PictureInPictureManager::DoStartMove()
{
    WLOGD("DoStartMove is called");
    if (!HasActiveController()) {
        return;
    }
    activeController_->StartMove();
}

void PictureInPictureManager::DoScale()
{
    WLOGD("DoScale is called");
    if (!HasActiveController()) {
        return;
    }
    activeController_->DoScale();
}

void PictureInPictureManager::DoActionEvent(std::string actionName)
{
    WLOGD("DoActionEvent is called");
    if (!HasActiveController()) {
        return;
    }
    activeController_->DoActionEvent(actionName);
}

void PictureInPictureManager::AutoStartPipWindow()
{
    WLOGD("AutoStartPipWindow is called");
    if (autoStartController_ == nullptr) {
        WLOGFE("autoStartController_ is null");
        return;
    }
    autoStartController_ -> StartPictureInPicture(StartPipType::AUTO_START);
}
}
}
}
