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

#include "avoid_area_controller.h"
#include "display_manager_service_inner.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AvoidAreaController"};
}

const int32_t AVOID_NUM = 4;

AvoidAreaController::AvoidAreaController(DisplayId displayId, UpdateAvoidAreaFunc callback)
{
    UpdateAvoidNodesMap(displayId, true);
    updateAvoidAreaCallBack_ = callback;
}

void AvoidAreaController::UpdateAvoidNodesMap(DisplayId displayId, bool isAdd)
{
    if (isAdd) {
        auto avoidNodesMapPtr = std::make_unique<std::map<uint32_t, sptr<WindowNode>>>();
        avoidNodesMaps_.insert(std::make_pair(displayId, std::move(avoidNodesMapPtr)));
    } else {
        avoidNodesMaps_.erase(displayId);
    }
}

bool AvoidAreaController::IsAvoidAreaNode(const sptr<WindowNode>& node) const
{
    if (node == nullptr) {
        WLOGFE("IsAvoidAreaNode Failed, node is nullprt");
        return false;
    }

    if (!WindowHelper::IsAvoidAreaWindow(node->GetWindowType())) {
        WLOGFE("IsAvoidAreaNode Failed, node type is not avoid type");
        return false;
    }

    return true;
}

std::map<uint32_t, sptr<WindowNode>>* AvoidAreaController::GetAvoidNodesByDisplayId(
    DisplayId displayId)
{
    if ((const_cast<AvoidAreaController*>(this)->avoidNodesMaps_).find(displayId) !=
        (const_cast<AvoidAreaController*>(this)->avoidNodesMaps_).end()) {
        return (const_cast<AvoidAreaController*>(this)->avoidNodesMaps_)[displayId].get();
    }
    return nullptr;
}

AvoidPosType AvoidAreaController::GetAvoidPosType(const Rect& rect, DisplayId displayId) const
{
    auto display = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (display == nullptr) {
        WLOGFE("GetAvoidPosType fail. Get display fail. displayId:%{public}" PRIu64"", displayId);
        return AvoidPosType::AVOID_POS_UNKNOWN;
    }
    uint32_t displayWidth = static_cast<uint32_t>(display->GetWidth());
    uint32_t displayHeight = static_cast<uint32_t>(display->GetHeight());

    return WindowHelper::GetAvoidPosType(rect, displayWidth, displayHeight);
}

WMError AvoidAreaController::AvoidControl(const sptr<WindowNode>& node, AvoidControlType type)
{
    if (!IsAvoidAreaNode(node)) {
        WLOGFE("AvoidControl check param Failed. Type: %{public}u", type);
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    auto avoidNodes = GetAvoidNodesByDisplayId(node->GetDisplayId());
    uint32_t windowId = node->GetWindowId();
    auto iter = avoidNodes->find(windowId);
    // do not add a exist node(the same id)
    if (type == AvoidControlType::AVOID_NODE_ADD && iter != avoidNodes->end()) {
        WLOGFE("WinId:%{public}d is added. AvoidControl Add Failed. Type: %{public}u", windowId, type);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    // do not update or removew a unexist node
    if (type != AvoidControlType::AVOID_NODE_ADD && iter == avoidNodes->end()) {
        WLOGFE("WinId:%{public}d not exist. AvoidControl Update or Remove Failed. Type: %{public}u", windowId, type);
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    switch (type) {
        case AvoidControlType::AVOID_NODE_ADD:
            (*avoidNodes)[windowId] = node;
            WLOGFI("WinId:%{public}d add. And the windowType is %{public}d", windowId, node->GetWindowType());
            break;
        case AvoidControlType::AVOID_NODE_UPDATE:
            (*avoidNodes)[windowId] = node;
            WLOGFI("WinId:%{public}d update. And the windowType is %{public}d", windowId, node->GetWindowType());
            break;
        case AvoidControlType::AVOID_NODE_REMOVE:
            avoidNodes->erase(iter);
            WLOGFI("WinId:%{public}d remove. And the windowType is %{public}d", windowId, node->GetWindowType());
            break;
        default:
            WLOGFE("invalid AvoidControlType: %{public}u", type);
            return WMError::WM_ERROR_INVALID_PARAM;
    }

    // get all Area info and notify windowcontainer
    std::vector<Rect> avoidAreas = GetAvoidArea(node->GetDisplayId());
    DumpAvoidArea(avoidAreas);
    UseCallbackNotifyAvoidAreaChanged(avoidAreas, node->GetDisplayId());
    return WMError::WM_OK;
}

std::vector<Rect> AvoidAreaController::GetAvoidArea(DisplayId displayId) const
{
    auto avoidNodesPtr = const_cast<AvoidAreaController*>(this)->GetAvoidNodesByDisplayId(displayId);
    std::vector<Rect> avoidArea(AVOID_NUM, {0, 0, 0, 0});  // avoid area  left, top, right, bottom
    for (auto iter = avoidNodesPtr->begin(); iter != avoidNodesPtr->end(); ++iter) {
        Rect curRect = iter->second->GetWindowRect();
        auto curPos = GetAvoidPosType(curRect, iter->second->GetDisplayId());
        if (curPos == AvoidPosType::AVOID_POS_UNKNOWN) {
            WLOGFE("GetAvoidArea AVOID_POS_UNKNOWN Rect: x : %{public}d, y:  %{public}d, w: %{public}u h: %{public}u",
                static_cast<uint32_t>(curRect.posX_), static_cast<uint32_t>(curRect.posY_),
                static_cast<uint32_t>(curRect.width_), static_cast<uint32_t>(curRect.height_));
            continue;
        }
        avoidArea[static_cast<uint32_t>(curPos)] = curRect;
    }
    return avoidArea;
}

std::vector<Rect> AvoidAreaController::GetAvoidAreaByType(AvoidAreaType avoidAreaType, DisplayId displayId) const
{
    if (avoidAreaType != AvoidAreaType::TYPE_SYSTEM) {
        WLOGFE("GetAvoidAreaByType. Support Type is AvoidAreaType::TYPE_SYSTEM. But current type is %{public}u",
            static_cast<uint32_t>(avoidAreaType));
        std::vector<Rect> avoidArea(AVOID_NUM, {0, 0, 0, 0});
        return avoidArea;
    }
    WLOGFI("AvoidAreaController::GetAvoidAreaByType Success");
    return GetAvoidArea(displayId);
}

void AvoidAreaController::UseCallbackNotifyAvoidAreaChanged(std::vector<Rect>& avoidArea, DisplayId displayId) const
{
    if (updateAvoidAreaCallBack_) {
        updateAvoidAreaCallBack_(avoidArea, displayId);
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
