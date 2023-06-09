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

#include "session_manager/include/screen_session_manager.h"

#include <transaction/rs_interfaces.h>
#include "window_manager_hilog.h"
#include "screen_session_manager.h"
#include "screen_scene_config.h"
#include "permission.h"
#include <parameters.h>
#include "sys_cap_util.h"
#include "surface_capture_future.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSessionManager" };
const std::string SCREEN_SESSION_MANAGER_THREAD = "ScreenSessionManager";
const std::string SCREEN_CAPTURE_PERMISSION = "ohos.permission.CAPTURE_SCREEN";
const std::string CONTROLLER_THREAD_ID = "ScreenSessionManagerThread";
} // namespace

ScreenSessionManager& ScreenSessionManager::GetInstance()
{
    static ScreenSessionManager screenSessionManager;
    return screenSessionManager;
}

ScreenSessionManager::ScreenSessionManager() : rsInterface_(RSInterfaces::GetInstance()),
    sessionDisplayPowerController_(new SessionDisplayPowerController(
        std::bind(&ScreenSessionManager::NotifyDisplayStateChange, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)))
{
    Init();
}

void ScreenSessionManager::RegisterScreenConnectionListener(sptr<IScreenConnectionListener>& screenConnectionListener)
{
    if (screenConnectionListener == nullptr) {
        WLOGFE("Failed to register screen connection callback, callback is null!");
        return;
    }

    if (std::find(screenConnectionListenerList_.begin(), screenConnectionListenerList_.end(),
        screenConnectionListener) != screenConnectionListenerList_.end()) {
        WLOGFE("Repeat to register screen connection callback!");
        return;
    }

    screenConnectionListenerList_.emplace_back(screenConnectionListener);
    for (auto sessionIt : screenSessionMap_) {
        screenConnectionListener->OnScreenConnect(sessionIt.second);
    }
}

void ScreenSessionManager::UnregisterScreenConnectionListener(sptr<IScreenConnectionListener>& screenConnectionListener)
{
    if (screenConnectionListener == nullptr) {
        WLOGFE("Failed to unregister screen connection listener, listener is null!");
        return;
    }

    screenConnectionListenerList_.erase(
        std::remove_if(screenConnectionListenerList_.begin(), screenConnectionListenerList_.end(),
            [screenConnectionListener](
                sptr<IScreenConnectionListener> listener) { return screenConnectionListener == listener; }),
        screenConnectionListenerList_.end());
}

