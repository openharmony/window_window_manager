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
#ifdef DEVICE_STATUS_ENABLE
#include <interaction_manager.h>
#endif // DEVICE_STATUS_ENABLE
#include <ipc_skeleton.h>
#include <parameter.h>
#include <parameters.h>
#include <system_ability_definition.h>
#include <transaction/rs_interfaces.h>
#include <xcollie/watchdog.h>
#include <hisysevent.h>
#include <power_mgr_client.h>

#include "dm_common.h"
#include "fold_screen_state_internel.h"
#include "multi_screen_manager.h"
#include "pipeline/rs_node_map.h"
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
#include "screen_session_dumper.h"
#include "mock_session_manager_service.h"
#include "connection/screen_snapshot_picker_connection.h"
#include "connection/screen_cast_connection.h"
#include "publish/screen_session_publish.h"
#include "dms_xcollie.h"

namespace OHOS::Rosen {
namespace {
const std::string SCREEN_SESSION_MANAGER_THREAD = "OS_ScreenSessionManager";
const std::string SCREEN_SESSION_MANAGER_SCREEN_POWER_THREAD = "OS_ScreenSessionManager_ScreenPower";
const std::string SCREEN_CAPTURE_PERMISSION = "ohos.permission.CAPTURE_SCREEN";
const std::string BOOTEVENT_BOOT_COMPLETED = "bootevent.boot.completed";
const int32_t SLEEP_10_MS = 10 * 1000; // 10ms
const int32_t CV_WAIT_SCREENON_MS = 300;
const int32_t CV_WAIT_SCREENOFF_MS = 1500;
const int32_t CV_WAIT_SCREENOFF_MS_MAX = 3000;
const int32_t CV_WAIT_SCBSWITCH_MS = 3000;
const std::u16string DEFAULT_USTRING = u"error";
const std::string DEFAULT_STRING = "error";
const std::string ARG_DUMP_HELP = "-h";
const std::string ARG_DUMP_ALL = "-a";
const std::string ARG_DUMP_SCREEN = "-s";
const std::string ARG_FOLD_DISPLAY_FULL = "-f";
const std::string ARG_FOLD_DISPLAY_MAIN = "-m";
const std::string ARG_FOLD_DISPLAY_SUB = "-sub";
const std::string ARG_FOLD_DISPLAY_COOR = "-coor";
const std::string STATUS_FOLD_HALF = "-z";
const std::string STATUS_EXPAND = "-y";
const std::string STATUS_FOLD = "-p";
const std::string ARG_LOCK_FOLD_DISPLAY_STATUS = "-l";
const std::string ARG_UNLOCK_FOLD_DISPLAY_STATUS = "-u";
const std::string SETTING_LOCKED_KEY = "settings.general.accelerometer_rotation_status";
const ScreenId SCREEN_ID_FULL = 0;
const ScreenId SCREEN_ID_MAIN = 5;
const ScreenId SCREEN_ID_PC_MAIN = 9;
const std::vector<std::string> displayModeCommands = {"-f", "-m", "-sub", "-coor"};
constexpr int32_t INVALID_UID = -1;
constexpr int32_t INVALID_USER_ID = -1;
constexpr int32_t INVALID_SCB_PID = -1;
constexpr int32_t BASE_USER_RANGE = 200000;
static bool g_foldScreenFlag = system::GetParameter("const.window.foldscreen.type", "") != "";
static const int32_t g_screenRotationOffSet = system::GetIntParameter<int32_t>("const.fold.screen_rotation.offset", 0);
static const int32_t ROTATION_90 = 1;
static const int32_t ROTATION_270 = 3;
static const int32_t AUTO_ROTATE_OFF = 0;
const unsigned int XCOLLIE_TIMEOUT_10S = 10;
const unsigned int XCOLLIE_TIMEOUT_5S = 5;
constexpr int32_t CAST_WIRED_PROJECTION_START = 1005;
constexpr int32_t CAST_WIRED_PROJECTION_STOP = 1007;
constexpr int32_t RES_FAILURE_FOR_PRIVACY_WINDOW = -2;

// based on the bundle_util
inline int32_t GetUserIdByCallingUid()
{
    int32_t uid = IPCSkeleton::GetCallingUid();
    TLOGD(WmsLogTag::WMS_MULTI_USER, "get calling uid(%{public}d)", uid);
    if (uid <= INVALID_UID) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "uid is illegal: %{public}d", uid);
        return INVALID_USER_ID;
    }
    return uid / BASE_USER_RANGE;
}
} // namespace

WM_IMPLEMENT_SINGLE_INSTANCE(ScreenSessionManager)

const bool REGISTER_RESULT = !SceneBoardJudgement::IsSceneBoardEnabled() ? false :
    SystemAbility::MakeAndRegisterAbility(&ScreenSessionManager::GetInstance());

ScreenSessionManager::ScreenSessionManager()
    : SystemAbility(DISPLAY_MANAGER_SERVICE_SA_ID, true), rsInterface_(RSInterfaces::GetInstance())
{
    screenEventTracker_.RecordEvent("Dms construct.");
    LoadScreenSceneXml();
    screenOffDelay_ = CV_WAIT_SCREENOFF_MS;
    taskScheduler_ = std::make_shared<TaskScheduler>(SCREEN_SESSION_MANAGER_THREAD, AppExecFwk::ThreadMode::FFRT);
    screenPowerTaskScheduler_ = std::make_shared<TaskScheduler>(SCREEN_SESSION_MANAGER_SCREEN_POWER_THREAD,
        AppExecFwk::ThreadMode::FFRT);
    screenCutoutController_ = new (std::nothrow) ScreenCutoutController();
    if (!screenCutoutController_) {
        TLOGE(WmsLogTag::DMS, "screenCutoutController_ is nullptr");
    }
    sessionDisplayPowerController_ = new SessionDisplayPowerController(mutex_,
        std::bind(&ScreenSessionManager::NotifyDisplayStateChange, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    if (g_foldScreenFlag) {
        HandleFoldScreenPowerInit();
    }
    WatchParameter(BOOTEVENT_BOOT_COMPLETED.c_str(), BootFinishedCallback, this);
}

void ScreenSessionManager::HandleFoldScreenPowerInit()
{
    TLOGI(WmsLogTag::DMS, "Enter");
    foldScreenController_ = new (std::nothrow) FoldScreenController(displayInfoMutex_, screenPowerTaskScheduler_);
    if (!foldScreenController_) {
        TLOGE(WmsLogTag::DMS, "foldScreenController_ is nullptr");
    }
    foldScreenController_->SetOnBootAnimation(true);
    auto ret = rsInterface_.SetScreenCorrection(SCREEN_ID_FULL, static_cast<ScreenRotation>(g_screenRotationOffSet));
    std::ostringstream oss;
    oss << "SetScreenCorrection g_screenRotationOffSet: " << g_screenRotationOffSet << "  ret value: " << ret;
    TLOGI(WmsLogTag::DMS, "%{public}s", oss.str().c_str());
    screenEventTracker_.RecordEvent(oss.str());
    FoldScreenPowerInit();
}

void ScreenSessionManager::FoldScreenPowerInit()
{
    SetFoldScreenPowerInit([&]() {
        int64_t timeStamp = 50;
        #ifdef TP_FEATURE_ENABLE
        int32_t tpType = 12;
        std::string fullTpChange = "0";
        std::string mainTpChange = "1";
        #endif
        if (rsInterface_.GetActiveScreenId() == SCREEN_ID_FULL) {
            TLOGI(WmsLogTag::DMS, "ScreenSessionManager Fold Screen Power Full animation Init 1.");
            #ifdef TP_FEATURE_ENABLE
            rsInterface_.SetTpFeatureConfig(tpType, mainTpChange.c_str());
            #endif
            rsInterface_.SetScreenPowerStatus(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_OFF_FAKE);
            rsInterface_.SetScreenPowerStatus(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_ON);
            std::this_thread::sleep_for(std::chrono::milliseconds(timeStamp));
            TLOGI(WmsLogTag::DMS, "ScreenSessionManager Fold Screen Power Full animation Init 2.");
            #ifdef TP_FEATURE_ENABLE
            rsInterface_.SetTpFeatureConfig(tpType, fullTpChange.c_str());
            #endif
            rsInterface_.SetScreenPowerStatus(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_OFF);
            rsInterface_.SetScreenPowerStatus(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_ON);
        } else if (rsInterface_.GetActiveScreenId() == SCREEN_ID_MAIN) {
            TLOGI(WmsLogTag::DMS, "ScreenSessionManager Fold Screen Power Main animation Init 3.");
            #ifdef TP_FEATURE_ENABLE
            rsInterface_.SetTpFeatureConfig(tpType, fullTpChange.c_str());
            #endif
            rsInterface_.SetScreenPowerStatus(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_OFF_FAKE);
            rsInterface_.SetScreenPowerStatus(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_ON);
            std::this_thread::sleep_for(std::chrono::milliseconds(timeStamp));
            TLOGI(WmsLogTag::DMS, "ScreenSessionManager Fold Screen Power Main animation Init 4.");
            #ifdef TP_FEATURE_ENABLE
            rsInterface_.SetTpFeatureConfig(tpType, mainTpChange.c_str());
            #endif
            rsInterface_.SetScreenPowerStatus(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_OFF);
            rsInterface_.SetScreenPowerStatus(SCREEN_ID_MAIN, ScreenPowerStatus::POWER_STATUS_ON);
        } else {
            TLOGI(WmsLogTag::DMS, "ScreenSessionManager Fold Screen Power Init, invalid active screen id");
        }
        FixPowerStatus();
        foldScreenController_->SetOnBootAnimation(false);
        RegisterApplicationStateObserver();
    });
}

void ScreenSessionManager::FixPowerStatus()
{
    if (!PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
        TLOGI(WmsLogTag::DMS, "Fix Screen Power State");
    }
}

void ScreenSessionManager::Init()
{
    constexpr uint64_t interval = 5 * 1000; // 5 second
    if (HiviewDFX::Watchdog::GetInstance().AddThread(
        SCREEN_SESSION_MANAGER_THREAD, taskScheduler_->GetEventHandler(), interval)) {
        TLOGW(WmsLogTag::DMS, "Add thread %{public}s to watchdog failed.", SCREEN_SESSION_MANAGER_THREAD.c_str());
    }

    if (HiviewDFX::Watchdog::GetInstance().AddThread(
        SCREEN_SESSION_MANAGER_SCREEN_POWER_THREAD, screenPowerTaskScheduler_->GetEventHandler(), interval)) {
        TLOGW(WmsLogTag::DMS, "Add thread %{public}s to watchdog failed.",
            SCREEN_SESSION_MANAGER_SCREEN_POWER_THREAD.c_str());
    }
    auto stringConfig = ScreenSceneConfig::GetStringConfig();
    if (stringConfig.count("defaultDisplayCutoutPath") != 0) {
        std::string defaultDisplayCutoutPath = static_cast<std::string>(stringConfig["defaultDisplayCutoutPath"]);
        TLOGD(WmsLogTag::DMS, "defaultDisplayCutoutPath = %{public}s.", defaultDisplayCutoutPath.c_str());
        ScreenSceneConfig::SetCutoutSvgPath(GetDefaultScreenId(), defaultDisplayCutoutPath);
    }

    RegisterScreenChangeListener();
    if (!ScreenSceneConfig::IsSupportRotateWithSensor()) {
        TLOGI(WmsLogTag::DMS, "Current device type not support SetSensorSubscriptionEnabled.");
    } else if (GetScreenPower(SCREEN_ID_FULL) == ScreenPowerState::POWER_ON) {
        // 多屏设备只要有屏幕亮,GetScreenPower获取的任意一块屏幕状态均是ON
        SetSensorSubscriptionEnabled();
        screenEventTracker_.RecordEvent("Dms subscribed to sensor successfully.");
    }
    // publish init
    ScreenSessionPublish::GetInstance().InitPublishEvents();
    screenEventTracker_.RecordEvent("Dms init end.");
}

void ScreenSessionManager::OnStart()
{
    TLOGI(WmsLogTag::DMS, "DMS SA OnStart");
    DmsXcollie dmsXcollie("DMS:OnStart", XCOLLIE_TIMEOUT_10S,
        [this](void *) { screenEventTracker_.LogWarningAllInfos(); });
    Init();
    sptr<ScreenSessionManager> dms(this);
    dms->IncStrongRef(nullptr);
    if (!Publish(dms)) {
        TLOGE(WmsLogTag::DMS, "Publish DMS failed");
        return;
    }
    TLOGI(WmsLogTag::DMS, "DMS SA OnStart end");
    screenEventTracker_.RecordEvent("Dms onstart end.");
}

DMError ScreenSessionManager::RegisterDisplayManagerAgent(
    const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type)
{
    TLOGI(WmsLogTag::DMS, " called type: %{public}u", type);
    DmsXcollie dmsXcollie("DMS:RegisterDisplayManagerAgent", XCOLLIE_TIMEOUT_10S);
    if (type == DisplayManagerAgentType::SCREEN_EVENT_LISTENER && !SessionPermission::IsSystemCalling()
        && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "register display manager agent permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if (type < DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER
        || type >= DisplayManagerAgentType::DISPLAY_MANAGER_MAX_AGENT_TYPE) {
        TLOGE(WmsLogTag::DMS, "DisplayManagerAgentType: %{public}u", static_cast<uint32_t>(type));
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        TLOGE(WmsLogTag::DMS, "displayManagerAgent invalid");
        return DMError::DM_ERROR_NULLPTR;
    }
    TLOGD(WmsLogTag::DMS, "Register display listener type: %{public}u, clientName: %{public}s",
        type, SysCapUtil::GetClientName().c_str());
    return dmAgentContainer_.RegisterAgent(displayManagerAgent, type) ? DMError::DM_OK :DMError::DM_ERROR_NULLPTR;
}

DMError ScreenSessionManager::UnregisterDisplayManagerAgent(
    const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type)
{
    TLOGI(WmsLogTag::DMS, " called type: %{public}u", type);
    if (type == DisplayManagerAgentType::SCREEN_EVENT_LISTENER && !SessionPermission::IsSystemCalling()
        && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "unregister display manager agent permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if ((displayManagerAgent == nullptr) || (displayManagerAgent->AsObject() == nullptr)) {
        TLOGE(WmsLogTag::DMS, "displayManagerAgent invalid");
        return DMError::DM_ERROR_NULLPTR;
    }

    TLOGD(WmsLogTag::DMS, "UnRegister display listener type: %{public}u, clientName: %{public}s",
        type, SysCapUtil::GetClientName().c_str());
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
    ConfigureDpi();
    if (numbersConfig.count("defaultDeviceRotationOffset") != 0) {
        defaultDeviceRotationOffset_ = static_cast<uint32_t>(numbersConfig["defaultDeviceRotationOffset"][0]);
        TLOGD(WmsLogTag::DMS, "defaultDeviceRotationOffset = %{public}u", defaultDeviceRotationOffset_);
    }
    if (enableConfig.count("isWaterfallDisplay") != 0) {
        bool isWaterfallDisplay = static_cast<bool>(enableConfig["isWaterfallDisplay"]);
        TLOGD(WmsLogTag::DMS, "isWaterfallDisplay = %d", isWaterfallDisplay);
    }
    if (numbersConfig.count("curvedScreenBoundary") != 0) {
        std::vector<int> vtBoundary = static_cast<std::vector<int>>(numbersConfig["curvedScreenBoundary"]);
        TLOGD(WmsLogTag::DMS, "vtBoundary.size=%{public}u", static_cast<uint32_t>(vtBoundary.size()));
    }
    if (stringConfig.count("subDisplayCutoutPath") != 0) {
        std::string subDisplayCutoutPath = static_cast<std::string>(stringConfig["subDisplayCutoutPath"]);
        TLOGD(WmsLogTag::DMS, "subDisplayCutoutPath = %{public}s.", subDisplayCutoutPath.c_str());
        ScreenSceneConfig::SetSubCutoutSvgPath(subDisplayCutoutPath);
    }
    if (stringConfig.count("rotationPolicy") != 0) {
        std::string rotationPolicy = static_cast<std::string>(stringConfig["rotationPolicy"]);
        TLOGD(WmsLogTag::DMS, "rotationPolicy = %{public}s.", rotationPolicy.c_str());
        deviceScreenConfig_.rotationPolicy_ = rotationPolicy;
    }
    if (enableConfig.count("isRightPowerButton") != 0) {
        bool isRightPowerButton = static_cast<bool>(enableConfig["isRightPowerButton"]);
        TLOGD(WmsLogTag::DMS, "isRightPowerButton = %d", isRightPowerButton);
        deviceScreenConfig_.isRightPowerButton_ = isRightPowerButton;
    }
    ConfigureWaterfallDisplayCompressionParams();
    ConfigureCastParams();

    if (numbersConfig.count("buildInDefaultOrientation") != 0) {
        Orientation orientation = static_cast<Orientation>(numbersConfig["buildInDefaultOrientation"][0]);
        TLOGD(WmsLogTag::DMS, "orientation = %d", orientation);
    }
    allDisplayPhysicalResolution_ = ScreenSceneConfig::GetAllDisplayPhysicalConfig();
}

void ScreenSessionManager::ConfigureDpi()
{
    auto numbersConfig = ScreenSceneConfig::GetIntNumbersConfig();
    if (numbersConfig.count("dpi") != 0) {
        uint32_t densityDpi = static_cast<uint32_t>(numbersConfig["dpi"][0]);
        TLOGI(WmsLogTag::DMS, "densityDpi = %u", densityDpi);
        if (densityDpi >= DOT_PER_INCH_MINIMUM_VALUE && densityDpi <= DOT_PER_INCH_MAXIMUM_VALUE) {
            isDensityDpiLoad_ = true;
            defaultDpi = densityDpi;
            cachedSettingDpi_ = defaultDpi;
            densityDpi_ = static_cast<float>(densityDpi) / BASELINE_DENSITY;
        }
    }
    if (numbersConfig.count("subDpi") != 0) {
        uint32_t subDensityDpi = static_cast<uint32_t>(numbersConfig["subDpi"][0]);
        TLOGI(WmsLogTag::DMS, "subDensityDpi = %u", subDensityDpi);
        if (subDensityDpi >= DOT_PER_INCH_MINIMUM_VALUE && subDensityDpi <= DOT_PER_INCH_MAXIMUM_VALUE) {
            isDensityDpiLoad_ = true;
            subDensityDpi_ = static_cast<float>(subDensityDpi) / BASELINE_DENSITY;
        }
    }
}

void ScreenSessionManager::ConfigureCastParams()
{
    auto stringConfig = ScreenSceneConfig::GetStringConfig();
    if (stringConfig.count("castBundleName") == 0) {
        TLOGE(WmsLogTag::DMS, "not find cast bundleName in config xml");
        return;
    }
    std::string castBundleName = static_cast<std::string>(stringConfig["castBundleName"]);
    TLOGD(WmsLogTag::DMS, "castBundleName = %{public}s", castBundleName.c_str());
    ScreenCastConnection::GetInstance().SetBundleName(castBundleName);
    if (stringConfig.count("castAbilityName") == 0) {
        TLOGE(WmsLogTag::DMS, "not find cast ability in config xml");
        return;
    }
    std::string castAbilityName = static_cast<std::string>(stringConfig["castAbilityName"]);
    TLOGD(WmsLogTag::DMS, "castAbilityName = %{public}s", castAbilityName.c_str());
    ScreenCastConnection::GetInstance().SetAbilityName(castAbilityName);
}

void ScreenSessionManager::ConfigureWaterfallDisplayCompressionParams()
{
    auto numbersConfig = ScreenSceneConfig::GetIntNumbersConfig();
    auto enableConfig = ScreenSceneConfig::GetEnableConfig();
    if (enableConfig.count("isWaterfallAreaCompressionEnableWhenHorizontal") != 0) {
        bool enable = static_cast<bool>(enableConfig["isWaterfallAreaCompressionEnableWhenHorizontal"]);
        TLOGD(WmsLogTag::DMS, "isWaterfallAreaCompressionEnableWhenHorizontal=%d.", enable);
    }
    ScreenSceneConfig::SetCurvedCompressionAreaInLandscape();
}

void ScreenSessionManager::ConfigureScreenSnapshotParams()
{
    auto stringConfig = ScreenSceneConfig::GetStringConfig();
    if (stringConfig.count("screenSnapshotBundleName") == 0) {
        TLOGE(WmsLogTag::DMS, "not find screen snapshot bundleName in config xml");
        return;
    }
    std::string screenSnapshotBundleName = static_cast<std::string>(stringConfig["screenSnapshotBundleName"]);
    TLOGD(WmsLogTag::DMS, "screenSnapshotBundleName = %{public}s.", screenSnapshotBundleName.c_str());
    ScreenSnapshotPickerConnection::GetInstance().SetBundleName(screenSnapshotBundleName);
    if (stringConfig.count("screenSnapshotAbilityName") == 0) {
        TLOGE(WmsLogTag::DMS, "not find screen snapshot ability in config xml");
        return;
    }
    std::string screenSnapshotAbilityName = static_cast<std::string>(stringConfig["screenSnapshotAbilityName"]);
    TLOGD(WmsLogTag::DMS, "screenSnapshotAbilityName = %{public}s.", screenSnapshotAbilityName.c_str());
    ScreenSnapshotPickerConnection::GetInstance().SetAbilityName(screenSnapshotAbilityName);
}

void ScreenSessionManager::RegisterScreenChangeListener()
{
    TLOGI(WmsLogTag::DMS, "Register screen change listener.");
    auto res = rsInterface_.SetScreenChangeCallback(
        [this](ScreenId screenId, ScreenEvent screenEvent) { OnScreenChange(screenId, screenEvent); });
    if (res != StatusCode::SUCCESS) {
        auto task = [this]() { RegisterScreenChangeListener(); };
        taskScheduler_->PostAsyncTask(task, "RegisterScreenChangeListener", 50); // Retry after 50 ms.
        screenEventTracker_.RecordEvent("Dms OnScreenChange register failed.");
    } else {
        screenEventTracker_.RecordEvent("Dms OnScreenChange register success.");
    }
}

void ScreenSessionManager::RegisterRefreshRateChangeListener()
{
    static bool isRegisterRefreshRateListener = false;
    if (!isRegisterRefreshRateListener) {
        auto res = rsInterface_.RegisterHgmRefreshRateUpdateCallback(
            [this](uint32_t refreshRate) { OnHgmRefreshRateChange(refreshRate); });
        if (res != StatusCode::SUCCESS) {
            TLOGE(WmsLogTag::DMS, "Register refresh rate mode change listener failed.");
            screenEventTracker_.RecordEvent("Dms RefreshRateChange register failed.");
        } else {
            isRegisterRefreshRateListener = true;
            screenEventTracker_.RecordEvent("Dms RefreshRateChange register success.");
        }
    }
}

void ScreenSessionManager::OnVirtualScreenChange(ScreenId screenId, ScreenEvent screenEvent)
{
    TLOGI(WmsLogTag::DMS, "Notify scb virtual screen change, ScreenId: %{public}" PRIu64 ", ScreenEvent: %{public}d",
        screenId, static_cast<int>(screenEvent));
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        TLOGE(WmsLogTag::DMS, "screenSession is nullptr");
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

bool ScreenSessionManager::IsDefaultMirrorMode(ScreenId screenId)
{
    if (screenId != SCREEN_ID_MAIN && screenId != SCREEN_ID_FULL && screenId != SCREEN_ID_PC_MAIN) {
        return true;
    }
    return false;
}

void ScreenSessionManager::FreeDisplayMirrorNodeInner(const sptr<ScreenSession> mirrorSession)
{
    if (mirrorSession == nullptr) {
        return;
    }
    std::shared_ptr<RSDisplayNode> displayNode = mirrorSession->GetDisplayNode();
    if (displayNode == nullptr) {
        return;
    }
    hdmiScreenCount_ = hdmiScreenCount_ > 0 ? hdmiScreenCount_ - 1 : 0;
    NotifyCaptureStatusChanged();
    displayNode->RemoveFromTree();
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        TLOGI(WmsLogTag::DMS, "FreeDisplayMirrorNodeInner free displayNode");
        transactionProxy->FlushImplicitTransaction();
    }
}

void ScreenSessionManager::OnScreenChange(ScreenId screenId, ScreenEvent screenEvent)
{
    std::ostringstream oss;
    oss << "OnScreenChange triggered. screenId: " << static_cast<int32_t>(screenId)
        << "  screenEvent: " << static_cast<int32_t>(screenEvent);
    screenEventTracker_.RecordEvent(oss.str());
    TLOGI(WmsLogTag::DMS, "screenId: %{public}" PRIu64 " screenEvent: %{public}d",
        screenId, static_cast<int>(screenEvent));
    auto screenSession = GetOrCreateScreenSession(screenId);
    if (!screenSession) {
        TLOGE(WmsLogTag::DMS, "screenSession is nullptr");
        return;
    }

    if (foldScreenController_ != nullptr) {
        screenSession->SetFoldScreen(true);
    }
    HandleScreenEvent(screenSession, screenId, screenEvent);
}

void ScreenSessionManager::SendCastEvent(const bool &isPlugIn)
{
    TLOGI(WmsLogTag::DMS, "SendCastEvent entry isPlugIn:%{public}d", isPlugIn);
    if (!ScreenCastConnection::GetInstance().CastConnectExtension()) {
        TLOGE(WmsLogTag::DMS, "CastConnectionExtension failed");
        return;
    }
    if (!ScreenCastConnection::GetInstance().IsConnectedSync()) {
        TLOGE(WmsLogTag::DMS, "CastConnectionExtension connected failed");
        ScreenCastConnection::GetInstance().CastDisconnectExtension();
    }
    MessageParcel data;
    MessageParcel reply;
    if (isPlugIn) {
        ScreenCastConnection::GetInstance().SendMessageToCastService(CAST_WIRED_PROJECTION_START, data, reply);
    } else {
        ScreenCastConnection::GetInstance().SendMessageToCastService(CAST_WIRED_PROJECTION_STOP, data, reply);
    }
    ScreenCastConnection::GetInstance().CastDisconnectExtension();
}

void ScreenSessionManager::NotifyCastWhenScreenConnectChange(bool isConnected)
{
    if (isConnected) {
        auto task = [this]() {
            SendCastEvent(true);
            ScreenSessionPublish::GetInstance().PublishCastPlugInEvent();
        };
        taskScheduler_->PostAsyncTask(task, "SendCastEventTrue");
        TLOGI(WmsLogTag::DMS, "PostAsyncTask SendCastEventTrue");
    } else {
        auto task = [this]() {
            SendCastEvent(false);
            ScreenSessionPublish::GetInstance().PublishCastPlugOutEvent();
        };
        taskScheduler_->PostAsyncTask(task, "SendCastEventFalse");
        TLOGI(WmsLogTag::DMS, "PostAsyncTask SendCastEventFalse");
    }
}

void ScreenSessionManager::HandleScreenEvent(sptr<ScreenSession> screenSession,
    ScreenId screenId, ScreenEvent screenEvent)
{
    bool phyMirrorEnable = IsDefaultMirrorMode(screenId);
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
        if (phyMirrorEnable) {
            NotifyScreenConnected(screenSession->ConvertToScreenInfo());
            NotifyCastWhenScreenConnectChange(true);
            isPhyScreenConnected_ = true;
        }
        return;
    } else if (screenEvent == ScreenEvent::DISCONNECTED) {
        if (phyMirrorEnable) {
            NotifyScreenDisconnected(screenSession->GetScreenId());
            NotifyCastWhenScreenConnectChange(false);
            FreeDisplayMirrorNodeInner(screenSession);
            isPhyScreenConnected_ = false;
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
        TLOGI(WmsLogTag::DMS, "DisconnectScreenSession success. ScreenId: %{public}" PRIu64 "", screenId);
    }
}

void ScreenSessionManager::OnHgmRefreshRateChange(uint32_t refreshRate)
{
    GetDefaultScreenId();
    TLOGD(WmsLogTag::DMS, "Set refreshRate: %{public}u, defaultscreenid: %{public}" PRIu64"",
        refreshRate, defaultScreenId_);
    sptr<ScreenSession> screenSession = GetScreenSession(defaultScreenId_);
    if (screenSession) {
        screenSession->UpdateRefreshRate(refreshRate);
        NotifyDisplayChanged(screenSession->ConvertToDisplayInfo(),
            DisplayChangeEvent::UPDATE_REFRESHRATE);
    } else {
        TLOGE(WmsLogTag::DMS, "Get default screen session failed.");
    }
    return;
}

sptr<ScreenSession> ScreenSessionManager::GetScreenSession(ScreenId screenId) const
{
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    if (screenSessionMap_.empty()) {
        screenEventTracker_.LogWarningAllInfos();
    }
    auto iter = screenSessionMap_.find(screenId);
    if (iter == screenSessionMap_.end()) {
        TLOGI(WmsLogTag::DMS, "Error found screen session with id: %{public}" PRIu64"", screenId);
        return nullptr;
    }
    return iter->second;
}

sptr<ScreenSession> ScreenSessionManager::GetDefaultScreenSession()
{
    GetDefaultScreenId();
    return GetScreenSession(defaultScreenId_);
}

sptr<DisplayInfo> ScreenSessionManager::HookDisplayInfoByUid(sptr<DisplayInfo> displayInfo)
{
    if (displayInfo == nullptr) {
        TLOGI(WmsLogTag::DMS, "ConvertToDisplayInfo error, displayInfo is nullptr.");
        return nullptr;
    }
    auto uid = IPCSkeleton::GetCallingUid();
    std::shared_lock<std::shared_mutex> lock(hookInfoMutex_);
    if (displayHookMap_.find(uid) != displayHookMap_.end()) {
        auto info = displayHookMap_[uid];
        TLOGI(WmsLogTag::DMS, "hookWidth: %{public}u, hookHeight: %{public}u, hookDensity: %{public}f, "
            "hookRotation: %{public}u, hookenableHookRotation: %{public}d, displayWidth: %{public}u, "
            "displayHeigth: %{public}u, displayrotation: %{public}u, displayOrientation: %{public}u",
            info.width_, info.height_, info.density_, info.rotation_, info.enableHookRotation_, displayInfo->GetWidth(),
            displayInfo->GetHeight(), displayInfo->GetRotation(), displayInfo->GetDisplayOrientation());

        displayInfo->SetWidth(info.width_);
        displayInfo->SetHeight(info.height_);
        displayInfo->SetVirtualPixelRatio(info.density_);
        if (info.enableHookRotation_) {
            sptr screenSession = GetScreenSession(displayInfo->GetScreenId());
            if (screenSession) {
                Rotation targetRotation = screenSession->ConvertIntToRotation(static_cast<int32_t>(info.rotation_));
                displayInfo->SetRotation(targetRotation);
                DisplayOrientation displayOrientation = screenSession->CalcDisplayOrientation(targetRotation,
                FoldDisplayMode::UNKNOWN);
                TLOGI(WmsLogTag::DMS, "targetRotation: %{public}u, targetOrientation: %{public}u",
                targetRotation, displayOrientation);
                displayInfo->SetDisplayOrientation(displayOrientation);
            } else {
                TLOGE(WmsLogTag::DMS, "Get default screen session failed.");
                return nullptr;
            }
        }
    }
    return displayInfo;
}

sptr<DisplayInfo> ScreenSessionManager::GetDefaultDisplayInfo()
{
    DmsXcollie dmsXcollie("DMS:GetDefaultDisplayInfo", XCOLLIE_TIMEOUT_10S);
    GetDefaultScreenId();
    sptr<ScreenSession> screenSession = GetScreenSession(defaultScreenId_);
    std::lock_guard<std::recursive_mutex> lock_info(displayInfoMutex_);
    if (screenSession) {
        sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
        if (displayInfo == nullptr) {
            TLOGI(WmsLogTag::DMS, "ConvertToDisplayInfo error, displayInfo is nullptr.");
            return nullptr;
        }
        // 在PC/PAD上安装的竖屏应用以及白名单中的应用在显示状态非全屏时需要hook displayinfo
        displayInfo = HookDisplayInfoByUid(displayInfo);
        return displayInfo;
    } else {
        TLOGE(WmsLogTag::DMS, "Get default screen session failed.");
        return nullptr;
    }
}

sptr<DisplayInfo> ScreenSessionManager::GetDisplayInfoById(DisplayId displayId)
{
    TLOGD(WmsLogTag::DMS, "GetDisplayInfoById enter, displayId: %{public}" PRIu64" ", displayId);
    DmsXcollie dmsXcollie("DMS:GetDisplayInfoById", XCOLLIE_TIMEOUT_10S);
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (auto sessionIt : screenSessionMap_) {
        auto screenSession = sessionIt.second;
        if (screenSession == nullptr) {
            TLOGI(WmsLogTag::DMS, "GetDisplayInfoById screenSession is nullptr, ScreenId: %{public}" PRIu64 "",
                sessionIt.first);
            continue;
        }
        sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
        if (displayInfo == nullptr) {
            TLOGI(WmsLogTag::DMS, "ConvertToDisplayInfo error, displayInfo is nullptr.");
            continue;
        }
        if (displayId == displayInfo->GetDisplayId()) {
            TLOGD(WmsLogTag::DMS, "GetDisplayInfoById success");
            displayInfo = HookDisplayInfoByUid(displayInfo);
            return displayInfo;
        }
    }
    TLOGE(WmsLogTag::DMS, "GetDisplayInfoById failed. displayId: %{public}" PRIu64" ", displayId);
    return nullptr;
}

sptr<DisplayInfo> ScreenSessionManager::GetDisplayInfoByScreen(ScreenId screenId)
{
    TLOGD(WmsLogTag::DMS, "enter, screenId: %{public}" PRIu64"", screenId);
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (auto sessionIt : screenSessionMap_) {
        auto screenSession = sessionIt.second;
        if (screenSession == nullptr) {
            TLOGI(WmsLogTag::DMS, "GetDisplayInfoByScreen screenSession is nullptr, ScreenId:%{public}" PRIu64"",
                sessionIt.first);
            continue;
        }
        sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
        if (displayInfo == nullptr) {
            TLOGI(WmsLogTag::DMS, "GetDisplayInfoByScreen error, displayInfo is nullptr.");
            continue;
        }
        if (screenId == displayInfo->GetScreenId()) {
            return displayInfo;
        }
    }
    TLOGE(WmsLogTag::DMS, "GetDisplayInfoByScreen failed. screenId: %{public}" PRIu64" ", screenId);
    return nullptr;
}

std::vector<DisplayId> ScreenSessionManager::GetAllDisplayIds()
{
    TLOGI(WmsLogTag::DMS, "GetAllDisplayIds enter");
    std::vector<DisplayId> res;
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (auto sessionIt : screenSessionMap_) {
        auto screenSession = sessionIt.second;
        if (screenSession == nullptr) {
            TLOGE(WmsLogTag::DMS, "GetAllDisplayIds screenSession is nullptr, ScreenId:%{public}" PRIu64"",
                sessionIt.first);
            continue;
        }
        sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
        if (displayInfo == nullptr) {
            TLOGE(WmsLogTag::DMS, "GetAllDisplayIds error, displayInfo is nullptr.");
            continue;
        }
        DisplayId displayId = displayInfo->GetDisplayId();
        res.push_back(displayId);
    }
    return res;
}

sptr<ScreenInfo> ScreenSessionManager::GetScreenInfoById(ScreenId screenId)
{
    DmsXcollie dmsXcollie("DMS:GetScreenInfoById", XCOLLIE_TIMEOUT_10S);
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "GetScreenInfoById permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return nullptr;
    }
    auto screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "GetScreenInfoById cannot find screenInfo: %{public}" PRIu64"", screenId);
        return nullptr;
    }
    return screenSession->ConvertToScreenInfo();
}

