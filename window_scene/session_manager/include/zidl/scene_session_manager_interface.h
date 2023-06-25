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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_INTERFACE_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_INTERFACE_H

#include "common/include/window_session_property.h"
#include <iremote_broker.h>
#include "interfaces/include/ws_common.h"
#include "interfaces/include/ws_common_inner.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "session/container/include/zidl/window_event_channel_interface.h"
#include "session/host/include/session.h"

#include "window_manager_interface.h"

namespace OHOS::Rosen {
class RSSurfaceNode;
class ISceneSessionManager : public IWindowManager {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ISceneSessionManager");

    enum class SceneSessionManagerMessage : uint32_t {
        TRANS_ID_CREATE_AND_CONNECT_SPECIFIC_SESSION,
        TRANS_ID_DESTROY_AND_DISCONNECT_SPECIFIC_SESSION,
        TRANS_ID_UPDATE_PROPERTY,
        TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT,
        TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT,
    };

    virtual WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session) = 0;
    virtual WSError DestroyAndDisconnectSpecificSession(const uint64_t& persistentId) = 0;
    virtual WSError UpdateProperty(sptr<WindowSessionProperty>& property, WSPropertyChangeAction action) = 0;

    // interfaces of IWindowManager
    WMError CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        uint32_t& windowId, sptr<IRemoteObject> token) override { return WMError::WM_OK; }
    WMError AddWindow(sptr<WindowProperty>& property) override { return WMError::WM_OK; }
    WMError RemoveWindow(uint32_t windowId, bool isFromInnerkits) override { return WMError::WM_OK; }
    WMError DestroyWindow(uint32_t windowId, bool onlySelf = false) override { return WMError::WM_OK; }
    WMError RequestFocus(uint32_t windowId) override { return WMError::WM_OK; }
    AvoidArea GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type) override { return {}; }
    WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId) override { return WMError::WM_OK; }
    void NotifyServerReadyToMoveOrDrag(uint32_t windowId, sptr<WindowProperty>& windowProperty,
        sptr<MoveDragProperty>& moveDragProperty) override {}
    void ProcessPointDown(uint32_t windowId, bool isPointDown) override {}
    void ProcessPointUp(uint32_t windowId) override {}
    WMError MinimizeAllAppWindows(DisplayId displayId) override { return WMError::WM_OK; }
    WMError ToggleShownStateForAllAppWindows() override { return WMError::WM_OK; }
    WMError SetWindowLayoutMode(WindowLayoutMode mode) override { return WMError::WM_OK; }
    WMError UpdateProperty(sptr<WindowProperty>& windowProperty, PropertyChangeAction action,
        bool isAsyncTask = false) override { return WMError::WM_OK; }
    WMError SetWindowGravity(uint32_t windowId, WindowGravity gravity, uint32_t percent) override
    {
        return WMError::WM_OK;
    }
    WMError RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override { return WMError::WM_OK; }
    WMError UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override { return WMError::WM_OK; }
    WMError GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos) override
    {
        return WMError::WM_OK;
    }
    WMError GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos) override { return WMError::WM_OK; }
    WMError SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller) override
    {
        return WMError::WM_OK;
    }
    WMError GetSystemConfig(SystemConfig& systemConfig) override { return WMError::WM_OK; }
    WMError NotifyWindowTransition(sptr<WindowTransitionInfo>& from, sptr<WindowTransitionInfo>& to,
        bool isFromClient = false) override { return WMError::WM_OK; }
    WMError GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones) override { return WMError::WM_OK; }
    void MinimizeWindowsByLauncher(std::vector<uint32_t> windowIds, bool isAnimated,
        sptr<RSIWindowAnimationFinishedCallback>& finishCallback) override {}
    WMError UpdateAvoidAreaListener(uint32_t windowId, bool haveListener) override { return WMError::WM_OK; }
    WMError UpdateRsTree(uint32_t windowId, bool isAdd) override { return WMError::WM_OK; }
    WMError BindDialogTarget(uint32_t& windowId, sptr<IRemoteObject> targetToken) override { return WMError::WM_OK; }
    void SetAnchorAndScale(int32_t x, int32_t y, float scale) override {}
    void SetAnchorOffset(int32_t deltaX, int32_t deltaY) override {}
    void OffWindowZoom() override {}
    WmErrorCode RaiseToAppTop(uint32_t windowId) override { return WmErrorCode::WM_OK; }
    std::shared_ptr<Media::PixelMap> GetSnapshot(int32_t windowId) override { return nullptr; }
    WMError SetGestureNavigaionEnabled(bool enable) override { return WMError::WM_OK; }
    void DispatchKeyEvent(uint32_t windowId, std::shared_ptr<MMI::KeyEvent> event) override {}
    void NotifyDumpInfoResult(const std::vector<std::string>& info) override {};
    WMError GetWindowAnimationTargets(std::vector<uint32_t> missionIds,
        std::vector<sptr<RSWindowAnimationTarget>>& targets) override { return WMError::WM_OK; }
    void SetMaximizeMode(MaximizeMode maximizeMode) override {}
    MaximizeMode GetMaximizeMode() override { return MaximizeMode::MODE_AVOID_SYSTEM_BAR; }
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_INTERFACE_H