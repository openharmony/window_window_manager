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

#include "session_manager/include/scene_session_manager.h"

#include <algorithm>

#include <ability_context.h>
#include <ability_manager_client.h>
#include <bundlemgr/launcher_service.h>
#include <hisysevent.h>
#include <parameters.h>
#include <hitrace_meter.h>
#include "parameter.h"
#include "publish/scb_dump_subscriber.h"
#include "screen_manager.h"

#ifdef POWERMGR_DISPLAY_MANAGER_ENABLE
#include <display_power_mgr_client.h>
#endif

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

#ifdef RES_SCHED_ENABLE
#include "res_type.h"
#include "res_sched_client.h"
#endif
#include "scene_system_ability_listener.h"

#include "anr_manager.h"
#include "color_parser.h"
#include "common/include/session_permission.h"
#include "display_manager.h"
#include "scene_input_manager.h"
#include "session/host/include/main_session.h"
#include "session/host/include/scb_system_session.h"
#include "session/host/include/scene_persistent_storage.h"
#include "session/host/include/session_utils.h"
#include "session/host/include/sub_session.h"
#include "session_helper.h"
#include "window_helper.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "singleton_container.h"
#include "xcollie/watchdog.h"
#include "session_manager_agent_controller.h"
#include "distributed_client.h"
#include "softbus_bus_center.h"
#include "perform_reporter.h"
#include "anr_manager.h"
#include "dms_reporter.h"
#include "res_sched_client.h"
#include "res_type.h"
#include "anomaly_detection.h"
#include "hidump_controller.h"
#include "window_pid_visibility_info.h"
#include "session/host/include/multi_instance_manager.h"

#ifdef MEMMGR_WINDOW_ENABLE
#include "mem_mgr_client.h"
#include "mem_mgr_window_info.h"
#endif

#ifdef SECURITY_COMPONENT_MANAGER_ENABLE
#include "sec_comp_enhance_kit.h"
#endif

#ifdef IMF_ENABLE
#include <input_method_controller.h>
#endif // IMF_ENABLE

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManager" };
const std::string SCENE_BOARD_BUNDLE_NAME = "com.ohos.sceneboard";
const std::string SCENE_BOARD_APP_IDENTIFIER = "";
const std::string SCENE_SESSION_MANAGER_THREAD = "OS_SceneSessionManager";
const std::string WINDOW_INFO_REPORT_THREAD = "OS_WindowInfoReportThread";
constexpr const char* PREPARE_TERMINATE_ENABLE_PARAMETER = "persist.sys.prepare_terminate";
constexpr const char* KEY_SESSION_ID = "com.ohos.param.sessionId";
constexpr uint32_t MAX_BRIGHTNESS = 255;
constexpr int32_t PREPARE_TERMINATE_ENABLE_SIZE = 6;
constexpr int32_t DEFAULT_USERID = -1;
constexpr int32_t SCALE_DIMENSION = 2;
constexpr int32_t TRANSLATE_DIMENSION = 2;
constexpr int32_t ROTAION_DIMENSION = 4;
constexpr int32_t CURVE_PARAM_DIMENSION = 4;
const std::string DM_PKG_NAME = "ohos.distributedhardware.devicemanager";
constexpr int32_t NON_ANONYMIZE_LENGTH = 6;
const std::string EMPTY_DEVICE_ID = "";
const int32_t MAX_NUMBER_OF_DISTRIBUTED_SESSIONS = 20;

constexpr int WINDOW_NAME_MAX_WIDTH = 21;
constexpr int DISPLAY_NAME_MAX_WIDTH = 10;
constexpr int VALUE_MAX_WIDTH = 5;
constexpr int MAX_RESEND_TIMES = 6;
constexpr int ORIEN_MAX_WIDTH = 12;
constexpr int OFFSET_MAX_WIDTH = 8;
constexpr int SCALE_MAX_WIDTH = 8;
constexpr int PID_MAX_WIDTH = 8;
constexpr int PARENT_ID_MAX_WIDTH = 6;
constexpr int WINDOW_NAME_MAX_LENGTH = 20;
constexpr int32_t STATUS_BAR_AVOID_AREA = 0;
const std::string ARG_DUMP_ALL = "-a";
const std::string ARG_DUMP_WINDOW = "-w";
const std::string ARG_DUMP_SCREEN = "-s";
const std::string ARG_DUMP_DISPLAY = "-d";
const std::string ARG_DUMP_PIPLINE = "-p";
const std::string ARG_DUMP_SCB = "-b";
const std::string ARG_DUMP_DETAIL = "-c";
constexpr uint64_t NANO_SECOND_PER_SEC = 1000000000; // ns
const int32_t LOGICAL_DISPLACEMENT_32 = 32;
constexpr int32_t GET_TOP_WINDOW_DELAY = 100;

const std::map<std::string, OHOS::AppExecFwk::DisplayOrientation> STRING_TO_DISPLAY_ORIENTATION_MAP = {
    {"unspecified",                         OHOS::AppExecFwk::DisplayOrientation::UNSPECIFIED},
    {"landscape",                           OHOS::AppExecFwk::DisplayOrientation::LANDSCAPE},
    {"portrait",                            OHOS::AppExecFwk::DisplayOrientation::PORTRAIT},
    {"follow_recent",                       OHOS::AppExecFwk::DisplayOrientation::FOLLOWRECENT},
    {"landscape_inverted",                  OHOS::AppExecFwk::DisplayOrientation::LANDSCAPE_INVERTED},
    {"portrait_inverted",                   OHOS::AppExecFwk::DisplayOrientation::PORTRAIT_INVERTED},
    {"auto_rotation",                       OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION},
    {"auto_rotation_landscape",             OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_LANDSCAPE},
    {"auto_rotation_portrait",              OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_PORTRAIT},
    {"auto_rotation_restricted",            OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_RESTRICTED},
    {"auto_rotation_landscape_restricted",  OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED},
    {"auto_rotation_portrait_restricted",   OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_PORTRAIT_RESTRICTED},
    {"locked",                              OHOS::AppExecFwk::DisplayOrientation::LOCKED},
    {"auto_rotation_unspecified",           OHOS::AppExecFwk::DisplayOrientation::AUTO_ROTATION_UNSPECIFIED},
    {"follow_desktop",                      OHOS::AppExecFwk::DisplayOrientation::FOLLOW_DESKTOP},
};

const std::chrono::milliseconds WAIT_TIME(10 * 1000); // 10 * 1000 wait for 10s

std::string GetCurrentTime()
{
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    uint64_t uTime = static_cast<uint64_t>(tn.tv_sec) * NANO_SECOND_PER_SEC +
        static_cast<uint64_t>(tn.tv_nsec);
    return std::to_string(uTime);
}
int Comp(const std::pair<uint64_t, WindowVisibilityState>& a, const std::pair<uint64_t, WindowVisibilityState>& b)
{
    return a.first < b.first;
}

bool GetSingleIntItem(const WindowSceneConfig::ConfigItem& item, int32_t& value)
{
    if (item.IsInts() && item.intsValue_ && item.intsValue_->size() == 1) {
        value = (*item.intsValue_)[0];
        return true;
    }
    return false;
}

int32_t GetPid()
{
    static int32_t pid = static_cast<int32_t>(getpid());
    return pid;
}

class BundleStatusCallback : public IRemoteStub<AppExecFwk::IBundleStatusCallback> {
public:
    BundleStatusCallback() = default;
    virtual ~BundleStatusCallback() = default;

    void OnBundleStateChanged(const uint8_t installType,
        const int32_t resultCode, const std::string& resultMsg, const std::string& bundleName) override {}

    void OnBundleAdded(const std::string& bundleName, const int userId) override
    {
        SceneSessionManager::GetInstance().OnBundleUpdated(bundleName, userId);
    }

    void OnBundleUpdated(const std::string& bundleName, const int userId) override
    {
        SceneSessionManager::GetInstance().OnBundleUpdated(bundleName, userId);
    }

    void OnBundleRemoved(const std::string& bundleName, const int userId) override
    {
        SceneSessionManager::GetInstance().OnBundleUpdated(bundleName, userId);
    }
};
} // namespace

sptr<SceneSessionManager> SceneSessionManager::CreateInstance()
{
    sptr<SceneSessionManager> sessionManager = new SceneSessionManager();
    sessionManager->Init();
    return sessionManager;
}

SceneSessionManager& SceneSessionManager::GetInstance()
{
    static sptr<SceneSessionManager> instance = CreateInstance();
    return *instance;
}

SceneSessionManager::SceneSessionManager() : rsInterface_(RSInterfaces::GetInstance())
{
    taskScheduler_ = std::make_shared<TaskScheduler>(SCENE_SESSION_MANAGER_THREAD);
    currentUserId_ = DEFAULT_USERID;
    launcherService_ = sptr<AppExecFwk::LauncherService>::MakeSptr();
    if (!launcherService_->RegisterCallback(new BundleStatusCallback())) {
        WLOGFE("Failed to register bundle status callback.");
    }
}

SceneSessionManager::~SceneSessionManager()
{
    ScbDumpSubscriber::UnSubscribe(scbDumpSubscriber_);
}

void SceneSessionManager::Init()
{
    auto deviceType = system::GetParameter("const.product.devicetype", "unknown");
    bool isScbCoreEnabled = (deviceType == "phone" || deviceType == "2in1" || deviceType == "tablet") &&
        system::GetParameter("persist.window.scbcore.enable", "1") == "1";
    Session::SetScbCoreEnabled(isScbCoreEnabled);

    constexpr uint64_t interval = 5 * 1000; // 5 second
    if (HiviewDFX::Watchdog::GetInstance().AddThread(
        SCENE_SESSION_MANAGER_THREAD, taskScheduler_->GetEventHandler(), interval)) {
        WLOGFW("Add thread %{public}s to watchdog failed.", SCENE_SESSION_MANAGER_THREAD.c_str());
    }

    bundleMgr_ = GetBundleManager();

    LoadWindowSceneXml();
    LoadWindowParameter();
    InitPrepareTerminateConfig();

    ScreenSessionManagerClient::GetInstance().RegisterDisplayChangeListener(sptr<DisplayChangeListener>::MakeSptr());

    // create handler for inner command at server
    eventLoop_ = AppExecFwk::EventRunner::Create(WINDOW_INFO_REPORT_THREAD);
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread(WINDOW_INFO_REPORT_THREAD, eventHandler_);
    if (ret != 0) {
        WLOGFW("Add thread %{public}s to watchdog failed.", WINDOW_INFO_REPORT_THREAD.c_str());
    }
    taskScheduler_->SetExportHandler(eventHandler_);

    listenerController_ = std::make_shared<SessionListenerController>();

    scbSessionHandler_ = sptr<ScbSessionHandler>::MakeSptr();
    AAFwk::AbilityManagerClient::GetInstance()->RegisterSessionHandler(scbSessionHandler_);
    StartWindowInfoReportLoop();
    WLOGI("SSM init success.");

    RegisterAppListener();
    openDebugTrace_ = std::atoi((system::GetParameter("persist.sys.graphic.openDebugTrace", "0")).c_str()) != 0;
    isKeyboardPanelEnabled_ = system::GetParameter("persist.sceneboard.keyboardPanel.enabled", "1")  == "1";
    SceneInputManager::GetInstance().Init();

    // MMI window state error check
    int32_t retCode = MMI::InputManager::GetInstance()->
        RegisterWindowStateErrorCallback([this](int32_t pid, int32_t persistentId) {
        this->NotifyWindowStateErrorFromMMI(pid, persistentId);
    });
    TLOGI(WmsLogTag::WMS_EVENT, "register WindowStateError callback with ret: %{public}d", retCode);

    scbDumpSubscriber_ = ScbDumpSubscriber::Subscribe();
    MultiInstanceManager::GetInstance().Init(bundleMgr_);
}

void SceneSessionManager::InitScheduleUtils()
{
#ifdef RES_SCHED_ENABLE
    SCBThreadInfo threadInfo = {
        .scbUid_ = std::to_string(getuid()), .scbPid_ = std::to_string(getprocpid()),
        .scbTid_ = std::to_string(getproctid()), .scbBundleName_ = SCENE_BOARD_BUNDLE_NAME
    };
    std::unordered_map<std::string, std::string> payload {
        { "pid", threadInfo.scbPid_ },
        { "tid", threadInfo.scbTid_ },
        { "uid", threadInfo.scbUid_ },
        { "bundleName", threadInfo.scbBundleName_ },
    };
    uint32_t type = OHOS::ResourceSchedule::ResType::RES_TYPE_REPORT_SCENE_BOARD;
    OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, 0, payload);
    auto task = [threadInfo = std::move(threadInfo)]() mutable {
        threadInfo.ssmThreadName_ = "OS_SceneSession";
        threadInfo.ssmTid_ = std::to_string(gettid());
        const int32_t userInteraction = 2;
        std::unordered_map<std::string, std::string> payload{
            { "pid", threadInfo.scbPid_ },
            { "tid", threadInfo.ssmTid_ },
            { "uid", threadInfo.scbUid_ },
            { "extType", "10002" },
            { "cgroupPrio", "1" },
            { "isSa", "0" },
            { "threadName", threadInfo.ssmThreadName_ }
        };
        uint32_t type = ResourceSchedule::ResType::RES_TYPE_KEY_PERF_SCENE;
        OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, userInteraction, payload);
        TLOGI(WmsLogTag::WMS_LIFE, "set RES_TYPE_KEY_PERF_SCENE success");
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            TLOGE(WmsLogTag::WMS_MAIN, "failed to get system ability manager client");
            return;
        }
        auto statusChangeListener = sptr<SceneSystemAbilityListener>::MakeSptr(threadInfo);
        int32_t ret = systemAbilityManager->SubscribeSystemAbility(RES_SCHED_SYS_ABILITY_ID, statusChangeListener);
        if (ret != ERR_OK) {
            TLOGI(WmsLogTag::WMS_MAIN, "failed to subscribe system ability manager");
        }
    };
    taskScheduler_->PostAsyncTask(task, "changeQosTask");
#endif
}

void SceneSessionManager::RegisterAppListener()
{
    appAnrListener_ = sptr<AppAnrListener>::MakeSptr();
    auto appMgrClient = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance();
    if (appMgrClient == nullptr) {
        WLOGFE("appMgrClient is nullptr.");
    } else {
        auto ret = static_cast<int32_t>(appMgrClient->RegisterAppDebugListener(appAnrListener_));
        WLOGFI("Register app debug listener, %{public}d.", ret);
    }
}

void SceneSessionManager::LoadWindowParameter()
{
    const std::string multiWindowUIType = system::GetParameter("const.window.multiWindowUIType", "HandsetSmartWindow");
    if (multiWindowUIType == "HandsetSmartWindow") {
        systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    } else if (multiWindowUIType == "FreeFormMultiWindow") {
        systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    } else if (multiWindowUIType == "TabletSmartWindow") {
        systemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    } else {
        WLOGFE("unknown multiWindowUIType:%{public}s.", multiWindowUIType.c_str());
    }
    appWindowSceneConfig_.multiWindowUIType_ = multiWindowUIType;
}

void SceneSessionManager::LoadWindowSceneXml()
{
    if (WindowSceneConfig::LoadConfigXml()) {
        if (WindowSceneConfig::GetConfig().IsMap()) {
            WindowSceneConfig::DumpConfig(*WindowSceneConfig::GetConfig().mapValue_);
        }
        ConfigWindowSceneXml();
    } else {
        WLOGFE("Load window scene xml failed");
    }
    ConfigDefaultKeyboardAnimation(appWindowSceneConfig_.keyboardAnimationIn_,
        appWindowSceneConfig_.keyboardAnimationOut_);
}

void SceneSessionManager::InitPrepareTerminateConfig()
{
    char value[PREPARE_TERMINATE_ENABLE_SIZE] = "false";
    int32_t retSysParam = GetParameter(PREPARE_TERMINATE_ENABLE_PARAMETER, "false", value,
        PREPARE_TERMINATE_ENABLE_SIZE);
    WLOGFI("InitPrepareTerminateConfig, %{public}s value is %{public}s.", PREPARE_TERMINATE_ENABLE_PARAMETER, value);
    if (retSysParam > 0 && !std::strcmp(value, "true")) {
        isPrepareTerminateEnable_ = true;
    }
}

void SceneSessionManager::ConfigWindowSceneXml()
{
    const auto& config = WindowSceneConfig::GetConfig();
    WindowSceneConfig::ConfigItem item = config["windowEffect"];
    if (item.IsMap()) {
        ConfigWindowEffect(item);
    }
    item = config["decor"];
    if (item.IsMap()) {
        ConfigDecor(item);
    }

    item = config["backgroundswitch"];
    int32_t param = -1;
    systemConfig_.backgroundswitch = GetSingleIntItem(item, param) && param == 1;
    WLOGFD("Load ConfigWindowSceneXml backgroundswitch%{public}d", systemConfig_.backgroundswitch);
    item = config["defaultWindowMode"];
    if (GetSingleIntItem(item, param) &&
        (param == static_cast<int32_t>(WindowMode::WINDOW_MODE_FULLSCREEN) ||
        param == static_cast<int32_t>(WindowMode::WINDOW_MODE_FLOATING))) {
        systemConfig_.defaultWindowMode_ = static_cast<WindowMode>(static_cast<uint32_t>(param));
    }
    item = config["defaultMaximizeMode"];
    if (GetSingleIntItem(item, param) &&
        (param == static_cast<int32_t>(MaximizeMode::MODE_AVOID_SYSTEM_BAR) ||
         param == static_cast<int32_t>(MaximizeMode::MODE_FULL_FILL))) {
        SceneSession::maximizeMode_ = static_cast<MaximizeMode>(param);
    }
    item = config["keyboardAnimation"];
    if (item.IsMap()) {
        ConfigKeyboardAnimation(item);
    }
    item = config["maxFloatingWindowSize"];
    if (GetSingleIntItem(item, param)) {
        systemConfig_.maxFloatingWindowSize_ = static_cast<uint32_t>(param);
    }
    item = config["windowAnimation"];
    if (item.IsMap()) {
        ConfigWindowAnimation(item);
    }
    item = config["startWindowTransitionAnimation"];
    if (item.IsMap()) {
        ConfigStartingWindowAnimation(item);
    }
    ConfigFreeMultiWindow();
    ConfigWindowSizeLimits();
    ConfigSnapshotScale();
    ConfigWindowSceneXml(config);
}

void SceneSessionManager::ConfigWindowSceneXml(const WindowSceneConfig::ConfigItem& config)
{
    WindowSceneConfig::ConfigItem item = config["systemUIStatusBar"];
    if (item.IsMap()) {
        ConfigSystemUIStatusBar(item);
    }
    item = config["backgroundScreenLock"].GetProp("enable");
    if (item.IsBool()) {
        appWindowSceneConfig_.backgroundScreenLock_ = item.boolValue_;
    }
    item = config["rotationMode"];
    if (item.IsString()) {
        appWindowSceneConfig_.rotationMode_ = item.stringValue_;
    }
    item = config["immersive"];
    if (item.IsMap()) {
        ConfigWindowImmersive(item);
    }
    item = config["supportTypeFloatWindow"].GetProp("enable");
    if (item.IsBool()) {
        systemConfig_.supportTypeFloatWindow_ = item.boolValue_;
    }
}

void SceneSessionManager::ConfigWindowImmersive(const WindowSceneConfig::ConfigItem& immersiveConfig)
{
    AppWindowSceneConfig config;
    WindowSceneConfig::ConfigItem item = immersiveConfig["inDesktopStatusBarConfig"];
    if (item.IsMap()) {
        if (ConfigStatusBar(item, config.windowImmersive_.desktopStatusBarConfig_)) {
            appWindowSceneConfig_.windowImmersive_.desktopStatusBarConfig_ =
                config.windowImmersive_.desktopStatusBarConfig_;
        }
    }
    item = immersiveConfig["inSplitStatusBarConfig"]["upDownSplit"];
    if (item.IsMap()) {
        if (ConfigStatusBar(item, config.windowImmersive_.upDownStatusBarConfig_)) {
            appWindowSceneConfig_.windowImmersive_.upDownStatusBarConfig_ =
                config.windowImmersive_.upDownStatusBarConfig_;
        }
    }
    item = immersiveConfig["inSplitStatusBarConfig"]["leftRightSplit"];
    if (item.IsMap()) {
        if (ConfigStatusBar(item, config.windowImmersive_.leftRightStatusBarConfig_)) {
            appWindowSceneConfig_.windowImmersive_.leftRightStatusBarConfig_ =
                config.windowImmersive_.leftRightStatusBarConfig_;
        }
    }
}

bool SceneSessionManager::ConfigStatusBar(const WindowSceneConfig::ConfigItem& config,
    StatusBarConfig& statusBarConfig)
{
    WindowSceneConfig::ConfigItem item = config["showHide"].GetProp("enable");
    if (item.IsBool()) {
        statusBarConfig.showHide_ = item.boolValue_;
    }
    item = config["contentColor"];
    if (item.IsString()) {
        statusBarConfig.contentColor_ = item.stringValue_;
    }
    item = config["backgroundColor"];
    if (item.IsString()) {
        statusBarConfig.backgroundColor_ = item.stringValue_;
    }
    return true;
}

void SceneSessionManager::ConfigFreeMultiWindow()
{
    const auto& config = WindowSceneConfig::GetConfig();
    WindowSceneConfig::ConfigItem freeMultiWindowConfig = config["freeMultiWindow"];
    if (freeMultiWindowConfig.IsMap()) {
        auto supportItem = freeMultiWindowConfig.GetProp("enable");
        if (supportItem.IsBool()) {
            systemConfig_.freeMultiWindowSupport_ = supportItem.boolValue_;
        }
        auto item = freeMultiWindowConfig["decor"];
        if (item.IsMap()) {
            ConfigDecor(item, false);
        }
        int32_t param = -1;
        item = freeMultiWindowConfig["defaultWindowMode"];
        if (GetSingleIntItem(item, param) &&
            (param == static_cast<int32_t>(WindowMode::WINDOW_MODE_FULLSCREEN) ||
            param == static_cast<int32_t>(WindowMode::WINDOW_MODE_FLOATING))) {
            systemConfig_.freeMultiWindowConfig_.defaultWindowMode_ =
                static_cast<WindowMode>(static_cast<uint32_t>(param));
        }
        item = freeMultiWindowConfig["maxMainFloatingWindowNumber"];
        if (GetSingleIntItem(item, param) && (param > 0)) {
            systemConfig_.freeMultiWindowConfig_.maxMainFloatingWindowNumber_ = static_cast<uint32_t>(param);
        }
    }
}

void SceneSessionManager::LoadFreeMultiWindowConfig(bool enable)
{
    FreeMultiWindowConfig freeMultiWindowConfig = systemConfig_.freeMultiWindowConfig_;
    if (enable) {
        systemConfig_.defaultWindowMode_ = freeMultiWindowConfig.defaultWindowMode_;
        systemConfig_.decorModeSupportInfo_ = freeMultiWindowConfig.decorModeSupportInfo_;
        systemConfig_.isSystemDecorEnable_ = freeMultiWindowConfig.isSystemDecorEnable_;
    } else {
        const auto& config = WindowSceneConfig::GetConfig();
        auto item = config["decor"];
        if (item.IsMap()) {
            ConfigDecor(item, true);
        }
        int32_t param = -1;
        item = config["defaultWindowMode"];
        if (GetSingleIntItem(item, param) &&
            (param == static_cast<int32_t>(WindowMode::WINDOW_MODE_FULLSCREEN) ||
            param == static_cast<int32_t>(WindowMode::WINDOW_MODE_FLOATING))) {
            systemConfig_.defaultWindowMode_ = static_cast<WindowMode>(static_cast<uint32_t>(param));
        }
    }
    systemConfig_.freeMultiWindowEnable_ = enable;
    rsInterface_.SetFreeMultiWindowStatus(enable);
}

const SystemSessionConfig& SceneSessionManager::GetSystemSessionConfig() const
{
    return systemConfig_;
}

WSError SceneSessionManager::SwitchFreeMultiWindow(bool enable)
{
    if (!systemConfig_.freeMultiWindowSupport_) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "device not support");
        return WSError::WS_ERROR_DEVICE_NOT_SUPPORT;
    }
    LoadFreeMultiWindowConfig(enable);
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (auto item = sceneSessionMap_.begin(); item != sceneSessionMap_.end(); ++item) {
        auto sceneSession = item->second;
        if (sceneSession == nullptr) {
            continue;
        }
        if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType()) &&
            !WindowHelper::IsSubWindow(sceneSession->GetWindowType())) {
            continue;
        }
        sceneSession->SwitchFreeMultiWindow(enable);
    }

    if (!remoteExtSessionMap_.empty()) {
        for (auto item = remoteExtSessionMap_.begin(); item != remoteExtSessionMap_.end(); ++item) {
            if ((item->first == nullptr) ||(item->second == nullptr)) {
                continue;
            }
            int32_t persistentId = INVALID_SESSION_ID;
            int32_t parentId = INVALID_SESSION_ID;
            if (!GetExtensionWindowIds(item->second, persistentId, parentId)) {
                TLOGE(WmsLogTag::WMS_UIEXT, "Get UIExtension window ids by token failed");
                return WSError::WS_ERROR_INVALID_WINDOW;
            }
            sptr<ISessionStage> sessionStage = iface_cast<ISessionStage>(item->first);
            sessionStage->SwitchFreeMultiWindow(enable);
        }
    }
    WindowStyleType type = enable ?
            WindowStyleType::WINDOW_STYLE_FREE_MULTI_WINDOW : WindowStyleType::WINDOW_STYLE_DEFAULT;
    SessionManagerAgentController::GetInstance().NotifyWindowStyleChange(type);
    return WSError::WS_OK;
}

WSError SceneSessionManager::GetFreeMultiWindowEnableState(bool& enable)
{
    enable = systemConfig_.freeMultiWindowEnable_;
    return WSError::WS_OK;
}

WSError SceneSessionManager::SetSessionContinueState(const sptr<IRemoteObject>& token,
    const ContinueState& continueState)
{
    TLOGI(WmsLogTag::DEFAULT, "Enter");
    auto task = [this, token, continueState]() {
        sptr <SceneSession> sceneSession = FindSessionByToken(token);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "fail to find session by token.");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        sceneSession->SetSessionInfoContinueState(continueState);
        DistributedClient::GetInstance().SetMissionContinueState(sceneSession->GetPersistentId(),
            static_cast<AAFwk::ContinueState>(continueState));
        TLOGI(WmsLogTag::DEFAULT, "SetSessionContinueState id:%{public}d, continueState:%{public}d",
            sceneSession->GetPersistentId(), continueState);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetSessionContinueState");
}

void SceneSessionManager::ConfigDecor(const WindowSceneConfig::ConfigItem& decorConfig, bool mainConfig)
{
    WindowSceneConfig::ConfigItem item = decorConfig.GetProp("enable");
    if (item.IsBool()) {
        if (mainConfig) {
            systemConfig_.isSystemDecorEnable_ = item.boolValue_;
        } else {
            systemConfig_.freeMultiWindowConfig_.isSystemDecorEnable_ = item.boolValue_;
        }
        bool decorEnable = item.boolValue_;
        uint32_t support = 0;
        std::vector<std::string> supportedModes;
        item = decorConfig["supportedMode"];
        if (item.IsStrings()) {
            supportedModes = *item.stringsValue_;
        }
        for (auto mode : supportedModes) {
            if (mode == "fullscreen") {
                support |= WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN;
            } else if (mode == "floating") {
                support |= WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING;
            } else if (mode == "pip") {
                support |= WindowModeSupport::WINDOW_MODE_SUPPORT_PIP;
            } else if (mode == "split") {
                support |= WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
                    WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY;
            } else {
                WLOGFW("Invalid supporedMode");
                support = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
                break;
            }
        }
        if (mainConfig && item.IsStrings()) {
            systemConfig_.decorModeSupportInfo_ = support;
        }
        if (!mainConfig && item.IsStrings()) {
            systemConfig_.freeMultiWindowConfig_.decorModeSupportInfo_ = support;
        }
    }
}

static void AddAlphaToColor(float alpha, std::string& color)
{
    if (color.size() == 9 || alpha > 1.0f) { // size 9: color is ARGB
        return;
    }

    uint32_t alphaValue = 0xFF * alpha;
    std::stringstream ss;
    ss << std::hex << alphaValue;
    std::string strAlpha = ss.str();
    if (strAlpha.size() == 1) {
        strAlpha.append(1, '0');
    }

    color.insert(1, strAlpha);
}

static inline bool IsAtomicServiceFreeInstall(const SessionInfo& sessionInfo)
{
    return sessionInfo.isAtomicService_ && sessionInfo.want != nullptr &&
        (sessionInfo.want->GetFlags() & AAFwk::Want::FLAG_INSTALL_ON_DEMAND) ==
        AAFwk::Want::FLAG_INSTALL_ON_DEMAND;
}

void SceneSessionManager::ConfigWindowEffect(const WindowSceneConfig::ConfigItem& effectConfig)
{
    AppWindowSceneConfig config;
    // config corner radius
    WindowSceneConfig::ConfigItem item = effectConfig["appWindows"]["cornerRadius"];
    if (item.IsMap()) {
        if (ConfigAppWindowCornerRadius(item["float"], config.floatCornerRadius_)) {
            appWindowSceneConfig_ = config;
        }
    }

    // config shadow
    item = effectConfig["appWindows"]["shadow"]["focused"];
    if (item.IsMap()) {
        if (ConfigAppWindowShadow(item, config.focusedShadow_)) {
            appWindowSceneConfig_.focusedShadow_ = config.focusedShadow_;
        }
    }

    item = effectConfig["appWindows"]["shadow"]["unfocused"];
    if (item.IsMap()) {
        if (ConfigAppWindowShadow(item, config.unfocusedShadow_)) {
            appWindowSceneConfig_.unfocusedShadow_ = config.unfocusedShadow_;
        }
    }

    AddAlphaToColor(appWindowSceneConfig_.focusedShadow_.alpha_, appWindowSceneConfig_.focusedShadow_.color_);
    AddAlphaToColor(appWindowSceneConfig_.unfocusedShadow_.alpha_, appWindowSceneConfig_.unfocusedShadow_.color_);

    WLOGFI("Config window effect successfully");
}

bool SceneSessionManager::ConfigAppWindowCornerRadius(const WindowSceneConfig::ConfigItem& item, float& out)
{
    std::map<std::string, float> stringToCornerRadius = {
        {"off", 0.0f}, {"defaultCornerRadiusXS", 4.0f}, {"defaultCornerRadiusS", 8.0f},
        {"defaultCornerRadiusM", 12.0f}, {"defaultCornerRadiusL", 16.0f}, {"defaultCornerRadiusXL", 24.0f}
    };

    if (item.IsString()) {
        auto value = item.stringValue_;
        if (stringToCornerRadius.find(value) != stringToCornerRadius.end()) {
            out = stringToCornerRadius[value];
            return true;
        }
    }
    return false;
}

void SceneSessionManager::SetEnableInputEvent(bool enabled)
{
    TLOGI(WmsLogTag::WMS_RECOVER, "enabled: %{public}u", enabled);
    enableInputEvent_ = enabled;
}

bool SceneSessionManager::IsInputEventEnabled()
{
    return enableInputEvent_;
}

void SceneSessionManager::ClearUnrecoveredSessions(const std::vector<int32_t>& recoveredPersistentIds)
{
    for (const auto& persistentId : alivePersistentIds_) {
        auto it = std::find(recoveredPersistentIds.begin(), recoveredPersistentIds.end(), persistentId);
        if (it != recoveredPersistentIds.end()) {
            continue;
        }
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_RECOVER, "Session is nullptr, persistentId = %{public}d", persistentId);
            continue;
        }
        if (sceneSession->IsRecovered()) {
            TLOGI(WmsLogTag::WMS_RECOVER, "persistentId=%{public}d", persistentId);
            std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            visibleWindowCountMap_.erase(sceneSession->GetCallingPid());
            sceneSessionMap_.erase(persistentId);
            MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession);
        }
    }
}

void SceneSessionManager::UpdateRecoveredSessionInfo(const std::vector<int32_t>& recoveredPersistentIds)
{
    TLOGI(WmsLogTag::WMS_RECOVER, "Number of persistentIds recovered = %{public}zu. CurrentUserId = %{public}d",
        recoveredPersistentIds.size(), currentUserId_);

    auto task = [this, recoveredPersistentIds]() {
        ClearUnrecoveredSessions(recoveredPersistentIds);
        std::list<AAFwk::SessionInfo> abilitySessionInfos;
        for (const auto& persistentId : recoveredPersistentIds) {
            if (failRecoveredPersistentIdSet_.count(persistentId)) {
                TLOGI(WmsLogTag::WMS_RECOVER, "failRecoveredPersistentId = %{public}d, continue", persistentId);
                continue;
            }
            auto sceneSession = GetSceneSession(persistentId);
            if (sceneSession == nullptr) {
                TLOGE(WmsLogTag::WMS_RECOVER, "Session is nullptr, persistentId = %{public}d", persistentId);
                continue;
            }
            const auto& abilitySessionInfo = SetAbilitySessionInfo(sceneSession);
            if (!abilitySessionInfo) {
                TLOGW(WmsLogTag::WMS_RECOVER, "abilitySessionInfo is null, persistentId = %{public}d", persistentId);
                continue;
            }
            TLOGD(WmsLogTag::WMS_RECOVER, "recovered persistentId = %{public}d", persistentId);
            abilitySessionInfos.emplace_back(*abilitySessionInfo);
        }
        std::vector<int32_t> unrecoverableSessionIds;
        AAFwk::AbilityManagerClient::GetInstance()->UpdateSessionInfoBySCB(
            abilitySessionInfos, currentUserId_, unrecoverableSessionIds);
        TLOGI(WmsLogTag::WMS_RECOVER, "Number of unrecoverableSessionIds = %{public}zu",
            unrecoverableSessionIds.size());
        for (const auto& sessionId : unrecoverableSessionIds) {
            auto sceneSession = GetSceneSession(sessionId);
            if (sceneSession == nullptr) {
                TLOGW(WmsLogTag::WMS_RECOVER, "There is no session corresponding to persistentId = %{public}d ",
                    sessionId);
                continue;
            }
            const auto& scnSessionInfo = SetAbilitySessionInfo(sceneSession);
            if (!scnSessionInfo) {
                TLOGW(WmsLogTag::WMS_RECOVER, "scnSessionInfo is null, persistentId = %{public}d", sessionId);
                continue;
            }
            TLOGI(WmsLogTag::WMS_RECOVER, "unrecoverable persistentId = %{public}d", sessionId);
            sceneSession->NotifySessionExceptionInner(scnSessionInfo, false);
        }
        removeFailRecoveredSession();
    };
    return taskScheduler_->PostAsyncTask(task, "UpdateSessionInfoBySCB");
}

bool SceneSessionManager::ConfigAppWindowShadow(const WindowSceneConfig::ConfigItem& shadowConfig,
    WindowShadowConfig& outShadow)
{
    WindowSceneConfig::ConfigItem item = shadowConfig["color"];
    if (item.IsString()) {
        auto color = item.stringValue_;
        uint32_t colorValue;
        if (!ColorParser::Parse(color, colorValue)) {
            return false;
        }
        outShadow.color_ = color;
    }

    item = shadowConfig["offsetX"];
    if (item.IsFloats()) {
        auto offsetX = *item.floatsValue_;
        if (offsetX.size() != 1) {
            return false;
        }
        outShadow.offsetX_ = offsetX[0];
    }

    item = shadowConfig["offsetY"];
    if (item.IsFloats()) {
        auto offsetY = *item.floatsValue_;
        if (offsetY.size() != 1) {
            return false;
        }
        outShadow.offsetY_ = offsetY[0];
    }

    item = shadowConfig["alpha"];
    if (item.IsFloats()) {
        auto alpha = *item.floatsValue_;
        if (alpha.size() != 1 ||
            (MathHelper::LessNotEqual(alpha[0], 0.0) && MathHelper::GreatNotEqual(alpha[0], 1.0))) {
            return false;
        }
        outShadow.alpha_ = alpha[0];
    }

    item = shadowConfig["radius"];
    if (item.IsFloats()) {
        auto radius = *item.floatsValue_;
        if (radius.size() != 1 || MathHelper::LessNotEqual(radius[0], 0.0)) {
            return false;
        }
        outShadow.radius_ = radius[0];
    }

    return true;
}

void SceneSessionManager::LoadKeyboardAnimation(const WindowSceneConfig::ConfigItem& item,
    KeyboardSceneAnimationConfig& config)
{
    if (item.IsMap() && item.mapValue_->count("curve")) {
        const auto& [curveType, curveParams] = CreateCurve(item["curve"]);
        config.curveType_ = curveType;
        if (curveParams.size() == CURVE_PARAM_DIMENSION) {
            config.ctrlX1_ = curveParams[0]; // 0: ctrl x1 index
            config.ctrlY1_ = curveParams[1]; // 1: ctrl y1 index
            config.ctrlX2_ = curveParams[2]; // 2: ctrl x2 index
            config.ctrlY2_ = curveParams[3]; // 3: ctrl y2 index
        }
    }

    const WindowSceneConfig::ConfigItem& duration = item["duration"];
    if (duration.IsInts()) {
        auto numbers = *duration.intsValue_;
        if (numbers.size() == 1) {
            config.duration_ = static_cast<uint32_t>(numbers[0]);
        }
    }
}

void SceneSessionManager::ConfigKeyboardAnimation(const WindowSceneConfig::ConfigItem& animationConfig)
{
    LoadKeyboardAnimation(animationConfig["animationIn"]["timing"], appWindowSceneConfig_.keyboardAnimationIn_);
    LoadKeyboardAnimation(animationConfig["animationOut"]["timing"], appWindowSceneConfig_.keyboardAnimationOut_);

    // config system animation
    const auto& appConfigIn = appWindowSceneConfig_.keyboardAnimationIn_;
    systemConfig_.animationIn_ = KeyboardAnimationCurve(appConfigIn.curveType_,
        {appConfigIn.ctrlX1_, appConfigIn.ctrlY1_, appConfigIn.ctrlX2_, appConfigIn.ctrlY2_},
        appConfigIn.duration_);
    const auto& appConfigOut = appWindowSceneConfig_.keyboardAnimationOut_;
    systemConfig_.animationOut_ = KeyboardAnimationCurve(appConfigOut.curveType_,
        {appConfigOut.ctrlX1_, appConfigOut.ctrlY1_, appConfigOut.ctrlX2_, appConfigOut.ctrlY2_},
        appConfigOut.duration_);
}

void SceneSessionManager::ConfigDefaultKeyboardAnimation(KeyboardSceneAnimationConfig& animationIn,
    KeyboardSceneAnimationConfig& animationOut)
{
    if (!(systemConfig_.animationIn_.curveType_.empty() && systemConfig_.animationOut_.curveType_.empty())) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "product config, curveIn:[%{public}s, %{public}u], "
            "curveOut:[%{public}s, %{public}u]", systemConfig_.animationIn_.curveType_.c_str(),
            systemConfig_.animationIn_.duration_, systemConfig_.animationOut_.curveType_.c_str(),
            systemConfig_.animationOut_.duration_);
        return;
    }

    // default animation curve params
    constexpr char CURVETYPE[] = "interpolatingSpring";
    constexpr float IN_CTRLX1 = 0.0f;
    constexpr float OUT_CTRLX1 = 4.0f;
    constexpr float CTRLY1 = 1.0f;
    constexpr float CTRLX2 = 342.0f;
    constexpr float CTRLY2 = 37.0f;
    constexpr uint32_t DURATION = 150;
    std::vector<float> in = { IN_CTRLX1, CTRLY1, CTRLX2, CTRLY2 };
    std::vector<float> out = { OUT_CTRLX1, CTRLY1, CTRLX2, CTRLY2 };

    // update system config for client
    systemConfig_.animationIn_ = KeyboardAnimationCurve(CURVETYPE, in, DURATION);
    systemConfig_.animationOut_ = KeyboardAnimationCurve(CURVETYPE, out, DURATION);

    // update app config for server
    animationIn.curveType_ = CURVETYPE;
    animationIn.ctrlX1_ = in[0]; // 0: ctrl x1 index
    animationIn.ctrlY1_ = in[1]; // 1: ctrl y1 index
    animationIn.ctrlX2_ = in[2]; // 2: ctrl x2 index
    animationIn.ctrlY2_ = in[3]; // 3: ctrl y2 index
    animationIn.duration_ = DURATION;

    animationOut.curveType_ = CURVETYPE;
    animationOut.ctrlX1_ = out[0]; // 0: ctrl x1 index
    animationOut.ctrlY1_ = out[1]; // 1: ctrl y1 index
    animationOut.ctrlX2_ = out[2]; // 2: ctrl x2 index
    animationOut.ctrlY2_ = out[3]; // 3: ctrl y2 index
    animationOut.duration_ = DURATION;
    TLOGI(WmsLogTag::WMS_KEYBOARD, "use default config");
}

void SceneSessionManager::ConfigWindowAnimation(const WindowSceneConfig::ConfigItem& windowAnimationConfig)
{
    WindowSceneConfig::ConfigItem item = windowAnimationConfig["timing"];
    if (item.IsMap() && item.mapValue_->count("curve")) {
        const auto& [curveType, curveParams] = CreateCurve(item["curve"]);
        appWindowSceneConfig_.windowAnimation_.curveType_ = curveType;
        if (curveParams.size() == CURVE_PARAM_DIMENSION) {
            appWindowSceneConfig_.windowAnimation_.ctrlX1_ = curveParams[0]; // 0: ctrl x1 index
            appWindowSceneConfig_.windowAnimation_.ctrlY1_ = curveParams[1]; // 1: ctrl y1 index
            appWindowSceneConfig_.windowAnimation_.ctrlX2_ = curveParams[2]; // 2: ctrl x2 index
            appWindowSceneConfig_.windowAnimation_.ctrlY2_ = curveParams[3]; // 3: ctrl y2 index
        }
    }
    item = windowAnimationConfig["timing"]["duration"];
    if (item.IsInts() && item.intsValue_->size() == 1) {
        auto duration = *item.intsValue_;
        appWindowSceneConfig_.windowAnimation_.duration_ = duration[0];
    }
    item = windowAnimationConfig["scale"];
    if (item.IsFloats() && item.floatsValue_->size() == SCALE_DIMENSION) {
        auto scales = *item.floatsValue_;
        appWindowSceneConfig_.windowAnimation_.scaleX_ = scales[0];
        appWindowSceneConfig_.windowAnimation_.scaleY_ = scales[1];
    }
    item = windowAnimationConfig["rotation"];
    if (item.IsFloats() && item.floatsValue_->size() == ROTAION_DIMENSION) {
        auto rotations = *item.floatsValue_;
        appWindowSceneConfig_.windowAnimation_.rotationX_ = rotations[0]; // 0 ctrlX1
        appWindowSceneConfig_.windowAnimation_.rotationY_ = rotations[1]; // 1 ctrlY1
        appWindowSceneConfig_.windowAnimation_.rotationZ_ = rotations[2]; // 2 ctrlX2
        appWindowSceneConfig_.windowAnimation_.angle_ = rotations[3]; // 3 ctrlY2
    }
    item = windowAnimationConfig["translate"];
    if (item.IsFloats() && item.floatsValue_->size() == TRANSLATE_DIMENSION) {
        auto translates = *item.floatsValue_;
        appWindowSceneConfig_.windowAnimation_.translateX_ = translates[0];
        appWindowSceneConfig_.windowAnimation_.translateY_ = translates[1];
    }
    item = windowAnimationConfig["opacity"];
    if (item.IsFloats() && item.floatsValue_->size() == 1) {
        auto opacity = *item.floatsValue_;
        appWindowSceneConfig_.windowAnimation_.opacity_ = opacity[0];
    }
}

void SceneSessionManager::ConfigStartingWindowAnimation(const WindowSceneConfig::ConfigItem& configItem)
{
    auto& config = appWindowSceneConfig_.startingWindowAnimationConfig_;
    auto item = configItem.GetProp("enable");
    if (item.IsBool()) {
        config.enabled_ = item.boolValue_;
    }
    item = configItem["timing"];
    if (item.IsMap() && item.mapValue_->count("curve")) {
        config.curve_ = std::get<std::string>(CreateCurve(item["curve"]));
    }
    item = configItem["timing"]["duration"];
    if (item.IsInts() && item.intsValue_->size() == 1) {
        config.duration_ = (*item.intsValue_)[0];
    }
    item = configItem["opacityStart"];
    if (item.IsFloats() && item.floatsValue_->size() == 1) {
        config.opacityStart_ = (*item.floatsValue_)[0];
    }
    item = configItem["opacityEnd"];
    if (item.IsFloats() && item.floatsValue_->size() == 1) {
        config.opacityEnd_ = (*item.floatsValue_)[0];
    }
}

std::tuple<std::string, std::vector<float>> SceneSessionManager::CreateCurve(
    const WindowSceneConfig::ConfigItem& curveConfig)
{
    static std::unordered_set<std::string> curveSet = { "easeOut", "ease", "easeIn", "easeInOut", "default",
        "linear", "spring", "interactiveSpring", "interpolatingSpring" };
    static std::unordered_set<std::string> paramCurveSet = {
        "spring", "interactiveSpring", "interpolatingSpring", "cubic" };

    std::string curveName = "easeOut";
    const auto& nameItem = curveConfig.GetProp("name");
    if (!nameItem.IsString()) {
        return {curveName, {}};
    }

    std::string name = nameItem.stringValue_;
    std::vector<float> curveParams;

    if (paramCurveSet.find(name) != paramCurveSet.end()) {
        curveName = name;
        curveParams = std::vector<float>(CURVE_PARAM_DIMENSION);
        if (curveConfig.IsFloats() && curveConfig.floatsValue_->size() <= CURVE_PARAM_DIMENSION) {
            std::copy(curveConfig.floatsValue_->begin(), curveConfig.floatsValue_->end(),
                curveParams.begin());
        }
    } else {
        auto iter = curveSet.find(name);
        if (iter != curveSet.end()) {
            curveName = name;
        }
    }

    return {curveName, curveParams};
}

void SceneSessionManager::ConfigWindowSizeLimits()
{
    const auto& config = WindowSceneConfig::GetConfig();
    WindowSceneConfig::ConfigItem item = config["mainWindowSizeLimits"];
    if (item.IsMap()) {
        ConfigMainWindowSizeLimits(item);
    }

    item = config["subWindowSizeLimits"];
    if (item.IsMap()) {
        ConfigSubWindowSizeLimits(item);
    }
}

void SceneSessionManager::ConfigMainWindowSizeLimits(const WindowSceneConfig::ConfigItem& mainWindowSizeConifg)
{
    auto item = mainWindowSizeConifg["miniWidth"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) {
            systemConfig_.miniWidthOfMainWindow_ = static_cast<uint32_t>(numbers[0]);
        }
    }

    item = mainWindowSizeConifg["miniHeight"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) {
            systemConfig_.miniHeightOfMainWindow_ = static_cast<uint32_t>(numbers[0]);
        }
    }
}

void SceneSessionManager::ConfigSubWindowSizeLimits(const WindowSceneConfig::ConfigItem& subWindowSizeConifg)
{
    auto item = subWindowSizeConifg["miniWidth"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) {
            systemConfig_.miniWidthOfSubWindow_ = static_cast<uint32_t>(numbers[0]);
        }
    }

    item = subWindowSizeConifg["miniHeight"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) {
            systemConfig_.miniHeightOfSubWindow_ = static_cast<uint32_t>(numbers[0]);
        }
    }
}

void SceneSessionManager::ConfigSnapshotScale()
{
    const auto& config = WindowSceneConfig::GetConfig();
    WindowSceneConfig::ConfigItem item = config["snapshotScale"];
    if (item.IsFloats()) {
        auto snapshotScale = *item.floatsValue_;
        if (snapshotScale.size() != 1 || snapshotScale[0] <= 0 || snapshotScale[0] > 1) {
            return;
        }
        snapshotScale_ = snapshotScale[0];
    }
}

void SceneSessionManager::ConfigSystemUIStatusBar(const WindowSceneConfig::ConfigItem& statusBarConfig)
{
    TLOGI(WmsLogTag::WMS_IMMS, "load ConfigSystemUIStatusBar");
    WindowSceneConfig::ConfigItem item = statusBarConfig["showInLandscapeMode"];
    if (item.IsInts() && item.intsValue_->size() == 1) {
        bool showInLandscapeMode = (*item.intsValue_)[0] > 0;
        appWindowSceneConfig_.systemUIStatusBarConfig_.showInLandscapeMode_ = showInLandscapeMode;
        TLOGI(WmsLogTag::WMS_IMMS, "ConfigSystemUIStatusBar showInLandscapeMode:%{public}d",
            appWindowSceneConfig_.systemUIStatusBarConfig_.showInLandscapeMode_);
    }

    item = statusBarConfig["immersiveStatusBarBgColor"];
    if (item.IsString()) {
        auto color = item.stringValue_;
        uint32_t colorValue;
        if (!ColorParser::Parse(color, colorValue)) {
            return;
        }
        appWindowSceneConfig_.systemUIStatusBarConfig_.immersiveStatusBarBgColor_ = color;
        TLOGI(WmsLogTag::WMS_IMMS, "ConfigSystemUIStatusBar immersiveStatusBarBgColor:%{public}s",
            appWindowSceneConfig_.systemUIStatusBarConfig_.immersiveStatusBarBgColor_.c_str());
    }

    item = statusBarConfig["immersiveStatusBarContentColor"];
    if (item.IsString()) {
        auto color = item.stringValue_;
        uint32_t colorValue;
        if (!ColorParser::Parse(color, colorValue)) {
            return;
        }
        appWindowSceneConfig_.systemUIStatusBarConfig_.immersiveStatusBarContentColor_ = color;
        TLOGI(WmsLogTag::WMS_IMMS, "ConfigSystemUIStatusBar immersiveStatusBarContentColor:%{public}s",
            appWindowSceneConfig_.systemUIStatusBarConfig_.immersiveStatusBarContentColor_.c_str());
    }
}

void SceneSessionManager::SetRootSceneContext(const std::weak_ptr<AbilityRuntime::Context>& contextWeak)
{
    rootSceneContextWeak_ = contextWeak;
}

sptr<RootSceneSession> SceneSessionManager::GetRootSceneSession()
{
    auto task = [this] {
        if (rootSceneSession_ != nullptr) {
            return rootSceneSession_;
        }
        system::SetParameter("bootevent.wms.fullscreen.ready", "true");
        rootSceneSession_ = sptr<RootSceneSession>::MakeSptr();
        rootSceneSession_->SetEventHandler(taskScheduler_->GetEventHandler());
        AAFwk::AbilityManagerClient::GetInstance()->SetRootSceneSession(rootSceneSession_->AsObject());
        return rootSceneSession_;
    };
    return taskScheduler_->PostSyncTask(task, "GetRootSceneSession");
}

WSRect SceneSessionManager::GetRootSessionAvoidSessionRect(AvoidAreaType type)
{
    sptr<RootSceneSession> rootSession = GetRootSceneSession();
    if (rootSession == nullptr || rootSession->GetSessionProperty() == nullptr) {
        return {};
    }
    DisplayId displayId = rootSession->GetSessionProperty()->GetDisplayId();
    std::vector<sptr<SceneSession>> sessionVector;
    switch (type) {
        case AvoidAreaType::TYPE_SYSTEM: {
            sessionVector = GetSceneSessionVectorByType(WindowType::WINDOW_TYPE_STATUS_BAR, displayId);
            break;
        }
        case AvoidAreaType::TYPE_KEYBOARD: {
            sessionVector = GetSceneSessionVectorByType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL, displayId);
            break;
        }
        default: {
            TLOGD(WmsLogTag::WMS_IMMS, "unsupported type %{public}u", type);
            return {};
        }
    }

    for (auto& session : sessionVector) {
        if (!session->IsVisible()) {
            continue;
        }
        const WSRect rect = session->GetSessionRect();
        TLOGI(WmsLogTag::WMS_IMMS, "type: %{public}u, rect: %{public}s", type, rect.ToString().c_str());
        return rect;
    }
    return {};
}

sptr<SceneSession> SceneSessionManager::GetSceneSession(int32_t persistentId)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    auto iter = sceneSessionMap_.find(persistentId);
    if (iter == sceneSessionMap_.end()) {
        WLOGFD("Error found scene session with id: %{public}d", persistentId);
        return nullptr;
    }
    return iter->second;
}

sptr<SceneSession> SceneSessionManager::GetSceneSessionByName(const ComparedSessionInfo& info)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession->GetSessionInfo().bundleName_ != info.bundleName_ ||
            sceneSession->GetSessionInfo().appIndex_ != info.appIndex_ ||
            sceneSession->GetSessionInfo().appInstanceKey_ != info.instanceKey_ ||
            sceneSession->GetSessionInfo().windowType_ != info.windowType_) {
            continue;
        }
        if (info.isAtomicService_) {
            if ((sceneSession->GetSessionInfo().moduleName_.empty() || sceneSession->GetSessionInfo().moduleName_ == info.moduleName_) &&
                (sceneSession->GetSessionInfo().abilityName_.empty() || sceneSession->GetSessionInfo().abilityName_ == info.abilityName_)){
                return sceneSession;
            }
        } else if (sceneSession->GetSessionInfo().moduleName_ == info.moduleName_ &&
            sceneSession->GetSessionInfo().abilityName_ == info.abilityName_) {
            return sceneSession;
        }
    }
    return nullptr;
}

sptr<SceneSession> SceneSessionManager::GetSceneSessionByType(WindowType type)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& [_, sceneSession] : sceneSessionMap_) {
        if (sceneSession && sceneSession->GetWindowType() == type) {
            return sceneSession;
        }
    }
    return nullptr;
}

std::vector<sptr<SceneSession>> SceneSessionManager::GetSceneSessionVectorByType(
    WindowType type, uint64_t displayId)
{
    if (displayId == DISPLAY_ID_INVALID) {
        TLOGE(WmsLogTag::WMS_LIFE, "displayId is invalid");
        return {};
    }
    std::vector<sptr<SceneSession>> sceneSessionVector;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession->GetWindowType() == type &&
            sceneSession->GetSessionProperty() &&
            sceneSession->GetSessionProperty()->GetDisplayId() == displayId) {
            sceneSessionVector.emplace_back(sceneSession);
        }
    }

    return sceneSessionVector;
}

WSError SceneSessionManager::UpdateParentSessionForDialog(const sptr<SceneSession>& sceneSession,
    sptr<WindowSessionProperty> property)
{
    if (property == nullptr) {
        TLOGD(WmsLogTag::WMS_DIALOG, "Property is null, no need to update parent info");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "Session is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto parentPersistentId = property->GetParentPersistentId();
    sceneSession->SetParentPersistentId(parentPersistentId);
    if (property->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG && parentPersistentId != INVALID_SESSION_ID) {
        auto parentSession = GetSceneSession(parentPersistentId);
        if (parentSession == nullptr) {
            TLOGE(WmsLogTag::WMS_DIALOG, "Parent session is nullptr, parentId:%{public}d", parentPersistentId);
            return WSError::WS_ERROR_NULLPTR;
        }
        parentSession->BindDialogSessionTarget(sceneSession);
        parentSession->BindDialogToParentSession(sceneSession);
        sceneSession->SetParentSession(parentSession);
        TLOGI(WmsLogTag::WMS_DIALOG, "Update parent of dialog success, id %{public}d, parentId %{public}d",
            sceneSession->GetPersistentId(), parentPersistentId);
    }
    return WSError::WS_OK;
}

sptr<SceneSession::SpecificSessionCallback> SceneSessionManager::CreateSpecificSessionCallback()
{
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    specificCb->onCreate_ = [this](const SessionInfo& sessionInfo, sptr<WindowSessionProperty> property) {
        return this->RequestSceneSession(sessionInfo, property);
    };
    specificCb->onDestroy_ = [this](const int32_t persistentId) {
        return this->DestroyAndDisconnectSpecificSessionInner(persistentId);
    };
    specificCb->onClearDisplayStatusBarTemporarilyFlags_ = [this] {
        this->ClearDisplayStatusBarTemporarilyFlags();
    };
    specificCb->onCameraFloatSessionChange_ = [this](uint32_t accessTokenId, bool isShowing) {
        this->UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
    };
    specificCb->onGetSceneSessionVectorByType_ = [this](WindowType type, uint64_t displayId) {
        return this->GetSceneSessionVectorByType(type, displayId);
    };
    specificCb->onUpdateAvoidArea_ = [this](const int32_t persistentId) {
        this->UpdateAvoidArea(persistentId);
    };
    specificCb->onUpdateOccupiedAreaIfNeed_ = [this](const int32_t& persistentId) {
        this->UpdateOccupiedAreaIfNeed(persistentId);
    };
    specificCb->onWindowInfoUpdate_ = [this](int32_t persistentId, WindowUpdateType type) {
        this->NotifyWindowInfoChange(persistentId, type);
    };
    specificCb->onWindowInputPidChangeCallback_ = [this](int32_t windowId, bool startMoving) {
        this->NotifyMMIWindowPidChange(windowId, startMoving);
    };
    specificCb->onSessionTouchOutside_ = [this](int32_t persistentId) {
        this->NotifySessionTouchOutside(persistentId);
    };
    specificCb->onGetAINavigationBarArea_ = [this](uint64_t displayId) {
        return this->GetAINavigationBarArea(displayId);
    };
    specificCb->onOutsideDownEvent_ = [this](int32_t x, int32_t y) {
        this->OnOutsideDownEvent(x, y);
    };
    specificCb->onHandleSecureSessionShouldHide_ = [this](const sptr<SceneSession>& sceneSession) {
        return this->HandleSecureSessionShouldHide(sceneSession);
    };
    specificCb->onCameraSessionChange_ = [this](uint32_t accessTokenId, bool isShowing) {
        this->UpdateCameraWindowStatus(accessTokenId, isShowing);
    };
    specificCb->onSetSkipSelfWhenShowOnVirtualScreen_ = [this](uint64_t surfaceNodeId, bool isSkip) {
        this->SetSkipSelfWhenShowOnVirtualScreen(surfaceNodeId, isSkip);
    };
    specificCb->onPiPStateChange_ = [this](const std::string& bundleName, bool isForeground) {
        this->UpdatePiPWindowStateChanged(bundleName, isForeground);
    };
    return specificCb;
}

void SceneSessionManager::SetSkipSelfWhenShowOnVirtualScreen(uint64_t surfaceNodeId, bool isSkip)
{
    TLOGI(WmsLogTag::WMS_SCB, "surfaceNodeId: %{public}" PRIu64, surfaceNodeId);
    auto it = std::find(skipSurfaceNodeIds_.begin(), skipSurfaceNodeIds_.end(), surfaceNodeId);
    if (isSkip) {
        if (it == skipSurfaceNodeIds_.end()) {
            skipSurfaceNodeIds_.push_back(surfaceNodeId);
        } else {
            return;
        }
    } else {
        if (it != skipSurfaceNodeIds_.end()) {
            skipSurfaceNodeIds_.erase(it);
        } else {
            return;
        }
    }
    rsInterface_.SetVirtualScreenBlackList(INVALID_SCREEN_ID, skipSurfaceNodeIds_);
}

sptr<KeyboardSession::KeyboardSessionCallback> SceneSessionManager::CreateKeyboardSessionCallback()
{
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    keyboardCb->onGetSceneSession_ = [this](int32_t persistentId) {
        return this->GetSceneSession(persistentId);
    };
    keyboardCb->onGetFocusedSessionId_ = [this] {
        return this->GetFocusedSessionId();
    };
    keyboardCb->onCallingSessionIdChange_ = callingSessionIdChangeFunc_;
    return keyboardCb;
}

WMError SceneSessionManager::CheckWindowId(int32_t windowId, int32_t& pid)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_EVENT, "CheckWindowId permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }

    auto task = [this, windowId, &pid]() -> WMError {
        pid = INVALID_PID;
        auto sceneSession = GetSceneSession(windowId);
        if (sceneSession == nullptr) {
            WLOGFE("sceneSession(%{public}d) is nullptr", windowId);
            return WMError::WM_ERROR_INVALID_WINDOW;
        }
        pid = sceneSession->GetCallingPid();
        WLOGFD("Window(%{public}d) to set the cursor style, pid:%{public}d", windowId, pid);
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "CheckWindowId:" + std::to_string(windowId));
}

void SceneSessionManager::CreateKeyboardPanelSession(sptr<SceneSession> keyboardSession)
{
    if (!isKeyboardPanelEnabled_) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "keyboardPanel is not enabled");
        return;
    }
    if (keyboardSession == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "keyboardSession is nullptr");
        return;
    }
    auto sessionProperty = keyboardSession->GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "sessionProperty is null");
        return;
    }
    DisplayId displayId = sessionProperty->GetDisplayId();
    const auto& panelVec = GetSceneSessionVectorByType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL, displayId);
    sptr<SceneSession> panelSession;
    if (panelVec.size() > 1) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Error size of keyboardPanel, size: %{public}zu", panelVec.size());
        return;
    } else if (panelVec.size() == 1) {
        panelSession = panelVec.front();
        TLOGI(WmsLogTag::WMS_KEYBOARD, "keyboardPanel is created, panelId:%{public}d", panelSession->GetPersistentId());
    } else {
        SessionInfo panelInfo = {
            .bundleName_ = "SCBKeyboardPanel",
            .moduleName_ = "SCBKeyboardPanel",
            .abilityName_ = "SCBKeyboardPanel",
            .isSystem_ = true,
            .sceneType_ = SceneType::SYSTEM_WINDOW_SCENE,
            .windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_KEYBOARD_PANEL),
            .screenId_ = static_cast<uint64_t>(displayId),
            .isRotable_ = true,
        };
        if (systemConfig_.IsPcWindow()) {
            panelInfo.sceneType_ = SceneType::INPUT_SCENE;
            TLOGI(WmsLogTag::WMS_KEYBOARD, "Set panel canvasNode");
        } else {
            TLOGI(WmsLogTag::WMS_KEYBOARD, "Set panel surfaceNode");
        }
        panelSession = RequestSceneSession(panelInfo, nullptr);
        if (panelSession == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "PanelSession is nullptr");
            return;
        }
    }
    keyboardSession->BindKeyboardPanelSession(panelSession);
    panelSession->BindKeyboardSession(keyboardSession);
    TLOGI(WmsLogTag::WMS_KEYBOARD, "success, panelId:%{public}d, keyboardId:%{public}d",
        panelSession->GetPersistentId(), keyboardSession->GetPersistentId());
}

sptr<SceneSession> SceneSessionManager::CreateSceneSession(const SessionInfo& sessionInfo,
    sptr<WindowSessionProperty> property)
{
    sptr<SceneSession::SpecificSessionCallback> specificCb = CreateSpecificSessionCallback();
    sptr<SceneSession> sceneSession = nullptr;
    if (sessionInfo.isSystem_) {
        sceneSession = new SCBSystemSession(sessionInfo, specificCb);
        WLOGFI("[WMSSCB]Create SCBSystemSession, type: %{public}d", sessionInfo.windowType_);
    } else if (property == nullptr && SessionHelper::IsMainWindow(static_cast<WindowType>(sessionInfo.windowType_))) {
        sceneSession = new MainSession(sessionInfo, specificCb);
        TLOGI(WmsLogTag::WMS_MAIN, "Create MainSession, id: %{public}d", sceneSession->GetPersistentId());
    } else if (property != nullptr && SessionHelper::IsSubWindow(property->GetWindowType())) {
        sceneSession = new SubSession(sessionInfo, specificCb);
        TLOGI(WmsLogTag::WMS_SUB, "Create SubSession, type: %{public}d", property->GetWindowType());
    } else if (property != nullptr && property->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb = CreateKeyboardSessionCallback();
        sceneSession = new KeyboardSession(sessionInfo, specificCb, keyboardCb);
        CreateKeyboardPanelSession(sceneSession);
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Create KeyboardSession, type: %{public}d", property->GetWindowType());
    } else if (property != nullptr && SessionHelper::IsSystemWindow(property->GetWindowType())) {
        sceneSession = new SystemSession(sessionInfo, specificCb);
        TLOGI(WmsLogTag::WMS_SYSTEM, "Create SystemSession, type: %{public}d", property->GetWindowType());
    } else {
        TLOGE(WmsLogTag::WMS_LIFE, "Invalid window type");
    }
    if (sceneSession != nullptr) {
        sceneSession->SetSessionInfoPersistentId(sceneSession->GetPersistentId());
        sceneSession->isKeyboardPanelEnabled_ = isKeyboardPanelEnabled_;
        sceneSession->RegisterForceSplitListener([this](const std::string& bundleName) {
            return this->GetAppForceLandscapeConfig(bundleName);
        });
        sceneSession->SetUpdatePrivateStateAndNotifyFunc([this](int32_t persistentId) {
            this->UpdatePrivateStateAndNotify(persistentId);
        });
        if (sceneSession->moveDragController_) {
            sceneSession->moveDragController_->SetIsPcWindow(systemConfig_.IsPcWindow());
        }
    }
    return sceneSession;
}

sptr<SceneSession> SceneSessionManager::RequestSceneSession(const SessionInfo& sessionInfo,
    sptr<WindowSessionProperty> property)
{
    if (sessionInfo.persistentId_ != 0 && !sessionInfo.isPersistentRecover_) {
        auto session = GetSceneSession(sessionInfo.persistentId_);
        if (session != nullptr) {
            NotifySessionUpdate(sessionInfo, ActionType::SINGLE_START);
            TLOGD(WmsLogTag::WMS_LIFE, "get exist session persistentId: %{public}d", sessionInfo.persistentId_);
            return session;
        }
        if (WindowHelper::IsMainWindow(static_cast<WindowType>(sessionInfo.windowType_))) {
            TLOGD(WmsLogTag::WMS_LIFE, "mainWindow bundleName: %{public}s, moduleName: %{public}s, "
                "abilityName: %{public}s, appIndex: %{public}d",
                sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(),
                sessionInfo.abilityName_.c_str(), sessionInfo.appIndex_);
            ComparedSessionInfo compareSessionInfo = { sessionInfo.bundleName_, sessionInfo.moduleName_, sessionInfo.abilityName_,
                sessionInfo.appIndex_, sessionInfo.appInstanceKey_, sessionInfo.windowType_, sessionInfo.isAtomicService_ };
            auto sceneSession = GetSceneSessionByName(compareSessionInfo);
            bool isSingleStart = sceneSession && sceneSession->GetAbilityInfo() &&
                sceneSession->GetAbilityInfo()->launchMode == AppExecFwk::LaunchMode::SINGLETON;
            if (isSingleStart) {
                NotifySessionUpdate(sessionInfo, ActionType::SINGLE_START);
                TLOGD(WmsLogTag::WMS_LIFE, "get exist singleton session persistentId: %{public}d",
                    sessionInfo.persistentId_);
                return sceneSession;
            }
        }
    }

    const char* const where = __func__;
    auto task = [this, sessionInfo, property, where] {
        TLOGI(WmsLogTag::WMS_LIFE, "RequestSceneSession, appName: [%{public}s %{public}s %{public}s]"
            "appIndex %{public}d, type %{public}u system %{public}u, isPersistentRecover %{public}u",
            sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(),
            sessionInfo.abilityName_.c_str(), sessionInfo.appIndex_, sessionInfo.windowType_,
            static_cast<uint32_t>(sessionInfo.isSystem_), static_cast<uint32_t>(sessionInfo.isPersistentRecover_));
        sptr<SceneSession> sceneSession = CreateSceneSession(sessionInfo, property);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "sceneSession is nullptr!");
            return sceneSession;
        }
        if (systemConfig_.IsPcWindow()) {
            MultiInstanceManager::GetInstance().FillInstanceKeyIfNeed(sceneSession);
        }
        InitSceneSession(sceneSession, sessionInfo, property);
        if (CheckCollaboratorType(sceneSession->GetCollaboratorType())) {
            TLOGNI(WmsLogTag::WMS_LIFE, "%{public}s: ancoSceneState: %{public}d",
                where, sceneSession->GetSessionInfo().ancoSceneState);
            PreHandleCollaborator(sceneSession);
            const auto& sessionAffinity = sceneSession->GetSessionInfo().sessionAffinity;
            if (auto reusedSceneSession = SceneSessionManager::GetInstance().FindSessionByAffinity(sessionAffinity)) {
                TLOGNI(WmsLogTag::WMS_LIFE,
                    "%{public}s: session reuse id:%{public}d type:%{public}d affinity:%{public}s",
                    where, reusedSceneSession->GetPersistentId(),
                    reusedSceneSession->GetWindowType(), sessionAffinity.c_str());
                NotifySessionUpdate(reusedSceneSession->GetSessionInfo(), ActionType::SINGLE_START);
                return reusedSceneSession;
            }
        }
        {
            std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            sceneSessionMap_.insert({ sceneSession->GetPersistentId(), sceneSession });
        }
        MultiInstanceManager::GetInstance().IncreaseInstanceKeyRefCount(sceneSession);
        PerformRegisterInRequestSceneSession(sceneSession);
        NotifySessionUpdate(sessionInfo, ActionType::SINGLE_START);
        TLOGI(WmsLogTag::WMS_LIFE, "RequestSceneSession id: %{public}d, type: %{public}d",
            sceneSession->GetPersistentId(), sceneSession->GetWindowType());
        return sceneSession;
    };
    return taskScheduler_->PostSyncTask(task, "RequestSceneSession:PID" + std::to_string(sessionInfo.persistentId_));
}

void SceneSessionManager::InitSceneSession(sptr<SceneSession>& sceneSession, const SessionInfo& sessionInfo,
    const sptr<WindowSessionProperty>& property)
{
    auto callerSession = GetSceneSession(sessionInfo.callerPersistentId_);
    DisplayId curDisplayId = DISPLAY_ID_INVALID;
    if (sessionInfo.screenId_ != SCREEN_ID_INVALID) {
        curDisplayId = sessionInfo.screenId_;
    } else if (callerSession) {
        auto callerSessionProperty = callerSession->GetSessionProperty();
        if (callerSessionProperty) {
            curDisplayId = callerSessionProperty->GetDisplayId();
        }
    }
    auto sessionProperty = sceneSession->GetSessionProperty();
    if (sessionProperty) {
        sessionProperty->SetDisplayId(curDisplayId);
        sceneSession->SetScreenId(curDisplayId);
        TLOGD(WmsLogTag::WMS_LIFE, "synchronous screenId with displayid %{public}" PRIu64,
            curDisplayId);
    }
    sceneSession->SetEventHandler(taskScheduler_->GetEventHandler(), eventHandler_);
    sceneSession->RegisterIsScreenLockedCallback([this] { return IsScreenLocked(); });
    if (sessionInfo.isSystem_) {
        sceneSession->SetCallingPid(IPCSkeleton::GetCallingRealPid());
        sceneSession->SetCallingUid(IPCSkeleton::GetCallingUid());
        auto rootContext = rootSceneContextWeak_.lock();
        sceneSession->SetAbilityToken(rootContext != nullptr ? rootContext->GetToken() : nullptr);
    } else {
        TLOGD(WmsLogTag::WMS_LIFE, "id: %{public}d, bundleName: %{public}s, "
            "moduleName: %{public}s, abilityName: %{public}s want: %{public}s", sceneSession->GetPersistentId(),
            sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str(),
            sessionInfo.want == nullptr ? "nullptr" : sessionInfo.want->ToString().c_str());
    }
    RegisterSessionExceptionFunc(sceneSession);
    RegisterVisibilityChangedDetectFunc(sceneSession);
    // Skip FillSessionInfo when atomicService free-install start.
    if (!IsAtomicServiceFreeInstall(sessionInfo)) {
        FillSessionInfo(sceneSession);
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSession(%d )", sceneSession->GetPersistentId());
    if (WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
        WindowInfoReporter::GetInstance().InsertCreateReportInfo(sessionInfo.bundleName_);
    }
    if (property != nullptr && WindowHelper::IsPipWindow(property->GetWindowType())) {
        sceneSession->SetPiPTemplateInfo(property->GetPiPTemplateInfo());
    }
    sceneSession->SetSystemConfig(systemConfig_);
    sceneSession->SetSnapshotScale(snapshotScale_);
    UpdateParentSessionForDialog(sceneSession, property);
}

void SceneSessionManager::NotifySessionUpdate(const SessionInfo& sessionInfo, ActionType action, ScreenId fromScreenId)
{
    auto task = [this, abilityName = sessionInfo.abilityName_,
        bundleName = sessionInfo.bundleName_, toScreenId = sessionInfo.screenId_, action, fromScreenId]() {
        sptr<DisplayChangeInfo> info = sptr<DisplayChangeInfo>::MakeSptr();
        info->action_ = action;
        info->abilityName_ = std::move(abilityName);
        info->bundleName_ = std::move(bundleName);
        info->toScreenId_ = std::move(toScreenId);
        info->fromScreenId_ = fromScreenId;
        ScreenSessionManagerClient::GetInstance().NotifyDisplayChangeInfoChanged(info);
        WLOGFI("Notify ability %{public}s bundle %{public}s update,toScreen id: %{public}" PRIu64 ".",
            info->abilityName_.c_str(), info->bundleName_.c_str(), info->toScreenId_);
    };
    taskScheduler_->PostAsyncTask(task, "NotifySessionUpdate");
}

void SceneSessionManager::PerformRegisterInRequestSceneSession(sptr<SceneSession>& sceneSession)
{
    RegisterSessionSnapshotFunc(sceneSession);
    RegisterSessionStateChangeNotifyManagerFunc(sceneSession);
    RegisterSessionInfoChangeNotifyManagerFunc(sceneSession);
    RegisterRequestFocusStatusNotifyManagerFunc(sceneSession);
    RegisterGetStateFromManagerFunc(sceneSession);
    RegisterSessionChangeByActionNotifyManagerFunc(sceneSession);
    RegisterAcquireRotateAnimationConfigFunc(sceneSession);
}

void SceneSessionManager::UpdateSceneSessionWant(const SessionInfo& sessionInfo)
{
    if (sessionInfo.persistentId_ != 0) {
        auto session = GetSceneSession(sessionInfo.persistentId_);
        if (session != nullptr && sessionInfo.want != nullptr) {
            TLOGI(WmsLogTag::WMS_MAIN, "Got session id:%{public}d", sessionInfo.persistentId_);
            if (!CheckCollaboratorType(session->GetCollaboratorType())) {
                session->SetSessionInfoWant(sessionInfo.want);
                TLOGI(WmsLogTag::WMS_MAIN, "Want updated, id:%{public}d", sessionInfo.persistentId_);
            } else {
                UpdateCollaboratorSessionWant(session, sessionInfo.persistentId_);
            }
        } else {
            TLOGI(WmsLogTag::WMS_MAIN, "Got session fail(%{public}d), id:%{public}d",
                session == nullptr, sessionInfo.persistentId_);
        }
    } else {
        TLOGI(WmsLogTag::WMS_MAIN, "sessionInfo.Id == 0");
    }
}

void SceneSessionManager::UpdateCollaboratorSessionWant(sptr<SceneSession>& session, int32_t persistentId)
{
    if (session != nullptr) {
        if (session->GetSessionInfo().ancoSceneState < AncoSceneState::NOTIFY_CREATE) {
            FillSessionInfo(session);
            if (CheckCollaboratorType(session->GetCollaboratorType())) {
                PreHandleCollaborator(session, persistentId);
            }
        }
    }
}

sptr<AAFwk::SessionInfo> SceneSessionManager::SetAbilitySessionInfo(const sptr<SceneSession>& scnSession)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = sptr<AAFwk::SessionInfo>::MakeSptr();
    auto sessionInfo = scnSession->GetSessionInfo();
    sptr<ISession> iSession(scnSession);
    abilitySessionInfo->sessionToken = iSession->AsObject();
    abilitySessionInfo->identityToken = std::to_string(std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count());
    abilitySessionInfo->callerToken = sessionInfo.callerToken_;
    abilitySessionInfo->sessionName = SessionUtils::ConvertSessionName(sessionInfo.bundleName_,
        sessionInfo.abilityName_, sessionInfo.moduleName_, sessionInfo.appIndex_);
    abilitySessionInfo->persistentId = scnSession->GetPersistentId();
    abilitySessionInfo->requestCode = sessionInfo.requestCode;
    abilitySessionInfo->resultCode = sessionInfo.resultCode;
    abilitySessionInfo->uiAbilityId = sessionInfo.uiAbilityId_;
    abilitySessionInfo->startSetting = sessionInfo.startSetting;
    abilitySessionInfo->callingTokenId = sessionInfo.callingTokenId_;
    abilitySessionInfo->userId = currentUserId_;
    abilitySessionInfo->isClearSession = sessionInfo.isClearSession;
    abilitySessionInfo->processOptions = sessionInfo.processOptions;
    if (sessionInfo.want != nullptr) {
        abilitySessionInfo->want = *sessionInfo.want;
    } else {
        abilitySessionInfo->want.SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_,
            sessionInfo.moduleName_);
    }
    int appIndex = abilitySessionInfo->want.GetIntParam(AAFwk::Want::PARAM_APP_CLONE_INDEX_KEY, 0);
    bool hasAppIndex = abilitySessionInfo->want.HasParameter(AAFwk::Want::PARAM_APP_CLONE_INDEX_KEY);
    if (hasAppIndex && sessionInfo.appIndex_ != appIndex) {
        TLOGW(WmsLogTag::WMS_LIFE, "appIndex not match want.appIndex:%{public}d, sessionInfo.appIndex_:%{public}d",
            appIndex, sessionInfo.appIndex_);
    }
    if (!hasAppIndex && sessionInfo.appIndex_ > 0) {
        TLOGI(WmsLogTag::WMS_LIFE, "want.appIndex is null, set want.appIndex:%{public}d", sessionInfo.appIndex_);
        abilitySessionInfo->want.SetParam(AAFwk::Want::PARAM_APP_CLONE_INDEX_KEY, sessionInfo.appIndex_);
    }
    auto sessionProperty = scnSession->GetSessionProperty();
    if (sessionProperty) {
        abilitySessionInfo->want.SetParam(AAFwk::Want::PARAM_RESV_DISPLAY_ID,
            static_cast<int>(sessionProperty->GetDisplayId()));
    }
    abilitySessionInfo->instanceKey = sessionInfo.appInstanceKey_;
    return abilitySessionInfo;
}

WSError SceneSessionManager::PrepareTerminate(int32_t persistentId, bool& isPrepareTerminate)
{
    auto task = [this, persistentId, &isPrepareTerminate]() {
        if (!isPrepareTerminateEnable_) { // not support prepareTerminate
            isPrepareTerminate = false;
            WLOGE("not support prepareTerminate, Id:%{public}d", persistentId);
            return WSError::WS_OK;
        }
        auto scnSession = GetSceneSession(persistentId);
        if (scnSession == nullptr) {
            WLOGFE("PrepareTerminate scnSession is null, Id:%{public}d", persistentId);
            isPrepareTerminate = false;
            return WSError::WS_ERROR_NULLPTR;
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (scnSessionInfo == nullptr) {
            WLOGFE("PrepareTerminate scnSessionInfo is null, Id:%{public}d", persistentId);
            isPrepareTerminate = false;
            return WSError::WS_ERROR_NULLPTR;
        }
        auto errorCode = AAFwk::AbilityManagerClient::GetInstance()->
            PrepareTerminateAbilityBySCB(scnSessionInfo, isPrepareTerminate);
        TLOGI(WmsLogTag::WMS_MAIN, "PrepareTerminateAbilityBySCB Id:%{public}d isPrepareTerminate:%{public}d "
            "errorCode:%{public}d", persistentId, isPrepareTerminate, errorCode);
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "PrepareTerminate:PID:" + std::to_string(persistentId));
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession, bool isNewActive)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, isNewActive]() {
        sptr<SceneSession> scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "Request active session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        if (!Session::IsScbCoreEnabled()) {
            scnSession->SetForegroundInteractiveStatus(true);
        }
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSessionActivation(%d )", persistentId);
        TLOGI(WmsLogTag::WMS_MAIN, "Request active id:%{public}d system:%{public}u isNewActive:%{public}d",
            persistentId, static_cast<uint32_t>(scnSession->GetSessionInfo().isSystem_), isNewActive);
        if (!GetSceneSession(persistentId)) {
            TLOGE(WmsLogTag::WMS_MAIN, "Request active session invalid by %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto ret = RequestSceneSessionActivationInner(scnSession, isNewActive);
        if (ret == WSError::WS_OK) {
            scnSession->SetExitSplitOnBackground(false);
        }
        scnSession->RemoveLifeCycleTask(LifeCycleTaskType::START);
        abilityInfoMap_.clear(); // clear cache after terminate
        return ret;
    };
    std::string taskName = "RequestSceneSessionActivation:PID:" +
        (sceneSession != nullptr ? std::to_string(sceneSession->GetPersistentId()) : "nullptr");
    taskScheduler_->PostAsyncTask(task, taskName);
    return WSError::WS_OK;
}

bool SceneSessionManager::IsKeyboardForeground()
{
    bool isKeyboardForeground = false;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession != nullptr && sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
            isKeyboardForeground = sceneSession->IsSessionForeground();
            break;
        }
    }

    return isKeyboardForeground;
}

void SceneSessionManager::RequestInputMethodCloseKeyboard(const int32_t persistentId)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session is nullptr");
        return;
    }
    // Hide keyboard when app is cold started, if keyboard is showing and screen is unlocked.
    if (!sceneSession->IsSessionValid() && IsKeyboardForeground() &&
        !sceneSession->GetStateFromManager(ManagerState::MANAGER_STATE_SCREEN_LOCKED)) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "Session is invalid, id: %{public}d state: %{public}u",
            persistentId, sceneSession->GetSessionState());
        sceneSession->RequestHideKeyboard(true);
    }
}

int32_t SceneSessionManager::StartUIAbilityBySCB(sptr<SceneSession>& scnSession)
{
    auto abilitySessionInfo = SetAbilitySessionInfo(scnSession);
    if (abilitySessionInfo == nullptr) {
        return ERR_NULL_OBJECT;
    }
    return StartUIAbilityBySCB(abilitySessionInfo);
}

int32_t SceneSessionManager::StartUIAbilityBySCB(sptr<AAFwk::SessionInfo>& abilitySessionInfo)
{
    bool isColdStart = false;
    return AAFwk::AbilityManagerClient::GetInstance()->StartUIAbilityBySCB(abilitySessionInfo, isColdStart);
}

int32_t SceneSessionManager::ChangeUIAbilityVisibilityBySCB(sptr<SceneSession>& scnSession, bool visibility)
{
    auto abilitySessionInfo = SetAbilitySessionInfo(scnSession);
    if (abilitySessionInfo == nullptr) {
        return ERR_NULL_OBJECT;
    }
    return AAFwk::AbilityManagerClient::GetInstance()->ChangeUIAbilityVisibilityBySCB(abilitySessionInfo, visibility);
}

WSError SceneSessionManager::RequestSceneSessionActivationInner(
    sptr<SceneSession>& scnSession, bool isNewActive)
{
    auto persistentId = scnSession->GetPersistentId();
    RequestInputMethodCloseKeyboard(persistentId);
    if (WindowHelper::IsMainWindow(scnSession->GetWindowType())) {
        scnSession->SetIsStarting(true);
    }
    if (WindowHelper::IsMainWindow(scnSession->GetWindowType()) && scnSession->IsFocusedOnShow()) {
        if (Session::IsScbCoreEnabled()) {
            if (scnSession->IsVisibleForeground()) {
                RequestSessionFocusImmediately(persistentId);
            } else {
                PostProcessFocusState state = { true, true, FocusChangeReason::SCB_START_APP };
                scnSession->SetPostProcessFocusState(state);
            }
        } else {
            RequestSessionFocusImmediately(persistentId);
        }
    }
    if (scnSession->GetSessionInfo().ancoSceneState < AncoSceneState::NOTIFY_CREATE) {
        FillSessionInfo(scnSession);
        if (!PreHandleCollaborator(scnSession, persistentId)) {
            TLOGE(WmsLogTag::WMS_LIFE, "persistentId: %{public}d, ancoSceneState: %{public}d",
                persistentId, scnSession->GetSessionInfo().ancoSceneState);
            scnSession->NotifySessionExceptionInner(SetAbilitySessionInfo(scnSession), true);
            return WSError::WS_ERROR_PRE_HANDLE_COLLABORATOR_FAILED;
        }
    }
    auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
    if (!scnSessionInfo) {
        TLOGE(WmsLogTag::WMS_LIFE, "create AbilityInfo fail id %{public}d", persistentId);
        return WSError::WS_ERROR_NULLPTR;
    }
    scnSession->NotifyActivation();
    scnSessionInfo->isNewWant = isNewActive;
    if (CheckCollaboratorType(scnSession->GetCollaboratorType())) {
        scnSessionInfo->want.SetParam(AncoConsts::ANCO_MISSION_ID, scnSessionInfo->persistentId);
        scnSessionInfo->collaboratorType = scnSession->GetCollaboratorType();
    }
    TLOGI(WmsLogTag::WMS_LIFE, "id %{public}d want-ability: %{public}s, bundle: %{public}s, "
        "module: %{public}s, uri: %{public}s, appIndex: %{public}d.", persistentId,
        scnSessionInfo->want.GetElement().GetAbilityName().c_str(),
        scnSessionInfo->want.GetElement().GetBundleName().c_str(),
        scnSessionInfo->want.GetElement().GetModuleName().c_str(),
        scnSessionInfo->want.GetElement().GetURI().c_str(),
        scnSessionInfo->want.GetIntParam(AAFwk::Want::PARAM_APP_CLONE_INDEX_KEY, 0));
    int32_t errCode = ERR_OK;
    bool isColdStart = false;
    bool isAppSupportPhoneInPc = false;
    auto sessionProperty = scnSession->GetSessionProperty();
    if (sessionProperty != nullptr) {
        isAppSupportPhoneInPc = sessionProperty->GetIsAppSupportPhoneInPc();
    }
    if (systemConfig_.backgroundswitch == false || isAppSupportPhoneInPc) {
        TLOGI(WmsLogTag::WMS_MAIN, "Begin StartUIAbility: %{public}d system: %{public}u", persistentId,
            static_cast<uint32_t>(scnSession->GetSessionInfo().isSystem_));
        errCode = AAFwk::AbilityManagerClient::GetInstance()->StartUIAbilityBySCB(scnSessionInfo, isColdStart);
    } else {
        TLOGI(WmsLogTag::WMS_MAIN, "Background switch on, isNewActive %{public}d state %{public}u",
            isNewActive, scnSession->GetSessionState());
        if (isNewActive || scnSession->GetSessionState() == SessionState::STATE_DISCONNECT ||
            scnSession->GetSessionState() == SessionState::STATE_END) {
            TLOGI(WmsLogTag::WMS_MAIN, "Call StartUIAbility: %{public}d system: %{public}u", persistentId,
                static_cast<uint32_t>(scnSession->GetSessionInfo().isSystem_));
            errCode = AAFwk::AbilityManagerClient::GetInstance()->StartUIAbilityBySCB(scnSessionInfo, isColdStart);
        } else {
            TLOGI(WmsLogTag::WMS_MAIN, "NotifySessionForeground: %{public}d", persistentId);
            scnSession->NotifySessionForeground(1, true);
        }
    }
    auto sessionInfo = scnSession->GetSessionInfo();
    if (WindowHelper::IsMainWindow(scnSession->GetWindowType())) {
        WindowInfoReporter::GetInstance().InsertShowReportInfo(sessionInfo.bundleName_);
    }
    NotifyCollaboratorAfterStart(scnSession, scnSessionInfo);

    if (errCode != ERR_OK) {
        TLOGI(WmsLogTag::WMS_MAIN, "failed! errCode: %{public}d", errCode);
        scnSession->NotifySessionExceptionInner(scnSessionInfo, true);
        if (startUIAbilityErrorFunc_ && static_cast<WSError>(errCode) == WSError::WS_ERROR_EDM_CONTROLLED) {
            startUIAbilityErrorFunc_(
                static_cast<uint32_t>(WS_JS_TO_ERROR_CODE_MAP.at(WSError::WS_ERROR_EDM_CONTROLLED)));
        }
    }
    if (isColdStart) {
        TLOGI(WmsLogTag::WMS_MAIN, "ColdStart, identityToken:%{public}s, bundleName:%{public}s",
            scnSessionInfo->identityToken.c_str(), sessionInfo.bundleName_.c_str());
        scnSession->SetClientIdentityToken(scnSessionInfo->identityToken);
        scnSession->ResetSessionConnectState();
        scnSession->ResetIsActive();
    }
    return WSError::WS_OK;
}

void SceneSessionManager::NotifyCollaboratorAfterStart(sptr<SceneSession>& scnSession,
    sptr<AAFwk::SessionInfo>& scnSessionInfo)
{
    if (scnSession == nullptr || scnSessionInfo == nullptr) {
        return;
    }
    if (CheckCollaboratorType(scnSession->GetCollaboratorType())) {
        NotifyLoadAbility(scnSession->GetCollaboratorType(),
            scnSessionInfo, scnSession->GetSessionInfo().abilityInfo);
        NotifyUpdateSessionInfo(scnSession);
        NotifyMoveSessionToForeground(scnSession->GetCollaboratorType(), scnSessionInfo->persistentId);
        scnSession->SetSessionInfoAncoSceneState(AncoSceneState::NOTIFY_FOREGROUND);
    }
}

bool SceneSessionManager::IsPcSceneSessionLifecycle(const sptr<SceneSession>& sceneSession)
{
    bool isPcAppInPad = false;
    bool isAppSupportPhoneInPc = false;
    auto property = sceneSession->GetSessionProperty();
    if (property) {
        isPcAppInPad = property->GetIsPcAppInPad();
        isAppSupportPhoneInPc = property->GetIsAppSupportPhoneInPc();
    }
    return (systemConfig_.backgroundswitch && !isAppSupportPhoneInPc) || isPcAppInPad;
}

WSError SceneSessionManager::RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession,
    const bool isDelegator, const bool isToDesktop, const bool isSaveSnapshot)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, isDelegator, isToDesktop, isSaveSnapshot]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        TLOGI(WmsLogTag::WMS_MAIN, "Request background id:%{public}d isDelegator:%{public}d "
            "isToDesktop:%{public}d isSaveSnapshot:%{public}d",
            persistentId, isDelegator, isToDesktop, isSaveSnapshot);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSessionBackground (%d )", persistentId);
        scnSession->SetActive(false);

        if (isToDesktop) {
            auto info = scnSession->GetSessionInfo();
            info.callerToken_ = nullptr;
            info.callingTokenId_ = 0;
            scnSession->SetSessionInfo(info);
        }

        scnSession->BackgroundTask(isSaveSnapshot);
        if (!GetSceneSession(persistentId)) {
            TLOGE(WmsLogTag::WMS_MAIN, "Request background session invalid by %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        if (persistentId == brightnessSessionId_) {
            UpdateBrightness(focusedSessionId_);
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            TLOGE(WmsLogTag::WMS_MAIN, "Create Ability info failed, id %{public}d", persistentId);
            return WSError::WS_ERROR_NULLPTR;
        }
        if (IsPcSceneSessionLifecycle(scnSession)) {
            TLOGI(WmsLogTag::WMS_MAIN, "NotifySessionBackground: %{public}d", persistentId);
            scnSession->NotifySessionBackground(1, true, true);
        } else {
            TLOGI(WmsLogTag::WMS_MAIN, "begin MinimzeUIAbility: %{public}d system: %{public}u",
                persistentId, static_cast<uint32_t>(scnSession->GetSessionInfo().isSystem_));
            if (!isDelegator) {
                AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIAbilityBySCB(scnSessionInfo);
            } else {
                AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIAbilityBySCB(scnSessionInfo, true);
            }
        }

        if (WindowHelper::IsMainWindow(scnSession->GetWindowType())) {
            auto sessionInfo = scnSession->GetSessionInfo();
            WindowInfoReporter::GetInstance().InsertHideReportInfo(sessionInfo.bundleName_);
        }
        return WSError::WS_OK;
    };
    std::string taskName = "RequestSceneSessionBackground:PID:" +
        (sceneSession != nullptr ? std::to_string(sceneSession->GetPersistentId()):"nullptr");
    taskScheduler_->PostAsyncTask(task, taskName);
    return WSError::WS_OK;
}

void SceneSessionManager::NotifyForegroundInteractiveStatus(const sptr<SceneSession>& sceneSession, bool interactive)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, interactive]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return;
        }
        auto persistentId = scnSession->GetPersistentId();
        WLOGFI("NotifyForeInteractive id: %{public}d, status: %{public}d", persistentId, interactive);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:NotifyForegroundInteractiveStatus (%d )", persistentId);
        if (!GetSceneSession(persistentId)) {
            WLOGFE("session is invalid with %{public}d", persistentId);
            return;
        }
        scnSession->NotifyForegroundInteractiveStatus(interactive);
    };

    taskScheduler_->PostAsyncTask(task, "NotifyForegroundInteractiveStatus");
}

WSError SceneSessionManager::DestroyDialogWithMainWindow(const sptr<SceneSession>& scnSession)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:DestroyDialogWithMainWindow");
    if (scnSession == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "session is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (scnSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        TLOGI(WmsLogTag::WMS_DIALOG, "Begin to destroy dialog, parentId: %{public}d", scnSession->GetPersistentId());
        auto dialogVec = scnSession->GetDialogVector();
        for (auto dialog : dialogVec) {
            if (dialog == nullptr) {
                TLOGE(WmsLogTag::WMS_DIALOG, "dialog is nullptr");
                continue;
            }
            auto sceneSession = GetSceneSession(dialog->GetPersistentId());
            if (sceneSession == nullptr) {
                TLOGE(WmsLogTag::WMS_DIALOG, "dialog is invalid, id: %{public}d", dialog->GetPersistentId());
                return WSError::WS_ERROR_INVALID_SESSION;
            }
            WindowDestroyNotifyVisibility(sceneSession);
            dialog->NotifyDestroy();
            dialog->Disconnect();

            auto dialogSceneSession = GetSceneSession(dialog->GetPersistentId());
            if (dialogSceneSession != nullptr) {
                dialogSceneSession->ClearSpecificSessionCbMap();
            }
            {
                std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
                sceneSessionMap_.erase(dialog->GetPersistentId());
                systemTopSceneSessionMap_.erase(dialog->GetPersistentId());
                nonSystemFloatSceneSessionMap_.erase(dialog->GetPersistentId());
            }
        }
        scnSession->ClearDialogVector();
        return WSError::WS_OK;
    }
    return WSError::WS_ERROR_INVALID_SESSION;
}

void SceneSessionManager::DestroySubSession(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFW("sceneSession is nullptr");
        return;
    }
    for (const auto& elem : sceneSession->GetSubSession()) {
        if (elem != nullptr) {
            const auto& persistentId = elem->GetPersistentId();
            TLOGI(WmsLogTag::WMS_SUB, "DestroySubSession, id: %{public}d", persistentId);
            DestroyAndDisconnectSpecificSessionInner(persistentId);
        }
    }
}

void SceneSessionManager::DestroyToastSession(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        TLOGW(WmsLogTag::WMS_LIFE, "ToastSession is nullptr");
        return;
    }
    for (const auto& elem : sceneSession->GetToastSession()) {
        if (elem != nullptr) {
            const auto& persistentId = elem->GetPersistentId();
            TLOGI(WmsLogTag::WMS_TOAST, "DestroyToastSession, id: %{public}d", persistentId);
            DestroyAndDisconnectSpecificSessionInner(persistentId);
        }
    }
}

void SceneSessionManager::EraseSceneSessionMapById(int32_t persistentId)
{
    auto sceneSession = GetSceneSession(persistentId);
    std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    if (sceneSession != nullptr) {
        visibleWindowCountMap_.erase(sceneSession->GetCallingPid());
    }
    sceneSessionMap_.erase(persistentId);
    systemTopSceneSessionMap_.erase(persistentId);
    nonSystemFloatSceneSessionMap_.erase(persistentId);
    MultiInstanceManager::GetInstance().DecreaseInstanceKeyRefCount(sceneSession);
}

WSError SceneSessionManager::RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession,
    bool needRemoveSession, bool isSaveSnapshot, bool isForceClean)
{
    auto task = [this, weakSceneSession = wptr<SceneSession>(sceneSession),
        needRemoveSession, isSaveSnapshot, isForceClean]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "Destruct session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        HandleCastScreenDisConnection(scnSession->GetSessionInfo().screenId_);
        auto persistentId = scnSession->GetPersistentId();
        TLOGI(WmsLogTag::WMS_MAIN, "Destruct session id:%{public}d unfocus", persistentId);
        RequestSessionUnfocus(persistentId, FocusChangeReason::SCB_SESSION_REQUEST_UNFOCUS);
        lastUpdatedAvoidArea_.erase(persistentId);
        DestroyDialogWithMainWindow(scnSession);
        DestroyToastSession(scnSession);
        DestroySubSession(scnSession); // destroy sub session by destruction
        TLOGI(WmsLogTag::WMS_MAIN, "Destruct session id:%{public}d remove:%{public}d isSaveSnapshot:%{public}d"
            " isForceClean:%{public}d", persistentId, needRemoveSession, isSaveSnapshot, isForceClean);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSessionDestruction (%" PRIu32" )", persistentId);
        if (WindowHelper::IsMainWindow(scnSession->GetWindowType())) {
            auto sessionInfo = scnSession->GetSessionInfo();
            WindowInfoReporter::GetInstance().InsertDestroyReportInfo(sessionInfo.bundleName_);
        }
        WindowDestroyNotifyVisibility(scnSession);
        NotifySessionUpdate(scnSession->GetSessionInfo(), ActionType::SINGLE_CLOSE);
        scnSession->DisconnectTask(false, isSaveSnapshot);
        if (!GetSceneSession(persistentId)) {
            TLOGE(WmsLogTag::WMS_MAIN, "Destruct session invalid by %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        scnSession->GetCloseAbilityWantAndClean(scnSessionInfo->want);
        if (scnSessionInfo->resultCode == -1) {
            OHOS::AAFwk::Want want;
            scnSessionInfo->want = want;
        }
        return RequestSceneSessionDestructionInner(scnSession, scnSessionInfo, needRemoveSession, isForceClean);
    };
    std::string taskName = "RequestSceneSessionDestruction:PID:" +
        (sceneSession != nullptr ? std::to_string(sceneSession->GetPersistentId()) : "nullptr");
    taskScheduler_->PostAsyncTask(task, taskName);
    return WSError::WS_OK;
}

void SceneSessionManager::HandleCastScreenDisConnection(uint64_t screenId)
{
    auto task = [screenId] {
        ScreenId defScreenId = ScreenSessionManagerClient::GetInstance().GetDefaultScreenId();
        if (defScreenId == screenId) {
            return;
        }
        auto flag = ScreenManager::GetInstance().GetVirtualScreenFlag(screenId);
        if (flag != VirtualScreenFlag::CAST) {
            return;
        }
        std::vector<uint64_t> mirrorIds { screenId };
        ScreenId groupId;
        DMError ret = ScreenManager::GetInstance().MakeMirror(0, mirrorIds, groupId);
        if (ret != Rosen::DMError::DM_OK) {
            TLOGI(WmsLogTag::WMS_LIFE, "MakeMirror failed, ret: %{public}d", ret);
            return;
        }
    };
    eventHandler_->PostTask(task, "HandleCastScreenDisConnection: ScreenId:" + std::to_string(screenId));
}

void SceneSessionManager::ResetWant(sptr<SceneSession>& sceneSession)
{
    auto& sessionInfo = sceneSession->GetSessionInfo();
    if (sessionInfo.want != nullptr) {
        const auto& bundleName = sessionInfo.want->GetElement().GetBundleName();
        const auto& abilityName = sessionInfo.want->GetElement().GetAbilityName();
        const auto& keySessionId = sessionInfo.want->GetStringParam(KEY_SESSION_ID);
        auto want = std::make_shared<AAFwk::Want>();
        if (want != nullptr) {
            AppExecFwk::ElementName element;
            element.SetBundleName(bundleName);
            element.SetAbilityName(abilityName);
            want->SetElement(element);
            want->SetBundle(bundleName);
            if (!keySessionId.empty()) {
                want->SetParam(KEY_SESSION_ID, keySessionId);
            }
            sceneSession->SetSessionInfoWant(want);
        }
    }
}

WSError SceneSessionManager::RequestSceneSessionDestructionInner(sptr<SceneSession>& sceneSession,
    sptr<AAFwk::SessionInfo> scnSessionInfo, const bool needRemoveSession, const bool isForceClean)
{
    auto persistentId = sceneSession->GetPersistentId();
    TLOGI(WmsLogTag::WMS_MAIN, "begin CloseUIAbility: %{public}d system: %{public}d",
        persistentId, sceneSession->GetSessionInfo().isSystem_);
    if (isForceClean) {
        AAFwk::AbilityManagerClient::GetInstance()->CleanUIAbilityBySCB(scnSessionInfo);
    } else {
        AAFwk::AbilityManagerClient::GetInstance()->CloseUIAbilityBySCB(scnSessionInfo);
    }
    sceneSession->SetSessionInfoAncoSceneState(AncoSceneState::DEFAULT_STATE);
    if (needRemoveSession) {
        if (CheckCollaboratorType(sceneSession->GetCollaboratorType())) {
            NotifyClearSession(sceneSession->GetCollaboratorType(), scnSessionInfo->persistentId);
        }
        EraseSceneSessionMapById(persistentId);
    } else {
        // if terminate, reset want. so start from recent, start a new one.
        TLOGI(WmsLogTag::WMS_MAIN, "reset want: %{public}d", persistentId);
        if (CheckCollaboratorType(sceneSession->GetCollaboratorType())) {
            sceneSession->SetSessionInfoWant(nullptr);
        }
        ResetWant(sceneSession);
        sceneSession->ResetSessionInfoResultCode();
    }
    NotifySessionForCallback(sceneSession, needRemoveSession);
    // Arrive at the STOP task end.
    sceneSession->RemoveLifeCycleTask(LifeCycleTaskType::STOP);
    // Clear js cb map if needed.
    sceneSession->ClearJsSceneSessionCbMap(needRemoveSession);
    return WSError::WS_OK;
}

void SceneSessionManager::AddClientDeathRecipient(const sptr<ISessionStage>& sessionStage,
    const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr || sessionStage == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "sessionStage(%{public}d) or sceneSession is null", sessionStage == nullptr);
        return;
    }

    auto remoteObject = sessionStage->AsObject();
    remoteObjectMap_.insert(std::make_pair(remoteObject, sceneSession->GetPersistentId()));
    if (windowDeath_ == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "failed to create death recipient");
        return;
    }
    if (!remoteObject->AddDeathRecipient(windowDeath_)) {
        TLOGE(WmsLogTag::WMS_LIFE, "failed to add death recipient");
        return;
    }
    WLOGFD("Id: %{public}d", sceneSession->GetPersistentId());
}

void SceneSessionManager::DestroySpecificSession(const sptr<IRemoteObject>& remoteObject)
{
    auto task = [this, remoteObject]() {
        auto iter = remoteObjectMap_.find(remoteObject);
        if (iter == remoteObjectMap_.end()) {
            WLOGFE("Invalid remoteObject");
            return;
        }
        WLOGFD("Remote died, id: %{public}d", iter->second);
        auto sceneSession = GetSceneSession(iter->second);
        if (sceneSession == nullptr) {
            WLOGFW("Remote died, session is nullptr, id: %{public}d", iter->second);
            return;
        }
        DestroyAndDisconnectSpecificSessionInner(iter->second);
        remoteObjectMap_.erase(iter);
    };
    taskScheduler_->PostAsyncTask(task, "DestroySpecificSession");
}

WSError SceneSessionManager::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
    SystemSessionConfig& systemConfig, sptr<IRemoteObject> token)
{
    if (property == nullptr) {
        WLOGFE("property is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }

    if (!CheckSystemWindowPermission(property) || !CheckModalSubWindowPermission(property)) {
        WLOGFE("create system window or modal subwindow permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    bool shouldBlock = (property->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT &&
                        property->IsFloatingWindowAppType() && shouldHideNonSecureFloatingWindows_.load());
    bool isSystemCalling = SessionPermission::IsSystemCalling();
    if (SessionHelper::IsNonSecureToUIExtension(property->GetWindowType()) && !isSystemCalling) {
        auto parentSession = GetSceneSession(property->GetParentPersistentId());
        if (parentSession) {
            shouldBlock = (shouldBlock || parentSession->GetCombinedExtWindowFlags().hideNonSecureWindowsFlag);
        }
    }
    if (shouldBlock) {
        TLOGE(WmsLogTag::WMS_UIEXT, "create non-secure window permission denied!");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }

    if (property->GetWindowType() == WindowType::WINDOW_TYPE_APP_SUB_WINDOW &&
        property->GetExtensionFlag() && property->GetIsUIExtensionAbilityProcess() &&
        SessionPermission::IsStartedByUIExtension()) {
        auto extensionParentSession = GetSceneSession(property->GetParentPersistentId());
        if (extensionParentSession == nullptr) {
            WLOGFE("extensionParentSession is invalid with %{public}d", property->GetParentPersistentId());
            return WSError::WS_ERROR_NULLPTR;
        }
        SessionInfo sessionInfo = extensionParentSession->GetSessionInfo();
        AAFwk::UIExtensionHostInfo hostInfo;
        AAFwk::AbilityManagerClient::GetInstance()->GetUIExtensionRootHostInfo(token, hostInfo);
        if (sessionInfo.bundleName_ != hostInfo.elementName_.GetBundleName()) {
            WLOGE("The hostWindow is not this parentwindow ! parentwindow bundleName: %{public}s, "
                "hostwindow bundleName: %{public}s", sessionInfo.bundleName_.c_str(),
                hostInfo.elementName_.GetBundleName().c_str());
            return WSError::WS_ERROR_INVALID_WINDOW;
        }
    }

    // WINDOW_TYPE_SYSTEM_ALARM_WINDOW has been deprecated, will be deleted after 5 versions.
    if (property->GetWindowType() == WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW) {
        WLOGFE("The alarm window has been deprecated!");
        return WSError::WS_ERROR_INVALID_WINDOW;
    }

    if (property->GetWindowType() == WindowType::WINDOW_TYPE_PIP && !isEnablePiPCreate(property)) {
        WLOGFE("pip window is not enable to create.");
        return WSError::WS_DO_NOTHING;
    }
    TLOGI(WmsLogTag::WMS_LIFE, "create specific start, name:%{public}s, type:%{public}d, touchable:%{public}d",
        property->GetWindowName().c_str(), property->GetWindowType(), property->GetTouchable());

    // Get pid and uid before posting task.
    auto pid = IPCSkeleton::GetCallingRealPid();
    auto uid = IPCSkeleton::GetCallingUid();
    auto task = [this, sessionStage, eventChannel, surfaceNode, property,
                    &persistentId, &session, &systemConfig, token, pid, uid, isSystemCalling]() {
        if (property == nullptr) {
            return WSError::WS_ERROR_NULLPTR;
        }
        const auto& type = property->GetWindowType();
        // create specific session
        SessionInfo info;
        info.windowType_ = static_cast<uint32_t>(type);
        info.bundleName_ = property->GetSessionInfo().bundleName_;
        info.abilityName_ = property->GetSessionInfo().abilityName_;
        info.moduleName_ = property->GetSessionInfo().moduleName_;

        ClosePipWindowIfExist(type);
        sptr<SceneSession> newSession = RequestSceneSession(info, property);
        if (newSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "[WMSSub][WMSSystem] session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        property->SetSystemCalling(isSystemCalling);
        auto errCode = newSession->ConnectInner(
            sessionStage, eventChannel, surfaceNode, systemConfig_, property, token, pid, uid);
        newSession->SetIsSystemSpecificSession(isSystemCalling);
        systemConfig = systemConfig_;
        if (property) {
            persistentId = property->GetPersistentId();
        }

        NotifyCreateSpecificSession(newSession, property, type);
        session = newSession;
        AddClientDeathRecipient(sessionStage, newSession);
        TLOGI(WmsLogTag::WMS_LIFE, "create specific session success, id: %{public}d, "
            "parentId: %{public}d, type: %{public}d",
            newSession->GetPersistentId(), newSession->GetParentPersistentId(), type);
        return errCode;
    };

    return taskScheduler_->PostSyncTask(task, "CreateAndConnectSpecificSession");
}

void SceneSessionManager::ClosePipWindowIfExist(WindowType type)
{
    if (type != WindowType::WINDOW_TYPE_PIP) {
        return;
    }
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& iter: sceneSessionMap_) {
        auto& session = iter.second;
        if (session && session->GetWindowType() == WindowType::WINDOW_TYPE_PIP) {
            session->NotifyCloseExistPipWindow();
            break;
        }
    }
}

bool SceneSessionManager::CheckPiPPriority(const PiPTemplateInfo& pipTemplateInfo)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& iter: sceneSessionMap_) {
        auto& session = iter.second;
        if (session && session->GetWindowMode() == WindowMode::WINDOW_MODE_PIP &&
            pipTemplateInfo.priority < session->GetPiPTemplateInfo().priority &&
            IsSessionVisibleForeground(session)) {
            TLOGE(WmsLogTag::WMS_PIP, "create pip window failed, reason: low priority.");
            return false;
        }
    }
    return true;
}

bool SceneSessionManager::isEnablePiPCreate(const sptr<WindowSessionProperty>& property)
{
    if (isScreenLocked_) {
        TLOGI(WmsLogTag::WMS_PIP, "skip create pip window as screen locked.");
        return false;
    }
    Rect pipRect = property->GetRequestRect();
    if (pipRect.width_ == 0 || pipRect.height_ == 0) {
        TLOGI(WmsLogTag::WMS_PIP, "pip rect is invalid.");
        return false;
    }
    if (!CheckPiPPriority(property->GetPiPTemplateInfo())) {
        TLOGI(WmsLogTag::WMS_PIP, "skip create pip window by priority");
        return false;
    }
    auto parentSession = GetSceneSession(property->GetParentPersistentId());
    if (parentSession == nullptr || parentSession->GetSessionState() == SessionState::STATE_DISCONNECT) {
        TLOGI(WmsLogTag::WMS_PIP, "skip create pip window, maybe parentSession is null or disconnected");
        return false;
    }
    return true;
}

bool SceneSessionManager::CheckModalSubWindowPermission(const sptr<WindowSessionProperty>& property)
{
    WindowType type = property->GetWindowType();
    if (!WindowHelper::IsSubWindow(type) || !property->IsTopmost()) {
        return true;
    }

    if (!WindowHelper::IsModalSubWindow(type, property->GetWindowFlags())) {
        TLOGE(WmsLogTag::WMS_SUB, "Normal subwindow has topmost");
        return false;
    }

    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_SUB, "Modal subwindow has topmost, but no system permission");
        return false;
    }

    return true;
}

bool SceneSessionManager::CheckSystemWindowPermission(const sptr<WindowSessionProperty>& property)
{
    WindowType type = property->GetWindowType();
    if (WindowHelper::IsUIExtensionWindow(type)) {
        // UIExtension window disallowed.
        return false;
    }
    if (!WindowHelper::IsSystemWindow(type)) {
        // type is not system
        return true;
    }
    if ((type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT || type == WindowType::WINDOW_TYPE_INPUT_METHOD_STATUS_BAR)
        && SessionPermission::IsStartedByInputMethod()) {
        // WINDOW_TYPE_INPUT_METHOD_FLOAT could be created by input method app
        WLOGFD("check create permission success, input method app create input method window.");
        return true;
    }
    if (type == WindowType::WINDOW_TYPE_DIALOG || type == WindowType::WINDOW_TYPE_PIP) {
        // some system types could be created by normal app
        return true;
    }
    if (type == WindowType::WINDOW_TYPE_FLOAT) {
        // WINDOW_TYPE_FLOAT could be created with the corresponding permission
        if (SessionPermission::VerifyCallingPermission("ohos.permission.SYSTEM_FLOAT_WINDOW") &&
            (SessionPermission::IsSystemCalling() || SessionPermission::IsStartByHdcd() ||
            systemConfig_.supportTypeFloatWindow_)) {
            WLOGFI("check float permission success.");
            return true;
        } else {
            WLOGFE("check float permission failed.");
            return false;
        }
    }
    if (SessionPermission::IsSystemCalling() || SessionPermission::IsStartByHdcd()) {
        WLOGFD("check create permission success, create with system calling.");
        return true;
    }
    WLOGFE("check system window permission failed.");
    return false;
}

SessionInfo SceneSessionManager::RecoverSessionInfo(const sptr<WindowSessionProperty>& property)
{
    SessionInfo sessionInfo;
    if (property == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "property is nullptr");
        return sessionInfo;
    }
    sessionInfo = property->GetSessionInfo();
    sessionInfo.persistentId_ = property->GetPersistentId();
    sessionInfo.windowMode = static_cast<int32_t>(property->GetWindowMode());
    sessionInfo.windowType_ = static_cast<uint32_t>(property->GetWindowType());
    sessionInfo.requestOrientation_ = static_cast<uint32_t>(property->GetRequestedOrientation());
    sessionInfo.sessionState_ = (property->GetWindowState() == WindowState::STATE_SHOWN)
                                    ? SessionState::STATE_ACTIVE
                                    : SessionState::STATE_BACKGROUND;
    sessionInfo.isPersistentRecover_ = true;
    TLOGI(WmsLogTag::WMS_RECOVER,
        "Recover and reconnect session with: bundleName=%{public}s, moduleName=%{public}s, "
        "abilityName=%{public}s, windowMode=%{public}d, windowType=%{public}u, persistentId=%{public}d, "
        "windowState=%{public}u",
        sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str(),
        sessionInfo.windowMode, sessionInfo.windowType_, sessionInfo.persistentId_, sessionInfo.sessionState_);
    return sessionInfo;
}

void SceneSessionManager::SetAlivePersistentIds(const std::vector<int32_t>& alivePersistentIds)
{
    TLOGI(WmsLogTag::WMS_RECOVER, "Number of persistentIds need to be recovered = %{public}zu. CurrentUserId = "
           "%{public}d", alivePersistentIds.size(), currentUserId_);
    alivePersistentIds_ = alivePersistentIds;
}

bool SceneSessionManager::IsNeedRecover(const int32_t persistentId)
{
    auto it = std::find(alivePersistentIds_.begin(), alivePersistentIds_.end(), persistentId);
    if (it == alivePersistentIds_.end()) {
        TLOGW(WmsLogTag::WMS_RECOVER, "recovered persistentId=%{public}d is not in alivePersistentIds_", persistentId);
        return false;
    }
    return true;
}

WSError SceneSessionManager::CheckSessionPropertyOnRecovery(const sptr<WindowSessionProperty>& property,
    bool isSpecificSession)
{
    if (property == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "property is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (!CheckSystemWindowPermission(property)) {
        TLOGE(WmsLogTag::WMS_RECOVER, "create system window permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (isSpecificSession) {
        if (property->GetParentPersistentId() > 0 && !IsNeedRecover(property->GetParentPersistentId())) {
            TLOGE(WmsLogTag::WMS_RECOVER, "no need to recover.");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
    } else {
        if (!IsNeedRecover(property->GetPersistentId())) {
            TLOGE(WmsLogTag::WMS_RECOVER, "no need to recover.");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
    }
    return WSError::WS_OK;
}

WSError SceneSessionManager::RecoverAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, sptr<ISession>& session, sptr<IRemoteObject> token)
{
    auto propCheckRet = CheckSessionPropertyOnRecovery(property, true);
    if (propCheckRet != WSError::WS_OK) {
        return propCheckRet;
    }
    auto pid = IPCSkeleton::GetCallingRealPid();
    auto uid = IPCSkeleton::GetCallingUid();
    auto task = [this, sessionStage, eventChannel, surfaceNode, property, &session, token, pid, uid]() {
        if (recoveringFinished_) {
            TLOGW(WmsLogTag::WMS_RECOVER, "Recover finished, not recovery anymore");
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
        // recover specific session
        SessionInfo info = RecoverSessionInfo(property);
        TLOGI(WmsLogTag::WMS_RECOVER, "callingSessionId = %{public}" PRIu32, property->GetCallingSessionId());
        ClosePipWindowIfExist(property->GetWindowType());
        sptr<SceneSession> sceneSession = RequestSceneSession(info, property);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_RECOVER, "RequestSceneSession failed");
            return WSError::WS_ERROR_NULLPTR;
        }

        auto persistentId = sceneSession->GetPersistentId();
        if (persistentId != info.persistentId_) {
            TLOGE(WmsLogTag::WMS_RECOVER,
                "SpecificSession PersistentId changed, from %{public}d to %{public}d, parentPersistentId is %{public}d",
                info.persistentId_, persistentId, property->GetParentPersistentId());
            failRecoveredPersistentIdSet_.insert(property->GetParentPersistentId());
            EraseSceneSessionMapById(persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }

        auto errCode = sceneSession->Reconnect(sessionStage, eventChannel, surfaceNode, property, token, pid, uid);
        if (errCode != WSError::WS_OK) {
            TLOGE(WmsLogTag::WMS_RECOVER, "SceneSession reconnect failed");
            EraseSceneSessionMapById(persistentId);
            return errCode;
        }
        NotifyCreateSpecificSession(sceneSession, property, property->GetWindowType());
        CacheSubSessionForRecovering(sceneSession, property);
        NotifySessionUnfocusedToClient(persistentId);
        AddClientDeathRecipient(sessionStage, sceneSession);
        session = sceneSession;
        return errCode;
    };
    return taskScheduler_->PostSyncTask(task, "RecoverAndConnectSpecificSession");
}

void SceneSessionManager::NotifyRecoveringFinished()
{
    taskScheduler_->PostAsyncTask([this]() {
            TLOGI(WmsLogTag::WMS_RECOVER, "RecoverFinished clear recoverSubSessionCacheMap");
            recoveringFinished_ = true;
            recoverSubSessionCacheMap_.clear();
        }, "NotifyRecoveringFinished");
}

void SceneSessionManager::CacheSubSessionForRecovering(
    sptr<SceneSession> sceneSession, const sptr<WindowSessionProperty>& property)
{
    if (recoveringFinished_) {
        TLOGW(WmsLogTag::WMS_RECOVER, "recovering is finished");
        return;
    }

    if (sceneSession == nullptr || property == nullptr) {
        TLOGE(WmsLogTag::WMS_RECOVER, "sceneSession or property is nullptr");
        return;
    }

    auto windowType = property->GetWindowType();
    if (!SessionHelper::IsSubWindow(windowType)) {
        return;
    }

    auto persistentId = property->GetParentPersistentId();
    if (createSubSessionFuncMap_.find(persistentId) != createSubSessionFuncMap_.end()) {
        return;
    }

    TLOGI(WmsLogTag::WMS_RECOVER,
        "Cache subsession persistentId = %{public}" PRId32 ", parent persistentId = %{public}" PRId32,
        sceneSession->GetPersistentId(), persistentId);

    if (recoverSubSessionCacheMap_.find(persistentId) == recoverSubSessionCacheMap_.end()) {
        recoverSubSessionCacheMap_[persistentId] = std::vector{ sceneSession };
    } else {
        recoverSubSessionCacheMap_[persistentId].emplace_back(sceneSession);
    }
}

void SceneSessionManager::RecoverCachedSubSession(int32_t persistentId)
{
    auto iter = recoverSubSessionCacheMap_.find(persistentId);
    if (iter == recoverSubSessionCacheMap_.end()) {
        return;
    }

    TLOGI(WmsLogTag::WMS_RECOVER, "Id=%{public}d", persistentId);
    for (auto& sceneSession : iter->second) {
        NotifyCreateSubSession(persistentId, sceneSession);
    }
    recoverSubSessionCacheMap_.erase(iter);
}

void SceneSessionManager::NotifySessionUnfocusedToClient(int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_RECOVER, "Id=%{public}d", persistentId);
    listenerController_->NotifySessionUnfocused(persistentId);
}

WSError SceneSessionManager::RecoverAndReconnectSceneSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<ISession>& session, sptr<WindowSessionProperty> property, sptr<IRemoteObject> token)
{
    auto propCheckRet = CheckSessionPropertyOnRecovery(property, false);
    if (propCheckRet != WSError::WS_OK) {
        return propCheckRet;
    }
    auto pid = IPCSkeleton::GetCallingRealPid();
    auto uid = IPCSkeleton::GetCallingUid();
    auto task = [this, sessionStage, eventChannel, surfaceNode, &session, property, token, pid, uid]() {
        if (recoveringFinished_) {
            TLOGW(WmsLogTag::WMS_RECOVER, "Recover finished, not recovery anymore");
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
        if (recoverSceneSessionFunc_ == nullptr) {
            TLOGE(WmsLogTag::WMS_RECOVER, "recoverSceneSessionFunc_ is null");
            return WSError::WS_ERROR_NULLPTR;
        }
        SessionInfo sessionInfo = RecoverSessionInfo(property);
        sptr<SceneSession> sceneSession = nullptr;
        if (SessionHelper::IsMainWindow(property->GetWindowType())) {
            sceneSession = RequestSceneSession(sessionInfo, nullptr);
        } else {
            sceneSession = RequestSceneSession(sessionInfo, property);
        }
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_RECOVER, "Request sceneSession failed");
            return WSError::WS_ERROR_NULLPTR;
        }
        int32_t persistentId = sceneSession->GetPersistentId();
        if (persistentId != sessionInfo.persistentId_) {
            TLOGE(WmsLogTag::WMS_RECOVER, "SceneSession PersistentId changed, from %{public}d to %{public}d",
                sessionInfo.persistentId_, persistentId);
            EraseSceneSessionMapById(persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto ret = sceneSession->Reconnect(sessionStage, eventChannel, surfaceNode, property, token, pid, uid);
        if (ret != WSError::WS_OK) {
            TLOGE(WmsLogTag::WMS_RECOVER, "Reconnect failed");
            EraseSceneSessionMapById(sessionInfo.persistentId_);
            return ret;
        }
        sceneSession->SetRecovered(true);
        recoverSceneSessionFunc_(sceneSession, sessionInfo);
        NotifySessionUnfocusedToClient(persistentId);
        session = sceneSession;
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "RecoverAndReconnectSceneSession");
}

void SceneSessionManager::SetRecoverSceneSessionListener(const NotifyRecoverSceneSessionFunc& func)
{
    TLOGI(WmsLogTag::WMS_RECOVER, "called");
    recoverSceneSessionFunc_ = func;
}

void SceneSessionManager::SetCreateSystemSessionListener(const NotifyCreateSystemSessionFunc& func)
{
    createSystemSessionFunc_ = func;
}

void SceneSessionManager::SetCreateKeyboardSessionListener(const NotifyCreateKeyboardSessionFunc& func)
{
    createKeyboardSessionFunc_ = func;
}

void SceneSessionManager::RegisterCreateSubSessionListener(int32_t persistentId,
    const NotifyCreateSubSessionFunc& func)
{
    TLOGI(WmsLogTag::WMS_SUB, "RegisterCreateSubSessionListener, id: %{public}d", persistentId);
    auto task = [this, persistentId, func]() {
        createSubSessionFuncMap_[persistentId] = func;
        RecoverCachedSubSession(persistentId);
        return WMError::WM_OK;
    };
    taskScheduler_->PostSyncTask(task, "RegisterCreateSubSessionListener");
}

void SceneSessionManager::NotifyCreateSpecificSession(sptr<SceneSession> newSession,
    sptr<WindowSessionProperty> property, const WindowType& type)
{
    if (newSession == nullptr) {
        WLOGFE("newSession is nullptr");
        return;
    }
    if (property == nullptr) {
        WLOGFE("property is nullptr");
        return;
    }
    if (SessionHelper::IsSystemWindow(type)) {
        if (type == WindowType::WINDOW_TYPE_FLOAT) {
            auto parentSession = GetSceneSession(property->GetParentPersistentId());
            if (parentSession != nullptr) {
                newSession->SetParentSession(parentSession);
            }
        }
        if (type == WindowType::WINDOW_TYPE_TOAST) {
            NotifyCreateToastSession(property->GetParentPersistentId(), newSession);
        }
        if (type != WindowType::WINDOW_TYPE_DIALOG) {
            if (WindowHelper::IsSystemSubWindow(type)) {
                NotifyCreateSubSession(property->GetParentPersistentId(), newSession);
            } else if (isKeyboardPanelEnabled_ && type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT
                && createKeyboardSessionFunc_) {
                createKeyboardSessionFunc_(newSession, newSession->GetKeyboardPanelSession());
            } else if (createSystemSessionFunc_) {
                createSystemSessionFunc_(newSession);
            }
            TLOGD(WmsLogTag::WMS_LIFE, "Create system session, id:%{public}d, type: %{public}d",
                newSession->GetPersistentId(), type);
        } else {
            TLOGW(WmsLogTag::WMS_LIFE, "Didn't create jsSceneSession for this system type, id:%{public}d, "
                "type:%{public}d", newSession->GetPersistentId(), type);
            return;
        }
    } else if (SessionHelper::IsSubWindow(type)) {
        NotifyCreateSubSession(property->GetParentPersistentId(), newSession);
        TLOGD(WmsLogTag::WMS_LIFE, "Notify sub jsSceneSession, id:%{public}d, parentId:%{public}d, type:%{public}d",
            newSession->GetPersistentId(), property->GetParentPersistentId(), type);
    } else {
        TLOGW(WmsLogTag::WMS_LIFE, "Invalid session type, id:%{public}d, type:%{public}d",
            newSession->GetPersistentId(), type);
    }
}

void SceneSessionManager::NotifyCreateSubSession(int32_t persistentId, sptr<SceneSession> session, uint32_t windowFlags)
{
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "SubSession is nullptr");
        return;
    }
    auto iter = createSubSessionFuncMap_.find(persistentId);
    if (iter == createSubSessionFuncMap_.end()) {
        TLOGW(WmsLogTag::WMS_LIFE, "Can't find CreateSubSessionListener, parentId: %{public}d", persistentId);
        return;
    }

    sptr<SceneSession> parentSession = nullptr;
    if (windowFlags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_IS_TOAST)) {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        parentSession = GetMainParentSceneSession(persistentId, sceneSessionMap_);
    } else {
        parentSession = GetSceneSession(persistentId);
    }
    if (parentSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "Can't find CreateSubSessionListener, parentId: %{public}d, subId: %{public}d",
            persistentId, session->GetPersistentId());
        return;
    }
    parentSession->AddSubSession(session);
    session->SetParentSession(parentSession);
    if (iter->second) {
        iter->second(session);
    }
    TLOGD(WmsLogTag::WMS_LIFE, "NotifyCreateSubSession success, parentId: %{public}d, subId: %{public}d",
        persistentId, session->GetPersistentId());
}

sptr<SceneSession> SceneSessionManager::GetMainParentSceneSession(int32_t persistentId,
    const std::map<int32_t, sptr<SceneSession>>& sessionMap)
{
    if (persistentId == INVALID_SESSION_ID) {
        TLOGW(WmsLogTag::WMS_LIFE, "invalid persistentId id");
        return nullptr;
    }
    auto iter = sessionMap.find(persistentId);
    if (iter == sessionMap.end()) {
        WLOGFD("Error found scene session with id: %{public}d", persistentId);
        return nullptr;
    }
    sptr<SceneSession> parentSession = iter->second;
    if (parentSession == nullptr) {
        TLOGW(WmsLogTag::WMS_LIFE, "not find parent session");
        return nullptr;
    }
    bool isNoParentSystemSession = WindowHelper::IsSystemWindow(parentSession->GetWindowType()) &&
        parentSession->GetParentPersistentId() == INVALID_SESSION_ID;
    if (WindowHelper::IsMainWindow(parentSession->GetWindowType()) || isNoParentSystemSession) {
        TLOGD(WmsLogTag::WMS_LIFE, "find main session, id:%{public}u", persistentId);
        return parentSession;
    }
    return GetMainParentSceneSession(parentSession->GetParentPersistentId(), sessionMap);
}

void SceneSessionManager::NotifyCreateToastSession(int32_t persistentId, sptr<SceneSession> session)
{
    if (session == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "toastSession is nullptr");
        return;
    }

    auto parentSession = GetSceneSession(persistentId);
    if (parentSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "Can't find parentSession, parentId: %{public}d, ToastId: %{public}d",
            persistentId, session->GetPersistentId());
        return;
    }
    parentSession->AddToastSession(session);
    session->SetParentSession(parentSession);
    TLOGD(WmsLogTag::WMS_LIFE, "NotifyCreateToastSession success, parentId: %{public}d, toastId: %{public}d",
        persistentId, session->GetPersistentId());
}

void SceneSessionManager::UnregisterCreateSubSessionListener(int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_SUB, "UnregisterCreateSubSessionListener, id: %{public}d", persistentId);
    auto task = [this, persistentId]() {
        auto iter = createSubSessionFuncMap_.find(persistentId);
        if (iter != createSubSessionFuncMap_.end()) {
            createSubSessionFuncMap_.erase(persistentId);
        } else {
            TLOGW(WmsLogTag::WMS_SUB, "Can't find CreateSubSessionListener, id: %{public}d", persistentId);
        }
        return WMError::WM_OK;
    };
    taskScheduler_->PostSyncTask(task);
}

void SceneSessionManager::SetStatusBarEnabledChangeListener(const ProcessStatusBarEnabledChangeFunc& func)
{
    WLOGFD("SetStatusBarEnabledChangeListener");
    if (!func) {
        WLOGFD("set func is null");
    }
    statusBarEnabledChangeFunc_ = func;
}

void SceneSessionManager::SetGestureNavigationEnabledChangeListener(
    const ProcessGestureNavigationEnabledChangeFunc& func)
{
    WLOGFD("SetGestureNavigationEnabledChangeListener");
    if (!func) {
        WLOGFD("set func is null");
    }
    gestureNavigationEnabledChangeFunc_ = func;
}

void SceneSessionManager::OnOutsideDownEvent(int32_t x, int32_t y)
{
    TLOGD(WmsLogTag::WMS_EVENT, "x=%{private}d, y=%{private}d", x, y);
    if (outsideDownEventFunc_) {
        outsideDownEventFunc_(x, y);
    }
}

void SceneSessionManager::NotifySessionTouchOutside(int32_t persistentId)
{
    auto task = [this, persistentId]() {
        int32_t callingSessionId = INVALID_SESSION_ID;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto &item : sceneSessionMap_) {
            auto sceneSession = item.second;
            if (sceneSession == nullptr) {
                continue;
            }
            auto sessionProperty = sceneSession->GetSessionProperty();
            if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT &&
                sessionProperty != nullptr) {
                callingSessionId = static_cast<int32_t>(sessionProperty->GetCallingSessionId());
                TLOGI(WmsLogTag::WMS_KEYBOARD, "persistentId: %{public}d, callingSessionId: %{public}d",
                    persistentId, callingSessionId);
            }
            if (!(sceneSession->IsVisible() ||
                sceneSession->GetSessionState() == SessionState::STATE_FOREGROUND ||
                sceneSession->GetSessionState() == SessionState::STATE_ACTIVE)) {
                continue;
            }
            auto sessionId = sceneSession->GetPersistentId();
            if ((!sceneSession->CheckOutTouchOutsideRegister()) &&
                (touchOutsideListenerSessionSet_.find(sessionId) == touchOutsideListenerSessionSet_.end())) {
                WLOGFD("id: %{public}d is not in touchOutsideListenerNodes, don't notify.", sessionId);
                continue;
            }
            if (sessionId == callingSessionId || sessionId == persistentId) {
                WLOGFD("No need to notify touch window, id: %{public}d", sessionId);
                continue;
            }
            sceneSession->NotifyTouchOutside();
        }
    };

    taskScheduler_->PostAsyncTask(task, "NotifySessionTouchOutside:PID" + std::to_string(persistentId));
    return;
}

void SceneSessionManager::SetOutsideDownEventListener(const ProcessOutsideDownEventFunc& func)
{
    WLOGFD("SetOutsideDownEventListener");
    outsideDownEventFunc_ = func;
}

WSError SceneSessionManager::DestroyAndDisconnectSpecificSessionInner(const int32_t persistentId)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        return WSError::WS_ERROR_NULLPTR;
    }
    auto ret = sceneSession->UpdateActiveStatus(false);
    WindowDestroyNotifyVisibility(sceneSession);
    if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
        auto parentSession = GetSceneSession(sceneSession->GetParentPersistentId());
        if (parentSession == nullptr) {
            TLOGE(WmsLogTag::WMS_DIALOG, "Dialog not bind parent");
        } else {
            parentSession->RemoveDialogToParentSession(sceneSession);
        }
    }
    if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_TOAST) {
        auto parentSession = GetSceneSession(sceneSession->GetParentPersistentId());
        if (parentSession != nullptr) {
            TLOGD(WmsLogTag::WMS_TOAST, "Find parentSession, id: %{public}d", persistentId);
            parentSession->RemoveToastSession(persistentId);
        } else {
            TLOGW(WmsLogTag::WMS_TOAST, "ParentSession is nullptr, id: %{public}d", persistentId);
        }
    }
    ret = sceneSession->Disconnect();
    sceneSession->ClearSpecificSessionCbMap();
    if (SessionHelper::IsSubWindow(sceneSession->GetWindowType())) {
        auto parentSession = GetSceneSession(sceneSession->GetParentPersistentId());
        if (parentSession != nullptr) {
            TLOGD(WmsLogTag::WMS_SUB, "Find parentSession, id: %{public}d", persistentId);
            parentSession->RemoveSubSession(persistentId);
        } else {
            TLOGW(WmsLogTag::WMS_SUB, "ParentSession is nullptr, id: %{public}d", persistentId);
        }
        DestroyUIServiceExtensionSubWindow(sceneSession);
    }
    {
        std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        visibleWindowCountMap_.erase(sceneSession->GetCallingPid());
        sceneSessionMap_.erase(persistentId);
        systemTopSceneSessionMap_.erase(persistentId);
        nonSystemFloatSceneSessionMap_.erase(persistentId);
        UnregisterCreateSubSessionListener(persistentId);
    }
    TLOGI(WmsLogTag::WMS_LIFE, "Destroy specific session end, id: %{public}d", persistentId);
    return ret;
}

WSError SceneSessionManager::DestroyAndDisconnectSpecificSession(const int32_t persistentId)
{
    const auto& callingPid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, persistentId, callingPid]() {
        TLOGI(WmsLogTag::WMS_LIFE, "Destroy specific session start, id: %{public}d", persistentId);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, persistentId:%{public}d", persistentId);
            return WSError::WS_ERROR_NULLPTR;
        }

        if (callingPid != sceneSession->GetCallingPid()) {
            TLOGE(WmsLogTag::WMS_LIFE, "Permission denied, not destroy by the same process");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        return DestroyAndDisconnectSpecificSessionInner(persistentId);
    };

    return taskScheduler_->PostSyncTask(task, "DestroyAndDisConnect:PID:" + std::to_string(persistentId));
}

WSError SceneSessionManager::DestroyAndDisconnectSpecificSessionWithDetachCallback(const int32_t persistentId,
    const sptr<IRemoteObject>& callback)
{
    if (callback == nullptr) {
        return WSError::WS_ERROR_NULLPTR;
    }
    const auto callingPid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, persistentId, callingPid, callback]() {
        TLOGI(WmsLogTag::WMS_LIFE, "Destroy specific session start, id: %{public}d", persistentId);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr, persistentId:%{public}d", persistentId);
            return WSError::WS_ERROR_NULLPTR;
        }

        if (callingPid != sceneSession->GetCallingPid()) {
            TLOGE(WmsLogTag::WMS_LIFE, "Permission denied, not destroy by the same process");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        sceneSession->RegisterDetachCallback(iface_cast<IPatternDetachCallback>(callback));
        return DestroyAndDisconnectSpecificSessionInner(persistentId);
    };

    return taskScheduler_->PostSyncTask(task, "DestroyAndDisConnect:PID:" + std::to_string(persistentId));
}

void SceneSessionManager::DestroyUIServiceExtensionSubWindow(const sptr<SceneSession>& sceneSession)
{
    if (!sceneSession) {
        TLOGE(WmsLogTag::WMS_SUB,"sceneSession is null");
        return;
    }
    auto sessionProperty = sceneSession->GetSessionProperty();
    if (sessionProperty && sessionProperty->GetExtensionFlag() == true &&
        !sessionProperty->GetIsUIExtensionAbilityProcess()) {
        sceneSession->NotifyDestroy();
        int32_t errCode = AAFwk::AbilityManagerClient::GetInstance()->
            TerminateUIServiceExtensionAbility(sceneSession->GetAbilityToken());
        TLOGI(WmsLogTag::WMS_SUB,"TerminateUIServiceExtensionAbility id:%{public}d errCode:%{public}d",
            sceneSession->GetPersistentId(), errCode);
    }
}

const AppWindowSceneConfig& SceneSessionManager::GetWindowSceneConfig() const
{
    return appWindowSceneConfig_;
}

void SceneSessionManager::UpdateRotateAnimationConfig(const RotateAnimationConfig& config)
{
    auto task = [this, config]() {
        TLOGI(WmsLogTag::DEFAULT, "update rotate animation config duration: %{public}d", config.duration_);
        rotateAnimationConfig_.duration_ = config.duration_;
    };
    taskScheduler_->PostAsyncTask(task, "UpdateRotateAnimationConfig");
}

WSError SceneSessionManager::ProcessBackEvent()
{
    auto task = [this]() {
        auto session = GetSceneSession(focusedSessionId_);
        if (!session) {
            WLOGFE("session is nullptr: %{public}d", focusedSessionId_);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        WLOGFI("ProcessBackEvent session persistentId:%{public}d needBlock::%{public}d",
            focusedSessionId_, needBlockNotifyFocusStatusUntilForeground_);
        if (needBlockNotifyFocusStatusUntilForeground_) {
            WLOGFD("RequestSessionBack when start session");
            session->RequestSessionBack(false);
            return WSError::WS_OK;
        }
        if (session->GetSessionInfo().isSystem_ && rootSceneProcessBackEventFunc_) {
            rootSceneProcessBackEventFunc_();
        } else {
            session->ProcessBackEvent();
        }
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task, "ProcessBackEvent");
    return WSError::WS_OK;
}

WSError SceneSessionManager::InitUserInfo(int32_t userId, std::string& fileDir)
{
    if (userId == DEFAULT_USERID || fileDir.empty()) {
        TLOGE(WmsLogTag::WMS_MAIN, "params invalid");
        return WSError::WS_DO_NOTHING;
    }
    TLOGI(WmsLogTag::WMS_MAIN, "userId : %{public}d, path : %{public}s", userId, fileDir.c_str());
    auto task = [this, userId, &fileDir]() {
        if (!ScenePersistence::CreateSnapshotDir(fileDir)) {
            TLOGD(WmsLogTag::WMS_MAIN, "Create snapshot directory failed");
        }
        if (!ScenePersistence::CreateUpdatedIconDir(fileDir)) {
            TLOGD(WmsLogTag::WMS_MAIN, "Create icon directory failed");
        }
        currentUserId_ = userId;
        SceneInputManager::GetInstance().SetCurrentUserId(currentUserId_);
        MultiInstanceManager::GetInstance().SetCurrentUserId(currentUserId_);
        RegisterSecSurfaceInfoListener();
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "InitUserInfo");
}

bool SceneSessionManager::IsNeedChangeLifeCycleOnUserSwitch(const sptr<SceneSession>& sceneSession, int32_t pid)
{
    auto sessionState = sceneSession->GetSessionState();
    auto isInvalidMainSession = !WindowHelper::IsMainWindow(sceneSession->GetWindowType()) ||
                                sessionState == SessionState::STATE_DISCONNECT ||
                                sessionState == SessionState::STATE_END;
    if (isInvalidMainSession) {
        TLOGD(WmsLogTag::WMS_MULTI_USER, "persistentId: %{public}d, type: %{public}d, state: %{public}d",
            sceneSession->GetPersistentId(), sceneSession->GetWindowType(), sceneSession->GetSessionState());
    }
    return sceneSession->GetCallingPid() != pid && IsPcSceneSessionLifecycle(sceneSession) && !isInvalidMainSession;
}

WSError SceneSessionManager::StartOrMinimizeUIAbilityBySCB(const sptr<SceneSession>& sceneSession, bool isUserActive)
{
    auto persistentId = sceneSession->GetPersistentId();
    auto abilitySessionInfo = SetAbilitySessionInfo(sceneSession);
    if (!abilitySessionInfo) {
        TLOGE(WmsLogTag::WMS_MULTI_USER, "Create Ability info failed, id %{public}d", persistentId);
        return WSError::WS_ERROR_NULLPTR;
    }
    int32_t errCode = ERR_OK;
    if (!isUserActive) {
        TLOGI(WmsLogTag::WMS_MULTI_USER,
            "MinimizeUIAbilityBySCB with persistentId: %{public}d, type: %{public}d, state: %{public}d", persistentId,
            sceneSession->GetWindowType(), sceneSession->GetSessionState());
        bool isFromUser = false;
        errCode = AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIAbilityBySCB(
            abilitySessionInfo, isFromUser, static_cast<uint32_t>(WindowStateChangeReason::USER_SWITCH));
        if (errCode == ERR_OK) {
            sceneSession->SetMinimizedFlagByUserSwitch(true);
        } else {
            TLOGE(WmsLogTag::WMS_MULTI_USER, "minimize failed! errCode: %{public}d", errCode);
        }
    } else if (sceneSession->IsMinimizedByUserSwitch()) {
        TLOGI(WmsLogTag::WMS_MULTI_USER,
            "StartUIAbilityBySCB with persistentId: %{public}d, type: %{public}d, state: %{public}d", persistentId,
            sceneSession->GetWindowType(), sceneSession->GetSessionState());
        sceneSession->SetMinimizedFlagByUserSwitch(false);
        bool isColdStart = false;
        errCode = AAFwk::AbilityManagerClient::GetInstance()->StartUIAbilityBySCB(
            abilitySessionInfo, isColdStart, static_cast<uint32_t>(WindowStateChangeReason::USER_SWITCH));
        if (errCode != ERR_OK) {
            TLOGE(WmsLogTag::WMS_MULTI_USER, "start failed! errCode: %{public}d", errCode);
            sceneSession->NotifySessionExceptionInner(abilitySessionInfo, true);
            if (startUIAbilityErrorFunc_ && static_cast<WSError>(errCode) == WSError::WS_ERROR_EDM_CONTROLLED) {
                startUIAbilityErrorFunc_(
                    static_cast<uint32_t>(WS_JS_TO_ERROR_CODE_MAP.at(WSError::WS_ERROR_EDM_CONTROLLED)));
            }
        }
    }
    return WSError::WS_OK;
}

void SceneSessionManager::NotifySwitchingUser(const bool isUserActive)
{
    auto task = [this, isUserActive]() {
        TLOGI(WmsLogTag::WMS_MULTI_USER,
            "Notify switching user. IsUserActive=%{public}u, currentUserId=%{public}d",
            isUserActive, currentUserId_);
        isUserBackground_ = !isUserActive;
        SceneInputManager::GetInstance().SetUserBackground(!isUserActive);
        if (isUserActive) { // switch to current user
            SceneInputManager::GetInstance().SetCurrentUserId(currentUserId_);
            MultiInstanceManager::GetInstance().SetCurrentUserId(currentUserId_);
            // notify screenSessionManager to recover current user
            ScreenSessionManagerClient::GetInstance().SwitchingCurrentUser();
            FlushWindowInfoToMMI(true);
            NotifyAllAccessibilityInfo();
        } else { // switch to another user
            SceneInputManager::GetInstance().FlushEmptyInfoToMMI();
        }

        // Change app life cycle in pc when user switch, do app freeze
        int32_t pid = GetPid();
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& [_, sceneSession] : sceneSessionMap_) {
            if (sceneSession == nullptr) {
                TLOGE(WmsLogTag::WMS_MULTI_USER, "session is null");
                continue;
            }
            if (IsNeedChangeLifeCycleOnUserSwitch(sceneSession, pid)) {
                StartOrMinimizeUIAbilityBySCB(sceneSession, isUserActive);
            }
        }
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "NotifySwitchingUser");
}

sptr<AppExecFwk::IBundleMgr> SceneSessionManager::GetBundleManager()
{
    auto systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        WLOGFE("Failed to get SystemAbilityManager.");
        return nullptr;
    }

    auto bmsObj = systemAbilityMgr->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bmsObj == nullptr) {
        WLOGFE("Failed to get BundleManagerService.");
        return nullptr;
    }

    return iface_cast<AppExecFwk::IBundleMgr>(bmsObj);
}

std::shared_ptr<Global::Resource::ResourceManager> SceneSessionManager::GetResourceManager(
    const AppExecFwk::AbilityInfo& abilityInfo)
{
    auto context = rootSceneContextWeak_.lock();
    if (!context) {
        WLOGFE("context is nullptr.");
        return nullptr;
    }
    auto resourceMgr = context->GetResourceManager();
    if (!resourceMgr) {
        WLOGFE("resourceMgr is nullptr.");
        return nullptr;
    }
    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    if (!resConfig) {
        WLOGFE("resConfig is nullptr.");
        return nullptr;
    }
    resourceMgr->GetResConfig(*resConfig);
    resourceMgr = Global::Resource::CreateResourceManager(
        abilityInfo.bundleName, abilityInfo.moduleName, "", {}, *resConfig);
    if (!resourceMgr) {
        WLOGFE("resourceMgr is nullptr.");
        return nullptr;
    }
    resourceMgr->UpdateResConfig(*resConfig);

    std::string loadPath;
    if (!abilityInfo.hapPath.empty()) { // zipped hap
        loadPath = abilityInfo.hapPath;
    } else {
        loadPath = abilityInfo.resourcePath;
    }

    if (!resourceMgr->AddResource(loadPath.c_str(), Global::Resource::SELECT_COLOR | Global::Resource::SELECT_MEDIA)) {
        WLOGFW("Add resource %{private}s failed.", loadPath.c_str());
    }
    return resourceMgr;
}

bool SceneSessionManager::GetStartupPageFromResource(const AppExecFwk::AbilityInfo& abilityInfo,
    std::string& path, uint32_t& bgColor)
{
    auto resourceMgr = GetResourceManager(abilityInfo);
    if (!resourceMgr) {
        WLOGFE("resourceMgr is nullptr.");
        return false;
    }

    if (resourceMgr->GetColorById(abilityInfo.startWindowBackgroundId, bgColor) != Global::Resource::RState::SUCCESS) {
        WLOGFE("Failed to get background color, id %{public}d.", abilityInfo.startWindowBackgroundId);
        return false;
    }

    if (resourceMgr->GetMediaById(abilityInfo.startWindowIconId, path) != Global::Resource::RState::SUCCESS) {
        WLOGFE("Failed to get icon, id %{public}d.", abilityInfo.startWindowIconId);
        return false;
    }

    if (!abilityInfo.hapPath.empty()) { // zipped hap
        auto pos = path.find_last_of('.');
        if (pos == std::string::npos) {
            WLOGFE("Format error, path %{private}s.", path.c_str());
            return false;
        }
        path = "resource:///" + std::to_string(abilityInfo.startWindowIconId) + path.substr(pos);
    }
    return true;
}

void SceneSessionManager::GetStartupPage(const SessionInfo& sessionInfo, std::string& path, uint32_t& bgColor)
{
    if (!bundleMgr_) {
        WLOGFE("bundleMgr_ is nullptr.");
        return;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:GetStartupPage");
    if (GetStartingWindowInfoFromCache(sessionInfo, path, bgColor)) {
        WLOGFI("Found in cache: %{public}s, %{public}x", path.c_str(), bgColor);
        return;
    }
    AAFwk::Want want;
    want.SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_, sessionInfo.moduleName_);
    AppExecFwk::AbilityInfo abilityInfo;
    if (!bundleMgr_->QueryAbilityInfo(
        want, AppExecFwk::GET_ABILITY_INFO_DEFAULT, AppExecFwk::Constants::ANY_USERID, abilityInfo)) {
        WLOGFE("Get ability info from BMS failed!");
        return;
    }

    if (GetStartupPageFromResource(abilityInfo, path, bgColor)) {
        CacheStartingWindowInfo(abilityInfo, path, bgColor);
    }
    WLOGFI("%{public}d, %{public}d, %{public}s, %{public}x",
        abilityInfo.startWindowIconId, abilityInfo.startWindowBackgroundId, path.c_str(), bgColor);
}

bool SceneSessionManager::GetStartingWindowInfoFromCache(
    const SessionInfo& sessionInfo, std::string& path, uint32_t& bgColor)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:GetStartingWindowInfoFromCache");
    std::shared_lock<std::shared_mutex> lock(startingWindowMapMutex_);
    auto iter = startingWindowMap_.find(sessionInfo.bundleName_);
    if (iter == startingWindowMap_.end()) {
        return false;
    }
    auto key = sessionInfo.moduleName_ + sessionInfo.abilityName_;
    const auto& infoMap = iter->second;
    auto infoMapIter = infoMap.find(key);
    if (infoMapIter == infoMap.end()) {
        return false;
    }
    path = infoMapIter->second.startingWindowIconPath_;
    bgColor = infoMapIter->second.startingWindowBackgroundColor_;
    return true;
}

void SceneSessionManager::CacheStartingWindowInfo(
    const AppExecFwk::AbilityInfo& abilityInfo, const std::string& path, const uint32_t& bgColor)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:CacheStartingWindowInfo");
    auto key = abilityInfo.moduleName + abilityInfo.name;
    StartingWindowInfo info = {
        .startingWindowBackgroundId_ = abilityInfo.startWindowBackgroundId,
        .startingWindowIconId_ = abilityInfo.startWindowIconId,
        .startingWindowBackgroundColor_ = bgColor,
        .startingWindowIconPath_ = path,
    };
    std::unique_lock<std::shared_mutex> lock(startingWindowMapMutex_);
    auto iter = startingWindowMap_.find(abilityInfo.bundleName);
    if (iter != startingWindowMap_.end()) {
        auto& infoMap = iter->second;
        infoMap.emplace(key, info);
        return;
    }
    if (startingWindowMap_.size() >= MAX_CACHE_COUNT) {
        startingWindowMap_.erase(startingWindowMap_.begin());
    }
    std::map<std::string, StartingWindowInfo> infoMap({{ key, info }});
    startingWindowMap_.emplace(abilityInfo.bundleName, infoMap);
}

void SceneSessionManager::OnBundleUpdated(const std::string& bundleName, int userId)
{
    taskScheduler_->PostAsyncTask([this, bundleName]() {
        std::unique_lock<std::shared_mutex> lock(startingWindowMapMutex_);
        auto iter = startingWindowMap_.find(bundleName);
        if (iter != startingWindowMap_.end()) {
            startingWindowMap_.erase(iter);
        }
    },
        "OnBundleUpdated");
}

void SceneSessionManager::OnConfigurationUpdated(const std::shared_ptr<AppExecFwk::Configuration>& configuration)
{
    taskScheduler_->PostAsyncTask([this]() {
        std::unique_lock<std::shared_mutex> lock(startingWindowMapMutex_);
        startingWindowMap_.clear();
    },
        "OnConfigurationUpdated");
}

void SceneSessionManager::FillSessionInfo(sptr<SceneSession>& sceneSession)
{
    auto sessionInfo = sceneSession->GetSessionInfo();
    if (sessionInfo.bundleName_.empty()) {
        WLOGFE("bundleName_ is empty");
        return;
    }
    if (sessionInfo.isSystem_) {
        WLOGFD("is system scene!");
        return;
    }
    auto abilityInfo = QueryAbilityInfoFromBMS(currentUserId_, sessionInfo.bundleName_, sessionInfo.abilityName_,
        sessionInfo.moduleName_);
    if (abilityInfo == nullptr) {
        WLOGFE("abilityInfo is nullptr!");
        return;
    }
    sceneSession->SetSessionInfoAbilityInfo(abilityInfo);
    sceneSession->SetSessionInfoTime(GetCurrentTime());
    if (abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::RESERVE_TYPE)) {
        sceneSession->SetCollaboratorType(CollaboratorType::RESERVE_TYPE);
    } else if (abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::OTHERS_TYPE)) {
        sceneSession->SetCollaboratorType(CollaboratorType::OTHERS_TYPE);
    }
    TLOGI(WmsLogTag::DEFAULT, "bundleName:%{public}s removeMissionAfterTerminate:%{public}d "
        "excludeFromMissions:%{public}d label:%{public}s iconPath:%{public}s collaboratorType:%{public}s.",
        abilityInfo->bundleName.c_str(), abilityInfo->removeMissionAfterTerminate, abilityInfo->excludeFromMissions,
        abilityInfo->label.c_str(), abilityInfo->iconPath.c_str(), abilityInfo->applicationInfo.codePath.c_str());
}

std::shared_ptr<AppExecFwk::AbilityInfo> SceneSessionManager::QueryAbilityInfoFromBMS(const int32_t uId,
    const std::string& bundleName, const std::string& abilityName, const std::string& moduleName)
{
    if (!bundleMgr_) {
        TLOGE(WmsLogTag::DEFAULT, "bundleMgr_ is nullptr");
        return nullptr;
    }
    SessionInfoList list = {
        .uid_ = uId, .bundleName_ = bundleName, .abilityName_ = abilityName, .moduleName_ = moduleName
    };
    if (abilityInfoMap_.count(list)) {
        return abilityInfoMap_[list];
    }
    AAFwk::Want want;
    want.SetElementName("", bundleName, abilityName, moduleName);
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    if (abilityInfo == nullptr) {
        WLOGFE("abilityInfo is nullptr!");
        return nullptr;
    }
    auto abilityInfoFlag = (AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION |
        AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_PERMISSION |
        AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_METADATA);
    bool ret = bundleMgr_->QueryAbilityInfo(want, abilityInfoFlag, uId, *abilityInfo);
    if (!ret) {
        WLOGFE("Get ability info from BMS failed!");
        return nullptr;
    }
    abilityInfoMap_[list] = abilityInfo;
    return abilityInfo;
}

static void GetTopWindowByTraverseSessionTree(const sptr<SceneSession>& session,
    uint32_t& topWinId, uint32_t& zOrder)
{
    const auto& subVec = session->GetSubSession();
    for (const auto& subSession : subVec) {
        if (subSession == nullptr || subSession->GetCallingPid() != session->GetCallingPid()) {
            TLOGW(WmsLogTag::WMS_SUB,
                "subSession is null or subWin's callingPid is not equal to mainWin's callingPid");
            continue;
        }
        if ((subSession->GetSessionState() == SessionState::STATE_FOREGROUND ||
             subSession->GetSessionState() == SessionState::STATE_ACTIVE) &&
            subSession->GetZOrder() > zOrder) {
            topWinId = static_cast<uint32_t>(subSession->GetPersistentId());
            zOrder = subSession->GetZOrder();
            TLOGD(WmsLogTag::WMS_SUB, "Current zorder is larger than mainWin, mainId: %{public}d, "
                "topWinId: %{public}d, zOrder: %{public}d", session->GetPersistentId(), topWinId, zOrder);
        }
        if (subSession->GetSubSession().size() > 0) {
            GetTopWindowByTraverseSessionTree(subSession, topWinId, zOrder);
        }
    }
}

/** @note @window.hierarchy */
WMError SceneSessionManager::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    const auto& callingPid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, mainWinId, &topWinId, callingPid]() {
        const auto& mainSession = GetSceneSession(mainWinId);
        if (mainSession == nullptr) {
            return WMError::WM_ERROR_INVALID_WINDOW;
        }

        if (callingPid != mainSession->GetCallingPid()) {
            WLOGFE("Permission denied, not destroy by the same process");
            return WMError::WM_ERROR_INVALID_PERMISSION;
        }
        uint32_t zOrder = mainSession->GetZOrder();
        topWinId = mainWinId;
        GetTopWindowByTraverseSessionTree(mainSession, topWinId, zOrder);
        TLOGI(WmsLogTag::WMS_SUB, "[GetTopWin] Get top window, mainId: %{public}d, topWinId: %{public}d, "
            "zOrder: %{public}d", mainWinId, topWinId, zOrder);
        return WMError::WM_OK;
    };

    if (!Session::IsScbCoreEnabled()) {
        return taskScheduler_->PostSyncTask(task, "GetTopWindowId");
    }
    bool postNow = false;
    auto mainSession = GetSceneSession(mainWinId);
    if (mainSession != nullptr) {
        postNow = !(mainSession->GetUIStateDirty());
    }
    auto mainEventRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    if (postNow || (mainEventRunner && mainEventRunner->IsCurrentRunnerThread()) ||
        taskScheduler_->GetEventHandler()->GetEventRunner()->IsCurrentRunnerThread()) {
        return taskScheduler_->PostSyncTask(task, "GetTopWindowId");
    }
    TLOGI(WmsLogTag::WMS_PIPELINE, "wait for flush UI, id: %{public}d", mainWinId);
    {
        std::unique_lock<std::mutex> lock(nextFlushCompletedMutex_);
        if (nextFlushCompletedCV_.wait_for(lock, std::chrono::milliseconds(GET_TOP_WINDOW_DELAY)) ==
            std::cv_status::timeout) {
            TLOGW(WmsLogTag::WMS_PIPELINE, "wait for 100ms");
        }
    }
    return taskScheduler_->PostSyncTask(task, "GetTopWindowId");
}

static WMError GetParentMainWindowIdInner(const std::map<int32_t, sptr<SceneSession>>& sceneSessionMap,
    uint32_t windowId, uint32_t& mainWindowId)
{
    auto iter = sceneSessionMap.find(windowId);
    if (iter == sceneSessionMap.end()) {
        TLOGD(WmsLogTag::WMS_SUB, "not found scene session with id: %{public}d", windowId);
        return WMError::WM_ERROR_NULLPTR;
    }
    sptr<SceneSession> sceneSession = iter->second;
    if (sceneSession == nullptr) {
        TLOGW(WmsLogTag::WMS_SUB, "not find parent session");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
        mainWindowId = sceneSession->GetPersistentId();
        return WMError::WM_OK;
    }
    if (WindowHelper::IsSubWindow(sceneSession->GetWindowType()) ||
        WindowHelper::IsDialogWindow(sceneSession->GetWindowType())) {
        return GetParentMainWindowIdInner(sceneSessionMap, sceneSession->GetParentPersistentId(), mainWindowId);
    }
    // not sub window, dialog, return invalid id
    mainWindowId = INVALID_SESSION_ID;
    return WMError::WM_OK;
}

WMError SceneSessionManager::GetParentMainWindowId(uint32_t windowId, uint32_t& mainWindowId)
{
    if (windowId == INVALID_SESSION_ID) {
        TLOGW(WmsLogTag::WMS_SUB, "invalid windowId id");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    return GetParentMainWindowIdInner(sceneSessionMap_, windowId, mainWindowId);
}

void SceneSessionManager::HandleSpecificSystemBarProperty(WindowType type, const sptr<WindowSessionProperty>& property,
    const sptr<SceneSession>& sceneSession)
{
    auto systemBarProperties = property->GetSystemBarProperty();
    for (auto iter : systemBarProperties) {
        if (iter.first == type) {
            sceneSession->SetSystemBarProperty(iter.first, iter.second);
            TLOGD(WmsLogTag::WMS_IMMS, "SetSystemBarProperty: %{public}d, enable: %{public}d",
                static_cast<int32_t>(iter.first), iter.second.enable_);
        }
    }
    NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
}

/** @note @window.hierarchy */
WMError SceneSessionManager::UpdateTopmostProperty(const sptr<WindowSessionProperty>& property,
    const sptr<SceneSession>& sceneSession)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "UpdateTopmostProperty permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }

    sceneSession->SetTopmost(property->IsTopmost());
    return WMError::WM_OK;
}

void SceneSessionManager::HandleHideNonSystemFloatingWindows(const sptr<WindowSessionProperty>& property,
    const sptr<SceneSession>& sceneSession)
{
    auto propertyOld = sceneSession->GetSessionProperty();
    if (propertyOld == nullptr) {
        TLOGI(WmsLogTag::DEFAULT, "session property null");
        return;
    }

    bool hideNonSystemFloatingWindowsOld = propertyOld->GetHideNonSystemFloatingWindows();
    bool hideNonSystemFloatingWindowsNew = property->GetHideNonSystemFloatingWindows();
    if (hideNonSystemFloatingWindowsOld == hideNonSystemFloatingWindowsNew) {
        TLOGI(WmsLogTag::DEFAULT, "property hideNonSystemFloatingWindows not change");
        return;
    }

    if (IsSessionVisibleForeground(sceneSession)) {
        if (hideNonSystemFloatingWindowsOld) {
            UpdateForceHideState(sceneSession, propertyOld, false);
        } else {
            UpdateForceHideState(sceneSession, property, true);
        }
    }
}

void SceneSessionManager::UpdateForceHideState(const sptr<SceneSession>& sceneSession,
    const sptr<WindowSessionProperty>& property, bool add)
{
    if (property == nullptr) {
        WLOGFD("property is null");
        return;
    }
    auto persistentId = sceneSession->GetPersistentId();
    bool forceHideFloatOld = !systemTopSceneSessionMap_.empty();
    bool notifyAll = false;
    if (add) {
        if (property->GetHideNonSystemFloatingWindows()) {
            systemTopSceneSessionMap_.insert({ persistentId, sceneSession });
            notifyAll = !forceHideFloatOld;
        } else if (property->IsFloatingWindowAppType()) {
            nonSystemFloatSceneSessionMap_.insert({ persistentId, sceneSession });
            if (forceHideFloatOld) {
                sceneSession->NotifyForceHideChange(true);
            }
        }
    } else {
        if (property->GetHideNonSystemFloatingWindows()) {
            systemTopSceneSessionMap_.erase(persistentId);
            notifyAll = forceHideFloatOld && systemTopSceneSessionMap_.empty();
        } else if (property->IsFloatingWindowAppType()) {
            nonSystemFloatSceneSessionMap_.erase(persistentId);
            if (property->GetForceHide()) {
                sceneSession->NotifyForceHideChange(false);
            }
        }
    }
    if (notifyAll) {
        bool forceHideFloatNew = !systemTopSceneSessionMap_.empty();
        for (const auto &item : nonSystemFloatSceneSessionMap_) {
            auto forceHideSceneSession = item.second;
            auto forceHideProperty = forceHideSceneSession->GetSessionProperty();
            if (forceHideProperty && forceHideFloatNew != forceHideProperty->GetForceHide()) {
                forceHideSceneSession->NotifyForceHideChange(forceHideFloatNew);
            }
        }
    }
}

void SceneSessionManager::HandleTurnScreenOn(const sptr<SceneSession>& sceneSession)
{
#ifdef POWER_MANAGER_ENABLE
    auto task = [this, sceneSession]() {
        if (sceneSession == nullptr) {
            WLOGFE("session is invalid");
            return;
        }
        WLOGFD("Win: %{public}s, is turn on%{public}d",
            sceneSession->GetWindowName().c_str(), sceneSession->IsTurnScreenOn());
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        if (sceneSession->IsTurnScreenOn() && !PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
            WLOGI("turn screen on");
            PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
        }
        // set ipc identity to raw
        IPCSkeleton::SetCallingIdentity(identity);
    };
    taskScheduler_->PostAsyncTask(task, "HandleTurnScreenOn");

#else
    WLOGFD("Can not found the sub system of PowerMgr");
#endif
}

void SceneSessionManager::HandleKeepScreenOn(const sptr<SceneSession>& sceneSession, bool requireLock)
{
#ifdef POWER_MANAGER_ENABLE
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, requireLock]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is invalid");
            return;
        }
        if (requireLock && scnSession->keepScreenLock_ == nullptr) {
            // reset ipc identity
            std::string identity = IPCSkeleton::ResetCallingIdentity();
            scnSession->keepScreenLock_ =
                PowerMgr::PowerMgrClient::GetInstance().CreateRunningLock(scnSession->GetWindowName(),
                PowerMgr::RunningLockType::RUNNINGLOCK_SCREEN);
            // set ipc identity to raw
            IPCSkeleton::SetCallingIdentity(identity);
        }
        if (scnSession->keepScreenLock_ == nullptr) {
            return;
        }
        bool shouldLock = requireLock && IsSessionVisibleForeground(scnSession);
        WLOGI("keep screen on: [%{public}s, %{public}d]", scnSession->GetWindowName().c_str(), shouldLock);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:HandleKeepScreenOn");
        ErrCode res;
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        if (shouldLock) {
            res = scnSession->keepScreenLock_->Lock();
        } else {
            res = scnSession->keepScreenLock_->UnLock();
        }
        // set ipc identity to raw
        IPCSkeleton::SetCallingIdentity(identity);
        if (res != ERR_OK) {
            WLOGFE("handle keep screen running lock failed: [operation: %{public}d, err: %{public}d]",
                requireLock, res);
        }
    };
    taskScheduler_->PostAsyncTask(task, "HandleKeepScreenOn");
#else
    WLOGFD("Can not found the sub system of PowerMgr");
#endif
}

WSError SceneSessionManager::SetBrightness(const sptr<SceneSession>& sceneSession, float brightness)
{
#ifdef POWERMGR_DISPLAY_MANAGER_ENABLE
    if (GetDisplayBrightness() != brightness &&
        GetFocusedSessionId() == sceneSession->GetPersistentId()) {
        bool setBrightnessRet = false;
        if (std::fabs(brightness - UNDEFINED_BRIGHTNESS) < std::numeric_limits<float>::min()) {
            auto task = []() {
                DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().RestoreBrightness();
            };
            setBrightnessRet = eventHandler_->PostTask(task, "DisplayPowerMgr:RestoreBrightness", 0);
            SetDisplayBrightness(UNDEFINED_BRIGHTNESS); // UNDEFINED_BRIGHTNESS means system default brightness
        } else {
            auto task = [brightness]() {
                DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().OverrideBrightness(
                    static_cast<uint32_t>(brightness * MAX_BRIGHTNESS));
            };
            setBrightnessRet = eventHandler_->PostTask(task, "DisplayPowerMgr:OverrideBrightness", 0);
            SetDisplayBrightness(brightness);
        }
        if (!setBrightnessRet) {
            WLOGFE("Report post listener callback task failed. the task name is SetBrightness");
        }
    }
#else
    WLOGFD("Can not found the sub system of DisplayPowerMgr");
#endif
    brightnessSessionId_ = sceneSession->GetPersistentId();
    return WSError::WS_OK;
}

WSError SceneSessionManager::UpdateBrightness(int32_t persistentId)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (!(sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW ||
            sceneSession->GetSessionInfo().isSystem_)) {
        WLOGW("only app main window can set brightness");
        return WSError::WS_DO_NOTHING;
    }
    auto brightness = sceneSession->GetBrightness();
    WLOGFI("Brightness: [%{public}f, %{public}f]", GetDisplayBrightness(), brightness);
    if (std::fabs(brightness - UNDEFINED_BRIGHTNESS) < std::numeric_limits<float>::min()) {
        if (GetDisplayBrightness() != brightness) {
            WLOGI("adjust brightness with default value");
            DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().RestoreBrightness();
            SetDisplayBrightness(UNDEFINED_BRIGHTNESS); // UNDEFINED_BRIGHTNESS means system default brightness
        }
        brightnessSessionId_ = INVALID_WINDOW_ID;
    } else {
        if (GetDisplayBrightness() != brightness) {
            WLOGI("adjust brightness with value");
            DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().OverrideBrightness(
                static_cast<uint32_t>(brightness * MAX_BRIGHTNESS));
            SetDisplayBrightness(brightness);
        }
        brightnessSessionId_ = sceneSession->GetPersistentId();
    }
    return WSError::WS_OK;
}

int32_t SceneSessionManager::GetCurrentUserId() const
{
    return currentUserId_;
}

void SceneSessionManager::SetDisplayBrightness(float brightness)
{
    displayBrightness_ = brightness;
}

float SceneSessionManager::GetDisplayBrightness() const
{
    return displayBrightness_;
}

WMError SceneSessionManager::SetGestureNavigationEnabled(bool enable)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGE(WmsLogTag::WMS_EVENT, "permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    std::string callerBundleName = SessionPermission::GetCallingBundleName();
    TLOGD(WmsLogTag::WMS_EVENT, "enable:%{public}d name:%{public}s", enable, callerBundleName.c_str());
    auto task = [this, enable, bundleName = std::move(callerBundleName)]() {
        SessionManagerAgentController::GetInstance().NotifyGestureNavigationEnabledResult(enable);
        if (!gestureNavigationEnabledChangeFunc_ && !statusBarEnabledChangeFunc_) {
            WLOGFE("callback func is null");
            return WMError::WM_OK;
        }
        if (gestureNavigationEnabledChangeFunc_) {
            gestureNavigationEnabledChangeFunc_(enable, bundleName);
        }
        if (statusBarEnabledChangeFunc_) {
            statusBarEnabledChangeFunc_(enable, bundleName);
        }
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetGestureNavigationEnabled");
}

WSError SceneSessionManager::SetFocusedSessionId(int32_t persistentId)
{
    if (focusedSessionId_ == persistentId) {
        WLOGI("Focus scene not change, id: %{public}d", focusedSessionId_);
        return WSError::WS_DO_NOTHING;
    }
    lastFocusedSessionId_ = focusedSessionId_;
    focusedSessionId_ = persistentId;
    return WSError::WS_OK;
}

int32_t SceneSessionManager::GetFocusedSessionId() const
{
    return focusedSessionId_;
}

void SceneSessionManager::GetFocusWindowInfo(FocusChangeInfo& focusInfo)
{
    if (!SessionPermission::IsSACalling()) {
        WLOGFE("GetFocusWindowInfo permission denied!");
        return;
    }
    auto sceneSession = GetSceneSession(focusedSessionId_);
    if (sceneSession) {
        WLOGFD("Get focus session info success");
        focusInfo.windowId_ = sceneSession->GetWindowId();
        focusInfo.displayId_ = static_cast<DisplayId>(0);
        focusInfo.pid_ = sceneSession->GetCallingPid();
        focusInfo.uid_ = sceneSession->GetCallingUid();
        focusInfo.windowType_ = sceneSession->GetWindowType();
        focusInfo.abilityToken_ = sceneSession->GetAbilityToken();
    }
    return;
}

static bool IsValidDigitString(const std::string& windowIdStr)
{
    if (windowIdStr.empty()) {
        return false;
    }
    for (char ch : windowIdStr) {
        if ((ch >= '0' && ch <= '9')) {
            continue;
        }
        WLOGFE("invalid window id");
        return false;
    }
    return true;
}

void SceneSessionManager::RegisterSessionExceptionFunc(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    NotifySessionExceptionFunc sessionExceptionFunc = [this](const SessionInfo& info, bool needRemoveSession = false) {
        auto task = [this, info] {
            auto session = GetSceneSession(info.persistentId_);
            if (session == nullptr) {
                WLOGW("NotifySessionExceptionFunc, Not found session, id: %{public}d",
                    info.persistentId_);
                return;
            }
            if (auto sessionProperty = session->GetSessionProperty()) {
                TLOGI(WmsLogTag::DEFAULT, "windId: %{public}d, recover requestedOrientation %{public}u when exception",
                    session->GetPersistentId(), static_cast<uint32_t>(sessionProperty->GetDefaultRequestedOrientation()));
                sessionProperty->SetRequestedOrientation(sessionProperty->GetDefaultRequestedOrientation());
            }
            if (session->GetSessionInfo().isSystem_) {
                WLOGW("NotifySessionExceptionFunc, id: %{public}d is system",
                    session->GetPersistentId());
                return;
            }
            WLOGI("NotifySessionExceptionFunc, errorCode: %{public}d, id: %{public}d",
                info.errorCode, info.persistentId_);
            if (info.errorCode == static_cast<int32_t>(AAFwk::ErrorLifecycleState::ABILITY_STATE_LOAD_TIMEOUT) ||
                info.errorCode == static_cast<int32_t>(AAFwk::ErrorLifecycleState::ABILITY_STATE_FOREGROUND_TIMEOUT)) {
                WLOGD("NotifySessionClosed when ability load timeout "
                    "or foreground timeout, id: %{public}d", info.persistentId_);
                listenerController_->NotifySessionClosed(info.persistentId_);
            }
        };
        taskScheduler_->PostVoidSyncTask(task, "sessionException");
    };
    sceneSession->SetSessionExceptionListener(sessionExceptionFunc, false);
    TLOGD(WmsLogTag::WMS_LIFE, "success, id: %{public}d", sceneSession->GetPersistentId());
}

void SceneSessionManager::RegisterVisibilityChangedDetectFunc(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr");
        return;
    }
    VisibilityChangedDetectFunc func = [this](const int32_t pid, const bool isVisible,
        const bool newIsVisible) {
        if (isVisible == newIsVisible || pid == -1) {
            return;
        }
        sptr<WindowPidVisibilityInfo> windowPidVisibilityInfo = sptr<WindowPidVisibilityInfo>::MakeSptr();
        windowPidVisibilityInfo->pid_ = pid;
        int32_t count = 0;
        int32_t beforeCount = 0;
        {
            std::unique_lock<std::mutex> lock(visibleWindowCountMapMutex_);
            if (visibleWindowCountMap_.find(pid) != visibleWindowCountMap_.end()) {
                beforeCount = visibleWindowCountMap_[pid];
            } else {
                visibleWindowCountMap_[pid] = 0;
            }
            visibleWindowCountMap_[pid] = newIsVisible ? visibleWindowCountMap_[pid] + 1 :
                visibleWindowCountMap_[pid] - 1;
            count = visibleWindowCountMap_[pid];
        }
        if (beforeCount > 0 && count == 0) {
            TLOGI(WmsLogTag::WMS_LIFE, "The windows of pid %{public}d change to invisibility.", pid);
            windowPidVisibilityInfo->visibilityState_ = WindowPidVisibilityState::INVISIBILITY_STATE;
            SessionManagerAgentController::GetInstance().NotifyWindowPidVisibilityChanged(windowPidVisibilityInfo);
        } else if (beforeCount == 0 && count == 1) {
            TLOGI(WmsLogTag::WMS_LIFE, "The windows of pid %{public}d change to visibility.", pid);
            windowPidVisibilityInfo->visibilityState_ = WindowPidVisibilityState::VISIBILITY_STATE;
            SessionManagerAgentController::GetInstance().NotifyWindowPidVisibilityChanged(windowPidVisibilityInfo);
        } else if (count < 0) {
            TLOGE(WmsLogTag::WMS_LIFE, "The count of visible windows in same pid:%{public}d is less than 0.", pid);
            RecoveryVisibilityPidCount(pid);
        }
    };
    sceneSession->SetVisibilityChangedDetectFunc(func);
}

void SceneSessionManager::RecoveryVisibilityPidCount(int32_t pid)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    visibleWindowCountMap_[pid] = 0;
    for (const auto& iter : sceneSessionMap_) {
        auto& session = iter.second;
        if (session && session->GetCallingPid() == pid && session->IsVisible()) {
            visibleWindowCountMap_[pid]++;
        }
    }
}

void SceneSessionManager::RegisterSessionSnapshotFunc(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    NotifySessionSnapshotFunc sessionSnapshotFunc = [this](int32_t persistentId) {
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGW("NotifySessionSnapshotFunc, Not found session, id: %{public}d", persistentId);
            return;
        }
        if (sceneSession->GetSessionInfo().isSystem_) {
            WLOGW("NotifySessionSnapshotFunc, id: %{public}d is system", sceneSession->GetPersistentId());
            return;
        }
        auto abilityInfoPtr = sceneSession->GetSessionInfo().abilityInfo;
        if (abilityInfoPtr == nullptr) {
            WLOGW("NotifySessionSnapshotFunc, abilityInfoPtr is nullptr");
            return;
        }
        if (!abilityInfoPtr->excludeFromMissions) {
            listenerController_->NotifySessionSnapshotChanged(persistentId);
        }
    };
    sceneSession->SetSessionSnapshotListener(sessionSnapshotFunc);
    WLOGFD("success, id: %{public}d", sceneSession->GetPersistentId());
}

void SceneSessionManager::RegisterAcquireRotateAnimationConfigFunc(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "session is nullptr");
        return;
    }
    AcquireRotateAnimationConfigFunc acquireRotateAnimationConfigFunc = [this](RotateAnimationConfig& config) {
        config.duration_ = rotateAnimationConfig_.duration_;
    };
    sceneSession->SetAcquireRotateAnimationConfigFunc(acquireRotateAnimationConfigFunc);
    TLOGD(WmsLogTag::DEFAULT, "Register acquire Rotate Animation config success, id: %{public}d",
        sceneSession->GetPersistentId());
}

void SceneSessionManager::NotifySessionForCallback(const sptr<SceneSession>& sceneSession, const bool needRemoveSession)
{
    if (sceneSession == nullptr) {
        WLOGFW("session is null");
        return;
    }
    if (sceneSession->GetSessionInfo().isSystem_) {
        WLOGFW("id: %{public}d is system", sceneSession->GetPersistentId());
        return;
    }
    if (sceneSession->GetSessionInfo().appIndex_ != 0) {
        WLOGFI("NotifyDestroy, appIndex: %{public}d, id: %{public}d",
               sceneSession->GetSessionInfo().appIndex_, sceneSession->GetPersistentId());
        listenerController_->NotifySessionDestroyed(sceneSession->GetPersistentId());
        return;
    }
    if (needRemoveSession) {
        WLOGFI("NotifyDestroy, needRemoveSession, id: %{public}d", sceneSession->GetPersistentId());
        listenerController_->NotifySessionDestroyed(sceneSession->GetPersistentId());
        return;
    }
    if (sceneSession->GetSessionInfo().abilityInfo == nullptr) {
        WLOGFW("abilityInfo is null, id: %{public}d", sceneSession->GetPersistentId());
    } else if ((sceneSession->GetSessionInfo().abilityInfo)->removeMissionAfterTerminate ||
               (sceneSession->GetSessionInfo().abilityInfo)->excludeFromMissions) {
        WLOGFI("NotifyDestroy, removeMissionAfterTerminate or excludeFromMissions, id: %{public}d",
            sceneSession->GetPersistentId());
        listenerController_->NotifySessionDestroyed(sceneSession->GetPersistentId());
        return;
    }
    WLOGFI("NotifyClosed, id: %{public}d", sceneSession->GetPersistentId());
    listenerController_->NotifySessionClosed(sceneSession->GetPersistentId());
}

void SceneSessionManager::NotifyWindowInfoChangeFromSession(int32_t persistentId)
{
    WLOGFD("NotifyWindowInfoChange, persistentId = %{public}d", persistentId);
    sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession nullptr");
        return;
    }

    SceneInputManager::GetInstance().NotifyWindowInfoChangeFromSession(sceneSession);
}

bool SceneSessionManager::IsSessionVisible(const sptr<SceneSession>& session)
{
    if (session == nullptr) {
        return false;
    }
    if (Session::IsScbCoreEnabled()) {
        return session->IsVisible();
    }
    const auto& state = session->GetSessionState();
    if (WindowHelper::IsSubWindow(session->GetWindowType())) {
        const auto& parentSceneSession = session->GetParentSession();
        if (parentSceneSession == nullptr) {
            WLOGFW("Can not find parent for this sub window, id: %{public}d", session->GetPersistentId());
            return false;
        }
        const auto& parentState = parentSceneSession->GetSessionState();
        if (session->IsVisible() || (state == SessionState::STATE_ACTIVE || state == SessionState::STATE_FOREGROUND)) {
            if (parentState == SessionState::STATE_INACTIVE || parentState == SessionState::STATE_BACKGROUND) {
                WLOGFD("Parent of this sub window is at background, id: %{public}d", session->GetPersistentId());
                return false;
            }
            WLOGFD("Sub window is at foreground, id: %{public}d", session->GetPersistentId());
            return true;
        }
        WLOGFD("Sub window is at background, id: %{public}d", session->GetPersistentId());
        return false;
    }

    if (session->IsVisible() || (state == SessionState::STATE_ACTIVE || state == SessionState::STATE_FOREGROUND)) {
        WLOGFD("Window is at foreground, id: %{public}d", session->GetPersistentId());
        return true;
    }
    WLOGFD("Window is at background, id: %{public}d", session->GetPersistentId());
    return false;
}

bool SceneSessionManager::IsSessionVisibleForeground(const sptr<SceneSession>& session)
{
    if (session == nullptr) {
        return false;
    }
    if (Session::IsScbCoreEnabled()) {
        return session->IsVisibleForeground();
    }
    return IsSessionVisible(session);
}

void SceneSessionManager::DumpSessionInfo(const sptr<SceneSession>& session, std::ostringstream& oss)
{
    if (session == nullptr) {
        return;
    }
    int32_t zOrder = IsSessionVisibleForeground(session) ? static_cast<int32_t>(session->GetZOrder()) : -1;
    WSRect rect = session->GetSessionRect();
    std::string sName;
    if (session->GetSessionInfo().isSystem_) {
        sName = session->GetSessionInfo().abilityName_;
    } else {
        sName = session->GetWindowName();
    }
    uint32_t flag = 0;
    uint64_t displayId = INVALID_SCREEN_ID;
    auto sessionProperty = session->GetSessionProperty();
    if (sessionProperty) {
        flag = sessionProperty->GetWindowFlags();
        displayId = sessionProperty->GetDisplayId();
    }
    uint32_t orientation = 0;
    const std::string& windowName = sName.size() <= WINDOW_NAME_MAX_LENGTH ?
        sName : sName.substr(0, WINDOW_NAME_MAX_LENGTH);
    // std::setw is used to set the output width and different width values are set to keep the format aligned.
    oss << std::left << std::setw(WINDOW_NAME_MAX_WIDTH) << windowName
        << std::left << std::setw(DISPLAY_NAME_MAX_WIDTH) << displayId
        << std::left << std::setw(PID_MAX_WIDTH) << session->GetCallingPid()
        << std::left << std::setw(PARENT_ID_MAX_WIDTH) << session->GetPersistentId()
        << std::left << std::setw(VALUE_MAX_WIDTH) << static_cast<uint32_t>(session->GetWindowType())
        << std::left << std::setw(VALUE_MAX_WIDTH) << static_cast<uint32_t>(session->GetWindowMode())
        << std::left << std::setw(VALUE_MAX_WIDTH) << flag
        << std::left << std::setw(VALUE_MAX_WIDTH) << zOrder
        << std::left << std::setw(ORIEN_MAX_WIDTH) << orientation
        << "[ "
        << std::left << std::setw(VALUE_MAX_WIDTH) << rect.posX_
        << std::left << std::setw(VALUE_MAX_WIDTH) << rect.posY_
        << std::left << std::setw(VALUE_MAX_WIDTH) << rect.width_
        << std::left << std::setw(VALUE_MAX_WIDTH) << rect.height_
        << "]"
        << " [ "
        << std::left << std::setw(OFFSET_MAX_WIDTH) << session->GetOffsetX()
        << std::left << std::setw(OFFSET_MAX_WIDTH) << session->GetOffsetY()
        << "]"
        << " [ "
        << std::left << std::setw(SCALE_MAX_WIDTH) << session->GetScaleX()
        << std::left << std::setw(SCALE_MAX_WIDTH) << session->GetScaleY()
        << std::left << std::setw(SCALE_MAX_WIDTH) << session->GetPivotX()
        << std::left << std::setw(SCALE_MAX_WIDTH) << session->GetPivotY()
        << "]"
        << std::endl;
}

WSError SceneSessionManager::GetAllSessionDumpInfo(std::string& dumpInfo)
{
    int32_t screenGroupId = 0;
    std::ostringstream oss;
    oss << "-------------------------------------ScreenGroup " << screenGroupId
        << "-------------------------------------" << std::endl;
    oss << "WindowName           DisplayId Pid     WinId Type Mode Flag ZOrd Orientation [ x    y    w    h    ]"
        << " [ OffsetX OffsetY ] [ ScaleX  ScaleY  PivotX  PivotY  ]"
        << std::endl;

    std::vector<sptr<SceneSession>> allSession;
    std::vector<sptr<SceneSession>> backgroundSession;
    std::map<int32_t, sptr<SceneSession>> sceneSessionMapCopy;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        sceneSessionMapCopy = sceneSessionMap_;
    }
    for (const auto& elem : sceneSessionMapCopy) {
        auto curSession = elem.second;
        if (curSession == nullptr) {
            continue;
        }
        if (IsSessionVisibleForeground(curSession)) {
            allSession.push_back(curSession);
        } else {
            backgroundSession.push_back(curSession);
        }
    }
    allSession.insert(allSession.end(), backgroundSession.begin(), backgroundSession.end());
    uint32_t count = 0;
    for (const auto& session : allSession) {
        if (session == nullptr) {
            continue;
        }
        if (count == static_cast<uint32_t>(allSession.size() - backgroundSession.size())) {
            oss << "---------------------------------------------------------------------------------------"
                << std::endl;
        }
        DumpSessionInfo(session, oss);
        count++;
    }
    oss << "Focus window: " << GetFocusedSessionId() << std::endl;
    oss << "Total window num: " << sceneSessionMapCopy.size() << std::endl;
    dumpInfo.append(oss.str());
    return WSError::WS_OK;
}

WSError SceneSessionManager::GetAllSessionDumpDetailInfo(std::string& dumpInfo)
{
    std::ostringstream oss;
    std::vector<sptr<SceneSession>> allSession;
    std::vector<sptr<SceneSession>> backgroundSession;

    std::map<int32_t, sptr<SceneSession>> sceneSessionMapCopy;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        sceneSessionMapCopy = sceneSessionMap_;
    }
    for (const auto& elem : sceneSessionMapCopy) {
        auto curSession = elem.second;
        if (curSession == nullptr) {
            continue;
        }
        if (IsSessionVisibleForeground(curSession)) {
            allSession.push_back(curSession);
        } else {
            backgroundSession.push_back(curSession);
        }
    }
    allSession.insert(allSession.end(), backgroundSession.begin(), backgroundSession.end());
    HidumpController::GetInstance().GetAllSessionDumpDetailedInfo(oss, allSession, backgroundSession);
    dumpInfo.append(oss.str());
    return WSError::WS_OK;
}

void SceneSessionManager::SetDumpRootSceneElementInfoListener(const DumpRootSceneElementInfoFunc& func)
{
    dumpRootSceneFunc_ = func;
}

void SceneSessionManager::DumpSessionElementInfo(const sptr<SceneSession>& session,
    const std::vector<std::string>& params, std::string& dumpInfo)
{
    std::vector<std::string> resetParams;
    resetParams.assign(params.begin() + 2, params.end()); // 2: params num
    if (resetParams.empty()) {
        WLOGI("do not dump ui info");
        return;
    }

    if (!session->GetSessionInfo().isSystem_) {
        WLOGFI("Dump normal session, not system");
        dumpInfoFuture_.ResetLock({});
        session->DumpSessionElementInfo(resetParams);
        std::vector<std::string> infos = dumpInfoFuture_.GetResult(2000); // 2000: wait for 2000ms
        for (auto& info: infos) {
            dumpInfo.append(info).append("\n");
        }
    } else {
        WLOGFI("Dump system session");
        std::vector<std::string> infos;
        dumpRootSceneFunc_(resetParams, infos);
        for (auto& info: infos) {
            dumpInfo.append(info).append("\n");
        }
    }
}

WSError SceneSessionManager::GetSpecifiedSessionDumpInfo(std::string& dumpInfo, const std::vector<std::string>& params,
    const std::string& strId)
{
    uint64_t persistentId = std::stoull(strId);
    auto session = GetSceneSession(persistentId);
    if (session == nullptr) {
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    auto sessionProperty = session->GetSessionProperty();
    if (sessionProperty == nullptr) {
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    WSRect rect = session->GetSessionRect();
    std::string isVisible = session->IsVisible() ? "true" : "false";
    std::string focusable = session->GetFocusable() ? "true" : "false";
    std::string decoStatus = sessionProperty->IsDecorEnable() ? "true" : "false";
    bool privacyMode = sessionProperty->GetSystemPrivacyMode() || sessionProperty->GetPrivacyMode();
    std::string isPrivacyMode = privacyMode ? "true" : "false";
    bool isFirstFrameAvailable = true;
    std::ostringstream oss;
    oss << "WindowName: " << session->GetWindowName()  << std::endl;
    oss << "DisplayId: " << 0 << std::endl;
    oss << "WinId: " << session->GetPersistentId() << std::endl;
    oss << "Pid: " << session->GetCallingPid() << std::endl;
    oss << "Type: " << static_cast<uint32_t>(session->GetWindowType()) << std::endl;
    oss << "Mode: " << static_cast<uint32_t>(session->GetWindowMode()) << std::endl;
    oss << "Flag: " << sessionProperty->GetWindowFlags() << std::endl;
    oss << "Orientation: " << static_cast<uint32_t>(session->GetRequestedOrientation()) << std::endl;
    oss << "FirstFrameCallbackCalled: " << isFirstFrameAvailable << std::endl;
    oss << "IsVisible: " << isVisible << std::endl;
    oss << "Focusable: "  << focusable << std::endl;
    oss << "DecoStatus: "  << decoStatus << std::endl;
    oss << "isPrivacyMode: "  << isPrivacyMode << std::endl;
    oss << "WindowRect: " << "[ "
        << rect.posX_ << ", " << rect.posY_ << ", " << rect.width_ << ", " << rect.height_
        << " ]" << std::endl;
    oss << "scaleX: " << session->GetScaleX() << std::endl;
    oss << "scaleY: " << session->GetScaleY() << std::endl;
    oss << "Offset: " << "[ "
        << session->GetOffsetX() << ", " << session->GetOffsetY() << " ]" << std::endl;
    oss << "Scale: " << "[ "
        << session->GetScaleX() << ", " << session->GetScaleY() << ", "
        << session->GetPivotX() << ", " << session->GetPivotY()
        << " ]" << std::endl;
    dumpInfo.append(oss.str());

    DumpSessionElementInfo(session, params, dumpInfo);
    return WSError::WS_OK;
}

WSError SceneSessionManager::GetSCBDebugDumpInfo(std::string&& cmd, std::string& dumpInfo)
{
    // publish data
    bool ret = eventHandler_->PostSyncTask(
        [this, cmd = std::move(cmd)] { return scbDumpSubscriber_->Publish(cmd); }, "PublishSCBDumper");
    if (!ret) {
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    // get response event
    auto task = [this, &dumpInfo] {
        dumpInfo.append(scbDumpSubscriber_->GetDebugDumpInfo(WAIT_TIME));
        return WSError::WS_OK;
    };
    eventHandler_->PostSyncTask(task, "GetDataSCBDumper");
    return WSError::WS_OK;
}

void SceneSessionManager::NotifyDumpInfoResult(const std::vector<std::string>& info)
{
    dumpInfoFuture_.SetValue(info);
    WLOGFD("NotifyDumpInfoResult");
}

WSError SceneSessionManager::GetSessionDumpInfo(const std::vector<std::string>& params, std::string& dumpInfo)
{
    if (!(SessionPermission::IsSACalling() || SessionPermission::IsStartByHdcd())) {
        WLOGFE("GetSessionDumpInfo permission denied!");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, params, &dumpInfo]() {
        if (params.size() == 1 && params[0] == ARG_DUMP_ALL) { // 1: params num
            return GetAllSessionDumpInfo(dumpInfo);
        }
        if (params.size() == 1 && params[0] == ARG_DUMP_DETAIL) { // 1: params num
            return GetAllSessionDumpDetailInfo(dumpInfo);
        }
        if (params.size() >= 2 && params[0] == ARG_DUMP_WINDOW && IsValidDigitString(params[1])) { // 2: params num
            return GetSpecifiedSessionDumpInfo(dumpInfo, params, params[1]);
        }
        if (params.size() >= 2 && params[0] == ARG_DUMP_SCB) { // 2: params num
            std::string cmd;
            std::for_each(params.begin() + 1, params.end(),
                          [&cmd](const std::string& value) {
                              cmd += value;
                              cmd += ' ';
                          });
            return GetSCBDebugDumpInfo(std::move(cmd), dumpInfo);
        }
        if (params.size() >= 2 && params[0] == ARG_DUMP_PIPLINE && IsValidDigitString(params[1])) { // 2: params num
            return GetTotalUITreeInfo(params[1], dumpInfo);
        }
        return WSError::WS_ERROR_INVALID_OPERATION;
    };
    return taskScheduler_->PostSyncTask(task);
}

WSError SceneSessionManager::GetTotalUITreeInfo(const std::string& strId, std::string& dumpInfo)
{
    TLOGI(WmsLogTag::WMS_PIPELINE, "begin");
    uint64_t screenId = std::stoull(strId);
    if (dumpUITreeFunc_) {
        dumpUITreeFunc_(screenId, dumpInfo);
    } else {
        TLOGE(WmsLogTag::WMS_PIPELINE, "dumpUITreeFunc is null");
    }
    return WSError::WS_OK;
}

void SceneSessionManager::SetDumpUITreeFunc(const DumpUITreeFunc& func)
{
    dumpUITreeFunc_ = func;
}

void FocusIDChange(int32_t persistentId, sptr<SceneSession>& sceneSession)
{
    // notify RS
    WLOGFD("current focus session: windowId: %{public}d, windowName: %{public}s, bundleName: %{public}s,"
        " abilityName: %{public}s, pid: %{public}d, uid: %{public}d", persistentId,
        sceneSession->GetSessionProperty()->GetWindowName().c_str(),
        sceneSession->GetSessionInfo().bundleName_.c_str(),
        sceneSession->GetSessionInfo().abilityName_.c_str(),
        sceneSession->GetCallingPid(), sceneSession->GetCallingUid());
    uint64_t focusNodeId = 0; // 0 means invalid
    if (sceneSession->GetSurfaceNode() == nullptr) {
        WLOGFW("focused window surfaceNode is null");
    } else {
        focusNodeId = sceneSession->GetSurfaceNode()->GetId();
    }
    FocusAppInfo appInfo = {
        sceneSession->GetCallingPid(), sceneSession->GetCallingUid(),
        sceneSession->GetSessionInfo().bundleName_,
        sceneSession->GetSessionInfo().abilityName_, focusNodeId};
    RSInterfaces::GetInstance().SetFocusAppInfo(appInfo);
}

// ordered vector by compare func
std::vector<std::pair<int32_t, sptr<SceneSession>>> SceneSessionManager::GetSceneSessionVector(CmpFunc cmp)
{
    std::vector<std::pair<int32_t, sptr<SceneSession>>> ret;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto& iter : sceneSessionMap_) {
            ret.push_back(iter);
        }
    }
    std::sort(ret.begin(), ret.end(), cmp);
    return ret;
}

void SceneSessionManager::TraverseSessionTree(TraverseFunc func, bool isFromTopToBottom)
{
    if (isFromTopToBottom) {
        TraverseSessionTreeFromTopToBottom(func);
    } else {
        TraverseSessionTreeFromBottomToTop(func);
    }
    return;
}

void SceneSessionManager::TraverseSessionTreeFromTopToBottom(TraverseFunc func)
{
    CmpFunc cmp = [](std::pair<int32_t, sptr<SceneSession>>& lhs, std::pair<int32_t, sptr<SceneSession>>& rhs) {
        uint32_t lhsZOrder = lhs.second != nullptr ? lhs.second->GetZOrder() : 0;
        uint32_t rhsZOrder = rhs.second != nullptr ? rhs.second->GetZOrder() : 0;
        return lhsZOrder < rhsZOrder;
    };
    auto sceneSessionVector = GetSceneSessionVector(cmp);

    for (auto iter = sceneSessionVector.rbegin(); iter != sceneSessionVector.rend(); ++iter) {
        auto session = iter->second;
        if (session == nullptr) {
            WLOGFE("session is nullptr");
            continue;
        }
        if (func(session)) {
            return;
        }
    }
    return;
}

void SceneSessionManager::TraverseSessionTreeFromBottomToTop(TraverseFunc func)
{
    // std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    CmpFunc cmp = [](std::pair<int32_t, sptr<SceneSession>>& lhs, std::pair<int32_t, sptr<SceneSession>>& rhs) {
        uint32_t lhsZOrder = lhs.second != nullptr ? lhs.second->GetZOrder() : 0;
        uint32_t rhsZOrder = rhs.second != nullptr ? rhs.second->GetZOrder() : 0;
        return lhsZOrder < rhsZOrder;
    };
    auto sceneSessionVector = GetSceneSessionVector(cmp);
    // std::map<int32_t, sptr<SceneSession>>::iterator iter;
    for (auto iter = sceneSessionVector.begin(); iter != sceneSessionVector.end(); ++iter) {
        auto session = iter->second;
        if (session == nullptr) {
            WLOGFE("session is nullptr");
            continue;
        }
        if (func(session)) {
            return;
        }
    }
    return;
}

WMError SceneSessionManager::RequestFocusStatus(int32_t persistentId, bool isFocused, bool byForeground,
    FocusChangeReason reason)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "id: %{public}d, reason: %{public}d", persistentId, reason);
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_FOCUS, "sceneSession is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    int32_t callingPid = IPCSkeleton::GetCallingPid();
    if (callingPid != sceneSession->GetCallingPid() &&
        !SessionPermission::IsSameAppAsCalling(SCENE_BOARD_BUNDLE_NAME, SCENE_BOARD_APP_IDENTIFIER)) {
        TLOGE(WmsLogTag::WMS_FOCUS, "permission denied, not call by the same process");
        return WMError::WM_ERROR_INVALID_CALLING;
    }
    auto task = [this, persistentId, isFocused, byForeground, reason]() {
        if (isFocused) {
            RequestSessionFocus(persistentId, byForeground, reason);
        } else {
            RequestSessionUnfocus(persistentId, reason);
        }
    };
    taskScheduler_->PostAsyncTask(task, "RequestFocusStatus" + std::to_string(persistentId));
    focusChangeReason_ = reason;
    return WMError::WM_OK;
}

WMError SceneSessionManager::RequestFocusStatusBySCB(int32_t persistentId, bool isFocused, bool byForeground,
    FocusChangeReason reason)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "id: %{public}d, reason: %{public}d", persistentId, reason);
    auto task = [this, persistentId, isFocused, byForeground, reason]() {
        if (isFocused) {
            if (reason == FocusChangeReason::FOREGROUND) {
                RequestSessionFocusImmediately(persistentId);
                return;
            }
            if (reason == FocusChangeReason::MOVE_UP) {
                auto session = GetSceneSession(persistentId);
                if (session && !session->IsFocused()) {
                    PostProcessFocusState state = { true, true, reason };
                    session->SetPostProcessFocusState(state);
                }
                return;
            }
            if (reason == FocusChangeReason::CLICK) {
                return;
            }
            if (RequestSessionFocus(persistentId, byForeground, reason) != WSError::WS_OK) {
                auto session = GetSceneSession(persistentId);
                if (session && !session->IsFocused()) {
                    PostProcessFocusState state = { true, true, reason };
                    session->SetPostProcessFocusState(state);
                }
            }
        } else {
            RequestSessionUnfocus(persistentId, reason);
        }
    };
    taskScheduler_->PostAsyncTask(task, "RequestFocusStatusBySCB" + std::to_string(persistentId));
    return WMError::WM_OK;
}

void SceneSessionManager::RequestAllAppSessionUnfocus()
{
    auto task = [this]() {
        RequestAllAppSessionUnfocusInner();
    };
    taskScheduler_->PostAsyncTask(task, "RequestAllAppSessionUnfocus");
    return;
}

/**
 * request focus and ignore its state
 * only used when app main window start before foreground
*/
WSError SceneSessionManager::RequestSessionFocusImmediately(int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "RequestSessionFocusImmediately, id: %{public}d", persistentId);
    // base block
    WSError basicCheckRet = RequestFocusBasicCheck(persistentId);
    if (basicCheckRet != WSError::WS_OK) {
        return basicCheckRet;
    }
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("[WMSComm]session is nullptr");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (!sceneSession->CheckFocusable()) {
        TLOGD(WmsLogTag::WMS_FOCUS, "session is not focusable!");
        return WSError::WS_DO_NOTHING;
    }
    if (!sceneSession->IsFocusedOnShow()) {
        TLOGD(WmsLogTag::WMS_FOCUS, "session is not focused on show!");
        return WSError::WS_DO_NOTHING;
    }

    // specific block
    FocusChangeReason reason = FocusChangeReason::SCB_START_APP;
    WSError specificCheckRet = RequestFocusSpecificCheck(sceneSession, true, reason);
    if (specificCheckRet != WSError::WS_OK) {
        return specificCheckRet;
    }

    needBlockNotifyUnfocusStatus_ = needBlockNotifyFocusStatusUntilForeground_;
    if (!sceneSession->GetSessionInfo().isSystem_ && !IsSessionVisibleForeground(sceneSession)) {
        needBlockNotifyFocusStatusUntilForeground_ = true;
    }
    ShiftFocus(sceneSession, reason);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSessionFocus(int32_t persistentId, bool byForeground, FocusChangeReason reason)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "id: %{public}d, by foreground: %{public}d, reason: %{public}d",
        persistentId, byForeground, reason);
    WSError basicCheckRet = RequestFocusBasicCheck(persistentId);
    if (basicCheckRet != WSError::WS_OK) {
        return basicCheckRet;
    }
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("[WMSComm]session is nullptr");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (!sceneSession->CheckFocusable() || !IsSessionVisibleForeground(sceneSession)) {
        TLOGD(WmsLogTag::WMS_FOCUS, "session is not focusable or not visible!");
        return WSError::WS_DO_NOTHING;
    }
    if (!sceneSession->IsFocusedOnShow()) {
        TLOGD(WmsLogTag::WMS_FOCUS, "session is not focused on show!");
        return WSError::WS_DO_NOTHING;
    }

    // subwindow/dialog state block
    if ((WindowHelper::IsSubWindow(sceneSession->GetWindowType()) ||
        sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) &&
        GetSceneSession(sceneSession->GetParentPersistentId()) &&
        !IsSessionVisibleForeground(GetSceneSession(sceneSession->GetParentPersistentId()))) {
            TLOGD(WmsLogTag::WMS_FOCUS, "parent session id: %{public}d is not visible!",
                sceneSession->GetParentPersistentId());
            return WSError::WS_DO_NOTHING;
    }
    // specific block
    WSError specificCheckRet = RequestFocusSpecificCheck(sceneSession, byForeground, reason);
    if (specificCheckRet != WSError::WS_OK) {
        return specificCheckRet;
    }

    needBlockNotifyUnfocusStatus_ = needBlockNotifyFocusStatusUntilForeground_;
    needBlockNotifyFocusStatusUntilForeground_ = false;
    ShiftFocus(sceneSession, reason);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSessionUnfocus(int32_t persistentId, FocusChangeReason reason)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "RequestSessionUnfocus, id: %{public}d", persistentId);
    if (persistentId == INVALID_SESSION_ID) {
        WLOGFE("id is invalid");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    auto focusedSession = GetSceneSession(focusedSessionId_);
    if (persistentId != focusedSessionId_ &&
        !(focusedSession && focusedSession->GetParentPersistentId() == persistentId)) {
        TLOGD(WmsLogTag::WMS_FOCUS, "session unfocused!");
        return WSError::WS_DO_NOTHING;
    }
    // if pop menu created by desktop request unfocus, back to desktop
    auto lastSession = GetSceneSession(lastFocusedSessionId_);
    if (focusedSession && focusedSession->GetWindowType() == WindowType::WINDOW_TYPE_SYSTEM_FLOAT &&
        lastSession && lastSession->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP &&
        RequestSessionFocus(lastFocusedSessionId_, false) == WSError::WS_OK) {
            TLOGD(WmsLogTag::WMS_FOCUS, "focus is back to desktop");
            return WSError::WS_OK;
    }
    auto nextSession = GetNextFocusableSession(persistentId);
    if (nextSession == nullptr) {
        DumpAllSessionFocusableInfo(persistentId);
    }

    needBlockNotifyUnfocusStatus_ = needBlockNotifyFocusStatusUntilForeground_;
    needBlockNotifyFocusStatusUntilForeground_ = false;

    if (CheckLastFocusedAppSessionFocus(focusedSession, nextSession)) {
        return WSError::WS_OK;
    }

    return ShiftFocus(nextSession, reason);
}

WSError SceneSessionManager::RequestAllAppSessionUnfocusInner()
{
    TLOGI(WmsLogTag::WMS_FOCUS, "RequestAllAppSessionUnfocus");
    auto focusedSession = GetSceneSession(focusedSessionId_);
    if (!focusedSession) {
        TLOGE(WmsLogTag::WMS_FOCUS, "focused session is null");
        return WSError::WS_DO_NOTHING;
    }
    if (!focusedSession->IsAppSession()) {
        WLOGW("[WMFocus]Focused session is non app session: %{public}d", focusedSessionId_);
        return WSError::WS_DO_NOTHING;
    }
    auto nextSession = GetTopFocusableNonAppSession();

    needBlockNotifyUnfocusStatus_ = needBlockNotifyFocusStatusUntilForeground_;
    needBlockNotifyFocusStatusUntilForeground_ = false;
    return ShiftFocus(nextSession, FocusChangeReason::WIND);
}

WSError SceneSessionManager::RequestFocusBasicCheck(int32_t persistentId)
{
    // basic focus rule
    if (persistentId == INVALID_SESSION_ID) {
        TLOGE(WmsLogTag::WMS_FOCUS, "id is invalid!");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (persistentId == focusedSessionId_) {
        TLOGD(WmsLogTag::WMS_FOCUS, "request id has been focused!");
        return WSError::WS_DO_NOTHING;
    }
    return WSError::WS_OK;
}

/**
 * @note @window.focus
 * When high zOrder System Session unfocus, check if the last focused app window can focus.
 */
bool SceneSessionManager::CheckLastFocusedAppSessionFocus(
    sptr<SceneSession>& focusedSession, sptr<SceneSession>& nextSession)
{
    if (focusedSession == nullptr || nextSession == nullptr) {
        return false;
    }

    TLOGI(WmsLogTag::WMS_FOCUS, "lastFocusedAppSessionId: %{public}d, nextSceneSession: %{public}d",
        lastFocusedAppSessionId_, nextSession->GetPersistentId());

    if (lastFocusedAppSessionId_ == INVALID_SESSION_ID || nextSession->GetPersistentId() == lastFocusedAppSessionId_) {
        return false;
    }

    if (!focusedSession->IsSystemSessionAboveApp()) {
        return false;
    }

    auto mode = nextSession->GetWindowMode();
    // only when next session is app, and in split or floation
    if (nextSession->IsAppSession() &&
        (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY ||
         mode == WindowMode::WINDOW_MODE_FLOATING)) {
        if (RequestSessionFocus(lastFocusedAppSessionId_, false, FocusChangeReason::LAST_FOCUSED_APP) ==
            WSError::WS_OK) {
            return true;
        }
        lastFocusedAppSessionId_ = INVALID_SESSION_ID;
    }
    return false;
}

/**
 * When switching focus, check if the blockingType window has been  traversed downwards.
 *
 * @return true: traversed downwards, false: not.
 */
bool SceneSessionManager::CheckFocusIsDownThroughBlockingType(sptr<SceneSession>& requestSceneSession,
    sptr<SceneSession>& focusedSession, bool includingAppSession)
{
    uint32_t requestSessionZOrder = requestSceneSession->GetZOrder();
    uint32_t focusedSessionZOrder = focusedSession->GetZOrder();
    TLOGD(WmsLogTag::WMS_FOCUS, "requestSessionZOrder: %{public}d, focusedSessionZOrder: %{public}d",
        requestSessionZOrder, focusedSessionZOrder);
    if  (requestSessionZOrder < focusedSessionZOrder)  {
        auto topNearestBlockingFocusSession = GetTopNearestBlockingFocusSession(requestSessionZOrder,
            includingAppSession);
        uint32_t topNearestBlockingZOrder = 0;
        if  (topNearestBlockingFocusSession)  {
            topNearestBlockingZOrder = topNearestBlockingFocusSession->GetZOrder();
            TLOGD(WmsLogTag::WMS_FOCUS,  "requestSessionZOrder: %{public}d, focusedSessionZOrder:  %{public}d\
                topNearestBlockingZOrder:  %{public}d",  requestSessionZOrder,  focusedSessionZOrder,
                topNearestBlockingZOrder);
        }
        if  (focusedSessionZOrder >=  topNearestBlockingZOrder && requestSessionZOrder < topNearestBlockingZOrder)  {
            TLOGD(WmsLogTag::WMS_FOCUS,  "focus pass through, needs to be intercepted");
            return true;
        }
    }
    TLOGD(WmsLogTag::WMS_FOCUS, "not through");
    return false;
}

bool SceneSessionManager::CheckTopmostWindowFocus(sptr<SceneSession>& focusedSession, sptr<SceneSession>& sceneSession)
{
    bool isFocusedMainSessionTopmost =
        focusedSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW && focusedSession->IsTopmost();
    auto parentSession = GetSceneSession(focusedSession->GetParentPersistentId());
    bool isFocusedSessionParentTopmost = parentSession &&
        parentSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW && parentSession->IsTopmost();
    if ((isFocusedMainSessionTopmost || isFocusedSessionParentTopmost) && sceneSession->IsAppSession() &&
        (sceneSession->GetMissionId() != focusedSession->GetMissionId())) {
        return true;
    }
    return false;
}

bool SceneSessionManager::CheckRequestFocusImmdediately(sptr<SceneSession>& sceneSession)
{
    if ((sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW ||
         (SessionHelper::IsSubWindow(sceneSession->GetWindowType()) && !sceneSession->IsModal())) &&
        (ProcessModalTopmostRequestFocusImmdediately(sceneSession) == WSError::WS_OK ||
         ProcessDialogRequestFocusImmdediately(sceneSession) == WSError::WS_OK)) {
        TLOGD(WmsLogTag::WMS_FOCUS, "dialog or modal subwindow get focused");
        return true;
    }
    return false;
}

bool SceneSessionManager::CheckClickFocusIsDownThroughFullScreen(const sptr<SceneSession>& focusedSession,
    const sptr<SceneSession>& sceneSession, FocusChangeReason reason)
{
    if (focusedSession->GetWindowType() != WindowType::WINDOW_TYPE_GLOBAL_SEARCH &&
        focusedSession->GetWindowType() != WindowType::WINDOW_TYPE_NEGATIVE_SCREEN) {
        return false;
    }
    if (reason != FocusChangeReason::CLICK || !focusedSession->GetBlockingFocus()) {
        return false;
    }
    return sceneSession->GetZOrder() < focusedSession->GetZOrder();
}

WSError SceneSessionManager::RequestFocusSpecificCheck(sptr<SceneSession>& sceneSession, bool byForeground,
    FocusChangeReason reason)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "FocusChangeReason: %{public}d", reason);
    int32_t persistentId = sceneSession->GetPersistentId();
    if (sceneSession->GetForceHideState() != ForceHideState::NOT_HIDDEN) {
        TLOGD(WmsLogTag::WMS_FOCUS, "the window hide id: %{public}d", persistentId);
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    // dialog get focus
    if (CheckRequestFocusImmdediately(sceneSession)) {
        return WSError::WS_DO_NOTHING;
    }
    // blocking-type session will block lower zOrder request focus
    auto focusedSession = GetSceneSession(focusedSessionId_);
    if (focusedSession) {
        TLOGD(WmsLogTag::WMS_FOCUS, "reason: %{public}d, byForeground: %{public}d",  reason,
            byForeground);
        if (CheckTopmostWindowFocus(focusedSession, sceneSession)) {
            // return ok if focused session is topmost
            return WSError::WS_OK;
        }
        if (reason == FocusChangeReason::CLIENT_REQUEST && sceneSession->IsAppSession() &&
            sceneSession->GetMissionId() == focusedSession->GetMissionId()) {
            TLOGD(WmsLogTag::WMS_FOCUS, "client request from the same app, skip blocking check");
            byForeground = false;
        }
        if (byForeground && CheckFocusIsDownThroughBlockingType(sceneSession,  focusedSession,  true))  {
            TLOGD(WmsLogTag::WMS_FOCUS, "check, need to be intercepted");
            return WSError::WS_DO_NOTHING;
        }
        if ((reason == FocusChangeReason::SPLIT_SCREEN || reason == FocusChangeReason::FLOATING_SCENE) &&
            !byForeground)  {
            if (!CheckFocusIsDownThroughBlockingType(sceneSession, focusedSession, false)
                && focusedSession->IsAppSession()) {
                TLOGD(WmsLogTag::WMS_FOCUS, "in split or floting , ok");
                return WSError::WS_OK;
            }
        }
        bool isBlockingType = focusedSession->IsAppSession() ||
            (focusedSession->GetSessionInfo().isSystem_ && focusedSession->GetBlockingFocus());
        // temp check
        if (isBlockingType && focusedSession->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD &&
            sceneSession->GetZOrder() < focusedSession->GetZOrder()) {
                TLOGD(WmsLogTag::WMS_FOCUS, "Lower session %{public}d cannot request focus from keyguard!",
                    persistentId);
                return WSError::WS_DO_NOTHING;
        }
        // desktop click temp check
        if (CheckClickFocusIsDownThroughFullScreen(focusedSession, sceneSession, reason)) {
            TLOGD(WmsLogTag::WMS_FOCUS, "click cannot request focus from full screen window!");
            return WSError::WS_DO_NOTHING;
        }
    }
    return WSError::WS_OK;
}

bool SceneSessionManager::CheckParentSessionVisible(const sptr<SceneSession>& session)
{
    if ((WindowHelper::IsSubWindow(session->GetWindowType()) ||
        session->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) &&
        GetSceneSession(session->GetParentPersistentId()) &&
        !IsSessionVisible(GetSceneSession(session->GetParentPersistentId()))) {
        return false;
    }
    return true;
}

void SceneSessionManager::DumpAllSessionFocusableInfo(int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "id: %{public}d", persistentId);
    auto func = [this](sptr<SceneSession> session) {
        if (session == nullptr) {
            return false;
        }
        bool parentVisible = CheckParentSessionVisible(session);
        bool sessionVisible = IsSessionVisible(session);
        TLOGI(WmsLogTag::WMS_FOCUS, "%{public}d, winType:%{public}d, hide:%{public}d, "
            "focusable:%{public}d, visible:%{public}d, parentVisible:%{public}d",
            session->GetPersistentId(), session->GetWindowType(), session->GetForceHideState(),
            session->GetFocusable(), sessionVisible, parentVisible);
        return false;
    };
    TraverseSessionTree(func, true);
}

sptr<SceneSession> SceneSessionManager::GetNextFocusableSession(int32_t persistentId)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "GetNextFocusableSession, id: %{public}d", persistentId);
    bool previousFocusedSessionFound = false;
    sptr<SceneSession> ret = nullptr;
    auto func = [this, persistentId, &previousFocusedSessionFound, &ret](sptr<SceneSession> session) {
        if (session == nullptr) {
            return false;
        }
        if (session->GetForceHideState() != ForceHideState::NOT_HIDDEN) {
            TLOGD(WmsLogTag::WMS_FOCUS, "the window hide id: %{public}d", persistentId);
            return false;
        }
        if (previousFocusedSessionFound && session->CheckFocusable() &&
            IsSessionVisibleForeground(session) && CheckParentSessionVisible(session)) {
            ret = session;
            return true;
        }
        if (session->GetPersistentId() == persistentId) {
            previousFocusedSessionFound = true;
        }
        return false;
    };
    TraverseSessionTree(func, true);
    return ret;
}

/**
 * Find the session through the specific zOrder, it is located abve it, its' blockingFocus attribute is true,
 * and it is the closest;
 */
sptr<SceneSession> SceneSessionManager::GetTopNearestBlockingFocusSession(uint32_t zOrder, bool includingAppSession)
{
    sptr<SceneSession> ret = nullptr;
    auto func = [this, &ret, zOrder, includingAppSession](sptr<SceneSession> session) {
        if (session == nullptr) {
            return false;
        }
        uint32_t sessionZOrder = session->GetZOrder();
        if (sessionZOrder <= zOrder) { // must be above the target session
            return false;
        }
        if (session->IsTopmost() && session->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            TLOGD(WmsLogTag::WMS_FOCUS, "topmost window do not block");
            return false;
        }
        auto parentSession = GetSceneSession(session->GetParentPersistentId());
        if (SessionHelper::IsSubWindow(session->GetWindowType()) && parentSession != nullptr &&
            parentSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
            parentSession->IsTopmost()) {
            TLOGD(WmsLogTag::WMS_FOCUS, "sub window of topmost do not block");
            return false;
        }
        bool isPhoneOrPad = systemConfig_.IsPhoneWindow() || systemConfig_.IsPadWindow();
        bool isBlockingType = (includingAppSession && session->IsAppSession()) ||
                              (session->GetSessionInfo().isSystem_ && session->GetBlockingFocus()) ||
                              (isPhoneOrPad && session->GetWindowType() == WindowType::WINDOW_TYPE_VOICE_INTERACTION);
        if (IsSessionVisibleForeground(session) && isBlockingType)  {
            ret = session;
            return true;
        }
        return false;
    };
    TraverseSessionTree(func, false);
    return ret;
}

sptr<SceneSession> SceneSessionManager::GetTopFocusableNonAppSession()
{
    TLOGD(WmsLogTag::WMS_FOCUS, "GetTopFocusableNonAppSession.");
    sptr<SceneSession> ret = nullptr;
    auto func = [this, &ret](sptr<SceneSession> session) {
        if (session == nullptr) {
            return false;
        }
        if (session->IsAppSession()) {
            return true;
        }
        if (session->CheckFocusable() && IsSessionVisibleForeground(session)) {
            ret = session;
        }
        return false;
    };
    TraverseSessionTree(func, false);
    return ret;
}

void SceneSessionManager::SetShiftFocusListener(const ProcessShiftFocusFunc& func)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "SetShiftFocusListener");
    shiftFocusFunc_ = func;
}

void SceneSessionManager::SetSCBFocusedListener(const NotifySCBAfterUpdateFocusFunc& func)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "SetSCBFocusedListener");
    notifySCBAfterFocusedFunc_ = func;
}

void SceneSessionManager::SetSCBUnfocusedListener(const NotifySCBAfterUpdateFocusFunc& func)
{
    TLOGD(WmsLogTag::WMS_FOCUS, "SetSCBUnfocusedListener");
    notifySCBAfterUnfocusedFunc_ = func;
}

void SceneSessionManager::SetCallingSessionIdSessionListenser(const ProcessCallingSessionIdChangeFunc& func)
{
    WLOGFD("SetCallingSessionIdSessionListenser");
    callingSessionIdChangeFunc_ = func;
}

void SceneSessionManager::SetStartUIAbilityErrorListener(const ProcessStartUIAbilityErrorFunc& func)
{
    WLOGFD("SetStartUIAbilityErrorListener");
    startUIAbilityErrorFunc_ = func;
}

void SceneSessionManager::SetAbilityManagerCollaboratorRegisteredFunc(
    const AbilityManagerCollaboratorRegisteredFunc& func)
{
    auto task = [this, func] {
        abilityManagerCollaboratorRegisteredFunc_ = func;
    };
    taskScheduler_->PostAsyncTask(task, __func__);
}

WSError SceneSessionManager::ShiftFocus(sptr<SceneSession>& nextSession, FocusChangeReason reason)
{
    // unfocus
    int32_t focusedId = focusedSessionId_;
    auto focusedSession = GetSceneSession(focusedSessionId_);
    UpdateFocusStatus(focusedSession, false);
    // focus
    int32_t nextId = INVALID_SESSION_ID;
    if (nextSession == nullptr) {
        std::string sessionLog(GetAllSessionFocusInfo());
        TLOGW(WmsLogTag::WMS_FOCUS, "ShiftFocus to nullptr! id: %{public}d, info: %{public}s",
            focusedSessionId_, sessionLog.c_str());
    } else {
        nextId = nextSession->GetPersistentId();
    }
    UpdateFocusStatus(nextSession, true);
    bool scbPrevFocus = focusedSession && focusedSession->GetSessionInfo().isSystem_;
    bool scbCurrFocus = nextSession && nextSession->GetSessionInfo().isSystem_;
    AnomalyDetection::FocusCheckProcess(focusedId, nextId);
    if (!scbPrevFocus && scbCurrFocus) {
        if (notifySCBAfterFocusedFunc_ != nullptr) {
            notifySCBAfterFocusedFunc_();
        }
    } else if (scbPrevFocus && !scbCurrFocus) {
        if (notifySCBAfterUnfocusedFunc_ != nullptr) {
            notifySCBAfterUnfocusedFunc_();
        }
    }
    TLOGI(WmsLogTag::WMS_FOCUS, "focusedId: %{public}d, nextId: %{public}d, reason: %{public}d",
        focusedId, nextId, reason);
    return WSError::WS_OK;
}

void SceneSessionManager::UpdateFocusStatus(sptr<SceneSession>& sceneSession, bool isFocused)
{
    if (sceneSession == nullptr) {
        if (isFocused) {
            SetFocusedSessionId(INVALID_SESSION_ID);
            lastFocusedAppSessionId_ = INVALID_SESSION_ID;
        }
        return;
    }
    TLOGD(WmsLogTag::WMS_FOCUS, "UpdateFocusStatus, name: %{public}s, id: %{public}d, isFocused: %{public}d",
        sceneSession->GetWindowNameAllType().c_str(), sceneSession->GetPersistentId(), isFocused);
    // set focused
    if (isFocused) {
        SetFocusedSessionId(sceneSession->GetPersistentId());
        if (sceneSession->IsAppOrLowerSystemSession()) {
            lastFocusedAppSessionId_ = sceneSession->GetPersistentId();
        }
    }
    sceneSession->UpdateFocus(isFocused);
    if ((isFocused && !needBlockNotifyFocusStatusUntilForeground_) || (!isFocused && !needBlockNotifyUnfocusStatus_)) {
        NotifyFocusStatus(sceneSession, isFocused);
    }
}

void SceneSessionManager::NotifyFocusStatus(sptr<SceneSession>& sceneSession, bool isFocused)
{
    if (sceneSession == nullptr) {
        WLOGFE("[WMSComm]session is nullptr");
        if (isFocused) {
            auto prevSession = GetSceneSession(lastFocusedSessionId_);
            NotifyUnFocusedByMission(prevSession);
        }
        return;
    }
    int32_t persistentId = sceneSession->GetPersistentId();

    TLOGI(WmsLogTag::WMS_FOCUS,
        "name: %{public}s/%{public}s/%{public}s, id: %{public}d, isFocused: %{public}d",
        sceneSession->GetSessionInfo().bundleName_.c_str(),
        sceneSession->GetSessionInfo().abilityName_.c_str(),
        sceneSession->GetWindowNameAllType().c_str(),
        persistentId, isFocused);
    if (isFocused) {
        if (IsSessionVisibleForeground(sceneSession)) {
            NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_FOCUSED);
        }
        UpdateBrightness(focusedSessionId_);
        FocusIDChange(sceneSession->GetPersistentId(), sceneSession);
        if (shiftFocusFunc_ != nullptr) {
            shiftFocusFunc_(persistentId);
        }
    }
    // notify window manager
    sptr<FocusChangeInfo> focusChangeInfo = sptr<FocusChangeInfo>::MakeSptr(
        sceneSession->GetWindowId(),
        static_cast<DisplayId>(0),
        sceneSession->GetCallingPid(),
        sceneSession->GetCallingUid(),
        sceneSession->GetWindowType(),
        sceneSession->GetAbilityToken()
    );
    SceneSessionManager::NotifyRssThawApp(focusChangeInfo->uid_, "", "THAW_BY_FOCUS_CHANGED");
    SessionManagerAgentController::GetInstance().UpdateFocusChangeInfo(focusChangeInfo, isFocused);
    sceneSession->NotifyFocusStatus(isFocused);
    std::string sName = "FoucusWindow:";
    if (sceneSession->GetSessionInfo().isSystem_) {
        sName += sceneSession->GetSessionInfo().abilityName_;
    } else {
        sName += sceneSession->GetWindowName();
    }
    if (isFocused) {
        StartAsyncTrace(HITRACE_TAG_WINDOW_MANAGER, sName, sceneSession->GetPersistentId());
    } else {
        FinishAsyncTrace(HITRACE_TAG_WINDOW_MANAGER, sName, sceneSession->GetPersistentId());
    }

    // notify listenerController
    auto prevSession = GetSceneSession(lastFocusedSessionId_);
    if (isFocused && MissionChanged(prevSession, sceneSession)) {
        NotifyFocusStatusByMission(prevSession, sceneSession);
    }
}

int32_t SceneSessionManager::NotifyRssThawApp(const int32_t uid, const std::string& bundleName,
    const std::string& reason)
{
    uint32_t resType = ResourceSchedule::ResType::SYNC_RES_TYPE_THAW_ONE_APP;
    nlohmann::json payload;
    payload.emplace("uid", uid);
    payload.emplace("bundleName", bundleName);
    payload.emplace("reason", reason);
    nlohmann::json reply;
    int32_t ret = ResourceSchedule::ResSchedClient::GetInstance().ReportSyncEvent(resType, 0, payload, reply);
    return ret;
}

void SceneSessionManager::NotifyFocusStatusByMission(sptr<SceneSession>& prevSession, sptr<SceneSession>& currSession)
{
    if (prevSession && !prevSession->GetSessionInfo().isSystem_) {
        TLOGD(WmsLogTag::WMS_FOCUS, "Unfocused, id: %{public}d", prevSession->GetMissionId());
        listenerController_->NotifySessionUnfocused(prevSession->GetMissionId());
    }
    if (currSession && !currSession->GetSessionInfo().isSystem_) {
        TLOGD(WmsLogTag::WMS_FOCUS, "Focused, id: %{public}d", currSession->GetMissionId());
        listenerController_->NotifySessionFocused(currSession->GetMissionId());
    }
}

void SceneSessionManager::NotifyUnFocusedByMission(sptr<SceneSession>& sceneSession)
{
    if (sceneSession && !sceneSession->GetSessionInfo().isSystem_) {
        TLOGD(WmsLogTag::WMS_FOCUS, "id: %{public}d", sceneSession->GetMissionId());
        listenerController_->NotifySessionUnfocused(sceneSession->GetMissionId());
    }
}

bool SceneSessionManager::MissionChanged(sptr<SceneSession>& prevSession, sptr<SceneSession>& currSession)
{
    if (prevSession == nullptr && currSession == nullptr) {
        return false;
    }
    if (prevSession == nullptr || currSession == nullptr) {
        return true;
    }
    return prevSession->GetMissionId() != currSession->GetMissionId();
}

std::string SceneSessionManager::GetAllSessionFocusInfo()
{
    std::ostringstream os;
    auto func = [&os](sptr<SceneSession> session) {
        if (session == nullptr) {
            WLOGE("sceneSession is nullptr");
            return false;
        }
        os << "WindowName: " << session->GetWindowName() << ", id: " << session->GetPersistentId() <<
           " ,focusable: "<< session->GetFocusable() << ";";
        return false;
    };
    TraverseSessionTree(func, true);
    return os.str();
}

WSError SceneSessionManager::UpdateFocus(int32_t persistentId, bool isFocused)
{
    auto task = [this, persistentId, isFocused]() {
        // notify session and client
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFE("UpdateFocus could not find window, persistentId:%{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_WINDOW;
        }
        WLOGFI("UpdateFocus, name: %{public}s, id: %{public}d, isFocused: %{public}u",
            sceneSession->GetWindowName().c_str(), persistentId, static_cast<uint32_t>(isFocused));
        // focusId change
        if (isFocused) {
            SetFocusedSessionId(persistentId);
            UpdateBrightness(focusedSessionId_);
            FocusIDChange(persistentId, sceneSession);
        } else if (persistentId == GetFocusedSessionId()) {
            SetFocusedSessionId(INVALID_SESSION_ID);
        }
        // notify window manager
        sptr<FocusChangeInfo> focusChangeInfo = sptr<FocusChangeInfo>::MakeSptr(
            sceneSession->GetWindowId(),
            static_cast<DisplayId>(0),
            sceneSession->GetCallingPid(),
            sceneSession->GetCallingUid(),
            sceneSession->GetWindowType(),
            sceneSession->GetAbilityToken()
        );
        SessionManagerAgentController::GetInstance().UpdateFocusChangeInfo(focusChangeInfo, isFocused);
        WSError res = WSError::WS_OK;
        res = sceneSession->UpdateFocus(isFocused);
        if (res != WSError::WS_OK) {
            return res;
        }
        WLOGI("UpdateFocus, id: %{public}d, system: %{public}d", sceneSession->GetPersistentId(),
               sceneSession->GetSessionInfo().isSystem_);
        if (!sceneSession->GetSessionInfo().isSystem_) {
            if (isFocused) {
                WLOGD("NotifyFocused, id: %{public}d", sceneSession->GetPersistentId());
                listenerController_->NotifySessionFocused(sceneSession->GetPersistentId());
            } else {
                WLOGD("NotifyUnfocused, id: %{public}d", sceneSession->GetPersistentId());
                listenerController_->NotifySessionUnfocused(sceneSession->GetPersistentId());
            }
        }
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task, "UpdateFocus" + std::to_string(persistentId));
    return WSError::WS_OK;
}

WSError SceneSessionManager::UpdateWindowMode(int32_t persistentId, int32_t windowMode)
{
    WLOGFD("id: %{public}d, mode: %{public}d", persistentId, windowMode);
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("could not find window, Id:%{public}d", persistentId);
        return WSError::WS_ERROR_INVALID_WINDOW;
    }
    WindowMode mode = static_cast<WindowMode>(windowMode);
    return sceneSession->UpdateWindowMode(mode);
}

#ifdef SECURITY_COMPONENT_MANAGER_ENABLE
static void FillSecCompEnhanceData(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
    MMI::PointerEvent::PointerItem& pointerItem)
{
    struct PointerEventData {
        double x;
        double y;
        uint64_t time;
    } pointerEventData = {
        .x = pointerItem.GetDisplayX(),
        .y = pointerItem.GetDisplayY(),
        .time = pointerEvent->GetActionTime()
    };

    const uint32_t MAX_HMAC_SIZE = 64;
    uint8_t outBuf[MAX_HMAC_SIZE] = { 0 };
    uint8_t *enhanceData = reinterpret_cast<uint8_t *>(&outBuf[0]);
    uint32_t enhanceDataLen = MAX_HMAC_SIZE;
    if (Security::SecurityComponent::SecCompEnhanceKit::GetPointerEventEnhanceData(&pointerEventData,
        sizeof(pointerEventData), enhanceData, enhanceDataLen) == 0) {
        pointerEvent->SetEnhanceData(std::vector<uint8_t>(outBuf, outBuf + enhanceDataLen));
    }
}
#endif // SECURITY_COMPONENT_MANAGER_ENABLE

WSError SceneSessionManager::SendTouchEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, uint32_t zIndex)
{
    if (!pointerEvent) {
        WLOGFE("pointerEvent is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
        WLOGFE("Failed to get pointerItem");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
#ifdef SECURITY_COMPONENT_MANAGER_ENABLE
    FillSecCompEnhanceData(pointerEvent, pointerItem);
#endif
    TLOGI(WmsLogTag::WMS_EVENT, "PointerId=%{public}d, action=%{public}d, deviceId=%{public}d, zIndex=%{public}ud",
        pointerEvent->GetPointerId(), pointerEvent->GetPointerAction(), pointerEvent->GetDeviceId(), zIndex);
    pointerEvent->AddFlag(MMI::PointerEvent::EVENT_FLAG_NO_INTERCEPT);
    MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent, static_cast<float>(zIndex));
    return WSError::WS_OK;
}

void SceneSessionManager::SetScreenLocked(const bool isScreenLocked)
{
    isScreenLocked_ = isScreenLocked;
    DeleteStateDetectTask();
}

void SceneSessionManager::DeleteStateDetectTask()
{
    if (!IsScreenLocked()) {
        return;
    }
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (auto iter : sceneSessionMap_) {
        auto& session = iter.second;
        if (session && session->GetDetectTaskInfo().taskState != DetectTaskState::NO_TASK) {
            taskScheduler_->GetEventHandler()->RemoveTask(session->GetWindowDetectTaskName());
            DetectTaskInfo detectTaskInfo;
            session->SetDetectTaskInfo(detectTaskInfo);
        }
    }
}

bool SceneSessionManager::IsScreenLocked() const
{
    return isScreenLocked_;
}

void SceneSessionManager::RegisterWindowChanged(const WindowChangedFunc& func)
{
    WindowChangedFunc_ = func;
}

bool SceneSessionManager::JudgeNeedNotifyPrivacyInfo(DisplayId displayId,
    const std::unordered_set<std::string>& privacyBundles)
{
    bool needNotify = false;
    static int reSendTimes = MAX_RESEND_TIMES;
    std::unique_lock<std::mutex> lock(privacyBundleMapMutex_);
    do {
        if (privacyBundleMap_.find(displayId) == privacyBundleMap_.end()) {
            TLOGD(WmsLogTag::WMS_MAIN, "can not find display[%{public}" PRIu64 "].", displayId);
            needNotify = !privacyBundles.empty();
            break;
        }
        const auto& lastPrivacyBundles = privacyBundleMap_[displayId];
        if (lastPrivacyBundles.size() != privacyBundles.size()) {
            TLOGD(WmsLogTag::WMS_MAIN, "privacy bundle list size is not equal, %{public}zu != %{public}zu.",
                  lastPrivacyBundles.size(), privacyBundles.size());
            needNotify = true;
            break;
        }
        for (const auto& bundle : lastPrivacyBundles) {
            if (privacyBundles.find(bundle) == privacyBundles.end()) {
                needNotify = true;
                break;
            }
        }
    } while (false);

    TLOGD(WmsLogTag::WMS_MAIN, "display[%{public}" PRIu64 "] need notify privacy state: %{public}d.",
          displayId, needNotify);
    if (needNotify) {
        reSendTimes = MAX_RESEND_TIMES;
        privacyBundleMap_[displayId] = privacyBundles;
    } else if (reSendTimes > 0) {
        needNotify = true;
        reSendTimes--;
        privacyBundleMap_[displayId] = privacyBundles;
    }
    return needNotify;
}

void SceneSessionManager::UpdatePrivateStateAndNotify(uint32_t persistentId)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "update privacy state failed, scene is nullptr, wid = %{public}u.", persistentId);
        return;
    }

    auto sessionProperty = sceneSession->GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "get session property failed, wid = %{public}u.", persistentId);
        return;
    }
    auto displayId = sessionProperty->GetDisplayId();
    std::unordered_set<std::string> privacyBundleList;
    GetSceneSessionPrivacyModeBundles(displayId, privacyBundleList);
    if (!JudgeNeedNotifyPrivacyInfo(displayId, privacyBundleList)) {
        return;
    }

    std::vector<std::string> bundleListForNotify(privacyBundleList.begin(), privacyBundleList.end());
    ScreenSessionManagerClient::GetInstance().SetPrivacyStateByDisplayId(displayId,
        !bundleListForNotify.empty() || specialExtWindowHasPrivacyMode_.load());
    ScreenSessionManagerClient::GetInstance().SetScreenPrivacyWindowList(displayId, bundleListForNotify);
    for (const auto& bundle : bundleListForNotify) {
        TLOGD(WmsLogTag::WMS_MAIN, "notify dms privacy bundle, display = %{public}" PRIu64 ", bundle = %{public}s.",
              displayId, bundle.c_str());
    }
}

void SceneSessionManager::UpdatePrivateStateAndNotifyForAllScreens()
{
    auto screenProperties = ScreenSessionManagerClient::GetInstance().GetAllScreensProperties();
    for (auto& iter : screenProperties) {
        auto displayId = iter.first;
        std::unordered_set<std::string> privacyBundleList;
        GetSceneSessionPrivacyModeBundles(displayId, privacyBundleList);

        ScreenSessionManagerClient::GetInstance().SetPrivacyStateByDisplayId(displayId,
            !privacyBundleList.empty() || specialExtWindowHasPrivacyMode_.load());
    }
}

void SceneSessionManager::GetSceneSessionPrivacyModeBundles(DisplayId displayId,
    std::unordered_set<std::string>& privacyBundles)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& item : sceneSessionMap_) {
        sptr<SceneSession> sceneSession = item.second;
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "scene session is nullptr, wid = %{public}d.", item.first);
            continue;
        }
        auto sessionProperty = sceneSession->GetSessionProperty();
        if (sessionProperty == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "scene session property is nullptr, wid = %{public}d.", item.first);
            continue;
        }
        auto currentDisplayId = sessionProperty->GetDisplayId();
        if (displayId != currentDisplayId) {
            continue;
        }
        bool isForeground =  sceneSession->GetSessionState() == SessionState::STATE_FOREGROUND ||
            sceneSession->GetSessionState() == SessionState::STATE_ACTIVE;
        if (isForeground && sceneSession->GetParentSession() != nullptr) {
            isForeground = isForeground &&
                (sceneSession->GetParentSession()->GetSessionState() == SessionState::STATE_FOREGROUND ||
                sceneSession->GetParentSession()->GetSessionState() == SessionState::STATE_ACTIVE);
        }
        bool isPrivate = sessionProperty->GetPrivacyMode() ||
            sceneSession->GetCombinedExtWindowFlags().privacyModeFlag;
        bool IsSystemWindowVisible = sceneSession->GetSessionInfo().isSystem_ && sceneSession->IsVisible();
        if ((isForeground || IsSystemWindowVisible) && isPrivate) {
            if (!sceneSession->GetSessionInfo().bundleName_.empty()) {
                privacyBundles.insert(sceneSession->GetSessionInfo().bundleName_);
            } else {
                TLOGW(WmsLogTag::WMS_MAIN, "bundle name is empty, wid = %{public}d.", item.first);
                privacyBundles.insert(sceneSession->GetWindowName());
            }
        }
    }
}

void SceneSessionManager::RegisterSessionStateChangeNotifyManagerFunc(sptr<SceneSession>& sceneSession)
{
    NotifySessionStateChangeNotifyManagerFunc func = [this](int32_t persistentId, const SessionState& state) {
        this->OnSessionStateChange(persistentId, state);
    };
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    sceneSession->SetSessionStateChangeNotifyManagerListener(func);
    WLOGFD("RegisterSessionStateChangeFunc success");
}

void SceneSessionManager::RegisterSessionInfoChangeNotifyManagerFunc(sptr<SceneSession>& sceneSession)
{
    wptr<SceneSessionManager> weakSessionManager = this;
    NotifySessionInfoChangeNotifyManagerFunc func = [weakSessionManager](int32_t persistentId) {
        auto sceneSessionManager = weakSessionManager.promote();
        if (sceneSessionManager == nullptr) {
            return;
        }
        sceneSessionManager->NotifyWindowInfoChangeFromSession(persistentId);
    };
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    sceneSession->SetSessionInfoChangeNotifyManagerListener(func);
}

void SceneSessionManager::RegisterRequestFocusStatusNotifyManagerFunc(sptr<SceneSession>& sceneSession)
{
    NotifyRequestFocusStatusNotifyManagerFunc func =
    [this](int32_t persistentId, const bool isFocused, const bool byForeground, FocusChangeReason reason) {
        this->RequestFocusStatus(persistentId, isFocused, byForeground, reason);
    };
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    sceneSession->SetRequestFocusStatusNotifyManagerListener(func);
    WLOGFD("RegisterSessionUpdateFocusStatusFunc success");
}

void SceneSessionManager::RegisterGetStateFromManagerFunc(sptr<SceneSession>& sceneSession)
{
    GetStateFromManagerFunc func = [this](const ManagerState key) {
        switch (key)
        {
        case ManagerState::MANAGER_STATE_SCREEN_LOCKED:
            return this->IsScreenLocked();
            break;
        default:
            return false;
            break;
        }
    };
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    sceneSession->SetGetStateFromManagerListener(func);
    WLOGFD("RegisterGetStateFromManagerFunc success");
}

void SceneSessionManager::RegisterSessionChangeByActionNotifyManagerFunc(sptr<SceneSession>& sceneSession)
{
    SessionChangeByActionNotifyManagerFunc func = [this](const sptr<SceneSession>& sceneSession,
        const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action) {
        if (sceneSession == nullptr || property == nullptr) {
            TLOGE(WmsLogTag::DEFAULT, "params is nullptr");
            return;
        }
        switch (action) {
            case WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON:
                HandleKeepScreenOn(sceneSession, property->IsKeepScreenOn());
                break;
            case WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE:
            case WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE:
            case WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS:
            case WSPropertyChangeAction::ACTION_UPDATE_STATUS_PROPS:
            case WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_PROPS:
            case WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_INDICATOR_PROPS:
                NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
                break;
            case WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS:
                SetBrightness(sceneSession, property->GetBrightness());
                break;
            case WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE:
            case WSPropertyChangeAction::ACTION_UPDATE_SYSTEM_PRIVACY_MODE:
                UpdatePrivateStateAndNotify(property->GetPersistentId());
                break;
            case WSPropertyChangeAction::ACTION_UPDATE_FLAGS:
                CheckAndNotifyWaterMarkChangedResult();
                NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
                break;
            case WSPropertyChangeAction::ACTION_UPDATE_MODE:
                if (sceneSession->GetSessionProperty() != nullptr) {
                    ProcessWindowModeType();
                }
                NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
                break;
            case WSPropertyChangeAction::ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS:
                HandleHideNonSystemFloatingWindows(property, sceneSession);
                break;
            case WSPropertyChangeAction::ACTION_UPDATE_WINDOW_MASK:
                FlushWindowInfoToMMI();
                break;
            default:
                break;
        }
    };
    if (sceneSession != nullptr) {
        sceneSession->SetSessionChangeByActionNotifyManagerListener(func);
    }
}

__attribute__((no_sanitize("cfi"))) void SceneSessionManager::OnSessionStateChange(
    int32_t persistentId, const SessionState& state)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:OnSessionStateChange%d", persistentId);
    WLOGFD("Session state change, id: %{public}d, state:%{public}u", persistentId, state);
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFD("session is nullptr");
        return;
    }
    switch (state) {
        case SessionState::STATE_FOREGROUND:
            ProcessFocusWhenForeground(sceneSession);
            if (!IsSessionVisibleForeground(sceneSession)) {
                sceneSession->SetPostProcessProperty(true);
                break;
            }
            UpdateForceHideState(sceneSession, sceneSession->GetSessionProperty(), true);
            NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_ADDED);
            HandleKeepScreenOn(sceneSession, sceneSession->IsKeepScreenOn());
            UpdatePrivateStateAndNotify(persistentId);
            if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                ProcessSubSessionForeground(sceneSession);
            }
            break;
        case SessionState::STATE_BACKGROUND:
            NotifySessionUpdate(sceneSession->GetSessionInfo(), ActionType::SINGLE_BACKGROUND);
            RequestSessionUnfocus(persistentId, FocusChangeReason::APP_BACKGROUND);
            UpdateForceHideState(sceneSession, sceneSession->GetSessionProperty(), false);
            NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_REMOVED);
            HandleKeepScreenOn(sceneSession, false);
            UpdatePrivateStateAndNotify(persistentId);
            if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                ProcessSubSessionBackground(sceneSession);
            }
            break;
        case SessionState::STATE_CONNECT:
            SetSessionSnapshotSkipForAppProcess(sceneSession);
            SetSessionSnapshotSkipForAppBundleName(sceneSession);
            SetSessionWatermarkForAppProcess(sceneSession);
            break;
        case SessionState::STATE_DISCONNECT:
            if (SessionHelper::IsMainWindow(sceneSession->GetWindowType())) {
                RemoveProcessSnapshotSkip(sceneSession->GetCallingPid());
                RemoveProcessWatermarkPid(sceneSession->GetCallingPid());
            }
            break;
        default:
            break;
    }
    ProcessWindowModeType();
}

void SceneSessionManager::ProcessFocusWhenForeground(sptr<SceneSession>& sceneSession)
{
    auto persistentId = sceneSession->GetPersistentId();
    if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
        persistentId == focusedSessionId_) {
        if (needBlockNotifyFocusStatusUntilForeground_) {
            needBlockNotifyUnfocusStatus_ = false;
            needBlockNotifyFocusStatusUntilForeground_ = false;
            NotifyFocusStatus(sceneSession, true);
        }
    } else if (!sceneSession->IsFocusedOnShow()) {
        sceneSession->SetFocusedOnShow(true);
    } else {
        if (Session::IsScbCoreEnabled()) {
            if (IsSessionVisibleForeground(sceneSession)) {
                RequestSessionFocus(persistentId, true, FocusChangeReason::APP_FOREGROUND);
            } else {
                PostProcessFocusState state = {true, true, FocusChangeReason::APP_FOREGROUND};
                sceneSession->SetPostProcessFocusState(state);
            }
        } else {
            RequestSessionFocus(persistentId, true, FocusChangeReason::APP_FOREGROUND);
        }
    }
}

void SceneSessionManager::ProcessWindowModeType()
{
    if (isScreenLocked_) {
        return;
    }
    NotifyRSSWindowModeTypeUpdate();
}

WindowModeType SceneSessionManager::CheckWindowModeType()
{
    bool inSplit = false;
    bool inFloating = false;
    bool fullScreen = false;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& session : sceneSessionMap_) {
            if (session.second == nullptr ||
                !WindowHelper::IsMainWindow(session.second->GetWindowType()) ||
                !Rosen::SceneSessionManager::GetInstance().IsSessionVisibleForeground(session.second)) {
                continue;
            }
            auto mode = session.second->GetWindowMode();
            if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
                inSplit = true;
            }
            if (mode == WindowMode::WINDOW_MODE_FLOATING) {
                inFloating = true;
            }
            if (WindowHelper::IsFullScreenWindow(mode)) {
                fullScreen = true;
            }
        }
    }

    WindowModeType type;
    if (inSplit) {
        if (inFloating) {
            type = WindowModeType::WINDOW_MODE_SPLIT_FLOATING;
        } else {
            type = WindowModeType::WINDOW_MODE_SPLIT;
        }
    } else {
        if (inFloating) {
            if (fullScreen) {
                type = WindowModeType::WINDOW_MODE_FULLSCREEN_FLOATING;
            } else {
                type = WindowModeType::WINDOW_MODE_FLOATING;
            }
        } else if (fullScreen) {
            type = WindowModeType::WINDOW_MODE_FULLSCREEN;
        } else {
            type = WindowModeType::WINDOW_MODE_OTHER;
        }
    }
    return type;
}

void SceneSessionManager::NotifyRSSWindowModeTypeUpdate()
{
    WindowModeType type = CheckWindowModeType();
    if (lastWindowModeType_ == type) {
        return;
    }
    lastWindowModeType_ = type;
    TLOGI(WmsLogTag::WMS_MAIN, "Notify RSS Window Mode Type Update, type : %{public}d",
        static_cast<uint8_t>(type));
    SessionManagerAgentController::GetInstance().UpdateWindowModeTypeInfo(type);
}

void SceneSessionManager::ProcessSubSessionForeground(sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFD("session is nullptr");
        return;
    }
    std::vector<sptr<Session>> modalVec = sceneSession->GetDialogVector();
    for (const auto& subSession : sceneSession->GetSubSession()) {
        if (subSession == nullptr) {
            TLOGD(WmsLogTag::WMS_SUB, "sub session is nullptr");
            continue;
        }
        if (subSession->IsTopmost()) {
            modalVec.push_back(subSession);
            TLOGD(WmsLogTag::WMS_SUB, "sub session is topmost modal sub window");
            continue;
        }
        const auto& state = subSession->GetSessionState();
        if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
            TLOGD(WmsLogTag::WMS_SUB, "sub session is not active");
            continue;
        }
        RequestSessionFocus(subSession->GetPersistentId(), true);
        NotifyWindowInfoChange(subSession->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_ADDED);
        HandleKeepScreenOn(subSession, subSession->IsKeepScreenOn());
    }

    for (const auto& modal : modalVec) {
        if (modal == nullptr) {
            TLOGD(WmsLogTag::WMS_DIALOG, "dialog or topmost modal sub window is nullptr");
            continue;
        }
        const auto& state = modal->GetSessionState();
        if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
            TLOGD(WmsLogTag::WMS_DIALOG, "dialog or topmost modal sub window is not active");
            continue;
        }
        auto modalSession = GetSceneSession(modal->GetPersistentId());
        if (modalSession == nullptr) {
            TLOGD(WmsLogTag::WMS_DIALOG, "modalSession is null");
            continue;
        }
        NotifyWindowInfoChange(modal->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_ADDED);
        if (modal->GetPersistentId() == focusedSessionId_ && needBlockNotifyFocusStatusUntilForeground_) {
            needBlockNotifyUnfocusStatus_ = false;
            needBlockNotifyFocusStatusUntilForeground_ = false;
            NotifyFocusStatus(modalSession, true);
        }
        HandleKeepScreenOn(modalSession, modalSession->IsKeepScreenOn());
    }
}

WSError SceneSessionManager::ProcessModalTopmostRequestFocusImmdediately(sptr<SceneSession>& sceneSession)
{
    // focus must on modal topmost subwindow when APP_MAIN_WINDOW or sub winodw request focus
    sptr<SceneSession> mainSession = nullptr;
    if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        mainSession = sceneSession;
    } else if (SessionHelper::IsSubWindow(sceneSession->GetWindowType())) {
        mainSession = GetSceneSession(sceneSession->GetParentPersistentId());
    }
    if (mainSession == nullptr) {
        TLOGD(WmsLogTag::WMS_FOCUS, "main window is nullptr");
        return WSError::WS_DO_NOTHING;
    }

    std::vector<sptr<SceneSession>> topmostVec;
    for (auto subSession : mainSession->GetSubSession()) {
        if (subSession && subSession->IsTopmost()) {
            topmostVec.push_back(subSession);
        }
    }
    if (std::find_if(topmostVec.begin(), topmostVec.end(),
        [this](sptr<SceneSession>& iter) { return iter && iter->GetPersistentId() == focusedSessionId_; })
        != topmostVec.end()) {
            TLOGD(WmsLogTag::WMS_SUB, "modal topmost subwindow id: %{public}d has been focused!", focusedSessionId_);
            return WSError::WS_OK;
    }
    WSError ret = WSError::WS_DO_NOTHING;
    for (auto topmostSession : topmostVec) {
        if (topmostSession == nullptr) {
            continue;
        }
        // no need to consider order, since rule of zOrder
        if (RequestSessionFocusImmediately(topmostSession->GetPersistentId()) == WSError::WS_OK) {
            ret = WSError::WS_OK;
        }
    }
    return ret;
}

WSError SceneSessionManager::ProcessDialogRequestFocusImmdediately(sptr<SceneSession>& sceneSession)
{
    // focus must on dialog when APP_MAIN_WINDOW or sub winodw request focus
    sptr<SceneSession> mainSession = nullptr;
    if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        mainSession = sceneSession;
    } else if (SessionHelper::IsSubWindow(sceneSession->GetWindowType())) {
        mainSession = GetSceneSession(sceneSession->GetParentPersistentId());
    }
    if (mainSession == nullptr) {
        TLOGD(WmsLogTag::WMS_FOCUS, "main window is nullptr");
        return WSError::WS_DO_NOTHING;
    }
    std::vector<sptr<Session>> dialogVec = mainSession->GetDialogVector();
    if (std::find_if(dialogVec.begin(), dialogVec.end(),
        [this](sptr<Session>& iter) { return iter && iter->GetPersistentId() == focusedSessionId_; })
        != dialogVec.end()) {
            TLOGD(WmsLogTag::WMS_DIALOG, "dialog id: %{public}d has been focused!", focusedSessionId_);
            return WSError::WS_OK;
    }
    WSError ret = WSError::WS_DO_NOTHING;
    for (auto dialog : dialogVec) {
        if (dialog == nullptr) {
            continue;
        }
        // no need to consider order, since rule of zOrder
        if (RequestSessionFocusImmediately(dialog->GetPersistentId()) == WSError::WS_OK) {
            ret = WSError::WS_OK;
        }
    }
    return ret;
}

void SceneSessionManager::ProcessSubSessionBackground(sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFD("session is nullptr");
        return;
    }
    for (const auto& subSession : sceneSession->GetSubSession()) {
        if (subSession == nullptr) {
            WLOGFD("sub session is nullptr");
            continue;
        }
        const auto& state = subSession->GetSessionState();
        if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
            WLOGFD("sub session is not active");
            continue;
        }
        NotifyWindowInfoChange(subSession->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_REMOVED);
        HandleKeepScreenOn(subSession, false);
        UpdatePrivateStateAndNotify(subSession->GetPersistentId());
    }
    std::vector<sptr<Session>> dialogVec = sceneSession->GetDialogVector();
    for (const auto& dialog : dialogVec) {
        if (dialog == nullptr) {
            TLOGD(WmsLogTag::WMS_DIALOG, "dialog is nullptr");
            continue;
        }
        auto dialogSession = GetSceneSession(dialog->GetPersistentId());
        NotifyWindowInfoChange(dialog->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_REMOVED);
        HandleKeepScreenOn(dialogSession, false);
        UpdatePrivateStateAndNotify(dialog->GetPersistentId());
    }
    for (const auto& toastSession : sceneSession->GetToastSession()) {
        if (toastSession == nullptr) {
            TLOGD(WmsLogTag::WMS_TOAST, "toastSession session is nullptr");
            continue;
        }
        const auto& state = toastSession->GetSessionState();
        if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
            TLOGD(WmsLogTag::WMS_TOAST, "toast session is not active");
            continue;
        }
        NotifyWindowInfoChange(toastSession->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_REMOVED);
        HandleKeepScreenOn(toastSession, false);
        UpdatePrivateStateAndNotify(toastSession->GetPersistentId());
        toastSession->SetActive(false);
        toastSession->BackgroundTask();
    }
}

WSError SceneSessionManager::SetWindowFlags(const sptr<SceneSession>& sceneSession,
    const sptr<WindowSessionProperty>& property)
{
    if (sceneSession == nullptr) {
        WLOGFD("session is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto sessionProperty = sceneSession->GetSessionProperty();
    if (sessionProperty == nullptr) {
        return WSError::WS_ERROR_NULLPTR;
    }
    uint32_t flags = property->GetWindowFlags();
    uint32_t oldFlags = sessionProperty->GetWindowFlags();
    if (((oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED) ||
        (oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK)) &&
        !property->GetSystemCalling()) {
            WLOGFE("Set window flags permission denied");
            return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    sessionProperty->SetWindowFlags(flags);
    CheckAndNotifyWaterMarkChangedResult();
    if ((oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED)) {
        sceneSession->OnShowWhenLocked(flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED));
    }
    WLOGFI("SetWindowFlags end, flags: %{public}u", flags);
    return WSError::WS_OK;
}

void SceneSessionManager::CheckAndNotifyWaterMarkChangedResult()
{
    bool currentWaterMarkShowState = false;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& iter: sceneSessionMap_) {
            auto& session = iter.second;
            if (!session) {
                continue;
            }
            auto sessionProperty = session->GetSessionProperty();
            if (!sessionProperty) {
                continue;
            }
            bool hasWaterMark = sessionProperty->GetWindowFlags() &
                static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
            bool isExtWindowHasWaterMarkFlag = session->GetCombinedExtWindowFlags().waterMarkFlag;
            if ((hasWaterMark && session->GetRSVisible()) || isExtWindowHasWaterMarkFlag) {
                currentWaterMarkShowState = true;
                break;
            }
        }
        if (combinedExtWindowFlags_.waterMarkFlag) {
            TLOGI(WmsLogTag::WMS_UIEXT, "CheckAndNotifyWaterMarkChangedResult scb uiext has water mark");
            currentWaterMarkShowState = true;
        }
    }
    if (lastWaterMarkShowState_ != currentWaterMarkShowState) {
        lastWaterMarkShowState_ = currentWaterMarkShowState;
        NotifyWaterMarkFlagChangedResult(currentWaterMarkShowState);
    }
}

WSError SceneSessionManager::NotifyWaterMarkFlagChangedResult(bool hasWaterMark)
{
    WLOGFI("WaterMark status : %{public}u", static_cast<uint32_t>(hasWaterMark));
    SessionManagerAgentController::GetInstance().NotifyWaterMarkFlagChangedResult(hasWaterMark);
    return WSError::WS_OK;
}

void SceneSessionManager::ProcessPreload(const AppExecFwk::AbilityInfo& abilityInfo) const
{
    if (!bundleMgr_) {
        WLOGFE("bundle manager is nullptr.");
        return;
    }

    AAFwk::Want want;
    want.SetElementName(abilityInfo.deviceId, abilityInfo.bundleName, abilityInfo.name, abilityInfo.moduleName);
    auto uid = abilityInfo.uid;
    want.SetParam("uid", uid);
    bundleMgr_->ProcessPreload(want);
}

void SceneSessionManager::NotifyCompleteFirstFrameDrawing(int32_t persistentId)
{
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, " scnSession is nullptr.");
        return;
    }

    const auto& sessionInfo = scnSession->GetSessionInfo();
    if (IsAtomicServiceFreeInstall(sessionInfo)) {
        TLOGI(WmsLogTag::WMS_LIFE, "AtomicService free-install start, id: %{public}d, type: %{public}d",
            scnSession->GetPersistentId(), scnSession->GetWindowType());
        FillSessionInfo(scnSession);
    }

    TLOGI(WmsLogTag::WMS_MAIN, " id: %{public}d, app info: [%{public}s %{public}s %{public}s]",
        scnSession->GetPersistentId(), sessionInfo.bundleName_.c_str(),
        sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str());
    auto abilityInfoPtr = sessionInfo.abilityInfo;
    if (abilityInfoPtr == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, " abilityInfoPtr is null, Id: %{public}d", persistentId);
        return;
    }

    [this, persistentId] {
        auto task = [persistentId] {
            AAFwk::AbilityManagerClient::GetInstance()->CompleteFirstFrameDrawing(persistentId);
        };
        WLOGI("Post CompleteFirstFrameDrawing task. Id: %{public}d", persistentId);
        eventHandler_->PostTask(task, "wms:CompleteFirstFrameDrawing", 0);
    }();

    auto task = [this, abilityInfoPtr, scnSession, persistentId] {
        if (!scnSession->GetSessionInfo().isSystem_ && !(abilityInfoPtr->excludeFromMissions)) {
            WLOGD("NotifyCreated, id: %{public}d", persistentId);
            listenerController_->NotifySessionCreated(persistentId);
        }
        ProcessPreload(*abilityInfoPtr);
    };
    return taskScheduler_->PostAsyncTask(task, "NotifyCompleteFirstFrameDrawing" + std::to_string(persistentId));
}

void SceneSessionManager::NotifySessionMovedToFront(int32_t persistentId)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("session is invalid with %{public}d", persistentId);
        return;
    }
    WLOGFI("id: %{public}d, system: %{public}d", sceneSession->GetPersistentId(),
           sceneSession->GetSessionInfo().isSystem_);
    if (!sceneSession->GetSessionInfo().isSystem_ &&
        sceneSession->GetSessionInfo().abilityInfo &&
        !(sceneSession->GetSessionInfo().abilityInfo)->excludeFromMissions) {
        listenerController_->NotifySessionMovedToFront(persistentId);
    }
}

WSError SceneSessionManager::SetSessionLabel(const sptr<IRemoteObject>& token, const std::string& label)
{
    WLOGFI("label: %{public}s", label.c_str());
    auto task = [this, &token, &label]() {
        auto sceneSession = FindSessionByToken(token);
        if (sceneSession == nullptr) {
            WLOGFI("fail to find session by token");
            return WSError::WS_ERROR_SET_SESSION_LABEL_FAILED;
        }
        sceneSession->SetSessionLabel(label);
        WLOGI("NotifySessionLabelUpdated, id: %{public}d, system: %{public}d", sceneSession->GetPersistentId(),
            sceneSession->GetSessionInfo().isSystem_);
        if (!sceneSession->GetSessionInfo().isSystem_) {
            WLOGD("NotifySessionLabelUpdated, id: %{public}d", sceneSession->GetPersistentId());
            listenerController_->NotifySessionLabelUpdated(sceneSession->GetPersistentId());
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetSessionLabel");
}

WSError SceneSessionManager::SetSessionIcon(const sptr<IRemoteObject>& token,
    const std::shared_ptr<Media::PixelMap>& icon)
{
    WLOGFI("Enter");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    auto task = [this, &token, &icon]() {
        auto sceneSession = FindSessionByToken(token);
        if (sceneSession == nullptr) {
            WLOGFI("fail to find session by token");
            return WSError::WS_ERROR_SET_SESSION_LABEL_FAILED;
        }
        sceneSession->SetSessionIcon(icon);
        WLOGI("NotifySessionIconChanged, id: %{public}d, system: %{public}d", sceneSession->GetPersistentId(),
            sceneSession->GetSessionInfo().isSystem_);
        if (!sceneSession->GetSessionInfo().isSystem_ &&
            sceneSession->GetSessionInfo().abilityInfo &&
            !(sceneSession->GetSessionInfo().abilityInfo)->excludeFromMissions) {
            WLOGD("NotifySessionIconChanged, id: %{public}d", sceneSession->GetPersistentId());
            listenerController_->NotifySessionIconChanged(sceneSession->GetPersistentId(), icon);
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetSessionIcon");
}

WSError SceneSessionManager::IsValidSessionIds(
    const std::vector<int32_t>& sessionIds, std::vector<bool>& results)
{
    WLOGFI("Enter");
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (auto i = 0; i < static_cast<int32_t>(sessionIds.size()); ++i) {
        auto search = sceneSessionMap_.find(sessionIds.at(i));
        if (search == sceneSessionMap_.end() || search->second == nullptr) {
            results.push_back(false);
            continue;
        }
        results.push_back(true);
    }
    return WSError::WS_OK;
}

WSError SceneSessionManager::RegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFI("in");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        TLOGE(WmsLogTag::WMS_LIFE, "permission denied");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &listener] {
        WSError ret = listenerController_->AddSessionListener(listener);
        // app continue report for distributed scheduled service
        SingletonContainer::Get<DmsReporter>().ReportContinueApp(ret == WSError::WS_OK,
            static_cast<int32_t>(ret));
        return ret;
    };
    return taskScheduler_->PostSyncTask(task, "AddSessionListener");
}

WSError SceneSessionManager::UnRegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFI("in");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        TLOGE(WmsLogTag::WMS_LIFE, "permission denied");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &listener] {
        listenerController_->DelSessionListener(listener);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "DelSessionListener");
}

WSError SceneSessionManager::GetSessionInfos(const std::string& deviceId, int32_t numMax,
                                             std::vector<SessionInfoBean>& sessionInfos)
{
    WLOGFI("Enter num max %{public}d", numMax);
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &deviceId, numMax, &sessionInfos]() {
        if (CheckIsRemote(deviceId)) {
            int ret = GetRemoteSessionInfos(deviceId, numMax, sessionInfos);
            if (ret != ERR_OK) {
                return WSError::WS_ERROR_INVALID_PARAM;
            } else {
                return WSError::WS_OK;
            }
        }
        std::map<int32_t, sptr<SceneSession>>::iterator iter;
        std::vector<sptr<SceneSession>> sceneSessionInfos;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (iter = sceneSessionMap_.begin(); iter != sceneSessionMap_.end(); iter++) {
            auto sceneSession = iter->second;
            if (sceneSession == nullptr) {
                WLOGFE("session is nullptr");
                continue;
            }
            auto sessionInfo = sceneSession->GetSessionInfo();
            if (sessionInfo.isSystem_) {
                WLOGFD("sessionId: %{public}d is SystemScene", sceneSession->GetPersistentId());
                continue;
            }
            auto want = sessionInfo.want;
            if (want == nullptr || sessionInfo.bundleName_.empty() || want->GetElement().GetBundleName().empty()) {
                WLOGFE("session: %{public}d, want is null or bundleName is empty or want bundleName is empty",
                    sceneSession->GetPersistentId());
                continue;
            }
            if (static_cast<int>(sceneSessionInfos.size()) >= numMax) {
                break;
            }
            WLOGFD("GetSessionInfos session: %{public}d, bundleName:%{public}s", sceneSession->GetPersistentId(),
                sessionInfo.bundleName_.c_str());
            sceneSessionInfos.emplace_back(sceneSession);
        }
        return SceneSessionConverter::ConvertToMissionInfos(sceneSessionInfos, sessionInfos);
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionInfos");
}

WSError SceneSessionManager::GetMainWindowStatesByPid(int32_t pid, std::vector<MainWindowState>& windowStates)
{
    TLOGI(WmsLogTag::WMS_LIFE, "pid:%{public}d", pid);
    if (!SessionPermission::IsSACalling() && !SessionPermission::IsShellCall()) {
        TLOGE(WmsLogTag::WMS_LIFE, "Get all mainWindow states failed, only support SA calling.");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    if (pid < 0) {
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    auto task = [this, pid, &windowStates] {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& [_, sceneSession] : sceneSessionMap_) {
            if (sceneSession != nullptr && sceneSession->GetCallingPid() == pid &&
                WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
                MainWindowState windowState;
                windowState.state_ = static_cast<int32_t>(sceneSession->GetSessionState());
                windowState.isVisible_ = sceneSession->GetRSVisible();
                windowState.isForegroundInteractive_ = sceneSession->GetForegroundInteractiveStatus();
                windowState.isPcOrPadEnableActivation_ = sceneSession->IsPcOrPadEnableActivation();
                windowStates.emplace_back(windowState);
            }
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetMainWindowStatesByPid");
}

int SceneSessionManager::GetRemoteSessionInfos(const std::string& deviceId, int32_t numMax,
                                               std::vector<SessionInfoBean>& sessionInfos)
{
    TLOGI(WmsLogTag::DEFAULT, "begin");
    int result = DistributedClient::GetInstance().GetMissionInfos(deviceId, numMax, sessionInfos);
    if (result != ERR_OK) {
        TLOGE(WmsLogTag::DEFAULT, "failed, result = %{public}d", result);
        return result;
    }
    return ERR_OK;
}

WSError SceneSessionManager::GetSessionInfo(const std::string& deviceId,
                                            int32_t persistentId, SessionInfoBean& sessionInfo)
{
    WLOGFI("id %{public}d", persistentId);
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &deviceId, persistentId, &sessionInfo]() {
        if (CheckIsRemote(deviceId)) {
            int ret = GetRemoteSessionInfo(deviceId, persistentId, sessionInfo);
            if (ret != ERR_OK) {
                return WSError::WS_ERROR_INVALID_PARAM;
            } else {
                return WSError::WS_OK;
            }
        }
        std::map<int32_t, sptr<SceneSession>>::iterator iter;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        iter = sceneSessionMap_.find(persistentId);
        if (iter != sceneSessionMap_.end()) {
            auto sceneSession = iter->second;
            if (sceneSession == nullptr) {
                WLOGFE("session: %{public}d is nullptr", persistentId);
                return WSError::WS_ERROR_INVALID_PARAM;
            }
            auto sceneSessionInfo = sceneSession->GetSessionInfo();
            if (sceneSessionInfo.isSystem_) {
                WLOGFD("sessionId: %{public}d  isSystemScene", persistentId);
                return WSError::WS_ERROR_INVALID_PARAM;
            }
            auto want = sceneSessionInfo.want;
            if (want == nullptr || sceneSessionInfo.bundleName_.empty() ||
                want->GetElement().GetBundleName().empty()) {
                WLOGFE("session: %{public}d, want is null or bundleName is empty or want bundleName is empty",
                    persistentId);
                return WSError::WS_ERROR_INTERNAL_ERROR;
            }
            WLOGFD("GetSessionInfo sessionId:%{public}d bundleName:%{public}s", persistentId,
                sceneSessionInfo.bundleName_.c_str());
            return SceneSessionConverter::ConvertToMissionInfo(iter->second, sessionInfo);
        } else {
            WLOGFW("sessionId: %{public}d not found", persistentId);
            return WSError::WS_ERROR_INVALID_PARAM;
        }
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionInfo");
}

WSError SceneSessionManager::GetSessionInfoByContinueSessionId(const std::string& continueSessionId,
    SessionInfoBean& sessionInfo)
{
    TLOGI(WmsLogTag::WMS_LIFE, "query session info with continueSessionId: %{public}s",
        continueSessionId.c_str());
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The interface only support for system service.");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted.");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, continueSessionId, &sessionInfo]() {
        WSError ret = WSError::WS_ERROR_INVALID_SESSION;
        {
            std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            for (auto& [persistentId, sceneSession] : sceneSessionMap_) {
                if (sceneSession && sceneSession->GetSessionInfo().continueSessionId_ == continueSessionId) {
                    ret = SceneSessionConverter::ConvertToMissionInfo(sceneSession, sessionInfo);
                    break;
                }
            }
        }

        TLOGI(WmsLogTag::WMS_LIFE, "get session info finished with ret code: %{public}d", ret);
        // app continue report for distributed scheduled service
        SingletonContainer::Get<DmsReporter>().ReportQuerySessionInfo(ret == WSError::WS_OK,
            static_cast<int32_t>(ret));
        return ret;
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionInfoByContinueSessionId");
}

int SceneSessionManager::GetRemoteSessionInfo(const std::string& deviceId,
                                              int32_t persistentId, SessionInfoBean& sessionInfo)
{
    WLOGFI("GetRemoteSessionInfoFromDms begin");
    std::vector<SessionInfoBean> sessionVector;
    int result = GetRemoteSessionInfos(deviceId, MAX_NUMBER_OF_DISTRIBUTED_SESSIONS, sessionVector);
    if (result != ERR_OK) {
        return result;
    }
    for (auto iter = sessionVector.begin(); iter != sessionVector.end(); iter++) {
        if (iter->id == persistentId) {
            sessionInfo = *iter;
            return ERR_OK;
        }
    }
    WLOGFW("missionId not found");
    return ERR_INVALID_VALUE;
}

bool SceneSessionManager::CheckIsRemote(const std::string& deviceId)
{
    if (deviceId.empty()) {
        WLOGFI("CheckIsRemote: deviceId is empty.");
        return false;
    }
    std::string localDeviceId;
    if (!GetLocalDeviceId(localDeviceId)) {
        WLOGFE("CheckIsRemote: get local deviceId failed");
        return false;
    }
    if (localDeviceId == deviceId) {
        WLOGFI("CheckIsRemote: deviceId is local.");
        return false;
    }
    WLOGFD("CheckIsRemote, deviceId = %{public}s", AnonymizeDeviceId(deviceId).c_str());
    return true;
}

bool SceneSessionManager::GetLocalDeviceId(std::string& localDeviceId)
{
    auto localNode = std::make_unique<NodeBasicInfo>();
    int32_t errCode = GetLocalNodeDeviceInfo(DM_PKG_NAME.c_str(), localNode.get());
    if (errCode != ERR_OK) {
        WLOGFE("GetLocalNodeDeviceInfo errCode = %{public}d", errCode);
        return false;
    }
    if (localNode != nullptr) {
        localDeviceId = localNode->networkId;
        WLOGFD("get local deviceId, deviceId = %{public}s", AnonymizeDeviceId(localDeviceId).c_str());
        return true;
    }
    WLOGFE("localDeviceId null");
    return false;
}

std::string SceneSessionManager::AnonymizeDeviceId(const std::string& deviceId)
{
    if (deviceId.length() < NON_ANONYMIZE_LENGTH) {
        return EMPTY_DEVICE_ID;
    }
    std::string anonDeviceId = deviceId.substr(0, NON_ANONYMIZE_LENGTH);
    anonDeviceId.append("******");
    return anonDeviceId;
}

WSError SceneSessionManager::DumpSessionAll(std::vector<std::string>& infos)
{
    WLOGFI("Dump all session.");
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("DumpSessionAll permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    auto task = [this, &infos]() {
        std::string dumpInfo = "User ID #" + std::to_string(currentUserId_);
        infos.push_back(dumpInfo);
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto &item : sceneSessionMap_) {
            auto& session = item.second;
            if (session) {
                session->DumpSessionInfo(infos);
            }
        }
        return WSError::WS_OK;
    };

    return taskScheduler_->PostSyncTask(task, "DumpSessionAll");
}

WSError SceneSessionManager::DumpSessionWithId(int32_t persistentId, std::vector<std::string>& infos)
{
    WLOGFI("Dump session with id %{public}d", persistentId);
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("DumpSessionWithId permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    auto task = [this, persistentId, &infos]() {
        std::string dumpInfo = "User ID #" + std::to_string(currentUserId_);
        infos.push_back(dumpInfo);
        auto session = GetSceneSession(persistentId);
        if (session) {
            session->DumpSessionInfo(infos);
        } else {
            infos.push_back("error: invalid mission number, please see 'aa dump --mission-list'.");
        }
        return WSError::WS_OK;
    };

    return taskScheduler_->PostSyncTask(task, "DumpSessionWithId");
}

__attribute__((no_sanitize("cfi"))) WSError SceneSessionManager::GetAllAbilityInfos(
    const AAFwk::Want& want, int32_t userId, std::vector<SCBAbilityInfo>& scbAbilityInfos)
{
    if (bundleMgr_ == nullptr) {
        WLOGFE("bundleMgr_ is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto elementName = want.GetElement();
    int32_t ret{0};
    auto flag = (AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION |
        AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_PERMISSION |
        AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_METADATA |
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_ABILITY) |
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION) |
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_HAP_MODULE));
    std::vector<AppExecFwk::BundleInfo> bundleInfos;
    if (elementName.GetBundleName().empty() && elementName.GetAbilityName().empty()) {
        WLOGFD("want is empty queryAllAbilityInfos");
        ret = static_cast<int32_t>(bundleMgr_->GetBundleInfosV9(flag, bundleInfos, userId));
        if (ret) {
            WLOGFE("Query all ability infos from BMS failed!");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
    } else if (!elementName.GetBundleName().empty()) {
        AppExecFwk::BundleInfo bundleInfo;
        WLOGFD("bundleName is not empty, query abilityInfo of %{public}s", elementName.GetBundleName().c_str());
        ret = static_cast<int32_t>(bundleMgr_->GetBundleInfoV9(elementName.GetBundleName(), flag, bundleInfo, userId));
        if (ret) {
            WLOGFE("Query ability info from BMS failed!");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        bundleInfos.push_back(bundleInfo);
    } else {
        WLOGFE("invalid want:%{public}s", want.ToString().c_str());
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    return GetAbilityInfosFromBundleInfo(bundleInfos, scbAbilityInfos);
}

__attribute__((no_sanitize("cfi"))) WSError SceneSessionManager::GetBatchAbilityInfos(
    const std::vector<std::string>& bundleNames, int32_t userId, std::vector<SCBAbilityInfo>& scbAbilityInfos)
{
    if (bundleMgr_ == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "bundleMgr is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (bundleNames.empty()) {
        TLOGE(WmsLogTag::DEFAULT, "bundleNames is empty");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    auto flag = AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION |
                AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_PERMISSION |
                AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_METADATA |
                static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_ABILITY) |
                static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION) |
                static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_HAP_MODULE);
    std::vector<AppExecFwk::BundleInfo> bundleInfos;
    auto ret = static_cast<int32_t>(bundleMgr_->BatchGetBundleInfo(bundleNames, flag, bundleInfos, userId));
    if (ret) {
        TLOGE(WmsLogTag::DEFAULT, "Query batch ability infos from BMS failed!");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    return GetAbilityInfosFromBundleInfo(bundleInfos, scbAbilityInfos);
}

WSError SceneSessionManager::GetAbilityInfosFromBundleInfo(std::vector<AppExecFwk::BundleInfo>& bundleInfos,
    std::vector<SCBAbilityInfo>& scbAbilityInfos)
{
    if (bundleInfos.empty()) {
        WLOGFE("bundleInfos is empty");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    for (auto bundleInfo: bundleInfos) {
        auto hapModulesList = bundleInfo.hapModuleInfos;
        auto sdkVersion = bundleInfo.targetVersion % 100; // %100 to get the real version
        if (hapModulesList.empty()) {
            WLOGFD("hapModulesList is empty");
            continue;
        }
        for (auto hapModule: hapModulesList) {
            auto abilityInfoList = hapModule.abilityInfos;
            for (auto abilityInfo : abilityInfoList) {
                SCBAbilityInfo scbAbilityInfo;
                scbAbilityInfo.abilityInfo_ = abilityInfo;
                scbAbilityInfo.sdkVersion_ = sdkVersion;
                scbAbilityInfo.codePath_ = bundleInfo.applicationInfo.codePath;
                GetOrientationFromResourceManager(scbAbilityInfo.abilityInfo_);
                scbAbilityInfos.push_back(scbAbilityInfo);
            }
        }
    }
    return WSError::WS_OK;
}

void SceneSessionManager::GetOrientationFromResourceManager(AppExecFwk::AbilityInfo& abilityInfo)
{
    if (abilityInfo.orientationId == 0) {
        return;
    }
    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    if (resConfig == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "resConfig is nullptr.");
        return;
    }
    std::shared_ptr<Global::Resource::ResourceManager> resourceMgr(Global::Resource::CreateResourceManager(
        abilityInfo.bundleName, abilityInfo.moduleName, "", {}, *resConfig));
    if (resourceMgr == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "resourceMgr is nullptr.");
        return;
    }
    std::string loadPath = abilityInfo.hapPath.empty() ? abilityInfo.resourcePath : abilityInfo.hapPath;
    if (!resourceMgr->AddResource(loadPath.c_str(), Global::Resource::SELECT_STRING)) {
        TLOGE(WmsLogTag::DEFAULT, "Add resource %{private}s failed.", loadPath.c_str());
    }
    std::string orientation;
    auto ret = resourceMgr->GetStringById(abilityInfo.orientationId, orientation);
    if (ret != Global::Resource::RState::SUCCESS) {
        TLOGE(WmsLogTag::DEFAULT, "GetStringById failed errcode:%{public}d, labelId:%{public}d",
            static_cast<int32_t>(ret), abilityInfo.orientationId);
        return;
    }
    if (STRING_TO_DISPLAY_ORIENTATION_MAP.find(orientation) == STRING_TO_DISPLAY_ORIENTATION_MAP.end()) {
        TLOGE(WmsLogTag::DEFAULT, "Do not support this orientation:%{public}s", orientation.c_str());
        return;
    }
    abilityInfo.orientation = STRING_TO_DISPLAY_ORIENTATION_MAP.at(orientation);
}

WSError SceneSessionManager::TerminateSessionNew(
    const sptr<AAFwk::SessionInfo> info, bool needStartCaller, bool isFromBroker)
{
    if (info == nullptr) {
        TLOGI(WmsLogTag::WMS_LIFE, "sessionInfo is nullptr.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    TLOGI(WmsLogTag::WMS_LIFE,
        "id:%{public}d bundleName:%{public}s needStartCaller:%{public}d isFromBroker:%{public}d",
        info->persistentId, info->want.GetElement().GetBundleName().c_str(), needStartCaller, isFromBroker);
    int32_t callingPid = IPCSkeleton::GetCallingPid();
    uint32_t callerToken = IPCSkeleton::GetCallingTokenID();
    auto task = [this, info, needStartCaller, isFromBroker, callingPid, callerToken]() {
        sptr<SceneSession> sceneSession = FindSessionByToken(info->sessionToken);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_LIFE, "TerminateSessionNew:fail to find session by token.");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        const bool pidCheck = (callingPid != -1) && (callingPid == sceneSession->GetCallingPid());
        if (!pidCheck &&
            !SessionPermission::VerifyPermissionByCallerToken(callerToken,
                PermissionConstants::PERMISSION_MANAGE_MISSION)) {
            TLOGE(WmsLogTag::WMS_LIFE,
                "The caller has not permission granted, callingPid_:%{public}d, callingPid:%{public}d",
                sceneSession->GetCallingPid(), callingPid);
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        WSError errCode = sceneSession->TerminateSessionNew(info, needStartCaller, isFromBroker);
        return errCode;
    };
    return taskScheduler_->PostSyncTask(task, "TerminateSessionNew");
}

WSError SceneSessionManager::SetVmaCacheStatus(bool flag)
{
    WLOGFI("flag: %{public}d", flag);
    RSInterfaces::GetInstance().SetVmaCacheStatus(flag);
    return WSError::WS_OK;
}

WSError SceneSessionManager::GetSessionSnapshot(const std::string& deviceId, int32_t persistentId,
                                                SessionSnapshot& snapshot, bool isLowResolution)
{
    WLOGFI("id: %{public}d isLowResolution: %{public}d", persistentId, isLowResolution);
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &deviceId, persistentId, &snapshot, isLowResolution]() {
        if (CheckIsRemote(deviceId)) {
            int ret = GetRemoteSessionSnapshotInfo(deviceId, persistentId, snapshot);
            if (ret != ERR_OK) {
                return WSError::WS_ERROR_INVALID_PARAM;
            } else {
                return WSError::WS_OK;
            }
        }
        sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
        if (!sceneSession) {
            WLOGFE("fail to find session by persistentId: %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        auto sessionInfo = sceneSession->GetSessionInfo();
        if (sessionInfo.abilityName_.empty() || sessionInfo.moduleName_.empty() || sessionInfo.bundleName_.empty()) {
            WLOGFW("sessionInfo: %{public}d, abilityName or moduleName or bundleName is empty",
                   sceneSession->GetPersistentId());
        }
        snapshot.topAbility.SetElementBundleName(&(snapshot.topAbility), sessionInfo.bundleName_.c_str());
        snapshot.topAbility.SetElementModuleName(&(snapshot.topAbility), sessionInfo.moduleName_.c_str());
        snapshot.topAbility.SetElementAbilityName(&(snapshot.topAbility), sessionInfo.abilityName_.c_str());
        auto oriSnapshot = sceneSession->Snapshot();
        if (oriSnapshot != nullptr) {
            if (isLowResolution) {
                OHOS::Media::InitializationOptions options;
                options.size.width = oriSnapshot->GetWidth() / 2; // low resolution ratio
                options.size.height = oriSnapshot->GetHeight() / 2; // low resolution ratio
                std::unique_ptr<OHOS::Media::PixelMap> reducedPixelMap = OHOS::Media::PixelMap::Create(*oriSnapshot, options);
                snapshot.snapshot = std::shared_ptr<OHOS::Media::PixelMap>(reducedPixelMap.release());
            } else {
                snapshot.snapshot = oriSnapshot;
            }
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionSnapshot");
}

WMError SceneSessionManager::GetSessionSnapshotById(int32_t persistentId, SessionSnapshot& snapshot)
{
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI() && !SessionPermission::IsShellCall()) {
        TLOGW(WmsLogTag::WMS_SYSTEM, "Get snapshot failed, Get snapshot by id must be system app!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    auto task = [this, persistentId, &snapshot]() {
        sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
        if (!sceneSession) {
            TLOGW(WmsLogTag::WMS_SYSTEM, "fail to find session by persistentId: %{public}d", persistentId);
            return WMError::WM_ERROR_INVALID_PARAM;
        }
        auto sessionInfo = sceneSession->GetSessionInfo();
        if (sessionInfo.abilityName_.empty() || sessionInfo.moduleName_.empty() || sessionInfo.bundleName_.empty()) {
            TLOGW(WmsLogTag::WMS_SYSTEM, "sessionInfo: %{public}d, abilityName or moduleName or bundleName is empty",
                sceneSession->GetPersistentId());
        }
        snapshot.topAbility.SetBundleName(sessionInfo.bundleName_.c_str());
        snapshot.topAbility.SetModuleName(sessionInfo.moduleName_.c_str());
        snapshot.topAbility.SetAbilityName(sessionInfo.abilityName_.c_str());
        auto oriSnapshot = sceneSession->Snapshot();
        if (oriSnapshot != nullptr) {
            snapshot.snapshot = oriSnapshot;
            return WMError::WM_OK;
        }
        return WMError::WM_ERROR_NULLPTR;
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionSnapshotById");
}

WSError SceneSessionManager::GetUIContentRemoteObj(int32_t persistentId, sptr<IRemoteObject>& uiContentRemoteObj)
{
    if (!SessionPermission::IsSACalling()) {
        TLOGE(WmsLogTag::DEFAULT, "Permission denied!");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    TLOGI(WmsLogTag::DEFAULT, "PersistentId=%{public}d", persistentId);
    sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "sceneSession is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    return sceneSession->GetUIContentRemoteObj(uiContentRemoteObj);
}

int SceneSessionManager::GetRemoteSessionSnapshotInfo(const std::string& deviceId, int32_t sessionId,
                                                      AAFwk::MissionSnapshot& sessionSnapshot)
{
    TLOGI(WmsLogTag::DEFAULT, "begin");
    int result = DistributedClient::GetInstance().GetRemoteMissionSnapshotInfo(deviceId,
        sessionId, sessionSnapshot);
    if (result != ERR_OK) {
        TLOGE(WmsLogTag::DEFAULT, "failed, result = %{public}d", result);
        return result;
    }
    return ERR_OK;
}

sptr<AAFwk::IAbilityManagerCollaborator> SceneSessionManager::GetCollaboratorByType(int32_t collaboratorType)
{
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator = nullptr;
    std::shared_lock<std::shared_mutex> lock(collaboratorMapLock_);
    auto iter = collaboratorMap_.find(collaboratorType);
    if (iter == collaboratorMap_.end()) {
        TLOGE(WmsLogTag::DEFAULT, "Fail to found collaborator with type: %{public}d", collaboratorType);
        return collaborator;
    }
    collaborator = iter->second;
    if (collaborator == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "Find collaborator type %{public}d, but value is nullptr!", collaboratorType);
    }
    return collaborator;
}

WSError SceneSessionManager::RequestSceneSessionByCall(const sptr<SceneSession>& sceneSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        if (!GetSceneSession(persistentId)) {
            WLOGFE("session is invalid with %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto sessionInfo = scnSession->GetSessionInfo();
        auto abilitySessionInfo = SetAbilitySessionInfo(scnSession);
        if (!abilitySessionInfo) {
            TLOGE(WmsLogTag::WMS_MAIN,
                "RequestSceneSessionByCall abilitySessionInfo is null, id:%{public}d", persistentId);
            return WSError::WS_ERROR_NULLPTR;
        }
        if (sessionInfo.callState_ == static_cast<uint32_t>(AAFwk::CallToState::BACKGROUND)) {
            scnSession->SetActive(false);
        } else if (sessionInfo.callState_ == static_cast<uint32_t>(AAFwk::CallToState::FOREGROUND)) {
            scnSession->SetActive(true);
        } else {
            WLOGFE("wrong callState_");
        }
        TLOGI(WmsLogTag::WMS_MAIN, "RequestSceneSessionByCall state:%{public}d, id:%{public}d",
            sessionInfo.callState_, persistentId);
        bool isColdStart = false;
        AAFwk::AbilityManagerClient::GetInstance()->CallUIAbilityBySCB(abilitySessionInfo, isColdStart);
        if (isColdStart) {
            TLOGI(WmsLogTag::WMS_MAIN, "ColdStart, identityToken:%{public}s, bundleName:%{public}s",
                abilitySessionInfo->identityToken.c_str(), sessionInfo.bundleName_.c_str());
            scnSession->SetClientIdentityToken(abilitySessionInfo->identityToken);
            scnSession->ResetSessionConnectState();
        }
        scnSession->RemoveLifeCycleTask(LifeCycleTaskType::START);
        return WSError::WS_OK;
    };
    std::string taskName = "RequestSceneSessionByCall:PID:" +
        (sceneSession != nullptr ? std::to_string(sceneSession->GetPersistentId()):"nullptr");
    taskScheduler_->PostAsyncTask(task, taskName);
    return WSError::WS_OK;
}

void SceneSessionManager::StartAbilityBySpecified(const SessionInfo& sessionInfo)
{
    auto task = [this, sessionInfo]() {
        WLOGFI("StartAbilityBySpecified: bundleName: %{public}s, moduleName: %{public}s, abilityName: %{public}s",
            sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str());
        AAFwk::Want want;
        want.SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_, sessionInfo.moduleName_);
        if (sessionInfo.want != nullptr) {
            want.SetParams(sessionInfo.want->GetParams());
        }
        AAFwk::AbilityManagerClient::GetInstance()->StartSpecifiedAbilityBySCB(want);
    };

    taskScheduler_->PostAsyncTask(task, "StartAbilityBySpecified:PID:" + sessionInfo.bundleName_);
}

void SceneSessionManager::NotifyWindowStateErrorFromMMI(int32_t pid, int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_LIFE, "pid: %{public}d, persistentId: %{public}d", pid, persistentId);
    if (pid == -1) {
        TLOGE(WmsLogTag::WMS_LIFE, "invalid pid");
        return;
    }
    int32_t ret = HiSysEventWrite(
        HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "WINDOW_STATE_ERROR",
        HiviewDFX::HiSysEvent::EventType::FAULT,
        "PID", pid,
        "PERSISTENT_ID", persistentId);
    if (ret != 0) {
        TLOGE(WmsLogTag::WMS_LIFE, "write HiSysEvent error, ret: %{public}d", ret);
    }
    auto task = [this, pid] {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& [_, sceneSession] : sceneSessionMap_) {
            if (!sceneSession || pid != sceneSession->GetCallingPid() ||
                !WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
                continue;
            }
            auto abilitySessionInfo = SetAbilitySessionInfo(sceneSession);
            if (abilitySessionInfo) {
                TLOGI(WmsLogTag::WMS_LIFE, "terminate session, persistentId: %{public}d",
                    abilitySessionInfo->persistentId);
                sceneSession->TerminateSessionNew(abilitySessionInfo, false, false);
            }
        }
    };
    // delay 2000ms, wait for hidumper
    taskScheduler_->PostAsyncTask(task, __func__, 2000);
}

sptr<SceneSession> SceneSessionManager::FindMainWindowWithToken(sptr<IRemoteObject> targetToken)
{
    if (!targetToken) {
        WLOGFE("Token is null, cannot find main window");
        return nullptr;
    }

    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    auto iter = std::find_if(sceneSessionMap_.begin(), sceneSessionMap_.end(),
        [targetToken](const std::map<uint64_t, sptr<SceneSession>>::value_type& pair) {
            if (pair.second->IsTerminated()) {
                return false;
            }
            if (pair.second->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                return pair.second->GetAbilityToken() == targetToken;
            }
            return false;
        });
    if (iter == sceneSessionMap_.end()) {
        WLOGFE("Cannot find session");
        return nullptr;
    }
    return iter->second;
}

WSError SceneSessionManager::BindDialogSessionTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_DIALOG, "BindDialogSessionTarget permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (targetToken == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "Target token is null");
        return WSError::WS_ERROR_NULLPTR;
    }

    auto task = [this, persistentId, targetToken]() {
        auto scnSession = GetSceneSession(static_cast<int32_t>(persistentId));
        if (scnSession == nullptr) {
            TLOGE(WmsLogTag::WMS_DIALOG, "Session is nullptr, persistentId:%{public}" PRIu64, persistentId);
            return WSError::WS_ERROR_NULLPTR;
        }
        if (scnSession->GetWindowType() != WindowType::WINDOW_TYPE_DIALOG) {
            TLOGE(WmsLogTag::WMS_DIALOG, "Session is not dialog, type:%{public}u", scnSession->GetWindowType());
            return WSError::WS_OK;
        }
        scnSession->dialogTargetToken_ = targetToken;
        sptr<SceneSession> parentSession = FindMainWindowWithToken(targetToken);
        if (parentSession == nullptr) {
            scnSession->NotifyDestroy();
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        scnSession->SetParentSession(parentSession);
        scnSession->SetParentPersistentId(parentSession->GetPersistentId());
        UpdateParentSessionForDialog(scnSession, scnSession->GetSessionProperty());
        TLOGI(WmsLogTag::WMS_DIALOG, "Bind dialog success, dialog id %{public}" PRIu64 ", parentId %{public}d",
            persistentId, parentSession->GetPersistentId());
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "BindDialogTarget:PID:" + std::to_string(persistentId));
}

void DisplayChangeListener::OnGetSurfaceNodeIdsFromMissionIds(std::vector<uint64_t>& missionIds,
    std::vector<uint64_t>& surfaceNodeIds)
{
    SceneSessionManager::GetInstance().GetSurfaceNodeIdsFromMissionIds(missionIds, surfaceNodeIds);
}

WMError SceneSessionManager::GetSurfaceNodeIdsFromMissionIds(std::vector<uint64_t>& missionIds,
    std::vector<uint64_t>& surfaceNodeIds)
{
    auto isSaCall = SessionPermission::IsSACalling();
    if (!isSaCall) {
        WLOGFE("The interface only support for sa call");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &missionIds, &surfaceNodeIds]() {
        std::map<int32_t, sptr<SceneSession>>::iterator iter;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto missionId : missionIds) {
            iter = sceneSessionMap_.find(static_cast<int32_t>(missionId));
            if (iter == sceneSessionMap_.end()) {
                continue;
            }
            auto sceneSession = iter->second;
            if (sceneSession == nullptr) {
                continue;
            }
            if (sceneSession->GetSurfaceNode() == nullptr) {
                continue;
            }
            surfaceNodeIds.push_back(sceneSession->GetSurfaceNode()->GetId());
            if (sceneSession->GetLeashWinSurfaceNode()) {
                surfaceNodeIds.push_back(sceneSession->GetLeashWinSurfaceNode()->GetId());
            }
        }
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetSurfaceNodeIdsFromMissionIds");
}

WMError SceneSessionManager::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if (type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_DRAWING_STATE ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_VISIBLE_WINDOW_NUM ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_MODE ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_PID_VISIBILITY) {
        if (!SessionPermission::IsSACalling()) {
            TLOGE(WmsLogTag::WMS_LIFE, "permission denied!");
            return WMError::WM_ERROR_INVALID_PERMISSION;
        }
    }
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        TLOGD(WmsLogTag::DEFAULT, "permission denied");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    const auto callingPid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, windowManagerAgent, type, callingPid]() {
        return SessionManagerAgentController::GetInstance()
            .RegisterWindowManagerAgent(windowManagerAgent, type, callingPid);
    };
    return taskScheduler_->PostSyncTask(task, "RegisterWindowManagerAgent");
}

WMError SceneSessionManager::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if (type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_GESTURE_NAVIGATION_ENABLED ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG) {
        if (!SessionPermission::IsSystemCalling()) {
            WLOGFE("UnregisterWindowManagerAgent permission denied!");
            return WMError::WM_ERROR_NOT_SYSTEM_APP;
        }
    }
    if (type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_VISIBILITY ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_DRAWING_STATE ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_VISIBLE_WINDOW_NUM ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WINDOW_MODE) {
        if (!SessionPermission::IsSACalling()) {
            TLOGE(WmsLogTag::WMS_LIFE, "permission denied!");
            return WMError::WM_ERROR_INVALID_PERMISSION;
        }
    }
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    const auto callingPid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, windowManagerAgent, type, callingPid]() {
        return SessionManagerAgentController::GetInstance()
            .UnregisterWindowManagerAgent(windowManagerAgent, type, callingPid);
    };
    return taskScheduler_->PostSyncTask(task, "UnregisterWindowManagerAgent");
}

void SceneSessionManager::UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    SessionManagerAgentController::GetInstance().UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
}

void SceneSessionManager::UpdateCameraWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    SessionManagerAgentController::GetInstance().UpdateCameraWindowStatus(accessTokenId, isShowing);
}

void SceneSessionManager::StartWindowInfoReportLoop()
{
    WLOGFD("in");
    if (isReportTaskStart_) {
        WLOGFE("Report is ReportTask Start");
        return;
    }
    auto task = [this] {
        WindowInfoReporter::GetInstance().ReportRecordedInfos();
        ReportWindowProfileInfos();
        isReportTaskStart_ = false;
        StartWindowInfoReportLoop();
    };
    int64_t delayTime = 1000 * 60 * 60; // an hour.
    bool ret = eventHandler_->PostTask(task, "wms:WindowInfoReport", delayTime);
    if (!ret) {
        WLOGFE("failed. task is WindowInfoReport");
        return;
    }
    isReportTaskStart_ = true;
}

void SceneSessionManager::InitPersistentStorage()
{
    if (ScenePersistentStorage::HasKey("maximize_state", ScenePersistentStorageType::MAXIMIZE_STATE)) {
        int32_t storageMode = -1;
        ScenePersistentStorage::Get("maximize_state", storageMode, ScenePersistentStorageType::MAXIMIZE_STATE);
        if (storageMode == static_cast<int32_t>(MaximizeMode::MODE_AVOID_SYSTEM_BAR) ||
            storageMode == static_cast<int32_t>(MaximizeMode::MODE_FULL_FILL)) {
            WLOGFI("init MaximizeMode as %{public}d from persistent storage", storageMode);
            SceneSession::maximizeMode_ = static_cast<MaximizeMode>(storageMode);
        }
    }
}

WMError SceneSessionManager::GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos)
{
    WLOGFD("Called.");
    if (!SessionPermission::IsSystemServiceCalling()) {
        WLOGFE("Only support for system service.");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    auto task = [this, &infos]() {
        std::map<int32_t, sptr<SceneSession>>::iterator iter;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (iter = sceneSessionMap_.begin(); iter != sceneSessionMap_.end(); iter++) {
            sptr<SceneSession> sceneSession = iter->second;
            if (sceneSession == nullptr) {
                WLOGFW("null scene session");
                continue;
            }
            bool isVisibleForAccessibility = Session::IsScbCoreEnabled() ?
                sceneSession->IsVisibleForAccessibility() :
                sceneSession->IsVisibleForAccessibility() && IsSessionVisibleForeground(sceneSession);
            WLOGFD("name=%{public}s, isSystem=%{public}d, persistentId=%{public}d, winType=%{public}d, "
                "state=%{public}d, visible=%{public}d", sceneSession->GetWindowName().c_str(),
                sceneSession->GetSessionInfo().isSystem_, iter->first, sceneSession->GetWindowType(),
                sceneSession->GetSessionState(), isVisibleForAccessibility);
            if (isVisibleForAccessibility) {
                FillWindowInfo(infos, iter->second);
            }
        }
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetAccessibilityWindowInfo");
}

static bool CheckUnreliableWindowType(WindowType windowType)
{
    if (windowType == WindowType::WINDOW_TYPE_APP_SUB_WINDOW ||
        windowType == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT ||
        windowType == WindowType::WINDOW_TYPE_TOAST) {
        return true;
    }
    TLOGD(WmsLogTag::DEFAULT, "false, WindowType = %{public}d", windowType);
    return false;
}

static void FillUnreliableWindowInfo(const sptr<SceneSession>& sceneSession,
    std::vector<sptr<UnreliableWindowInfo>>& infos)
{
    if (sceneSession == nullptr) {
        TLOGW(WmsLogTag::DEFAULT, "null scene session.");
        return;
    }
    if (sceneSession->GetSessionInfo().bundleName_.find("SCBGestureBack") != std::string::npos ||
        sceneSession->GetSessionInfo().bundleName_.find("SCBGestureNavBar") != std::string::npos ||
        sceneSession->GetSessionInfo().bundleName_.find("SCBGestureTopBar") != std::string::npos) {
        TLOGD(WmsLogTag::DEFAULT, "filter gesture window.");
        return;
    }
    sptr<UnreliableWindowInfo> info = sptr<UnreliableWindowInfo>::MakeSptr();
    info->windowId_ = sceneSession->GetPersistentId();
    WSRect windowRect = sceneSession->GetSessionRect();
    info->windowRect_ = { windowRect.posX_, windowRect.posY_, windowRect.width_, windowRect.height_ };
    info->zOrder_ = sceneSession->GetZOrder();
    info->floatingScale_ = sceneSession->GetFloatingScale();
    info->scaleX_ = sceneSession->GetScaleX();
    info->scaleY_ = sceneSession->GetScaleY();
    infos.emplace_back(info);
    TLOGD(WmsLogTag::WMS_MAIN, "wid = %{public}d", info->windowId_);
}

WMError SceneSessionManager::GetUnreliableWindowInfo(int32_t windowId,
    std::vector<sptr<UnreliableWindowInfo>>& infos)
{
    TLOGD(WmsLogTag::DEFAULT, "Called.");
    if (!SessionPermission::IsSystemServiceCalling()) {
        TLOGE(WmsLogTag::DEFAULT, "only support for system service.");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    auto task = [this, windowId, &infos]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& [sessionId, sceneSession] : sceneSessionMap_) {
            if (sceneSession == nullptr) {
                TLOGW(WmsLogTag::DEFAULT, "null scene session");
                continue;
            }
            if (sessionId == windowId) {
                TLOGI(WmsLogTag::DEFAULT, "persistentId: %{public}d is parameter chosen", sessionId);
                FillUnreliableWindowInfo(sceneSession, infos);
                continue;
            }
            if (!sceneSession->GetRSVisible()) {
                TLOGD(WmsLogTag::DEFAULT, "persistentId: %{public}d is not visible", sessionId);
                continue;
            }
            TLOGD(WmsLogTag::DEFAULT, "name = %{public}s, isSystem = %{public}d, "
                "persistentId = %{public}d, winType = %{public}d, state = %{public}d, visible = %{public}d",
                sceneSession->GetWindowName().c_str(), sceneSession->GetSessionInfo().isSystem_, sessionId,
                sceneSession->GetWindowType(), sceneSession->GetSessionState(), sceneSession->GetRSVisible());
            if (CheckUnreliableWindowType(sceneSession->GetWindowType())) {
                TLOGI(WmsLogTag::DEFAULT, "persistentId = %{public}d, "
                    "WindowType = %{public}d", sessionId, sceneSession->GetWindowType());
                FillUnreliableWindowInfo(sceneSession, infos);
            }
        }
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetUnreliableWindowInfo");
}

void SceneSessionManager::NotifyWindowInfoChange(int32_t persistentId, WindowUpdateType type)
{
    WLOGFD("NotifyWindowInfoChange, persistentId = %{public}d, updateType = %{public}d", persistentId, type);
    sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("NotifyWindowInfoChange sceneSession nullptr!");
        return;
    }
    wptr<SceneSession> weakSceneSession(sceneSession);
    if (processingFlushUIParams_.load()) {
        TLOGD(WmsLogTag::WMS_PIPELINE, "Processing flush, notify later.");
        auto task = [this, weakSceneSession, type]() {
            auto sceneSession = weakSceneSession.promote();
            if (WindowChangedFunc_ != nullptr && sceneSession != nullptr &&
                sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                WindowChangedFunc_(sceneSession->GetPersistentId(), type);
            }
        };
        taskScheduler_->PostAsyncTask(task, "WindowChangeFunc:id:" + std::to_string(persistentId));
        return;
    }
    auto task = [this, weakSceneSession, type]() {
        auto sceneSession = weakSceneSession.promote();
        NotifyAllAccessibilityInfo();
        if (WindowChangedFunc_ != nullptr && sceneSession != nullptr &&
            sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            WindowChangedFunc_(sceneSession->GetPersistentId(), type);
        }
    };
    taskScheduler_->PostAsyncTask(task, "NotifyWindowInfoChange:PID:" + std::to_string(persistentId));
    
    auto notifySceneInputTask = [weakSceneSession, type]() {
        auto sceneSession = weakSceneSession.promote();
        if (sceneSession == nullptr) {
            return;
        }
        SceneInputManager::GetInstance().NotifyWindowInfoChange(sceneSession, type);
    };
    taskScheduler_->PostAsyncTask(notifySceneInputTask);
}

bool SceneSessionManager::FillWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos,
    const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFW("null scene session.");
        return false;
    }
    if (sceneSession->GetSessionInfo().bundleName_.find("SCBGestureBack") != std::string::npos ||
        sceneSession->GetSessionInfo().bundleName_.find("SCBGestureNavBar") != std::string::npos ||
        sceneSession->GetSessionInfo().bundleName_.find("SCBGestureTopBar") != std::string::npos) {
        WLOGFD("filter gesture window.");
        return false;
    }
    sptr<AccessibilityWindowInfo> info = sptr<AccessibilityWindowInfo>::MakeSptr();
    if (sceneSession->GetSessionInfo().isSystem_) {
        info->wid_ = 1;
        info->innerWid_ = static_cast<int32_t>(sceneSession->GetPersistentId());
    } else {
        info->wid_ = static_cast<int32_t>(sceneSession->GetPersistentId());
    }
    info->uiNodeId_ = sceneSession->GetUINodeId();
    WSRect wsrect = sceneSession->GetSessionGlobalRect(); // only accessability and mmi need global
    info->windowRect_ = {wsrect.posX_, wsrect.posY_, wsrect.width_, wsrect.height_ };
    info->focused_ = sceneSession->GetPersistentId() == focusedSessionId_;
    info->type_ = sceneSession->GetWindowType();
    info->mode_ = sceneSession->GetWindowMode();
    info->layer_ = sceneSession->GetZOrder();
    info->scaleVal_ = sceneSession->GetFloatingScale();
    info->scaleX_ = sceneSession->GetScaleX();
    info->scaleY_ = sceneSession->GetScaleY();
    info->bundleName_ = sceneSession->GetSessionInfo().bundleName_;
    info->touchHotAreas_ = sceneSession->GetTouchHotAreas();
    auto property = sceneSession->GetSessionProperty();
    if (property != nullptr) {
        info->displayId_ = property->GetDisplayId();
        info->isDecorEnable_ = property->IsDecorEnable();
    }
    infos.emplace_back(info);
    TLOGD(WmsLogTag::WMS_MAIN, "wid = %{public}d, inWid = %{public}d, uiNId = %{public}d, bundleName = %{public}s",
        info->wid_, info->innerWid_, info->uiNodeId_, info->bundleName_.c_str());
    return true;
}

std::string SceneSessionManager::GetSessionSnapshotFilePath(int32_t persistentId)
{
    WLOGFI("GetSessionSnapshotFilePath persistentId %{public}d", persistentId);
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("GetSessionSnapshotFilePath sceneSession nullptr!");
        return "";
    }
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return std::string("");
        }
        std::string filePath = scnSession->GetSessionSnapshotFilePath();
        return filePath;
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionSnapshotFilePath" + std::to_string(persistentId));
}

sptr<SceneSession> SceneSessionManager::SelectSesssionFromMap(const uint64_t& surfaceId)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession == nullptr) {
            continue;
        }
        if (sceneSession->GetSurfaceNode() == nullptr) {
            continue;
        }
        if (surfaceId == sceneSession->GetSurfaceNode()->GetId()) {
            return sceneSession;
        }
    }
    return nullptr;
}

void SceneSessionManager::WindowLayerInfoChangeCallback(std::shared_ptr<RSOcclusionData> occlusiontionData)
{
    WLOGFD("WindowLayerInfoChangeCallback: entry");
    std::weak_ptr<RSOcclusionData> weak(occlusiontionData);

    auto task = [this, weak]() {
        auto weakOcclusionData = weak.lock();
        if (weakOcclusionData == nullptr) {
            WLOGFE("weak occlusionData is nullptr");
            return;
        }
        std::vector<std::pair<uint64_t, WindowVisibilityState>> currVisibleData;
        std::vector<std::pair<uint64_t, bool>> currDrawingContentData;
        GetWindowLayerChangeInfo(weakOcclusionData, currVisibleData, currDrawingContentData);
        std::vector<std::pair<uint64_t, WindowVisibilityState>> visibilityChangeInfos;
        if (currVisibleData.size() != 0) {
            visibilityChangeInfos = GetWindowVisibilityChangeInfo(currVisibleData);
        }
        if (visibilityChangeInfos.size() != 0) {
            DealwithVisibilityChange(visibilityChangeInfos, currVisibleData);
            CacVisibleWindowNum();
        }

        std::vector<std::pair<uint64_t, bool>> drawingContentChangeInfos;
        if (currDrawingContentData.size() != 0) {
            drawingContentChangeInfos = GetWindowDrawingContentChangeInfo(currDrawingContentData);
        }
        if (drawingContentChangeInfos.size() != 0) {
            DealwithDrawingContentChange(drawingContentChangeInfos);
        }
    };
    taskScheduler_->PostVoidSyncTask(task, "WindowLayerInfoChangeCallback");
}

void SceneSessionManager::GetWindowLayerChangeInfo(std::shared_ptr<RSOcclusionData> occlusiontionData,
    std::vector<std::pair<uint64_t, WindowVisibilityState>>& currVisibleData,
    std::vector<std::pair<uint64_t, bool>>& currDrawingContentData)
{
    VisibleData& rsVisibleData = occlusiontionData->GetVisibleData();
    for (auto iter = rsVisibleData.begin(); iter != rsVisibleData.end(); iter++) {
        WindowLayerState windowLayerState = static_cast<WindowLayerState>(iter->second);
        switch (windowLayerState) {
            case WINDOW_ALL_VISIBLE:
            case WINDOW_SEMI_VISIBLE:
            case WINDOW_IN_VISIBLE:
                currVisibleData.emplace_back(iter->first, static_cast<WindowVisibilityState>(iter->second));
                break;
            case WINDOW_LAYER_DRAWING:
                currDrawingContentData.emplace_back(iter->first, true);
                break;
            case WINDOW_LAYER_NO_DRAWING:
                currDrawingContentData.emplace_back(iter->first, false);
                break;
            default:
                break;
        }
    }
}

void SceneSessionManager::UpdateSubWindowVisibility(const sptr<SceneSession>& session,
    WindowVisibilityState visibleState,
    const std::vector<std::pair<uint64_t, WindowVisibilityState>>& visibilityChangeInfo,
    std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos, std::string& visibilityInfo,
    const std::vector<std::pair<uint64_t, WindowVisibilityState>>& currVisibleData)
{
    if (WindowHelper::IsMainWindow(session->GetWindowType()) &&
            visibleState < WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION) {
        auto subSessions = GetSubSceneSession(session->GetWindowId());
        if (subSessions.empty()) {
            return;
        }

        RemoveDuplicateSubSession(visibilityChangeInfo, subSessions);

        for (const auto& subSession : subSessions) {
            if (subSession == nullptr) {
                continue;
            }
            if (subSession->IsSessionForeground() || GetSessionRSVisible(subSession, currVisibleData)) {
                TLOGI(WmsLogTag::DEFAULT, "Update subwindow visibility for winId: %{public}d",
                    subSession->GetWindowId());
                SetSessionVisibilityInfo(subSession, visibleState, windowVisibilityInfos, visibilityInfo);
            }
        }
    }
}

bool SceneSessionManager::GetSessionRSVisible(const sptr<Session>& session,
    const std::vector<std::pair<uint64_t, WindowVisibilityState>>& currVisibleData)
{
    bool sessionRSVisible = false;
    for (const auto& elem : currVisibleData) {
        uint64_t surfaceId = elem.first;
        WindowVisibilityState visibleState = elem.second;
        bool isVisible = visibleState < WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION;
        sptr<SceneSession> visibilitySession = SelectSesssionFromMap(surfaceId);
        if (visibilitySession == nullptr) {
            continue;
        }
        if (session->GetWindowId() == visibilitySession->GetWindowId()) {
            if (isVisible) {
                sessionRSVisible = true;
            }
            break;
        }
    }
    return sessionRSVisible;
}

void SceneSessionManager::SetSessionVisibilityInfo(const sptr<SceneSession>& session,
    WindowVisibilityState visibleState, std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos,
    std::string& visibilityInfo)
{
    if (session == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "Session is invalid!");
        return;
    }
    session->SetRSVisible(visibleState < WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION);
    session->SetVisibilityState(visibleState);
    int32_t windowId = session->GetWindowId();
    if (windowVisibilityListenerSessionSet_.find(windowId) != windowVisibilityListenerSessionSet_.end()) {
        session->NotifyWindowVisibility();
    }
    windowVisibilityInfos.emplace_back(sptr<WindowVisibilityInfo>::MakeSptr(
        windowId, session->GetCallingPid(), session->GetCallingUid(), visibleState, session->GetWindowType()));

    visibilityInfo +=
        "[" + session->GetWindowName() + ", " + std::to_string(windowId) + ", " + std::to_string(visibleState) + "], ";
}

void SceneSessionManager::RemoveDuplicateSubSession(
    const std::vector<std::pair<uint64_t, WindowVisibilityState>>& visibilityChangeInfo,
    std::vector<sptr<SceneSession>>& subSessions)
{
    for (const auto& elem : visibilityChangeInfo) {
        uint64_t surfaceId = elem.first;
        sptr<SceneSession> session = SelectSesssionFromMap(surfaceId);
        if (session == nullptr) {
            continue;
        }
        for (auto iterator = subSessions.begin(); iterator != subSessions.end();) {
            auto subSession = *iterator;
            if (subSession && subSession->GetWindowId() == session->GetWindowId()) {
                iterator = subSessions.erase(iterator);
            } else {
                ++iterator;
            }
        }
    }
}

std::vector<sptr<SceneSession>> SceneSessionManager::GetSubSceneSession(int32_t parentWindowId)
{
    std::vector<sptr<SceneSession>> subSessions;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& iter : sceneSessionMap_) {
        auto sceneSession = iter.second;
        if (sceneSession == nullptr) {
            continue;
        }
        if (sceneSession->GetParentSession() != nullptr &&
            sceneSession->GetParentSession()->GetWindowId() == parentWindowId) {
            subSessions.push_back(sceneSession);
        }
    }
    return subSessions;
}

std::vector<std::pair<uint64_t, WindowVisibilityState>> SceneSessionManager::GetWindowVisibilityChangeInfo(
    std::vector<std::pair<uint64_t, WindowVisibilityState>>& currVisibleData)
{
    std::vector<std::pair<uint64_t, WindowVisibilityState>> visibilityChangeInfo;
    std::sort(currVisibleData.begin(), currVisibleData.end(), Comp);
    uint32_t i, j;
    i = j = 0;
    for (; i < lastVisibleData_.size() && j < currVisibleData.size();) {
        if (lastVisibleData_[i].first < currVisibleData[j].first) {
            visibilityChangeInfo.emplace_back(lastVisibleData_[i].first, WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION);
            i++;
        } else if (lastVisibleData_[i].first > currVisibleData[j].first &&
                currVisibleData[j].second != WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION) {
            visibilityChangeInfo.emplace_back(currVisibleData[j].first, currVisibleData[j].second);
            j++;
        } else {
            if (lastVisibleData_[i].second != currVisibleData[j].second) {
                visibilityChangeInfo.emplace_back(currVisibleData[j].first, currVisibleData[j].second);
            }
            i++;
            j++;
        }
    }
    for (; i < lastVisibleData_.size(); ++i) {
        visibilityChangeInfo.emplace_back(lastVisibleData_[i].first, WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION);
    }
    for (; j < currVisibleData.size(); ++j) {
        if (currVisibleData[j].second != WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION) {
            visibilityChangeInfo.emplace_back(currVisibleData[j].first, currVisibleData[j].second);
        }
    }
    lastVisibleData_ = currVisibleData;
    return visibilityChangeInfo;
}

void SceneSessionManager::DealwithVisibilityChange(const std::vector<std::pair<uint64_t, WindowVisibilityState>>&
    visibilityChangeInfo, const std::vector<std::pair<uint64_t, WindowVisibilityState>>& currVisibleData)
{
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
#ifdef MEMMGR_WINDOW_ENABLE
    std::vector<sptr<Memory::MemMgrWindowInfo>> memMgrWindowInfos;
#endif

    std::string visibilityInfo = "WindowVisibilityInfos [name, winId, visibleState]: ";
    for (const auto& elem : visibilityChangeInfo) {
        uint64_t surfaceId = elem.first;
        WindowVisibilityState visibleState = elem.second;
        bool isVisible = visibleState < WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION;
        sptr<SceneSession> session = SelectSesssionFromMap(surfaceId);
        if (session == nullptr) {
            continue;
        }
        if ((WindowHelper::IsSubWindow(session->GetWindowType()) ||
            session->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) && isVisible == true) {
            if (session->GetParentSession() != nullptr &&
                !session->GetParentSession()->IsSessionForeground() &&
                !GetSessionRSVisible(session->GetParentSession(), currVisibleData)) {
                continue;
            }
        }
        SetSessionVisibilityInfo(session, visibleState, windowVisibilityInfos, visibilityInfo);
        UpdateSubWindowVisibility(session, visibleState, visibilityChangeInfo, windowVisibilityInfos, visibilityInfo, currVisibleData);
#ifdef MEMMGR_WINDOW_ENABLE
        memMgrWindowInfos.emplace_back(new Memory::MemMgrWindowInfo(session->GetWindowId(), session->GetCallingPid(),
            session->GetCallingUid(), isVisible));
#endif
        CheckAndNotifyWaterMarkChangedResult();
    }
    if (windowVisibilityInfos.size() != 0) {
        WLOGI("Visibility changed, size: %{public}zu, %{public}s", windowVisibilityInfos.size(),
            visibilityInfo.c_str());
        SessionManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
    }
#ifdef MEMMGR_WINDOW_ENABLE
    if (memMgrWindowInfos.size() != 0) {
        WLOGD("Notify memMgrWindowInfos changed start");
        taskScheduler_ ->AddExportTask("notifyMemMgr", [memMgrWindowInfos = std::move(memMgrWindowInfos)]() {
            Memory::MemMgrClient::GetInstance().OnWindowVisibilityChanged(memMgrWindowInfos);
        });
    }
#endif
}

void SceneSessionManager::DealwithDrawingContentChange(const std::vector<std::pair<uint64_t, bool>>&
    drawingContentChangeInfo)
{
    std::vector<sptr<WindowDrawingContentInfo>> windowDrawingContenInfos;
    for (const auto& elem : drawingContentChangeInfo) {
        uint64_t surfaceId = elem.first;
        bool drawingState = elem.second;
        sptr<SceneSession> session = SelectSesssionFromMap(surfaceId);
        if (session == nullptr) {
            continue;
        }
        windowDrawingContenInfos.emplace_back(new WindowDrawingContentInfo(session->GetWindowId(),
            session->GetCallingPid(), session->GetCallingUid(), drawingState, session->GetWindowType()));
        if (openDebugTrace_) {
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "Drawing status changed pid:(%d ) surfaceId:(%" PRIu64 ")"
                "drawingState:(%d )", session->GetCallingPid(), surfaceId, drawingState);
        }
        WLOGFD("drawing status changed pid:%{public}d, "
            "surfaceId:%{public}" PRIu64 ", drawingState:%{public}d", session->GetCallingPid(), surfaceId, drawingState);
    }
    if (windowDrawingContenInfos.size() != 0) {
        WLOGFD("Notify WindowDrawingContenInfo changed start");
        SessionManagerAgentController::GetInstance().UpdateWindowDrawingContentInfo(windowDrawingContenInfos);
    }
}

std::vector<std::pair<uint64_t, bool>> SceneSessionManager::GetWindowDrawingContentChangeInfo(
    std::vector<std::pair<uint64_t, bool>> currDrawingContentData)
{
    std::vector<std::pair<uint64_t, bool>> processDrawingContentChangeInfo;
    for (const auto& data : currDrawingContentData) {
        uint64_t windowId = data.first;
        bool currentDrawingContentState = data.second;
        int32_t pid = 0;
        bool isChange = false;
        if (GetPreWindowDrawingState(windowId, pid, currentDrawingContentState) == currentDrawingContentState) {
            continue;
        } else {
            isChange = GetProcessDrawingState(windowId, pid, currentDrawingContentState);
        }

        if (isChange) {
            processDrawingContentChangeInfo.emplace_back(windowId, currentDrawingContentState);
        }
    }
    return processDrawingContentChangeInfo;
}

bool SceneSessionManager::GetPreWindowDrawingState(uint64_t windowId, int32_t& pid, bool currentDrawingContentState)
{
    bool preWindowDrawingState = true;
    sptr<SceneSession> session = SelectSesssionFromMap(windowId);
    if (session == nullptr) {
        return false;
    }
    pid = session->GetCallingPid();
    preWindowDrawingState = session->GetDrawingContentState();
    session->SetDrawingContentState(currentDrawingContentState);
    return preWindowDrawingState;
}

bool SceneSessionManager::GetProcessDrawingState(uint64_t windowId, int32_t pid, bool currentDrawingContentState)
{
    bool isChange = true;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession == nullptr) {
            continue;
        }
        if (sceneSession->GetCallingPid() == pid && sceneSession->GetSurfaceNode() != nullptr &&
            windowId != sceneSession->GetSurfaceNode()->GetId()) {
                if (sceneSession->GetDrawingContentState()) {
                    return false;
                }
            }
        }
    return isChange;
}


void SceneSessionManager::InitWithRenderServiceAdded()
{
    auto windowVisibilityChangeCb = [this](std::shared_ptr<RSOcclusionData> occlusiontionData) {
        this->WindowLayerInfoChangeCallback(occlusiontionData);
    };
    WLOGI("RegisterWindowVisibilityChangeCallback");
    if (rsInterface_.RegisterOcclusionChangeCallback(windowVisibilityChangeCb) != WM_OK) {
        WLOGFE("RegisterWindowVisibilityChangeCallback failed");
    }
}

WMError SceneSessionManager::SetSystemAnimatedScenes(SystemAnimatedSceneType sceneType)
{
    if (sceneType > SystemAnimatedSceneType::SCENE_OTHERS) {
        WLOGFE("The input scene type is valid, scene type is %{public}d", sceneType);
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    auto task = [this, sceneType]() {
        WLOGFD("Set system animated scene %{public}d.", sceneType);
        bool ret = rsInterface_.SetSystemAnimatedScenes(static_cast<SystemAnimatedScenes>(sceneType));
        if (!ret) {
            WLOGFE("Set system animated scene failed.");
        }
    };
    taskScheduler_->PostAsyncTask(task, "SetSystemAnimatedScenes");
    return WMError::WM_OK;
}

WSError SceneSessionManager::NotifyWindowExtensionVisibilityChange(int32_t pid, int32_t uid, bool visible)
{
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (pid != IPCSkeleton::GetCallingRealPid() || uid != IPCSkeleton::GetCallingUid()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "pid and uid check failed!");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    TLOGI(WmsLogTag::WMS_UIEXT, "visibility change to %{public}s for pid: %{public}d, uid: %{public}d",
        visible ? "VISIBLE" : "INVISIBLE", pid, uid);
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
    windowVisibilityInfos.emplace_back(new WindowVisibilityInfo(INVALID_WINDOW_ID, pid, uid,
        visible ? WINDOW_VISIBILITY_STATE_NO_OCCLUSION : WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION,
        WindowType::WINDOW_TYPE_APP_COMPONENT));
    SessionManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
    return WSError::WS_OK;
}

void SceneSessionManager::WindowDestroyNotifyVisibility(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr!");
        return;
    }
    if (sceneSession->GetRSVisible()) {
        std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
#ifdef MEMMGR_WINDOW_ENABLE
        std::vector<sptr<Memory::MemMgrWindowInfo>> memMgrWindowInfos;
#endif
        sceneSession->SetRSVisible(false);
        sceneSession->SetVisibilityState(WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION);
        sceneSession->ClearExtWindowFlags();
        windowVisibilityInfos.emplace_back(new WindowVisibilityInfo(sceneSession->GetWindowId(),
            sceneSession->GetCallingPid(), sceneSession->GetCallingUid(),
            WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION, sceneSession->GetWindowType()));
#ifdef MEMMGR_WINDOW_ENABLE
        memMgrWindowInfos.emplace_back(new Memory::MemMgrWindowInfo(sceneSession->GetWindowId(),
        sceneSession->GetCallingPid(), sceneSession->GetCallingUid(), false));
#endif
        WLOGFD("covered status changed window:%{public}u, isVisible:%{public}d",
            sceneSession->GetWindowId(), sceneSession->GetRSVisible());
        CheckAndNotifyWaterMarkChangedResult();
        SessionManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
#ifdef MEMMGR_WINDOW_ENABLE
        WLOGD("Notify memMgrWindowInfos changed start");
        Memory::MemMgrClient::GetInstance().OnWindowVisibilityChanged(memMgrWindowInfos);
#endif
    }
}

sptr<SceneSession> SceneSessionManager::FindSessionByToken(const sptr<IRemoteObject>& token)
{
    if (token == nullptr) {
        TLOGW(WmsLogTag::DEFAULT, "token is nullptr");
        return nullptr;
    }
    sptr<SceneSession> session = nullptr;
    auto cmpFunc = [token](const std::map<uint64_t, sptr<SceneSession>>::value_type& pair) {
        if (pair.second == nullptr) {
            return false;
        }
        return pair.second->GetAbilityToken() == token || pair.second->AsObject() == token;
    };
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    auto iter = std::find_if(sceneSessionMap_.begin(), sceneSessionMap_.end(), cmpFunc);
    if (iter != sceneSessionMap_.end()) {
        session = iter->second;
    }
    return session;
}

sptr<SceneSession> SceneSessionManager::FindSessionByAffinity(std::string affinity)
{
    if (affinity.size() == 0) {
        WLOGFI("AbilityInfo affinity is empty");
        return nullptr;
    }
    sptr<SceneSession> session = nullptr;
    auto cmpFunc = [this, affinity](const std::map<uint64_t, sptr<SceneSession>>::value_type& pair) {
        if (pair.second == nullptr || !CheckCollaboratorType(pair.second->GetCollaboratorType())) {
            return false;
        }
        return pair.second->GetSessionInfo().sessionAffinity == affinity;
    };
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    auto iter = std::find_if(sceneSessionMap_.begin(), sceneSessionMap_.end(), cmpFunc);
    if (iter != sceneSessionMap_.end()) {
        session = iter->second;
    }
    return session;
}

void SceneSessionManager::PreloadInLakeApp(const std::string& bundleName)
{
    WLOGFD("Enter name %{public}s", bundleName.c_str());
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator = GetCollaboratorByType(CollaboratorType::RESERVE_TYPE);
    if (collaborator != nullptr) {
        WLOGFI("NotifyPreloadAbility: %{public}s", bundleName.c_str());
        collaborator->NotifyPreloadAbility(bundleName);
    }
}

WSError SceneSessionManager::PendingSessionToForeground(const sptr<IRemoteObject>& token)
{
    TLOGI(WmsLogTag::DEFAULT, "Enter");
    auto pid = IPCSkeleton::GetCallingRealPid();
    if (!SessionPermission::IsSACalling() && !SessionPermission::CheckCallingIsUserTestMode(pid)) {
        TLOGE(WmsLogTag::DEFAULT, "Permission denied for going to foreground!");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }

    auto task = [this, &token]() {
        auto session = FindSessionByToken(token);
        if (session != nullptr) {
            return session->PendingSessionToForeground();
        }
        TLOGE(WmsLogTag::DEFAULT, "PendingForeground: fail to find token");
        return WSError::WS_ERROR_INVALID_PARAM;
    };
    return taskScheduler_->PostSyncTask(task, "PendingSessionToForeground");
}

WSError SceneSessionManager::PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject>& token)
{
    auto task = [this, &token]() {
        auto session = FindSessionByToken(token);
        if (session != nullptr) {
            return session->PendingSessionToBackgroundForDelegator();
        }
        TLOGE(WmsLogTag::WMS_LIFE, "PendingBackgroundForDelegator: fail to find token");
        return WSError::WS_ERROR_INVALID_PARAM;
    };
    return taskScheduler_->PostSyncTask(task, "PendingSessionToBackgroundForDelegator");
}

void SceneSessionManager::ClearDisplayStatusBarTemporarilyFlags()
{
    for (auto persistentId : avoidAreaListenerSessionSet_) {
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            continue;
        }
        sceneSession->SetIsDisplayStatusBarTemporarily(false);
    }
}

WSError SceneSessionManager::GetFocusSessionToken(sptr<IRemoteObject>& token)
{
    if (!SessionPermission::IsSACalling()) {
        WLOGFE("GetFocusSessionToken permission denied!");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &token]() {
        WLOGFD("GetFocusSessionToken with focusedSessionId: %{public}d", focusedSessionId_);
        auto sceneSession = GetSceneSession(focusedSessionId_);
        if (sceneSession) {
            token = sceneSession->GetAbilityToken();
            if (token == nullptr) {
                WLOGFE("token is nullptr");
                return WSError::WS_ERROR_INVALID_PARAM;
            }
            return WSError::WS_OK;
        }
        return WSError::WS_ERROR_INVALID_SESSION;
    };
    return taskScheduler_->PostSyncTask(task, "GetFocusSessionToken");
}

WSError SceneSessionManager::GetFocusSessionElement(AppExecFwk::ElementName& element)
{
    AppExecFwk::RunningProcessInfo info;
    auto pid = IPCSkeleton::GetCallingRealPid();
    DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance()->GetRunningProcessInfoByPid(pid, info);
    if (!info.isTestProcess && !SessionPermission::IsSystemCalling()) {
        WLOGFE("SystemCalling permission denied!");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &element]() {
        WLOGFD("GetFocusSessionElement with focusedSessionId: %{public}d", focusedSessionId_);
        auto sceneSession = GetSceneSession(focusedSessionId_);
        if (sceneSession) {
            auto sessionInfo = sceneSession->GetSessionInfo();
            element = AppExecFwk::ElementName("", sessionInfo.bundleName_,
                sessionInfo.abilityName_, sessionInfo.moduleName_);
            return WSError::WS_OK;
        }
        return WSError::WS_ERROR_INVALID_SESSION;
    };
    return taskScheduler_->PostSyncTask(task, "GetFocusSessionElement");
}

WSError SceneSessionManager::UpdateSessionAvoidAreaListener(int32_t& persistentId, bool haveListener)
{
    const auto callingPid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, persistentId, haveListener, callingPid]() {
        TLOGI(WmsLogTag::WMS_IMMS,
            "UpdateSessionAvoidAreaListener persistentId: %{public}d haveListener:%{public}d",
            persistentId, haveListener);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGD(WmsLogTag::WMS_IMMS, "sceneSession is nullptr.");
            return WSError::WS_DO_NOTHING;
        }
        if (callingPid != sceneSession->GetCallingPid()) {
            TLOGE(WmsLogTag::WMS_IMMS, "Permission denied, not called by the same process");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        if (haveListener) {
            avoidAreaListenerSessionSet_.insert(persistentId);
            UpdateAvoidArea(persistentId);
        } else {
            lastUpdatedAvoidArea_.erase(persistentId);
            avoidAreaListenerSessionSet_.erase(persistentId);
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "UpdateSessionAvoidAreaListener:PID:" + std::to_string(persistentId));
}

bool SceneSessionManager::UpdateSessionAvoidAreaIfNeed(const int32_t& persistentId,
    const sptr<SceneSession>& sceneSession, const AvoidArea& avoidArea, AvoidAreaType avoidAreaType)
{
    if (sceneSession == nullptr) {
        TLOGI(WmsLogTag::WMS_IMMS, "scene session null no need update avoid area");
        return false;
    }
    if (lastUpdatedAvoidArea_.find(persistentId) == lastUpdatedAvoidArea_.end()) {
        lastUpdatedAvoidArea_[persistentId] = {};
    }

    bool needUpdate = true;
    if (auto iter = lastUpdatedAvoidArea_[persistentId].find(avoidAreaType);
        iter != lastUpdatedAvoidArea_[persistentId].end()) {
        needUpdate = iter->second != avoidArea;
    } else {
        if (avoidArea.isEmptyAvoidArea()) {
            TLOGI(WmsLogTag::WMS_IMMS, "window %{public}d type %{public}d empty avoid area",
                persistentId, avoidAreaType);
            needUpdate = false;
            return needUpdate;
        }
    }
    if (needUpdate) {
        lastUpdatedAvoidArea_[persistentId][avoidAreaType] = avoidArea;
        sceneSession->UpdateAvoidArea(new AvoidArea(avoidArea), avoidAreaType);
    }

    return needUpdate;
}

void SceneSessionManager::UpdateAvoidSessionAvoidArea(WindowType type, bool& needUpdate)
{
    bool ret = true;
    AvoidAreaType avoidType = (type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) ?
        AvoidAreaType::TYPE_KEYBOARD : AvoidAreaType::TYPE_SYSTEM;
    for (auto& persistentId : avoidAreaListenerSessionSet_) {
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr || !IsSessionVisibleForeground(sceneSession)) {
            continue;
        }
        AvoidArea avoidArea = sceneSession->GetAvoidAreaByType(static_cast<AvoidAreaType>(avoidType));
        ret = UpdateSessionAvoidAreaIfNeed(
            persistentId, sceneSession, avoidArea, static_cast<AvoidAreaType>(avoidType));
        needUpdate = needUpdate || ret;
    }

    return;
}

static bool CheckAvoidAreaForAINavigationBar(bool isVisible, const AvoidArea& avoidArea, int32_t sessionBottom)
{
    if (!avoidArea.topRect_.IsUninitializedRect() || !avoidArea.leftRect_.IsUninitializedRect() ||
        !avoidArea.rightRect_.IsUninitializedRect()) {
        return false;
    }
    if (avoidArea.bottomRect_.IsUninitializedRect()) {
        return true;
    }
    if (isVisible &&
        (avoidArea.bottomRect_.posY_ + static_cast<int32_t>(avoidArea.bottomRect_.height_) == sessionBottom)) {
        return true;
    }
    return false;
}

void SceneSessionManager::UpdateNormalSessionAvoidArea(
    const int32_t& persistentId, sptr<SceneSession>& sceneSession, bool& needUpdate)
{
    bool ret = true;
    if (sceneSession == nullptr || !IsSessionVisibleForeground(sceneSession)) {
        needUpdate = false;
        return;
    }
    if (avoidAreaListenerSessionSet_.find(persistentId) == avoidAreaListenerSessionSet_.end()) {
        TLOGD(WmsLogTag::WMS_IMMS,
            "id:%{public}d is not in avoidAreaListenerNodes, don't update avoid area.", persistentId);
        needUpdate = false;
        return;
    }
    uint32_t start = static_cast<uint32_t>(AvoidAreaType::TYPE_SYSTEM);
    uint32_t end = static_cast<uint32_t>(AvoidAreaType::TYPE_NAVIGATION_INDICATOR);
    for (uint32_t avoidType = start; avoidType <= end; avoidType++) {
        AvoidArea avoidArea = sceneSession->GetAvoidAreaByType(static_cast<AvoidAreaType>(avoidType));
        if (avoidType == static_cast<uint32_t>(AvoidAreaType::TYPE_NAVIGATION_INDICATOR) &&
            !CheckAvoidAreaForAINavigationBar(isAINavigationBarVisible_, avoidArea,
                sceneSession->GetSessionRect().height_)) {
            continue;
        }
        ret = UpdateSessionAvoidAreaIfNeed(
            persistentId, sceneSession, avoidArea, static_cast<AvoidAreaType>(avoidType));
        needUpdate = needUpdate || ret;
    }

    return;
}

void SceneSessionManager::NotifyMMIWindowPidChange(int32_t windowId, bool startMoving)
{
    int32_t pid = startMoving ? static_cast<int32_t>(getpid()) : -1;
    auto sceneSession = GetSceneSession(windowId);
    if (sceneSession == nullptr) {
        WLOGFW("window not exist: %{public}d", windowId);
        return;
    }

    wptr<SceneSession> weakSceneSession(sceneSession);
    WLOGFI("SceneSessionManager NotifyMMIWindowPidChange to notify window: %{public}d, pid: %{public}d", windowId, pid);
    auto task = [weakSceneSession, startMoving]() -> WSError {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFW("session is null");
            return WSError::WS_ERROR_NULLPTR;
        }
        SceneInputManager::GetInstance().NotifyMMIWindowPidChange(scnSession, startMoving);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostAsyncTask(task);
}

void SceneSessionManager::UpdateAvoidArea(const int32_t persistentId)
{
    auto task = [this, persistentId]() {
        bool needUpdate = false;
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGD(WmsLogTag::WMS_IMMS, "sceneSession is nullptr.");
            return;
        }
        WindowType type = sceneSession->GetWindowType();
        if (sceneSession->IsImmersiveType()) {
            UpdateAvoidSessionAvoidArea(type, needUpdate);
        } else {
            UpdateNormalSessionAvoidArea(persistentId, sceneSession, needUpdate);
        }
        if (needUpdate) {
            NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_BOUNDS);
        }
        return;
    };
    taskScheduler_->PostAsyncTask(task, "UpdateAvoidArea:PID:" + std::to_string(persistentId));
    return;
}

void SceneSessionManager::UpdateOccupiedAreaIfNeed(const int32_t& persistentId)
{
    auto task = [this, persistentId]() {
        sptr<SceneSession> keyboardSession = nullptr;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto item = sceneSessionMap_.rbegin(); item != sceneSessionMap_.rend(); ++item) {
            auto sceneSession = item->second;
            if (sceneSession != nullptr &&
                sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
                keyboardSession = sceneSession;
                break;
            }
        }
        if (keyboardSession == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "keyboardSession is nullptr.");
            return;
        }
        if (keyboardSession->GetSessionProperty() == nullptr ||
            keyboardSession->GetSessionProperty()->GetCallingSessionId() != static_cast<uint32_t>(persistentId)) {
            return;
        }

        keyboardSession->OnCallingSessionUpdated();
        return;
    };
    taskScheduler_->PostAsyncTask(task, "UpdateOccupiedAreaIfNeed:PID:" + std::to_string(persistentId));
    return;
}

WSError SceneSessionManager::NotifyStatusBarShowStatus(int32_t persistentId, bool isVisible)
{
    TLOGD(WmsLogTag::WMS_IMMS, "isVisible %{public}u, persistentId %{public}u",
        isVisible, persistentId);
    auto task = [this, persistentId, isVisible] {
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_IMMS, "scene session is nullptr");
            return;
        }
        sceneSession->SetIsStatusBarVisible(isVisible);
    };
    taskScheduler_->PostTask(task, "NotifyStatusBarShowStatus");
    return WSError::WS_OK;
}

WSError SceneSessionManager::NotifyAINavigationBarShowStatus(bool isVisible, WSRect barArea, uint64_t displayId)
{
    WLOGFI("isVisible: %{public}u, "
        "area{%{public}d,%{public}d,%{public}d,%{public}d}, displayId: %{public}" PRIu64,
        isVisible, barArea.posX_, barArea.posY_, barArea.width_, barArea.height_, displayId);
    auto task = [this, isVisible, barArea, displayId]() {
        bool isNeedNotify = isAINavigationBarVisible_ != isVisible;
        {
            std::unique_lock<std::shared_mutex> lock(currAINavigationBarAreaMapMutex_);
            bool isNeedUpdate = isAINavigationBarVisible_ != isVisible ||
                currAINavigationBarAreaMap_.count(displayId) == 0 ||
                currAINavigationBarAreaMap_[displayId] != barArea;
            if (isNeedUpdate) {
                isAINavigationBarVisible_ = isVisible;
                currAINavigationBarAreaMap_.clear();
                currAINavigationBarAreaMap_[displayId] = barArea;
            }
            if (isNeedUpdate && !isVisible && !barArea.IsEmpty()) {
                WLOGFD("NotifyAINavigationBar: barArea should be empty if invisible");
                currAINavigationBarAreaMap_[displayId] = WSRect();
            }
        }
        if (isNeedNotify) {
            WLOGFI("NotifyAINavigationBar: enter: %{public}u, {%{public}d,%{public}d,%{public}d,%{public}d}",
                isVisible, barArea.posX_, barArea.posY_, barArea.width_, barArea.height_);
            for (auto persistentId : avoidAreaListenerSessionSet_) {
                NotifySessionAINavigationBarChange(persistentId);
            }
        }
    };
    taskScheduler_->PostAsyncTask(task, "NotifyAINavigationBarShowStatus");
    return WSError::WS_OK;
}

void SceneSessionManager::NotifySessionAINavigationBarChange(int32_t persistentId)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr || !IsSessionVisibleForeground(sceneSession)) {
        return;
    }
    AvoidArea avoidArea = sceneSession->GetAvoidAreaByType(AvoidAreaType::TYPE_NAVIGATION_INDICATOR);
    if (!CheckAvoidAreaForAINavigationBar(isAINavigationBarVisible_, avoidArea,
        sceneSession->GetSessionRect().height_)) {
        return;
    }
    WLOGFI("NotifyAINavigationBarShowStatus: persistentId: %{public}d, "
        "{%{public}d,%{public}d,%{public}d,%{public}d}", persistentId,
        avoidArea.bottomRect_.posX_, avoidArea.bottomRect_.posY_,
        avoidArea.bottomRect_.width_, avoidArea.bottomRect_.height_);
    UpdateSessionAvoidAreaIfNeed(persistentId, sceneSession, avoidArea,
        AvoidAreaType::TYPE_NAVIGATION_INDICATOR);
}

WSRect SceneSessionManager::GetAINavigationBarArea(uint64_t displayId)
{
    std::shared_lock<std::shared_mutex> lock(currAINavigationBarAreaMapMutex_);
    if (currAINavigationBarAreaMap_.count(displayId) == 0) {
        return {};
    }
    return currAINavigationBarAreaMap_[displayId];
}

WSError SceneSessionManager::UpdateSessionTouchOutsideListener(int32_t& persistentId, bool haveListener)
{
    const auto callingPid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, persistentId, haveListener, callingPid]() {
        TLOGI(WmsLogTag::WMS_EVENT, "persistentId:%{public}d haveListener:%{public}d",
            persistentId, haveListener);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_EVENT, "sceneSession is nullptr.");
            return WSError::WS_DO_NOTHING;
        }
        if (callingPid != sceneSession->GetCallingPid()) {
            TLOGE(WmsLogTag::WMS_EVENT, "Permission denied");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        if (haveListener) {
            touchOutsideListenerSessionSet_.insert(persistentId);
        } else {
            touchOutsideListenerSessionSet_.erase(persistentId);
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "UpdateSessionTouchOutsideListener" + std::to_string(persistentId));
}

WSError SceneSessionManager::UpdateSessionWindowVisibilityListener(int32_t persistentId, bool haveListener)
{
    const auto& callingPid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, persistentId, haveListener, callingPid]() -> WSError {
        WLOGFI("UpdateSessionWindowVisibilityListener persistentId: %{public}d haveListener:%{public}d",
            persistentId, haveListener);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFD("sceneSession is nullptr.");
            return WSError::WS_DO_NOTHING;
        }
        if (callingPid != sceneSession->GetCallingPid()) {
            TLOGE(WmsLogTag::WMS_LIFE, "Permission denied, neither register nor unreigster allowed by other process");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        if (haveListener) {
            windowVisibilityListenerSessionSet_.insert(persistentId);
            sceneSession->NotifyWindowVisibility();
        } else {
            windowVisibilityListenerSessionSet_.erase(persistentId);
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "UpdateSessionWindowVisibilityListener");
}

void SceneSessionManager::SetVirtualPixelRatioChangeListener(const ProcessVirtualPixelRatioChangeFunc& func)
{
    processVirtualPixelRatioChangeFunc_ = func;
    WLOGFI("SetVirtualPixelRatioChangeListener");
}

void SceneSessionManager::ProcessVirtualPixelRatioChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    if (displayInfo == nullptr) {
        WLOGFE("SceneSessionManager::ProcessVirtualPixelRatioChange displayInfo is nullptr.");
        return;
    }
    auto task = [this, displayInfo]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        if (processVirtualPixelRatioChangeFunc_ != nullptr &&
            displayInfo->GetVirtualPixelRatio() == displayInfo->GetDensityInCurResolution()) {
            Rect rect = { displayInfo->GetOffsetX(), displayInfo->GetOffsetY(),
                displayInfo->GetWidth(), displayInfo->GetHeight()
            };
            processVirtualPixelRatioChangeFunc_(displayInfo->GetVirtualPixelRatio(), rect);
        }
        for (const auto &item : sceneSessionMap_) {
            auto scnSession = item.second;
            if (scnSession == nullptr) {
                WLOGFE("SceneSessionManager::ProcessVirtualPixelRatioChange null scene session");
                continue;
            }
            SessionInfo sessionInfo = scnSession->GetSessionInfo();
            if (sessionInfo.isSystem_) {
                continue;
            }
            if (scnSession->GetSessionState() == SessionState::STATE_FOREGROUND ||
                scnSession->GetSessionState() == SessionState::STATE_ACTIVE) {
                scnSession->UpdateDensity();
                WLOGFD("UpdateDensity name=%{public}s, persistentId=%{public}d, winType=%{public}d, "
                    "state=%{public}d, visible-%{public}d", scnSession->GetWindowName().c_str(), item.first,
                    scnSession->GetWindowType(), scnSession->GetSessionState(), scnSession->IsVisible());
            }
        }
        UpdateDisplayRegion(displayInfo);
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "ProcessVirtualPixelRatioChange:DID:" + std::to_string(defaultDisplayId));
}

void SceneSessionManager::ProcessUpdateRotationChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    if (displayInfo == nullptr) {
        WLOGFE("SceneSessionManager::ProcessUpdateRotationChange displayInfo is nullptr.");
        return;
    }
    auto task = [this, displayInfo]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto &item : sceneSessionMap_) {
            auto scnSession = item.second;
            if (scnSession == nullptr) {
                WLOGFE("SceneSessionManager::ProcessUpdateRotationChange null scene session");
                continue;
            }
            if (scnSession->GetSessionState() != SessionState::STATE_FOREGROUND &&
                scnSession->GetSessionState() != SessionState::STATE_ACTIVE) {
                continue;
            }
            if (NearEqual(scnSession->GetBounds().width_, static_cast<float>(displayInfo->GetWidth())) &&
                NearEqual(scnSession->GetBounds().height_, static_cast<float>(displayInfo->GetHeight())) &&
                scnSession->GetRotation() != displayInfo->GetRotation()) {
                scnSession->UpdateRotationAvoidArea();
                TLOGD(WmsLogTag::DMS, "UpdateRotationAvoidArea name=%{public}s, persistentId=%{public}d, "
                    "winType=%{public}d, state=%{public}d, visible-%{public}d", scnSession->GetWindowName().c_str(),
                    item.first, scnSession->GetWindowType(), scnSession->GetSessionState(), scnSession->IsVisible());
            }
            scnSession->SetRotation(displayInfo->GetRotation());
            scnSession->UpdateOrientation();
        }
        UpdateDisplayRegion(displayInfo);
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "ProcessUpdateRotationChange" + std::to_string(defaultDisplayId));
}

void SceneSessionManager::ProcessDisplayScale(sptr<DisplayInfo>& displayInfo)
{
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::DMS, "displayInfo is nullptr");
        return;
    }

    auto task = [displayInfo]() -> WSError {
        ScreenSessionManagerClient::GetInstance().UpdateDisplayScale(displayInfo->GetScreenId(),
            displayInfo->GetScaleX(),
            displayInfo->GetScaleY(),
            displayInfo->GetPivotX(),
            displayInfo->GetPivotY(),
            displayInfo->GetTranslateX(),
            displayInfo->GetTranslateY());
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSessionManager::FlushWindowInfoToMMI");
        SceneInputManager::GetInstance().FlushDisplayInfoToMMI(true);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostAsyncTask(task);
}

void DisplayChangeListener::OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    WLOGFD("type: %{public}u", type);
    switch (type) {
        case DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE: {
            SceneSessionManager::GetInstance().ProcessVirtualPixelRatioChange(defaultDisplayId,
                displayInfo, displayInfoMap, type);
            break;
        }
        case DisplayStateChangeType::UPDATE_ROTATION: {
            SceneSessionManager::GetInstance().ProcessUpdateRotationChange(defaultDisplayId,
                displayInfo, displayInfoMap, type);
            break;
        }
        case DisplayStateChangeType::UPDATE_SCALE: {
            SceneSessionManager::GetInstance().ProcessDisplayScale(displayInfo);
            break;
        }
        default:
            return;
    }
}

void DisplayChangeListener::OnScreenshot(DisplayId displayId)
{
    SceneSessionManager::GetInstance().OnScreenshot(displayId);
}

void SceneSessionManager::OnScreenshot(DisplayId displayId)
{
    auto task = [this, displayId]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& iter : sceneSessionMap_) {
            auto sceneSession = iter.second;
            if (sceneSession == nullptr) {
                continue;
            }
            auto state = sceneSession->GetSessionState();
            if (state == SessionState::STATE_FOREGROUND || state == SessionState::STATE_ACTIVE) {
                sceneSession->NotifyScreenshot();
            }
        }
    };
    taskScheduler_->PostAsyncTask(task, "OnScreenshot:PID:" + std::to_string(displayId));
}

WSError SceneSessionManager::ClearSession(int32_t persistentId)
{
    WLOGFI("id: %{public}d", persistentId);
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, persistentId]() {
        sptr<SceneSession> sceneSession = GetSceneSession(persistentId);
        return ClearSession(sceneSession);
    };
    taskScheduler_->PostAsyncTask(task, "ClearSession:PID:" + std::to_string(persistentId));
    return WSError::WS_OK;
}

WSError SceneSessionManager::ClearSession(sptr<SceneSession> sceneSession)
{
    WLOGFD("Enter");
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (!IsSessionClearable(sceneSession)) {
        WLOGFI("session cannot be clear, Id %{public}d.", sceneSession->GetPersistentId());
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    const WSError errCode = sceneSession->Clear();
    return errCode;
}

WSError SceneSessionManager::ClearAllSessions()
{
    WLOGFI("Enter");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this]() {
        std::vector<sptr<SceneSession>> sessionVector;
        GetAllClearableSessions(sessionVector);
        for (uint32_t i = 0; i < sessionVector.size(); i++) {
            ClearSession(sessionVector[i]);
        }
        return WSError::WS_OK;
    };
    taskScheduler_->PostAsyncTask(task, "ClearAllSessions");
    return WSError::WS_OK;
}

void SceneSessionManager::GetAllClearableSessions(std::vector<sptr<SceneSession>>& sessionVector)
{
    WLOGFI("Enter");
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto scnSession = item.second;
        if (IsSessionClearable(scnSession)) {
            sessionVector.push_back(scnSession);
        }
    }
}

WSError SceneSessionManager::LockSession(int32_t sessionId)
{
    WLOGFI("id: %{public}d", sessionId);
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, sessionId]() {
        auto sceneSession = GetSceneSession(sessionId);
        if (sceneSession == nullptr) {
            WLOGFE("LockSession cannot find session, id: %{public}d", sessionId);
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        sceneSession->SetSessionInfoLockedState(true);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "LockSession:SID:" + std::to_string(sessionId));
}

WSError SceneSessionManager::UnlockSession(int32_t sessionId)
{
    WLOGFI("id: %{public}d", sessionId);
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, sessionId]() {
        auto sceneSession = GetSceneSession(sessionId);
        if (sceneSession == nullptr) {
            WLOGFE("UnlockSession cannot find session, id: %{public}d", sessionId);
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        sceneSession->SetSessionInfoLockedState(false);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "UnlockSession" + std::to_string(sessionId));
}

WSError SceneSessionManager::MoveSessionsToForeground(const std::vector<int32_t>& sessionIds, int32_t topSessionId)
{
    TLOGI(WmsLogTag::WMS_LIFE, "Enter");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }

    return WSError::WS_OK;
}

WSError SceneSessionManager::MoveSessionsToBackground(const std::vector<int32_t>& sessionIds,
    std::vector<int32_t>& result)
{
    TLOGI(WmsLogTag::WMS_LIFE, "Enter");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }

    result.insert(result.end(), sessionIds.begin(), sessionIds.end());
    return WSError::WS_OK;
}

bool SceneSessionManager::IsSessionClearable(sptr<SceneSession> scnSession)
{
    if (scnSession == nullptr) {
        WLOGFI("scnSession is nullptr");
        return false;
    }
    SessionInfo sessionInfo = scnSession->GetSessionInfo();
    if (sessionInfo.abilityInfo == nullptr) {
        WLOGFI("scnSession abilityInfo is nullptr");
        return false;
    }
    if (sessionInfo.abilityInfo->excludeFromMissions) {
        WLOGFI("persistentId %{public}d is excludeFromMissions", scnSession->GetPersistentId());
        return false;
    }
    if (sessionInfo.abilityInfo->unclearableMission) {
        WLOGFI("persistentId %{public}d is unclearable", scnSession->GetPersistentId());
        return false;
    }
    if (sessionInfo.isSystem_) {
        WLOGFI("persistentId %{public}d is system app", scnSession->GetPersistentId());
        return false;
    }
    if (sessionInfo.lockedState) {
        WLOGFI("persistentId %{public}d is in lockedState", scnSession->GetPersistentId());
        return false;
    }

    return true;
}

WSError SceneSessionManager::RegisterIAbilityManagerCollaborator(int32_t type,
    const sptr<AAFwk::IAbilityManagerCollaborator>& impl)
{
    WLOGFI("type: %{public}d", type);
    auto isSaCall = SessionPermission::IsSACalling();
    if (!isSaCall || !SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        TLOGE(WmsLogTag::DEFAULT, "The caller has not permission granted or not SACalling");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    if (!CheckCollaboratorType(type)) {
        WLOGFW("collaborator register failed, invalid type.");
        return WSError::WS_ERROR_INVALID_TYPE;
    }
    {
        std::unique_lock<std::shared_mutex> lock(collaboratorMapLock_);
        collaboratorMap_[type] = impl;
    }
    auto task = [this] {
        if (abilityManagerCollaboratorRegisteredFunc_) {
            abilityManagerCollaboratorRegisteredFunc_();
        }
    };
    taskScheduler_->PostTask(task, __func__);
    return WSError::WS_OK;
}

WSError SceneSessionManager::UnregisterIAbilityManagerCollaborator(int32_t type)
{
    WLOGFI("type: %{public}d", type);
    auto isSaCall = SessionPermission::IsSACalling();
    if (!isSaCall || !SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        TLOGE(WmsLogTag::DEFAULT, "The caller has not permission granted or not SACalling");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    if (!CheckCollaboratorType(type)) {
        WLOGFE("collaborator unregister failed, invalid type.");
        return WSError::WS_ERROR_INVALID_TYPE;
    }
    {
        std::unique_lock<std::shared_mutex> lock(collaboratorMapLock_);
        collaboratorMap_.erase(type);
    }
    return WSError::WS_OK;
}

bool SceneSessionManager::CheckCollaboratorType(int32_t type)
{
    if (type != CollaboratorType::RESERVE_TYPE && type != CollaboratorType::OTHERS_TYPE) {
        WLOGFD("type is invalid");
        return false;
    }
    return true;
}

BrokerStates SceneSessionManager::CheckIfReuseSession(SessionInfo& sessionInfo)
{
    auto abilityInfo = QueryAbilityInfoFromBMS(currentUserId_, sessionInfo.bundleName_, sessionInfo.abilityName_,
        sessionInfo.moduleName_);
    if (abilityInfo == nullptr) {
        WLOGFE("abilityInfo is nullptr!");
        return BrokerStates::BROKER_UNKOWN;
    }
    sessionInfo.abilityInfo = abilityInfo;
    int32_t collaboratorType = CollaboratorType::DEFAULT_TYPE;
    if (abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::RESERVE_TYPE)) {
        collaboratorType = CollaboratorType::RESERVE_TYPE;
    } else if (abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::OTHERS_TYPE)) {
        collaboratorType = CollaboratorType::OTHERS_TYPE;
    }
    if (!CheckCollaboratorType(collaboratorType)) {
        WLOGFW("checked not collaborator!");
        return BrokerStates::BROKER_UNKOWN;
    }
    BrokerStates resultValue = NotifyStartAbility(collaboratorType, sessionInfo);
    sessionInfo.collaboratorType_ = collaboratorType;
    sessionInfo.sessionAffinity = sessionInfo.want->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY);
    if (FindSessionByAffinity(sessionInfo.sessionAffinity) != nullptr) {
        WLOGFI("FindSessionByAffinity: %{public}s, try to reuse", sessionInfo.sessionAffinity.c_str());
        sessionInfo.reuse = true;
    } else {
        sessionInfo.reuse = false;
    }
    WLOGFI("end: affinity %{public}s type %{public}d reuse %{public}d",
        sessionInfo.sessionAffinity.c_str(), collaboratorType, sessionInfo.reuse);
    return resultValue;
}

BrokerStates SceneSessionManager::NotifyStartAbility(
    int32_t collaboratorType, const SessionInfo& sessionInfo, int32_t persistentId)
{
    WLOGFI("type %{public}d id %{public}d", collaboratorType, persistentId);
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator = GetCollaboratorByType(collaboratorType);
    if (collaborator == nullptr) {
        return BrokerStates::BROKER_UNKOWN;
    }
    if (sessionInfo.want == nullptr) {
        WLOGFI("sessionInfo.want is nullptr, init");
        sessionInfo.want = std::make_shared<AAFwk::Want>();
        sessionInfo.want->SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_,
            sessionInfo.moduleName_);
    }
    auto accessTokenIDEx = sessionInfo.callingTokenId_;
    if (collaborator != nullptr) {
        containerStartAbilityTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        std::string affinity = sessionInfo.want->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY);
        if (!affinity.empty() && FindSessionByAffinity(affinity) != nullptr) {
            WLOGFI("want affinity exit %{public}s.", affinity.c_str());
            return BrokerStates::BROKER_UNKOWN;
        }
        sessionInfo.want->SetParam("oh_persistentId", persistentId);
        int32_t ret = collaborator->NotifyStartAbility(*(sessionInfo.abilityInfo),
            currentUserId_, *(sessionInfo.want), static_cast<uint64_t>(accessTokenIDEx));
        WLOGFI("collaborator ret: %{public}d", ret);
        if (ret == 0) {
            return BrokerStates::BROKER_STARTED;
        } else {
            return BrokerStates::BROKER_NOT_START;
        }
    }
    return BrokerStates::BROKER_UNKOWN;
}

void SceneSessionManager::NotifySessionCreate(sptr<SceneSession> sceneSession, const SessionInfo& sessionInfo)
{
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }
    if (sessionInfo.want == nullptr) {
        WLOGFI("sessionInfo.want is nullptr");
        return;
    }
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator = GetCollaboratorByType(sceneSession->GetCollaboratorType());
    if (collaborator == nullptr) {
        return;
    }
    auto abilitySessionInfo = SetAbilitySessionInfo(sceneSession);
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is nullptr");
        return;
    }
    abilitySessionInfo->want = *(sessionInfo.want);
    if (collaborator != nullptr) {
        int32_t missionId = abilitySessionInfo->persistentId;
        std::string bundleName = sessionInfo.bundleName_;
        int64_t timestamp = containerStartAbilityTime;
        WindowInfoReporter::GetInstance().ReportContainerStartBegin(missionId, bundleName, timestamp);
        WLOGFI("call NotifyMissionCreated, persistentId: %{public}d, bundleName: %{public}s",
            missionId, bundleName.c_str());
        collaborator->NotifyMissionCreated(abilitySessionInfo);
    }
}

void SceneSessionManager::NotifyLoadAbility(int32_t collaboratorType,
    sptr<AAFwk::SessionInfo> abilitySessionInfo, std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo)
{
    WLOGFD("type: %{public}d", collaboratorType);
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator = GetCollaboratorByType(collaboratorType);
    if (collaborator != nullptr) {
        WLOGFI("called NotifyLoadAbility");
        collaborator->NotifyLoadAbility(*abilityInfo, abilitySessionInfo);
    }
}


void SceneSessionManager::NotifyUpdateSessionInfo(sptr<SceneSession> sceneSession)
{
    WLOGFD("Enter");
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator = GetCollaboratorByType(sceneSession->GetCollaboratorType());
    auto abilitySessionInfo = SetAbilitySessionInfo(sceneSession);
    if (collaborator != nullptr) {
        WLOGFI("called UpdateMissionInfo");
        collaborator->UpdateMissionInfo(abilitySessionInfo);
    }
}

void SceneSessionManager::NotifyMoveSessionToForeground(int32_t collaboratorType, int32_t persistentId)
{
    WLOGFD("id: %{public}d, type: %{public}d", persistentId, collaboratorType);
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator = GetCollaboratorByType(collaboratorType);
    if (collaborator != nullptr) {
        WLOGFI("called NotifyMoveMissionToForeground %{public}d", persistentId);
        collaborator->NotifyMoveMissionToForeground(persistentId);
    }
}

void SceneSessionManager::NotifyClearSession(int32_t collaboratorType, int32_t persistentId)
{
    WLOGFD("id: %{public}d, type: %{public}d", persistentId, collaboratorType);
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator = GetCollaboratorByType(collaboratorType);
    if (collaborator != nullptr) {
        WLOGFI("called NotifyClearMission %{public}d", persistentId);
        collaborator->NotifyClearMission(persistentId);
    }
}

bool SceneSessionManager::PreHandleCollaborator(sptr<SceneSession>& sceneSession, int32_t persistentId)
{
    if (sceneSession == nullptr) {
        WLOGFI("sceneSession is null");
        return false;
    }
    std::string sessionAffinity;
    WLOGFI("call NotifyStartAbility & NotifySessionCreate");
    if (sceneSession->GetSessionInfo().want != nullptr) {
        sessionAffinity = sceneSession->GetSessionInfo().want
            ->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY);
    }
    if (sessionAffinity.empty()) {
        WLOGFI("Session affinity is empty");
        BrokerStates notifyReturn = NotifyStartAbility(
            sceneSession->GetCollaboratorType(), sceneSession->GetSessionInfo(), persistentId);
        if (notifyReturn != BrokerStates::BROKER_STARTED) {
            WLOGFI("notifyReturn not BROKER_STARTED!");
            return false;
        }
    }
    if (sceneSession->GetSessionInfo().want != nullptr) {
        sceneSession->SetSessionInfoAffinity(sceneSession->GetSessionInfo().want
            ->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY));
        WLOGFI("ANCO_SESSION_ID: %{public}d, want affinity: %{public}s.",
            sceneSession->GetSessionInfo().want->GetIntParam(AncoConsts::ANCO_SESSION_ID, 0),
            sceneSession->GetSessionInfo().sessionAffinity.c_str());
    } else {
        WLOGFI("sceneSession->GetSessionInfo().want is nullptr");
    }
    NotifySessionCreate(sceneSession, sceneSession->GetSessionInfo());
    sceneSession->SetSessionInfoAncoSceneState(AncoSceneState::NOTIFY_CREATE);
    return true;
}

void SceneSessionManager::AddWindowDragHotArea(uint64_t displayId, uint32_t type, WSRect& area)
{
    WLOGFI("displayId: %{public}" PRIu64 " type: %{public}d, posX: %{public}d, posY: %{public}d, width: %{public}d, "
        "height: %{public}d", displayId, type, area.posX_, area.posY_, area.width_, area.height_);
    SceneSession::AddOrUpdateWindowDragHotArea(displayId, type, area);
}

WSError SceneSessionManager::UpdateMaximizeMode(int32_t persistentId, bool isMaximize)
{
    auto task = [this, persistentId, isMaximize]() -> WSError {
        WLOGFD("update maximize mode, id: %{public}d, isMaximize: %{public}d", persistentId, isMaximize);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFE("could not find window, persistentId:%{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_WINDOW;
        }
        sceneSession->UpdateMaximizeMode(isMaximize);
        return WSError::WS_OK;
    };
    taskScheduler_->PostAsyncTask(task, "UpdateMaximizeMode:PID:" + std::to_string(persistentId));
    return WSError::WS_OK;
}

WSError SceneSessionManager::GetIsLayoutFullScreen(bool& isLayoutFullScreen)
{
    auto task = [this, &isLayoutFullScreen]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto item = sceneSessionMap_.begin(); item != sceneSessionMap_.end(); ++item) {
            auto sceneSession = item->second;
            if (sceneSession == nullptr) {
                WLOGFE("Session is nullptr");
                continue;
            }
            if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
                continue;
            }
            auto state = sceneSession->GetSessionState();
            if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
                continue;
            }
            if (sceneSession->GetWindowMode() != WindowMode::WINDOW_MODE_FULLSCREEN) {
                continue;
            }
            auto property = sceneSession->GetSessionProperty();
            if (property == nullptr) {
                WLOGFE("Property is nullptr");
                continue;
            }
            isLayoutFullScreen = property->IsLayoutFullScreen();
            auto persistentId = sceneSession->GetPersistentId();
            if (isLayoutFullScreen) {
                WLOGFD("Current window is immersive, persistentId:%{public}d", persistentId);
                return WSError::WS_OK;
            } else {
                WLOGFD("Current window is not immersive, persistentId:%{public}d", persistentId);
            }
        }
        WLOGFD("No immersive window");
        return WSError::WS_OK;
    };

    taskScheduler_->PostSyncTask(task, "GetIsLayoutFullScreen");
    return WSError::WS_OK;
}

WSError SceneSessionManager::UpdateSessionDisplayId(int32_t persistentId, uint64_t screenId)
{
    auto scnSession = GetSceneSession(persistentId);
    if (!scnSession) {
        WLOGFE("session is nullptr");
        return WSError::WS_ERROR_INVALID_WINDOW;
    }
    auto fromScreenId = scnSession->GetSessionInfo().screenId_;
    scnSession->SetScreenId(screenId);
    auto sessionProperty = scnSession->GetSessionProperty();
    if (!sessionProperty) {
        WLOGFE("Property is null, synchronous screenId failed");
        return WSError::WS_ERROR_NULLPTR;
    }
    sessionProperty->SetDisplayId(screenId);
    WLOGFD("Session move display %{public}" PRIu64 " from %{public}" PRIu64, screenId, fromScreenId);
    NotifySessionUpdate(scnSession->GetSessionInfo(), ActionType::MOVE_DISPLAY, fromScreenId);
    scnSession->NotifyDisplayMove(fromScreenId, screenId);
    scnSession->UpdateDensity();
    return WSError::WS_OK;
}

WSError SceneSessionManager::NotifyStackEmpty(int32_t persistentId)
{
    TLOGI(WmsLogTag::WMS_LIFE, "NotifyStackEmpty, persistentId %{public}d", persistentId);
    auto task = [this, persistentId]() {
        auto scnSession = GetSceneSession(persistentId);
        if (!scnSession) {
            TLOGE(WmsLogTag::WMS_LIFE, "session is nullptr");
            return WSError::WS_ERROR_INVALID_WINDOW;
        }
        NotifySessionUpdate(scnSession->GetSessionInfo(), ActionType::STACK_EMPTY);
        return WSError::WS_OK;
    };
    taskScheduler_->PostAsyncTask(task, "NotifyStackEmpty:PID:" + std::to_string(persistentId));
    return WSError::WS_OK;
}

void DisplayChangeListener::OnImmersiveStateChange(bool& immersive)
{
    immersive = SceneSessionManager::GetInstance().GetImmersiveState();
}

bool SceneSessionManager::GetImmersiveState()
{
    auto task = [this] {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto item = sceneSessionMap_.begin(); item != sceneSessionMap_.end(); ++item) {
            auto sceneSession = item->second;
            if (sceneSession == nullptr) {
                WLOGFE("Session is nullptr");
                continue;
            }
            if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
                continue;
            }
            auto state = sceneSession->GetSessionState();
            if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
                continue;
            }
            if (sceneSession->GetWindowMode() != WindowMode::WINDOW_MODE_FULLSCREEN) {
                continue;
            }
            auto property = sceneSession->GetSessionProperty();
            if (property == nullptr) {
                WLOGFE("Property is nullptr");
                continue;
            }
            auto sysBarProperty = property->GetSystemBarProperty();
            if (sysBarProperty[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ == false) {
                WLOGI("GetImmersiveState, window is immersive. id:%{public}d", sceneSession->GetPersistentId());
                return true;
            } else {
                WLOGI("GetImmersiveState, statusBar is enabled. id:%{public}d", sceneSession->GetPersistentId());
                break;
            }
        }
        WLOGI("GetImmersiveState, not immersive");
        return false;
    };
    return taskScheduler_->PostSyncTask(task, "GetImmersiveState");
}

void SceneSessionManager::NotifySessionForeground(const sptr<SceneSession>& session, uint32_t reason,
    bool withAnimation)
{
    session->NotifySessionForeground(reason, withAnimation);
}

void SceneSessionManager::NotifySessionBackground(const sptr<SceneSession>& session, uint32_t reason,
    bool withAnimation, bool isFromInnerkits)
{
    session->NotifySessionBackground(reason, withAnimation, isFromInnerkits);
}

WSError SceneSessionManager::UpdateTitleInTargetPos(int32_t persistentId, bool isShow, int32_t height)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("could not find window, persistentId:%{public}d", persistentId);
        return WSError::WS_ERROR_INVALID_WINDOW;
    }
    return sceneSession->UpdateTitleInTargetPos(isShow, height);
}

void AppAnrListener::OnAppDebugStarted(const std::vector<AppExecFwk::AppDebugInfo>& debugInfos)
{
    WLOGFI("AppAnrListener OnAppDebugStarted");
    if (debugInfos.empty()) {
        WLOGFE("AppAnrListener OnAppDebugStarted debugInfos is empty");
        return;
    }
    DelayedSingleton<ANRManager>::GetInstance()->SwitchAnr(false);
}

void AppAnrListener::OnAppDebugStoped(const std::vector<AppExecFwk::AppDebugInfo>& debugInfos)
{
    WLOGFI("AppAnrListener OnAppDebugStoped");
    if (debugInfos.empty()) {
        WLOGFE("AppAnrListener OnAppDebugStoped debugInfos is empty");
        return;
    }
    DelayedSingleton<ANRManager>::GetInstance()->SwitchAnr(true);
}

void SceneSessionManager::FlushUIParams(ScreenId screenId, std::unordered_map<int32_t, SessionUIParam>&& uiParams)
{
    if (!Session::IsScbCoreEnabled()) {
        return;
    }
    auto task = [this, screenId, uiParams = std::move(uiParams)]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSessionManager::FlushUIParams");
        TLOGD(WmsLogTag::WMS_PIPELINE, "FlushUIParams");
        {
            std::unique_lock<std::mutex> lock(nextFlushCompletedMutex_);
            nextFlushCompletedCV_.notify_all();
        }
        processingFlushUIParams_.store(true);
        uint32_t sessionMapDirty = 0;
        {
            std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            for (const auto& item : sceneSessionMap_) {
                auto sceneSession = item.second;
                if (sceneSession == nullptr) {
                    continue;
                }
                if (sceneSession->GetSessionInfo().screenId_ != screenId) {
                    continue;
                }
                auto iter = uiParams.find(sceneSession->GetPersistentId());
                if (iter != uiParams.end()) {
                    sessionMapDirty |= sceneSession->UpdateUIParam(iter->second);
                } else {
                    sessionMapDirty |= sceneSession->UpdateUIParam();
                }
            }
        }
        processingFlushUIParams_.store(false);

        // post process if dirty
        if ((sessionMapDirty & (~static_cast<uint32_t>(SessionUIDirtyFlag::AVOID_AREA))) !=
            static_cast<uint32_t>(SessionUIDirtyFlag::NONE)) {
            TLOGD(WmsLogTag::WMS_PIPELINE, "FlushUIParams found dirty: %{public}d", sessionMapDirty);
            for (const auto& item : uiParams) {
                TLOGD(WmsLogTag::WMS_PIPELINE,
                    "id: %{public}d, zOrder: %{public}d, rect: %{public}s, transX: %{public}f, transY: %{public}f,"
                    " needSync: %{public}d, interactive: %{public}d",
                    item.first, item.second.zOrder_, item.second.rect_.ToString().c_str(), item.second.transX_,
                    item.second.transY_, item.second.needSync_, item.second.interactive_);
            }
            ProcessFocusZOrderChange(sessionMapDirty);
            PostProcessFocus();
            PostProcessProperty(sessionMapDirty);
            NotifyAllAccessibilityInfo();
            FlushWindowInfoToMMI();
            AnomalyDetection::SceneZOrderCheckProcess();
        } else if (sessionMapDirty == static_cast<uint32_t>(SessionUIDirtyFlag::AVOID_AREA)) {
            PostProcessProperty(sessionMapDirty);
        }
        {
            std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            for (const auto& item : sceneSessionMap_) {
                auto sceneSession = item.second;
                if (sceneSession == nullptr) {
                    continue;
                }
                sceneSession->ResetDirtyFlags();
                if (WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
                    sceneSession->SetUIStateDirty(false);
                }
            }
        }
    };
    taskScheduler_->PostAsyncTask(task, "FlushUIParams");
}

void SceneSessionManager::ProcessFocusZOrderChange(uint32_t dirty) {
    if (!(dirty & static_cast<uint32_t>(SessionUIDirtyFlag::Z_ORDER))) {
        return;
    }
    if (!systemConfig_.IsPhoneWindow() && !systemConfig_.IsPadWindow()) {
        return;
    }
    TLOGD(WmsLogTag::WMS_FOCUS, "has zOrder dirty");
    auto focusedSession = GetSceneSession(focusedSessionId_);
    // only when it's from a high zOrder to a low zOrder
    if (focusedSession == nullptr || focusedSession->GetWindowType() == WindowType::WINDOW_TYPE_VOICE_INTERACTION ||
        focusedSession->GetLastZOrder() <= focusedSession->GetZOrder()) {
        return;
    }
    auto voiceInteractionSession = GetSceneSessionByType(WindowType::WINDOW_TYPE_VOICE_INTERACTION);
    if (voiceInteractionSession == nullptr) {
        return;
    }
    TLOGD(WmsLogTag::WMS_FOCUS, "interactionSession: id %{public}d zOrder %{public}d, focusedSession: lastZOrder "
          "%{public}d zOrder %{public}d", voiceInteractionSession->GetPersistentId(),
          voiceInteractionSession->GetZOrder(), focusedSession->GetLastZOrder(), focusedSession->GetZOrder());
    if (focusedSession->GetLastZOrder() < voiceInteractionSession->GetZOrder() ||
        focusedSession->GetZOrder() > voiceInteractionSession->GetZOrder()) {
        return;
    }
    RequestSessionFocus(voiceInteractionSession->GetPersistentId(), true, FocusChangeReason::VOICE_INTERACTION);
}

void SceneSessionManager::PostProcessFocus()
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSessionManager::PostProcessFocus");
    // priority process focus requests from top to bottom
    std::vector<std::pair<int32_t, sptr<SceneSession>>> processingSessions;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto& iter : sceneSessionMap_) {
            auto session = iter.second;
            if (session == nullptr || !session->GetPostProcessFocusState().enabled_) {
                continue;
            }
            processingSessions.push_back(iter);
        }
    }
    CmpFunc cmp = [](std::pair<int32_t, sptr<SceneSession>>& lhs, std::pair<int32_t, sptr<SceneSession>>& rhs) {
        bool focusCmp = lhs.second->GetPostProcessFocusState().isFocused_ &&
            !rhs.second->GetPostProcessFocusState().isFocused_;
        uint32_t lhsZOrder = lhs.second != nullptr ? lhs.second->GetZOrder() : 0;
        uint32_t rhsZOrder = rhs.second != nullptr ? rhs.second->GetZOrder() : 0;
        return focusCmp || lhsZOrder > rhsZOrder;
    };
    std::sort(processingSessions.begin(), processingSessions.end(), cmp);

    // only change focus one time
    bool focusChanged = false;
    for (auto iter = processingSessions.begin(); iter != processingSessions.end(); ++iter) {
        auto session = iter->second;
        if (session == nullptr) {
            WLOGFE("session is nullptr");
            continue;
        }
        TLOGD(WmsLogTag::WMS_PIPELINE, "id: %{public}d, isFocused: %{public}d, reason: %{public}d",
            session->GetPersistentId(), session->GetPostProcessFocusState().isFocused_,
            session->GetPostProcessFocusState().reason_);
        if (focusChanged) {
            session->ResetPostProcessFocusState();
            continue;
        }
        WSError ret = WSError::WS_DO_NOTHING;
        if (session->GetPostProcessFocusState().isFocused_) {
            if (session->GetPostProcessFocusState().reason_ == FocusChangeReason::SCB_START_APP) {
                ret = RequestSessionFocusImmediately(session->GetPersistentId());
            } else {
                ret = RequestSessionFocus(session->GetPersistentId(), true,
                                          session->GetPostProcessFocusState().reason_);
            }
        } else {
            ret = RequestSessionUnfocus(session->GetPersistentId(), session->GetPostProcessFocusState().reason_);
        }
        session->ResetPostProcessFocusState();
        // if succeed then end process
        if (ret == WSError::WS_OK) {
            focusChanged = true;
        }
    }
}

void SceneSessionManager::PostProcessProperty(uint32_t dirty)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSessionManager::PostProcessProperty");
    if (dirty == static_cast<uint32_t>(SessionUIDirtyFlag::AVOID_AREA)) {
        // only trigger update avoid area
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto& iter : sceneSessionMap_) {
            auto session = iter.second;
            if (session == nullptr) {
                continue;
            }
            session->PostProcessNotifyAvoidArea();
        }
        return;
    }

    std::vector<std::pair<int32_t, sptr<SceneSession>>> processingSessions;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto& iter : sceneSessionMap_) {
            auto session = iter.second;
            if (session == nullptr || !session->GetPostProcessProperty()) {
                continue;
            }
            processingSessions.push_back(iter);
        }
    }

    for (auto iter = processingSessions.begin(); iter != processingSessions.end(); ++iter) {
        auto session = iter->second;
        if (session == nullptr) {
            WLOGFE("session is nullptr");
            continue;
        }
        TLOGD(WmsLogTag::WMS_PIPELINE, "id: %{public}d", session->GetPersistentId());
        UpdateForceHideState(session, session->GetSessionProperty(), true);
        HandleKeepScreenOn(session, session->IsKeepScreenOn());
        UpdatePrivateStateAndNotify(session->GetPersistentId());
        if (session->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            ProcessSubSessionForeground(session);
        }
        session->SetPostProcessProperty(false);
    }

    // update avoid area
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto& iter : sceneSessionMap_) {
            auto session = iter.second;
            if (session == nullptr) {
                continue;
            }
            session->PostProcessNotifyAvoidArea();
        }
    }
}

/** @note @window.hierarchy */
WSError SceneSessionManager::RaiseWindowToTop(int32_t persistentId)
{
    WLOGFI("RaiseWindowToTop, id %{public}d", persistentId);
    auto isSaCall = SessionPermission::IsSACalling();
    if (!isSaCall) {
        WLOGFE("The interface only support for sa call");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, persistentId]() {
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFE("session is nullptr");
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        if (!IsSessionVisibleForeground(sceneSession)) {
            WLOGFD("session is not visible!");
            return WSError::WS_DO_NOTHING;
        }
        FocusChangeReason reason = FocusChangeReason::MOVE_UP;
        RequestSessionFocus(persistentId, true, reason);
        if (WindowHelper::IsSubWindow(sceneSession->GetWindowType())) {
            sceneSession->RaiseToAppTop();
        }
        if (WindowHelper::IsSubWindow(sceneSession->GetWindowType()) ||
            sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
            WLOGFD("parent session id: %{public}d", sceneSession->GetParentPersistentId());
            sceneSession = GetSceneSession(sceneSession->GetParentPersistentId());
        }
        if (sceneSession == nullptr) {
            WLOGFE("parent session is nullptr");
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            sceneSession->NotifyClick();
            return WSError::WS_OK;
        } else {
            WLOGFE("session is not app main window!");
            return WSError::WS_ERROR_INVALID_SESSION;
        }
    };
    taskScheduler_->PostAsyncTask(task, "RaiseWindowToTop");
    return WSError::WS_OK;
}

WSError SceneSessionManager::ShiftAppWindowFocus(int32_t sourcePersistentId, int32_t targetPersistentId)
{
    WLOGFI("from id: %{public}d to id: %{public}d", sourcePersistentId, targetPersistentId);
    if (sourcePersistentId != focusedSessionId_) {
        WLOGFE("source session need be focused");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    if (targetPersistentId == focusedSessionId_) {
        WLOGFE("target session has been focused");
        return WSError::WS_DO_NOTHING;
    }
    sptr<SceneSession> sourceSession = nullptr;
    WSError ret = GetAppMainSceneSession(sourceSession, sourcePersistentId);
    if (ret != WSError::WS_OK) {
        return ret;
    }
    sptr<SceneSession> targetSession = nullptr;
    ret = GetAppMainSceneSession(targetSession, targetPersistentId);
    if (ret != WSError::WS_OK) {
        return ret;
    }
    if (sourceSession->GetSessionInfo().bundleName_ != targetSession->GetSessionInfo().bundleName_) {
        WLOGFE("verify bundle failed, source name is %{public}s but target name is %{public}s)",
            sourceSession->GetSessionInfo().bundleName_.c_str(), targetSession->GetSessionInfo().bundleName_.c_str());
        return WSError::WS_ERROR_INVALID_CALLING;
    }
    if (!SessionPermission::IsSameBundleNameAsCalling(targetSession->GetSessionInfo().bundleName_)) {
        return WSError::WS_ERROR_INVALID_CALLING;
    }
    int32_t callingPid = IPCSkeleton::GetCallingPid();
    if (callingPid != targetSession->GetCallingPid()) {
        TLOGE(WmsLogTag::WMS_FOCUS, "permission denied, not call by the same process");
        return WSError::WS_ERROR_INVALID_CALLING;
    }
    targetSession->NotifyClick();
    FocusChangeReason reason = FocusChangeReason::CLIENT_REQUEST;
    return RequestSessionFocus(targetPersistentId, false, reason);
}

WSError SceneSessionManager::GetAppMainSceneSession(sptr<SceneSession>& sceneSession, int32_t persistentId)
{
    sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("session(%{public}d) is nullptr", persistentId);
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_APP_SUB_WINDOW) {
            WLOGFE("session(%{public}d) is not main window or sub window", persistentId);
            return WSError::WS_ERROR_INVALID_CALLING;
        }
        sceneSession = GetSceneSession(sceneSession->GetParentPersistentId());
        if (sceneSession == nullptr) {
            WLOGFE("session(%{public}d) parent is nullptr", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
    }
    return WSError::WS_OK;
}

std::shared_ptr<Media::PixelMap> SceneSessionManager::GetSessionSnapshotPixelMap(const int32_t persistentId,
    const float scaleParam)
{
    auto sceneSession = GetSceneSession(persistentId);
    if (!sceneSession) {
        WLOGFE("get scene session is nullptr");
        return nullptr;
    }

    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, persistentId, scaleParam, weakSceneSession]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:GetSessionSnapshotPixelMap(%d )", persistentId);
        auto scnSession = weakSceneSession.promote();
        std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
        if (scnSession == nullptr) {
            WLOGFE("session is nullptr");
            return pixelMap;
        }

        bool isPc = systemConfig_.IsPcWindow() || systemConfig_.IsFreeMultiWindowMode();
        pixelMap = scnSession->Snapshot(false, scaleParam, isPc);
        if (!pixelMap) {
            WLOGFI("get local snapshot pixelmap start");
            pixelMap = scnSession->GetSnapshotPixelMap(snapshotScale_, scaleParam);
        }
        return pixelMap;
    };
    return taskScheduler_->PostSyncTask(task, "GetSessionSnapshotPixelMap" + std::to_string(persistentId));
}

const std::map<int32_t, sptr<SceneSession>> SceneSessionManager::GetSceneSessionMap()
{
    std::map<int32_t, sptr<SceneSession>> retSceneSessionMap;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        retSceneSessionMap = sceneSessionMap_;
    }
    EraseIf(retSceneSessionMap, [this](const auto& pair) {
        if (pair.second == nullptr) {
            return true;
        }

        if (pair.second->GetWindowType() == WindowType::WINDOW_TYPE_KEYBOARD_PANEL) {
            if (pair.second->IsVisible()) {
                return false;
            }
            return true;
        }

        if (pair.second->IsSystemInput()) {
            return false;
        } else if (pair.second->IsSystemSession() && pair.second->IsVisible() && pair.second->IsSystemActive()) {
            return false;
        }

        if (!Rosen::SceneSessionManager::GetInstance().IsSessionVisible(pair.second)) {
            return true;
        }
        return false;
    });
    return retSceneSessionMap;
}

void SceneSessionManager::NotifyUpdateRectAfterLayout()
{
    auto transactionController = Rosen::RSSyncTransactionController::GetInstance();
    std::shared_ptr<RSTransaction> rsTransaction = nullptr;
    if (transactionController) {
        rsTransaction = transactionController->GetRSTransaction();
    }
    auto task = [this, rsTransaction]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& iter: sceneSessionMap_) {
            auto sceneSession = iter.second;
            if (sceneSession && sceneSession->IsDirtyWindow()) {
                sceneSession->NotifyClientToUpdateRect("AfterLayoutFromPersistentTask", rsTransaction);
            }
        }
    };
    // need sync task since animation transcation need
    return taskScheduler_->PostAsyncTask(task, "NotifyUpdateRectAfterLayout");
}

WMError SceneSessionManager::GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos)
{
    if (!SessionPermission::IsSystemCalling()) {
        WLOGFE("GetVisibilityWindowInfo permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    auto task = [this, &infos]() {
        for (auto [surfaceId, _] : lastVisibleData_) {
            sptr<SceneSession> session = SelectSesssionFromMap(surfaceId);
            if (session == nullptr) {
                continue;
            }
            WSRect hostRect = session->GetSessionRect();
            Rect rect = {hostRect.posX_, hostRect.posY_,
                         static_cast<uint32_t>(hostRect.width_), static_cast<uint32_t>(hostRect.height_)};
            auto windowStatus = GetWindowStatus(session->GetWindowMode(), session->GetSessionState(),
                                                session->GetSessionProperty());
            infos.emplace_back(sptr<WindowVisibilityInfo>::MakeSptr(session->GetWindowId(), session->GetCallingPid(),
                session->GetCallingUid(), session->GetVisibilityState(), session->GetWindowType(), windowStatus, rect,
                session->GetSessionInfo().bundleName_, session->GetSessionInfo().abilityName_));
        }
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetVisibilityWindowInfo");
}

void SceneSessionManager::GetAllWindowVisibilityInfos(std::vector<std::pair<int32_t, uint32_t>>& windowVisibilityInfos)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& [id, session] : sceneSessionMap_) {
        if (session == nullptr) {
            continue;
        }
        uint32_t visibilityState = static_cast<uint32_t>(session->GetVisibilityState());
        windowVisibilityInfos.push_back(std::make_pair(id, visibilityState));
    }
}

void SceneSessionManager::FlushWindowInfoToMMI(const bool forceFlush)
{
    auto task = [this, forceFlush] {
        if (isUserBackground_) {
            TLOGD(WmsLogTag::WMS_MULTI_USER, "The user is in the background, no need to flush info to MMI");
            return;
        }
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSessionManager::FlushWindowInfoToMMI");
        SceneInputManager::GetInstance().FlushDisplayInfoToMMI(forceFlush);
    };
    taskScheduler_->PostAsyncTask(task);
}

void SceneSessionManager::PostFlushWindowInfoTask(FlushWindowInfoTask &&task,
    const std::string taskName, const int delayTime)
{
    taskScheduler_->PostAsyncTask(std::move(task), taskName, delayTime);
}

bool SceneSessionManager::GetExtensionWindowIds(const sptr<IRemoteObject>& token, int32_t& persistentId,
    int32_t& parentId)
{
    // This function should be called in task
    auto iter = extSessionInfoMap_.find(token);
    if (iter == extSessionInfoMap_.end()) {
        return false;
    }
    persistentId = iter->second.persistentId;
    parentId = iter->second.parentId;
    return true;
}

void SceneSessionManager::DestroyExtensionSession(const sptr<IRemoteObject>& remoteExtSession)
{
    auto task = [this, remoteExtSession]() {
        auto iter = remoteExtSessionMap_.find(remoteExtSession);
        if (iter == remoteExtSessionMap_.end()) {
            TLOGI(WmsLogTag::WMS_UIEXT, "Invalid remoteExtSession or already destroyed");
            return;
        }
        int32_t persistentId = INVALID_SESSION_ID;
        int32_t parentId = INVALID_SESSION_ID;
        if (!GetExtensionWindowIds(iter->second, persistentId, parentId)) {
            TLOGE(WmsLogTag::WMS_UIEXT, "Get UIExtension window ids by token failed");
            return;
        }

        TLOGI(WmsLogTag::WMS_UIEXT, "DestroyExtensionSession: persistentId=%{public}d, parentId=%{public}d",
            persistentId, parentId);
        auto sceneSession = GetSceneSession(parentId);
        if (sceneSession != nullptr) {
            auto oldFlags = sceneSession->GetCombinedExtWindowFlags();
            sceneSession->RemoveExtWindowFlags(persistentId);
            if (oldFlags.hideNonSecureWindowsFlag) {
                HandleSecureSessionShouldHide(sceneSession);
            }
            if (oldFlags.waterMarkFlag) {
                CheckAndNotifyWaterMarkChangedResult();
            }
            if (oldFlags.privacyModeFlag) {
                UpdatePrivateStateAndNotify(parentId);
            }
            sceneSession->RemoveModalUIExtension(persistentId);
            sceneSession->RemoveUIExtSurfaceNodeId(persistentId);
        } else {
            ExtensionWindowFlags actions;
            actions.SetAllActive();
            HandleSpecialExtWindowFlagsChange(persistentId, ExtensionWindowFlags(), actions);
        }
        extSessionInfoMap_.erase(iter->second);
        remoteExtSessionMap_.erase(iter);
    };
    taskScheduler_->PostAsyncTask(task, "DestroyExtensionSession");
}

void SceneSessionManager::UpdateModalExtensionRect(const sptr<IRemoteObject>& token, Rect rect)
{
    auto pid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, token, pid, rect]() {
        int32_t persistentId = INVALID_SESSION_ID;
        int32_t parentId = INVALID_SESSION_ID;
        if (!GetExtensionWindowIds(token, persistentId, parentId)) {
            TLOGE(WmsLogTag::WMS_UIEXT, "Get UIExtension window ids by token failed");
            return;
        }
        TLOGI(WmsLogTag::WMS_UIEXT, "UpdateModalExtensionRect: pid=%{public}d, persistentId=%{public}d, "
            "parentId=%{public}d, Rect:[%{public}d %{public}d %{public}d %{public}d]",
            pid, persistentId, parentId, rect.posX_, rect.posY_, rect.width_, rect.height_);
        auto parentSession = GetSceneSession(parentId);
        if (parentSession) {
            ExtensionWindowEventInfo extensionInfo {persistentId, pid, rect};
            parentSession->UpdateModalUIExtension(extensionInfo);
        }
    };
    taskScheduler_->PostAsyncTask(task, "UpdateModalExtensionRect");
}

void SceneSessionManager::ProcessModalExtensionPointDown(const sptr<IRemoteObject>& token, int32_t posX, int32_t posY)
{
    auto pid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, token, pid, posX, posY]() {
        int32_t persistentId = INVALID_SESSION_ID;
        int32_t parentId = INVALID_SESSION_ID;
        if (!GetExtensionWindowIds(token, persistentId, parentId)) {
            TLOGE(WmsLogTag::WMS_UIEXT, "Get UIExtension window ids by token failed");
            return;
        }
        TLOGI(WmsLogTag::WMS_UIEXT, "ProcessModalExtensionPointDown: pid=%{public}d, persistentId=%{public}d, "
            "parentId=%{public}d", pid, persistentId, parentId);
        auto parentSession = GetSceneSession(parentId);
        if (parentSession && parentSession->HasModalUIExtension()) {
            auto modalUIExtension = parentSession->GetLastModalUIExtensionEventInfo();
            if ((modalUIExtension.pid == pid) && (modalUIExtension.persistentId == persistentId)) {
                parentSession->ProcessPointDownSession(posX, posY);
            }
        }
    };
    taskScheduler_->PostAsyncTask(task, "ProcessModalExtensionPointDown");
}

void SceneSessionManager::AddExtensionWindowStageToSCB(const sptr<ISessionStage>& sessionStage,
    const sptr<IRemoteObject>& token, uint64_t surfaceNodeId)
{
    auto pid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, sessionStage, token, surfaceNodeId, pid]() {
        if (sessionStage == nullptr || token == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "input is nullptr");
            return;
        }
        auto remoteExtSession = sessionStage->AsObject();
        if (remoteExtSession == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "sessionStage object is nullptr");
            return;
        }
        if (extensionDeath_ == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "failed to create death recipient");
            return;
        }
        if (!remoteExtSession->AddDeathRecipient(extensionDeath_)) {
            TLOGE(WmsLogTag::WMS_UIEXT, "failed to add death recipient");
            return;
        }

        AAFwk::UIExtensionSessionInfo info;
        AAFwk::AbilityManagerClient::GetInstance()->GetUIExtensionSessionInfo(token, info);
        if (info.persistentId == INVALID_SESSION_ID || info.hostWindowId == INVALID_SESSION_ID) {
            TLOGE(WmsLogTag::WMS_UIEXT, "Get UIExtension session info failed");
            return;
        }

        int32_t persistentId = info.persistentId;
        int32_t parentId = static_cast<int32_t>(info.hostWindowId);
        UIExtensionUsage usage = static_cast<UIExtensionUsage>(info.uiExtensionUsage);
        TLOGI(WmsLogTag::WMS_UIEXT, "AddExtensionWindowStageToSCB: persistentId=%{public}d, parentId=%{public}d, "
            "usage=%{public}u, surfaceNodeId=%{public}" PRIu64", pid=%{public}d", persistentId, parentId, usage,
            surfaceNodeId, pid);

        remoteExtSessionMap_.insert(std::make_pair(remoteExtSession, token));
        extSessionInfoMap_.insert(std::make_pair(token, ExtensionWindowAbilityInfo{ persistentId, parentId, usage }));

        auto parentSession = GetSceneSession(parentId);
        if (parentSession) {
            parentSession->AddUIExtSurfaceNodeId(surfaceNodeId, persistentId);
        }
        if (usage == UIExtensionUsage::MODAL && parentSession) {
            ExtensionWindowEventInfo extensionInfo {
                .persistentId = persistentId,
                .pid = pid,
            };
            parentSession->AddModalUIExtension(extensionInfo);
        }
    };
    taskScheduler_->PostAsyncTask(task, "AddExtensionWindowStageToSCB");
}

void SceneSessionManager::RemoveExtensionWindowStageFromSCB(const sptr<ISessionStage>& sessionStage,
    const sptr<IRemoteObject>& token)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "called");
    auto task = [this, sessionStage, token]() {
        if (sessionStage == nullptr || token == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "input is nullptr");
            return;
        }
        auto remoteExtSession = sessionStage->AsObject();
        if (remoteExtSession == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "sessionStage object is nullptr");
            return;
        }
        auto iter = remoteExtSessionMap_.find(remoteExtSession);
        if (iter->second != token) {
            TLOGE(WmsLogTag::WMS_UIEXT, "token not match");
            return;
        }

        DestroyExtensionSession(remoteExtSession);
    };
    taskScheduler_->PostAsyncTask(task, "RemoveExtensionWindowStageFromSCB");
}

void SceneSessionManager::CalculateCombinedExtWindowFlags()
{
    // Only correct when each flag is true when active, and once a uiextension is active, the host is active
    combinedExtWindowFlags_.bitData = 0;
    for (const auto& iter: extWindowFlagsMap_) {
        combinedExtWindowFlags_.bitData |= iter.second.bitData;
    }
    specialExtWindowHasPrivacyMode_.store(combinedExtWindowFlags_.privacyModeFlag);
}

void SceneSessionManager::UpdateSpecialExtWindowFlags(int32_t persistentId, ExtensionWindowFlags flags,
    ExtensionWindowFlags actions)
{
    auto iter = extWindowFlagsMap_.find(persistentId);
    // Each flag is false when inactive, 0 means all flags are inactive
    auto oldFlags = iter != extWindowFlagsMap_.end() ? iter->second : ExtensionWindowFlags();
    ExtensionWindowFlags newFlags((flags.bitData & actions.bitData) | (oldFlags.bitData & ~actions.bitData));
    if (newFlags.bitData == 0) {
        extWindowFlagsMap_.erase(persistentId);
    } else {
        extWindowFlagsMap_[persistentId] = newFlags;
    }
    CalculateCombinedExtWindowFlags();
}

void SceneSessionManager::HideNonSecureFloatingWindows()
{
    bool shouldHide = false;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& iter: sceneSessionMap_) {
            auto& session = iter.second;
            if (session && session->GetCombinedExtWindowFlags().hideNonSecureWindowsFlag) {
                shouldHide = true;
                break;
            }
        }
    }
    if (combinedExtWindowFlags_.hideNonSecureWindowsFlag) {
        TLOGI(WmsLogTag::WMS_UIEXT, "SCB UIExtension hide non-secure windows");
        shouldHide = true;
    }
    if (shouldHide == shouldHideNonSecureFloatingWindows_.load()) {
        return;
    }

    shouldHideNonSecureFloatingWindows_.store(shouldHide);
    for (const auto& [persistentId, session] : nonSystemFloatSceneSessionMap_) {
        if (session && session->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT) {
            session->NotifyForceHideChange(shouldHide);
            TLOGI(WmsLogTag::WMS_UIEXT, "name=%{public}s, persistentId=%{public}d, shouldHide=%{public}u",
                session->GetWindowName().c_str(), persistentId, shouldHide);
        }
    }
}

void SceneSessionManager::HideNonSecureSubWindows(const sptr<SceneSession>& sceneSession)
{
    // don't let sub-window show when switching secure host window to background
    if (!sceneSession->IsSessionForeground()) {
        return;
    }

    auto parentId = sceneSession->GetPersistentId();
    bool shouldHide = sceneSession->GetCombinedExtWindowFlags().hideNonSecureWindowsFlag;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& [persistentId, session] : sceneSessionMap_) {
        if (!session) {
            continue;
        }
        auto property = session->GetSessionProperty();
        if (!property || property->GetParentPersistentId() != parentId) {
            continue;
        }

        if (SessionHelper::IsNonSecureToUIExtension(property->GetWindowType()) && !session->IsSystemSpecificSession()) {
            session->NotifyForceHideChange(shouldHide);
            TLOGI(WmsLogTag::WMS_UIEXT, "name=%{public}s, persistentId=%{public}d, shouldHide=%{public}u",
                session->GetWindowName().c_str(), session->GetPersistentId(), shouldHide);
        }
    }
}

WSError SceneSessionManager::HandleSecureSessionShouldHide(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "sceneSession is nullptr");
        return WSError::WS_ERROR_INVALID_SESSION;
    }

    HideNonSecureFloatingWindows();
    HideNonSecureSubWindows(sceneSession);
    return WSError::WS_OK;
}

void SceneSessionManager::HandleSpecialExtWindowFlagsChange(int32_t persistentId, ExtensionWindowFlags flags,
    ExtensionWindowFlags actions)
{
    UpdateSpecialExtWindowFlags(persistentId, flags, actions);
    if (actions.waterMarkFlag) {
        CheckAndNotifyWaterMarkChangedResult();
    }
    if (actions.hideNonSecureWindowsFlag) {
        HideNonSecureFloatingWindows();
    }
    if (actions.privacyModeFlag) {
        UpdatePrivateStateAndNotifyForAllScreens();
    }
}

WSError SceneSessionManager::AddOrRemoveSecureSession(int32_t persistentId, bool shouldHide)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "persistentId=%{public}d, shouldHide=%{public}u", persistentId, shouldHide);
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "HideNonSecureWindows permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    const auto callingPid = IPCSkeleton::GetCallingRealPid();
    auto task = [this, persistentId, shouldHide, callingPid]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        auto iter = sceneSessionMap_.find(persistentId);
        if (iter == sceneSessionMap_.end()) {
            TLOGE(WmsLogTag::WMS_UIEXT, "AddOrRemoveSecureSession: Session with persistentId %{public}d not found",
                persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto sceneSession = iter->second;
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "AddOrRemoveSecureSession: sceneSession is nullptr.");
            return WSError::WS_ERROR_NULLPTR;
        }
        if (callingPid != sceneSession->GetCallingPid()) {
            TLOGE(WmsLogTag::WMS_UIEXT, "AddOrRemoveSecureSession: Permission denied");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        sceneSession->SetShouldHideNonSecureWindows(shouldHide);
        return HandleSecureSessionShouldHide(sceneSession);
    };

    taskScheduler_->PostAsyncTask(task, "AddOrRemoveSecureSession");
    return WSError::WS_OK;
}

WSError SceneSessionManager::CheckExtWindowFlagsPermission(ExtensionWindowFlags& actions) const
{
    auto ret = WSError::WS_OK;
    bool needSystemCalling = actions.hideNonSecureWindowsFlag || actions.waterMarkFlag;
    if (needSystemCalling && !SessionPermission::IsSystemCalling()) {
        actions.hideNonSecureWindowsFlag = false;
        actions.waterMarkFlag = false;
        TLOGE(WmsLogTag::WMS_UIEXT, "system calling permission denied!");
        ret = WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    auto needPrivacyWindow = actions.privacyModeFlag;
    if (needPrivacyWindow && !SessionPermission::VerifyCallingPermission("ohos.permission.PRIVACY_WINDOW")) {
        actions.privacyModeFlag = false;
        TLOGE(WmsLogTag::WMS_UIEXT, "privacy window permission denied!");
        ret = WSError::WS_ERROR_INVALID_PERMISSION;
    }
    return ret;
}

WSError SceneSessionManager::UpdateExtWindowFlags(const sptr<IRemoteObject>& token, uint32_t extWindowFlags,
    uint32_t extWindowActions)
{
    ExtensionWindowFlags actions(extWindowActions);
    auto ret = CheckExtWindowFlagsPermission(actions);
    if (actions.bitData == 0) {
        return ret;
    }

    ExtensionWindowFlags flags(extWindowFlags);
    auto task = [this, token, flags, actions]() {
        int32_t persistentId = INVALID_SESSION_ID;
        int32_t parentId = INVALID_SESSION_ID;
        if (!GetExtensionWindowIds(token, persistentId, parentId)) {
            TLOGE(WmsLogTag::WMS_UIEXT, "Get UIExtension window ids by token failed");
            return WSError::WS_ERROR_INVALID_OPERATION;
        }

        TLOGI(WmsLogTag::WMS_UIEXT, "UpdateExtWindowFlags: parentId=%{public}d, persistentId=%{public}d, "
            "extWindowFlags=%{public}d, actions=%{public}d", parentId, persistentId, flags.bitData, actions.bitData);
        auto sceneSession = GetSceneSession(parentId);
        if (sceneSession == nullptr) {
            TLOGD(WmsLogTag::WMS_UIEXT, "UpdateExtWindowFlags: Parent session with persistentId %{public}d not found",
                parentId);
            HandleSpecialExtWindowFlagsChange(persistentId, flags, actions);
            return WSError::WS_OK;
        }

        auto oldFlags = sceneSession->GetCombinedExtWindowFlags();
        sceneSession->UpdateExtWindowFlags(persistentId, flags, actions);
        auto newFlags = sceneSession->GetCombinedExtWindowFlags();
        if (oldFlags.hideNonSecureWindowsFlag != newFlags.hideNonSecureWindowsFlag) {
            HandleSecureSessionShouldHide(sceneSession);
        }
        if (oldFlags.waterMarkFlag != newFlags.waterMarkFlag) {
            CheckAndNotifyWaterMarkChangedResult();
        }
        if (oldFlags.privacyModeFlag != newFlags.privacyModeFlag) {
            UpdatePrivateStateAndNotify(parentId);
        }
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task, "UpdateExtWindowFlags");
    return ret;
}

void SceneSessionManager::ReportWindowProfileInfos()
{
    enum class WindowVisibleState : int32_t {
        FOCUSBLE = 0,
        VISIBLE,
        MINIMIZED,
        OCCLUSION
    };
    std::map<int32_t, sptr<SceneSession>> sceneSessionMapCopy;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        sceneSessionMapCopy = sceneSessionMap_;
    }
    auto focusWindowId = GetFocusedSessionId();
    for (const auto& elem : sceneSessionMapCopy) {
        auto curSession = elem.second;
        if (curSession == nullptr || curSession->GetSessionInfo().isSystem_ ||
            curSession->GetWindowType() !=  WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            continue;
        }
        WindowProfileInfo windowProfileInfo;
        windowProfileInfo.bundleName = curSession->GetSessionInfo().bundleName_;
        windowProfileInfo.windowLocatedScreen = static_cast<int32_t>(
            curSession->GetSessionProperty()->GetDisplayId());
        windowProfileInfo.windowSceneMode = static_cast<int32_t>(curSession->GetWindowMode());
        if (focusWindowId == static_cast<int32_t>(curSession->GetWindowId())) {
            windowProfileInfo.windowVisibleState = static_cast<int32_t>(WindowVisibleState::FOCUSBLE);
        } else if (curSession->GetSessionState() == SessionState::STATE_BACKGROUND) {
            windowProfileInfo.windowVisibleState = static_cast<int32_t>(WindowVisibleState::MINIMIZED);
        } else if (!curSession->GetRSVisible()) {
            windowProfileInfo.windowVisibleState = static_cast<int32_t>(WindowVisibleState::OCCLUSION);
        } else {
            windowProfileInfo.windowVisibleState = static_cast<int32_t>(WindowVisibleState::VISIBLE);
        }
        WindowInfoReporter::GetInstance().ReportWindowProfileInfo(windowProfileInfo);
        WLOGFD("ReportWindowProfileInfo, bundleName:%{public}s, windowVisibleState:%{public}d, "
            "windowLocatedScreen:%{public}d, windowSceneMode:%{public}d",
            windowProfileInfo.bundleName.c_str(), windowProfileInfo.windowVisibleState,
            windowProfileInfo.windowLocatedScreen, windowProfileInfo.windowSceneMode);
    }
}

bool SceneSessionManager::IsVectorSame(const std::vector<VisibleWindowNumInfo>& lastInfo,
    const std::vector<VisibleWindowNumInfo>& currentInfo)
{
    if (lastInfo.size() != currentInfo.size()) {
        WLOGFE("last and current info is not Same");
        return false;
    }
    size_t sizeOfLastInfo = lastInfo.size();
    for (size_t i = 0; i < sizeOfLastInfo; i++) {
        if (lastInfo[i].displayId != currentInfo[i].displayId ||
            lastInfo[i].visibleWindowNum != currentInfo[i].visibleWindowNum) {
            WLOGFE("last and current visible window num is not Same");
            return false;
        }
    }
    return true;
}

void SceneSessionManager::CacVisibleWindowNum()
{
    std::map<int32_t, sptr<SceneSession>> sceneSessionMapCopy;
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        sceneSessionMapCopy = sceneSessionMap_;
    }
    std::vector<VisibleWindowNumInfo> visibleWindowNumInfo;
    bool isFullScreen = true;
    for (const auto& elem : sceneSessionMapCopy) {
        auto curSession = elem.second;
        if (curSession == nullptr) {
            continue;
        }
        bool isTargetWindow = (WindowHelper::IsMainWindow(curSession->GetWindowType()) ||
            curSession->GetWindowType() == WindowType::WINDOW_TYPE_WALLPAPER);
        if (!isTargetWindow || curSession->GetSessionState() == SessionState::STATE_BACKGROUND) {
            continue;
        }

        bool isWindowVisible = curSession->GetRSVisible();
        if (isWindowVisible) {
            auto windowMode = curSession->GetWindowMode();
            if (windowMode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
                windowMode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY ||
                windowMode == WindowMode::WINDOW_MODE_FLOATING || windowMode == WindowMode::WINDOW_MODE_PIP) {
                isFullScreen = false;
            }
            uint32_t displayId = static_cast<uint32_t>(curSession->GetSessionProperty()->GetDisplayId());
            auto it = std::find_if(visibleWindowNumInfo.begin(), visibleWindowNumInfo.end(),
                [=](const VisibleWindowNumInfo& info) {
                    return info.displayId == displayId;
            });
            if (it == visibleWindowNumInfo.end()) {
                visibleWindowNumInfo.push_back({displayId, 1});
            } else {
                it->visibleWindowNum++;
            }
        }
    }
    if (isFullScreen) {
        std::for_each(visibleWindowNumInfo.begin(), visibleWindowNumInfo.end(),
                      [](auto& info) { info.visibleWindowNum = 1; });
    }
    std::unique_lock<std::shared_mutex> lock(lastInfoMutex_);
    if (visibleWindowNumInfo.size() > 0 && !IsVectorSame(lastInfo_, visibleWindowNumInfo)) {
        SessionManagerAgentController::GetInstance().UpdateVisibleWindowNum(visibleWindowNumInfo);
        lastInfo_ = visibleWindowNumInfo;
    }
}

WSError SceneSessionManager::GetHostWindowRect(int32_t hostWindowId, Rect& rect)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "hostWindowId:%{public}d", hostWindowId);
    if (!SessionPermission::IsSystemCalling()) {
        TLOGE(WmsLogTag::WMS_UIEXT, "GetHostWindowRect permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    auto task = [this, hostWindowId, &rect]() {
        auto sceneSession = GetSceneSession(hostWindowId);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_UIEXT, "Session with persistentId %{public}d not found", hostWindowId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        WSRect hostRect = sceneSession->GetSessionRect();
        rect = {hostRect.posX_, hostRect.posY_, hostRect.width_, hostRect.height_};
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "GetHostWindowRect");
    return WSError::WS_OK;
}

std::shared_ptr<SkRegion> SceneSessionManager::GetDisplayRegion(DisplayId displayId)
{
    if (displayRegionMap_.find(displayId) != displayRegionMap_.end()) {
        return std::make_shared<SkRegion>(displayRegionMap_[displayId]->getBounds());
    }
    TLOGI(WmsLogTag::WMS_MAIN, "can not find display info from mem, sync dispslay region from dms.");
    auto display = SingletonContainer::Get<DisplayManager>().GetDisplayById(displayId);
    if (display == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "get display object failed of display: %{public}" PRIu64, displayId);
        return nullptr;
    }
    auto displayInfo = display->GetDisplayInfo();
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "get display info failed of display: %{public}" PRIu64, displayId);
        return nullptr;
    }
    int32_t displayWidth = displayInfo->GetWidth();
    int32_t displayHeight = displayInfo->GetHeight();
    if (displayWidth == 0 || displayHeight == 0) {
        TLOGE(WmsLogTag::WMS_MAIN, "invalid display size of display: %{public}" PRIu64, displayId);
        return nullptr;
    }

    SkIRect rect {.fLeft = 0, .fTop = 0, .fRight = displayWidth, .fBottom = displayHeight};
    auto region = std::make_shared<SkRegion>(rect);
    displayRegionMap_[displayId] = region;
    TLOGI(WmsLogTag::WMS_MAIN, "update display region to w = %{public}d, h = %{public}d", displayWidth, displayHeight);
    return std::make_shared<SkRegion>(rect);
}

void SceneSessionManager::UpdateDisplayRegion(const sptr<DisplayInfo>& displayInfo)
{
    if (displayInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "update display region failed, displayInfo is nullptr.");
        return;
    }
    auto displayId = displayInfo->GetDisplayId();
    int32_t displayWidth = displayInfo->GetWidth();
    int32_t displayHeight = displayInfo->GetHeight();
    if (displayWidth == 0 || displayHeight == 0) {
        TLOGE(WmsLogTag::WMS_MAIN, "invalid display size of display: %{public}" PRIu64, displayId);
        return;
    }
    SkIRect rect {.fLeft = 0, .fTop = 0, .fRight = displayWidth, .fBottom = displayHeight};
    auto region = std::make_shared<SkRegion>(rect);
    displayRegionMap_[displayId] = region;
    TLOGI(WmsLogTag::WMS_MAIN, "update display region to w = %{public}d, h = %{public}d", displayWidth, displayHeight);
}

void SceneSessionManager::GetAllSceneSessionForAccessibility(std::vector<sptr<SceneSession>>& sceneSessionList)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession == nullptr) {
            continue;
        }
        if (Session::IsScbCoreEnabled()) {
            if (!sceneSession->IsVisibleForAccessibility()) {
                continue;
            }
        } else {
            if (!sceneSession->IsVisibleForAccessibility() || !IsSessionVisible(sceneSession)) {
                continue;
            }
        }
        if (sceneSession->GetSessionInfo().bundleName_.find("SCBGestureBack") != std::string::npos ||
            sceneSession->GetSessionInfo().bundleName_.find("SCBGestureNavBar") != std::string::npos ||
            sceneSession->GetSessionInfo().bundleName_.find("SCBGestureTopBar") != std::string::npos) {
            continue;
        }
        sceneSessionList.push_back(sceneSession);
    }
}

void SceneSessionManager::FillAccessibilityInfo(std::vector<sptr<SceneSession>>& sceneSessionList,
    std::vector<sptr<AccessibilityWindowInfo>>& accessibilityInfo)
{
    for (const auto& sceneSession : sceneSessionList) {
        if (!FillWindowInfo(accessibilityInfo, sceneSession)) {
            TLOGW(WmsLogTag::WMS_MAIN, "fill accessibilityInfo failed");
        }
    }
}

void SceneSessionManager::FilterSceneSessionCovered(std::vector<sptr<SceneSession>>& sceneSessionList)
{
    std::sort(sceneSessionList.begin(), sceneSessionList.end(), [](sptr<SceneSession> a, sptr<SceneSession> b) {
        return a->GetZOrder() > b->GetZOrder();
    });
    std::vector<sptr<SceneSession>> result;
    std::unordered_map<DisplayId, std::shared_ptr<SkRegion>> unaccountedSpaceMap;
    for (const auto& sceneSession : sceneSessionList) {
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "invalid scene session");
            continue;
        }
        auto sessionProperty = sceneSession->GetSessionProperty();
        if (sessionProperty == nullptr) {
            TLOGE(WmsLogTag::WMS_MAIN, "get property of session: %{public}d", sceneSession->GetPersistentId());
            continue;
        }
        std::shared_ptr<SkRegion> unaccountedSpace = nullptr;
        auto displayId = sessionProperty->GetDisplayId();
        if (unaccountedSpaceMap.find(displayId) != unaccountedSpaceMap.end()) {
            unaccountedSpace = unaccountedSpaceMap[displayId];
        } else {
            unaccountedSpace = GetDisplayRegion(displayId);
            if (unaccountedSpace == nullptr) {
                TLOGE(WmsLogTag::WMS_MAIN, "get display region of display: %{public}" PRIu64, displayId);
                continue;
            }
            unaccountedSpaceMap[displayId] = unaccountedSpace;
        }
        WSRect wsRect = sceneSession->GetSessionRect();
        SkIRect windowBounds {.fLeft = wsRect.posX_, .fTop = wsRect.posY_,
                              .fRight = wsRect.posX_ + wsRect.width_, .fBottom = wsRect.posY_ + wsRect.height_};
        SkRegion windowRegion(windowBounds);
        if (unaccountedSpace->quickReject(windowRegion)) {
            TLOGD(WmsLogTag::WMS_MAIN, "quick reject: [l=%{public}d,t=%{public}d,r=%{public}d,b=%{public}d]",
                windowBounds.fLeft, windowBounds.fTop, windowBounds.fRight, windowBounds.fBottom);
            continue;
        }
        if (!unaccountedSpace->intersects(windowRegion)) {
            TLOGD(WmsLogTag::WMS_MAIN, "no intersects: [l=%{public}d,t=%{public}d,r=%{public}d,b=%{public}d]",
                windowBounds.fLeft, windowBounds.fTop, windowBounds.fRight, windowBounds.fBottom);
            continue;
        }
        result.push_back(sceneSession);
        unaccountedSpace->op(windowRegion, SkRegion::Op::kDifference_Op);
        if (unaccountedSpace->isEmpty()) {
            break;
        }
    }
    sceneSessionList = result;
}

void SceneSessionManager::NotifyAllAccessibilityInfo()
{
    if (isUserBackground_) {
        TLOGD(WmsLogTag::WMS_MULTI_USER, "The user is in the background");
        return;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SceneSessionManager::NotifyAllAccessibilityInfo");
    std::vector<sptr<SceneSession>> sceneSessionList;
    GetAllSceneSessionForAccessibility(sceneSessionList);
    FilterSceneSessionCovered(sceneSessionList);

    std::vector<sptr<AccessibilityWindowInfo>> accessibilityInfo;
    FillAccessibilityInfo(sceneSessionList, accessibilityInfo);

    for (const auto& item : accessibilityInfo) {
        TLOGD(WmsLogTag::WMS_MAIN, "notify accessibilityWindow wid = %{public}d, inWid = %{public}d, "
            "bundle=%{public}s,bounds=(x = %{public}d, y = %{public}d, w = %{public}d, h = %{public}d)",
            item->wid_, item->innerWid_, item->bundleName_.c_str(),
            item->windowRect_.posX_, item->windowRect_.posY_, item->windowRect_.width_, item->windowRect_.height_);
        for (const auto& rect : item->touchHotAreas_) {
            TLOGD(WmsLogTag::WMS_MAIN, "window touch hot areas rect[x=%{public}d,y=%{public}d,"
                "w=%{public}d,h=%{public}d]", rect.posX_, rect.posY_, rect.width_, rect.height_);
        }
    }

    SessionManagerAgentController::GetInstance().NotifyAccessibilityWindowInfo(accessibilityInfo,
        WindowUpdateType::WINDOW_UPDATE_ALL);
}

void SceneSessionManager::removeFailRecoveredSession()
{
    for (const auto& persistentId : failRecoveredPersistentIdSet_) {
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGE(WmsLogTag::WMS_RECOVER, "Session is nullptr, persistentId = %{public}d", persistentId);
            continue;
        }
        if (!sceneSession->IsRecovered()) {
            TLOGW(WmsLogTag::WMS_RECOVER, "not recovered session persistentId = %{public}d", persistentId);
            continue;
        }
        const auto &scnSessionInfo = SetAbilitySessionInfo(sceneSession);
        if (!scnSessionInfo) {
            TLOGW(WmsLogTag::WMS_RECOVER, "scnSessionInfo is nullptr, persistentId = %{public}d", persistentId);
            continue;
        }
        TLOGI(WmsLogTag::WMS_RECOVER, "remove recover failed persistentId = %{public}d", persistentId);
        sceneSession->NotifySessionExceptionInner(scnSessionInfo, true);
    }
    failRecoveredPersistentIdSet_.clear();
}

int32_t SceneSessionManager::ReclaimPurgeableCleanMem()
{
#ifdef MEMMGR_WINDOW_ENABLE
    return Memory::MemMgrClient::GetInstance().ReclaimPurgeableCleanMem();
#else
    return -1;
#endif
}

WindowStatus SceneSessionManager::GetWindowStatus(WindowMode mode, SessionState sessionState,
    const sptr<WindowSessionProperty>& property)
{
    auto windowStatus = WindowStatus::WINDOW_STATUS_UNDEFINED;
    if (property == nullptr) {
        return windowStatus;
    }
    if (mode == WindowMode::WINDOW_MODE_FLOATING) {
        windowStatus = WindowStatus::WINDOW_STATUS_FLOATING;
        if (property->GetMaximizeMode() == MaximizeMode::MODE_AVOID_SYSTEM_BAR) { // maximize floating
            windowStatus = WindowStatus::WINDOW_STATUS_MAXIMIZE;
        }
    } else if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        windowStatus = WindowStatus::WINDOW_STATUS_SPLITSCREEN;
    } else if (mode == WindowMode::WINDOW_MODE_FULLSCREEN) {
        windowStatus = WindowStatus::WINDOW_STATUS_FULLSCREEN;
    } else if (sessionState != SessionState::STATE_FOREGROUND && sessionState != SessionState::STATE_ACTIVE) {
        windowStatus = WindowStatus::WINDOW_STATUS_MINIMIZE;
    }
    return windowStatus;
}

WMError SceneSessionManager::GetCallingWindowWindowStatus(int32_t persistentId, WindowStatus& windowStatus)
{
    if (!SessionPermission::IsStartedByInputMethod()) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "permission is not allowed persistentId: %{public}d", persistentId);
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "scnSession is null, persistentId: %{public}d", persistentId);
        return WMError::WM_ERROR_NULLPTR;
    }

    TLOGD(WmsLogTag::WMS_KEYBOARD, "persistentId: %{public}d, windowType: %{public}d",
        persistentId, scnSession->GetWindowType());

    auto sessionProperty = scnSession->GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session property is null");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    uint32_t callingWindowId = sessionProperty->GetCallingSessionId();
    auto callingSession = GetSceneSession(callingWindowId);
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "callingsSession is null");
        callingSession = GetSceneSession(focusedSessionId_);
        if (callingSession == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "callingsSession obtained through focusedSession fail");
            return WMError::WM_ERROR_INVALID_WINDOW;
        }
    }
    if (callingSession->IsSystemSession()) {
        windowStatus = WindowStatus::WINDOW_STATUS_FULLSCREEN;
    } else {
        windowStatus = GetWindowStatus(callingSession->GetWindowMode(), callingSession->GetSessionState(),
            callingSession->GetSessionProperty());
    }
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Get WindowStatus persistentId: %{public}d windowstatus: %{public}d",
        persistentId, windowStatus);
    return WMError::WM_OK;
}

WMError SceneSessionManager::GetCallingWindowRect(int32_t persistentId, Rect& rect)
{
    if (!SessionPermission::IsStartedByInputMethod()) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "permission is not allowed persistentId: %{public}d", persistentId);
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "scnSession is null, persistentId: %{public}d", persistentId);
        return WMError::WM_ERROR_NULLPTR;
    }
    TLOGD(WmsLogTag::WMS_KEYBOARD, "persistentId: %{public}d, windowType: %{public}d",
        persistentId, scnSession->GetWindowType());
    auto sessionProperty = scnSession->GetSessionProperty();
    if (sessionProperty == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "session property is null");
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    uint32_t callingWindowId = sessionProperty->GetCallingSessionId();
    auto callingSession = GetSceneSession(callingWindowId);
    if (callingSession == nullptr) {
        TLOGI(WmsLogTag::WMS_KEYBOARD, "callingsSession is null");
        callingSession = GetSceneSession(focusedSessionId_);
        if (callingSession == nullptr) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "callingsSession obtained through focusedSession fail");
            return WMError::WM_ERROR_INVALID_WINDOW;
        }
    }
    WSRect sessionRect = callingSession->GetSessionRect();
    rect = {sessionRect.posX_, sessionRect.posY_, sessionRect.width_, sessionRect.height_};
    TLOGI(WmsLogTag::WMS_KEYBOARD, "Get Rect persistentId: %{public}d, x: %{public}d, y: %{public}d, "
        "height: %{public}u, width: %{public}u", persistentId, rect.posX_, rect.posY_, rect.width_, rect.height_);
    return WMError::WM_OK;
}

WMError SceneSessionManager::GetWindowModeType(WindowModeType& windowModeType)
{
    if (!SessionPermission::IsSACalling()) {
        WLOGFE("GetWindowModeType permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    windowModeType = CheckWindowModeType();
    return WMError::WM_OK;
}

WMError SceneSessionManager::GetWindowStyleType(WindowStyleType& windowStyleType)
{
    if (!SessionPermission::IsSACalling()) {
        TLOGE(WmsLogTag::WMS_LIFE, "permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    if (systemConfig_.IsPcWindow()) {
        windowStyleType = WindowStyleType::WINDOW_STYLE_FREE_MULTI_WINDOW;
        return WMError::WM_OK;
    }
    windowStyleType = systemConfig_.freeMultiWindowSupport_ && systemConfig_.freeMultiWindowEnable_ ?
        WindowStyleType::WINDOW_STYLE_FREE_MULTI_WINDOW : WindowStyleType::WINDOW_STYLE_DEFAULT;
    return WMError::WM_OK;
}

void SceneSessionManager::CheckSceneZOrder()
{
    auto task = [this]() {
        AnomalyDetection::SceneZOrderCheckProcess();
    };
    taskScheduler_->PostAsyncTask(task, "CheckSceneZOrder");
}

int32_t SceneSessionManager::GetCustomDecorHeight(int32_t persistentId)
{
    int32_t height = 0;
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Session with persistentId %{public}d not found", persistentId);
        return 0;
    }
    height = sceneSession->GetCustomDecorHeight();
    TLOGD(WmsLogTag::WMS_LAYOUT, "GetCustomDecorHeight: %{public}d", height);
    return height;
}

WMError SceneSessionManager::GetMainWindowInfos(int32_t topNum, std::vector<MainWindowInfo>& topNInfo)
{
    if (!(SessionPermission::IsSACalling() || SessionPermission::IsStartByHdcd())) {
        TLOGE(WmsLogTag::WMS_MAIN, "permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }

    if (!topNInfo.empty() || (topNum <= 0)) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    TLOGD(WmsLogTag::WMS_MAIN, "topNum: %{public}d", topNum);
    auto func = [this, &topNum, &topNInfo](sptr<SceneSession> session) {
        if (session == nullptr) {
            return false;
        }

        if (topNum == 0) {
            return true;
        }

        if (!WindowHelper::IsMainWindow(session->GetWindowType()) || !IsSessionVisibleForeground(session)) {
            TLOGD(WmsLogTag::WMS_MAIN, "not main window %{public}d", session->GetWindowType());
            return false;
        }

        MainWindowInfo info;
        info.pid_ = session->GetCallingPid();
        info.bundleName_ = session->GetSessionInfo().bundleName_;
        topNInfo.push_back(info);
        topNum--;
        TLOGE(WmsLogTag::WMS_MAIN, "topnNum: %{public}d, pid: %{public}d, bundleName: %{public}s",
            topNum, info.pid_, info.bundleName_.c_str());
        return false;
    };
    TraverseSessionTree(func, true);

    return WMError::WM_OK;
}

WSError SceneSessionManager::NotifyEnterRecentTask(bool enterRecent)
{
    TLOGI(WmsLogTag::WMS_IMMS, "enterRecent: %{public}u", enterRecent);
    enterRecent_.store(enterRecent);
    SetSystemAnimatedScenes(enterRecent ?
        SystemAnimatedSceneType::SCENE_ENTER_RECENTS : SystemAnimatedSceneType::SCENE_EXIT_RECENTS);
    return WSError::WS_OK;
}

WMError SceneSessionManager::GetAllMainWindowInfos(std::vector<MainWindowInfo>& infos) const
{
    if (!infos.empty()) {
        TLOGE(WmsLogTag::WMS_MAIN, "Input param invalid, infos must be empty.");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (!SessionPermission::IsSACalling() && !SessionPermission::IsShellCall()) {
        TLOGE(WmsLogTag::WMS_MAIN, "Get all mainWindow infos failed, only support SA calling.");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& iter : sceneSessionMap_) {
        auto& session = iter.second;
        if (session == nullptr || !WindowHelper::IsMainWindow(session->GetWindowType())) {
            continue;
        }
        MainWindowInfo info;
        auto abilityInfo = session->GetSessionInfo().abilityInfo;
        if (abilityInfo == nullptr) {
            TLOGW(WmsLogTag::WMS_MAIN, "Session id:%{public}d abilityInfo is null.", session->GetPersistentId());
            continue;
        }
        info.pid_ = session->GetCallingPid();
        info.bundleName_ = session->GetSessionInfo().bundleName_;
        info.persistentId_ = session->GetPersistentId();
        info.bundleType_ = static_cast<int32_t>(abilityInfo->applicationInfo.bundleType);
        TLOGD(WmsLogTag::WMS_MAIN, "Get mainWindow info, Session id:%{public}d, bundleName:%{public}s, "
            "bundleType:%{public}d", session->GetPersistentId(), info.bundleName_.c_str(), info.bundleType_);
        infos.push_back(info);
    }
    return WMError::WM_OK;
}

WMError SceneSessionManager::ClearMainSessions(const std::vector<int32_t>& persistentIds,
    std::vector<int32_t>& clearFailedIds)
{
    if (!SessionPermission::IsSACalling() && !SessionPermission::IsShellCall()) {
        TLOGE(WmsLogTag::WMS_MAIN, "Clear main sessions failed, only support SA calling.");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    if (!SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_MANAGE_MISSION)) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has not permission granted");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    clearFailedIds.clear();
    for (const auto persistentId : persistentIds) {
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            TLOGW(WmsLogTag::WMS_MAIN, "Session id:%{public}d is not found.", persistentId);
            clearFailedIds.push_back(persistentId);
            continue;
        }
        if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
            TLOGW(WmsLogTag::WMS_MAIN, "Session id:%{public}d is not mainWindow.", persistentId);
            clearFailedIds.push_back(persistentId);
            continue;
        }
        sceneSession->Clear();
        TLOGD(WmsLogTag::WMS_MAIN, "Clear succeed: session id:%{public}d.", persistentId);
    }
    return WMError::WM_OK;
}

WMError SceneSessionManager::UpdateDisplayHookInfo(int32_t uid, uint32_t width, uint32_t height, float_t density,
    bool enable)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "width: %{public}u, height: %{public}u, density: %{public}f, enable: %{public}d",
        width, height, density, enable);

    DMHookInfo dmHookInfo;
    dmHookInfo.width_ = width;
    dmHookInfo.height_ = height;
    dmHookInfo.density_ = density;
    dmHookInfo.rotation_ = 0;
    dmHookInfo.enableHookRotation_ = false;
    ScreenSessionManagerClient::GetInstance().UpdateDisplayHookInfo(uid, enable, dmHookInfo);
    return WMError::WM_OK;
}

WMError SceneSessionManager::UpdateAppHookDisplayInfo(int32_t uid, const HookInfo& hookInfo, bool enable)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "width: %{public}u, height: %{public}u, density: %{public}f, rotation: %{public}u, "
        "enableHookRotation: %{public}d, enable: %{public}d", hookInfo.width_, hookInfo.height_, hookInfo.density_,
        hookInfo.rotation_, hookInfo.enableHookRotation_, enable);

    DMHookInfo dmHookInfo;
    dmHookInfo.width_ = hookInfo.width_;
    dmHookInfo.height_ = hookInfo.height_;
    dmHookInfo.density_ = hookInfo.density_;
    dmHookInfo.rotation_ = hookInfo.rotation_;
    dmHookInfo.enableHookRotation_ = hookInfo.enableHookRotation_;
    ScreenSessionManagerClient::GetInstance().UpdateDisplayHookInfo(uid, enable, dmHookInfo);
    return WMError::WM_OK;
}

void DisplayChangeListener::OnScreenFoldStatusChanged(const std::vector<std::string>& screenFoldInfo)
{
    SceneSessionManager::GetInstance().ReportScreenFoldStatusChange(screenFoldInfo);
}

WMError SceneSessionManager::ReportScreenFoldStatusChange(const std::vector<std::string>& screenFoldInfo)
{
    ScreenFoldData screenFoldData;
    WMError ret = MakeScreenFoldData(screenFoldInfo, screenFoldData);
    if (ret != WMError::WM_OK) {
        return ret;
    }
    return CheckAndReportScreenFoldStatus(screenFoldData);
}

WMError SceneSessionManager::MakeScreenFoldData(const std::vector<std::string>& screenFoldInfo,
    ScreenFoldData& screenFoldData)
{
    if (screenFoldInfo.size() < ScreenFoldData::DMS_PARAM_NUMBER) {
        TLOGI(WmsLogTag::DMS, "Error: Init DMS param number is wrong.");
        return WMError::WM_DO_NOTHING;
    }

    screenFoldData.currentScreenFoldStatus_ = std::stoi(screenFoldInfo[0]); // 0: current screen fold status
    screenFoldData.nextScreenFoldStatus_ = std::stoi(screenFoldInfo[1]); // 1: next screen fold status
    screenFoldData.currentScreenFoldStatusDuration_ = std::stoi(screenFoldInfo[2]); // 2: current duration
    screenFoldData.postureAngle_ = std::atof(screenFoldInfo[3].c_str()); // 3: posture angle (type: float)
    screenFoldData.screenRotation_ = std::stoi(screenFoldInfo[4]); // 4: screen rotation
    if (!screenFoldData.GetTypeCThermalWithUtil()) {
        TLOGI(WmsLogTag::DMS, "Error: fail to get typeC thermal.");
        return WMError::WM_DO_NOTHING;
    }
    AppExecFwk::ElementName element = {};
    WSError ret = GetFocusSessionElement(element);
    if (ret != WSError::WS_OK) {
        TLOGI(WmsLogTag::DMS, "Error: fail to get focused package name.");
        return WMError::WM_DO_NOTHING;
    }
    screenFoldData.SetFocusedPkgName(element.GetURI());
    return WMError::WM_OK;
}

WMError SceneSessionManager::CheckAndReportScreenFoldStatus(ScreenFoldData& data)
{
    static ScreenFoldData lastScreenHalfFoldData;
    if (data.nextScreenFoldStatus_ == static_cast<int32_t>(FoldStatus::HALF_FOLD)) {
        lastScreenHalfFoldData = data;
        return WMError::WM_DO_NOTHING;
    }
    WMError lastScreenHalfFoldReportRet = WMError::WM_OK;
    if (data.currentScreenFoldStatus_ == static_cast<int32_t>(FoldStatus::HALF_FOLD)) {
        if (data.currentScreenFoldStatusDuration_ >= ScreenFoldData::HALF_FOLD_REPORT_TRIGGER_DURATION) {
            lastScreenHalfFoldReportRet = ReportScreenFoldStatus(lastScreenHalfFoldData);
        } else if (lastScreenHalfFoldData.currentScreenFoldStatus_ != ScreenFoldData::INVALID_VALUE) {
            // if stay at half-fold less than 15s, combine this change with last
            data.currentScreenFoldStatus_ = lastScreenHalfFoldData.currentScreenFoldStatus_;
            data.currentScreenFoldStatusDuration_ += lastScreenHalfFoldData.currentScreenFoldStatusDuration_;
            data.postureAngle_ = lastScreenHalfFoldData.postureAngle_;
        }
        lastScreenHalfFoldData.SetInvalid();
    }
    WMError currentScreenFoldStatusReportRet = ReportScreenFoldStatus(data);
    return (currentScreenFoldStatusReportRet == WMError::WM_OK) ? lastScreenHalfFoldReportRet :
        currentScreenFoldStatusReportRet;
}

// report screen_fold_status event when it changes to fold/expand or stays 15s at half-fold
WMError SceneSessionManager::ReportScreenFoldStatus(const ScreenFoldData& data)
{
    if (data.currentScreenFoldStatus_ == ScreenFoldData::INVALID_VALUE) {
        return WMError::WM_DO_NOTHING;
    }

    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FOLDSTATE_UE,
        "FOLDSCREEN_STATE_CHANGE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "PNAMEID", "OCCUPATION", "PVERSIONID", "OCCUPATION",
        "LASTFOLDSTATE", data.currentScreenFoldStatus_,
        "CURRENTFOLDSTATE", data.nextScreenFoldStatus_,
        "STATE", -1,
        "TIME", data.currentScreenFoldStatusDuration_,
        "ROTATION", data.screenRotation_,
        "PACKAGE", data.focusedPackageName_,
        "ANGLE", data.postureAngle_,
        "TYPECTHERMAL", data.typeCThermal_,
        "SCREENTHERMAL", -1,
        "SCANGLE", -1,
        "ISTENT", -1);
    if (ret != 0) {
        TLOGE(WmsLogTag::DMS, "Write HiSysEvent error, ret: %{public}d.", ret);
        return WMError::WM_DO_NOTHING;
    }
    return WMError::WM_OK;
}

void SceneSessionManager::UpdateSecSurfaceInfo(std::shared_ptr<RSUIExtensionData> secExtensionData, uint64_t userid)
{
    if (currentUserId_ != static_cast<int32_t>(userid)) {
        TLOGW(WmsLogTag::WMS_MULTI_USER, "currentUserId_:%{public}d userid:%{public}" PRIu64"", currentUserId_, userid);
        return;
    }
    auto secSurfaceInfoMap = secExtensionData->GetSecData();
    auto task = [secSurfaceInfoMap]()-> WSError {
        SceneInputManager::GetInstance().UpdateSecSurfaceInfo(secSurfaceInfoMap);
        return WSError::WS_OK;
    };
    taskScheduler_->PostAsyncTask(task, "UpdateSecSurfaceInfo");
}

void SceneSessionManager::RegisterSecSurfaceInfoListener()
{
    auto callBack = [this](std::shared_ptr<RSUIExtensionData> secExtensionData, uint64_t userid) {
        this->UpdateSecSurfaceInfo(secExtensionData, userid);
    };
    TLOGI(WmsLogTag::WMS_EVENT, "RegisterSecSurfaceInfoListener");
    if (rsInterface_.RegisterUIExtensionCallback(currentUserId_, callBack) != WM_OK) {
        TLOGE(WmsLogTag::WMS_EVENT, "RegisterSecSurfaceInfoListener failed");
    }
}

WSError SceneSessionManager::SetAppForceLandscapeConfig(const std::string& bundleName,
    const AppForceLandscapeConfig& config)
{
    if (bundleName.empty()) {
        TLOGE(WmsLogTag::DEFAULT, "bundle name is empty");
        return WSError::WS_ERROR_NULLPTR;
    }
    std::unique_lock<std::shared_mutex> lock(appForceLandscapeMutex_);
    appForceLandscapeMap_[bundleName] = config;
    TLOGI(WmsLogTag::DEFAULT, "app: %{public}s, mode: %{public}d, homePage: %{public}s",
        bundleName.c_str(), config.mode_, config.homePage_.c_str());
    return WSError::WS_OK;
}

AppForceLandscapeConfig SceneSessionManager::GetAppForceLandscapeConfig(const std::string& bundleName)
{
    if (bundleName.empty()) {
        return {};
    }
    std::shared_lock<std::shared_mutex> lock(appForceLandscapeMutex_);
    if (appForceLandscapeMap_.empty() ||
        appForceLandscapeMap_.find(bundleName) == appForceLandscapeMap_.end()) {
        TLOGD(WmsLogTag::DEFAULT, "app: %{public}s, config not find", bundleName.c_str());
        return {};
    }
    return appForceLandscapeMap_[bundleName];
}

WMError SceneSessionManager::TerminateSessionByPersistentId(int32_t persistentId)
{
    if (!SessionPermission::VerifyCallingPermission(PermissionConstants::PERMISSION_KILL_APP_PROCESS) ||
        !SessionPermission::IsSystemAppCall()) {
        TLOGE(WmsLogTag::WMS_LIFE, "The caller has no permission granted.");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "Session id:%{public}d is not found.", persistentId);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    if (!WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
        TLOGE(WmsLogTag::WMS_MAIN, "Session id:%{public}d is not mainWindow.", persistentId);
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    sceneSession->Clear(true);
    TLOGI(WmsLogTag::WMS_LIFE, "Terminate success, id:%{public}d.", persistentId);
    return WMError::WM_OK;
}

void SceneSessionManager::SetRootSceneProcessBackEventFunc(const RootSceneProcessBackEventFunc& processBackEventFunc)
{
    rootSceneProcessBackEventFunc_ = processBackEventFunc;
    TLOGI(WmsLogTag::WMS_EVENT, "called");
}

WMError SceneSessionManager::GetProcessSurfaceNodeIdByPersistentId(const int32_t pid,
    const std::vector<int32_t>& persistentIds, std::vector<uint64_t>& surfaceNodeIds)
{
    if (!SessionPermission::IsSACalling() && !SessionPermission::IsShellCall()) {
        TLOGE(WmsLogTag::DEFAULT, "The caller has no permission granted.");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }

    surfaceNodeIds.clear();
    TLOGI(WmsLogTag::DEFAULT, "Get process surfaceNodeId by persistentId, pid:%{public}d", pid);
    for (auto persistentId : persistentIds) {
        TLOGI(WmsLogTag::DEFAULT, "convert wid:%{public}d", persistentId);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            continue;
        }
        auto callingPid = sceneSession->GetCallingPid();
        auto surfaceNode = sceneSession->GetSurfaceNode();
        if (surfaceNode != nullptr && callingPid == pid) {
            surfaceNodeIds.push_back(surfaceNode->GetId());
            auto leashWinSurfaceNode = sceneSession->GetLeashWinSurfaceNode();
            if (leashWinSurfaceNode != nullptr) {
                surfaceNodeIds.push_back(leashWinSurfaceNode->GetId());
            }
        }
    }

    return WMError::WM_OK;
}

void SceneSessionManager::RefreshPcZOrderList(uint32_t startZOrder, std::vector<int32_t>&& persistentIds)
{
    auto task = [this, startZOrder, persistentIds = std::move(persistentIds)] {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < persistentIds.size(); i++) {
            int32_t persistentId = persistentIds[i];
            oss << persistentId;
            if (i < persistentIds.size() - 1) {
                oss << ",";
            }
            auto sceneSession = GetSceneSession(persistentId);
            if (sceneSession == nullptr) {
                TLOGE(WmsLogTag::WMS_LAYOUT, "sceneSession is nullptr persistentId = %{public}d", persistentId);
                continue;
            }
            if (i > UINT32_MAX - startZOrder) {
                TLOGE(WmsLogTag::WMS_LAYOUT, "Z order overflow, stop refresh");
                break;
            }
            sceneSession->SetPcScenePanel(true);
            sceneSession->SetZOrder(i + startZOrder);
        }
        oss << "]";
        TLOGI(WmsLogTag::WMS_LAYOUT, "RefreshPcZOrderList:%{public}s", oss.str().c_str());
        return WSError::WS_OK;
    };
    taskScheduler_->PostTask(task, "RefreshPcZOrderList");
}

void SceneSessionManager::SetCloseTargetFloatWindowFunc(const ProcessCloseTargetFloatWindowFunc& func)
{
    TLOGD(WmsLogTag::WMS_MULTI_WINDOW, "in");
    closeTargetFloatWindowFunc_ = func;
}

WMError SceneSessionManager::CloseTargetFloatWindow(const std::string& bundleName)
{
    if (!SessionPermission::IsSystemServiceCalling(false)) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "failed, not system service called.");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, bundleName] {
        if (closeTargetFloatWindowFunc_) {
            TLOGI(WmsLogTag::WMS_MULTI_WINDOW, "bundleName:%{public}s", bundleName.c_str());
            closeTargetFloatWindowFunc_(bundleName);
        }
    };
    taskScheduler_->PostTask(task, "CloseTargetFloatWindow");
    return WMError::WM_OK;
}

void SceneSessionManager::UpdatePiPWindowStateChanged(const std::string& bundleName, bool isForeground)
{
    SessionManagerAgentController::GetInstance().UpdatePiPWindowStateChanged(bundleName, isForeground);
}

WMError SceneSessionManager::CloseTargetPiPWindow(const std::string& bundleName)
{
    if (!SessionPermission::IsSystemServiceCalling(false)) {
        TLOGE(WmsLogTag::WMS_PIP, "failed, not system service called.");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& iter: sceneSessionMap_) {
        auto& session = iter.second;
        if (session && session->GetWindowType() == WindowType::WINDOW_TYPE_PIP &&
            session->GetSessionInfo().bundleName_ == bundleName) {
            session->NotifyCloseExistPipWindow();
            break;
        }
    }
    return WMError::WM_OK;
}

WMError SceneSessionManager::GetCurrentPiPWindowInfo(std::string& bundleName)
{
    if (!SessionPermission::IsSystemServiceCalling(false)) {
        TLOGE(WmsLogTag::WMS_PIP, "failed, not system service called.");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& iter: sceneSessionMap_) {
        auto& session = iter.second;
        if (session && session->GetWindowType() == WindowType::WINDOW_TYPE_PIP) {
            bundleName = session->GetSessionInfo().bundleName_;
            return WMError::WM_OK;
        }
    }
    TLOGW(WmsLogTag::WMS_PIP, "no PiP window");
    return WMError::WM_OK;
}

WMError SceneSessionManager::SkipSnapshotForAppProcess(int32_t pid, bool skip)
{
    if (!SessionPermission::IsSACalling() && !SessionPermission::IsShellCall()) {
        TLOGE(WmsLogTag::DEFAULT, "permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    TLOGI(WmsLogTag::WMS_LIFE, "pid:%{public}d, skip:%{public}u", pid, skip);
    auto task = [this, pid, skip] {
        if (skip) {
            snapshotSkipPidSet_.insert(pid);
        } else {
            snapshotSkipPidSet_.erase(pid);
        }
        std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& [persistentId, sceneSession] : sceneSessionMap_) {
            if (sceneSession == nullptr) {
                continue;
            }
            if (pid == sceneSession->GetCallingPid()) {
                TLOGI(WmsLogTag::DEFAULT, "send rs set snapshot skip, persistentId:%{public}d, skip:%{public}u",
                    persistentId, skip);
                sceneSession->SetSnapshotSkip(skip);
            }
        }
    };
    taskScheduler_->PostTask(task, "SkipSnapshotForAppProcess");
    return WMError::WM_OK;
}

void SceneSessionManager::RemoveProcessSnapshotSkip(int32_t pid)
{
    if (snapshotSkipPidSet_.erase(pid) != 0) {
        TLOGI(WmsLogTag::DEFAULT, "process died, delete pid from snapshot skip pid set. pid:%{public}d", pid);
    }
}

void SceneSessionManager::SetSessionSnapshotSkipForAppProcess(const sptr<SceneSession>& sceneSession)
{
    auto callingPid = sceneSession->GetCallingPid();
    if (snapshotSkipPidSet_.find(callingPid) != snapshotSkipPidSet_.end()) {
        sceneSession->SetSnapshotSkip(true);
    }
}

WMError SceneSessionManager::SetSnapshotSkipByUserIdAndBundleNameList(const int32_t userId,
    const std::vector<std::string>& bundleNameList)
{
    if (!SessionPermission::IsSACalling() && !SessionPermission::IsShellCall()) {
        TLOGE(WmsLogTag::DEFAULT, "The caller has no permission granted.");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    TLOGI(WmsLogTag::DEFAULT, "userId:%{public}d", userId);
    auto task = [this, userId, bundleNameList] {
        snapshotBundleNameSet_.clear();
        for (auto& bundleName : bundleNameList) {
            snapshotBundleNameSet_.insert(bundleName);
        }
        std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& [persistentId, sceneSession] : sceneSessionMap_) {
            if (sceneSession == nullptr) {
                continue;
            }
            std::string name = sceneSession->GetSessionInfo().bundleName_;
            if (snapshotBundleNameSet_.find(name) != snapshotBundleNameSet_.end()) {
                TLOGI(WmsLogTag::DEFAULT, "set RS snapshot skip true, name:%{public}s",
                    name.c_str());
                sceneSession->SetSnapshotSkip(true);
                continue;
            }
            sceneSession->SetSnapshotSkip(false);
        }
    };
    taskScheduler_->PostTask(task, "SetSnapshotSkipByUserIdAndBundleNameList");
    return WMError::WM_OK;
}

void SceneSessionManager::SetSessionSnapshotSkipForAppBundleName(const sptr<SceneSession>& sceneSession)
{
    std::string name = sceneSession->GetSessionInfo().bundleName_;
    if (snapshotBundleNameSet_.find(name) != snapshotBundleNameSet_.end()) {
        TLOGI(WmsLogTag::DEFAULT, "new session set RS snapshot skip true, name:%{public}s",
                name.c_str());
        sceneSession->SetSnapshotSkip(true);
    }
}

WMError SceneSessionManager::SetProcessWatermark(int32_t pid, const std::string& watermarkName, bool isEnabled)
{
    if (!SessionPermission::IsSACalling() && !SessionPermission::IsShellCall()) {
        TLOGE(WmsLogTag::DEFAULT, "permission denied!");
        return WMError::WM_ERROR_INVALID_PERMISSION;
    }
    TLOGI(WmsLogTag::DEFAULT, "pid:%{public}d, watermarkName:%{public}s, isEnabled:%{public}u",
        pid, watermarkName.c_str(), isEnabled);
    if (isEnabled && watermarkName.empty()) {
        TLOGE(WmsLogTag::DEFAULT, "watermarkName is empty!");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    auto task = [this, pid, watermarkName, isEnabled] {
        if (isEnabled) {
            processWatermarkPidMap_.insert_or_assign(pid, watermarkName);
        } else {
            processWatermarkPidMap_.erase(pid);
        }

        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& [_, sceneSession] : sceneSessionMap_) {
            if (sceneSession == nullptr) {
                continue;
            }
            if (pid == sceneSession->GetCallingPid()) {
                sceneSession->SetWatermarkEnabled(watermarkName, isEnabled);
            }
        }
    };
    taskScheduler_->PostTask(task, __func__);
    return WMError::WM_OK;
}

void SceneSessionManager::SetSessionWatermarkForAppProcess(const sptr<SceneSession>& sceneSession)
{
    if (auto iter = processWatermarkPidMap_.find(sceneSession->GetCallingPid());
        iter != processWatermarkPidMap_.end()) {
        sceneSession->SetWatermarkEnabled(iter->second, true);
    }
}

void SceneSessionManager::RemoveProcessWatermarkPid(int32_t pid)
{
    if (processWatermarkPidMap_.find(pid) != processWatermarkPidMap_.end()) {
        TLOGI(WmsLogTag::DEFAULT, "process died, delete pid from watermark pid map. pid:%{public}d", pid);
        processWatermarkPidMap_.erase(pid);
    }
}

int32_t SceneSessionManager::GetMaxInstanceCount(const std::string& bundleName)
{
    return MultiInstanceManager::GetInstance().GetMaxInstanceCount(bundleName);
}

int32_t SceneSessionManager::GetInstanceCount(const std::string& bundleName)
{
    return MultiInstanceManager::GetInstance().GetInstanceCount(bundleName);
}

std::string SceneSessionManager::GetLastInstanceKey(const std::string& bundleName)
{
    return MultiInstanceManager::GetInstance().GetLastInstanceKey(bundleName);
}

void SceneSessionManager::PackageRemovedOrChanged(const std::string& bundleName)
{
    return MultiInstanceManager::GetInstance().RemoveAppInfo(bundleName);
}
} // namespace OHOS::Rosen
