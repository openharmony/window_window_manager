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

#include "window_manager_service.h"

#include <thread>

#include <ability_manager_client.h>
#include <cinttypes>
#include <chrono>
#include <hisysevent.h>
#include <hitrace_meter.h>
#include <ipc_skeleton.h>
#include <parameters.h>
#include <rs_iwindow_animation_controller.h>
#include <system_ability_definition.h>
#include <sstream>
#include "xcollie/watchdog.h"

#include "color_parser.h"
#include "display_manager_service_inner.h"
#include "dm_common.h"
#include "drag_controller.h"
#include "minimize_app.h"
#include "permission.h"
#include "remote_animation.h"
#include "singleton_container.h"
#include "ui/rs_ui_director.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_math.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerService"};
    constexpr int REPORT_SHOW_WINDOW_TIMES = 50;
}
WM_IMPLEMENT_SINGLE_INSTANCE(WindowManagerService)

const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(&SingletonContainer::Get<WindowManagerService>());

WindowManagerService::WindowManagerService() : SystemAbility(WINDOW_MANAGER_SERVICE_ID, true),
    rsInterface_(RSInterfaces::GetInstance())
{
    windowRoot_ = new WindowRoot(
        [this](Event event, const sptr<IRemoteObject>& remoteObject) { OnWindowEvent(event, remoteObject); });
    inputWindowMonitor_ = new InputWindowMonitor(windowRoot_);
    windowController_ = new WindowController(windowRoot_, inputWindowMonitor_);
    dragController_ = new DragController(windowRoot_);
    windowDumper_ = new WindowDumper(windowRoot_);
    freezeDisplayController_ = new FreezeController();
    windowCommonEvent_ = std::make_shared<WindowCommonEvent>();
    startingOpen_ = system::GetParameter("persist.window.sw.enabled", "1") == "1"; // startingWin default enabled
    runner_ = AppExecFwk::EventRunner::Create(name_);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    snapshotController_ = new SnapshotController(windowRoot_, handler_);
    int ret = HiviewDFX::Watchdog::GetInstance().AddThread(name_, handler_);
    if (ret != 0) {
        WLOGFE("Add watchdog thread failed");
    }

    // init RSUIDirector, it will handle animation callback
    rsUiDirector_ = RSUIDirector::Create();
    rsUiDirector_->SetUITaskRunner([this](const std::function<void()>& task) { PostAsyncTask(task); });
    rsUiDirector_->Init(false);
}

void WindowManagerService::OnStart()
{
    WLOGFI("WindowManagerService::OnStart start");
    if (!Init()) {
        return;
    }
    WindowInnerManager::GetInstance().Start(system::GetParameter("persist.window.holder.enable", "0") == "1");
    sptr<IDisplayChangeListener> listener = new DisplayChangeListener();
    DisplayManagerServiceInner::GetInstance().RegisterDisplayChangeListener(listener);
    sptr<IWindowInfoQueriedListener> windowInfoQueriedListener = new WindowInfoQueriedListener();
    DisplayManagerServiceInner::GetInstance().RegisterWindowInfoQueriedListener(windowInfoQueriedListener);
    RegisterSnapshotHandler();
    RegisterWindowManagerServiceHandler();
    RegisterWindowVisibilityChangeCallback();
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
}

void WindowManagerService::PostAsyncTask(Task task)
{
    if (handler_) {
        bool ret = handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        if (!ret) {
            WLOGFE("EventHandler PostTask Failed");
        }
    }
}

void WindowManagerService::PostVoidSyncTask(Task task)
{
    if (handler_) {
        bool ret = handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        if (!ret) {
            WLOGFE("EventHandler PostVoidSyncTask Failed");
        }
    }
}


void WindowManagerService::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    WLOGFI(" %{public}d", systemAbilityId);
    if (systemAbilityId == COMMON_EVENT_SERVICE_ID) {
        windowCommonEvent_->SubscriberEvent();
    }
}

void WindowManagerService::OnAccountSwitched(int accountId)
{
    PostAsyncTask([this, accountId]() {
        windowRoot_->RemoveSingleUserWindowNodes(accountId);
    });
    WLOGFI("called");
}

void WindowManagerService::WindowVisibilityChangeCallback(std::shared_ptr<RSOcclusionData> occlusionData)
{
    WLOGFD("NotifyWindowVisibilityChange: enter");
    std::weak_ptr<RSOcclusionData> weak(occlusionData);
    PostVoidSyncTask([this, weak]() {
        auto weakOcclusionData = weak.lock();
        if (weakOcclusionData == nullptr) {
            WLOGFE("weak occlusionData is nullptr");
            return;
        }
        windowRoot_->NotifyWindowVisibilityChange(weakOcclusionData);
    });
}

