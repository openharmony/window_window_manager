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

sptr<PictureInPictureController> PictureInPictureManager::curPipController_ = nullptr;
sptr<PictureInPictureController> PictureInPictureManager::activePipController_ = nullptr;
std::map<int32_t, sptr<PictureInPictureController>> PictureInPictureManager::windowToControllerMap_ = {};
std::mutex PictureInPictureManager::pipWindowStateMutex_;
PipWindowState PictureInPictureManager::pipWindowState_ = PipWindowState::STATE_UNDEFINED;
sptr<IWindowLifeCycle> PictureInPictureManager::mainWindowLifeCycleImpl_;

PictureInPictureManager::PictureInPictureManager()
{
}

PictureInPictureManager::~PictureInPictureManager()
{
}

void PictureInPictureManager::PutPipControllerInfo(int32_t windowId, sptr<PictureInPictureController> pipController)
{
    WLOGD("PutPipControllerInfo is called, windowId %{public}u", windowId);
    windowToControllerMap_.insert(std::make_pair(windowId, pipController));
}

void PictureInPictureManager::RemovePipControllerInfo(int32_t windowId)
{
    WLOGD("RemovePipControllerInfo is called, windowId %{public}u", windowId);
    windowToControllerMap_.erase(windowId);
}

void PictureInPictureManager::SetPipWindowState(PipWindowState pipWindowState)
{
    WLOGD("SetPipWindowState is called, state %{public}u", pipWindowState);
    std::lock_guard<std::mutex> lock(PictureInPictureManager::pipWindowStateMutex_);
    pipWindowState_ = pipWindowState;
}

PipWindowState PictureInPictureManager::GetPipWindowState()
{
    WLOGD("GetPipWindowState is called");
    std::lock_guard<std::mutex> lock(PictureInPictureManager::pipWindowStateMutex_);
    return pipWindowState_;
}

bool PictureInPictureManager::IsCurrentPipControllerExist()
{
    WLOGD("IsCurrentPipControllerExist is called");
    return curPipController_ != nullptr;
}

bool PictureInPictureManager::IsCurrentPipController(wptr<PictureInPictureController> pipController)
{
    WLOGD("IsCurrentPipController is called");
    if (!IsCurrentPipControllerExist()) {
        return false;
    }
    return pipController.GetRefPtr() == curPipController_.GetRefPtr();
}

void PictureInPictureManager::SetCurrentPipController(sptr<PictureInPictureController> pipController)
{
    WLOGD("SetCurrentPipController is called");
    curPipController_ = pipController;
}

void PictureInPictureManager::RemoveCurrentPipController()
{
    WLOGD("RemoveCurrentPipController is called");
    curPipController_ = nullptr;
}

void PictureInPictureManager::RemoveCurrentPipControllerSafety()
{
    WLOGD("RemoveCurrentPipControllerSafety is called");
    if (!IsCurrentPipControllerExist()) {
        return;
    }
    curPipController_->SetPipWindow(nullptr);
    RemoveCurrentPipController();
}

void PictureInPictureManager::AttachActivePipController(sptr<PictureInPictureController> pipController)
{
    WLOGD("Attach active pipController");
    if (pipController == nullptr) {
        return;
    }
    if (activePipController_ != nullptr && mainWindowLifeCycleImpl_ != nullptr) {
        sptr<WindowSessionImpl> previousMainWindow = WindowSceneSessionImpl::GetMainWindowWithId(
            activePipController_ -> GetMainWindowId());
        if (previousMainWindow != nullptr) {
            previousMainWindow -> UnregisterLifeCycleListener(mainWindowLifeCycleImpl_);
        }
    }
    activePipController_ = pipController;
    sptr<WindowSessionImpl> mainWindow = WindowSceneSessionImpl::GetMainWindowWithId(
        activePipController_ -> GetMainWindowId());
    if (mainWindow != nullptr) {
        mainWindowLifeCycleImpl_ = new PictureInPictureController::PipMainWindowLifeCycleImpl();
        mainWindow -> RegisterLifeCycleListener(mainWindowLifeCycleImpl_);
    }
}

void PictureInPictureManager::DetachActivePipController(sptr<PictureInPictureController> pipController)
{
    WLOGD("Detach active pipController");
    if (pipController != nullptr &&
        pipController.GetRefPtr() != activePipController_.GetRefPtr()) {
        WLOGFE("not same pip controller or no active pip controller");
        return;
    }
    sptr<WindowSessionImpl> mainWindow = WindowSceneSessionImpl::GetMainWindowWithId(
        activePipController_ -> GetMainWindowId());
    if (mainWindow != nullptr && mainWindowLifeCycleImpl_ != nullptr) {
        mainWindow -> UnregisterLifeCycleListener(mainWindowLifeCycleImpl_);
    }
    activePipController_ = nullptr;
}

bool PictureInPictureManager::IsAttachedPipWindow(uint32_t windowId)
{
    WLOGD("IsAttachedPipWindow is called");
    if (!IsCurrentPipControllerExist()) {
        return false;
    }
    return curPipController_->GetMainWindowId() == windowId;
}

sptr<Window> PictureInPictureManager::GetCurrentWindow()
{
    WLOGD("GetCurrentWindow is called");
    if (!IsCurrentPipControllerExist()) {
        return nullptr;
    }
    return curPipController_->GetPipWindow();
}

bool PictureInPictureManager::IsActiveController(wptr<PictureInPictureController> pipController)
{
    return pipController.GetRefPtr() == activePipController_.GetRefPtr();
}

void PictureInPictureManager::DoRestore()
{
    WLOGD("DoRestore is called");
    if (!PictureInPictureManager::IsCurrentPipControllerExist()) {
        return;
    }
    PictureInPictureManager::curPipController_->RestorePictureInPictureWindow();
}

void PictureInPictureManager::DoClose(bool needAnim)
{
    WLOGD("DoClose is called");
    if (!PictureInPictureManager::IsCurrentPipControllerExist()) {
        return;
    }
    curPipController_->StopPictureInPicture(needAnim);
}

void PictureInPictureManager::DoStartMove()
{
    WLOGD("DoStartMove is called");
    if (!PictureInPictureManager::IsCurrentPipControllerExist()) {
        return;
    }
    PictureInPictureManager::curPipController_->StartMove();
}

void PictureInPictureManager::DoScale()
{
    WLOGD("DoScale is called");
    if (!PictureInPictureManager::IsCurrentPipControllerExist()) {
        return;
    }
    PictureInPictureManager::curPipController_->DoScale();
}

void PictureInPictureManager::DoActionEvent(std::string actionName)
{
    WLOGD("DoActionEvent is called");
    if (!PictureInPictureManager::IsCurrentPipControllerExist()) {
        return;
    }
    PictureInPictureManager::curPipController_->DoActionEvent(actionName);
}

void PictureInPictureManager::AutoStartPipWindow()
{
    WLOGD("AutoStartPipWindow is called");
    if (activePipController_ == nullptr) {
        WLOGFE("activePipController_ is null");
        return;
    }
    activePipController_ -> StartPictureInPicture();
}
}
}
