/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "display_manager_agent_controller.h"
#include "display_manager_config.h"
#include "dm_common.h"
#include "permission.h"
#include "transaction/rs_interfaces.h"
#include "window_manager_hilog.h"
#include "wm_trace.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerService"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(DisplayManagerService)
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<DisplayManagerService>());
float DisplayManagerService::customVirtualPixelRatio_ = -1.0f;

#define CHECK_SCREEN_AND_RETURN(ret) \
    do { \
        if (screenId == SCREEN_ID_INVALID) { \
            WLOGFE("screenId invalid"); \
            return ret; \
        } \
    } while (false)

DisplayManagerService::DisplayManagerService() : SystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID, true),
    abstractDisplayController_(new AbstractDisplayController(mutex_,
    std::bind(&DisplayManagerService::NotifyDisplayStateChange, this, std::placeholders::_1, std::placeholders::_2))),
    abstractScreenController_(new AbstractScreenController(mutex_)),
    displayPowerController_(new DisplayPowerController(mutex_,
    std::bind(&DisplayManagerService::NotifyDisplayStateChange, this, std::placeholders::_1, std::placeholders::_2)))
{
}

int DisplayManagerService::Dump(int fd, const std::vector<std::u16string>& args)
{
    if (displayDumper_ == nullptr) {
        displayDumper_ = new DisplayDumper(abstractDisplayController_, abstractScreenController_, mutex_);
    }
    return static_cast<int>(displayDumper_->Dump(fd, args));
}

void DisplayManagerService::OnStart()
{
    WLOGFI("DisplayManagerService::OnStart start");
    if (!Init()) {
        WLOGFW("Init failed");
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
    if (DisplayManagerConfig::LoadConfigXml(DISPLAY_MANAGER_CONFIG_XML)) {
        DisplayManagerConfig::DumpConfig();
        ConfigureDisplayManagerService();
    }
    abstractScreenController_->Init();
    abstractDisplayController_->Init(abstractScreenController_);
    WLOGFI("DisplayManagerService::Init success");
    return true;
}

void DisplayManagerService::ConfigureDisplayManagerService()
{
    auto numbersConfig = DisplayManagerConfig::GetIntNumbersConfig();
    if (numbersConfig.count("dpi") != 0) {
        uint32_t densityDpi = static_cast<uint32_t>(numbersConfig["dpi"][0]);
        if (densityDpi == 0) {
            WLOGI("No custom virtual pixel ratio value is configured, use default value instead");
            return;
        }
        if (densityDpi < DOT_PER_INCH_MINIMUM_VALUE || densityDpi > DOT_PER_INCH_MAXIMUM_VALUE) {
            WLOGE("Invalid input dpi value, the valid input range for DPI values is %{public}u ~ %{public}u",
                DOT_PER_INCH_MINIMUM_VALUE, DOT_PER_INCH_MAXIMUM_VALUE);
            return;
        }
        float virtualPixelRatio = static_cast<float>(densityDpi) / BASELINE_DENSITY;
        DisplayManagerService::customVirtualPixelRatio_ = virtualPixelRatio;
    }
}

void DisplayManagerService::RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener)
{
    displayChangeListener_ = listener;
    WLOGFI("IDisplayChangeListener registered");
}

void DisplayManagerService::NotifyDisplayStateChange(DisplayId id, DisplayStateChangeType type)
{
    WLOGFI("DisplayId %{public}" PRIu64"", id);
    if (displayChangeListener_ != nullptr) {
        displayChangeListener_->OnDisplayStateChange(id, type);
    }
}

sptr<DisplayInfo> DisplayManagerService::GetDefaultDisplayInfo()
{
    ScreenId dmsScreenId = abstractScreenController_->GetDefaultAbstractScreenId();
    WLOGFI("GetDefaultDisplayInfo %{public}" PRIu64"", dmsScreenId);
    sptr<AbstractDisplay> display = abstractDisplayController_->GetAbstractDisplayByScreen(dmsScreenId);
    if (display == nullptr) {
        WLOGFE("fail to get displayInfo by id: invalid display");
        return nullptr;
    }
    return display->ConvertToDisplayInfo();
}