DMError ScreenSessionManager::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    TLOGI(WmsLogTag::DMS, "SetScreenActiveMode: ScreenId: %{public}" PRIu64", modeId: %{public}u", screenId, modeId);
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set screen active permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "SetScreenActiveMode: invalid screenId");
        return DMError::DM_ERROR_NULLPTR;
    }
    {
        sptr<ScreenSession> screenSession = GetScreenSession(screenId);
        if (screenSession == nullptr) {
            TLOGE(WmsLogTag::DMS, "SetScreenActiveMode: Get ScreenSession failed");
            return DMError::DM_ERROR_NULLPTR;
        }
        ScreenId rsScreenId = SCREEN_ID_INVALID;
        if (!screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId)) {
            TLOGE(WmsLogTag::DMS, "SetScreenActiveMode: No corresponding rsId");
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

bool ScreenSessionManager::ConvertScreenIdToRsScreenId(ScreenId screenId, ScreenId& rsScreenId)
{
    return screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId);
}

void ScreenSessionManager::UpdateDisplayHookInfo(int32_t uid, bool enable, DMHookInfo hookInfo)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "UpdateDisplayHookInfo permission denied!");
        return;
    }

    std::unique_lock<std::shared_mutex> lock(hookInfoMutex_);
    if (enable) {
        if (uid != 0) {
            displayHookMap_[uid] = hookInfo;
        }
    } else {
        displayHookMap_.erase(uid);
    }
}

void ScreenSessionManager::NotifyScreenChanged(sptr<ScreenInfo> screenInfo, ScreenChangeEvent event)
{
    if (screenInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "NotifyScreenChanged error, screenInfo is nullptr.");
        return;
    }
    auto task = [=] {
        TLOGI(WmsLogTag::DMS, "NotifyScreenChanged,  screenId:%{public}" PRIu64"", screenInfo->GetScreenId());
        auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
        if (agents.empty()) {
            TLOGI(WmsLogTag::DMS, "NotifyScreenChanged agents is empty");
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
        TLOGE(WmsLogTag::DMS, "set virtual pixel permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }

    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        TLOGE(WmsLogTag::DMS, "screen session is nullptr");
        return DMError::DM_ERROR_UNKNOWN;
    }
    if (screenSession->isScreenGroup_) {
        TLOGE(WmsLogTag::DMS, "cannot set virtual pixel ratio to the combination. screen: %{public}" PRIu64"",
            screenId);
        return DMError::DM_ERROR_NULLPTR;
    }
    if (fabs(screenSession->GetScreenProperty().GetVirtualPixelRatio() - virtualPixelRatio) < 1e-6) {
        TLOGE(WmsLogTag::DMS,
            "The density is equivalent to the original value, no update operation is required, aborted.");
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

DMError ScreenSessionManager::SetVirtualPixelRatioSystem(ScreenId screenId, float virtualPixelRatio)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set virtual pixel permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }

    if (clientProxy_) {
        clientProxy_->SetVirtualPixelRatioSystem(screenId, virtualPixelRatio);
    }
    return DMError::DM_OK;
}

DMError ScreenSessionManager::SetResolution(ScreenId screenId, uint32_t width, uint32_t height, float virtualPixelRatio)
{
    TLOGI(WmsLogTag::DMS,
        "SetResolution ScreenId: %{public}" PRIu64 ", w: %{public}u, h: %{public}u, virtualPixelRatio: %{public}f",
        screenId, width, height, virtualPixelRatio);
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "SetResolution permission denied! calling clientName: %{public}s, pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "SetResolution: invalid screenId");
        return DMError::DM_ERROR_NULLPTR;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "SetResolution: Get ScreenSession failed");
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
        TLOGE(WmsLogTag::DMS, "SetResolution invalid param! w:%{public}u h:%{public}u min:%{public}f max:%{public}f",
            screenSessionModes->width_,
            screenSessionModes->height_,
            static_cast<float>(DOT_PER_INCH_MINIMUM_VALUE) / DOT_PER_INCH,
            static_cast<float>(DOT_PER_INCH_MAXIMUM_VALUE) / DOT_PER_INCH);
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    screenSession->SetDensityInCurResolution(virtualPixelRatio);
    DMError ret = SetVirtualPixelRatio(screenId, virtualPixelRatio);
    if (ret != DMError::DM_OK) {
        TLOGE(WmsLogTag::DMS, "Failed to setVirtualPixelRatio when settingResolution");
        screenSession->SetDensityInCurResolution(screenSession->GetScreenProperty().GetVirtualPixelRatio());
        return ret;
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

DMError ScreenSessionManager::GetDensityInCurResolution(ScreenId screenId, float& virtualPixelRatio)
{
    DmsXcollie dmsXcollie("DMS:GetDensityInCurResolution", XCOLLIE_TIMEOUT_10S);
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "GetDensityInCurResolution: Get ScreenSession failed");
        return DMError::DM_ERROR_NULLPTR;
    }

    virtualPixelRatio = screenSession->GetScreenProperty().GetDensityInCurResolution();
    return DMError::DM_OK;
}

DMError ScreenSessionManager::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    TLOGI(WmsLogTag::DMS, "GetScreenColorGamut::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "GetScreenColorGamut screenId invalid");
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
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set screen color gamut  permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }

    TLOGI(WmsLogTag::DMS, "SetScreenColorGamut::ScreenId: %{public}" PRIu64 ", colorGamutIdx %{public}d",
        screenId, colorGamutIdx);
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "SetScreenColorGamut screenId invalid");
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
    TLOGI(WmsLogTag::DMS, "GetScreenGamutMap::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "GetScreenGamutMap screenId invalid");
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
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set screen gamut map permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }

    TLOGI(WmsLogTag::DMS, "SetScreenGamutMap::ScreenId: %{public}" PRIu64 ", ScreenGamutMap %{public}u",
        screenId, static_cast<uint32_t>(gamutMap));
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "SetScreenGamutMap screenId invalid");
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
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set Screen color transform permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }

    TLOGI(WmsLogTag::DMS, "SetScreenColorTransform::ScreenId: %{public}" PRIu64 "", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "SetScreenColorTransform screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->SetScreenColorTransform();
}

sptr<ScreenSession> ScreenSessionManager::CreatePhysicalMirrorSessionInner(ScreenId screenId, ScreenId defScreenId,
    ScreenProperty property)
{
    sptr<ScreenSession> screenSession = nullptr;
    if (system::GetBoolParameter("persist.edm.disallow_mirror", false)) {
        TLOGW(WmsLogTag::DMS, "mirror disabled by edm!");
        return screenSession;
    }
    sptr<ScreenSession> defaultScreen = GetDefaultScreenSession();
    if (defaultScreen == nullptr || defaultScreen->GetDisplayNode() == nullptr) {
        TLOGE(WmsLogTag::DMS, "default screen null");
        return screenSession;
    }
    NodeId nodeId = defaultScreen->GetDisplayNode()->GetId();
    TLOGI(WmsLogTag::DMS, "physical mirror screen nodeId: %{public}" PRIu64 "", nodeId);
    ScreenSessionConfig config = {
        .screenId = screenId,
        .defaultScreenId = defScreenId,
        .mirrorNodeId = nodeId,
        .property = property,
    };
    screenSession = new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_MIRROR);
    if (!screenSession) {
        TLOGE(WmsLogTag::DMS, "screenSession is null");
        return nullptr;
    }
    if (ScreenSceneConfig::GetExternalScreenDefaultMode() == "none") {
        // pc is none, pad&&phone is mirror
        screenSession->SetName("ExtendedDisplay");
    } else {
        screenSession->SetName("CastEngine");
    }
    screenSession->SetMirrorScreenType(MirrorScreenType::PHYSICAL_MIRROR);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    NotifyScreenChanged(screenSession->ConvertToScreenInfo(), ScreenChangeEvent::SCREEN_SWITCH_CHANGE);
    hdmiScreenCount_ = hdmiScreenCount_ + 1;
    NotifyCaptureStatusChanged();
    return screenSession;
}

