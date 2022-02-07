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

#ifndef OHOS_WINDOW_MANAGER_AGENT_INTERFACE_H
#define OHOS_WINDOW_MANAGER_AGENT_INTERFACE_H

#include <iremote_broker.h>
#include "wm_common.h"
#include "window_manager.h"

namespace OHOS {
namespace Rosen {
enum class WindowManagerAgentType : uint32_t {
    WINDOW_MANAGER_AGENT_TYPE_FOCUS,
    WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR,
    WINDOW_MANAGER_AGENT_TYPE_WINDOW_UPDATE,
};

class IWindowManagerAgent : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IWindowManagerAgent");

    enum {
        TRANS_ID_UPDATE_FOCUS_STATUS = 1,
        TRANS_ID_UPDATE_SYSTEM_BAR_PROPS,
        TRANS_ID_UPDATE_WINDOW_STATUS,
    };

    virtual void UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken, WindowType windowType,
        DisplayId displayId, bool focused) = 0;
    virtual void UpdateSystemBarRegionTints(DisplayId displayId, const SystemBarRegionTints& tints) = 0;
    virtual void UpdateWindowStatus(const sptr<WindowInfo>& windowInfo, WindowUpdateType type) = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_MANAGER_AGENT_INTERFACE_H
