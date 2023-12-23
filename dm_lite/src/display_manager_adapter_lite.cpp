// /*
//  * Copyright (c) 2023 Huawei Device Co., Ltd.
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at,
//  *
//  *     http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software,
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */

#include "display_manager_adapter_lite.h"

#include <iremote_broker.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "display_manager_lite.h"
#include "dm_common.h"
#include "window_manager_hilog.h"
#include "session_manager_lite.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerAdapterLite"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerAdapterLite)


#define INIT_PROXY_CHECK_RETURN(ret) \
    do { \
        if (!InitDMSProxy()) { \
            WLOGFE("InitDMSProxy fail"); \
            return ret; \
        } \
    } while (false)

DMError BaseAdapterLite::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->RegisterDisplayManagerAgent(displayManagerAgent, type);
}

DMError BaseAdapterLite::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->UnregisterDisplayManagerAgent(displayManagerAgent, type);
}

bool BaseAdapterLite::InitDMSProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isProxyValid_) {
        displayManagerServiceProxy_ = SessionManagerLite::GetInstance().GetScreenSessionManagerLiteProxy();
        if ((!displayManagerServiceProxy_) || (!displayManagerServiceProxy_->AsObject())) {
            WLOGFE("Failed to get system scene session manager services");
            return false;
        }
        dmsDeath_ = new (std::nothrow) DMSDeathRecipientLite(*this);
        if (!dmsDeath_) {
            WLOGFE("Failed to create death Recipient ptr DMSDeathRecipientLite");
            return false;
        }
        sptr<IRemoteObject> remoteObject = displayManagerServiceProxy_->AsObject();
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(dmsDeath_)) {
            WLOGFE("Failed to add death recipient.");
            return false;
        }
        isProxyValid_ = true;
    }
    return true;
}

DMSDeathRecipientLite::DMSDeathRecipientLite(BaseAdapterLite& adapter) : adapter_(adapter)
{
}

void DMSDeathRecipientLite::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("wptrDeath is nullptr");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("object is nullptr");
        return;
    }
    WLOGFI("dms OnRemoteDied");
    adapter_.Clear();
    SingletonContainer::Get<DisplayManagerLite>().OnRemoteDied();
    SingletonContainer::Get<SessionManagerLite>().ClearSessionManagerProxy();
    return;
}

BaseAdapterLite::~BaseAdapterLite()
{
    WLOGFI("BaseAdapterLite destroy");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayManagerServiceProxy_ = nullptr;
}

void BaseAdapterLite::Clear()
{
    WLOGFI("BaseAdapterLite Clear");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((displayManagerServiceProxy_ != nullptr) && (displayManagerServiceProxy_->AsObject() != nullptr)) {
        displayManagerServiceProxy_->AsObject()->RemoveDeathRecipient(dmsDeath_);
    }
    isProxyValid_ = false;
}
} // namespace OHOS::Rosen