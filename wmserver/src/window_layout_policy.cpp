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

#include "window_layout_policy.h"
#include "display_manager_service_inner.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_common_inner.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowLayoutPolicy"};
}
WindowLayoutPolicy::WindowLayoutPolicy(const Rect& displayRect, const uint64_t& screenId,
    sptr<WindowNode>& belowAppNode, sptr<WindowNode>& appNode, sptr<WindowNode>& aboveAppNode)
    : displayRect_(displayRect), screenId_(screenId),
    belowAppWindowNode_(belowAppNode), appWindowNode_(appNode), aboveAppWindowNode_(aboveAppNode)
{
}

void WindowLayoutPolicy::UpdateDisplayInfo()
{
    limitRect_ = displayRect_;
    UpdateDefaultFoatingRect();
}

void WindowLayoutPolicy::Launch()
{
    UpdateDisplayInfo();
    WLOGFI("WindowLayoutPolicy::Launch");
}

void WindowLayoutPolicy::Clean()
{
    WLOGFI("WindowLayoutPolicy::Clean");
}

void WindowLayoutPolicy::Reorder()
{
    WLOGFI("WindowLayoutPolicy::Reorder");
}

void WindowLayoutPolicy::LayoutWindowTree()
{
    limitRect_ = displayRect_;
    LayoutWindowNode(aboveAppWindowNode_); // ensure that the avoid area windows are traversed first
    if (IsFullScreenRecentWindowExist()) {
        WLOGFI("recent window on top, early exit layout tree");
        return;
    }
    LayoutWindowNode(appWindowNode_);
    LayoutWindowNode(belowAppWindowNode_);
}

void WindowLayoutPolicy::LayoutWindowNode(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    if (node->parent_ != nullptr) { // isn't root node
        if (!node->currentVisibility_) {
            WLOGFI("window[%{public}u] currently not visible, no need layout", node->GetWindowId());
            return;
        }
        UpdateLayoutRect(node);
        if (avoidTypes_.find(node->GetWindowType()) != avoidTypes_.end()) {
            UpdateLimitRect(node, limitRect_);
        }
    }
    for (auto& childNode : node->children_) {
        LayoutWindowNode(childNode);
    }
}

void WindowLayoutPolicy::AddWindowNode(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    if (WindowHelper::IsEmptyRect(node->GetWindowProperty()->GetWindowRect())) {
        SetRectForFloating(node);
    }
    UpdateWindowNode(node); // currently, update and add do the same process
}

void WindowLayoutPolicy::UpdateDefaultFoatingRect()
{
    constexpr uint32_t half = 2;
    constexpr float ratio = 0.75;  // 0.75: default height/width ratio

    // calculate default H and w
    uint32_t defaultW = static_cast<uint32_t>(displayRect_.width_ * ratio);
    uint32_t defaultH = static_cast<uint32_t>(displayRect_.height_ * ratio);

    // calculate default x and y
    Rect resRect = {0, 0, defaultW, defaultH};
    if (defaultW <= limitRect_.width_ && defaultH <= limitRect_.height_) {
        int32_t centerPosX = limitRect_.posX_ + static_cast<int32_t>(limitRect_.width_ / half);
        resRect.posX_ = centerPosX - static_cast<int32_t>(defaultW / half);

        int32_t centerPosY = limitRect_.posY_ + static_cast<int32_t>(limitRect_.height_ / half);
        resRect.posY_ = centerPosY - static_cast<int32_t>(defaultH / half);
    }
    defaultFloatingRect_ = resRect;
}

void WindowLayoutPolicy::SetRectForFloating(const sptr<WindowNode>& node)
{
    float virtualPixelRatio = GetVirtualPixelRatio();
    uint32_t winFrameW = static_cast<uint32_t>(WINDOW_FRAME_WIDTH * virtualPixelRatio);
    uint32_t winTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);
    // deduct decor size
    Rect rect = defaultFloatingRect_;
    if (node->GetWindowProperty()->GetDecorEnable()) {
        rect.width_ -= (winFrameW + winFrameW);
        rect.height_ -= (winTitleBarH + winFrameW);
    }

    node->SetWindowRect(rect);
}

bool WindowLayoutPolicy::IsVertical() const
{
    return displayRect_.width_ < displayRect_.height_;
}

void WindowLayoutPolicy::RemoveWindowNode(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    auto type = node->GetWindowType();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree();
    } else if (type == WindowType::WINDOW_TYPE_DOCK_SLICE) { // split screen mode
        LayoutWindowTree();
    }
    Rect winRect = node->GetWindowProperty()->GetWindowRect();
    node->SetLayoutRect(winRect);
    node->GetWindowToken()->UpdateWindowRect(winRect, WindowSizeChangeReason::HIDE);
}

