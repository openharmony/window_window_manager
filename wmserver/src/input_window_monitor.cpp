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

#include "input_window_monitor.h"

#include <ipc_skeleton.h>

#include "display_manager_service_inner.h"
#include "dm_common.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "InputWindowMonitor"};
}
static inline void convertRectsToMmiRects(const std::vector<Rect>& rects, std::vector<MMI::Rect>& mmiRects)
{
    for (const auto& rect : rects) {
        mmiRects.emplace_back(
            MMI::Rect{ rect.posX_, rect.posY_, static_cast<int32_t>(rect.width_), static_cast<int32_t>(rect.height_) });
    }
}

void InputWindowMonitor::UpdateInputWindow(uint32_t windowId)
{
    if (windowRoot_ == nullptr) {
        WLOGFE("windowRoot is null.");
        return;
    }
    sptr<WindowNode> windowNode = windowRoot_->GetWindowNode(windowId);
    if (windowNode == nullptr) {
        WLOGFE("window node could not be found.");
        return;
    }
    if (windowTypeSkipped_.find(windowNode->GetWindowProperty()->GetWindowType()) != windowTypeSkipped_.end()) {
        return;
    }
    DisplayId displayId = windowNode->GetDisplayId();
    UpdateInputWindowByDisplayId(displayId);
}

void InputWindowMonitor::UpdateInputWindowByDisplayId(DisplayId displayId)
{
    if (displayId == DISPLAY_ID_INVALID) {
        return;
    }
    auto container = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("can not get window node container.");
        return;
    }

    auto displayInfo = container->GetDisplayInfo(displayId);
    if (displayInfo == nullptr) {
        return;
    }

    UpdateDisplayGroupInfo(container, displayGroupInfo_);
    UpdateDisplayInfo(displayInfo, displayGroupInfo_.displaysInfo);
    std::vector<sptr<WindowNode>> windowNodes;
    container->TraverseContainer(windowNodes);
    TraverseWindowNodes(windowNodes, displayGroupInfo_.windowsInfo);
    WLOGFI("update display info to IMS, displayId: %{public}" PRIu64"", displayId);
    MMI::InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo_);
}

void InputWindowMonitor::UpdateDisplayGroupInfo(const sptr<WindowNodeContainer>& windowNodeContainer,
                                                MMI::DisplayGroupInfo& displayGroupInfo)
{
    const Rect&& rect = windowNodeContainer->GetDisplayGroupRect();
    displayGroupInfo.width = static_cast<int32_t>(rect.width_);
    displayGroupInfo.height = static_cast<int32_t>(rect.height_);
    displayGroupInfo.focusWindowId = static_cast<int32_t>(windowNodeContainer->GetFocusWindow());
    displayGroupInfo.windowsInfo.clear();
    displayGroupInfo.displaysInfo.clear();
}

void InputWindowMonitor::UpdateDisplayInfo(const sptr<DisplayInfo>& displayInfo,
                                           std::vector<MMI::DisplayInfo>& displayInfoVector)
{
    uint32_t displayWidth = displayInfo->GetWidth();
    uint32_t displayHeight = displayInfo->GetHeight();
    if (displayInfo->GetRotation() == Rotation::ROTATION_90 || displayInfo->GetRotation() == Rotation::ROTATION_270) {
        std::swap(displayWidth, displayHeight);
    }
    MMI::DisplayInfo display = {
        .id = static_cast<int32_t>(displayInfo->GetDisplayId()),
        .x = displayInfo->GetOffsetX(),
        .y = displayInfo->GetOffsetY(),
        .width = static_cast<int32_t>(displayWidth),
        .height = static_cast<int32_t>(displayHeight),
        .name = (std::stringstream("display ")<<displayInfo->GetDisplayId()).str(),
        .uniq = "default0",
        .direction = GetDisplayDirectionForMmi(displayInfo->GetRotation()),
    };
    auto displayIter = std::find_if(displayInfoVector.begin(), displayInfoVector.end(),
        [&display](MMI::DisplayInfo& displayInfoTmp) {
        return displayInfoTmp.id == display.id;
    });
    if (displayIter != displayInfoVector.end()) {
        *displayIter = display;
    } else {
        displayInfoVector.emplace_back(display);
    }
}

void InputWindowMonitor::TraverseWindowNodes(const std::vector<sptr<WindowNode>> &windowNodes,
                                             std::vector<MMI::WindowInfo>& windowsInfo)
{
    for (const auto& windowNode: windowNodes) {
        if (windowTypeSkipped_.find(windowNode->GetWindowProperty()->GetWindowType()) != windowTypeSkipped_.end()) {
            WLOGFI("window has been skipped. [id: %{public}u, type: %{public}d]", windowNode->GetWindowId(),
                   windowNode->GetWindowProperty()->GetWindowType());
            continue;
        }
        std::vector<Rect> touchHotAreas;
        windowNode->GetTouchHotAreas(touchHotAreas);
        Rect windowRect = windowNode->GetWindowRect();
        MMI::WindowInfo windowInfo = {
            .id = static_cast<int32_t>(windowNode->GetWindowId()),
            .pid = windowNode->GetCallingPid(),
            .uid = windowNode->GetCallingUid(),
            .area = MMI::Rect{ windowRect.posX_, windowRect.posY_,
                static_cast<int32_t>(windowRect.width_), static_cast<int32_t>(windowRect.height_) },
            .agentWindowId = static_cast<int32_t>(windowNode->GetWindowId()),
        };
        convertRectsToMmiRects(touchHotAreas, windowInfo.defaultHotAreas);
        convertRectsToMmiRects(touchHotAreas, windowInfo.pointerHotAreas);
        if (!windowNode->GetWindowProperty()->GetTouchable()) {
            WLOGFI("window is not touchable: %{public}u", windowNode->GetWindowId());
            windowInfo.flags |= MMI::WindowInfo::FLAG_BIT_UNTOUCHABLE;
        }
        windowsInfo.emplace_back(windowInfo);
    }
}

MMI::Direction InputWindowMonitor::GetDisplayDirectionForMmi(Rotation rotation)
{
    MMI::Direction direction = MMI::Direction0;
    switch (rotation) {
        case Rotation::ROTATION_0:
            direction = MMI::Direction0;
            break;
        case Rotation::ROTATION_90:
            direction = MMI::Direction90;
            break;
        case Rotation::ROTATION_180:
            direction = MMI::Direction180;
            break;
        case Rotation::ROTATION_270:
            direction = MMI::Direction270;
            break;
        default:
            break;
    }
    return direction;
}
}
}