sptr<ScreenSession> ScreenSessionManager::GetScreenSessionInner(ScreenId screenId, ScreenProperty property)
{
    ScreenId defScreenId = GetDefaultScreenId();
    TLOGI(WmsLogTag::DMS, "GetScreenSessionInner: screenId:%{public}" PRIu64 "", screenId);
    if (IsDefaultMirrorMode(screenId)) {
        return CreatePhysicalMirrorSessionInner(screenId, defScreenId, property);
    }
    std::string screenName = "UNKNOWN";
    if (screenId == SCREEN_ID_MAIN) {
        screenName = "SubScreen";
    }
    ScreenSessionConfig config = {
        .screenId = screenId,
        .defaultScreenId = defScreenId,
        .name = screenName,
        .property = property,
    };
    return new ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_REAL);
}

void ScreenSessionManager::CreateScreenProperty(ScreenId screenId, ScreenProperty& property)
{
    int id = HiviewDFX::XCollie::GetInstance().SetTimer("CreateScreenPropertyCallRS", XCOLLIE_TIMEOUT_10S, nullptr,
        nullptr, HiviewDFX::XCOLLIE_FLAG_LOG);
    TLOGI(WmsLogTag::DMS, "Call rsInterface_ GetScreenActiveMode ScreenId: %{public}" PRIu64 "", screenId);
    auto screenMode = rsInterface_.GetScreenActiveMode(screenId);
    auto screenBounds = RRect({ 0, 0, screenMode.GetScreenWidth(), screenMode.GetScreenHeight() }, 0.0f, 0.0f);
    auto screenRefreshRate = screenMode.GetScreenRefreshRate();
    TLOGI(WmsLogTag::DMS, "Call rsInterface_ GetScreenCapability ScreenId: %{public}" PRIu64 "", screenId);
    auto screenCapability = rsInterface_.GetScreenCapability(screenId);
    HiviewDFX::XCollie::GetInstance().CancelTimer(id);
    TLOGI(WmsLogTag::DMS, "Call RS interface end, create ScreenProperty begin");
    property.SetRotation(0.0f);
    property.SetPhyWidth(screenCapability.GetPhyWidth());
    property.SetPhyHeight(screenCapability.GetPhyHeight());
    property.SetDpiPhyBounds(screenCapability.GetPhyWidth(), screenCapability.GetPhyHeight());
    property.SetPhyBounds(screenBounds);
    property.SetBounds(screenBounds);
    property.SetAvailableArea({0, 0, screenMode.GetScreenWidth(), screenMode.GetScreenHeight()});
    if (isDensityDpiLoad_) {
        if (screenId == SCREEN_ID_MAIN) {
            TLOGI(WmsLogTag::DMS, "subDensityDpi_ = %{public}f", subDensityDpi_);
            property.SetVirtualPixelRatio(subDensityDpi_);
            property.SetDefaultDensity(subDensityDpi_);
            property.SetDensityInCurResolution(subDensityDpi_);
        } else {
            TLOGI(WmsLogTag::DMS, "densityDpi_ = %{public}f", densityDpi_);
            property.SetVirtualPixelRatio(densityDpi_);
            property.SetDefaultDensity(densityDpi_);
            property.SetDensityInCurResolution(densityDpi_);
        }
    } else {
        property.UpdateVirtualPixelRatio(screenBounds);
    }
    property.SetRefreshRate(screenRefreshRate);
    property.SetDefaultDeviceRotationOffset(defaultDeviceRotationOffset_);

    if (foldScreenController_ != nullptr && screenId == 0
        && (g_screenRotationOffSet == ROTATION_90 || g_screenRotationOffSet == ROTATION_270)) {
        screenBounds = RRect({ 0, 0, screenMode.GetScreenHeight(), screenMode.GetScreenWidth() }, 0.0f, 0.0f);
        property.SetBounds(screenBounds);
    }
    property.CalcDefaultDisplayOrientation();
}

sptr<ScreenSession> ScreenSessionManager::GetOrCreateScreenSession(ScreenId screenId)
{
    TLOGI(WmsLogTag::DMS, "GetOrCreateScreenSession ENTER. ScreenId: %{public}" PRIu64 "", screenId);
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession) {
        TLOGI(WmsLogTag::DMS, "screenSession Exist ScreenId: %{public}" PRIu64 "", screenId);
        return screenSession;
    }

    ScreenId rsId = screenId;
    screenIdManager_.UpdateScreenId(rsId, screenId);

    ScreenProperty property;
    CreateScreenProperty(screenId, property);
    TLOGI(WmsLogTag::DMS, "CreateScreenProperty end");
    screenEventTracker_.RecordEvent("CreateScreenProperty by rsInterface success.");
    {
        std::lock_guard<std::recursive_mutex> lock_phy(phyScreenPropMapMutex_);
        phyScreenPropMap_[screenId] = property;
    }

    if (foldScreenController_ != nullptr) {
        // sensor may earlier than screen connect, when physical screen property changed, update
        foldScreenController_->UpdateForPhyScreenPropertyChange();
        /* folder screen outer screenId is 5 */
        if (screenId == SCREEN_ID_MAIN) {
            SetPostureAndHallSensorEnabled();
        }
        if (screenId == SCREEN_ID_MAIN && !FoldScreenStateInternel::IsDualDisplayFoldDevice()) {
            return nullptr;
        }
    }

    sptr<ScreenSession> session = GetScreenSessionInner(screenId, property);
    if (session == nullptr) {
        TLOGE(WmsLogTag::DMS, "get screen session fail ScreenId: %{public}" PRIu64, screenId);
        return session;
    }
    session->RegisterScreenChangeListener(this);
    InitAbstractScreenModesInfo(session);
    session->groupSmsId_ = 1;
    {
        std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
        screenSessionMap_[screenId] = session;
    }
    screenEventTracker_.RecordEvent("create screen session success.");
    SetHdrFormats(screenId, session);
    SetColorSpaces(screenId, session);
    RegisterRefreshRateChangeListener();
    TLOGI(WmsLogTag::DMS, "CreateScreenSession success. ScreenId: %{public}" PRIu64 "", screenId);
    return session;
}

void ScreenSessionManager::SetHdrFormats(ScreenId screenId, sptr<ScreenSession>& session)
{
    TLOGI(WmsLogTag::DMS, "SetHdrFormats %{public}" PRIu64, screenId);
    std::vector<ScreenHDRFormat> rsHdrFormat;
    auto status = rsInterface_.GetScreenSupportedHDRFormats(screenId, rsHdrFormat);
    if (static_cast<StatusCode>(status) != StatusCode::SUCCESS) {
        TLOGE(WmsLogTag::DMS, "get hdr format failed! status code: %{public}d", status);
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
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set Screen color spaces permission denied!");
        return;
    }

    TLOGI(WmsLogTag::DMS, "SetColorSpaces %{public}" PRIu64, screenId);
    std::vector<GraphicCM_ColorSpaceType> rsColorSpace;
    auto status = rsInterface_.GetScreenSupportedColorSpaces(screenId, rsColorSpace);
    if (static_cast<StatusCode>(status) != StatusCode::SUCCESS) {
        TLOGE(WmsLogTag::DMS, "get color space failed! status code: %{public}d", status);
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
        std::ostringstream oss;
        oss << "Default screen id : " << defaultScreenId_;
        TLOGI(WmsLogTag::DMS, "%{public}s", oss.str().c_str());
        screenEventTracker_.RecordEvent(oss.str());
    }
    return defaultScreenId_;
}

bool ScreenSessionManager::WakeUpBegin(PowerStateChangeReason reason)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "[UL_POWER]ssm:WakeUpBegin(%u)", reason);
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "WakeUpBegin permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return false;
    }
    currentWakeUpReason_ = reason;
    TLOGI(WmsLogTag::DMS, "[UL_POWER]WakeUpBegin reason: %{public}u", static_cast<uint32_t>(reason));

    if (reason == PowerStateChangeReason::STATE_CHANGE_REASON_COLLABORATION) {
        isMultiScreenCollaboration_ = true;
        return true;
    }
    // Handling Power Button Conflicts
    if (IsFastFingerprintReason(lastWakeUpReason_) && reason == PowerStateChangeReason::STATE_CHANGE_REASON_POWER_KEY) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]WakeUpBegin buttonBlock_ is true");
        buttonBlock_ = true;
        screenOnCV_.notify_all();
        usleep(SLEEP_10_MS);
    }
    lastWakeUpReason_ = reason;
    return NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP, EventStatus::BEGIN, reason);
}

bool ScreenSessionManager::WakeUpEnd()
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "[UL_POWER]ssm:WakeUpEnd");
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "WakeUpEnd permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return false;
    }
    TLOGI(WmsLogTag::DMS, "[UL_POWER]WakeUpEnd enter");
    if (isMultiScreenCollaboration_) {
        isMultiScreenCollaboration_ = false;
        return true;
    }
    return NotifyDisplayPowerEvent(DisplayPowerEvent::WAKE_UP, EventStatus::END,
        PowerStateChangeReason::STATE_CHANGE_REASON_INIT);
}

bool ScreenSessionManager::SuspendBegin(PowerStateChangeReason reason)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "[UL_POWER]ssm:SuspendBegin(%u)", reason);
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "SuspendBegin permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return false;
    }

    gotScreenlockFingerprint_ = false;
    TLOGI(WmsLogTag::DMS, "[UL_POWER]Reason: %{public}u", static_cast<uint32_t>(reason));
    lastWakeUpReason_ = PowerStateChangeReason::STATE_CHANGE_REASON_INIT;
    if (reason == PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_OFF) {
        lastWakeUpReason_ = PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_OFF;
    }

    gotScreenOffNotify_  = false;
    sessionDisplayPowerController_->SuspendBegin(reason);
    if (reason == PowerStateChangeReason::STATE_CHANGE_REASON_COLLABORATION) {
        isMultiScreenCollaboration_ = true;
        return true;
    }
    return NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP, EventStatus::BEGIN, reason);
}

bool ScreenSessionManager::SuspendEnd()
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "SuspendEnd permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return false;
    }
    TLOGI(WmsLogTag::DMS, "[UL_POWER]SuspendEnd enter");
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "[UL_POWER]ssm:SuspendEnd");
    if (isMultiScreenCollaboration_) {
        isMultiScreenCollaboration_ = false;
        return true;
    }
    return NotifyDisplayPowerEvent(DisplayPowerEvent::SLEEP, EventStatus::END,
        PowerStateChangeReason::STATE_CHANGE_REASON_INIT);
}

bool ScreenSessionManager::IsPreBrightAuthFail(void)
{
    return lastWakeUpReason_ == PowerStateChangeReason::
        STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_OFF;
}

bool ScreenSessionManager::BlockSetDisplayState(void)
{
    return prePowerStateChangeReason_ == PowerStateChangeReason::POWER_BUTTON;
}

bool ScreenSessionManager::SetDisplayState(DisplayState state)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "SetDisplayState permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return false;
    }
    TLOGI(WmsLogTag::DMS, "[UL_POWER]SetDisplayState enter");
    auto screenIds = GetAllScreenIds();
    if (screenIds.empty()) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]no screen info");
        return false;
    }

    for (auto screenId : screenIds) {
        sptr<ScreenSession> screenSession = GetScreenSession(screenId);
        if (screenSession == nullptr) {
            TLOGW(WmsLogTag::DMS, "[UL_POWER]SetDisplayState cannot get ScreenSession, screenId: %{public}" PRIu64"",
                screenId);
            continue;
        }
        screenSession->UpdateDisplayState(state);
        TLOGI(WmsLogTag::DMS, "[UL_POWER]set screenSession displayState property: %{public}u",
            screenSession->GetScreenProperty().GetDisplayState());
    }
    return sessionDisplayPowerController_->SetDisplayState(state);
}

bool ScreenSessionManager::IsFastFingerprintReason(PowerStateChangeReason reason)
{
    switch (reason) {
        case PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT : {
            [[fallthrough]];
        }
        case PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_SUCCESS : {
            [[fallthrough]];
        }
        case PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_ON : {
            [[fallthrough]];
        }
        case PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_OFF : {
            return true;
        }
        default: {
            return false;
        }
    }
}

void ScreenSessionManager::BlockScreenOnByCV(void)
{
    // Handling Power Button Conflicts
    if (buttonBlock_ && currentWakeUpReason_ != PowerStateChangeReason::STATE_CHANGE_REASON_POWER_KEY) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]BlockScreenOnByCV exit because buttonBlock_");
        return;
    }

    if (keyguardDrawnDone_ == false) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]screenOnCV_ set");
        needScreenOnWhenKeyguardNotify_ = true;
        std::unique_lock<std::mutex> lock(screenOnMutex_);
        if (screenOnCV_.wait_for(lock, std::chrono::milliseconds(CV_WAIT_SCREENON_MS)) == std::cv_status::timeout) {
            TLOGI(WmsLogTag::DMS, "[UL_POWER]wait ScreenOnCV_ timeout");
        }
    }
}

void ScreenSessionManager::BlockScreenOffByCV(void)
{
    if (gotScreenOffNotify_  == false) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]screenOffCV_ set, delay:%{public}d", screenOffDelay_);
        needScreenOffNotify_ = true;
        std::unique_lock<std::mutex> lock(screenOffMutex_);
        if (screenOffCV_.wait_for(lock, std::chrono::milliseconds(screenOffDelay_)) == std::cv_status::timeout) {
            isScreenLockSuspend_ = false;
            TLOGI(WmsLogTag::DMS, "[UL_POWER]wait ScreenOffCV_ timeout, isScreenLockSuspend_ is false");
        }
    }
}

int32_t ScreenSessionManager::SetScreenOffDelayTime(int32_t delay)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set screen off delay time permission denied!");
        return 0;
    }

    if (delay < CV_WAIT_SCREENOFF_MS) {
        screenOffDelay_ = CV_WAIT_SCREENOFF_MS;
    } else if (delay > CV_WAIT_SCREENOFF_MS_MAX) {
        screenOffDelay_ = CV_WAIT_SCREENOFF_MS_MAX;
    } else {
        screenOffDelay_ = delay;
    }
    TLOGI(WmsLogTag::DMS, "SetScreenOffDelayTime, delay:%{public}d", delay);
    return screenOffDelay_;
}

bool ScreenSessionManager::IsScreenLockSuspend(void)
{
    return isScreenLockSuspend_;
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

bool ScreenSessionManager::SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state,
    PowerStateChangeReason reason)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "SetSpecifiedScreenPower permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return false;
    }
    TLOGI(WmsLogTag::DMS, "[UL_POWER]SetSpecifiedScreenPower: screen id:%{public}" PRIu64 ", state:%{public}u",
        screenId, state);

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
            TLOGW(WmsLogTag::DMS, "[UL_POWER]SetScreenPowerStatus state not support");
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
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "SetScreenPowerForAll permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return false;
    }
    TLOGI(WmsLogTag::DMS, "[UL_POWER]state: %{public}u, reason: %{public}u",
        static_cast<uint32_t>(state), static_cast<uint32_t>(reason));
    ScreenPowerStatus status;

    // Handling Power Button Conflicts
    if (buttonBlock_ && reason != PowerStateChangeReason::STATE_CHANGE_REASON_POWER_KEY &&
            state == ScreenPowerState::POWER_ON) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]SetScreenPowerForAll exit because buttonBlock_");
        buttonBlock_ = false;
        return true;
    }

    if (!GetPowerStatus(state, reason, status)) {
        return false;
    }
    gotScreenOffNotify_  = false;
    keyguardDrawnDone_ = false;
    TLOGI(WmsLogTag::DMS, "[UL_POWER]SetScreenPowerForAll keyguardDrawnDone_ is false");
    prePowerStateChangeReason_ = reason;
    return SetScreenPower(status, reason);
}

bool ScreenSessionManager::GetPowerStatus(ScreenPowerState state, PowerStateChangeReason reason,
    ScreenPowerStatus& status)
{
    switch (state) {
        case ScreenPowerState::POWER_ON: {
            if (reason == PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT) {
                status = ScreenPowerStatus::POWER_STATUS_ON_ADVANCED;
                TLOGI(WmsLogTag::DMS, "[UL_POWER]Set ScreenPowerStatus: POWER_STATUS_ON_ADVANCED");
            } else {
                status = ScreenPowerStatus::POWER_STATUS_ON;
                TLOGI(WmsLogTag::DMS, "[UL_POWER]Set ScreenPowerStatus: POWER_STATUS_ON");
            }
            break;
        }
        case ScreenPowerState::POWER_OFF: {
            if (reason == PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_OFF) {
                status = ScreenPowerStatus::POWER_STATUS_OFF_ADVANCED;
                TLOGI(WmsLogTag::DMS, "[UL_POWER]Set ScreenPowerStatus: POWER_STATUS_OFF_ADVANCED");
            } else {
                status = ScreenPowerStatus::POWER_STATUS_OFF;
                TLOGI(WmsLogTag::DMS, "[UL_POWER]Set ScreenPowerStatus: POWER_STATUS_OFF");
            }
            rsInterface_.MarkPowerOffNeedProcessOneFrame();
            break;
        }
        case ScreenPowerState::POWER_SUSPEND: {
            status = ScreenPowerStatus::POWER_STATUS_SUSPEND;
            TLOGI(WmsLogTag::DMS, "[UL_POWER]Set ScreenPowerStatus: POWER_SUSPEND");
            rsInterface_.MarkPowerOffNeedProcessOneFrame();
            break;
        }
        default: {
            TLOGW(WmsLogTag::DMS, "[UL_POWER]SetScreenPowerStatus state not support");
            return false;
        }
    }
    return true;
}

void ScreenSessionManager::ExitCoordination(const std::string& reason)
{
    if (GetFoldDisplayMode() != FoldDisplayMode::COORDINATION) {
        return;
    }
    if (foldScreenController_ != nullptr) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]ExitCoordination, reason:%{public}s", reason.c_str());
        foldScreenController_->ExitCoordination();
    }
}

bool ScreenSessionManager::SetScreenPower(ScreenPowerStatus status, PowerStateChangeReason reason)
{
    TLOGI(WmsLogTag::DMS, "[UL_POWER]SetScreenPower enter status:%{public}u", status);
    auto screenIds = GetAllScreenIds();
    if (screenIds.empty()) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]SetScreenPower screenIds empty");
        return false;
    }

    // Handling Power Button Conflicts
    if (buttonBlock_ && reason != PowerStateChangeReason::STATE_CHANGE_REASON_POWER_KEY &&
            status != ScreenPowerStatus::POWER_STATUS_OFF) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]SetScreenPower exit because buttonBlock_");
        buttonBlock_ = false;
        return true;
    }

    if (status == ScreenPowerStatus::POWER_STATUS_OFF || status == ScreenPowerStatus::POWER_STATUS_SUSPEND) {
        ExitCoordination("Press PowerKey");
    }

    if (((status == ScreenPowerStatus::POWER_STATUS_OFF || status == ScreenPowerStatus::POWER_STATUS_SUSPEND) &&
        gotScreenlockFingerprint_ == true) &&
        prePowerStateChangeReason_ != PowerStateChangeReason::STATE_CHANGE_REASON_SHUT_DOWN) {
        gotScreenlockFingerprint_ = false;
        return NotifyDisplayPowerEvent(status == ScreenPowerStatus::POWER_STATUS_ON ? DisplayPowerEvent::DISPLAY_ON :
            DisplayPowerEvent::DISPLAY_OFF, EventStatus::END, reason);
    }

    if (foldScreenController_ != nullptr) {
        rsInterface_.SetScreenPowerStatus(foldScreenController_->GetCurrentScreenId(), status);
    } else {
        for (auto screenId : screenIds) {
            rsInterface_.SetScreenPowerStatus(screenId, status);
        }
    }
    HandlerSensor(status, reason);
    if (reason == PowerStateChangeReason::STATE_CHANGE_REASON_COLLABORATION) {
        return true;
    }
    buttonBlock_ = false;
    if ((status == ScreenPowerStatus::POWER_STATUS_OFF || status == ScreenPowerStatus::POWER_STATUS_SUSPEND) &&
        gotScreenlockFingerprint_ == true) {
        gotScreenlockFingerprint_ = false;
    }

    return NotifyDisplayPowerEvent(status == ScreenPowerStatus::POWER_STATUS_ON ? DisplayPowerEvent::DISPLAY_ON :
        DisplayPowerEvent::DISPLAY_OFF, EventStatus::END, reason);
}

void ScreenSessionManager::SetScreenPowerForFold(ScreenPowerStatus status)
{
    SetScreenPowerForFold(foldScreenController_->GetCurrentScreenId(), status);
}

void ScreenSessionManager::SetScreenPowerForFold(ScreenId screenId, ScreenPowerStatus status)
{
    rsInterface_.SetScreenPowerStatus(screenId, status);
}

void ScreenSessionManager::SetKeyguardDrawnDoneFlag(bool flag)
{
    keyguardDrawnDone_ = flag;
}

