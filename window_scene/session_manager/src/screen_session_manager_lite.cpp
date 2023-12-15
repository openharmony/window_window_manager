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
    return screenSessionManager_->RegisterDisplayManagerAgent(displayManagerAgent, type);
}

DMError ScreenSessionManagerLite::UnregisterDisplayManagerAgent(
    const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type)
{
    ConnectToServer();
    return screenSessionManager_->UnregisterDisplayManagerAgent(displayManagerAgent, type);
}

std::vector<DisplayId> ScreenSessionManagerLite::GetAllDisplayIds()
{
    ConnectToServer();
    return screenSessionManager_->GetAllDisplayIds();
}

sptr<DisplayInfo> ScreenSessionManagerLite::GetDisplayInfoById(DisplayId displayId)
{
    ConnectToServer();
    return screenSessionManager_->GetDisplayInfoById(displayId);
}

sptr<DisplayInfo> ScreenSessionManagerLite::GetDefaultDisplayInfo()
{
    ConnectToServer();
    return screenSessionManager_->GetDefaultDisplayInfo();
}

sptr<DisplayInfo> ScreenSessionManagerLite::GetDisplayInfoByScreen(ScreenId screenId)
{
    ConnectToServer();
    return screenSessionManager_->GetDisplayInfoByScreen(screenId);
}

DMError ScreenSessionManagerLite::HasPrivateWindow(DisplayId id, bool& hasPrivateWindow)
{
    ConnectToServer();
    return screenSessionManager_->HasPrivateWindow(id, hasPrivateWindow);
}

DMError ScreenSessionManagerLite::DisableDisplaySnapshot(bool disableOrNot)
{
    ConnectToServer();
    return screenSessionManager_->DisableDisplaySnapshot(disableOrNot);
}

bool ScreenSessionManagerLite::WakeUpBegin(PowerStateChangeReason reason)
{
    ConnectToServer();
    return screenSessionManager_->WakeUpBegin(reason);
}

bool ScreenSessionManagerLite::WakeUpEnd()
{
    ConnectToServer();
    return screenSessionManager_->WakeUpEnd();
}

bool ScreenSessionManagerLite::SuspendBegin(PowerStateChangeReason reason)
{
    ConnectToServer();
    return screenSessionManager_->SuspendBegin(reason);
}

bool ScreenSessionManagerLite::SuspendEnd()
{
    ConnectToServer();
    return screenSessionManager_->SuspendEnd();
}

bool ScreenSessionManagerLite::SetDisplayState(DisplayState state)
{
    ConnectToServer();
    return screenSessionManager_->SetDisplayState(state);
}

DisplayState ScreenSessionManagerLite::GetDisplayState(DisplayId displayId)
{
    ConnectToServer();
    return screenSessionManager_->GetDisplayState(displayId);
}

void ScreenSessionManagerLite::NotifyDisplayEvent(DisplayEvent event)
{
    ConnectToServer();
    return screenSessionManager_->NotifyDisplayEvent(event);
}

void ScreenSessionManagerLite::SetFoldDisplayMode(const FoldDisplayMode displayMode)
{
    ConnectToServer();
    return screenSessionManager_->SetFoldDisplayMode(displayMode);
}

FoldDisplayMode ScreenSessionManagerLite::GetFoldDisplayMode()
{
    ConnectToServer();
    return screenSessionManager_->GetFoldDisplayMode();
}

bool ScreenSessionManagerLite::IsFoldable()
{
    ConnectToServer();
    return screenSessionManager_->IsFoldable();
}

FoldStatus ScreenSessionManagerLite::GetFoldStatus()
{
    ConnectToServer();
    return screenSessionManager_->GetFoldStatus();
}

sptr<FoldCreaseRegion> ScreenSessionManagerLite::GetCurrentFoldCreaseRegion()
{
    ConnectToServer();
    return screenSessionManager_->GetCurrentFoldCreaseRegion();
}

sptr<ScreenInfo> ScreenSessionManagerLite::GetScreenInfoById(ScreenId screenId)
{
    ConnectToServer();
    return screenSessionManager_->GetScreenInfoById(screenId);
}

sptr<ScreenGroupInfo> ScreenSessionManagerLite::GetScreenGroupInfoById(ScreenId screenId)
{
    ConnectToServer();
    return screenSessionManager_->GetScreenGroupInfoById(screenId);
}

DMError ScreenSessionManagerLite::GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos)
{
    ConnectToServer();
    return screenSessionManager_->GetAllScreenInfos(screenInfos);
}

bool ScreenSessionManagerLite::SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state,
    PowerStateChangeReason reason)
{
    ConnectToServer();
    return screenSessionManager_->SetSpecifiedScreenPower(screenId, state, reason);
}

bool ScreenSessionManagerLite::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    ConnectToServer();
    return screenSessionManager_->SetScreenPowerForAll(state, reason);
}

ScreenPowerState ScreenSessionManagerLite::GetScreenPower(ScreenId screenId)
{
    ConnectToServer();
    return screenSessionManager_->GetScreenPower(screenId);
}

DMError ScreenSessionManagerLite::SetScreenRotationLocked(bool isLocked)
{
    ConnectToServer();
    return screenSessionManager_->SetScreenRotationLocked(isLocked);
}

DMError ScreenSessionManagerLite::IsScreenRotationLocked(bool& isLocked)
{
    ConnectToServer();
    return screenSessionManager_->IsScreenRotationLocked(isLocked);
}

DMError ScreenSessionManagerLite::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    ConnectToServer();
    return screenSessionManager_->SetScreenActiveMode(screenId, modeId);
}

DMError ScreenSessionManagerLite::SetOrientation(ScreenId screenId, Orientation orientation)
{
    ConnectToServer();
    return screenSessionManager_->SetOrientation(screenId, orientation);
}

DMError ScreenSessionManagerLite::SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio)
{
    ConnectToServer();
    return screenSessionManager_->SetVirtualPixelRatio(screenId, virtualPixelRatio);
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
