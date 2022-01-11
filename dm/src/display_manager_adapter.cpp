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

#include "display_manager_adapter.h"

#include <iremote_broker.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManagerAdapter"};
}

DisplayId DisplayManagerAdapter::GetDefaultDisplayId()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (defaultDisplayId_ != DISPLAY_ID_INVALD) {
        return defaultDisplayId_;
    }

    if (!InitDMSProxyLocked()) {
        WLOGFE("displayManagerAdapter::GetDefaultDisplayId: InitDMSProxyLocked failed!");
        return DISPLAY_ID_INVALD;
    }
    defaultDisplayId_ = displayManagerServiceProxy_->GetDefaultDisplayId();
    return defaultDisplayId_;
}

sptr<Display> DisplayManagerAdapter::GetDisplayById(DisplayId displayId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto iter = displayMap_.find(displayId);
    if (iter != displayMap_.end()) {
        return iter->second;
    }

    if (!InitDMSProxyLocked()) {
        WLOGFE("displayManagerAdapter::GetDisplayById: InitDMSProxyLocked failed!");
        return nullptr;
    }
    DisplayInfo displayInfo = displayManagerServiceProxy_->GetDisplayInfoById(displayId);
    sptr<Display> display = new Display("", &displayInfo);
    if (display->GetId() != DISPLAY_ID_INVALD) {
        displayMap_[display->GetId()] = display;
    }
    return display;
}

sptr<Media::PixelMap> DisplayManagerAdapter::GetDisplaySnapshot(DisplayId displayId)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!InitDMSProxyLocked()) {
        WLOGFE("displayManagerAdapter::GetDisplaySnapshot: InitDMSProxyLocked failed!");
        return nullptr;
    }

    sptr<Media::PixelMap> dispalySnapshot = displayManagerServiceProxy_->GetDispalySnapshot(displayId);

    return dispalySnapshot;
}

DisplayId DisplayManagerAdapter::CreateVirtualDisplay(const VirtualDisplayInfo &virtualDisplayInfo,
    sptr<Surface> surface)
{
    if (!InitDMSProxyLocked()) {
        return DISPLAY_ID_INVALD;
    }
    WLOGFI("DisplayManagerAdapter::CreateVirtualDisplay");
    return displayManagerServiceProxy_->CreateVirtualDisplay(virtualDisplayInfo, surface);
}

bool DisplayManagerAdapter::DestroyVirtualDisplay(DisplayId displayId)
{
    if (!InitDMSProxyLocked()) {
        return false;
    }
    WLOGFI("DisplayManagerAdapter::DestroyVirtualDisplay");
    return displayManagerServiceProxy_->DestroyVirtualDisplay(displayId);
}

bool DisplayManagerAdapter::SuspendBegin(PowerStateChangeReason reason)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!InitDMSProxyLocked()) {
        return false;
    }
    return displayManagerServiceProxy_->SuspendBegin(reason);
}

bool DisplayManagerAdapter::SetDisplayState(DisplayState state, DisplayStateCallback callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!InitDMSProxyLocked()) {
        return false;
    }
    callback_ = callback;
    bool ret = displayManagerServiceProxy_->SetDisplayState(state);
    if (state == DisplayState::OFF) {
        NotifyDisplayChange(state);
    }
    // TODO: NotifyDisplayChange ON when keyguard is drawn
    return ret;
}

DisplayState DisplayManagerAdapter::GetDisplayState(uint64_t displayId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!InitDMSProxyLocked()) {
        return DisplayState::UNKNOWN;
    }
    return displayManagerServiceProxy_->GetDisplayState(displayId);
}

void DisplayManagerAdapter::NotifyDisplayEvent(DisplayEvent event)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!InitDMSProxyLocked()) {
        return;
    }
    displayManagerServiceProxy_->NotifyDisplayEvent(event);
}

void DisplayManagerAdapter::NotifyDisplayChange(DisplayState state)
{
    if (callback_) {
        callback_(state);
        return;
    }
    WLOGFW("callback_ target is not set!");
}

bool DisplayManagerAdapter::InitDMSProxyLocked()
{
    WLOGFI("InitDMSProxy");
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

        dmsDeath_ = new DMSDeathRecipient();
        if (!dmsDeath_) {
            WLOGFE("Failed to create death Recipient ptr DMSDeathRecipient");
            return false;
        }
        if (!remoteObject->AddDeathRecipient(dmsDeath_)) {
            WLOGFE("Failed to add death recipient");
            return false;
        }
    }
    return true;
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
    SingletonContainer::Get<DisplayManagerAdapter>().Clear();
    return;
}

void DisplayManagerAdapter::Clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if ((displayManagerServiceProxy_ != nullptr) && (displayManagerServiceProxy_->AsObject() != nullptr)) {
        displayManagerServiceProxy_->AsObject()->RemoveDeathRecipient(dmsDeath_);
    }
    displayManagerServiceProxy_ = nullptr;
}
} // namespace OHOS::Rosen