DMError ScreenSessionManager::RegisterDisplayManagerAgent(
    const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type)
{
    if (type == DisplayManagerAgentType::SCREEN_EVENT_LISTENER && !Permission::IsSystemCalling()
        && !Permission::IsStartByHdcd()) {
        WLOGFE("register display manager agent permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return DMError::DM_ERROR_NULLPTR;
    }

    return dmAgentContainer_.RegisterAgent(displayManagerAgent, type) ? DMError::DM_OK :DMError::DM_ERROR_NULLPTR;
}

DMError ScreenSessionManager::UnregisterDisplayManagerAgent(
    const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type)
{
    if (type == DisplayManagerAgentType::SCREEN_EVENT_LISTENER && !Permission::IsSystemCalling()
        && !Permission::IsStartByHdcd()) {
        WLOGFE("unregister display manager agent permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return DMError::DM_ERROR_NULLPTR;
    }

    return dmAgentContainer_.UnregisterAgent(displayManagerAgent, type) ? DMError::DM_OK :DMError::DM_ERROR_NULLPTR;
}

void ScreenSessionManager::Init()
{
    msgScheduler_ = std::make_shared<MessageScheduler>(SCREEN_SESSION_MANAGER_THREAD);
    RegisterScreenChangeListener();
    auto runner = AppExecFwk::EventRunner::Create(CONTROLLER_THREAD_ID);
    controllerHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    LoadScreenSceneXml();
}

void ScreenSessionManager::LoadScreenSceneXml()
{
    WLOGFI("ScreenSession load screen scene xml");
    if (ScreenSceneConfig::LoadConfigXml()) {
        ScreenSceneConfig::DumpConfig();
        ConfigureScreenScene();
    }
}

void ScreenSessionManager::ConfigureScreenScene()
{
    auto numbersConfig = ScreenSceneConfig::GetIntNumbersConfig();
    auto enableConfig = ScreenSceneConfig::GetEnableConfig();
    auto stringConfig = ScreenSceneConfig::GetStringConfig();
    if (numbersConfig.count("defaultDeviceRotationOffset") != 0) {
        uint32_t defaultDeviceRotationOffset = static_cast<uint32_t>(numbersConfig["defaultDeviceRotationOffset"][0]);
        WLOGFD("defaultDeviceRotationOffset = %u", defaultDeviceRotationOffset);
    }
    if (enableConfig.count("isWaterfallDisplay") != 0) {
        bool isWaterfallDisplay = static_cast<bool>(enableConfig["isWaterfallDisplay"]);
        WLOGFD("isWaterfallDisplay = %d", isWaterfallDisplay);
    }
    if (numbersConfig.count("curvedScreenBoundary") != 0) {
        std::vector<int> vtBoundary = static_cast<std::vector<int>>(numbersConfig["curvedScreenBoundary"]);
        WLOGFD("vtBoundary = %u", vtBoundary.size());
    }
    if (stringConfig.count("defaultDisplayCutoutPath") != 0) {
        std::string defaultDisplayCutoutPath = static_cast<std::string>(stringConfig["defaultDisplayCutoutPath"]);
        WLOGFD("defaultDisplayCutoutPath = %s.", defaultDisplayCutoutPath.c_str());
    }
    ConfigureWaterfallDisplayCompressionParams();

    if (numbersConfig.count("buildInDefaultOrientation") != 0) {
        Orientation orientation = static_cast<Orientation>(numbersConfig["buildInDefaultOrientation"][0]);
        WLOGFD("orientation = %d", orientation);
    }
}

void ScreenSessionManager::ConfigureWaterfallDisplayCompressionParams()
{
    auto numbersConfig = ScreenSceneConfig::GetIntNumbersConfig();
    auto enableConfig = ScreenSceneConfig::GetEnableConfig();
    if (enableConfig.count("isWaterfallAreaCompressionEnableWhenHorizontal") != 0) {
        bool enable = static_cast<bool>(enableConfig["isWaterfallAreaCompressionEnableWhenHorizontal"]);
        WLOGD("isWaterfallAreaCompressionEnableWhenHorizontal=%d.", enable);
    }
    if (numbersConfig.count("waterfallAreaCompressionSizeWhenHorzontal") != 0) {
        uint32_t uSize = static_cast<uint32_t>(numbersConfig["waterfallAreaCompressionSizeWhenHorzontal"][0]);
        WLOGD("waterfallAreaCompressionSizeWhenHorzontal =%u.", uSize);
    }
}

void ScreenSessionManager::RegisterScreenChangeListener()
{
    WLOGFD("Register screen change listener.");
    auto res = rsInterface_.SetScreenChangeCallback(
        [this](ScreenId screenId, ScreenEvent screenEvent) { OnScreenChange(screenId, screenEvent); });
    if (res != StatusCode::SUCCESS) {
        auto task = [this]() { RegisterScreenChangeListener(); };
        WS_CHECK_NULL_SCHE_VOID(msgScheduler_, task);
        msgScheduler_->PostAsyncTask(task, 50); // Retry after 50 ms.
    }
}

void ScreenSessionManager::OnScreenChange(ScreenId screenId, ScreenEvent screenEvent)
{
    WLOGFI("SCB: On screen change. ScreenId: %{public}" PRIu64 ", ScreenEvent: %{public}d", screenId,
        static_cast<int>(screenEvent));
    auto screenSession = GetOrCreateScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is nullptr");
        return;
    }
    if (screenEvent == ScreenEvent::CONNECTED) {
        for (auto listener : screenConnectionListenerList_) {
            listener->OnScreenConnect(screenSession);
        }
        screenSession->Connect();
    } else if (screenEvent == ScreenEvent::DISCONNECTED) {
        screenSession->Disconnect();
        for (auto listener : screenConnectionListenerList_) {
            listener->OnScreenDisconnect(screenSession);
        }
        WLOGFI("SCB: OnScreenChange. screenSessionMap_ delete ScreenId: %{public}" PRIu64 " ", screenId);
        screenSessionMap_.erase(screenId);
    } else {
        WLOGE("Unknown ScreenEvent: %{public}d", static_cast<int>(screenEvent));
    }
}

sptr<ScreenSession> ScreenSessionManager::GetScreenSession(ScreenId screenId) const
{
    auto iter = screenSessionMap_.find(screenId);
    if (iter == screenSessionMap_.end()) {
        WLOGFE("Error found screen session with id: %{public}" PRIu64, screenId);
        return nullptr;
    }
    return iter->second;
}

sptr<DisplayInfo> ScreenSessionManager::GetDefaultDisplayInfo()
{
    GetDefaultScreenId();
    sptr<ScreenSession> screenSession = GetScreenSession(defaultScreenId_);
    if (screenSession) {
        return screenSession->ConvertToDisplayInfo();
    } else {
        WLOGFE("Get default screen session failed.");
        return nullptr;
    }
}

sptr<DisplayInfo> ScreenSessionManager::GetDisplayInfoById(DisplayId displayId)
{
    for (auto sessionIt : screenSessionMap_) {
        auto screenSession = sessionIt.second;
        sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
        if (displayId == displayInfo->GetDisplayId()) {
            return displayInfo;
        }
    }
    WLOGFE("SCB: ScreenSessionManager::GetDisplayInfoById failed.");
    return nullptr;
}

sptr<ScreenInfo> ScreenSessionManager::GetScreenInfoById(ScreenId screenId)
{
    auto screenSession = GetOrCreateScreenSession(screenId);
    if (screenSession == nullptr) {
        WLOGE("SCB: ScreenSessionManager::GetScreenInfoById cannot find screenInfo: %{public}" PRIu64"", screenId);
        return nullptr;
    }
    return screenSession->ConvertToScreenInfo();
}

sptr<ScreenSession> ScreenSessionManager::GetOrCreateScreenSession(ScreenId screenId)
{
    WLOGFE("SCB: ScreenSessionManager::GetOrCreateScreenSession ENTER");
    auto sessionIt = screenSessionMap_.find(screenId);
    if (sessionIt != screenSessionMap_.end()) {
        return sessionIt->second;
    }

    ScreenId rsId = rsInterface_.GetDefaultScreenId();
    screenIdManager_.rs2SmsScreenIdMap_[rsId] = screenId;
    screenIdManager_.sms2RsScreenIdMap_[screenId] = rsId;

    auto screenMode = rsInterface_.GetScreenActiveMode(screenId);
    auto screenBounds = RRect({ 0, 0, screenMode.GetScreenWidth(), screenMode.GetScreenHeight() }, 0.0f, 0.0f);
    ScreenProperty property;
    property.SetRotation(0.0f);
    property.SetBounds(screenBounds);
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession(screenId, property);
    if (!session) {
        WLOGFE("screen session is nullptr");
        return session;
    }

    ScreenId smsGroupScreenId = 1;
    std::ostringstream buffer;
    buffer<<"ScreenGroup_"<<smsGroupScreenId;
    std::string name = buffer.str();
    isExpandCombination_ = system::GetParameter("persist.display.expand.enabled", "0") == "1";
    sptr<ScreenSessionGroup> screenGroup;
    if (isExpandCombination_) {
        screenGroup = new(std::nothrow) ScreenSessionGroup(smsGroupScreenId,
            SCREEN_ID_INVALID, name, ScreenCombination::SCREEN_EXPAND);
    } else {
        screenGroup = new(std::nothrow) ScreenSessionGroup(smsGroupScreenId,
            SCREEN_ID_INVALID, name, ScreenCombination::SCREEN_MIRROR);
    }
    screenGroup->groupSmsId_ = 1;
    smsScreenGroupMap_.insert(std::make_pair(smsGroupScreenId, screenGroup));
    session->groupSmsId_ = 1;

    screenSessionMap_[screenId] = session;
    return session;
}

ScreenId ScreenSessionManager::GetDefaultScreenId()
{
    if (defaultScreenId_ == INVALID_SCREEN_ID) {
        defaultScreenId_ = rsInterface_.GetDefaultScreenId();
    }
    return defaultScreenId_;
}

bool ScreenSessionManager::WakeUpBegin(PowerStateChangeReason reason)
{
    return NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP, EventStatus::BEGIN);
}

