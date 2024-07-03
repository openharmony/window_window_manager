/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "persistent_storage.h"
#include "remote_animation.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"
#include "window_manager_service_utils.h"
#include "wm_common_inner.h"
#include "wm_math.h"
#include <transaction/rs_sync_transaction_controller.h>

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "Layout"};
}

uint32_t WindowLayoutPolicy::floatingBottomPosY_ = 0;
uint32_t WindowLayoutPolicy::maxFloatingWindowSize_ = 1920;  // 1920: default max size of floating window

WindowLayoutPolicy::WindowLayoutPolicy(DisplayGroupWindowTree& displayGroupWindowTree)
    : displayGroupWindowTree_(displayGroupWindowTree)
{
    limitRectMap_ = DisplayGroupInfo::GetInstance().GetAllDisplayRects();
}

void WindowLayoutPolicy::Launch()
{
    WLOGI("WindowLayoutPolicy::Launch");
}

void WindowLayoutPolicy::Reorder()
{
    WLOGI("WindowLayoutPolicy::Reorder");
}

void WindowLayoutPolicy::LimitWindowToBottomRightCorner(const sptr<WindowNode>& node)
{
    Rect windowRect = node->GetRequestRect();
    Rect displayRect = DisplayGroupInfo::GetInstance().GetDisplayRect(node->GetDisplayId());
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

    WLOGD("WindowId: %{public}d, newRect: [%{public}d, %{public}d, %{public}d, %{public}d]",
        node->GetWindowId(), windowRect.posX_, windowRect.posY_, windowRect.width_, windowRect.height_);

    for (auto& childNode : node->children_) {
        LimitWindowToBottomRightCorner(childNode);
    }
}

void WindowLayoutPolicy::UpdateDisplayGroupRect()
{
    Rect newDisplayGroupRect = { 0, 0, 0, 0 };
    // current multi-display is only support left-right combination, maxNum is two
    for (auto& elem : DisplayGroupInfo::GetInstance().GetAllDisplayRects()) {
        newDisplayGroupRect.posX_ = std::min(displayGroupRect_.posX_, elem.second.posX_);
        newDisplayGroupRect.posY_ = std::min(displayGroupRect_.posY_, elem.second.posY_);
        int32_t right = std::max(newDisplayGroupRect.posX_ + static_cast<int32_t>(newDisplayGroupRect.width_),
                                 elem.second.posX_+ static_cast<int32_t>(elem.second.width_));
        newDisplayGroupRect.width_ = right - newDisplayGroupRect.posX_;
        int32_t maxHeight = std::max(newDisplayGroupRect.posY_ + static_cast<int32_t>(newDisplayGroupRect.height_),
                                     elem.second.posY_+ static_cast<int32_t>(elem.second.height_));
        newDisplayGroupRect.height_ = maxHeight - newDisplayGroupRect.posY_;
    }
    displayGroupRect_ = newDisplayGroupRect;
    WLOGD("Update displayGroupRect: [%{public}d, %{public}d, %{public}d, %{public}d]",
        displayGroupRect_.posX_, displayGroupRect_.posY_, displayGroupRect_.width_, displayGroupRect_.height_);
}

void WindowLayoutPolicy::UpdateDisplayGroupLimitRect()
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
        newDisplayGroupLimitRect.width_  = static_cast<uint32_t>(maxWidth - newDisplayGroupLimitRect.posX_);
        newDisplayGroupLimitRect.height_ = static_cast<uint32_t>(maxHeight - newDisplayGroupLimitRect.posY_);
    }
    displayGroupLimitRect_ = newDisplayGroupLimitRect;
    WLOGFD("Update displayGroupLimitRect: [%{public}d, %{public}d, %{public}d, %{public}d]",
        displayGroupLimitRect_.posX_, displayGroupLimitRect_.posY_,
        displayGroupLimitRect_.width_, displayGroupLimitRect_.height_);
}

