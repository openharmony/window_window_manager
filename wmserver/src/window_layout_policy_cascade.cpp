/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <hitrace_meter.h>

#include "minimize_app.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"
#include "wm_math.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WLP_Cascade"};
}

WindowLayoutPolicyCascade::WindowLayoutPolicyCascade(const sptr<DisplayGroupInfo>& displayGroupInfo,
    DisplayGroupWindowTree& displayGroupWindowTree)
    : WindowLayoutPolicy(displayGroupInfo, displayGroupWindowTree)
{
    CascadeRects cascadeRects {
        .primaryRect_        = {0, 0, 0, 0},
        .secondaryRect_      = {0, 0, 0, 0},
        .dividerRect_        = {0, 0, 0, 0},
        .defaultCascadeRect_ = {0, 0, 0, 0},
    };
    for (auto& iter : displayGroupInfo_->GetAllDisplayRects()) {
        cascadeRectsMap_.insert(std::make_pair(iter.first, cascadeRects));
    }
}

void WindowLayoutPolicyCascade::Launch()
{
    InitAllRects();
    WLOGI("WindowLayoutPolicyCascade::Launch");
}

void WindowLayoutPolicyCascade::Reorder()
{
    WLOGFD("Cascade reorder start");
    for (auto& iter : displayGroupInfo_->GetAllDisplayRects()) {
        DisplayId displayId = iter.first;
        Rect rect = cascadeRectsMap_[displayId].defaultCascadeRect_;
        bool isFirstReorderedWindow = true;
        const auto& appWindowNodeVec = *(displayGroupWindowTree_[displayId][WindowRootNodeType::APP_WINDOW_NODE]);
        for (auto nodeIter = appWindowNodeVec.begin(); nodeIter != appWindowNodeVec.end(); nodeIter++) {
            auto node = *nodeIter;
            if (node == nullptr || node->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                WLOGFW("get node failed or not app window.");
                continue;
            }
            // if window don't support floating mode, or default rect of cascade is not satisfied with limits
            if (!WindowHelper::IsWindowModeSupported(node->GetModeSupportInfo(), WindowMode::WINDOW_MODE_FLOATING) ||
                !WindowHelper::IsRectSatisfiedWithSizeLimits(rect, node->GetWindowUpdatedSizeLimits())) {
                MinimizeApp::AddNeedMinimizeApp(node, MinimizeReason::LAYOUT_CASCADE);
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
            WLOGFD("Cascade reorder Id: %{public}d, rect:[%{public}d, %{public}d, %{public}d, %{public}d]",
                node->GetWindowId(), rect.posX_, rect.posY_, rect.width_, rect.height_);
        }
        LayoutWindowTree(displayId);
    }
    WLOGI("Cascade Reorder end");
}

void WindowLayoutPolicyCascade::InitAllRects()
{
    UpdateDisplayGroupRect();
    for (auto& iter : displayGroupInfo_->GetAllDisplayRects()) {
        auto displayId = iter.first;
        InitSplitRects(displayId);
        LayoutWindowTree(displayId);
        InitCascadeRect(displayId);
    }
}

void WindowLayoutPolicyCascade::LayoutSplitNodes(DisplayId displayId, WindowUpdateType type, bool layoutByDivider)
{
    std::vector<WindowRootNodeType> rootNodeType = {
        WindowRootNodeType::ABOVE_WINDOW_NODE,
        WindowRootNodeType::APP_WINDOW_NODE,
        WindowRootNodeType::BELOW_WINDOW_NODE
    };
    for (const auto& rootType : rootNodeType) {
        if (displayGroupWindowTree_[displayId].find(rootType) == displayGroupWindowTree_[displayId].end()) {
            continue;
        }
        auto appWindowNodeVec = *(displayGroupWindowTree_[displayId][rootType]);
        for (const auto& childNode : appWindowNodeVec) {
            if (type == WindowUpdateType::WINDOW_UPDATE_REMOVED) {
                /*
                * If updateType is remove we need to layout all appNodes, cause remove split node or
                * divider means exit split mode, split node may change to other mode
                */
                LayoutWindowNode(childNode);
            } else if (childNode->IsSplitMode()) { // add or update type, layout split node
                if (layoutByDivider && type == WindowUpdateType::WINDOW_UPDATE_ACTIVE) {
                    childNode->SetWindowSizeChangeReason(WindowSizeChangeReason::DRAG);
                }
                LayoutWindowNode(childNode);
            }
        }
    }
}

void WindowLayoutPolicyCascade::LayoutDivider(const sptr<WindowNode>& node, WindowUpdateType type)
{
    auto displayId = node->GetDisplayId();
    switch (type) {
        case WindowUpdateType::WINDOW_UPDATE_ADDED:
            SetInitialDividerRect(node, displayId);
            [[fallthrough]];
        case WindowUpdateType::WINDOW_UPDATE_ACTIVE:
            UpdateDividerPosition(node);
            LayoutWindowNode(node);
            SetSplitRectByDivider(node->GetWindowRect(), displayId); // set splitRect by divider
            break;
        case WindowUpdateType::WINDOW_UPDATE_REMOVED:
            InitSplitRects(displayId); // reinit split rects when remove divider
            break;
        default:
            WLOGFW("Unknown update type, type: %{public}u", type);
    }
    LayoutSplitNodes(displayId, type, true);
}

void WindowLayoutPolicyCascade::PerformWindowLayout(const sptr<WindowNode>& node, WindowUpdateType updateType)
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    const auto& windowType = node->GetWindowType();
    const auto& requestRect = node->GetRequestRect();
    WLOGFD("windowId: %{public}u, windowType: %{public}u, updateType: %{public}u, requestRect: "
        "requestRect: [%{public}d, %{public}d, %{public}u, %{public}u]", node->GetWindowId(), windowType, updateType,
        requestRect.posX_, requestRect.posY_, requestRect.width_, requestRect.height_);
    SetDefaultCascadeRect(node);
    FixWindowRectWithinDisplay(node);
    switch (windowType) {
        case WindowType::WINDOW_TYPE_DOCK_SLICE:
            LayoutDivider(node, updateType);
            break;
        case WindowType::WINDOW_TYPE_STATUS_BAR:
        case WindowType::WINDOW_TYPE_NAVIGATION_BAR:
            LayoutWindowTree(node->GetDisplayId());
            break;
        default:
            if (node->IsSplitMode()) {
                LayoutSplitNodes(node->GetDisplayId(), updateType);
            } else {
                LayoutWindowNode(node);
            }
    }
    if (updateType == WindowUpdateType::WINDOW_UPDATE_REMOVED) {
        NotifyClientAndAnimation(node, node->GetRequestRect(), WindowSizeChangeReason::HIDE);
    }
}

