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

#ifndef OHOS_ROSEN_MINIMIZE_APP_H
#define OHOS_ROSEN_MINIMIZE_APP_H

#include <map>
#include <vector>

#include <refbase.h>
#include "wm_common.h"
#include "window_node.h"

namespace OHOS {
namespace Rosen {
enum class MinimizeReason : uint32_t {
    MINIMIZE_BUTTOM,
    MINIMIZE_ALL,
    LAYOUT_TILE,
    MAX_APP_COUNT,
    OTHER_WINDOW,
};

class MinimizeApp : public RefBase {
public:
    MinimizeApp() = delete;
    ~MinimizeApp() = default;

    static void AddNeedMinimizeApp(const sptr<WindowNode>& node, MinimizeReason reason);
    static void ExecuteMinimizeAll();
    static void ExecuteMinimizeTargetReason(MinimizeReason reason);
    static void SetMinimizedByOtherConfig(bool isMinimizedByOther);
    static void ClearNodesWithReason(MinimizeReason reason);
    static bool IsNodeNeedMinimize(const sptr<WindowNode>& node);
private:
    static inline bool IsFromUser(MinimizeReason reason)
    {
        return (reason == MinimizeReason::MINIMIZE_ALL || reason == MinimizeReason::MINIMIZE_BUTTOM ||
            reason == MinimizeReason::MAX_APP_COUNT || reason == MinimizeReason::LAYOUT_TILE);
    }

    static std::map<MinimizeReason, std::vector<wptr<WindowNode>>> needMinimizeAppNodes_;
    static bool isMinimizedByOtherWindow_;
    static std::recursive_mutex mutex_;
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_MINIMIZE_APP_H
