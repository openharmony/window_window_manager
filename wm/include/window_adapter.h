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

#ifndef OHOS_WINDOW_ADAPTER_H
#define OHOS_WINDOW_ADAPTER_H

#include <refbase.h>
#include <zidl/window_manager_agent_interface.h>

#include "common/include/window_session_property.h"
#include "window.h"
#include "zidl/window_interface.h"
#include "singleton_delegator.h"
#include "window_property.h"
#include "wm_single_instance.h"
#include "zidl/window_manager_interface.h"

namespace OHOS {
namespace Rosen {
class WMSDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;
};

class WindowAdapter {
WM_DECLARE_SINGLE_INSTANCE(WindowAdapter);
public:
    using SessionRecoverCallbackFunc = std::function<void()>;
    using WMSConnectionChangedCallbackFunc = std::function<void(int32_t userId, int32_t screenId, bool isConnected)>;
    virtual WMError CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& windowProperty,
        std::shared_ptr<RSSurfaceNode> surfaceNode, uint32_t& windowId, const sptr<IRemoteObject>& token);
    virtual WMError AddWindow(sptr<WindowProperty>& windowProperty);
    virtual WMError RemoveWindow(uint32_t windowId, bool isFromInnerkits);
    virtual WMError DestroyWindow(uint32_t windowId);
    virtual WMError RequestFocus(uint32_t windowId);
    virtual WMError GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type, AvoidArea& avoidRect);
    virtual WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId);
    virtual void NotifyServerReadyToMoveOrDrag(uint32_t windowId, sptr<WindowProperty>& windowProperty,
        sptr<MoveDragProperty>& moveDragProperty);
    virtual void ProcessPointDown(uint32_t windowId, bool isPointDown = true);
    virtual void ProcessPointUp(uint32_t windowId);
    virtual WMError MinimizeAllAppWindows(DisplayId displayId);
    virtual WMError ToggleShownStateForAllAppWindows();
    virtual WMError SetWindowLayoutMode(WindowLayoutMode mode);
    virtual WMError UpdateProperty(sptr<WindowProperty>& windowProperty, PropertyChangeAction action);
    virtual WMError SetWindowGravity(uint32_t windowId, WindowGravity gravity, uint32_t percent);
    virtual WMError GetSystemConfig(SystemConfig& systemConfig);
    virtual WMError GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones);
    virtual WMError UpdateRsTree(uint32_t windowId, bool isAdd);
    virtual WMError BindDialogTarget(uint32_t& windowId, sptr<IRemoteObject> targetToken);
    virtual WMError RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent);
    virtual WMError UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent);
    virtual WMError CheckWindowId(int32_t windowId, int32_t &pid);

    virtual WMError SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller);
    virtual WMError NotifyWindowTransition(sptr<WindowTransitionInfo> from, sptr<WindowTransitionInfo> to);
    virtual WMError UpdateAvoidAreaListener(uint32_t windowId, bool haveListener);
    virtual void ClearWindowAdapter();

    virtual WMError GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos);
    virtual WMError GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos);
    virtual void MinimizeWindowsByLauncher(std::vector<uint32_t> windowIds, bool isAnimated,
        sptr<RSIWindowAnimationFinishedCallback>& finishCallback);
    virtual void SetAnchorAndScale(int32_t x, int32_t y, float scale);
    virtual void SetAnchorOffset(int32_t deltaX, int32_t deltaY);
    virtual void OffWindowZoom();
    virtual WmErrorCode RaiseToAppTop(uint32_t windowId);
    virtual std::shared_ptr<Media::PixelMap> GetSnapshot(int32_t windowId);
    virtual WMError SetGestureNavigaionEnabled(bool enable);
    virtual void DispatchKeyEvent(uint32_t windowId, std::shared_ptr<MMI::KeyEvent> event);
    virtual void NotifyDumpInfoResult(const std::vector<std::string>& info);
    virtual WMError DumpSessionAll(std::vector<std::string> &infos);
    virtual WMError DumpSessionWithId(int32_t persistentId, std::vector<std::string> &infos);
    virtual WMError GetWindowAnimationTargets(std::vector<uint32_t> missionIds,
        std::vector<sptr<RSWindowAnimationTarget>>& targets);
    virtual void SetMaximizeMode(MaximizeMode maximizeMode);
    virtual MaximizeMode GetMaximizeMode();
    virtual void GetFocusWindowInfo(FocusChangeInfo& focusInfo);
    virtual WMError UpdateSessionAvoidAreaListener(int32_t& persistentId, bool haveListener);
    virtual WMError UpdateSessionTouchOutsideListener(int32_t& persistentId, bool haveListener);
    virtual WMError NotifyWindowExtensionVisibilityChange(int32_t pid, int32_t uid, bool visible);
    virtual WMError UpdateSessionWindowVisibilityListener(int32_t persistentId, bool haveListener);
    virtual void CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
        sptr<IRemoteObject> token = nullptr);
    virtual void RecoverAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, sptr<ISession>& session, sptr<IRemoteObject> token = nullptr);
    virtual void DestroyAndDisconnectSpecificSession(const int32_t& persistentId);
    virtual WMError RecoverAndReconnectSceneSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<ISession>& session, sptr<WindowSessionProperty> property, sptr<IRemoteObject> token = nullptr);
    void RegisterSessionRecoverCallbackFunc(int32_t persistentId, const SessionRecoverCallbackFunc& callbackFunc);
    void UnregisterSessionRecoverCallbackFunc(int32_t persistentId);
    WMError RegisterWMSConnectionChangedListener(const WMSConnectionChangedCallbackFunc& callbackFunc);
    WMError UnregisterWMSConnectionChangedListener();
    virtual WMError UpdateSessionProperty(const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action);
    virtual WMError SetSessionGravity(int32_t persistentId, SessionGravity gravity, uint32_t percent);
    virtual WMError BindDialogSessionTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken);
    virtual WMError RequestFocusStatus(int32_t persistentId, bool isFocused);
    virtual WMError RaiseWindowToTop(int32_t persistentId);
    virtual WMError ShiftAppWindowFocus(int32_t sourcePersistentId, int32_t targetPersistentId);
private:
    static inline SingletonDelegator<WindowAdapter> delegator;
    bool InitWMSProxy();
    bool InitSSMProxy();

    void WindowManagerAndSessionRecover();

    std::recursive_mutex mutex_;
    sptr<IWindowManager> windowManagerServiceProxy_ = nullptr;
    sptr<WMSDeathRecipient> wmsDeath_ = nullptr;
    bool isProxyValid_ { false };

    bool recoverInitialized = false;
    std::map<int32_t, SessionRecoverCallbackFunc> sessionRecoverCallbackFuncMap_;
    std::map<WindowManagerAgentType, std::set<sptr<IWindowManagerAgent>>> windowManagerAgentMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_ADAPTER_H
