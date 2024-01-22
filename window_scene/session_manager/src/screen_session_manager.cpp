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

#include <csignal>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <string_ex.h>
#include <unique_fd.h>

#include <hitrace_meter.h>
#include <parameter.h>
#include <parameters.h>
#include <system_ability_definition.h>
#include <transaction/rs_interfaces.h>
#include <xcollie/watchdog.h>
#include <hisysevent.h>

#include "dm_common.h"
#include "scene_board_judgement.h"
#include "session_permission.h"
#include "screen_scene_config.h"
#include "surface_capture_future.h"
#include "sys_cap_util.h"
#include "permission.h"
#include "window_manager_hilog.h"
#include "screen_rotation_property.h"
#include "screen_sensor_connector.h"
#include "screen_setting_helper.h"
#include "mock_session_manager_service.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSessionManager" };
const std::string SCREEN_SESSION_MANAGER_THREAD = "OS_ScreenSessionManager";
const std::string SCREEN_SESSION_MANAGER_SCREEN_POWER_THREAD = "OS_ScreenSessionManager_ScreenPower";
const std::string SCREEN_CAPTURE_PERMISSION = "ohos.permission.CAPTURE_SCREEN";
const std::string BOOTEVENT_BOOT_COMPLETED = "bootevent.boot.completed";
const int SLEEP_US = 48 * 1000; // 48ms
const std::u16string DEFAULT_USTRING = u"error";
const std::string DEFAULT_STRING = "error";
const std::string ARG_DUMP_HELP = "-h";
const std::string ARG_DUMP_ALL = "-a";
const std::string ARG_DUMP_SCREEN = "-s";
const std::string ARG_FOLD_DISPLAY_FULL = "-f";
const std::string ARG_FOLD_DISPLAY_MAIN = "-m";
const std::string ARG_LOCK_FOLD_DISPLAY_STATUS = "-l";
const std::string ARG_UNLOCK_FOLD_DISPLAY_STATUS = "-u";
const ScreenId SCREEN_ID_FULL = 0;
const ScreenId SCREEN_ID_MAIN = 5;
} // namespace

WM_IMPLEMENT_SINGLE_INSTANCE(ScreenSessionManager)

const bool REGISTER_RESULT = !SceneBoardJudgement::IsSceneBoardEnabled() ? false :
    SystemAbility::MakeAndRegisterAbility(&ScreenSessionManager::GetInstance());

ScreenSessionManager::ScreenSessionManager()
    : SystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID, true), rsInterface_(RSInterfaces::GetInstance())
{
    LoadScreenSceneXml();
    taskScheduler_ = std::make_shared<TaskScheduler>(SCREEN_SESSION_MANAGER_THREAD);
    screenPowerTaskScheduler_ = std::make_shared<TaskScheduler>(SCREEN_SESSION_MANAGER_SCREEN_POWER_THREAD);
    screenCutoutController_ = new (std::nothrow) ScreenCutoutController();
    sessionDisplayPowerController_ = new SessionDisplayPowerController(mutex_,
        std::bind(&ScreenSessionManager::NotifyDisplayStateChange, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    bool foldScreenFlag = system::GetParameter("const.window.foldscreen.type", "") != "";
    if (foldScreenFlag) {
        foldScreenController_ = new (std::nothrow) FoldScreenController(displayInfoMutex_, screenPowerTaskScheduler_);
        foldScreenController_->SetOnBootAnimation(true);
        rsInterface_.SetScreenCorrection(SCREEN_ID_FULL, ScreenRotation::ROTATION_270);
        SetFoldScreenPowerInit([&]() {
            int64_t timeStamp = 50;
            #ifdef TP_FEATURE_ENABLE
            int32_t tpType = 12;
            std::string fullTpChange = "0";
            std::string mainTpChange = "1";
            #endif
            if (rsInterface_.GetActiveScreenId() == SCREEN_ID_FULL) {
                WLOGFI("ScreenSessionManager Fold Screen Power Full animation Init 1.");
                #ifdef TP_FEATURE_ENABLE
                rsInterface_.SetTpFeatureConfig(tpType, mainTpChange.c_str());
                #endif
                rsInterface_.SetScreenPowerStatus(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_OFF);
                rsInterface_.SetScreenPowerStatus(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_ON);
                std::this_thread::sleep_for(std::chrono::milliseconds(timeStamp));
                WLOGFI("ScreenSessionManager Fold Screen Power Full animation Init 2.");
                #ifdef TP_FEATURE_ENABLE
                rsInterface_.SetTpFeatureConfig(tpType, fullTpChange.c_str());
                #endif
                rsInterface_.SetScreenPowerStatus(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_OFF);
                rsInterface_.SetScreenPowerStatus(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_ON);
            } else if (rsInterface_.GetActiveScreenId() == SCREEN_ID_MAIN) {
                WLOGFI("ScreenSessionManager Fold Screen Power Main animation Init 3.");
                #ifdef TP_FEATURE_ENABLE
                rsInterface_.SetTpFeatureConfig(tpType, fullTpChange.c_str());
                #endif
                rsInterface_.SetScreenPowerStatus(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_OFF);
                rsInterface_.SetScreenPowerStatus(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_ON);
                std::this_thread::sleep_for(std::chrono::milliseconds(timeStamp));

                WLOGFI("ScreenSessionManager Fold Screen Power Main animation Init 4.");
                #ifdef TP_FEATURE_ENABLE
                rsInterface_.SetTpFeatureConfig(tpType, mainTpChange.c_str());
                #endif
                rsInterface_.SetScreenPowerStatus(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_OFF);
                rsInterface_.SetScreenPowerStatus(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_ON);
            } else {
                WLOGFI("ScreenSessionManager Fold Screen Power Init, invalid active screen id");
            }
            foldScreenController_->SetOnBootAnimation(false);
        });
    }
    WatchParameter(BOOTEVENT_BOOT_COMPLETED.c_str(), BootFinishedCallback, this);
}

void ScreenSessionManager::Init()
{
    constexpr uint64_t interval = 5 * 1000; // 5 second
    if (HiviewDFX::Watchdog::GetInstance().AddThread(
        SCREEN_SESSION_MANAGER_THREAD, taskScheduler_->GetEventHandler(), interval)) {
        WLOGFW("Add thread %{public}s to watchdog failed.", SCREEN_SESSION_MANAGER_THREAD.c_str());
    }

    if (HiviewDFX::Watchdog::GetInstance().AddThread(
        SCREEN_SESSION_MANAGER_SCREEN_POWER_THREAD, screenPowerTaskScheduler_->GetEventHandler(), interval)) {
        WLOGFW("Add thread %{public}s to watchdog failed.", SCREEN_SESSION_MANAGER_SCREEN_POWER_THREAD.c_str());
    }

    RegisterScreenChangeListener();

    bool isPcDevice = system::GetParameter("const.product.devicetype", "unknown") == "2in1";
    if (isPcDevice) {
        WLOGFI("Current device type not support SetSensorSubscriptionEnabled.");
    } else {
        SetSensorSubscriptionEnabled();
    }
}

void ScreenSessionManager::OnStart()
{
    WLOGFI("begin");
    Init();
    sptr<ScreenSessionManager> dms(this);
    dms->IncStrongRef(nullptr);
    if (!Publish(dms)) {
        WLOGFE("Publish failed");
        return;
    }
    WLOGFI("end");
}

DMError ScreenSessionManager::RegisterDisplayManagerAgent(
    const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type)
{
    if (type == DisplayManagerAgentType::SCREEN_EVENT_LISTENER && !SessionPermission::IsSystemCalling()
        && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("register display manager agent permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if (type < DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER
        || type > DisplayManagerAgentType::DISPLAY_MODE_CHANGED_LISTENER) {
        WLOGFE("SCB:DisplayManagerAgentType: %{public}u", static_cast<uint32_t>(type));
        return DMError::DM_ERROR_INVALID_PARAM;
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
            defaultDpi = densityDpi;
            cachedSettingDpi_ = defaultDpi;
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
        WLOGFD("vtBoundary.size=%{public}u", static_cast<uint32_t>(vtBoundary.size()));
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
        taskScheduler_->PostAsyncTask(task, "RegisterScreenChangeListener", 50); // Retry after 50 ms.
    }
}

void ScreenSessionManager::OnVirtualScreenChange(ScreenId screenId, ScreenEvent screenEvent)
{
    WLOGFI("Notify scb virtual screen change, ScreenId: %{public}" PRIu64 ", ScreenEvent: %{public}d", screenId,
        static_cast<int>(screenEvent));
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is nullptr");
        return;
    }
    if (screenEvent == ScreenEvent::CONNECTED) {
        if (clientProxy_) {
            clientProxy_->OnScreenConnectionChanged(screenId, ScreenEvent::CONNECTED,
                screenSession->GetRSScreenId(), screenSession->GetName());
        }
        return;
    }
    if (screenEvent == ScreenEvent::DISCONNECTED) {
        if (clientProxy_) {
            clientProxy_->OnScreenConnectionChanged(screenId, ScreenEvent::DISCONNECTED,
                screenSession->GetRSScreenId(), screenSession->GetName());
        }
    }
}

void ScreenSessionManager::FreeDisplayMirrorNodeInner(const sptr<ScreenSession> mirrorSession)
{
    bool phyMirrorEnable = system::GetParameter("const.product.devicetype", "unknown") == "phone";
    if (mirrorSession == nullptr || !phyMirrorEnable) {
        return;
    }
    std::shared_ptr<RSDisplayNode> displayNode = mirrorSession->GetDisplayNode();
    if (displayNode == nullptr) {
        return;
    }
    displayNode->RemoveFromTree();
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        WLOGFI("FreeDisplayMirrorNodeInner free displayNode");
        transactionProxy->FlushImplicitTransaction();
    }
}

void ScreenSessionManager::OnScreenChange(ScreenId screenId, ScreenEvent screenEvent)
{
    WLOGFI("screenId: %{public}" PRIu64 " screenEvent: %{public}d", screenId, static_cast<int>(screenEvent));
    bool phyMirrorEnable = system::GetParameter("const.product.devicetype", "unknown") == "phone";
    auto screenSession = GetOrCreateScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is nullptr");
        return;
    }

    if (foldScreenController_ != nullptr) {
        screenSession->SetFoldScreen(true);
    }

    if (screenEvent == ScreenEvent::CONNECTED) {
        if (foldScreenController_ != nullptr) {
            if (screenId == 0 && clientProxy_) {
                clientProxy_->OnScreenConnectionChanged(screenId, ScreenEvent::CONNECTED,
                    screenSession->GetRSScreenId(), screenSession->GetName());
            }
            return;
        }

        if (clientProxy_ && !phyMirrorEnable) {
            clientProxy_->OnScreenConnectionChanged(screenId, ScreenEvent::CONNECTED,
                screenSession->GetRSScreenId(), screenSession->GetName());
        }
        return;
    }
    if (screenEvent == ScreenEvent::DISCONNECTED) {
        if (phyMirrorEnable) {
            FreeDisplayMirrorNodeInner(screenSession);
        }
        if (clientProxy_) {
            clientProxy_->OnScreenConnectionChanged(screenId, ScreenEvent::DISCONNECTED,
                screenSession->GetRSScreenId(), screenSession->GetName());
        }
        {
            std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
            screenSessionMap_.erase(screenId);
        }
        {
            std::lock_guard<std::recursive_mutex> lock_phy(phyScreenPropMapMutex_);
            phyScreenPropMap_.erase(screenId);
        }
    }
}