void WindowLayoutPolicy::UpdateWindowNode(sptr<WindowNode>& node, bool isAddWindow)
{
    WM_FUNCTION_TRACE();
    auto type = node->GetWindowType();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree();
    } else if (type == WindowType::WINDOW_TYPE_DOCK_SLICE) { // split screen mode
        LayoutWindowTree();
    } else { // layout single window
        LayoutWindowNode(node);
    }
}

void WindowLayoutPolicy::UpdateFloatingLayoutRect(Rect& limitRect, Rect& winRect)
{
    winRect.width_ = std::min(limitRect.width_, winRect.width_);
    winRect.height_ = std::min(limitRect.height_, winRect.height_);
    winRect.posX_ = std::max(limitRect.posX_, winRect.posX_);
    winRect.posY_ = std::max(limitRect.posY_, winRect.posY_);
    winRect.posX_ = std::min(
        limitRect.posX_ + static_cast<int32_t>(limitRect.width_) - static_cast<int32_t>(winRect.width_),
        winRect.posX_);
    winRect.posY_ = std::min(
        limitRect.posY_ + static_cast<int32_t>(limitRect.height_) - static_cast<int32_t>(winRect.height_),
        winRect.posY_);
}

Rect WindowLayoutPolicy::ComputeDecoratedWindowRect(const Rect& winRect)
{
    float virtualPixelRatio = GetVirtualPixelRatio();
    uint32_t winFrameW = static_cast<uint32_t>(WINDOW_FRAME_WIDTH * virtualPixelRatio);
    uint32_t winTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);

    Rect rect;
    rect.posX_ = winRect.posX_;
    rect.posY_ = winRect.posY_;
    rect.width_ = winRect.width_ + winFrameW + winFrameW;
    rect.height_ = winRect.height_ + winTitleBarH + winFrameW;
    return rect;
}

void WindowLayoutPolicy::CalcAndSetNodeHotZone(Rect layoutOutRect, sptr<WindowNode>& node)
{
    Rect rect = layoutOutRect;
    float virtualPixelRatio = GetVirtualPixelRatio();
    uint32_t hotZone = static_cast<uint32_t>(HOTZONE * virtualPixelRatio);

    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        if (rect.width_ < rect.height_) {
            rect.posX_ -= hotZone;
            rect.width_ += (hotZone + hotZone);
        } else {
            rect.posY_ -= hotZone;
            rect.height_ += (hotZone + hotZone);
        }
    } else if (node->GetWindowType() == WindowType::WINDOW_TYPE_LAUNCHER_RECENT) {
        rect = displayRect_;
    } else if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
        rect.posX_ -= hotZone;
        rect.posY_ -= hotZone;
        rect.width_ += (hotZone + hotZone);
        rect.height_ += (hotZone + hotZone);
    }
    node->SetHotZoneRect(rect);
}

void WindowLayoutPolicy::LimitWindowSize(const sptr<WindowNode>& node, const Rect& displayRect, Rect& winRect)
{
    float virtualPixelRatio = GetVirtualPixelRatio();
    uint32_t minVerticalFloatingW = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_WIDTH * virtualPixelRatio);
    uint32_t minVerticalFloatingH = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_HEIGHT * virtualPixelRatio);
    uint32_t windowTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);

    WindowType windowType = node->GetWindowType();
    WindowMode windowMode = node->GetWindowMode();
    bool isVertical = (displayRect.height_ > displayRect.width_) ? true : false;

    if (windowMode == WindowMode::WINDOW_MODE_FLOATING) {
        // limit minimum size of floating (not system type) window
        if (!WindowHelper::IsSystemWindow(windowType)) {
            if (isVertical) {
                winRect.width_ = std::max(minVerticalFloatingW, winRect.width_);
                winRect.height_ = std::max(minVerticalFloatingH, winRect.height_);
            } else {
                winRect.width_ = std::max(minVerticalFloatingH, winRect.width_);
                winRect.height_ = std::max(minVerticalFloatingW, winRect.height_);
            }
        }
        // limit maximum size of all floating window
        winRect.width_ = std::min(static_cast<uint32_t>(MAX_FLOATING_SIZE * virtualPixelRatio), winRect.width_);
        winRect.height_ = std::min(static_cast<uint32_t>(MAX_FLOATING_SIZE * virtualPixelRatio), winRect.height_);
    }

    // limit position of the main floating window(window which support dragging)
    if (WindowHelper::IsMainFloatingWindow(windowType, windowMode)) {
        winRect.posY_ = std::max(limitRect_.posY_, winRect.posY_);
        winRect.posY_ = std::min(limitRect_.posY_ + static_cast<int32_t>(limitRect_.height_ - windowTitleBarH),
                                 winRect.posY_);

        winRect.posX_ = std::max(limitRect_.posX_ + static_cast<int32_t>(windowTitleBarH - winRect.width_),
                                 winRect.posX_);
        winRect.posX_ = std::min(limitRect_.posX_ + static_cast<int32_t>(limitRect_.width_ - windowTitleBarH),
                                 winRect.posX_);
    }
}