void WindowManagerService::RegisterWindowVisibilityChangeCallback()
{
    auto windowVisibilityChangeCb = std::bind(&WindowManagerService::WindowVisibilityChangeCallback, this,
        std::placeholders::_1);
    if (rsInterface_.RegisterOcclusionChangeCallback(windowVisibilityChangeCb) != WM_OK) {
        WLOGFW("WindowManagerService::RegisterWindowVisibilityChangeCallback failed, create async thread!");
        auto fun = [this, windowVisibilityChangeCb]() {
            WLOGFI("WindowManagerService::RegisterWindowVisibilityChangeCallback async thread enter!");
            int counter = 0;
            while (rsInterface_.RegisterOcclusionChangeCallback(windowVisibilityChangeCb) != WM_OK) {
                usleep(10000); // 10000us equals to 10ms
                counter++;
                if (counter >= 2000) { // wait for 2000 * 10ms = 20s
                    WLOGFE("WindowManagerService::RegisterWindowVisibilityChangeCallback timeout!");
                    return;
                }
            }
            WLOGFI("WindowManagerService::RegisterWindowVisibilityChangeCallback async thread register handler"
                " successfully!");
        };
        std::thread thread(fun);
        thread.detach();
        WLOGFI("WindowManagerService::RegisterWindowVisibilityChangeCallback async thread has been detached!");
    } else {
        WLOGFI("WindowManagerService::RegisterWindowVisibilityChangeCallback OnStart succeed!");
    }
}

void WindowManagerService::RegisterSnapshotHandler()
{
    if (snapshotController_ == nullptr) {
        snapshotController_ = new SnapshotController(windowRoot_, handler_);
    }
    if (AAFwk::AbilityManagerClient::GetInstance()->RegisterSnapshotHandler(snapshotController_) != ERR_OK) {
        WLOGFW("WindowManagerService::RegisterSnapshotHandler failed, create async thread!");
        auto fun = [this]() {
            WLOGFI("WindowManagerService::RegisterSnapshotHandler async thread enter!");
            int counter = 0;
            while (AAFwk::AbilityManagerClient::GetInstance()->RegisterSnapshotHandler(snapshotController_) != ERR_OK) {
                usleep(10000); // 10000us equals to 10ms
                counter++;
                if (counter >= 2000) { // wait for 2000 * 10ms = 20s
                    WLOGFE("WindowManagerService::RegisterSnapshotHandler timeout!");
                    return;
                }
            }
            WLOGFI("WindowManagerService::RegisterSnapshotHandler async thread register handler successfully!");
        };
        std::thread thread(fun);
        thread.detach();
        WLOGFI("WindowManagerService::RegisterSnapshotHandler async thread has been detached!");
    } else {
        WLOGFI("WindowManagerService::RegisterSnapshotHandler OnStart succeed!");
    }
}

void WindowManagerService::RegisterWindowManagerServiceHandler()
{
    if (wmsHandler_ == nullptr) {
        wmsHandler_ = new WindowManagerServiceHandler();
    }
    if (AAFwk::AbilityManagerClient::GetInstance()->RegisterWindowManagerServiceHandler(wmsHandler_) != ERR_OK) {
        WLOGFW("RegisterWindowManagerServiceHandler failed, create async thread!");
        auto fun = [this]() {
            WLOGFI("RegisterWindowManagerServiceHandler async thread enter!");
            int counter = 0;
            while (AAFwk::AbilityManagerClient::GetInstance()->
                RegisterWindowManagerServiceHandler(wmsHandler_) != ERR_OK) {
                usleep(10000); // 10000us equals to 10ms
                counter++;
                if (counter >= 2000) { // wait for 2000 * 10ms = 20s
                    WLOGFE("RegisterWindowManagerServiceHandler timeout!");
                    return;
                }
            }
            WLOGFI("RegisterWindowManagerServiceHandler async thread register handler successfully!");
        };
        std::thread thread(fun);
        thread.detach();
        WLOGFI("RegisterWindowManagerServiceHandler async thread has been detached!");
    } else {
        WLOGFI("RegisterWindowManagerServiceHandler OnStart succeed!");
    }
}

void WindowManagerServiceHandler::NotifyWindowTransition(
    sptr<AAFwk::AbilityTransitionInfo> from, sptr<AAFwk::AbilityTransitionInfo> to)
{
    sptr<WindowTransitionInfo> fromInfo = new WindowTransitionInfo(from);
    sptr<WindowTransitionInfo> toInfo = new WindowTransitionInfo(to);
    WindowManagerService::GetInstance().NotifyWindowTransition(fromInfo, toInfo, false);
}

int32_t WindowManagerServiceHandler::GetFocusWindow(sptr<IRemoteObject>& abilityToken)
{
    return static_cast<int32_t>(WindowManagerService::GetInstance().GetFocusWindowInfo(abilityToken));
}

void WindowManagerServiceHandler::StartingWindow(
    sptr<AAFwk::AbilityTransitionInfo> info, sptr<Media::PixelMap> pixelMap)
{
    sptr<WindowTransitionInfo> windowInfo = new WindowTransitionInfo(info);
    WLOGFI("hot start is called");
    WindowManagerService::GetInstance().StartingWindow(windowInfo, pixelMap, false);
}

void WindowManagerServiceHandler::StartingWindow(
    sptr<AAFwk::AbilityTransitionInfo> info, sptr<Media::PixelMap> pixelMap, uint32_t bgColor)
{
    sptr<WindowTransitionInfo> windowInfo = new WindowTransitionInfo(info);
    WLOGFI("cold start is called");
    WindowManagerService::GetInstance().StartingWindow(windowInfo, pixelMap, true, bgColor);
}

