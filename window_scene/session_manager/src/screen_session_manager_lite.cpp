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

#include "session_manager/include/screen_session_manager_lite.h"
#include <system_ability_definition.h>
#include <iservice_registry.h>
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSessionManagerLite" };
std::recursive_mutex g_instanceMutex;
} // namespace

ScreenSessionManagerLite::~ScreenSessionManagerLite()
{
    WLOGFD("ScreenSessionManagerLite destroy");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    destroyed_ = true;
}

void ScreenSessionManagerLite::ConnectToServer()
{
    if (screenSessionManager_) {
        return;
    }
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityMgr) {
        WLOGFE("Failed to get system ability mgr");
        return;
    }

    auto remoteObject = systemAbilityMgr->GetSystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID);
    if (!remoteObject) {
        WLOGFE("Failed to get display manager service");
        return;
    }

    screenSessionManager_ = iface_cast<IScreenSessionManager>(remoteObject);
    if (screenSessionManager_) {
        ssmDeath_ = new (std::nothrow) ScreenSMDeathRecipient();
        if (!ssmDeath_) {
            WLOGFE("Failed to create death Recipient ptr WMSDeathRecipient");
            return;
        }
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(ssmDeath_)) {
            WLOGFE("Failed to add death recipient");
            return;
        }
    }
    if (!screenSessionManager_) {
        WLOGFE("Failed to get screen session manager proxy");
        return;
    }
}

ScreenSessionManagerLite& ScreenSessionManagerLite::GetInstance()
{
    std::lock_guard<std::recursive_mutex> lock(g_instanceMutex);
    static ScreenSessionManagerLite* instance = nullptr;
    if (instance == nullptr) {
        instance = new ScreenSessionManagerLite();
    }
    return *instance;
}

DMError ScreenSessionManagerLite::RegisterDisplayManagerAgent(
    const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type)
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->RegisterDisplayManagerAgent(displayManagerAgent, type);
    }
    return DMError::DM_ERROR_NULLPTR;
}

DMError ScreenSessionManagerLite::UnregisterDisplayManagerAgent(
    const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type)
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->UnregisterDisplayManagerAgent(displayManagerAgent, type);
    }
    return DMError::DM_ERROR_NULLPTR;
}

std::vector<DisplayId> ScreenSessionManagerLite::GetAllDisplayIds()
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->GetAllDisplayIds();
    }
    std::vector<DisplayId> res;
    return res;
}

sptr<DisplayInfo> ScreenSessionManagerLite::GetDisplayInfoById(DisplayId displayId)
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->GetDisplayInfoById(displayId);
    }
    return nullptr;
}

sptr<DisplayInfo> ScreenSessionManagerLite::GetDefaultDisplayInfo()
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->GetDefaultDisplayInfo();
    }
    return nullptr;
}

sptr<DisplayInfo> ScreenSessionManagerLite::GetDisplayInfoByScreen(ScreenId screenId)
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->GetDisplayInfoByScreen(screenId);
    }
    return nullptr;
}

DMError ScreenSessionManagerLite::HasPrivateWindow(DisplayId id, bool& hasPrivateWindow)
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->HasPrivateWindow(id, hasPrivateWindow);
    }
    return DMError::DM_ERROR_NULLPTR;
}

DMError ScreenSessionManagerLite::DisableDisplaySnapshot(bool disableOrNot)
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->DisableDisplaySnapshot(disableOrNot);
    }
    return DMError::DM_ERROR_NULLPTR;
}

bool ScreenSessionManagerLite::WakeUpBegin(PowerStateChangeReason reason)
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->WakeUpBegin(reason);
    }
    return false;
}

bool ScreenSessionManagerLite::WakeUpEnd()
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->WakeUpEnd();
    }
    return false;
}

bool ScreenSessionManagerLite::SuspendBegin(PowerStateChangeReason reason)
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->SuspendBegin(reason);
    }
    return false;
}

bool ScreenSessionManagerLite::SuspendEnd()
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->SuspendEnd();
    }
    return false;
}

bool ScreenSessionManagerLite::SetDisplayState(DisplayState state)
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->SetDisplayState(state);
    }
    return false;
}

DisplayState ScreenSessionManagerLite::GetDisplayState(DisplayId displayId)
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->GetDisplayState(displayId);
    }
    return DisplayState::UNKNOWN;
}

void ScreenSessionManagerLite::NotifyDisplayEvent(DisplayEvent event)
{
    ConnectToServer();
    if (screenSessionManager_) {
        return screenSessionManager_->NotifyDisplayEvent(event);
    }
}

void ScreenSessionManagerLite::Clear()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((screenSessionManager_ != nullptr) && (screenSessionManager_->AsObject() != nullptr)) {
        screenSessionManager_->AsObject()->RemoveDeathRecipient(ssmDeath_);
    }
    if (destroyed_) {
        WLOGFD("Already destroyed");
        return;
    }
    screenSessionManager_ = nullptr;
}

void ScreenSMDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
{
    if (wptrDeath == nullptr) {
        WLOGFE("ScreenSMDeathRecipient wptrDeath is null");
        return;
    }

    sptr<IRemoteObject> object = wptrDeath.promote();
    if (!object) {
        WLOGFE("ScreenSMDeathRecipient object is null");
        return;
    }
    WLOGI("ScreenSessionManagerLite OnRemoteDied");
    ScreenSessionManagerLite::GetInstance().Clear();
}
} // namespace OHOS::Rosen
