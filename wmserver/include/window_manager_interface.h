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
        TRANS_ID_MOVE,
        TRANS_ID_RESIZE,
        TRANS_ID_REQUEST_FOCUS,
        TRANS_ID_UPDATE_TYPE,
        TRANS_ID_UPDATE_MODE,
        TRANS_ID_UPDATE_FLAGS,
        TRANS_ID_UPDATE_SYSTEM_BAR_PROPERTY,
        TRANS_ID_SEND_ABILITY_TOKEN,
        TRANS_ID_REGISTER_FOCUS_CHANGED_LISTENER,
        TRANS_ID_UNREGISTER_FOCUS_CHANGED_LISTENER,
        TRANS_ID_MINIMIZE_ALL_APP_WINDOW,
        TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT,
        TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT,
        TRANS_ID_GET_AVOID_AREA,
        TRANS_ID_GET_TOP_WINDOW_ID,
        TRANS_ID_PROCESS_WINDOW_TOUCHED_EVENT,
        TRANS_ID_MINIMIZE_ALL_APP_WINDOWS,
    };
    virtual WMError CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId)  = 0;
    virtual WMError AddWindow(sptr<WindowProperty>& property) = 0;
    virtual WMError RemoveWindow(uint32_t windowId) = 0;
    virtual WMError DestroyWindow(uint32_t windowId) = 0;
    virtual WMError MoveTo(uint32_t windowId, int32_t x, int32_t y) = 0;
    virtual WMError Resize(uint32_t windowId, uint32_t width, uint32_t height) = 0;
    virtual WMError RequestFocus(uint32_t windowId) = 0;
    virtual WMError SetWindowMode(uint32_t windowId, WindowMode mode) = 0;
    virtual WMError SetWindowType(uint32_t windowId, WindowType type) = 0;
    virtual WMError SetWindowFlags(uint32_t windowId, uint32_t flags) = 0;
    virtual WMError SetSystemBarProperty(uint32_t windowId, WindowType type, const SystemBarProperty& prop) = 0;
    virtual WMError SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId) = 0;
    virtual std::vector<Rect> GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type) = 0;
    virtual WMError MinimizeAllAppNodeAbility(uint32_t windowId) = 0;
    virtual WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId) = 0;
    virtual void ProcessWindowTouchedEvent(uint32_t windowId) = 0;
    virtual void MinimizeAllAppWindows(DisplayId displayId) = 0;

    virtual void RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) = 0;
    virtual void UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) = 0;
};
}
}
#endif // OHOS_WINDOW_MANAGER_INTERFACE_H
