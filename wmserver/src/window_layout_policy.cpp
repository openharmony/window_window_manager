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

WindowLayoutPolicy::WindowLayoutPolicy(const sptr<DisplayGroupInfo>& displayGroupInfo,
    DisplayGroupWindowTree& displayGroupWindowTree)
    : displayGroupInfo_(displayGroupInfo), displayGroupWindowTree_(displayGroupWindowTree)
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

void WindowLayoutPolicy::LimitWindowToBottomRightCorner(const sptr<WindowNode>& node)
{
    Rect windowRect = node->GetRequestRect();
    Rect displayRect = displayGroupInfo_->GetDisplayRect(node->GetDisplayId());
    windowRect.posX_ = std::max(windowRect.posX_, displayRect.posX_);
    windowRect.posY_ = std::max(windowRect.posY_, displayRect.posY_);
    windowRect.width_ = std::min(windowRect.width_, displayRect.width_);
    windowRect.height_ = std::min(windowRect.height_, displayRect.height_);

    if (windowRect.posX_ + static_cast<int32_t>(windowRect.width_) >
        displayRect.posX_ + static_cast<int32_t>(displayRect.width_)) {
        windowRect.posX_ = displayRect.posX_ + static_cast<int32_t>(displayRect.width_) -
            static_cast<int32_t>(windowRect.width_);
    }

    if (windowRect.posY_ + static_cast<int32_t>(windowRect.height_) >
        displayRect.posY_ + static_cast<int32_t>(displayRect.height_)) {
        windowRect.posY_ = displayRect.posY_ + static_cast<int32_t>(displayRect.height_) -
            static_cast<int32_t>(windowRect.height_);
    }
    node->SetRequestRect(windowRect);

    WLOGFI("windowId: %{public}d, newRect: [%{public}d, %{public}d, %{public}d, %{public}d]",
        node->GetWindowId(), windowRect.posX_, windowRect.posY_, windowRect.width_, windowRect.height_);

    for (auto& childNode : node->children_) {
        LimitWindowToBottomRightCorner(childNode);
    }
}

void WindowLayoutPolicy::UpdateDisplayGroupRect()
{
    Rect newDisplayGroupRect = { 0, 0, 0, 0 };
    // current mutiDisplay is only support left-right combination, maxNum is two
    for (auto& elem : displayGroupInfo_->GetAllDisplayRects()) {
        newDisplayGroupRect.posX_ = std::min(displayGroupRect_.posX_, elem.second.posX_);
        newDisplayGroupRect.posY_ = std::min(displayGroupRect_.posY_, elem.second.posY_);
        newDisplayGroupRect.width_ += elem.second.width_;
        int32_t maxHeight = std::max(newDisplayGroupRect.posY_ + static_cast<int32_t>(newDisplayGroupRect.height_),
                                     elem.second.posY_+ static_cast<int32_t>(elem.second.height_));
        newDisplayGroupRect.height_ = maxHeight - newDisplayGroupRect.posY_;
    }
    displayGroupRect_ = newDisplayGroupRect;
    WLOGFI("displayGroupRect_: [ %{public}d, %{public}d, %{public}d, %{public}d]",
        displayGroupRect_.posX_, displayGroupRect_.posY_, displayGroupRect_.width_, displayGroupRect_.height_);
}

void WindowLayoutPolicy::UpdateDisplayGroupLimitRect_()
{
    auto firstDisplayLimitRect = limitRectMap_.begin()->second;
    Rect newDisplayGroupLimitRect = { firstDisplayLimitRect.posX_, firstDisplayLimitRect.posY_, 0, 0 };
    for (auto& elem : limitRectMap_) {
        newDisplayGroupLimitRect.posX_ = std::min(newDisplayGroupLimitRect.posX_, elem.second.posX_);
        newDisplayGroupLimitRect.posY_ = std::min(newDisplayGroupLimitRect.posY_, elem.second.posY_);

        int32_t maxWidth = std::max(newDisplayGroupLimitRect.posX_ +
                                    static_cast<int32_t>(newDisplayGroupLimitRect.width_),
                                    elem.second.posX_+ static_cast<int32_t>(elem.second.width_));

        int32_t maxHeight = std::max(newDisplayGroupLimitRect.posY_ +
                                     static_cast<int32_t>(newDisplayGroupLimitRect.height_),
                                     elem.second.posY_+ static_cast<int32_t>(elem.second.height_));
        newDisplayGroupLimitRect.width_  = maxWidth - newDisplayGroupLimitRect.posX_;
        newDisplayGroupLimitRect.height_ = maxHeight - newDisplayGroupLimitRect.posY_;
    }
    displayGroupLimitRect_ = newDisplayGroupLimitRect;
    WLOGFI("displayGroupLimitRect_: [ %{public}d, %{public}d, %{public}d, %{public}d]",
        displayGroupLimitRect_.posX_, displayGroupLimitRect_.posY_,
        displayGroupLimitRect_.width_, displayGroupLimitRect_.height_);
}

