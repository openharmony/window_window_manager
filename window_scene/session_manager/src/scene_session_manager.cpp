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

#include <cinttypes>
#include <csignal>
#include <cstdint>
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <unistd.h>

#include <ability_context.h>
#include <ability_info.h>
#include <ability_manager_client.h>
#include <bundle_mgr_interface.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <hisysevent.h>
#include <parameters.h>
#include "parameter.h"
#include <pointer_event.h>
#include <resource_manager.h>
#include <running_lock.h>
#include <session_info.h>
#include <start_options.h>
#include <system_ability_definition.h>
#include <want.h>
#include <hitrace_meter.h>
#include <transaction/rs_interfaces.h>
#include <transaction/rs_transaction.h>
#include "transaction/rs_sync_transaction_controller.h"

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

#include "ability_start_setting.h"
#include "anr_manager.h"
#include "color_parser.h"
#include "common/include/session_permission.h"
#include "display_manager.h"
#include "interfaces/include/ws_common.h"
#include "interfaces/include/ws_common_inner.h"
#include "session/host/include/main_session.h"
#include "session/host/include/scb_system_session.h"
#include "session/host/include/scene_persistent_storage.h"
#include "session/host/include/session_utils.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/system_session.h"
#include "session_helper.h"
#include "window_helper.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "singleton_container.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_math.h"
#include "xcollie/watchdog.h"
#include "zidl/window_manager_agent_interface.h"
#include "session_manager_agent_controller.h"
#include "distributed_client.h"
#include "softbus_bus_center.h"
#include "window_manager.h"
#include "perform_reporter.h"
#include "pip_util.h"
#include "focus_change_info.h"
#include "anr_manager.h"

#include "window_visibility_info.h"
#include "window_drawing_content_info.h"
#ifdef MEMMGR_WINDOW_ENABLE
#include "mem_mgr_client.h"
#include "mem_mgr_window_info.h"
#endif

#ifdef EFFICIENCY_MANAGER_ENABLE
#include "suspend_manager_client.h"
#endif // EFFICIENCY_MANAGER_ENABLE

#ifdef SECURITY_COMPONENT_MANAGER_ENABLE
#include "sec_comp_enhance_kit.h"
#endif

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManager" };
#ifdef RES_SCHED_ENABLE
const std::string SCENE_BOARD_BUNDLE_NAME = "com.ohos.sceneboard";
#endif
const std::string SCENE_SESSION_MANAGER_THREAD = "OS_SceneSessionManager";
const std::string WINDOW_INFO_REPORT_THREAD = "OS_WindowInfoReportThread";
constexpr const char* PREPARE_TERMINATE_ENABLE_PARAMETER = "persist.sys.prepare_terminate";
std::recursive_mutex g_instanceMutex;
constexpr uint32_t MAX_BRIGHTNESS = 255;
constexpr int32_t PREPARE_TERMINATE_ENABLE_SIZE = 6;
constexpr int32_t DEFAULT_USERID = -1;
constexpr int32_t SCALE_DIMENSION = 2;
constexpr int32_t TRANSLATE_DIMENSION = 2;
constexpr int32_t ROTAION_DIMENSION = 4;
constexpr int32_t CUBIC_CURVE_DIMENSION = 4;
const std::string DM_PKG_NAME = "ohos.distributedhardware.devicemanager";
constexpr int32_t NON_ANONYMIZE_LENGTH = 6;
const std::string EMPTY_DEVICE_ID = "";
const int32_t MAX_NUMBER_OF_DISTRIBUTED_SESSIONS = 20;

constexpr int WINDOW_NAME_MAX_WIDTH = 21;
constexpr int DISPLAY_NAME_MAX_WIDTH = 10;
constexpr int VALUE_MAX_WIDTH = 5;
constexpr int ORIEN_MAX_WIDTH = 12;
constexpr int OFFSET_MAX_WIDTH = 10;
constexpr int PID_MAX_WIDTH = 8;
constexpr int PARENT_ID_MAX_WIDTH = 6;
constexpr int SCALE_MAX_WIDTH = 10;
constexpr int WINDOW_NAME_MAX_LENGTH = 20;
constexpr int32_t STATUS_BAR_AVOID_AREA = 0;
const std::string ARG_DUMP_ALL = "-a";
const std::string ARG_DUMP_WINDOW = "-w";
const std::string ARG_DUMP_SCREEN = "-s";
const std::string ARG_DUMP_DISPLAY = "-d";
constexpr uint64_t NANO_SECOND_PER_SEC = 1000000000; // ns
std::string GetCurrentTime()
{
    struct timespec tn;
    clock_gettime(CLOCK_REALTIME, &tn);
    uint64_t uTime = static_cast<uint64_t>(tn.tv_sec) * NANO_SECOND_PER_SEC +
        static_cast<uint64_t>(tn.tv_nsec);
    return std::to_string(uTime);
}
int Comp(const std::pair<uint64_t, WindowVisibilityState> &a, const std::pair<uint64_t, WindowVisibilityState> &b)
{
    return a.first < b.first;
}
} // namespace

SceneSessionManager& SceneSessionManager::GetInstance()
{
    std::lock_guard<std::recursive_mutex> lock(g_instanceMutex);
    static SceneSessionManager* instance = nullptr;
    if (instance == nullptr) {
        instance = new SceneSessionManager();
        instance->Init();
    }
    return *instance;
}

SceneSessionManager::SceneSessionManager() : rsInterface_(RSInterfaces::GetInstance())
{
    taskScheduler_ = std::make_shared<TaskScheduler>(SCENE_SESSION_MANAGER_THREAD);
    currentUserId_ = DEFAULT_USERID;
}

void SceneSessionManager::Init()
{
    constexpr uint64_t interval = 5 * 1000; // 5 second
    auto mainEventRunner = AppExecFwk::EventRunner::GetMainEventRunner();
    auto mainEventHandler = std::make_shared<AppExecFwk::EventHandler>(mainEventRunner);
    if (HiviewDFX::Watchdog::GetInstance().AddThread("MainThread", mainEventHandler, interval)) {
        WLOGFW("Add thread MainThread to watchdog failed.");
    }
    if (HiviewDFX::Watchdog::GetInstance().AddThread(
        SCENE_SESSION_MANAGER_THREAD, taskScheduler_->GetEventHandler(), interval)) {
        WLOGFW("Add thread %{public}s to watchdog failed.", SCENE_SESSION_MANAGER_THREAD.c_str());
    }

#ifdef RES_SCHED_ENABLE
    std::unordered_map<std::string, std::string> payload {
        { "pid", std::to_string(getpid()) },
        { "tid", std::to_string(gettid()) },
        { "uid", std::to_string(getuid()) },
        { "bundleName", SCENE_BOARD_BUNDLE_NAME },
    };
    uint32_t type = OHOS::ResourceSchedule::ResType::RES_TYPE_REPORT_SCENE_BOARD;
    int64_t value = 0;
    OHOS::ResourceSchedule::ResSchedClient::GetInstance().ReportData(type, value, payload);
#endif

    bundleMgr_ = GetBundleManager();
    LoadWindowSceneXml();
    sptr<IDisplayChangeListener> listener = new DisplayChangeListener();
    ScreenSessionManagerClient::GetInstance().RegisterDisplayChangeListener(listener);
    InitPrepareTerminateConfig();

    // create handler for inner command at server
    eventLoop_ = AppExecFwk::EventRunner::Create(WINDOW_INFO_REPORT_THREAD);
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
    if (eventHandler_ == nullptr) {
        WLOGFE("Invalid eventHander");
        return ;
    }
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread(WINDOW_INFO_REPORT_THREAD, eventHandler_);
    if (ret != 0) {
        WLOGFW("Add thread %{public}s to watchdog failed.", WINDOW_INFO_REPORT_THREAD.c_str());
    }

    listenerController_ = std::make_shared<SessionListenerController>();
    listenerController_->Init();
    scbSessionHandler_ = new ScbSessionHandler();
    AAFwk::AbilityManagerClient::GetInstance()->RegisterSessionHandler(scbSessionHandler_);
    StartWindowInfoReportLoop();
    WLOGI("SceneSessionManager init success.");
    RegisterAppListener();
}

void SceneSessionManager::RegisterAppListener()
{
    appAnrListener_ = new (std::nothrow) AppAnrListener();
    auto appMgrClient_ = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance();
    if (appMgrClient_ == nullptr) {
        WLOGFE("appMgrClient_ is nullptr.");
    } else {
        auto flag = static_cast<int32_t>(appMgrClient_->RegisterAppDebugListener(appAnrListener_));
        if (flag != ERR_OK) {
            WLOGFE("Register app debug listener failed.");
        } else {
            WLOGFI("Register app debug listener success.");
        }
    }
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
    ConfigDefaultKeyboardAnimation();
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
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1 && numbers[0] == 1) {
            systemConfig_.backgroundswitch = true;
        }
    }
    WLOGFD("Load ConfigWindowSceneXml backgroundswitch%{public}d", systemConfig_.backgroundswitch);

    item = config["defaultWindowMode"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1 &&
            (numbers[0] == static_cast<int32_t>(WindowMode::WINDOW_MODE_FULLSCREEN) ||
             numbers[0] == static_cast<int32_t>(WindowMode::WINDOW_MODE_FLOATING))) {
            systemConfig_.defaultWindowMode_ = static_cast<WindowMode>(static_cast<uint32_t>(numbers[0]));
        }
    }

    item = config["defaultMaximizeMode"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1 &&
            (numbers[0] == static_cast<int32_t>(MaximizeMode::MODE_AVOID_SYSTEM_BAR) ||
            numbers[0] == static_cast<int32_t>(MaximizeMode::MODE_FULL_FILL))) {
            SceneSession::maximizeMode_ = static_cast<MaximizeMode>(numbers[0]);
        }
    }

    item = config["keyboardAnimation"];
    if (item.IsMap()) {
        ConfigKeyboardAnimation(item);
    }

    item = config["maxFloatingWindowSize"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) {
            systemConfig_.maxFloatingWindowSize_ = static_cast<uint32_t>(numbers[0]);
        }
    }

    item = config["windowAnimation"];
    if (item.IsMap()) {
        ConfigWindowAnimation(item);
    }

    item = config["startWindowTransitionAnimation"];
    if (item.IsMap()) {
        ConfigStartingWindowAnimation(item);
    }

    ConfigWindowSizeLimits();
    ConfigSnapshotScale();
}