void WindowLayoutPolicy::UpdateRectInDisplayGroup(const sptr<WindowNode>& node,
                                                  const Rect& oriDisplayRect,
                                                  const Rect& newDisplayRect)
{
    Rect newRect = node->GetRequestRect();
    WLOGD("Before update rect in display group, windowId: %{public}d, rect: [%{public}d, %{public}d, "
        "%{public}d, %{public}d]", node->GetWindowId(), newRect.posX_, newRect.posY_, newRect.width_, newRect.height_);

    newRect.posX_ = newRect.posX_ - oriDisplayRect.posX_ + newDisplayRect.posX_;
    newRect.posY_ = newRect.posY_ - oriDisplayRect.posY_ + newDisplayRect.posY_;
    node->SetRequestRect(newRect);
    WLOGD("After update rect in display group, windowId: %{public}d, newRect: [%{public}d, %{public}d, "
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
    if (DisplayGroupInfo::GetInstance().GetAllDisplayIds().size() > 1) {
        isMultiDisplay_ = true;
        WLOGFD("Current mode is multi-display");
    } else {
        isMultiDisplay_ = false;
        WLOGFD("Current mode is not multi-display");
    }
}

void WindowLayoutPolicy::UpdateRectInDisplayGroupForAllNodes(DisplayId displayId,
                                                             const Rect& oriDisplayRect,
                                                             const Rect& newDisplayRect)
{
    WLOGFD("DisplayId: %{public}" PRIu64", oriDisplayRect: [ %{public}d, %{public}d, %{public}d, %{public}d] "
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
        WLOGFD("Recalculate window rect in display group, displayId: %{public}" PRIu64", rootType: %{public}d",
            displayId, iter.first);
    }
}

void WindowLayoutPolicy::UpdateDisplayRectAndDisplayGroupInfo(const std::map<DisplayId, Rect>& displayRectMap)
{
    for (auto& elem : displayRectMap) {
        auto& displayId = elem.first;
        auto& displayRect = elem.second;
        DisplayGroupInfo::GetInstance().SetDisplayRect(displayId, displayRect);
    }
}

void WindowLayoutPolicy::PostProcessWhenDisplayChange()
{
    DisplayGroupInfo::GetInstance().UpdateLeftAndRightDisplayId();
    UpdateMultiDisplayFlag();
    Launch();
}

void WindowLayoutPolicy::ProcessDisplayCreate(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap)
{
    const auto& oriDisplayRectMap = DisplayGroupInfo::GetInstance().GetAllDisplayRects();
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
    WLOGI("Process display create, displayId: %{public}" PRIu64"", displayId);
}

void WindowLayoutPolicy::ProcessDisplayDestroy(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap)
{
    const auto& oriDisplayRectMap = DisplayGroupInfo::GetInstance().GetAllDisplayRects();
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
    WLOGI("Process display destroy, displayId: %{public}" PRIu64"", displayId);
}

void WindowLayoutPolicy::ProcessDisplaySizeChangeOrRotation(DisplayId displayId,
                                                            const std::map<DisplayId, Rect>& displayRectMap)
{
    const auto& oriDisplayRectMap = DisplayGroupInfo::GetInstance().GetAllDisplayRects();
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
    WLOGI("Process display change, displayId: %{public}" PRIu64"", displayId);
}

void WindowLayoutPolicy::ProcessDisplayVprChange(DisplayId displayId)
{
    Launch();
}

void WindowLayoutPolicy::LayoutWindowNodesByRootType(const std::vector<sptr<WindowNode>>& nodeVec)
{
    if (nodeVec.empty()) {
        WLOGW("The node vector is empty!");
        return;
    }
    for (auto& node : nodeVec) {
        LayoutWindowNode(node);
    }
}

void WindowLayoutPolicy::NotifyAnimationSizeChangeIfNeeded()
{
    if (!RemoteAnimation::CheckAnimationController()) {
        WLOGFD("no animation controller!");
        return;
    }
    std::vector<uint32_t> fullScreenWinIds;
    std::vector<uint32_t> floatMainIds;
    for (auto& iter : displayGroupWindowTree_) {
        auto& displayWindowTree = iter.second;
        auto& nodeVec = *(displayWindowTree[WindowRootNodeType::APP_WINDOW_NODE]);
        if (nodeVec.empty()) {
            WLOGE("The node vector is empty!");
            return;
        }
        for (auto& node : nodeVec) {
            // just has one fullscreen app node on foreground
            if (WindowHelper::IsMainFullScreenWindow(node->GetWindowType(), node->GetWindowMode())) {
                fullScreenWinIds.emplace_back(node->GetWindowId());
            }
            if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
                floatMainIds.emplace_back(node->GetWindowId());
            }
        }
    }
    RemoteAnimation::NotifyAnimationTargetsUpdate(fullScreenWinIds, floatMainIds);
}

