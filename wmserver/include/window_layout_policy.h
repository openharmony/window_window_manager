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

#ifndef OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H
#define OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H

#include <map>
#include <refbase.h>
#include <set>

#include "window_node.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
using WindowNodeMaps = std::map<DisplayId,
    std::map<WindowRootNodeType, std::unique_ptr<std::vector<sptr<WindowNode>>>>>;
class WindowLayoutPolicy : public RefBase {
public:
    WindowLayoutPolicy() = delete;
    WindowLayoutPolicy(const std::map<DisplayId, Rect>& displayRectMap,
                       WindowNodeMaps& windowNodeMaps);
    ~WindowLayoutPolicy() = default;
    virtual void Launch();
    virtual void Clean();
    virtual void Reset();
    virtual void Reorder();
    virtual void UpdateDisplayInfo(const std::map<DisplayId, Rect>& displayRectMap);
    virtual void AddWindowNode(const sptr<WindowNode>& node) = 0;
    virtual void LayoutWindowTree(DisplayId displayId);
    virtual void RemoveWindowNode(const sptr<WindowNode>& node);
    virtual void UpdateWindowNode(const sptr<WindowNode>& node, bool isAddWindow = false);
    virtual void UpdateLayoutRect(const sptr<WindowNode>& node) = 0;
    float GetVirtualPixelRatio(DisplayId displayId) const;

protected:
    void UpdateFloatingLayoutRect(Rect& limitRect, Rect& winRect);
    void UpdateLimitRect(const sptr<WindowNode>& node, Rect& limitRect);
    virtual void LayoutWindowNode(const sptr<WindowNode>& node);
    AvoidPosType GetAvoidPosType(const Rect& rect, DisplayId displayId) const;
    void CalcAndSetNodeHotZone(Rect layoutOutRect, const sptr<WindowNode>& node) const;
    void LimitWindowSize(const sptr<WindowNode>& node, const Rect& displayRect, Rect& winRect) const;
    void ComputeDecoratedRequestRect(const sptr<WindowNode>& node) const;
    bool IsVerticalDisplay(DisplayId displayId) const;
    bool IsFullScreenRecentWindowExist(const std::vector<sptr<WindowNode>>& nodeVec) const;
    void LayoutWindowNodesByRootType(const std::vector<sptr<WindowNode>>& nodeVec);

    const std::set<WindowType> avoidTypes_ {
        WindowType::WINDOW_TYPE_STATUS_BAR,
        WindowType::WINDOW_TYPE_NAVIGATION_BAR,
    };
    mutable std::map<DisplayId, Rect> displayRectMap_;
    mutable std::map<DisplayId, Rect> limitRectMap_;
    WindowNodeMaps& windowNodeMaps_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H
