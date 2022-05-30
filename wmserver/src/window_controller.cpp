/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include <ability_manager_client.h>
#include <parameters.h>
#include <power_mgr_client.h>
#include <transaction/rs_transaction.h>

#include "minimize_app.h"
#include "remote_animation.h"
#include "starting_window.h"
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

void WindowController::StartingWindow(sptr<WindowTransitionInfo> info, sptr<Media::PixelMap> pixelMap,
    uint32_t bkgColor, bool isColdStart)
{
    if (!info || info->GetAbilityToken() == nullptr) {
        WLOGFE("info or AbilityToken is nullptr!");
        return;
    }
    auto node = windowRoot_->FindWindowNodeWithToken(info->GetAbilityToken());
    if (node == nullptr) {
        if (!isColdStart) {
            WLOGFE("no windowNode exists but is hot start!");
            return;
        }
        node = StartingWindow::CreateWindowNode(info, GenWindowId());
        if (node == nullptr) {
            return;
        }
        if (windowRoot_->SaveWindow(node) != WMError::WM_OK) {
            return;
        }
        if (!RemoteAnimation::CheckAnimationController()) {
            UpdateWindowAnimation(node);
        }
    } else {
        if (isColdStart) {
            WLOGFE("windowNode exists but is cold start!");
            return;
        }
    }
    if (windowRoot_->AddWindowNode(0, node, true) != WMError::WM_OK) {
        return;
    }
    StartingWindow::DrawStartingWindow(node, pixelMap, bkgColor, isColdStart);
    RSTransaction::FlushImplicitTransaction();
    node->startingWindowShown_ = true;
    WLOGFI("StartingWindow show success with id:%{public}u!", node->GetWindowId());
}

void WindowController::CancelStartingWindow(sptr<IRemoteObject> abilityToken)
{
    WLOGFI("begin CancelStartingWindow!");
    auto node = windowRoot_->FindWindowNodeWithToken(abilityToken);
    if (node == nullptr) {
        WLOGFI("cannot find windowNode!");
        return;
    }
    WLOGFI("CancelStartingWindow with id:%{public}u!", node->GetWindowId());
    WMError res = windowRoot_->DestroyWindow(node->GetWindowId(), false);
    if (res != WMError::WM_OK) {
        WLOGFE("DestroyWindow failed!");
    }
}

