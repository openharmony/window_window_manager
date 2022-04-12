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
WindowLayoutPolicy::WindowLayoutPolicy(const std::map<DisplayId, Rect>& displayRectMap,
    WindowNodeMaps& windowNodeMaps)
    : displayRectMap_(displayRectMap), windowNodeMaps_(windowNodeMaps)
{
}

void WindowLayoutPolicy::Launch()
{
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

void WindowLayoutPolicy::UpdateDisplayInfo(const std::map<DisplayId, Rect>& displayRectMap)
{
    displayRectMap_.clear();
    for (auto& displayRect : displayRectMap) {
        displayRectMap_.insert(displayRect);
    }
    Launch();
}

void WindowLayoutPolicy::LayoutWindowNodesByRootType(const std::vector<sptr<WindowNode>>& nodeVec)
{
    if (nodeVec.empty()) {
        WLOGE("The node vector is empty!");
        return;
    }
    for (auto& node : nodeVec) {
        LayoutWindowNode(node);
    }
}

void WindowLayoutPolicy::LayoutWindowTree(DisplayId displayId)
{
    auto& windowNodeMap = windowNodeMaps_[displayId];
    limitRectMap_[displayId] = displayRectMap_[displayId];
    // ensure that the avoid area windows are traversed first
    LayoutWindowNodesByRootType(*(windowNodeMap[WindowRootNodeType::ABOVE_WINDOW_NODE]));
    if (IsFullScreenRecentWindowExist(*(windowNodeMap[WindowRootNodeType::ABOVE_WINDOW_NODE]))) {
        WLOGFI("recent window on top, early exit layout tree");
        return;
    }
    LayoutWindowNodesByRootType(*(windowNodeMap[WindowRootNodeType::APP_WINDOW_NODE]));
    LayoutWindowNodesByRootType(*(windowNodeMap[WindowRootNodeType::BELOW_WINDOW_NODE]));
}

void WindowLayoutPolicy::LayoutWindowNode(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    WLOGFI("LayoutWindowNode, window[%{public}u]", node->GetWindowId());
    if (node->parent_ != nullptr) { // isn't root node
        if (!node->currentVisibility_) {
            WLOGFI("window[%{public}u] currently not visible, no need layout", node->GetWindowId());
            return;
        }
        UpdateLayoutRect(node);
        if (avoidTypes_.find(node->GetWindowType()) != avoidTypes_.end()) {
            UpdateLimitRect(node, limitRectMap_[node->GetDisplayId()]);
        }
    }
    for (auto& childNode : node->children_) {
        LayoutWindowNode(childNode);
    }
}

bool WindowLayoutPolicy::IsVerticalDisplay(DisplayId displayId) const
{
    return displayRectMap_[displayId].width_ < displayRectMap_[displayId].height_;
}

void WindowLayoutPolicy::RemoveWindowNode(const sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    auto type = node->GetWindowType();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree(node->GetDisplayId());
    } else if (type == WindowType::WINDOW_TYPE_DOCK_SLICE) { // split screen mode
        LayoutWindowTree(node->GetDisplayId());
    }
    Rect reqRect = node->GetRequestRect();
    node->GetWindowToken()->UpdateWindowRect(reqRect, node->GetDecoStatus(), WindowSizeChangeReason::HIDE);
}

void WindowLayoutPolicy::UpdateWindowNode(const sptr<WindowNode>& node, bool isAddWindow)
{
    WM_FUNCTION_TRACE();
    auto type = node->GetWindowType();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree(node->GetDisplayId());
    } else if (type == WindowType::WINDOW_TYPE_DOCK_SLICE) { // split screen mode
        LayoutWindowTree(node->GetDisplayId());
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

void WindowLayoutPolicy::ComputeDecoratedRequestRect(const sptr<WindowNode>& node) const
{
    auto property = node->GetWindowProperty();
    if (property == nullptr) {
        WLOGE("window property is nullptr");
        return;
    }
    auto reqRect = property->GetRequestRect();
    if (!property->GetDecorEnable() || property->GetDecoStatus()) {
        return;
    }
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t winFrameW = static_cast<uint32_t>(WINDOW_FRAME_WIDTH * virtualPixelRatio);
    uint32_t winTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);

    Rect rect;
    rect.posX_ = reqRect.posX_;
    rect.posY_ = reqRect.posY_;
    rect.width_ = reqRect.width_ + winFrameW + winFrameW;
    rect.height_ = reqRect.height_ + winTitleBarH + winFrameW;
    property->SetRequestRect(rect);
    property->SetDecoStatus(true);
}

