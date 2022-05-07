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

#include "window_layout_policy_cascade.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"
#include "wm_common_inner.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowLayoutPolicyCascade"};
}

WindowLayoutPolicyCascade::WindowLayoutPolicyCascade(const std::map<DisplayId, Rect>& displayRectMap,
    WindowNodeMaps& windowNodeMaps, std::map<DisplayId, sptr<DisplayInfo>>& displayInfosMap)
    : WindowLayoutPolicy(displayRectMap, windowNodeMaps, displayInfosMap)
{
    CascadeRects cascadeRects {
        .primaryRect_        = {0, 0, 0, 0},
        .secondaryRect_      = {0, 0, 0, 0},
        .primaryLimitRect_   = {0, 0, 0, 0},
        .secondaryLimitRect_ = {0, 0, 0, 0},
        .dividerRect_        = {0, 0, 0, 0},
        .firstCascadeRect_   = {0, 0, 0, 0},
    };
    for (auto& iter : displayRectMap) {
        cascadeRectsMap_.insert(std::make_pair(iter.first, cascadeRects));
    }
}

void WindowLayoutPolicyCascade::Launch()
{
    InitAllRects();
    WLOGFI("WindowLayoutPolicyCascade::Launch");
}

void WindowLayoutPolicyCascade::Clean()
{
    WLOGFI("WindowLayoutPolicyCascade::Clean");
}

void WindowLayoutPolicyCascade::Reset()
{
    // reset split and limit rects
    for (auto& iter : displayRectMap_) {
        InitSplitRects(iter.first);
        InitLimitRects(iter.first);
    }
}

void WindowLayoutPolicyCascade::InitAllRects()
{
    for (auto& iter : displayRectMap_) {
        // init split and limit rects
        InitSplitRects(iter.first);
        InitLimitRects(iter.first);
        // init cascade rect
        auto& windowNodeMap = windowNodeMaps_[iter.first];
        LayoutWindowNodesByRootType(*(windowNodeMap[WindowRootNodeType::ABOVE_WINDOW_NODE]));
        InitCascadeRect(iter.first);
    }
}

void WindowLayoutPolicyCascade::LayoutWindowNode(const sptr<WindowNode>& node)
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
            const DisplayId& displayId = node->GetDisplayId();
            Rect& primaryLimitRect = cascadeRectsMap_[displayId].primaryLimitRect_;
            Rect& secondaryLimitRect = cascadeRectsMap_[displayId].secondaryLimitRect_;
            UpdateLimitRect(node, limitRectMap_[displayId]);
            UpdateSplitLimitRect(limitRectMap_[displayId], primaryLimitRect);
            UpdateSplitLimitRect(limitRectMap_[displayId], secondaryLimitRect);
            WLOGFI("priLimitRect: %{public}d %{public}d %{public}u %{public}u, " \
                "secLimitRect: %{public}d %{public}d %{public}u %{public}u", primaryLimitRect.posX_,
                primaryLimitRect.posY_, primaryLimitRect.width_, primaryLimitRect.height_, secondaryLimitRect.posX_,
                secondaryLimitRect.posY_, secondaryLimitRect.width_, secondaryLimitRect.height_);
        }
    }
    for (auto& childNode : node->children_) {
        LayoutWindowNode(childNode);
    }
}

void WindowLayoutPolicyCascade::LayoutWindowTree(DisplayId displayId)
{
    InitLimitRects(displayId);
    WindowLayoutPolicy::LayoutWindowTree(displayId);
}

void WindowLayoutPolicyCascade::RemoveWindowNode(const sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    auto type = node->GetWindowType();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree(node->GetDisplayId());
    } else if (type == WindowType::WINDOW_TYPE_DOCK_SLICE) { // split screen mode
        InitSplitRects(node->GetDisplayId());
        LayoutWindowTree(node->GetDisplayId());
    }
    Rect reqRect = node->GetRequestRect();
    if (node->GetWindowToken()) {
        node->GetWindowToken()->UpdateWindowRect(reqRect, node->GetDecoStatus(), WindowSizeChangeReason::HIDE);
    }
}