void WindowLayoutPolicy::UpdateRectInDisplayGroup(const sptr<WindowNode>& node,
                                                  const Rect& oriDisplayRect,
                                                  const Rect& newDisplayRect)
{
    Rect newRect = node->GetRequestRect();
    WLOGFI("before update rect in display group, windowId: %{public}d, rect: [%{public}d, %{public}d, "
        "%{public}d, %{public}d]", node->GetWindowId(), newRect.posX_, newRect.posY_, newRect.width_, newRect.height_);

    newRect.posX_ = newRect.posX_ - oriDisplayRect.posX_ + newDisplayRect.posX_;
    newRect.posY_ = newRect.posY_ - oriDisplayRect.posY_ + newDisplayRect.posY_;
    node->SetRequestRect(newRect);
    WLOGFI("after update rect in display group, windowId: %{public}d, newRect: [%{public}d, %{public}d, "
        "%{public}d, %{public}d]", node->GetWindowId(), newRect.posX_, newRect.posY_, newRect.width_, newRect.height_);

    for (auto& childNode : node->children_) {
        UpdateRectInDisplayGroup(childNode, oriDisplayRect, newDisplayRect);
    }
}

bool WindowLayoutPolicy::IsMultiDisplay()
{
    return isMultiDisplay_;
}

void WindowLayoutPolicy::UpdateMultiDisplayFlag()
{
    if (displayGroupInfo_->GetAllDisplayRects().size() > 1) {
        isMultiDisplay_ = true;
        WLOGFI("current mode is muti-display");
    } else {
        isMultiDisplay_ = false;
        WLOGFI("current mode is not muti-display");
    }
}

void WindowLayoutPolicy::UpdateRectInDisplayGroupForAllNodes(DisplayId displayId,
                                                             const Rect& oriDisplayRect,
                                                             const Rect& newDisplayRect)
{
    WLOGFI("displayId: %{public}" PRIu64", oriDisplayRect: [ %{public}d, %{public}d, %{public}d, %{public}d] "
        "newDisplayRect: [ %{public}d, %{public}d, %{public}d, %{public}d]",
        displayId, oriDisplayRect.posX_, oriDisplayRect.posY_, oriDisplayRect.width_, oriDisplayRect.height_,
        newDisplayRect.posX_, newDisplayRect.posY_, newDisplayRect.width_, newDisplayRect.height_);

    auto& displayWindowTree = displayGroupWindowTree_[displayId];
    for (auto& iter : displayWindowTree) {
        auto& nodeVector = *(iter.second);
        for (auto& node : nodeVector) {
            if (!node->isShowingOnMultiDisplays_) {
                UpdateRectInDisplayGroup(node, oriDisplayRect, newDisplayRect);
            }
            if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
                LimitWindowToBottomRightCorner(node);
            }
        }
        WLOGFI("Recalculate window rect in display group, displayId: %{public}" PRIu64", rootType: %{public}d",
            displayId, iter.first);
    }
}

void WindowLayoutPolicy::UpdateDisplayRectAndDisplayGroupInfo(const std::map<DisplayId, Rect>& displayRectMap)
{
    for (auto& elem : displayRectMap) {
        auto& displayId = elem.first;
        auto& displayRect = elem.second;
        displayGroupInfo_->SetDisplayRect(displayId, displayRect);
    }
}

void WindowLayoutPolicy::PostProcessWhenDisplayChange()
{
    displayGroupInfo_->UpdateLeftAndRightDisplayId();
    UpdateMultiDisplayFlag();
    UpdateDisplayGroupRect();
    Launch();
    for (auto& elem : displayGroupInfo_->GetAllDisplayRects()) {
        LayoutWindowTree(elem.first);
        WLOGFI("LayoutWindowTree, displayId: %{public}" PRIu64", displayRect: [ %{public}d, %{public}d, %{public}d, "
            "%{public}d]", elem.first, elem.second.posX_, elem.second.posY_, elem.second.width_, elem.second.height_);
    }
}