void ScreenSessionManager::HandlerSensor(ScreenPowerStatus status, PowerStateChangeReason reason)
{
    if (ScreenSceneConfig::IsSupportRotateWithSensor()) {
        if (status == ScreenPowerStatus::POWER_STATUS_ON) {
            DmsXcollie dmsXcollie("DMS:SubscribeRotationSensor", XCOLLIE_TIMEOUT_5S);
            TLOGI(WmsLogTag::DMS, "subscribe rotation and posture sensor when phone turn on");
            ScreenSensorConnector::SubscribeRotationSensor();
#ifdef SENSOR_ENABLE
            if (g_foldScreenFlag && reason != PowerStateChangeReason::STATE_CHANGE_REASON_DISPLAY_SWITCH) {
                FoldScreenSensorManager::GetInstance().RegisterPostureCallback();
            } else {
                TLOGI(WmsLogTag::DMS, "not fold product, switch screen reason, failed register posture.");
            }
#endif
        } else if (status == ScreenPowerStatus::POWER_STATUS_OFF || status == ScreenPowerStatus::POWER_STATUS_SUSPEND) {
            TLOGI(WmsLogTag::DMS, "unsubscribe rotation and posture sensor when phone turn off");
            if (isMultiScreenCollaboration_) {
                TLOGI(WmsLogTag::DMS, "[UL_POWER]MultiScreenCollaboration, not unsubscribe rotation sensor");
            } else {
                DmsXcollie dmsXcollie("DMS:UnsubscribeRotationSensor", XCOLLIE_TIMEOUT_5S);
                ScreenSensorConnector::UnsubscribeRotationSensor();
            }
#ifdef SENSOR_ENABLE
            if (g_foldScreenFlag && reason != PowerStateChangeReason::STATE_CHANGE_REASON_DISPLAY_SWITCH) {
                FoldScreenSensorManager::GetInstance().UnRegisterPostureCallback();
            } else {
                TLOGI(WmsLogTag::DMS, "not fold product, switch screen reason, failed unregister posture.");
            }
#endif
        } else {
            TLOGI(WmsLogTag::DMS, "SetScreenPower state not support");
        }
    }
}

void ScreenSessionManager::BootFinishedCallback(const char *key, const char *value, void *context)
{
    if (strcmp(key, BOOTEVENT_BOOT_COMPLETED.c_str()) == 0 && strcmp(value, "true") == 0) {
        TLOGI(WmsLogTag::DMS, "BootFinishedCallback boot animation finished");
        auto &that = *reinterpret_cast<ScreenSessionManager *>(context);
        that.SetRotateLockedFromSettingData();
        that.SetDpiFromSettingData();
        that.RegisterSettingDpiObserver();
        if (that.foldScreenPowerInit_ != nullptr) {
            that.foldScreenPowerInit_();
        }
        that.RegisterSettingRotationObserver();
    }
}

void ScreenSessionManager::SetFoldScreenPowerInit(std::function<void()> foldScreenPowerInit)
{
    foldScreenPowerInit_ = foldScreenPowerInit;
}

void ScreenSessionManager::SetRotateLockedFromSettingData()
{
    uint32_t autoRotateStatus = AUTO_ROTATE_OFF;  // 0代表自动旋转关闭,1代表自动旋转打开
    bool islocked = true;
    // ret为true表示从数据库读到了值，并赋给了autoRotateStatus
    bool ret = ScreenSettingHelper::GetSettingValue(autoRotateStatus, SETTING_LOCKED_KEY);
    TLOGI(WmsLogTag::DMS, "get autoRotateStatus from settingdata: %{public}u", autoRotateStatus);
    if (autoRotateStatus) {
        islocked =false;
    }
    if (ret) {
        TLOGI(WmsLogTag::DMS, "get islocked success");
        SetScreenRotationLockedFromJs(islocked);
    }
}

void ScreenSessionManager::RegisterSettingDpiObserver()
{
    TLOGI(WmsLogTag::DMS, "Register Setting Dpi Observer");
    SettingObserver::UpdateFunc updateFunc = [&](const std::string& key) { SetDpiFromSettingData(); };
    ScreenSettingHelper::RegisterSettingDpiObserver(updateFunc);
}

void ScreenSessionManager::SetDpiFromSettingData()
{
    uint32_t settingDpi;
    bool ret = ScreenSettingHelper::GetSettingDpi(settingDpi);
    if (!ret) {
        TLOGW(WmsLogTag::DMS, "get setting dpi failed,use default dpi");
        settingDpi = defaultDpi;
    } else {
        TLOGI(WmsLogTag::DMS, "get setting dpi success,settingDpi: %{public}u", settingDpi);
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
    TLOGI(WmsLogTag::DMS, "[UL_POWER]NotifyDisplayEvent receive keyguardDrawnDone");
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGI(WmsLogTag::DMS, "NotifyDisplayEvent permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return;
    }
    sessionDisplayPowerController_->NotifyDisplayEvent(event);
    if (event == DisplayEvent::KEYGUARD_DRAWN) {
        keyguardDrawnDone_ = true;
        TLOGI(WmsLogTag::DMS, "[UL_POWER]keyguardDrawnDone_ is true");
        if (needScreenOnWhenKeyguardNotify_) {
            std::unique_lock <std::mutex> lock(screenOnMutex_);
            screenOnCV_.notify_all();
            TLOGI(WmsLogTag::DMS, "[UL_POWER]screenOnCV_ notify one");
            needScreenOnWhenKeyguardNotify_=false;
        }
    }

    if (event == DisplayEvent::SCREEN_LOCK_SUSPEND) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]screen lock suspend");
        gotScreenOffNotify_ = true;
        if (isPhyScreenConnected_) {
            isScreenLockSuspend_ = false;
            TLOGI(WmsLogTag::DMS, "[UL_POWER]isScreenLockSuspend__  is false");
        } else {
            isScreenLockSuspend_ = true;
        }
        if (needScreenOffNotify_) {
            ScreenOffCVNotify();
        }
    }

    if (event == DisplayEvent::SCREEN_LOCK_OFF) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]screen lock off");
        gotScreenOffNotify_ = true;
        isScreenLockSuspend_ = false;
        TLOGI(WmsLogTag::DMS, "[UL_POWER]isScreenLockSuspend__  is false");
        if (needScreenOffNotify_) {
            ScreenOffCVNotify();
        }
    }

    if (event == DisplayEvent::SCREEN_LOCK_FINGERPRINT) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]screen lock fingerprint");
        gotScreenOffNotify_ = true;
        gotScreenlockFingerprint_ = true;
        if (needScreenOffNotify_) {
            ScreenOffCVNotify();
        }
    }
}

void ScreenSessionManager::ScreenOffCVNotify(void)
{
    std::unique_lock <std::mutex> lock(screenOffMutex_);
    screenOffCV_.notify_all();
    needScreenOffNotify_ = false;
    TLOGI(WmsLogTag::DMS, "[UL_POWER]screenOffCV_ notify one");
}

ScreenPowerState ScreenSessionManager::GetScreenPower(ScreenId screenId)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "GetScreenPower permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return ScreenPowerState::INVALID_STATE;
    }
    auto state = static_cast<ScreenPowerState>(RSInterfaces::GetInstance().GetScreenPowerStatus(screenId));
    TLOGI(WmsLogTag::DMS, "GetScreenPower:%{public}u, screenId:%{public}" PRIu64".", state, screenId);
    return state;
}

DMError ScreenSessionManager::IsScreenRotationLocked(bool& isLocked)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "screen rotation locked permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    sptr<ScreenSession> screenSession = GetDefaultScreenSession();
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "fail to get default screenSession");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    isLocked = screenSession->IsScreenRotationLocked();
    TLOGI(WmsLogTag::DMS, "IsScreenRotationLocked:isLocked: %{public}u", isLocked);
    return DMError::DM_OK;
}

DMError ScreenSessionManager::SetScreenRotationLocked(bool isLocked)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set screen rotation locked permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    sptr<ScreenSession> screenSession = GetDefaultScreenSession();
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "fail to get default screenSession");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    screenSession->SetScreenRotationLocked(isLocked);
    TLOGI(WmsLogTag::DMS, "SetScreenRotationLocked: isLocked: %{public}u", isLocked);
    return DMError::DM_OK;
}

DMError ScreenSessionManager::SetScreenRotationLockedFromJs(bool isLocked)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set screen rotation locked from js permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    sptr<ScreenSession> screenSession = GetDefaultScreenSession();
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "fail to get default screenSession");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    screenSession->SetScreenRotationLockedFromJs(isLocked);
    TLOGI(WmsLogTag::DMS, "isLocked: %{public}u", isLocked);
    return DMError::DM_OK;
}

void ScreenSessionManager::UpdateScreenRotationProperty(ScreenId screenId, const RRect& bounds, float rotation,
    ScreenPropertyChangeType screenPropertyChangeType)
{
    DmsXcollie dmsXcollie("DMS:UpdateScreenRotationProperty", XCOLLIE_TIMEOUT_10S);
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "update screen rotation property permission denied!");
        return;
    }

    if (screenPropertyChangeType == ScreenPropertyChangeType::ROTATION_BEGIN) {
        // Rs is used to mark the start of the rotation animation
        TLOGI(WmsLogTag::DMS, "EnableCacheForRotation");
        RSInterfaces::GetInstance().EnableCacheForRotation();
    } else if (screenPropertyChangeType == ScreenPropertyChangeType::ROTATION_END) {
        // Rs is used to mark the end of the rotation animation
        TLOGI(WmsLogTag::DMS, "DisableCacheForRotation");
        RSInterfaces::GetInstance().DisableCacheForRotation();
        return;
    }

    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "fail to update screen rotation property, cannot find screen %{public}" PRIu64"",
            screenId);
        return;
    }
    screenSession->UpdatePropertyAfterRotation(bounds, rotation, GetFoldDisplayMode());
    sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "fail to update screen rotation property, displayInfo is nullptr");
        return;
    }
    NotifyDisplayChanged(displayInfo, DisplayChangeEvent::UPDATE_ROTATION);
    NotifyScreenChanged(screenSession->ConvertToScreenInfo(), ScreenChangeEvent::UPDATE_ROTATION);

    std::map<DisplayId, sptr<DisplayInfo>> emptyMap;
    NotifyDisplayStateChange(GetDefaultScreenId(), screenSession->ConvertToDisplayInfo(),
        emptyMap, DisplayStateChangeType::UPDATE_ROTATION);
    // 异步发送屏幕旋转公共事件
    auto task = [=]() {
        TLOGI(WmsLogTag::DMS, "publish dms rotation event");
        ScreenSessionPublish::GetInstance().PublishDisplayRotationEvent(
            displayInfo->GetScreenId(), displayInfo->GetRotation());
    };
    taskScheduler_->PostAsyncTask(task, "UpdateScreenRotationProperty");
}

void ScreenSessionManager::NotifyDisplayChanged(sptr<DisplayInfo> displayInfo, DisplayChangeEvent event)
{
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "NotifyDisplayChanged error, displayInfo is nullptr.");
        return;
    }
    auto task = [=] {
        if (event == DisplayChangeEvent::UPDATE_REFRESHRATE) {
            TLOGD(WmsLogTag::DMS, "evevt:%{public}d, displayId:%{public}" PRIu64"",
                event, displayInfo->GetDisplayId());
        } else {
            TLOGI(WmsLogTag::DMS, "evevt:%{public}d, displayId:%{public}" PRIu64"",
                event, displayInfo->GetDisplayId());
        }
        auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
        if (agents.empty()) {
            TLOGI(WmsLogTag::DMS, "NotifyDisplayChanged agents is empty");
            return;
        }
        std::lock_guard<std::mutex> lock(freezedPidListMutex_);
        for (auto& agent : agents) {
            int32_t agentPid = dmAgentContainer_.GetAgentPid(agent);
            if (freezedPidList_.count(agentPid) == 0) {
                agent->OnDisplayChange(displayInfo, event);
            } else {
                TLOGD(WmsLogTag::DMS, "Agent is freezed, no need notify. PID: %{public}d.", agentPid);
            }
        }
    };
    taskScheduler_->PostAsyncTask(task, "NotifyDisplayChanged");
}

DMError ScreenSessionManager::SetOrientation(ScreenId screenId, Orientation orientation)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set orientation permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if (orientation < Orientation::UNSPECIFIED || orientation > Orientation::REVERSE_HORIZONTAL) {
        TLOGE(WmsLogTag::DMS, "set orientation: %{public}u", static_cast<uint32_t>(orientation));
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:SetOrientation");
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "fail to set orientation, cannot find screen %{public}" PRIu64"", screenId);
        return DMError::DM_ERROR_NULLPTR;
    }
    // just for get orientation test
    screenSession->SetOrientation(orientation);
    screenSession->ScreenOrientationChange(orientation, GetFoldDisplayMode());
    return DMError::DM_OK;
}

bool ScreenSessionManager::SetRotation(ScreenId screenId, Rotation rotationAfter, bool isFromWindow)
{
    TLOGI(WmsLogTag::DMS,
        "Enter SetRotation, screenId: %{public}" PRIu64 ", rotation: %{public}u, isFromWindow: %{public}u,",
        screenId, rotationAfter, isFromWindow);
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "SetRotation error, cannot get screen with screenId: %{public}" PRIu64, screenId);
        return false;
    }
    if (rotationAfter == screenSession->GetRotation()) {
        TLOGE(WmsLogTag::DMS, "rotation not changed. screen %{public}" PRIu64" rotation %{public}u",
            screenId, rotationAfter);
        return false;
    }
    TLOGI(WmsLogTag::DMS, "set orientation. rotation %{public}u", rotationAfter);
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
        TLOGE(WmsLogTag::DMS, "autoRotation is not open");
        return;
    }
    ScreenSensorConnector::SubscribeRotationSensor();
    TLOGI(WmsLogTag::DMS, "subscribe rotation sensor successful");
}

void ScreenSessionManager::SetPostureAndHallSensorEnabled()
{
#ifdef SENSOR_ENABLE
    if (!g_foldScreenFlag) {
        TLOGI(WmsLogTag::DMS, "current device is not fold phone.");
        return;
    }
    FoldScreenSensorManager::GetInstance().RegisterPostureCallback();
    FoldScreenSensorManager::GetInstance().RegisterHallCallback();
    TLOGI(WmsLogTag::DMS, "subscribe Posture and Hall sensor successful");
#endif
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
        TLOGE(WmsLogTag::DMS, "can not get display.");
        return nullptr;
    }
    auto screenInfo = GetScreenInfoById(displayInfo->GetScreenId());
    if (screenInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "can not get screen.");
        return nullptr;
    }
    auto modes = screenInfo->GetModes();
    auto id = screenInfo->GetModeId();
    if (id >= modes.size()) {
        TLOGE(WmsLogTag::DMS, "can not get screenMode.");
        return nullptr;
    }
    return modes[id];
}

sptr<ScreenInfo> ScreenSessionManager::GetScreenInfoByDisplayId(DisplayId displayId)
{
    auto displayInfo = GetDisplayInfoById(displayId);
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "can not get displayInfo.");
        return nullptr;
    }
    return GetScreenInfoById(displayInfo->GetScreenId());
}

bool ScreenSessionManager::NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status,
    PowerStateChangeReason reason)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER);
    if (agents.empty()) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]NotifyDisplayPowerEvent agents is empty");
        return false;
    }
    TLOGD(WmsLogTag::DMS, "[UL_POWER]NotifyDisplayPowerEvent");
    for (auto& agent : agents) {
        agent->NotifyDisplayPowerEvent(event, status);
    }

    {
        std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
        if (screenSessionMap_.empty()) {
            TLOGE(WmsLogTag::DMS, "[UL_POWER]screenSessionMap is empty");
            return false;
        }
        // The on/off screen will send a notification based on the number of screens.
        // The dual display device just notify the current screen usage
        if (FoldScreenStateInternel::IsDualDisplayFoldDevice()) {
            ScreenId currentScreenId = foldScreenController_->GetCurrentScreenId();
            auto iter = screenSessionMap_.find(currentScreenId);
            if (iter != screenSessionMap_.end() && iter->second != nullptr) {
                iter->second->PowerStatusChange(event, status, reason);
            }
            return true;
        }
    }

    auto screenIds = GetAllScreenIds();
    if (screenIds.empty()) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]no screenID");
        return false;
    }
    for (auto screenId : screenIds) {
        sptr<ScreenSession> screenSession = GetScreenSession(screenId);
        if (screenSession == nullptr) {
            TLOGW(WmsLogTag::DMS, "[UL_POWER]Cannot get ScreenSession, screenId: %{public}" PRIu64"", screenId);
            continue;
        }
        screenSession->PowerStatusChange(event, status, reason);
    }
    return true;
}

bool ScreenSessionManager::NotifyDisplayStateChanged(DisplayId id, DisplayState state)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_STATE_LISTENER);
    if (agents.empty()) {
        TLOGI(WmsLogTag::DMS, "NotifyDisplayStateChanged agents is empty");
        return false;
    }
    TLOGI(WmsLogTag::DMS, "NotifyDisplayStateChanged");
    for (auto& agent : agents) {
        agent->NotifyDisplayStateChanged(id, state);
    }
    return true;
}
DMError ScreenSessionManager::GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "GetAllScreenInfos get all screen infos permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    std::vector<ScreenId> screenIds = GetAllScreenIds();
    for (auto screenId : screenIds) {
        auto screenInfo = GetScreenInfoById(screenId);
        if (screenInfo == nullptr) {
            TLOGE(WmsLogTag::DMS, "GetAllScreenInfos cannot find screenInfo: %{public}" PRIu64"", screenId);
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
    TLOGI(WmsLogTag::DMS, "GetScreenSupportedColorGamuts ENTER");
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "GetScreenSupportedColorGamuts permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    sptr<ScreenSession> screen = GetScreenSession(screenId);
    if (screen == nullptr) {
        TLOGE(WmsLogTag::DMS, "GetScreenSupportedColorGamuts nullptr");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screen->GetScreenSupportedColorGamuts(colorGamuts);
}

DMError ScreenSessionManager::GetPixelFormat(ScreenId screenId, GraphicPixelFormat& pixelFormat)
{
    TLOGI(WmsLogTag::DMS, "GetPixelFormat::ScreenId: %{public}" PRIu64, screenId);
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "GetPixelFormat screenId invalid");
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
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set pixel format  permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }

    TLOGI(WmsLogTag::DMS, "SetPixelFormat::ScreenId: %{public}" PRIu64 ", pixelFormat %{public}d",
        screenId, pixelFormat);
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "SetPixelFormat screenId invalid");
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
    TLOGI(WmsLogTag::DMS, "GetSupportedHDRFormats %{public}" PRIu64, screenId);
    sptr<ScreenSession> screen = GetScreenSession(screenId);
    if (screen == nullptr) {
        TLOGE(WmsLogTag::DMS, "GetSupportedHDRFormats nullptr");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screen->GetSupportedHDRFormats(hdrFormats);
}

DMError ScreenSessionManager::GetScreenHDRFormat(ScreenId screenId, ScreenHDRFormat& hdrFormat)
{
    TLOGI(WmsLogTag::DMS, "GetScreenHDRFormat::ScreenId: %{public}" PRIu64, screenId);
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "screenId invalid");
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
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set screen HDR format permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }

    TLOGI(WmsLogTag::DMS, "SetScreenHDRFormat::ScreenId: %{public}" PRIu64 ", modeIdx %{public}d", screenId, modeIdx);
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "SetScreenHDRFormat screenId invalid");
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
    TLOGI(WmsLogTag::DMS, "GetSupportedColorSpaces %{public}" PRIu64, screenId);
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "GetSupportedColorSpaces nullptr");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->GetSupportedColorSpaces(colorSpaces);
}

DMError ScreenSessionManager::GetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType& colorSpace)
{
    TLOGI(WmsLogTag::DMS, "GetScreenColorSpace::ScreenId: %{public}" PRIu64, screenId);
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "screenId invalid");
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
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "set screen color space permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }

    TLOGI(WmsLogTag::DMS, "SetScreenColorSpace::ScreenId: %{public}" PRIu64 ", colorSpace %{public}d",
        screenId, colorSpace);
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "SetScreenColorSpace screenId invalid");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    return screenSession->SetScreenColorSpace(colorSpace);
}

void ScreenSessionManager::AddVirtualScreenDeathRecipient(const sptr<IRemoteObject>& displayManagerAgent,
    ScreenId smsScreenId)
{
    if (deathRecipient_ == nullptr) {
        TLOGI(WmsLogTag::DMS, "CreateVirtualScreen Create deathRecipient");
        deathRecipient_ =
            new(std::nothrow) AgentDeathRecipient([this](const sptr<IRemoteObject>& agent) { OnRemoteDied(agent); });
    }
    if (deathRecipient_ != nullptr) {
        auto agIter = screenAgentMap_.find(displayManagerAgent);
        if (agIter == screenAgentMap_.end()) {
            displayManagerAgent->AddDeathRecipient(deathRecipient_);
        }
    }
    screenAgentMap_[displayManagerAgent].emplace_back(smsScreenId);
}

