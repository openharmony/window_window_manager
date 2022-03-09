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
class WindowLayoutPolicy : public RefBase {
public:
    WindowLayoutPolicy() = delete;
    WindowLayoutPolicy(const Rect& displayRect, const uint64_t& screenId,
        sptr<WindowNode>& belowAppNode, sptr<WindowNode>& appNode, sptr<WindowNode>& aboveAppNode);
    ~WindowLayoutPolicy() = default;
    virtual void Launch();
    virtual void Clean();
    virtual void Reset();
    virtual void Reorder();
    virtual void UpdateDisplayInfo();
    virtual void AddWindowNode(sptr<WindowNode>& node);
    virtual void LayoutWindowTree();
    virtual void RemoveWindowNode(sptr<WindowNode>& node);
    virtual void UpdateWindowNode(sptr<WindowNode>& node);
    virtual void UpdateLayoutRect(sptr<WindowNode>& node) = 0;
    void UpdateDefaultFoatingRect();
    float GetVirtualPixelRatio() const;
    Rect GetDisplayLimitRect() const;

protected:
    const Rect& displayRect_;
    const uint64_t& screenId_;
    sptr<WindowNode>& belowAppWindowNode_;
    sptr<WindowNode>& appWindowNode_;
    sptr<WindowNode>& aboveAppWindowNode_;
    Rect limitRect_ = { 0, 0, 0, 0 };
    const std::set<WindowType> avoidTypes_ {
        WindowType::WINDOW_TYPE_STATUS_BAR,
        WindowType::WINDOW_TYPE_NAVIGATION_BAR,
    };
    void UpdateFloatingLayoutRect(Rect& limitRect, Rect& winRect);
    void UpdateLimitRect(const sptr<WindowNode>& node, Rect& limitRect);
    virtual void LayoutWindowNode(sptr<WindowNode>& node);
    AvoidPosType GetAvoidPosType(const Rect& rect);
    void CalcAndSetNodeHotZone(Rect layoutOutRect, sptr<WindowNode>& node);
    void LimitWindowSize(const sptr<WindowNode>& node, const Rect& displayRect, Rect& winRect);
    void SetRectForFloating(const sptr<WindowNode>& node);
    Rect ComputeDecoratedWindowRect(const Rect& winRect);
    bool IsVertical() const;
    Rect defaultFloatingRect_ = { 0, 0, 0, 0 };
};
}
}
#endif // OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H
