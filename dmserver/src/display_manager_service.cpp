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
#include "window_manager_service.h"

#include <cinttypes>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "display_manager_agent_controller.h"
#include "window_manager_hilog.h"

#include "transaction/rs_interfaces.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManagerService"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerService)
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

ScreenId DisplayManagerService::CreateVirtualScreen(VirtualScreenOption option)
{
    ScreenId screenId = abstractScreenController_->CreateVirtualScreen(option);
    if (screenId == SCREEN_ID_INVALD) {
        WLOGFE("DisplayManagerService::CreateVirtualScreen: Get virtualScreenId failed");
        return SCREEN_ID_INVALD;
    }
    return screenId;
}

DMError DisplayManagerService::DestroyVirtualScreen(ScreenId screenId)
{
    WLOGFI("DisplayManagerService::DestroyVirtualScreen");
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: virtualScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    std::map<ScreenId, std::shared_ptr<RSDisplayNode>>::iterator iter = displayNodeMap_.find(screenId);
    if (iter == displayNodeMap_.end()) {
        WLOGFE("DisplayManagerService: displayNode is nullptr");
        return abstractScreenController_->DestroyVirtualScreen(screenId);
    }
    displayNodeMap_[screenId]->RemoveFromTree();
    displayNodeMap_.erase(screenId);
    return abstractScreenController_->DestroyVirtualScreen(screenId);
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
    DisplayManagerAgentController::GetInstance().RegisterDisplayManagerAgent(displayManagerAgent, type);
}

void DisplayManagerService::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return;
    }
    DisplayManagerAgentController::GetInstance().UnregisterDisplayManagerAgent(displayManagerAgent, type);
}

bool DisplayManagerService::WakeUpBegin(PowerStateChangeReason reason)
{
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP,
        EventStatus::BEGIN);
}

bool DisplayManagerService::WakeUpEnd()
{
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP,
        EventStatus::END);
}

bool DisplayManagerService::SuspendBegin(PowerStateChangeReason reason)
{
    displayPowerController_.SuspendBegin(reason);
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP,
        EventStatus::BEGIN);
}

bool DisplayManagerService::SuspendEnd()
{
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP,
        EventStatus::END);
}

bool DisplayManagerService::SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason)
{
    WLOGFI("SetScreenPowerForAll");
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(
        state == DisplayPowerState::POWER_ON ? DisplayPowerEvent::DISPLAY_ON :
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

DMError DisplayManagerService::AddMirror(ScreenId mainScreenId, ScreenId mirrorScreenId)
{
    if (mainScreenId == SCREEN_ID_INVALID) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    std::shared_ptr<RSDisplayNode> displayNode =
        SingletonContainer::Get<WindowManagerService>().GetDisplayNode(mainScreenId);
    if (displayNode == nullptr) {
        WLOGFE("DisplayManagerService::AddMirror: GetDisplayNode failed, displayNode is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    NodeId nodeId = displayNode->GetId();

    struct RSDisplayNodeConfig config = {mirrorScreenId, true, nodeId};
    displayNodeMap_[mainScreenId] = RSDisplayNode::Create(config);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    transactionProxy->FlushImplicitTransaction();
    WLOGFI("DisplayManagerService::AddMirror: NodeId: %{public}" PRIu64 "", nodeId >> 32);
    return DMError::DM_OK;
}
} // namespace OHOS::Rosen