bool ScreenSessionManager::WakeUpEnd()
{
    return NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP, EventStatus::END);
}

bool ScreenSessionManager::SuspendBegin(PowerStateChangeReason reason)
{
    sessionDisplayPowerController_->SuspendBegin(reason);
    return NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP, EventStatus::BEGIN);
}

bool ScreenSessionManager::SuspendEnd()
{
    return NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP, EventStatus::END);
}

bool ScreenSessionManager::SetDisplayState(DisplayState state)
{
    return sessionDisplayPowerController_->SetDisplayState(state);
}

void ScreenSessionManager::NotifyDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    if (displayChangeListener_ != nullptr) {
        displayChangeListener_->OnDisplayStateChange(defaultDisplayId, displayInfo, displayInfoMap, type);
    }
}

bool ScreenSessionManager::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    auto screenIds = GetAllScreenIds();
    if (screenIds.empty()) {
        WLOGFE("no screen info");
        return false;
    }

    ScreenPowerStatus status;
    switch (state) {
        case ScreenPowerState::POWER_ON: {
            status = ScreenPowerStatus::POWER_STATUS_ON;
            break;
        }
        case ScreenPowerState::POWER_OFF: {
            status = ScreenPowerStatus::POWER_STATUS_OFF;
            break;
        }
        default: {
            WLOGFW("SetScreenPowerStatus state not support");
            return false;
        }
    }

    for (auto screenId : screenIds) {
        rsInterface_.SetScreenPowerStatus(screenId, status);
    }

    return NotifyDisplayPowerEvent(state == ScreenPowerState::POWER_ON ? DisplayPowerEvent::DISPLAY_ON :
        DisplayPowerEvent::DISPLAY_OFF, EventStatus::END);
}