void WindowLayoutPolicy::CalcAndSetNodeHotZone(Rect layoutOutRect, const sptr<WindowNode>& node) const
{
    Rect rect = layoutOutRect;
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
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
        rect = displayRectMap_[node->GetDisplayId()];
    } else if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
        rect.posX_ -= hotZone;
        rect.posY_ -= hotZone;
        rect.width_ += (hotZone + hotZone);
        rect.height_ += (hotZone + hotZone);
    }
    node->SetHotZoneRect(rect);
}

void WindowLayoutPolicy::LimitWindowSize(const sptr<WindowNode>& node, const Rect& displayRect, Rect& winRect)  const
{
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
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

    const Rect& limitRect = limitRectMap_[node->GetDisplayId()];
    // limit position of the main floating window(window which support dragging)
    if (WindowHelper::IsMainFloatingWindow(windowType, windowMode)) {
        winRect.posY_ = std::max(limitRect.posY_, winRect.posY_);
        winRect.posY_ = std::min(limitRect.posY_ + static_cast<int32_t>(limitRect.height_ - windowTitleBarH),
                                 winRect.posY_);

        winRect.posX_ = std::max(limitRect.posX_ + static_cast<int32_t>(windowTitleBarH - winRect.width_),
                                 winRect.posX_);
        winRect.posX_ = std::min(limitRect.posX_ + static_cast<int32_t>(limitRect.width_ - windowTitleBarH),
                                 winRect.posX_);
    }
}

AvoidPosType WindowLayoutPolicy::GetAvoidPosType(const Rect& rect, DisplayId displayId) const
{
    auto display = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (display == nullptr) {
        WLOGFE("GetAvoidPosType fail. Get display fail. displayId: %{public}" PRIu64"", displayId);
        return AvoidPosType::AVOID_POS_UNKNOWN;
    }
    uint32_t displayWidth = display->GetWidth();
    uint32_t displayHeight = display->GetHeight();

    return WindowHelper::GetAvoidPosType(rect, displayWidth, displayHeight);
}

void WindowLayoutPolicy::UpdateLimitRect(const sptr<WindowNode>& node, Rect& limitRect)
{
    const auto& layoutRect = node->GetWindowRect();
    int32_t limitH = static_cast<int32_t>(limitRect.height_);
    int32_t limitW = static_cast<int32_t>(limitRect.width_);
    int32_t layoutH = static_cast<int32_t>(layoutRect.height_);
    int32_t layoutW = static_cast<int32_t>(layoutRect.width_);
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        auto avoidPosType = GetAvoidPosType(layoutRect, node->GetDisplayId());
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

float WindowLayoutPolicy::GetVirtualPixelRatio(DisplayId displayId) const
{
    auto display = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (display == nullptr) {
        WLOGFE("GetVirtualPixel fail. Get display fail. displayId:%{public}" PRIu64", use Default vpr:1.0", displayId);
        return 1.0;  // Use DefaultVPR 1.0
    }

    float virtualPixelRatio = display->GetVirtualPixelRatio();
    if (virtualPixelRatio == 0.0) {
        WLOGFE("GetVirtualPixel fail. vpr is 0.0. displayId:%{public}" PRIu64", use Default vpr:1.0", displayId);
        return 1.0;  // Use DefaultVPR 1.0
    }

    WLOGFI("GetVirtualPixel success. displayId:%{public}" PRIu64", vpr:%{public}f", displayId, virtualPixelRatio);
    return virtualPixelRatio;
}

bool WindowLayoutPolicy::IsFullScreenRecentWindowExist(const std::vector<sptr<WindowNode>>& nodeVec) const
{
    for (auto& node : nodeVec) {
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_LAUNCHER_RECENT &&
            node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) {
            return true;
        }
    }
    return false;
}
}
}