sptr<ScreenSession> ScreenSessionManager::GetScreenSession(ScreenId screenId) const
{
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    auto iter = screenSessionMap_.find(screenId);
    if (iter == screenSessionMap_.end()) {
        WLOGFD("Error found screen session with id: %{public}" PRIu64"", screenId);
        return nullptr;
    }
    return iter->second;
}

sptr<ScreenSession> ScreenSessionManager::GetDefaultScreenSession()
{
    GetDefaultScreenId();
    return GetScreenSession(defaultScreenId_);
}

sptr<DisplayInfo> ScreenSessionManager::GetDefaultDisplayInfo()
{
    std::lock_guard<std::recursive_mutex> lock_info(displayInfoMutex_);
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
    WLOGFI("GetDisplayInfoById enter, displayId: %{public}" PRIu64" ", displayId);
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (auto sessionIt : screenSessionMap_) {
        auto screenSession = sessionIt.second;
        if (screenSession == nullptr) {
            WLOGFE("GetDisplayInfoById screenSession is nullptr, ScreenId: %{public}" PRIu64 "", sessionIt.first);
            continue;
        }
        sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
        if (displayInfo == nullptr) {
            WLOGFE("ConvertToDisplayInfo error, displayInfo is nullptr.");
            continue;
        }
        if (displayId == displayInfo->GetDisplayId()) {
            WLOGFI("GetDisplayInfoById success");
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
        if (screenSession == nullptr) {
            WLOGFE("GetDisplayInfoByScreen screenSession is nullptr, ScreenId:%{public}" PRIu64"", sessionIt.first);
            continue;
        }
        sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
        if (displayInfo == nullptr) {
            WLOGFE("GetDisplayInfoByScreen error, displayInfo is nullptr.");
            continue;
        }
        if (screenId == displayInfo->GetScreenId()) {
            return displayInfo;
        }
    }
    WLOGFE("SCB: ScreenSessionManager::GetDisplayInfoByScreen failed.");
    return nullptr;
}

std::vector<DisplayId> ScreenSessionManager::GetAllDisplayIds()
{
    WLOGFI("GetAllDisplayIds enter");
    std::vector<DisplayId> res;
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (auto sessionIt : screenSessionMap_) {
        auto screenSession = sessionIt.second;
        if (screenSession == nullptr) {
            WLOGFE("GetAllDisplayIds screenSession is nullptr, ScreenId:%{public}" PRIu64"", sessionIt.first);
            continue;
        }
        sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
        if (displayInfo == nullptr) {
            WLOGFE("GetAllDisplayIds error, displayInfo is nullptr.");
            continue;
        }
        DisplayId displayId = displayInfo->GetDisplayId();
        res.push_back(displayId);
    }
    return res;
}

sptr<ScreenInfo> ScreenSessionManager::GetScreenInfoById(ScreenId screenId)
{
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("SCB: ScreenSessionManager::GetScreenInfoById permission denied!");
        return nullptr;
    }
    auto screenSession = GetScreenSession(screenId);
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
        NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(), DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
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
    taskScheduler_->PostAsyncTask(task, "NotifyScreenChanged:SID:" + std::to_string(screenInfo->GetScreenId()));
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
    std::map<DisplayId, sptr<DisplayInfo>> emptyMap;
    NotifyDisplayStateChange(GetDefaultScreenId(), screenSession->ConvertToDisplayInfo(),
        emptyMap, DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE);
    NotifyScreenChanged(screenSession->ConvertToScreenInfo(), ScreenChangeEvent::VIRTUAL_PIXEL_RATIO_CHANGED);
    NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(),
        DisplayChangeEvent::DISPLAY_VIRTUAL_PIXEL_RATIO_CHANGED);
    return DMError::DM_OK;
}

DMError ScreenSessionManager::SetResolution(ScreenId screenId, uint32_t width, uint32_t height, float virtualPixelRatio)
{
    WLOGI("SetResolution ScreenId: %{public}" PRIu64 ", w: %{public}u, h: %{public}u, virtualPixelRatio: %{public}f",
        screenId, width, height, virtualPixelRatio);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("SetResolution: invalid screenId");
        return DMError::DM_ERROR_NULLPTR;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        WLOGFE("SetResolution: Get ScreenSession failed");
        return DMError::DM_ERROR_NULLPTR;
    }
    sptr<SupportedScreenModes> screenSessionModes = screenSession->GetActiveScreenMode();
    if (screenSessionModes == nullptr) {
        return DMError::DM_ERROR_NULLPTR;
    }
    if (width <= 0 || width > screenSessionModes->width_ ||
        height <= 0 || height > screenSessionModes->height_ ||
        virtualPixelRatio < (static_cast<float>(DOT_PER_INCH_MINIMUM_VALUE) / DOT_PER_INCH) ||
        virtualPixelRatio > (static_cast<float>(DOT_PER_INCH_MAXIMUM_VALUE) / DOT_PER_INCH)) {
        WLOGFE("SetResolution invalid param! w:%{public}u h:%{public}u min:%{public}f max:%{public}f",
            screenSessionModes->width_,
            screenSessionModes->height_,
            static_cast<float>(DOT_PER_INCH_MINIMUM_VALUE) / DOT_PER_INCH,
            static_cast<float>(DOT_PER_INCH_MAXIMUM_VALUE) / DOT_PER_INCH);
        return DMError::DM_ERROR_INVALID_PARAM;
    }

    DMError ret = SetVirtualPixelRatio(screenId, virtualPixelRatio);
    if (ret != DMError::DM_OK) {
        WLOGFE("Failed to setVirtualPixelRatio when settingResolution");
    }
    {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:SetResolution(%" PRIu64", %u, %u, %f)",
            screenId, width, height, virtualPixelRatio);
        screenSession->UpdatePropertyByResolution(width, height);
        screenSession->PropertyChange(screenSession->GetScreenProperty(), ScreenPropertyChangeReason::CHANGE_MODE);
        NotifyScreenChanged(screenSession->ConvertToScreenInfo(), ScreenChangeEvent::CHANGE_MODE);
        NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(), DisplayChangeEvent::DISPLAY_SIZE_CHANGED);
    }
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

sptr<ScreenSession> ScreenSessionManager::GetScreenSessionInner(ScreenId screenId, ScreenProperty property)
{
    bool phyMirrorEnable = system::GetParameter("const.product.devicetype", "unknown") == "phone";
    sptr<ScreenSession> session = nullptr;
    ScreenId defScreenId = GetDefaultScreenId();
    if (phyMirrorEnable && screenId != defScreenId) {
        NodeId nodeId = 0;
        auto sIt = screenSessionMap_.find(defScreenId);
        if (sIt != screenSessionMap_.end() && sIt->second != nullptr && sIt->second->GetDisplayNode() != nullptr) {
            nodeId = sIt->second->GetDisplayNode()->GetId();
        }
        WLOGFI("GetScreenSessionInner: nodeId:%{public}" PRIu64 "", nodeId);
        session = new ScreenSession(screenId, property, nodeId, defScreenId);
    } else {
        session = new ScreenSession(screenId, property, defScreenId);
    }
    return session;
}

