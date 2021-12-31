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
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowLayoutPolicy"};
}

WMError WindowLayoutPolicy::UpdateDisplayInfo(const Rect& displayRect)
{
    if (displayRect.height_ == 0 || displayRect.width_ == 0) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    displayRect_ = displayRect;
    limitRect_ = displayRect;
    aviodNodes_.clear();
    return WMError::WM_OK;
}

WMError WindowLayoutPolicy::LayoutWindow(sptr<WindowNode>& node)
{
    if (node->GetWindowType() >= WindowType::SYSTEM_WINDOW_END) {
        WLOGFE("unknown window type!");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    UpdateLayoutRects(node);
    if (aviodTypes_.find(node->GetWindowType()) != aviodTypes_.end()) {
        RecordAvoidRect(node);
    }
    return WMError::WM_OK;
}

bool WindowLayoutPolicy::IsRectChanged(const Rect& l, const Rect& r)
{
    return !((l.posX_ == r.posX_) && (l.posY_ == r.posY_) && (l.width_ == r.width_) && (l.height_ == r.height_));
}

bool WindowLayoutPolicy::UpdateLayoutRects(sptr<WindowNode>& node)
{
    bool needAvoid = IsNeedAvoidNode(node);
    bool fullScreen = IsFullScreenNode(node);
    bool parentLimit = IsParentLimitNode(node);
    bool subWindow = (node->GetWindowType() == WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    bool floatingWindow = (node->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING);
    WLOGFI("Id:%{public}d, avoid:%{public}d fullS:%{public}d parLimit:%{public}d float:%{public}d, types:%{public}d",
        node->GetWindowId(), needAvoid, fullScreen, parentLimit, floatingWindow,
        static_cast<uint32_t>(node->GetWindowType()));
    const LayoutRects& layoutRects = node->GetLayoutRects();
    Rect lastRect = layoutRects.rect_;
    Rect dRect = layoutRects.displayRect_;
    Rect pRect = layoutRects.parentRect_;
    Rect lRect = layoutRects.limitRect_;
    Rect winRect = node->GetWindowProperty()->GetWindowRect();

    if (needAvoid) {
        dRect = limitRect_;
    } else {
        dRect = displayRect_;
    }

    if (subWindow) {
        pRect = node->parent_->GetLayoutRects().rect_;
    } else {
        pRect = dRect;
    }

    if (parentLimit) {
        lRect = pRect;
    } else {
        lRect = dRect;
    }

    if (fullScreen) {
        winRect = lRect;
    } else if (!floatingWindow) {
        winRect.width_ = std::min(lRect.width_, winRect.width_);
        winRect.height_ = std::min(lRect.height_, winRect.height_);
        winRect.posX_ = std::max(lRect.posX_, winRect.posX_);
        winRect.posY_ = std::max(lRect.posY_, winRect.posY_);
        winRect.posX_ = std::min(
            lRect.posX_ + static_cast<int32_t>(lRect.width_) - static_cast<int32_t>(winRect.width_),
            winRect.posX_);
        winRect.posY_ = std::min(
            lRect.posY_ + static_cast<int32_t>(lRect.height_) - static_cast<int32_t>(winRect.height_),
            winRect.posY_);
    }
    node->UpdateLayoutRects({ dRect, pRect, lRect, winRect });
    if (IsRectChanged(lastRect, winRect)) {
        node->GetWindowToken()->UpdateWindowRect(winRect);
        node->surfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
        WLOGFI("UpdateLayoutRects for winId: %{public}d, Rect: %{public}d %{public}d %{public}d %{public}d",
               node->GetWindowId(), winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
        return true;
    }
    WLOGFI("UpdateLayoutRects rect not changed for winId: %{public}d", node->GetWindowId());
    return false;
}

void WindowLayoutPolicy::UpdateLimitRect(const sptr<WindowNode>& node)
{
    auto& layoutRects = node->GetLayoutRects();
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR) { // STATUS_BAR
        int32_t boundTop = limitRect_.posY_;
        int32_t rectBottom = layoutRects.rect_.posY_ + layoutRects.rect_.height_;
        int32_t offsetH = rectBottom - boundTop;
        limitRect_.posY_ += offsetH;
        limitRect_.height_ -= offsetH;
    } else if (node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) { // NAVIGATION_BAR
        int32_t boundBottom = limitRect_.posY_ + limitRect_.height_;
        int32_t offsetH = boundBottom - layoutRects.rect_.posY_;
        limitRect_.height_ -= offsetH;
    }
    WLOGFI("after add WinId: %{public}d, limitRect: %{public}d %{public}d %{public}d %{public}d",
        node->GetWindowId(), limitRect_.posX_, limitRect_.posY_, limitRect_.width_, limitRect_.height_);
}

void WindowLayoutPolicy::RecordAvoidRect(const sptr<WindowNode>& node)
{
    uint32_t id = node->GetWindowId();
    if (aviodNodes_.find(id) == aviodNodes_.end()) { // new avoid rect
        aviodNodes_.insert(std::pair<uint32_t, sptr<WindowNode>>(id, node));
        UpdateLimitRect(node);
    } else { // update existing avoid rect
        limitRect_ = displayRect_;
        aviodNodes_[id] = node;
        for (auto item : aviodNodes_) {
            UpdateLimitRect(item.second);
        }
    }
}

bool WindowLayoutPolicy::IsNeedAvoidNode(const sptr<WindowNode>& node)
{
    auto type = node->GetWindowType();
    auto mode = node->GetWindowMode();
    auto flags = node->GetWindowFlags();
    if (mode == WindowMode::WINDOW_MODE_FLOATING) {
        return false;
    }
    if (type >= WindowType::APP_WINDOW_BASE &&
        type <= WindowType::APP_SUB_WINDOW_END &&
        (flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID))) {
        return true;
    }
    return false;
}

bool WindowLayoutPolicy::IsFullScreenNode(const sptr<WindowNode>& node)
{
    auto mode = node->GetWindowMode();
    if (mode == WindowMode::WINDOW_MODE_FULLSCREEN || mode == WindowMode::WINDOW_MODE_SPLIT) {
        return true;
    }
    return false;
}

bool WindowLayoutPolicy::IsParentLimitNode(const sptr<WindowNode>& node)
{
    auto mode = node->GetWindowMode();
    auto flags = node->GetWindowFlags();
    if (mode != WindowMode::WINDOW_MODE_FLOATING &&
        (flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT))) {
        return true;
    }
    return false;
}
}
}