WMError WindowController::NotifyWindowTransition(sptr<WindowTransitionInfo>& srcInfo,
    sptr<WindowTransitionInfo>& dstInfo)
{
    WLOGFI("NotifyWindowTransition begin!");
    if (!srcInfo || !dstInfo) {
        WLOGFE("srcInfo or dstInfo is nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto dstNode = windowRoot_->FindWindowNodeWithToken(dstInfo->GetAbilityToken());
    auto srcNode = windowRoot_->FindWindowNodeWithToken(srcInfo->GetAbilityToken());
    if (!RemoteAnimation::CheckTransition(srcInfo, srcNode, dstInfo, dstNode)) {
        return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    auto transitionEvent = RemoteAnimation::GetTransitionEvent(srcInfo, dstInfo, srcNode, dstNode);
    switch (transitionEvent) {
        case TransitionEvent::APP_TRANSITION: {
            if (dstNode->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) {
                windowRoot_->MinimizeStructuredAppWindowsExceptSelf(dstNode); // avoid split/float mode minimize
            }
            return RemoteAnimation::NotifyAnimationTransition(srcInfo, dstInfo, srcNode, dstNode);
        }
        case TransitionEvent::MINIMIZE:
            return RemoteAnimation::NotifyAnimationMinimize(srcInfo, srcNode);
        case TransitionEvent::CLOSE:
            return RemoteAnimation::NotifyAnimationClose(srcInfo, srcNode);
        default:
            return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    // Minimize Other judge need : isMinimizedByOtherWindow_, self type.mode
    return WMError::WM_OK;
}

WMError WindowController::GetFocusWindowInfo(sptr<IRemoteObject>& abilityToken)
{
    DisplayId displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    auto windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (windowNodeContainer == nullptr) {
        WLOGFE("windowNodeContainer is null, displayId: %{public}" PRIu64"", displayId);
        return WMError::WM_ERROR_NULLPTR;
    }
    uint32_t focusWindowId = windowNodeContainer->GetFocusWindow();
    auto windowNode = windowRoot_->GetWindowNode(focusWindowId);
    if (windowNode == nullptr || !windowNode->currentVisibility_) {
        WLOGFE("focusWindowNode is null or invisible, focusWindowId: %{public}u", focusWindowId);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    abilityToken = windowNode->abilityToken_;
    WLOGFI("focusWindowId: %{public}u", focusWindowId);
    return WMError::WM_OK;
}

WMError WindowController::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId, sptr<IRemoteObject> token,
    int32_t pid, int32_t uid)
{
    uint32_t parentId = property->GetParentId();
    if ((parentId != INVALID_WINDOW_ID) && !WindowHelper::IsSubWindow(property->GetWindowType())) {
        WLOGFE("create window failed, type is error");
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    sptr<WindowNode> node = windowRoot_->FindWindowNodeWithToken(token);
    if (node != nullptr && WindowHelper::IsMainWindow(property->GetWindowType()) && node->startingWindowShown_) {
        StartingWindow::HandleClientWindowCreate(node, window, windowId, surfaceNode, property, pid, uid);
        windowRoot_->AddDeathRecipient(node);
        return WMError::WM_OK;
    }
    windowId = GenWindowId();
    sptr<WindowProperty> windowProperty = new WindowProperty(property);
    windowProperty->SetWindowId(windowId);
    node = new WindowNode(windowProperty, window, surfaceNode, pid, uid);
    node->abilityToken_ = token;
    UpdateWindowAnimation(node);
    WLOGFI("createWindow name:%{public}u, windowName:%{public}s",
        windowId, node->GetWindowName().c_str());
    return windowRoot_->SaveWindow(node);
}

WMError WindowController::AddWindowNode(sptr<WindowProperty>& property)
{
    auto node = windowRoot_->GetWindowNode(property->GetWindowId());
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (node->currentVisibility_ && !node->startingWindowShown_) {
        WLOGFE("current window is visible, windowId: %{public}u", node->GetWindowId());
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    // using starting window rect if client rect is empty
    if (WindowHelper::IsEmptyRect(property->GetRequestRect()) && node->startingWindowShown_) { // for tile and cascade
        property->SetRequestRect(node->GetRequestRect());
        property->SetWindowRect(node->GetWindowRect());
        property->SetDecoStatus(true);
    }
    node->GetWindowProperty()->CopyFrom(property);

    // Need 'check permission'
    // Need 'adjust property'
    WMError res = windowRoot_->AddWindowNode(property->GetParentId(), node);
    if (res != WMError::WM_OK) {
        MinimizeApp::ClearNodesWithReason(MinimizeReason::OTHER_WINDOW);
        return res;
    }
    windowRoot_->FocusFaultDetection();
    FlushWindowInfo(property->GetWindowId());
    HandleTurnScreenOn(node);

    if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        sysBarWinId_[node->GetWindowType()] = node->GetWindowId();
        ResizeSystemBarPropertySizeIfNeed(node);
    }
    StopBootAnimationIfNeed(node->GetWindowType());
    MinimizeApp::ExecuteMinimizeAll();
    return WMError::WM_OK;
}

void WindowController::ResizeSystemBarPropertySizeIfNeed(const sptr<WindowNode>& node)
{
    auto displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(node->GetDisplayId());
    if (displayInfo == nullptr) {
        WLOGFE("displayInfo is null");
        return;
    }
    uint32_t displayWidth = static_cast<uint32_t>(displayInfo->GetWidth());
    uint32_t displayHeight = static_cast<uint32_t>(displayInfo->GetHeight());
    Rect targetRect = node->GetWindowRect();
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR) {
        auto statusBarRectIter =
            systemBarRect_[WindowType::WINDOW_TYPE_STATUS_BAR][displayWidth].find(displayHeight);
        if (statusBarRectIter != systemBarRect_[WindowType::WINDOW_TYPE_STATUS_BAR][displayWidth].end()) {
            targetRect = statusBarRectIter->second;
        }
    } else if (node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        auto navigationBarRectIter =
            systemBarRect_[WindowType::WINDOW_TYPE_NAVIGATION_BAR][displayWidth].find(displayHeight);
        if (navigationBarRectIter != systemBarRect_[WindowType::WINDOW_TYPE_NAVIGATION_BAR][displayWidth].end()) {
            targetRect = navigationBarRectIter->second;
        }
    }
    if (curDisplayInfo_.find(displayInfo->GetDisplayId()) == curDisplayInfo_.end()) {
        curDisplayInfo_[displayInfo->GetDisplayId()] = displayInfo;
    }
    Rect propertyRect = node->GetWindowRect();
    if (propertyRect != targetRect) {
        ResizeRect(node->GetWindowId(), targetRect, WindowSizeChangeReason::DRAG);
    }
}

void WindowController::HandleTurnScreenOn(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("window is invalid");
        return;
    }
    WLOGFI("handle turn screen on: [%{public}s, %{public}d]", node->GetWindowName().c_str(), node->IsTurnScreenOn());
    // reset ipc identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    if (node->IsTurnScreenOn() && !PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        WLOGFI("turn screen on");
        PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
    }
    // set ipc identity to raw
    IPCSkeleton::SetCallingIdentity(identity);
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
    if (node->GetWindowMode() != WindowMode::WINDOW_MODE_FLOATING) {
        WLOGFE("fullscreen window could not resize");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    auto property = node->GetWindowProperty();
    node->SetWindowSizeChangeReason(reason);
    Rect lastRect = property->GetWindowRect();
    Rect newRect;
    if (reason == WindowSizeChangeReason::MOVE) {
        newRect = { rect.posX_, rect.posY_, lastRect.width_, lastRect.height_ };
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
            if (windowRoot_->IsForbidDockSliceMove(node->GetDisplayId())) {
                WLOGFI("dock slice is forbidden to move");
                newRect = lastRect;
            } else if (windowRoot_->isVerticalDisplay(node)) {
                newRect.posX_ = lastRect.posX_;
            } else {
                newRect.posY_ = lastRect.posY_;
            }
        }
    } else if (reason == WindowSizeChangeReason::RESIZE) {
        newRect = { lastRect.posX_, lastRect.posY_, rect.width_, rect.height_ };
    } else if (reason == WindowSizeChangeReason::DRAG) {
        newRect = rect;
    }
    property->SetRequestRect(newRect);
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
    WMError ret = windowRoot_->SetWindowMode(node, dstMode);
    if (ret != WMError::WM_OK) {
        return ret;
    }
    FlushWindowInfo(windowId);
    MinimizeApp::ExecuteMinimizeAll();
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
            isScreenLocked_ = true;
            windowRoot_->ProcessWindowStateChange(WindowState::STATE_FROZEN, WindowStateChangeReason::KEYGUARD);
            break;
        }
        case DisplayStateChangeType::BEFORE_UNLOCK: {
            windowRoot_->ProcessWindowStateChange(WindowState::STATE_UNFROZEN, WindowStateChangeReason::KEYGUARD);
            isScreenLocked_ = false;
            break;
        }
        case DisplayStateChangeType::CREATE: {
            windowRoot_->ProcessDisplayCreate(displayId);
            break;
        }
        case DisplayStateChangeType::DESTROY: {
            windowRoot_->ProcessDisplayDestroy(displayId);
            break;
        }
        case DisplayStateChangeType::SIZE_CHANGE:
        case DisplayStateChangeType::UPDATE_ROTATION:
        case DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE: {
            ProcessDisplayChange(displayId, type);
            break;
        }
        default: {
            WLOGFE("unknown DisplayStateChangeType:%{public}u", type);
            return;
        }
    }
}

