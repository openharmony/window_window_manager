/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_WINDOW_MANAGER_LITE_INTERFACE_H
#define OHOS_WINDOW_MANAGER_LITE_INTERFACE_H

#include <iremote_broker.h>

#include "interfaces/include/ws_common.h"
#include "window_property.h"
#include "window_transition_info.h"
#include "zidl/window_manager_agent_interface.h"

namespace OHOS {
namespace MMI {
class KeyEvent;
}
namespace Rosen {

class IWindowManagerLite : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IWindowManagerLite");
    // do not need enum
    virtual WMError RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) = 0;
    virtual WMError UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) = 0;
    virtual WMError GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos) { return WMError::WM_OK; };
    virtual void GetFocusWindowInfo(FocusChangeInfo& focusInfo) = 0;
    virtual WMError CheckWindowId(int32_t windowId, int32_t &pid) = 0;
    virtual WMError GetWindowModeType(WindowModeType& windowModeType) { return WMError::WM_OK; }
    virtual WMError GetMainWindowInfos(int32_t topNum, std::vector<MainWindowInfo>& topNInfo) = 0;
};
}
}
#endif // OHOS_WINDOW_MANAGER_LITE_INTERFACE_H
