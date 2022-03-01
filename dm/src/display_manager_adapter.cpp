/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "display_manager_adapter.h"

#include <iremote_broker.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerAdapter"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerAdapter)
WM_IMPLEMENT_SINGLE_INSTANCE(ScreenManagerAdapter)

DisplayId DisplayManagerAdapter::GetDefaultDisplayId()
{
    if (!InitDMSProxy()) {
        WLOGFE("displayManagerAdapter::GetDefaultDisplayId: InitDMSProxy failed!");
        return DISPLAY_ID_INVALD;
    }
    return displayManagerServiceProxy_->GetDefaultDisplayId();
}

sptr<DisplayInfo> DisplayManagerAdapter::GetDisplayInfoByScreenId(ScreenId screenId)
{
    if (!InitDMSProxy()) {
        WLOGFE("get display by screenId: init dms proxy failed!");
        return nullptr;
    }
    return  displayManagerServiceProxy_->GetDisplayInfoByScreen(screenId);
}

std::shared_ptr<Media::PixelMap> DisplayManagerAdapter::GetDisplaySnapshot(DisplayId displayId)
{
    if (!InitDMSProxy()) {
        WLOGFE("GetDisplaySnapshot: init dms proxy failed!");
        return nullptr;
    }

    return displayManagerServiceProxy_->GetDisplaySnapshot(displayId);
}

DMError ScreenManagerAdapter::GetScreenSupportedColorGamuts(ScreenId screenId,
    std::vector<ScreenColorGamut>& colorGamuts)
{
    if (!InitDMSProxy()) {
        WLOGFE("displayManagerAdapter::GetScreenSupportedColorGamuts: InitDMSProxy failed!");
        return DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED;
    }

    return displayManagerServiceProxy_->GetScreenSupportedColorGamuts(screenId, colorGamuts);
}

DMError ScreenManagerAdapter::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    if (!InitDMSProxy()) {
        WLOGFE("displayManagerAdapter::GetScreenColorGamut: InitDMSProxy failed!");
        return DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED;
    }

    return displayManagerServiceProxy_->GetScreenColorGamut(screenId, colorGamut);
}

DMError ScreenManagerAdapter::SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx)
{
    if (!InitDMSProxy()) {
        WLOGFE("displayManagerAdapter::SetScreenColorGamut: InitDMSProxy failed!");
        return DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED;
    }

    return displayManagerServiceProxy_->SetScreenColorGamut(screenId, colorGamutIdx);
}

DMError ScreenManagerAdapter::GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap)
{
    if (!InitDMSProxy()) {
        WLOGFE("displayManagerAdapter::GetScreenGamutMap: InitDMSProxy failed!");
        return DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED;
    }

    return displayManagerServiceProxy_->GetScreenGamutMap(screenId, gamutMap);
}

DMError ScreenManagerAdapter::SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap)
{
    if (!InitDMSProxy()) {
        WLOGFE("displayManagerAdapter::SetScreenGamutMap: InitDMSProxy failed!");
        return DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED;
    }

    return displayManagerServiceProxy_->SetScreenGamutMap(screenId, gamutMap);
}

DMError ScreenManagerAdapter::SetScreenColorTransform(ScreenId screenId)
{
    if (!InitDMSProxy()) {
        WLOGFE("displayManagerAdapter::SetScreenColorTransform: InitDMSProxy failed!");
        return DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED;
    }

    return displayManagerServiceProxy_->SetScreenColorTransform(screenId);
}

ScreenId ScreenManagerAdapter::CreateVirtualScreen(VirtualScreenOption option)
{
    if (!InitDMSProxy()) {
        return SCREEN_ID_INVALID;
    }
    WLOGFI("DisplayManagerAdapter::CreateVirtualScreen");
    return displayManagerServiceProxy_->CreateVirtualScreen(option);
}

DMError ScreenManagerAdapter::DestroyVirtualScreen(ScreenId screenId)
{
    if (!InitDMSProxy()) {
        return DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED;
    }
    WLOGFI("DisplayManagerAdapter::DestroyVirtualScreen");
    return displayManagerServiceProxy_->DestroyVirtualScreen(screenId);
}

DMError ScreenManagerAdapter::SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface)
{
    if (!InitDMSProxy()) {
        return DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED;
    }
    WLOGFI("DisplayManagerAdapter::SetVirtualScreenSurface");
    return displayManagerServiceProxy_->SetVirtualScreenSurface(screenId, surface);
}

bool ScreenManagerAdapter::SetOrientation(ScreenId screenId, Orientation orientation)
{
    if (!InitDMSProxy()) {
        WLOGFE("fail to set orientation: InitDMSProxy failed!");
        return false;
    }
    return displayManagerServiceProxy_->SetOrientation(screenId, orientation);
}

bool BaseAdapter::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    if (!InitDMSProxy()) {
        return false;
    }
    return displayManagerServiceProxy_->RegisterDisplayManagerAgent(displayManagerAgent, type);
}

bool BaseAdapter::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    if (!InitDMSProxy()) {
        return false;
    }
    return displayManagerServiceProxy_->UnregisterDisplayManagerAgent(displayManagerAgent, type);
}

bool DisplayManagerAdapter::WakeUpBegin(PowerStateChangeReason reason)
{
    if (!InitDMSProxy()) {
        return false;
    }
    return displayManagerServiceProxy_->WakeUpBegin(reason);
}

bool DisplayManagerAdapter::WakeUpEnd()
{
    if (!InitDMSProxy()) {
        return false;
    }
    return displayManagerServiceProxy_->WakeUpEnd();
}

