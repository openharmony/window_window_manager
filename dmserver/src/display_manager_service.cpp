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
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "display_manager_agent_controller.h"
#include "transaction/rs_interfaces.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerService"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerService)
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<DisplayManagerService>());

DisplayManagerService::DisplayManagerService() : SystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID, true),
    abstractDisplayController_(new AbstractDisplayController(mutex_)),
    abstractScreenController_(new AbstractScreenController(mutex_))
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

void DisplayManagerService::RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener)
{
    displayChangeListener_ = listener;
    WLOGFI("IDisplayChangeListener registered");
}

void DisplayManagerService::NotifyDisplayStateChange(DisplayId id, DisplayStateChangeType type)
{
    if (displayChangeListener_ != nullptr) {
        displayChangeListener_->OnDisplayStateChange(id, type);
    }
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

sptr<AbstractDisplay> DisplayManagerService::GetAbstractDisplay(DisplayId displayId)
{
    return abstractDisplayController_->GetAbstractDisplay(displayId);
}
 
ScreenId DisplayManagerService::CreateVirtualScreen(VirtualScreenOption option)
{
    WM_SCOPED_TRACE("dms:CreateVirtualScreen(%s)", option.name_.c_str());
    WLOGFI("DumpScreenInfo before Create VirtualScreen");
    abstractScreenController_->DumpScreenInfo();
    ScreenId screenId = abstractScreenController_->CreateVirtualScreen(option);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService::CreateVirtualScreen: Get virtualScreenId failed");
        return SCREEN_ID_INVALID;
    }
    WLOGFI("DumpScreenInfo after Create VirtualScreen");
    abstractScreenController_->DumpScreenInfo();
    return screenId;
}

DMError DisplayManagerService::DestroyVirtualScreen(ScreenId screenId)
{
    WLOGFI("DestroyVirtualScreen::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: virtualScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:DestroyVirtualScreen(%" PRIu64")", screenId);
    std::map<ScreenId, std::shared_ptr<RSDisplayNode>>::iterator iter = displayNodeMap_.find(screenId);
    if (iter == displayNodeMap_.end()) {
        WLOGFE("DisplayManagerService: displayNode is nullptr");
        return abstractScreenController_->DestroyVirtualScreen(screenId);
    }
    displayNodeMap_[screenId]->RemoveFromTree();
    WLOGFE("DisplayManagerService: displayNode remove from tree");
    displayNodeMap_.erase(screenId);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
    return abstractScreenController_->DestroyVirtualScreen(screenId);
}

bool DisplayManagerService::RequestRotation(ScreenId screenId, Rotation rotation)
{
    WM_SCOPED_TRACE("dms:RequestRotation(%" PRIu64")", screenId);
    return abstractScreenController_->RequestRotation(screenId, rotation);
}

std::shared_ptr<Media::PixelMap> DisplayManagerService::GetDispalySnapshot(DisplayId displayId)
{
    WM_SCOPED_TRACE("dms:GetDispalySnapshot(%" PRIu64")", displayId);
    std::shared_ptr<Media::PixelMap> screenSnapshot
        = abstractDisplayController_->GetScreenSnapshot(displayId);
    return screenSnapshot;
}

ScreenId DisplayManagerService::GetRSScreenId(DisplayId displayId) const
{
    ScreenId dmsScreenId = GetScreenIdByDisplayId(displayId);
    return abstractScreenController_->ConvertToRsScreenId(dmsScreenId);
}

DMError DisplayManagerService::GetScreenSupportedColorGamuts(ScreenId screenId,
    std::vector<ScreenColorGamut>& colorGamuts)
{
    WLOGFI("GetScreenSupportedColorGamuts::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: ScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:GetScreenSupportedColorGamuts(%" PRIu64")", screenId);

    return abstractScreenController_->GetScreenSupportedColorGamuts(screenId, colorGamuts);
}

DMError DisplayManagerService::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    WLOGFI("GetScreenColorGamut::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: ScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:GetScreenColorGamut(%" PRIu64")", screenId);

    return abstractScreenController_->GetScreenColorGamut(screenId, colorGamut);
}

