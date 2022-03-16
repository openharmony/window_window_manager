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

#include "window_layout_policy_tile.h"
#include <ability_manager_client.h>
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowLayoutPolicyTile"};
    constexpr uint32_t MAX_WIN_NUM_VERTICAL = 2;
    constexpr uint32_t MAX_WIN_NUM_HORIZONTAL = 3;
}
WindowLayoutPolicyTile::WindowLayoutPolicyTile(const Rect& displayRect, const uint64_t& screenId,
    sptr<WindowNode>& belowAppNode, sptr<WindowNode>& appNode, sptr<WindowNode>& aboveAppNode)
    : WindowLayoutPolicy(displayRect, screenId, belowAppNode, appNode, aboveAppNode)
{
}

void WindowLayoutPolicyTile::Launch()
{
    // compute limit rect
    UpdateDisplayInfo();
    // select app min win in queue, and minimize others
    InitForegroundNodeQueue();
    AssignNodePropertyForTileWindows();
    LayoutForegroundNodeQueue();
    LayoutWindowNode(belowAppWindowNode_);
    WLOGFI("WindowLayoutPolicyTile::Launch");
}

void WindowLayoutPolicyTile::UpdateDisplayInfo()
{
    limitRect_ = displayRect_;
    LayoutWindowNode(aboveAppWindowNode_);
    InitTileWindowRects();
}

void WindowLayoutPolicyTile::InitTileWindowRects()
{
    constexpr uint32_t edgeInterval = 48;
    constexpr uint32_t midInterval = 24;
    float virtualPixelRatio = GetVirtualPixelRatio();
    uint32_t edgeIntervalVp = static_cast<uint32_t>(edgeInterval * virtualPixelRatio);
    uint32_t midIntervalVp = static_cast<uint32_t>(midInterval * virtualPixelRatio);

    constexpr float ratio = 0.75;  // 0.75: default height/width ratio
    constexpr float edgeRatio = 0.125;
    constexpr int half = 2;
    maxTileWinNum_ = IsVertical() ? MAX_WIN_NUM_VERTICAL : MAX_WIN_NUM_HORIZONTAL;
    presetRects_.clear();
    int x = limitRect_.posX_ + (limitRect_.width_ * edgeRatio);
    int y = limitRect_.posY_ + (limitRect_.height_ * edgeRatio);
    uint32_t w = limitRect_.width_ * ratio;
    uint32_t h = limitRect_.height_ * ratio;
    std::vector<Rect> single = {{ x, y, w, h }};
    presetRects_.emplace_back(single);
    for (uint32_t num = 2; num <= maxTileWinNum_; num++) { // start calc preset with 2 windows
        w = (limitRect_.width_ - edgeIntervalVp * half - midIntervalVp * (num - 1)) / num;
        std::vector<Rect> curLevel;
        for (uint32_t i = 0; i < num; i++) {
            int curX = limitRect_.posX_ + edgeIntervalVp + i * (w + midIntervalVp);
            Rect curRect = { curX, y, w, h };
            WLOGFI("presetRects: level %{public}d, id %{public}d, [%{public}d %{public}d %{public}d %{public}d]",
                num, i, curX, y, w, h);
            curLevel.emplace_back(curRect);
        }
        presetRects_.emplace_back(curLevel);
    }
}

void WindowLayoutPolicyTile::AddWindowNode(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        ForegroundNodeQueuePushBack(node);
        AssignNodePropertyForTileWindows();
        LayoutForegroundNodeQueue();
    } else {
        UpdateWindowNode(node); // currently, update and add do the same process
    }
}

void WindowLayoutPolicyTile::UpdateWindowNode(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    WindowLayoutPolicy::UpdateWindowNode(node);
    if (avoidTypes_.find(node->GetWindowType()) != avoidTypes_.end()) {
        InitTileWindowRects();
        AssignNodePropertyForTileWindows();
        LayoutForegroundNodeQueue();
    }
}

void WindowLayoutPolicyTile::RemoveWindowNode(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    WLOGFI("RemoveWindowNode %{public}d in tile", node->GetWindowId());
    auto type = node->GetWindowType();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree();
    } else {
        ForegroundNodeQueueRemove(node);
        AssignNodePropertyForTileWindows();
        LayoutForegroundNodeQueue();
    }
    Rect winRect = node->GetWindowProperty()->GetWindowRect();
    node->GetWindowToken()->UpdateWindowRect(winRect, node->GetWindowSizeChangeReason());
}