bool DisplayManagerAdapter::SuspendBegin(PowerStateChangeReason reason)
{
    if (!InitDMSProxy()) {
        return false;
    }
    return displayManagerServiceProxy_->SuspendBegin(reason);
}

bool DisplayManagerAdapter::SuspendEnd()
{
    if (!InitDMSProxy()) {
        return false;
    }
    return displayManagerServiceProxy_->SuspendEnd();
}

bool DisplayManagerAdapter::SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason)
{
    if (!InitDMSProxy()) {
        return false;
    }
    return displayManagerServiceProxy_->SetScreenPowerForAll(state, reason);
}


bool DisplayManagerAdapter::SetDisplayState(DisplayState state)
{
    if (!InitDMSProxy()) {
        return false;
    }
    return displayManagerServiceProxy_->SetDisplayState(state);
}

DisplayState DisplayManagerAdapter::GetDisplayState(DisplayId displayId)
{
    if (!InitDMSProxy()) {
        return DisplayState::UNKNOWN;
    }
    return displayManagerServiceProxy_->GetDisplayState(displayId);
}

void DisplayManagerAdapter::NotifyDisplayEvent(DisplayEvent event)
{
    if (!InitDMSProxy()) {
        return;
    }
    displayManagerServiceProxy_->NotifyDisplayEvent(event);
}

bool BaseAdapter::InitDMSProxy()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!isProxyValid_) {
        sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            WLOGFE("Failed to get system ability mgr.");
            return false;
        }

        sptr<IRemoteObject> remoteObject
            = systemAbilityManager->GetSystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID);
        if (!remoteObject) {
            WLOGFE("Failed to get display manager service.");
            return false;
        }

        displayManagerServiceProxy_ = iface_cast<IDisplayManager>(remoteObject);
        if ((!displayManagerServiceProxy_) || (!displayManagerServiceProxy_->AsObject())) {
            WLOGFE("Failed to get system display manager services");
            return false;
        }

        dmsDeath_ = new DMSDeathRecipient(*this);
        if (!dmsDeath_) {
            WLOGFE("Failed to create death Recipient ptr DMSDeathRecipient");
            return false;
        }
        if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(dmsDeath_)) {
            WLOGFE("Failed to add death recipient");
            return false;
        }
        isProxyValid_ = true;
    }
    return true;
}

DMSDeathRecipient::DMSDeathRecipient(BaseAdapter& adapter) : adapter_(adapter)
{
}

void DMSDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
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
    adapter_.Clear();
    return;
}

void BaseAdapter::Clear()
{
    if ((displayManagerServiceProxy_ != nullptr) && (displayManagerServiceProxy_->AsObject() != nullptr)) {
        displayManagerServiceProxy_->AsObject()->RemoveDeathRecipient(dmsDeath_);
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    isProxyValid_ = false;
}

ScreenId ScreenManagerAdapter::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId)
{
    if (!InitDMSProxy()) {
        return SCREEN_ID_INVALID;
    }
    return displayManagerServiceProxy_->MakeMirror(mainScreenId, mirrorScreenId);
}

sptr<ScreenInfo> ScreenManagerAdapter::GetScreenInfo(ScreenId screenId)
{
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screen id is invalid");
        return nullptr;
    }
    if (!InitDMSProxy()) {
        WLOGFE("InitDMSProxy failed!");
        return nullptr;
    }
    sptr<ScreenInfo> screenInfo = displayManagerServiceProxy_->GetScreenInfoById(screenId);
    return screenInfo;
}

std::vector<DisplayId> DisplayManagerAdapter::GetAllDisplayIds()
{
    if (!InitDMSProxy()) {
        WLOGFE("InitDMSProxyLocked failed!");
        return {};
    }
    return displayManagerServiceProxy_->GetAllDisplayIds();
}

sptr<DisplayInfo> DisplayManagerAdapter::GetDisplayInfo(DisplayId displayId)
{
    if (displayId == DISPLAY_ID_INVALD) {
        WLOGFE("screen id is invalid");
        return nullptr;
    }
    if (!InitDMSProxy()) {
        WLOGFE("InitDMSProxy failed!");
        return nullptr;
    }
    return displayManagerServiceProxy_->GetDisplayInfoById(displayId);
}

sptr<ScreenGroupInfo> ScreenManagerAdapter::GetScreenGroupInfoById(ScreenId screenId)
{
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenGroup id is invalid");
        return nullptr;
    }
    if (!InitDMSProxy()) {
        WLOGFE("InitDMSProxy failed!");
        return nullptr;
    }
    return displayManagerServiceProxy_->GetScreenGroupInfoById(screenId);
}

std::vector<sptr<ScreenInfo>> ScreenManagerAdapter::GetAllScreenInfos()
{
    if (!InitDMSProxy()) {
        WLOGFE("InitDMSProxy failed!");
        std::vector<sptr<ScreenInfo>> result;
        return result;
    }
    return displayManagerServiceProxy_->GetAllScreenInfos();
}

ScreenId ScreenManagerAdapter::MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint)
{
    if (!InitDMSProxy()) {
        return SCREEN_ID_INVALID;
    }
    return displayManagerServiceProxy_->MakeExpand(screenId, startPoint);
}

void ScreenManagerAdapter::CancelMakeMirrorOrExpand(std::vector<ScreenId> screens)
{
    if (!InitDMSProxy()) {
        return;
    }
    displayManagerServiceProxy_->CancelMakeMirrorOrExpand(screens);
}

bool ScreenManagerAdapter::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    if (!InitDMSProxy()) {
        return false;
    }
    return displayManagerServiceProxy_->SetScreenActiveMode(screenId, modeId);
}
} // namespace OHOS::Rosen