ScreenId ScreenSessionManager::CreateVirtualScreen(VirtualScreenOption option,
                                                   const sptr<IRemoteObject>& displayManagerAgent)
{
    if (!(Permission::IsSystemCalling() && Permission::CheckCallingPermission(SCREEN_CAPTURE_PERMISSION)) &&
        !SessionPermission::IsShellCall()) {
        TLOGE(WmsLogTag::DMS, "create virtual screen permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return SCREEN_ID_INVALID;
    }
    ExitCoordination("CreateVirtualScreen");
    TLOGI(WmsLogTag::DMS, "ENTER");
    if (SessionPermission::IsBetaVersion()) {
        CheckAndSendHiSysEvent("CREATE_VIRTUAL_SCREEN", "hmos.screenrecorder");
    }
    if (clientProxy_ && option.missionIds_.size() > 0) {
        std::vector<uint64_t> surfaceNodeIds;
        clientProxy_->OnGetSurfaceNodeIdsFromMissionIdsChanged(option.missionIds_, surfaceNodeIds);
        option.missionIds_ = surfaceNodeIds;
    }
    ScreenId rsId = rsInterface_.CreateVirtualScreen(option.name_, option.width_,
        option.height_, option.surface_, SCREEN_ID_INVALID, option.flags_);
    if (rsId == SCREEN_ID_INVALID) {
        TLOGI(WmsLogTag::DMS, "rsId is invalid");
        return SCREEN_ID_INVALID;
    }
    TLOGI(WmsLogTag::DMS, "rsId: %{public}" PRIu64"", rsId);
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:CreateVirtualScreen(%s)", option.name_.c_str());
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    ScreenId smsScreenId = SCREEN_ID_INVALID;
    if (!screenIdManager_.ConvertToSmsScreenId(rsId, smsScreenId)) {
        TLOGI(WmsLogTag::DMS, "!ConvertToSmsScreenId(rsId, smsScreenId)");
        smsScreenId = screenIdManager_.CreateAndGetNewScreenId(rsId);
        auto screenSession = InitVirtualScreen(smsScreenId, rsId, option);
        if (screenSession == nullptr) {
            TLOGI(WmsLogTag::DMS, "screenSession is nullptr");
            screenIdManager_.DeleteScreenId(smsScreenId);
            return SCREEN_ID_INVALID;
        }
        screenSession->SetName(option.name_);
        screenSession->SetMirrorScreenType(MirrorScreenType::VIRTUAL_MIRROR);
        screenSessionMap_.insert(std::make_pair(smsScreenId, screenSession));
        if (option.name_ == "CastEngine") {
            screenSession->SetVirtualScreenFlag(VirtualScreenFlag::CAST);
        }
        NotifyScreenConnected(screenSession->ConvertToScreenInfo());
        TLOGI(WmsLogTag::DMS, "create success. ScreenId: %{public}" PRIu64", rsId: %{public}" PRIu64"",
            smsScreenId, rsId);
        if (displayManagerAgent == nullptr) {
            virtualScreenCount_ = virtualScreenCount_ + 1;
            NotifyCaptureStatusChanged();
            return smsScreenId;
        }
        AddVirtualScreenDeathRecipient(displayManagerAgent, smsScreenId);
    }
    virtualScreenCount_ = virtualScreenCount_ + 1;
    NotifyCaptureStatusChanged();
    return smsScreenId;
}

DMError ScreenSessionManager::SetVirtualScreenSurface(ScreenId screenId, sptr<IBufferProducer> surface)
{
    if (!(Permission::IsSystemCalling() && Permission::CheckCallingPermission(SCREEN_CAPTURE_PERMISSION)) &&
        !SessionPermission::IsShellCall()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if (surface == nullptr) {
        TLOGE(WmsLogTag::DMS, "surface is null");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "No such screen.");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    TLOGI(WmsLogTag::DMS, "enter set virtual screen surface");
    ScreenId rsScreenId;
    int32_t res = -1;
    if (screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId)) {
        sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(surface);
        if (pSurface != nullptr) {
            res = rsInterface_.SetVirtualScreenSurface(rsScreenId, pSurface);
        }
    }
    if (res != 0) {
        TLOGE(WmsLogTag::DMS, "fail to set virtual screen surface in RenderService");
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    return DMError::DM_OK;
}

DMError ScreenSessionManager::SetVirtualMirrorScreenScaleMode(ScreenId screenId, ScreenScaleMode scaleMode)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "No such screen.");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    ScreenId rsScreenId;
    if (!screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId)) {
        TLOGE(WmsLogTag::DMS, "No corresponding rsId");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    bool res = rsInterface_.SetVirtualMirrorScreenScaleMode(rsScreenId, scaleMode);
    if (!res) {
        TLOGE(WmsLogTag::DMS, "failed in RenderService");
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    return DMError::DM_OK;
}

DMError ScreenSessionManager::SetVirtualMirrorScreenCanvasRotation(ScreenId screenId, bool autoRotate)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "No such screen.");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    TLOGI(WmsLogTag::DMS, "enter set virtual mirror screen canvas rotation");
    bool res = false;
    ScreenId rsScreenId;
    if (!screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId)) {
        TLOGE(WmsLogTag::DMS, "No corresponding rsId");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    res = rsInterface_.SetVirtualMirrorScreenCanvasRotation(rsScreenId, autoRotate);
    if (!res) {
        TLOGE(WmsLogTag::DMS, "failed in RenderService");
        return DMError::DM_ERROR_RENDER_SERVICE_FAILED;
    }
    TLOGI(WmsLogTag::DMS, "set virtual mirror screen canvas rotation success");
    return DMError::DM_OK;
}

DMError ScreenSessionManager::ResizeVirtualScreen(ScreenId screenId, uint32_t width, uint32_t height)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    TLOGI(WmsLogTag::DMS, "screenId: %{public}" PRIu64", width: %{public}u, height: %{public}u.",
        screenId, width, height);
    sptr<ScreenSession> screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "No such screen.");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    ScreenId rsScreenId;
    if (!screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId)) {
        TLOGE(WmsLogTag::DMS, "No corresponding rsId");
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
        TLOGE(WmsLogTag::DMS, "destroy virtual screen permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }

    // virtual screen destroy callback to notify scb
    TLOGI(WmsLogTag::DMS, "destroy virtual screen");
    OnVirtualScreenChange(screenId, ScreenEvent::DISCONNECTED);
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    ScreenId rsScreenId = SCREEN_ID_INVALID;
    screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId);

    for (auto &agentIter : screenAgentMap_) {
        auto iter = std::find(agentIter.second.begin(), agentIter.second.end(), screenId);
        if (iter != agentIter.second.end()) {
            iter = agentIter.second.erase(iter);
            if (agentIter.first != nullptr && agentIter.second.empty()) {
                screenAgentMap_.erase(agentIter.first);
            }
            break;
        }
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:DestroyVirtualScreen(%" PRIu64")", screenId);
    if (rsScreenId != SCREEN_ID_INVALID && GetScreenSession(screenId) != nullptr) {
        auto screen = GetScreenSession(screenId);
        if (CheckScreenInScreenGroup(screen)) {
            NotifyDisplayDestroy(screenId);
        }
        auto smsScreenMapIter = screenSessionMap_.find(screenId);
        if (smsScreenMapIter != screenSessionMap_.end()) {
            auto screenGroup = RemoveFromGroupLocked(smsScreenMapIter->second);
            if (screenGroup != nullptr) {
                NotifyScreenGroupChanged(
                    smsScreenMapIter->second->ConvertToScreenInfo(), ScreenGroupChangeEvent::REMOVE_FROM_GROUP);
            }
            screenSessionMap_.erase(smsScreenMapIter);
            NotifyScreenDisconnected(screenId);
            TLOGI(WmsLogTag::DMS, "DestroyVirtualScreen success, id: %{public}" PRIu64"", screenId);
        }
    }
    screenIdManager_.DeleteScreenId(screenId);
    virtualScreenCount_ = virtualScreenCount_ > 0 ? virtualScreenCount_ - 1 : 0;
    NotifyCaptureStatusChanged();
    if (rsScreenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "DestroyVirtualScreen: No corresponding rsScreenId");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    rsInterface_.RemoveVirtualScreen(rsScreenId);
    return DMError::DM_OK;
}

DMError ScreenSessionManager::DisableMirror(bool disableOrNot)
{
    TLOGI(WmsLogTag::DMS, "DisableMirror %{public}d", disableOrNot);
    if (!SessionPermission::IsSystemCalling()) {
        TLOGI(WmsLogTag::DMS, "DisableMirror permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    TLOGI(WmsLogTag::DMS, "DisableMirror enter %{public}d", disableOrNot);
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

void ScreenSessionManager::MirrorSwitchNotify(ScreenId screenId)
{
    auto mirrorScreen = GetScreenSession(screenId);
    if (mirrorScreen != nullptr) {
        mirrorScreen->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
        NotifyScreenChanged(mirrorScreen->ConvertToScreenInfo(), ScreenChangeEvent::SCREEN_SWITCH_CHANGE);
    }
}

DMError ScreenSessionManager::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
                                         ScreenId& screenGroupId)
{
    TLOGI(WmsLogTag::DMS, "enter!");
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if (system::GetBoolParameter("persist.edm.disallow_mirror", false)) {
        TLOGW(WmsLogTag::DMS, "disabled by edm!");
        return DMError::DM_ERROR_INVALID_PERMISSION;
    }

    TLOGI(WmsLogTag::DMS, "mainScreenId :%{public}" PRIu64"", mainScreenId);
    auto allMirrorScreenIds = GetAllValidScreenIds(mirrorScreenIds);
    auto iter = std::find(allMirrorScreenIds.begin(), allMirrorScreenIds.end(), mainScreenId);
    if (iter != allMirrorScreenIds.end()) {
        allMirrorScreenIds.erase(iter);
    }
    if (allMirrorScreenIds.empty()) {
        TLOGI(WmsLogTag::DMS, "make mirror screen id empty");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:MakeMirror start");
    for (ScreenId screenId : allMirrorScreenIds) {
        OnVirtualScreenChange(screenId, ScreenEvent::DISCONNECTED);
    }
    DMError makeResult = MultiScreenManager::GetInstance().MirrorSwitch(mainScreenId,
        allMirrorScreenIds, screenGroupId);
    for (ScreenId screenId : allMirrorScreenIds) {
        MirrorSwitchNotify(screenId);
    }
    RegisterCastObserver(allMirrorScreenIds);
    TLOGI(WmsLogTag::DMS, "make mirror notify scb end makeResult=%{public}d", makeResult);
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "dms:MakeMirror end");
    return makeResult;
}

void ScreenSessionManager::RegisterCastObserver(std::vector<ScreenId>& mirrorScreenIds)
{
    mirrorScreenIds_ = mirrorScreenIds;
    TLOGI(WmsLogTag::DMS, "Register Setting cast Observer");
    SettingObserver::UpdateFunc updateFunc = [&](const std::string& key) { SetCastFromSettingData(); };
    ScreenSettingHelper::RegisterSettingCastObserver(updateFunc);
}

void ScreenSessionManager::SetCastFromSettingData()
{
    bool enable;
    bool ret = ScreenSettingHelper::GetSettingCast(enable);
    if (!ret) {
        TLOGW(WmsLogTag::DMS, "get setting cast failed, default enable");
        enable = true;
    } else {
        TLOGI(WmsLogTag::DMS, "get setting cast success, enable: %{public}u", enable);
    }
    for (ScreenId screenId : mirrorScreenIds_) {
        ScreenId rsScreenId;
        if (!screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId)) {
            TLOGE(WmsLogTag::DMS, "No corresponding rsId");
            continue;
        }
        rsInterface_.SetCastScreenEnableSkipWindow(rsScreenId, enable);
    }
}

void ScreenSessionManager::RegisterSettingRotationObserver()
{
    TLOGI(WmsLogTag::DMS, "Register setting rotation observer");
    SettingObserver::UpdateFunc updateFunc = [&](const std::string& key) {
        int32_t rotation = -1;
        int32_t screenId = -1;
        if (ScreenSettingHelper::GetSettingRotation(rotation) &&
            ScreenSettingHelper::GetSettingRotationScreenID(screenId)) {
            TLOGI(WmsLogTag::DMS, "current dms setting rotation:%{public}d, screenId:%{public}d",
                rotation, screenId);
        } else {
            TLOGI(WmsLogTag::DMS, "get current dms setting rotation and screenId failed");
        }
    };
    ScreenSettingHelper::RegisterSettingRotationObserver(updateFunc);
}

DMError ScreenSessionManager::StopMirror(const std::vector<ScreenId>& mirrorScreenIds)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "StopMirror permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    auto allMirrorScreenIds = GetAllValidScreenIds(mirrorScreenIds);
    if (allMirrorScreenIds.empty()) {
        TLOGI(WmsLogTag::DMS, "StopMirror done. screens' size:%{public}u",
            static_cast<uint32_t>(allMirrorScreenIds.size()));
        return DMError::DM_OK;
    }

    DMError ret = StopScreens(allMirrorScreenIds, ScreenCombination::SCREEN_MIRROR);
    if (ret != DMError::DM_OK) {
        TLOGE(WmsLogTag::DMS, "StopMirror failed.");
        return ret;
    }
    ScreenSettingHelper::UnregisterSettingCastObserver();

    return DMError::DM_OK;
}

DMError ScreenSessionManager::StopScreens(const std::vector<ScreenId>& screenIds, ScreenCombination stopCombination)
{
    for (ScreenId screenId : screenIds) {
        TLOGI(WmsLogTag::DMS, "StopScreens ScreenId: %{public}" PRIu64"", screenId);
        auto screen = GetScreenSession(screenId);
        if (screen == nullptr) {
            TLOGW(WmsLogTag::DMS, "StopScreens screen:%{public}" PRIu64" is nullptr", screenId);
            continue;
        }
        sptr<ScreenSessionGroup> screenGroup = GetAbstractScreenGroup(screen->groupSmsId_);
        if (!screenGroup) {
            TLOGW(WmsLogTag::DMS, "StopScreens groupDmsId:%{public}" PRIu64"is not in smsScreenGroupMap_",
                screen->groupSmsId_);
            continue;
        }
        if (screenGroup->combination_ != stopCombination) {
            TLOGW(WmsLogTag::DMS, "StopScreens try to stop screen in another combination");
            continue;
        }
        if (screenGroup->combination_ == ScreenCombination::SCREEN_MIRROR &&
            screen->screenId_ == screenGroup->mirrorScreenId_) {
            TLOGW(WmsLogTag::DMS, "StopScreens try to stop main mirror screen");
            continue;
        }
        bool res = RemoveChildFromGroup(screen, screenGroup);
        if (res) {
            NotifyScreenGroupChanged(screen->ConvertToScreenInfo(), ScreenGroupChangeEvent::REMOVE_FROM_GROUP);
        }
    }
    return DMError::DM_OK;
}

VirtualScreenFlag ScreenSessionManager::GetVirtualScreenFlag(ScreenId screenId)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return VirtualScreenFlag::DEFAULT;
    }
    auto screen = GetScreenSession(screenId);
    if (screen == nullptr) {
        TLOGE(WmsLogTag::DMS, "get virtual screen flag screen session null");
        return VirtualScreenFlag::DEFAULT;
    }
    return screen->GetVirtualScreenFlag();
}