void WindowManagerServiceHandler::CancelStartingWindow(sptr<IRemoteObject> abilityToken)
{
    WLOGFI("WindowManagerServiceHandler CancelStartingWindow!");
    WindowManagerService::GetInstance().CancelStartingWindow(abilityToken);
}

bool WindowManagerService::Init()
{
    WLOGFI("WindowManagerService::Init start");
    bool ret = Publish(this);
    if (!ret) {
        WLOGFW("WindowManagerService::Init failed");
        return false;
    }
    if (WindowManagerConfig::LoadConfigXml()) {
        if (WindowManagerConfig::GetConfig().IsMap()) {
            WindowManagerConfig::DumpConfig(*WindowManagerConfig::GetConfig().mapValue_);
        }
        ConfigureWindowManagerService();
    }
    WLOGFI("WindowManagerService::Init success");
    return true;
}

int WindowManagerService::Dump(int fd, const std::vector<std::u16string>& args)
{
    if (windowDumper_ == nullptr) {
        windowDumper_ = new WindowDumper(windowRoot_);
    }

    return PostSyncTask([this, fd, &args]() {
        return static_cast<int>(windowDumper_->Dump(fd, args));
    });
}

void WindowManagerService::ConfigureWindowManagerService()
{
    const auto& config = WindowManagerConfig::GetConfig();
    WindowManagerConfig::ConfigItem item = config["decor"].GetProp("enable");
    if (item.IsBool()) {
        systemConfig_.isSystemDecorEnable_ = item.boolValue_;
    }
    item = config["minimizeByOther"].GetProp("enable");
    if (item.IsBool()) {
        MinimizeApp::SetMinimizedByOtherConfig(item.boolValue_);
    }
    item = config["stretchable"].GetProp("enable");
    if (item.IsBool()) {
        systemConfig_.isStretchable_ = item.boolValue_;
    }
    item = config["defaultWindowMode"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1 &&
            (numbers[0] == static_cast<uint32_t>(WindowMode::WINDOW_MODE_FULLSCREEN) ||
             numbers[0] == static_cast<uint32_t>(WindowMode::WINDOW_MODE_FLOATING))) {
            systemConfig_.defaultWindowMode_ = static_cast<WindowMode>(static_cast<uint32_t>(numbers[0]));
        }
    }
    item = config["remoteAnimation"].GetProp("enable");
    if (item.IsBool()) {
        RemoteAnimation::isRemoteAnimationEnable_ = item.boolValue_;
    }
    item = config["maxAppWindowNumber"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1 && numbers[0] > 0) {
            windowRoot_->SetMaxAppWindowNumber(static_cast<uint32_t>(numbers[0]));
        }
    }
    item = config["modeChangeHotZones"];
    if (item.IsInts()) {
        ConfigHotZones(*item.intsValue_);
    }
    item = config["splitRatios"];
    if (item.IsFloats()) {
        windowRoot_->SetSplitRatios(*item.floatsValue_);
    }
    item = config["exitSplitRatios"];
    if (item.IsFloats()) {
        windowRoot_->SetExitSplitRatios(*item.floatsValue_);
    }
    item = config["windowAnimation"];
    if (item.IsMap()) {
        ConfigWindowAnimation(item);
    }
    item = config["keyboardAnimation"];
    if (item.IsMap()) {
        ConfigKeyboardAnimation(item);
    }
    item = config["windowEffect"];
    if (item.IsMap()) {
        ConfigWindowEffect(item);
    }
}

void WindowManagerService::ConfigHotZones(const std::vector<int>& numbers)
{
    if (numbers.size() == 3) { // 3 hot zones
        hotZonesConfig_.fullscreenRange_ = static_cast<uint32_t>(numbers[0]); // 0 fullscreen
        hotZonesConfig_.primaryRange_ = static_cast<uint32_t>(numbers[1]);    // 1 primary
        hotZonesConfig_.secondaryRange_ = static_cast<uint32_t>(numbers[2]);  // 2 secondary
        hotZonesConfig_.isModeChangeHotZoneConfigured_ = true;
    }
}

