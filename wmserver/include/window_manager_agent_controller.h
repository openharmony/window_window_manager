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

#ifndef OHOS_ROSEN_WINDOW_MANAGER_AGENT_CONTROLLER_H
#define OHOS_ROSEN_WINDOW_MANAGER_AGENT_CONTROLLER_H

#include <mutex>
#include "client_agent_container.h"
#include "wm_single_instance.h"
#include "zidl/window_manager_agent_interface.h"

namespace OHOS {
namespace Rosen {
class WindowManagerAgentController {
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowManagerAgentController)
public:
    void RegisterWindowManagerAgent(const sptr<IWindowManagerAgent>& windowManagerAgent,
        WindowManagerAgentType type);
    void UnregisterWindowManagerAgent(const sptr<IWindowManagerAgent>& windowManagerAgent,
        WindowManagerAgentType type);

    void UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken, WindowType windowType,
        DisplayId displayId, bool focused);
    void UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool focused);
    void UpdateSystemBarRegionTints(DisplayId displayId, const SystemBarRegionTints& tints);
    void UpdateWindowStatus(const sptr<AccessibilityWindowInfo>& windowInfo, WindowUpdateType type);
    void UpdateWindowVisibilityInfo(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos);

private:
    WindowManagerAgentController() {}
    virtual ~WindowManagerAgentController() = default;

    ClientAgentContainer<IWindowManagerAgent, WindowManagerAgentType> wmAgentContainer_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_MANAGER_AGENT_CONTROLLER_H