DMError ScreenSessionManager::SetVirtualScreenFlag(ScreenId screenId, VirtualScreenFlag screenFlag)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if (screenFlag < VirtualScreenFlag::DEFAULT || screenFlag >= VirtualScreenFlag::MAX) {
        TLOGE(WmsLogTag::DMS, "set virtual screen flag range error");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    auto screen = GetScreenSession(screenId);
    if (screen == nullptr) {
        TLOGE(WmsLogTag::DMS, "set virtual screen flag screen session null");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    screen->SetVirtualScreenFlag(screenFlag);
    return DMError::DM_OK;
}

DMError ScreenSessionManager::SetVirtualScreenRefreshRate(ScreenId screenId, uint32_t refreshInterval)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "SetVirtualScreenRefreshRate, permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    TLOGI(WmsLogTag::DMS, "SetVirtualScreenRefreshRate, screenId: %{public}" PRIu64", refreshInterval:  %{public}u",
        screenId, refreshInterval);
    if (screenId == GetDefaultScreenId()) {
        TLOGE(WmsLogTag::DMS,
        "cannot set refresh rate of main screen, main screen id: %{public}" PRIu64".", GetDefaultScreenId());
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    if (refreshInterval == 0) {
        TLOGE(WmsLogTag::DMS, "SetVirtualScreenRefreshRate, refresh interval is 0.");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    auto screenSession = GetScreenSession(screenId);
    auto defaultScreenSession = GetDefaultScreenSession();
    if (screenSession == nullptr || defaultScreenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "SetVirtualScreenRefreshRate, screenSession is null.");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    ScreenId rsScreenId;
    if (!screenIdManager_.ConvertToRsScreenId(screenId, rsScreenId)) {
        TLOGE(WmsLogTag::DMS, "SetVirtualScreenRefreshRate, No corresponding rsId.");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    int32_t res = rsInterface_.SetScreenSkipFrameInterval(rsScreenId, refreshInterval);
    if (res != StatusCode::SUCCESS) {
        TLOGE(WmsLogTag::DMS, "SetVirtualScreenRefreshRate, rsInterface error: %{public}d", res);
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    screenSession->UpdateRefreshRate(defaultScreenSession->GetRefreshRate() / refreshInterval);
    return DMError::DM_OK;
}

DMError ScreenSessionManager::VirtualScreenUniqueSwitch(const std::vector<ScreenId>& screenIds)
{
    TLOGI(WmsLogTag::DMS, "enter");
    auto defaultScreen = GetDefaultScreenSession();
    if (!defaultScreen) {
        TLOGE(WmsLogTag::DMS, "default screen is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    defaultScreen->groupSmsId_ = 1;
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    auto iter = smsScreenGroupMap_.find(defaultScreen->groupSmsId_);
    if (iter != smsScreenGroupMap_.end()) {
        smsScreenGroupMap_.erase(iter);
    }
    DMError uniqueSwitchRet = MultiScreenManager::GetInstance().VirtualScreenUniqueSwitch(defaultScreen, screenIds);
    TLOGI(WmsLogTag::DMS, "virtual screen unique switch result: %{public}d", uniqueSwitchRet);
    return uniqueSwitchRet;
}

DMError ScreenSessionManager::MakeUniqueScreen(const std::vector<ScreenId>& screenIds)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    TLOGI(WmsLogTag::DMS, "enter!");
    if (screenIds.empty()) {
        TLOGE(WmsLogTag::DMS, "screen is empty");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    ScreenId uniqueScreenId = screenIds[0];
    auto uniqueScreen = GetScreenSession(uniqueScreenId);
    if (uniqueScreen != nullptr && uniqueScreen->GetVirtualScreenFlag() == VirtualScreenFlag::CAST) {
        return MultiScreenManager::GetInstance().UniqueSwitch(screenIds);
    }
    for (auto screenId : screenIds) {
        ScreenId rsScreenId = SCREEN_ID_INVALID;
        bool res = ConvertScreenIdToRsScreenId(screenId, rsScreenId);
        TLOGI(WmsLogTag::DMS, "unique screenId: %{public}" PRIu64" rsScreenId: %{public}" PRIu64"",
            screenId, rsScreenId);
        if (!res) {
            TLOGE(WmsLogTag::DMS, "convert screenId to rsScreenId failed");
            continue;
        }
        auto screenSession = GetScreenSession(screenId);
        if (!screenSession) {
            TLOGE(WmsLogTag::DMS, "screen session is nullptr");
            continue;
        }
        screenSession->SetDisplayNodeScreenId(rsScreenId);
        // notify scb to build Screen widget
        OnVirtualScreenChange(screenId, ScreenEvent::CONNECTED);
    }
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        TLOGD(WmsLogTag::DMS, "flush data");
        transactionProxy->FlushImplicitTransaction();
    }
    TLOGI(WmsLogTag::DMS, "end!");
    return DMError::DM_OK;
}

DMError ScreenSessionManager::MakeExpand(std::vector<ScreenId> screenId,
                                         std::vector<Point> startPoint,
                                         ScreenId& screenGroupId)
{
    TLOGI(WmsLogTag::DMS, "MakeExpand enter!");
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "MakeExpand permission denied! pid: %{public}d", IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    if (screenId.empty() || startPoint.empty() || screenId.size() != startPoint.size()) {
        TLOGE(WmsLogTag::DMS, "create expand fail, screenId size:%{public}ud,startPoint size:%{public}ud",
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
        TLOGE(WmsLogTag::DMS, "allExpandScreenIds is empty. make expand failed.");
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
    TLOGI(WmsLogTag::DMS, "OnMakeExpand, defaultScreenId:%{public}" PRIu64"", defaultScreenId);
    auto defaultScreen = GetScreenSession(defaultScreenId);
    if (defaultScreen == nullptr) {
        TLOGI(WmsLogTag::DMS, "OnMakeExpand failed.");
        return false;
    }
    auto group = GetAbstractScreenGroup(defaultScreen->groupSmsId_);
    if (group == nullptr) {
        group = AddToGroupLocked(defaultScreen);
        if (group == nullptr) {
            TLOGE(WmsLogTag::DMS, "group is nullptr");
            return false;
        }
        NotifyScreenGroupChanged(defaultScreen->ConvertToScreenInfo(), ScreenGroupChangeEvent::ADD_TO_GROUP);
    }
    bool filterExpandScreen = group->combination_ == ScreenCombination::SCREEN_EXPAND;
    ChangeScreenGroup(group, screenId, startPoint, filterExpandScreen, ScreenCombination::SCREEN_EXPAND);
    TLOGI(WmsLogTag::DMS, "OnMakeExpand success");
    return true;
}

DMError ScreenSessionManager::StopExpand(const std::vector<ScreenId>& expandScreenIds)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "StopExpand permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    auto allExpandScreenIds = GetAllValidScreenIds(expandScreenIds);
    if (allExpandScreenIds.empty()) {
        TLOGI(WmsLogTag::DMS, "StopExpand done. screens' size:%{public}u",
            static_cast<uint32_t>(allExpandScreenIds.size()));
        return DMError::DM_OK;
    }

    DMError ret = StopScreens(allExpandScreenIds, ScreenCombination::SCREEN_EXPAND);
    if (ret != DMError::DM_OK) {
        TLOGE(WmsLogTag::DMS, "StopExpand stop expand failed.");
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
    TLOGI(WmsLogTag::DMS, "CreateAndGetNewScreenId screenId: %{public}" PRIu64"", smsScreenId);
    if (sms2RsScreenIdMap_.find(smsScreenId) != sms2RsScreenIdMap_.end()) {
        TLOGW(WmsLogTag::DMS, "CreateAndGetNewScreenId screenId: %{public}" PRIu64" exit", smsScreenId);
    }
    sms2RsScreenIdMap_[smsScreenId] = rsScreenId;
    if (rsScreenId == SCREEN_ID_INVALID) {
        return smsScreenId;
    }
    if (rs2SmsScreenIdMap_.find(rsScreenId) != rs2SmsScreenIdMap_.end()) {
        TLOGW(WmsLogTag::DMS, "CreateAndGetNewScreenId rsScreenId: %{public}" PRIu64" exit", rsScreenId);
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
    TLOGI(WmsLogTag::DMS, "InitVirtualScreen: Enter");
    ScreenSessionConfig config = {
        .screenId = smsScreenId,
        .rsId = rsId,
        .defaultScreenId = GetDefaultScreenId(),
        .name = option.name_,
    };
    sptr<ScreenSession> screenSession =
        new(std::nothrow) ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    sptr<SupportedScreenModes> info = new(std::nothrow) SupportedScreenModes();
    if (screenSession == nullptr || info == nullptr) {
        TLOGI(WmsLogTag::DMS, "InitVirtualScreen: new screenSession or info failed");
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
    TLOGI(WmsLogTag::DMS, "Call rsInterface_ GetScreenSupportedModes");
    std::vector<RSScreenModeInfo> allModes = rsInterface_.GetScreenSupportedModes(
        screenIdManager_.ConvertToRsScreenId(screenSession->screenId_));
    if (allModes.size() == 0) {
        TLOGE(WmsLogTag::DMS, "allModes.size() == 0, screenId=%{public}" PRIu64"", screenSession->rsId_);
        return false;
    }
    for (const RSScreenModeInfo& rsScreenModeInfo : allModes) {
        sptr<SupportedScreenModes> info = new(std::nothrow) SupportedScreenModes();
        if (info == nullptr) {
            TLOGE(WmsLogTag::DMS, "create SupportedScreenModes failed");
            return false;
        }
        info->id_ = static_cast<uint32_t>(rsScreenModeInfo.GetScreenModeId());
        info->width_ = static_cast<uint32_t>(rsScreenModeInfo.GetScreenWidth());
        info->height_ = static_cast<uint32_t>(rsScreenModeInfo.GetScreenHeight());
        info->refreshRate_ = rsScreenModeInfo.GetScreenRefreshRate();
        screenSession->modes_.push_back(info);
        TLOGI(WmsLogTag::DMS, "fill screen idx:%{public}d w/h:%{public}d/%{public}d",
            rsScreenModeInfo.GetScreenModeId(), info->width_, info->height_);
    }
    TLOGI(WmsLogTag::DMS, "Call rsInterface_ GetScreenActiveMode");
    int32_t activeModeId = rsInterface_.GetScreenActiveMode(screenSession->rsId_).GetScreenModeId();
    TLOGI(WmsLogTag::DMS, "fill screen activeModeId:%{public}d", activeModeId);
    if (static_cast<std::size_t>(activeModeId) >= allModes.size()) {
        TLOGE(WmsLogTag::DMS, "activeModeId exceed, screenId=%{public}" PRIu64", activeModeId:%{public}d/%{public}ud",
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
    TLOGI(WmsLogTag::DMS, "Screen name is %{public}s, phyWidth is %{public}u, phyHeight is %{public}u",
        screenCapability.GetName().c_str(), screenCapability.GetPhyWidth(), screenCapability.GetPhyHeight());
    ScreenSessionConfig config = {
        .screenId = smsScreenId,
        .rsId = rsScreenId,
        .defaultScreenId = GetDefaultScreenId(),
        .name = screenCapability.GetName(),
    };
    sptr<ScreenSession> screenSession =
        new(std::nothrow) ScreenSession(config, ScreenSessionReason::CREATE_SESSION_FOR_VIRTUAL);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "InitAndGetScreen: screenSession == nullptr.");
        screenIdManager_.DeleteScreenId(smsScreenId);
        return nullptr;
    }
    if (!InitAbstractScreenModesInfo(screenSession)) {
        screenIdManager_.DeleteScreenId(smsScreenId);
        TLOGE(WmsLogTag::DMS, "InitAndGetScreen: InitAndGetScreen failed.");
        return nullptr;
    }
    TLOGI(WmsLogTag::DMS, "InitAndGetScreen: screenSessionMap_ add screenId=%{public}" PRIu64"", smsScreenId);
    screenSessionMap_.insert(std::make_pair(smsScreenId, screenSession));
    return screenSession;
}

sptr<ScreenSessionGroup> ScreenSessionManager::AddToGroupLocked(sptr<ScreenSession> newScreen, bool isUnique)
{
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    sptr<ScreenSessionGroup> res;
    if (smsScreenGroupMap_.empty()) {
        TLOGI(WmsLogTag::DMS, "connect the first screen");
        res = AddAsFirstScreenLocked(newScreen, isUnique);
    } else {
        res = AddAsSuccedentScreenLocked(newScreen);
    }
    return res;
}

sptr<ScreenSessionGroup> ScreenSessionManager::AddAsFirstScreenLocked(sptr<ScreenSession> newScreen, bool isUnique)
{
    ScreenId smsGroupScreenId(1);
    std::ostringstream buffer;
    buffer << "ScreenGroup_" << smsGroupScreenId;
    std::string name = buffer.str();
    // default ScreenCombination is mirror
    isExpandCombination_ = system::GetParameter("persist.display.expand.enabled", "0") == "1";
    sptr<ScreenSessionGroup> screenGroup;
    if (isExpandCombination_) {
        screenGroup = new(std::nothrow) ScreenSessionGroup(smsGroupScreenId,
            SCREEN_ID_INVALID, name, ScreenCombination::SCREEN_EXPAND);
        newScreen->SetScreenCombination(ScreenCombination::SCREEN_EXPAND);
    } else if (isUnique) {
        screenGroup = new(std::nothrow) ScreenSessionGroup(smsGroupScreenId,
            SCREEN_ID_INVALID, name, ScreenCombination::SCREEN_UNIQUE);
        newScreen->SetScreenCombination(ScreenCombination::SCREEN_UNIQUE);
    } else {
        screenGroup = new(std::nothrow) ScreenSessionGroup(smsGroupScreenId,
            SCREEN_ID_INVALID, name, ScreenCombination::SCREEN_MIRROR);
        newScreen->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    }
    if (screenGroup == nullptr) {
        TLOGE(WmsLogTag::DMS, "new ScreenSessionGroup failed");
        screenIdManager_.DeleteScreenId(smsGroupScreenId);
        return nullptr;
    }
    screenGroup->groupSmsId_ = 1;
    Point point;
    if (!screenGroup->AddChild(newScreen, point, GetScreenSession(GetDefaultScreenId()))) {
        TLOGE(WmsLogTag::DMS, "fail to add screen to group. screen=%{public}" PRIu64"", newScreen->screenId_);
        screenIdManager_.DeleteScreenId(smsGroupScreenId);
        return nullptr;
    }
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    auto iter = smsScreenGroupMap_.find(smsGroupScreenId);
    if (iter != smsScreenGroupMap_.end()) {
        TLOGE(WmsLogTag::DMS, "group screen existed. id=%{public}" PRIu64"", smsGroupScreenId);
        smsScreenGroupMap_.erase(iter);
    }
    smsScreenGroupMap_.insert(std::make_pair(smsGroupScreenId, screenGroup));
    screenGroup->mirrorScreenId_ = newScreen->screenId_;
    TLOGI(WmsLogTag::DMS, "connect new group screen, screenId: %{public}" PRIu64", screenGroupId: %{public}" PRIu64", "
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
        TLOGE(WmsLogTag::DMS,
            "AddAsSuccedentScreenLocked. defaultScreenId:%{public}" PRIu64" is not in screenSessionMap_.",
            defaultScreenId);
        return nullptr;
    }
    auto screen = iter->second;
    auto screenGroupIter = smsScreenGroupMap_.find(screen->groupSmsId_);
    if (screenGroupIter == smsScreenGroupMap_.end()) {
        TLOGE(WmsLogTag::DMS, "AddAsSuccedentScreenLocked. groupSmsId:%{public}" PRIu64" is not in smsScreenGroupMap_.",
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
    TLOGI(WmsLogTag::DMS, "RemoveFromGroupLocked.");
    auto groupSmsId = screen->groupSmsId_;
    sptr<ScreenSessionGroup> screenGroup = GetAbstractScreenGroup(groupSmsId);
    if (!screenGroup) {
        TLOGE(WmsLogTag::DMS, "RemoveFromGroupLocked. groupSmsId:%{public}" PRIu64"is not in smsScreenGroupMap_.",
            groupSmsId);
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
        TLOGE(WmsLogTag::DMS,
            "RemoveChildFromGroup. remove screen:%{public}" PRIu64" failed from screenGroup:%{public}" PRIu64".",
            screen->screenId_, screen->groupSmsId_);
        return false;
    }
    if (screenGroup->GetChildCount() == 0) {
        // Group removed, need to do something.
        std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
        smsScreenGroupMap_.erase(screenGroup->screenId_);
        screenSessionMap_.erase(screenGroup->screenId_);
        TLOGE(WmsLogTag::DMS, "RemoveChildFromGroup. screenSessionMap_ remove screen:%{public}" PRIu64"",
            screenGroup->screenId_);
    }
    return true;
}

DMError ScreenSessionManager::SetMirror(ScreenId screenId, std::vector<ScreenId> screens)
{
    TLOGI(WmsLogTag::DMS, "SetMirror, screenId:%{public}" PRIu64"", screenId);
    sptr<ScreenSession> screen = GetScreenSession(screenId);
    if (screen == nullptr || screen->GetScreenProperty().GetScreenType() != ScreenType::REAL) {
        TLOGE(WmsLogTag::DMS, "SetMirror screen is nullptr, or screenType is not real.");
        return DMError::DM_ERROR_NULLPTR;
    }
    screen->groupSmsId_ = 1;
    auto group = GetAbstractScreenGroup(screen->groupSmsId_);
    if (group == nullptr) {
        group = AddToGroupLocked(screen);
        if (group == nullptr) {
            TLOGE(WmsLogTag::DMS, "SetMirror group is nullptr");
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
    TLOGI(WmsLogTag::DMS, "SetMirror success");
    return DMError::DM_OK;
}

sptr<ScreenSessionGroup> ScreenSessionManager::GetAbstractScreenGroup(ScreenId smsScreenId)
{
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    auto iter = smsScreenGroupMap_.find(smsScreenId);
    if (iter == smsScreenGroupMap_.end()) {
        TLOGE(WmsLogTag::DMS, "did not find screen:%{public}" PRIu64"", smsScreenId);
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
        TLOGE(WmsLogTag::DMS, "groupSmsId:%{public}" PRIu64"is not in smsScreenGroupMap_.", groupSmsId);
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
        TLOGI(WmsLogTag::DMS, "ChangeScreenGroup ScreenId: %{public}" PRIu64"", screenId);
        auto screen = GetScreenSession(screenId);
        if (screen == nullptr) {
            TLOGE(WmsLogTag::DMS, "ChangeScreenGroup screen:%{public}" PRIu64" is nullptr", screenId);
            continue;
        }
        TLOGI(WmsLogTag::DMS, "ChangeScreenGroup Screen->groupSmsId_: %{public}" PRIu64"", screen->groupSmsId_);
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
        TLOGI(WmsLogTag::DMS, "AddScreenToGroup screenId: %{public}" PRIu64", Point: %{public}d, %{public}d",
            screen->screenId_, expandPoint.posX_, expandPoint.posY_);
        bool addChildRes = group->AddChild(screen, expandPoint, GetScreenSession(GetDefaultScreenId()));
        if (removeChildResMap[screenId] && addChildRes) {
            changeGroup.emplace_back(screen->ConvertToScreenInfo());
            TLOGD(WmsLogTag::DMS, "changeGroup");
        } else if (removeChildResMap[screenId]) {
            TLOGD(WmsLogTag::DMS, "removeChild");
            removeFromGroup.emplace_back(screen->ConvertToScreenInfo());
        } else if (addChildRes) {
            TLOGD(WmsLogTag::DMS, "AddChild");
            addToGroup.emplace_back(screen->ConvertToScreenInfo());
        } else {
            TLOGD(WmsLogTag::DMS, "default, AddChild failed");
        }
        NotifyDisplayCreate(screen->ConvertToDisplayInfo());
    }

    NotifyScreenGroupChanged(removeFromGroup, ScreenGroupChangeEvent::REMOVE_FROM_GROUP);
    NotifyScreenGroupChanged(changeGroup, ScreenGroupChangeEvent::CHANGE_GROUP);
    NotifyScreenGroupChanged(addToGroup, ScreenGroupChangeEvent::ADD_TO_GROUP);
}

void ScreenSessionManager::RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens)
{
    TLOGI(WmsLogTag::DMS, "RemoveVirtualScreenFromGroup enter!");
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "RemoveVirtualScreenFromGroup permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
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
        TLOGE(WmsLogTag::DMS, "GetRSDisplayNodeByScreenId screen == nullptr!");
        return notFound;
    }
    if (screen->GetDisplayNode() == nullptr) {
        TLOGE(WmsLogTag::DMS, "GetRSDisplayNodeByScreenId displayNode_ == nullptr!");
        return notFound;
    }
    TLOGI(WmsLogTag::DMS, "GetRSDisplayNodeByScreenId: screen: %{public}" PRIu64", nodeId: %{public}" PRIu64" ",
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
                TLOGE(WmsLogTag::DMS, "GetScreenSnapshot screenSession is nullptr!");
                continue;
            }
            sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
            if (displayInfo == nullptr) {
                TLOGE(WmsLogTag::DMS, "GetScreenSnapshot displayInfo is nullptr!");
                continue;
            }
            TLOGI(WmsLogTag::DMS, "GetScreenSnapshot: displayId %{public}" PRIu64"", displayInfo->GetDisplayId());
            if (displayId == displayInfo->GetDisplayId()) {
                displayNode = screenSession->GetDisplayNode();
                screenId = sessionIt.first;
                break;
            }
        }
    }
    if (screenId == SCREEN_ID_INVALID) {
        TLOGE(WmsLogTag::DMS, "GetScreenSnapshot screenId == SCREEN_ID_INVALID!");
        return nullptr;
    }
    if (displayNode == nullptr) {
        TLOGE(WmsLogTag::DMS, "GetScreenSnapshot displayNode == nullptr!");
        return nullptr;
    }
    std::shared_ptr<SurfaceCaptureFuture> callback = std::make_shared<SurfaceCaptureFuture>();
    bool ret = rsInterface_.TakeSurfaceCapture(displayNode, callback);
    if (!ret) {
        TLOGE(WmsLogTag::DMS, "GetScreenSnapshot TakeSurfaceCapture failed");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> screenshot = callback->GetResult(2000); // wait for <= 2000ms
    if (screenshot == nullptr) {
        TLOGE(WmsLogTag::DMS, "Failed to get pixelmap from RS, return nullptr!");
    } else {
        TLOGI(WmsLogTag::DMS, "Sucess to get pixelmap from RS!");
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
    TLOGD(WmsLogTag::DMS, "ENTER!");

    if (system::GetBoolParameter("persist.edm.disallow_screenshot", false)) {
        TLOGI(WmsLogTag::DMS, "GetDisplaySnapshot was disabled by edm!");
        return nullptr;
    }
    if ((Permission::IsSystemCalling() && Permission::CheckCallingPermission(SCREEN_CAPTURE_PERMISSION)) ||
        SessionPermission::IsShellCall()) {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:GetDisplaySnapshot(%" PRIu64")", displayId);
        auto res = GetScreenSnapshot(displayId);
        if (res != nullptr) {
            NotifyScreenshot(displayId);
            if (SessionPermission::IsBetaVersion()) {
                CheckAndSendHiSysEvent("GET_DISPLAY_SNAPSHOT", "hmos.screenshot");
            }
        }
        isScreenShot_ = true;
        NotifyCaptureStatusChanged();
        return res;
    } else if (errorCode) {
        *errorCode = DmErrorCode::DM_ERROR_NO_PERMISSION;
    }
    return nullptr;
}

std::shared_ptr<Media::PixelMap> ScreenSessionManager::GetSnapshotByPicker(Media::Rect &rect, DmErrorCode* errorCode)
{
    TLOGD(WmsLogTag::DMS, "ENTER!");
    *errorCode = DmErrorCode::DM_ERROR_SYSTEM_INNORMAL;
    std::lock_guard<std::mutex> lock(snapBypickerMutex_);

    if (system::GetBoolParameter("persist.edm.disallow_screenshot", false)) {
        *errorCode = DmErrorCode::DM_ERROR_NO_PERMISSION;
        TLOGI(WmsLogTag::DMS, "snapshot was disabled by edm!");
        return nullptr;
    }
    ScreenId screenId = SCREEN_ID_INVALID;
    // get snapshot area frome Screenshot extension
    if (!GetSnapshotArea(rect, errorCode, screenId)) {
        return nullptr;
    }
    auto screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "can not get screen session");
        return nullptr;
    }
    sptr<DisplayInfo> displayInfo = screenSession->ConvertToDisplayInfo();
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "can not get default display");
        return nullptr;
    }
    DisplayId displayId = displayInfo->GetDisplayId();
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:GetSnapshotByPicker(%" PRIu64")", displayId);
    auto pixelMap = GetScreenSnapshot(displayId);
    if (pixelMap != nullptr && SessionPermission::IsBetaVersion()) {
        CheckAndSendHiSysEvent("GET_DISPLAY_SNAPSHOT", "hmos.screenshot");
    }
    isScreenShot_ = true;
    NotifyCaptureStatusChanged();
    *errorCode = DmErrorCode::DM_OK;
    return pixelMap;
}

bool ScreenSessionManager::GetSnapshotArea(Media::Rect &rect, DmErrorCode* errorCode, ScreenId &screenId)
{
    ConfigureScreenSnapshotParams();
    if (ScreenSnapshotPickerConnection::GetInstance().SnapshotPickerConnectExtension()) {
        int32_t ret = ScreenSnapshotPickerConnection::GetInstance().GetScreenSnapshotInfo(rect, screenId);
        if (ret != 0) {
            TLOGE(WmsLogTag::DMS, "GetScreenSnapshotInfo failed");
            ScreenSnapshotPickerConnection::GetInstance().SnapshotPickerDisconnectExtension();
            if (ret == RES_FAILURE_FOR_PRIVACY_WINDOW) {
                *errorCode = DmErrorCode::DM_ERROR_INVALID_CALLING;
            } else {
                *errorCode = DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT;
            }
            return false;
        }
        ScreenSnapshotPickerConnection::GetInstance().SnapshotPickerDisconnectExtension();
    } else {
        *errorCode = DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT;
        TLOGE(WmsLogTag::DMS, "SnapshotPickerConnectExtension failed");
        return false;
    }
    return true;
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
            TLOGI(WmsLogTag::DMS, "destroy screenId in OnRemoteDied: %{public}" PRIu64"", diedId);
            DMError res = DestroyVirtualScreen(diedId);
            if (res != DMError::DM_OK) {
                TLOGE(WmsLogTag::DMS, "destroy failed in OnRemoteDied: %{public}" PRIu64"", diedId);
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
        TLOGE(WmsLogTag::DMS, "GetScreenGroupInfoById permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return nullptr;
    }
    auto screenSessionGroup = GetAbstractScreenGroup(screenId);
    if (screenSessionGroup == nullptr) {
        TLOGE(WmsLogTag::DMS, "GetScreenGroupInfoById cannot find screenGroupInfo: %{public}" PRIu64"", screenId);
        return nullptr;
    }
    return screenSessionGroup->ConvertToScreenGroupInfo();
}

void ScreenSessionManager::NotifyScreenConnected(sptr<ScreenInfo> screenInfo)
{
    if (screenInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "NotifyScreenConnected error, screenInfo is nullptr.");
        return;
    }
    auto task = [=] {
        TLOGI(WmsLogTag::DMS, "NotifyScreenConnected, screenId:%{public}" PRIu64"", screenInfo->GetScreenId());
        OnScreenConnect(screenInfo);
    };
    taskScheduler_->PostAsyncTask(task, "NotifyScreenConnected");
}

void ScreenSessionManager::NotifyScreenDisconnected(ScreenId screenId)
{
    auto task = [=] {
        TLOGI(WmsLogTag::DMS, "NotifyScreenDisconnected, screenId:%{public}" PRIu64"", screenId);
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
    TLOGI(WmsLogTag::DMS, "NotifyDisplayCreate");
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
    TLOGI(WmsLogTag::DMS, "NotifyDisplayDestroy");
    for (auto& agent : agents) {
        agent->OnDisplayDestroy(displayId);
    }
}

void ScreenSessionManager::NotifyScreenGroupChanged(
    const sptr<ScreenInfo>& screenInfo, ScreenGroupChangeEvent event)
{
    if (screenInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "screenInfo is nullptr.");
        return;
    }
    std::string trigger = SysCapUtil::GetClientName();
    auto task = [=] {
        TLOGI(WmsLogTag::DMS, "screenId:%{public}" PRIu64", trigger:[%{public}s]",
            screenInfo->GetScreenId(), trigger.c_str());
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
        TLOGI(WmsLogTag::DMS, "trigger:[%{public}s]", trigger.c_str());
        OnScreenGroupChange(trigger, screenInfo, event);
    };
    taskScheduler_->PostAsyncTask(task, "NotifyScreenGroupChanged");
}

void ScreenSessionManager::NotifyPrivateSessionStateChanged(bool hasPrivate)
{
    if (hasPrivate == screenPrivacyStates) {
        TLOGD(WmsLogTag::DMS, "screen session state is not changed, return");
        return;
    }
    TLOGI(WmsLogTag::DMS, "PrivateSession status : %{public}u", hasPrivate);
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
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "SetScreenPrivacyState permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return;
    }
    TLOGI(WmsLogTag::DMS, "SetScreenPrivacyState enter, hasPrivate: %{public}d", hasPrivate);
    ScreenId id = GetDefaultScreenId();
    auto screenSession = GetScreenSession(id);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "can not get default screen now");
        return;
    }
    screenSession->SetPrivateSessionForeground(hasPrivate);
    NotifyPrivateSessionStateChanged(hasPrivate);
}

void ScreenSessionManager::SetPrivacyStateByDisplayId(DisplayId id, bool hasPrivate)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "SetPrivacyStateByDisplayId permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return;
    }
    TLOGI(WmsLogTag::DMS, "SetPrivacyStateByDisplayId enter, hasPrivate: %{public}d", hasPrivate);
    std::vector<ScreenId> screenIds = GetAllScreenIds();
    auto iter = std::find(screenIds.begin(), screenIds.end(), id);
    if (iter == screenIds.end()) {
        TLOGE(WmsLogTag::DMS, "SetPrivacyStateByDisplayId invalid displayId");
        return;
    }
    auto screenSession = GetScreenSession(id);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "can not get id: %{public}" PRIu64" screen now", id);
        return;
    }
    screenSession->SetPrivateSessionForeground(hasPrivate);
    NotifyPrivateSessionStateChanged(hasPrivate);
}

