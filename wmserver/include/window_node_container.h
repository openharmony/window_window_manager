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

#ifndef OHOS_ROSEN_WINDOW_NODE_CONTAINER_H
#define OHOS_ROSEN_WINDOW_NODE_CONTAINER_H

#include <ui/rs_display_node.h>
#include "avoid_area_controller.h"
#include "window_layout_policy.h"
#include "window_manager.h"
#include "window_node.h"
#include "window_zorder_policy.h"
#include "wm_common.h"
#include "wm_common_inner.h"
#include "window_pair.h"

namespace OHOS {
namespace Rosen {
using WindowNodeOperationFunc = std::function<bool(sptr<WindowNode>)>; // return true indicates to stop traverse
using SysBarNodeMap = std::unordered_map<WindowType, sptr<WindowNode>>;
using SysBarTintMap = std::unordered_map<WindowType, SystemBarRegionTint>;
class WindowNodeContainer : public RefBase {
public:
    WindowNodeContainer(DisplayId displayId, uint32_t width, uint32_t height, bool isMinimizedByOther);
    ~WindowNodeContainer();
    WMError AddWindowNode(sptr<WindowNode>& node, sptr<WindowNode>& parentNode);
    WMError RemoveWindowNode(sptr<WindowNode>& node);
    WMError UpdateWindowNode(sptr<WindowNode>& node, WindowUpdateReason reason);
    WMError DestroyWindowNode(sptr<WindowNode>& node, std::vector<uint32_t>& windowIds);
    const std::vector<uint32_t>& Destroy();
    void AssignZOrder();
    WMError SetFocusWindow(uint32_t windowId);
    uint32_t GetFocusWindow() const;
    WMError SetActiveWindow(uint32_t windowId, bool byRemoved);
    uint32_t GetActiveWindow() const;
    void SetDisplayBrightness(float brightness);
    float GetDisplayBrightness() const;
    void SetBrightnessWindow(uint32_t windowId);
    uint32_t GetBrightnessWindow() const;
    uint32_t ToOverrideBrightness(float brightness);
    void UpdateBrightness(uint32_t id, bool byRemoved);
    void HandleKeepScreenOn(const sptr<WindowNode>& node, bool requireLock);
    std::vector<Rect> GetAvoidAreaByType(AvoidAreaType avoidAreaType, DisplayId displayId);
    WMError MinimizeStructuredAppWindowsExceptSelf(const sptr<WindowNode>& node);
    void TraverseContainer(std::vector<sptr<WindowNode>>& windowNodes) const;
    uint64_t GetScreenId(DisplayId displayId) const;
    Rect GetDisplayRect(DisplayId displayId) const;
    std::unordered_map<WindowType, SystemBarProperty> GetExpectImmersiveProperty() const;
    void NotifyAccessibilityWindowInfo(const sptr<WindowNode>& windowId, WindowUpdateType type) const;
    int GetWindowCountByType(WindowType windowType);

    void OnAvoidAreaChange(const std::vector<Rect>& avoidAreas, DisplayId displayId);
    bool isVerticalDisplay(DisplayId displayId) const;
    WMError RaiseZOrderForAppWindow(sptr<WindowNode>& node, sptr<WindowNode>& parentNode);
    sptr<WindowNode> GetNextFocusableWindow(uint32_t windowId) const;
    sptr<WindowNode> GetNextActiveWindow(uint32_t windowId) const;
    void MinimizeAllAppWindows(DisplayId displayId);
    void MinimizeOldestAppWindow();
    void ProcessWindowStateChange(WindowState state, WindowStateChangeReason reason);
    void NotifySystemBarTints(std::vector<DisplayId> displayIdVec);
    void NotifySystemBarDismiss(sptr<WindowNode>& node);
    WMError MinimizeAppNodeExceptOptions(bool fromUser, const std::vector<uint32_t> &exceptionalIds = {},
                                         const std::vector<WindowMode> &exceptionalModes = {});
    WMError SetWindowMode(sptr<WindowNode>& node, WindowMode dstMode);
    WMError SwitchLayoutPolicy(WindowLayoutMode mode, DisplayId displayId, bool reorder = false);
    void RaiseSplitRelatedWindowToTop(sptr<WindowNode>& node);
    void MoveWindowNodes(DisplayId displayId, std::vector<uint32_t>& windowIds);
    float GetVirtualPixelRatio(DisplayId displayId) const;
    void TraverseWindowTree(const WindowNodeOperationFunc& func, bool isFromTopToBottom = true) const;
    void UpdateSizeChangeReason(sptr<WindowNode>& node, WindowSizeChangeReason reason);
    void GetWindowList(std::vector<sptr<WindowInfo>>& windowList) const;
    void DropShowWhenLockedWindowIfNeeded(const sptr<WindowNode>& node);
    void ProcessDisplayCreate(DisplayId displayId, const Rect& displayRect);
    void ProcessDisplayDestroy(DisplayId displayId, std::vector<uint32_t>& windowIds);
    void ProcessDisplayChange(DisplayId displayId, const Rect& displayRect);

private:
    void TraverseWindowNode(sptr<WindowNode>& root, std::vector<sptr<WindowNode>>& windowNodes) const;
    sptr<WindowNode> FindRoot(WindowType type) const;
    std::vector<sptr<WindowNode>>* FindNodeVectorOfRoot(DisplayId displayId, WindowRootNodeType type);
    sptr<WindowNode> FindWindowNodeById(uint32_t id) const;
    void UpdateFocusStatus(uint32_t id, bool focused) const;
    void UpdateActiveStatus(uint32_t id, bool isActive) const;
    void UpdateWindowTree(sptr<WindowNode>& node);
    bool UpdateRSTree(sptr<WindowNode>& node, bool isAdd);