void WindowLayoutPolicyCascade::UpdateWindowNode(const sptr<WindowNode>& node, bool isAddWindow)
{
    WM_FUNCTION_TRACE();
    auto type = node->GetWindowType();
    const DisplayId& displayId = node->GetDisplayId();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree(displayId);
    } else if (type == WindowType::WINDOW_TYPE_DOCK_SLICE) { // split screen mode
        UpdateLayoutRect(node);
        auto splitDockerRect = node->GetWindowRect();
        SetSplitRect(splitDockerRect, displayId); // calculate primary/secondary depend on divider rect
        WLOGFI("UpdateDividerRects WinId: %{public}u, Rect: %{public}d %{public}d %{public}u %{public}u",
            node->GetWindowId(), splitDockerRect.posX_, splitDockerRect.posY_,
            splitDockerRect.width_, splitDockerRect.height_);
        if (!isAddWindow) {
            const auto& appWindowNodeVec = *(windowNodeMaps_[displayId][WindowRootNodeType::APP_WINDOW_NODE]);
            for (auto& childNode : appWindowNodeVec) { // update split node size change reason
                if (childNode->IsSplitMode()) {
                    childNode->SetWindowSizeChangeReason(WindowSizeChangeReason::DRAG);
                }
            }
        }
        LayoutWindowTree(displayId);
    } else if (node->IsSplitMode()) {
        LayoutWindowTree(displayId);
    } else { // layout single window
        LayoutWindowNode(node);
    }
}

void WindowLayoutPolicyCascade::AddWindowNode(const sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    auto property = node->GetWindowProperty();
    if (property == nullptr) {
        WLOGFE("window property is nullptr.");
        return;
    }
    if (WindowHelper::IsEmptyRect(property->GetRequestRect())) {
        SetCascadeRect(node);
    }
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        node->SetRequestRect(cascadeRectsMap_[node->GetDisplayId()].dividerRect_); // init divider bar
    }
    UpdateWindowNode(node, true); // currently, update and add do the same process
}

void WindowLayoutPolicyCascade::LimitMoveBounds(Rect& rect, DisplayId displayId) const
{
    float virtualPixelRatio = GetVirtualPixelRatio(displayId);
    uint32_t minHorizontalSplitW = static_cast<uint32_t>(MIN_HORIZONTAL_SPLIT_WIDTH * virtualPixelRatio);
    uint32_t minVerticalSplitH = static_cast<uint32_t>(MIN_VERTICAL_SPLIT_HEIGHT * virtualPixelRatio);

    const Rect& limitRect = limitRectMap_[displayId];
    if (rect.width_ < rect.height_) {
        if (rect.posX_ < (limitRect.posX_ + static_cast<int32_t>(minHorizontalSplitW))) {
            rect.posX_ = limitRect.posX_ + static_cast<int32_t>(minHorizontalSplitW);
        } else if (rect.posX_ >
            (limitRect.posX_ + static_cast<int32_t>(limitRect.width_ - minHorizontalSplitW))) {
            rect.posX_ = limitRect.posX_ + static_cast<int32_t>(limitRect.width_ - minHorizontalSplitW);
        }
    } else {
        if (rect.posY_ < (limitRect.posY_ + static_cast<int32_t>(minVerticalSplitH))) {
            rect.posY_ = limitRect.posY_ + static_cast<int32_t>(minVerticalSplitH);
        } else if (rect.posY_ >
            (limitRect.posY_ + static_cast<int32_t>(limitRect.height_ - minVerticalSplitH))) {
            rect.posY_ = limitRect.posY_ + static_cast<int32_t>(limitRect.height_ - minVerticalSplitH);
        }
    }
    WLOGFI("limit divider move bounds:[%{public}d, %{public}d, %{public}u, %{public}u]",
        rect.posX_, rect.posY_, rect.width_, rect.height_);
}

