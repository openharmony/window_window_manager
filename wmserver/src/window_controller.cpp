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
#include <parameters.h>
#include <transaction/rs_transaction.h>
#include "window_manager_hilog.h"
#include "window_helper.h"
#include "wm_common.h"
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
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId, sptr<IRemoteObject> token)
{
    uint32_t parentId = property->GetParentId();
    if ((parentId != INVALID_WINDOW_ID) && !WindowHelper::IsSubWindow(property->GetWindowType())) {
        WLOGFE("create window failed, type is error");
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    windowId = GenWindowId();
    sptr<WindowProperty> windowProperty = new WindowProperty(property);
    windowProperty->SetWindowId(windowId);
    sptr<WindowNode> node = new WindowNode(windowProperty, window, surfaceNode);
    node->abilityToken_ = token;
    UpdateWindowAnimation(node);
    return windowRoot_->SaveWindow(node);
}

WMError WindowController::AddWindowNode(sptr<WindowProperty>& property)
{
    auto node = windowRoot_->GetWindowNode(property->GetWindowId());
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->GetWindowProperty()->CopyFrom(property);

    // Need 'check permission'
    // Need 'adjust property'
    WMError res = windowRoot_->AddWindowNode(property->GetParentId(), node);
    if (res != WMError::WM_OK) {
        return res;
    }
    windowRoot_->FocusFaultDetection();
    FlushWindowInfo(property->GetWindowId());

    if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        sysBarWinId_[node->GetWindowType()] = node->GetWindowId();
    }

    if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
        WindowHelper::IsAppWindow(node->GetWindowType())) {
        WM_SCOPED_TRACE_BEGIN("controller:MinimizeStructuredAppWindowsExceptSelf");
        res = windowRoot_->MinimizeStructuredAppWindowsExceptSelf(node);
        WM_SCOPED_TRACE_END();
        if (res != WMError::WM_OK) {
            WLOGFE("Minimize other structured window failed");
            return res;
        }
    }
    StopBootAnimationIfNeed(node->GetWindowType());
    return WMError::WM_OK;
}

WMError WindowController::RemoveWindowNode(uint32_t windowId)
{
    WMError res = windowRoot_->RemoveWindowNode(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    windowRoot_->FocusFaultDetection();
    FlushWindowInfo(windowId);
    return res;
}

WMError WindowController::DestroyWindow(uint32_t windowId, bool onlySelf)
{
    DisplayId displayId = DISPLAY_ID_INVALID;
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node != nullptr) {
        displayId = node->GetDisplayId();
    }
    WMError res = windowRoot_->DestroyWindow(windowId, onlySelf);
    if (res != WMError::WM_OK) {
        return res;
    }
    windowRoot_->FocusFaultDetection();
    FlushWindowInfoWithDisplayId(displayId);
    return res;
}

WMError WindowController::ResizeRect(uint32_t windowId, const Rect& rect, WindowSizeChangeReason reason)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto property = node->GetWindowProperty();
    Rect lastRect = property->GetWindowRect();
    Rect newRect;
    node->SetWindowSizeChangeReason(reason);
    if (reason == WindowSizeChangeReason::MOVE) {
        newRect = { rect.posX_, rect.posY_, lastRect.width_, lastRect.height_ };
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
            if (windowRoot_->isVerticalDisplay(node)) {
                newRect.posX_ = lastRect.posX_;
            } else {
                newRect.posY_ = lastRect.posY_;
            }
        }
    } else if (reason == WindowSizeChangeReason::RESIZE) {
        node->hasDecorated_ = false;
        newRect = { lastRect.posX_, lastRect.posY_, rect.width_, rect.height_ };
    } else if (reason == WindowSizeChangeReason::DRAG) {
        if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
            // fix rect in case of moving window when dragging
            float virtualPixelRatio = windowRoot_->GetVirtualPixelRatio(node->GetDisplayId());
            Rect displayLimitRect = windowRoot_->GetDisplayLimitRect(node->GetDisplayId());
            newRect = WindowHelper::GetFixedWindowRectByLimitSize(rect, lastRect,
                windowRoot_->isVerticalDisplay(node), virtualPixelRatio);
            newRect = WindowHelper::GetFixedWindowRectByLimitPosition(newRect, lastRect,
                virtualPixelRatio, displayLimitRect);
        } else {
            newRect = rect;
        }
    }

    property->SetWindowRect(newRect);
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_RECT);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::RequestFocus(uint32_t windowId)
{
    if (windowRoot_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    return windowRoot_->RequestFocus(windowId);
}

WMError WindowController::SetWindowMode(uint32_t windowId, WindowMode dstMode)
{
    WM_FUNCTION_TRACE();
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    WindowMode srcMode = node->GetWindowMode();
    if (srcMode == dstMode) {
        return WMError::WM_OK;
    }
    WMError res = WMError::WM_OK;
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
        node->SetWindowMode(dstMode);
    } else if (!WindowHelper::IsSplitWindowMode(srcMode) && WindowHelper::IsSplitWindowMode(dstMode)) {
        // change other mode to split
        node->SetWindowMode(dstMode);
        res = windowRoot_->EnterSplitWindowMode(node);
    } else {
        node->SetWindowMode(dstMode);
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
    res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_MODE);
    if (res != WMError::WM_OK) {
        WLOGFE("Set window mode failed, update node failed");
        return res;
    }
    node->GetWindowToken()->UpdateWindowMode(node->GetWindowMode());
    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::SetWindowBackgroundBlur(uint32_t windowId, WindowBlurLevel dstLevel)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }

    WLOGFI("WindowEffect WindowController SetWindowBackgroundBlur level: %{public}u", dstLevel);
    node->SetWindowBackgroundBlur(dstLevel);
    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::SetAlpha(uint32_t windowId, float dstAlpha)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }

    WLOGFI("WindowEffect WindowController SetAlpha alpha: %{public}f", dstAlpha);
    node->SetAlpha(dstAlpha);

    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

