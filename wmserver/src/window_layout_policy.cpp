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

#include "window_layout_policy.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowLayoutPolicy"};
    constexpr uint32_t WINDOW_TITLE_BAR_HEIGHT = 48;
    constexpr uint32_t WINDOW_FRAME_WIDTH = 4;
    constexpr uint32_t MIN_VERTICAL_FLOATING_WIDTH = 360;
    constexpr uint32_t MIN_VERTICAL_FLOATING_HEIGHT = 480;
}

WindowLayoutPolicy::WindowLayoutPolicy(const sptr<WindowNode>& belowAppNode,
    const sptr<WindowNode>& appNode, const sptr<WindowNode>& aboveAppNode)
{
    belowAppWindowNode_ = belowAppNode;
    appWindowNode_ = appNode;
    aboveAppWindowNode_ = aboveAppNode;
}

void WindowLayoutPolicy::UpdateDisplayInfo(const Rect& primaryRect,
                                           const Rect& secondaryRect,
                                           const Rect& displayRect)
{
    dependRects.priRect_ = primaryRect;
    dependRects.secRect_ = secondaryRect;
    dependRects.fullRect_ = displayRect;
    InitLimitRects();
    UpdateDefaultFoatingRect();
}

void WindowLayoutPolicy::UpdateSplitInfo(const Rect& primaryRect, const Rect& secondaryRect)
{
    dependRects.priRect_ = primaryRect;
    dependRects.secRect_ = secondaryRect;
    dependRects.limitPriRect_ = dependRects.priRect_;
    dependRects.limitSecRect_ = dependRects.secRect_;

    UpdateSplitLimitRect(dependRects.limitFullRect_, dependRects.limitPriRect_);
    UpdateSplitLimitRect(dependRects.limitFullRect_, dependRects.limitSecRect_);
}

void WindowLayoutPolicy::LayoutWindowTree()
{
    InitLimitRects();
    std::vector<sptr<WindowNode>> rootNodes = { aboveAppWindowNode_, appWindowNode_, belowAppWindowNode_ };
    for (auto& node : rootNodes) { // ensure that the avoid area windows are traversed first
        LayoutWindowNode(node);
    }
}

