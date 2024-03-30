/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "display_group_info.h"
#include "display_info.h"
#include "window_node.h"
#include "wm_common.h"


namespace OHOS {
namespace Rosen {
using DisplayGroupWindowTree = std::map<DisplayId,
    std::map<WindowRootNodeType, std::unique_ptr<std::vector<sptr<WindowNode>>>>>;
enum class DockWindowShowState : uint32_t {
    NOT_SHOWN = 0,
    SHOWN_IN_BOTTOM = 1,
    SHOWN_IN_LEFT = 2,
    SHOWN_IN_RIGHT = 3,
    SHOWN_IN_TOP = 4,
};
class WindowLayoutPolicy : public RefBase {
public:
    WindowLayoutPolicy() = delete;
    WindowLayoutPolicy(DisplayGroupWindowTree& displayGroupWindowTree);
    ~WindowLayoutPolicy() = default;
    virtual void Launch();
    virtual void Reorder();
    virtual void PerformWindowLayout(const sptr<WindowNode>& node, WindowUpdateType type) = 0;
    void ProcessDisplayCreate(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap);
    void ProcessDisplayDestroy(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap);
    void ProcessDisplaySizeChangeOrRotation(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap);
    void ProcessDisplayVprChange(DisplayId displayId);

    virtual void SetSplitDividerWindowRects(std::map<DisplayId, Rect> dividerWindowRects) {};
    virtual Rect GetDividerRect(DisplayId displayId) const;
    virtual bool IsTileRectSatisfiedWithSizeLimits(const sptr<WindowNode>& node);
    bool IsMultiDisplay();
    Rect GetDisplayGroupRect() const;
    void SetSplitRatioPoints(DisplayId displayId, const std::vector<int32_t>& splitRatioPoints);
    void NotifyClientAndAnimation(const sptr<WindowNode>& node, const Rect& winRect, WindowSizeChangeReason reason);
    // methods for setting bottom posY limit for cascade rect on pc
    static void SetCascadeRectBottomPosYLimit(uint32_t floatingBottomPosY);
    static void SetMaxFloatingWindowSize(uint32_t maxSize);
    static void CalcAndSetNodeHotZone(const Rect& winRect, const sptr<WindowNode>& node);
    virtual void GetMaximizeRect(const sptr<WindowNode>& node, Rect& maxRect);

protected:
    /*
     * methods for calculate window rect
     */
    virtual void UpdateLayoutRect(const sptr<WindowNode>& node) = 0;
    void LayoutWindowTree(DisplayId displayId);
    void LayoutWindowNode(const sptr<WindowNode>& node);
    void LayoutWindowNodesByRootType(const std::vector<sptr<WindowNode>>& nodeVec);
    void FixWindowRectWithinDisplay(const sptr<WindowNode>& node) const;

    /*
     * methods for get/update display information or limit information
     */
    AvoidPosType GetAvoidPosType(const Rect& rect, DisplayId displayId) const;
    void UpdateDisplayLimitRect(const sptr<WindowNode>& node, Rect& limitRect);
    bool IsVerticalDisplay(DisplayId displayId) const;
    bool IsFullScreenRecentWindowExist(const std::vector<sptr<WindowNode>>& nodeVec) const;
    void UpdateWindowSizeLimits(const sptr<WindowNode>& node);
    WindowSizeLimits GetSystemSizeLimits(const sptr<WindowNode>& node, const Rect& displayRect, float vpr);

    /*
     * methods for multiDisplay
     */
    void UpdateMultiDisplayFlag();
    void UpdateDisplayGroupRect();
    void UpdateDisplayGroupLimitRect();
    void PostProcessWhenDisplayChange();
    void LimitWindowToBottomRightCorner(const sptr<WindowNode>& node);
    void UpdateRectInDisplayGroupForAllNodes(DisplayId displayId, const Rect& oriDisplayRect,
        const Rect& newDisplayRect);
    void UpdateRectInDisplayGroup(const sptr<WindowNode>& node, const Rect& oriDisplayRect,
        const Rect& newDisplayRect);
    void UpdateDisplayRectAndDisplayGroupInfo(const std::map<DisplayId, Rect>& displayRectMap);

    /*
     * methods for update node latest information, include:
     * 1. notify client and animation
     * 2. update hot zone
     * 3. update surface bounds
     */
    void NotifyAnimationSizeChangeIfNeeded();
    static Rect CalcEntireWindowHotZone(const sptr<WindowNode>& node, const Rect& winRect, uint32_t hotZone,
        float vpr, TransformHelper::Vector2 hotZoneScale);
    void UpdateSurfaceBounds(const sptr<WindowNode>& node, const Rect& winRect, const Rect& preRect);

    void GetStoragedAspectRatio(const sptr<WindowNode>& node);
    bool IsNeedAnimationSync(WindowType type);

    Rect displayGroupRect_{};
    Rect displayGroupLimitRect_{};
    bool isMultiDisplay_ = false;
    mutable std::map<DisplayId, Rect> limitRectMap_;
    DisplayGroupWindowTree& displayGroupWindowTree_;
    std::map<DisplayId, Rect> restoringDividerWindowRects_;
    mutable std::map<DisplayId, std::vector<int32_t>> splitRatioPointsMap_;
    // bottom posY limit for cascade rect on pc
    static uint32_t floatingBottomPosY_;
    // max size of floating window in config
    static uint32_t maxFloatingWindowSize_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H
