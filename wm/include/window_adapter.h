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

#ifndef OHOS_WINDOW_ADAPTER_H
#define OHOS_WINDOW_ADAPTER_H

#include <refbase.h>
#include <zidl/window_manager_agent_interface.h>

#include "window.h"
#include "window_interface.h"
#include "wm_single_instance.h"
#include "singleton_delegator.h"
#include "window_property.h"
#include "window_manager_interface.h"
namespace OHOS {
namespace Rosen {
class WMSDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class WindowAdapter {
WM_DECLARE_SINGLE_INSTANCE(WindowAdapter);
public:
    virtual WMError CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& windowProperty,
        std::shared_ptr<RSSurfaceNode> surfaceNode, uint32_t& windowId);
    virtual WMError AddWindow(sptr<WindowProperty>& windowProperty);
    virtual WMError RemoveWindow(uint32_t windowId);
    virtual WMError DestroyWindow(uint32_t windowId);
    virtual WMError SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId);
    virtual WMError MoveTo(uint32_t windowId, int32_t x, int32_t y);
    virtual WMError Resize(uint32_t windowId, uint32_t width, uint32_t height);
    virtual WMError Drag(uint32_t windowId, const Rect& rect);
    virtual WMError RequestFocus(uint32_t windowId);
    virtual WMError SetWindowFlags(uint32_t windowId, uint32_t flags);
    virtual WMError SetSystemBarProperty(uint32_t windowId, WindowType type, const SystemBarProperty& property);
    virtual WMError GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type, std::vector<Rect>& avoidRect);
    virtual WMError SetWindowMode(uint32_t windowId, WindowMode mode);
    virtual WMError SetWindowBackgroundBlur(uint32_t windowId, WindowBlurLevel level);
    virtual WMError SetAlpha(uint32_t windowId, float alpha);
    virtual WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId);
    virtual void ProcessWindowTouchedEvent(uint32_t windowId);
    virtual void MinimizeAllAppWindows(DisplayId displayId);
    virtual WMError SetWindowLayoutMode(DisplayId displayId, WindowLayoutMode mode);

    // colorspace, gamut
    virtual bool IsSupportWideGamut(uint32_t windowId);
    virtual void SetColorSpace(uint32_t windowId, ColorSpace colorSpace);
    virtual ColorSpace GetColorSpace(uint32_t windowId);

    virtual void RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent);
    virtual void UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent);

    virtual void ClearWindowAdapter();
private:
    static inline SingletonDelegator<WindowAdapter> delegator;
    bool InitWMSProxyLocked();

    std::recursive_mutex mutex_;
    sptr<IWindowManager> windowManagerServiceProxy_ = nullptr;
    sptr<WMSDeathRecipient> wmsDeath_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_ADAPTER_H