void ScreenSessionManager::SetScreenPrivacyWindowList(DisplayId id, std::vector<std::string> privacyWindowList)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "SetScreenPrivacyWindowList permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return;
    }
    TLOGI(WmsLogTag::DMS, "SetScreenPrivacyWindowList enter");
    std::vector<ScreenId> screenIds = GetAllScreenIds();
    auto iter = std::find(screenIds.begin(), screenIds.end(), id);
    if (iter == screenIds.end()) {
        TLOGE(WmsLogTag::DMS, "SetScreenPrivacyWindowList invalid displayId");
        return;
    }
    auto screenSession = GetScreenSession(id);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "can not get id: %{public}" PRIu64" screen now", id);
        return;
    }
    NotifyPrivateWindowListChanged(id, privacyWindowList);
}

void ScreenSessionManager::NotifyPrivateWindowListChanged(DisplayId id, std::vector<std::string> privacyWindowList)
{
    TLOGI(WmsLogTag::DMS, "Notify displayid: %{public}" PRIu64" PrivateWindowListChanged", id);
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::PRIVATE_WINDOW_LIST_LISTENER);
    if (agents.empty()) {
        return;
    }
    for (auto& agent : agents) {
        agent->NotifyPrivateStateWindowListChanged(id, privacyWindowList);
    }
}

DMError ScreenSessionManager::HasPrivateWindow(DisplayId id, bool& hasPrivateWindow)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "HasPrivateWindow permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    std::vector<ScreenId> screenIds = GetAllScreenIds();
    auto iter = std::find(screenIds.begin(), screenIds.end(), id);
    if (iter == screenIds.end()) {
        TLOGE(WmsLogTag::DMS, "HasPrivateWindow invalid displayId");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    auto screenSession = GetScreenSession(id);
    if (screenSession == nullptr) {
        return DMError::DM_ERROR_NULLPTR;
    }
    hasPrivateWindow = screenSession->HasPrivateSessionForeground();
    TLOGI(WmsLogTag::DMS, "id: %{public}" PRIu64" has private window: %{public}u",
        id, static_cast<uint32_t>(hasPrivateWindow));
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
        TLOGE(WmsLogTag::DMS, "OnScreenConnect screenInfo nullptr");
        return;
    }
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
    if (agents.empty()) {
        TLOGI(WmsLogTag::DMS, "OnScreenConnect agents empty");
        return;
    }
    TLOGI(WmsLogTag::DMS, "OnScreenConnect");
    for (auto& agent : agents) {
        agent->OnScreenConnect(screenInfo);
    }
}

void ScreenSessionManager::OnScreenDisconnect(ScreenId screenId)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
    if (agents.empty()) {
        TLOGI(WmsLogTag::DMS, "OnScreenDisconnect agents empty");
        return;
    }
    TLOGI(WmsLogTag::DMS, "OnScreenDisconnect");
    for (auto& agent : agents) {
        agent->OnScreenDisconnect(screenId);
    }
}

void ScreenSessionManager::OnScreenshot(sptr<ScreenshotInfo> info)
{
    if (info == nullptr) {
        TLOGE(WmsLogTag::DMS, "OnScreenshot info is null");
        return;
    }
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREENSHOT_EVENT_LISTENER);
    if (agents.empty()) {
        TLOGD(WmsLogTag::DMS, "agents empty");
        return;
    }
    TLOGI(WmsLogTag::DMS, "onScreenshot");
    for (auto& agent : agents) {
        agent->OnScreenshot(info);
    }
}

sptr<CutoutInfo> ScreenSessionManager::GetCutoutInfo(DisplayId displayId)
{
    DmsXcollie dmsXcollie("DMS:GetCutoutInfo", XCOLLIE_TIMEOUT_10S);
    return screenCutoutController_ ? screenCutoutController_->GetScreenCutoutInfo(displayId) : nullptr;
}

DMError ScreenSessionManager::HasImmersiveWindow(bool& immersive)
{
    if (!clientProxy_) {
        TLOGI(WmsLogTag::DMS, "clientProxy_ is null");
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
        TLOGW(WmsLogTag::DMS, "screenSession or screenCutoutController_ is null");
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
        TLOGE(WmsLogTag::DMS, "DumpAllScreensInfo permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
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
        oss << std::left << std::setw(21) << screenInfo->GetName() // 21 is width
            << std::left << std::setw(9) << screenType // 9 is width
            << std::left << std::setw(8) << (screenSession->isScreenGroup_ ? "true" : "false") // 8 is width
            << std::left << std::setw(6) << screenSession->screenId_ // 6 is width
            << std::left << std::setw(21) << screenSession->rsId_ // 21 is width
            << std::left << std::setw(10) << screenSession->activeIdx_ // 10 is width
            << std::left << std::setw(4) << screenInfo->GetVirtualPixelRatio() // 4 is width
            << std::left << std::setw(9) << static_cast<uint32_t>(screenInfo->GetRotation()) // 9 is width
            << std::left << std::setw(12) << static_cast<uint32_t>(screenInfo->GetOrientation()) // 12 is width
            << std::left << std::setw(19) // 19 is width
                << static_cast<uint32_t>(screenSession->GetScreenRequestedOrientation())
            << std::left << std::setw(21) << nodeId // 21 is width
            << std::endl;
    }
    oss << "total screen num: " << screenSessionMap_.size() << std::endl;
    dumpInfo.append(oss.str());
}

void ScreenSessionManager::DumpSpecialScreenInfo(ScreenId id, std::string& dumpInfo)
{
    if (!(SessionPermission::IsSACalling() || SessionPermission::IsStartByHdcd())) {
        TLOGE(WmsLogTag::DMS, "DumpSpecialScreenInfo permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return;
    }
    std::ostringstream oss;
    sptr<ScreenSession> session = GetScreenSession(id);
    if (!session) {
        TLOGE(WmsLogTag::DMS, "Get screen session failed.");
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
        TLOGI(WmsLogTag::DMS, "Error found physic screen config with id: %{public}" PRIu64, screenId);
        return property;
    }
    return iter->second;
}

void ScreenSessionManager::SetFoldDisplayMode(const FoldDisplayMode displayMode)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "SetFoldDisplayMode permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return;
    }
    if (!g_foldScreenFlag) {
        return;
    }
    if (foldScreenController_ == nullptr) {
        TLOGI(WmsLogTag::DMS, "SetFoldDisplayMode foldScreenController_ is null");
        return;
    }
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "SetFoldDisplayMode permission denied!");
        return;
    }
    foldScreenController_->SetDisplayMode(displayMode);
    NotifyClientProxyUpdateFoldDisplayMode(displayMode);
}

void ScreenSessionManager::SetDisplayScale(ScreenId screenId, float scaleX, float scaleY, float pivotX,
    float pivotY)
{
    if (!SessionPermission::IsSACalling()) {
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return;
    }
    auto session = GetScreenSession(screenId);
    if (session == nullptr) {
        TLOGE(WmsLogTag::DMS, "session is null");
        return;
    }
    auto displayNode = GetDisplayNode(screenId);
    if (displayNode == nullptr) {
        TLOGE(WmsLogTag::DMS, "displayNode is null");
        return;
    }
    TLOGD(WmsLogTag::DMS,
        "scale [%{public}f, %{public}f] pivot [%{public}f, %{public}f]",
        scaleX,
        scaleY,
        pivotX,
        pivotY);
    displayNode->SetScale(scaleX, scaleY);
    displayNode->SetPivot(pivotX, pivotY);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        TLOGI(WmsLogTag::DMS, "FreeDisplayMirrorNodeInner free displayNode");
        transactionProxy->FlushImplicitTransaction();
    }

    session->SetScreenScale(scaleX, scaleY, pivotX, pivotY);
    std::map<DisplayId, sptr<DisplayInfo>> emptyMap;
    NotifyDisplayStateChange(GetDefaultScreenId(), session->ConvertToDisplayInfo(),
        emptyMap, DisplayStateChangeType::UPDATE_SCALE);
}

void ScreenSessionManager::SetFoldStatusLocked(bool locked)
{
    if (!g_foldScreenFlag) {
        return;
    }
    if (foldScreenController_ == nullptr) {
        TLOGI(WmsLogTag::DMS, "SetFoldStatusLocked foldScreenController_ is null");
        return;
    }
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "SetFoldStatusLocked permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return;
    }
    foldScreenController_->LockDisplayStatus(locked);
}

FoldDisplayMode ScreenSessionManager::GetFoldDisplayMode()
{
    DmsXcollie dmsXcollie("DMS:GetFoldDisplayMode", XCOLLIE_TIMEOUT_10S);
    if (!g_foldScreenFlag) {
        return FoldDisplayMode::UNKNOWN;
    }
    if (foldScreenController_ == nullptr) {
        TLOGD(WmsLogTag::DMS, "GetFoldDisplayMode foldScreenController_ is null");
        return FoldDisplayMode::UNKNOWN;
    }
    return foldScreenController_->GetDisplayMode();
}

bool ScreenSessionManager::IsFoldable()
{
    DmsXcollie dmsXcollie("DMS:IsFoldable", XCOLLIE_TIMEOUT_10S);
    // Most applications do not adapt to Lem rotation and are temporarily treated as non fold device
    if (FoldScreenStateInternel::IsDualDisplayFoldDevice()) {
        return false;
    }
    
    if (!g_foldScreenFlag) {
        return false;
    }
    if (foldScreenController_ == nullptr) {
        TLOGI(WmsLogTag::DMS, "foldScreenController_ is null");
        return false;
    }
    return foldScreenController_->IsFoldable();
}

bool ScreenSessionManager::IsCaptured()
{
    return isScreenShot_ || virtualScreenCount_ > 0 || hdmiScreenCount_ > 0;
}

bool ScreenSessionManager::IsMultiScreenCollaboration()
{
    return isMultiScreenCollaboration_;
}

FoldStatus ScreenSessionManager::GetFoldStatus()
{
    DmsXcollie dmsXcollie("DMS:GetFoldStatus", XCOLLIE_TIMEOUT_10S);
    if (!g_foldScreenFlag) {
        return FoldStatus::UNKNOWN;
    }
    if (foldScreenController_ == nullptr) {
        TLOGI(WmsLogTag::DMS, "foldScreenController_ is null");
        return FoldStatus::UNKNOWN;
    }
    return foldScreenController_->GetFoldStatus();
}

sptr<FoldCreaseRegion> ScreenSessionManager::GetCurrentFoldCreaseRegion()
{
    if (!g_foldScreenFlag) {
        return nullptr;
    }
    if (foldScreenController_ == nullptr) {
        TLOGI(WmsLogTag::DMS, "foldScreenController_ is null");
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
    TLOGI(WmsLogTag::DMS, "NotifyFoldStatusChanged foldStatus:%{public}d", foldStatus);
    sptr<ScreenSession> screenSession = GetDefaultScreenSession();
    if (screenSession != nullptr) {
        if (foldStatus == FoldStatus::FOLDED) {
            screenSession->SetDefaultDeviceRotationOffset(0);
        } else {
            screenSession->SetDefaultDeviceRotationOffset(defaultDeviceRotationOffset_);
        }
    }
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::FOLD_STATUS_CHANGED_LISTENER);
    if (agents.empty()) {
        TLOGI(WmsLogTag::DMS, "NotifyFoldStatusChanged agents is empty");
        return;
    }
    for (auto& agent : agents) {
        agent->NotifyFoldStatusChanged(foldStatus);
    }
}

void ScreenSessionManager::NotifyFoldAngleChanged(std::vector<float> foldAngles)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::FOLD_ANGLE_CHANGED_LISTENER);
    if (agents.empty()) {
        TLOGI(WmsLogTag::DMS, "NotifyFoldAngleChanged agents is empty");
        return;
    }
    for (auto& agent : agents) {
        agent->NotifyFoldAngleChanged(foldAngles);
    }
}

void ScreenSessionManager::NotifyCaptureStatusChanged()
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::CAPTURE_STATUS_CHANGED_LISTENER);
    bool isCapture = IsCaptured();
    isScreenShot_ = false;
    if (agents.empty()) {
        TLOGI(WmsLogTag::DMS, "agents is empty");
        return;
    }
    for (auto& agent : agents) {
        agent->NotifyCaptureStatusChanged(isCapture);
    }
}

void ScreenSessionManager::NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info)
{
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_UPDATE_LISTENER);
    if (agents.empty()) {
        TLOGI(WmsLogTag::DMS, "Agents is empty");
        return;
    }
    for (auto& agent : agents) {
        agent->NotifyDisplayChangeInfoChanged(info);
    }
}

void ScreenSessionManager::NotifyDisplayModeChanged(FoldDisplayMode displayMode)
{
    TLOGI(WmsLogTag::DMS, "DisplayMode:%{public}d", displayMode);
    NotifyClientProxyUpdateFoldDisplayMode(displayMode);
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_MODE_CHANGED_LISTENER);
    if (agents.empty()) {
        TLOGI(WmsLogTag::DMS, "Agents is empty");
        return;
    }
    for (auto& agent : agents) {
        agent->NotifyDisplayModeChanged(displayMode);
    }
}

void ScreenSessionManager::SetDisplayNodeScreenId(ScreenId screenId, ScreenId displayNodeScreenId)
{
    TLOGI(WmsLogTag::DMS, "screenId: %{public}" PRIu64 " displayNodeScreenId: %{public}" PRIu64,
        screenId, displayNodeScreenId);
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:SetDisplayNodeScreenId");
    if (!clientProxy_) {
        TLOGI(WmsLogTag::DMS, "SetDisplayNodeScreenId clientProxy_ is null");
        return;
    }
    clientProxy_->SetDisplayNodeScreenId(screenId, displayNodeScreenId);
#ifdef DEVICE_STATUS_ENABLE
    SetDragWindowScreenId(screenId, displayNodeScreenId);
#endif // DEVICE_STATUS_ENABLE
}

#ifdef DEVICE_STATUS_ENABLE
void ScreenSessionManager::SetDragWindowScreenId(ScreenId screenId, ScreenId displayNodeScreenId)
{
    auto interactionManager = Msdp::DeviceStatus::InteractionManager::GetInstance();
    if (interactionManager != nullptr) {
        interactionManager->SetDragWindowScreenId(screenId, displayNodeScreenId);
    }
}
#endif // DEVICE_STATUS_ENABLE

void ScreenSessionManager::OnPropertyChange(const ScreenProperty& newProperty, ScreenPropertyChangeReason reason,
    ScreenId screenId)
{
    TLOGI(WmsLogTag::DMS, "screenId: %{public}" PRIu64 " reason: %{public}d", screenId, static_cast<int>(reason));
    if (!clientProxy_) {
        TLOGI(WmsLogTag::DMS, "OnPropertyChange clientProxy_ is null");
        return;
    }
    clientProxy_->OnPropertyChanged(screenId, newProperty, reason);
}

void ScreenSessionManager::OnPowerStatusChange(DisplayPowerEvent event, EventStatus status,
    PowerStateChangeReason reason)
{
    TLOGI(WmsLogTag::DMS, "[UL_POWER]event: %{public}d, status: %{public}d, reason: %{public}d",
        static_cast<int>(event),
        static_cast<int>(status), static_cast<int>(reason));
    if (!clientProxy_) {
        TLOGI(WmsLogTag::DMS, "[UL_POWER]OnPowerStatusChange clientProxy_ is null");
        return;
    }
    clientProxy_->OnPowerStatusChanged(event, status, reason);
}

void ScreenSessionManager::OnSensorRotationChange(float sensorRotation, ScreenId screenId)
{
    TLOGI(WmsLogTag::DMS, "screenId: %{public}" PRIu64 " sensorRotation: %{public}f", screenId, sensorRotation);
    if (!clientProxy_) {
        TLOGI(WmsLogTag::DMS, "OnSensorRotationChange clientProxy_ is null");
        return;
    }
    clientProxy_->OnSensorRotationChanged(screenId, sensorRotation);
}

void ScreenSessionManager::OnScreenOrientationChange(float screenOrientation, ScreenId screenId)
{
    TLOGI(WmsLogTag::DMS, "screenId: %{public}" PRIu64 " screenOrientation: %{public}f", screenId, screenOrientation);
    if (!clientProxy_) {
        TLOGI(WmsLogTag::DMS, "OnScreenOrientationChange clientProxy_ is null");
        return;
    }
    clientProxy_->OnScreenOrientationChanged(screenId, screenOrientation);
}

void ScreenSessionManager::OnScreenRotationLockedChange(bool isLocked, ScreenId screenId)
{
    TLOGI(WmsLogTag::DMS, "screenId: %{public}" PRIu64 " isLocked: %{public}d", screenId, isLocked);
    if (!clientProxy_) {
        TLOGI(WmsLogTag::DMS, "OnScreenRotationLockedChange clientProxy_ is null");
        return;
    }
    clientProxy_->OnScreenRotationLockedChanged(screenId, isLocked);
}

void ScreenSessionManager::NotifyClientProxyUpdateFoldDisplayMode(FoldDisplayMode displayMode)
{
    if (clientProxy_) {
        TLOGI(WmsLogTag::DMS, "NotifyClientProxyUpdateFoldDisplayMode displayMode = %{public}d",
            static_cast<int>(displayMode));
        clientProxy_->OnUpdateFoldDisplayMode(displayMode);
    }
}

void ScreenSessionManager::ScbClientDeathCallback(int32_t deathScbPid)
{
    std::unique_lock<std::mutex> lock(oldScbPidsMutex_);
    if (deathScbPid == currentScbPId_ || currentScbPId_ == INVALID_SCB_PID) {
        clientProxy_ = nullptr;
        TLOGE(WmsLogTag::DMS, "death callback, clientProxy is set null");
    }
    if (scbSwitchCV_.wait_for(lock, std::chrono::milliseconds(CV_WAIT_SCBSWITCH_MS))
        == std::cv_status::timeout) {
        TLOGE(WmsLogTag::DMS, "set client task deathScbPid:%{public}d, timeout: %{public}d",
            deathScbPid, CV_WAIT_SCBSWITCH_MS);
    }
    std::ostringstream oss;
    oss << "Scb client death: " << deathScbPid;
    TLOGI(WmsLogTag::DMS, "%{public}s", oss.str().c_str());
    screenEventTracker_.RecordEvent(oss.str());
    oldScbPids_.erase(std::remove(oldScbPids_.begin(), oldScbPids_.end(), deathScbPid), oldScbPids_.end());
}

void ScreenSessionManager::AddScbClientDeathRecipient(const sptr<IScreenSessionManagerClient>& scbClient,
    int32_t scbPid)
{
    sptr<ScbClientListenerDeathRecipient> scbClientDeathListener =
        new (std::nothrow) ScbClientListenerDeathRecipient(scbPid);
    if (scbClientDeathListener == nullptr) {
        TLOGE(WmsLogTag::DMS, "add scb: %{public}d death listener failed", scbPid);
        return;
    }
    if (scbClient != nullptr && scbClient->AsObject() != nullptr) {
        TLOGI(WmsLogTag::DMS, "add scb: %{public}d death listener", scbPid);
        scbClient->AsObject()->AddDeathRecipient(scbClientDeathListener);
    }
}

void ScreenSessionManager::SwitchUser()
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "permission denied, calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return;
    }
    auto userId = GetUserIdByCallingUid();
    auto newScbPid = IPCSkeleton::GetCallingPid();
    SwitchScbNodeHandle(userId, newScbPid, false);
    MockSessionManagerService::GetInstance().NotifyWMSConnected(userId, GetDefaultScreenId(), false);
}

void ScreenSessionManager::ScbStatusRecoveryWhenSwitchUser(std::vector<int32_t> oldScbPids, int32_t newScbPid)
{
    NotifyFoldStatusChanged(GetFoldStatus());
    NotifyDisplayModeChanged(GetFoldDisplayMode());
    sptr<ScreenSession> screenSession = GetDefaultScreenSession();
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "fail to get default screenSession");
        return;
    }
    if (g_foldScreenFlag) {
        auto foldStatus = GetFoldStatus();
        // fold device will be callback NotifyFoldToExpandCompletion to UpdateRotationAfterBoot
        if (foldStatus == FoldStatus::EXPAND || foldStatus == FoldStatus::HALF_FOLD) {
            screenSession->UpdatePropertyByFoldControl(GetPhyScreenProperty(SCREEN_ID_FULL));
            screenSession->PropertyChange(screenSession->GetScreenProperty(),
                ScreenPropertyChangeReason::FOLD_SCREEN_EXPAND);
        } else if (foldStatus == FoldStatus::FOLDED) {
            screenSession->UpdatePropertyByFoldControl(GetPhyScreenProperty(SCREEN_ID_MAIN));
            screenSession->PropertyChange(screenSession->GetScreenProperty(),
                ScreenPropertyChangeReason::FOLD_SCREEN_FOLDING);
        } else {
            TLOGE(WmsLogTag::DMS, "unsupport foldStatus: %{public}u", foldStatus);
        }
    } else {
        screenSession->UpdateRotationAfterBoot(true);
    }
    clientProxy_->SwitchUserCallback(oldScbPids, newScbPid);
}

