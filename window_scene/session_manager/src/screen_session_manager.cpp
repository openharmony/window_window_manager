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

#include <hitrace_meter.h>
#include <iomanip>
#include <parameters.h>
#include <transaction/rs_interfaces.h>
#include <xcollie/watchdog.h>

#include "session_permission.h"
#include "screen_scene_config.h"
#include "surface_capture_future.h"
#include "sys_cap_util.h"
#include "window_manager_hilog.h"
#include "screen_rotation_property.h"
#include "screen_sensor_connector.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSessionManager" };
const std::string SCREEN_SESSION_MANAGER_THREAD = "ScreenSessionManager";
const std::string SCREEN_CAPTURE_PERMISSION = "ohos.permission.CAPTURE_SCREEN";
std::recursive_mutex g_instanceMutex;
} // namespace

ScreenSessionManager& ScreenSessionManager::GetInstance()
{
    std::lock_guard<std::recursive_mutex> lock(g_instanceMutex);
    static ScreenSessionManager* instance = nullptr;
    if (instance == nullptr) {
        instance = new ScreenSessionManager();
        instance->Init();
    }
    return *instance;
}

ScreenSessionManager::ScreenSessionManager() : rsInterface_(RSInterfaces::GetInstance())
{
    LoadScreenSceneXml();
    taskScheduler_ = std::make_shared<TaskScheduler>(SCREEN_SESSION_MANAGER_THREAD);
    screenCutoutController_ = new (std::nothrow) ScreenCutoutController();
    sessionDisplayPowerController_ = new SessionDisplayPowerController(
        std::bind(&ScreenSessionManager::NotifyDisplayStateChange, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

void ScreenSessionManager::Init()
{
    constexpr uint64_t interval = 5 * 1000; // 5 second
    if (HiviewDFX::Watchdog::GetInstance().AddThread(
        SCREEN_SESSION_MANAGER_THREAD, taskScheduler_->GetEventHandler(), interval)) {
        WLOGFW("Add thread %{public}s to watchdog failed.", SCREEN_SESSION_MANAGER_THREAD.c_str());
    }

    RegisterScreenChangeListener();
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

    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (auto& iter : screenSessionMap_) {
        screenConnectionListener->OnScreenConnect(iter.second);
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
    if (type == DisplayManagerAgentType::SCREEN_EVENT_LISTENER && !SessionPermission::IsSystemCalling()
        && !SessionPermission::IsStartByHdcd()) {
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
    if (type == DisplayManagerAgentType::SCREEN_EVENT_LISTENER && !SessionPermission::IsSystemCalling()
        && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("unregister display manager agent permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        WLOGFE("displayManagerAgent invalid");
        return DMError::DM_ERROR_NULLPTR;
    }

    return dmAgentContainer_.UnregisterAgent(displayManagerAgent, type) ? DMError::DM_OK :DMError::DM_ERROR_NULLPTR;
}

void ScreenSessionManager::LoadScreenSceneXml()
{
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
    if (numbersConfig.count("dpi") != 0) {
        uint32_t densityDpi = static_cast<uint32_t>(numbersConfig["dpi"][0]);
        WLOGFD("densityDpi = %u", densityDpi);
        if (densityDpi >= DOT_PER_INCH_MINIMUM_VALUE && densityDpi <= DOT_PER_INCH_MAXIMUM_VALUE) {
            isDensityDpiLoad_ = true;
            densityDpi_ = static_cast<float>(densityDpi) / BASELINE_DENSITY;
        }
    }
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
        WLOGFD("vtBoundary");
    }
    if (stringConfig.count("defaultDisplayCutoutPath") != 0) {
        std::string defaultDisplayCutoutPath = static_cast<std::string>(stringConfig["defaultDisplayCutoutPath"]);
        WLOGFD("defaultDisplayCutoutPath = %{public}s.", defaultDisplayCutoutPath.c_str());
        ScreenSceneConfig::SetCutoutSvgPath(defaultDisplayCutoutPath);
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
    ScreenSceneConfig::SetCurvedCompressionAreaInLandscape();
}

void ScreenSessionManager::RegisterScreenChangeListener()
{
    WLOGFD("Register screen change listener.");
    auto res = rsInterface_.SetScreenChangeCallback(
        [this](ScreenId screenId, ScreenEvent screenEvent) { OnScreenChange(screenId, screenEvent); });
    if (res != StatusCode::SUCCESS) {
        auto task = [this]() { RegisterScreenChangeListener(); };
        taskScheduler_->PostAsyncTask(task, 50); // Retry after 50 ms.
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
        std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
        screenSessionMap_.erase(screenId);
    }
}

sptr<ScreenSession> ScreenSessionManager::GetScreenSession(ScreenId screenId) const
{
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
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
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (auto sessionIt : screenSessionMap_) {
        auto screenSession = sessionIt.second;
        if (screenSession == nullptr) {
            continue;
        }
        sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
        if (displayInfo == nullptr) {
            WLOGFE("ConvertToDisplayInfo error, displayInfo is nullptr.");
            continue;
        }
        if (displayId == displayInfo->GetDisplayId()) {
            return displayInfo;
        }
    }
    WLOGFE("SCB: ScreenSessionManager::GetDisplayInfoById failed.");
    return nullptr;
}

sptr<DisplayInfo> ScreenSessionManager::GetDisplayInfoByScreen(ScreenId screenId)
{
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (auto sessionIt : screenSessionMap_) {
        auto screenSession = sessionIt.second;
        sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
        if (screenId == displayInfo->GetScreenId()) {
            return displayInfo;
        }
    }
    WLOGFE("SCB: ScreenSessionManager::GetDisplayInfoByScreen failed.");
    return nullptr;
}

std::vector<DisplayId> ScreenSessionManager::GetAllDisplayIds()
{
    std::vector<DisplayId> res;
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (auto sessionIt : screenSessionMap_) {
        auto screenSession = sessionIt.second;
        sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
        DisplayId displayId = displayInfo->GetDisplayId();
        res.push_back(displayId);
    }
    return res;
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

DMError ScreenSessionManager::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    WLOGI("SetScreenActiveMode: ScreenId: %{public}" PRIu64", modeId: %{public}u", screenId, modeId);
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("set screen active permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("SetScreenActiveMode: invalid screenId");
        return DMError::DM_ERROR_NULLPTR;
    }
    {
        sptr<ScreenSession> screenSession = GetScreenSession(screenId);
        if (screenSession == nullptr) {
            WLOGFE("SetScreenActiveMode: Get ScreenSession failed");
            return DMError::DM_ERROR_NULLPTR;
        }
        ScreenId rsScreenId = SCREEN_ID_INVALID;
        if (!screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId)) {
            WLOGFE("SetScreenActiveMode: No corresponding rsId");
            return DMError::DM_ERROR_NULLPTR;
        }
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:SetScreenActiveMode(%" PRIu64", %u)", screenId, modeId);
        rsInterface_.SetScreenActiveMode(rsScreenId, modeId);
        screenSession->activeIdx_ = static_cast<int32_t>(modeId);
        screenSession->UpdatePropertyByActiveMode();
        screenSession->PropertyChange(screenSession->GetScreenProperty(), ScreenPropertyChangeReason::CHANGE_MODE);
        NotifyScreenChanged(screenSession->ConvertToScreenInfo(), ScreenChangeEvent::CHANGE_MODE);
    }
    return DMError::DM_OK;
}

void ScreenSessionManager::NotifyScreenChanged(sptr<ScreenInfo> screenInfo, ScreenChangeEvent event)
{
    if (screenInfo == nullptr) {
        WLOGFE("NotifyScreenChanged error, screenInfo is nullptr.");
        return;
    }
    auto task = [=] {
        WLOGFI("NotifyScreenChanged,  screenId:%{public}" PRIu64"", screenInfo->GetScreenId());
        auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
        if (agents.empty()) {
            return;
        }
        for (auto& agent : agents) {
            agent->OnScreenChange(screenInfo, event);
        }
    };
    taskScheduler_->PostAsyncTask(task);
}

DMError ScreenSessionManager::SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("set virtual pixel permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }

    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screen session is nullptr");
        return DMError::DM_ERROR_UNKNOWN;
    }
    if (screenSession->isScreenGroup_) {
        WLOGE("cannot set virtual pixel ratio to the combination. screen: %{public}" PRIu64"", screenId);
        return DMError::DM_ERROR_NULLPTR;
    }
    if (fabs(screenSession->GetScreenProperty().GetVirtualPixelRatio() - virtualPixelRatio) < 1e-6) {
        WLOGE("The density is equivalent to the original value, no update operation is required, aborted.");
        return DMError::DM_OK;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:SetVirtualPixelRatio(%" PRIu64", %f)", screenId,
        virtualPixelRatio);
    screenSession->SetVirtualPixelRatio(virtualPixelRatio);
    NotifyScreenChanged(screenSession->ConvertToScreenInfo(), ScreenChangeEvent::VIRTUAL_PIXEL_RATIO_CHANGED);

    return DMError::DM_OK;
}

DMError ScreenSessionManager::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    WLOGFI("GetScreenColorGamut::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->GetScreenColorGamut(colorGamut);
}

DMError ScreenSessionManager::SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx)
{
    WLOGFI("SetScreenColorGamut::ScreenId: %{public}" PRIu64 ", colorGamutIdx %{public}d", screenId, colorGamutIdx);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->SetScreenColorGamut(colorGamutIdx);
}

DMError ScreenSessionManager::GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap)
{
    WLOGFI("GetScreenGamutMap::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->GetScreenGamutMap(gamutMap);
}

DMError ScreenSessionManager::SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap)
{
    WLOGFI("SetScreenGamutMap::ScreenId: %{public}" PRIu64 ", ScreenGamutMap %{public}u",
        screenId, static_cast<uint32_t>(gamutMap));
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->SetScreenGamutMap(gamutMap);
}

DMError ScreenSessionManager::SetScreenColorTransform(ScreenId screenId)
{
    WLOGFI("SetScreenColorTransform::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->SetScreenColorTransform();
}

sptr<ScreenSession> ScreenSessionManager::GetOrCreateScreenSession(ScreenId screenId)
{
    WLOGFI("SCB: ScreenSessionManager::GetOrCreateScreenSession ENTER");
    {
        std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
        auto sessionIt = screenSessionMap_.find(screenId);
        if (sessionIt != screenSessionMap_.end()) {
            return sessionIt->second;
        }
    }

    ScreenId rsId = rsInterface_.GetDefaultScreenId();
    screenIdManager_.rs2SmsScreenIdMap_[rsId] = screenId;
    screenIdManager_.sms2RsScreenIdMap_[screenId] = rsId;

    auto screenMode = rsInterface_.GetScreenActiveMode(screenId);
    auto screenBounds = RRect({ 0, 0, screenMode.GetScreenWidth(), screenMode.GetScreenHeight() }, 0.0f, 0.0f);
    auto screenRefreshRate = screenMode.GetScreenRefreshRate();
    auto screenCapability = rsInterface_.GetScreenCapability(screenId);
    ScreenProperty property;
    property.SetRotation(0.0f);
    property.SetPhyWidth(screenCapability.GetPhyWidth());
    property.SetPhyHeight(screenCapability.GetPhyHeight());
    property.SetBounds(screenBounds);
    if (isDensityDpiLoad_) {
        property.SetVirtualPixelRatio(densityDpi_);
    } else {
        property.UpdateVirtualPixelRatio(screenBounds);
    }
    property.SetRefreshRate(screenRefreshRate);
    sptr<ScreenSession> session = new(std::nothrow) ScreenSession(screenId, property, GetDefaultAbstractScreenId());
    if (!session) {
        WLOGFE("screen session is nullptr");
        return session;
    }
    InitAbstractScreenModesInfo(session);
    session->groupSmsId_ = 1;
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
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
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:WakeUpBegin(%u)", reason);
    return NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP, EventStatus::BEGIN);
}

bool ScreenSessionManager::WakeUpEnd()
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:WakeUpEnd");
    return NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP, EventStatus::END);
}