void WindowLayoutPolicy::ProcessDisplayCreate(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap)
{
    const auto& oriDisplayRectMap = displayGroupInfo_->GetAllDisplayRects();
    // check displayId and displayRectMap size
    if (oriDisplayRectMap.find(displayId) == oriDisplayRectMap.end() ||
        displayRectMap.size() != oriDisplayRectMap.size()) {
        WLOGFE("current display is exited or displayInfo map size is error, displayId: %{public}" PRIu64"", displayId);
        return;
    }
    for (auto& elem : displayRectMap) {
        auto iter = oriDisplayRectMap.find(elem.first);
        if (iter != oriDisplayRectMap.end()) {
            const auto& oriDisplayRect = iter->second;
            const auto& newDisplayRect = elem.second;
            UpdateRectInDisplayGroupForAllNodes(elem.first, oriDisplayRect, newDisplayRect);
        } else {
            if (elem.first != displayId) {
                WLOGFE("Wrong display, displayId: %{public}" PRIu64"", displayId);
                return;
            }
        }
    }
    UpdateDisplayRectAndDisplayGroupInfo(displayRectMap);
    PostProcessWhenDisplayChange();
    WLOGFI("Process display create, displayId: %{public}" PRIu64"", displayId);
}

void WindowLayoutPolicy::ProcessDisplayDestroy(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap)
{
    const auto& oriDisplayRectMap = displayGroupInfo_->GetAllDisplayRects();
    // check displayId and displayRectMap size
    if (oriDisplayRectMap.find(displayId) != oriDisplayRectMap.end() ||
        displayRectMap.size() != oriDisplayRectMap.size()) {
        WLOGFE("can not find current display or displayInfo map size is error, displayId: %{public}" PRIu64"",
               displayId);
        return;
    }
    for (auto oriIter = oriDisplayRectMap.begin(); oriIter != oriDisplayRectMap.end();) {
        auto newIter = displayRectMap.find(oriIter->first);
        if (newIter != displayRectMap.end()) {
            const auto& oriDisplayRect = oriIter->second;
            const auto& newDisplayRect = newIter->second;
            UpdateRectInDisplayGroupForAllNodes(oriIter->first, oriDisplayRect, newDisplayRect);
        } else {
            if (oriIter->first != displayId) {
                WLOGFE("Wrong display, displayId: %{public}" PRIu64"", displayId);
                return;
            }
        }
        ++oriIter;
    }

    UpdateDisplayRectAndDisplayGroupInfo(displayRectMap);
    PostProcessWhenDisplayChange();
    WLOGFI("Process display destroy, displayId: %{public}" PRIu64"", displayId);
}

void WindowLayoutPolicy::ProcessDisplaySizeChangeOrRotation(DisplayId displayId,
                                                            const std::map<DisplayId, Rect>& displayRectMap)
{
    const auto& oriDisplayRectMap = displayGroupInfo_->GetAllDisplayRects();
    // check displayId and displayRectMap size
    if (oriDisplayRectMap.find(displayId) == oriDisplayRectMap.end() ||
        displayRectMap.size() != oriDisplayRectMap.size()) {
        WLOGFE("can not find current display or displayInfo map size is error, displayId: %{public}" PRIu64"",
               displayId);
        return;
    }

    for (auto& elem : displayRectMap) {
        auto iter = oriDisplayRectMap.find(elem.first);
        if (iter != oriDisplayRectMap.end()) {
            UpdateRectInDisplayGroupForAllNodes(elem.first, iter->second, elem.second);
        }
    }

    UpdateDisplayRectAndDisplayGroupInfo(displayRectMap);
    PostProcessWhenDisplayChange();
    WLOGFI("Process display change, displayId: %{public}" PRIu64"", displayId);
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
    auto& displayWindowTree = displayGroupWindowTree_[displayId];
    limitRectMap_[displayId] = displayGroupInfo_->GetDisplayRect(displayId);
    // ensure that the avoid area windows are traversed first
    LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::ABOVE_WINDOW_NODE]));
    if (IsFullScreenRecentWindowExist(*(displayWindowTree[WindowRootNodeType::ABOVE_WINDOW_NODE]))) {
        WLOGFI("recent window on top, early exit layout tree");
        return;
    }
    LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::APP_WINDOW_NODE]));
    LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::BELOW_WINDOW_NODE]));
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
            UpdateDisplayGroupLimitRect_();
        }
    }
    for (auto& childNode : node->children_) {
        LayoutWindowNode(childNode);
    }
}

bool WindowLayoutPolicy::IsVerticalDisplay(DisplayId displayId) const
{
    return displayGroupInfo_->GetDisplayRect(displayId).width_ < displayGroupInfo_->GetDisplayRect(displayId).height_;
}