sptr<DisplayInfo> DisplayManagerService::GetDisplayInfoById(DisplayId displayId)
{
    sptr<AbstractDisplay> display = abstractDisplayController_->GetAbstractDisplay(displayId);
    if (display == nullptr) {
        WLOGFE("fail to get displayInfo by id: invalid display");
        return nullptr;
    }
    return display->ConvertToDisplayInfo();
}

sptr<DisplayInfo> DisplayManagerService::GetDisplayInfoByScreen(ScreenId screenId)
{
    sptr<AbstractDisplay> display = abstractDisplayController_->GetAbstractDisplayByScreen(screenId);
    if (display == nullptr) {
        WLOGFE("fail to get displayInfo by screenId: invalid display");
        return nullptr;
    }
    return display->ConvertToDisplayInfo();
}

ScreenId DisplayManagerService::CreateVirtualScreen(VirtualScreenOption option,
    const sptr<IRemoteObject>& displayManagerAgent)
{
    WM_SCOPED_TRACE("dms:CreateVirtualScreen(%s)", option.name_.c_str());
    ScreenId screenId = abstractScreenController_->CreateVirtualScreen(option, displayManagerAgent);
    CHECK_SCREEN_AND_RETURN(SCREEN_ID_INVALID);
    accessTokenIdMaps_[screenId] = IPCSkeleton::GetCallingTokenID();
    WLOGFI("DumpScreenInfo after Create VirtualScreen");
    abstractScreenController_->DumpScreenInfo();
    return screenId;
}

DMError DisplayManagerService::DestroyVirtualScreen(ScreenId screenId)
{
    if (accessTokenIdMaps_[screenId] != IPCSkeleton::GetCallingTokenID()) {
        return DMError::DM_ERROR_INVALID_CALLING;
    }
    accessTokenIdMaps_.erase(screenId);
    WLOGFI("DestroyVirtualScreen::ScreenId: %{public}" PRIu64 "", screenId);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);

    WM_SCOPED_TRACE("dms:DestroyVirtualScreen(%" PRIu64")", screenId);
    return abstractScreenController_->DestroyVirtualScreen(screenId);
}

DMError DisplayManagerService::SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface)
{
    WLOGFI("SetVirtualScreenSurface::ScreenId: %{public}" PRIu64 "", screenId);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);

    WM_SCOPED_TRACE("dms:SetVirtualScreenSurface(%" PRIu64")", screenId);
    return abstractScreenController_->SetVirtualScreenSurface(screenId, surface);
}

bool DisplayManagerService::SetOrientation(ScreenId screenId, Orientation orientation)
{
    WM_SCOPED_TRACE("dms:SetOrientation(%" PRIu64")", screenId);
    return abstractScreenController_->SetOrientation(screenId, orientation, false);
}

bool DisplayManagerService::SetOrientationFromWindow(ScreenId screenId, Orientation orientation)
{
    WM_SCOPED_TRACE("dms:SetOrientationFromWindow(%" PRIu64")", screenId);
    return abstractScreenController_->SetOrientation(screenId, orientation, true);
}

std::shared_ptr<Media::PixelMap> DisplayManagerService::GetDisplaySnapshot(DisplayId displayId)
{
    WM_SCOPED_TRACE("dms:GetDisplaySnapshot(%" PRIu64")", displayId);
    if (Permission::CheckCallingPermission("ohos.permission.CAPTURE_SCREEN") ||
        Permission::IsStartByHdcd()) {
        return abstractDisplayController_->GetScreenSnapshot(displayId);
    }
    return nullptr;
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
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);

    WM_SCOPED_TRACE("dms:GetScreenSupportedColorGamuts(%" PRIu64")", screenId);

    return abstractScreenController_->GetScreenSupportedColorGamuts(screenId, colorGamuts);
}

