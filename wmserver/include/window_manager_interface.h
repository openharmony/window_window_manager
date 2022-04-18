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

#ifndef OHOS_WINDOW_MANAGER_INTERFACE_H
#define OHOS_WINDOW_MANAGER_INTERFACE_H

#include <iremote_broker.h>
#include <ui/rs_surface_node.h>
#include "window_property.h"
#include "window_interface.h"
#include "zidl/window_manager_agent_interface.h"

namespace OHOS {
namespace Rosen {
class IWindowManager : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IWindowManager");

    enum {
        TRANS_ID_CREATE_WINDOW,
        TRANS_ID_ADD_WINDOW,
        TRANS_ID_REMOVE_WINDOW,
        TRANS_ID_DESTROY_WINDOW,
        TRANS_ID_REQUEST_FOCUS,
        TRANS_ID_SEND_ABILITY_TOKEN,
        TRANS_ID_REGISTER_FOCUS_CHANGED_LISTENER,
        TRANS_ID_UNREGISTER_FOCUS_CHANGED_LISTENER,
        TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT,
        TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT,
        TRANS_ID_GET_AVOID_AREA,
        TRANS_ID_GET_TOP_WINDOW_ID,
        TRANS_ID_PROCESS_POINT_DOWN,
        TRANS_ID_PROCESS_POINT_UP,
        TRANS_ID_MINIMIZE_ALL_APP_WINDOWS,
        TRANS_ID_SET_BACKGROUND_BLUR,
        TRANS_ID_SET_APLPHA,
        TRANS_ID_UPDATE_LAYOUT_MODE,
        TRANS_ID_MAXMIZE_WINDOW,
        TRANS_ID_UPDATE_PROPERTY,
    };
    virtual WMError CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId)  = 0;
    virtual WMError AddWindow(sptr<WindowProperty>& property) = 0;
    virtual WMError RemoveWindow(uint32_t windowId) = 0;
    virtual WMError DestroyWindow(uint32_t windowId, bool onlySelf = false) = 0;
    virtual WMError RequestFocus(uint32_t windowId) = 0;
    virtual WMError SetWindowBackgroundBlur(uint32_t windowId, WindowBlurLevel level) = 0;
    virtual WMError SetAlpha(uint32_t windowId, float alpha) = 0;
    virtual WMError SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId) = 0;
    virtual std::vector<Rect> GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type) = 0;
    virtual WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId) = 0;
    virtual void ProcessPointDown(uint32_t windowId, bool isStartDrag) = 0;
    virtual void ProcessPointUp(uint32_t windowId) = 0;
    virtual void MinimizeAllAppWindows(DisplayId displayId) = 0;
    virtual WMError MaxmizeWindow(uint32_t windowId) = 0;
    virtual WMError SetWindowLayoutMode(DisplayId displayId, WindowLayoutMode mode) = 0;
    virtual WMError UpdateProperty(sptr<WindowProperty>& windowProperty, PropertyChangeAction action) = 0;
    virtual void RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) = 0;
    virtual void UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) = 0;
};
}
}
#endif // OHOS_WINDOW_MANAGER_INTERFACE_H