void WindowLayoutPolicy::UpdateClientRectAndResetReason(const sptr<WindowNode>& node,
    const Rect& lastLayoutRect, const Rect& winRect)
{
    auto reason = node->GetWindowSizeChangeReason();
    if (node->GetWindowToken()) {
        WLOGFI("notify client id: %{public}d, windowRect:[%{public}d, %{public}d, %{public}u, %{public}u], reason: "
            "%{public}u", node->GetWindowId(), winRect.posX_, winRect.posY_, winRect.width_, winRect.height_, reason);
        node->GetWindowToken()->UpdateWindowRect(winRect, node->GetDecoStatus(), reason);
    }
    if (reason == WindowSizeChangeReason::DRAG || reason == WindowSizeChangeReason::DRAG_END) {
        node->ResetWindowSizeChangeReason();
    }
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
    if (node->GetWindowToken()) {
        node->GetWindowToken()->UpdateWindowRect(reqRect, node->GetDecoStatus(), WindowSizeChangeReason::HIDE);
    }
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
        rect = displayGroupInfo_->GetDisplayRect(node->GetDisplayId());
    } else if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
        rect.posX_ -= hotZone;
        rect.posY_ -= hotZone;
        rect.width_ += (hotZone + hotZone);
        rect.height_ += (hotZone + hotZone);
    }
    node->SetHotZoneRect(rect);
}

void WindowLayoutPolicy::LimitFloatingWindowSize(const sptr<WindowNode>& node,
                                                 const Rect& displayRect,
                                                 Rect& winRect) const
{
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t minVerticalFloatingW = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_WIDTH * virtualPixelRatio);
    uint32_t minVerticalFloatingH = static_cast<uint32_t>(MIN_VERTICAL_FLOATING_HEIGHT * virtualPixelRatio);

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
}

DockWindowShowState WindowLayoutPolicy::GetDockWindowShowState(DisplayId displayId, Rect& dockWinRect) const
{
    auto& displayWindowTree = displayGroupWindowTree_[displayId];
    auto& nodeVec = *(displayWindowTree[WindowRootNodeType::ABOVE_WINDOW_NODE]);
    for (auto& node : nodeVec) {
        if (node->GetWindowType() != WindowType::WINDOW_TYPE_LAUNCHER_DOCK) {
            continue;
        }

        dockWinRect = node->GetWindowRect();
        auto displayRect = displayGroupInfo_->GetDisplayRect(displayId);
        WLOGFI("begin dockWinRect :[%{public}d, %{public}d, %{public}u, %{public}u]",
            dockWinRect.posX_, dockWinRect.posY_, dockWinRect.width_, dockWinRect.height_);
        if (dockWinRect.height_ < dockWinRect.width_) {
            if (static_cast<uint32_t>(dockWinRect.posY_) + dockWinRect.height_ == displayRect.height_) {
                return DockWindowShowState::SHOWN_IN_BOTTOM;
            } else {
                return DockWindowShowState::NOT_SHOWN;
            }
        } else {
            if (dockWinRect.posX_ == 0) {
                return DockWindowShowState::SHOWN_IN_LEFT;
            } else if (static_cast<uint32_t>(dockWinRect.posX_) + dockWinRect.width_ == displayRect.width_) {
                return DockWindowShowState::SHOWN_IN_RIGHT;
            } else {
                return DockWindowShowState::NOT_SHOWN;
            }
        }
    }
    return DockWindowShowState::NOT_SHOWN;
}

void WindowLayoutPolicy::LimitMainFloatingWindowPositionWithDrag(const sptr<WindowNode>& node, Rect& winRect) const
{
    if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
        float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
        uint32_t windowTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);
        const Rect lastRect = node->GetWindowRect();
        // fix rect in case of moving window when dragging
        winRect = WindowHelper::GetFixedWindowRectByLimitSize(winRect, lastRect,
            IsVerticalDisplay(node->GetDisplayId()), virtualPixelRatio);

        // if is mutiDisplay, the limit rect should be full limitRect when move or drag
        Rect limitRect;
        if (isMultiDisplay_) {
            limitRect = displayGroupLimitRect_;
        } else {
            limitRect = limitRectMap_[node->GetDisplayId()];
        }
        winRect = WindowHelper::GetFixedWindowRectByLimitPosition(winRect, lastRect,
            virtualPixelRatio, limitRect);
        Rect dockWinRect;
        DockWindowShowState dockShownState = GetDockWindowShowState(node->GetDisplayId(), dockWinRect);
        if (dockShownState == DockWindowShowState::SHOWN_IN_BOTTOM) {
            WLOGFI("dock window show in bottom");
            winRect.posY_ = std::min(dockWinRect.posY_ - static_cast<int32_t>(windowTitleBarH), winRect.posY_);
        } else if (dockShownState == DockWindowShowState::SHOWN_IN_LEFT) {
            WLOGFI("dock window show in left");
            winRect.posX_ = std::max(static_cast<int32_t>(dockWinRect.width_ + windowTitleBarH - winRect.width_),
                                     winRect.posX_);
        } else if (dockShownState == DockWindowShowState::SHOWN_IN_RIGHT) {
            WLOGFI("dock window show in right");
            winRect.posX_ = std::min(dockWinRect.posX_ - static_cast<int32_t>(windowTitleBarH),
                                     winRect.posX_);
        }
    }
}