DMError DisplayManagerService::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    WLOGFI("GetScreenColorGamut::ScreenId: %{public}" PRIu64 "", screenId);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);

    WM_SCOPED_TRACE("dms:GetScreenColorGamut(%" PRIu64")", screenId);

    return abstractScreenController_->GetScreenColorGamut(screenId, colorGamut);
}

DMError DisplayManagerService::SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx)
{
    WLOGFI("SetScreenColorGamut::ScreenId: %{public}" PRIu64 ", colorGamutIdx %{public}d", screenId, colorGamutIdx);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);

    WM_SCOPED_TRACE("dms:SetScreenColorGamut(%" PRIu64")", screenId);

    return abstractScreenController_->SetScreenColorGamut(screenId, colorGamutIdx);
}

DMError DisplayManagerService::GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap)
{
    WLOGFI("GetScreenGamutMap::ScreenId: %{public}" PRIu64 "", screenId);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);

    WM_SCOPED_TRACE("dms:GetScreenGamutMap(%" PRIu64")", screenId);

    return abstractScreenController_->GetScreenGamutMap(screenId, gamutMap);
}

DMError DisplayManagerService::SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap)
{
    WLOGFI("SetScreenGamutMap::ScreenId: %{public}" PRIu64 ", ScreenGamutMap %{public}u",
        screenId, static_cast<uint32_t>(gamutMap));
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);

    WM_SCOPED_TRACE("dms:SetScreenGamutMap(%" PRIu64")", screenId);

    return abstractScreenController_->SetScreenGamutMap(screenId, gamutMap);
}

DMError DisplayManagerService::SetScreenColorTransform(ScreenId screenId)
{
    WLOGFI("SetScreenColorTransform::ScreenId: %{public}" PRIu64 "", screenId);
    CHECK_SCREEN_AND_RETURN(DMError::DM_ERROR_INVALID_PARAM);

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
    if (!Permission::IsSystemCalling()) {
        WLOGFI("permission denied!");
        return false;
    }
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP,
        EventStatus::BEGIN);
}

bool DisplayManagerService::WakeUpEnd()
{
    if (!Permission::IsSystemCalling()) {
        WLOGFI("permission denied!");
        return false;
    }
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP,
        EventStatus::END);
}

bool DisplayManagerService::SuspendBegin(PowerStateChangeReason reason)
{
    WM_SCOPED_TRACE("dms:SuspendBegin(%u)", reason);
    if (!Permission::IsSystemCalling()) {
        WLOGFI("permission denied!");
        return false;
    }
    displayPowerController_->SuspendBegin(reason);
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP,
        EventStatus::BEGIN);
}

bool DisplayManagerService::SuspendEnd()
{
    if (!Permission::IsSystemCalling()) {
        WLOGFI("permission denied!");
        return false;
    }
    return DisplayManagerAgentController::GetInstance().NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP,
        EventStatus::END);
}

bool DisplayManagerService::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    WLOGFI("SetScreenPowerForAll");
    if (!Permission::IsSystemCalling()) {
        WLOGFI("permission denied!");
        return false;
    }
    return abstractScreenController_->SetScreenPowerForAll(state, reason);
}

ScreenPowerState DisplayManagerService::GetScreenPower(ScreenId dmsScreenId)
{
    return abstractScreenController_->GetScreenPower(dmsScreenId);
}

bool DisplayManagerService::SetDisplayState(DisplayState state)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFI("permission denied!");
        return false;
    }
    return displayPowerController_->SetDisplayState(state);
}

ScreenId DisplayManagerService::GetScreenIdByDisplayId(DisplayId displayId) const
{
    sptr<AbstractDisplay> abstractDisplay = abstractDisplayController_->GetAbstractDisplay(displayId);
    if (abstractDisplay == nullptr) {
        WLOGFE("GetScreenIdByDisplayId: GetAbstarctDisplay failed");
        return SCREEN_ID_INVALID;
    }
    return abstractDisplay->GetAbstractScreenId();
}

