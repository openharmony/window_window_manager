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

#ifndef OHOS_WINDOW_MANAGER_PROXY_H
#define OHOS_WINDOW_MANAGER_PROXY_H

#include <iremote_proxy.h>
#include "window_manager_interface.h"

namespace OHOS {
namespace Rosen {
class WindowManagerProxy : public IRemoteProxy<IWindowManager> {
public:
    explicit WindowManagerProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IWindowManager>(impl) {};

    ~WindowManagerProxy() {};

    WMError CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        uint32_t& windowId, sptr<IRemoteObject> token) override;
    WMError AddWindow(sptr<WindowProperty>& property) override;
    WMError RemoveWindow(uint32_t windowId) override;
    WMError NotifyWindowTransition(sptr<WindowTransitionInfo>& from, sptr<WindowTransitionInfo>& to,
        bool isFromClient = false) override;
    WMError DestroyWindow(uint32_t windowId, bool onlySelf = false) override;
    WMError RequestFocus(uint32_t windowId) override;
    WMError SetWindowBackgroundBlur(uint32_t windowId, WindowBlurLevel level) override;
    WMError SetAlpha(uint32_t windowId, float alpha) override;
    std::vector<Rect> GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type) override;
    WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId) override;
    void ProcessPointDown(uint32_t windowId, bool isStartDrag) override;
    void ProcessPointUp(uint32_t windowId) override;
    void MinimizeAllAppWindows(DisplayId displayId) override;
    WMError ToggleShownStateForAllAppWindows() override;
    WMError MaxmizeWindow(uint32_t windowId) override;
    WMError SetWindowLayoutMode(WindowLayoutMode mode) override;
    WMError UpdateProperty(sptr<WindowProperty>& windowProperty,
        PropertyChangeAction action, uint64_t dirtyState) override;

    void RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;
    void UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;
    WMError SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller) override;

    WMError GetAccessibilityWindowInfo(sptr<AccessibilityWindowInfo>& windowInfo) override;
    WMError GetSystemConfig(SystemConfig& systemConfig) override;
    WMError GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones) override;
    void MinimizeWindowsByLauncher(std::vector<uint32_t> windowIds, bool isAnimated,
        sptr<RSIWindowAnimationFinishedCallback>& finishCallback) override;
private:
    static inline BrokerDelegator<WindowManagerProxy> delegator_;
};
}
}
#endif // OHOS_WINDOW_MANAGER_PROXY_H