bool ScreenSessionManager::SuspendBegin(PowerStateChangeReason reason)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:SuspendBegin(%u)", reason);
    sessionDisplayPowerController_->SuspendBegin(reason);
    return NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP, EventStatus::BEGIN);
}

bool ScreenSessionManager::SuspendEnd()
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:SuspendEnd");
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

void ScreenSessionManager::NotifyScreenshot(DisplayId displayId)
{
    if (displayChangeListener_ != nullptr) {
        displayChangeListener_->OnScreenshot(displayId);
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
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
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
    auto state = static_cast<ScreenPowerState>(RSInterfaces::GetInstance().GetScreenPowerStatus(screenId));
    WLOGFI("GetScreenPower:%{public}u, rsscreen:%{public}" PRIu64".", state, screenId);
    return state;
}

DMError ScreenSessionManager::IsScreenRotationLocked(bool& isLocked)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("SCB: ScreenSessionManager is screen rotation locked permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    isLocked = ScreenRotationProperty::IsScreenRotationLocked();
    WLOGFI("SCB: IsScreenRotationLocked:isLocked: %{public}u", isLocked);
    return DMError::DM_OK;
}

DMError ScreenSessionManager::SetScreenRotationLocked(bool isLocked)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("SCB: ScreenSessionManager set screen rotation locked permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    WLOGFI("SCB: SetScreenRotationLocked: isLocked: %{public}u", isLocked);
    return ScreenRotationProperty::SetScreenRotationLocked(isLocked);
}

void ScreenSessionManager::UpdateScreenRotationProperty(ScreenId screenId, RRect bounds, int rotation)
{
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        WLOGFE("fail to update screen rotation property, cannot find screen %{public}" PRIu64"", screenId);
        return;
    }
    Rotation targetRotation = Rotation::ROTATION_0;
    switch (rotation) {
        case 90: // Rotation 90 degree
            targetRotation = Rotation::ROTATION_90;
            break;
        case 180: // Rotation 180 degree
            targetRotation = Rotation::ROTATION_180;
            break;
        case 270: // Rotation 270 degree
            targetRotation = Rotation::ROTATION_270;
            break;
        default:
            targetRotation = Rotation::ROTATION_0;
            break;
    }
    sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
    if (displayInfo == nullptr) {
        WLOGFE("fail to update screen rotation property, displayInfo is nullptr");
        return;
    }
    displayInfo->SetRotation(targetRotation);
    displayInfo->SetWidth(bounds.rect_.GetWidth());
    displayInfo->SetHeight(bounds.rect_.GetHeight());
    NotifyDisplayChanged(displayInfo, DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
}

void ScreenSessionManager::NotifyDisplayChanged(sptr<DisplayInfo> displayInfo, DisplayChangeEvent event)
{
    if (displayInfo == nullptr) {
        WLOGFE("NotifyDisplayChanged error, displayInfo is nullptr.");
        return;
    }
    auto task = [=] {
        WLOGFI("NotifyDisplayChanged, displayId:%{public}" PRIu64"", displayInfo->GetDisplayId());
        auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
        if (agents.empty()) {
            return;
        }
        for (auto& agent : agents) {
            agent->OnDisplayChange(displayInfo, event);
        }
    };
    taskScheduler_->PostAsyncTask(task);
}

DMError ScreenSessionManager::SetOrientation(ScreenId screenId, Orientation orientation)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("SCB: ScreenSessionManager set orientation permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if (orientation < Orientation::UNSPECIFIED || orientation > Orientation::REVERSE_HORIZONTAL) {
        WLOGFE("SCB: ScreenSessionManager set orientation: %{public}u", static_cast<uint32_t>(orientation));
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return SetOrientationController(screenId, orientation, false);
}

DMError ScreenSessionManager::SetOrientationFromWindow(DisplayId displayId, Orientation orientation)
{
    sptr<DisplayInfo> displayInfo = GetDisplayInfoById(displayId);
    if (displayInfo == nullptr) {
        return DMError::DM_ERROR_NULLPTR;
    }
    return SetOrientationController(displayInfo->GetScreenId(), orientation, true);
}

DMError ScreenSessionManager::SetOrientationController(ScreenId screenId, Orientation newOrientation,
    bool isFromWindow)
{
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        WLOGFE("fail to set orientation, cannot find screen %{public}" PRIu64"", screenId);
        return DMError::DM_ERROR_NULLPTR;
    }

    if (isFromWindow) {
        if (newOrientation == Orientation::UNSPECIFIED) {
            newOrientation = screenSession->GetScreenRequestedOrientation();
        }
    } else {
        screenSession->SetScreenRequestedOrientation(newOrientation);
    }

    if (screenSession->GetOrientation() == newOrientation) {
        return DMError::DM_OK;
    }
    if (isFromWindow) {
        ScreenRotationProperty::ProcessOrientationSwitch(newOrientation);
    } else {
        Rotation rotationAfter = screenSession->CalcRotation(newOrientation);
        SetRotation(screenId, rotationAfter, false);
    }
    screenSession->SetOrientation(newOrientation);
    screenSession->PropertyChange(screenSession->GetScreenProperty(), ScreenPropertyChangeReason::ROTATION);
    // Notify rotation event to ScreenManager
    NotifyScreenChanged(screenSession->ConvertToScreenInfo(), ScreenChangeEvent::UPDATE_ORIENTATION);
    return DMError::DM_OK;
}