std::vector<ScreenId> ScreenSessionManager::GetAllScreenIds()
{
    std::vector<ScreenId> res;
    for (const auto& iter : screenSessionMap_) {
        res.emplace_back(iter.first);
    }
    return res;
}

DisplayState ScreenSessionManager::GetDisplayState(DisplayId displayId)
{
    return sessionDisplayPowerController_->GetDisplayState(displayId);
}

void ScreenSessionManager::NotifyDisplayEvent(DisplayEvent event)
{
    sessionDisplayPowerController_->NotifyDisplayEvent(event);
}

ScreenPowerState ScreenSessionManager::GetScreenPower(ScreenId screenId)
{
    if (screenSessionMap_.find(screenId) == screenSessionMap_.end()) {
        WLOGFE("cannot find screen %{public}" PRIu64"", screenId);
        return ScreenPowerState::INVALID_STATE;
    }

    auto state = static_cast<ScreenPowerState>(RSInterfaces::GetInstance().GetScreenPowerStatus(screenId));
    WLOGFI("GetScreenPower:%{public}u, rsscreen:%{public}" PRIu64".", state, screenId);
    return state;
}

void ScreenSessionManager::RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener)
{
    displayChangeListener_ = listener;
    WLOGFD("IDisplayChangeListener registered");
}

bool ScreenSessionManager::NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER);
    if (agents.empty()) {
        return false;
    }
    WLOGFI("NotifyDisplayPowerEvent");
    for (auto& agent : agents) {
        agent->NotifyDisplayPowerEvent(event, status);
    }
    return true;
}

bool ScreenSessionManager::NotifyDisplayStateChanged(DisplayId id, DisplayState state)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_STATE_LISTENER);
    if (agents.empty()) {
        return false;
    }
    WLOGFI("NotifyDisplayStateChanged");
    for (auto& agent : agents) {
        agent->NotifyDisplayStateChanged(id, state);
    }
    return true;
}
DMError ScreenSessionManager::GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos)
{
    return DMError::DM_OK;
}

std::vector<ScreenId> ScreenSessionManager::GetAllScreenIds() const
{
    std::vector<ScreenId> res;
    for (const auto& iter : screenSessionMap_) {
        res.emplace_back(iter.first);
    }
    return res;
}

DMError ScreenSessionManager::GetScreenSupportedColorGamuts(ScreenId screenId,
    std::vector<ScreenColorGamut>& colorGamuts)
{
    return DMError::DM_OK;
}

ScreenId ScreenSessionManager::CreateVirtualScreen(VirtualScreenOption option,
                                                   const sptr<IRemoteObject>& displayManagerAgent)
{
    return ScreenId(0);
}

DMError ScreenSessionManager::SetVirtualScreenSurface(ScreenId screenId, sptr<IBufferProducer> surface)
{
    return DMError::DM_OK;
}

DMError ScreenSessionManager::DestroyVirtualScreen(ScreenId screenId)
{
    return DMError::DM_OK;
}

DMError ScreenSessionManager::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
                                         ScreenId& screenGroupId)
{
    return DMError::DM_OK;
}

bool ScreenSessionManager::ScreenIdManager::ConvertToRsScreenId(ScreenId smsScreenId, ScreenId& rsScreenId) const
{
    return true;
}

