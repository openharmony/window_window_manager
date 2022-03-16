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

#ifndef OHOS_ROSEN_WINDOW_CONTROLLER_H
#define OHOS_ROSEN_WINDOW_CONTROLLER_H

#include <refbase.h>
#include "input_window_monitor.h"
#include "zidl/window_manager_agent_interface.h"
#include "window_root.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowController : public RefBase {
public:
    WindowController(sptr<WindowRoot>& root, sptr<InputWindowMonitor> inputWindowMonitor) : windowRoot_(root),
        inputWindowMonitor_(inputWindowMonitor) {}
    ~WindowController() = default;

    WMError CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId);
    WMError AddWindowNode(sptr<WindowProperty>& property);
    WMError RemoveWindowNode(uint32_t windowId);
    WMError DestroyWindow(uint32_t windowId, bool onlySelf);
    WMError ResizeRect(uint32_t windowId, const Rect& rect, WindowSizeChangeReason reason);
    WMError RequestFocus(uint32_t windowId);
    WMError SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId);
    WMError SetWindowMode(uint32_t windowId, WindowMode dstMode);
    WMError SetWindowBackgroundBlur(uint32_t windowId, WindowBlurLevel level);
    WMError SetAlpha(uint32_t windowId, float alpha);
    WMError SetWindowType(uint32_t windowId, WindowType type);
    WMError SetWindowFlags(uint32_t windowId, uint32_t flags);
    WMError SetSystemBarProperty(uint32_t windowId, WindowType type, const SystemBarProperty& property);
    std::vector<Rect> GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType);
    WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId);
    void NotifyDisplayStateChange(DisplayId id, DisplayStateChangeType type);
    WMError ProcessPointDown(uint32_t windowId);
    WMError ProcessPointUp(uint32_t windowId);
    void MinimizeAllAppWindows(DisplayId displayId);
    WMError MaxmizeWindow(uint32_t windowId);
    WMError SetWindowLayoutMode(DisplayId displayId, WindowLayoutMode mode);
    void NotifySystemBarTints();

private:
    uint32_t GenWindowId();
    void FlushWindowInfo(uint32_t windowId);
    void FlushWindowInfoWithDisplayId(DisplayId displayId);
    void UpdateWindowAnimation(const sptr<WindowNode>& node);
    void ProcessDisplayChange(DisplayId displayId, DisplayStateChangeType type);
    void StopBootAnimationIfNeed(WindowType type) const;

    sptr<WindowRoot> windowRoot_;
    sptr<InputWindowMonitor> inputWindowMonitor_;
    std::atomic<uint32_t> windowId_ { INVALID_WINDOW_ID };
    // Remove 'sysBarWinId_' after SystemUI resize 'systembar'
    std::unordered_map<WindowType, uint32_t> sysBarWinId_ {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     INVALID_WINDOW_ID },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, INVALID_WINDOW_ID },
    };
    constexpr static float SYSTEM_BAR_HEIGHT_RATIO = 0.08;
};
}
}
#endif // OHOS_ROSEN_WINDOW_CONTROLLER_H
