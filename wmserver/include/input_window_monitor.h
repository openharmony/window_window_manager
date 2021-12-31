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

#ifndef OHOS_INPUT_WINDOW_MONITOR_H
#define OHOS_INPUT_WINDOW_MONITOR_H

#include <input_manager.h>
#include <refbase.h>

#include "window_root.h"

namespace OHOS {
namespace Rosen {
class InputWindowMonitor : public RefBase {
public:
    InputWindowMonitor(sptr<WindowRoot>& root) : windowRoot_(root)
    {
        MMI::PhysicalDisplayInfo physicalDisplayInfo = {
            .id = 0, //todo: update when DMS is ready
            .leftDisplayId = -1, // todo: invalid displayId for testing
            .upDisplayId = -1, // todo: invalid displayId for testing
            .topLeftX = 0, // todo: use wgn info for testing
            .topLeftY = 0, // todo: use wgn info for testing
            .width = 2560, // todo: use wgn info for testing
            .height = 1600, // todo: use wgn info for testing
            .name = "physical_display0", // todo: wait for DMS
            .seatId = "seat0", // todo: update seatId
            .seatName = "default0", // todo: update seatId
            .logicWidth = 2560, // todo: use wgn info for testing
            .logicHeight = 1600, // todo: use wgn info for testing
            .direction = MMI::Direction0 // todo: use direction 0 for testing
        };
        physicalDisplays_.emplace_back(physicalDisplayInfo);
        MMI::LogicalDisplayInfo logicalDisplayInfo = {
            .id = 0, //todo: update when DMS is ready
            .topLeftX = 0, // todo: use wgn info for testing
            .topLeftY = 0, // todo: use wgn info for testing
            .width = 2560, // todo: use wgn info for testing
            .height = 1600, // todo: use wgn info for testing
            .name = "logical_display0", // todo: wait for DMS
            .seatId = "seat0", // todo: update seatId
            .seatName = "default0", // todo: update seatName
            .focusWindowId = -1,
            .windowsInfo_ = {},
        };
        logicalDisplays_.emplace_back(logicalDisplayInfo);
    }
    ~InputWindowMonitor() = default;
    void UpdateInputWindow(uint32_t windowId);

private:
    sptr<WindowRoot> windowRoot_;
    std::vector<MMI::PhysicalDisplayInfo> physicalDisplays_;
    std::vector<MMI::LogicalDisplayInfo> logicalDisplays_;
    void TraverseWindowNodes(const std::vector<sptr<WindowNode>>& windowNodes,
                              std::vector<MMI::LogicalDisplayInfo>::iterator& iter);
};
}
}
#endif // OHOS_INPUT_WINDOW_MONITOR_H