sptr<ScreenSession> ScreenSessionManager::GetOrCreateScreenSession(ScreenId screenId)
{
    WLOGFI("SCB: ScreenSessionManager::GetOrCreateScreenSession ENTER");
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    auto sessionIt = screenSessionMap_.find(screenId);
    if (sessionIt != screenSessionMap_.end()) {
        return sessionIt->second;
    }

    ScreenId rsId = screenId;
    screenIdManager_.UpdateScreenId(rsId, screenId);

    auto screenMode = rsInterface_.GetScreenActiveMode(screenId);
    auto screenBounds = RRect({ 0, 0, screenMode.GetScreenWidth(), screenMode.GetScreenHeight() }, 0.0f, 0.0f);
    auto screenRefreshRate = screenMode.GetScreenRefreshRate();
    auto screenCapability = rsInterface_.GetScreenCapability(screenId);
    ScreenProperty property;
    property.SetRotation(0.0f);
    property.SetPhyWidth(screenCapability.GetPhyWidth());
    property.SetPhyHeight(screenCapability.GetPhyHeight());
    property.SetPhyBounds(screenBounds);
    property.SetBounds(screenBounds);
    property.SetAvailableArea({0, 0, screenMode.GetScreenWidth(), screenMode.GetScreenHeight()});
    if (isDensityDpiLoad_) {
        property.SetVirtualPixelRatio(densityDpi_);
        property.SetDefaultDensity(densityDpi_);
    } else {
        property.UpdateVirtualPixelRatio(screenBounds);
    }
    property.SetRefreshRate(screenRefreshRate);

    if (foldScreenController_ != nullptr && screenId == 0) {
        screenBounds = RRect({ 0, 0, screenMode.GetScreenHeight(), screenMode.GetScreenWidth() }, 0.0f, 0.0f);
        property.SetBounds(screenBounds);
    }
    property.CalcDefaultDisplayOrientation();

    {
        std::lock_guard<std::recursive_mutex> lock_phy(phyScreenPropMapMutex_);
        phyScreenPropMap_[screenId] = property;
    }

    if (foldScreenController_ != nullptr) {
        // sensor may earlier than screen connect, when physical screen property changed, update
        foldScreenController_->UpdateForPhyScreenPropertyChange();
        /* folder screen outer screenId is 5 */
        if (screenId == 5) {
            return nullptr;
        }
    }

    sptr<ScreenSession> session = GetScreenSessionInner(screenId, property);
    session->RegisterScreenChangeListener(this);
    InitAbstractScreenModesInfo(session);
    session->groupSmsId_ = 1;
    screenSessionMap_[screenId] = session;
    SetHdrFormats(screenId, session);
    SetColorSpaces(screenId, session);
    return session;
}

void ScreenSessionManager::SetHdrFormats(ScreenId screenId, sptr<ScreenSession>& session)
{
    WLOGFI("SCB: ScreenSessionManager::SetHdrFormats %{public}" PRIu64, screenId);
    std::vector<ScreenHDRFormat> rsHdrFormat;
    auto status = rsInterface_.GetScreenSupportedHDRFormats(screenId, rsHdrFormat);
    if (static_cast<StatusCode>(status) != StatusCode::SUCCESS) {
        WLOGFE("get hdr format failed! status code: %{public}d", status);
    } else {
        std::vector<uint32_t> hdrFormat(rsHdrFormat.size());
        std::transform(rsHdrFormat.begin(), rsHdrFormat.end(), hdrFormat.begin(), [](int val) {
            return static_cast<uint32_t>(val);
        });
        session->SetHdrFormats(std::move(hdrFormat));
    }
}

void ScreenSessionManager::SetColorSpaces(ScreenId screenId, sptr<ScreenSession>& session)
{
    WLOGFI("SCB: ScreenSessionManager::SetColorSpaces %{public}" PRIu64, screenId);
    std::vector<GraphicCM_ColorSpaceType> rsColorSpace;
    auto status = rsInterface_.GetScreenSupportedColorSpaces(screenId, rsColorSpace);
    if (static_cast<StatusCode>(status) != StatusCode::SUCCESS) {
        WLOGFE("get color space failed! status code: %{public}d", status);
    } else {
        std::vector<uint32_t> colorSpace(rsColorSpace.size());
        std::transform(rsColorSpace.begin(), rsColorSpace.end(), colorSpace.begin(), [](int val) {
            return static_cast<uint32_t>(val);
        });
        session->SetColorSpaces(std::move(colorSpace));
    }
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
    WLOGFI("ScreenSessionManager::WakeUpBegin remove suspend begin task");
    blockScreenPowerChange_ = false;
    taskScheduler_->RemoveTask("suspendBeginTask");
    if (reason == PowerStateChangeReason::STATE_CHANGE_REASON_COLLABORATION) {
        isMultiScreenCollaboration_ = true;
        return true;
    }
    return NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP, EventStatus::BEGIN, reason);
}

bool ScreenSessionManager::WakeUpEnd()
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:WakeUpEnd");
    if (isMultiScreenCollaboration_) {
        isMultiScreenCollaboration_ = false;
        return true;
    }
    return NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP, EventStatus::END,
        PowerStateChangeReason::STATE_CHANGE_REASON_INIT);
}

bool ScreenSessionManager::SuspendBegin(PowerStateChangeReason reason)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:SuspendBegin(%u)", reason);
    WLOGFI("ScreenSessionManager::SuspendBegin block screen power change is true");
    blockScreenPowerChange_ = true;
    auto suspendBeginTask = [this]() {
        WLOGFI("ScreenSessionManager::SuspendBegin delay task start");
        blockScreenPowerChange_ = false;
        SetScreenPower(ScreenPowerStatus::POWER_STATUS_OFF, PowerStateChangeReason::STATE_CHANGE_REASON_INIT);
    };
    taskScheduler_->PostTask(suspendBeginTask, "suspendBeginTask", 1500);
    sessionDisplayPowerController_->SuspendBegin(reason);
    if (reason == PowerStateChangeReason::STATE_CHANGE_REASON_COLLABORATION) {
        isMultiScreenCollaboration_ = true;
        return true;
    }
    return NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP, EventStatus::BEGIN, reason);
}

bool ScreenSessionManager::SuspendEnd()
{
    WLOGFI("ScreenSessionManager::SuspendEnd enter");
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:SuspendEnd");
    blockScreenPowerChange_ = false;
    if (isMultiScreenCollaboration_) {
        isMultiScreenCollaboration_ = false;
        return true;
    }
    return NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP, EventStatus::END,
        PowerStateChangeReason::STATE_CHANGE_REASON_INIT);
}

bool ScreenSessionManager::SetDisplayState(DisplayState state)
{
    WLOGFI("ScreenSessionManager::SetDisplayState enter");
    return sessionDisplayPowerController_->SetDisplayState(state);
}

void ScreenSessionManager::NotifyDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    if (clientProxy_) {
        clientProxy_->OnDisplayStateChanged(defaultDisplayId, displayInfo, displayInfoMap, type);
    }
}

void ScreenSessionManager::NotifyScreenshot(DisplayId displayId)
{
    if (clientProxy_) {
        clientProxy_->OnScreenshot(displayId);
    }
}

bool ScreenSessionManager::SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason)
{
    WLOGFI("SetSpecifiedScreenPower: screen id:%{public}" PRIu64 ", state:%{public}u", screenId, state);

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

    rsInterface_.SetScreenPowerStatus(screenId, status);
    if (reason == PowerStateChangeReason::STATE_CHANGE_REASON_COLLABORATION) {
        return true;
    }
    return NotifyDisplayPowerEvent(state == ScreenPowerState::POWER_ON ? DisplayPowerEvent::DISPLAY_ON :
        DisplayPowerEvent::DISPLAY_OFF, EventStatus::END, reason);
}

bool ScreenSessionManager::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    ScreenPowerStatus status;
    if (blockScreenPowerChange_) {
        WLOGFI("ScreenSessionManager::SetScreenPowerForAll block screen power change");
        return true;
    }
    switch (state) {
        case ScreenPowerState::POWER_ON: {
            if (keyguardDrawnDone_) {
                WLOGFI("ScreenSessionManager::SetScreenPowerForAll keyguardDrawnDone_ is true step 1");
                status = ScreenPowerStatus::POWER_STATUS_ON;
                break;
            } else {
                needScreenOnWhenKeyguardNotify_ = true;
                auto task = [this]() {
                    SetScreenPower(ScreenPowerStatus::POWER_STATUS_ON,
                        PowerStateChangeReason::STATE_CHANGE_REASON_INIT);
                    needScreenOnWhenKeyguardNotify_ = false;
                    keyguardDrawnDone_ = true;
                    WLOGFI("ScreenSessionManager::SetScreenPowerForAll keyguardDrawnDone_ is true step 2");
                };
                taskScheduler_->PostTask(task, "screenOnTask", 300); // Retry after 300 ms.
                return true;
            }
        }
        case ScreenPowerState::POWER_OFF: {
            keyguardDrawnDone_ = false;
            WLOGFI("ScreenSessionManager::SetScreenPowerForAll keyguardDrawnDone_ is false");
            status = ScreenPowerStatus::POWER_STATUS_OFF;
            break;
        }
        default: {
            WLOGFW("SetScreenPowerStatus state not support");
            return false;
        }
    }
    return SetScreenPower(status, reason);
}

bool ScreenSessionManager::SetScreenPower(ScreenPowerStatus status, PowerStateChangeReason reason)
{
    WLOGFI("ScreenSessionManager::SetScreenPower enter status:%{public}u", status);
    auto screenIds = GetAllScreenIds();
    if (screenIds.empty()) {
        WLOGFE("no screen info");
        return false;
    }

    if (foldScreenController_ != nullptr) {
        rsInterface_.SetScreenPowerStatus(foldScreenController_->GetCurrentScreenId(), status);
    } else {
        for (auto screenId : screenIds) {
            rsInterface_.SetScreenPowerStatus(screenId, status);
            HandlerSensor(status);
        }
    }
    if (reason == PowerStateChangeReason::STATE_CHANGE_REASON_COLLABORATION) {
        return true;
    }
    return NotifyDisplayPowerEvent(status == ScreenPowerStatus::POWER_STATUS_ON ? DisplayPowerEvent::DISPLAY_ON :
        DisplayPowerEvent::DISPLAY_OFF, EventStatus::END, reason);
}

void ScreenSessionManager::SetKeyguardDrawnDoneFlag(bool flag)
{
    keyguardDrawnDone_ = flag;
}

void ScreenSessionManager::HandlerSensor(ScreenPowerStatus status)
{
    auto isPhone = system::GetParameter("const.product.devicetype", "unknown") == "phone";
    if (isPhone) {
        if (status == ScreenPowerStatus::POWER_STATUS_ON) {
            WLOGFI("subscribe rotation sensor when phone turn on");
            ScreenSensorConnector::SubscribeRotationSensor();
        } else if (status == ScreenPowerStatus::POWER_STATUS_OFF || status == ScreenPowerStatus::POWER_STATUS_SUSPEND) {
            WLOGFI("unsubscribe rotation sensor when phone turn off");
            ScreenSensorConnector::UnsubscribeRotationSensor();
        } else {
            WLOGFI("SetScreenPower state not support");
        }
    }
}