void WindowLayoutPolicyCascade::InitCascadeRect(DisplayId displayId)
{
    constexpr uint32_t half = 2;
    constexpr float ratio = DEFAULT_ASPECT_RATIO;

    // calculate default H and w
    uint32_t defaultW = static_cast<uint32_t>(displayRectMap_[displayId].width_ * ratio);
    uint32_t defaultH = static_cast<uint32_t>(displayRectMap_[displayId].height_ * ratio);

    // calculate default x and y
    Rect resRect = {0, 0, defaultW, defaultH};
    const Rect& limitRect = limitRectMap_[displayId];
    if (defaultW <= limitRect.width_ && defaultH <= limitRect.height_) {
        int32_t centerPosX = limitRect.posX_ + static_cast<int32_t>(limitRect.width_ / half);
        resRect.posX_ = centerPosX - static_cast<int32_t>(defaultW / half);

        int32_t centerPosY = limitRect.posY_ + static_cast<int32_t>(limitRect.height_ / half);
        resRect.posY_ = centerPosY - static_cast<int32_t>(defaultH / half);
    }
    WLOGFI("init CascadeRect :[%{public}d, %{public}d, %{public}d, %{public}d]",
        resRect.posX_, resRect.posY_, resRect.width_, resRect.height_);
    cascadeRectsMap_[displayId].firstCascadeRect_ = resRect;
}

void WindowLayoutPolicyCascade::ApplyWindowRectConstraints(const sptr<WindowNode>& node, Rect& winRect) const
{
    WLOGFI("Before apply constraints winRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    auto reason = node->GetWindowSizeChangeReason();
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) { // if divider, limit position
        LimitMoveBounds(winRect, node->GetDisplayId());
    }
    if (reason == WindowSizeChangeReason::DRAG) { // if drag window, limit size and position
        if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
            const Rect lastRect = node->GetWindowRect();
            // fix rect in case of moving window when dragging
            winRect = WindowHelper::GetFixedWindowRectByLimitSize(winRect, lastRect,
                IsVerticalDisplay(node->GetDisplayId()), virtualPixelRatio);
            winRect = WindowHelper::GetFixedWindowRectByLimitPosition(winRect, lastRect,
                virtualPixelRatio, limitRectMap_[node->GetDisplayId()]);
        }
    }
    // Limit window to the maximum window size
    LimitWindowSize(node, displayRectMap_[node->GetDisplayId()], winRect);
    WLOGFI("After apply constraints winRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
}