DMError DisplayManagerService::SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx)
{
    WLOGFI("SetScreenColorGamut::ScreenId: %{public}" PRIu64 ", colorGamutIdx %{public}d", screenId, colorGamutIdx);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: ScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:SetScreenColorGamut(%" PRIu64")", screenId);

    return abstractScreenController_->SetScreenColorGamut(screenId, colorGamutIdx);
}

DMError DisplayManagerService::GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap)
{
    WLOGFI("GetScreenGamutMap::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: ScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:GetScreenGamutMap(%" PRIu64")", screenId);

    return abstractScreenController_->GetScreenGamutMap(screenId, gamutMap);
}

DMError DisplayManagerService::SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap)
{
    WLOGFI("SetScreenGamutMap::ScreenId: %{public}" PRIu64 ", ScreenGamutMap %{public}u",
        screenId, static_cast<uint32_t>(gamutMap));
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: ScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:SetScreenGamutMap(%" PRIu64")", screenId);

    return abstractScreenController_->SetScreenGamutMap(screenId, gamutMap);
}

DMError DisplayManagerService::SetScreenColorTransform(ScreenId screenId)
{
    WLOGFI("SetScreenColorTransform::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService: ScreenId is invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:SetScreenColorTransform(%" PRIu64")", screenId);

    return abstractScreenController_->SetScreenColorTransform(screenId);
}


void DisplayManagerService::OnStop()
{
    WLOGFI("ready to stop display service.");
}

bool DisplayManagerService::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return false;
    }
    return DisplayManagerAgentController::GetInstance().RegisterDisplayManagerAgent(displayManagerAgent, type);
}

bool DisplayManagerService::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return false;
    }
    return DisplayManagerAgentController::GetInstance().UnregisterDisplayManagerAgent(displayManagerAgent, type);
}

bool DisplayManagerService::WakeUpBegin(PowerStateChangeReason reason)
{
    WM_SCOPED_TRACE("dms:WakeUpBegin(%u)", reason);
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
    WM_SCOPED_TRACE("dms:SuspendBegin(%u)", reason);
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

ScreenId DisplayManagerService::GetScreenIdByDisplayId(DisplayId displayId) const
{
    sptr<AbstractDisplay> abstractDisplay = abstractDisplayController_->GetAbstractDisplay(displayId);
    if (abstractDisplay == nullptr) {
        WLOGFE("DisplayManagerService::GetScreenIdByDisplayId: GetAbstarctDisplay failed");
        return SCREEN_ID_INVALID;
    }
    ScreenId dmsScreenId = abstractDisplay->GetAbstractScreenId();
    return dmsScreenId;
}

sptr<AbstractDisplay> DisplayManagerService::GetDisplayByDisplayId(DisplayId displayId) const
{
    sptr<AbstractDisplay> abstractDisplay = abstractDisplayController_->GetAbstractDisplay(displayId);
    return abstractDisplay;
}

sptr<AbstractScreenController> DisplayManagerService::GetAbstractScreenController()
{
    return abstractScreenController_;
}

DisplayState DisplayManagerService::GetDisplayState(DisplayId displayId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return displayPowerController_.GetDisplayState(displayId);
}

void DisplayManagerService::NotifyDisplayEvent(DisplayEvent event)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    displayPowerController_.NotifyDisplayEvent(event);
}


std::shared_ptr<RSDisplayNode> DisplayManagerService::GetRSDisplayNodeByDisplayId(DisplayId displayId) const
{
    ScreenId dmsScreenId = GetScreenIdByDisplayId(displayId);
    if (dmsScreenId == SCREEN_ID_INVALID) {
        WLOGFE("DisplayManagerService::GetRSDisplayNodeByDisplayId: ScreenId invalid");
        return nullptr;
    }
    return abstractScreenController_->GetRSDisplayNodeByScreenId(dmsScreenId);
}

