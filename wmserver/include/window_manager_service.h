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

#ifndef OHOS_WINDOW_MANAGER_SERVICE_H
#define OHOS_WINDOW_MANAGER_SERVICE_H

#include <vector>
#include <map>
#include "event_handler.h"

#include <input_window_monitor.h>
#include <nocopyable.h>
#include <system_ability.h>
#include <window_manager_service_handler_stub.h>
#include <transaction/rs_interfaces.h>
#include "atomic_map.h"
#include "display_change_listener.h"
#include "drag_controller.h"
#include "freeze_controller.h"
#include "singleton_delegator.h"
#include "wm_common_inner.h"
#include "wm_single_instance.h"
#include "window_common_event.h"
#include "window_controller.h"
#include "zidl/window_manager_stub.h"
#include "window_dumper.h"
#include "window_manager_config.h"
#include "window_root.h"
#include "snapshot_controller.h"

namespace OHOS {
namespace Rosen {
class DisplayChangeListener : public IDisplayChangeListener {
public:
    virtual void OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type) override;
    virtual void OnGetWindowPreferredOrientation(DisplayId displayId, Orientation &orientation) override;
    virtual void OnScreenshot(DisplayId displayId) override;
};

class WindowInfoQueriedListener : public IWindowInfoQueriedListener {
public:
    virtual void HasPrivateWindow(DisplayId id, bool& hasPrivateWindow) override;
};

class WindowManagerServiceHandler : public AAFwk::WindowManagerServiceHandlerStub {
public:
    virtual void NotifyWindowTransition(
        sptr<AAFwk::AbilityTransitionInfo> from, sptr<AAFwk::AbilityTransitionInfo> to) override;
    int32_t GetFocusWindow(sptr<IRemoteObject>& abilityToken) override;
    virtual void StartingWindow(
        sptr<AAFwk::AbilityTransitionInfo> info, sptr<Media::PixelMap> pixelMap, uint32_t bgColor) override;
    virtual void StartingWindow(sptr<AAFwk::AbilityTransitionInfo> info, sptr<Media::PixelMap> pixelMap) override;
    virtual void CancelStartingWindow(sptr<IRemoteObject> abilityToken) override;
};

class RSUIDirector;
class WindowManagerService : public SystemAbility, public WindowManagerStub {
friend class DisplayChangeListener;
friend class WindowManagerServiceHandler;
DECLARE_SYSTEM_ABILITY(WindowManagerService);
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowManagerService);

public:
    using Task = std::function<void()>;
    void OnStart() override;
    void OnStop() override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    int Dump(int fd, const std::vector<std::u16string>& args) override;

