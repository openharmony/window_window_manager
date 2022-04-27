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
#ifndef OHOS_ROSEN_WINDOW_ROOT_H
#define OHOS_ROSEN_WINDOW_ROOT_H

#include <refbase.h>
#include <iremote_object.h>

#include "agent_death_recipient.h"
#include "display_manager_service_inner.h"
#include "window_node_container.h"
#include "zidl/window_manager_agent_interface.h"

namespace OHOS {
namespace Rosen {
enum class Event : uint32_t {
    REMOTE_DIED,
};

class WindowRoot : public RefBase {
using Callback = std::function<void (Event event, uint32_t windowId)>;

public:
    WindowRoot(std::recursive_mutex& mutex, Callback callback) : mutex_(mutex), callback_(callback) {}
    ~WindowRoot() = default;

    sptr<WindowNodeContainer> GetOrCreateWindowNodeContainer(DisplayId displayId);
    sptr<WindowNodeContainer> CreateWindowNodeContainer(DisplayId displayId);
    sptr<WindowNode> GetWindowNode(uint32_t windowId) const;

    WMError SaveWindow(const sptr<WindowNode>& node);
    WMError SaveWindowWithWindowToken(sptr<WindowNode> node);
    WMError SaveDesWindowNode(sptr<WindowNode>& node);
    WMError ShowInTransition(sptr<WindowNode>& node);
    sptr<WindowNode> FindWindowNodeWithToken(const sptr<IRemoteObject>& token) const;
    WMError AddWindowNode(uint32_t parentId, sptr<WindowNode>& node);
    WMError RemoveWindowNode(uint32_t windowId);
    WMError DestroyWindow(uint32_t windowId, bool onlySelf);
    WMError UpdateWindowNode(uint32_t windowId, WindowUpdateReason reason);
    bool isVerticalDisplay(sptr<WindowNode>& node) const;

    WMError RequestFocus(uint32_t windowId);
    WMError RequestActiveWindow(uint32_t windowId);
    WMError MinimizeStructuredAppWindowsExceptSelf(sptr<WindowNode>& node);
    std::vector<Rect> GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType);
    WMError SetWindowMode(sptr<WindowNode>& node, WindowMode dstMode);
    std::shared_ptr<RSSurfaceNode> GetSurfaceNodeByAbilityToken(const sptr<IRemoteObject>& abilityToken) const;
    WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId);
    void MinimizeAllAppWindows(DisplayId displayId);
    WMError MaxmizeWindow(uint32_t windowId);
    WMError SetWindowLayoutMode(DisplayId displayId, WindowLayoutMode mode);

    void ProcessWindowStateChange(WindowState state, WindowStateChangeReason reason);
    void ProcessDisplayChange(sptr<DisplayInfo> abstractDisplay);
    void ProcessDisplayDestroy(DisplayId displayId);
    void ProcessDisplayCreate(DisplayId displayId);
    void NotifySystemBarTints();
    WMError RaiseZOrderForAppWindow(sptr<WindowNode>& node);
    void FocusFaultDetection() const;
    float GetVirtualPixelRatio(DisplayId displayId) const;
    WMError UpdateSizeChangeReason(uint32_t windowId, WindowSizeChangeReason reason);
    void SetBrightness(uint32_t windowId, float brightness);
    void HandleKeepScreenOn(uint32_t windowId, bool requireLock);
    void UpdateFocusableProperty(uint32_t windowId);
    WMError GetAccessibilityWindowInfo(sptr<AccessibilityWindowInfo>& windowInfo);
    void SetMaxAppWindowNumber(int windowNum);
    void SetMinimizedByOtherWindow(bool isMinimizedByOtherWindow);
    WMError GetModeChangeHotZones(DisplayId displayId,
        ModeChangeHotZones& hotZones, const ModeChangeHotZonesConfig& config);

private:
    void OnRemoteDied(const sptr<IRemoteObject>& remoteObject);
    WMError DestroyWindowInner(sptr<WindowNode>& node);
    void UpdateFocusWindowWithWindowRemoved(const sptr<WindowNode>& node,
        const sptr<WindowNodeContainer>& container) const;
    void UpdateActiveWindowWithWindowRemoved(const sptr<WindowNode>& node,
        const sptr<WindowNodeContainer>& container) const;
    void UpdateBrightnessWithWindowRemoved(uint32_t windowId, const sptr<WindowNodeContainer>& container) const;
    std::string GenAllWindowsLogInfo() const;
    bool CheckDisplayInfo(const sptr<DisplayInfo>& display);
    void NotifyKeyboardSizeChangeInfo(const sptr<WindowNode>& node,
        const sptr<WindowNodeContainer>& container, Rect rect);
    ScreenId GetScreenGroupId(DisplayId displayId, bool& isRecordedDisplay);
    void ProcessExpandDisplayCreate(DisplayId displayId, ScreenId screenGroupId);

    std::recursive_mutex& mutex_;
    std::map<uint32_t, sptr<WindowNode>> windowNodeMap_;
    std::map<sptr<IRemoteObject>, uint32_t> windowIdMap_;
    std::map<ScreenId, sptr<WindowNodeContainer>> windowNodeContainerMap_;
    std::map<ScreenId, std::vector<DisplayId>> displayIdMap_;
    bool needCheckFocusWindow = false;
    bool isMinimizedByOtherWindow_ = true;

    std::map<WindowManagerAgentType, std::vector<sptr<IWindowManagerAgent>>> windowManagerAgents_;

    sptr<AgentDeathRecipient> windowDeath_ = new AgentDeathRecipient(std::bind(&WindowRoot::OnRemoteDied,
        this, std::placeholders::_1));
    Callback callback_;
    int maxAppWindowNumber_ = 100;
};
}
}
#endif // OHOS_ROSEN_WINDOW_ROOT_H
