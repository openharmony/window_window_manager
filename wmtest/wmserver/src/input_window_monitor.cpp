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

#include "input_window_monitor.h"

#include <vector>

#include <ipc_skeleton.h>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "InputWindowMonitor"};
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
    int32_t displayId = windowNode->GetDisplayId();
    auto container = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("can not get window node container.");
        return;
    }
    std::vector<sptr<WindowNode>> windowNodes;
    container->TraverseContainer(windowNodes);

    auto iter = std::find_if(logicalDisplays_.begin(), logicalDisplays_.end(),
                             [displayId](MMI::LogicalDisplayInfo& logicalDisplay) {
        return logicalDisplay.id == displayId;
    });
    if (iter != logicalDisplays_.end()) {
        TraverseWindowNodes(windowNodes, iter);
        if (!iter->windowsInfo_.empty()) {
            iter->focusWindowId = container->GetFocusWindow();
        }
    } else {
        WLOGFE("There is no display for this window action.");
        return;
    }
    WLOGFI("update display info to IMS.");
    MMI::InputManager::GetInstance()->UpdateDisplayInfo(physicalDisplays_, logicalDisplays_);
}

void InputWindowMonitor::TraverseWindowNodes(const std::vector<sptr<WindowNode>> &windowNodes,
                                              std::vector<MMI::LogicalDisplayInfo>::iterator& iter)
{
    iter->windowsInfo_.clear();
    for (auto& windowNode: windowNodes) {
        MMI::WindowInfo windowInfo = {
            .id = static_cast<int32_t>(windowNode->GetWindowId()),
            .pid = windowNode->GetCallingPid(),
            .uid = windowNode->GetCallingUid(),
            .topLeftX = windowNode->GetLayoutRects().rect_.posX_,
            .topLeftY = windowNode->GetLayoutRects().rect_.posY_,
            .width = static_cast<int32_t>(windowNode->GetLayoutRects().rect_.width_),
            .height = static_cast<int32_t>(windowNode->GetLayoutRects().rect_.height_),
            .displayId = windowNode->GetDisplayId(),
            .agentWindowId = static_cast<int32_t>(windowNode->GetWindowId()),
        };
        iter->windowsInfo_.emplace_back(windowInfo);
    }
}
}
}