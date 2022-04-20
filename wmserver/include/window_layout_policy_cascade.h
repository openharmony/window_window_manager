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

#ifndef OHOS_ROSEN_WINDOW_LAYOUT_POLICY_CASCADE_H
#define OHOS_ROSEN_WINDOW_LAYOUT_POLICY_CASCADE_H

#include <map>
#include <refbase.h>
#include <set>

#include "window_layout_policy.h"
#include "window_node.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowLayoutPolicyCascade : public WindowLayoutPolicy {
public:
    WindowLayoutPolicyCascade() = delete;
    WindowLayoutPolicyCascade(const Rect& displayRect, const uint64_t& screenId,
        sptr<WindowNode>& belowAppNode, sptr<WindowNode>& appNode, sptr<WindowNode>& aboveAppNode);
    ~WindowLayoutPolicyCascade() = default;
    void Launch() override;
    void Clean() override;
    void Reset() override;
    void Reorder() override;
    void UpdateDisplayInfo() override;
    void AddWindowNode(sptr<WindowNode>& node) override;
    void UpdateWindowNode(sptr<WindowNode>& node, bool isAddWindow = false) override;
    void UpdateLayoutRect(sptr<WindowNode>& node) override;
    void RemoveWindowNode(sptr<WindowNode>& node) override;

private:
    // Rects for split screen
    Rect primaryRect_ = {0, 0, 0, 0};
    Rect secondaryRect_ = {0, 0, 0, 0};
    Rect dividerRect_ = {0, 0, 0, 0};
    Rect primaryLimitRect_ = {0, 0, 0, 0};
    Rect secondaryLimitRect_ = {0, 0, 0, 0};
    // cascade mode rect
    Rect firstCascadeRect_ = {0, 0, 0, 0};

    void InitSplitRects();
    void SetSplitRectByRatio(float ratio);
    void SetSplitRect(const Rect& rect);
    void UpdateSplitLimitRect(const Rect& limitRect, Rect& limitSplitRect);
    void LayoutWindowNode(sptr<WindowNode>& node) override;
    void LayoutWindowTree() override;
    void InitLimitRects();
    void LimitMoveBounds(Rect& rect) const;
    void InitCascadeRect();
    void SetCascadeRect(const sptr<WindowNode>& node);
    void ApplyWindowRectConstraints(const sptr<WindowNode>& node, Rect& winRect) const;

    Rect GetRectByWindowMode(const WindowMode& mode) const;
    Rect GetLimitRect(const WindowMode mode) const;
    Rect GetDisplayRect(const WindowMode mode) const;
    Rect GetCurCascadeRect(const sptr<WindowNode>& node) const;
    Rect StepCascadeRect(Rect rect) const;
};
}
}
#endif // OHOS_ROSEN_WINDOW_LAYOUT_POLICY_CASCADE_H
