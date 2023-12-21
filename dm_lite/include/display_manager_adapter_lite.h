/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at,
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software,
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FOUNDATION_DM_DISPLAY_MANAGER_ADAPTER_LITE_H
#define FOUNDATION_DM_DISPLAY_MANAGER_ADAPTER_LITE_H

#include <map>
#include <mutex>

#include "display_lite.h"
#include "dm_common.h"
#include "singleton_delegator.h"
#include "zidl/screen_session_manager_lite_interface.h"

namespace OHOS::Rosen {
class BaseAdapterLite {
public:
    virtual ~BaseAdapterLite();
    virtual DMError RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type);
    virtual DMError UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type);
    virtual void Clear();
protected:
    bool InitDMSProxy();
    std::recursive_mutex mutex_;
    sptr<IScreenSessionManagerLite> displayManagerServiceProxy_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> dmsDeath_ = nullptr;
    bool isProxyValid_ { false };
};

class DMSDeathRecipientLite : public IRemoteObject::DeathRecipient {
public:
    explicit DMSDeathRecipientLite(BaseAdapterLite& adapter);
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
private:
    BaseAdapterLite& adapter_;
};

class DisplayManagerAdapterLite : public BaseAdapterLite {
WM_DECLARE_SINGLE_INSTANCE(DisplayManagerAdapterLite);
public:
private:
    static inline SingletonDelegator<DisplayManagerAdapterLite> delegator;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DM_DISPLAY_MANAGER_ADAPTER_LITE_H