WSError SceneSessionManager::SetSessionContinueState(const sptr<IRemoteObject> &token,
    const ContinueState& continueState)
{
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    WLOGFI("run SetSessionContinueState");
    auto task = [this, token, continueState]() {
        sptr <SceneSession> sceneSession = FindSessionByToken(token);
        if (sceneSession == nullptr) {
            WLOGFI("fail to find session by token.");
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        sceneSession->SetSessionInfoContinueState(continueState);
        DistributedClient dmsClient;
        dmsClient.SetMissionContinueState(sceneSession->GetPersistentId(),
            static_cast<AAFwk::ContinueState>(continueState));
        WLOGFI("SetSessionContinueState sessionId:%{public}d, continueState:%{public}d",
            sceneSession->GetPersistentId(), continueState);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetSessionContinueState");
}

void SceneSessionManager::ConfigDecor(const WindowSceneConfig::ConfigItem& decorConfig)
{
    WindowSceneConfig::ConfigItem item = decorConfig.GetProp("enable");
    if (item.IsBool()) {
        systemConfig_.isSystemDecorEnable_ = item.boolValue_;
        std::vector<std::string> supportedModes;
        item = decorConfig["supportedMode"];
        if (item.IsStrings()) {
            systemConfig_.decorModeSupportInfo_ = 0;
            supportedModes = *item.stringsValue_;
        }
        for (auto mode : supportedModes) {
            if (mode == "fullscreen") {
                systemConfig_.decorModeSupportInfo_ |= WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN;
            } else if (mode == "floating") {
                systemConfig_.decorModeSupportInfo_ |= WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING;
            } else if (mode == "pip") {
                systemConfig_.decorModeSupportInfo_ |= WindowModeSupport::WINDOW_MODE_SUPPORT_PIP;
            } else if (mode == "split") {
                systemConfig_.decorModeSupportInfo_ |= WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
                    WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY;
            } else {
                WLOGFW("Invalid supporedMode");
                systemConfig_.decorModeSupportInfo_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
                break;
            }
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
    WLOGFI("[RECOVER]Set enable input event: %{public}u", enabled);
    enableInputEvent_ = enabled;
}

bool SceneSessionManager::IsInputEventEnabled()
{
    return enableInputEvent_;
}

void SceneSessionManager::UpdateRecoveredSessionInfo(const std::vector<int32_t>& recoveredPersistentIds)
{
    WLOGFI("[RECOVER]Number of persistentIds recovered = %{public}zu. CurrentUserId = "
           "%{public}d",
        recoveredPersistentIds.size(), currentUserId_);
    std::vector<AAFwk::SessionInfo> abilitySessionInfos;

    for (auto i = 0; i < static_cast<int32_t>(recoveredPersistentIds.size()); i++) {
        auto search = sceneSessionMap_.find(recoveredPersistentIds.at(i));
        if (search == sceneSessionMap_.end() || search->second == nullptr) {
            continue;
        }
        WLOGFD("[RECOVER]recovered persistentId = %{public}d", search->first);
        auto sceneSession = search->second;
        auto abilitySessionInfo = SetAbilitySessionInfo(sceneSession);
        if (!abilitySessionInfo) {
            WLOGFW("[RECOVER]abilitySessionInfo is null");
            return;
        }
        abilitySessionInfos.emplace_back(*abilitySessionInfo);
    }
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

void SceneSessionManager::ConfigKeyboardAnimation(const WindowSceneConfig::ConfigItem& animationConfig)
{
    LoadKeyboardAnimation(animationConfig["animationIn"]["timing"], appWindowSceneConfig_.keyboardAnimationIn_);
    LoadKeyboardAnimation(animationConfig["animationOut"]["timing"], appWindowSceneConfig_.keyboardAnimationOut_);

    const auto& defaultAnimation = appWindowSceneConfig_.keyboardAnimationIn_;
    systemConfig_.keyboardAnimationConfig_.curveType_ = defaultAnimation.curveType_;
    systemConfig_.keyboardAnimationConfig_.curveParams_.assign({
        defaultAnimation.ctrlX1_,
        defaultAnimation.ctrlY1_,
        defaultAnimation.ctrlX2_,
        defaultAnimation.ctrlY2_,
    });
    systemConfig_.keyboardAnimationConfig_.durationIn_ = appWindowSceneConfig_.keyboardAnimationIn_.duration_;
    systemConfig_.keyboardAnimationConfig_.durationOut_ = appWindowSceneConfig_.keyboardAnimationOut_.duration_;
}

void SceneSessionManager::LoadKeyboardAnimation(const WindowSceneConfig::ConfigItem& item,
    KeyboardSceneAnimationConfig& config)
{
    if (item.IsMap() && item.mapValue_->count("curve")) {
        const auto& [curveType, curveParams] = CreateCurve(item["curve"]);
        config.curveType_ = curveType;
        if (curveParams.size() == CUBIC_CURVE_DIMENSION) {
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

void SceneSessionManager::ConfigDefaultKeyboardAnimation()
{
    constexpr char CURVETYPE[] = "interpolatingSpring";
    constexpr float CTRLX1 = 0;
    constexpr float CTRLY1 = 1;
    constexpr float CTRLX2 = 342;
    constexpr float CTRLY2 = 37;
    constexpr uint32_t DURATION = 150;

    if (!systemConfig_.keyboardAnimationConfig_.curveType_.empty()) {
        return;
    }

    systemConfig_.keyboardAnimationConfig_.curveType_ = CURVETYPE;
    std::vector<float> keyboardCurveParams = {CTRLX1, CTRLY1, CTRLX2, CTRLY2};
    systemConfig_.keyboardAnimationConfig_.curveParams_.assign(
        keyboardCurveParams.begin(), keyboardCurveParams.end());;
    systemConfig_.keyboardAnimationConfig_.durationIn_ = DURATION;
    systemConfig_.keyboardAnimationConfig_.durationOut_ = DURATION;
}

void SceneSessionManager::ConfigWindowAnimation(const WindowSceneConfig::ConfigItem& windowAnimationConfig)
{
    WindowSceneConfig::ConfigItem item = windowAnimationConfig["timing"];
    if (item.IsMap() && item.mapValue_->count("curve")) {
        const auto& [curveType, curveParams] = CreateCurve(item["curve"]);
        appWindowSceneConfig_.windowAnimation_.curveType_ = curveType;
        if (curveParams.size() == CUBIC_CURVE_DIMENSION) {
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

    std::string curveName = "easeOut";
    const auto& nameItem = curveConfig.GetProp("name");
    if (!nameItem.IsString()) {
        return {curveName, {}};
    }

    std::string name = nameItem.stringValue_;
    std::vector<float> curveParams;

    if (name == "cubic" && curveConfig.IsFloats() && curveConfig.floatsValue_->size() == CUBIC_CURVE_DIMENSION) {
        curveName = name;
        curveParams = *curveConfig.floatsValue_;
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

void SceneSessionManager::SetRootSceneContext(const std::weak_ptr<AbilityRuntime::Context>& contextWeak)
{
    rootSceneContextWeak_ = contextWeak;
}

sptr<RootSceneSession> SceneSessionManager::GetRootSceneSession()
{
    auto task = [this]() -> sptr<RootSceneSession> {
        if (rootSceneSession_ != nullptr) {
            return rootSceneSession_;
        }
        system::SetParameter("bootevent.wms.fullscreen.ready", "true");
        rootSceneSession_ = new RootSceneSession();
        rootSceneSession_->SetEventHandler(taskScheduler_->GetEventHandler());
        AAFwk::AbilityManagerClient::GetInstance()->SetRootSceneSession(rootSceneSession_->AsObject());
        return rootSceneSession_;
    };

    return taskScheduler_->PostSyncTask(task, "GetRootSceneSession");
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

sptr<SceneSession> SceneSessionManager::GetSceneSessionByName(const std::string& bundleName,
    const std::string& moduleName, const std::string& abilityName, const int32_t appIndex)
{
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession->GetSessionInfo().bundleName_ == bundleName &&
            sceneSession->GetSessionInfo().moduleName_ == moduleName &&
            sceneSession->GetSessionInfo().abilityName_ == abilityName &&
            sceneSession->GetSessionInfo().appIndex_ == appIndex) {
            return sceneSession;
        }
    }
    return nullptr;
}

std::vector<sptr<SceneSession>> SceneSessionManager::GetSceneSessionVectorByType(WindowType type)
{
    std::vector<sptr<SceneSession>> sceneSessionVector;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto &item : sceneSessionMap_) {
        auto sceneSession = item.second;
        if (sceneSession->GetWindowType() == type) {
            sceneSessionVector.emplace_back(sceneSession);
        }
    }

    return sceneSessionVector;
}

WSError SceneSessionManager::UpdateParentSessionForDialog(const sptr<SceneSession>& sceneSession,
    sptr<WindowSessionProperty> property)
{
    if (property == nullptr) {
        WLOGFD("[WMSDialog] Property is null, no need to update parent info");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (sceneSession == nullptr) {
        WLOGFE("[WMSDialog] Session is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto parentPersistentId = property->GetParentPersistentId();
    if (property->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG && parentPersistentId != INVALID_SESSION_ID) {
        auto parentSession = GetSceneSession(parentPersistentId);
        if (parentSession == nullptr) {
            WLOGFE("[WMSDialog] Parent session is nullptr, parentPersistentId:%{public}d", parentPersistentId);
            return WSError::WS_ERROR_NULLPTR;
        }
        parentSession->BindDialogSessionTarget(sceneSession);
        parentSession->BindDialogToParentSession(sceneSession);
        sceneSession->SetParentSession(parentSession);
        WLOGFI("[WMSDialog] Update parent of dialog success, id %{public}d, parentId %{public}d",
            sceneSession->GetPersistentId(), parentPersistentId);
    }
    return WSError::WS_OK;
}

sptr<SceneSession::SpecificSessionCallback> SceneSessionManager::CreateSpecificSessionCallback()
{
    sptr<SceneSession::SpecificSessionCallback> specificCb = new (std::nothrow)SceneSession::SpecificSessionCallback();
    if (specificCb == nullptr) {
        WLOGFE("SpecificSessionCallback is nullptr");
        return nullptr;
    }
    specificCb->onCreate_ = std::bind(&SceneSessionManager::RequestSceneSession,
        this, std::placeholders::_1, std::placeholders::_2);
    specificCb->onDestroy_ = std::bind(&SceneSessionManager::DestroyAndDisconnectSpecificSession,
        this, std::placeholders::_1);
    specificCb->onCameraFloatSessionChange_ = std::bind(&SceneSessionManager::UpdateCameraFloatWindowStatus,
        this, std::placeholders::_1, std::placeholders::_2);
    specificCb->onGetSceneSessionVectorByType_ = std::bind(&SceneSessionManager::GetSceneSessionVectorByType,
        this, std::placeholders::_1);
    specificCb->onUpdateAvoidArea_ = std::bind(&SceneSessionManager::UpdateAvoidArea, this, std::placeholders::_1);
    specificCb->onWindowInfoUpdate_ = std::bind(&SceneSessionManager::NotifyWindowInfoChange,
        this, std::placeholders::_1, std::placeholders::_2);
    specificCb->onSessionTouchOutside_ = std::bind(&SceneSessionManager::NotifySessionTouchOutside,
        this, std::placeholders::_1);
    specificCb->onGetAINavigationBarArea_ = std::bind(&SceneSessionManager::GetAINavigationBarArea, this);
    specificCb->onRecoveryPullPiPMainWindow_ = std::bind(&SceneSessionManager::RecoveryPullPiPMainWindow,
        this, std::placeholders::_1, std::placeholders::_2);
    return specificCb;
}

WMError SceneSessionManager::CheckWindowId(int32_t windowId, int32_t &pid)
{
    auto task = [this, windowId, &pid]() -> WMError {
        pid = INVALID_PID;
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        auto iter = sceneSessionMap_.find(windowId);
        if (iter == sceneSessionMap_.end()) {
            WLOGFE("Window(%{public}d) cannot set cursor style", windowId);
            return WMError::WM_ERROR_INVALID_WINDOW;
        }
        auto sceneSession = iter->second;
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

sptr<SceneSession> SceneSessionManager::CreateSceneSession(const SessionInfo& sessionInfo,
    sptr<WindowSessionProperty> property)
{
    sptr<SceneSession::SpecificSessionCallback> specificCb = CreateSpecificSessionCallback();
    sptr<SceneSession> sceneSession = nullptr;
    if (sessionInfo.isSystem_) {
        sceneSession = new (std::nothrow) SCBSystemSession(sessionInfo, specificCb);
        WLOGFI("[WMSSCB]Create SCBSystemSession, type: %{public}d", sessionInfo.windowType_);
    } else if (property == nullptr && SessionHelper::IsMainWindow(static_cast<WindowType>(sessionInfo.windowType_))) {
        sceneSession = new (std::nothrow) MainSession(sessionInfo, specificCb);
        WLOGFI("[WMSMain]Create MainSession");
    } else if (property != nullptr && SessionHelper::IsSubWindow(property->GetWindowType())) {
        sceneSession = new (std::nothrow) SubSession(sessionInfo, specificCb);
        WLOGFI("[WMSSub]Create SubSession, type: %{public}d", property->GetWindowType());
    } else if (property != nullptr && SessionHelper::IsSystemWindow(property->GetWindowType())) {
        sceneSession = new (std::nothrow) SystemSession(sessionInfo, specificCb);
        WLOGFI("[WMSSystem]Create SystemSession, type: %{public}d", property->GetWindowType());
    } else {
        WLOGFE("[WMSLife]Invalid window type");
    }
    return sceneSession;
}

sptr<SceneSession> SceneSessionManager::RequestSceneSession(const SessionInfo& sessionInfo,
    sptr<WindowSessionProperty> property)
{
    if (sessionInfo.persistentId_ != 0 && !sessionInfo.isPersistentRecover_ &&
        sessionInfo.bundleName_.find("hmsapp.samplemanagement") == std::string::npos) {
        auto session = GetSceneSession(sessionInfo.persistentId_);
        if (session != nullptr) {
            WLOGFD("[WMSLife] get exist session persistentId: %{public}d", sessionInfo.persistentId_);
            return session;
        }
    }

    auto task = [this, sessionInfo, property]() {
        WLOGFI("[WMSLife] RequestSceneSession, appName: [%{public}s %{public}s %{public}s]"
            "appIndex: %{public}d, type %{public}u isSystem:%{public}u, isPersistentRecover: %{public}u",
            sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(),
            sessionInfo.abilityName_.c_str(), sessionInfo.appIndex_, sessionInfo.windowType_,
            static_cast<uint32_t>(sessionInfo.isSystem_), static_cast<uint32_t>(sessionInfo.isPersistentRecover_));
        sptr<SceneSession> sceneSession = CreateSceneSession(sessionInfo, property);
        if (sceneSession == nullptr) {
            WLOGFE("[WMSLife]sceneSession is nullptr!");
            return sceneSession;
        }
        sceneSession->SetEventHandler(taskScheduler_->GetEventHandler());
        if (sessionInfo.isSystem_) {
            sceneSession->SetCallingPid(IPCSkeleton::GetCallingPid());
            sceneSession->SetCallingUid(IPCSkeleton::GetCallingUid());
            auto rootContext = rootSceneContextWeak_.lock();
            sceneSession->SetAbilityToken(rootContext != nullptr ? rootContext->GetToken() : nullptr);
        } else {
            WLOGFD("[WMSLife]RequestSceneSession, id %{public}d, bundleName: %{public}s, moduleName: %{public}s,"
                "abilityName: %{public}s want:%{public}s", sceneSession->GetPersistentId(),
                sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str(),
                sessionInfo.want == nullptr ? "nullptr" : sessionInfo.want->ToString().c_str());
        }
        RegisterSessionExceptionFunc(sceneSession);
        FillSessionInfo(sceneSession);
        auto persistentId = sceneSession->GetPersistentId();
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSession(%d )", persistentId);
        if (WindowHelper::IsMainWindow(sceneSession->GetWindowType())) {
            WindowInfoReporter::GetInstance().InsertCreateReportInfo(sessionInfo.bundleName_);
        }
        sceneSession->SetSystemConfig(systemConfig_);
        sceneSession->SetSnapshotScale(snapshotScale_);
        UpdateParentSessionForDialog(sceneSession, property);
        if (CheckCollaboratorType(sceneSession->GetCollaboratorType())) {
            WLOGFD("ancoSceneState: %{public}d", sceneSession->GetSessionInfo().ancoSceneState);
            PreHandleCollaborator(sceneSession);
        }
        {
            std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            sceneSessionMap_.insert({ persistentId, sceneSession });
        }
        PerformRegisterInRequestSceneSession(sceneSession);
        NotifySessionUpdate(sessionInfo, ActionType::SINGLE_START);
        WLOGFI("[WMSLife] RequestSceneSession, id: %{public}d, type: %{public}d",
            persistentId, sceneSession->GetWindowType());
        return sceneSession;
    };
    return taskScheduler_->PostSyncTask(task, "RequestSceneSession:PID" + std::to_string(sessionInfo.persistentId_));
}

void SceneSessionManager::NotifySessionUpdate(const SessionInfo& sessionInfo, ActionType action, ScreenId fromScreenId)
{
    sptr<DisplayChangeInfo> info = new (std::nothrow) DisplayChangeInfo();
    if (!info) {
        WLOGFE("new info failed");
        return;
    }
    info->action_ = action;
    info->abilityName_ = sessionInfo.abilityName_;
    info->bundleName_ = sessionInfo.bundleName_;
    info->toScreenId_ = sessionInfo.screenId_;
    info->fromScreenId_ = fromScreenId;
    ScreenSessionManagerClient::GetInstance().NotifyDisplayChangeInfoChanged(info);
}

void SceneSessionManager::PerformRegisterInRequestSceneSession(sptr<SceneSession>& sceneSession)
{
    RegisterSessionSnapshotFunc(sceneSession);
    RegisterSessionStateChangeNotifyManagerFunc(sceneSession);
    RegisterRequestFocusStatusNotifyManagerFunc(sceneSession);
    RegisterGetStateFromManagerFunc(sceneSession);
    RegisterInputMethodUpdateFunc(sceneSession);
    RegisterInputMethodShownFunc(sceneSession);
    RegisterInputMethodHideFunc(sceneSession);
}

void SceneSessionManager::UpdateSceneSessionWant(const SessionInfo& sessionInfo)
{
    if (sessionInfo.persistentId_ != 0) {
        auto session = GetSceneSession(sessionInfo.persistentId_);
        if (session != nullptr && sessionInfo.want != nullptr) {
            WLOGFI("get exist session persistentId: %{public}d", sessionInfo.persistentId_);
            if (!CheckCollaboratorType(session->GetCollaboratorType())) {
                session->SetSessionInfoWant(sessionInfo.want);
                WLOGFI("RequestSceneSession update want, persistentId:%{public}d", sessionInfo.persistentId_);
            } else {
                UpdateCollaboratorSessionWant(session);
            }
        }
    }
}

void SceneSessionManager::UpdateCollaboratorSessionWant(sptr<SceneSession>& session)
{
    if (session != nullptr) {
        if (session->GetSessionInfo().ancoSceneState < AncoSceneState::NOTIFY_CREATE) {
            FillSessionInfo(session);
            if (CheckCollaboratorType(session->GetCollaboratorType())) {
                PreHandleCollaborator(session);
            }
        }
    }
}

void SceneSessionManager::RegisterInputMethodUpdateFunc(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("[WMSInput] session is nullptr");
        return;
    }
    NotifyCallingSessionForegroundFunc onInputMethodUpdate = [this](int32_t persistentId) {
        this->OnInputMethodUpdate(persistentId);
    };
    sceneSession->SetNotifyCallingSessionUpdateRectFunc(onInputMethodUpdate);
    WLOGFD("[WMSInput] RegisterInputMethodUpdateFunc success");
}

void SceneSessionManager::OnInputMethodUpdate(const int32_t& persistentId)
{
    WLOGFD("[WMSInput] Resize input method calling window");
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        WLOGFE("[WMSInput] Input method is null");
        return;
    }
    ResizeSoftInputCallingSessionIfNeed(scnSession, true);
}

void SceneSessionManager::RegisterInputMethodShownFunc(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("[WMSInput] session is nullptr");
        return;
    }
    NotifyCallingSessionForegroundFunc onInputMethodShown = [this](int32_t persistentId) {
        this->OnInputMethodShown(persistentId);
    };
    sceneSession->SetNotifyCallingSessionForegroundFunc(onInputMethodShown);
    WLOGFD("[WMSInput] RegisterInputMethodShownFunc success");
}

void SceneSessionManager::OnInputMethodShown(const int32_t& persistentId)
{
    WLOGFD("[WMSInput] Resize input method calling window");
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        WLOGFE("[WMSInput] Input method is null");
        return;
    }

    uint32_t callingWindowId = INVALID_WINDOW_ID;
    if (scnSession->GetSessionProperty() != nullptr) {
        callingWindowId = scnSession->GetSessionProperty()->GetCallingWindow();
    }
    WLOGFI("[WMSInput] Calling window id: %{public}d", callingWindowId);
    callingSession_ = GetSceneSession(callingWindowId);
    if (callingSession_ == nullptr) {
        WLOGFI("[WMSInput] Calling session obtained through callingWindowId is nullptr");
        callingSession_ = GetSceneSession(focusedSessionId_);
        if (callingSession_ == nullptr) {
            WLOGFE("[WMSInput] Calling session obtained through focusedSessionId_ is nullptr");
            return;
        }
    }
    callingSession_->SetTextFieldAvoidInfo(scnSession->textFieldPositionY_, scnSession->textFieldHeight_);
    ResizeSoftInputCallingSessionIfNeed(scnSession);
}

void SceneSessionManager::RegisterInputMethodHideFunc(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("[WMSInput] session is nullptr");
        return;
    }
    NotifyCallingSessionBackgroundFunc onInputMethodHide = [this]() {
        this->RestoreCallingSessionSizeIfNeed();
    };
    sceneSession->SetNotifyCallingSessionBackgroundFunc(onInputMethodHide);
    WLOGFD("[WMSInput] RegisterInputMethodHideFunc success");
}

sptr<AAFwk::SessionInfo> SceneSessionManager::SetAbilitySessionInfo(const sptr<SceneSession>& scnSession)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new (std::nothrow) AAFwk::SessionInfo();
    if (!abilitySessionInfo) {
        WLOGFE("abilitySessionInfo is nullptr");
        return nullptr;
    }
    auto sessionInfo = scnSession->GetSessionInfo();
    sptr<ISession> iSession(scnSession);
    abilitySessionInfo->sessionToken = iSession->AsObject();
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
    if (sessionInfo.want != nullptr) {
        abilitySessionInfo->want = *sessionInfo.want;
    } else {
        abilitySessionInfo->want.SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_,
            sessionInfo.moduleName_);
    }
    return abilitySessionInfo;
}

WSError SceneSessionManager::PrepareTerminate(int32_t persistentId, bool& isPrepareTerminate)
{
    auto task = [this, persistentId, &isPrepareTerminate]() {
        if (!isPrepareTerminateEnable_) { // not support prepareTerminate
            isPrepareTerminate = false;
            WLOGE("not support prepareTerminate, persistentId%{public}d", persistentId);
            return WSError::WS_OK;
        }
        auto scnSession = GetSceneSession(persistentId);
        if (scnSession == nullptr) {
            WLOGFE("scnSession is nullptr persistentId:%{public}d", persistentId);
            isPrepareTerminate = false;
            return WSError::WS_ERROR_NULLPTR;
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (scnSessionInfo == nullptr) {
            WLOGFE("scnSessionInfo is nullptr, persistentId:%{public}d", persistentId);
            isPrepareTerminate = false;
            return WSError::WS_ERROR_NULLPTR;
        }
        WLOGI("[WMSMain]PrepareTerminateAbilityBySCB persistentId:%{public}d", persistentId);
        auto errorCode = AAFwk::AbilityManagerClient::GetInstance()->
            PrepareTerminateAbilityBySCB(scnSessionInfo, isPrepareTerminate);
        WLOGI("[WMSMain]PrepareTerminateAbilityBySCB isPrepareTerminate:%{public}d errorCode:%{public}d",
            isPrepareTerminate, errorCode);
        return WSError::WS_OK;
    };

    taskScheduler_->PostSyncTask(task, "PrepareTerminate:PID:" + std::to_string(persistentId));
    return WSError::WS_OK;
}

std::future<int32_t> SceneSessionManager::RequestSceneSessionActivation(
    const sptr<SceneSession>& sceneSession, bool isNewActive)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    std::shared_ptr<std::promise<int32_t>> promise = std::make_shared<std::promise<int32_t>>();
    auto future = promise->get_future();
    auto task = [this, weakSceneSession, isNewActive, promise]() {
        sptr<SceneSession> scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("[WMSMain]session is nullptr");
            promise->set_value(static_cast<int32_t>(WSError::WS_ERROR_NULLPTR));
            return WSError::WS_ERROR_INVALID_WINDOW;
        }

        auto persistentId = scnSession->GetPersistentId();
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSessionActivation(%d )", persistentId);
        WLOGFI("[WMSMain]active persistentId: %{public}d isSystem_:%{public}u",
            persistentId, static_cast<uint32_t>(scnSession->GetSessionInfo().isSystem_));
        if (!GetSceneSession(persistentId)) {
            WLOGFE("[WMSMain]session is invalid with %{public}d", persistentId);
            promise->set_value(static_cast<int32_t>(WSError::WS_ERROR_INVALID_SESSION));
            return WSError::WS_ERROR_INVALID_WINDOW;
        }
        if (CheckCollaboratorType(scnSession->GetCollaboratorType())) {
            WLOGFI("collaborator use native session");
            scnSession = GetSceneSession(persistentId);
        }
        return RequestSceneSessionActivationInner(scnSession, isNewActive, promise);
    };
    std::string taskName = "RequestSceneSessionActivation:PID:" +
        (sceneSession != nullptr ? std::to_string(sceneSession->GetPersistentId()):"nullptr");
    taskScheduler_->PostAsyncTask(task, taskName);
    return future;
}

WSError SceneSessionManager::RequestSceneSessionActivationInner(
    sptr<SceneSession>& scnSession, bool isNewActive, const std::shared_ptr<std::promise<int32_t>>& promise)
{
    auto persistentId = scnSession->GetPersistentId();
    if (WindowHelper::IsMainWindow(scnSession->GetWindowType())) {
        RequestSessionFocusImmediately(persistentId);
    }
    if (scnSession->GetSessionInfo().ancoSceneState < AncoSceneState::NOTIFY_CREATE) {
        FillSessionInfo(scnSession);
        PreHandleCollaborator(scnSession);
    }
    auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
    if (!scnSessionInfo) {
        promise->set_value(static_cast<int32_t>(WSError::WS_ERROR_NULLPTR));
        return WSError::WS_ERROR_INVALID_WINDOW;
    }
    scnSession->NotifyActivation();
    scnSessionInfo->isNewWant = isNewActive;
    if (CheckCollaboratorType(scnSession->GetCollaboratorType())) {
        scnSessionInfo->want.SetParam(AncoConsts::ANCO_MISSION_ID, scnSessionInfo->persistentId);
        scnSessionInfo->collaboratorType = scnSession->GetCollaboratorType();
    }
    WLOGFI("[WMSMain]begin StartUIAbility: %{public}d isSystem:%{public}u", persistentId,
        static_cast<uint32_t>(scnSession->GetSessionInfo().isSystem_));
    auto errCode = AAFwk::AbilityManagerClient::GetInstance()->StartUIAbilityBySCB(scnSessionInfo);
    if (systemConfig_.backgroundswitch) {
        WLOGFD("[WMSMain]RequestSceneSessionActivationInner: %{public}d", systemConfig_.backgroundswitch);
        scnSession->NotifySessionForeground(1, true);
    }
    auto sessionInfo = scnSession->GetSessionInfo();
    if (WindowHelper::IsMainWindow(scnSession->GetWindowType())) {
        WindowInfoReporter::GetInstance().InsertShowReportInfo(sessionInfo.bundleName_);
    }
    NotifyCollaboratorAfterStart(scnSession, scnSessionInfo);
    promise->set_value(static_cast<int32_t>(errCode));
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

WSError SceneSessionManager::RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession,
    const bool isDelegator)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, isDelegator]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("[WMSMain]session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        WLOGFI("[WMSMain]background session persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSessionBackground (%d )", persistentId);
        scnSession->SetActive(false);
        scnSession->Background();
        if (!GetSceneSession(persistentId)) {
            WLOGFE("[WMSMain]session is invalid with %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        if (persistentId == brightnessSessionId_) {
            UpdateBrightness(focusedSessionId_);
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }

        if (systemConfig_.backgroundswitch) {
            WLOGFD("[WMSMain]RequestSceneSessionBackground: %{public}d", systemConfig_.backgroundswitch);
            scnSession->NotifySessionBackground(1, true, true);
        } else {
            WLOGFI("[WMSMain]begin MinimzeUIAbility: %{public}d isSystem:%{public}u",
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
        WLOGFI("notify interactive session persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:NotifyForegroundInteractiveStatus (%d )", persistentId);
        if (!GetSceneSession(persistentId)) {
            WLOGFE("session is invalid with %{public}d", persistentId);
            return;
        }
        const auto& state = scnSession->GetSessionState();
        if (WindowHelper::IsMainWindow(scnSession->GetWindowType()) &&
            (scnSession->IsVisible() || state == SessionState::STATE_ACTIVE || state == SessionState::STATE_FOREGROUND)) {
            scnSession->NotifyForegroundInteractiveStatus(interactive);
        }
    };

    taskScheduler_->PostAsyncTask(task, "NotifyForegroundInteractiveStatus");
}

WSError SceneSessionManager::DestroyDialogWithMainWindow(const sptr<SceneSession>& scnSession)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:DestroyDialogWithMainWindow");
    if (scnSession == nullptr) {
        WLOGFE("[WMSDialog] scnSession is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (scnSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        WLOGFI("[WMSDialog] Begin to destroy its dialog, parentId: %{public}d", scnSession->GetPersistentId());
        auto dialogVec = scnSession->GetDialogVector();
        for (auto dialog : dialogVec) {
            if (dialog == nullptr) {
                WLOGFE("[WMSDialog] dialog is nullptr");
                continue;
            }
            auto sceneSession = GetSceneSession(dialog->GetPersistentId());
            if (sceneSession == nullptr) {
                WLOGFE("[WMSDialog] dialog is invalid, id: %{public}d", dialog->GetPersistentId());
                return WSError::WS_ERROR_INVALID_SESSION;
            }
            WindowDestroyNotifyVisibility(sceneSession);
            dialog->NotifyDestroy();
            dialog->Disconnect();
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
            WLOGFI("[WMSSub] DestroySubSession, id: %{public}d", persistentId);
            DelayedSingleton<ANRManager>::GetInstance()->OnSessionLost(persistentId);
            WindowDestroyNotifyVisibility(sceneSession);
            NotifyWindowInfoChange(elem->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_REMOVED);
            {
                std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
                sceneSessionMap_.erase(persistentId);
            }
        }
    }
}

void SceneSessionManager::EraseSceneSessionMapById(int32_t persistentId)
{
    std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    sceneSessionMap_.erase(persistentId);
    systemTopSceneSessionMap_.erase(persistentId);
    nonSystemFloatSceneSessionMap_.erase(persistentId);
}

WSError SceneSessionManager::RequestSceneSessionDestruction(
    const sptr<SceneSession>& sceneSession, const bool needRemoveSession)
{
    wptr<SceneSession> weakSceneSession(sceneSession);
    auto task = [this, weakSceneSession, needRemoveSession]() {
        auto scnSession = weakSceneSession.promote();
        if (scnSession == nullptr) {
            WLOGFE("[WMSMain]session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto persistentId = scnSession->GetPersistentId();
        RequestSessionUnfocus(persistentId);
        lastUpdatedAvoidArea_.erase(persistentId);
        DestroyDialogWithMainWindow(scnSession);
        DestroySubSession(scnSession); // destroy sub session by destruction
        WLOGFI("[WMSMain] destroy session persistentId: %{public}d", persistentId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:RequestSceneSessionDestruction (%" PRIu32" )", persistentId);
        if (WindowHelper::IsMainWindow(scnSession->GetWindowType())) {
            auto sessionInfo = scnSession->GetSessionInfo();
            WindowInfoReporter::GetInstance().InsertDestroyReportInfo(sessionInfo.bundleName_);
        }
        WindowDestroyNotifyVisibility(scnSession);
        scnSession->Disconnect();
        if (!GetSceneSession(persistentId)) {
            WLOGFE("[WMSMain]session is invalid with %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
        if (!scnSessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        scnSession->GetCloseAbilityWantAndClean(scnSessionInfo->want);
        if (scnSessionInfo->isClearSession) {
            scnSessionInfo->resultCode = -1;
        }
        if (scnSessionInfo->resultCode == -1) {
            OHOS::AAFwk::Want want;
            scnSessionInfo->want = want;
        }
        NotifySessionUpdate(scnSession->GetSessionInfo(), ActionType::SINGLE_CLOSE);
        WLOGFI("[WMSMain]begin CloseUIAbility: %{public}d isSystem:%{public}u",
            persistentId, static_cast<uint32_t>(scnSession->GetSessionInfo().isSystem_));
        AAFwk::AbilityManagerClient::GetInstance()->CloseUIAbilityBySCB(scnSessionInfo);
        scnSession->SetSessionInfoAncoSceneState(AncoSceneState::DEFAULT_STATE);
        if (needRemoveSession) {
            if (CheckCollaboratorType(scnSession->GetCollaboratorType())) {
                NotifyClearSession(scnSession->GetCollaboratorType(), scnSessionInfo->persistentId);
            }
            EraseSceneSessionMapById(persistentId);
        } else {
            // if terminate, set want to null. so start from recent, start a new one.
            scnSession->SetSessionInfoWant(nullptr);
        }
        if (listenerController_ != nullptr) {
            NotifySessionForCallback(scnSession, needRemoveSession);
        }
        return WSError::WS_OK;
    };
    std::string taskName = "RequestSceneSessionDestruction:PID:" +
        (sceneSession != nullptr ? std::to_string(sceneSession->GetPersistentId()):"nullptr");
    taskScheduler_->PostAsyncTask(task, taskName);
    return WSError::WS_OK;
}

void SceneSessionManager::AddClientDeathRecipient(const sptr<ISessionStage>& sessionStage,
    const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr || sessionStage == nullptr) {
        WLOGFE("[WMSLife]sessionStage or sceneSession is nullptr");
        return;
    }

    auto remoteObject = sessionStage->AsObject();
    remoteObjectMap_.insert(std::make_pair(remoteObject, sceneSession->GetPersistentId()));
    if (windowDeath_ == nullptr) {
        WLOGFE("[WMSLife]failed to create death recipient");
        return;
    }
    if (!remoteObject->AddDeathRecipient(windowDeath_)) {
        WLOGFE("[WMSLife]failed to add death recipient");
        return;
    }
    WLOGFD("Id: %{public}d", sceneSession->GetPersistentId());
}

void SceneSessionManager::DestroySpecificSession(const sptr<IRemoteObject>& remoteObject)
{
    auto task = [this, remoteObject] {
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
        DestroyAndDisconnectSpecificSessionInner(sceneSession);
        remoteObjectMap_.erase(iter);
    };
    taskScheduler_->PostAsyncTask(task, "DestroySpecificSession");
}

WSError SceneSessionManager::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
    sptr<IRemoteObject> token)
{
    if (property == nullptr) {
        WLOGFE("property is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (!CheckSystemWindowPermission(property)) {
        WLOGFE("create system window permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    WLOGFI("[WMSLife] create specific start, name: %{public}s, type: %{public}d",
        property->GetWindowName().c_str(), property->GetWindowType());
    // Get pid and uid before posting task.
    auto pid = IPCSkeleton::GetCallingPid();
    auto uid = IPCSkeleton::GetCallingUid();
    auto task = [this, sessionStage, eventChannel, surfaceNode, property,
                    &persistentId, &session, token, pid, uid]() {
        if (property == nullptr) {
            return WSError::WS_ERROR_NULLPTR;
        }
        const auto& type = property->GetWindowType();
        // create specific session
        SessionInfo info;
        info.windowType_ = static_cast<uint32_t>(type);
        ClosePipWindowIfExist(type);
        sptr<SceneSession> newSession = RequestSceneSession(info, property);
        if (newSession == nullptr) {
            WLOGFE("[WMSSub][WMSSystem] session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        auto errCode = newSession->Connect(
            sessionStage, eventChannel, surfaceNode, systemConfig_, property, token, pid, uid);
        if (property) {
            persistentId = property->GetPersistentId();
        }

        NotifyCreateSpecificSession(newSession, property, type);
        session = newSession;
        AddClientDeathRecipient(sessionStage, newSession);
        WLOGFI("[WMSLife] create specific session success, id: %{public}d, parentId: %{public}d, type: %{public}d",
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
    for (const auto& iter: sceneSessionMap_) {
        auto& session = iter.second;
        if (session && session->GetWindowType() == WindowType::WINDOW_TYPE_PIP) {
            session->NotifyCloseExistPipWindow();
            break;
        }
    }
}

bool SceneSessionManager::CheckSystemWindowPermission(const sptr<WindowSessionProperty>& property) const
{
    WindowType type = property->GetWindowType();
    if (!WindowHelper::IsSystemWindow(type)) {
        // type is not system
        return true;
    }
    if (type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT && SessionPermission::IsStartedByInputMethod()) {
        // WINDOW_TYPE_INPUT_METHOD_FLOAT counld be created by input method app
        WLOGFD("check create permission success, input method app create input method window.");
        return true;
    }
    if (type == WindowType::WINDOW_TYPE_DRAGGING_EFFECT || type == WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW ||
        type == WindowType::WINDOW_TYPE_TOAST || type == WindowType::WINDOW_TYPE_DIALOG ||
        type == WindowType::WINDOW_TYPE_PIP) {
        // some system types counld be created by normal app
        return true;
    }
    if (type == WindowType::WINDOW_TYPE_FLOAT &&
        SessionPermission::VerifyCallingPermission("ohos.permission.SYSTEM_FLOAT_WINDOW")) {
        // WINDOW_TYPE_FLOAT counld be created by normal app with the corresponding permission
        WLOGFD("check create permission success, normal app create float window with request permission.");
        return true;
    }
    if (SessionPermission::IsSystemCalling() || SessionPermission::IsStartByHdcd()) {
        WLOGFD("check create permission success, create with system calling.");
        return true;
    }
    WLOGFE("check system window permission failed.");
    return false;
}

WSError SceneSessionManager::RecoverAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, sptr<ISession>& session, sptr<IRemoteObject> token)
{
    if (property == nullptr) {
        WLOGFE("[RECOVER] property is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }

    auto pid = IPCSkeleton::GetCallingPid();
    auto uid = IPCSkeleton::GetCallingUid();
    auto task = [this, sessionStage, eventChannel, surfaceNode, property, &session, token, pid, uid]() {
        // recover specific session
        const auto& type = property->GetWindowType();
        SessionInfo info = property->GetSessionInfo();
        info.isPersistentRecover_ = true;
        info.persistentId_ = property->GetPersistentId();
        info.windowMode = static_cast<int32_t>(property->GetWindowMode());
        info.windowType_ = static_cast<uint32_t>(type);
        info.requestOrientation_ = static_cast<uint32_t>(property->GetRequestedOrientation());
        info.sessionState_ = (property->GetWindowState() == WindowState::STATE_SHOWN) ? SessionState::STATE_ACTIVE
                                                                                      : SessionState::STATE_BACKGROUND;

        WLOGI("[RECOVER] RecoverAndConnectSpecificSession windowName = %{public}s, windowMode = %{public}d, "
              "windowType = %{public}u, persistentId = %{public}d, windowState = %{public}u",
            property->GetWindowName().c_str(), info.windowMode, info.windowType_, info.persistentId_,
            property->GetWindowState());

        ClosePipWindowIfExist(type);
        sptr<SceneSession> sceneSession = RequestSceneSession(info, property);
        if (sceneSession == nullptr) {
            WLOGFE("[RECOVER] RequestSceneSession failed");
            return WSError::WS_ERROR_NULLPTR;
        }

        auto persistentId = sceneSession->GetPersistentId();
        if (persistentId != info.persistentId_) {
            WLOGFW("[RECOVER] PersistentId changed, from %{public}" PRId32 " to %{public}" PRId32,
                info.persistentId_, persistentId);
        }

        auto errCode =
            sceneSession->Reconnect(sessionStage, eventChannel, surfaceNode, systemConfig_, property, token, pid, uid);
        if (errCode != WSError::WS_OK) {
            WLOGFE("[RECOVER] SceneSession reconnect failed");
            EraseSceneSessionMapById(persistentId);
            return errCode;
        }

        NotifyCreateSpecificSession(sceneSession, property, type);
        RecoverWindowSessionProperty(sceneSession, property);
        AddClientDeathRecipient(sessionStage, sceneSession);
        session = sceneSession;
        return errCode;
    };

    return taskScheduler_->PostSyncTask(task);
}

void SceneSessionManager::RecoverWindowSessionProperty(
    sptr<SceneSession> sceneSession, const sptr<WindowSessionProperty>& property)
{
    if (sceneSession == nullptr || property == nullptr) {
        WLOGFE("[RECOVER] sceneSession or property is nullptr");
        return;
    }

    WLOGFI("[RECOVER] WindowType = %{public}u", property->GetWindowType());

    if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        RelayoutKeyBoard(sceneSession);
    } else {
        const auto& rect = property->GetWindowRect();
        const auto& requestRect = sceneSession->GetSessionProperty()->GetRequestRect();
        Rect newRect = { requestRect.posX_, requestRect.posY_, rect.width_, rect.height_ };
        sceneSession->GetSessionProperty()->SetRequestRect(newRect);
        auto wsRectSize = SessionHelper::TransferToWSRect(newRect);
        sceneSession->UpdateSessionRect(wsRectSize, SizeChangeReason::RESIZE);

        sceneSession->GetSessionProperty()->SetRequestRect(rect);
        auto wsRectPos = SessionHelper::TransferToWSRect(rect);
        sceneSession->UpdateSessionRect(wsRectPos, SizeChangeReason::MOVE);
    }
}

WSError SceneSessionManager::RecoverAndReconnectSceneSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    SystemSessionConfig& systemConfig, sptr<ISession>& session, sptr<WindowSessionProperty> property,
    sptr<IRemoteObject> token, int32_t pid, int32_t uid)
{
    if (property == nullptr) {
        WLOGFE("[RECOVER]property is nullptr!");
        return WSError::WS_ERROR_NULLPTR;
    }
    SessionInfo sessionInfo = property->GetSessionInfo();
    sessionInfo.persistentId_ = property->GetPersistentId();
    sessionInfo.windowMode = static_cast<int32_t>(property->GetWindowMode());
    sessionInfo.windowType_ = static_cast<uint32_t>(property->GetWindowType());
    sessionInfo.requestOrientation_ = static_cast<uint32_t>(property->GetRequestedOrientation());
    WindowState windowState = property->GetWindowState();
    WLOGFI("[RECOVER]Recover and reconnect sceneSession with: bundleName=%{public}s, moduleName=%{public}s, "
        "abilityName=%{public}s, windowMode=%{public}d, windowType=%{public}u, persistentId=%{public}d, "
        "windowState=%{public}u",
        sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str(),
        sessionInfo.windowMode, sessionInfo.windowType_, sessionInfo.persistentId_, static_cast<uint32_t>(windowState));
    sptr<SceneSession> sceneSession = nullptr;
    if (SessionHelper::IsMainWindow(property->GetWindowType())) {
        sceneSession = RequestSceneSession(sessionInfo, nullptr);
    } else {
        sceneSession = RequestSceneSession(sessionInfo, property);
    }
    if (sceneSession == nullptr) {
        WLOGFE("[RECOVER]Request sceneSession failed");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto ret =
        sceneSession->Reconnect(sessionStage, eventChannel, surfaceNode, systemConfig, property, token, pid, uid);
    if (ret != WSError::WS_OK) {
        WLOGFE("[RECOVER]Reconnect failed");
        std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        sceneSessionMap_.erase(sessionInfo.persistentId_);
        return ret;
    }
    sessionInfo.sessionState_ = sceneSession->GetSessionState();
    WLOGFI("[RECOVER]sessionState=%{public}d", sessionInfo.sessionState_);
    if (recoverSceneSessionFunc_) {
        recoverSceneSessionFunc_(sceneSession, sessionInfo);
    } else {
        WLOGFE("[RECOVER]recoverSceneSessionFunc_ is null");
        std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        sceneSessionMap_.erase(sessionInfo.persistentId_);
        return WSError::WS_ERROR_NULLPTR;
    }
    session = sceneSession;
    return WSError::WS_OK;
}

void SceneSessionManager::SetRecoverSceneSessionListener(const NotifyRecoverSceneSessionFunc& func)
{
    WLOGFI("[RECOVER]SetRecoverSceneSessionListener");
    recoverSceneSessionFunc_ = func;
}

void SceneSessionManager::SetCreateSystemSessionListener(const NotifyCreateSystemSessionFunc& func)
{
    createSystemSessionFunc_ = func;
}

void SceneSessionManager::RegisterCreateSubSessionListener(int32_t persistentId,
    const NotifyCreateSubSessionFunc& func)
{
    WLOGFI("[WMSSub] RegisterCreateSubSessionListener, id: %{public}d", persistentId);
    auto task = [this, persistentId, func]() {
        auto iter = createSubSessionFuncMap_.find(persistentId);
        if (iter == createSubSessionFuncMap_.end()) {
            createSubSessionFuncMap_.insert(std::make_pair(persistentId, func));
        } else {
            WLOGFW("[WMSSub] CreateSubSessionListener is existed, id: %{public}d", persistentId);
        }
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
        if (createSystemSessionFunc_ && type != WindowType::WINDOW_TYPE_DIALOG) {
            createSystemSessionFunc_(newSession);
            WLOGFD("[WMSLife] Create system session, id:%{public}d, type: %{public}d",
                newSession->GetPersistentId(), type);
        } else {
            WLOGFW("[WMSLife] Didn't create jsSceneSession for this system type, id:%{public}d, "
                "type: %{public}d", newSession->GetPersistentId(), type);
            return;
        }
    } else if (SessionHelper::IsSubWindow(type)) {
        NotifyCreateSubSession(property->GetParentPersistentId(), newSession);
        WLOGFD("[WMSLife] Notify sub jsSceneSession, id:%{public}d, parentId: %{public}d, type: %{public}d",
            newSession->GetPersistentId(), property->GetParentPersistentId(), type);
    } else {
        WLOGFW("[WMSLife] Invalid session type, id:%{public}d, type: %{public}d",
            newSession->GetPersistentId(), type);
    }
}

void SceneSessionManager::NotifyCreateSubSession(int32_t persistentId, sptr<SceneSession> session)
{
    if (session == nullptr) {
        WLOGFE("[WMSLife] SubSession is nullptr");
        return;
    }
    auto iter = createSubSessionFuncMap_.find(persistentId);
    if (iter == createSubSessionFuncMap_.end()) {
        WLOGFW("[WMSLife] Can't find CreateSubSessionListener, parentId: %{public}d", persistentId);
        return;
    }

    auto parentSession = GetSceneSession(persistentId);
    if (parentSession == nullptr) {
        WLOGFE("[WMSLife] Can't find CreateSubSessionListener, parentId: %{public}d, subId: %{public}d",
            persistentId, session->GetPersistentId());
        return;
    }
    parentSession->AddSubSession(session);
    session->SetParentSession(parentSession);
    if (iter->second) {
        iter->second(session);
    }
    WLOGFD("[WMSLife] NotifyCreateSubSession success, parentId: %{public}d, subId: %{public}d",
        persistentId, session->GetPersistentId());
}

void SceneSessionManager::UnregisterCreateSubSessionListener(int32_t persistentId)
{
    WLOGFI("[WMSSub] UnregisterCreateSubSessionListener, id: %{public}d", persistentId);
    auto task = [this, persistentId]() {
        auto iter = createSubSessionFuncMap_.find(persistentId);
        if (iter != createSubSessionFuncMap_.end()) {
            createSubSessionFuncMap_.erase(persistentId);
        } else {
            WLOGFW("[WMSSub] Can't find CreateSubSessionListener, id: %{public}d", persistentId);
        }
        return WMError::WM_OK;
    };
    taskScheduler_->PostSyncTask(task);
}

void SceneSessionManager::NotifyStatusBarEnabledChange(bool enable)
{
    WLOGFI("NotifyStatusBarEnabledChange enable %{public}d", enable);
    auto task = [this, enable]() {
        if (statusBarEnabledChangeFunc_) {
            statusBarEnabledChangeFunc_(enable);
        }
        return WMError::WM_OK;
    };
    taskScheduler_->PostSyncTask(task, "NotifyStatusBarEnabledChange");
}

void SceneSessionManager::SetStatusBarEnabledChangeListener(const ProcessStatusBarEnabledChangeFunc& func)
{
    WLOGFD("SetStatusBarEnabledChangeListener");
    if (!func) {
        WLOGFD("set func is null");
    }
    statusBarEnabledChangeFunc_ = func;
    NotifyStatusBarEnabledChange(gestureNavigationEnabled_);
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
    WLOGFD("OnOutsideDownEvent x = %{public}d, y = %{public}d", x, y);
    if (outsideDownEventFunc_) {
        outsideDownEventFunc_(x, y);
    }
}

void SceneSessionManager::NotifySessionTouchOutside(int32_t persistentId)
{
    auto task = [this, persistentId]() {
        int32_t callingSessionId = persistentId;
        auto sceneSession = GetSceneSession(persistentId);
        if (callingSession_ != nullptr && sceneSession != nullptr &&
            sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
            callingSessionId = callingSession_->GetPersistentId();
        }
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto &item : sceneSessionMap_) {
            sceneSession = item.second;
            if (sceneSession == nullptr) {
                continue;
            }
            if (sceneSession->GetSessionInfo().isSystem_ ||
                (sceneSession->GetSessionState() != SessionState::STATE_FOREGROUND &&
                sceneSession->GetSessionState() != SessionState::STATE_ACTIVE)) {
                continue;
            }
            auto sessionId = sceneSession->GetPersistentId();
            if ((!sceneSession->CheckOutTouchOutsideRegister()) &&
                (touchOutsideListenerSessionSet_.find(sessionId) == touchOutsideListenerSessionSet_.end())) {
                WLOGFD("id: %{public}d is not in touchOutsideListenerNodes, don't notify.", sessionId);
                continue;
            }
            if (sessionId == callingSessionId) {
                WLOGFD("id: %{public}d is callingSession, don't notify.", sessionId);
                continue;
            }
            if (sessionId != persistentId) {
                sceneSession->NotifyTouchOutside();
            }
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

WSError SceneSessionManager::DestroyAndDisconnectSpecificSessionInner(sptr<SceneSession> sceneSession)
{
    if (sceneSession == nullptr) {
        return WSError::WS_ERROR_NULLPTR;
    }
    const auto& persistentId = sceneSession->GetPersistentId();
    auto ret = sceneSession->UpdateActiveStatus(false);
    WindowDestroyNotifyVisibility(sceneSession);
    if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
        auto parentSession = GetSceneSession(sceneSession->GetParentPersistentId());
        if (parentSession == nullptr) {
            WLOGFE("[WMSSystem][WMSDialog] Dialog not bind parent");
        } else {
            parentSession->RemoveDialogToParentSession(sceneSession);
        }
        sceneSession->NotifyDestroy();
    }
    ret = sceneSession->Disconnect();
    sceneSession->ClearSpecificSessionCbMap();
    if (SessionHelper::IsSubWindow(sceneSession->GetWindowType())) {
        auto parentSession = GetSceneSession(sceneSession->GetParentPersistentId());
        if (parentSession != nullptr) {
            WLOGFD("[WMSSub] Find parentSession, id: %{public}d", persistentId);
            parentSession->RemoveSubSession(persistentId);
        } else {
            WLOGFW("[WMSSub] ParentSession is nullptr, id: %{public}d", persistentId);
        }
    }
    {
        std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        sceneSessionMap_.erase(persistentId);
        systemTopSceneSessionMap_.erase(persistentId);
        nonSystemFloatSceneSessionMap_.erase(persistentId);
        UnregisterCreateSubSessionListener(persistentId);
    }
    WLOGFI("[WMSLife] Destroy specific session end, id: %{public}d", persistentId);
    return ret;
}

WSError SceneSessionManager::DestroyAndDisconnectSpecificSession(const int32_t& persistentId)
{
    const auto& callingPid = IPCSkeleton::GetCallingPid();
    auto task = [this, persistentId, callingPid]() {
        WLOGFI("[WMSLife] Destroy specific session start, id: %{public}d", persistentId);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            return WSError::WS_ERROR_NULLPTR;
        }

        if (callingPid != sceneSession->GetCallingPid()) {
            WLOGFE("[WMSLife] Permission denied, not destroy by the same process");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
        return DestroyAndDisconnectSpecificSessionInner(sceneSession);
    };

    return taskScheduler_->PostSyncTask(task, "DestroyAndDisConnect:PID:" + std::to_string(persistentId));
}

const AppWindowSceneConfig& SceneSessionManager::GetWindowSceneConfig() const
{
    return appWindowSceneConfig_;
}

WSError SceneSessionManager::ProcessBackEvent()
{
    auto task = [this]() {
        auto session = GetSceneSession(focusedSessionId_);
        if (!session) {
            WLOGFE("session is nullptr: %{public}d", focusedSessionId_);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        WLOGFI("ProcessBackEvent session persistentId: %{public}d", focusedSessionId_);
        if (needBlockNotifyFocusStatusUntilForeground_) {
            WLOGFD("RequestSessionBack when start session");
            session->RequestSessionBack(false);
            return WSError::WS_OK;
        }
        session->ProcessBackEvent();
        return WSError::WS_OK;
    };

    taskScheduler_->PostAsyncTask(task, "ProcessBackEvent");
    return WSError::WS_OK;
}

void SceneSessionManager::CleanUserMap()
{
    std::unique_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    WLOGFI("CleanUserMap in size = %{public}zu", sceneSessionMap_.size());
    auto iter = sceneSessionMap_.begin();
    while (iter != sceneSessionMap_.end()) {
        if (iter->second != nullptr && !iter->second->GetSessionInfo().isSystem_ &&
            iter->second->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
            iter = sceneSessionMap_.erase(iter);
        } else {
            iter++;
        }
    }
    WLOGFI("CleanUserMap out size = %{public}zu", sceneSessionMap_.size());

    WLOGFI("Clean systemTopSceneSessionMap in size = %{public}zu", systemTopSceneSessionMap_.size());
    iter = systemTopSceneSessionMap_.begin();
    while (iter != systemTopSceneSessionMap_.end()) {
        if (iter->second != nullptr && !iter->second->GetSessionInfo().isSystem_) {
            iter = systemTopSceneSessionMap_.erase(iter);
        } else {
            iter++;
        }
    }
    WLOGFI("Clean systemTopSceneSessionMap out size = %{public}zu", systemTopSceneSessionMap_.size());
    WLOGFI("Clean nonSystemFloatSceneSessionMap in size = %{public}zu", nonSystemFloatSceneSessionMap_.size());
    iter = nonSystemFloatSceneSessionMap_.begin();
    while (iter != nonSystemFloatSceneSessionMap_.end()) {
        if (iter->second != nullptr && !iter->second->GetSessionInfo().isSystem_) {
            iter = nonSystemFloatSceneSessionMap_.erase(iter);
        } else {
            iter++;
        }
    }
    WLOGFI("Clean nonSystemFloatSceneSessionMap out size = %{public}zu", nonSystemFloatSceneSessionMap_.size());
}

WSError SceneSessionManager::SwitchUser(int32_t oldUserId, int32_t newUserId, std::string &fileDir)
{
    if (oldUserId != currentUserId_ || oldUserId == newUserId || fileDir.empty()) {
        WLOGFE("SwitchUser params invalid");
        return WSError::WS_DO_NOTHING;
    }
    WLOGFD("SwitchUser oldUserId : %{public}d newUserId : %{public}d path : %{public}s",
        oldUserId, newUserId, fileDir.c_str());
    auto task = [this, newUserId, &fileDir]() {
        ScenePersistence::CreateSnapshotDir(fileDir);
        ScenePersistence::CreateUpdatedIconDir(fileDir);
        currentUserId_ = newUserId;
        {
            std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
            for (const auto &item : sceneSessionMap_) {
                auto scnSession = item.second;
                auto persistentId = scnSession->GetPersistentId();
                scnSession->SetActive(false);
                scnSession->Background();
                if (persistentId == brightnessSessionId_) {
                    UpdateBrightness(focusedSessionId_);
                }
                auto scnSessionInfo = SetAbilitySessionInfo(scnSession);
                if (!scnSessionInfo) {
                    return WSError::WS_ERROR_NULLPTR;
                }
                WLOGFI("[WMSMain]begin MinimizeUIAbility newUserId : %{public}d persistentId: %{public}d",
                    newUserId, persistentId);
                AAFwk::AbilityManagerClient::GetInstance()->MinimizeUIAbilityBySCB(scnSessionInfo);
            }
        }
        CleanUserMap();
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "SwitchUser");
    return WSError::WS_OK;
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
    resourceMgr.reset(Global::Resource::CreateResourceManager());
    resourceMgr->UpdateResConfig(*resConfig);

    std::string loadPath;
    if (!abilityInfo.hapPath.empty()) { // zipped hap
        loadPath = abilityInfo.hapPath;
    } else {
        loadPath = abilityInfo.resourcePath;
    }

    if (!resourceMgr->AddResource(loadPath.c_str())) {
        WLOGFE("Add resource %{private}s failed.", loadPath.c_str());
        return nullptr;
    }
    return resourceMgr;
}

void SceneSessionManager::GetStartupPageFromResource(const AppExecFwk::AbilityInfo& abilityInfo,
    std::string& path, uint32_t& bgColor)
{
    auto resourceMgr = GetResourceManager(abilityInfo);
    if (resourceMgr == nullptr) {
        WLOGFE("resource manager is nullptr.");
        return;
    }

    if (resourceMgr->GetColorById(abilityInfo.startWindowBackgroundId, bgColor) != Global::Resource::RState::SUCCESS) {
        WLOGFW("Failed to get background color id %{public}d.", abilityInfo.startWindowBackgroundId);
    }

    if (resourceMgr->GetMediaById(abilityInfo.startWindowIconId, path) != Global::Resource::RState::SUCCESS) {
        WLOGFE("Failed to get icon id %{public}d.", abilityInfo.startWindowIconId);
        return;
    }

    if (!abilityInfo.hapPath.empty()) { // zipped hap
        auto pos = path.find_last_of('.');
        if (pos == std::string::npos) {
            WLOGFE("Format error, path %{private}s.", path.c_str());
            return;
        }
        path = "resource:///" + std::to_string(abilityInfo.startWindowIconId) + path.substr(pos);
    }
}

void SceneSessionManager::GetStartupPage(const SessionInfo& sessionInfo, std::string& path, uint32_t& bgColor)
{
    if (!bundleMgr_) {
        WLOGFE("bundle manager is nullptr.");
        return;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:GetStartupPage");
    AAFwk::Want want;
    want.SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_, sessionInfo.moduleName_);
    AppExecFwk::AbilityInfo abilityInfo;
    bool ret = bundleMgr_->QueryAbilityInfo(
        want, AppExecFwk::GET_ABILITY_INFO_DEFAULT, AppExecFwk::Constants::ANY_USERID, abilityInfo);
    if (!ret) {
        WLOGFE("Get ability info from BMS failed!");
        return;
    }

    GetStartupPageFromResource(abilityInfo, path, bgColor);
}

void SceneSessionManager::FillSessionInfo(sptr<SceneSession>& sceneSession)
{
    auto sessionInfo = sceneSession->GetSessionInfo();
    if (sessionInfo.bundleName_.empty()) {
        WLOGFE("FillSessionInfo bundleName_ is null");
        return;
    }
    if (sessionInfo.isSystem_) {
        WLOGFD("FillSessionInfo systemScene!");
        return;
    }
    auto abilityInfo = QueryAbilityInfoFromBMS(currentUserId_, sessionInfo.bundleName_, sessionInfo.abilityName_,
        sessionInfo.moduleName_);
    if (abilityInfo == nullptr) {
        WLOGFE("FillSessionInfo abilityInfo is nullptr!");
        return;
    }
    sceneSession->SetSessionInfoAbilityInfo(abilityInfo);
    sceneSession->SetSessionInfoTime(GetCurrentTime());
    if (abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::RESERVE_TYPE)) {
        sceneSession->SetCollaboratorType(CollaboratorType::RESERVE_TYPE);
    } else if (abilityInfo->applicationInfo.codePath == std::to_string(CollaboratorType::OTHERS_TYPE)) {
        sceneSession->SetCollaboratorType(CollaboratorType::OTHERS_TYPE);
    }
    WLOGFI("FillSessionInfo end, removeMissionAfterTerminate: %{public}d excludeFromMissions: %{public}d "
           " label:%{public}s iconPath:%{public}s collaboratorType:%{public}s",
           abilityInfo->removeMissionAfterTerminate, abilityInfo->excludeFromMissions,
           abilityInfo->label.c_str(), abilityInfo->iconPath.c_str(), abilityInfo->applicationInfo.codePath.c_str());
}

std::shared_ptr<AppExecFwk::AbilityInfo> SceneSessionManager::QueryAbilityInfoFromBMS(const int32_t uId,
    const std::string& bundleName, const std::string& abilityName, const std::string& moduleName)
{
    AAFwk::Want want;
    want.SetElementName("", bundleName, abilityName, moduleName);
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    if (abilityInfo == nullptr) {
        WLOGFE("QueryAbilityInfoFromBMS abilityInfo is nullptr!");
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
    return abilityInfo;
}

WMError SceneSessionManager::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    const auto& callingPid = IPCSkeleton::GetCallingPid();
    auto task = [this, mainWinId, &topWinId, callingPid]() {
        const auto& mainSession = GetSceneSession(mainWinId);
        if (mainSession == nullptr) {
            return WMError::WM_ERROR_INVALID_WINDOW;
        }

        if (callingPid != mainSession->GetCallingPid()) {
            WLOGFE("Permission denied, not destroy by the same process");
            return WMError::WM_ERROR_INVALID_PERMISSION;
        }
        const auto& subVec = mainSession->GetSubSession();
        uint32_t zOrder = mainSession->GetZOrder();
        topWinId = mainWinId;
        for (const auto& subSession : subVec) {
            if (subSession != nullptr && (subSession->GetSessionState() == SessionState::STATE_FOREGROUND ||
                subSession->GetSessionState() == SessionState::STATE_ACTIVE) && subSession->GetZOrder() > zOrder) {
                topWinId = subSession->GetPersistentId();
                zOrder = subSession->GetZOrder();
                WLOGFD("[GetTopWin] Current zorder is larger than mainWin, mainId: %{public}d, topWinId: %{public}d, "
                    "zOrder: %{public}d", mainWinId, topWinId, zOrder);
            }
        }
        WLOGFD("[GetTopWin] Get top window, mainId: %{public}d, topWinId: %{public}d, zOrder: %{public}d",
            mainWinId, topWinId, zOrder);
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetTopWindowId");
}

WMError SceneSessionManager::UpdateSessionProperty(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    if (property == nullptr) {
        WLOGFE("property is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (action == WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE) {
        if (!SessionPermission::VerifyCallingPermission("ohos.permission.PRIVACY_WINDOW")) {
            return WMError::WM_ERROR_INVALID_PERMISSION;
        }
    }

    bool isSystemCalling = SessionPermission::IsSystemCalling() || SessionPermission::IsStartByHdcd();
    property->SetSystemCalling(isSystemCalling);
    wptr<SceneSessionManager> weak = this;
    auto task = [weak, property, action]() -> WMError {
        auto weakSession = weak.promote();
        if (weakSession == nullptr) {
            WLOGFE("the session is nullptr");
            return WMError::WM_DO_NOTHING;
        }
        if (property == nullptr) {
            WLOGFE("the property is nullptr");
            return WMError::WM_DO_NOTHING;
        }
        auto sceneSession = weakSession->GetSceneSession(property->GetPersistentId());
        if (sceneSession == nullptr) {
            WLOGFW("the scene session is nullptr");
            return WMError::WM_DO_NOTHING;
        }
        WLOGD("Id: %{public}d, action: %{public}u", sceneSession->GetPersistentId(), action);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:UpdateProperty");
        return weakSession->HandleUpdateProperty(property, action, sceneSession);
    };
    return taskScheduler_->PostSyncTask(task, "UpdateProperty");
}

WMError SceneSessionManager::UpdatePropertyDragEnabled(const sptr<WindowSessionProperty>& property,
                                                       const sptr<SceneSession>& sceneSession)
{
    if (!property->GetSystemCalling()) {
        WLOGFE("Update property dragEnabled permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }

    if (sceneSession->GetSessionProperty() != nullptr) {
        sceneSession->GetSessionProperty()->SetDragEnabled(property->GetDragEnabled());
    }
    return WMError::WM_OK;
}

WMError SceneSessionManager::UpdatePropertyRaiseEnabled(const sptr<WindowSessionProperty>& property,
                                                        const sptr<SceneSession>& sceneSession)
{
    if (!property->GetSystemCalling()) {
        WLOGFE("Update property raiseEnabled permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }

    if (sceneSession->GetSessionProperty() != nullptr) {
        sceneSession->GetSessionProperty()->SetRaiseEnabled(property->GetRaiseEnabled());
    }
    return WMError::WM_OK;
}

void SceneSessionManager::HandleSpecificSystemBarProperty(WindowType type, const sptr<WindowSessionProperty>& property,
    const sptr<SceneSession>& sceneSession)
{
    auto systemBarProperties = property->GetSystemBarProperty();
    for (auto iter : systemBarProperties) {
        if (iter.first == type) {
            sceneSession->SetSystemBarProperty(iter.first, iter.second);
            WLOGFD("SetSystemBarProperty: %{public}d, enable: %{public}d",
                   static_cast<int32_t>(iter.first), iter.second.enable_);
        }
    }
    NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
}

WMError SceneSessionManager::HandleUpdateProperty(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action, const sptr<SceneSession>& sceneSession)
{
    switch (action) {
        case WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON: {
            sceneSession->SetTurnScreenOn(property->IsTurnScreenOn());
            HandleTurnScreenOn(sceneSession);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON: {
            sceneSession->SetKeepScreenOn(property->IsKeepScreenOn());
            HandleKeepScreenOn(sceneSession, property->IsKeepScreenOn());
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE: {
            sceneSession->SetFocusable(property->GetFocusable());
            NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE: {
            sceneSession->SetTouchable(property->GetTouchable());
            NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS: {
            if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                WLOGW("only app main window can set brightness");
                return WMError::WM_OK;
            }
            // @todo if sceneSession is inactive, return
            SetBrightness(sceneSession, property->GetBrightness());
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION: {
            sceneSession->SetRequestedOrientation(property->GetRequestedOrientation());
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE: {
            bool isPrivacyMode = property->GetPrivacyMode() || property->GetSystemPrivacyMode();
            sceneSession->SetPrivacyMode(isPrivacyMode);
            UpdatePrivateStateAndNotify(sceneSession->GetPersistentId());
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_SYSTEM_PRIVACY_MODE: {
            bool isPrivacyMode = property->GetPrivacyMode() || property->GetSystemPrivacyMode();
            sceneSession->SetPrivacyMode(isPrivacyMode);
            UpdatePrivateStateAndNotify(sceneSession->GetPersistentId());
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetMaximizeMode(property->GetMaximizeMode());
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS: {
            auto systemBarProperties = property->GetSystemBarProperty();
            for (auto iter : systemBarProperties) {
                sceneSession->SetSystemBarProperty(iter.first, iter.second);
            }
            NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_STATUS_PROPS: {
            HandleSpecificSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property, sceneSession);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_PROPS: {
            HandleSpecificSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR, property, sceneSession);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_INDICATOR_PROPS: {
            HandleSpecificSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_INDICATOR, property, sceneSession);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_FLAGS: {
            SetWindowFlags(sceneSession, property);
            NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_MODE: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetWindowMode(property->GetWindowMode());
            }
            NotifyWindowInfoChange(property->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetAnimationFlag(property->GetAnimationFlag());
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                std::vector<Rect> touchHotAreas;
                property->GetTouchHotAreas(touchHotAreas);
                sceneSession->GetSessionProperty()->SetTouchHotAreas(touchHotAreas);
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_CALLING_WINDOW: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetCallingWindow(property->GetCallingWindow());
            }
            if (callingWindowIdChangeFunc_ != nullptr) {
                callingWindowIdChangeFunc_(property->GetCallingWindow());
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE: {
            if (property != nullptr && !property->GetSystemCalling()) {
                WLOGFE("update decor enable permission denied!");
                break;
            }
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetDecorEnable(property->IsDecorEnable());
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS: {
            if (sceneSession->GetSessionProperty() != nullptr) {
                sceneSession->GetSessionProperty()->SetWindowLimits(property->GetWindowLimits());
            }
            break;
        }
        case WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED: {
            return UpdatePropertyDragEnabled(property, sceneSession);
        }
        case WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED: {
            return UpdatePropertyRaiseEnabled(property, sceneSession);
        }
        case WSPropertyChangeAction::ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS: {
            UpdateHideNonSystemFloatingWindows(property, sceneSession);
            break;
        }
        default:
            break;
    }
    return WMError::WM_OK;
}

void SceneSessionManager::UpdateHideNonSystemFloatingWindows(const sptr<WindowSessionProperty>& property,
    const sptr<SceneSession>& sceneSession)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("Update property hideNonSystemFloatingWindows permission denied!");
        return;
    }

    auto propertyOld = sceneSession->GetSessionProperty();
    if (propertyOld == nullptr) {
        WLOGFI("UpdateHideNonSystemFloatingWindows, session property null");
        return;
    }

    bool hideNonSystemFloatingWindowsOld = propertyOld->GetHideNonSystemFloatingWindows();
    bool hideNonSystemFloatingWindowsNew = property->GetHideNonSystemFloatingWindows();
    if (hideNonSystemFloatingWindowsOld == hideNonSystemFloatingWindowsNew) {
        WLOGFI("property hideNonSystemFloatingWindows not change");
        return;
    }

    if (IsSessionVisible(sceneSession)) {
        if (hideNonSystemFloatingWindowsOld) {
            UpdateForceHideState(sceneSession, propertyOld, false);
        } else {
            UpdateForceHideState(sceneSession, property, true);
        }
    }
    propertyOld->SetHideNonSystemFloatingWindows(hideNonSystemFloatingWindowsNew);
}

void SceneSessionManager::UpdateForceHideState(const sptr<SceneSession>& sceneSession,
    const sptr<WindowSessionProperty>& property, bool add)
{
    if (property == nullptr) {
        WLOGFD("property is nullptr");
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
        bool shouldLock = requireLock && IsSessionVisible(scnSession);
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
    if (!sceneSession->IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (brightness == sceneSession->GetBrightness()) {
        WLOGFD("Session brightness do not change: [%{public}f]", brightness);
        return WSError::WS_DO_NOTHING;
    }
    sceneSession->SetBrightness(brightness);
#ifdef POWERMGR_DISPLAY_MANAGER_ENABLE
    if (GetDisplayBrightness() != brightness) {
        if (std::fabs(brightness - UNDEFINED_BRIGHTNESS) < std::numeric_limits<float>::min()) {
            DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().RestoreBrightness();
            SetDisplayBrightness(UNDEFINED_BRIGHTNESS); // UNDEFINED_BRIGHTNESS means system default brightness
        } else {
            DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().OverrideBrightness(
                static_cast<uint32_t>(brightness * MAX_BRIGHTNESS));
            SetDisplayBrightness(brightness);
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
    WLOGI("Brightness: [%{public}f, %{public}f]", GetDisplayBrightness(), brightness);
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

WMError SceneSessionManager::SetGestureNavigaionEnabled(bool enable)
{
    if (!SessionPermission::IsSystemCalling() && !SessionPermission::IsStartByHdcd()) {
        WLOGFE("SetGestureNavigationEnabled permission denied!");
        return WMError::WM_ERROR_NOT_SYSTEM_APP;
    }
    WLOGFD("SetGestureNavigationEnabled, enable: %{public}d", enable);
    gestureNavigationEnabled_ = enable;
    auto task = [this, enable]() {
        if (!gestureNavigationEnabledChangeFunc_ && !statusBarEnabledChangeFunc_) {
            WLOGFE("callback func is null");
            return WMError::WM_DO_NOTHING;
        }
        if (gestureNavigationEnabledChangeFunc_) {
            gestureNavigationEnabledChangeFunc_(enable);
        }
        if (statusBarEnabledChangeFunc_) {
            statusBarEnabledChangeFunc_(enable);
        }
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetGestureNavigaionEnabled");
}

WSError SceneSessionManager::SetFocusedSession(int32_t persistentId)
{
    if (focusedSessionId_ == persistentId) {
        WLOGI("Focus scene not change, id: %{public}d", focusedSessionId_);
        return WSError::WS_DO_NOTHING;
    }
    lastFocusedSessionId_ = focusedSessionId_;
    focusedSessionId_ = persistentId;
    return WSError::WS_OK;
}

int32_t SceneSessionManager::GetFocusedSession() const
{
    return focusedSessionId_;
}

void SceneSessionManager::GetFocusWindowInfo(FocusChangeInfo& focusInfo)
{
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
    NotifySessionExceptionFunc sessionExceptionFunc = [this](const SessionInfo& info) {
        auto task = [this, info]() {
            auto scnSession = GetSceneSession(info.persistentId_);
            if (scnSession == nullptr) {
                WLOGFW("[WMSLife]NotifySessionExceptionFunc, Not found session, id: %{public}d", info.persistentId_);
                return;
            }
            if (listenerController_ == nullptr) {
                WLOGFW("[WMSLife]NotifySessionExceptionFunc, listenerController_ is nullptr");
                return;
            }
            if (scnSession->GetSessionInfo().isSystem_) {
                WLOGFW("[WMSLife]NotifySessionExceptionFunc, id: %{public}d is system", scnSession->GetPersistentId());
                return;
            }
            WLOGFI("[WMSLife]NotifySessionExceptionFunc, errorCode: %{public}d, id: %{public}d", info.errorCode,
                   info.persistentId_);
            if (info.errorCode == static_cast<int32_t>(AAFwk::ErrorLifecycleState::ABILITY_STATE_LOAD_TIMEOUT) ||
                info.errorCode == static_cast<int32_t>(AAFwk::ErrorLifecycleState::ABILITY_STATE_FOREGROUND_TIMEOUT)) {
                WLOGFD("[WMSLife]NotifySessionClosed when ability load timeout or foreground timeout, id: %{public}d",
                       info.persistentId_);
                listenerController_->NotifySessionClosed(info.persistentId_);
            }
        };
        taskScheduler_->PostVoidSyncTask(task, "sessionException");
    };
    sceneSession->SetSessionExceptionListener(sessionExceptionFunc);
    WLOGFD("[WMSLife]RegisterSessionExceptionFunc success, id: %{public}d", sceneSession->GetPersistentId());
}

void SceneSessionManager::RegisterSessionSnapshotFunc(const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("session is nullptr");
        return;
    }
    NotifySessionSnapshotFunc sessionSnapshotFunc = [this](int32_t persistentId) {
        auto task = [this, persistentId]() {
            auto scnSession = GetSceneSession(persistentId);
            if (scnSession == nullptr) {
                WLOGFW("NotifySessionSnapshotFunc, Not found session, id: %{public}d", persistentId);
                return;
            }
            if (scnSession->GetSessionInfo().isSystem_) {
                WLOGFW("NotifySessionSnapshotFunc, id: %{public}d is system", scnSession->GetPersistentId());
                return;
            }
            auto abilityInfoPtr = scnSession->GetSessionInfo().abilityInfo;
            if (abilityInfoPtr == nullptr) {
                WLOGFW("NotifySessionSnapshotFunc, abilityInfoPtr is nullptr");
                return;
            }
            if (listenerController_ == nullptr) {
                WLOGFW("NotifySessionSnapshotFunc, listenerController_ is nullptr");
                return;
            }
            if (!(abilityInfoPtr->excludeFromMissions)) {
                listenerController_->NotifySessionSnapshotChanged(persistentId);
            }
        };
        taskScheduler_->PostVoidSyncTask(task, "sessionSnapshot");
    };
    sceneSession->SetSessionSnapshotListener(sessionSnapshotFunc);
    WLOGFD("RegisterSessionSnapshotFunc success, id: %{public}d", sceneSession->GetPersistentId());
}

void SceneSessionManager::NotifySessionForCallback(const sptr<SceneSession>& scnSession, const bool needRemoveSession)
{
    if (scnSession == nullptr) {
        WLOGFW("NotifySessionForCallback, scnSession is nullptr");
        return;
    }
    if (scnSession->GetSessionInfo().isSystem_) {
        WLOGFW("NotifySessionForCallback, id: %{public}d is system", scnSession->GetPersistentId());
        return;
    }
    WLOGFI("NotifySessionForCallback, id: %{public}d, needRemoveSession: %{public}u", scnSession->GetPersistentId(),
           static_cast<uint32_t>(needRemoveSession));
    if (scnSession->GetSessionInfo().appIndex_ != 0) {
        WLOGFI("NotifySessionDestroy, appIndex_: %{public}d, id: %{public}d",
               scnSession->GetSessionInfo().appIndex_, scnSession->GetPersistentId());
        listenerController_->NotifySessionDestroyed(scnSession->GetPersistentId());
        return;
    }
    if (needRemoveSession) {
        WLOGFI("NotifySessionDestroy, needRemoveSession, id: %{public}d", scnSession->GetPersistentId());
        listenerController_->NotifySessionDestroyed(scnSession->GetPersistentId());
        return;
    }
    if (scnSession->GetSessionInfo().abilityInfo == nullptr) {
        WLOGFW("abilityInfo is nullptr, id: %{public}d", scnSession->GetPersistentId());
    } else if ((scnSession->GetSessionInfo().abilityInfo)->removeMissionAfterTerminate ||
               (scnSession->GetSessionInfo().abilityInfo)->excludeFromMissions) {
        WLOGFI("NotifySessionDestroy, removeMissionAfterTerminate or excludeFromMissions, id: %{public}d",
            scnSession->GetPersistentId());
        listenerController_->NotifySessionDestroyed(scnSession->GetPersistentId());
        return;
    }
    WLOGFI("NotifySessionClosed, id: %{public}d", scnSession->GetPersistentId());
    listenerController_->NotifySessionClosed(scnSession->GetPersistentId());
}

bool SceneSessionManager::IsSessionVisible(const sptr<SceneSession>& session)
{
    if (session == nullptr) {
        return false;
    }
    const auto& state = session->GetSessionState();
    if (WindowHelper::IsSubWindow(session->GetWindowType())) {
        const auto& parentSceneSession = GetSceneSession(session->GetParentPersistentId());
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

    if (session->IsVisible() || state == SessionState::STATE_ACTIVE || state == SessionState::STATE_FOREGROUND) {
        WLOGFD("Window is at foreground, id: %{public}d", session->GetPersistentId());
        return true;
    }
    WLOGFD("Window is at background, id: %{public}d", session->GetPersistentId());
    return false;
}

void SceneSessionManager::DumpSessionInfo(const sptr<SceneSession>& session, std::ostringstream& oss)
{
    if (session == nullptr) {
        return;
    }
    int32_t zOrder = IsSessionVisible(session) ? static_cast<int32_t>(session->GetZOrder()) : -1;
    WSRect rect = session->GetSessionRect();
    std::string sName;
    if (session->GetSessionInfo().isSystem_) {
        sName = session->GetSessionInfo().abilityName_;
    } else {
        sName = session->GetWindowName();
    }
    uint32_t displayId = session->GetSessionInfo().screenId_;
    uint32_t flag = 0;
    if (session->GetSessionProperty()) {
        flag = session->GetSessionProperty()->GetWindowFlags();
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
        << std::left << std::setw(OFFSET_MAX_WIDTH) << session->GetOffsetX()
        << std::left << std::setw(OFFSET_MAX_WIDTH) << session->GetOffsetY()
        << "]"
        << std::left << std::setw(SCALE_MAX_WIDTH) << session->GetScaleX()
        << std::left << std::setw(SCALE_MAX_WIDTH) << session->GetScaleY()
        << std::endl;
}

void SceneSessionManager::DumpAllAppSessionInfo(std::ostringstream& oss)
{
    oss << std::endl << "Current mission lists:" << std::endl;
    oss << " MissionList Type #NORMAL" << std::endl;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& elem : sceneSessionMap_) {
        auto curSession = elem.second;
        if (curSession == nullptr) {
            WLOGFW("curSession is nullptr");
            continue;
        }
        if (curSession->GetSessionInfo().isSystem_ ||
            curSession->GetWindowType() < WindowType::APP_MAIN_WINDOW_BASE ||
            curSession->GetWindowType() >= WindowType::APP_MAIN_WINDOW_END) {
            WLOGFW("No need to dump, id: %{public}d, isSystem: %{public}d, windowType: %{public}d",
                curSession->GetPersistentId(), curSession->GetSessionInfo().isSystem_, curSession->GetWindowType());
            continue;
        }

        const auto& sessionInfo = curSession->GetSessionInfo();
        std::string isActive = curSession->IsActive() ? "FOREGROUND" : "BACKGROUND";
        oss << "    Mission ID #" << curSession->GetPersistentId() << "  mission name #" << "[#"
            << sessionInfo.bundleName_ << ":" << sessionInfo.moduleName_ << ":" << sessionInfo.abilityName_
            << "]" << "    lockedState #0" << std::endl;
        oss << "    app name [" << sessionInfo.bundleName_ << "]" << std::endl;
        oss << "    main name [" << sessionInfo.abilityName_ << "]" << std::endl;
        oss << "    bundle name [" << sessionInfo.bundleName_ << "]" << std::endl;
        oss << "    ability type [PAGE]" << std::endl;
        oss << "    state #" << isActive.c_str() << std::endl;
        oss << "    app state #" << isActive.c_str() << std::endl;
        oss << "    callee connections:" << std::endl;
    }
}

WSError SceneSessionManager::GetAllSessionDumpInfo(std::string& dumpInfo)
{
    int32_t screenGroupId = 0;
    std::ostringstream oss;
    oss << "-------------------------------------ScreenGroup " << screenGroupId
        << "-------------------------------------" << std::endl;
    oss << "WindowName           DisplayId Pid     WinId Type Mode Flag ZOrd Orientation [ x    y    w    h    "
        << "offsetX   offsetY ] scaleX    ScaleY"
        << std::endl;

    std::vector<sptr<SceneSession>> allSession;
    std::vector<sptr<SceneSession>> backgroundSession;
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    for (const auto& elem : sceneSessionMap_) {
        auto curSession = elem.second;
        if (curSession == nullptr) {
            continue;
        }
        if (IsSessionVisible(curSession)) {
            allSession.push_back(curSession);
        } else {
            backgroundSession.push_back(curSession);
        }
    }
    lock.unlock();
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
    oss << "Focus window: " << GetFocusedSession() << std::endl;
    oss << "Total window num: " << sceneSessionMap_.size() << std::endl;
    DumpAllAppSessionInfo(oss);
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

    WSRect rect = session->GetSessionRect();
    std::string isVisible = session->GetVisible() ? "true" : "false";
    std::string Focusable = session->GetFocusable() ? "true" : "false";
    std::string DecoStatus = session->GetSessionProperty()->IsDecorEnable() ? "true" : "false";
    bool PrivacyMode = session->GetSessionProperty()->GetSystemPrivacyMode() ||
        session->GetSessionProperty()->GetPrivacyMode();
    std::string isPrivacyMode = PrivacyMode ? "true" : "false";
    bool isFirstFrameAvailable = true;
    std::ostringstream oss;
    oss << "WindowName: " << session->GetWindowName()  << std::endl;
    oss << "DisplayId: " << 0 << std::endl;
    oss << "WinId: " << session->GetPersistentId() << std::endl;
    oss << "Pid: " << session->GetCallingPid() << std::endl;
    oss << "Type: " << static_cast<uint32_t>(session->GetWindowType()) << std::endl;
    oss << "Mode: " << static_cast<uint32_t>(session->GetWindowMode()) << std::endl;
    oss << "Flag: " << session->GetSessionProperty()->GetWindowFlags() << std::endl;
    oss << "Orientation: " << static_cast<uint32_t>(session->GetRequestedOrientation()) << std::endl;
    oss << "FirstFrameCallbackCalled: " << isFirstFrameAvailable << std::endl;
    oss << "IsVisible: " << isVisible << std::endl;
    oss << "Focusable: "  << Focusable << std::endl;
    oss << "DecoStatus: "  << DecoStatus << std::endl;
    oss << "isPrivacyMode: "  << isPrivacyMode << std::endl;
    oss << "WindowRect: " << "[ "
        << rect.posX_ << ", " << rect.posY_ << ", " << rect.width_ << ", " << rect.height_
        << " ]" << std::endl;
    oss << "scaleX: " << session->GetScaleX() << std::endl;
    oss << "scaleY: " << session->GetScaleY() << std::endl;
    dumpInfo.append(oss.str());

    DumpSessionElementInfo(session, params, dumpInfo);
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
        if (params.size() >= 2 && params[0] == ARG_DUMP_WINDOW && IsValidDigitString(params[1])) { // 2: params num
            return GetSpecifiedSessionDumpInfo(dumpInfo, params, params[1]);
        }
        return WSError::WS_ERROR_INVALID_OPERATION;
    };
    return taskScheduler_->PostSyncTask(task);
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

WMError SceneSessionManager::RequestFocusStatus(int32_t persistentId, bool isFocused, bool byForeground)
{
    auto task = [this, persistentId, isFocused, byForeground]() {
        if (isFocused) {
            RequestSessionFocus(persistentId, byForeground);
        } else {
            RequestSessionUnfocus(persistentId);
        }
    };
    taskScheduler_->PostAsyncTask(task, "RequestFocusStatus" + std::to_string(persistentId));
    return WMError::WM_OK;
}

/**
 * request focus and ignore its state
 * only used when app main window start before foreground
*/
WSError SceneSessionManager::RequestSessionFocusImmediately(int32_t persistentId)
{
    WLOGFI("[WMSFocus]RequestSessionFocusImmediately, id: %{public}d", persistentId);
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
    if (!sceneSession->GetFocusable()) {
        WLOGFD("[WMSFocus]session is not focusable!");
        return WSError::WS_DO_NOTHING;
    }
    // specific block
    WSError specificCheckRet = RequestFocusSpecificCheck(sceneSession, true);
    if (specificCheckRet != WSError::WS_OK) {
        return specificCheckRet;
    }

    needBlockNotifyUnfocusStatus_ = needBlockNotifyFocusStatusUntilForeground_;
    if (!sceneSession->IsSessionValid()) {
        needBlockNotifyFocusStatusUntilForeground_ = true;
    }
    ShiftFocus(sceneSession);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSessionFocus(int32_t persistentId, bool byForeground)
{
    WLOGFI("[WMSFocus]RequestSessionFocus, id: %{public}d, by foreground: %{public}d", persistentId, byForeground);
    WSError basicCheckRet = RequestFocusBasicCheck(persistentId);
    if (basicCheckRet != WSError::WS_OK) {
        return basicCheckRet;
    }
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("[WMSComm]session is nullptr");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (!sceneSession->GetFocusable() || !IsSessionVisible(sceneSession)) {
        WLOGFD("[WMSFocus]session is not focusable or not visible!");
        return WSError::WS_DO_NOTHING;
    }
    // subwindow/dialog state block
    if ((WindowHelper::IsSubWindow(sceneSession->GetWindowType()) ||
        sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) &&
        GetSceneSession(sceneSession->GetParentPersistentId()) &&
        !IsSessionVisible(GetSceneSession(sceneSession->GetParentPersistentId()))) {
            WLOGFD("[WMSFocus]parent session id: %{public}d is not visible!", sceneSession->GetParentPersistentId());
            return WSError::WS_DO_NOTHING;
    }
    // specific block
    WSError specificCheckRet = RequestFocusSpecificCheck(sceneSession, byForeground);
    if (specificCheckRet != WSError::WS_OK) {
        return specificCheckRet;
    }

    needBlockNotifyUnfocusStatus_ = needBlockNotifyFocusStatusUntilForeground_;
    needBlockNotifyFocusStatusUntilForeground_ = false;
    ShiftFocus(sceneSession);
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestSessionUnfocus(int32_t persistentId)
{
    WLOGFI("[WMSFocus]RequestSessionUnfocus, id: %{public}d", persistentId);
    if (persistentId == INVALID_SESSION_ID) {
        WLOGFE("id is invalid");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    auto focusedSession = GetSceneSession(focusedSessionId_);
    if (persistentId != focusedSessionId_ &&
        !(focusedSession && focusedSession->GetParentPersistentId() == persistentId)) {
        WLOGFD("[WMSFocus]unfocused id cannot request unfocus!");
        return WSError::WS_DO_NOTHING;
    }
    // if pop menu created by desktop request unfocus, back to desktop
    auto lastSession = GetSceneSession(lastFocusedSessionId_);
    if (focusedSession && focusedSession->GetWindowType() == WindowType::WINDOW_TYPE_SYSTEM_FLOAT &&
        lastSession && lastSession->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP &&
        RequestSessionFocus(lastFocusedSessionId_) == WSError::WS_OK) {
            WLOGFD("[WMSFocus]focus is back to desktop");
            return WSError::WS_OK;
    }
    auto nextSession = GetNextFocusableSession(persistentId);

    needBlockNotifyUnfocusStatus_ = needBlockNotifyFocusStatusUntilForeground_;
    needBlockNotifyFocusStatusUntilForeground_ = false;
    return ShiftFocus(nextSession);
}

WSError SceneSessionManager::RequestFocusBasicCheck(int32_t persistentId)
{
    // basic focus rule
    if (persistentId == INVALID_SESSION_ID) {
        WLOGFE("[WMSFocus]id is invalid!");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (persistentId == focusedSessionId_) {
        WLOGFD("[WMSFocus]request id has been focused!");
        return WSError::WS_DO_NOTHING;
    }
    return WSError::WS_OK;
}

WSError SceneSessionManager::RequestFocusSpecificCheck(sptr<SceneSession>& sceneSession, bool byForeground)
{
    int32_t persistentId = sceneSession->GetPersistentId();
    // dialog get focus
    if ((sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW ||
        SessionHelper::IsSubWindow(sceneSession->GetWindowType())) &&
        ProcessDialogRequestFocusImmdediately(sceneSession) == WSError::WS_OK) {
            WLOGFD("[WMSFocus]dialog is focused");
            return WSError::WS_DO_NOTHING;
    }
    // blocking-type session will block lower zOrder request focus
    auto focusedSession = GetSceneSession(focusedSessionId_);
    if (focusedSession) {
        bool isBlockingType = focusedSession->IsAppSession() ||
            (focusedSession->GetSessionInfo().isSystem_ && focusedSession->GetBlockingFocus());
        if (byForeground && isBlockingType && sceneSession->GetZOrder() < focusedSession->GetZOrder()) {
            WLOGFD("[WMSFocus]session %{public}d is lower than focused session %{public}d",
                persistentId, focusedSessionId_);
            return WSError::WS_DO_NOTHING;
        }
    }
    return WSError::WS_OK;
}

sptr<SceneSession> SceneSessionManager::GetNextFocusableSession(int32_t persistentId)
{
    WLOGFD("[WMSFocus]GetNextFocusableSession, id: %{public}d", persistentId);
    bool previousFocusedSessionFound = false;
    sptr<SceneSession> ret = nullptr;
    auto func = [this, persistentId, &previousFocusedSessionFound, &ret](sptr<SceneSession> session) {
        if (session == nullptr) {
            return false;
        }
        bool parentVisible = true;
        if ((WindowHelper::IsSubWindow(session->GetWindowType()) ||
            session->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) &&
            GetSceneSession(session->GetParentPersistentId()) &&
            !IsSessionVisible(GetSceneSession(session->GetParentPersistentId()))) {
                parentVisible = false;
        }
        if (previousFocusedSessionFound && session->GetFocusable() && IsSessionVisible(session) && parentVisible) {
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

void SceneSessionManager::SetShiftFocusListener(const ProcessShiftFocusFunc& func)
{
    WLOGFD("[WMSFocus]SetShiftFocusListener");
    shiftFocusFunc_ = func;
}

void SceneSessionManager::SetSCBFocusedListener(const NotifySCBAfterUpdateFocusFunc& func)
{
    WLOGFD("[WMSFocus]SetSCBFocusedListener");
    notifySCBAfterFocusedFunc_ = func;
}

void SceneSessionManager::SetSCBUnfocusedListener(const NotifySCBAfterUpdateFocusFunc& func)
{
    WLOGFD("[WMSFocus]SetSCBUnfocusedListener");
    notifySCBAfterUnfocusedFunc_ = func;
}

void SceneSessionManager::SetShowPiPMainWindowListener(const ProcessShowPiPMainWindowFunc& func)
{
    WLOGFD("SetShowPiPMainWindowListener");
    showPiPMainWindowFunc_ = func;
}

void SceneSessionManager::SetCallingWindowIdChangeListenser(const ProcessCallingWindowIdChangeFunc& func)
{
    WLOGFD("SetCallingWindowIdChangeListenser");
    callingWindowIdChangeFunc_ = func;
}

WSError SceneSessionManager::ShiftFocus(sptr<SceneSession>& nextSession)
{
    // unfocus
    int32_t focusedId = focusedSessionId_;
    auto focusedSession = GetSceneSession(focusedSessionId_);
    UpdateFocusStatus(focusedSession, false);
    // focus
    int32_t nextId = INVALID_SESSION_ID;
    if (nextSession == nullptr) {
        std::string sessionLog(GetAllSessionFocusInfo());
        WLOGFW("[WMSFocus]ShiftFocus to nullptr! id: %{public}d, info: %{public}s", focusedSessionId_, sessionLog.c_str());
    } else {
        nextId = nextSession->GetPersistentId();
    }
    UpdateFocusStatus(nextSession, true);
    bool scbPrevFocus = focusedSession && focusedSession->GetSessionInfo().isSystem_;
    bool scbCurrFocus = nextSession && nextSession->GetSessionInfo().isSystem_;
    if (!scbPrevFocus && scbCurrFocus) {
        if (notifySCBAfterFocusedFunc_ != nullptr) {
            notifySCBAfterFocusedFunc_();
        }
    } else if (scbPrevFocus && !scbCurrFocus) {
        if (notifySCBAfterUnfocusedFunc_ != nullptr) {
            notifySCBAfterUnfocusedFunc_();
        }
    }
    WLOGFI("[WMSFocus]ShiftFocus, focusedId: %{public}d, nextId: %{public}d", focusedId, nextId);
    return WSError::WS_OK;
}

void SceneSessionManager::UpdateFocusStatus(sptr<SceneSession>& sceneSession, bool isFocused)
{
    if (sceneSession == nullptr) {
        if (isFocused) {
            SetFocusedSession(INVALID_SESSION_ID);
        }
        return;
    }
    // name
    std::string sName;
    if (sceneSession->GetSessionInfo().isSystem_) {
        sName = sceneSession->GetSessionInfo().abilityName_;
    } else {
        sName = sceneSession->GetWindowName();
    }
    WLOGFI("[WMSFocus]UpdateFocusStatus, name: %{public}s, id: %{public}d, isFocused: %{public}d",
        sName.c_str(), sceneSession->GetPersistentId(), isFocused);
    // set focused
    if (isFocused) {
        SetFocusedSession(sceneSession->GetPersistentId());
    }
    sceneSession->UpdateFocus(isFocused);
    if ((isFocused && !needBlockNotifyFocusStatusUntilForeground_) || (!isFocused && !needBlockNotifyUnfocusStatus_)) {
        NotifyFocusStatus(sceneSession, isFocused);
    }
}

void SceneSessionManager::NotifyFocusStatus(sptr<SceneSession>& sceneSession, bool isFocused)
{
    int32_t persistentId = sceneSession->GetPersistentId();
    WLOGFI("[WMSFocus]NotifyFocusStatus, id: %{public}d, isFocused: %{public}d",
        sceneSession->GetPersistentId(), isFocused);
    if (isFocused) {
        if (sceneSession && IsSessionVisible(sceneSession)) {
            NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_FOCUSED);
        }
        UpdateBrightness(focusedSessionId_);
        FocusIDChange(sceneSession->GetPersistentId(), sceneSession);
        if (shiftFocusFunc_ != nullptr) {
            shiftFocusFunc_(persistentId);
        }
    }
    // notify window manager
    sptr<FocusChangeInfo> focusChangeInfo = new FocusChangeInfo(
        sceneSession->GetWindowId(),
        static_cast<DisplayId>(0),
        sceneSession->GetCallingPid(),
        sceneSession->GetCallingUid(),
        sceneSession->GetWindowType(),
        sceneSession->GetAbilityToken()
    );
#ifdef EFFICIENCY_MANAGER_ENABLE
        SuspendManager::SuspendManagerClient::GetInstance().ThawOneApplication(focusChangeInfo->uid_,
            "", "THAW_BY_FOCUS_CHANGED");
#endif // EFFICIENCY_MANAGER_ENABLE
    SessionManagerAgentController::GetInstance().UpdateFocusChangeInfo(focusChangeInfo, isFocused);
    WSError res = WSError::WS_OK;
    res = sceneSession->NotifyFocusStatus(isFocused);
    if (res != WSError::WS_OK) {
        return;
    }
    // notify listenerController
    if (listenerController_ != nullptr && !sceneSession->GetSessionInfo().isSystem_) {
        if (isFocused) {
            WLOGFD("[WMSFocus]NotifySessionFocused, id: %{public}d", sceneSession->GetPersistentId());
            listenerController_->NotifySessionFocused(sceneSession->GetPersistentId());
        } else {
            WLOGFD("[WMSFocus]NotifySessionUnfocused, id: %{public}d", sceneSession->GetPersistentId());
            listenerController_->NotifySessionUnfocused(sceneSession->GetPersistentId());
        }
    }
    return;
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
            SetFocusedSession(persistentId);
            UpdateBrightness(focusedSessionId_);
            FocusIDChange(persistentId, sceneSession);
        } else if (persistentId == GetFocusedSession()) {
            SetFocusedSession(INVALID_SESSION_ID);
        }
        // notify window manager
        sptr<FocusChangeInfo> focusChangeInfo = new FocusChangeInfo(
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
        WLOGFI("UpdateFocus, id: %{public}d, isSystem: %{public}d", sceneSession->GetPersistentId(),
               sceneSession->GetSessionInfo().isSystem_);
        if (listenerController_ != nullptr && !sceneSession->GetSessionInfo().isSystem_) {
            if (isFocused) {
                WLOGFD("NotifySessionFocused, id: %{public}d", sceneSession->GetPersistentId());
                listenerController_->NotifySessionFocused(sceneSession->GetPersistentId());
            } else {
                WLOGFD("NotifySessionUnfocused, id: %{public}d", sceneSession->GetPersistentId());
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
    WLOGFD("update window mode, id: %{public}d, mode: %{public}d", persistentId, windowMode);
    auto sceneSession = GetSceneSession(persistentId);
    if (sceneSession == nullptr) {
        WLOGFE("could not find window, persistentId:%{public}d", persistentId);
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
    uint32_t targetZIndex = 0;
    sptr<SceneSession> targetSession;
    MMI::PointerEvent::PointerItem pointerItem;
    if (!pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
        WLOGFE("Failed to get pointerItem");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    auto windowX = pointerItem.GetWindowX();
    auto windowY = pointerItem.GetWindowY();
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SendTouchEvent [%d, %d]", windowX, windowY);
    {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (const auto& [id, session] : sceneSessionMap_) {
            if (!session || session->IsSystemSession() || !session->GetTouchable() || !IsSessionVisible(session)) {
                continue;
            }
            auto zOrder = session->GetZOrder();
            if (zOrder <= targetZIndex || zOrder >= zIndex) {
                continue;
            }
            if (!session->GetSessionRect().IsInRegion(windowX, windowY)) {
                continue;
            }
            targetZIndex = zOrder;
            targetSession = session;
        }
    }
    if (!targetSession) {
        return WSError::WS_DO_NOTHING;
    }
    WLOGFI("Send touch event to session with id: %{public}d, zIndex: %{public}u, "
        "windowX: %{public}d, windowY: %{public}d", targetSession->GetPersistentId(), targetZIndex, windowX, windowY);
#ifdef SECURITY_COMPONENT_MANAGER_ENABLE
    FillSecCompEnhanceData(pointerEvent, pointerItem);
#endif
    targetSession->TransferPointerEvent(pointerEvent);
    return WSError::WS_OK;
}

void SceneSessionManager::SetScreenLocked(const bool isScreenLocked)
{
    isScreenLocked_ = isScreenLocked;
}

bool SceneSessionManager::IsScreenLocked() const
{
    return isScreenLocked_;
}

void SceneSessionManager::RegisterWindowChanged(const WindowChangedFunc& func)
{
    WindowChangedFunc_ = func;
}

void SceneSessionManager::UpdatePrivateStateAndNotify(uint32_t persistentId)
{
    int counts = GetSceneSessionPrivacyModeCount();
    bool hasPrivateWindow = (counts != 0);
    ScreenSessionManagerClient::GetInstance().SetScreenPrivacyState(hasPrivateWindow);
}

int SceneSessionManager::GetSceneSessionPrivacyModeCount()
{
    auto countFunc = [](const std::pair<int32_t, sptr<SceneSession>>& sessionPair) -> bool {
        sptr<SceneSession> sceneSession = sessionPair.second;
        bool isForeground =  sceneSession->GetSessionState() == SessionState::STATE_FOREGROUND ||
            sceneSession->GetSessionState() == SessionState::STATE_ACTIVE;
        if (isForeground && sceneSession->GetParentSession() != nullptr) {
            isForeground &= sceneSession->GetParentSession()->GetSessionState() == SessionState::STATE_FOREGROUND ||
            sceneSession->GetParentSession()->GetSessionState() == SessionState::STATE_ACTIVE;
        }
        bool isPrivate = sceneSession->GetSessionProperty() != nullptr &&
            sceneSession->GetSessionProperty()->GetPrivacyMode();
        bool IsSystemWindowVisible = sceneSession->GetSessionInfo().isSystem_ && sceneSession->IsVisible();
        return (isForeground || IsSystemWindowVisible) && isPrivate;
    };
    std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
    return std::count_if(sceneSessionMap_.begin(), sceneSessionMap_.end(), countFunc);
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

void SceneSessionManager::RegisterRequestFocusStatusNotifyManagerFunc(sptr<SceneSession>& sceneSession)
{
    NotifyRequestFocusStatusNotifyManagerFunc func = [this](int32_t persistentId, const bool isFocused) {
        this->RequestFocusStatus(persistentId, isFocused);
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

void SceneSessionManager::OnSessionStateChange(int32_t persistentId, const SessionState& state)
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
            if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW &&
                persistentId == focusedSessionId_) {
                if (needBlockNotifyFocusStatusUntilForeground_) {
                    needBlockNotifyUnfocusStatus_ = false;
                    needBlockNotifyFocusStatusUntilForeground_ = false;
                    NotifyFocusStatus(sceneSession, true);
                }
            } else {
                RequestSessionFocus(persistentId, true);
            }
            UpdateForceHideState(sceneSession, sceneSession->GetSessionProperty(), true);
            NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_ADDED);
            HandleKeepScreenOn(sceneSession, sceneSession->IsKeepScreenOn());
            UpdatePrivateStateAndNotify(persistentId);
            if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                ProcessSubSessionForeground(sceneSession);
            }
            if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_PIP) {
                ProcessPiPSessionForeground(sceneSession);
            }
            break;
        case SessionState::STATE_BACKGROUND:
            RequestSessionUnfocus(persistentId);
            UpdateForceHideState(sceneSession, sceneSession->GetSessionProperty(), false);
            NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_REMOVED);
            HandleKeepScreenOn(sceneSession, false);
            UpdatePrivateStateAndNotify(persistentId);
            if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                ProcessSubSessionBackground(sceneSession);
            }
            break;
        default:
            break;
    }
}

void SceneSessionManager::ProcessSubSessionForeground(sptr<SceneSession>& sceneSession)
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
        NotifyWindowInfoChange(subSession->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_ADDED);
        HandleKeepScreenOn(subSession, subSession->IsKeepScreenOn());
    }
    std::vector<sptr<Session>> dialogVec = sceneSession->GetDialogVector();
    for (const auto& dialog : dialogVec) {
        if (dialog == nullptr) {
            WLOGFD("[WMSDialog] dialog is nullptr");
            continue;
        }
        const auto& state = dialog->GetSessionState();
        if (state != SessionState::STATE_FOREGROUND && state != SessionState::STATE_ACTIVE) {
            WLOGFD("[WMSDialog] dialog is not active");
            continue;
        }
        auto dialogSession = GetSceneSession(dialog->GetPersistentId());
        NotifyWindowInfoChange(dialog->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_ADDED);
        HandleKeepScreenOn(dialogSession, dialogSession->IsKeepScreenOn());
    }
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
        WLOGFD("[WMSFocus]main window is nullptr");
        return WSError::WS_DO_NOTHING;
    }
    std::vector<sptr<Session>> dialogVec = mainSession->GetDialogVector();
    if (std::find_if(dialogVec.begin(), dialogVec.end(),
        [this](sptr<Session>& iter) { return iter && iter->GetPersistentId() == focusedSessionId_; })
        != dialogVec.end()) {
            WLOGFD("[WMSDialog] dialog id: %{public}d has been focused!", focusedSessionId_);
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
            WLOGFD("[WMSDialog] dialog is nullptr");
            continue;
        }
        auto dialogSession = GetSceneSession(dialog->GetPersistentId());
        NotifyWindowInfoChange(dialog->GetPersistentId(), WindowUpdateType::WINDOW_UPDATE_REMOVED);
        HandleKeepScreenOn(dialogSession, false);
        UpdatePrivateStateAndNotify(dialog->GetPersistentId());
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
            if (!session || !session->GetSessionProperty()) {
                continue;
            }
            bool hasWaterMark = session->GetSessionProperty()->GetWindowFlags()
                & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_WATER_MARK);
            if (hasWaterMark && session->GetVisible()) {
                currentWaterMarkShowState = true;
                break;
            }
        }
    }
    if (lastWaterMarkShowState_ != currentWaterMarkShowState) {
        lastWaterMarkShowState_ = currentWaterMarkShowState;
        NotifyWaterMarkFlagChangedResult(currentWaterMarkShowState);
    }
    return;
}

WSError SceneSessionManager::NotifyWaterMarkFlagChangedResult(bool hasWaterMark)
{
    WLOGFI("WaterMark status : %{public}u", static_cast<uint32_t>(hasWaterMark));
    SessionManagerAgentController::GetInstance().NotifyWaterMarkFlagChangedResult(hasWaterMark);
    return WSError::WS_OK;
}

void SceneSessionManager::ProcessPreload(const AppExecFwk::AbilityInfo &abilityInfo) const
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
    WLOGFI("[WMSMain] NotifyCompleteFirstFrameDrawing, persistentId: %{public}d", persistentId);
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        WLOGFE("[WMSMain] scnSession is nullptr.");
        return;
    }

    const auto& sessionInfo = scnSession->GetSessionInfo();
    auto abilityInfoPtr = sessionInfo.abilityInfo;
    if (abilityInfoPtr == nullptr) {
        WLOGFE("[WMSMain] abilityInfoPtr is nullptr, persistentId: %{public}d", persistentId);
        return;
    }
    WLOGFI("[WMSMain] NotifyCompleteFirstFrameDrawing, id: %{public}d, isSystem: %{public}d, appName: [%{public}s "
        "%{public}s %{public}s]", scnSession->GetPersistentId(), sessionInfo.isSystem_,
        sessionInfo.bundleName_.c_str(), sessionInfo.moduleName_.c_str(), sessionInfo.abilityName_.c_str());
    if ((listenerController_ != nullptr) && !scnSession->GetSessionInfo().isSystem_ &&
        !(abilityInfoPtr->excludeFromMissions)) {
        WLOGFD("NotifySessionCreated, id: %{public}d", persistentId);
        listenerController_->NotifySessionCreated(persistentId);
    }

    if (taskScheduler_ == nullptr) {
        return;
    }
    auto task = [this, abilityInfoPtr]() {
        ProcessPreload(*abilityInfoPtr);
    };
    return taskScheduler_->PostAsyncTask(task, "NotifyCompleteFirstFrameDrawing" + std::to_string(persistentId));
}

void SceneSessionManager::NotifySessionMovedToFront(int32_t persistentId)
{
    WLOGFI("NotifySessionMovedToFront, persistentId: %{public}d", persistentId);
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        WLOGFE("session is invalid with %{public}d", persistentId);
        return;
    }
    WLOGFI("NotifySessionMovedToFront, id: %{public}d, isSystem: %{public}d", scnSession->GetPersistentId(),
           scnSession->GetSessionInfo().isSystem_);
    if (listenerController_ != nullptr &&
        !scnSession->GetSessionInfo().isSystem_ &&
        (scnSession->GetSessionInfo().abilityInfo) != nullptr &&
        !(scnSession->GetSessionInfo().abilityInfo)->excludeFromMissions) {
        listenerController_->NotifySessionMovedToFront(persistentId);
    }
}

WSError SceneSessionManager::SetSessionLabel(const sptr<IRemoteObject> &token, const std::string &label)
{
    WLOGFI("run SetSessionLabel");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }

    auto task = [this, &token, &label]() {
        auto sceneSession = FindSessionByToken(token);
        if (sceneSession == nullptr) {
            WLOGFI("fail to find session by token");
            return WSError::WS_ERROR_SET_SESSION_LABEL_FAILED;
        }
        sceneSession->SetSessionLabel(label);
        if (sessionListener_ != nullptr) {
            WLOGFI("try to run OnSessionLabelChange");
            sessionListener_->OnSessionLabelChange(sceneSession->GetPersistentId(), label);
        }
        WLOGFI("NotifySessionLabelUpdated, id: %{public}d, isSystem: %{public}d", sceneSession->GetPersistentId(),
            sceneSession->GetSessionInfo().isSystem_);
        if (listenerController_ != nullptr && !sceneSession->GetSessionInfo().isSystem_) {
            WLOGFD("NotifySessionLabelUpdated, id: %{public}d", sceneSession->GetPersistentId());
            listenerController_->NotifySessionLabelUpdated(sceneSession->GetPersistentId());
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetSessionLabel");
}

WSError SceneSessionManager::SetSessionIcon(const sptr<IRemoteObject> &token,
    const std::shared_ptr<Media::PixelMap> &icon)
{
    WLOGFI("run SetSessionIcon");
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
        if (sessionListener_ != nullptr) {
            WLOGFI("try to run OnSessionIconChange.");
            sessionListener_->OnSessionIconChange(sceneSession->GetPersistentId(), icon);
        }
        WLOGFI("NotifySessionIconChanged, id: %{public}d, isSystem: %{public}d", sceneSession->GetPersistentId(),
            sceneSession->GetSessionInfo().isSystem_);
        if (listenerController_ != nullptr &&
            !sceneSession->GetSessionInfo().isSystem_ &&
            (sceneSession->GetSessionInfo().abilityInfo) != nullptr &&
            !(sceneSession->GetSessionInfo().abilityInfo)->excludeFromMissions) {
            WLOGFD("NotifySessionIconChanged, id: %{public}d", sceneSession->GetPersistentId());
            listenerController_->NotifySessionIconChanged(sceneSession->GetPersistentId(), icon);
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetSessionIcon");
}

WSError SceneSessionManager::IsValidSessionIds(
    const std::vector<int32_t> &sessionIds, std::vector<bool> &results)
{
    WLOGFI("run IsValidSessionIds");
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
    WLOGFI("run RegisterSessionListener");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &listener]() {
        if (listenerController_ != nullptr) {
            return listenerController_->AddSessionListener(listener);
        } else {
            WLOGFE("The listenerController is nullptr");
            return WSError::WS_DO_NOTHING;
        }
    };
    return taskScheduler_->PostSyncTask(task, "AddSessionListener");
}

WSError SceneSessionManager::UnRegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFI("run UnRegisterSessionListener");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, &listener]() {
        if (listenerController_ != nullptr) {
            listenerController_->DelSessionListener(listener);
            return WSError::WS_OK;
        } else {
            WLOGFE("The listenerController is nullptr");
            return WSError::WS_DO_NOTHING;
        }
    };
    return taskScheduler_->PostSyncTask(task, "DelSessionListener");
}

WSError SceneSessionManager::GetSessionInfos(const std::string& deviceId, int32_t numMax,
                                             std::vector<SessionInfoBean>& sessionInfos)
{
    WLOGFI("run GetSessionInfos");
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
                WLOGFD("sessionId: %{public}d  isSystemScene", sceneSession->GetPersistentId());
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

int SceneSessionManager::GetRemoteSessionInfos(const std::string& deviceId, int32_t numMax,
                                               std::vector<SessionInfoBean>& sessionInfos)
{
    WLOGFI("GetRemoteSessionInfos From Dms begin");
    DistributedClient dmsClient;
    int result = dmsClient.GetMissionInfos(deviceId, numMax, sessionInfos);
    if (result != ERR_OK) {
        WLOGFE("GetRemoteMissionInfos failed, result = %{public}d", result);
        return result;
    }
    return ERR_OK;
}

WSError SceneSessionManager::GetSessionInfo(const std::string& deviceId,
                                            int32_t persistentId, SessionInfoBean& sessionInfo)
{
    WLOGFI("run GetSessionInfo");
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

WSError SceneSessionManager::DumpSessionAll(std::vector<std::string> &infos)
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

WSError SceneSessionManager::DumpSessionWithId(int32_t persistentId, std::vector<std::string> &infos)
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

WSError SceneSessionManager::GetAllAbilityInfos(const AAFwk::Want &want, int32_t userId,
    std::vector<AppExecFwk::AbilityInfo> &abilityInfos)
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
    return GetAbilityInfosFromBundleInfo(bundleInfos, abilityInfos);
}

WSError SceneSessionManager::GetAbilityInfosFromBundleInfo(std::vector<AppExecFwk::BundleInfo> &bundleInfos,
    std::vector<AppExecFwk::AbilityInfo> &abilityInfos)
{
    if (bundleInfos.empty()) {
        WLOGFE("bundleInfos is empty");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    for (auto bundleInfo: bundleInfos) {
        auto hapModulesList = bundleInfo.hapModuleInfos;
        if (hapModulesList.empty()) {
            WLOGFD("hapModulesList is empty");
            continue;
        }
        for (auto hapModule: hapModulesList) {
            auto abilityInfoList = hapModule.abilityInfos;
            abilityInfos.insert(abilityInfos.end(), abilityInfoList.begin(), abilityInfoList.end());
        }
    }
    return WSError::WS_OK;
}

WSError SceneSessionManager::TerminateSessionNew(const sptr<AAFwk::SessionInfo> info, bool needStartCaller)
{
    WLOGFI("run TerminateSessionNew");
    if (info == nullptr) {
        WLOGFI("sessionInfo is nullptr.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    sptr<SceneSession> sceneSession = FindSessionByToken(info->sessionToken);
    if (sceneSession == nullptr) {
        WLOGFI("fail to find session by token.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    const WSError& errCode = sceneSession->TerminateSessionNew(info, needStartCaller);
    return errCode;
}

WSError SceneSessionManager::GetSessionSnapshot(const std::string& deviceId, int32_t persistentId,
                                                SessionSnapshot& snapshot, bool isLowResolution)
{
    WLOGFI("run GetSessionSnapshot");
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

int SceneSessionManager::GetRemoteSessionSnapshotInfo(const std::string& deviceId, int32_t sessionId,
                                                      AAFwk::MissionSnapshot& sessionSnapshot)
{
    WLOGFI("GetRemoteSessionSnapshotInfo begin");
    std::unique_ptr<AAFwk::MissionSnapshot> sessionSnapshotPtr = std::make_unique<AAFwk::MissionSnapshot>();
    DistributedClient dmsClient;
    int result = dmsClient.GetRemoteMissionSnapshotInfo(deviceId, sessionId, sessionSnapshotPtr);
    if (result != ERR_OK) {
        WLOGFE("GetRemoteMissionSnapshotInfo failed, result = %{public}d", result);
        return result;
    }
    sessionSnapshot = *sessionSnapshotPtr;
    return ERR_OK;
}

WSError SceneSessionManager::RegisterSessionListener(const sptr<ISessionChangeListener> sessionListener)
{
    WLOGFI("run RegisterSessionListener");
    if (sessionListener == nullptr) {
        return WSError::WS_ERROR_INVALID_SESSION_LISTENER;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, sessionListener]() {
        sessionListener_ = sessionListener;
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "RegisterSessionListener");
}

void SceneSessionManager::UnregisterSessionListener()
{
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return;
    }
    auto task = [this]() {
        sessionListener_ = nullptr;
    };
    taskScheduler_->PostAsyncTask(task, "UnregisterSessionListener");
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
        WLOGFI("[WMSMain]RequestSceneSessionByCall persistentId: %{public}d", persistentId);
        if (!GetSceneSession(persistentId)) {
            WLOGFE("session is invalid with %{public}d", persistentId);
            return WSError::WS_ERROR_INVALID_SESSION;
        }
        auto sessionInfo = scnSession->GetSessionInfo();
        auto abilitySessionInfo = SetAbilitySessionInfo(scnSession);
        if (!abilitySessionInfo) {
            return WSError::WS_ERROR_NULLPTR;
        }
        if (sessionInfo.callState_ == static_cast<uint32_t>(AAFwk::CallToState::BACKGROUND)) {
            scnSession->SetActive(false);
        } else if (sessionInfo.callState_ == static_cast<uint32_t>(AAFwk::CallToState::FOREGROUND)) {
            scnSession->SetActive(true);
        } else {
            WLOGFE("wrong callState_");
        }
        WLOGFI("[WMSMain]RequestSceneSessionByCall callState:%{public}d, persistentId: %{public}d",
            sessionInfo.callState_, persistentId);
        AAFwk::AbilityManagerClient::GetInstance()->CallUIAbilityBySCB(abilitySessionInfo);
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
        AAFwk::AbilityManagerClient::GetInstance()->StartSpecifiedAbilityBySCB(want);
    };

    taskScheduler_->PostAsyncTask(task, "StartAbilityBySpecified:PID:" + sessionInfo.bundleName_);
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
        WLOGFE("[WMSDialog] BindDialogSessionTarget permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (targetToken == nullptr) {
        WLOGFE("[WMSDialog] Target token is null");
        return WSError::WS_ERROR_NULLPTR;
    }

    auto task = [this, persistentId, targetToken]() {
        auto scnSession = GetSceneSession(static_cast<int32_t>(persistentId));
        if (scnSession == nullptr) {
            WLOGFE("[WMSDialog] Session is nullptr, persistentId:%{public}" PRIu64, persistentId);
            return WSError::WS_ERROR_NULLPTR;
        }
        if (scnSession->GetWindowType() != WindowType::WINDOW_TYPE_DIALOG) {
            WLOGFE("[WMSDialog] Session is not dialog window, window type:%{public}u", scnSession->GetWindowType());
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
        WLOGFI("[WMSDialog] Bind dialog success, dialog id %{public}" PRIu64 ", parentId %{public}d",
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
    if (type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_GESTURE_NAVIGATION_ENABLED ||
        type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_WATER_MARK_FLAG) {
        if (!SessionPermission::IsSystemCalling()) {
            WLOGFE("RegisterWindowManagerAgent permission denied!");
            return WMError::WM_ERROR_NOT_SYSTEM_APP;
        }
    }
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto task = [this, &windowManagerAgent, type]() {
        return SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(windowManagerAgent, type);
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
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto task = [this, &windowManagerAgent, type]() {
        return SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(windowManagerAgent, type);
    };
    return taskScheduler_->PostSyncTask(task, "UnregisterWindowManagerAgent");
}

void SceneSessionManager::UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    SessionManagerAgentController::GetInstance().UpdateCameraFloatWindowStatus(accessTokenId, isShowing);
}

void SceneSessionManager::StartWindowInfoReportLoop()
{
    WLOGFD("Report loop");
    if (eventHandler_ == nullptr) {
        WLOGFE("Report event null");
        return ;
    }
    if (isReportTaskStart_) {
        WLOGFE("Report is ReportTask Start");
        return;
    }
    auto task = [this]() {
        WindowInfoReporter::GetInstance().ReportRecordedInfos();
        isReportTaskStart_ = false;
        StartWindowInfoReportLoop();
    };
    int64_t delayTime = 1000 * 60 * 60; // an hour.
    bool ret = eventHandler_->PostTask(task, "wms:WindowInfoReport", delayTime);
    if (!ret) {
        WLOGFE("Report post listener callback task failed. the task name is WindowInfoReport");
        return;
    }
    isReportTaskStart_ = true;
}

int32_t SceneSessionManager::GetStatusBarHeight()
{
    int32_t statusBarHeight = 0;
    int32_t height = 0;
    std::vector<sptr<SceneSession>> statusBarVector = GetSceneSessionVectorByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    for (auto& statusBar : statusBarVector) {
        if (statusBar == nullptr || !IsSessionVisible(statusBar)) {
            continue;
        }
        height = statusBar->GetSessionRect().height_;
        statusBarHeight = (statusBarHeight > height) ? statusBarHeight : height;
    }

    return statusBarHeight;
}

void SceneSessionManager::ResizeSoftInputCallingSessionIfNeed(
    const sptr<SceneSession>& sceneSession, bool isInputUpdated)
{
    if (callingSession_ == nullptr) {
        WLOGFI("[WMSInput] calling session is nullptr");
        return;
    }
    SessionGravity gravity;
    uint32_t percent = 0;
    sceneSession->GetSessionProperty()->GetSessionGravity(gravity, percent);
    if (gravity != SessionGravity::SESSION_GRAVITY_BOTTOM && gravity != SessionGravity::SESSION_GRAVITY_DEFAULT) {
        WLOGFI("[WMSInput] input method window gravity is not bottom, no need to raise calling window");
        return;
    }

    bool isCallingSessionFloating;
    if (callingSession_->GetSessionProperty() &&
        callingSession_->GetSessionProperty()->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING) {
        isCallingSessionFloating = true;
    } else {
        isCallingSessionFloating = false;
    }

    const WSRect& softInputSessionRect = sceneSession->GetSessionRect();
    WSRect callingSessionRect;
    if (isInputUpdated && isCallingSessionFloating) {
        callingSessionRect = callingWindowRestoringRect_;
    } else {
        callingSessionRect = callingSession_->GetSessionRect();
    }
    if (SessionHelper::IsEmptyRect(SessionHelper::GetOverlap(softInputSessionRect, callingSessionRect, 0, 0))) {
        WLOGFD("[WMSInput] There is no overlap area");
        return;
    }

    WSRect newRect = callingSessionRect;
    if (isCallingSessionFloating) {
        // calculate new rect of calling window
        newRect.posY_ = softInputSessionRect.posY_ - static_cast<int32_t>(newRect.height_);
        int32_t statusHeight = GetStatusBarHeight();
        newRect.posY_ = std::max(newRect.posY_, statusHeight);
    }

    if (!isInputUpdated) {
        callingWindowRestoringRect_ = callingSessionRect;
    }
    NotifyOccupiedAreaChangeInfo(sceneSession, newRect, softInputSessionRect);
    if (isCallingSessionFloating) {
        needUpdateSessionRect_ = true;
        callingSession_->UpdateSessionRect(newRect, SizeChangeReason::UNDEFINED);
    }
}

void SceneSessionManager::NotifyOccupiedAreaChangeInfo(const sptr<SceneSession> sceneSession,
    const WSRect& rect, const WSRect& occupiedArea)
{
    // if keyboard will occupy calling, notify calling window the occupied area and safe height
    const WSRect& safeRect = SessionHelper::GetOverlap(occupiedArea, rect, 0, 0);
    const WSRect& lastSafeRect = callingSession_->GetLastSafeRect();
    if (lastSafeRect == safeRect) {
        WLOGFI("[WMSInput] NotifyOccupiedAreaChangeInfo lastSafeRect is same to safeRect");
        return;
    }
    callingSession_->SetLastSafeRect(safeRect);
    sptr<OccupiedAreaChangeInfo> info = new OccupiedAreaChangeInfo(OccupiedAreaType::TYPE_INPUT,
        SessionHelper::TransferToRect(safeRect), safeRect.height_,
        sceneSession->textFieldPositionY_, sceneSession->textFieldHeight_);
    WLOGFD("[WMSInput] OccupiedAreaChangeInfo rect: %{public}u %{public}u %{public}u %{public}u",
        occupiedArea.posX_, occupiedArea.posY_, occupiedArea.width_, occupiedArea.height_);
    callingSession_->NotifyOccupiedAreaChangeInfo(info);
}

void SceneSessionManager::RestoreCallingSessionSizeIfNeed()
{
    WLOGFD("[WMSInput] RestoreCallingSessionSizeIfNeed");
    if (callingSession_ == nullptr) {
        WLOGFI("[WMSInput] Calling session is nullptr");
        return;
    }
    if (!SessionHelper::IsEmptyRect(callingWindowRestoringRect_)) {
        WSRect overlapRect = { 0, 0, 0, 0 };
        NotifyOccupiedAreaChangeInfo(callingSession_, callingWindowRestoringRect_, overlapRect);
        if (needUpdateSessionRect_ && callingSession_->GetSessionProperty() &&
            callingSession_->GetSessionProperty()->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING) {
            callingSession_->UpdateSessionRect(callingWindowRestoringRect_, SizeChangeReason::UNDEFINED);
        }
    }
    needUpdateSessionRect_ = false;
    callingWindowRestoringRect_ = { 0, 0, 0, 0 };
    callingSession_ = nullptr;
}

WSError SceneSessionManager::SetSessionGravity(int32_t persistentId, SessionGravity gravity, uint32_t percent)
{
    auto task = [this, persistentId, gravity, percent]() -> WSError {
        auto sceneSession = GetSceneSession(persistentId);
        if (!sceneSession) {
            WLOGFE("scene session is nullptr");
            return WSError::WS_ERROR_NULLPTR;
        }
        if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
            WLOGFE("scene session is not input method");
            return WSError::WS_ERROR_INVALID_TYPE;
        }
        sceneSession->GetSessionProperty()->SetSessionGravity(gravity, percent);
        RelayoutKeyBoard(sceneSession);
        if (gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
            WLOGFD("input method is float mode");
            sceneSession->SetWindowAnimationFlag(false);
            RestoreCallingSessionSizeIfNeed();
        } else {
            WLOGFD("input method is bottom mode");
            sceneSession->SetWindowAnimationFlag(true);
            ResizeSoftInputCallingSessionIfNeed(sceneSession);
        }
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "SetSessionGravity" + std::to_string(persistentId));
}

void SceneSessionManager::RelayoutKeyBoard(sptr<SceneSession> sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("[WMSInput] sceneSession is nullptr");
        return;
    }
    SessionGravity gravity;
    uint32_t percent = 0;
    sceneSession->GetSessionProperty()->GetSessionGravity(gravity, percent);
    if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT ||
        gravity == SessionGravity::SESSION_GRAVITY_FLOAT) {
        return;
    }

    auto defaultDisplayInfo = DisplayManager::GetInstance().GetDefaultDisplay();
    if (defaultDisplayInfo == nullptr) {
        WLOGFE("[WMSInput] screenSession is null");
        return;
    }

    auto requestRect = sceneSession->GetSessionProperty()->GetRequestRect();
    if (gravity == SessionGravity::SESSION_GRAVITY_BOTTOM) {
        requestRect.width_ = static_cast<uint32_t>(defaultDisplayInfo->GetWidth());
        requestRect.posX_ = 0;
        if (percent != 0) {
            requestRect.height_ =
                static_cast<uint32_t>(defaultDisplayInfo->GetHeight()) * percent / 100u; // 100: for calc percent.
        }
    }
    requestRect.posY_ = defaultDisplayInfo->GetHeight() -
        static_cast<int32_t>(requestRect.height_);
    sceneSession->GetSessionProperty()->SetRequestRect(requestRect);
    WLOGFD("[WMSInput] Id: %{public}d, rect: %{public}s", sceneSession->GetPersistentId(),
        SessionHelper::TransferToWSRect(requestRect).ToString().c_str());
    sceneSession->UpdateSessionRect(SessionHelper::TransferToWSRect(requestRect), SizeChangeReason::UNDEFINED);
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
    WLOGFI("GetAccessibilityWindowInfo Called.");
    if (!SessionPermission::IsSystemServiceCalling()) {
        WLOGFE("The interface only support for system service.");
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
            WLOGFD("name = %{public}s, isSystem = %{public}d, persistendId = %{public}d, winType = %{public}d, "
                "state = %{public}d, visible = %{public}d", sceneSession->GetWindowName().c_str(),
                sceneSession->GetSessionInfo().isSystem_, iter->first, sceneSession->GetWindowType(),
                sceneSession->GetSessionState(), sceneSession->IsVisible());
            if (IsSessionVisible(sceneSession)) {
                FillWindowInfo(infos, iter->second);
            }
        }
        return WMError::WM_OK;
    };
    return taskScheduler_->PostSyncTask(task, "GetAccessibilityWindowInfo");
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
    auto task = [this, weakSceneSession, type]() {
        std::vector<sptr<AccessibilityWindowInfo>> infos;
        auto scnSession = weakSceneSession.promote();
        if (FillWindowInfo(infos, scnSession)) {
            SessionManagerAgentController::GetInstance().NotifyAccessibilityWindowInfo(infos, type);
        }
        if (WindowChangedFunc_ != nullptr && scnSession != nullptr &&
            scnSession->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            WindowChangedFunc_(scnSession->GetPersistentId(), type);
        }
    };
    taskScheduler_->PostAsyncTask(task, "NotifyWindowInfoChange:PID:" + std::to_string(persistentId));
}

bool SceneSessionManager::FillWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos,
    const sptr<SceneSession>& sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFW("null scene session.");
        return false;
    }
    if (sceneSession->GetSessionInfo().bundleName_.find("SCBGestureBack") != std::string::npos
        || sceneSession->GetSessionInfo().bundleName_.find("SCBGestureNavBar") != std::string::npos) {
        WLOGFD("filter gesture window.");
        return false;
    }
    sptr<AccessibilityWindowInfo> info = new (std::nothrow) AccessibilityWindowInfo();
    if (info == nullptr) {
        WLOGFE("null info.");
        return false;
    }
    if (sceneSession->GetSessionInfo().isSystem_) {
        info->wid_ = 1;
        info->innerWid_ = static_cast<int32_t>(sceneSession->GetPersistentId());
    } else {
        info->wid_ = static_cast<int32_t>(sceneSession->GetPersistentId());
    }
    info->uiNodeId_ = sceneSession->GetUINodeId();
    WSRect wsrect = sceneSession->GetSessionRect();
    info->windowRect_ = {wsrect.posX_, wsrect.posY_, wsrect.width_, wsrect.height_ };
    info->focused_ = sceneSession->GetPersistentId() == focusedSessionId_;
    info->type_ = sceneSession->GetWindowType();
    info->mode_ = sceneSession->GetWindowMode();
    info->layer_ = sceneSession->GetZOrder();
    info->scaleVal_ = sceneSession->GetScaleX();
    info->scaleX_ = sceneSession->GetScaleX();
    info->scaleY_ = sceneSession->GetScaleY();
    auto property = sceneSession->GetSessionProperty();
    if (property != nullptr) {
        info->displayId_ = property->GetDisplayId();
        info->isDecorEnable_ = property->IsDecorEnable();
    }
    infos.emplace_back(info);
    WLOGFD("wid = %{public}d, inWid = %{public}d, uiNId = %{public}d", info->wid_, info->innerWid_, info->uiNodeId_);
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
            DealwithVisibilityChange(visibilityChangeInfos);
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
        } else if (lastVisibleData_[i].first > currVisibleData[j].first) {
            visibilityChangeInfo.emplace_back(currVisibleData[j].first, currVisibleData[j].second);
            j++;
        } else {
            visibilityChangeInfo.emplace_back(currVisibleData[j].first, currVisibleData[j].second);
            i++;
            j++;
        }
    }
    for (; i < lastVisibleData_.size(); ++i) {
        visibilityChangeInfo.emplace_back(lastVisibleData_[i].first, WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION);
    }
    for (; j < currVisibleData.size(); ++j) {
        visibilityChangeInfo.emplace_back(currVisibleData[j].first, currVisibleData[j].second);
    }
    lastVisibleData_ = currVisibleData;
    return visibilityChangeInfo;
}

void SceneSessionManager::DealwithVisibilityChange(const std::vector<std::pair<uint64_t, WindowVisibilityState>>&
    visibilityChangeInfo)
{
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
#ifdef MEMMGR_WINDOW_ENABLE
    std::vector<sptr<Memory::MemMgrWindowInfo>> memMgrWindowInfos;
#endif

for (const auto& elem : visibilityChangeInfo) {
    uint64_t surfaceId = elem.first;
    WindowVisibilityState visibleState = elem.second;
    bool isVisible = visibleState < WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION;
    sptr<SceneSession> session = SelectSesssionFromMap(surfaceId);
    if (session == nullptr) {
        continue;
    }
    session->SetVisible(isVisible);
    int32_t windowId = session->GetWindowId();
    if (windowVisibilityListenerSessionSet_.find(windowId) != windowVisibilityListenerSessionSet_.end()) {
        session->NotifyWindowVisibility();
    }
    windowVisibilityInfos.emplace_back(new WindowVisibilityInfo(windowId, session->GetCallingPid(),
         session->GetCallingUid(), visibleState, session->GetWindowType()));
#ifdef MEMMGR_WINDOW_ENABLE
    memMgrWindowInfos.emplace_back(new Memory::MemMgrWindowInfo(session->GetWindowId(), session->GetCallingPid(),
        session->GetCallingUid(), isVisible));
#endif
    WLOGFD("Notify window visibility Change, window: name=%{public}s, id=%{public}u, visibleState:%{public}d",
        session->GetWindowName().c_str(), windowId, visibleState);
    CheckAndNotifyWaterMarkChangedResult();
}
    if (windowVisibilityInfos.size() != 0) {
        WLOGFD("Notify windowvisibilityinfo changed start");
        SessionManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
    }
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
        WLOGFD("NotifyWindowDrawingContenInfoChange: drawing status changed pid:%{public}d,"
            "surfaceId:%{public}" PRIu64", drawingState:%{public}d", session->GetCallingPid(), surfaceId, drawingState);
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
    for (auto& data : currDrawingContentData) {
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
    for (auto& item : sceneSessionMap_) {
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
    auto windowVisibilityChangeCb = std::bind(&SceneSessionManager::WindowLayerInfoChangeCallback, this,
        std::placeholders::_1);
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
        WLOGFE("NotifyWindowExtensionVisibilityChange permission denied!");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    WLOGFI("Notify WindowExtension visibility change to %{public}s for pid: %{public}d, uid: %{public}d",
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
    if (sceneSession->GetVisible()) {
        std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
#ifdef MEMMGR_WINDOW_ENABLE
        std::vector<sptr<Memory::MemMgrWindowInfo>> memMgrWindowInfos;
#endif
        sceneSession->SetVisible(false);
        windowVisibilityInfos.emplace_back(new WindowVisibilityInfo(sceneSession->GetWindowId(),
            sceneSession->GetCallingPid(), sceneSession->GetCallingUid(),
            WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION, sceneSession->GetWindowType()));
#ifdef MEMMGR_WINDOW_ENABLE
        memMgrWindowInfos.emplace_back(new Memory::MemMgrWindowInfo(sceneSession->GetWindowId(),
        sceneSession->GetCallingPid(), sceneSession->GetCallingUid(), false));
#endif
        WLOGFD("NotifyWindowVisibilityChange: covered status changed window:%{public}u, isVisible:%{public}d",
            sceneSession->GetWindowId(), sceneSession->GetVisible());
        CheckAndNotifyWaterMarkChangedResult();
        SessionManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
#ifdef MEMMGR_WINDOW_ENABLE
        WLOGD("Notify memMgrWindowInfos changed start");
        Memory::MemMgrClient::GetInstance().OnWindowVisibilityChanged(memMgrWindowInfos);
#endif
    }
}

sptr<SceneSession> SceneSessionManager::FindSessionByToken(const sptr<IRemoteObject> &token)
{
    sptr<SceneSession> session = nullptr;
    auto cmpFunc = [token](const std::map<uint64_t, sptr<SceneSession>>::value_type& pair) {
        if (pair.second == nullptr) {
            return false;
        }
        return pair.second->GetAbilityToken() == token || pair.second->GetSelfToken() == token;
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
    WLOGFI("run PreloadInLakeApp");
    auto iter = collaboratorMap_.find(CollaboratorType::RESERVE_TYPE);
    if (iter == collaboratorMap_.end()) {
        WLOGFE("Fail to found collaborator with type: RESERVE_TYPE");
        return;
    }
    auto collaborator = iter->second;
    if (collaborator != nullptr) {
        collaborator->NotifyPreloadAbility(bundleName);
    }
}

WSError SceneSessionManager::PendingSessionToForeground(const sptr<IRemoteObject> &token)
{
    WLOGFI("run PendingSessionToForeground");
    auto task = [this, &token]() {
        auto session = FindSessionByToken(token);
        if (session != nullptr) {
            return session->PendingSessionToForeground();
        }
        WLOGFE("fail to find token");
        return WSError::WS_ERROR_INVALID_PARAM;
    };
    return taskScheduler_->PostSyncTask(task, "PendingSessionToForeground");
}

WSError SceneSessionManager::PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject> &token)
{
    auto task = [this, &token]() {
        auto session = FindSessionByToken(token);
        if (session != nullptr) {
            return session->PendingSessionToBackgroundForDelegator();
        }
        WLOGFE("fail to find token");
        return WSError::WS_ERROR_INVALID_PARAM;
    };
    return taskScheduler_->PostSyncTask(task, "PendingSessionToBackgroundForDelegator");
}

WSError SceneSessionManager::GetFocusSessionToken(sptr<IRemoteObject> &token)
{
    auto task = [this, &token]() {
        WLOGFD("run GetFocusSessionToken with focusedSessionId: %{public}d", focusedSessionId_);
        auto sceneSession = GetSceneSession(focusedSessionId_);
        if (sceneSession) {
            token = sceneSession->GetAbilityToken();
            if (token == nullptr) {
                WLOGFE("token is nullptr");
                return WSError::WS_ERROR_INVALID_PARAM;
            }
            return WSError::WS_OK;
        }
        return WSError::WS_ERROR_INVALID_PARAM;
    };
    return taskScheduler_->PostSyncTask(task, "GetFocusSessionToken");
}

WSError SceneSessionManager::UpdateSessionAvoidAreaListener(int32_t& persistentId, bool haveListener)
{
    auto task = [this, persistentId, haveListener]() {
        WLOGFI("UpdateSessionAvoidAreaListener persistentId: %{public}d haveListener:%{public}d",
            persistentId, haveListener);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFD("sceneSession is nullptr.");
            return WSError::WS_DO_NOTHING;
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
        return false;
    }
    auto iter = lastUpdatedAvoidArea_.find(persistentId);
    bool needUpdate = true;

    if (iter != lastUpdatedAvoidArea_.end()) {
        auto avoidAreaIter = iter->second.find(avoidAreaType);
        if (avoidAreaIter != iter->second.end()) {
            needUpdate = avoidAreaIter->second != avoidArea;
        } else {
            if (avoidArea.isEmptyAvoidArea()) {
                needUpdate = false;
                return needUpdate;
            }
        }
    } else {
        if (avoidArea.isEmptyAvoidArea()) {
            needUpdate = false;
            return needUpdate;
        }
    }
    if (needUpdate ||
        avoidAreaType == AvoidAreaType::TYPE_SYSTEM || avoidAreaType == AvoidAreaType::TYPE_NAVIGATION_INDICATOR) {
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
        if (sceneSession == nullptr || !IsSessionVisible(sceneSession)) {
            continue;
        }
        AvoidArea avoidArea = sceneSession->GetAvoidAreaByType(static_cast<AvoidAreaType>(avoidType));
        ret = UpdateSessionAvoidAreaIfNeed(
            persistentId, sceneSession, avoidArea, static_cast<AvoidAreaType>(avoidType));
        needUpdate = needUpdate || ret;
    }

    return;
}

void SceneSessionManager::UpdateNormalSessionAvoidArea(
    const int32_t& persistentId, sptr<SceneSession>& sceneSession, bool& needUpdate)
{
    bool ret = true;
    if (sceneSession == nullptr || !IsSessionVisible(sceneSession)) {
        needUpdate = false;
        return;
    }
    if (avoidAreaListenerSessionSet_.find(persistentId) == avoidAreaListenerSessionSet_.end()) {
        WLOGD("id:%{public}d is not in avoidAreaListenerNodes, don't update avoid area.", persistentId);
        needUpdate = false;
        return;
    }
    uint32_t start = static_cast<uint32_t>(AvoidAreaType::TYPE_SYSTEM);
    uint32_t end = static_cast<uint32_t>(AvoidAreaType::TYPE_NAVIGATION_INDICATOR);
    for (uint32_t avoidType = start; avoidType <= end; avoidType++) {
        AvoidArea avoidArea = sceneSession->GetAvoidAreaByType(static_cast<AvoidAreaType>(avoidType));
        ret = UpdateSessionAvoidAreaIfNeed(
            persistentId, sceneSession, avoidArea, static_cast<AvoidAreaType>(avoidType));
        needUpdate = needUpdate || ret;
    }

    return;
}

void SceneSessionManager::UpdateAvoidArea(const int32_t& persistentId)
{
    auto task = [this, persistentId]() {
        bool needUpdate = false;
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFD("sceneSession is nullptr.");
            return;
        }
        NotifyWindowInfoChange(persistentId, WindowUpdateType::WINDOW_UPDATE_BOUNDS);

        WindowType type = sceneSession->GetWindowType();
        SessionGravity gravity = SessionGravity::SESSION_GRAVITY_DEFAULT;
        uint32_t percent = 0;
        if (sceneSession->GetSessionProperty() != nullptr) {
            sceneSession->GetSessionProperty()->GetSessionGravity(gravity, percent);
        }
        if (type == WindowType::WINDOW_TYPE_STATUS_BAR ||
            type == WindowType::WINDOW_TYPE_NAVIGATION_BAR ||
            (type == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT &&
            (gravity == SessionGravity::SESSION_GRAVITY_BOTTOM ||
            gravity == SessionGravity::SESSION_GRAVITY_DEFAULT))) {
            UpdateAvoidSessionAvoidArea(type, needUpdate);
        } else {
            UpdateNormalSessionAvoidArea(persistentId, sceneSession, needUpdate);
        }
        return;
    };
    taskScheduler_->PostAsyncTask(task, "UpdateAvoidArea:PID:" + std::to_string(persistentId));
    return;
}

WSError SceneSessionManager::NotifyAINavigationBarShowStatus(bool isVisible, WSRect barArea)
{
    WLOGFI("NotifyAINavigationBarShowStatus: isVisible: %{public}u, area{%{public}d,%{public}d,%{public}d,%{public}d}",
        isVisible, barArea.posX_, barArea.posY_, barArea.width_, barArea.height_);
    auto task = [this, isVisible, barArea]() {
        if (isAINavigationBarVisible_ != isVisible || currAINavigationBarArea_ != barArea) {
            isAINavigationBarVisible_ = isVisible;
            currAINavigationBarArea_ = barArea;
            if (!isVisible && !barArea.IsEmpty()) {
                WLOGFD("NotifyAINavigationBarShowStatus: barArea should be empty if invisible");
                currAINavigationBarArea_ = WSRect();
            }
            WLOGFI("NotifyAINavigationBarShowStatus: enter: %{public}u, {%{public}d,%{public}d,%{public}d,%{public}d}",
                isVisible, barArea.posX_, barArea.posY_, barArea.width_, barArea.height_);
            for (auto persistentId : avoidAreaListenerSessionSet_) {
                auto sceneSession = GetSceneSession(persistentId);
                if (sceneSession == nullptr) {
                    continue;
                }
                AvoidArea avoidArea = sceneSession->GetAvoidAreaByType(AvoidAreaType::TYPE_NAVIGATION_INDICATOR);
                if (!avoidArea.topRect_.IsUninitializedRect() || !avoidArea.leftRect_.IsUninitializedRect() ||
                    !avoidArea.rightRect_.IsUninitializedRect()) {
                    continue;
                }
                if (isVisible && avoidArea.isEmptyAvoidArea()) {
                    continue;
                }
                WLOGFI("NotifyAINavigationBarShowStatus: persistentId: %{public}d, "
                    "{%{public}d,%{public}d,%{public}d,%{public}d}", persistentId,
                    avoidArea.bottomRect_.posX_, avoidArea.bottomRect_.posY_,
                    avoidArea.bottomRect_.width_, avoidArea.bottomRect_.height_);
                UpdateSessionAvoidAreaIfNeed(persistentId, sceneSession, avoidArea,
                                             AvoidAreaType::TYPE_NAVIGATION_INDICATOR);
            }
        }
    };
    taskScheduler_->PostAsyncTask(task, "NotifyAINavigationBarShowStatus");
    return WSError::WS_OK;
}

WSRect SceneSessionManager::GetAINavigationBarArea()
{
    return currAINavigationBarArea_;
}

WSError SceneSessionManager::UpdateSessionTouchOutsideListener(int32_t& persistentId, bool haveListener)
{
    auto task = [this, persistentId, haveListener]() {
        WLOGFI("UpdateSessionTouchOutsideListener persistentId: %{public}d haveListener:%{public}d",
            persistentId, haveListener);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFD("sceneSession is nullptr.");
            return WSError::WS_DO_NOTHING;
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
    auto task = [this, persistentId, haveListener]() -> WSError {
        WLOGFI("UpdateSessionWindowVisibilityListener persistentId: %{public}d haveListener:%{public}d",
            persistentId, haveListener);
        auto sceneSession = GetSceneSession(persistentId);
        if (sceneSession == nullptr) {
            WLOGFD("sceneSession is nullptr.");
            return WSError::WS_DO_NOTHING;
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
                WLOGFD("UpdateDensity name=%{public}s, persistendId=%{public}d, winType=%{public}d, "
                    "state=%{public}d, visible-%{public}d", scnSession->GetWindowName().c_str(), item.first,
                    scnSession->GetWindowType(), scnSession->GetSessionState(), scnSession->IsVisible());
            }
        }
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
            if (scnSession->GetSessionState() == SessionState::STATE_FOREGROUND ||
                scnSession->GetSessionState() == SessionState::STATE_ACTIVE) {
                scnSession->UpdateRotationAvoidArea();
                WLOGFD("UpdateRotationAvoidArea name=%{public}s, persistendId=%{public}d, winType=%{public}d, "
                    "state=%{public}d, visible-%{public}d", scnSession->GetWindowName().c_str(), item.first,
                    scnSession->GetWindowType(), scnSession->GetSessionState(), scnSession->IsVisible());
            }
        }
        return WSError::WS_OK;
    };
    taskScheduler_->PostSyncTask(task, "ProcessUpdateRotationChange" + std::to_string(defaultDisplayId));
}

void DisplayChangeListener::OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    WLOGFD("DisplayChangeListener::OnDisplayStateChange: %{public}u", type);
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
        auto sceneSession = GetSceneSession(focusedSessionId_);
        if (sceneSession) {
            sceneSession->NotifyScreenshot();
        }
    };
    taskScheduler_->PostAsyncTask(task, "OnScreenshot:PID:" + std::to_string(displayId));
}

WSError SceneSessionManager::ClearSession(int32_t persistentId)
{
    WLOGFI("run ClearSession with persistentId: %{public}d", persistentId);
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
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
    WLOGFI("run ClearSession");
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (!IsSessionClearable(sceneSession)) {
        WLOGFI("sceneSession cannot be clear, persistentId %{public}d.", sceneSession->GetPersistentId());
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    const WSError& errCode = sceneSession->Clear();
    return errCode;
}

WSError SceneSessionManager::ClearAllSessions()
{
    WLOGFI("run ClearAllSessions");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
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
    WLOGFI("run GetAllClearableSessions");
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
    WLOGFI("run LockSession with persistentId: %{public}d", sessionId);
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, sessionId]() {
        auto sceneSession = GetSceneSession(sessionId);
        if (sceneSession == nullptr) {
            WLOGFE("can not find sceneSession, sessionId:%{public}d", sessionId);
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        sceneSession->SetSessionInfoLockedState(true);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "LockSession:SID:" + std::to_string(sessionId));
}

WSError SceneSessionManager::UnlockSession(int32_t sessionId)
{
    WLOGFI("run UnlockSession with persistentId: %{public}d", sessionId);
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    auto task = [this, sessionId]() {
        auto sceneSession = GetSceneSession(sessionId);
        if (sceneSession == nullptr) {
            WLOGFE("can not find sceneSession, sessionId:%{public}d", sessionId);
            return WSError::WS_ERROR_INVALID_PARAM;
        }
        sceneSession->SetSessionInfoLockedState(false);
        return WSError::WS_OK;
    };
    return taskScheduler_->PostSyncTask(task, "UnlockSession" + std::to_string(sessionId));
}

WSError SceneSessionManager::MoveSessionsToForeground(const std::vector<int32_t>& sessionIds, int32_t topSessionId)
{
    WLOGFI("run MoveSessionsToForeground");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }

    return WSError::WS_OK;
}

WSError SceneSessionManager::MoveSessionsToBackground(const std::vector<int32_t>& sessionIds,
    std::vector<int32_t>& result)
{
    WLOGFI("run MoveSessionsToBackground");
    if (!SessionPermission::JudgeCallerIsAllowedToUseSystemAPI()) {
        WLOGFE("The caller is not system-app, can not use system-api");
        return WSError::WS_ERROR_NOT_SYSTEM_APP;
    }
    if (!SessionPermission::VerifySessionPermission()) {
        WLOGFE("The caller has not permission granted");
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
    const sptr<AAFwk::IAbilityManagerCollaborator> &impl)
{
    WLOGFI("RegisterIAbilityManagerCollaborator with type : %{public}d", type);
    auto isSaCall = SessionPermission::IsSACalling();
    auto callingUid = IPCSkeleton::GetCallingUid();
    if (!isSaCall || (callingUid != BROKER_UID && callingUid != BROKER_RESERVE_UID)) {
        WLOGFE("The interface only support for broker");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    if (!CheckCollaboratorType(type)) {
        WLOGFW("collaborator register failed, invalid type.");
        return WSError::WS_ERROR_INVALID_TYPE;
    }
    {
        std::shared_lock<std::shared_mutex> lock(collaboratorMapLock_);
        collaboratorMap_[type] = impl;
    }
    return WSError::WS_OK;
}

WSError SceneSessionManager::UnregisterIAbilityManagerCollaborator(int32_t type)
{
    WLOGFI("UnregisterIAbilityManagerCollaborator with type : %{public}d", type);
    auto isSaCall = SessionPermission::IsSACalling();
    auto callingUid = IPCSkeleton::GetCallingUid();
    if (!isSaCall || (callingUid != BROKER_UID && callingUid != BROKER_RESERVE_UID)) {
        WLOGFE("The interface only support for broker");
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    if (!CheckCollaboratorType(type)) {
        WLOGFE("collaborator unregister failed, invalid type.");
        return WSError::WS_ERROR_INVALID_TYPE;
    }
    {
        std::shared_lock<std::shared_mutex> lock(collaboratorMapLock_);
        collaboratorMap_.erase(type);
    }
    return WSError::WS_OK;
}

void SceneSessionManager::ProcessPiPSessionForeground(const sptr<SceneSession> sceneSession)
{
    if (sceneSession == nullptr) {
        WLOGFE("pip window not found");
        return;
    }
    WLOGFD("start pip rect");
    sceneSession->UpdatePiPRect(0, 0, PiPRectUpdateReason::REASON_PIP_START_WINDOW);
}

WSError SceneSessionManager::RecoveryPullPiPMainWindow(const int32_t& persistentId, const Rect& rect)
{
    auto scnSession = GetSceneSession(persistentId);
    if (scnSession == nullptr) {
        WLOGFE("scnSession is nullptr, persistentId: %{public}d", persistentId);
        return WSError::WS_ERROR_NULLPTR;
    }
    if (!WindowHelper::IsPipWindow(scnSession->GetWindowType())) {
        WLOGFE("not pip window");
        return WSError::WS_DO_NOTHING;
    }
    if (!showPiPMainWindowFunc_) {
        WLOGFE("showPiPMainWindowFunc_ init error, persistentId: %{public}d", persistentId);
        return WSError::WS_DO_NOTHING;
    }
    auto task = [this, scnSession, rect]() {
        showPiPMainWindowFunc_(scnSession->GetParentPersistentId());
        WSRect rectPos = SessionHelper::TransferToWSRect(rect);
        scnSession->UpdateSessionRect(rectPos, SizeChangeReason::RECOVER);
        return WSError::WS_OK;
    };
    taskScheduler_->PostAsyncTask(task, "RecoveryPullPiPMainWindow");
    return WSError::WS_OK;
}

bool SceneSessionManager::CheckCollaboratorType(int32_t type)
{
    if (type != CollaboratorType::RESERVE_TYPE && type != CollaboratorType::OTHERS_TYPE) {
        WLOGFI("type is invalid");
        return false;
    }
    return true;
}

BrokerStates SceneSessionManager::CheckIfReuseSession(SessionInfo& sessionInfo)
{
    auto abilityInfo = QueryAbilityInfoFromBMS(currentUserId_, sessionInfo.bundleName_, sessionInfo.abilityName_,
        sessionInfo.moduleName_);
    if (abilityInfo == nullptr) {
        WLOGFE("CheckIfReuseSession abilityInfo is nullptr!");
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
        WLOGFW("CheckIfReuseSession not collaborator!");
        return BrokerStates::BROKER_UNKOWN;
    }
    BrokerStates resultValue = NotifyStartAbility(collaboratorType, sessionInfo);
    sessionInfo.collaboratorType_ = collaboratorType;
    sessionInfo.sessionAffinity = sessionInfo.want->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY);
    if (FindSessionByAffinity(sessionInfo.sessionAffinity) != nullptr) {
        WLOGFI("FindSessionByAffinity: %{public}s, try to reuse", sessionInfo.sessionAffinity.c_str());
        sessionInfo.reuse = true;
    }
    WLOGFI("CheckIfReuseSession end");
    return resultValue;
}

BrokerStates SceneSessionManager::NotifyStartAbility(int32_t collaboratorType, const SessionInfo& sessionInfo)
{
    WLOGFI("run NotifyStartAbility");
    auto iter = collaboratorMap_.find(collaboratorType);
    if (iter == collaboratorMap_.end()) {
        WLOGFI("Fail to found collaborator with type: %{public}d", collaboratorType);
        return BrokerStates::BROKER_UNKOWN;
    }
    if (sessionInfo.want == nullptr) {
        WLOGFI("sessionInfo.want is nullptr, init");
        sessionInfo.want = std::make_shared<AAFwk::Want>();
        sessionInfo.want->SetElementName("", sessionInfo.bundleName_, sessionInfo.abilityName_,
            sessionInfo.moduleName_);
    }
    auto collaborator = iter->second;
    uint64_t accessTokenIDEx = IPCSkeleton::GetCallingFullTokenID();
    if (collaborator != nullptr) {
        containerStartAbilityTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();

        std::string affinity = sessionInfo.want->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY);
        if (!affinity.empty() && FindSessionByAffinity(affinity) != nullptr) {
            WLOGFI("NotifyStartAbility affinity exit %{public}s", affinity.c_str());
            return BrokerStates::BROKER_UNKOWN;
        }
        int32_t ret = collaborator->NotifyStartAbility(*(sessionInfo.abilityInfo),
            currentUserId_, *(sessionInfo.want), accessTokenIDEx);
        WLOGFI("NotifyStartAbility ret: %{public}d", ret);
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
    WLOGFI("run NotifySessionCreate");
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }
    if (sessionInfo.want == nullptr) {
        WLOGFI("sessionInfo.want is nullptr");
        return;
    }
    auto iter = collaboratorMap_.find(sceneSession->GetCollaboratorType());
    if (iter == collaboratorMap_.end()) {
        WLOGFI("Fail to found collaborator with type: %{public}d", sceneSession->GetCollaboratorType());
        return;
    }
    auto collaborator = iter->second;
    auto abilitySessionInfo = SetAbilitySessionInfo(sceneSession);
    sceneSession->SetSelfToken(abilitySessionInfo->sessionToken);
    abilitySessionInfo->want = *(sessionInfo.want);
    if (collaborator != nullptr) {
        int32_t missionId = abilitySessionInfo->persistentId;
        std::string bundleName = sessionInfo.bundleName_;
        int64_t timestamp = containerStartAbilityTime;
        WindowInfoReporter::GetInstance().ReportContainerStartBegin(missionId, bundleName, timestamp);

        collaborator->NotifyMissionCreated(abilitySessionInfo);
    }
}

void SceneSessionManager::NotifyLoadAbility(int32_t collaboratorType,
    sptr<AAFwk::SessionInfo> abilitySessionInfo, std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo)
{
    WLOGFI("run NotifyLoadAbility");
    auto iter = collaboratorMap_.find(collaboratorType);
    if (iter == collaboratorMap_.end()) {
        WLOGFE("Fail to found collaborator with type: %{public}d", collaboratorType);
        return;
    }
    auto collaborator = iter->second;
    if (collaborator != nullptr) {
        collaborator->NotifyLoadAbility(*abilityInfo, abilitySessionInfo);
    }
}


void SceneSessionManager::NotifyUpdateSessionInfo(sptr<SceneSession> sceneSession)
{
    WLOGFI("run NotifyUpdateSessionInfo");
    if (sceneSession == nullptr) {
        WLOGFE("sceneSession is nullptr");
        return;
    }
    auto iter = collaboratorMap_.find(sceneSession->GetCollaboratorType());
    if (iter == collaboratorMap_.end()) {
        WLOGFE("Fail to found collaborator with type: %{public}d", sceneSession->GetCollaboratorType());
        return;
    }
    auto collaborator = iter->second;
    auto abilitySessionInfo = SetAbilitySessionInfo(sceneSession);
    if (collaborator != nullptr) {
        collaborator->UpdateMissionInfo(abilitySessionInfo);
    }
}

void SceneSessionManager::NotifyMoveSessionToForeground(int32_t collaboratorType, int32_t persistentId)
{
    WLOGFI("run NotifyMoveSessionToForeground");
    auto iter = collaboratorMap_.find(collaboratorType);
    if (iter == collaboratorMap_.end()) {
        WLOGFE("Fail to found collaborator with type: %{public}d", collaboratorType);
        return;
    }
    auto collaborator = iter->second;
    if (collaborator != nullptr) {
        collaborator->NotifyMoveMissionToForeground(persistentId);
    }
}

void SceneSessionManager::NotifyClearSession(int32_t collaboratorType, int32_t persistentId)
{
    WLOGFI("run NotifyClearSession with persistentId %{public}d", persistentId);
    auto iter = collaboratorMap_.find(collaboratorType);
    if (iter == collaboratorMap_.end()) {
        WLOGFE("Fail to found collaborator with type: %{public}d", collaboratorType);
        return;
    }
    auto collaborator = iter->second;
    if (collaborator != nullptr) {
        collaborator->NotifyClearMission(persistentId);
    }
}

void SceneSessionManager::PreHandleCollaborator(sptr<SceneSession>& sceneSession)
{
    WLOGFI("run PreHandleCollaborator");
    if (sceneSession == nullptr) {
        return;
    }
    std::string sessionAffinity;
    WLOGFI("try to run NotifyStartAbility and NotifySessionCreate");
    if (sceneSession->GetSessionInfo().want != nullptr) {
        sessionAffinity = sceneSession->GetSessionInfo().want
            ->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY);
    }
    if (sessionAffinity.empty()) {
        WLOGFI("PreHandleCollaborator sessionAffinity: %{public}s", sessionAffinity.c_str());
        BrokerStates notifyReturn = NotifyStartAbility(sceneSession->GetCollaboratorType(), sceneSession->GetSessionInfo());
        if (notifyReturn != BrokerStates::BROKER_STARTED) {
            WLOGFI("PreHandleCollaborator cant notify");
            return;
        }

    }
    if (sceneSession->GetSessionInfo().want != nullptr) {
        WLOGFI("broker persistentId: %{public}d",
            sceneSession->GetSessionInfo().want->GetIntParam(AncoConsts::ANCO_SESSION_ID, 0));
        sceneSession->SetSessionInfoAffinity(sceneSession->GetSessionInfo().want
            ->GetStringParam(Rosen::PARAM_KEY::PARAM_MISSION_AFFINITY_KEY));
        WLOGFI("affinity: %{public}s", sceneSession->GetSessionInfo().sessionAffinity.c_str());
    } else {
        WLOGFI("sceneSession->GetSessionInfo().want is nullptr");
    }
    NotifySessionCreate(sceneSession, sceneSession->GetSessionInfo());
    sceneSession->SetSessionInfoAncoSceneState(AncoSceneState::NOTIFY_CREATE);
}

void SceneSessionManager::AddWindowDragHotArea(int32_t type, WSRect& area)
{
    WLOGFI("run AddWindowDragHotArea, type: %{public}d,posX: %{public}d,posY: %{public}d,width: %{public}d,"
        "height: %{public}d", type, area.posX_, area.posY_, area.width_, area.height_);
    SceneSession::windowDragHotAreaMap_.insert({type, area});
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

WSError SceneSessionManager::UpdateSessionDisplayId(int32_t persistentId, uint64_t screenId)
{
    auto scnSession = GetSceneSession(persistentId);
    if (!scnSession) {
        WLOGFE("session is nullptr");
        return WSError::WS_ERROR_INVALID_WINDOW;
    }
    auto fromScreenId = scnSession->GetSessionInfo().screenId_;
    scnSession->SetScreenId(screenId);
    NotifySessionUpdate(scnSession->GetSessionInfo(), ActionType::MOVE_DISPLAY, fromScreenId);
    return WSError::WS_OK;
}

void DisplayChangeListener::OnImmersiveStateChange(bool& immersive)
{
    immersive = SceneSessionManager::GetInstance().UpdateImmersiveState();
}

bool SceneSessionManager::UpdateImmersiveState()
{
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
            WLOGFD("Current window is immersive");
            return true;
        } else {
            WLOGFD("Current window is not immersive");
            break;
        }
    }
    return false;
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

void SceneSessionManager::NotifyUpdateRectAfterLayout()
{
    auto transactionController = Rosen::RSSyncTransactionController::GetInstance();
    std::shared_ptr<RSTransaction> rsTransaction = nullptr;
    if (transactionController) {
        rsTransaction = transactionController->GetRSTransaction();
    }
    auto task = [this, rsTransaction]() {
        std::shared_lock<std::shared_mutex> lock(sceneSessionMapMutex_);
        for (auto& iter: sceneSessionMap_) {
            auto sceneSession = iter.second;
            if (sceneSession && sceneSession->IsDirtyWindow()) {
                sceneSession->NotifyClientToUpdateRect(rsTransaction);
            }
        }
    };
    // need sync task since animation transcation need
    return taskScheduler_->PostAsyncTask(task, "NotifyUpdateRectAfterLayout");
}

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
        if (!IsSessionVisible(sceneSession)) {
            WLOGFD("session is not visible!");
            return WSError::WS_DO_NOTHING;
        }
        RequestSessionFocus(persistentId);
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
    WLOGI("run ShiftAppWindowFocus, form id: %{public}d to id: %{public}d", sourcePersistentId, targetPersistentId);
    if (sourcePersistentId != focusedSessionId_) {
        WLOGE("source session need be focused");
        return WSError::WS_ERROR_INVALID_OPERATION;
    }
    if (targetPersistentId == focusedSessionId_) {
        WLOGE("target session has been focused");
        return WSError::WS_DO_NOTHING;
    }
    WSError ret = WSError::WS_OK;
    auto sourceSession = GetSceneSession(sourcePersistentId);
    if (ret == WSError::WS_OK && sourceSession != nullptr) {
        ret = GetAppMainSceneSession(sourceSession, sourcePersistentId);
    }
    auto targetSession = GetSceneSession(targetPersistentId);
    if (ret == WSError::WS_OK && targetSession != nullptr) {
        ret = GetAppMainSceneSession(targetSession, targetPersistentId);
    }
    if (ret != WSError::WS_OK) {
        return ret;
    }
    if (sourceSession->GetSessionInfo().bundleName_ != targetSession->GetSessionInfo().bundleName_) {
        WLOGE("verify bundle name failed, source bundle name is %{public}s but target bundle name is %{public}s)",
            sourceSession->GetSessionInfo().bundleName_.c_str(), targetSession->GetSessionInfo().bundleName_.c_str());
        return WSError::WS_ERROR_INVALID_CALLING;
    }
    if (!SessionPermission::IsSameBundleNameAsCalling(targetSession->GetSessionInfo().bundleName_)) {
        return WSError::WS_ERROR_INVALID_CALLING;
    }
    targetSession->NotifyClick();
    return RequestSessionFocus(targetPersistentId);
}

WSError SceneSessionManager::GetAppMainSceneSession(sptr<SceneSession>& sceneSession, int32_t persistentId)
{
    if (sceneSession == nullptr) {
        WLOGE("session(id: %{public}d) is nullptr", persistentId);
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        if (sceneSession->GetWindowType() != WindowType::WINDOW_TYPE_APP_SUB_WINDOW) {
            WLOGE("session(id: %{public}d) is not main window or sub window", persistentId);
            return WSError::WS_ERROR_INVALID_CALLING;
        }
        if (GetSceneSession(sceneSession->GetParentPersistentId()) != nullptr) {
            sceneSession = GetSceneSession(sceneSession->GetParentPersistentId());
            return WSError::WS_OK;
        }
        WLOGE("session(id: %{public}d) parent is nullptr", persistentId);
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    return WSError::WS_OK;
}

void AppAnrListener::OnAppDebugStarted(const std::vector<AppExecFwk::AppDebugInfo> &debugInfos)
{
    WLOGFI("AppAnrListener OnAppDebugStarted");
    if (debugInfos.empty()) {
        WLOGFE("AppAnrListener OnAppDebugStarted debugInfos is empty");
        return;
    }
    DelayedSingleton<ANRManager>::GetInstance()->SwitchAnr(false);
}

void AppAnrListener::OnAppDebugStoped(const std::vector<AppExecFwk::AppDebugInfo> &debugInfos)
{
    WLOGFI("AppAnrListener OnAppDebugStoped");
    if (debugInfos.empty()) {
        WLOGFE("AppAnrListener OnAppDebugStoped debugInfos is empty");
        return;
    }
    DelayedSingleton<ANRManager>::GetInstance()->SwitchAnr(true);
}
} // namespace OHOS::Rosen