AvoidPosType WindowLayoutPolicy::GetAvoidPosType(const Rect& rect)
{
    auto display = DisplayManagerServiceInner::GetInstance().GetDisplayById(screenId_);
    if (display == nullptr) {
        WLOGFE("GetAvoidPosType fail. Get display fail. displayId:%{public}" PRIu64"", screenId_);
        return AvoidPosType::AVOID_POS_UNKNOWN;
    }
    uint32_t displayWidth = display->GetWidth();
    uint32_t displayHeight = display->GetHeight();

    return WindowHelper::GetAvoidPosType(rect, displayWidth, displayHeight);
}

void WindowLayoutPolicy::UpdateLimitRect(const sptr<WindowNode>& node, Rect& limitRect)
{
    auto& layoutRect = node->GetLayoutRect();
    int32_t limitH = static_cast<int32_t>(limitRect.height_);
    int32_t limitW = static_cast<int32_t>(limitRect.width_);
    int32_t layoutH = static_cast<int32_t>(layoutRect.height_);
    int32_t layoutW = static_cast<int32_t>(layoutRect.width_);
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        auto avoidPosType = GetAvoidPosType(layoutRect);
        int32_t offsetH = 0;
        int32_t offsetW = 0;
        switch (avoidPosType) {
            case AvoidPosType::AVOID_POS_TOP:
                offsetH = layoutRect.posY_ + layoutH - limitRect.posY_;
                limitRect.posY_ += offsetH;
                limitH -= offsetH;
                break;
            case AvoidPosType::AVOID_POS_BOTTOM:
                offsetH = limitRect.posY_ + limitH - layoutRect.posY_;
                limitH -= offsetH;
                break;
            case AvoidPosType::AVOID_POS_LEFT:
                offsetW = layoutRect.posX_ + layoutW - limitRect.posX_;
                limitRect.posX_ += offsetW;
                limitW -= offsetW;
                break;
            case AvoidPosType::AVOID_POS_RIGHT:
                offsetW = limitRect.posX_ + limitW - layoutRect.posX_;
                limitW -= offsetW;
                break;
            default:
                WLOGFE("invalid avoidPosType: %{public}d", avoidPosType);
        }
    }
    limitRect.height_ = static_cast<uint32_t>(limitH < 0 ? 0 : limitH);
    limitRect.width_ = static_cast<uint32_t>(limitW < 0 ? 0 : limitW);
    WLOGFI("Type: %{public}d, limitRect: %{public}d %{public}d %{public}u %{public}u",
        node->GetWindowType(), limitRect.posX_, limitRect.posY_, limitRect.width_, limitRect.height_);
}

void WindowLayoutPolicy::Reset()
{
}

float WindowLayoutPolicy::GetVirtualPixelRatio() const
{
    auto display = DisplayManagerServiceInner::GetInstance().GetDisplayById(screenId_);
    if (display == nullptr) {
        WLOGFE("GetVirtualPixel fail. Get display fail. displayId:%{public}" PRIu64", use Default vpr:1.0", screenId_);
        return 1.0;  // Use DefaultVPR 1.0
    }

    float virtualPixelRatio = display->GetVirtualPixelRatio();
    if (virtualPixelRatio == 0.0) {
        WLOGFE("GetVirtualPixel fail. vpr is 0.0. displayId:%{public}" PRIu64", use Default vpr:1.0", screenId_);
        return 1.0;  // Use DefaultVPR 1.0
    }

    WLOGFI("GetVirtualPixel success. displayId:%{public}" PRIu64", vpr:%{public}f", screenId_, virtualPixelRatio);
    return virtualPixelRatio;
}

bool WindowLayoutPolicy::IsFullScreenRecentWindowExist() const
{
    for (auto& childNode : aboveAppWindowNode_->children_) {
        if (childNode->GetWindowType() == WindowType::WINDOW_TYPE_LAUNCHER_RECENT &&
            childNode->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) {
            return true;
        }
    }
    return false;
}

Rect WindowLayoutPolicy::GetDisplayLimitRect() const
{
    return limitRect_;
}
}
}
