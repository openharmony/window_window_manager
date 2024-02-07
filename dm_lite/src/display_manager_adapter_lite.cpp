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
#include "zidl/mock_session_manager_service_interface.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DMS_DM_LITE, "DisplayManagerAdapterLite"};
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
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            WLOGFE("Failed to get system ability mgr.");
            return false;
        }
        sptr<IRemoteObject> mockRemoteObject = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
        if (!mockRemoteObject) {
            WLOGFI("Remote object is nullptr");
            return false;
        }
        auto mockSessionManagerServiceProxy_ = iface_cast<IMockSessionManagerInterface>(mockRemoteObject);
        if (!mockSessionManagerServiceProxy_) {
            WLOGFW("Get mock session manager service proxy failed, nullptr");
            return false;
        }

        sptr<IRemoteObject> displayRemoteObject = mockSessionManagerServiceProxy_->GetScreenSessionManagerLite();
        if (!displayRemoteObject) {
            WLOGFE("displayRemoteObject is nullptr");
            return false;
        }
        displayManagerServiceProxy_ = iface_cast<IScreenSessionManagerLite>(displayRemoteObject);
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

sptr<DisplayInfo> DisplayManagerAdapterLite::GetDefaultDisplayInfo()
{
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetDefaultDisplayInfo();
}

bool DisplayManagerAdapterLite::IsFoldable()
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->IsFoldable();
}

FoldStatus DisplayManagerAdapterLite::GetFoldStatus()
{
    INIT_PROXY_CHECK_RETURN(FoldStatus::UNKNOWN);

    return displayManagerServiceProxy_->GetFoldStatus();
}

FoldDisplayMode DisplayManagerAdapterLite::GetFoldDisplayMode()
{
    INIT_PROXY_CHECK_RETURN(FoldDisplayMode::UNKNOWN);

    return displayManagerServiceProxy_->GetFoldDisplayMode();
}

sptr<DisplayInfo> DisplayManagerAdapterLite::GetDisplayInfo(DisplayId displayId)
{
    if (displayId == DISPLAY_ID_INVALID) {
        WLOGFW("screen id is invalid");
        return nullptr;
    }
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetDisplayInfoById(displayId);
}

sptr<CutoutInfo> DisplayManagerAdapterLite::GetCutoutInfo(DisplayId displayId)
{
    if (displayId == DISPLAY_ID_INVALID) {
        WLOGFE("screen id is invalid");
        return nullptr;
    }
    INIT_PROXY_CHECK_RETURN(nullptr);
    return displayManagerServiceProxy_->GetCutoutInfo(displayId);
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