void WindowController::ProcessSystemBarChange(const sptr<DisplayInfo>& displayInfo)
{
    DisplayId displayId = displayInfo->GetDisplayId();
    auto iter = curDisplayInfo_.find(displayId);
    if (iter != curDisplayInfo_.end()) {
        auto lastDisplayInfo = iter->second;
        uint32_t lastDisplayWidth = static_cast<uint32_t>(lastDisplayInfo->GetWidth());
        uint32_t lastDisplayHeight = static_cast<uint32_t>(lastDisplayInfo->GetHeight());
        auto statusBarNode = windowRoot_->GetWindowNode(sysBarWinId_[WindowType::WINDOW_TYPE_STATUS_BAR]);
        auto navigationBarNode =
            windowRoot_->GetWindowNode(sysBarWinId_[WindowType::WINDOW_TYPE_NAVIGATION_BAR]);
        systemBarRect_[WindowType::WINDOW_TYPE_STATUS_BAR][lastDisplayWidth][lastDisplayHeight]
            = statusBarNode->GetWindowProperty()->GetWindowRect();
        systemBarRect_[WindowType::WINDOW_TYPE_NAVIGATION_BAR][lastDisplayWidth][lastDisplayHeight]
            = navigationBarNode->GetWindowProperty()->GetWindowRect();
    }
    curDisplayInfo_[displayId] = displayInfo;
    // Remove 'sysBarWinId_' after SystemUI resize 'systembar'
    uint32_t width = static_cast<uint32_t>(displayInfo->GetWidth());
    uint32_t height = static_cast<uint32_t>(displayInfo->GetHeight() * SYSTEM_BAR_HEIGHT_RATIO);
    Rect newRect = { 0, 0, width, height };
    uint32_t displayWidth = static_cast<uint32_t>(displayInfo->GetWidth());
    uint32_t displayHeight = static_cast<uint32_t>(displayInfo->GetHeight());
    auto statusBarRectIter =
        systemBarRect_[WindowType::WINDOW_TYPE_STATUS_BAR][displayWidth].find(displayHeight);
    if (statusBarRectIter != systemBarRect_[WindowType::WINDOW_TYPE_STATUS_BAR][displayWidth].end()) {
        newRect = statusBarRectIter->second;
    }
    const auto& statusBarNode = windowRoot_->GetWindowNode(sysBarWinId_[WindowType::WINDOW_TYPE_STATUS_BAR]);
    if (statusBarNode->GetDisplayId() == displayId) {
        ResizeRect(sysBarWinId_[WindowType::WINDOW_TYPE_STATUS_BAR], newRect, WindowSizeChangeReason::DRAG);
    }

    newRect = { 0, displayInfo->GetHeight() - static_cast<int32_t>(height), width, height };
    auto navigationBarRectIter =
        systemBarRect_[WindowType::WINDOW_TYPE_NAVIGATION_BAR][displayWidth].find(displayHeight);
    if (navigationBarRectIter != systemBarRect_[WindowType::WINDOW_TYPE_NAVIGATION_BAR][displayWidth].end()) {
        newRect = navigationBarRectIter->second;
    }
    const auto& naviBarNode = windowRoot_->GetWindowNode(sysBarWinId_[WindowType::WINDOW_TYPE_NAVIGATION_BAR]);
    if (naviBarNode->GetDisplayId() == displayId) {
        ResizeRect(sysBarWinId_[WindowType::WINDOW_TYPE_NAVIGATION_BAR], newRect, WindowSizeChangeReason::DRAG);
    }
}