DisplayState DisplayManagerService::GetDisplayState(DisplayId displayId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    return displayPowerController_->GetDisplayState(displayId);
}

void DisplayManagerService::NotifyDisplayEvent(DisplayEvent event)
{
    displayPowerController_->NotifyDisplayEvent(event);
}

bool DisplayManagerService::SetFreeze(std::vector<DisplayId> displayIds, bool isFreeze)
{
    abstractDisplayController_->SetFreeze(displayIds, isFreeze);
    return true;
}

std::shared_ptr<RSDisplayNode> DisplayManagerService::GetRSDisplayNodeByDisplayId(DisplayId displayId) const
{
    ScreenId screenId = GetScreenIdByDisplayId(displayId);
    CHECK_SCREEN_AND_RETURN(nullptr);

    return abstractScreenController_->GetRSDisplayNodeByScreenId(screenId);
}

ScreenId DisplayManagerService::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds)
{
    WLOGFI("MakeMirror. mainScreenId :%{public}" PRIu64"", mainScreenId);
    auto shotScreenIds = abstractScreenController_->GetShotScreenIds(mirrorScreenIds);
    auto iter = std::find(shotScreenIds.begin(), shotScreenIds.end(), mainScreenId);
    if (iter != shotScreenIds.end()) {
        shotScreenIds.erase(iter);
    }
    auto allMirrorScreenIds = abstractScreenController_->GetAllExpandOrMirrorScreenIds(mirrorScreenIds);
    iter = std::find(allMirrorScreenIds.begin(), allMirrorScreenIds.end(), mainScreenId);
    if (iter != allMirrorScreenIds.end()) {
        allMirrorScreenIds.erase(iter);
    }
    if (mainScreenId == SCREEN_ID_INVALID || (shotScreenIds.empty() && allMirrorScreenIds.empty())) {
        WLOGFI("create mirror fail, screen is invalid. Screen :%{public}" PRIu64"", mainScreenId);
        return SCREEN_ID_INVALID;
    }
    abstractScreenController_->SetShotScreen(mainScreenId, shotScreenIds);
    WM_SCOPED_TRACE("dms:MakeMirror");
    if (!allMirrorScreenIds.empty() && !abstractScreenController_->MakeMirror(mainScreenId, allMirrorScreenIds)) {
        WLOGFE("make mirror failed.");
        return SCREEN_ID_INVALID;
    }
    abstractScreenController_->DumpScreenInfo();
    auto screen = abstractScreenController_->GetAbstractScreen(mainScreenId);
    if (screen == nullptr || abstractScreenController_->GetAbstractScreenGroup(screen->groupDmsId_) == nullptr) {
        WLOGFE("get screen group failed.");
        return SCREEN_ID_INVALID;
    }
    return screen->groupDmsId_;
}

void DisplayManagerService::RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens)
{
    abstractScreenController_->RemoveVirtualScreenFromGroup(screens);
}