bool ScreenSessionManager::SetRotation(ScreenId screenId, Rotation rotationAfter, bool isFromWindow)
{
    WLOGFI("Enter SetRotation, screenId: %{public}" PRIu64 ", rotation: %{public}u, isFromWindow: %{public}u,",
        screenId, rotationAfter, isFromWindow);
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        WLOGFE("SetRotation error, cannot get screen with screenId: %{public}" PRIu64, screenId);
        return false;
    }
    if (rotationAfter == screenSession->GetRotation()) {
        WLOGFE("rotation not changed. screen %{public}" PRIu64" rotation %{public}u", screenId, rotationAfter);
        return false;
    }
    WLOGFD("set orientation. rotation %{public}u", rotationAfter);
    SetDisplayBoundary(screenSession);
    screenSession->SetRotation(rotationAfter);
    screenSession->PropertyChange(screenSession->GetScreenProperty(), ScreenPropertyChangeReason::ROTATION);
    NotifyScreenChanged(screenSession->ConvertToScreenInfo(), ScreenChangeEvent::UPDATE_ROTATION);
    NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(), DisplayChangeEvent::UPDATE_ROTATION);
    return true;
}

void ScreenSessionManager::SetSensorSubscriptionEnabled()
{
    isAutoRotationOpen_ = system::GetParameter("persist.display.ar.enabled", "1") == "1";
    if (!isAutoRotationOpen_) {
        WLOGFE("autoRotation is not open");
        ScreenRotationProperty::Init();
        return;
    }
    ScreenSensorConnector::SubscribeRotationSensor();
}

bool ScreenSessionManager::SetRotationFromWindow(Rotation targetRotation)
{
    sptr<DisplayInfo> displayInfo = GetDefaultDisplayInfo();
    if (displayInfo == nullptr) {
        return false;
    }
    return SetRotation(displayInfo->GetScreenId(), targetRotation, true);
}

sptr<SupportedScreenModes> ScreenSessionManager::GetScreenModesByDisplayId(DisplayId displayId)
{
    auto displayInfo = GetDisplayInfoById(displayId);
    if (displayInfo == nullptr) {
        WLOGFE("can not get display.");
        return nullptr;
    }
    auto screenInfo = GetScreenInfoById(displayInfo->GetScreenId());
    if (screenInfo == nullptr) {
        WLOGFE("can not get screen.");
        return nullptr;
    }
    auto modes = screenInfo->GetModes();
    auto id = screenInfo->GetModeId();
    if (id >= modes.size()) {
        WLOGFE("can not get screenMode.");
        return nullptr;
    }
    return modes[id];
}