void WindowLayoutPolicy::LayoutWindowTree(DisplayId displayId)
{
    // reset limit rect
    limitRectMap_[displayId] = DisplayGroupInfo::GetInstance().GetDisplayRect(displayId);
    displayGroupLimitRect_ = displayGroupRect_;

    // ensure that the avoid area windows are traversed first
    auto& displayWindowTree = displayGroupWindowTree_[displayId];
    LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::ABOVE_WINDOW_NODE]));
    LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::APP_WINDOW_NODE]));
    LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::BELOW_WINDOW_NODE]));
}

void WindowLayoutPolicy::LayoutWindowNode(const sptr<WindowNode>& node)
{
    if (node == nullptr || node->parent_ == nullptr) {
        WLOGFE("Node or it's parent is nullptr");
        return;
    }
    if (!node->currentVisibility_) {
        WLOGFD("window[%{public}u] currently not visible, no need to layout", node->GetWindowId());
        return;
    }

    /*
     * 1. update window rect
     * 2. update diplayLimitRect and displayGroupRect if this is avoidNode
     */
    UpdateLayoutRect(node);
    if (WindowHelper::IsSystemBarWindow(node->GetWindowType())) {
        UpdateDisplayLimitRect(node, limitRectMap_[node->GetDisplayId()]);
        UpdateDisplayGroupLimitRect();
        WindowInnerManager::GetInstance().NotifyDisplayLimitRectChange(limitRectMap_);
    }
    for (auto& childNode : node->children_) {
        LayoutWindowNode(childNode);
    }
}

bool WindowLayoutPolicy::IsVerticalDisplay(DisplayId displayId) const
{
    return DisplayGroupInfo::GetInstance().GetDisplayRect(displayId).width_ <
        DisplayGroupInfo::GetInstance().GetDisplayRect(displayId).height_;
}

void WindowLayoutPolicy::NotifyClientAndAnimation(const sptr<WindowNode>& node,
    const Rect& winRect, WindowSizeChangeReason reason)
{
    if (node->GetWindowToken()) {
        auto type = node->GetWindowType();
        auto syncTransactionController = RSSyncTransactionController::GetInstance();
        if (reason == WindowSizeChangeReason::ROTATION && syncTransactionController && IsNeedAnimationSync(type)) {
            node->GetWindowToken()->UpdateWindowRect(winRect, node->GetDecoStatus(), reason,
                syncTransactionController->GetRSTransaction());
        } else {
            node->GetWindowToken()->UpdateWindowRect(winRect, node->GetDecoStatus(), reason);
        }
        WLOGFD("Id: %{public}d, winRect:[%{public}d, %{public}d, %{public}u, %{public}u], reason: "
            "%{public}u", node->GetWindowId(), winRect.posX_, winRect.posY_, winRect.width_, winRect.height_, reason);
    }
    if ((reason != WindowSizeChangeReason::MOVE) && (node->GetWindowType() != WindowType::WINDOW_TYPE_DOCK_SLICE)) {
        node->ResetWindowSizeChangeReason();
    }
    NotifyAnimationSizeChangeIfNeeded();
}

bool WindowLayoutPolicy::IsNeedAnimationSync(WindowType type)
{
    if (type == WindowType::WINDOW_TYPE_POINTER ||
        type == WindowType::WINDOW_TYPE_BOOT_ANIMATION) {
        return false;
    }
    return true;
}