void WindowLayoutPolicyCascade::UpdateLayoutRect(const sptr<WindowNode>& node)
{
    auto type = node->GetWindowType();
    auto mode = node->GetWindowMode();
    auto property = node->GetWindowProperty();
    if (property == nullptr) {
        WLOGFE("window property is nullptr.");
        return;
    }

    bool needAvoid = (node->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    bool parentLimit = (node->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT));
    bool subWindow = WindowHelper::IsSubWindow(type);
    bool floatingWindow = (mode == WindowMode::WINDOW_MODE_FLOATING);
    const Rect lastWinRect = node->GetWindowRect();
    Rect displayRect = GetDisplayRect(mode, node->GetDisplayId());
    Rect limitRect = displayRect;
    ComputeDecoratedRequestRect(node);
    Rect winRect = property->GetRequestRect();

    WLOGFI("Id:%{public}u, avoid:%{public}d parLimit:%{public}d floating:%{public}d, sub:%{public}d, " \
        "deco:%{public}d, type:%{public}d, requestRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
        node->GetWindowId(), needAvoid, parentLimit, floatingWindow, subWindow, property->GetDecorEnable(),
        static_cast<uint32_t>(type), winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    if (needAvoid) {
        limitRect = GetLimitRect(mode, node->GetDisplayId());
    }

    if (!floatingWindow) { // fullscreen window
        winRect = limitRect;
    } else { // floating window
        if (subWindow && parentLimit) { // subwidow and limited by parent
            limitRect = node->parent_->GetWindowRect();
            UpdateFloatingLayoutRect(limitRect, winRect);
        }
    }
    ApplyWindowRectConstraints(node, winRect);
    node->SetWindowRect(winRect);
    CalcAndSetNodeHotZone(winRect, node);

    UpdateClientRectAndResetReason(node, lastWinRect, winRect);
    // update node bounds

    UpdateSurfaceBounds(node, winRect);
}

void WindowLayoutPolicyCascade::InitLimitRects(DisplayId displayId)
{
    limitRectMap_[displayId] = displayRectMap_[displayId];
    cascadeRectsMap_[displayId].primaryLimitRect_ = cascadeRectsMap_[displayId].primaryRect_;
    cascadeRectsMap_[displayId].secondaryLimitRect_ = cascadeRectsMap_[displayId].secondaryRect_;
}

Rect WindowLayoutPolicyCascade::GetLimitRect(const WindowMode mode, DisplayId displayId) const
{
    if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) {
        return cascadeRectsMap_[displayId].primaryLimitRect_;
    } else if (mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        return cascadeRectsMap_[displayId].secondaryLimitRect_;
    } else {
        return limitRectMap_[displayId];
    }
}

Rect WindowLayoutPolicyCascade::GetDisplayRect(const WindowMode mode, DisplayId displayId) const
{
    if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY) {
        return cascadeRectsMap_[displayId].primaryRect_;
    } else if (mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        return cascadeRectsMap_[displayId].secondaryRect_;
    } else {
        return displayRectMap_[displayId];
    }
}

void WindowLayoutPolicyCascade::UpdateSplitLimitRect(const Rect& limitRect, Rect& limitSplitRect)
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

void WindowLayoutPolicyCascade::InitSplitRects(DisplayId displayId)
{
    float virtualPixelRatio = GetVirtualPixelRatio(displayId);
    uint32_t dividerWidth = static_cast<uint32_t>(DIVIDER_WIDTH * virtualPixelRatio);
    auto& dividerRect = cascadeRectsMap_[displayId].dividerRect_;
    auto& displayRect = displayRectMap_[displayId];
    if (!IsVerticalDisplay(displayId)) {
        dividerRect = { static_cast<uint32_t>((displayRect.width_ - dividerWidth) * DEFAULT_SPLIT_RATIO), 0,
                dividerWidth, displayRect.height_ };
    } else {
        dividerRect = { 0, static_cast<uint32_t>((displayRect.height_ - dividerWidth) * DEFAULT_SPLIT_RATIO),
               displayRect.width_, dividerWidth };
    }
    WLOGFI("init dividerRect :[%{public}d, %{public}d, %{public}u, %{public}u]",
        dividerRect.posX_, dividerRect.posY_, dividerRect.width_, dividerRect.height_);
    SetSplitRect(dividerRect, displayId);
}

void WindowLayoutPolicyCascade::SetSplitRectByRatio(float ratio, DisplayId displayId)
{
    auto& dividerRect = cascadeRectsMap_[displayId].dividerRect_;
    const Rect& limitRect = limitRectMap_[displayId];
    if (!IsVerticalDisplay(displayId)) {
        dividerRect.posX_ = limitRect.posX_ +
            static_cast<uint32_t>((limitRect.width_ - dividerRect.width_) * ratio);
    } else {
        dividerRect.posY_ = limitRect.posY_ +
            static_cast<uint32_t>((limitRect.height_ - dividerRect.height_) * ratio);
    }
    WLOGFI("set dividerRect :[%{public}d, %{public}d, %{public}u, %{public}u]",
        dividerRect.posX_, dividerRect.posY_, dividerRect.width_, dividerRect.height_);
    SetSplitRect(dividerRect, displayId);
}

