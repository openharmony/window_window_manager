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

#define INIT_PROXY_CHECK_RETURN(ret) \
    do { \
        if (!InitDMSProxy()) { \
            WLOGFE("InitDMSProxy failed!"); \
            return ret; \
        } \
    } while (false)

DisplayId DisplayManagerAdapter::GetDefaultDisplayId()
{
    INIT_PROXY_CHECK_RETURN(DISPLAY_ID_INVALD);

    return displayManagerServiceProxy_->GetDefaultDisplayId();
}

sptr<DisplayInfo> DisplayManagerAdapter::GetDisplayInfoByScreenId(ScreenId screenId)
{
    INIT_PROXY_CHECK_RETURN(nullptr);

    return  displayManagerServiceProxy_->GetDisplayInfoByScreen(screenId);
}

std::shared_ptr<Media::PixelMap> DisplayManagerAdapter::GetDisplaySnapshot(DisplayId displayId)
{
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetDisplaySnapshot(displayId);
}

DMError ScreenManagerAdapter::GetScreenSupportedColorGamuts(ScreenId screenId,
    std::vector<ScreenColorGamut>& colorGamuts)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->GetScreenSupportedColorGamuts(screenId, colorGamuts);
}

DMError ScreenManagerAdapter::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->GetScreenColorGamut(screenId, colorGamut);
}

DMError ScreenManagerAdapter::SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->SetScreenColorGamut(screenId, colorGamutIdx);
}

DMError ScreenManagerAdapter::GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->GetScreenGamutMap(screenId, gamutMap);
}

DMError ScreenManagerAdapter::SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->SetScreenGamutMap(screenId, gamutMap);
}

DMError ScreenManagerAdapter::SetScreenColorTransform(ScreenId screenId)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    return displayManagerServiceProxy_->SetScreenColorTransform(screenId);
}

ScreenId ScreenManagerAdapter::CreateVirtualScreen(VirtualScreenOption option)
{
    INIT_PROXY_CHECK_RETURN(SCREEN_ID_INVALID);

    WLOGFI("DisplayManagerAdapter::CreateVirtualScreen");
    return displayManagerServiceProxy_->CreateVirtualScreen(option);
}

DMError ScreenManagerAdapter::DestroyVirtualScreen(ScreenId screenId)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    WLOGFI("DisplayManagerAdapter::DestroyVirtualScreen");
    return displayManagerServiceProxy_->DestroyVirtualScreen(screenId);
}

DMError ScreenManagerAdapter::SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface)
{
    INIT_PROXY_CHECK_RETURN(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED);

    WLOGFI("DisplayManagerAdapter::SetVirtualScreenSurface");
    return displayManagerServiceProxy_->SetVirtualScreenSurface(screenId, surface);
}

bool ScreenManagerAdapter::SetOrientation(ScreenId screenId, Orientation orientation)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->SetOrientation(screenId, orientation);
}

bool BaseAdapter::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->RegisterDisplayManagerAgent(displayManagerAgent, type);
}

bool BaseAdapter::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->UnregisterDisplayManagerAgent(displayManagerAgent, type);
}

bool DisplayManagerAdapter::WakeUpBegin(PowerStateChangeReason reason)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->WakeUpBegin(reason);
}

bool DisplayManagerAdapter::WakeUpEnd()
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->WakeUpEnd();
}

bool DisplayManagerAdapter::SuspendBegin(PowerStateChangeReason reason)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->SuspendBegin(reason);
}

bool DisplayManagerAdapter::SuspendEnd()
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->SuspendEnd();
}

bool DisplayManagerAdapter::SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->SetScreenPowerForAll(state, reason);
}


bool DisplayManagerAdapter::SetDisplayState(DisplayState state)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->SetDisplayState(state);
}

DisplayState DisplayManagerAdapter::GetDisplayState(DisplayId displayId)
{
    INIT_PROXY_CHECK_RETURN(DisplayState::UNKNOWN);

    return displayManagerServiceProxy_->GetDisplayState(displayId);
}

void DisplayManagerAdapter::NotifyDisplayEvent(DisplayEvent event)
{
    INIT_PROXY_CHECK_RETURN();

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
    INIT_PROXY_CHECK_RETURN(SCREEN_ID_INVALID);

    return displayManagerServiceProxy_->MakeMirror(mainScreenId, mirrorScreenId);
}

sptr<ScreenInfo> ScreenManagerAdapter::GetScreenInfo(ScreenId screenId)
{
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screen id is invalid");
        return nullptr;
    }
    INIT_PROXY_CHECK_RETURN(nullptr);

    sptr<ScreenInfo> screenInfo = displayManagerServiceProxy_->GetScreenInfoById(screenId);
    return screenInfo;
}

std::vector<DisplayId> DisplayManagerAdapter::GetAllDisplayIds()
{
    INIT_PROXY_CHECK_RETURN(std::vector<DisplayId>());

    return displayManagerServiceProxy_->GetAllDisplayIds();
}

sptr<DisplayInfo> DisplayManagerAdapter::GetDisplayInfo(DisplayId displayId)
{
    if (displayId == DISPLAY_ID_INVALD) {
        WLOGFE("screen id is invalid");
        return nullptr;
    }
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetDisplayInfoById(displayId);
}

sptr<ScreenGroupInfo> ScreenManagerAdapter::GetScreenGroupInfoById(ScreenId screenId)
{
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenGroup id is invalid");
        return nullptr;
    }
    INIT_PROXY_CHECK_RETURN(nullptr);

    return displayManagerServiceProxy_->GetScreenGroupInfoById(screenId);
}

std::vector<sptr<ScreenInfo>> ScreenManagerAdapter::GetAllScreenInfos()
{
    INIT_PROXY_CHECK_RETURN(std::vector<sptr<ScreenInfo>>());

    return displayManagerServiceProxy_->GetAllScreenInfos();
}

ScreenId ScreenManagerAdapter::MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint)
{
    INIT_PROXY_CHECK_RETURN(SCREEN_ID_INVALID);

    return displayManagerServiceProxy_->MakeExpand(screenId, startPoint);
}

void ScreenManagerAdapter::RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens)
{
    INIT_PROXY_CHECK_RETURN();

    displayManagerServiceProxy_->RemoveVirtualScreenFromGroup(screens);
}

bool ScreenManagerAdapter::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    INIT_PROXY_CHECK_RETURN(false);

    return displayManagerServiceProxy_->SetScreenActiveMode(screenId, modeId);
}
} // namespace OHOS::Rosen