void WindowManagerService::ConfigWindowAnimation(const WindowManagerConfig::ConfigItem& animeConfig)
{
    auto& windowAnimationConfig = WindowNodeContainer::GetAnimationConfigRef().windowAnimationConfig_;
    WindowManagerConfig::ConfigItem item = animeConfig["timing"];
    if (item.IsMap() && item.mapValue_->count("curve")) {
        windowAnimationConfig.animationTiming_.timingCurve_ = CreateCurve(item["curve"]);
    }
    item = animeConfig["timing"]["duration"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) { // duration
            windowAnimationConfig.animationTiming_.timingProtocol_ =
                RSAnimationTimingProtocol(numbers[0]);
        }
    }
    item = animeConfig["scale"];
    if (item.IsFloats()) {
        auto numbers = *item.floatsValue_;
        if (numbers.size() == 1) { // 1 xy scale
            windowAnimationConfig.scale_.x_ =
            windowAnimationConfig.scale_.y_ = numbers[0]; // 0 xy scale
        } else if (numbers.size() == 2) { // 2 x,y sclae
            windowAnimationConfig.scale_.x_ = numbers[0]; // 0 x scale
            windowAnimationConfig.scale_.y_ = numbers[1]; // 1 y scale
        } else if (numbers.size() == 3) { // 3 x,y,z scale
            windowAnimationConfig.scale_ = Vector3f(&numbers[0]);
        }
    }
    item = animeConfig["rotation"];
    if (item.IsFloats() && item.floatsValue_->size() == 4) { // 4 (axix,angle)
        windowAnimationConfig.rotation_ = Vector4f(item.floatsValue_->data());
    }
    item = animeConfig["translate"];
    if (item.IsFloats()) {
        auto numbers = *item.floatsValue_;
        if (numbers.size() == 2) { // 2 translate xy
            windowAnimationConfig.translate_.x_ = numbers[0]; // 0 translate x
            windowAnimationConfig.translate_.y_ = numbers[1]; // 1 translate y
        } else if (numbers.size() == 3) { // 3 translate xyz
            windowAnimationConfig.translate_.x_ = numbers[0]; // 0 translate x
            windowAnimationConfig.translate_.y_ = numbers[1]; // 1 translate y
            windowAnimationConfig.translate_.z_ = numbers[2]; // 2 translate z
        }
    }
    item = animeConfig["opacity"];
    if (item.IsFloats()) {
        auto numbers = *item.floatsValue_;
        if (numbers.size() == 1) {
            windowAnimationConfig.opacity_ = numbers[0]; // 0 opacity
        }
    }
}

void WindowManagerService::ConfigKeyboardAnimation(const WindowManagerConfig::ConfigItem& animeConfig)
{
    auto& animationConfig = WindowNodeContainer::GetAnimationConfigRef();
    WindowManagerConfig::ConfigItem item = animeConfig["timing"];
    if (item.IsMap() && item.mapValue_->count("curve")) {
        animationConfig.keyboardAnimationConfig_.curve_ = CreateCurve(item["curve"]);
    }
    item = animeConfig["timing"]["durationIn"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) { // duration
            animationConfig.keyboardAnimationConfig_.durationIn_ =
                RSAnimationTimingProtocol(numbers[0]);
        }
    }
    item = animeConfig["timing"]["durationOut"];
    if (item.IsInts()) {
        auto numbers = *item.intsValue_;
        if (numbers.size() == 1) { // duration
            animationConfig.keyboardAnimationConfig_.durationOut_ =
                RSAnimationTimingProtocol(numbers[0]);
        }
    }
}

bool WindowManagerService::ConfigAppWindowCornerRadius(const WindowManagerConfig::ConfigItem& item, float& out)
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

bool WindowManagerService::ConfigAppWindowShadow(const WindowManagerConfig::ConfigItem& shadowConfig,
    WindowShadowParameters& outShadow)
{
    WindowManagerConfig::ConfigItem item = shadowConfig["elevation"];
    if (item.IsFloats()) {
        auto elevation = *item.floatsValue_;
        if (elevation.size() != 1 || (elevation.size() == 1 && MathHelper::LessNotEqual(elevation[0], 0.0))) {
            return false;
        }
        outShadow.elevation_ = elevation[0];
    }

    item = shadowConfig["color"];
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
        if (alpha.size() != 1 || (alpha.size() == 1 &&
            MathHelper::LessNotEqual(alpha[0], 0.0) && MathHelper::GreatNotEqual(alpha[0], 1.0))) {
            return false;
        }
        outShadow.alpha_ = alpha[0];
    }
    return true;
}

void WindowManagerService::ConfigWindowEffect(const WindowManagerConfig::ConfigItem& effectConfig)
{
    AppWindowEffectConfig config;

    // config corner radius
    WindowManagerConfig::ConfigItem item = effectConfig["appWindows"]["cornerRadius"];
    if (item.IsMap()) {
        if (ConfigAppWindowCornerRadius(item["fullScreen"], config.fullScreenCornerRadius_) &&
            ConfigAppWindowCornerRadius(item["split"], config.splitCornerRadius_) &&
            ConfigAppWindowCornerRadius(item["float"], config.floatCornerRadius_)) {
            systemConfig_.effectConfig_ = config;
        }
    }

    // config shadow
    item = effectConfig["appWindows"]["shadow"]["focused"];
    if (item.IsMap()) {
        if (ConfigAppWindowShadow(item, config.focusedShadow_)) {
            systemConfig_.effectConfig_.focusedShadow_ = config.focusedShadow_;
        }
    }

    item = effectConfig["appWindows"]["shadow"]["unfocused"];
    if (item.IsMap()) {
        if (ConfigAppWindowShadow(item, config.unfocusedShadow_)) {
            systemConfig_.effectConfig_.unfocusedShadow_ = config.unfocusedShadow_;
        }
    }
}