void WindowLayoutPolicyCascade::SetInitialDividerRect(const sptr<WindowNode>& node, DisplayId displayId)
{
    const auto& restoredRect = restoringDividerWindowRects_[displayId];
    const auto& presetRect = cascadeRectsMap_[node->GetDisplayId()].dividerRect_;
    auto divRect = WindowHelper::IsEmptyRect(restoredRect) ? presetRect : restoredRect;
    node->SetRequestRect(divRect);
    restoringDividerWindowRects_.erase(displayId);
}

void WindowLayoutPolicyCascade::SetSplitDividerWindowRects(std::map<DisplayId, Rect> dividerWindowRects)
{
    restoringDividerWindowRects_ = dividerWindowRects;
}

void WindowLayoutPolicyCascade::LimitDividerInDisplayRegion(Rect& rect, DisplayId displayId) const
{
    const Rect& limitRect = limitRectMap_[displayId];
    if (rect.width_ < rect.height_) {
        if (rect.posX_ < limitRect.posX_) {
            rect.posX_ = limitRect.posX_;
        } else if (rect.posX_ + static_cast<int32_t>(rect.width_) >
            limitRect.posX_ + static_cast<int32_t>(limitRect.width_)) {
            rect.posX_ = limitRect.posX_ + static_cast<int32_t>(limitRect.width_ - rect.width_);
        }
    } else {
        if (rect.posY_ < limitRect.posY_) {
            rect.posY_ = limitRect.posY_;
        } else if (rect.posY_ + static_cast<int32_t>(rect.height_) >
            limitRect.posY_ + static_cast<int32_t>(limitRect.height_)) {
            rect.posY_ = limitRect.posY_ + static_cast<int32_t>(limitRect.height_ - rect.height_);
        }
    }
    WLOGFD("limit divider move bounds: [%{public}d, %{public}d, %{public}u, %{public}u]",
        rect.posX_, rect.posY_, rect.width_, rect.height_);
}

