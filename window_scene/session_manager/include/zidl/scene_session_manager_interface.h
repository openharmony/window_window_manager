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
#include "focus_change_info.h"
#include "session_listener_interface.h"
#include "window_manager.h"
#include "zidl/window_manager_interface.h"
#include "session_info.h"
#include "mission_listener_interface.h"
#include "mission_info.h"
#include "iability_manager_collaborator.h"

namespace OHOS::Media {
class PixelMap;
} // namespace OHOS::Media

namespace OHOS::Rosen {
class RSSurfaceNode;
using ISessionListener = AAFwk::IMissionListener;
using SessionInfoBean = AAFwk::MissionInfo;
class ISceneSessionManager : public IWindowManager {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ISceneSessionManager");

    enum class SceneSessionManagerMessage : uint32_t {
        TRANS_ID_CREATE_AND_CONNECT_SPECIFIC_SESSION,
        TRANS_ID_DESTROY_AND_DISCONNECT_SPECIFIC_SESSION,
        TRANS_ID_UPDATE_PROPERTY,
        TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT,
        TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT,
        TRANS_ID_BIND_DIALOG_TARGET,
        TRANS_ID_GET_FOCUS_SESSION_INFO,
        TRANS_ID_SET_SESSION_GRAVITY,
        TRANS_ID_SET_GESTURE_NAVIGATION_ENABLED,
        TRANS_ID_SET_SESSION_LABEL,
        TRANS_ID_SET_SESSION_ICON,
        TRANS_ID_IS_VALID_SESSION_IDS,
        TRANS_ID_REGISTER_SESSION_CHANGE_LISTENER,
        TRANS_ID_UNREGISTER_SESSION_CHANGE_LISTENER,
        TRANS_ID_GET_WINDOW_INFO,
        TRANS_ID_PENDING_SESSION_TO_FOREGROUND,
        TRANS_ID_PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR,
        TRANS_ID_GET_FOCUS_SESSION_TOKEN,
        TRANS_ID_REGISTER_SESSION_LISTENER,
        TRANS_ID_UNREGISTER_SESSION_LISTENER,
        TRANS_ID_GET_MISSION_INFOS,
        TRANS_ID_GET_MISSION_INFO_BY_ID,
        TRANS_ID_DUMP_SESSION_ALL,
        TRANS_ID_DUMP_SESSION_WITH_ID,
        TRANS_ID_TERMINATE_SESSION_NEW,
        TRANS_ID_GET_SESSION_DUMP_INFO,
        TRANS_ID_UPDATE_AVOIDAREA_LISTENER,
        TRANS_ID_GET_SESSION_SNAPSHOT,
        TRANS_ID_SET_SESSION_CONTINUE_STATE,
        TRANS_ID_NOTIFY_DUMP_INFO_RESULT,
        TRANS_ID_CLEAR_SESSION,
        TRANS_ID_CLEAR_ALL_SESSIONS,
        TRANS_ID_LOCK_SESSION,
        TRANS_ID_UNLOCK_SESSION,
        TRANS_ID_MOVE_MISSIONS_TO_FOREGROUND,
        TRANS_ID_MOVE_MISSIONS_TO_BACKGROUND,
        TRANS_ID_REGISTER_COLLABORATOR,
        TRANS_ID_UNREGISTER_COLLABORATOR,
    };

    virtual WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
        sptr<IRemoteObject> token = nullptr) = 0;
    virtual WSError DestroyAndDisconnectSpecificSession(const int32_t& persistentId) = 0;
    virtual WSError UpdateProperty(sptr<WindowSessionProperty>& property, WSPropertyChangeAction action) = 0;
    virtual WSError BindDialogTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken) = 0;
    virtual WSError SetSessionGravity(int32_t persistentId, SessionGravity gravity, uint32_t percent) = 0;
    virtual WSError SetSessionLabel(const sptr<IRemoteObject> &token, const std::string &label) = 0;
    virtual WSError SetSessionIcon(const sptr<IRemoteObject> &token, const std::shared_ptr<Media::PixelMap> &icon) = 0;
    virtual WSError IsValidSessionIds(const std::vector<int32_t> &sessionIds, std::vector<bool> &results) = 0;
    virtual WSError RegisterSessionListener(const sptr<ISessionChangeListener> sessionListener) = 0;
    virtual void UnregisterSessionListener() = 0;
    virtual WSError PendingSessionToForeground(const sptr<IRemoteObject> &token) = 0;
    virtual WSError PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject> &token) = 0;
    virtual WSError GetFocusSessionToken(sptr<IRemoteObject> &token) = 0;

    virtual WSError RegisterSessionListener(const sptr<ISessionListener>& listener) = 0;
    virtual WSError UnRegisterSessionListener(const sptr<ISessionListener>& listener) = 0;
    virtual WSError GetSessionInfos(const std::string& deviceId,
                                    int32_t numMax, std::vector<SessionInfoBean>& sessionInfos) = 0;
    virtual WSError GetSessionInfo(const std::string& deviceId, int32_t persistentId, SessionInfoBean& sessionInfo) = 0;
    virtual WSError DumpSessionAll(std::vector<std::string> &infos) override { return WSError::WS_OK; }
    virtual WSError DumpSessionWithId(int32_t persistentId, std::vector<std::string> &infos) override
    {
        return WSError::WS_OK;
    }
    virtual WSError SetSessionContinueState(const sptr<IRemoteObject> &token, const ContinueState& continueState) = 0;

    virtual WSError TerminateSessionNew(const sptr<AAFwk::SessionInfo> info, bool needStartCaller) = 0;
    virtual WSError GetSessionDumpInfo(const std::vector<std::string>& params, std::string& info) = 0;
    virtual WSError GetSessionSnapshot(const std::string& deviceId, int32_t persistentId,
                                       std::shared_ptr<Media::PixelMap> &snapshot, bool isLowResolution) = 0;
    virtual WSError ClearSession(int32_t persistentId) = 0;
    virtual WSError ClearAllSessions() = 0;
    virtual WSError LockSession(int32_t sessionId) = 0;
    virtual WSError UnlockSession(int32_t sessionId) = 0;
    virtual WSError MoveSessionsToForeground(const std::vector<std::int32_t>& sessionIds) = 0;
    virtual WSError MoveSessionsToBackground(const std::vector<std::int32_t>& sessionIds) = 0;

    virtual WSError RegisterIAbilityManagerCollaborator(int32_t type, const sptr<AAFwk::IAbilityManagerCollaborator> &impl) = 0;
    virtual WSError UnregisterIAbilityManagerCollaborator(int32_t type) = 0;
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
    void GetFocusWindowInfo(FocusChangeInfo& focusInfo) override {}
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_INTERFACE_H