RSAnimationTimingCurve WindowManagerService::CreateCurve(const WindowManagerConfig::ConfigItem& curveConfig)
{
    const auto& nameItem = curveConfig.GetProp("name");
    if (nameItem.IsString()) {
        std::string name = nameItem.stringValue_;
        if (name == "easeOut") {
            return RSAnimationTimingCurve::EASE_OUT;
        } else if (name == "ease") {
            return RSAnimationTimingCurve::EASE;
        } else if (name == "easeIn") {
            return RSAnimationTimingCurve::EASE_IN;
        } else if (name == "easeInOut") {
            return RSAnimationTimingCurve::EASE_IN_OUT;
        } else if (name == "default") {
            return RSAnimationTimingCurve::DEFAULT;
        } else if (name == "linear") {
            return RSAnimationTimingCurve::LINEAR;
        } else if (name == "spring") {
            return RSAnimationTimingCurve::SPRING;
        } else if (name == "interactiveSpring") {
            return RSAnimationTimingCurve::INTERACTIVE_SPRING;
        } else if (name == "cubic" && curveConfig.IsFloats() &&
            curveConfig.floatsValue_->size() == 4) { // 4 curve parameter
            const auto& numbers = *curveConfig.floatsValue_;
            return RSAnimationTimingCurve::CreateCubicCurve(numbers[0], // 0 ctrlX1
                numbers[1], // 1 ctrlY1
                numbers[2], // 2 ctrlX2
                numbers[3]); // 3 ctrlY2
        }
    }
    return RSAnimationTimingCurve::EASE_OUT;
}

void WindowManagerService::OnStop()
{
    windowCommonEvent_->UnSubscriberEvent();
    WindowInnerManager::GetInstance().Stop();
    WLOGFI("ready to stop service.");
}

WMError WindowManagerService::NotifyWindowTransition(
    sptr<WindowTransitionInfo>& fromInfo, sptr<WindowTransitionInfo>& toInfo, bool isFromClient)
{
    if (!isFromClient) {
        WLOGFI("NotifyWindowTransition asynchronously.");
        PostAsyncTask([this, fromInfo, toInfo]() mutable {
            return windowController_->NotifyWindowTransition(fromInfo, toInfo);
        });
        return WMError::WM_OK;
    } else {
        WLOGFI("NotifyWindowTransition synchronously.");
        return PostSyncTask([this, &fromInfo, &toInfo]() {
            return windowController_->NotifyWindowTransition(fromInfo, toInfo);
        });
    }
}

WMError WindowManagerService::GetFocusWindowInfo(sptr<IRemoteObject>& abilityToken)
{
    return PostSyncTask([this, &abilityToken]() {
        return windowController_->GetFocusWindowInfo(abilityToken);
    });
}

void WindowManagerService::StartingWindow(sptr<WindowTransitionInfo> info, sptr<Media::PixelMap> pixelMap,
    bool isColdStart, uint32_t bkgColor)
{
    if (!startingOpen_) {
        WLOGFI("startingWindow not open!");
        return;
    }
    PostAsyncTask([this, info, pixelMap, isColdStart, bkgColor]() {
        windowController_->StartingWindow(info, pixelMap, bkgColor, isColdStart);
    });
}

void WindowManagerService::CancelStartingWindow(sptr<IRemoteObject> abilityToken)
{
    WLOGFI("begin CancelStartingWindow!");
    if (!startingOpen_) {
        WLOGFI("startingWindow not open!");
        return;
    }
    PostAsyncTask([this, abilityToken]() {
        windowController_->CancelStartingWindow(abilityToken);
    });
}

WMError WindowManagerService::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId, sptr<IRemoteObject> token)
{
    if (window == nullptr || property == nullptr || surfaceNode == nullptr) {
        WLOGFE("window is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    if ((!window) || (!window->AsObject())) {
        WLOGFE("failed to get window agent");
        return WMError::WM_ERROR_NULLPTR;
    }
    int pid = IPCSkeleton::GetCallingPid();
    int uid = IPCSkeleton::GetCallingUid();
    WMError ret = PostSyncTask([this, pid, uid, &window, &property, &surfaceNode, &windowId, &token]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:CreateWindow(%u)", windowId);
        return windowController_->CreateWindow(window, property, surfaceNode, windowId, token, pid, uid);
    });
    accessTokenIdMaps_.insert(std::pair(windowId, IPCSkeleton::GetCallingTokenID()));
    return ret;
}

WMError WindowManagerService::AddWindow(sptr<WindowProperty>& property)
{
    return PostSyncTask([this, &property]() {
        auto startTime = std::chrono::steady_clock::now();
        if (property == nullptr) {
            WLOGFE("property is nullptr");
            return WMError::WM_ERROR_NULLPTR;
        }
        Rect rect = property->GetRequestRect();
        uint32_t windowId = property->GetWindowId();
        WLOGFI("[WMS] Add: %{public}5d %{public}4d %{public}4d %{public}4d [%{public}4d %{public}4d " \
            "%{public}4d %{public}4d]", windowId, property->GetWindowType(), property->GetWindowMode(),
            property->GetWindowFlags(), rect.posX_, rect.posY_, rect.width_, rect.height_);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:AddWindow(%u)", windowId);
        WMError res = windowController_->AddWindowNode(property);
        if (property->GetWindowType() == WindowType::WINDOW_TYPE_DRAGGING_EFFECT) {
            dragController_->StartDrag(windowId);
        }
        if (res == WMError::WM_OK) {
            showWindowTimeConfig_.showWindowTimes_++;
            auto currentTime = std::chrono::steady_clock::now();
            int64_t costTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
            RecordShowTimeEvent(costTime);
        }
        return res;
    });
}