void WindowLayoutPolicyCascade::UpdateDividerPosition(const sptr<WindowNode>& node) const
{
    auto rect = node->GetRequestRect();
    auto displayId = node->GetDisplayId();
    LimitDividerInDisplayRegion(rect, displayId);
    if (node->GetWindowSizeChangeReason() == WindowSizeChangeReason::DRAG_END) {
        LimitDividerPositionBySplitRatio(displayId, rect);
    }
    node->SetRequestRect(rect);
}

void WindowLayoutPolicyCascade::InitCascadeRect(DisplayId displayId)
{
    constexpr uint32_t half = 2;
    constexpr float ratio = DEFAULT_ASPECT_RATIO;

    /*
     * Calculate default width and height, if width or height is
     * smaller than minWidth or minHeight, use the minimum limits
     */
    const auto& displayRect = displayGroupInfo_->GetDisplayRect(displayId);
    auto vpr = displayGroupInfo_->GetDisplayVirtualPixelRatio(displayId);
    uint32_t defaultW = std::max(static_cast<uint32_t>(displayRect.width_ * ratio),
                                 static_cast<uint32_t>(MIN_FLOATING_WIDTH * vpr));
    uint32_t defaultH = std::max(static_cast<uint32_t>(displayRect.height_ * ratio),
                                 static_cast<uint32_t>(MIN_FLOATING_HEIGHT * vpr));

    // calculate default x and y
    Rect resRect = {0, 0, defaultW, defaultH};
    const Rect& limitRect = limitRectMap_[displayId];
    if (defaultW <= limitRect.width_ && defaultH <= limitRect.height_) {
        resRect.posX_ = limitRect.posX_ + static_cast<int32_t>((limitRect.width_ - defaultW) / half);

        resRect.posY_ = limitRect.posY_ + static_cast<int32_t>((limitRect.height_ - defaultH) / half);
    }
    WLOGI("Init CascadeRect :[%{public}d, %{public}d, %{public}d, %{public}d]",
        resRect.posX_, resRect.posY_, resRect.width_, resRect.height_);
    cascadeRectsMap_[displayId].defaultCascadeRect_ = resRect;
}

bool WindowLayoutPolicyCascade::CheckAspectRatioBySizeLimits(const sptr<WindowNode>& node,
    WindowSizeLimits& newLimits) const
{
    // get new limit config with the settings of system and app
    const auto& sizeLimits = node->GetWindowUpdatedSizeLimits();
    float vpr = displayGroupInfo_->GetDisplayVirtualPixelRatio(node->GetDisplayId());
    uint32_t winFrameW = static_cast<uint32_t>(WINDOW_FRAME_WIDTH * vpr) * 2; // 2 mean double decor width
    uint32_t winFrameH = static_cast<uint32_t>(WINDOW_FRAME_WIDTH * vpr) +
        static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * vpr); // decor height

    newLimits.maxWidth_ = sizeLimits.maxWidth_ - winFrameW;
    newLimits.minWidth_ = sizeLimits.minWidth_ - winFrameW;
    newLimits.maxHeight_ = sizeLimits.maxHeight_ - winFrameH;
    newLimits.minHeight_ = sizeLimits.minHeight_ - winFrameH;
    float maxRatio = static_cast<float>(newLimits.maxWidth_) / static_cast<float>(newLimits.minHeight_);
    float minRatio = static_cast<float>(newLimits.minWidth_) / static_cast<float>(newLimits.maxHeight_);
    float aspectRatio = node->GetAspectRatio();
    if (maxRatio < aspectRatio || aspectRatio < minRatio) {
        return false;
    }
    uint32_t newMaxWidth = static_cast<uint32_t>(static_cast<float>(newLimits.maxHeight_) * aspectRatio);
    newLimits.maxWidth_ = std::min(newMaxWidth, newLimits.maxWidth_);
    uint32_t newMinWidth = static_cast<uint32_t>(static_cast<float>(newLimits.minHeight_) * aspectRatio);
    newLimits.minWidth_ = std::max(newMinWidth, newLimits.minWidth_);
    uint32_t newMaxHeight = static_cast<uint32_t>(static_cast<float>(newLimits.maxWidth_) / aspectRatio);
    newLimits.maxHeight_ = std::min(newMaxHeight, newLimits.maxHeight_);
    uint32_t newMinHeight = static_cast<uint32_t>(static_cast<float>(newLimits.minWidth_) / aspectRatio);
    newLimits.minHeight_ = std::max(newMinHeight, newLimits.minHeight_);
    return true;
}