void WindowLayoutPolicyCascade::SetSplitRect(const Rect& divRect, DisplayId displayId)
{
    auto& dividerRect = cascadeRectsMap_[displayId].dividerRect_;
    auto& primaryRect = cascadeRectsMap_[displayId].primaryRect_;
    auto& secondaryRect = cascadeRectsMap_[displayId].secondaryRect_;
    auto& displayRect = displayRectMap_[displayId];

    dividerRect.width_ = divRect.width_;
    dividerRect.height_ = divRect.height_;
    if (!IsVerticalDisplay(displayId)) {
        primaryRect.posX_ = displayRect.posX_;
        primaryRect.posY_ = displayRect.posY_;
        primaryRect.width_ = divRect.posX_;
        primaryRect.height_ = displayRect.height_;

        secondaryRect.posX_ = divRect.posX_ + dividerRect.width_;
        secondaryRect.posY_ = displayRect.posY_;
        secondaryRect.width_ = displayRect.width_ - secondaryRect.posX_;
        secondaryRect.height_ = displayRect.height_;
    } else {
        primaryRect.posX_ = displayRect.posX_;
        primaryRect.posY_ = displayRect.posY_;
        primaryRect.height_ = divRect.posY_;
        primaryRect.width_ = displayRect.width_;

        secondaryRect.posX_ = displayRect.posX_;
        secondaryRect.posY_ = divRect.posY_ + dividerRect.height_;
        secondaryRect.height_ = displayRect.height_ - secondaryRect.posY_;
        secondaryRect.width_ = displayRect.width_;
    }
}

void WindowLayoutPolicyCascade::Reorder()
{
    WLOGFI("Cascade reorder start");
    for (auto& iter : displayRectMap_) {
        DisplayId displayId = iter.first;
        Rect rect = cascadeRectsMap_[displayId].firstCascadeRect_;
        bool isFirstReorderedWindow = true;
        const auto& appWindowNodeVec = *(windowNodeMaps_[displayId][WindowRootNodeType::APP_WINDOW_NODE]);
        for (auto iter = appWindowNodeVec.begin(); iter != appWindowNodeVec.end(); iter++) {
            auto node = *iter;
            if (node == nullptr || node->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                WLOGFI("get node failed or not app window.");
                continue;
            }
            if (isFirstReorderedWindow) {
                isFirstReorderedWindow = false;
            } else {
                rect = StepCascadeRect(rect, displayId);
            }
            node->SetRequestRect(rect);
            node->SetDecoStatus(true);
            if (node->GetWindowMode() != WindowMode::WINDOW_MODE_FLOATING) {
                node->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
                if (node->GetWindowToken()) {
                    node->GetWindowToken()->UpdateWindowMode(WindowMode::WINDOW_MODE_FLOATING);
                }
            }
            WLOGFI("Cascade reorder Id: %{public}d, rect:[%{public}d, %{public}d, %{public}d, %{public}d]",
                node->GetWindowId(), rect.posX_, rect.posY_, rect.width_, rect.height_);
        }
        LayoutWindowTree(displayId);
    }
    WLOGFI("Reorder end");
}