DMError DisplayManagerService::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds)
{
    WLOGFI("MakeMirror. mainScreenId :%{public}" PRIu64"", mainScreenId);
    abstractScreenController_->DumpScreenInfo();
    auto shotScreenIds = abstractScreenController_->GetShotScreenIds(mirrorScreenIds);
    auto iter = std::find(shotScreenIds.begin(), shotScreenIds.end(), mainScreenId);
    if (iter != shotScreenIds.end()) {
        shotScreenIds.erase(iter);
    }
    auto allMirrorScreenIds = abstractScreenController_->GetAllMirrorScreenIds(mirrorScreenIds);
    iter = std::find(allMirrorScreenIds.begin(), allMirrorScreenIds.end(), mainScreenId);
    if (iter != allMirrorScreenIds.end()) {
        allMirrorScreenIds.erase(iter);
    }
    if (mainScreenId == SCREEN_ID_INVALID || (shotScreenIds.empty() && allMirrorScreenIds.empty())) {
        WLOGFI("create mirror fail, screen is invalid. Screen :%{public}" PRIu64"", mainScreenId);
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    WM_SCOPED_TRACE("dms:MakeMirror");
    WLOGFI("create mirror. Screen: %{public}" PRIu64"", mainScreenId);
    std::shared_ptr<RSDisplayNode> displayNode = abstractScreenController_->GetRSDisplayNodeByScreenId(mainScreenId);
    if (displayNode == nullptr) {
        WLOGFE("create mirror fail, cannot get DisplayNode");
        return DMError::DM_ERROR_NULLPTR;
    }
    NodeId nodeId = displayNode->GetId();
    WLOGI("MakeMirror, nodeId:%{public}" PRIu64"", nodeId);
    for (ScreenId mirrorScreenId : shotScreenIds) {
        mirrorScreenId = abstractScreenController_->ConvertToRsScreenId(mirrorScreenId);
        if (mirrorScreenId == INVALID_SCREEN_ID) {
            continue;
        }
        struct RSDisplayNodeConfig config = {mirrorScreenId, true, nodeId};
        displayNodeMap_[mirrorScreenId] = RSDisplayNode::Create(config);
    }
    auto transactionProxy = RSTransactionProxy::GetInstance();
    transactionProxy->FlushImplicitTransaction();

    if (!allMirrorScreenIds.empty() && !abstractScreenController_->MakeMirror(mainScreenId, allMirrorScreenIds)) {
        WLOGFE("make mirror failed.");
        return DMError::DM_ERROR_NULLPTR;
    }
    WLOGFI("create mirror. NodeId: %{public}" PRIu64"", nodeId);
    abstractScreenController_->DumpScreenInfo();
    return DMError::DM_OK;
}

void DisplayManagerService::UpdateRSTree(DisplayId displayId, std::shared_ptr<RSSurfaceNode>& surfaceNode,
    bool isAdd)
{
    WLOGI("DisplayManagerService::UpdateRSTree");
    ScreenId dmsScreenId = GetScreenIdByDisplayId(displayId);
    if (dmsScreenId == SCREEN_ID_INVALID) {
        return;
    }
    abstractScreenController_->UpdateRSTree(dmsScreenId, surfaceNode, isAdd);
}

sptr<ScreenInfo> DisplayManagerService::GetScreenInfoById(ScreenId screenId)
{
    auto screen = abstractScreenController_->GetAbstractScreen(screenId);
    if (screen == nullptr) {
        WLOGE("cannot find screenInfo: %{public}" PRIu64"", screenId);
        return nullptr;
    }
    return screen->ConvertToScreenInfo();
}

sptr<ScreenGroupInfo> DisplayManagerService::GetScreenGroupInfoById(ScreenId screenId)
{
    auto screenGroup = abstractScreenController_->GetAbstractScreenGroup(screenId);
    if (screenGroup == nullptr) {
        WLOGE("cannot find screenGroupInfo: %{public}" PRIu64"", screenId);
        return nullptr;
    }
    return screenGroup->ConvertToScreenGroupInfo();
}

std::vector<sptr<ScreenInfo>> DisplayManagerService::GetAllScreenInfos()
{
    std::vector<ScreenId> screenIds = abstractScreenController_->GetAllScreenIds();
    std::vector<sptr<ScreenInfo>> screenInfos;
    for (auto screenId: screenIds) {
        auto screenInfo = GetScreenInfoById(screenId);
        if (screenInfo == nullptr) {
            WLOGE("cannot find screenInfo: %{public}" PRIu64"", screenId);
            continue;
        }
        screenInfos.emplace_back(screenInfo);
    }
    return screenInfos;
}

DMError DisplayManagerService::MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint)
{
    // todo: make expand
    return DMError::DM_OK;
}

bool DisplayManagerService::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    return abstractScreenController_->SetScreenActiveMode(screenId, modeId);
}
} // namespace OHOS::Rosen