Rect WindowLayoutPolicy::CalcEntireWindowHotZone(const sptr<WindowNode>& node, const Rect& winRect, uint32_t hotZone,
    float vpr, TransformHelper::Vector2 hotZoneScale)
{
    Rect rect = winRect;
    uint32_t hotZoneX = static_cast<uint32_t>(hotZone * vpr / hotZoneScale.x_);
    uint32_t hotZoneY = static_cast<uint32_t>(hotZone * vpr / hotZoneScale.y_);

    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        if (rect.width_ < rect.height_) {
            rect.posX_ -= static_cast<int32_t>(hotZoneX);
            rect.width_ += (hotZoneX + hotZoneX);
        } else {
            rect.posY_ -= static_cast<int32_t>(hotZoneY);
            rect.height_ += (hotZoneY + hotZoneY);
        }
    } else if (node->GetWindowType() == WindowType::WINDOW_TYPE_LAUNCHER_RECENT) {
        rect = DisplayGroupInfo::GetInstance().GetDisplayRect(node->GetDisplayId());
    } else if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
        rect.posX_ -= static_cast<int32_t>(hotZoneX);
        rect.posY_ -= static_cast<int32_t>(hotZoneY);
        rect.width_ += (hotZoneX + hotZoneX);
        rect.height_ += (hotZoneY + hotZoneY);
    }
    return rect;
}

void WindowLayoutPolicy::CalcAndSetNodeHotZone(const Rect& winRect, const sptr<WindowNode>& node)
{
    float virtualPixelRatio = DisplayGroupInfo::GetInstance().GetDisplayVirtualPixelRatio(node->GetDisplayId());
    TransformHelper::Vector2 hotZoneScale(1, 1);
    if (node->GetWindowProperty()->isNeedComputerTransform()) {
        node->ComputeTransform();
        hotZoneScale = WindowHelper::CalculateHotZoneScale(node->GetWindowProperty()->GetTransformMat());
    }

    auto hotZoneRectTouch = CalcEntireWindowHotZone(node, winRect, HOTZONE_TOUCH, virtualPixelRatio, hotZoneScale);
    auto hotZoneRectPointer = CalcEntireWindowHotZone(node, winRect, HOTZONE_POINTER, virtualPixelRatio, hotZoneScale);

    node->SetEntireWindowTouchHotArea(hotZoneRectTouch);
    node->SetEntireWindowPointerHotArea(hotZoneRectPointer);

    std::vector<Rect> requestedHotAreas;
    node->GetWindowProperty()->GetTouchHotAreas(requestedHotAreas);
    std::vector<Rect> touchHotAreas;
    std::vector<Rect> pointerHotAreas;
    if (requestedHotAreas.empty()) {
        touchHotAreas.emplace_back(hotZoneRectTouch);
        pointerHotAreas.emplace_back(hotZoneRectPointer);
    } else {
        if (!WindowHelper::CalculateTouchHotAreas(winRect, requestedHotAreas, touchHotAreas)) {
            WLOGFW("some parameters in requestedHotAreas are abnormal");
        }
        pointerHotAreas = touchHotAreas;
    }
    node->SetTouchHotAreas(touchHotAreas);
    node->SetPointerHotAreas(pointerHotAreas);
}