void ScreenSessionManager::BootFinishedCallback(const char *key, const char *value, void *context)
{
    if (strcmp(key, BOOTEVENT_BOOT_COMPLETED.c_str()) == 0 && strcmp(value, "true") == 0) {
        WLOGFI("ScreenSessionManager BootFinishedCallback boot animation finished");
        auto &that = *reinterpret_cast<ScreenSessionManager *>(context);
        that.SetDpiFromSettingData();
        that.RegisterSettingDpiObserver();
        if (that.foldScreenPowerInit_ != nullptr) {
            that.foldScreenPowerInit_();
        }
    }
}

void ScreenSessionManager::SetFoldScreenPowerInit(std::function<void()> foldScreenPowerInit)
{
    foldScreenPowerInit_ = foldScreenPowerInit;
}

void ScreenSessionManager::RegisterSettingDpiObserver()
{
    WLOGFI("Register Setting Dpi Observer");
    SettingObserver::UpdateFunc updateFunc = [&](const std::string& key) { SetDpiFromSettingData(); };
    ScreenSettingHelper::RegisterSettingDpiObserver(updateFunc);
}

void ScreenSessionManager::SetDpiFromSettingData()
{
    uint32_t settingDpi;
    bool ret = ScreenSettingHelper::GetSettingDpi(settingDpi);
    if (!ret) {
        WLOGFW("get setting dpi failed,use default dpi");
        settingDpi = defaultDpi;
    } else {
        WLOGFI("get setting dpi success,settingDpi: %{public}u", settingDpi);
    }
    if (settingDpi >= DOT_PER_INCH_MINIMUM_VALUE && settingDpi <= DOT_PER_INCH_MAXIMUM_VALUE
        && cachedSettingDpi_ != settingDpi) {
        cachedSettingDpi_ = settingDpi;
        float dpi = static_cast<float>(settingDpi) / BASELINE_DENSITY;
        ScreenId defaultScreenId = GetDefaultScreenId();
        SetVirtualPixelRatio(defaultScreenId, dpi);
    }
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
    WLOGFI("ScreenSessionManager::NotifyDisplayEvent receive keyguardDrawnDone");
    sessionDisplayPowerController_->NotifyDisplayEvent(event);
    if (event == DisplayEvent::KEYGUARD_DRAWN) {
        keyguardDrawnDone_ = true;
        WLOGFI("ScreenSessionManager::NotifyDisplayEvent keyguardDrawnDone_ is true");
        if (needScreenOnWhenKeyguardNotify_) {
            taskScheduler_->RemoveTask("screenOnTask");
            usleep(SLEEP_US);
            SetScreenPower(ScreenPowerStatus::POWER_STATUS_ON, PowerStateChangeReason::STATE_CHANGE_REASON_INIT);
            needScreenOnWhenKeyguardNotify_ = false;
        }
    }

    if (event == DisplayEvent::SCREEN_LOCK_SUSPEND) {
        WLOGFI("ScreenSessionManager::NotifyDisplayEvent screen lock suspend");
        taskScheduler_->RemoveTask("suspendBeginTask");
        blockScreenPowerChange_ = false;
        SetScreenPower(ScreenPowerStatus::POWER_STATUS_SUSPEND, PowerStateChangeReason::STATE_CHANGE_REASON_INIT);
    }

    if (event == DisplayEvent::SCREEN_LOCK_OFF) {
        WLOGFI("ScreenSessionManager::NotifyDisplayEvent screen lock off");
        taskScheduler_->RemoveTask("suspendBeginTask");
        blockScreenPowerChange_ = false;
        SetScreenPower(ScreenPowerStatus::POWER_STATUS_OFF, PowerStateChangeReason::STATE_CHANGE_REASON_INIT);
    }
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
    sptr<ScreenSession> screenSession = GetDefaultScreenSession();
    if (screenSession == nullptr) {
        WLOGFE("fail to get default screenSession");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    isLocked = screenSession->IsScreenRotationLocked();
    WLOGFI("SCB: IsScreenRotationLocked:isLocked: %{public}u", isLocked);
    return DMError::DM_OK;
}

DMError ScreenSessionManager::SetScreenRotationLocked(bool isLocked)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("SCB: ScreenSessionManager set screen rotation locked permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    sptr<ScreenSession> screenSession = GetDefaultScreenSession();
    if (screenSession == nullptr) {
        WLOGFE("fail to get default screenSession");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    screenSession->SetScreenRotationLocked(isLocked);
    WLOGFI("SCB: SetScreenRotationLocked: isLocked: %{public}u", isLocked);
    return DMError::DM_OK;
}

void ScreenSessionManager::UpdateScreenRotationProperty(ScreenId screenId, const RRect& bounds, float rotation)
{
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        WLOGFE("fail to update screen rotation property, cannot find screen %{public}" PRIu64"", screenId);
        return;
    }
    bool needNotifyAvoidArea = false;
    if (screenSession->GetScreenProperty().GetBounds() == bounds &&
        screenSession->GetScreenProperty().GetRotation() != static_cast<float>(rotation)) {
        needNotifyAvoidArea = true;
    }
    screenSession->UpdatePropertyAfterRotation(bounds, rotation, GetFoldDisplayMode());
    sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
    if (displayInfo == nullptr) {
        WLOGFE("fail to update screen rotation property, displayInfo is nullptr");
        return;
    }
    NotifyDisplayChanged(displayInfo, DisplayChangeEvent::UPDATE_ROTATION);
    NotifyScreenChanged(screenSession->ConvertToScreenInfo(), ScreenChangeEvent::UPDATE_ROTATION);
    if (needNotifyAvoidArea) {
        std::map<DisplayId, sptr<DisplayInfo>> emptyMap;
        NotifyDisplayStateChange(GetDefaultScreenId(), screenSession->ConvertToDisplayInfo(),
            emptyMap, DisplayStateChangeType::UPDATE_ROTATION);
    }
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
    taskScheduler_->PostAsyncTask(task, "NotifyDisplayChanged");
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
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:SetOrientation");
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        WLOGFE("fail to set orientation, cannot find screen %{public}" PRIu64"", screenId);
        return DMError::DM_ERROR_NULLPTR;
    }
    // just for get orientation test
    screenSession->SetOrientation(orientation);
    screenSession->ScreenOrientationChange(orientation, GetFoldDisplayMode());
    return DMError::DM_OK;
}