void WindowLayoutPolicy::LayoutWindowNode(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    if (node->parent_ != nullptr) { // isn't root node
        if (!node->currentVisibility_) {
            WLOGFI("window[%{public}d] currently not visible, no need layout", node->GetWindowId());
            return;
        }
        UpdateLayoutRect(node);
        if (avoidTypes_.find(node->GetWindowType()) != avoidTypes_.end()) {
            UpdateLimitRect(node, dependRects.limitFullRect_);
            UpdateSplitLimitRect(dependRects.limitFullRect_, dependRects.limitPriRect_);
            UpdateSplitLimitRect(dependRects.limitFullRect_, dependRects.limitSecRect_);
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
    Rect displayRect = GetDisplayRect(WindowMode::WINDOW_MODE_FLOATING);
    uint32_t defaultW = static_cast<uint32_t>(displayRect.width_ * ratio);
    uint32_t defaultH = static_cast<uint32_t>(displayRect.height_ * ratio);

    // calculate default x and y
    Rect limitRect = GetLimitRect(WindowMode::WINDOW_MODE_FLOATING);
    Rect resRect = {0, 0, defaultW, defaultH};
    if (defaultW <= limitRect.width_ && defaultH <= limitRect.height_) {
        int32_t centerPosX = limitRect.posX_ + static_cast<int32_t>(limitRect.width_ / half);
        resRect.posX_ = centerPosX - static_cast<int32_t>(defaultW / half);

        int32_t centerPosY = limitRect.posY_ + static_cast<int32_t>(limitRect.height_ / half);
        resRect.posY_ = centerPosY - static_cast<int32_t>(defaultH / half);
    }

    defaultFloatingRect_ = resRect;
}

void WindowLayoutPolicy::SetRectForFloating(const sptr<WindowNode>& node)
{
    // deduct decor size
    Rect rect = defaultFloatingRect_;
    if (node->GetWindowProperty()->GetDecorEnable()) {
        rect.width_ -= (WINDOW_FRAME_WIDTH + WINDOW_FRAME_WIDTH);
        rect.height_ -= (WINDOW_TITLE_BAR_HEIGHT + WINDOW_FRAME_WIDTH);
    }

    node->SetWindowRect(rect);
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
}

void WindowLayoutPolicy::UpdateWindowNode(sptr<WindowNode>& node)
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

static bool IsLayoutChanged(const Rect& l, const Rect& r)
{
    return !((l.posX_ == r.posX_) && (l.posY_ == r.posY_) && (l.width_ == r.width_) && (l.height_ == r.height_));
}

void WindowLayoutPolicy::LimitMoveBounds(Rect& rect)
{
    Rect curRect = rect;
    rect.posX_ = std::max(curRect.posX_, dependRects.limitFullRect_.posX_);
    rect.posY_ = std::max(curRect.posY_, dependRects.limitFullRect_.posY_);
    if (rect.width_ < rect.height_) {
        rect.posX_ = std::min(curRect.posX_ + rect.width_,
                              dependRects.limitFullRect_.posX_ + dependRects.limitFullRect_.width_) - rect.width_;
        rect.height_ = curRect.posY_ + curRect.height_ - rect.posY_;
    } else {
        rect.posY_ = std::min(curRect.posY_ + rect.height_,
                              dependRects.limitFullRect_.posY_ + dependRects.limitFullRect_.height_) - rect.height_;
        rect.width_ = curRect.posX_ + curRect.width_ - rect.posX_;
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

static Rect ComputeDecoratedWindowRect(const Rect& winRect)
{
    Rect rect;
    rect.posX_ = winRect.posX_;
    rect.posY_ = winRect.posY_;
    rect.width_ = winRect.width_ + WINDOW_FRAME_WIDTH + WINDOW_FRAME_WIDTH;
    rect.height_ = winRect.height_ + WINDOW_TITLE_BAR_HEIGHT + WINDOW_FRAME_WIDTH;
    return rect;
}

void WindowLayoutPolicy::UpdateLayoutRect(sptr<WindowNode>& node)
{
    auto type = node->GetWindowType();
    auto mode = node->GetWindowMode();
    auto flags = node->GetWindowFlags();
    auto decorEnbale = node->GetWindowProperty()->GetDecorEnable();
    bool needAvoid = (flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    bool parentLimit = (flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT));
    bool subWindow = WindowHelper::IsSubWindow(type);
    bool floatingWindow = (mode == WindowMode::WINDOW_MODE_FLOATING);
    const Rect& layoutRect = node->GetLayoutRect();
    Rect lastRect = layoutRect;
    Rect displayRect = GetDisplayRect(mode);
    Rect limitRect = displayRect;
    Rect winRect = node->GetWindowProperty()->GetWindowRect();

    WLOGFI("Id:%{public}d, avoid:%{public}d parLimit:%{public}d floating:%{public}d, sub:%{public}d, " \
        "deco:%{public}d, type:%{public}d, requestRect:[%{public}d, %{public}d, %{public}d, %{public}d]",
        node->GetWindowId(), needAvoid, parentLimit, floatingWindow, subWindow, decorEnbale,
        static_cast<uint32_t>(type), winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    if (needAvoid) {
        limitRect = GetLimitRect(mode);
    }

    if (!floatingWindow) { // fullscreen window
        winRect = limitRect;
    } else { // floating window
        if (node->GetWindowProperty()->GetDecorEnable()) { // is decorable
            winRect = ComputeDecoratedWindowRect(winRect);
        }
        if (subWindow && parentLimit) { // subwidow and limited by parent
            limitRect = node->parent_->GetLayoutRect();
            UpdateFloatingLayoutRect(limitRect, winRect);
        }
    }
    // Limit window to the maximum window size
    LimitWindowSize(node, displayRect, winRect);

    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        LimitMoveBounds(winRect);
    }

    node->SetLayoutRect(winRect);
    if (IsLayoutChanged(lastRect, winRect)) {
        node->GetWindowToken()->UpdateWindowRect(winRect);
        node->surfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    }
}

void WindowLayoutPolicy::LimitWindowSize(const sptr<WindowNode>& node, const Rect& displayRect, Rect& winRect)
{
    bool floatingWindow = (node->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING);

    winRect.width_ = std::min(displayRect.width_, winRect.width_);
    winRect.height_ = std::min(displayRect.height_, winRect.height_);
    bool isVertical = (displayRect.height_ > displayRect.width_) ? true : false;
    WindowType windowType = node->GetWindowProperty()->GetWindowType();
    if (floatingWindow && !WindowHelper::IsSystemWindow(windowType)) {
        if (isVertical) {
            winRect.width_ = std::max(MIN_VERTICAL_FLOATING_WIDTH, winRect.width_);
            winRect.height_ = std::max(MIN_VERTICAL_FLOATING_HEIGHT, winRect.height_);
        } else {
            winRect.width_ = std::max(MIN_VERTICAL_FLOATING_HEIGHT, winRect.width_);
            winRect.height_ = std::max(MIN_VERTICAL_FLOATING_WIDTH, winRect.height_);
        }
    }
}

AvoidPosType WindowLayoutPolicy::GetAvoidPosType(const Rect& rect)
{
    if (rect.width_ >=  rect.height_) {
        if (rect.posY_ == 0) {
            return AvoidPosType::AVOID_POS_TOP;
        } else {
            return AvoidPosType::AVOID_POS_BOTTOM;
        }
    } else {
        if (rect.posX_ == 0) {
            return AvoidPosType::AVOID_POS_LEFT;
        } else {
            return AvoidPosType::AVOID_POS_RIGHT;
        }
    }
    return AvoidPosType::AVOID_POS_UNKNOWN;
}

void WindowLayoutPolicy::InitLimitRects()
{
    dependRects.limitPriRect_ = dependRects.priRect_;
    dependRects.limitSecRect_ = dependRects.secRect_;
    dependRects.limitFullRect_ = dependRects.fullRect_;
}

Rect& WindowLayoutPolicy::GetLimitRect(const WindowMode mode)
{
    if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) {
        return dependRects.limitPriRect_;
    } else if (mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        return dependRects.limitSecRect_;
    } else {
        return dependRects.limitFullRect_;
    }
}

Rect& WindowLayoutPolicy::GetDisplayRect(const WindowMode mode)
{
    if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) {
        return dependRects.priRect_;
    } else if (mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        return dependRects.secRect_;
    } else {
        return dependRects.fullRect_;
    }
}

Rect WindowLayoutPolicy::GetDependDisplayRects() const
{
    Rect displayRect = dependRects.fullRect_;
    Rect limitDisplayRect = dependRects.limitFullRect_;
    if (IsLayoutChanged(displayRect, limitDisplayRect)) {
        return limitDisplayRect;
    }
    return displayRect;
}

void WindowLayoutPolicy::UpdateSplitLimitRect(const Rect& limitRect, Rect& limitSplitRect)
{
    Rect curLimitRect = limitSplitRect;
    limitSplitRect.posX_ = std::max(limitRect.posX_, curLimitRect.posX_);
    limitSplitRect.posY_ = std::max(limitRect.posY_, curLimitRect.posY_);
    limitSplitRect.width_ = std::min(limitRect.posX_ + limitRect.width_,
                                     curLimitRect.posX_ + curLimitRect.width_) -
                                     limitSplitRect.posX_;
    limitSplitRect.height_ = std::min(limitRect.posY_ + limitRect.height_,
                                      curLimitRect.posY_ + curLimitRect.height_) -
                                      limitSplitRect.posY_;
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
                WLOGFE("invaild avoidPosType: %{public}d", avoidPosType);
        }
    }
    limitRect.height_ = static_cast<uint32_t>(limitH < 0 ? 0 : limitH);
    limitRect.width_ = static_cast<uint32_t>(limitW < 0 ? 0 : limitW);
    WLOGFI("Type: %{public}d, limitRect: %{public}d %{public}d %{public}d %{public}d",
        node->GetWindowType(), limitRect.posX_, limitRect.posY_, limitRect.width_, limitRect.height_);
}
}
}