void WindowController::ProcessDisplayChange(DisplayId displayId, DisplayStateChangeType type)
{
    const sptr<DisplayInfo> displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (displayInfo == nullptr) {
        WLOGFE("get display failed displayId:%{public}" PRIu64 "", displayId);
        return;
    }
    switch (type) {
        case DisplayStateChangeType::SIZE_CHANGE:
        case DisplayStateChangeType::UPDATE_ROTATION:
            ProcessSystemBarChange(displayInfo);
            [[fallthrough]];
        case DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE: {
            windowRoot_->ProcessDisplayChange(displayId, type);
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
    uint32_t oldFlags = property->GetWindowFlags();
    property->SetWindowFlags(flags);
    // only forbid_split_move flag change, just set property
    if ((oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE)) {
        return WMError::WM_OK;
    }
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

WMError WindowController::SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller)
{
    return RemoteAnimation::SetWindowAnimationController(controller);
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
    if (!node->currentVisibility_) {
        WLOGFE("this window is not visibile and not in window tree, windowId: %{public}u", windowId);
        return WMError::WM_ERROR_INVALID_OPERATION;
    }

    NotifyOutsidePressed(node);

    if (isStartDrag) {
        WMError res = windowRoot_->UpdateSizeChangeReason(windowId, WindowSizeChangeReason::DRAG_START);
        return res;
    }

    WLOGFI("process point down, windowId: %{public}u", windowId);
    WMError zOrderRes = windowRoot_->RaiseZOrderForAppWindow(node);
    WMError focusRes = windowRoot_->RequestFocus(windowId);
    windowRoot_->RequestActiveWindow(windowId);
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
    WMError res = windowRoot_->UpdateSizeChangeReason(windowId, WindowSizeChangeReason::DRAG_END);
    if (res != WMError::WM_OK) {
        return res;
    }
    return WMError::WM_OK;
}

void WindowController::MinimizeAllAppWindows(DisplayId displayId)
{
    windowRoot_->MinimizeAllAppWindows(displayId);
    MinimizeApp::ExecuteMinimizeAll();
}

WMError WindowController::ToggleShownStateForAllAppWindows()
{
    if (isScreenLocked_) {
        return WMError::WM_DO_NOTHING;
    }
    windowRoot_->ToggleShownStateForAllAppWindows();
    return WMError::WM_OK;
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
    if (node == nullptr || (node->leashWinSurfaceNode_ == nullptr && node->surfaceNode_ == nullptr)) {
        WLOGFE("windowNode or surfaceNode is nullptr");
        return;
    }

    uint32_t animationFlag = node->GetWindowProperty()->GetAnimationFlag();
    uint32_t windowId = node->GetWindowProperty()->GetWindowId();
    WLOGFI("windowId: %{public}u, animationFlag: %{public}u", windowId, animationFlag);
    if (animationFlag == static_cast<uint32_t>(WindowAnimation::DEFAULT)) {
        // set default transition effect for window: scale from 1.0 to 0.7, fade from 1.0 to 0.0
        static const auto effect = RSTransitionEffect::Create()->Scale(Vector3f(0.7f, 0.7f, 0.0f))->Opacity(0.0f);
        if (node->leashWinSurfaceNode_) {
            node->leashWinSurfaceNode_->SetTransitionEffect(effect);
        }
        if (node->surfaceNode_) {
            node->surfaceNode_->SetTransitionEffect(effect);
        }
    } else {
        if (node->leashWinSurfaceNode_) {
            node->leashWinSurfaceNode_->SetTransitionEffect(nullptr);
        }
        if (node->surfaceNode_) {
            node->surfaceNode_->SetTransitionEffect(nullptr);
        }
    }
}

WMError WindowController::SetWindowLayoutMode(WindowLayoutMode mode)
{
    WMError res = WMError::WM_OK;
    auto displayIds = windowRoot_->GetAllDisplayIds();
    for (auto displayId : displayIds) {
        WMError res = windowRoot_->SetWindowLayoutMode(displayId, mode);
        if (res != WMError::WM_OK) {
            return res;
        }
        FlushWindowInfoWithDisplayId(displayId);
    }
    MinimizeApp::ExecuteMinimizeAll();
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
    WLOGFI("window: [%{public}s, %{public}u] update property for action: %{public}u", node->GetWindowName().c_str(),
        node->GetWindowId(), static_cast<uint32_t>(action));
    switch (action) {
        case PropertyChangeAction::ACTION_UPDATE_RECT: {
            node->SetDecoStatus(property->GetDecoStatus());
            return ResizeRect(windowId, property->GetRequestRect(), property->GetWindowSizeChangeReason());
        }
        case PropertyChangeAction::ACTION_UPDATE_MODE: {
            return SetWindowMode(windowId, property->GetWindowMode());
        }
        case PropertyChangeAction::ACTION_UPDATE_FLAGS: {
            return SetWindowFlags(windowId, property->GetWindowFlags());
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
            windowRoot_->UpdateFocusableProperty(windowId);
            FlushWindowInfo(windowId);
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_TOUCHABLE: {
            node->SetTouchable(property->GetTouchable());
            FlushWindowInfo(windowId);
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_CALLING_WINDOW: {
            node->SetCallingWindow(property->GetCallingWindow());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_ORIENTATION: {
            node->SetRequestedOrientation(property->GetRequestedOrientation());
            if (WindowHelper::IsMainWindow(node->GetWindowType()) &&
                node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) {
                DisplayManagerServiceInner::GetInstance().
                    SetOrientationFromWindow(node->GetDisplayId(), property->GetRequestedOrientation());
            }
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON: {
            node->SetTurnScreenOn(property->IsTurnScreenOn());
            HandleTurnScreenOn(node);
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON: {
            node->SetKeepScreenOn(property->IsKeepScreenOn());
            windowRoot_->HandleKeepScreenOn(node->GetWindowId(), node->IsKeepScreenOn());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS: {
            node->SetBrightness(property->GetBrightness());
            windowRoot_->SetBrightness(node->GetWindowId(), node->GetBrightness());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO: {
            node->SetModeSupportInfo(property->GetModeSupportInfo());
            break;
        }
        default:
            break;
    }
    return WMError::WM_OK;
}

WMError WindowController::GetModeChangeHotZones(DisplayId displayId,
    ModeChangeHotZones& hotZones, const ModeChangeHotZonesConfig& config)
{
    return windowRoot_->GetModeChangeHotZones(displayId, hotZones, config);
}

void WindowController::NotifyOutsidePressed(const sptr<WindowNode>& node)
{
    auto windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (windowNodeContainer == nullptr) {
        WLOGFE("window node container is null");
        return;
    }

    std::vector<sptr<WindowNode>> windowNodes;
    windowNodeContainer->TraverseContainer(windowNodes);
    uint32_t skipNodeId = GetEmbedNodeId(windowNodes, node);
    for (auto& windowNode : windowNodes) {
        if (windowNode == nullptr || windowNode->GetWindowToken() == nullptr ||
            windowNode->GetWindowId() == skipNodeId ||
            windowNode->GetWindowId() == node->GetWindowId()) {
            WLOGFD("continue %{public}s", windowNode == nullptr ? "nullptr" : windowNode->GetWindowName().c_str());
            continue;
        }
        WLOGFD("notify %{public}s id %{public}d", windowNode->GetWindowName().c_str(), windowNode->GetWindowId());
        windowNode->GetWindowToken()->NotifyOutsidePressed();
    }
}

uint32_t WindowController::GetEmbedNodeId(const std::vector<sptr<WindowNode>>& windowNodes,
    const sptr<WindowNode>& node)
{
    if (node->GetWindowType() != WindowType::WINDOW_TYPE_APP_COMPONENT) {
        return 0;
    }

    Rect nodeRect = node->GetWindowRect();
    bool isSkip = true;
    for (auto& windowNode : windowNodes) {
        if (windowNode == nullptr) {
            continue;
        }
        if (windowNode->GetWindowId() == node->GetWindowId()) {
            isSkip = false;
            continue;
        }
        if (isSkip) {
            continue;
        }
        if (nodeRect.IsInsideOf(windowNode->GetWindowRect())) {
            WLOGI("OutsidePressed window type is component %{public}s windowNode %{public}d",
                windowNode->GetWindowName().c_str(), windowNode->GetWindowId());
            return windowNode->GetWindowId();
        }
    }
    return 0;
}
} // namespace OHOS
} // namespace Rosen