DMError ScreenSessionManager::SetOrientationFromWindow(DisplayId displayId, Orientation orientation)
{
    sptr<DisplayInfo> displayInfo = GetDisplayInfoById(displayId);
    if (displayInfo == nullptr) {
        return DMError::DM_ERROR_NULLPTR;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:SetOrientationFromWindow");
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
        Rotation rotationAfter = screenSession->CalcRotation(newOrientation, GetFoldDisplayMode());
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
    ScreenRotationProperty::Init();
    ScreenSensorConnector::SubscribeRotationSensor();
    WLOGFI("subscribe rotation sensor successful");
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
        WLOGFW("can not get display.");
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

bool ScreenSessionManager::NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status,
    PowerStateChangeReason reason)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER);
    if (agents.empty()) {
        return false;
    }
    WLOGFD("NotifyDisplayPowerEvent");
    for (auto& agent : agents) {
        agent->NotifyDisplayPowerEvent(event, status);
    }

    auto screenIds = GetAllScreenIds();
    if (screenIds.empty()) {
        WLOGFE("NotifyDisplayPowerEvent no screen info");
        return false;
    }

    for (auto screenId : screenIds) {
        sptr<ScreenSession> screen = GetScreenSession(screenId);
        screen->PowerStatusChange(event, status, reason);
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
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("SCB: ScreenSessionManager::GetScreenSupportedColorGamuts permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    sptr<ScreenSession> screen = GetScreenSession(screenId);
    if (screen == nullptr) {
        WLOGFE("SCB: ScreenSessionManager::GetScreenSupportedColorGamuts nullptr");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screen->GetScreenSupportedColorGamuts(colorGamuts);
}

DMError ScreenSessionManager::GetPixelFormat(ScreenId screenId, GraphicPixelFormat& pixelFormat)
{
    WLOGFI("GetPixelFormat::ScreenId: %{public}" PRIu64, screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->GetPixelFormat(pixelFormat);
}

DMError ScreenSessionManager::SetPixelFormat(ScreenId screenId, GraphicPixelFormat pixelFormat)
{
    WLOGFI("SetPixelFormat::ScreenId: %{public}" PRIu64 ", pixelFormat %{public}d", screenId, pixelFormat);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->SetPixelFormat(pixelFormat);
}

DMError ScreenSessionManager::GetSupportedHDRFormats(ScreenId screenId,
    std::vector<ScreenHDRFormat>& hdrFormats)
{
    WLOGFI("SCB: ScreenSessionManager::GetSupportedHDRFormats %{public}" PRIu64, screenId);
    sptr<ScreenSession> screen = GetScreenSession(screenId);
    if (screen == nullptr) {
        WLOGFE("SCB: ScreenSessionManager::GetSupportedHDRFormats nullptr");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screen->GetSupportedHDRFormats(hdrFormats);
}

DMError ScreenSessionManager::GetScreenHDRFormat(ScreenId screenId, ScreenHDRFormat& hdrFormat)
{
    WLOGFI("GetScreenHDRFormat::ScreenId: %{public}" PRIu64, screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->GetScreenHDRFormat(hdrFormat);
}

DMError ScreenSessionManager::SetScreenHDRFormat(ScreenId screenId, int32_t modeIdx)
{
    WLOGFI("SetScreenHDRFormat::ScreenId: %{public}" PRIu64 ", modeIdx %{public}d", screenId, modeIdx);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->SetScreenHDRFormat(modeIdx);
}

DMError ScreenSessionManager::GetSupportedColorSpaces(ScreenId screenId,
    std::vector<GraphicCM_ColorSpaceType>& colorSpaces)
{
    WLOGFI("SCB: ScreenSessionManager::GetSupportedColorSpaces %{public}" PRIu64, screenId);
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        WLOGFE("SCB: ScreenSessionManager::GetSupportedColorSpaces nullptr");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->GetSupportedColorSpaces(colorSpaces);
}

DMError ScreenSessionManager::GetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType& colorSpace)
{
    WLOGFI("GetScreenColorSpace::ScreenId: %{public}" PRIu64, screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->GetScreenColorSpace(colorSpace);
}

DMError ScreenSessionManager::SetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType colorSpace)
{
    WLOGFI("SetScreenColorSpace::ScreenId: %{public}" PRIu64 ", colorSpace %{public}d", screenId, colorSpace);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->SetScreenColorSpace(colorSpace);
}

ScreenId ScreenSessionManager::CreateVirtualScreen(VirtualScreenOption option,
                                                   const sptr<IRemoteObject>& displayManagerAgent)
{
    if (!(Permission::IsSystemCalling() && Permission::CheckCallingPermission(SCREEN_CAPTURE_PERMISSION)) &&
        !SessionPermission::IsShellCall()) {
        WLOGFE("create virtual screen permission denied!");
        return SCREEN_ID_INVALID;
    }
    WLOGFI("SCB: ScreenSessionManager::CreateVirtualScreen ENTER");

    int32_t eventRet = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "CREATE_VIRTUAL_SCREEN",
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "PID", getpid(),
        "UID", getuid());
    WLOGI("CreateVirtualScreen: Write HiSysEvent ret:%{public}d", eventRet);

    if (clientProxy_ && option.missionIds_.size() > 0) {
        std::vector<uint64_t> surfaceNodeIds;
        clientProxy_->OnGetSurfaceNodeIdsFromMissionIdsChanged(option.missionIds_, surfaceNodeIds);
        option.missionIds_ = surfaceNodeIds;
    }
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
        screenSession->SetName(option.name_);
        screenSessionMap_.insert(std::make_pair(smsScreenId, screenSession));
        NotifyScreenConnected(screenSession->ConvertToScreenInfo());
        if (deathRecipient_ == nullptr) {
            WLOGFI("SCB: ScreenSessionManager::CreateVirtualScreen Create deathRecipient");
            deathRecipient_ =
                new AgentDeathRecipient([this](const sptr<IRemoteObject>& agent) { OnRemoteDied(agent); });
        }
        if (displayManagerAgent == nullptr) {
            return smsScreenId;
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
    if (!(Permission::IsSystemCalling() && Permission::CheckCallingPermission(SCREEN_CAPTURE_PERMISSION)) &&
        !SessionPermission::IsShellCall()) {
        WLOGFE("set virtual screenSurface permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
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

DMError ScreenSessionManager::SetVirtualMirrorScreenCanvasRotation(ScreenId screenId, bool autoRotate)
{
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("SetVirtualMirrorScreenCanvasRotation denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    WLOGFI("SCB: ScreenSessionManager::SetVirtualMirrorScreenCanvasRotation ENTER");

    bool res = false;
    ScreenId rsScreenId;
    if (!screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId)) {
        WLOGFE("SetVirtualMirrorScreenCanvasRotation: No corresponding rsId");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    res = rsInterface_.SetVirtualMirrorScreenCanvasRotation(rsScreenId, autoRotate);
    if (!res) {
        WLOGE("SetVirtualMirrorScreenCanvasRotation failed in RenderService");
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    WLOGI("SetVirtualMirrorScreenCanvasRotation success");
    return DMError::DM_OK;
}

DMError ScreenSessionManager::ResizeVirtualScreen(ScreenId screenId, uint32_t width, uint32_t height)
{
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("ResizeVirtualScreen permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    WLOGFI("ResizeVirtualScreen, screenId: %{public}" PRIu64", width: %{public}u, height: %{public}u.",
        screenId, width, height);
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        WLOGFE("ResizeVirtualScreen: no such screen.");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    ScreenId rsScreenId;
    if (!screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId)) {
        WLOGFE("ResizeVirtualScreen: No corresponding rsId");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    rsInterface_.ResizeVirtualScreen(rsScreenId, width, height);
    screenSession->Resize(width, height);
    screenSession->PropertyChange(screenSession->GetScreenProperty(),
        ScreenPropertyChangeReason::VIRTUAL_SCREEN_RESIZE);
    return DMError::DM_OK;
}

DMError ScreenSessionManager::DestroyVirtualScreen(ScreenId screenId)
{
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("destroy virtual screen permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }

    // virtual screen destroy callback to notify scb
    auto screen = GetScreenSession(screenId);
    if (CheckScreenInScreenGroup(screen)) {
        NotifyDisplayDestroy(screenId);
    }
    WLOGFI("destroy callback virtual screen");
    OnVirtualScreenChange(screenId, ScreenEvent::DISCONNECTED);

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
            NotifyScreenDisconnected(screenId);
            WLOGFI("SCB: ScreenSessionManager::DestroyVirtualScreen id: %{public}" PRIu64"", screenId);
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

DMError ScreenSessionManager::DisableMirror(bool disableOrNot)
{
    WLOGFI("SCB:ScreenSessionManager::DisableMirror %{public}d", disableOrNot);
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFI("DisableMirror permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    WLOGFI("SCB:ScreenSessionManager::DisableMirror enter %{public}d", disableOrNot);
    if (disableOrNot) {
        std::vector<ScreenId> screenIds;
        auto allScreenIds = GetAllScreenIds();
        for (auto screenId : allScreenIds) {
            auto screen = GetScreenSession(screenId);
            if (screen && screen->GetScreenProperty().GetScreenType() == ScreenType::VIRTUAL) {
                screenIds.push_back(screenId);
            }
        }
        StopMirror(screenIds);
    }
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
    if (system::GetBoolParameter("persist.edm.disallow_mirror", false)) {
        WLOGFW("SCB:ScreenSessionManager::MakeMirror was disabled by edm!");
        return DMError::DM_ERROR_INVALID_PERMISSION;
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

DMError ScreenSessionManager::StopMirror(const std::vector<ScreenId>& mirrorScreenIds)
{
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("StopMirror permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    auto allMirrorScreenIds = GetAllValidScreenIds(mirrorScreenIds);
    if (allMirrorScreenIds.empty()) {
        WLOGFI("SCB: StopMirror done. screens' size:%{public}u", static_cast<uint32_t>(allMirrorScreenIds.size()));
        return DMError::DM_OK;
    }

    DMError ret = StopScreens(allMirrorScreenIds, ScreenCombination::SCREEN_MIRROR);
    if (ret != DMError::DM_OK) {
        WLOGFE("SCB: StopMirror failed.");
        return ret;
    }

    return DMError::DM_OK;
}

DMError ScreenSessionManager::StopScreens(const std::vector<ScreenId>& screenIds, ScreenCombination stopCombination)
{
    for (ScreenId screenId : screenIds) {
        WLOGFI("SCB: StopMirror ScreenId: %{public}" PRIu64"", screenId);
        auto screen = GetScreenSession(screenId);
        if (screen == nullptr) {
            WLOGFW("SCB: StopMirror screen:%{public}" PRIu64" is nullptr", screenId);
            continue;
        }
        sptr<ScreenSessionGroup> screenGroup = GetAbstractScreenGroup(screen->groupSmsId_);
        if (!screenGroup) {
            WLOGFW("SCB: StopMirror groupDmsId:%{public}" PRIu64"is not in smsScreenGroupMap_", screen->groupSmsId_);
            continue;
        }
        if (screenGroup->combination_ != stopCombination) {
            WLOGFW("SCB: StopMirror try to stop screen in another combination");
            continue;
        }
        if (screenGroup->combination_ == ScreenCombination::SCREEN_MIRROR &&
            screen->screenId_ == screenGroup->mirrorScreenId_) {
            WLOGFW("SCB: StopMirror try to stop main mirror screen");
            continue;
        }
        bool res = RemoveChildFromGroup(screen, screenGroup);
        if (res) {
            NotifyScreenGroupChanged(screen->ConvertToScreenInfo(), ScreenGroupChangeEvent::REMOVE_FROM_GROUP);
        }
    }
    return DMError::DM_OK;
}

DMError ScreenSessionManager::MakeUniqueScreen(const std::vector<ScreenId>& screenIds)
{
    WLOGFI("SCB:ScreenSessionManager::MakeUniqueScreen enter!");
    if (screenIds.empty()) {
        WLOGFE("screen is empty");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    ScreenId mainScreenId = GetDefaultScreenId();
    ScreenId uniqueScreenId = screenIds[0];
    WLOGFI("MainScreenId %{public}" PRIu64" unique screenId %{public}" PRIu64".", mainScreenId, uniqueScreenId);

    auto defaultScreen = GetDefaultScreenSession();
    if (!defaultScreen) {
        WLOGFE("Default screen is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    auto group = GetAbstractScreenGroup(defaultScreen->groupSmsId_);
    if (group == nullptr) {
        group = AddToGroupLocked(defaultScreen);
        if (group == nullptr) {
            WLOGFE("group is nullptr");
            return DMError::DM_ERROR_NULLPTR;
        }
        NotifyScreenGroupChanged(defaultScreen->ConvertToScreenInfo(), ScreenGroupChangeEvent::ADD_TO_GROUP);
    }
    Point point;
    std::vector<Point> startPoints;
    startPoints.insert(startPoints.begin(), screenIds.size(), point);
    ChangeScreenGroup(group, screenIds, startPoints, true, ScreenCombination::SCREEN_UNIQUE);

    // virtual screen create callback to notify scb
    OnVirtualScreenChange(uniqueScreenId, ScreenEvent::CONNECTED);
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
    ScreenId defaultScreenId = GetDefaultScreenId();
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
    ScreenId defaultScreenId = GetDefaultScreenId();
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

DMError ScreenSessionManager::StopExpand(const std::vector<ScreenId>& expandScreenIds)
{
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("StopExpand permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    auto allExpandScreenIds = GetAllValidScreenIds(expandScreenIds);
    if (allExpandScreenIds.empty()) {
        WLOGFI("SCB: StopExpand done. screens' size:%{public}u", static_cast<uint32_t>(allExpandScreenIds.size()));
        return DMError::DM_OK;
    }

    DMError ret = StopScreens(allExpandScreenIds, ScreenCombination::SCREEN_EXPAND);
    if (ret != DMError::DM_OK) {
        WLOGFE("SCB: StopExpand stop expand failed.");
        return ret;
    }

    return DMError::DM_OK;
}

bool ScreenSessionManager::ScreenIdManager::ConvertToRsScreenId(ScreenId smsScreenId, ScreenId& rsScreenId) const
{
    std::shared_lock lock(screenIdMapMutex_);
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
    std::shared_lock lock(screenIdMapMutex_);
    auto iter = rs2SmsScreenIdMap_.find(rsScreenId);
    if (iter == rs2SmsScreenIdMap_.end()) {
        return false;
    }
    smsScreenId = iter->second;
    return true;
}

ScreenId ScreenSessionManager::ScreenIdManager::CreateAndGetNewScreenId(ScreenId rsScreenId)
{
    std::unique_lock lock(screenIdMapMutex_);
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

void ScreenSessionManager::ScreenIdManager::UpdateScreenId(ScreenId rsScreenId, ScreenId smsScreenId)
{
    std::unique_lock lock(screenIdMapMutex_);
    rs2SmsScreenIdMap_[rsScreenId] = smsScreenId;
    sms2RsScreenIdMap_[smsScreenId] = rsScreenId;
}

bool ScreenSessionManager::ScreenIdManager::DeleteScreenId(ScreenId smsScreenId)
{
    std::unique_lock lock(screenIdMapMutex_);
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
    std::shared_lock lock(screenIdMapMutex_);
    return rs2SmsScreenIdMap_.find(smsScreenId) != rs2SmsScreenIdMap_.end();
}

sptr<ScreenSession> ScreenSessionManager::InitVirtualScreen(ScreenId smsScreenId, ScreenId rsId,
    VirtualScreenOption option)
{
    WLOGFI("SCB: ScreenSessionManager::InitVirtualScreen: Enter");
    sptr<ScreenSession> screenSession =
        new(std::nothrow) ScreenSession(option.name_, smsScreenId, rsId, GetDefaultScreenId());
    sptr<SupportedScreenModes> info = new(std::nothrow) SupportedScreenModes();
    if (screenSession == nullptr || info == nullptr) {
        WLOGFI("SCB: ScreenSessionManager::InitVirtualScreen: new screenSession or info failed");
        screenIdManager_.DeleteScreenId(smsScreenId);
        rsInterface_.RemoveVirtualScreen(rsId);
        return nullptr;
    }
    info->width_ = option.width_;
    info->height_ = option.height_;
    auto defaultScreen = GetScreenSession(GetDefaultScreenId());
    if (defaultScreen != nullptr && defaultScreen->GetActiveScreenMode() != nullptr) {
        info->refreshRate_ = defaultScreen->GetActiveScreenMode()->refreshRate_;
    }
    screenSession->modes_.emplace_back(info);
    screenSession->activeIdx_ = 0;
    screenSession->SetScreenType(ScreenType::VIRTUAL);
    screenSession->SetVirtualPixelRatio(option.density_);
    screenSession->SetDisplayBoundary(RectF(0, 0, option.width_, option.height_), 0);
    screenSession->RegisterScreenChangeListener(this);
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
        info->id_ = static_cast<uint32_t>(rsScreenModeInfo.GetScreenModeId());
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
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    ScreenId smsScreenId = screenIdManager_.CreateAndGetNewScreenId(rsScreenId);
    RSScreenCapability screenCapability = rsInterface_.GetScreenCapability(rsScreenId);
    WLOGFD("SCB: Screen name is %{public}s, phyWidth is %{public}u, phyHeight is %{public}u",
        screenCapability.GetName().c_str(), screenCapability.GetPhyWidth(), screenCapability.GetPhyHeight());
    sptr<ScreenSession> screenSession =
        new(std::nothrow) ScreenSession(screenCapability.GetName(), smsScreenId, rsScreenId, GetDefaultScreenId());
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
    screenSessionMap_.insert(std::make_pair(smsScreenId, screenSession));
    return screenSession;
}

sptr<ScreenSessionGroup> ScreenSessionManager::AddToGroupLocked(sptr<ScreenSession> newScreen)
{
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
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
    if (!screenGroup->AddChild(newScreen, point, GetScreenSession(GetDefaultScreenId()))) {
        WLOGE("fail to add screen to group. screen=%{public}" PRIu64"", newScreen->screenId_);
        screenIdManager_.DeleteScreenId(smsGroupScreenId);
        return nullptr;
    }
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    auto iter = smsScreenGroupMap_.find(smsGroupScreenId);
    if (iter != smsScreenGroupMap_.end()) {
        WLOGE("group screen existed. id=%{public}" PRIu64"", smsGroupScreenId);
        smsScreenGroupMap_.erase(iter);
    }
    smsScreenGroupMap_.insert(std::make_pair(smsGroupScreenId, screenGroup));
    screenSessionMap_.insert(std::make_pair(smsGroupScreenId, screenGroup));
    screenGroup->mirrorScreenId_ = newScreen->screenId_;
    WLOGI("connect new group screen, screenId: %{public}" PRIu64", screenGroupId: %{public}" PRIu64", "
        "combination:%{public}u", newScreen->screenId_, smsGroupScreenId,
        newScreen->GetScreenProperty().GetScreenType());
    return screenGroup;
}

sptr<ScreenSessionGroup> ScreenSessionManager::AddAsSuccedentScreenLocked(sptr<ScreenSession> newScreen)
{
    ScreenId defaultScreenId = GetDefaultScreenId();
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
    sptr<ScreenSessionGroup> screenGroup = GetAbstractScreenGroup(groupSmsId);
    if (!screenGroup) {
        WLOGE("RemoveFromGroupLocked. groupSmsId:%{public}" PRIu64"is not in smsScreenGroupMap_.", groupSmsId);
        return nullptr;
    }
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
        std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
        smsScreenGroupMap_.erase(screenGroup->screenId_);
        screenSessionMap_.erase(screenGroup->screenId_);
        WLOGE("SCB: RemoveFromGroupLocked. screenSessionMap_ remove screen:%{public}" PRIu64"", screenGroup->screenId_);
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
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    auto iter = smsScreenGroupMap_.find(smsScreenId);
    if (iter == smsScreenGroupMap_.end()) {
        WLOGE("did not find screen:%{public}" PRIu64"", smsScreenId);
        return nullptr;
    }
    return iter->second;
}

bool ScreenSessionManager::CheckScreenInScreenGroup(sptr<ScreenSession> screen) const
{
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    auto groupSmsId = screen->groupSmsId_;
    auto iter = smsScreenGroupMap_.find(groupSmsId);
    if (iter == smsScreenGroupMap_.end()) {
        WLOGFE("groupSmsId:%{public}" PRIu64"is not in smsScreenGroupMap_.", groupSmsId);
        return false;
    }
    sptr<ScreenSessionGroup> screenGroup = iter->second;
    return screenGroup->HasChild(screen->screenId_);
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
        if (CheckScreenInScreenGroup(screen)) {
            NotifyDisplayDestroy(screenId);
        }
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
        bool addChildRes = group->AddChild(screen, expandPoint, GetScreenSession(GetDefaultScreenId()));
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
    WLOGFI("SCB: ScreenSessionManager::RemoveVirtualScreenFromGroup enter!");
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("SCB: ScreenSessionManager::RemoveVirtualScreenFromGroup permission denied!");
        return;
    }
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
            if (screenSession == nullptr) {
                WLOGFE("SCB: ScreenSessionManager::GetScreenSnapshot screenSession is nullptr!");
                continue;
            }
            sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
            if (displayInfo == nullptr) {
                WLOGFE("SCB: ScreenSessionManager::GetScreenSnapshot displayInfo is nullptr!");
                continue;
            }
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

    int32_t eventRet = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "GET_DISPLAY_SNAPSHOT",
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "PID", getpid(),
        "UID", getuid());
    WLOGI("GetDisplaySnapshot: Write HiSysEvent ret:%{public}d", eventRet);

    if (system::GetBoolParameter("persist.edm.disallow_screenshot", false)) {
        WLOGFW("SCB: ScreenSessionManager::GetDisplaySnapshot was disabled by edm!");
        return nullptr;
    }
    if ((Permission::IsSystemCalling() && Permission::CheckCallingPermission(SCREEN_CAPTURE_PERMISSION)) ||
        SessionPermission::IsShellCall()) {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:GetDisplaySnapshot(%" PRIu64")", displayId);
        auto res = GetScreenSnapshot(displayId);
        if (res != nullptr) {
            NotifyScreenshot(displayId);
        }
        return res;
    } else if (errorCode) {
        *errorCode = DmErrorCode::DM_ERROR_NO_PERMISSION;
    }
    return nullptr;
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
        if (iter != screenSessionMap_.end() && iter->second != nullptr &&
                iter->second->GetScreenProperty().GetScreenType() != ScreenType::UNDEFINED) {
            validScreenIds.emplace_back(screenId);
        }
    }
    return validScreenIds;
}

sptr<ScreenGroupInfo> ScreenSessionManager::GetScreenGroupInfoById(ScreenId screenId)
{
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("SCB: ScreenSessionManager::GetScreenGroupInfoById permission denied!");
        return nullptr;
    }
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
    taskScheduler_->PostAsyncTask(task, "NotifyScreenConnected");
}

void ScreenSessionManager::NotifyScreenDisconnected(ScreenId screenId)
{
    auto task = [=] {
        WLOGFI("NotifyScreenDisconnected,  screenId:%{public}" PRIu64"", screenId);
        OnScreenDisconnect(screenId);
    };
    taskScheduler_->PostAsyncTask(task, "NotifyScreenDisconnected");
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
    taskScheduler_->PostAsyncTask(task, "NotifyScreenGroupChanged:PID");
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
    taskScheduler_->PostAsyncTask(task, "NotifyScreenGroupChanged");
}

void ScreenSessionManager::NotifyPrivateSessionStateChanged(bool hasPrivate)
{
    if (hasPrivate == screenPrivacyStates) {
        WLOGFD("screen session state is not changed, return");
        return;
    }
    WLOGI("PrivateSession status : %{public}u", hasPrivate);
    screenPrivacyStates = hasPrivate;
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::PRIVATE_WINDOW_LISTENER);
    if (agents.empty()) {
        return;
    }
    for (auto& agent : agents) {
        agent->NotifyPrivateWindowStateChanged(hasPrivate);
    }
}

void ScreenSessionManager::SetScreenPrivacyState(bool hasPrivate)
{
    ScreenId id = GetDefaultScreenId();
    auto screenSession = GetScreenSession(id);
    if (screenSession == nullptr) {
        WLOGFE("can not get default screen now");
        return;
    }
    screenSession->SetPrivateSessionForeground(hasPrivate);
    NotifyPrivateSessionStateChanged(hasPrivate);
}

DMError ScreenSessionManager::HasPrivateWindow(DisplayId id, bool& hasPrivateWindow)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("SCB: ScreenSessionManager HasPrivateWindow permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
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
    return screenCutoutController_ ? screenCutoutController_->GetScreenCutoutInfo(displayId) : nullptr;
}

DMError ScreenSessionManager::HasImmersiveWindow(bool& immersive)
{
    if (!clientProxy_) {
        WLOGFE("clientProxy_ is null");
        return DMError::DM_ERROR_NULLPTR;
    }
    clientProxy_->OnImmersiveStateChanged(immersive);
    return DMError::DM_OK;
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
    if (!(SessionPermission::IsSACalling() || SessionPermission::IsStartByHdcd())) {
        WLOGFE("DumpAllScreensInfo permission denied!");
        return;
    }
    std::ostringstream oss;
    oss << "--------------------------------------Free Screen"
        << "--------------------------------------"
        << std::endl;
    oss << "ScreenName           Type     IsGroup DmsId RsId                 ActiveIdx VPR Rotation Orientation "
        << "RequestOrientation NodeId               "
        << std::endl;
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (auto sessionIt : screenSessionMap_) {
        auto screenSession = sessionIt.second;
        if (screenSession == nullptr) {
            continue;
        }
        sptr<ScreenInfo> screenInfo = screenSession->ConvertToScreenInfo();
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
    if (!(SessionPermission::IsSACalling() || SessionPermission::IsStartByHdcd())) {
        WLOGFE("DumpSpecialScreenInfo permission denied!");
        return;
    }
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

// --- Fold Screen ---
ScreenProperty ScreenSessionManager::GetPhyScreenProperty(ScreenId screenId)
{
    std::lock_guard<std::recursive_mutex> lock_phy(phyScreenPropMapMutex_);
    ScreenProperty property;
    auto iter = phyScreenPropMap_.find(screenId);
    if (iter == phyScreenPropMap_.end()) {
        WLOGFD("Error found physic screen config with id: %{public}" PRIu64, screenId);
        return property;
    }
    return iter->second;
}

void ScreenSessionManager::SetFoldDisplayMode(const FoldDisplayMode displayMode)
{
    if (foldScreenController_ == nullptr) {
        WLOGFW("SetFoldDisplayMode foldScreenController_ is null");
        return;
    }
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("SetFoldDisplayMode permission denied!");
        return;
    }
    foldScreenController_->SetDisplayMode(displayMode);
}

void ScreenSessionManager::SetFoldStatusLocked(bool locked)
{
    if (foldScreenController_ == nullptr) {
        WLOGFW("SetFoldStatusLocked foldScreenController_ is null");
        return;
    }
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("SetFoldStatusLocked permission denied!");
        return;
    }
    foldScreenController_->LockDisplayStatus(locked);
}

FoldDisplayMode ScreenSessionManager::GetFoldDisplayMode()
{
    if (foldScreenController_ == nullptr) {
        WLOGFW("GetFoldDisplayMode foldScreenController_ is null");
        return FoldDisplayMode::UNKNOWN;
    }
    return foldScreenController_->GetDisplayMode();
}

bool ScreenSessionManager::IsFoldable()
{
    if (foldScreenController_ == nullptr) {
        WLOGFD("foldScreenController_ is null");
        return false;
    }
    return foldScreenController_->IsFoldable();
}

bool ScreenSessionManager::IsMultiScreenCollaboration()
{
    return isMultiScreenCollaboration_;
}

FoldStatus ScreenSessionManager::GetFoldStatus()
{
    if (foldScreenController_ == nullptr) {
        WLOGFD("foldScreenController_ is null");
        return FoldStatus::UNKNOWN;
    }
    return foldScreenController_->GetFoldStatus();
}

sptr<FoldCreaseRegion> ScreenSessionManager::GetCurrentFoldCreaseRegion()
{
    if (foldScreenController_ == nullptr) {
        WLOGFW("foldScreenController_ is null");
        return nullptr;
    }
    return foldScreenController_->GetCurrentFoldCreaseRegion();
}

uint32_t ScreenSessionManager::GetCurvedCompressionArea()
{
    return ScreenSceneConfig::GetCurvedCompressionAreaInLandscape();
}

void ScreenSessionManager::NotifyFoldStatusChanged(FoldStatus foldStatus)
{
    WLOGI("NotifyFoldStatusChanged foldStatus:%{public}d", foldStatus);
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::FOLD_STATUS_CHANGED_LISTENER);
    if (agents.empty()) {
        return;
    }
    for (auto& agent: agents) {
        agent->NotifyFoldStatusChanged(foldStatus);
    }
}

void ScreenSessionManager::NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_UPDATE_LISTENER);
    if (agents.empty()) {
        return;
    }
    for (auto& agent: agents) {
        agent->NotifyDisplayChangeInfoChanged(info);
    }
}

void ScreenSessionManager::NotifyDisplayModeChanged(FoldDisplayMode displayMode)
{
    WLOGI("NotifyDisplayModeChanged displayMode:%{public}d", displayMode);
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_MODE_CHANGED_LISTENER);
    if (agents.empty()) {
        return;
    }
    for (auto& agent: agents) {
        agent->NotifyDisplayModeChanged(displayMode);
    }
}

void ScreenSessionManager::SetDisplayNodeScreenId(ScreenId screenId, ScreenId displayNodeScreenId)
{
    WLOGFI("screenId: %{public}" PRIu64 " displayNodeScreenId: %{public}" PRIu64, screenId, displayNodeScreenId);
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:SetDisplayNodeScreenId");
    if (!clientProxy_) {
        WLOGFD("clientProxy_ is null");
        return;
    }
    clientProxy_->SetDisplayNodeScreenId(screenId, displayNodeScreenId);
}

void ScreenSessionManager::OnPropertyChange(const ScreenProperty& newProperty, ScreenPropertyChangeReason reason,
    ScreenId screenId)
{
    WLOGFI("screenId: %{public}" PRIu64 " reason: %{public}d", screenId, static_cast<int>(reason));
    if (!clientProxy_) {
        WLOGFD("clientProxy_ is null");
        return;
    }
    clientProxy_->OnPropertyChanged(screenId, newProperty, reason);
}

void ScreenSessionManager::OnPowerStatusChange(DisplayPowerEvent event, EventStatus status,
    PowerStateChangeReason reason)
{
    WLOGFI("event: %{public}d, status: %{public}d, reason: %{public}d", static_cast<int>(event),
        static_cast<int>(status), static_cast<int>(reason));
    if (!clientProxy_) {
        WLOGFD("clientProxy_ is null");
        return;
    }
    clientProxy_->OnPowerStatusChanged(event, status, reason);
}

void ScreenSessionManager::OnSensorRotationChange(float sensorRotation, ScreenId screenId)
{
    WLOGFI("screenId: %{public}" PRIu64 " sensorRotation: %{public}f", screenId, sensorRotation);
    if (!clientProxy_) {
        WLOGFD("clientProxy_ is null");
        return;
    }
    clientProxy_->OnSensorRotationChanged(screenId, sensorRotation);
}

void ScreenSessionManager::OnScreenOrientationChange(float screenOrientation, ScreenId screenId)
{
    WLOGFI("screenId: %{public}" PRIu64 " screenOrientation: %{public}f", screenId, screenOrientation);
    if (!clientProxy_) {
        WLOGFD("clientProxy_ is null");
        return;
    }
    clientProxy_->OnScreenOrientationChanged(screenId, screenOrientation);
}

void ScreenSessionManager::OnScreenRotationLockedChange(bool isLocked, ScreenId screenId)
{
    WLOGFI("screenId: %{public}" PRIu64 " isLocked: %{public}d", screenId, isLocked);
    if (!clientProxy_) {
        WLOGFD("clientProxy_ is null");
        return;
    }
    clientProxy_->OnScreenRotationLockedChanged(screenId, isLocked);
}

void ScreenSessionManager::SetClient(const sptr<IScreenSessionManagerClient>& client, int32_t userId)
{
    if (!client) {
        WLOGFE("client is null");
        return;
    }
    WLOGFI("SetClient userId= %{public}d", userId);
    MockSessionManagerService::GetInstance().NotifyWMSConnected(userId, 0);
    clientProxy_ = client;
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (const auto& iter : screenSessionMap_) {
        if (!iter.second) {
            continue;
        }
        // In the rotating state, after scb restarts, the screen information needs to be reset.
        float phyWidth = 0.0f;
        float phyHeight = 0.0f;
        bool isReset = true;
        if (foldScreenController_ != nullptr) {
            FoldDisplayMode displayMode = GetFoldDisplayMode();
            WLOGFI("fold screen with screenId = %{public}u", displayMode);
            if (displayMode == FoldDisplayMode::MAIN) {
                auto phyBounds = GetPhyScreenProperty(SCREEN_ID_MAIN).GetPhyBounds();
                phyWidth = phyBounds.rect_.width_;
                phyHeight = phyBounds.rect_.height_;
            } else if (displayMode == FoldDisplayMode::FULL) {
                auto phyBounds = GetPhyScreenProperty(SCREEN_ID_FULL).GetPhyBounds();
                phyWidth = phyBounds.rect_.height_;
                phyHeight = phyBounds.rect_.width_;
            } else {
                isReset = false;
            }
        } else {
            auto remoteScreenMode = rsInterface_.GetScreenActiveMode(iter.first);
            phyWidth = remoteScreenMode.GetScreenWidth();
            phyHeight = remoteScreenMode.GetScreenHeight();
        }
        auto localScreenBounds = iter.second->GetScreenProperty().GetBounds();
        WLOGFI("phyWidth = :%{public}f, phyHeight = :%{public}f, local width = :%{public}f, local height = :%{public}f",
            phyWidth, phyHeight, localScreenBounds.rect_.width_, localScreenBounds.rect_.height_);
        bool isModeChanged =
            (localScreenBounds.rect_.width_ < localScreenBounds.rect_.height_) != (phyWidth < phyHeight);
        if (isModeChanged && isReset) {
            WLOGFI("[WMSRecover]screen(id:%{public}" PRIu64 ") current mode is not default mode, reset it", iter.first);
            SetRotation(iter.first, Rotation::ROTATION_0, false);
            iter.second->SetDisplayBoundary(RectF(0, 0, phyWidth, phyHeight), 0);
        }
        clientProxy_->OnScreenConnectionChanged(iter.first, ScreenEvent::CONNECTED,
            iter.second->GetRSScreenId(), iter.second->GetName());
    }
}

ScreenProperty ScreenSessionManager::GetScreenProperty(ScreenId screenId)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return {};
    }
    return screenSession->GetScreenProperty();
}

std::shared_ptr<RSDisplayNode> ScreenSessionManager::GetDisplayNode(ScreenId screenId)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        WLOGFE("screenSession is null");
        return nullptr;
    }
    return screenSession->GetDisplayNode();
}

void ScreenSessionManager::ShowHelpInfo(std::string& dumpInfo)
{
    dumpInfo.append("Usage:\n")
        .append(" -h                             ")
        .append("|help text for the tool\n")
        .append(" -a                             ")
        .append("|dump all screen information in the system\n")
        .append(" -s {screen id}                 ")
        .append("|dump specified screen information\n")
        .append(" -f                             ")
        .append("|switch the screen to full display mode\n")
        .append(" -m                             ")
        .append("|switch the screen to main display mode\n")
        .append(" -l                             ")
        .append("|lock the screen display status\n")
        .append(" -u                             ")
        .append("|unlock the screen display status\n");
}

void ScreenSessionManager::ShowIllegalArgsInfo(std::string& dumpInfo)
{
    dumpInfo.append("The arguments are illegal and you can enter '-h' for help.");
}

bool ScreenSessionManager::IsValidDigitString(const std::string& idStr) const
{
    if (idStr.empty()) {
        return false;
    }
    for (char ch : idStr) {
        if ((ch >= '0' && ch <= '9')) {
            continue;
        }
        WLOGFE("invalid id");
        return false;
    }
    return true;
}

int ScreenSessionManager::DumpScreenInfo(const std::vector<std::string>& args, std::string& dumpInfo)
{
    if (args.empty()) {
        return -1;
    }
    if (args.size() == 1 && args[0] == ARG_DUMP_ALL) { // 1: params num
        return DumpAllScreenInfo(dumpInfo);
    } else if (args[0] == ARG_DUMP_SCREEN && IsValidDigitString(args[1])) {
        ScreenId screenId = std::stoull(args[1]);
        return DumpSpecifiedScreenInfo(screenId, dumpInfo);
    } else {
        return -1;
    }
}

int ScreenSessionManager::DumpAllScreenInfo(std::string& dumpInfo)
{
    DumpAllScreensInfo(dumpInfo);
    return 0;
}

int ScreenSessionManager::DumpSpecifiedScreenInfo(ScreenId screenId, std::string& dumpInfo)
{
    DumpSpecialScreenInfo(screenId, dumpInfo);
    return 0;
}

static std::string Str16ToStr8(const std::u16string& str)
{
    if (str == DEFAULT_USTRING) {
        return DEFAULT_STRING;
    }
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert(DEFAULT_STRING);
    std::string result = convert.to_bytes(str);
    return result == DEFAULT_STRING ? "" : result;
}

int ScreenSessionManager::Dump(int fd, const std::vector<std::u16string>& args)
{
    WLOGFI("Dump begin");
    if (fd < 0) {
        return -1;
    }
    (void) signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE crash
    UniqueFd ufd = UniqueFd(fd); // auto close
    fd = ufd.Get();
    std::vector<std::string> params;
    for (auto& arg : args) {
        params.emplace_back(Str16ToStr8(arg));
    }
    std::string dumpInfo;
    if (params.empty()) {
        ShowHelpInfo(dumpInfo);
    } else if (params.size() == 1 && params[0] == ARG_DUMP_HELP) { // 1: params num
        ShowHelpInfo(dumpInfo);
    } else if (params.size() == 1 && (params[0] == ARG_FOLD_DISPLAY_FULL || params[0] == ARG_FOLD_DISPLAY_MAIN)) {
        int errCode = SetFoldDisplayMode(params[0]);
        if (errCode != 0) {
            ShowIllegalArgsInfo(dumpInfo);
        }
    } else if (params.size() == 1 && (params[0] == ARG_LOCK_FOLD_DISPLAY_STATUS
                || params[0] == ARG_UNLOCK_FOLD_DISPLAY_STATUS)) {
        int errCode = SetFoldStatusLocked(params[0]);
        if (errCode != 0) {
            ShowIllegalArgsInfo(dumpInfo);
        }
    } else {
        int errCode = DumpScreenInfo(params, dumpInfo);
        if (errCode != 0) {
            ShowIllegalArgsInfo(dumpInfo);
        }
    }
    int ret = dprintf(fd, "%s\n", dumpInfo.c_str());
    if (ret < 0) {
        WLOGFE("dprintf error");
        return -1; // WMError::WM_ERROR_INVALID_OPERATION;
    }
    WLOGI("dump end");
    return 0;
}

int ScreenSessionManager::SetFoldDisplayMode(const std::string& modeParam)
{
    if (modeParam.empty()) {
        return -1;
    }
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    if (modeParam == ARG_FOLD_DISPLAY_FULL) {
        displayMode = FoldDisplayMode::FULL;
    } else if (modeParam == ARG_FOLD_DISPLAY_MAIN) {
        displayMode = FoldDisplayMode::MAIN;
    } else {
        WLOGFW("SetFoldDisplayMode mode not support");
        return -1;
    }
    SetFoldDisplayMode(displayMode);
    return 0;
}

int ScreenSessionManager::SetFoldStatusLocked(const std::string& lockParam)
{
    if (lockParam.empty()) {
        return -1;
    }
    bool lockDisplayStatus = false;
    if (lockParam == ARG_LOCK_FOLD_DISPLAY_STATUS) {
        lockDisplayStatus = true;
    } else if (lockParam == ARG_UNLOCK_FOLD_DISPLAY_STATUS) {
        lockDisplayStatus = false;
    } else {
        WLOGFW("SetFoldStatusLocked status not support");
        return -1;
    }
    SetFoldStatusLocked(lockDisplayStatus);
    return 0;
}

void ScreenSessionManager::NotifyAvailableAreaChanged(DMRect area)
{
    WLOGI("NotifyAvailableAreaChanged call");
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::AVAILABLE_AREA_CHANGED_LISTENER);
    if (agents.empty()) {
        return;
    }
    for (auto& agent: agents) {
        agent->NotifyAvailableAreaChanged(area);
    }
}

DMError ScreenSessionManager::GetAvailableArea(DisplayId displayId, DMRect& area)
{
    auto displayInfo = GetDisplayInfoById(displayId);
    if (displayInfo == nullptr) {
        WLOGFE("can not get displayInfo.");
        return DMError::DM_ERROR_NULLPTR;
    }
    auto screenSession = GetScreenSession(displayInfo->GetScreenId());
    if (screenSession == nullptr) {
        WLOGFE("can not get default screen now");
        return DMError::DM_ERROR_NULLPTR;
    }
    area = screenSession->GetAvailableArea();
    return DMError::DM_OK;
}

void ScreenSessionManager::UpdateAvailableArea(ScreenId screenId, DMRect area)
{
    auto screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        WLOGFE("can not get default screen now");
        return;
    }
    if (!screenSession->UpdateAvailableArea(area)) {
        return;
    }
    NotifyAvailableAreaChanged(area);
}

void ScreenSessionManager::NotifyFoldToExpandCompletion(bool foldToExpand)
{
    WLOGFI("ScreenSessionManager::NotifyFoldToExpandCompletion");
    SetDisplayNodeScreenId(SCREEN_ID_FULL, foldToExpand ? SCREEN_ID_FULL : SCREEN_ID_MAIN);
    sptr<ScreenSession> screenSession = GetDefaultScreenSession();
    if (screenSession == nullptr) {
        WLOGFE("fail to get default screenSession");
        return;
    }
    screenSession->UpdateAfterFoldExpand(foldToExpand);
}

} // namespace OHOS::Rosen