    void NotifyIfSystemBarTintChanged(DisplayId displayId);
    void NotifyIfSystemBarRegionChanged(DisplayId displayId);
    void TraverseAndUpdateWindowState(WindowState state, int32_t topPriority);
    void UpdateWindowState(sptr<WindowNode> node, int32_t topPriority, WindowState state);
    void HandleKeepScreenOn(const sptr<WindowNode>& node, WindowState state);
    bool IsTopWindow(uint32_t windowId, sptr<WindowNode>& rootNode) const;
    sptr<WindowNode> FindDividerNode() const;
    void RaiseWindowToTop(uint32_t windowId, std::vector<sptr<WindowNode>>& windowNodes);
    void MinimizeWindowFromAbility(const sptr<WindowNode>& node, bool fromUser);
    void ResetLayoutPolicy();
    bool IsAboveSystemBarNode(sptr<WindowNode> node) const;
    bool IsFullImmersiveNode(sptr<WindowNode> node) const;
    bool IsSplitImmersiveNode(sptr<WindowNode> node) const;
    bool TraverseFromTopToBottom(sptr<WindowNode> node, const WindowNodeOperationFunc& func) const;
    bool TraverseFromBottomToTop(sptr<WindowNode> node, const WindowNodeOperationFunc& func) const;
    void RcoveryScreenDefaultOrientationIfNeed(DisplayId displayId);
    // cannot determine in case of a window covered by union of several windows or with transparent value
    void UpdateWindowVisibilityInfos(std::vector<sptr<WindowVisibilityInfo>>& infos);
    void RaiseOrderedWindowToTop(std::vector<sptr<WindowNode>>& orderedNodes,
        std::vector<sptr<WindowNode>>& windowNodes);
    static bool ReadIsWindowAnimationEnabledProperty();
    void DumpScreenWindowTree();
    void RaiseInputMethodWindowPriorityIfNeeded(const sptr<WindowNode>& node) const;
    void RaiseShowWhenLockedWindowIfNeeded(const sptr<WindowNode>& node);
    void ReZOrderShowWhenLockedWindows(const sptr<WindowNode>& node, bool up);

    void InitSysBarMapForDisplay(DisplayId displayId);
    void InitWindowNodeMapForDisplay(DisplayId displayId);
    WMError AddWindowNodeOnWindowTree(sptr<WindowNode>& node, sptr<WindowNode>& parentNode);
    void RemoveWindowNodeFromWindowTree(sptr<WindowNode>& node);
    void AddWindowNodeInRootNodeVector(sptr<WindowNode>& node, WindowRootNodeType rootType);
    void RemoveWindowNodeFromRootNodeVector(sptr<WindowNode>& node, WindowRootNodeType rootType);
    void UpdateWindowNodeMaps();

    float displayBrightness_ = UNDEFINED_BRIGHTNESS;
    uint32_t brightnessWindow_ = INVALID_WINDOW_ID;
    uint32_t zOrder_ { 0 };
    uint32_t focusedWindow_ { INVALID_WINDOW_ID };
    uint32_t activeWindow_ = INVALID_WINDOW_ID;

    sptr<AvoidAreaController> avoidController_;
    sptr<WindowZorderPolicy> zorderPolicy_ = new WindowZorderPolicy();
    std::unordered_map<WindowLayoutMode, sptr<WindowLayoutPolicy>> layoutPolicys_;
    WindowLayoutMode layoutMode_ = WindowLayoutMode::CASCADE;
    sptr<WindowLayoutPolicy> layoutPolicy_;

    std::vector<Rect> currentCoveredArea_;
    std::map<DisplayId, SysBarNodeMap> sysBarNodeMaps_;
    std::map<DisplayId, SysBarTintMap> sysBarTintMaps_;

    sptr<WindowPair> windowPair_;
    std::vector<uint32_t> removedIds_;
    sptr<WindowNode> belowAppWindowNode_ = new WindowNode();
    sptr<WindowNode> appWindowNode_ = new WindowNode();
    sptr<WindowNode> aboveAppWindowNode_ = new WindowNode();
    std::map<DisplayId, Rect> displayRectMap_;
    WindowNodeMaps windowNodeMaps_;
    bool isMinimizedByOther_ = true;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_NODE_CONTAINER_H
