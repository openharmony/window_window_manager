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

#include "inner_window.h"

#include "ui_service_mgr_client.h"
#include "window_manager_hilog.h"
#include "surface_draw.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "InnerWindow"};
    const std::string IMAGE_PLACE_HOLDER_PNG_PATH = "/etc/window/resources/bg_place_holder.png";
}
WM_IMPLEMENT_SINGLE_INSTANCE(InnerWindowFactory)
WMError InnerWindowFactory::CreateInnerWindow(std::string name, DisplayId displyId, Rect rect,
    WindowType type, WindowMode mode)
{
    WLOGFI("createInnerWindow begin type: %{public}u", type);
    if ((type != WindowType::WINDOW_TYPE_DOCK_SLICE) && (type !=  WindowType::WINDOW_TYPE_PLACE_HOLDER)) {
        WLOGFE("create inner window failed, current type: %{public}u not surpport.", type);
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    if (innerWindowMap_.find(type) != std::end(innerWindowMap_)) {
        if (innerWindowMap_[type] != nullptr && innerWindowMap_[type]->GetState() ==
            InnerWindowState::INNER_WINDOW_STATE_CRATED) {
            WLOGFW("create inner window failed, current inner window type %{public}u has created.", type);
            return WMError::WM_ERROR_INVALID_TYPE;
        }
        innerWindowMap_.erase(type);
    }
    switch (type) {
        case WindowType::WINDOW_TYPE_DOCK_SLICE : {
            innerWindowMap_[type] = std::make_unique<DividerWindow>(name, displyId, rect);
            innerWindowMap_[type]->Create();
            break;
        }
        case WindowType::WINDOW_TYPE_PLACE_HOLDER : {
            innerWindowMap_[type] = std::make_unique<PlaceHolderWindow>(name, displyId, rect, mode);
            innerWindowMap_[type]->Create();
            break;
        }
        default :
            break;
    }
    WLOGFI("createInnerWindow end");
    return WMError::WM_OK;
}

WMError InnerWindowFactory::DestroyInnerWindow(DisplayId displyId, WindowType type)
{
    WLOGFI("destroy inner window type: %{public}u begin.", type);
    if (innerWindowMap_.find(type) == std::end(innerWindowMap_)) {
        WLOGFW("current inner window has created");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (innerWindowMap_[type] != nullptr) {
        innerWindowMap_[type]->Destroy();
    }
    innerWindowMap_.erase(type);
    WLOGFI("destroy inner window type: %{public}u end.", type);
    return WMError::WM_OK;
}

PlaceHolderWindow::~PlaceHolderWindow()
{
    Destroy();
}

void PlaceHolderWindow::OnTouchOutside()
{
    Destroy();
}

void PlaceHolderWindow::OnKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    Destroy();
}

void PlaceHolderWindow::OnPointerInputEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    Destroy();
}

void PlaceHolderWindow::AfterUnfocused()
{
    Destroy();
}

void PlaceHolderWindow::Create()
{
    WLOGFI("create inner display id: %{public}" PRIu64"", displayId_);
    if (window_ != nullptr) {
        WLOGFW("window has created.");
        window_->Show();
        return;
    }
    WLOGFD("create palce holder Window start");
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    if (option == nullptr) {
        WLOGFE("window option is nullptr.");
        return;
    }
    option->SetWindowType(WindowType::WINDOW_TYPE_PLACE_HOLDER);
    option->SetWindowMode(mode_);
    option->SetFocusable(false);
    option->SetWindowRect(rect_);
    window_ = Window::Create(name_, option);
    if (window_ == nullptr) {
        WLOGFE("window is nullptr.");
        return;
    }
    RegitsterWindowListener();
    if (!OHOS::Rosen::SurfaceDraw::DrawImage(window_->GetSurfaceNode(), rect_.width_, rect_.height_,
        IMAGE_PLACE_HOLDER_PNG_PATH)) {
        WLOGE("draw surface failed");
        return;
    }
    window_->Show();
    state_ = InnerWindowState::INNER_WINDOW_STATE_CRATED;
    WLOGFD("create palce holder Window end");
}

void PlaceHolderWindow::RegitsterWindowListener()
{
    if (window_ == nullptr) {
        WLOGFE("Window is nullptr, regitster window listener failed.");
        return;
    }
    window_->RegisterTouchOutsideListener(this);
    window_->RegisterInputEventListener(this);
    window_->RegisterLifeCycleListener(this);
}

void PlaceHolderWindow::UnRegitsterWindowListener()
{
    if (window_ == nullptr) {
        WLOGFE("Window is nullptr, unregitster window listener failed.");
        return;
    }
    window_->UnregisterTouchOutsideListener(this);
    window_->UnregisterInputEventListener(this);
    window_->UnregisterLifeCycleListener(this);
}

void PlaceHolderWindow::Destroy()
{
    UnRegitsterWindowListener();
    if (window_ != nullptr) {
        window_->Destroy();
    }
    window_ = nullptr;
    state_ = InnerWindowState::INNER_WINDOW_STATE_DESTROYED;
}

DividerWindow::~DividerWindow()
{
    Destroy();
}

void DividerWindow::Create()
{
    WLOGFI("create inner display id: %{public}" PRIu64"", displayId_);
    auto dialogCallback = [this](int32_t id, const std::string& event, const std::string& params) {
        if (params == "EVENT_CANCEL_CODE") {
            Ace::UIServiceMgrClient::GetInstance()->CancelDialog(id);
        }
    };
    std::string params;
    Ace::UIServiceMgrClient::GetInstance()->ShowDialog(name_, params, WindowType::WINDOW_TYPE_DOCK_SLICE,
        rect_.posX_, rect_.posY_, rect_.width_, rect_.height_, dialogCallback, &dialogId_);
    state_ = InnerWindowState::INNER_WINDOW_STATE_CRATED;
    WLOGFI("create inner window id: %{public}d success", dialogId_);
}

void DividerWindow::Destroy()
{
    if (dialogId_ == -1) {
        state_ = InnerWindowState::INNER_WINDOW_STATE_DESTROYED;
        return;
    }
    WLOGFI("destroy inner window id:: %{public}d.", dialogId_);
    Ace::UIServiceMgrClient::GetInstance()->CancelDialog(dialogId_);
    dialogId_ = -1;
    state_ = InnerWindowState::INNER_WINDOW_STATE_DESTROYED;
}
} // Rosen
} // OHOS