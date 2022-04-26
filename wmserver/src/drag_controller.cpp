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
#include "drag_controller.h"

#include <vector>

#include "display.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "window_node.h"
#include "window_node_container.h"
#include "window_property.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DragController"};
}

void DragController::UpdateDragInfo(uint32_t windowId)
{
    PointInfo point;
    if (!GetHitPoint(windowId, point)) {
        return;
    }
    sptr<WindowNode> dragNode = windowRoot_->GetWindowNode(windowId);
    if (dragNode == nullptr) {
        return;
    }
    sptr<WindowNode> hitWindowNode = GetHitWindow(dragNode->GetDisplayId(), point);
    if (hitWindowNode == nullptr) {
        WLOGFE("Get point failed %{public}d %{public}d", point.x, point.y);
        return;
    }
    if (hitWindowNode->GetWindowId() == hitWindowId_) {
        hitWindowNode->GetWindowToken()->UpdateWindowDragInfo(point, DragEvent::DRAG_EVENT_MOVE);
        return;
    }
    hitWindowNode->GetWindowToken()->UpdateWindowDragInfo(point, DragEvent::DRAG_EVENT_IN);
    sptr<WindowNode> oldHitWindow = windowRoot_->GetWindowNode(hitWindowId_);
    if (oldHitWindow != nullptr) {
        oldHitWindow->GetWindowToken()->UpdateWindowDragInfo(point, DragEvent::DRAG_EVENT_OUT);
    }
    hitWindowId_ = hitWindowNode->GetWindowId();
}

void DragController::StartDrag(uint32_t windowId)
{
    PointInfo point;
    if (!GetHitPoint(windowId, point)) {
        WLOGFE("Get hit point failed");
        return;
    }
    sptr<WindowNode> dragNode = windowRoot_->GetWindowNode(windowId);
    if (dragNode == nullptr) {
        return;
    }
    sptr<WindowNode> hitWindow = GetHitWindow(dragNode->GetDisplayId(), point);
    if (hitWindow == nullptr) {
        WLOGFE("Get point failed %{public}d %{public}d", point.x, point.y);
        return;
    }
    hitWindow->GetWindowToken()->UpdateWindowDragInfo(point, DragEvent::DRAG_EVENT_IN);
    hitWindowId_ = windowId;
    WLOGFI("start Drag");
}

void DragController::FinishDrag(uint32_t windowId)
{
    sptr<WindowNode> node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("get node failed");
        return;
    }
    if (node->GetWindowType() != WindowType::WINDOW_TYPE_DRAGGING_EFFECT) {
        return;
    }

    sptr<WindowNode> hitWindow = windowRoot_->GetWindowNode(hitWindowId_);
    if (hitWindow != nullptr) {
        auto property = node->GetWindowProperty();
        PointInfo point = {property->GetWindowRect().posX_ + property->GetHitOffset().x,
            property->GetWindowRect().posY_ + property->GetHitOffset().y};
        hitWindow->GetWindowToken()->UpdateWindowDragInfo(point, DragEvent::DRAG_EVENT_END);
    }
    WLOGFI("end drag");
}

sptr<WindowNode> DragController::GetHitWindow(DisplayId id, PointInfo point)
{
    // Need get display by point
    if (id == DISPLAY_ID_INVALID) {
        WLOGFE("Get invalid display");
        return nullptr;
    }
    sptr<WindowNodeContainer> container = windowRoot_->GetOrCreateWindowNodeContainer(id);
    if (container == nullptr) {
        WLOGFE("get container failed %{public}" PRIu64"", id);
        return nullptr;
    }

    std::vector<sptr<WindowNode>> windowNodes;
    container->TraverseContainer(windowNodes);
    for (auto windowNode : windowNodes) {
        if (windowNode->GetWindowType() >= WindowType::WINDOW_TYPE_DRAGGING_EFFECT) {
            continue;
        }
        if (WindowHelper::IsPointInTargetRect(point.x, point.y, windowNode->GetWindowRect())) {
            return windowNode;
        }
    }
    return nullptr;
}

bool DragController::GetHitPoint(uint32_t windowId, PointInfo& point)
{
    sptr<WindowNode> windowNode = windowRoot_->GetWindowNode(windowId);
    if (windowNode == nullptr || windowNode->GetWindowType() != WindowType::WINDOW_TYPE_DRAGGING_EFFECT) {
        WLOGFE("Get hit point failed");
        return false;
    }
    sptr<WindowProperty> property = windowNode->GetWindowProperty();
    point.x = property->GetWindowRect().posX_ + property->GetHitOffset().x;
    point.y = property->GetWindowRect().posY_ + property->GetHitOffset().y;
    return true;
}
}
}