ScreenId ScreenSessionManager::ScreenIdManager::ConvertToRsScreenId(ScreenId dmsScreenId) const
{
    ScreenId rsScreenId = SCREEN_ID_INVALID;
    ConvertToRsScreenId(dmsScreenId, rsScreenId);
    return rsScreenId;
}

ScreenId ScreenSessionManager::ScreenIdManager::ConvertToSmsScreenId(ScreenId rsScreenId) const
{
    ScreenId smsScreenId = SCREEN_ID_INVALID;
    ConvertToSmsScreenId(rsScreenId, smsScreenId);
    return smsScreenId;
}

bool ScreenSessionManager::ScreenIdManager::ConvertToSmsScreenId(ScreenId rsScreenId, ScreenId& smsScreenId) const
{
    return true;
}

ScreenId ScreenSessionManager::ScreenIdManager::CreateAndGetNewScreenId(ScreenId rsScreenId)
{
    return ScreenId(0);
}

bool ScreenSessionManager::ScreenIdManager::DeleteScreenId(ScreenId smsScreenId)
{
    return true;
}

bool ScreenSessionManager::ScreenIdManager::HasRsScreenId(ScreenId smsScreenId) const
{
    return rs2SmsScreenIdMap_.find(smsScreenId) != rs2SmsScreenIdMap_.end();
}

ScreenId ScreenSessionManager::GetDefaultAbstractScreenId()
{
    return ScreenId(1);
}

sptr<ScreenSession> ScreenSessionManager::InitVirtualScreen(ScreenId smsScreenId, ScreenId rsId,
    VirtualScreenOption option)
{
    return nullptr;
}

bool ScreenSessionManager::InitAbstractScreenModesInfo(sptr<ScreenSession>& screenSession)
{
    return true;
}

sptr<ScreenSession> ScreenSessionManager::InitAndGetScreen(ScreenId rsScreenId)
{
    return nullptr;
}

void ScreenSessionManager::ProcessScreenConnected(ScreenId rsScreenId)
{
}

void ScreenSessionManager::ProcessScreenDisconnected(ScreenId rsScreenId)
{
}

sptr<ScreenSessionGroup> ScreenSessionManager::AddToGroupLocked(sptr<ScreenSession> newScreen)
{
    return nullptr;
}

sptr<ScreenSessionGroup> ScreenSessionManager::AddAsFirstScreenLocked(sptr<ScreenSession> newScreen)
{
    return nullptr;
}

sptr<ScreenSessionGroup> ScreenSessionManager::AddAsSuccedentScreenLocked(sptr<ScreenSession> newScreen)
{
    return nullptr;
}

sptr<ScreenSessionGroup> ScreenSessionManager::RemoveFromGroupLocked(sptr<ScreenSession> screen)
{
    return nullptr;
}

bool ScreenSessionManager::RemoveChildFromGroup(sptr<ScreenSession> screen, sptr<ScreenSessionGroup> screenGroup)
{
    return true;
}

DMError ScreenSessionManager::SetMirror(ScreenId screenId, std::vector<ScreenId> screens)
{
    return DMError::DM_OK;
}

sptr<ScreenSessionGroup> ScreenSessionManager::GetAbstractScreenGroup(ScreenId smsScreenId)
{
    auto iter = smsScreenGroupMap_.find(smsScreenId);
    if (iter == smsScreenGroupMap_.end()) {
        WLOGE("did not find screen:%{public}" PRIu64"", smsScreenId);
        return nullptr;
    }
    return iter->second;
}

void ScreenSessionManager::ChangeScreenGroup(sptr<ScreenSessionGroup> group, const std::vector<ScreenId>& screens,
    const std::vector<Point>& startPoints, bool filterScreen, ScreenCombination combination)
{
}

void ScreenSessionManager::AddScreenToGroup(sptr<ScreenSessionGroup> group,
    const std::vector<ScreenId>& addScreens, const std::vector<Point>& addChildPos,
    std::map<ScreenId, bool>& removeChildResMap)
{
}

void ScreenSessionManager::RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens)
{
}

const std::shared_ptr<RSDisplayNode>& ScreenSessionManager::GetRSDisplayNodeByScreenId(ScreenId smsScreenId) const
{
    static std::shared_ptr<RSDisplayNode> notFound = nullptr;
    sptr<ScreenSession> screen = GetScreenSession(smsScreenId);
    if (screen == nullptr) {
        WLOGFE("SCB: ScreenSessionManager::GetRSDisplayNodeByScreenId screen == nullptr!");
        return notFound;
    }
    if (screen->displayNode_ == nullptr) {
        WLOGFE("SCB: ScreenSessionManager::GetRSDisplayNodeByScreenId displayNode_ == nullptr!");
        return notFound;
    }
    WLOGI("GetRSDisplayNodeByScreenId: screen: %{public}" PRIu64", nodeId: %{public}" PRIu64" ",
        screen->screenId_, screen->displayNode_->GetId());
    return screen->displayNode_;
}

