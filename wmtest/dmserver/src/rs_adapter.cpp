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

#include "rs_adapter.h"

#include <iremote_broker.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "RsAdapter"};
}

IMPLEMENT_SINGLE_INSTANCE(RsAdapter);

bool RsAdapter::InitRSProxyLocked()
{
    WLOGFI("InitRProxy");
    if (!rsProxy_) {
        sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            WLOGFE("Failed to get system ability mgr.");
            return false;
        }

        sptr<IRemoteObject> remoteObject
            = systemAbilityManager->GetSystemAbility();
        if (!remoteObject) {
            WLOGFE("Failed to get display manager service.");
            return false;
        }

        rsProxy_ = iface_cast<RSIRenderService>(remoteObject);
        if ((!rsProxy_) || (!rsProxy_->AsObject())) {
            WLOGFE("Failed to get system display manager services");
            return false;
        }

        rsDeath_ = new RSDeathRecipient(this);
        if (!rsDeath_) {
            WLOGFE("Failed to create death Recipient ptr RSDeathRecipient");
            return false;
        }
        if (!remoteObject->AddDeathRecipient(rsDeath_)) {
            WLOGFE("Failed to add death recipient");
            return false;
        }
    }
    return true;
}

RSDeathRecipient::RSDeathRecipient(sptr<RSAdatper>& rsAdapter)
{
    rsAdatper_ = rsAdapter;
}

void RSDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("object is null");
        return;
    }
    if (rsAdapter_ != null) {
        rsAdatper_.Clear();
        rsAdatper_ = nullptr;
    }
}

void RsAdapter::Clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if ((rsProxy_ != nullptr) && (rsProxy_->AsObject() != nullptr)) {
        rsProxy_->AsObject()->RemoveDeathRecipient(rsDeath_);
    }
    rsProxy_ = nullptr;
}
} // namespace OHOS::Rosen
