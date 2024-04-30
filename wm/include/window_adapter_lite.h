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

#ifndef OHOS_WINDOW_ADAPTER_LITE_H
#define OHOS_WINDOW_ADAPTER_LITE_H

#include <refbase.h>
#include <zidl/window_manager_agent_interface.h>

#include "singleton_delegator.h"
#include "window_property.h"
#include "wm_single_instance.h"
#include "zidl/window_manager_lite_interface.h"

namespace OHOS {
namespace Rosen {
class WMSDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class WindowAdapterLite {
WM_DECLARE_SINGLE_INSTANCE(WindowAdapterLite);
public:
    virtual void GetFocusWindowInfo(FocusChangeInfo& focusInfo);
    virtual WMError RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent);
    virtual WMError UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent);
    virtual WMError CheckWindowId(int32_t windowId, int32_t &pid);
    virtual WMError GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos);
    virtual void ClearWindowAdapter();
    virtual WMError GetWindowModeType(WindowModeType& windowModeType);
    virtual WMError GetMainWindowInfos(int32_t topNum, std::vector<MainWindowInfo>& topNInfo);

private:
    static inline SingletonDelegator<WindowAdapterLite> delegator;
    bool InitSSMProxy();
    void OnUserSwitch();

    std::recursive_mutex mutex_;
    sptr<IWindowManagerLite> windowManagerServiceProxy_ = nullptr;
    sptr<WMSDeathRecipient> wmsDeath_ = nullptr;
    bool isProxyValid_ { false };
    bool isRegisteredUserSwitchListener_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_ADAPTER_H