WindowSizeLimits WindowLayoutPolicy::GetSystemSizeLimits(const sptr<WindowNode>& node,
    const Rect& displayRect, float vpr)
{
    WindowSizeLimits systemLimits;
    systemLimits.maxWidth_ = static_cast<uint32_t>(maxFloatingWindowSize_ * vpr);
    systemLimits.maxHeight_ = static_cast<uint32_t>(maxFloatingWindowSize_ * vpr);

    // Float camera window has a special limit:
    // if display sw <= 600dp, portrait: min width = display sw * 30%, landscape: min width = sw * 50%
    // if display sw > 600dp, portrait: min width = display sw * 12%, landscape: min width = sw * 30%
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
        uint32_t smallWidth = displayRect.height_ <= displayRect.width_ ? displayRect.height_ : displayRect.width_;
        float hwRatio = static_cast<float>(displayRect.height_) / static_cast<float>(displayRect.width_);
        if (smallWidth <= static_cast<uint32_t>(600 * vpr)) { // sw <= 600dp
            if (displayRect.width_ <= displayRect.height_) {
                systemLimits.minWidth_ = static_cast<uint32_t>(smallWidth * 0.3); // min width = display sw * 0.3
            } else {
                systemLimits.minWidth_ = static_cast<uint32_t>(smallWidth * 0.5); // min width = display sw * 0.5
            }
        } else {
            if (displayRect.width_ <= displayRect.height_) {
                systemLimits.minWidth_ = static_cast<uint32_t>(smallWidth * 0.12); // min width = display sw * 0.12
            } else {
                systemLimits.minWidth_ = static_cast<uint32_t>(smallWidth * 0.3); // min width = display sw * 0.3
            }
        }
        systemLimits.minHeight_ = static_cast<uint32_t>(systemLimits.minWidth_ * hwRatio);
    } else {
        systemLimits.minWidth_ = static_cast<uint32_t>(MIN_FLOATING_WIDTH * vpr);
        systemLimits.minHeight_ = static_cast<uint32_t>(MIN_FLOATING_HEIGHT * vpr);
    }
    WLOGFD("[System SizeLimits] [maxWidth: %{public}u, minWidth: %{public}u, maxHeight: %{public}u, "
        "minHeight: %{public}u]", systemLimits.maxWidth_, systemLimits.minWidth_,
        systemLimits.maxHeight_, systemLimits.minHeight_);
    return systemLimits;
}

void WindowLayoutPolicy::UpdateWindowSizeLimits(const sptr<WindowNode>& node)
{
    const auto& displayRect = DisplayGroupInfo::GetInstance().GetDisplayRect(node->GetDisplayId());
    const auto& virtualPixelRatio = DisplayGroupInfo::GetInstance().GetDisplayVirtualPixelRatio(node->GetDisplayId());
    const auto& systemLimits = GetSystemSizeLimits(node, displayRect, virtualPixelRatio);
    const auto& customizedLimits = node->GetWindowSizeLimits();

    WindowSizeLimits newLimits = systemLimits;

    // configured limits of floating window
    uint32_t configuredMaxWidth = static_cast<uint32_t>(customizedLimits.maxWidth_ * virtualPixelRatio);
    uint32_t configuredMaxHeight = static_cast<uint32_t>(customizedLimits.maxHeight_ * virtualPixelRatio);
    uint32_t configuredMinWidth = static_cast<uint32_t>(customizedLimits.minWidth_ * virtualPixelRatio);
    uint32_t configuredMinHeight = static_cast<uint32_t>(customizedLimits.minHeight_ * virtualPixelRatio);

    // calculate new limit size
    if (systemLimits.minWidth_ <= configuredMaxWidth && configuredMaxWidth <= systemLimits.maxWidth_) {
        newLimits.maxWidth_ = configuredMaxWidth;
    }
    if (systemLimits.minHeight_ <= configuredMaxHeight && configuredMaxHeight <= systemLimits.maxHeight_) {
        newLimits.maxHeight_ = configuredMaxHeight;
    }
    if (systemLimits.minWidth_ <= configuredMinWidth && configuredMinWidth <= newLimits.maxWidth_) {
        newLimits.minWidth_ = configuredMinWidth;
    }
    if (systemLimits.minHeight_ <= configuredMinHeight && configuredMinHeight <= newLimits.maxHeight_) {
        newLimits.minHeight_ = configuredMinHeight;
    }

    // calculate new limit ratio
    newLimits.maxRatio_ = static_cast<float>(newLimits.maxWidth_) / static_cast<float>(newLimits.minHeight_);
    newLimits.minRatio_ = static_cast<float>(newLimits.minWidth_) / static_cast<float>(newLimits.maxHeight_);
    if (newLimits.minRatio_ <= customizedLimits.maxRatio_ && customizedLimits.maxRatio_ <= newLimits.maxRatio_) {
        newLimits.maxRatio_ = customizedLimits.maxRatio_;
    }
    if (newLimits.minRatio_ <= customizedLimits.minRatio_ && customizedLimits.minRatio_ <= newLimits.maxRatio_) {
        newLimits.minRatio_ = customizedLimits.minRatio_;
    }

    // recalculate limit size by new ratio
    uint32_t newMaxWidth = static_cast<uint32_t>(static_cast<float>(newLimits.maxHeight_) * newLimits.maxRatio_);
    newLimits.maxWidth_ = std::min(newMaxWidth, newLimits.maxWidth_);
    uint32_t newMinWidth = static_cast<uint32_t>(static_cast<float>(newLimits.minHeight_) * newLimits.minRatio_);
    newLimits.minWidth_ = std::max(newMinWidth, newLimits.minWidth_);
    uint32_t newMaxHeight = static_cast<uint32_t>(static_cast<float>(newLimits.maxWidth_) / newLimits.minRatio_);
    newLimits.maxHeight_ = std::min(newMaxHeight, newLimits.maxHeight_);
    uint32_t newMinHeight = static_cast<uint32_t>(static_cast<float>(newLimits.minWidth_) / newLimits.maxRatio_);
    newLimits.minHeight_ = std::max(newMinHeight, newLimits.minHeight_);

    WLOGFD("[Update SizeLimits] winId: %{public}u, Width: [max:%{public}u, min:%{public}u], Height: [max:%{public}u, "
        "min:%{public}u], Ratio: [max:%{public}f, min:%{public}f]", node->GetWindowId(), newLimits.maxWidth_,
        newLimits.minWidth_, newLimits.maxHeight_, newLimits.minHeight_, newLimits.maxRatio_, newLimits.minRatio_);
    node->SetWindowUpdatedSizeLimits(newLimits);
}