void WindowLayoutPolicyCascade::ComputeRectByAspectRatio(const sptr<WindowNode>& node) const
{
    float aspectRatio = node->GetAspectRatio();
    if (!WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode()) ||
        node->GetWindowSizeChangeReason() == WindowSizeChangeReason::MOVE || MathHelper::NearZero(aspectRatio)) {
        return;
    }

    // 1. check ratio by size limits
    WindowSizeLimits newLimits;
    if (!CheckAspectRatioBySizeLimits(node, newLimits)) {
        return;
    }

    float vpr = displayGroupInfo_->GetDisplayVirtualPixelRatio(node->GetDisplayId());
    uint32_t winFrameW = static_cast<uint32_t>(WINDOW_FRAME_WIDTH * vpr) * 2; // 2 mean double decor width
    uint32_t winFrameH = static_cast<uint32_t>(WINDOW_FRAME_WIDTH * vpr) +
        static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * vpr); // decor height

    // 2. get rect without decoration
    auto newRect = node->GetRequestRect();
    newRect.width_ -= winFrameW;
    newRect.height_ -= winFrameH;
    auto oriRect = newRect;

    // 3. update window rect by new limits and aspect ratio
    newRect.width_ = std::max(newLimits.minWidth_, newRect.width_);
    newRect.height_ = std::max(newLimits.minHeight_, newRect.height_);
    newRect.width_ = std::min(newLimits.maxWidth_, newRect.width_);
    newRect.height_ = std::min(newLimits.maxHeight_, newRect.height_);
    float curRatio = static_cast<float>(newRect.width_) / static_cast<float>(newRect.height_);
    if (std::abs(curRatio - aspectRatio) > 0.0001f) {
        if (node->GetDragType() == DragType::DRAG_BOTTOM_OR_TOP) {
            // if drag height, use height to fix size.
            newRect.width_ = static_cast<uint32_t>(static_cast<float>(newRect.height_) * aspectRatio);
        } else {
            // if drag width or corner, use width to fix size.
            newRect.height_ = static_cast<uint32_t>(static_cast<float>(newRect.width_) / aspectRatio);
        }
    }

    // 4. fix window pos in case of moving window when dragging
    FixWindowRectWhenDrag(node, oriRect, newRect);

    // 5. if posY is smaller than limit posY when drag, use the last window rect
    if (newRect.posY_ < limitRectMap_[node->GetDisplayId()].posY_ &&
        node->GetWindowSizeChangeReason() == WindowSizeChangeReason::DRAG) {
        auto lastRect = node->GetWindowRect();
        lastRect.width_ -= winFrameW;
        lastRect.height_ -= winFrameH;
        newRect = lastRect;
    }
    node->SetRequestRect(newRect);
    node->SetDecoStatus(false); // newRect is not rect with decor, reset decor status
    WLOGI("ComputeRectByAspectRatio, winId: %{public}u, newRect: %{public}d %{public}d %{public}u %{public}u",
        node->GetWindowId(), newRect.posX_, newRect.posY_, newRect.width_, newRect.height_);
}

void WindowLayoutPolicyCascade::ComputeDecoratedRequestRect(const sptr<WindowNode>& node) const
{
    auto property = node->GetWindowProperty();
    if (property == nullptr) {
        WLOGE("window property is nullptr");
        return;
    }

    if (!property->GetDecorEnable() || property->GetDecoStatus() ||
        node->GetWindowSizeChangeReason() == WindowSizeChangeReason::MOVE) {
        return;
    }

    float virtualPixelRatio = displayGroupInfo_->GetDisplayVirtualPixelRatio(node->GetDisplayId());
    uint32_t winFrameW = static_cast<uint32_t>(WINDOW_FRAME_WIDTH * virtualPixelRatio);
    uint32_t winTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);

    auto oriRect = property->GetRequestRect();
    Rect dstRect;
    dstRect.posX_ = oriRect.posX_;
    dstRect.posY_ = oriRect.posY_;
    dstRect.width_ = oriRect.width_ + winFrameW + winFrameW;
    dstRect.height_ = oriRect.height_ + winTitleBarH + winFrameW;
    property->SetRequestRect(dstRect);
    property->SetDecoStatus(true);
}