void WindowController::NotifyDisplayStateChange(DisplayId displayId, DisplayStateChangeType type)
{
    WLOGFD("DisplayStateChangeType:%{public}u", type);
    switch (type) {
        case DisplayStateChangeType::BEFORE_SUSPEND: {
            windowRoot_->NotifyWindowStateChange(WindowState::STATE_FROZEN, WindowStateChangeReason::KEYGUARD);
            break;
        }
        case DisplayStateChangeType::BEFORE_UNLOCK: {
            windowRoot_->NotifyWindowStateChange(WindowState::STATE_UNFROZEN, WindowStateChangeReason::KEYGUARD);
            break;
        }
        case DisplayStateChangeType::SIZE_CHANGE:
        case DisplayStateChangeType::UPDATE_ROTATION: {
            ProcessDisplayChange(displayId, type);
            break;
        }
        case DisplayStateChangeType::DESTROY: {
            windowRoot_->NotifyDisplayDestroy(displayId);
            break;
        }
        default: {
            WLOGFE("unknown DisplayStateChangeType:%{public}u", type);
            return;
        }
    }
}

void WindowController::ProcessDisplayChange(DisplayId displayId, DisplayStateChangeType type)
{
    const sptr<DisplayInfo> displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (displayInfo == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRId64 "", displayId);
        return;
    }

    switch (type) {
        case DisplayStateChangeType::SIZE_CHANGE:
        case DisplayStateChangeType::UPDATE_ROTATION: {
            windowRoot_->NotifyDisplayChange(displayInfo);

            // Remove 'sysBarWinId_' after SystemUI resize 'systembar'
            uint32_t width = static_cast<uint32_t>(displayInfo->GetWidth());
            uint32_t height = static_cast<uint32_t>(displayInfo->GetHeight() * SYSTEM_BAR_HEIGHT_RATIO);
            Rect newRect = { 0, 0, width, height };
            ResizeRect(sysBarWinId_[WindowType::WINDOW_TYPE_STATUS_BAR], newRect, WindowSizeChangeReason::DRAG);
            newRect = { 0, displayInfo->GetHeight() - height, width, height };
            ResizeRect(sysBarWinId_[WindowType::WINDOW_TYPE_NAVIGATION_BAR], newRect, WindowSizeChangeReason::DRAG);
            break;
        }
        default: {
            WLOGFE("unknown DisplayStateChangeType:%{public}u", type);
            return;
        }
    }
    FlushWindowInfoWithDisplayId(displayId);
    WLOGFI("Finish ProcessDisplayChange");
}

void WindowController::StopBootAnimationIfNeed(WindowType type) const
{
    if (WindowType::WINDOW_TYPE_DESKTOP == type) {
        WLOGFD("stop boot animation");
        system::SetParameter("persist.window.boot.inited", "1");
    }
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
    UpdateWindowAnimation(node);
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_TYPE);
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
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_FLAGS);
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
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_OTHER_PROPS);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    WLOGFI("SetSystemBarProperty end");
    return res;
}