WMError WindowManagerService::RemoveWindow(uint32_t windowId)
{
    return PostSyncTask([this, windowId]() {
        WLOGFI("[WMS] Remove: %{public}u", windowId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:RemoveWindow(%u)", windowId);
        WindowInnerManager::GetInstance().NotifyWindowRemovedOrDestroyed(windowId);
        WMError res = windowController_->RecoverInputEventToClient(windowId);
        if (res != WMError::WM_OK) {
            return res;
        }
        return windowController_->RemoveWindowNode(windowId);
    });
}

WMError WindowManagerService::DestroyWindow(uint32_t windowId, bool onlySelf)
{
    if (!accessTokenIdMaps_.isExistAndRemove(windowId, IPCSkeleton::GetCallingTokenID())) {
        WLOGFI("Operation rejected");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    return PostSyncTask([this, windowId, onlySelf]() {
        WLOGFI("[WMS] Destroy: %{public}u", windowId);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:DestroyWindow(%u)", windowId);
        WindowInnerManager::GetInstance().NotifyWindowRemovedOrDestroyed(windowId);
        auto node = windowRoot_->GetWindowNode(windowId);
        if (node != nullptr && node->GetWindowType() == WindowType::WINDOW_TYPE_DRAGGING_EFFECT) {
            dragController_->FinishDrag(windowId);
        }
        return windowController_->DestroyWindow(windowId, onlySelf);
    });
}

WMError WindowManagerService::RequestFocus(uint32_t windowId)
{
    return PostSyncTask([this, windowId]() {
        WLOGFI("[WMS] RequestFocus: %{public}u", windowId);
        return windowController_->RequestFocus(windowId);
    });
}

AvoidArea WindowManagerService::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType)
{
    return PostSyncTask([this, windowId, avoidAreaType]() {
        WLOGFI("[WMS] GetAvoidAreaByType: %{public}u, Type: %{public}u", windowId,
            static_cast<uint32_t>(avoidAreaType));
        return windowController_->GetAvoidAreaByType(windowId, avoidAreaType);
    });
}

void WindowManagerService::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return;
    }
    PostVoidSyncTask([this, &windowManagerAgent, type]() {
        WindowManagerAgentController::GetInstance().RegisterWindowManagerAgent(windowManagerAgent, type);
        if (type == WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_SYSTEM_BAR) { // if system bar, notify once
            windowController_->NotifySystemBarTints();
        }
    });
}

void WindowManagerService::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    if ((windowManagerAgent == nullptr) || (windowManagerAgent->AsObject() == nullptr)) {
        WLOGFE("windowManagerAgent is null");
        return;
    }
    PostVoidSyncTask([this, &windowManagerAgent, type]() {
        WindowManagerAgentController::GetInstance().UnregisterWindowManagerAgent(windowManagerAgent, type);
    });
}

WMError WindowManagerService::SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller)
{
    if (controller == nullptr) {
        WLOGFE("RSWindowAnimation: Failed to set window animation controller, controller is null!");
        return WMError::WM_ERROR_NULLPTR;
    }

    sptr<AgentDeathRecipient> deathRecipient = new AgentDeathRecipient(
        [this](sptr<IRemoteObject>& remoteObject) {
            PostVoidSyncTask([&remoteObject]() {
                RemoteAnimation::OnRemoteDie(remoteObject);
            });
        }
    );
    controller->AsObject()->AddDeathRecipient(deathRecipient);
    return PostSyncTask([this, &controller]() {
        return windowController_->SetWindowAnimationController(controller);
    });
}

void WindowManagerService::OnWindowEvent(Event event, const sptr<IRemoteObject>& remoteObject)
{
    if (event == Event::REMOTE_DIED) {
        PostVoidSyncTask([this, &remoteObject, event]() {
            uint32_t windowId = windowRoot_->GetWindowIdByObject(remoteObject);
            auto node = windowRoot_->GetWindowNode(windowId);
            if (node != nullptr && node->GetWindowType() == WindowType::WINDOW_TYPE_DRAGGING_EFFECT) {
                dragController_->FinishDrag(windowId);
            }
            windowController_->DestroyWindow(windowId, true);
        });
    }
}

void WindowManagerService::NotifyDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:NotifyDisplayStateChange(%u)", type);
    DisplayId displayId = (displayInfo == nullptr) ? DISPLAY_ID_INVALID : displayInfo->GetDisplayId();
    if (type == DisplayStateChangeType::FREEZE) {
        freezeDisplayController_->FreezeDisplay(displayId);
    } else if (type == DisplayStateChangeType::UNFREEZE) {
        freezeDisplayController_->UnfreezeDisplay(displayId);
    } else {
        PostAsyncTask([this, defaultDisplayId, displayInfo, displayInfoMap, type]() mutable {
            windowController_->NotifyDisplayStateChange(defaultDisplayId, displayInfo, displayInfoMap, type);
        });
    }
}

