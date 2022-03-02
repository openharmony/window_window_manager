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
    WindowLayoutPolicyTile(const Rect& displayRect, const uint64_t& screenId,
        sptr<WindowNode>& belowAppNode, sptr<WindowNode>& appNode, sptr<WindowNode>& aboveAppNode);
    ~WindowLayoutPolicyTile() = default;
    void Launch() override;
    void AddWindowNode(sptr<WindowNode>& node) override;
    void RemoveWindowNode(sptr<WindowNode>& node) override;
    void UpdateLayoutRect(sptr<WindowNode>& node) override;

private:
    Rect singleRect_ = { 0, 0, 0, 0 };
    std::vector<Rect> doubleRects_ = std::vector<Rect>(2);
    std::vector<Rect> tripleRects_ = std::vector<Rect>(3);
    std::deque<sptr<WindowNode>> foregroundNodes_;
    void UpdateDisplayInfo() override;
    void InitTileWindowRects();
    void AssignNodePropertyForTileWindows();
    void LayoutForegroundNodeQueue();
    void InitForegroundNodeQueue();
    void ForegroundNodeQueuePushBack(sptr<WindowNode>& node);
    void ForegroundNodeQueueRemove(sptr<WindowNode>& node);
};
}
}
#endif // OHOS_ROSEN_WINDOW_LAYOUT_POLICY_TILE_H