sptr<ScreenInfo> ScreenSessionManager::GetScreenInfoByDisplayId(DisplayId displayId)
{
    auto displayInfo = GetDisplayInfoById(displayId);
    if (displayInfo == nullptr) {
        WLOGFE("can not get displayInfo.");
        return nullptr;
    }
    return GetScreenInfoById(displayInfo->GetScreenId());
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
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("SCB: ScreenSessionManager::GetAllScreenInfos get all screen infos permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    std::vector<ScreenId> screenIds = GetAllScreenIds();
    for (auto screenId: screenIds) {
        auto screenInfo = GetScreenInfoById(screenId);
        if (screenInfo == nullptr) {
            WLOGE("SCB: ScreenSessionManager::GetAllScreenInfos cannot find screenInfo: %{public}" PRIu64"", screenId);
            continue;
        }
        screenInfos.emplace_back(screenInfo);
    }
    return DMError::DM_OK;
}

std::vector<ScreenId> ScreenSessionManager::GetAllScreenIds() const
{
    std::vector<ScreenId> res;
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (const auto& iter : screenSessionMap_) {
        res.emplace_back(iter.first);
    }
    return res;
}

DMError ScreenSessionManager::GetScreenSupportedColorGamuts(ScreenId screenId,
    std::vector<ScreenColorGamut>& colorGamuts)
{
    WLOGFI("SCB: ScreenSessionManager::GetScreenSupportedColorGamuts ENTER");
    sptr<ScreenSession> screen = GetScreenSession(screenId);
    if (screen == nullptr) {
        WLOGFE("SCB: ScreenSessionManager::GetScreenSupportedColorGamuts nullptr");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screen->GetScreenSupportedColorGamuts(colorGamuts);
}

ScreenId ScreenSessionManager::CreateVirtualScreen(VirtualScreenOption option,
                                                   const sptr<IRemoteObject>& displayManagerAgent)
{
    WLOGFI("SCB: ScreenSessionManager::CreateVirtualScreen ENTER");
    ScreenId rsId = rsInterface_.CreateVirtualScreen(option.name_, option.width_,
        option.height_, option.surface_, SCREEN_ID_INVALID, option.flags_);
    WLOGFI("SCB: ScreenSessionManager::CreateVirtualScreen rsid: %{public}" PRIu64"", rsId);
    if (rsId == SCREEN_ID_INVALID) {
        WLOGFI("SCB: ScreenSessionManager::CreateVirtualScreen rsid is invalid");
        return SCREEN_ID_INVALID;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:CreateVirtualScreen(%s)", option.name_.c_str());
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    ScreenId smsScreenId = SCREEN_ID_INVALID;
    if (!screenIdManager_.ConvertToSmsScreenId(rsId, smsScreenId)) {
        WLOGFI("SCB: ScreenSessionManager::CreateVirtualScreen !ConvertToSmsScreenId(rsId, smsScreenId)");
        smsScreenId = screenIdManager_.CreateAndGetNewScreenId(rsId);
        auto screenSession = InitVirtualScreen(smsScreenId, rsId, option);
        if (screenSession == nullptr) {
            WLOGFI("SCB: ScreenSessionManager::CreateVirtualScreen screensession is nullptr");
            screenIdManager_.DeleteScreenId(smsScreenId);
            return SCREEN_ID_INVALID;
        }
        screenSessionMap_.insert(std::make_pair(smsScreenId, screenSession));
        NotifyScreenConnected(screenSession->ConvertToScreenInfo());
        if (deathRecipient_ == nullptr) {
            WLOGFI("SCB: ScreenSessionManager::CreateVirtualScreen Create deathRecipient");
            deathRecipient_ =
                new AgentDeathRecipient([this](const sptr<IRemoteObject>& agent) { OnRemoteDied(agent); });
        }
        auto agIter = screenAgentMap_.find(displayManagerAgent);
        if (agIter == screenAgentMap_.end()) {
            displayManagerAgent->AddDeathRecipient(deathRecipient_);
        }
        screenAgentMap_[displayManagerAgent].emplace_back(smsScreenId);
    } else {
        WLOGFI("SCB: ScreenSessionManager::CreateVirtualScreen id: %{public}" PRIu64" in screenIdManager_", rsId);
    }
    return smsScreenId;
}

DMError ScreenSessionManager::SetVirtualScreenSurface(ScreenId screenId, sptr<IBufferProducer> surface)
{
    WLOGFI("SCB: ScreenSessionManager::SetVirtualScreenSurface ENTER");
    ScreenId rsScreenId;
    int32_t res = -1;
    if (screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId)) {
        sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(surface);
        res = rsInterface_.SetVirtualScreenSurface(rsScreenId, pSurface);
    }
    if (res != 0) {
        WLOGE("SCB: ScreenSessionManager::SetVirtualScreenSurface failed in RenderService");
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    return DMError::DM_OK;
}

DMError ScreenSessionManager::DestroyVirtualScreen(ScreenId screenId)
{
    WLOGI("SCB: ScreenSessionManager::DestroyVirtualScreen Enter");
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    ScreenId rsScreenId = SCREEN_ID_INVALID;
    screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId);

    bool agentFound = false;
    for (auto &agentIter : screenAgentMap_) {
        for (auto iter = agentIter.second.begin(); iter != agentIter.second.end(); iter++) {
            if (*iter == screenId) {
                iter = agentIter.second.erase(iter);
                agentFound = true;
                break;
            }
        }
        if (agentFound) {
            if (agentIter.first != nullptr && agentIter.second.empty()) {
                screenAgentMap_.erase(agentIter.first);
            }
            break;
        }
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:DestroyVirtualScreen(%" PRIu64")", screenId);
    if (rsScreenId != SCREEN_ID_INVALID && GetScreenSession(screenId) != nullptr) {
        auto smsScreenMapIter = screenSessionMap_.find(screenId);
        if (smsScreenMapIter != screenSessionMap_.end()) {
            auto screenGroup = RemoveFromGroupLocked(smsScreenMapIter->second);
            if (screenGroup != nullptr) {
                NotifyScreenGroupChanged(
                    smsScreenMapIter->second->ConvertToScreenInfo(), ScreenGroupChangeEvent::REMOVE_FROM_GROUP);
            }
            screenSessionMap_.erase(smsScreenMapIter);
        }
    }
    screenIdManager_.DeleteScreenId(screenId);

    if (rsScreenId == SCREEN_ID_INVALID) {
        WLOGFE("SCB: ScreenSessionManager::DestroyVirtualScreen: No corresponding rsScreenId");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    rsInterface_.RemoveVirtualScreen(rsScreenId);
    return DMError::DM_OK;
}

DMError ScreenSessionManager::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
                                         ScreenId& screenGroupId)
{
    WLOGFI("SCB:ScreenSessionManager::MakeMirror enter!");
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("SCB:ScreenSessionManager::MakeMirror permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    WLOGFI("SCB:ScreenSessionManager::MakeMirror mainScreenId :%{public}" PRIu64"", mainScreenId);
    auto allMirrorScreenIds = GetAllValidScreenIds(mirrorScreenIds);
    auto iter = std::find(allMirrorScreenIds.begin(), allMirrorScreenIds.end(), mainScreenId);
    if (iter != allMirrorScreenIds.end()) {
        allMirrorScreenIds.erase(iter);
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:MakeMirror");
    auto mainScreen = GetScreenSession(mainScreenId);
    if (mainScreen == nullptr || allMirrorScreenIds.empty()) {
        WLOGFE("SCB:ScreenSessionManager::MakeMirror fail. mainScreen :%{public}" PRIu64", screens size:%{public}u",
            mainScreenId, static_cast<uint32_t>(allMirrorScreenIds.size()));
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    DMError ret = SetMirror(mainScreenId, allMirrorScreenIds);
    if (ret != DMError::DM_OK) {
        WLOGFE("SCB:ScreenSessionManager::MakeMirror set mirror failed.");
        return ret;
    }
    if (GetAbstractScreenGroup(mainScreen->groupSmsId_) == nullptr) {
        WLOGFE("SCB:ScreenSessionManager::MakeMirror get screen group failed.");
        return DMError::DM_ERROR_NULLPTR;
    }
    screenGroupId = mainScreen->groupSmsId_;
    return DMError::DM_OK;
}

DMError ScreenSessionManager::MakeExpand(std::vector<ScreenId> screenId,
                                         std::vector<Point> startPoint,
                                         ScreenId& screenGroupId)
{
    WLOGFI("SCB:ScreenSessionManager::MakeExpand enter!");
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("SCB:ScreenSessionManager::MakeExpand permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if (screenId.empty() || startPoint.empty() || screenId.size() != startPoint.size()) {
        WLOGFE("create expand fail, screenId size:%{public}ud,startPoint size:%{public}ud",
            static_cast<uint32_t>(screenId.size()), static_cast<uint32_t>(startPoint.size()));
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    std::map<ScreenId, Point> pointsMap;
    uint32_t size = screenId.size();
    for (uint32_t i = 0; i < size; i++) {
        if (pointsMap.find(screenId[i]) != pointsMap.end()) {
            continue;
        }
        pointsMap[screenId[i]] = startPoint[i];
    }
    ScreenId defaultScreenId = GetDefaultAbstractScreenId();
    auto allExpandScreenIds = GetAllValidScreenIds(screenId);
    auto iter = std::find(allExpandScreenIds.begin(), allExpandScreenIds.end(), defaultScreenId);
    if (iter != allExpandScreenIds.end()) {
        allExpandScreenIds.erase(iter);
    }
    if (allExpandScreenIds.empty()) {
        WLOGFE("allExpandScreenIds is empty. make expand failed.");
        return DMError::DM_ERROR_NULLPTR;
    }
    std::shared_ptr<RSDisplayNode> rsDisplayNode;
    std::vector<Point> points;
    for (uint32_t i = 0; i < allExpandScreenIds.size(); i++) {
        rsDisplayNode = GetRSDisplayNodeByScreenId(allExpandScreenIds[i]);
        points.emplace_back(pointsMap[allExpandScreenIds[i]]);
        if (rsDisplayNode != nullptr) {
            rsDisplayNode->SetDisplayOffset(pointsMap[allExpandScreenIds[i]].posX_,
                pointsMap[allExpandScreenIds[i]].posY_);
        }
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:MakeExpand");
    if (!OnMakeExpand(allExpandScreenIds, points)) {
        return DMError::DM_ERROR_NULLPTR;
    }
    auto screen = GetScreenSession(allExpandScreenIds[0]);
    if (screen == nullptr || GetAbstractScreenGroup(screen->groupSmsId_) == nullptr) {
        return DMError::DM_ERROR_NULLPTR;
    }
    screenGroupId = screen->groupSmsId_;
    return DMError::DM_OK;
}

bool ScreenSessionManager::OnMakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint)
{
    ScreenId defaultScreenId = GetDefaultAbstractScreenId();
    WLOGI("OnMakeExpand, defaultScreenId:%{public}" PRIu64"", defaultScreenId);
    auto defaultScreen = GetScreenSession(defaultScreenId);
    if (defaultScreen == nullptr) {
        WLOGFI("OnMakeExpand failed.");
        return false;
    }
    auto group = GetAbstractScreenGroup(defaultScreen->groupSmsId_);
    if (group == nullptr) {
        group = AddToGroupLocked(defaultScreen);
        if (group == nullptr) {
            WLOGFE("group is nullptr");
            return false;
        }
        NotifyScreenGroupChanged(defaultScreen->ConvertToScreenInfo(), ScreenGroupChangeEvent::ADD_TO_GROUP);
    }
    bool filterExpandScreen = group->combination_ == ScreenCombination::SCREEN_EXPAND;
    ChangeScreenGroup(group, screenId, startPoint, filterExpandScreen, ScreenCombination::SCREEN_EXPAND);
    WLOGFI("OnMakeExpand success");
    return true;
}

bool ScreenSessionManager::ScreenIdManager::ConvertToRsScreenId(ScreenId smsScreenId, ScreenId& rsScreenId) const
{
    auto iter = sms2RsScreenIdMap_.find(smsScreenId);
    if (iter == sms2RsScreenIdMap_.end()) {
        return false;
    }
    rsScreenId = iter->second;
    return true;
}

ScreenId ScreenSessionManager::ScreenIdManager::ConvertToRsScreenId(ScreenId screenId) const
{
    ScreenId rsScreenId = SCREEN_ID_INVALID;
    ConvertToRsScreenId(screenId, rsScreenId);
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
    auto iter = rs2SmsScreenIdMap_.find(rsScreenId);
    if (iter == rs2SmsScreenIdMap_.end()) {
        return false;
    }
    smsScreenId = iter->second;
    return true;
}

ScreenId ScreenSessionManager::ScreenIdManager::CreateAndGetNewScreenId(ScreenId rsScreenId)
{
    ScreenId smsScreenId = smsScreenCount_++;
    WLOGFI("SCB: ScreenSessionManager::CreateAndGetNewScreenId screenId: %{public}" PRIu64"", smsScreenId);
    if (sms2RsScreenIdMap_.find(smsScreenId) != sms2RsScreenIdMap_.end()) {
        WLOGFW("SCB: ScreenSessionManager::CreateAndGetNewScreenId screenId: %{public}" PRIu64" exit", smsScreenId);
    }
    sms2RsScreenIdMap_[smsScreenId] = rsScreenId;
    if (rsScreenId == SCREEN_ID_INVALID) {
        return smsScreenId;
    }
    if (rs2SmsScreenIdMap_.find(rsScreenId) != rs2SmsScreenIdMap_.end()) {
        WLOGFW("SCB: ScreenSessionManager::CreateAndGetNewScreenId rsScreenId: %{public}" PRIu64" exit", rsScreenId);
    }
    rs2SmsScreenIdMap_[rsScreenId] = smsScreenId;
    return smsScreenId;
}

bool ScreenSessionManager::ScreenIdManager::DeleteScreenId(ScreenId smsScreenId)
{
    auto iter = sms2RsScreenIdMap_.find(smsScreenId);
    if (iter == sms2RsScreenIdMap_.end()) {
        return false;
    }
    ScreenId rsScreenId = iter->second;
    sms2RsScreenIdMap_.erase(smsScreenId);
    rs2SmsScreenIdMap_.erase(rsScreenId);
    return true;
}

bool ScreenSessionManager::ScreenIdManager::HasRsScreenId(ScreenId smsScreenId) const
{
    return rs2SmsScreenIdMap_.find(smsScreenId) != rs2SmsScreenIdMap_.end();
}

ScreenId ScreenSessionManager::GetDefaultAbstractScreenId()
{
    WLOGFI("SCB: ScreenSessionManager::GetDefaultAbstractScreenId ENTER");
    defaultRsScreenId_ = rsInterface_.GetDefaultScreenId();
    return screenIdManager_.ConvertToSmsScreenId(defaultRsScreenId_);
}

sptr<ScreenSession> ScreenSessionManager::InitVirtualScreen(ScreenId smsScreenId, ScreenId rsId,
    VirtualScreenOption option)
{
    WLOGFI("SCB: ScreenSessionManager::InitVirtualScreen: Enter");
    sptr<ScreenSession> screenSession =
        new(std::nothrow) ScreenSession(option.name_, smsScreenId, rsId, GetDefaultAbstractScreenId());
    sptr<SupportedScreenModes> info = new(std::nothrow) SupportedScreenModes();
    if (screenSession == nullptr || info == nullptr) {
        WLOGFI("SCB: ScreenSessionManager::InitVirtualScreen: new screenSession or info failed");
        screenIdManager_.DeleteScreenId(smsScreenId);
        rsInterface_.RemoveVirtualScreen(rsId);
        return nullptr;
    }
    info->width_ = option.width_;
    info->height_ = option.height_;
    auto defaultScreen = GetScreenSession(GetDefaultAbstractScreenId());
    if (defaultScreen != nullptr && defaultScreen->GetActiveScreenMode() != nullptr) {
        info->refreshRate_ = defaultScreen->GetActiveScreenMode()->refreshRate_;
    }
    screenSession->modes_.emplace_back(info);
    screenSession->activeIdx_ = 0;
    screenSession->SetScreenType(ScreenType::VIRTUAL);
    screenSession->SetVirtualPixelRatio(option.density_);
    return screenSession;
}

bool ScreenSessionManager::InitAbstractScreenModesInfo(sptr<ScreenSession>& screenSession)
{
    std::vector<RSScreenModeInfo> allModes = rsInterface_.GetScreenSupportedModes(
        screenIdManager_.ConvertToRsScreenId(screenSession->screenId_));
    if (allModes.size() == 0) {
        WLOGE("SCB: allModes.size() == 0, screenId=%{public}" PRIu64"", screenSession->rsId_);
        return false;
    }
    for (const RSScreenModeInfo& rsScreenModeInfo : allModes) {
        sptr<SupportedScreenModes> info = new(std::nothrow) SupportedScreenModes();
        if (info == nullptr) {
            WLOGFE("SCB: ScreenSessionManager::InitAbstractScreenModesInfo:create SupportedScreenModes failed");
            return false;
        }
        info->width_ = static_cast<uint32_t>(rsScreenModeInfo.GetScreenWidth());
        info->height_ = static_cast<uint32_t>(rsScreenModeInfo.GetScreenHeight());
        info->refreshRate_ = rsScreenModeInfo.GetScreenRefreshRate();
        screenSession->modes_.push_back(info);
        WLOGI("SCB: fill screen idx:%{public}d w/h:%{public}d/%{public}d",
            rsScreenModeInfo.GetScreenModeId(), info->width_, info->height_);
    }
    int32_t activeModeId = rsInterface_.GetScreenActiveMode(screenSession->rsId_).GetScreenModeId();
    WLOGI("SCB: ScreenSessionManager::InitAbstractScreenModesInfo: fill screen activeModeId:%{public}d", activeModeId);
    if (static_cast<std::size_t>(activeModeId) >= allModes.size()) {
        WLOGE("SCB: activeModeId exceed, screenId=%{public}" PRIu64", activeModeId:%{public}d/%{public}ud",
            screenSession->rsId_, activeModeId, static_cast<uint32_t>(allModes.size()));
        return false;
    }
    screenSession->activeIdx_ = activeModeId;
    return true;
}

sptr<ScreenSession> ScreenSessionManager::InitAndGetScreen(ScreenId rsScreenId)
{
    ScreenId smsScreenId = screenIdManager_.CreateAndGetNewScreenId(rsScreenId);
    RSScreenCapability screenCapability = rsInterface_.GetScreenCapability(rsScreenId);
    WLOGFD("SCB: Screen name is %{public}s, phyWidth is %{public}u, phyHeight is %{public}u",
        screenCapability.GetName().c_str(), screenCapability.GetPhyWidth(), screenCapability.GetPhyHeight());
    sptr<ScreenSession> screenSession =
        new(std::nothrow) ScreenSession(screenCapability.GetName(), smsScreenId, rsScreenId, GetDefaultAbstractScreenId());
    if (screenSession == nullptr) {
        WLOGFE("SCB: ScreenSessionManager::InitAndGetScreen: screenSession == nullptr.");
        screenIdManager_.DeleteScreenId(smsScreenId);
        return nullptr;
    }
    if (!InitAbstractScreenModesInfo(screenSession)) {
        screenIdManager_.DeleteScreenId(smsScreenId);
        WLOGFE("SCB: ScreenSessionManager::InitAndGetScreen: InitAndGetScreen failed.");
        return nullptr;
    }
    WLOGI("SCB: InitAndGetScreen: screenSessionMap_ add screenId=%{public}" PRIu64"", smsScreenId);
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    screenSessionMap_.insert(std::make_pair(smsScreenId, screenSession));
    return screenSession;
}

sptr<ScreenSessionGroup> ScreenSessionManager::AddToGroupLocked(sptr<ScreenSession> newScreen)
{
    sptr<ScreenSessionGroup> res;
    if (smsScreenGroupMap_.empty()) {
        WLOGI("connect the first screen");
        res = AddAsFirstScreenLocked(newScreen);
    } else {
        res = AddAsSuccedentScreenLocked(newScreen);
    }
    return res;
}

sptr<ScreenSessionGroup> ScreenSessionManager::AddAsFirstScreenLocked(sptr<ScreenSession> newScreen)
{
    ScreenId smsGroupScreenId(1);
    std::ostringstream buffer;
    buffer<<"ScreenGroup_"<<smsGroupScreenId;
    std::string name = buffer.str();
    // default ScreenCombination is mirror
    isExpandCombination_ = system::GetParameter("persist.display.expand.enabled", "0") == "1";
    sptr<ScreenSessionGroup> screenGroup;
    if (isExpandCombination_) {
        screenGroup = new(std::nothrow) ScreenSessionGroup(smsGroupScreenId,
            SCREEN_ID_INVALID, name, ScreenCombination::SCREEN_EXPAND);
        newScreen->SetScreenCombination(ScreenCombination::SCREEN_EXPAND);
    } else {
        screenGroup = new(std::nothrow) ScreenSessionGroup(smsGroupScreenId,
            SCREEN_ID_INVALID, name, ScreenCombination::SCREEN_MIRROR);
        newScreen->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    }
    if (screenGroup == nullptr) {
        WLOGE("new ScreenSessionGroup failed");
        screenIdManager_.DeleteScreenId(smsGroupScreenId);
        return nullptr;
    }
    screenGroup->groupSmsId_ = 1;
    Point point;
    if (!screenGroup->AddChild(newScreen, point, GetScreenSession(GetDefaultAbstractScreenId()))) {
        WLOGE("fail to add screen to group. screen=%{public}" PRIu64"", newScreen->screenId_);
        screenIdManager_.DeleteScreenId(smsGroupScreenId);
        return nullptr;
    }
    auto iter = smsScreenGroupMap_.find(smsGroupScreenId);
    if (iter != smsScreenGroupMap_.end()) {
        WLOGE("group screen existed. id=%{public}" PRIu64"", smsGroupScreenId);
        smsScreenGroupMap_.erase(iter);
    }
    smsScreenGroupMap_.insert(std::make_pair(smsGroupScreenId, screenGroup));
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    screenSessionMap_.insert(std::make_pair(smsGroupScreenId, screenGroup));
    screenGroup->mirrorScreenId_ = newScreen->screenId_;
    WLOGI("connect new group screen, screenId: %{public}" PRIu64", screenGroupId: %{public}" PRIu64", "
        "combination:%{public}u", newScreen->screenId_, smsGroupScreenId,
        newScreen->GetScreenProperty().GetScreenType());
    return screenGroup;
}

sptr<ScreenSessionGroup> ScreenSessionManager::AddAsSuccedentScreenLocked(sptr<ScreenSession> newScreen)
{
    ScreenId defaultScreenId = GetDefaultAbstractScreenId();
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    auto iter = screenSessionMap_.find(defaultScreenId);
    if (iter == screenSessionMap_.end()) {
        WLOGE("AddAsSuccedentScreenLocked. defaultScreenId:%{public}" PRIu64" is not in screenSessionMap_.",
            defaultScreenId);
        return nullptr;
    }
    auto screen = iter->second;
    auto screenGroupIter = smsScreenGroupMap_.find(screen->groupSmsId_);
    if (screenGroupIter == smsScreenGroupMap_.end()) {
        WLOGE("AddAsSuccedentScreenLocked. groupSmsId:%{public}" PRIu64" is not in smsScreenGroupMap_.",
            screen->groupSmsId_);
        return nullptr;
    }
    auto screenGroup = screenGroupIter->second;
    Point point;
    if (screenGroup->combination_ == ScreenCombination::SCREEN_EXPAND) {
        point = {screen->GetActiveScreenMode()->width_, 0};
    }
    screenGroup->AddChild(newScreen, point, screen);
    return screenGroup;
}

sptr<ScreenSessionGroup> ScreenSessionManager::RemoveFromGroupLocked(sptr<ScreenSession> screen)
{
    WLOGI("RemoveFromGroupLocked.");
    auto groupSmsId = screen->groupSmsId_;
    auto iter = smsScreenGroupMap_.find(groupSmsId);
    if (iter == smsScreenGroupMap_.end()) {
        WLOGE("RemoveFromGroupLocked. groupSmsId:%{public}" PRIu64"is not in smsScreenGroupMap_.", groupSmsId);
        return nullptr;
    }
    sptr<ScreenSessionGroup> screenGroup = iter->second;
    if (!RemoveChildFromGroup(screen, screenGroup)) {
        return nullptr;
    }
    return screenGroup;
}

bool ScreenSessionManager::RemoveChildFromGroup(sptr<ScreenSession> screen, sptr<ScreenSessionGroup> screenGroup)
{
    bool res = screenGroup->RemoveChild(screen);
    if (!res) {
        WLOGE("RemoveFromGroupLocked. remove screen:%{public}" PRIu64" failed from screenGroup:%{public}" PRIu64".",
              screen->screenId_, screen->groupSmsId_);
        return false;
    }
    if (screenGroup->GetChildCount() == 0) {
        // Group removed, need to do something.
        smsScreenGroupMap_.erase(screenGroup->screenId_);

        WLOGE("SCB: RemoveFromGroupLocked. screenSessionMap_ remove screen:%{public}" PRIu64"", screenGroup->screenId_);
        std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
        screenSessionMap_.erase(screenGroup->screenId_);
    }
    return true;
}

DMError ScreenSessionManager::SetMirror(ScreenId screenId, std::vector<ScreenId> screens)
{
    WLOGI("SetMirror, screenId:%{public}" PRIu64"", screenId);
    sptr<ScreenSession> screen = GetScreenSession(screenId);
    if (screen == nullptr || screen->GetScreenProperty().GetScreenType() != ScreenType::REAL) {
        WLOGFE("screen is nullptr, or screenType is not real.");
        return DMError::DM_ERROR_NULLPTR;
    }
    screen->groupSmsId_ = 1;
    auto group = GetAbstractScreenGroup(screen->groupSmsId_);
    if (group == nullptr) {
        group = AddToGroupLocked(screen);
        if (group == nullptr) {
            WLOGFE("group is nullptr");
            return DMError::DM_ERROR_NULLPTR;
        }
        NotifyScreenGroupChanged(screen->ConvertToScreenInfo(), ScreenGroupChangeEvent::ADD_TO_GROUP);
    }
    Point point;
    std::vector<Point> startPoints;
    startPoints.insert(startPoints.begin(), screens.size(), point);
    bool filterMirroredScreen =
        group->combination_ == ScreenCombination::SCREEN_MIRROR && group->mirrorScreenId_ == screen->screenId_;
    group->mirrorScreenId_ = screen->screenId_;
    ChangeScreenGroup(group, screens, startPoints, filterMirroredScreen, ScreenCombination::SCREEN_MIRROR);
    WLOGFI("SetMirror success");
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
    std::map<ScreenId, bool> removeChildResMap;
    std::vector<ScreenId> addScreens;
    std::vector<Point> addChildPos;
    for (uint64_t i = 0; i != screens.size(); i++) {
        ScreenId screenId = screens[i];
        WLOGFI("ScreenId: %{public}" PRIu64"", screenId);
        auto screen = GetScreenSession(screenId);
        if (screen == nullptr) {
            WLOGFE("screen:%{public}" PRIu64" is nullptr", screenId);
            continue;
        }
        WLOGFI("Screen->groupSmsId_: %{public}" PRIu64"", screen->groupSmsId_);
        screen->groupSmsId_ = 1;
        if (filterScreen && screen->groupSmsId_ == group->screenId_ && group->HasChild(screen->screenId_)) {
            continue;
        }
        NotifyDisplayDestroy(screenId);
        auto originGroup = RemoveFromGroupLocked(screen);
        addChildPos.emplace_back(startPoints[i]);
        removeChildResMap[screenId] = originGroup != nullptr;
        addScreens.emplace_back(screenId);
    }
    group->combination_ = combination;
    AddScreenToGroup(group, addScreens, addChildPos, removeChildResMap);
}

void ScreenSessionManager::AddScreenToGroup(sptr<ScreenSessionGroup> group,
    const std::vector<ScreenId>& addScreens, const std::vector<Point>& addChildPos,
    std::map<ScreenId, bool>& removeChildResMap)
{
    std::vector<sptr<ScreenInfo>> addToGroup;
    std::vector<sptr<ScreenInfo>> removeFromGroup;
    std::vector<sptr<ScreenInfo>> changeGroup;
    for (uint64_t i = 0; i != addScreens.size(); i++) {
        ScreenId screenId = addScreens[i];
        sptr<ScreenSession> screen = GetScreenSession(screenId);
        if (screen == nullptr) {
            continue;
        }
        Point expandPoint = addChildPos[i];
        WLOGFI("screenId: %{public}" PRIu64", Point: %{public}d, %{public}d",
            screen->screenId_, expandPoint.posX_, expandPoint.posY_);
        bool addChildRes = group->AddChild(screen, expandPoint, GetScreenSession(GetDefaultAbstractScreenId()));
        if (removeChildResMap[screenId] && addChildRes) {
            changeGroup.emplace_back(screen->ConvertToScreenInfo());
            WLOGFD("changeGroup");
        } else if (removeChildResMap[screenId]) {
            WLOGFD("removeChild");
            removeFromGroup.emplace_back(screen->ConvertToScreenInfo());
        } else if (addChildRes) {
            WLOGFD("AddChild");
            addToGroup.emplace_back(screen->ConvertToScreenInfo());
        } else {
            WLOGFD("default, AddChild failed");
        }
        NotifyDisplayCreate(screen->ConvertToDisplayInfo());
    }

    NotifyScreenGroupChanged(removeFromGroup, ScreenGroupChangeEvent::REMOVE_FROM_GROUP);
    NotifyScreenGroupChanged(changeGroup, ScreenGroupChangeEvent::CHANGE_GROUP);
    NotifyScreenGroupChanged(addToGroup, ScreenGroupChangeEvent::ADD_TO_GROUP);
}

void ScreenSessionManager::RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens)
{
    WLOGFE("SCB: ScreenSessionManager::RemoveVirtualScreenFromGroup enter!");
    if (screens.empty()) {
        return;
    }
    std::vector<sptr<ScreenInfo>> removeFromGroup;
    for (ScreenId screenId : screens) {
        auto screen = GetScreenSession(screenId);
        if (screen == nullptr || screen->GetScreenProperty().GetScreenType() != ScreenType::VIRTUAL) {
            continue;
        }
        auto originGroup = GetAbstractScreenGroup(screen->groupSmsId_);
        if (originGroup == nullptr) {
            continue;
        }
        if (!originGroup->HasChild(screenId)) {
            continue;
        }
        RemoveFromGroupLocked(screen);
        removeFromGroup.emplace_back(screen->ConvertToScreenInfo());
    }
    NotifyScreenGroupChanged(removeFromGroup, ScreenGroupChangeEvent::REMOVE_FROM_GROUP);
}

const std::shared_ptr<RSDisplayNode> ScreenSessionManager::GetRSDisplayNodeByScreenId(ScreenId smsScreenId) const
{
    static std::shared_ptr<RSDisplayNode> notFound = nullptr;
    sptr<ScreenSession> screen = GetScreenSession(smsScreenId);
    if (screen == nullptr) {
        WLOGFE("SCB: ScreenSessionManager::GetRSDisplayNodeByScreenId screen == nullptr!");
        return notFound;
    }
    if (screen->GetDisplayNode() == nullptr) {
        WLOGFE("SCB: ScreenSessionManager::GetRSDisplayNodeByScreenId displayNode_ == nullptr!");
        return notFound;
    }
    WLOGI("GetRSDisplayNodeByScreenId: screen: %{public}" PRIu64", nodeId: %{public}" PRIu64" ",
        screen->screenId_, screen->GetDisplayNode()->GetId());
    return screen->GetDisplayNode();
}

std::shared_ptr<Media::PixelMap> ScreenSessionManager::GetScreenSnapshot(DisplayId displayId)
{
    ScreenId screenId = SCREEN_ID_INVALID;
    std::shared_ptr<RSDisplayNode> displayNode = nullptr;
    {
        std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
        for (auto sessionIt : screenSessionMap_) {
            auto screenSession = sessionIt.second;
            sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
            WLOGI("SCB: GetScreenSnapshot: displayId %{public}" PRIu64"", displayInfo->GetDisplayId());
            if (displayId == displayInfo->GetDisplayId()) {
                displayNode = screenSession->GetDisplayNode();
                screenId = sessionIt.first;
                break;
            }
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
    bool ret = rsInterface_.TakeSurfaceCapture(displayNode, callback);
    if (!ret) {
        WLOGFE("SCB: ScreenSessionManager::GetScreenSnapshot TakeSurfaceCapture failed");
        return nullptr;
    }
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
    WLOGFI("SCB: ScreenSessionManager::GetDisplaySnapshot ENTER!");
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:GetDisplaySnapshot(%" PRIu64")", displayId);
    auto res = GetScreenSnapshot(displayId);
    if (res != nullptr) {
        NotifyScreenshot(displayId);
    }
    return res;
}

bool ScreenSessionManager::OnRemoteDied(const sptr<IRemoteObject>& agent)
{
    if (agent == nullptr) {
        return false;
    }
    auto agentIter = screenAgentMap_.find(agent);
    if (agentIter != screenAgentMap_.end()) {
        while (screenAgentMap_[agent].size() > 0) {
            auto diedId = screenAgentMap_[agent][0];
            WLOGI("destroy screenId in OnRemoteDied: %{public}" PRIu64"", diedId);
            DMError res = DestroyVirtualScreen(diedId);
            if (res != DMError::DM_OK) {
                WLOGE("destroy failed in OnRemoteDied: %{public}" PRIu64"", diedId);
            }
        }
        screenAgentMap_.erase(agent);
    }
    return true;
}

std::vector<ScreenId> ScreenSessionManager::GetAllValidScreenIds(const std::vector<ScreenId>& screenIds) const
{
    std::vector<ScreenId> validScreenIds;
    for (ScreenId screenId : screenIds) {
        auto screenIdIter = std::find(validScreenIds.begin(), validScreenIds.end(), screenId);
        if (screenIdIter != validScreenIds.end()) {
            continue;
        }
        std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
        auto iter = screenSessionMap_.find(screenId);
        if (iter != screenSessionMap_.end() &&
                iter->second->GetScreenProperty().GetScreenType() != ScreenType::UNDEFINED) {
            validScreenIds.emplace_back(screenId);
        }
    }
    return validScreenIds;
}

sptr<ScreenGroupInfo> ScreenSessionManager::GetScreenGroupInfoById(ScreenId screenId)
{
    auto screenSessionGroup = GetAbstractScreenGroup(screenId);
    if (screenSessionGroup == nullptr) {
        WLOGE("SCB: GetScreenGroupInfoById cannot find screenGroupInfo: %{public}" PRIu64"", screenId);
        return nullptr;
    }
    return screenSessionGroup->ConvertToScreenGroupInfo();
}

void ScreenSessionManager::NotifyScreenConnected(sptr<ScreenInfo> screenInfo)
{
    if (screenInfo == nullptr) {
        WLOGFE("SCB: NotifyScreenConnected error, screenInfo is nullptr.");
        return;
    }
    auto task = [=] {
        WLOGFI("SCB: NotifyScreenConnected,  screenId:%{public}" PRIu64"", screenInfo->GetScreenId());
        OnScreenConnect(screenInfo);
    };
    taskScheduler_->PostAsyncTask(task);
}

void ScreenSessionManager::NotifyScreenDisconnected(ScreenId screenId)
{
    auto task = [=] {
        WLOGFI("NotifyScreenDisconnected,  screenId:%{public}" PRIu64"", screenId);
        OnScreenDisconnect(screenId);
    };
    taskScheduler_->PostAsyncTask(task);
}

void ScreenSessionManager::NotifyDisplayCreate(sptr<DisplayInfo> displayInfo)
{
    if (displayInfo == nullptr) {
        return;
    }
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
    if (agents.empty()) {
        return;
    }
    WLOGFI("NotifyDisplayCreate");
    for (auto& agent : agents) {
        agent->OnDisplayCreate(displayInfo);
    }
}

void ScreenSessionManager::NotifyDisplayDestroy(DisplayId displayId)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
    if (agents.empty()) {
        return;
    }
    WLOGFI("NotifyDisplayDestroy");
    for (auto& agent : agents) {
        agent->OnDisplayDestroy(displayId);
    }
}

void ScreenSessionManager::NotifyScreenGroupChanged(
    const sptr<ScreenInfo>& screenInfo, ScreenGroupChangeEvent event)
{
    if (screenInfo == nullptr) {
        WLOGFE("screenInfo is nullptr.");
        return;
    }
    std::string trigger = SysCapUtil::GetClientName();
    auto task = [=] {
        WLOGFI("SCB: screenId:%{public}" PRIu64", trigger:[%{public}s]", screenInfo->GetScreenId(), trigger.c_str());
        OnScreenGroupChange(trigger, screenInfo, event);
    };
    taskScheduler_->PostAsyncTask(task);
}

void ScreenSessionManager::NotifyScreenGroupChanged(
    const std::vector<sptr<ScreenInfo>>& screenInfo, ScreenGroupChangeEvent event)
{
    if (screenInfo.empty()) {
        return;
    }
    std::string trigger = SysCapUtil::GetClientName();
    auto task = [=] {
        WLOGFI("SCB: trigger:[%{public}s]", trigger.c_str());
        OnScreenGroupChange(trigger, screenInfo, event);
    };
    taskScheduler_->PostAsyncTask(task);
}

void ScreenSessionManager::NotifyPrivateSessionStateChanged(bool hasPrivate)
{
    if (hasPrivate == screenPrivacyStates) {
        WLOGFD("screen session state is not changed, return");
        return;
    }
    screenPrivacyStates = hasPrivate;
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::PRIVATE_WINDOW_LISTENER);
    if (agents.empty()) {
        return;
    }
    WLOGI("PrivateSession status : %{public}u", hasPrivate);
    for (auto& agent : agents) {
        agent->NotifyPrivateWindowStateChanged(hasPrivate);
    }
}

void ScreenSessionManager::SetScreenPrivacyState(ScreenId id, bool hasPrivate)
{
    auto screenSession = GetScreenSession(id);
    screenSession->SetPrivateSessionForeground(hasPrivate);
    NotifyPrivateSessionStateChanged(hasPrivate);
}

ScreenId ScreenSessionManager::GetScreenSessionIdBySceneSessionId(uint32_t persistentId)
{
    return 0; // current only has one screen
}

DMError ScreenSessionManager::HasPrivateWindow(DisplayId id, bool& hasPrivateWindow)
{
    // delete permission
    std::vector<ScreenId> screenIds = GetAllScreenIds();
    auto iter = std::find(screenIds.begin(), screenIds.end(), id);
    if (iter == screenIds.end()) {
        WLOGFE("invalid displayId");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    auto screenSession = GetScreenSession(id);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_NULLPTR;
    }
    hasPrivateWindow = screenSession->HasPrivateSessionForeground();
    WLOGFD("id: %{public}" PRIu64" has private window: %{public}u", id, static_cast<uint32_t>(hasPrivateWindow));
    return DMError::DM_OK;
}

void ScreenSessionManager::OnScreenGroupChange(const std::string& trigger,
    const sptr<ScreenInfo>& screenInfo, ScreenGroupChangeEvent groupEvent)
{
    if (screenInfo == nullptr) {
        return;
    }
    std::vector<sptr<ScreenInfo>> screenInfos;
    screenInfos.push_back(screenInfo);
    OnScreenGroupChange(trigger, screenInfos, groupEvent);
}

void ScreenSessionManager::OnScreenGroupChange(const std::string& trigger,
    const std::vector<sptr<ScreenInfo>>& screenInfos, ScreenGroupChangeEvent groupEvent)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
    std::vector<sptr<ScreenInfo>> infos;
    for (auto& screenInfo : screenInfos) {
        if (screenInfo != nullptr) {
            infos.emplace_back(screenInfo);
        }
    }
    if (agents.empty() || infos.empty()) {
        return;
    }
    for (auto& agent : agents) {
        agent->OnScreenGroupChange(trigger, infos, groupEvent);
    }
}

void ScreenSessionManager::OnScreenConnect(const sptr<ScreenInfo> screenInfo)
{
    if (screenInfo == nullptr) {
        return;
    }
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
    if (agents.empty()) {
        return;
    }
    WLOGFI("SCB: OnScreenConnect");
    for (auto& agent : agents) {
        agent->OnScreenConnect(screenInfo);
    }
}

void ScreenSessionManager::OnScreenDisconnect(ScreenId screenId)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
    if (agents.empty()) {
        return;
    }
    WLOGFI("SCB: OnScreenDisconnect");
    for (auto& agent : agents) {
        agent->OnScreenDisconnect(screenId);
    }
}

void ScreenSessionManager::OnScreenshot(sptr<ScreenshotInfo> info)
{
    if (info == nullptr) {
        return;
    }
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREENSHOT_EVENT_LISTENER);
    if (agents.empty()) {
        return;
    }
    WLOGFI("SCB: onScreenshot");
    for (auto& agent : agents) {
        agent->OnScreenshot(info);
    }
}

sptr<CutoutInfo> ScreenSessionManager::GetCutoutInfo(DisplayId displayId)
{
    return screenCutoutController_ ? screenCutoutController_->GetScreenCutoutInfo() : nullptr;
}

void ScreenSessionManager::SetDisplayBoundary(const sptr<ScreenSession> screenSession)
{
    if (screenSession && screenCutoutController_) {
        RectF rect =
            screenCutoutController_->CalculateCurvedCompression(screenSession->GetScreenProperty());
        if (!rect.IsEmpty()) {
            screenSession->SetDisplayBoundary(rect, screenCutoutController_->GetOffsetY());
        }
    } else {
        WLOGFW("screenSession or screenCutoutController_ is null");
    }
}

std::string ScreenSessionManager::TransferTypeToString(ScreenType type) const
{
    std::string screenType;
    switch (type) {
        case ScreenType::REAL:
            screenType = "REAL";
            break;
        case ScreenType::VIRTUAL:
            screenType = "VIRTUAL";
            break;
        default:
            screenType = "UNDEFINED";
            break;
    }
    return screenType;
}

void ScreenSessionManager::DumpAllScreensInfo(std::string& dumpInfo)
{
    std::ostringstream oss;
    oss << "--------------------------------------Free Screen"
        << "--------------------------------------"
        << std::endl;
    oss << "ScreenName           Type     IsGroup DmsId RsId                 ActiveIdx VPR Rotation Orientation "
        << "RequestOrientation NodeId               "
        << std::endl;
    for (auto sessionIt : screenSessionMap_) {
        auto screenSession = sessionIt.second;
        if (screenSession == nullptr) {
            continue;
        }
        sptr<ScreenInfo> screenInfo = GetScreenInfoById(sessionIt.first);
        if (screenInfo == nullptr) {
            continue;
        }
        std::string screenType = TransferTypeToString(screenInfo->GetType());
        NodeId nodeId = (screenSession->GetDisplayNode() == nullptr) ?
            SCREEN_ID_INVALID : screenSession->GetDisplayNode()->GetId();
        oss << std::left << std::setw(21) << screenInfo->GetName()
            << std::left << std::setw(9) << screenType
            << std::left << std::setw(8) << (screenSession->isScreenGroup_ ? "true" : "false")
            << std::left << std::setw(6) << screenSession->screenId_
            << std::left << std::setw(21) << screenSession->rsId_
            << std::left << std::setw(10) << screenSession->activeIdx_
            << std::left << std::setw(4) << screenInfo->GetVirtualPixelRatio()
            << std::left << std::setw(9) << static_cast<uint32_t>(screenInfo->GetRotation())
            << std::left << std::setw(12) << static_cast<uint32_t>(screenInfo->GetOrientation())
            << std::left << std::setw(19) << static_cast<uint32_t>(screenSession->GetScreenRequestedOrientation())
            << std::left << std::setw(21) << nodeId
            << std::endl;
    }
    oss << "total screen num: " << screenSessionMap_.size() << std::endl;
    dumpInfo.append(oss.str());
}

void ScreenSessionManager::DumpSpecialScreenInfo(ScreenId id, std::string& dumpInfo)
{
    std::ostringstream oss;
    sptr<ScreenSession> session = GetScreenSession(id);
    if (!session) {
        WLOGFE("Get screen session failed.");
        oss << "This screen id is invalid.";
        dumpInfo.append(oss.str());
        return;
    }
    sptr<ScreenInfo> screenInfo = GetScreenInfoById(id);
    if (screenInfo == nullptr) {
        return;
    }
    std::string screenType = TransferTypeToString(screenInfo->GetType());
    NodeId nodeId = (session->GetDisplayNode() == nullptr) ?
        SCREEN_ID_INVALID : session->GetDisplayNode()->GetId();
    oss << "ScreenName: " << screenInfo->GetName() << std::endl;
    oss << "Type: " << screenType << std::endl;
    oss << "IsGroup: " << (session->isScreenGroup_ ? "true" : "false") << std::endl;
    oss << "DmsId: " << id << std::endl;
    oss << "RsId: " << session->rsId_ << std::endl;
    oss << "ActiveIdx: " << session->activeIdx_ << std::endl;
    oss << "VPR: " << screenInfo->GetVirtualPixelRatio() << std::endl;
    oss << "Rotation: " << static_cast<uint32_t>(screenInfo->GetRotation()) << std::endl;
    oss << "Orientation: " << static_cast<uint32_t>(screenInfo->GetOrientation()) << std::endl;
    oss << "RequestOrientation: " << static_cast<uint32_t>(session->GetScreenRequestedOrientation()) << std::endl;
    oss << "NodeId: " << nodeId << std::endl;
    dumpInfo.append(oss.str());
}
} // namespace OHOS::Rosen