void WindowController::NotifySystemBarTints()
{
    windowRoot_->NotifySystemBarTints();
}

std::vector<Rect> WindowController::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType)
{
    std::vector<Rect> avoidArea = windowRoot_->GetAvoidAreaByType(windowId, avoidAreaType);
    return avoidArea;
}

WMError WindowController::ProcessPointDown(uint32_t windowId, bool isStartDrag)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFW("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (isStartDrag) {
        node->GetWindowToken()->UpdateWindowRect(node->GetLayoutRect(), WindowSizeChangeReason::DRAG_START);
        WLOGFI("Notify window that the drag action is start: %{public}d", node->GetWindowId());
        return WMError::WM_OK;
    }

    WMError zOrderRes = windowRoot_->RaiseZOrderForAppWindow(node);
    WMError focusRes = windowRoot_->RequestFocus(windowId);
    if (zOrderRes == WMError::WM_OK || focusRes == WMError::WM_OK) {
        FlushWindowInfo(windowId);
        WLOGFI("ProcessPointDown end");
        return WMError::WM_OK;
    }
    windowRoot_->FocusFaultDetection();
    return WMError::WM_ERROR_INVALID_OPERATION;
}

WMError WindowController::ProcessPointUp(uint32_t windowId)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFW("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    WMError res = windowRoot_->UpdateSizeChangeReasonForPointUp(windowId);
    if (res != WMError::WM_OK) {
        return res;
    }
    return WMError::WM_OK;
}

void WindowController::MinimizeAllAppWindows(DisplayId displayId)
{
    windowRoot_->MinimizeAllAppWindows(displayId);
}

WMError WindowController::MaxmizeWindow(uint32_t windowId)
{
    WMError ret = SetWindowMode(windowId, WindowMode::WINDOW_MODE_FULLSCREEN);
    if (ret != WMError::WM_OK) {
        return ret;
    }
    ret = windowRoot_->MaxmizeWindow(windowId);
    FlushWindowInfo(windowId);
    return ret;
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

void WindowController::UpdateWindowAnimation(const sptr<WindowNode>& node)
{
    if (node == nullptr || node->surfaceNode_ == nullptr) {
        WLOGFE("windowNode or surfaceNode is nullptr");
        return;
    }

    uint32_t animationFlag = node->GetWindowProperty()->GetAnimationFlag();
    uint32_t windowId = node->GetWindowProperty()->GetWindowId();
    WLOGFI("windowId: %{public}u, animationFlag: %{public}u", windowId, animationFlag);
    if (animationFlag == static_cast<uint32_t>(WindowAnimation::DEFAULT)) {
        // set default transition effect for window: scale from 1.0 to 0.7, fade from 1.0 to 0.0
        static const auto effect = RSTransitionEffect::Create()->Scale(Vector3f(0.7f, 0.7f, 0.0f))->Opacity(0.0f);
        node->surfaceNode_->SetTransitionEffect(effect);
    } else {
        node->surfaceNode_->SetTransitionEffect(nullptr);
    }
}

WMError WindowController::SetWindowLayoutMode(DisplayId displayId, WindowLayoutMode mode)
{
    WMError res = windowRoot_->SetWindowLayoutMode(displayId, mode);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfoWithDisplayId(displayId);
    return res;
}

WMError WindowController::UpdateProperty(sptr<WindowProperty>& property, PropertyChangeAction action)
{
    if (property == nullptr) {
        WLOGFE("property is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    uint32_t windowId = property->GetWindowId();
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("window is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    switch (action) {
        case PropertyChangeAction::ACTION_UPDATE_RECT: {
            ResizeRect(windowId, property->GetWindowRect(), property->GetWindowSizeChangeReason());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_MODE: {
            SetWindowMode(windowId, property->GetWindowMode());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_FLAGS: {
            SetWindowFlags(windowId, property->GetWindowFlags());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_OTHER_PROPS: {
            auto& props = property->GetSystemBarProperty();
            for (auto& iter : props) {
                SetSystemBarProperty(windowId, iter.first, iter.second);
            }
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_FOCUSABLE: {
            node->SetFocusable(property->GetFocusable());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_TOUCHABLE: {
            node->SetTouchable(property->GetTouchable());
            break;
        }
        default:
            break;
    }
    return WMError::WM_OK;
}
} // namespace OHOS
} // namespace Rosen