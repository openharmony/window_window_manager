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
            UpdateLimitRect(node, dependRects.limitPriRect_);
            UpdateLimitRect(node, dependRects.limitSecRect_);
            UpdateLimitRect(node, dependRects.limitFullRect_);
        }
    }
    for (auto& childNode : node->children_) {
        LayoutWindowNode(childNode);
    }
}

void WindowLayoutPolicy::AddWindowNode(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    UpdateWindowNode(node); // currently, update and add do the same process
}

void WindowLayoutPolicy::RemoveWindowNode(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    auto type = node->GetWindowType();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree();
    } else if (type == WindowType::WINDOW_TYPE_DOCK_SLICE) { // split screen mode
        // TODO: change split screen
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
        // TODO: change split screen
        LayoutWindowTree();
    } else { // layout single window
        LayoutWindowNode(node);
    }
}

static bool IsLayoutChanged(const Rect& l, const Rect& r)
{
    return !((l.posX_ == r.posX_) && (l.posY_ == r.posY_) && (l.width_ == r.width_) && (l.height_ == r.height_));
}

void WindowLayoutPolicy::UpdateLayoutRect(sptr<WindowNode>& node)
{
    auto type = node->GetWindowType();
    auto mode = node->GetWindowMode();
    auto flags = node->GetWindowFlags();
    bool needAvoid = (flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    bool parentLimit = (flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT));
    bool subWindow = WindowHelper::IsSubWindow(type);
    bool floatingWindow = (mode == WindowMode::WINDOW_MODE_FLOATING);
    const Rect& layoutRect = node->GetLayoutRect();
    Rect lastRect = layoutRect;
    Rect displayRect = GetDisplayRect(mode);
    Rect limitRect = displayRect;
    Rect winRect = node->GetWindowProperty()->GetWindowRect();

    WLOGFI("Id:%{public}d, avoid:%{public}d parLimit:%{public}d floating:%{public}d, sub:%{public}d," \
        "type:%{public}d, requestRect:[%{public}d, %{public}d, %{public}d, %{public}d]",
        node->GetWindowId(), needAvoid, parentLimit, floatingWindow, subWindow,
        static_cast<uint32_t>(type), winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    if (needAvoid) {
        limitRect = GetLimitRect(mode);
    }

    if (!floatingWindow) { // fullscreen window
        winRect = limitRect;
    } else { // floating window
        if (subWindow && parentLimit) { // subwidow and limited by parent
            limitRect = node->parent_->GetLayoutRect();
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
    }
    // Limit window to the maximum window size
    winRect.width_ = std::min(displayRect.width_, winRect.width_);
    winRect.height_ = std::min(displayRect.height_, winRect.height_);
    winRect.width_ = std::max(1u, winRect.width_);
    winRect.height_ = std::max(1u, winRect.height_);

    node->SetLayoutRect(winRect);
    if (IsLayoutChanged(lastRect, winRect)) {
        node->GetWindowToken()->UpdateWindowRect(winRect);
        node->surfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
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

void WindowLayoutPolicy::UpdateLimitRect(const sptr<WindowNode>& node, Rect& limitRect)
{
    auto& layoutRect = node->GetLayoutRect();
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        auto avoidPosType = GetAvoidPosType(layoutRect);
        int32_t offsetH = 0;
        int32_t offsetW = 0;
        switch (avoidPosType) {
            case AvoidPosType::AVOID_POS_TOP:
                offsetH = layoutRect.posY_ + layoutRect.height_ - limitRect.posY_;
                limitRect.posY_ += offsetH;
                limitRect.height_ -= offsetH;
                break;
            case AvoidPosType::AVOID_POS_BOTTOM:
                offsetH = limitRect.posY_ + limitRect.height_ - layoutRect.posY_;
                limitRect.height_ -= offsetH;
                break;
            case AvoidPosType::AVOID_POS_LEFT:
                offsetW = layoutRect.posX_ + layoutRect.width_ - limitRect.posX_;
                limitRect.posX_ += offsetW;
                limitRect.width_ -= offsetW;
                break;
            case AvoidPosType::AVOID_POS_RIGHT:
                offsetW = limitRect.posX_ + limitRect.width_ - layoutRect.posX_;
                limitRect.width_ -= offsetW;
                break;
            default:
                WLOGFE("invaild avoidPosType: %{public}d", avoidPosType);
        }
    }
    WLOGFI("Type: %{public}d, limitRect: %{public}d %{public}d %{public}d %{public}d",
        node->GetWindowType(), limitRect.posX_, limitRect.posY_, limitRect.width_, limitRect.height_);
}
}
}