Rect WindowLayoutPolicyCascade::GetCurCascadeRect(const sptr<WindowNode>& node) const
{
    Rect cascadeRect = {0, 0, 0, 0};
    const DisplayId& displayId = node->GetDisplayId();
    const auto& appWindowNodeVec = *(const_cast<WindowLayoutPolicyCascade*>(this)->
        windowNodeMaps_[displayId][WindowRootNodeType::APP_WINDOW_NODE]);
    const auto& aboveAppWindowNodeVec = *(const_cast<WindowLayoutPolicyCascade*>(this)->
        windowNodeMaps_[displayId][WindowRootNodeType::ABOVE_WINDOW_NODE]);

    std::vector<std::vector<sptr<WindowNode>>> roots = { aboveAppWindowNodeVec, appWindowNodeVec };
    for (auto& root : roots) {
        for (auto iter = root.rbegin(); iter != root.rend(); iter++) {
            if ((*iter)->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
                (*iter)->GetWindowId() != node->GetWindowId()) {
                auto property = (*iter)->GetWindowProperty();
                if (property != nullptr) {
                    cascadeRect = property->GetRequestRect();
                }
                WLOGFI("Get current cascadeRect: %{public}u [%{public}d, %{public}d, %{public}u, %{public}u]",
                    (*iter)->GetWindowId(), cascadeRect.posX_, cascadeRect.posY_,
                    cascadeRect.width_, cascadeRect.height_);
                break;
            }
        }
    }

    if (WindowHelper::IsEmptyRect(cascadeRect)) {
        WLOGFI("cascade rect is empty use first cascade rect");
        return cascadeRectsMap_[displayId].firstCascadeRect_;
    }
    return StepCascadeRect(cascadeRect, displayId);
}

Rect WindowLayoutPolicyCascade::StepCascadeRect(Rect rect, DisplayId displayId) const
{
    float virtualPixelRatio = GetVirtualPixelRatio(displayId);
    uint32_t cascadeWidth = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);
    uint32_t cascadeHeight = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);

    const Rect& limitRect = limitRectMap_[displayId];
    Rect cascadeRect = {0, 0, 0, 0};
    cascadeRect.width_ = rect.width_;
    cascadeRect.height_ = rect.height_;
    cascadeRect.posX_ = (rect.posX_ + static_cast<int32_t>(cascadeWidth) >= limitRect.posX_) &&
                        (rect.posX_ + static_cast<int32_t>(rect.width_ + cascadeWidth) <=
                        (limitRect.posX_ + static_cast<int32_t>(limitRect.width_))) ?
                        (rect.posX_ + static_cast<int32_t>(cascadeWidth)) : limitRect.posX_;
    cascadeRect.posY_ = (rect.posY_ + static_cast<int32_t>(cascadeHeight) >= limitRect.posY_) &&
                        (rect.posY_ + static_cast<int32_t>(rect.height_ + cascadeHeight) <=
                        (limitRect.posY_ + static_cast<int32_t>(limitRect.height_))) ?
                        (rect.posY_ + static_cast<int32_t>(cascadeHeight)) : limitRect.posY_;
    WLOGFI("step cascadeRect :[%{public}d, %{public}d, %{public}u, %{public}u]",
        cascadeRect.posX_, cascadeRect.posY_, cascadeRect.width_, cascadeRect.height_);
    return cascadeRect;
}

void WindowLayoutPolicyCascade::SetCascadeRect(const sptr<WindowNode>& node)
{
    static bool isFirstAppWindow = true;
    Rect rect = {0, 0, 0, 0};
    auto property = node->GetWindowProperty();
    if (property == nullptr) {
        WLOGFE("window property is nullptr.");
        return;
    }
    if (WindowHelper::IsAppWindow(property->GetWindowType()) && isFirstAppWindow) {
        WLOGFI("set first app window cascade rect");
        rect = cascadeRectsMap_[node->GetDisplayId()].firstCascadeRect_;
        isFirstAppWindow = false;
    } else if (WindowHelper::IsAppWindow(property->GetWindowType()) && !isFirstAppWindow) {
        WLOGFI("set other app window cascade rect");
        rect = GetCurCascadeRect(node);
    } else {
        // system window
        WLOGFI("set system window cascade rect");
        rect = cascadeRectsMap_[node->GetDisplayId()].firstCascadeRect_;
    }
    WLOGFI("set cascadeRect :[%{public}d, %{public}d, %{public}u, %{public}u]",
        rect.posX_, rect.posY_, rect.width_, rect.height_);
    node->SetRequestRect(rect);
    node->SetDecoStatus(true);
}
} // Rosen
} // OHOS