void WindowLayoutPolicy::LimitMainFloatingWindowPosition(const sptr<WindowNode>& node, Rect& winRect) const
{
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t windowTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);

    Rect limitRect;
    // if is corss-display window, the limit rect should be full limitRect
    if (node->isShowingOnMultiDisplays_) {
        limitRect = displayGroupLimitRect_;
    } else {
        limitRect = limitRectMap_[node->GetDisplayId()];
    }

    // limit position of the main floating window(window which support dragging)
    if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
        Rect dockWinRect;
        DockWindowShowState dockShownState = GetDockWindowShowState(node->GetDisplayId(), dockWinRect);
        winRect.posY_ = std::max(limitRect.posY_, winRect.posY_);
        winRect.posY_ = std::min(limitRect.posY_ + static_cast<int32_t>(limitRect.height_ - windowTitleBarH),
                                 winRect.posY_);
        if (dockShownState == DockWindowShowState::SHOWN_IN_BOTTOM) {
            WLOGFI("dock window show in bottom");
            winRect.posY_ = std::min(dockWinRect.posY_ + static_cast<int32_t>(dockWinRect.height_ - windowTitleBarH),
                                     winRect.posY_);
        }
        winRect.posX_ = std::max(limitRect.posX_ + static_cast<int32_t>(windowTitleBarH - winRect.width_),
                                 winRect.posX_);
        if (dockShownState == DockWindowShowState::SHOWN_IN_LEFT) {
            WLOGFI("dock window show in left");
            winRect.posX_ = std::max(static_cast<int32_t>(dockWinRect.width_ + windowTitleBarH - winRect.width_),
                                     winRect.posX_);
        }
        winRect.posX_ = std::min(limitRect.posX_ + static_cast<int32_t>(limitRect.width_ - windowTitleBarH),
                                 winRect.posX_);
        if (dockShownState == DockWindowShowState::SHOWN_IN_RIGHT) {
            WLOGFI("dock window show in right");
            winRect.posX_ = std::min(dockWinRect.posX_ - static_cast<int32_t>(windowTitleBarH),
                                     winRect.posX_);
        }
    }
}

AvoidPosType WindowLayoutPolicy::GetAvoidPosType(const Rect& rect, DisplayId displayId) const
{
    const auto& displayRectMap = displayGroupInfo_->GetAllDisplayRects();
    if (displayRectMap.find(displayId) == std::end(displayRectMap)) {
        WLOGFE("GetAvoidPosType fail. Get display fail. displayId: %{public}" PRIu64"", displayId);
        return AvoidPosType::AVOID_POS_UNKNOWN;
    }
    const auto& displayRect = displayGroupInfo_->GetDisplayRect(displayId);
    return WindowHelper::GetAvoidPosType(rect, displayRect.width_, displayRect.height_);
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
    float virtualPixelRatio = displayGroupInfo_->GetDisplayVirtualPixelRatio(displayId);
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

void WindowLayoutPolicy::UpdateSurfaceBounds(const sptr<WindowNode>& node, const Rect& winRect)
{
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_APP_COMPONENT) {
        WLOGFI("not need to update bounds");
        return;
    }
    if (node->leashWinSurfaceNode_) {
        node->leashWinSurfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
        if (node->startingWinSurfaceNode_) {
            node->startingWinSurfaceNode_->SetBounds(0, 0, winRect.width_, winRect.height_);
        }
        if (node->surfaceNode_) {
            node->surfaceNode_->SetBounds(0, 0, winRect.width_, winRect.height_);
        }
    } else if (node->surfaceNode_) {
        node->surfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    }
}

Rect WindowLayoutPolicy::GetDisplayGroupRect() const
{
    return displayGroupRect_;
}
}
}