void DisplayManagerService::UpdateRSTree(DisplayId displayId, std::shared_ptr<RSSurfaceNode>& surfaceNode,
    bool isAdd)
{
    WLOGI("UpdateRSTree");
    ScreenId screenId = GetScreenIdByDisplayId(displayId);
    CHECK_SCREEN_AND_RETURN();

    abstractScreenController_->UpdateRSTree(screenId, surfaceNode, isAdd);
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

ScreenId DisplayManagerService::GetScreenGroupIdByScreenId(ScreenId screenId)
{
    auto screen = abstractScreenController_->GetAbstractScreen(screenId);
    if (screen == nullptr) {
        WLOGE("cannot find screenInfo: %{public}" PRIu64"", screenId);
        return SCREEN_ID_INVALID;
    }
    return screen->GetScreenGroupId();
}

std::vector<DisplayId> DisplayManagerService::GetAllDisplayIds()
{
    return abstractDisplayController_->GetAllDisplayIds();
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

ScreenId DisplayManagerService::MakeExpand(std::vector<ScreenId> expandScreenIds, std::vector<Point> startPoints)
{
    WLOGI("MakeExpand");
    if (expandScreenIds.empty() || startPoints.empty() || expandScreenIds.size() != startPoints.size()) {
        WLOGFI("create expand fail, input params is invalid. "
            "screenId vector size :%{public}ud, startPoint vector size :%{public}ud",
            static_cast<uint32_t>(expandScreenIds.size()), static_cast<uint32_t>(startPoints.size()));
        return SCREEN_ID_INVALID;
    }
    ScreenId defaultScreenId = abstractScreenController_->GetDefaultAbstractScreenId();
    WLOGI("MakeExpand, defaultScreenId:%{public}" PRIu64"", defaultScreenId);
    auto shotScreenIds = abstractScreenController_->GetShotScreenIds(expandScreenIds);
    auto iter = std::find(shotScreenIds.begin(), shotScreenIds.end(), defaultScreenId);
    if (iter != shotScreenIds.end()) {
        shotScreenIds.erase(iter);
    }
    auto allExpandScreenIds = abstractScreenController_->GetAllExpandOrMirrorScreenIds(expandScreenIds);
    iter = std::find(allExpandScreenIds.begin(), allExpandScreenIds.end(), defaultScreenId);
    auto startPointIter = iter - allExpandScreenIds.begin() + startPoints.begin();
    if (iter != allExpandScreenIds.end()) {
        allExpandScreenIds.erase(iter);
    }
    if (allExpandScreenIds.empty()) {
        WLOGFE("allExpandScreenIds is empty. make expand failed.");
        return SCREEN_ID_INVALID;
    }
    std::shared_ptr<RSDisplayNode> rsDisplayNode;
    for (uint32_t i = 0; i < allExpandScreenIds.size(); i++) {
        rsDisplayNode = abstractScreenController_->GetRSDisplayNodeByScreenId(allExpandScreenIds[i]);
        if (rsDisplayNode != nullptr) {
            rsDisplayNode->SetDisplayOffset(startPoints[i].posX_, startPoints[i].posY_);
        }
    }
    if (startPointIter != startPoints.end()) {
        startPoints.erase(startPointIter);
    }
    abstractScreenController_->SetShotScreen(defaultScreenId, shotScreenIds);
    WM_SCOPED_TRACE("dms:MakeExpand");
    if (!allExpandScreenIds.empty() && !abstractScreenController_->MakeExpand(allExpandScreenIds, startPoints)) {
        WLOGFE("make expand failed.");
        return SCREEN_ID_INVALID;
    }
    abstractScreenController_->DumpScreenInfo();
    auto screen = abstractScreenController_->GetAbstractScreen(allExpandScreenIds[0]);
    if (screen == nullptr || abstractScreenController_->GetAbstractScreenGroup(screen->groupDmsId_) == nullptr) {
        WLOGFE("get screen group failed.");
        return SCREEN_ID_INVALID;
    }
    return screen->groupDmsId_;
}

bool DisplayManagerService::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    WM_SCOPED_TRACE("dms:SetScreenActiveMode(%" PRIu64", %u)", screenId, modeId);
    return abstractScreenController_->SetScreenActiveMode(screenId, modeId);
}

bool DisplayManagerService::SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio)
{
    WM_SCOPED_TRACE("dms:SetVirtualPixelRatio(%" PRIu64", %f)", screenId, virtualPixelRatio);
    return abstractScreenController_->SetVirtualPixelRatio(screenId, virtualPixelRatio);
}

float DisplayManagerService::GetCustomVirtualPixelRatio()
{
    return DisplayManagerService::customVirtualPixelRatio_;
}
} // namespace OHOS::Rosen