AvoidPosType WindowLayoutPolicy::GetAvoidPosType(const Rect& rect, DisplayId displayId) const
{
    const auto& displayRectMap = DisplayGroupInfo::GetInstance().GetAllDisplayRects();
    if (displayRectMap.find(displayId) == std::end(displayRectMap)) {
        WLOGFE("GetAvoidPosType fail. Get display fail. displayId: %{public}" PRIu64"", displayId);
        return AvoidPosType::AVOID_POS_UNKNOWN;
    }
    const auto& displayRect = DisplayGroupInfo::GetInstance().GetDisplayRect(displayId);
    return WindowHelper::GetAvoidPosType(rect, displayRect);
}

void WindowLayoutPolicy::UpdateDisplayLimitRect(const sptr<WindowNode>& node, Rect& limitRect)
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
    WLOGFD("AvoidNodeId: %{public}d, avoidNodeRect: [%{public}d %{public}d "
        "%{public}u %{public}u], limitDisplayRect: [%{public}d %{public}d, %{public}u %{public}u]",
        node->GetWindowId(), layoutRect.posX_, layoutRect.posY_, layoutRect.width_, layoutRect.height_,
        limitRect.posX_, limitRect.posY_, limitRect.width_, limitRect.height_);
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

static void AdjustFixedOrientationRSSurfaceNode(const sptr<WindowNode>& node, const Rect& winRect,
    std::shared_ptr<RSSurfaceNode> surfaceNode, sptr<DisplayInfo> displayInfo)
{
    if (!displayInfo) {
        WLOGFE("display invaild");
        return;
    }
    auto requestOrientation = node->GetRequestedOrientation();
    if (!WmsUtils::IsFixedOrientation(requestOrientation, node->GetWindowMode(), node->GetWindowFlags())) {
        return;
    }

    auto displayOri = displayInfo->GetDisplayOrientation();
    auto displayW = displayInfo->GetWidth();
    auto displayH = displayInfo->GetHeight();
    if (WINDOW_TO_DISPLAY_ORIENTATION_MAP.count(requestOrientation) == 0) {
        return;
    }
    int32_t diffOrientation = static_cast<int32_t>(WINDOW_TO_DISPLAY_ORIENTATION_MAP.at(requestOrientation)) -
        static_cast<int32_t>(displayOri);
    float rotation = (displayInfo->GetIsDefaultVertical() ? -90.f : 90.f) * (diffOrientation); // 90.f is base degree
    WLOGFD("[FixOrientation] %{public}d adjust display [%{public}d, %{public}d], rotation: %{public}f",
        node->GetWindowId(), displayW, displayH, rotation);
    surfaceNode->SetTranslateX((displayW - static_cast<int32_t>(winRect.width_)) / 2); // 2 is half
    surfaceNode->SetTranslateY((displayH - static_cast<int32_t>(winRect.height_)) / 2); // 2 is half
    surfaceNode->SetPivotX(0.5); // 0.5 means center
    surfaceNode->SetPivotY(0.5); // 0.5 means center
    surfaceNode->SetRotation(rotation);
}

