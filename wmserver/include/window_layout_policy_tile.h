/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_LAYOUT_POLICY_TILE_H
#define OHOS_ROSEN_WINDOW_LAYOUT_POLICY_TILE_H

#include <map>
#include <queue>
#include <refbase.h>
#include <set>

#include "window_layout_policy.h"
#include "window_node.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowLayoutPolicyTile : public WindowLayoutPolicy {
public:
    WindowLayoutPolicyTile() = delete;
    WindowLayoutPolicyTile(DisplayGroupWindowTree& displayGroupWindowTree);
    ~WindowLayoutPolicyTile() = default;
    void Launch() override;
    bool IsTileRectSatisfiedWithSizeLimits(const sptr<WindowNode>& node) override;
    void PerformWindowLayout(const sptr<WindowNode>& node, WindowUpdateType type) override;

private:
    /*
     * methods for calculate maxTileNum and preset tileRects
     */
    void InitTileRects(DisplayId displayId);
    uint32_t GetMaxTileWinNum(DisplayId displayId) const;

    /*
     * methods for update tile queue
     */
    void InitTileQueue(DisplayId displayId);
    void LayoutTileQueue(DisplayId displayId);
    void RefreshTileQueue(DisplayId displayId, std::vector<sptr<WindowNode>>& needMinimizeNodes,
        std::vector<sptr<WindowNode>>& needRecoverNodes);
    void PushBackNodeInTileQueue(const sptr<WindowNode>& node, DisplayId displayId);
    void RemoveNodeFromTileQueue(const sptr<WindowNode>& node);

    /*
     * methods for calculate tile window rect
     */
    void ApplyPresetRectForTileWindows(DisplayId displayId);
    void UpdateLayoutRect(const sptr<WindowNode>& node) override;
    bool IsWindowAlreadyInTileQueue(const sptr<WindowNode>& node);
    bool IsValidTileQueueAndPresetRects(DisplayId displayId);

    std::map<DisplayId, uint32_t> maxTileWinNumMap_;
    std::map<DisplayId, std::vector<std::vector<Rect>>> presetRectsMap_;
    std::map<DisplayId, std::deque<sptr<WindowNode>>> foregroundNodesMap_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_LAYOUT_POLICY_TILE_H