std::shared_ptr<Media::PixelMap> ScreenSessionManager::GetScreenSnapshot(DisplayId displayId)
{
    ScreenId screenId = SCREEN_ID_INVALID;
    std::shared_ptr<RSDisplayNode> displayNode = nullptr;
    for (auto sessionIt : screenSessionMap_) {
        auto screenSession = sessionIt.second;
        sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
        WLOGI("SCB: GetScreenSnapshot: displayId %{public}" PRIu64"", displayInfo->GetDisplayId());
        if (displayId == displayInfo->GetDisplayId()) {
            displayNode = screenSession->displayNode_;
            screenId = sessionIt.first;
            break;
        }
    }
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("SCB: ScreenSessionManager::GetScreenSnapshot screenId == SCREEN_ID_INVALID!");
        return nullptr;
    }
    if (displayNode == nullptr) {
        WLOGFE("SCB: ScreenSessionManager::GetScreenSnapshot displayNode == nullptr!");
        return nullptr;
    }

    std::shared_ptr<SurfaceCaptureFuture> callback = std::make_shared<SurfaceCaptureFuture>();
    rsInterface_.TakeSurfaceCapture(displayNode, callback);
    std::shared_ptr<Media::PixelMap> screenshot = callback->GetResult(2000); // wait for <= 2000ms
    if (screenshot == nullptr) {
        WLOGFE("SCB: Failed to get pixelmap from RS, return nullptr!");
    }

    // notify dm listener
    sptr<ScreenshotInfo> snapshotInfo = new ScreenshotInfo();
    snapshotInfo->SetTrigger(SysCapUtil::GetClientName());
    snapshotInfo->SetDisplayId(displayId);
    OnScreenshot(snapshotInfo);

    return screenshot;
}

std::shared_ptr<Media::PixelMap> ScreenSessionManager::GetDisplaySnapshot(DisplayId displayId, DmErrorCode* errorCode)
{
    WLOGFE("SCB: ScreenSessionManager::GetDisplaySnapshot ENTER!");
    auto res = GetScreenSnapshot(displayId);
    return res;
}

bool ScreenSessionManager::OnRemoteDied(const sptr<IRemoteObject>& agent)
{
    return true;
}

std::vector<ScreenId> ScreenSessionManager::GetAllValidScreenIds(const std::vector<ScreenId>& screenIds) const
{
    std::vector<ScreenId> validScreenIds;
    return validScreenIds;
}

sptr<ScreenGroupInfo> ScreenSessionManager::GetScreenGroupInfoById(ScreenId screenId)
{
    auto screenSessionGroup = GetAbstractScreenGroup(screenId);
    return screenSessionGroup->ConvertToScreenGroupInfo();
}

void ScreenSessionManager::NotifyScreenConnected(sptr<ScreenInfo> screenInfo)
{
}

void ScreenSessionManager::NotifyScreenDisconnected(ScreenId screenId)
{
}

void ScreenSessionManager::NotifyScreenGroupChanged(
    const sptr<ScreenInfo>& screenInfo, ScreenGroupChangeEvent event)
{
}

void ScreenSessionManager::NotifyScreenGroupChanged(
    const std::vector<sptr<ScreenInfo>>& screenInfo, ScreenGroupChangeEvent event)
{
}

void ScreenSessionManager::OnScreenGroupChange(const std::string& trigger,
    const sptr<ScreenInfo>& screenInfo, ScreenGroupChangeEvent groupEvent)
{
}

void ScreenSessionManager::OnScreenGroupChange(const std::string& trigger,
    const std::vector<sptr<ScreenInfo>>& screenInfos, ScreenGroupChangeEvent groupEvent)
{
}

void ScreenSessionManager::OnScreenConnect(const sptr<ScreenInfo> screenInfo)
{
}

void ScreenSessionManager::OnScreenDisconnect(ScreenId screenId)
{
}

void ScreenSessionManager::OnScreenshot(sptr<ScreenshotInfo> info)
{
}

} // namespace OHOS::Rosen
