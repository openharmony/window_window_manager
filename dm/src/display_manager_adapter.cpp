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
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (defaultDisplayId_ != DISPLAY_ID_INVALD) {
            return defaultDisplayId_;
        }
    }

    if (!InitDMSProxy()) {
        WLOGFE("displayManagerAdapter::GetDefaultDisplayId: InitDMSProxy failed!");
        return DISPLAY_ID_INVALD;
    }
    DisplayId displayId = displayManagerServiceProxy_->GetDefaultDisplayId();
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        defaultDisplayId_ = displayId;
    }
    return displayId;
}

sptr<Display> DisplayManagerAdapter::GetDisplayById(DisplayId displayId)
{
    if (!InitDMSProxy()) {
        WLOGFE("GetDisplayById: InitDMSProxy failed!");
        return nullptr;
    }

    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = displayMap_.find(displayId);
    if (iter != displayMap_.end()) {
        // Update display in map
        // should be updated automatically
        auto displayInfo = displayManagerServiceProxy_->GetDisplayInfoById(displayId);
        if (displayInfo == nullptr) {
            WLOGFE("GetDisplayById: displayInfo is nullptr!");
            displayMap_.erase(iter);
            return nullptr;
        }
        if (displayInfo->GetDisplayId() == DISPLAY_ID_INVALD) {
            WLOGFE("GetDisplayById: Get invalid displayInfo!");
            displayMap_.erase(iter);
            return nullptr;
        }
        sptr<Display> display = iter->second;
        display->UpdateDisplayInfo(displayInfo);
        return iter->second;
    }
    auto displayInfo = displayManagerServiceProxy_->GetDisplayInfoById(displayId);
    if (displayInfo == nullptr) {
        WLOGFE("GetDisplayById: displayInfo is nullptr!");
        return nullptr;
    }
    sptr<Display> display = new Display("", displayInfo);
    if (display->GetId() != DISPLAY_ID_INVALD) {
        displayMap_[display->GetId()] = display;
    }
    return display;
}

bool ScreenManagerAdapter::RequestRotation(ScreenId screenId, Rotation rotation)
{
    if (!InitDMSProxy()) {
        WLOGFE("fail to request rotation: InitDMSProxy failed!");
        return false;
    }

    return displayManagerServiceProxy_->RequestRotation(screenId, rotation);
}

std::shared_ptr<Media::PixelMap> DisplayManagerAdapter::GetDisplaySnapshot(DisplayId displayId)
{
    if (!InitDMSProxy()) {
        WLOGFE("displayManagerAdapter::GetDisplaySnapshot: InitDMSProxy failed!");
        return nullptr;
    }

    return displayManagerServiceProxy_->GetDispalySnapshot(displayId);
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
    if (!displayManagerServiceProxy_) {
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
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if ((displayManagerServiceProxy_ != nullptr) && (displayManagerServiceProxy_->AsObject() != nullptr)) {
        displayManagerServiceProxy_->AsObject()->RemoveDeathRecipient(dmsDeath_);
    }
    displayManagerServiceProxy_ = nullptr;
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
    if (screenInfo == nullptr) {
        WLOGFE("screenInfo is null");
    }
    return screenInfo;
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

sptr<Screen> ScreenManagerAdapter::GetScreenById(ScreenId screenId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    sptr<ScreenInfo> screenInfo = GetScreenInfo(screenId);
    if (screenInfo == nullptr) {
        WLOGFE("screenInfo is null");
        screenMap_.erase(screenId);
        return nullptr;
    }
    auto iter = screenMap_.find(screenId);
    if (iter != screenMap_.end()) {
        WLOGFI("get screen in screen map");
        iter->second->UpdateScreenInfo(screenInfo);
        return iter->second;
    }
    sptr<Screen> screen = new Screen(screenInfo);
    screenMap_.insert(std::make_pair(screenId, screen));
    return screen;
}

sptr<ScreenGroup> ScreenManagerAdapter::GetScreenGroupById(ScreenId screenId)
{
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenGroup id is invalid");
        return nullptr;
    }
    if (!InitDMSProxy()) {
        WLOGFE("InitDMSProxy failed!");
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        screenGroupMap_.clear();
        return nullptr;
    }
    sptr<ScreenGroupInfo> screenGroupInfo = displayManagerServiceProxy_->GetScreenGroupInfoById(screenId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (screenGroupInfo == nullptr) {
        WLOGFE("screenGroupInfo is null");
        screenGroupMap_.erase(screenId);
        return nullptr;
    }
    auto iter = screenGroupMap_.find(screenId);
    if (iter != screenGroupMap_.end()) {
        WLOGFI("get screenGroup in screenGroup map");
        iter->second->UpdateScreenGroupInfo(screenGroupInfo);
        return iter->second;
    }
    sptr<ScreenGroup> screenGroup = new ScreenGroup(screenGroupInfo);
    screenGroupMap_.insert(std::make_pair(screenId, screenGroup));
    return screenGroup;
}

std::vector<sptr<Screen>> ScreenManagerAdapter::GetAllScreens()
{
    std::vector<sptr<Screen>> screens;
    if (!InitDMSProxy()) {
        WLOGFE("InitDMSProxy failed!");
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        screenMap_.clear();
        return screens;
    }
    std::vector<sptr<ScreenInfo>> screenInfos = displayManagerServiceProxy_->GetAllScreenInfos();
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto info: screenInfos) {
        if (info == nullptr) {
            WLOGFE("screenInfo is null");
            continue;
        }
        auto iter = screenMap_.find(info->GetScreenId());
        if (iter != screenMap_.end()) {
            WLOGFI("get screen in screen map");
            iter->second->UpdateScreenInfo(info);
            screens.emplace_back(iter->second);
        } else {
            sptr<Screen> screen = new Screen(info);
            screens.emplace_back(screen);
        }
    }
    screenMap_.clear();
    for (auto screen: screens) {
        screenMap_.insert(std::make_pair(screen->GetId(), screen));
    }
    return screens;
}

ScreenId ScreenManagerAdapter::MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint)
{
    if (!InitDMSProxy()) {
        return SCREEN_ID_INVALID;
    }
    return displayManagerServiceProxy_->MakeExpand(screenId, startPoint);
}

bool ScreenManagerAdapter::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    if (!InitDMSProxy()) {
        return false;
    }
    return displayManagerServiceProxy_->SetScreenActiveMode(screenId, modeId);
}

void ScreenManagerAdapter::UpdateScreenInfo(ScreenId screenId)
{
    auto screenInfo = GetScreenInfo(screenId);
    if (screenInfo == nullptr) {
        WLOGFE("screenInfo is invalid");
        return;
    }
    auto iter = screenMap_.find(screenId);
    if (iter != screenMap_.end()) {
        iter->second->UpdateScreenInfo(screenInfo);
    }
}

void DisplayManagerAdapter::UpdateDisplayInfo(DisplayId displayId)
{
    auto displayInfo = GetDisplayInfo(displayId);
    if (displayInfo == nullptr) {
        WLOGFE("displayInfo is invalid");
        return;
    }
    auto iter = displayMap_.find(displayId);
    if (iter != displayMap_.end()) {
        iter->second->UpdateDisplayInfo(displayInfo);
    }
}
} // namespace OHOS::Rosen