static void SetBounds(const sptr<WindowNode>& node, const Rect& winRect, const Rect& preRect)
{
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_APP_COMPONENT ||
        node->GetWindowSizeChangeReason() == WindowSizeChangeReason::TRANSFORM) {
        WLOGI("not need to update bounds");
        return;
    }

    WLOGFD("Name:%{public}s id:%{public}u preRect: [%{public}d, %{public}d, %{public}d, %{public}d], "
        "winRect: [%{public}d, %{public}d, %{public}d, %{public}d],  %{public}u", node->GetWindowName().c_str(),
        node->GetWindowId(), preRect.posX_, preRect.posY_, preRect.width_, preRect.height_,
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_, node->GetWindowSizeChangeReason());
    auto& displayGroupInfo = DisplayGroupInfo::GetInstance();
    if (node->leashWinSurfaceNode_) {
        if (winRect != preRect) {
            // avoid animation interpreted when client coming
            node->leashWinSurfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
        }
        if (node->startingWinSurfaceNode_) {
            node->startingWinSurfaceNode_->SetBounds(0, 0, winRect.width_, winRect.height_);
        }
        if (node->surfaceNode_) {
            node->surfaceNode_->SetBounds(0, 0, winRect.width_, winRect.height_);
        }
        AdjustFixedOrientationRSSurfaceNode(node, winRect, node->leashWinSurfaceNode_,
            displayGroupInfo.GetDisplayInfo(node->GetDisplayId()));
    } else if (node->surfaceNode_) {
        node->surfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
        AdjustFixedOrientationRSSurfaceNode(node, winRect, node->surfaceNode_,
            displayGroupInfo.GetDisplayInfo(node->GetDisplayId()));
    }
}

void WindowLayoutPolicy::UpdateSurfaceBounds(const sptr<WindowNode>& node, const Rect& winRect, const Rect& preRect)
{
    wptr<WindowNode> weakNode = node;
    auto SetBoundsFunc = [weakNode, winRect, preRect]() {
        auto winNode = weakNode.promote();
        if (winNode == nullptr) {
            WLOGI("winNode is nullptr");
            return;
        }
        SetBounds(winNode, winRect, preRect);
    };

    switch (node->GetWindowSizeChangeReason()) {
        case WindowSizeChangeReason::MAXIMIZE:
            [[fallthrough]];
        case WindowSizeChangeReason::RECOVER: {
            const RSAnimationTimingProtocol timingProtocol(400); // animation time
            RSNode::Animate(timingProtocol, RSAnimationTimingCurve::EASE_OUT, SetBoundsFunc);
            break;
        }
        case WindowSizeChangeReason::ROTATION: {
            if (WmsUtils::IsFixedOrientation(node->GetRequestedOrientation(),
                node->GetWindowMode(), node->GetWindowFlags())) {
                auto disInfo = DisplayGroupInfo::GetInstance().GetDisplayInfo(node->GetDisplayId());
                if (disInfo && disInfo->GetDisplayStateChangeType() != DisplayStateChangeType::UPDATE_ROTATION) {
                    WLOGI("[FixOrientation] winNode %{public}u orientation, skip animation", node->GetWindowId());
                    SetBoundsFunc();
                    return;
                }
            }
            const RSAnimationTimingProtocol timingProtocol(600); // animation time
            const RSAnimationTimingCurve curve_ = RSAnimationTimingCurve::CreateCubicCurve(
                0.2, 0.0, 0.2, 1.0); // animation curve: cubic [0.2, 0.0, 0.2, 1.0]
            RSNode::Animate(timingProtocol, curve_, SetBoundsFunc);
            break;
        }
        case WindowSizeChangeReason::FULL_TO_SPLIT:
        case WindowSizeChangeReason::SPLIT_TO_FULL: {
            const RSAnimationTimingProtocol timingProtocol(350); // animation time
            RSNode::Animate(timingProtocol, RSAnimationTimingCurve::EASE_OUT, SetBoundsFunc);
            break;
        }
        case WindowSizeChangeReason::UNDEFINED:
            [[fallthrough]];
        default:
            SetBoundsFunc();
    }
}

