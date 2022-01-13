/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "window_controller.h"
#include <transaction/rs_transaction.h>
#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowController"};
}
uint32_t WindowController::GenWindowId()
{
    return ++windowId_;
}

WMError WindowController::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId)
{
    windowId = GenWindowId();
    property->SetWindowId(windowId);

    sptr<WindowNode> node = new WindowNode(property, window, surfaceNode);
    return windowRoot_->SaveWindow(node);
}

WMError WindowController::SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->abilityToken_ = abilityToken;
    return WMError::WM_OK;
}

WMError WindowController::AddWindowNode(sptr<WindowProperty>& property)
{
    auto node = windowRoot_->GetWindowNode(property->GetWindowId());
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->SetWindowProperty(property);
    // TODO: check permission
    // TODO: adjust property

    WMError res = windowRoot_->AddWindowNode(property->GetParentId(), node);
    if (res != WMError::WM_OK) {
        return res;
    }
    RSTransaction::FlushImplicitTransaction();

    if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) {
        WM_SCOPED_TRACE_BEGIN("controller:MinimizeOtherFullScreenAbility");
        WMError res = windowRoot_->MinimizeOtherFullScreenAbility(node);
        WM_SCOPED_TRACE_END();
        if (res != WMError::WM_OK) {
            WLOGFE("Minimize other fullscreen window failed");
        }
    }
    return res;
}

WMError WindowController::RemoveWindowNode(uint32_t windowId)
{
    WMError res = windowRoot_->RemoveWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    RSTransaction::FlushImplicitTransaction();
    return res;
}

WMError WindowController::DestroyWindow(uint32_t windowId)
{
    WMError res = windowRoot_->DestroyWindow(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    RSTransaction::FlushImplicitTransaction();
    return res;
}

WMError WindowController::MoveTo(uint32_t windowId, int32_t x, int32_t y)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto property = node->GetWindowProperty();
    Rect lastRect = property->GetWindowRect();
    Rect newRect = { x, y, lastRect.width_, lastRect.height_ };
    property->SetWindowRect(newRect);
    WMError res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

WMError WindowController::Resize(uint32_t windowId, uint32_t width, uint32_t height)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto property = node->GetWindowProperty();
    Rect lastRect = property->GetWindowRect();
    Rect newRect = { lastRect.posX_, lastRect.posY_, width, height };
    property->SetWindowRect(newRect);
    WMError res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

WMError WindowController::RequestFocus(uint32_t windowId)
{
    return windowRoot_->RequestFocus(windowId);
}

WMError WindowController::SetWindowMode(uint32_t windowId, WindowMode dstMode)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (node->GetWindowMode() == dstMode) {
        return WMError::WM_OK;
    }
    WMError res = WMError::WM_OK;
    if (node->IsSplitMode()) {
        // change split mode to other
        node->SetWindowMode(dstMode);
        res = windowRoot_->HandleSplitWindowModeChange(node, false);
    } else if (!node->IsSplitMode() &&
        (dstMode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || dstMode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY)) {
        // change other mode to split
        node->SetWindowMode(dstMode);
        res = windowRoot_->HandleSplitWindowModeChange(node, true);
    } else {
        node->SetWindowMode(dstMode);
    }
    if (res != WMError::WM_OK) {
        WLOGFE("HandleSplitWindowModeChange failed, resume last window mode");
        node->GetWindowProperty()->ResumeLastWindowMode();
        return res;
    }
    res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

WMError WindowController::MinimizeAllAppNodeAbility(uint32_t windowId)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("Count node find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    return windowRoot_->MinimizeAllAppNodeAbility(node);
}

WMError WindowController::SetWindowType(uint32_t windowId, WindowType type)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto property = node->GetWindowProperty();
    property->SetWindowType(type);
    WMError res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    RSTransaction::FlushImplicitTransaction();
    WLOGFI("SetWindowType FlushImplicitTransaction end");
    return res;
}

WMError WindowController::SetWindowFlags(uint32_t windowId, uint32_t flags)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto property = node->GetWindowProperty();
    property->SetWindowFlags(flags);
    WMError res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    RSTransaction::FlushImplicitTransaction();
    WLOGFI("SetWindowFlags FlushImplicitTransaction end");
    return res;
}

WMError WindowController::SetSystemBarProperty(uint32_t windowId, WindowType type, const SystemBarProperty& property)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->SetSystemBarProperty(type, property);
    WMError res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    RSTransaction::FlushImplicitTransaction();
    WLOGFI("SetSystemBarProperty FlushImplicitTransaction end");
    return res;
}

void WindowController::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    windowRoot_->RegisterWindowManagerAgent(type, windowManagerAgent);
}

void WindowController::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    windowRoot_->UnregisterWindowManagerAgent(type, windowManagerAgent);
}
}
}