    WMError CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        uint32_t& windowId, sptr<IRemoteObject> token) override;
    WMError AddWindow(sptr<WindowProperty>& property) override;
    WMError RemoveWindow(uint32_t windowId) override;
    WMError NotifyWindowTransition(sptr<WindowTransitionInfo>& from, sptr<WindowTransitionInfo>& to,
        bool isFromClient = false) override;
    WMError DestroyWindow(uint32_t windowId, bool onlySelf = false) override;
    WMError RequestFocus(uint32_t windowId) override;
    AvoidArea GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType) override;
    void ProcessPointDown(uint32_t windowId, sptr<WindowProperty>& windowProperty,
        sptr<MoveDragProperty>& moveDragProperty) override;
    void ProcessPointUp(uint32_t windowId) override;
    WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId) override;
    void MinimizeAllAppWindows(DisplayId displayId) override;
    WMError ToggleShownStateForAllAppWindows() override;
    WMError SetWindowLayoutMode(WindowLayoutMode mode) override;
    WMError UpdateProperty(sptr<WindowProperty>& windowProperty, PropertyChangeAction action,
        bool isAsyncTask = false) override;
    WMError GetAccessibilityWindowInfo(sptr<AccessibilityWindowInfo>& windowInfo) override;

    void RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;
    void UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;

    WMError SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller) override;
    WMError GetSystemConfig(SystemConfig& systemConfig) override;
    WMError GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones) override;
    WMError UpdateAvoidAreaListener(uint32_t windowId, bool haveAvoidAreaListener) override;
    void StartingWindow(sptr<WindowTransitionInfo> info, sptr<Media::PixelMap> pixelMap,
        bool isColdStart, uint32_t bkgColor = 0xffffffff);
    void CancelStartingWindow(sptr<IRemoteObject> abilityToken);
    void MinimizeWindowsByLauncher(std::vector<uint32_t> windowIds, bool isAnimated,
        sptr<RSIWindowAnimationFinishedCallback>& finishCallback) override;
    void GetWindowPreferredOrientation(DisplayId displayId, Orientation &orientation);
    WMError UpdateRsTree(uint32_t windowId, bool isAdd) override;
    void OnScreenshot(DisplayId displayId);
    void OnAccountSwitched(int accountId);
    WMError BindDialogTarget(uint32_t& windowId, sptr<IRemoteObject> targetToken) override;
    void HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow);
    void NotifyWindowClientPointUp(uint32_t windowId, const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

protected:
    WindowManagerService();
    virtual ~WindowManagerService() = default;

private:
    std::string name_ = "WindowManagerService";
    bool Init();
    void RegisterSnapshotHandler();
    void RegisterWindowManagerServiceHandler();
    void RegisterWindowVisibilityChangeCallback();
    void WindowVisibilityChangeCallback(std::shared_ptr<RSOcclusionData> occlusionData);
    void OnWindowEvent(Event event, const sptr<IRemoteObject>& remoteObject);
    void NotifyDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type);
    WMError GetFocusWindowInfo(sptr<IRemoteObject>& abilityToken);
    void ConfigureWindowManagerService();
    void PostAsyncTask(Task task);
    void PostVoidSyncTask(Task task);
    template<typename SyncTask, typename Return = std::invoke_result_t<SyncTask>>
    Return PostSyncTask(SyncTask&& task)
    {
        Return ret;
        std::function<void()> syncTask([&ret, &task]() {ret = task();});
        if (handler_) {
            handler_->PostSyncTask(syncTask, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        }
        return ret;
    }
    void ConfigHotZones(const std::vector<int>& hotZones);
    void ConfigWindowAnimation(const WindowManagerConfig::ConfigItem& animeConfig);
    void ConfigKeyboardAnimation(const WindowManagerConfig::ConfigItem& animeConfig);
    RSAnimationTimingCurve CreateCurve(const WindowManagerConfig::ConfigItem& curveConfig);
    void RecordShowTimeEvent(int64_t costTime);
    void ConfigWindowEffect(const WindowManagerConfig::ConfigItem& effectConfig);
    bool ConfigAppWindowCornerRadius(const WindowManagerConfig::ConfigItem& item, float& out);
    bool ConfigAppWindowShadow(const WindowManagerConfig::ConfigItem& shadowConfig, WindowShadowParameters& outShadow);

    static inline SingletonDelegator<WindowManagerService> delegator;
    AtomicMap<uint32_t, uint32_t> accessTokenIdMaps_;
    sptr<WindowRoot> windowRoot_;
    sptr<WindowController> windowController_;
    sptr<InputWindowMonitor> inputWindowMonitor_;
    sptr<SnapshotController> snapshotController_;
    sptr<WindowManagerServiceHandler> wmsHandler_;
    sptr<DragController> dragController_;
    sptr<FreezeController> freezeDisplayController_;
    sptr<WindowDumper> windowDumper_;
    SystemConfig systemConfig_;
    ModeChangeHotZonesConfig hotZonesConfig_ { false, 0, 0, 0 };
    std::shared_ptr<WindowCommonEvent> windowCommonEvent_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    RSInterfaces& rsInterface_;
    bool startingOpen_ = true;
    std::shared_ptr<RSUIDirector> rsUiDirector_;
    ShowWindowTimeConfig showWindowTimeConfig_ = { 0, 0, 0, 0, 0 };
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_MANAGER_SERVICE_H
