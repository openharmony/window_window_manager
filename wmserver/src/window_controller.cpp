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
#include "window_helper.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowController"};
}
uint32_t WindowController::GenWindowId()
{
    return ++windowId_;
}

WMError WindowController::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId)
{
    uint32_t parentId = property->GetParentId();
    if ((parentId != INVALID_WINDOW_ID) && !WindowHelper::IsSubWindow(property->GetWindowType())) {
        WLOGFE("create window failed, type is error");
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    windowId = GenWindowId();
    property->SetWindowId(windowId);

    // set default transition effect for window
    static auto effect = RSTransitionEffect::Create()->Scale({ 0.0f, 0.0f, 0.0f })->Opacity(0.0f);
    if (surfaceNode != nullptr) {
        surfaceNode->SetTransitionEffect(effect);
    }

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
    FlushWindowInfo(property->GetWindowId());

    if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
        WindowHelper::IsAppWindow(node->GetWindowType())) {
        WM_SCOPED_TRACE_BEGIN("controller:MinimizeStructuredAppWindowsExceptSelf");
        res = windowRoot_->MinimizeStructuredAppWindowsExceptSelf(node);
        WM_SCOPED_TRACE_END();
        if (res != WMError::WM_OK) {
            WLOGFE("Minimize other structured window failed");
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
    FlushWindowInfo(windowId);
    return res;
}

WMError WindowController::DestroyWindow(uint32_t windowId)
{
    DisplayId displayId = DISPLAY_ID_INVALD;
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node != nullptr) {
        displayId = node->GetDisplayId();
    }
    WMError res = windowRoot_->DestroyWindow(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfoWithDisplayId(displayId);
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
    Rect newRect;
    WMError res;
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        if (windowRoot_->isVerticalDisplay(node)) {
            newRect = { lastRect.posX_, y, lastRect.width_, lastRect.height_ };
        } else {
            newRect = { x, lastRect.posY_, lastRect.width_, lastRect.height_ };
        }
        property->SetWindowRect(newRect);
    } else {
        newRect = { x, y, lastRect.width_, lastRect.height_ };
        property->SetWindowRect(newRect);
    }

    res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::Resize(uint32_t windowId, uint32_t width, uint32_t height)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->SetWindowSizeChangeReason(WindowSizeChangeReason::RESIZE);
    auto property = node->GetWindowProperty();
    Rect lastRect = property->GetWindowRect();
    Rect newRect = { lastRect.posX_, lastRect.posY_, width, height };
    property->SetWindowRect(newRect);
    node->hasDecorated = false;
    WMError res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::Drag(uint32_t windowId, const Rect& rect)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->SetWindowSizeChangeReason(WindowSizeChangeReason::DRAG);
    auto property = node->GetWindowProperty();

    // fix rect in case of moving window when dragging
    Rect newRect = WindowHelper::GetFixedWindowRectByMinRect(rect,
        property->GetWindowRect(), windowRoot_->isVerticalDisplay(node));
    property->SetWindowRect(newRect);
    WMError res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::RequestFocus(uint32_t windowId)
{
    return windowRoot_->RequestFocus(windowId);
}

WMError WindowController::SetWindowMode(uint32_t windowId, WindowMode dstMode)
{
    WM_FUNCTION_TRACE();
    auto node = windowRoot_->GetWindowNode(windowId);
    WindowMode srcMode = node->GetWindowMode();
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (srcMode == dstMode) {
        return WMError::WM_OK;
    }
    WMError res = WMError::WM_OK;
    node->SetWindowMode(dstMode);
    if ((srcMode == WindowMode::WINDOW_MODE_FULLSCREEN) && (dstMode == WindowMode::WINDOW_MODE_FLOATING)) {
        node->SetWindowSizeChangeReason(WindowSizeChangeReason::RECOVER);
    } else if (dstMode == WindowMode::WINDOW_MODE_FULLSCREEN) {
        node->SetWindowSizeChangeReason(WindowSizeChangeReason::MAXIMIZE);
    } else {
        node->SetWindowSizeChangeReason(WindowSizeChangeReason::RESIZE);
    }
    if (WindowHelper::IsSplitWindowMode(srcMode)) {
        // change split mode to other
        res = windowRoot_->ExitSplitWindowMode(node);
    } else if (!WindowHelper::IsSplitWindowMode(srcMode) && WindowHelper::IsSplitWindowMode(dstMode)) {
        // change other mode to split
        res = windowRoot_->EnterSplitWindowMode(node);
    }
    if (res != WMError::WM_OK) {
        node->GetWindowProperty()->ResumeLastWindowMode();
        return res;
    }
    if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
        WindowHelper::IsAppWindow(node->GetWindowType())) {
        // minimize other app window
        res = windowRoot_->MinimizeStructuredAppWindowsExceptSelf(node);
        if (res != WMError::WM_OK) {
            return res;
        }
    }
    res = windowRoot_->UpdateWindowNode(windowId);
    if (res != WMError::WM_OK) {
        WLOGFE("Set window mode failed, update node failed");
        return res;
    }
    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::SetWindowBackgroundBlur(uint32_t windowId, WindowBlurLevel level)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->SetWindowBackgroundBlur(level);

    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::SetAlpha(uint32_t windowId, float alpha)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->SetAlpha(alpha);

    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

void WindowController::NotifyDisplayStateChange(DisplayStateChangeType type)
{
    WLOGFI("DisplayStateChangeType:%{public}u", type);
    WindowState state;
    WindowStateChangeReason reason;
    switch (type) {
        case DisplayStateChangeType::BEFORE_SUSPEND: {
            state = WindowState::STATE_FROZEN;
            reason = WindowStateChangeReason::KEYGUARD;
            break;
        }
        case DisplayStateChangeType::BEFORE_UNLOCK: {
            state = WindowState::STATE_UNFROZEN;
            reason = WindowStateChangeReason::KEYGUARD;
            break;
        }
        default: {
            WLOGFE("unknown DisplayStateChangeType:%{public}u", type);
            return;
        }
    }
    windowRoot_->NotifyWindowStateChange(state, reason);
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
    FlushWindowInfo(windowId);
    WLOGFI("SetWindowType end");
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
    FlushWindowInfo(windowId);
    WLOGFI("SetWindowFlags end");
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
    FlushWindowInfo(windowId);
    WLOGFI("SetSystemBarProperty end");
    return res;
}

std::vector<Rect> WindowController::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType)
{
    std::vector<Rect> avoidArea = windowRoot_->GetAvoidAreaByType(windowId, avoidAreaType);
    return avoidArea;
}

WMError WindowController::ProcessWindowTouchedEvent(uint32_t windowId)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFW("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    WMError zOrderRes = windowRoot_->RaiseZOrderForAppWindow(node);
    WMError focusRes = windowRoot_->RequestFocus(windowId);
    if (zOrderRes == WMError::WM_OK || focusRes == WMError::WM_OK) {
        FlushWindowInfo(windowId);
        WLOGFI("ProcessWindowTouchedEvent end");
        return WMError::WM_OK;
    }
    return WMError::WM_ERROR_INVALID_OPERATION;
}

void WindowController::MinimizeAllAppWindows(DisplayId displayId)
{
    windowRoot_->MinimizeAllAppWindows(displayId);
}

WMError WindowController::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    return windowRoot_->GetTopWindowId(mainWinId, topWinId);
}

void WindowController::FlushWindowInfo(uint32_t windowId)
{
    WLOGFI("FlushWindowInfo");
    RSTransaction::FlushImplicitTransaction();
    inputWindowMonitor_->UpdateInputWindow(windowId);
}

void WindowController::FlushWindowInfoWithDisplayId(DisplayId displayId)
{
    WLOGFI("FlushWindowInfoWithDisplayId");
    RSTransaction::FlushImplicitTransaction();
    inputWindowMonitor_->UpdateInputWindowByDisplayId(displayId);
}
}
}