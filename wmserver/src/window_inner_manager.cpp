/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "window_inner_manager.h"

#include "ui_service_mgr_client.h"
#include "window_manager_hilog.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowInnerManager"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowInnerManager)

WindowInnerManager::WindowInnerManager() : eventHandler_(nullptr), eventLoop_(nullptr),
    state_(InnerWMRunningState::STATE_NOT_START)
{
}

WindowInnerManager::~WindowInnerManager()
{
    Stop();
}

bool WindowInnerManager::Init()
{
    // create handler for inner command at server
    eventLoop_ = AppExecFwk::EventRunner::Create(INNER_WM_THREAD_NAME);
    if (eventLoop_ == nullptr) {
        return false;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    if (eventHandler_ == nullptr) {
        return false;
    }

    moveDragController_ = new MoveDragController();
    if (!moveDragController_->Init()) {
        WLOGFE("Init window drag controller failed");
        return false;
    }

    WLOGFI("init window inner manager service success.");
    return true;
}

void WindowInnerManager::Start(bool enableRecentholder)
{
    isRecentHolderEnable_ = enableRecentholder;
    if (state_ == InnerWMRunningState::STATE_RUNNING) {
        WLOGFI("window inner manager service has already started.");
    }
    if (!Init()) {
        WLOGFI("failed to init window inner manager service.");
        return;
    }
    state_ = InnerWMRunningState::STATE_RUNNING;
    eventLoop_->Run();

    pid_ = getpid();
    WLOGFI("window inner manager service start success.");
}

void WindowInnerManager::Stop()
{
    WLOGFI("stop window inner manager service.");
    if (eventLoop_ != nullptr) {
        eventLoop_->Stop();
        eventLoop_.reset();
    }
    if (eventHandler_ != nullptr) {
        eventHandler_.reset();
    }
    moveDragController_->Stop();
    state_ = InnerWMRunningState::STATE_NOT_START;
}

void WindowInnerManager::CreateInnerWindow(std::string name, DisplayId displayId, Rect rect,
    WindowType type, WindowMode mode)
{
    eventHandler_->PostTask([this, name, displayId, rect, mode, type]() {
        switch (type) {
            case WindowType::WINDOW_TYPE_PLACEHOLDER: {
                if (isRecentHolderEnable_) {
                    PlaceHolderWindow::GetInstance().Create(name, displayId, rect, mode);
                }
                break;
            }
            case WindowType::WINDOW_TYPE_DOCK_SLICE: {
                DividerWindow::GetInstance().Create(name, displayId, rect, mode);
                DividerWindow::GetInstance().Update(rect.width_, rect.height_);
                break;
            }
            default:
                break;
        }
    });
    return;
}

void WindowInnerManager::DestroyInnerWindow(DisplayId displayId, WindowType type)
{
    eventHandler_->PostTask([this, type]() {
        switch (type) {
            case WindowType::WINDOW_TYPE_PLACEHOLDER: {
                if (isRecentHolderEnable_) {
                    PlaceHolderWindow::GetInstance().Destroy();
                }
                break;
            }
            case WindowType::WINDOW_TYPE_DOCK_SLICE: {
                DividerWindow::GetInstance().Destroy();
                break;
            }
            default:
                break;
        }
    });
    return;
}

void WindowInnerManager::UpdateInnerWindow(DisplayId displayId, WindowType type, uint32_t width, uint32_t height)
{
    eventHandler_->PostTask([this, type, width, height]() {
        switch (type) {
            case WindowType::WINDOW_TYPE_PLACEHOLDER: {
                if (isRecentHolderEnable_) {
                    PlaceHolderWindow::GetInstance().Update(width, height);
                }
                break;
            }
            case WindowType::WINDOW_TYPE_DOCK_SLICE: {
                DividerWindow::GetInstance().Update(width, height);
                break;
            }
            default:
                break;
        }
    });
    return;
}

pid_t WindowInnerManager::GetPid()
{
    return pid_;
}

bool WindowInnerManager::NotifyWindowReadyToMoveOrDrag(uint32_t windowId, sptr<WindowProperty>& windowProperty,
    sptr<MoveDragProperty>& moveDragProperty)
{
    if (moveDragController_->GetActiveWindowId() != INVALID_WINDOW_ID) {
        WLOGFW("Is already in dragging or moving state, invalid operation");
        return false;
    }
    moveDragController_->HandleReadyToMoveOrDrag(windowId, windowProperty, moveDragProperty);
    WLOGFI("NotifyWindowReadyToMoveOrDrag, windowId: %{public}u", windowId);
    return true;
}

void WindowInnerManager::NotifyWindowEndUpMovingOrDragging(uint32_t windowId)
{
    if (moveDragController_->GetActiveWindowId() != windowId) {
        return;
    }
    moveDragController_->HandleEndUpMovingOrDragging(windowId);
    WLOGFI("NotifyWindowEndUpMovingOrDragging, windowId: %{public}u", windowId);
}

void WindowInnerManager::NotifyWindowRemovedOrDestroyed(uint32_t windowId)
{
    if (moveDragController_->GetActiveWindowId() != windowId) {
        return;
    }
    moveDragController_->HandleWindowRemovedOrDestroyed(windowId);
    WLOGFI("NotifyWindowRemovedOrDestroyed, windowId: %{public}u", windowId);
}

void WindowInnerManager::ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    uint32_t windowId = static_cast<uint32_t>(pointerEvent->GetAgentWindowId());
    if (moveDragController_->GetActiveWindowId() != windowId ||
        moveDragController_->GetActiveWindowId() == INVALID_WINDOW_ID) {
        WLOGFE("active winId or inputEvent winId is invalid, windowId: %{public}u, activeWinId: %{public}u",
            windowId, moveDragController_->GetActiveWindowId());
        return;
    }
    moveDragController_->ConsumePointerEvent(pointerEvent);
}
} // Rosen
} // OHOS