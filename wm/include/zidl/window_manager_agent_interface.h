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

namespace OHOS {
namespace Rosen {
class IWindowManagerAgent : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IWindowManagerAgent");

    enum {
        TRANS_ID_UPDATE_FOCUS_STATUS = 1,
    };

    virtual void UpdateFocusStatus(uint32_t windowId, const sptr<IRemoteObject>& abilityToken, WindowType windowType,
        int32_t displayId, bool focused) = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_MANAGER_AGENT_INTERFACE_H