void DisplayChangeListener::OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    WindowManagerService::GetInstance().NotifyDisplayStateChange(defaultDisplayId, displayInfo, displayInfoMap, type);
}

void DisplayChangeListener::OnGetWindowPreferredOrientation(DisplayId displayId, Orientation &orientation)
{
    WindowManagerService::GetInstance().GetWindowPreferredOrientation(displayId, orientation);
}

void DisplayChangeListener::OnScreenshot(DisplayId displayId)
{
    WindowManagerService::GetInstance().OnScreenshot(displayId);
}

void WindowManagerService::NotifyServerReadyToMoveOrDrag(uint32_t windowId, sptr<WindowProperty>& windowProperty,
    sptr<MoveDragProperty>& moveDragProperty)
{
    if (windowProperty == nullptr || moveDragProperty == nullptr) {
        WLOGFE("windowProperty or moveDragProperty is invalid");
        return;
    }

    PostAsyncTask([this, windowId, windowProperty, moveDragProperty]() mutable {
        if (moveDragProperty->startDragFlag_ || moveDragProperty->startMoveFlag_) {
            bool res = WindowInnerManager::GetInstance().NotifyServerReadyToMoveOrDrag(windowId,
                windowProperty, moveDragProperty);
            if (!res) {
                WLOGFE("invalid operation");
                return;
            }
            windowController_->InterceptInputEventToServer(windowId);
        }
        windowController_->NotifyServerReadyToMoveOrDrag(windowId, moveDragProperty);
    });
}

void WindowManagerService::ProcessPointDown(uint32_t windowId)
{
    PostAsyncTask([this, windowId]() {
        windowController_->ProcessPointDown(windowId);
    });
}

void WindowManagerService::ProcessPointUp(uint32_t windowId)
{
    PostAsyncTask([this, windowId]() {
        WindowInnerManager::GetInstance().NotifyWindowEndUpMovingOrDragging(windowId);
        windowController_->RecoverInputEventToClient(windowId);
        windowController_->ProcessPointUp(windowId);
    });
}

void WindowManagerService::NotifyWindowClientPointUp(uint32_t windowId,
    const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    PostAsyncTask([this, windowId, pointerEvent]() mutable {
        windowController_->NotifyWindowClientPointUp(windowId, pointerEvent);
    });
}

void WindowManagerService::MinimizeAllAppWindows(DisplayId displayId)
{
    PostAsyncTask([this, displayId]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:MinimizeAllAppWindows(%" PRIu64")", displayId);
        WLOGFI("displayId %{public}" PRIu64"", displayId);
        windowController_->MinimizeAllAppWindows(displayId);
    });
}

WMError WindowManagerService::ToggleShownStateForAllAppWindows()
{
    PostAsyncTask([this]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:ToggleShownStateForAllAppWindows");
        return windowController_->ToggleShownStateForAllAppWindows();
    });
    return WMError::WM_OK;
}

WMError WindowManagerService::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    return PostSyncTask([this, &topWinId, mainWinId]() {
        return windowController_->GetTopWindowId(mainWinId, topWinId);
    });
}

WMError WindowManagerService::SetWindowLayoutMode(WindowLayoutMode mode)
{
    return PostSyncTask([this, mode]() {
        WLOGFI("layoutMode: %{public}u", mode);
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:SetWindowLayoutMode");
        return windowController_->SetWindowLayoutMode(mode);
    });
}

WMError WindowManagerService::UpdateProperty(sptr<WindowProperty>& windowProperty, PropertyChangeAction action,
    bool isAsyncTask)
{
    if (windowProperty == nullptr) {
        WLOGFE("windowProperty is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (action == PropertyChangeAction::ACTION_UPDATE_TRANSFORM_PROPERTY) {
        PostAsyncTask([this, windowProperty, action]() mutable {
            windowController_->UpdateProperty(windowProperty, action);
        });
        return WMError::WM_OK;
    }

    if (isAsyncTask) {
        PostAsyncTask([this, windowProperty, action]() mutable {
            HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:UpdateProperty");
            WMError res = windowController_->UpdateProperty(windowProperty, action);
            if (action == PropertyChangeAction::ACTION_UPDATE_RECT && res == WMError::WM_OK &&
                windowProperty->GetWindowSizeChangeReason() == WindowSizeChangeReason::MOVE) {
                dragController_->UpdateDragInfo(windowProperty->GetWindowId());
            }
        });
        return WMError::WM_OK;
    }

    return PostSyncTask([this, &windowProperty, action]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:UpdateProperty");
        WMError res = windowController_->UpdateProperty(windowProperty, action);
        if (action == PropertyChangeAction::ACTION_UPDATE_RECT && res == WMError::WM_OK &&
            windowProperty->GetWindowSizeChangeReason() == WindowSizeChangeReason::MOVE) {
            dragController_->UpdateDragInfo(windowProperty->GetWindowId());
        }
        return res;
    });
}

WMError WindowManagerService::GetAccessibilityWindowInfo(sptr<AccessibilityWindowInfo>& windowInfo)
{
    if (windowInfo == nullptr) {
        WLOGFE("windowInfo is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    return PostSyncTask([this, &windowInfo]() {
        return windowController_->GetAccessibilityWindowInfo(windowInfo);
    });
}

