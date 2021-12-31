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
        TRANS_ID_SEND_ABILITY_TOKEN,
    };
    virtual WMError CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId)  = 0;
    virtual WMError AddWindow(sptr<WindowProperty>& property) = 0;
    virtual WMError RemoveWindow(uint32_t windowId) = 0;
    virtual WMError DestroyWindow(uint32_t windowId) = 0;
    virtual WMError MoveTo(uint32_t windowId, int32_t x, int32_t y) = 0;
    virtual WMError Resize(uint32_t windowId, uint32_t width, uint32_t height) = 0;
    virtual WMError RequestFocus(uint32_t windowId) = 0;
    virtual WMError SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId) = 0;
};
}
}
#endif // OHOS_WINDOW_MANAGER_INTERFACE_H
