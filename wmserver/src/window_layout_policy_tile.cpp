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

#include "window_layout_policy_tile.h"
#include <ability_manager_client.h>
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowLayoutPolicyTile"};
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
    limitRect_ = displayRect_;
    LayoutWindowNode(aboveAppWindowNode_);
    InitTileWindowRects();
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
    constexpr float ratio = 0.75;  // 0.75: default height/width ratio
    constexpr float edgeRatio = 0.125;
    constexpr int half = 2;
    int x = limitRect_.posX_ + (limitRect_.width_ * edgeRatio);
    int y = limitRect_.posY_ + (limitRect_.height_ * edgeRatio);
    uint32_t w = limitRect_.width_ * ratio;
    uint32_t h = limitRect_.height_ * ratio;
    singleRect_ = { x, y, w, h };
    WLOGFI("singleRect_: %{public}d %{public}d %{public}d %{public}d", x, y, w, h);
    x = edgeInterval;
    w = (limitRect_.width_ - edgeInterval * half - midInterval) / half;
    // calc doubleRect
    doubleRects_[0] = {x, y, w, h};
    doubleRects_[1] = {x + w + midInterval, y, w, h};
    WLOGFI("doubleRects_: %{public}d %{public}d %{public}d %{public}d", x, y, w, h);
    // calc tripleRect
    w = (limitRect_.width_ - edgeInterval * half - midInterval * half) / MAX_WIN_NUM_HORIZONTAL;
    tripleRects_[0] = {x, y, w, h};
    tripleRects_[1] = {x + w + midInterval, y, w, h};
    tripleRects_[2] = {x + w * half + midInterval * half, y, w, h}; // 2 is third index
    WLOGFI("tripleRects_: %{public}d %{public}d %{public}d %{public}d", x, y, w, h);
}

void WindowLayoutPolicyTile::AddWindowNode(sptr<WindowNode>& node)
{
    WM_FUNCTION_TRACE();
    if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        UpdateForegroundNodeQueue(node);
        AssignNodePropertyForTileWindows();
        LayoutForegroundNodeQueue();
    } else {
        UpdateWindowNode(node); // currently, update and add do the same process
    }
}

void WindowLayoutPolicyTile::LayoutForegroundNodeQueue()
{
    for (auto& node : foregroundNodes_) {
        Rect lastRect = node->GetLayoutRect();
        Rect winRect = node->GetWindowProperty()->GetWindowRect();
        node->SetLayoutRect(winRect);
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
    lastForegroundNodeId_ = 0;
    for (auto& node : appWindowNode_->children_) {
        UpdateForegroundNodeQueue(node);
    }
}

void WindowLayoutPolicyTile::UpdateForegroundNodeQueue(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    uint32_t maxTileWinNum = IsVertical() ? MAX_WIN_NUM_VERTICAL : MAX_WIN_NUM_HORIZONTAL;
    if (foregroundNodes_.size() < maxTileWinNum) {
        foregroundNodes_.push_back(node);
        lastForegroundNodeId_ = ((++lastForegroundNodeId_) % maxTileWinNum);
    } else {
        AAFwk::AbilityManagerClient::GetInstance()->
            MinimizeAbility(foregroundNodes_[lastForegroundNodeId_]->abilityToken_);
        foregroundNodes_[lastForegroundNodeId_] = node;
        lastForegroundNodeId_ = ((++lastForegroundNodeId_) % maxTileWinNum);
    }
}

void WindowLayoutPolicyTile::AssignNodePropertyForTileWindows()
{
    // set rect for foreground windows
    int num = foregroundNodes_.size();
    if (num == 1) {
        WLOGFI("set rect for win id: %{public}d", foregroundNodes_[0]->GetWindowMode());
        foregroundNodes_[0]->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        foregroundNodes_[0]->SetWindowRect(singleRect_);
        WLOGFI("set rect for win id: %{public}d [%{public}d %{public}d %{public}d %{public}d]",
            foregroundNodes_[0]->GetWindowId(),
            singleRect_.posX_, singleRect_.posY_, singleRect_.width_, singleRect_.height_);
    } else {
        auto& rects = (num == MAX_WIN_NUM_HORIZONTAL) ? tripleRects_ : doubleRects_;
        for (uint32_t i = 0; i < foregroundNodes_.size(); i++) {
            foregroundNodes_[(lastForegroundNodeId_ + i) % num]->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            foregroundNodes_[(lastForegroundNodeId_ + i) % num]->SetWindowRect(rects[i]);
            WLOGFI("set rect for qwin id: %{public}d [%{public}d %{public}d %{public}d %{public}d]",
                foregroundNodes_[(lastForegroundNodeId_ + i) % num]->GetWindowId(),
                rects[i].posX_, rects[i].posY_, rects[i].width_, rects[i].height_);
        }
    }
}
}
}