void WindowLayoutPolicyCascade::ApplyWindowRectConstraints(const sptr<WindowNode>& node, Rect& winRect) const
{
    WLOGFD("[Before constraints] windowId: %{public}u, winRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
        node->GetWindowId(), winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    ComputeRectByAspectRatio(node);
    ComputeDecoratedRequestRect(node);
    winRect = node->GetRequestRect();
    LimitFloatingWindowSize(node, winRect);
    LimitMainFloatingWindowPosition(node, winRect);

    /*
     * Use the orientation of the window and display to determine
     * whether the screen is rotating, then rotate the divider
     */
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE &&
        ((!WindowHelper::IsLandscapeRect(winRect) && IsVerticalDisplay(node->GetDisplayId())) ||
        (WindowHelper::IsLandscapeRect(winRect) && !IsVerticalDisplay(node->GetDisplayId())))) {
        winRect = cascadeRectsMap_[node->GetDisplayId()].dividerRect_;
        node->SetRequestRect(winRect);
        WLOGFD("Reset divider when display rotation, divRect: [%{public}d, %{public}d, %{public}u, %{public}u]",
            winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    }

    WLOGFD("[After constraints] windowId: %{public}u, winRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
        node->GetWindowId(), winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
}

void WindowLayoutPolicyCascade::UpdateLayoutRect(const sptr<WindowNode>& node)
{
    auto property = node->GetWindowProperty();
    if (property == nullptr) {
        WLOGFE("window property is nullptr.");
        return;
    }
    auto mode = node->GetWindowMode();
    Rect winRect = property->GetRequestRect();
    auto displayId = node->GetDisplayId();
    WLOGFD("[Before CascadeLayout] windowId: %{public}u, mode: %{public}u, type: %{public}u requestRect: [%{public}d, "
        "%{public}d, %{public}u, %{public}u]", node->GetWindowId(), mode, node->GetWindowType(), winRect.posX_,
        winRect.posY_, winRect.width_, winRect.height_);
    switch (mode) {
        case WindowMode::WINDOW_MODE_SPLIT_PRIMARY:
            winRect = cascadeRectsMap_[displayId].primaryRect_;
            break;
        case WindowMode::WINDOW_MODE_SPLIT_SECONDARY:
            winRect = cascadeRectsMap_[displayId].secondaryRect_;
            break;
        case WindowMode::WINDOW_MODE_FULLSCREEN: {
            bool needAvoid = (node->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
            winRect = needAvoid ? limitRectMap_[displayId] : displayGroupInfo_->GetDisplayRect(displayId);
            break;
        }
        case WindowMode::WINDOW_MODE_FLOATING: {
            if (node->GetWindowType() == WindowType::WINDOW_TYPE_APP_COMPONENT) {
                break;
            }
            UpdateWindowSizeLimits(node);
            winRect = property->GetRequestRect();
            ApplyWindowRectConstraints(node, winRect);
            break;
        }
        default:
            WLOGFW("Layout invalid mode, winId: %{public}u, mode: %{public}u", node->GetWindowId(), mode);
    }
    WLOGFD("[After CascadeLayout] windowId: %{public}u, isDecor: %{public}u, winRect: [%{public}d, %{public}d, "
        "%{public}u, %{public}u]", node->GetWindowId(), node->GetDecoStatus(), winRect.posX_, winRect.posY_,
        winRect.width_, winRect.height_);

    const Rect& lastWinRect = node->GetWindowRect();
    node->SetWindowRect(winRect);

    // postProcess after update winRect
    CalcAndSetNodeHotZone(winRect, node);
    UpdateSurfaceBounds(node, winRect, lastWinRect);
    NotifyClientAndAnimation(node, winRect, node->GetWindowSizeChangeReason());
}

void WindowLayoutPolicyCascade::LimitDividerPositionBySplitRatio(DisplayId displayId, Rect& winRect) const
{
    int32_t oriPos = IsVerticalDisplay(displayId) ? winRect.posY_ : winRect.posX_;
    int32_t& dstPos = IsVerticalDisplay(displayId) ? winRect.posY_ : winRect.posX_;
    if (splitRatioPointsMap_[displayId].size() == 0) {
        return;
    }
    uint32_t minDiff = std::max(limitRectMap_[displayId].width_, limitRectMap_[displayId].height_);
    int32_t closestPoint = oriPos;
    for (const auto& elem : splitRatioPointsMap_[displayId]) {
        uint32_t diff = (oriPos > elem) ? static_cast<uint32_t>(oriPos - elem) : static_cast<uint32_t>(elem - oriPos);
        if (diff < minDiff) {
            closestPoint = elem;
            minDiff = diff;
        }
    }
    dstPos = closestPoint;
}

void WindowLayoutPolicyCascade::InitSplitRects(DisplayId displayId)
{
    float virtualPixelRatio = displayGroupInfo_->GetDisplayVirtualPixelRatio(displayId);
    uint32_t dividerWidth = static_cast<uint32_t>(DIVIDER_WIDTH * virtualPixelRatio);
    auto& dividerRect = cascadeRectsMap_[displayId].dividerRect_;
    const auto& displayRect = displayGroupInfo_->GetDisplayRect(displayId);
    if (!IsVerticalDisplay(displayId)) {
        dividerRect = { static_cast<uint32_t>((displayRect.width_ - dividerWidth) * DEFAULT_SPLIT_RATIO), 0,
                dividerWidth, displayRect.height_ };
    } else {
        dividerRect = { 0, static_cast<uint32_t>((displayRect.height_ - dividerWidth) * DEFAULT_SPLIT_RATIO),
               displayRect.width_, dividerWidth };
    }
    SetSplitRectByDivider(dividerRect, displayId);
}

void WindowLayoutPolicyCascade::SetSplitRectByDivider(const Rect& divRect, DisplayId displayId)
{
    auto& dividerRect = cascadeRectsMap_[displayId].dividerRect_;
    auto& primaryRect = cascadeRectsMap_[displayId].primaryRect_;
    auto& secondaryRect = cascadeRectsMap_[displayId].secondaryRect_;
    const auto& displayRect = displayGroupInfo_->GetDisplayRect(displayId);

    dividerRect.width_ = divRect.width_;
    dividerRect.height_ = divRect.height_;
    if (!IsVerticalDisplay(displayId)) {
        primaryRect.posX_ = displayRect.posX_;
        primaryRect.posY_ = displayRect.posY_;
        primaryRect.width_ = divRect.posX_;
        primaryRect.height_ = displayRect.height_;

        secondaryRect.posX_ = divRect.posX_ + static_cast<int32_t>(dividerRect.width_);
        secondaryRect.posY_ = displayRect.posY_;
        secondaryRect.width_ = static_cast<uint32_t>(static_cast<int32_t>(displayRect.width_) - secondaryRect.posX_);
        secondaryRect.height_ = displayRect.height_;
    } else {
        primaryRect.posX_ = displayRect.posX_;
        primaryRect.posY_ = displayRect.posY_;
        primaryRect.height_ = divRect.posY_;
        primaryRect.width_ = displayRect.width_;

        secondaryRect.posX_ = displayRect.posX_;
        secondaryRect.posY_ = divRect.posY_ + static_cast<int32_t>(dividerRect.height_);
        secondaryRect.height_ = static_cast<uint32_t>(static_cast<int32_t>(displayRect.height_) - secondaryRect.posY_);
        secondaryRect.width_ = displayRect.width_;
    }
    WLOGFD("DividerRect: [%{public}d %{public}d %{public}u %{public}u] "
        "PrimaryRect: [%{public}d %{public}d %{public}u %{public}u] "
        "SecondaryRect: [%{public}d %{public}d %{public}u %{public}u]",
        dividerRect.posX_, dividerRect.posY_, dividerRect.width_, dividerRect.height_,
        primaryRect.posX_, primaryRect.posY_, primaryRect.width_, primaryRect.height_,
        secondaryRect.posX_, secondaryRect.posY_, secondaryRect.width_, secondaryRect.height_);
}

Rect WindowLayoutPolicyCascade::GetCurCascadeRect(const sptr<WindowNode>& node) const
{
    Rect cascadeRect = {0, 0, 0, 0};
    const DisplayId& displayId = node->GetDisplayId();
    const auto& appWindowNodeVec = *(const_cast<WindowLayoutPolicyCascade*>(this)->
        displayGroupWindowTree_[displayId][WindowRootNodeType::APP_WINDOW_NODE]);
    const auto& aboveAppWindowNodeVec = *(const_cast<WindowLayoutPolicyCascade*>(this)->
        displayGroupWindowTree_[displayId][WindowRootNodeType::ABOVE_WINDOW_NODE]);

    std::vector<std::vector<sptr<WindowNode>>> roots = { aboveAppWindowNodeVec, appWindowNodeVec };
    for (auto& root : roots) {
        for (auto iter = root.rbegin(); iter != root.rend(); iter++) {
            if ((*iter)->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
                (*iter)->GetWindowId() != node->GetWindowId()) {
                auto property = (*iter)->GetWindowProperty();
                if (property != nullptr) {
                    cascadeRect = ((*iter)->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING ?
                        property->GetWindowRect() : property->GetRequestRect());
                }
                WLOGFD("Get current cascadeRect: %{public}u [%{public}d, %{public}d, %{public}u, %{public}u]",
                    (*iter)->GetWindowId(), cascadeRect.posX_, cascadeRect.posY_,
                    cascadeRect.width_, cascadeRect.height_);
                break;
            }
        }
    }

    if (WindowHelper::IsEmptyRect(cascadeRect)) {
        WLOGFD("cascade rect is empty use first cascade rect");
        return cascadeRectsMap_[displayId].defaultCascadeRect_;
    }
    return StepCascadeRect(cascadeRect, displayId);
}

Rect WindowLayoutPolicyCascade::StepCascadeRect(Rect rect, DisplayId displayId) const
{
    float virtualPixelRatio = displayGroupInfo_->GetDisplayVirtualPixelRatio(displayId);
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
    WLOGFD("Step cascadeRect :[%{public}d, %{public}d, %{public}u, %{public}u]",
        cascadeRect.posX_, cascadeRect.posY_, cascadeRect.width_, cascadeRect.height_);
    return cascadeRect;
}

void WindowLayoutPolicyCascade::SetDefaultCascadeRect(const sptr<WindowNode>& node)
{
    auto property = node->GetWindowProperty();
    if (property == nullptr) {
        WLOGFE("window property is nullptr.");
        return;
    }
    if (!WindowHelper::IsEmptyRect(property->GetRequestRect())) {
        return;
    }

    static bool isFirstAppWindow = true;
    Rect rect;
    if (WindowHelper::IsAppWindow(property->GetWindowType()) && isFirstAppWindow) {
        WLOGFD("Set first app window cascade rect");
        rect = cascadeRectsMap_[node->GetDisplayId()].defaultCascadeRect_;
        isFirstAppWindow = false;
    } else if (WindowHelper::IsAppWindow(property->GetWindowType()) && !isFirstAppWindow) {
        WLOGFD("Set other app window cascade rect");
        rect = GetCurCascadeRect(node);
    } else {
        // system window
        WLOGFD("Set system window cascade rect");
        rect = cascadeRectsMap_[node->GetDisplayId()].defaultCascadeRect_;
    }
    WLOGFD("Set cascadeRect :[%{public}d, %{public}d, %{public}u, %{public}u]",
        rect.posX_, rect.posY_, rect.width_, rect.height_);
    node->SetRequestRect(rect);
    node->SetDecoStatus(true);
}

Rect WindowLayoutPolicyCascade::GetDividerRect(DisplayId displayId) const
{
    Rect dividerRect = {0, 0, 0, 0};
    if (cascadeRectsMap_.find(displayId) != std::end(cascadeRectsMap_)) {
        dividerRect = cascadeRectsMap_[displayId].dividerRect_;
    }
    return dividerRect;
}
} // Rosen
} // OHOS
