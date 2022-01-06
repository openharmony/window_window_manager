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

#include <unordered_set>
#include <input_manager.h>
#include <refbase.h>

#include "window_root.h"

namespace OHOS {
namespace Rosen {
class InputWindowMonitor : public RefBase {
public:
    InputWindowMonitor(sptr<WindowRoot>& root) : windowRoot_(root) {}
    ~InputWindowMonitor() = default;
    void UpdateInputWindow(uint32_t windowId);

private:
    sptr<WindowRoot> windowRoot_;
    std::vector<MMI::PhysicalDisplayInfo> physicalDisplays_;
    std::vector<MMI::LogicalDisplayInfo> logicalDisplays_;
    std::unordered_set<WindowType> windowTypeSkipped_ { WindowType::WINDOW_TYPE_POINTER};
    const int INVALID_DISPLAY_ID = -1;
    const int INVALID_WINDOW_ID = -1;
    void TraverseWindowNodes(const std::vector<sptr<WindowNode>>& windowNodes,
                              std::vector<MMI::LogicalDisplayInfo>::iterator& iter);
    void UpdateDisplaysInfo(const sptr<WindowNodeContainer>& container);
};
}
}
#endif // OHOS_INPUT_WINDOW_MONITOR_H