Rect WindowLayoutPolicy::GetDisplayGroupRect() const
{
    return displayGroupRect_;
}

void WindowLayoutPolicy::SetSplitRatioPoints(DisplayId displayId, const std::vector<int32_t>& splitRatioPoints)
{
    splitRatioPointsMap_[displayId] = splitRatioPoints;
}

Rect WindowLayoutPolicy::GetDividerRect(DisplayId displayId) const
{
    return INVALID_EMPTY_RECT;
}

bool WindowLayoutPolicy::IsTileRectSatisfiedWithSizeLimits(const sptr<WindowNode>& node)
{
    return true;
}

void WindowLayoutPolicy::SetCascadeRectBottomPosYLimit(uint32_t floatingBottomPosY)
{
    floatingBottomPosY_ = floatingBottomPosY;
}

void WindowLayoutPolicy::SetMaxFloatingWindowSize(uint32_t maxSize)
{
    maxFloatingWindowSize_ = maxSize;
}

void WindowLayoutPolicy::GetStoragedAspectRatio(const sptr<WindowNode>& node)
{
    if (!WindowHelper::IsMainWindow(node->GetWindowType())) {
        return;
    }

    std::string abilityName = node->abilityInfo_.abilityName_;
    std::vector<std::string> nameVector;
    if (abilityName.size() > 0) {
        nameVector = WindowHelper::Split(abilityName, ".");
    }
    std::string keyName = nameVector.empty() ? node->abilityInfo_.bundleName_ :
                                                node->abilityInfo_.bundleName_ + "." + nameVector.back();
    if (PersistentStorage::HasKey(keyName, PersistentStorageType::ASPECT_RATIO)) {
        float ratio = 0.0;
        PersistentStorage::Get(keyName, ratio, PersistentStorageType::ASPECT_RATIO);
        node->SetAspectRatio(ratio);
    }
}

void WindowLayoutPolicy::FixWindowRectWithinDisplay(const sptr<WindowNode>& node) const
{
    auto displayId = node->GetDisplayId();
    const Rect& displayRect = DisplayGroupInfo::GetInstance().GetDisplayRect(displayId);
    auto displayInfo = DisplayGroupInfo::GetInstance().GetDisplayInfo(displayId);
    auto type = node->GetWindowType();
    Rect rect = node->GetRequestRect();
    switch (type) {
        case WindowType::WINDOW_TYPE_STATUS_BAR:
            rect.posY_ = displayRect.posY_;
            break;
        case WindowType::WINDOW_TYPE_NAVIGATION_BAR:
            rect.posY_ = static_cast<int32_t>(displayRect.height_) + displayRect.posY_ -
                static_cast<int32_t>(rect.height_);
            break;
        default:
            if (!displayInfo->GetWaterfallDisplayCompressionStatus()) {
                return;
            }
            rect.posY_ = std::max(rect.posY_, displayRect.posY_);
            rect.posY_ = std::min(rect.posY_, displayRect.posY_ + static_cast<int32_t>(displayRect.height_));
    }
    node->SetRequestRect(rect);
    WLOGFD("WinId: %{public}d, requestRect: [%{public}d, %{public}d, %{public}u, %{public}u]",
        node->GetWindowId(), rect.posX_, rect.posY_, rect.width_, rect.height_);
}

void WindowLayoutPolicy::GetMaximizeRect(const sptr<WindowNode>& node, Rect& maxRect)
{
    WLOGFI("WindowLayoutPolicy GetMaximizeRect maxRect = %{public}d, %{public}d, %{public}u, %{public}u ",
        maxRect.posX_, maxRect.posY_, maxRect.width_, maxRect.height_);
}
}
}