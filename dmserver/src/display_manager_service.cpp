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

#include "display_manager_service.h"

#include <cinttypes>
#include <unistd.h>

#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManagerService"};
}

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<DisplayManagerService>());

DisplayManagerService::DisplayManagerService() : SystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID, true),
    abstractScreenController_(new AbstractScreenController(mutex_)),
    abstractDisplayController_(new AbstractDisplayController(mutex_))
{
}

void DisplayManagerService::OnStart()
{
    WLOGFI("DisplayManagerService::OnStart start");
    if (!Init()) {
        return;
    }
}

bool DisplayManagerService::Init()
{
    WLOGFI("DisplayManagerService::Init start");
    bool ret = Publish(this);
    if (!ret) {
        WLOGFW("DisplayManagerService::Init failed");
        return false;
    }
    abstractScreenController_->Init();
    abstractDisplayController_->Init(abstractScreenController_);
    WLOGFI("DisplayManagerService::Init success");
    return true;
}

DisplayId DisplayManagerService::GetDisplayIdFromScreenId(ScreenId screenId)
{
    return (DisplayId)screenId;
}

ScreenId DisplayManagerService::GetScreenIdFromDisplayId(DisplayId displayId)
{
    return (ScreenId)displayId;
}

DisplayId DisplayManagerService::GetDefaultDisplayId()
{
    ScreenId screenId = abstractDisplayController_->GetDefaultScreenId();
    WLOGFI("GetDefaultDisplayId %{public}" PRIu64"", screenId);
    return GetDisplayIdFromScreenId(screenId);
}

DisplayInfo DisplayManagerService::GetDisplayInfoById(DisplayId displayId)
{
    DisplayInfo displayInfo;
    ScreenId screenId = GetScreenIdFromDisplayId(displayId);
    auto screenModeInfo = abstractDisplayController_->GetScreenActiveMode(screenId);
    displayInfo.id_ = displayId;
    displayInfo.width_ = screenModeInfo.GetScreenWidth();
    displayInfo.height_ = screenModeInfo.GetScreenHeight();
    displayInfo.freshRate_ = screenModeInfo.GetScreenFreshRate();
    return displayInfo;
}

DisplayId DisplayManagerService::CreateVirtualDisplay(const VirtualDisplayInfo &virtualDisplayInfo,
    sptr<Surface> surface)
{
    WLOGFI("name %{public}s, width %{public}u, height %{public}u, mirrotId %{public}" PRIu64", flags %{public}d",
        virtualDisplayInfo.name_.c_str(), virtualDisplayInfo.width_, virtualDisplayInfo.height_,
        virtualDisplayInfo.displayIdToMirror_, virtualDisplayInfo.flags_);
    ScreenId screenId = abstractDisplayController_->CreateVirtualScreen(virtualDisplayInfo, surface);
    return GetDisplayIdFromScreenId(screenId);
}

bool DisplayManagerService::DestroyVirtualDisplay(DisplayId displayId)
{
    WLOGFI("DisplayManagerService::DestroyVirtualDisplay");
    ScreenId screenId = GetScreenIdFromDisplayId(displayId);
    return abstractDisplayController_->DestroyVirtualScreen(screenId);
}

std::shared_ptr<Media::PixelMap> DisplayManagerService::GetDispalySnapshot(DisplayId displayId)
{
    ScreenId screenId = GetScreenIdFromDisplayId(displayId);
    std::shared_ptr<Media::PixelMap> screenSnapshot
        = abstractDisplayController_->GetScreenSnapshot(displayId, screenId);
    return screenSnapshot;
}

void DisplayManagerService::OnStop()
{
    WLOGFI("ready to stop display service.");
}

void DisplayManagerService::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayManagerAgentMap_[type].push_back(displayManagerAgent);
    WLOGFI("agent registered");
    if (dmAgentDeath_ == nullptr) {
        WLOGFI("death Recipient is nullptr");
        return;
    }
    if (!displayManagerAgent->AsObject()->AddDeathRecipient(dmAgentDeath_)) {
        WLOGFI("failed to add death recipient");
    }
}

void DisplayManagerService::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    if (displayManagerAgent == nullptr || displayManagerAgentMap_.count(type) == 0) {
        WLOGFE("displayManagerAgent invalid");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto& displayManagerAgents = displayManagerAgentMap_.at(type);
    UnregisterDisplayManagerAgent(displayManagerAgents, displayManagerAgent->AsObject());
}

bool DisplayManagerService::UnregisterDisplayManagerAgent(std::vector<sptr<IDisplayManagerAgent>>& displayManagerAgents,
    const sptr<IRemoteObject>& displayManagerAgent)
{
    auto iter = std::find_if(displayManagerAgents.begin(), displayManagerAgents.end(),
        finder_t(displayManagerAgent));
    if (iter == displayManagerAgents.end()) {
        WLOGFE("could not find this listener");
        return false;
    }
    displayManagerAgents.erase(iter);
    WLOGFI("agent unregistered");
    return true;
}

void DisplayManagerService::RemoveDisplayManagerAgent(const sptr<IRemoteObject>& remoteObject)
{
    WLOGFI("RemoveDisplayManagerAgent");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto& elem : displayManagerAgentMap_) {
        if (UnregisterDisplayManagerAgent(elem.second, remoteObject)) {
            break;
        }
    }
    remoteObject->RemoveDeathRecipient(dmAgentDeath_);
}

bool DisplayManagerService::NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status)
{
    if (displayManagerAgentMap_.count(DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER) == 0) {
        WLOGFI("no display power event agent registered!");
        return false;
    }
    WLOGFI("NotifyDisplayPowerEvent");
    for (auto& agent : displayManagerAgentMap_.at(DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER)) {
        agent->NotifyDisplayPowerEvent(event, status);
    }
    return true;
}

bool DisplayManagerService::WakeUpBegin(PowerStateChangeReason reason)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP, EventStatus::BEGIN);
}

bool DisplayManagerService::WakeUpEnd()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP, EventStatus::END);
}

bool DisplayManagerService::SuspendBegin(PowerStateChangeReason reason)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayPowerController_.SuspendBegin(reason);
    return NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP, EventStatus::BEGIN);
}

bool DisplayManagerService::SuspendEnd()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP, EventStatus::END);
}

bool DisplayManagerService::SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason)
{
    WLOGFI("SetScreenPowerForAll");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return NotifyDisplayPowerEvent(state == DisplayPowerState::POWER_ON ? DisplayPowerEvent::DISPLAY_ON :
        DisplayPowerEvent::DISPLAY_OFF, EventStatus::END);
}

bool DisplayManagerService::SetDisplayState(DisplayState state)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return displayPowerController_.SetDisplayState(state);
}

DisplayState DisplayManagerService::GetDisplayState(uint64_t displayId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return displayPowerController_.GetDisplayState(displayId);
}

void DisplayManagerService::NotifyDisplayEvent(DisplayEvent event)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayPowerController_.NotifyDisplayEvent(event);
}

sptr<AbstractScreenController> DisplayManagerService::GetAbstractScreenController()
{
    return abstractScreenController_;
}

void DMAgentDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& wptrDeath)
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
    WLOGFI("call OnRemoteDied callback");
    callback_(object);
}
} // namespace OHOS::Rosen