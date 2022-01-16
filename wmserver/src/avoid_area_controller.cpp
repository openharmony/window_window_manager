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

#include "avoid_area_controller.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "AvoidAreaController"};
}

const int32_t AVOID_NUM = 4;

bool AvoidAreaController::IsAvoidAreaNode(const sptr<WindowNode>& node) const
{
    if (avoidType_.find(node->GetWindowType()) != avoidType_.end()) {
        return true;
    }
    return false;
}

static AvoidPos GetAvoidPosType(const Rect& rect)
{
    if (rect.width_ >=  rect.height_) {
        if (rect.posY_ == 0) {
            return AvoidPos::AVOID_POS_TOP;
        } else {
            return AvoidPos::AVOID_POS_BOTTOM;
            }
    } else {
        if (rect.posX_ == 0) {
            return AvoidPos::AVOID_POS_LEFT;
        } else {
            return AvoidPos::AVOID_POS_RIGHT;
        }
    }
    return AvoidPos::AVOID_POS_UNKNOWN;
}

WMError AvoidAreaController::AddAvoidAreaNode(const sptr<WindowNode>& node)
{
    uint32_t windowId = node->GetWindowId();
    auto iter = avoidNodes_.find(windowId);
    if (iter != avoidNodes_.end()) {
        WLOGFE("windowId: %{public}d is added.AddAvoidAreaNode Failed", windowId);
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    // add
    avoidNodes_[windowId] = node;
    WLOGFI("AvoidArea, WindowId:%{public}d add. And the windowType is %{public}d", windowId, node->GetWindowType());

    // get all Area info and notify windowcontainer
    std::vector<Rect> avoidAreas = GetAvoidArea();
    DumpAvoidArea(avoidAreas);
    UseCallbackNotifyAvoidAreaChanged(avoidAreas);
    return WMError::WM_OK;
}

WMError AvoidAreaController::RemoveAvoidAreaNode(const sptr<WindowNode>& node)
{
    uint32_t windowId = node->GetWindowId();
    auto iter = avoidNodes_.find(windowId);
    if (iter == avoidNodes_.end()) {
        WLOGFE("windowId: %{public}d not exist. RemoveAvoidAreaNode Failed.", windowId);
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    // remove
    avoidNodes_.erase(iter);
    WLOGFI("AvoidArea, WindowId:%{public}d remove. And the windowType is %{public}d", windowId, node->GetWindowType());

    // get all Area info and notify windowcontainer
    std::vector<Rect> avoidAreas = GetAvoidArea();
    DumpAvoidArea(avoidAreas);
    UseCallbackNotifyAvoidAreaChanged(avoidAreas);
    return WMError::WM_OK;
}

WMError AvoidAreaController::UpdateAvoidAreaNode(const sptr<WindowNode>& node)
{
    uint32_t windowId = node->GetWindowId();
    auto iter = avoidNodes_.find(windowId);
    if (iter == avoidNodes_.end()) {
        WLOGFE("windowId: %{public}d not exist. UpdateAvoidAreaNode Failed.", windowId);
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    // update
    avoidNodes_[windowId] = node;
    WLOGFI("AvoidArea, WindowId:%{public}d Update. And the windowType is %{public}d", windowId, node->GetWindowType());

    // get all Area info and notify windowcontainer
    std::vector<Rect> avoidAreas = GetAvoidArea();
    DumpAvoidArea(avoidAreas);
    UseCallbackNotifyAvoidAreaChanged(avoidAreas);
    return WMError::WM_OK;
}

std::vector<Rect> AvoidAreaController::GetAvoidArea() const
{
    std::vector<Rect> avoidArea(AVOID_NUM, {0, 0, 0, 0});  // avoid area  left, top, right, bottom
    for (auto iter = avoidNodes_.begin(); iter != avoidNodes_.end(); ++iter) {
        Rect curRect = iter->second->GetLayoutRect();
        auto curPos = GetAvoidPosType(curRect);
        if (curPos == AvoidPos::AVOID_POS_UNKNOWN) {
            WLOGFE("GetAvoidArea AVOID_POS_UNKNOWN Rect: x : %{public}d, y:  %{public}d, w: %{public}u h: %{public}u",
                static_cast<uint32_t>(curRect.posX_), static_cast<uint32_t>(curRect.posY_),
                static_cast<uint32_t>(curRect.width_), static_cast<uint32_t>(curRect.height_));
            continue;
        }
        avoidArea[static_cast<uint32_t>(curPos)] = curRect;
    }
    return avoidArea;
}

std::vector<Rect> AvoidAreaController::GetAvoidAreaByType(AvoidAreaType avoidAreaType) const
{
    if (avoidAreaType != AvoidAreaType::TYPE_SYSTEM) {
        WLOGFE("GetAvoidAreaByType. Support Type is AvoidAreaType::TYPE_SYSTEM. But current type is %{public}u",
            static_cast<uint32_t>(avoidAreaType));
        std::vector<Rect> avoidArea(AVOID_NUM, {0, 0, 0, 0});
        return avoidArea;
    }
    WLOGFI("AvoidAreaController::GetAvoidAreaByType Success");
    return GetAvoidArea();
}

void AvoidAreaController::UseCallbackNotifyAvoidAreaChanged(std::vector<Rect>& avoidArea) const
{
    if (updateAvoidAreaCallBack_) {
        updateAvoidAreaCallBack_(avoidArea);
    }
}

void AvoidAreaController::DumpAvoidArea(const std::vector<Rect>& avoidArea) const
{
    WLOGFI("----------------- AvoidArea Begin-----------------");
    WLOGFI("  No [   x    y    w    h]");
    for (uint32_t i = 0; i < avoidArea.size(); i++) {
        WLOGFI("%{public}4u [%{public}4d %{public}4d %{public}4u %{public}4u]", i,
            avoidArea[i].posX_, avoidArea[i].posY_, avoidArea[i].width_, avoidArea[i].height_);
    }
    WLOGFI("----------------- AvoidArea End-----------------");
}
}
}