void ScreenSessionManager::SetClient(const sptr<IScreenSessionManagerClient>& client)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::DMS, "permission denied, calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return;
    }
    if (!client) {
        TLOGE(WmsLogTag::DMS, "SetClient client is null");
        return;
    }
    clientProxy_ = client;
    auto userId = GetUserIdByCallingUid();
    auto newScbPid = IPCSkeleton::GetCallingPid();

    std::ostringstream oss;
    oss << "set client userId: " << userId
        << " clientName: " << SysCapUtil::GetClientName();
    TLOGI(WmsLogTag::DMS, "%{public}s", oss.str().c_str());
    screenEventTracker_.RecordEvent(oss.str());
    
    MockSessionManagerService::GetInstance().NotifyWMSConnected(userId, GetDefaultScreenId(), true);
    NotifyClientProxyUpdateFoldDisplayMode(GetFoldDisplayMode());
    SetClientInner();
    SwitchScbNodeHandle(userId, newScbPid, true);
    AddScbClientDeathRecipient(client, newScbPid);
}

void ScreenSessionManager::SwitchScbNodeHandle(int32_t newUserId, int32_t newScbPid, bool coldBoot)
{
    std::ostringstream oss;
    oss << "currentUserId: " << currentUserId_
        << "  currentScbPId" << currentScbPId_
        << "  newUserId: " << newUserId
        << "  newScbPid: " << newScbPid
        << "  coldBoot: " << static_cast<int32_t>(coldBoot);
    TLOGI(WmsLogTag::DMS, "%{public}s", oss.str().c_str());
    screenEventTracker_.RecordEvent(oss.str());

    std::unique_lock<std::mutex> lock(oldScbPidsMutex_);
    if (currentScbPId_ != INVALID_SCB_PID) {
        auto pidIter = std::find(oldScbPids_.begin(), oldScbPids_.end(), currentScbPId_);
        if (pidIter == oldScbPids_.end() && currentScbPId_ > 0) {
            oldScbPids_.emplace_back(currentScbPId_);
        }
        oldScbPids_.erase(std::remove(oldScbPids_.begin(), oldScbPids_.end(), newScbPid), oldScbPids_.end());
        if (oldScbPids_.size() == 0) {
            TLOGE(WmsLogTag::DMS, "swicth user failed, oldScbPids is null");
            screenEventTracker_.RecordEvent("swicth user failed, oldScbPids is null");
        }
    }
    if (!clientProxy_) {
        TLOGE(WmsLogTag::DMS, "clientProxy is null");
        return;
    }
    if (coldBoot) {
        clientProxy_->SwitchUserCallback(oldScbPids_, newScbPid);
        clientProxyMap_[newUserId] = clientProxy_;
    } else {
        // hot switch
        if (clientProxyMap_.count(newUserId) == 0) {
            TLOGE(WmsLogTag::DMS, "not found client proxy. userId:%{public}d.", newUserId);
            return;
        }
        if (newUserId == currentUserId_) {
            TLOGI(WmsLogTag::DMS, "switch user not change");
            return;
        }
        clientProxy_ = clientProxyMap_[newUserId];
        ScbStatusRecoveryWhenSwitchUser(oldScbPids_, newScbPid);
    }
    currentUserId_ = newUserId;
    currentScbPId_ = newScbPid;
    scbSwitchCV_.notify_all();
}

void ScreenSessionManager::SetClientInner()
{
    std::lock_guard<std::recursive_mutex> lock(screenSessionMapMutex_);
    for (const auto& iter : screenSessionMap_) {
        if (!iter.second) {
            continue;
        }
        // In the rotating state, after scb restarts, the screen information needs to be reset.
        float phyWidth = 0.0f;
        float phyHeight = 0.0f;
        bool isReset = true;
        GetCurrentScreenPhyBounds(phyWidth, phyHeight, isReset, iter.first);
        auto localRotation = iter.second->GetRotation();
        TLOGI(WmsLogTag::DMS, "phyWidth = :%{public}f, phyHeight = :%{public}f, localRotation = :%{public}u",
            phyWidth, phyHeight, localRotation);
        bool isModeChanged = localRotation != Rotation::ROTATION_0;
        if (isModeChanged && isReset) {
            TLOGI(WmsLogTag::DMS, "screen(id:%{public}" PRIu64 ") current is not default mode, reset it", iter.first);
            SetRotation(iter.first, Rotation::ROTATION_0, false);
            iter.second->SetDisplayBoundary(RectF(0, 0, phyWidth, phyHeight), 0);
        }
        if (!clientProxy_) {
            TLOGE(WmsLogTag::DMS, "clientProxy is null");
            return;
        }
        clientProxy_->OnScreenConnectionChanged(iter.first, ScreenEvent::CONNECTED,
            iter.second->GetRSScreenId(), iter.second->GetName());
    }
}

void ScreenSessionManager::GetCurrentScreenPhyBounds(float& phyWidth, float& phyHeight,
                                                     bool& isReset, const ScreenId& screenid)
{
    if (foldScreenController_ != nullptr) {
        FoldDisplayMode displayMode = GetFoldDisplayMode();
        TLOGI(WmsLogTag::DMS, "fold screen with screenId = %{public}u", displayMode);
        if (displayMode == FoldDisplayMode::MAIN) {
            auto phyBounds = GetPhyScreenProperty(SCREEN_ID_MAIN).GetPhyBounds();
            phyWidth = phyBounds.rect_.width_;
            phyHeight = phyBounds.rect_.height_;
        } else if (displayMode == FoldDisplayMode::FULL) {
            auto phyBounds = GetPhyScreenProperty(SCREEN_ID_FULL).GetPhyBounds();
            phyWidth = phyBounds.rect_.width_;
            phyHeight = phyBounds.rect_.height_;
            if (g_screenRotationOffSet == ROTATION_90 || g_screenRotationOffSet == ROTATION_270) {
                std::swap(phyWidth, phyHeight);
            }
        } else {
            isReset = false;
        }
    } else {
        int id = HiviewDFX::XCollie::GetInstance().SetTimer("GetCurrentScreenPhyBounds", XCOLLIE_TIMEOUT_10S, nullptr,
            nullptr, HiviewDFX::XCOLLIE_FLAG_LOG);
        auto remoteScreenMode = rsInterface_.GetScreenActiveMode(screenid);
        HiviewDFX::XCollie::GetInstance().CancelTimer(id);
        phyWidth = remoteScreenMode.GetScreenWidth();
        phyHeight = remoteScreenMode.GetScreenHeight();
    }
}

ScreenProperty ScreenSessionManager::GetScreenProperty(ScreenId screenId)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        TLOGI(WmsLogTag::DMS, "GetScreenProperty screenSession is null");
        return {};
    }
    return screenSession->GetScreenProperty();
}

std::shared_ptr<RSDisplayNode> ScreenSessionManager::GetDisplayNode(ScreenId screenId)
{
    auto screenSession = GetScreenSession(screenId);
    if (!screenSession) {
        TLOGE(WmsLogTag::DMS, "GetDisplayNode screenSession is null");
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
        .append("|unlock the screen display status\n")
        .append(" -z                             ")
        .append("|switch to fold half status\n")
        .append(" -y                             ")
        .append("|switch to expand status\n")
        .append(" -p                             ")
        .append("|switch to fold status\n");
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
        TLOGE(WmsLogTag::DMS, "invalid id");
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
    TLOGI(WmsLogTag::DMS, "Dump begin");
    sptr<ScreenSessionDumper> dumper = new ScreenSessionDumper(fd, args);
    if (dumper == nullptr) {
        TLOGE(WmsLogTag::DMS, "dumper is nullptr");
        return -1;
    }
    dumper->DumpFreezedPidList(freezedPidList_);
    dumper->DumpEventTracker(screenEventTracker_);
    dumper->DumpMultiUserInfo(oldScbPids_, currentUserId_, currentScbPId_);
    dumper->ExcuteDumpCmd();

    std::vector<std::string> params;
    for (auto& arg : args) {
        params.emplace_back(Str16ToStr8(arg));
    }
    std::string dumpInfo;
    if (params.empty()) {
        ShowHelpInfo(dumpInfo);
    } else if (params.size() == 1 && params[0] == ARG_DUMP_HELP) { // 1: params num
        ShowHelpInfo(dumpInfo);
    } else if (params.size() == 1 && IsValidDisplayModeCommand(params[0])) {
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
    TLOGI(WmsLogTag::DMS, "dump end");
    return 0;
}

bool ScreenSessionManager::IsValidDisplayModeCommand(std::string command)
{
    if (std::find(displayModeCommands.begin(), displayModeCommands.end(), command) != displayModeCommands.end()) {
        return true;
    }
    return false;
}

int ScreenSessionManager::SetFoldDisplayMode(const std::string& modeParam)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "SetFoldDisplayMode permission denied!");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return -1;
    }
    if (modeParam.empty()) {
        return -1;
    }
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    if (modeParam == ARG_FOLD_DISPLAY_FULL) {
        displayMode = FoldDisplayMode::FULL;
    } else if (modeParam == ARG_FOLD_DISPLAY_MAIN) {
        displayMode = FoldDisplayMode::MAIN;
    } else if (modeParam == ARG_FOLD_DISPLAY_SUB) {
        displayMode = FoldDisplayMode::SUB;
    } else if (modeParam == ARG_FOLD_DISPLAY_COOR) {
        displayMode = FoldDisplayMode::COORDINATION;
    } else {
        TLOGW(WmsLogTag::DMS, "SetFoldDisplayMode mode not support");
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
        TLOGW(WmsLogTag::DMS, "SetFoldStatusLocked status not support");
        return -1;
    }
    SetFoldStatusLocked(lockDisplayStatus);
    return 0;
}

int ScreenSessionManager::NotifyFoldStatusChanged(const std::string& statusParam)
{
    TLOGI(WmsLogTag::DMS, "NotifyFoldStatusChanged is dump log");
    if (statusParam.empty()) {
        return -1;
    }
    FoldStatus foldStatus = FoldStatus::UNKNOWN;
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    if (statusParam == STATUS_FOLD_HALF) {
        foldStatus = FoldStatus::HALF_FOLD;
        displayMode = FoldDisplayMode::FULL;
    } else if (statusParam == STATUS_EXPAND) {
        foldStatus = FoldStatus::EXPAND;
        displayMode = FoldDisplayMode::FULL;
    } else if (statusParam == STATUS_FOLD) {
        foldStatus = FoldStatus::FOLDED;
        displayMode = FoldDisplayMode::MAIN;
    } else {
        TLOGW(WmsLogTag::DMS, "NotifyFoldStatusChanged status not support");
        return -1;
    }
    SetFoldDisplayMode(displayMode);
    if (foldScreenController_ != nullptr) {
        foldScreenController_->SetFoldStatus(foldStatus);
    }
    NotifyFoldStatusChanged(foldStatus);
    return 0;
}

void ScreenSessionManager::NotifyAvailableAreaChanged(DMRect area)
{
    TLOGI(WmsLogTag::DMS, "NotifyAvailableAreaChanged call");
    auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::AVAILABLE_AREA_CHANGED_LISTENER);
    if (agents.empty()) {
        TLOGI(WmsLogTag::DMS, "NotifyAvailableAreaChanged agents is empty");
        return;
    }
    for (auto& agent : agents) {
        agent->NotifyAvailableAreaChanged(area);
    }
}

DMError ScreenSessionManager::GetAvailableArea(DisplayId displayId, DMRect& area)
{
    auto displayInfo = GetDisplayInfoById(displayId);
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "can not get displayInfo.");
        return DMError::DM_ERROR_NULLPTR;
    }
    auto screenSession = GetScreenSession(displayInfo->GetScreenId());
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "can not get default screen now");
        return DMError::DM_ERROR_NULLPTR;
    }
    area = screenSession->GetAvailableArea();
    return DMError::DM_OK;
}

void ScreenSessionManager::UpdateAvailableArea(ScreenId screenId, DMRect area)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "update available area permission denied!");
        return;
    }

    auto screenSession = GetScreenSession(screenId);
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "can not get default screen now");
        return;
    }
    if (!screenSession->UpdateAvailableArea(area)) {
        return;
    }
    NotifyAvailableAreaChanged(area);
}

void ScreenSessionManager::NotifyFoldToExpandCompletion(bool foldToExpand)
{
    TLOGI(WmsLogTag::DMS, "NotifyFoldToExpandCompletion ENTER");
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "notify permission denied");
        TLOGE(WmsLogTag::DMS, "calling clientName: %{public}s, calling pid: %{public}d",
            SysCapUtil::GetClientName().c_str(), IPCSkeleton::GetCallingPid());
        return;
    }
    if (FoldScreenStateInternel::IsSingleDisplayFoldDevice()) {
        SetDisplayNodeScreenId(SCREEN_ID_FULL, foldToExpand ? SCREEN_ID_FULL : SCREEN_ID_MAIN);
    }
    sptr<ScreenSession> screenSession = GetDefaultScreenSession();
    if (screenSession == nullptr) {
        TLOGE(WmsLogTag::DMS, "fail to get default screenSession");
        return;
    }
    screenSession->UpdateRotationAfterBoot(foldToExpand);
}

void ScreenSessionManager::CheckAndSendHiSysEvent(const std::string& eventName, const std::string& bundleName) const
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:CheckAndSendHiSysEvent");
    if (eventName != "CREATE_VIRTUAL_SCREEN") {
        if (!Permission::CheckIsCallingBundleName(bundleName)) {
            TLOGD(WmsLogTag::DMS, "BundleName not in whitelist!");
            return;
        }
    }
    int32_t eventRet = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        eventName, // CREATE_VIRTUAL_SCREEN, GET_DISPLAY_SNAPSHOT
        OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "PID", getpid(),
        "UID", getuid());
    TLOGI(WmsLogTag::DMS, "%{public}s: Write HiSysEvent ret:%{public}d", eventName.c_str(), eventRet);
}

DMError ScreenSessionManager::ProxyForFreeze(const std::set<int32_t>& pidList, bool isProxy)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    {
        std::lock_guard<std::mutex> lock(freezedPidListMutex_);
        for (auto pid : pidList) {
            if (isProxy) {
                freezedPidList_.insert(pid);
            } else {
                freezedPidList_.erase(pid); // set删除不存在的元素不会引发异常
            }
        }
    }
    if (isProxy) {
        return DMError::DM_OK;
    }

    // 进程解冻时刷新一次displaychange
    sptr<ScreenSession> screenSession = GetScreenSession(GetDefaultScreenId());
    if (!screenSession) {
        return DMError::DM_ERROR_NULLPTR;
    }
    auto task = [=] {
        auto agents = dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::DISPLAY_EVENT_LISTENER);
        for (auto& agent : agents) {
            int32_t agentPid = dmAgentContainer_.GetAgentPid(agent);
            if (pidList.count(agentPid) != 0) {
                agent->OnDisplayChange(screenSession->ConvertToDisplayInfo(), DisplayChangeEvent::DISPLAY_UNFREEZED);
            }
        }
    };
    taskScheduler_->PostAsyncTask(task, "ProxyForUnFreeze NotifyDisplayChanged");
    return DMError::DM_OK;
}

DMError ScreenSessionManager::ResetAllFreezeStatus()
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        return DMError::DM_ERROR_NOT_SYSTEM_APP;
    }
    std::lock_guard<std::mutex> lock(freezedPidListMutex_);
    freezedPidList_.clear();
    TLOGI(WmsLogTag::DMS, "freezedPidList_ has been clear.");
    return DMError::DM_OK;
}

DeviceScreenConfig ScreenSessionManager::GetDeviceScreenConfig()
{
    DmsXcollie dmsXcollie("DMS:GetDeviceScreenConfig", XCOLLIE_TIMEOUT_10S);
    return deviceScreenConfig_;
}

void ScreenSessionManager::RegisterApplicationStateObserver()
{
#ifdef SENSOR_ENABLE
    std::string identify = IPCSkeleton::ResetCallingIdentity();
    FoldScreenSensorManager::GetInstance().RegisterApplicationStateObserver();
    IPCSkeleton::SetCallingIdentity(identify);
#endif
}

void ScreenSessionManager::SetVirtualScreenBlackList(ScreenId screenId, std::vector<uint64_t>& windowIdList)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        return;
    }
    TLOGI(WmsLogTag::DMS, "Enter, screenId: %{public}" PRIu64, screenId);
    if (windowIdList.empty()) {
        TLOGE(WmsLogTag::DMS, "WindowIdList is empty");
        return;
    }
    ScreenId rsScreenId = SCREEN_ID_INVALID;
    if (!ConvertScreenIdToRsScreenId(screenId, rsScreenId)) {
        TLOGE(WmsLogTag::DMS, "No corresponding rsId");
        return;
    }
    if (!clientProxy_) {
        TLOGE(WmsLogTag::DMS, "clientProxy_ is nullptr");
        return;
    }
    std::vector<uint64_t> surfaceNodeIds;
    clientProxy_->OnGetSurfaceNodeIdsFromMissionIdsChanged(windowIdList, surfaceNodeIds);
    rsInterface_.SetVirtualScreenBlackList(rsScreenId, surfaceNodeIds);
}

void ScreenSessionManager::DisablePowerOffRenderControl(ScreenId screenId)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        return;
    }
    TLOGI(WmsLogTag::DMS, "Enter, screenId: %{public}" PRIu64, screenId);
    ScreenId rsScreenId = SCREEN_ID_INVALID;
    if (!ConvertScreenIdToRsScreenId(screenId, rsScreenId)) {
        TLOGE(WmsLogTag::DMS, "No corresponding rsId");
        return;
    }
    rsInterface_.DisablePowerOffRenderControl(rsScreenId);
}

void ScreenSessionManager::ReportFoldStatusToScb(std::vector<std::string>& screenFoldInfo)
{
    if (clientProxy_) {
        auto screenInfo = GetDefaultScreenSession();
        int32_t rotation = -1;
        if (screenInfo != nullptr) {
            rotation = static_cast<int32_t>(screenInfo->GetRotation());
        }
        screenFoldInfo.emplace_back(std::to_string(rotation));

        clientProxy_->OnFoldStatusChangedReportUE(screenFoldInfo);
    }
}

std::vector<DisplayPhysicalResolution> ScreenSessionManager::GetAllDisplayPhysicalResolution()
{
    if (allDisplayPhysicalResolution_.empty()) {
        sptr<ScreenSession> defaultScreen = GetDefaultScreenSession();
        if (defaultScreen == nullptr) {
            TLOGE(WmsLogTag::DMS, "default screen null");
            return allDisplayPhysicalResolution_;
        }
        ScreenProperty defaultScreenProperty = defaultScreen->GetScreenProperty();
        DisplayPhysicalResolution defaultSize;
        defaultSize.foldDisplayMode_ = FoldDisplayMode::UNKNOWN;
        defaultSize.physicalWidth_ = defaultScreenProperty.GetPhyBounds().rect_.width_;
        defaultSize.physicalHeight_ = defaultScreenProperty.GetPhyBounds().rect_.height_;
        allDisplayPhysicalResolution_.emplace_back(defaultSize);
    }
    return allDisplayPhysicalResolution_;
}

bool ScreenSessionManager::SetVirtualScreenStatus(ScreenId screenId, VirtualScreenStatus screenStatus)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        return false;
    }
    TLOGI(WmsLogTag::DMS, "set virtual screen status enter, screenId: %{public}" PRIu64 " screenStatus: %{public}d",
        screenId, static_cast<int32_t>(screenStatus));
    ScreenId rsScreenId = SCREEN_ID_INVALID;
    if (!ConvertScreenIdToRsScreenId(screenId, rsScreenId)) {
        TLOGE(WmsLogTag::DMS, "No corresponding rsId");
        return false;
    }

    return rsInterface_.SetVirtualScreenStatus(screenId, screenStatus);
}

DMError ScreenSessionManager::SetVirtualScreenSecurityExemption(ScreenId screenId, uint32_t pid,
    std::vector<uint64_t>& windowIdList)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::DMS, "permission denied!");
        return DMError::DM_ERROR_INVALID_CALLING;
    }

    // MockSessionManagerService提供的接口有误，临时规避
    std::vector<int32_t> tmpWindowIdList;
    for (uint64_t id : windowIdList) {
        tmpWindowIdList.push_back(static_cast<int32_t>(id));
    }
    std::vector<uint64_t> surfaceNodeIds;
    MockSessionManagerService::GetInstance().GetProcessSurfaceNodeIdByPersistentId(
        pid, tmpWindowIdList, surfaceNodeIds);
    auto rsId = screenIdManager_.ConvertToRsScreenId(screenId);
    auto ret = rsInterface_.SetVirtualScreenSecurityExemptionList(rsId, surfaceNodeIds);

    std::ostringstream oss;
    oss << "screenId:" << screenId << ", rsID: " << rsId << ", pid: " << pid
        << ", winListSize:[ ";
    for (auto val : windowIdList) {
        oss << val << " ";
    }
    oss << "]" << ", surfaceListSize:[ ";
    for (auto val : surfaceNodeIds) {
        oss << val << " ";
    }
    oss << "]" << ", ret: " << ret;
    TLOGI(WmsLogTag::DMS, "%{public}s", oss.str().c_str());
    return ret == 0 ? DMError::DM_OK : DMError::DM_ERROR_UNKNOWN;
}

} // namespace OHOS::Rosen