WMError WindowManagerService::GetSystemConfig(SystemConfig& systemConfig)
{
    systemConfig = systemConfig_;
    return WMError::WM_OK;
}

WMError WindowManagerService::GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones)
{
    if (!hotZonesConfig_.isModeChangeHotZoneConfigured_) {
        return WMError::WM_DO_NOTHING;
    }

    return windowController_->GetModeChangeHotZones(displayId, hotZones, hotZonesConfig_);
}

void WindowManagerService::MinimizeWindowsByLauncher(std::vector<uint32_t> windowIds, bool isAnimated,
    sptr<RSIWindowAnimationFinishedCallback>& finishCallback)
{
    PostVoidSyncTask([this, windowIds, isAnimated, &finishCallback]() mutable {
        windowController_->MinimizeWindowsByLauncher(windowIds, isAnimated, finishCallback);
    });
}

void WindowManagerService::GetWindowPreferredOrientation(DisplayId displayId, Orientation &orientation)
{
    PostVoidSyncTask([this, displayId, &orientation]() mutable {
        orientation = windowController_->GetWindowPreferredOrientation(displayId);
    });
}

WMError WindowManagerService::UpdateAvoidAreaListener(uint32_t windowId, bool haveAvoidAreaListener)
{
    return PostSyncTask([this, windowId, haveAvoidAreaListener]() {
        sptr<WindowNode> node = windowRoot_->GetWindowNode(windowId);
        if (node == nullptr) {
            WLOGFE("get window node failed. win %{public}u", windowId);
            return WMError::WM_DO_NOTHING;
        }
        sptr<WindowNodeContainer> container = windowRoot_->GetWindowNodeContainer(node->GetDisplayId());
        if (container == nullptr) {
            WLOGFE("get container failed. win %{public}u display %{public}" PRIu64"", windowId, node->GetDisplayId());
            return WMError::WM_DO_NOTHING;
        }
        container->UpdateAvoidAreaListener(node, haveAvoidAreaListener);
        return WMError::WM_OK;
    });
}

WMError WindowManagerService::UpdateRsTree(uint32_t windowId, bool isAdd)
{
    return PostSyncTask([this, windowId, isAdd]() {
        return windowRoot_->UpdateRsTree(windowId, isAdd);
    });
}

void WindowManagerService::OnScreenshot(DisplayId displayId)
{
    PostAsyncTask([this, displayId]() {
        windowController_->OnScreenshot(displayId);
    });
}

WMError WindowManagerService::BindDialogTarget(uint32_t& windowId, sptr<IRemoteObject> targetToken)
{
    return PostSyncTask([this, &windowId, targetToken]() {
        return windowController_->BindDialogTarget(windowId, targetToken);
    });
}

void WindowManagerService::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    PostVoidSyncTask([this, displayId, &hasPrivateWindow]() mutable {
        hasPrivateWindow = windowRoot_->HasPrivateWindow(displayId);
    });
    WLOGFI("called %{public}u", hasPrivateWindow);
}

void WindowInfoQueriedListener::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    WLOGFI("called");
    WindowManagerService::GetInstance().HasPrivateWindow(displayId, hasPrivateWindow);
}

void WindowManagerService::RecordShowTimeEvent(int64_t costTime)
{
    WLOGFI("show window cost time(ms): %{public}" PRIu64", show window times: %{public}u", costTime,
        showWindowTimeConfig_.showWindowTimes_.load());
    if (costTime <= 20) { // 20: means cost time is 20ms
        showWindowTimeConfig_.below20msTimes_++;
    } else if (costTime <= 35) { // 35: means cost time is 35ms
        showWindowTimeConfig_.below35msTimes_++;
    } else if (costTime <= 50) { // 50: means cost time is 50ms
        showWindowTimeConfig_.below50msTimes_++;
    } else {
        showWindowTimeConfig_.above50msTimes_++;
    }
    if (showWindowTimeConfig_.showWindowTimes_ >= REPORT_SHOW_WINDOW_TIMES) {
        std::ostringstream oss;
        oss << "show window: " << "BELOW20(ms): " << showWindowTimeConfig_.below20msTimes_
            << ", BELOW35(ms):" << showWindowTimeConfig_.below35msTimes_
            << ", BELOW50(ms): " << showWindowTimeConfig_.below50msTimes_
            << ", ABOVE50(ms): " << showWindowTimeConfig_.above50msTimes_ << ";";
        int32_t ret = OHOS::HiviewDFX::HiSysEvent::Write(
            OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
            "SHOW_WINDOW_TIME",
            OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,
            "MSG", oss.str());
        if (ret != 0) {
            WLOGFE("Write HiSysEvent error, ret:%{public}d", ret);
        } else {
            showWindowTimeConfig_.showWindowTimes_ = 0;
            showWindowTimeConfig_.below20msTimes_ = 0;
            showWindowTimeConfig_.below35msTimes_ = 0;
            showWindowTimeConfig_.below50msTimes_ = 0;
            showWindowTimeConfig_.above50msTimes_ = 0;
        }
    }
}
} // namespace Rosen
} // namespace OHOS