void WindowLayoutPolicyTile::LayoutForegroundNodeQueue()
{
    for (auto& node : foregroundNodes_) {
        Rect lastRect = node->GetLayoutRect();
        Rect winRect = node->GetWindowProperty()->GetWindowRect();
        node->SetLayoutRect(winRect);
        CalcAndSetNodeHotZone(winRect, node);
        if (!(lastRect == winRect)) {
            node->GetWindowToken()->UpdateWindowRect(winRect, node->GetWindowSizeChangeReason());
            node->surfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
        }
        for (auto& childNode : node->children_) {
            LayoutWindowNode(childNode);
        }
    }
}

void WindowLayoutPolicyTile::InitForegroundNodeQueue()
{
    foregroundNodes_.clear();
    for (auto& node : appWindowNode_->children_) {
        if (WindowHelper::IsMainWindow(node->GetWindowType())) {
            ForegroundNodeQueuePushBack(node);
        }
    }
}

void WindowLayoutPolicyTile::ForegroundNodeQueuePushBack(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    WLOGFI("add win in tile for win id: %{public}d", node->GetWindowId());
    while (foregroundNodes_.size() >= maxTileWinNum_) {
        auto removeNode = foregroundNodes_.front();
        foregroundNodes_.pop_front();
        WLOGFI("pop win in queue head id: %{public}d, for add new win", removeNode->GetWindowId());
        if (removeNode->abilityToken_ != nullptr) {
            WLOGFI("minimize win %{public}d in tile", removeNode->GetWindowId());
            AAFwk::AbilityManagerClient::GetInstance()->MinimizeAbility(removeNode->abilityToken_);
        }
    }
    foregroundNodes_.push_back(node);
}

void WindowLayoutPolicyTile::ForegroundNodeQueueRemove(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    auto iter = std::find(foregroundNodes_.begin(), foregroundNodes_.end(), node);
    if (iter != foregroundNodes_.end()) {
        WLOGFI("remove win in tile for win id: %{public}d", node->GetWindowId());
        foregroundNodes_.erase(iter);
    }
}

void WindowLayoutPolicyTile::AssignNodePropertyForTileWindows()
{
    // set rect for foreground windows
    uint32_t num = foregroundNodes_.size();
    if (num > maxTileWinNum_ || num > presetRects_.size() || num == 0) {
        WLOGE("invalid tile queue");
        return;
    }
    std::vector<Rect>& presetRect = presetRects_[num - 1];
    if (presetRect.size() != num) {
        WLOGE("invalid preset rects");
        return;
    }
    auto rectIt = presetRect.begin();
    for (auto node : foregroundNodes_) {
        auto& rect = (*rectIt);
        node->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        node->GetWindowToken()->UpdateWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        node->SetWindowRect(rect);
        node->hasDecorated_ = true;
        WLOGFI("set rect for qwin id: %{public}d [%{public}d %{public}d %{public}d %{public}d]",
            node->GetWindowId(), rect.posX_, rect.posY_, rect.width_, rect.height_);
        rectIt++;
    }
}

void WindowLayoutPolicyTile::UpdateLayoutRect(sptr<WindowNode>& node)
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
    Rect displayRect = displayRect_;
    Rect limitRect = displayRect;
    Rect winRect = node->GetWindowProperty()->GetWindowRect();

    WLOGFI("Id:%{public}d, avoid:%{public}d parLimit:%{public}d floating:%{public}d, sub:%{public}d, " \
        "deco:%{public}d, type:%{public}d, requestRect:[%{public}d, %{public}d, %{public}d, %{public}d]",
        node->GetWindowId(), needAvoid, parentLimit, floatingWindow, subWindow, decorEnbale,
        static_cast<uint32_t>(type), winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    if (needAvoid) {
        limitRect = limitRect_;
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
    LimitWindowSize(node, displayRect, winRect);
    node->SetLayoutRect(winRect);
    CalcAndSetNodeHotZone(winRect, node);
    if (!(lastRect == winRect)) {
        node->GetWindowToken()->UpdateWindowRect(winRect, node->GetWindowSizeChangeReason());
        node->surfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    }
}
} // Rosen
} // OHOS