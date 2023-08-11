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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H
#define OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H

#include <shared_mutex>
#include <mutex>

#include <transaction/rs_interfaces.h>
#include "agent_death_recipient.h"
#include "common/include/task_scheduler.h"
#include "future.h"
#include "interfaces/include/ws_common.h"
#include "mission_snapshot.h"
#include "session_listener_controller.h"
#include "scene_session_converter.h"
#include "scb_session_handler.h"
#include "session/host/include/root_scene_session.h"
#include "session_manager/include/zidl/scene_session_manager_stub.h"
#include "wm_single_instance.h"
#include "window_scene_config.h"
#include "display_info.h"
#include "display_change_listener.h"

namespace OHOS::AAFwk {
class SessionInfo;
} // namespace OHOS::AAFwk

namespace OHOS::AppExecFwk {
class IBundleMgr;
struct AbilityInfo;
} // namespace OHOS::AppExecFwk

namespace OHOS::Global::Resource {
class ResourceManager;
} // namespace OHOS::Global::Resource

namespace OHOS::Rosen {
class SceneSession;
class AccessibilityWindowInfo;
using NotifyCreateSpecificSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using ProcessGestureNavigationEnabledChangeFunc = std::function<void(bool enable)>;
using ProcessOutsideDownEventFunc = std::function<void(int32_t x, int32_t y)>;
using NotifySetFocusSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using DumpRootSceneElementInfoFunc = std::function<void(const std::vector<std::string>& params,
    std::vector<std::string>& infos)>;
using WindowFocusChangedFunc = std::function<void(int32_t persistentId, bool isFocused)>;

class DisplayChangeListener : public IDisplayChangeListener {
public:
    virtual void OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type) override;
    virtual void OnScreenshot(DisplayId displayId) override;
};

class SceneSessionManager : public SceneSessionManagerStub {
WM_DECLARE_SINGLE_INSTANCE_BASE(SceneSessionManager)
public:
    sptr<SceneSession> RequestSceneSession(const SessionInfo& sessionInfo,
        sptr<WindowSessionProperty> property = nullptr);
    std::future<int32_t> RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession, bool isNewActive);
    WSError RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession, const bool isDelegator = false);
    WSError RequestSceneSessionDestruction(
        const sptr<SceneSession>& sceneSession, const bool needRemoveSession = true);
    WSError RequestSceneSessionByCall(const sptr<SceneSession>& sceneSession);
    void StartAbilityBySpecified(const SessionInfo& sessionInfo);
    void SetRootSceneContext(AbilityRuntime::Context* context);
    sptr<RootSceneSession> GetRootSceneSession();
    sptr<SceneSession> GetSceneSession(int32_t persistentId);
    sptr<SceneSession> GetSceneSessionByName(const std::string& bundleName,
        const std::string& moduleName, const std::string& abilityName);
    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
        sptr<IRemoteObject> token = nullptr);
    WSError DestroyAndDisconnectSpecificSession(const int32_t& persistentId);
    WSError UpdateProperty(sptr<WindowSessionProperty>& property, WSPropertyChangeAction action);
    void SetCreateSpecificSessionListener(const NotifyCreateSpecificSessionFunc& func);
    void SetGestureNavigationEnabledChangeListener(const ProcessGestureNavigationEnabledChangeFunc& func);
    void SetOutsideDownEventListener(const ProcessOutsideDownEventFunc& func);
    const AppWindowSceneConfig& GetWindowSceneConfig() const;
    WSError ProcessBackEvent();
    WSError BindDialogTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken);
    void GetStartPage(const SessionInfo& sessionInfo, std::string& path, uint32_t& bgColor);
    WMError SetGestureNavigaionEnabled(bool enable);
    WMError RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent);
    WMError UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent);

    WSError SetFocusedSession(int32_t persistentId);
    int32_t GetFocusedSession() const;
    WSError GetAllSessionDumpInfo(std::string& info);
    WSError GetSpecifiedSessionDumpInfo(std::string& dumpInfo, const std::vector<std::string>& params,
        const std::string& strId);
    WSError GetSessionDumpInfo(const std::vector<std::string>& params, std::string& info);
    WSError UpdateFocus(int32_t persistentId, bool isFocused);
    WSError UpdateWindowMode(int32_t persistentId, int32_t windowMode);
    WSError SwitchUser(int32_t oldUserId, int32_t newUserId, std::string &fileDir);
    int32_t GetCurrentUserId() const;
    void StartWindowInfoReportLoop();
    void GetFocusWindowInfo(FocusChangeInfo& focusInfo);
    void NotifyCompleteFirstFrameDrawing(int32_t persistentId);
    void NotifySessionMovedToFront(int32_t persistentId);
    WSError SetSessionGravity(int32_t persistentId, SessionGravity gravity, uint32_t percent);
    WSError SetSessionLabel(const sptr<IRemoteObject> &token, const std::string &label);
    WSError SetSessionIcon(const sptr<IRemoteObject> &token, const std::shared_ptr<Media::PixelMap> &icon);
    WSError RegisterSessionListener(const sptr<ISessionChangeListener> sessionListener);
    void UnregisterSessionListener();
    void HandleTurnScreenOn(const sptr<SceneSession>& sceneSession);
    void HandleKeepScreenOn(const sptr<SceneSession>& sceneSession, bool requireLock);
    void InitWithRenderServiceAdded();
    WSError PendingSessionToForeground(const sptr<IRemoteObject> &token);
    WSError PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject> &token);
    WSError GetFocusSessionToken(sptr<IRemoteObject> &token);

    WSError RegisterSessionListener(const sptr<ISessionListener>& listener);
    WSError UnRegisterSessionListener(const sptr<ISessionListener>& listener);
    WSError GetSessionInfos(const std::string& deviceId, int32_t numMax, std::vector<SessionInfoBean>& sessionInfos);
    WSError GetSessionInfo(const std::string& deviceId, int32_t persistentId, SessionInfoBean& sessionInfo);
    WSError GetAllAbilityInfos(const AAFwk::Want &want, int32_t userId,
        std::vector<AppExecFwk::AbilityInfo> &abilityInfos);

    WSError TerminateSessionNew(const sptr<AAFwk::SessionInfo> info, bool needStartCaller);
    WSError UpdateSessionAvoidAreaListener(int32_t& persistentId, bool haveListener);
    WSError GetSessionSnapshot(const std::string& deviceId, int32_t persistentId,
                               std::shared_ptr<Media::PixelMap>& snapshot, bool isLowResolution);
    WSError SetSessionContinueState(const sptr<IRemoteObject> &token, const ContinueState& continueState);
    WSError ClearSession(int32_t persistentId);
    WSError ClearAllSessions();

    void UpdatePrivateStateAndNotify(bool isAddingPrivateSession);
    void InitPersistentStorage();
    std::string GetSessionSnapshotFilePath(int32_t persistentId);
    void OnOutsideDownEvent(int32_t x, int32_t y);
    void NotifySessionTouchOutside(int32_t action, int32_t x, int32_t y);

    WMError GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos);
    WSError SetWindowFlags(const sptr<SceneSession>& sceneSession, uint32_t flags);

    void SetWaterMarkSessionCount(int32_t count);
    int32_t GetWaterMarkSessionCount() const;
    void NotifyOccupiedAreaChangeInfo(const sptr<SceneSession> callingSession,
        const WSRect& rect, const WSRect& occupiedArea);
    void OnScreenshot(DisplayId displayId);
    void NotifyDumpInfoResult(const std::vector<std::string>& info);
    void SetDumpRootSceneElementInfoListener(const DumpRootSceneElementInfoFunc& func);

    RunnableFuture<std::vector<std::string>> dumpInfoFuture_;
    void RegisterWindowFocusChanged(const WindowFocusChangedFunc& func);

    WSError RegisterIAbilityManagerCollaborator(int32_t type, const sptr<AAFwk::IAbilityManagerCollaborator> &impl);
    WSError UnregisterIAbilityManagerCollaborator(int32_t type);
protected:
    SceneSessionManager();
    virtual ~SceneSessionManager() = default;

private:
    void Init();
    void LoadWindowSceneXml();
    void ConfigWindowSceneXml();
    void ConfigWindowEffect(const WindowSceneConfig::ConfigItem& effectConfig);
    void ConfigKeyboardAnimation(const WindowSceneConfig::ConfigItem& animationConfig);
    bool ConfigAppWindowCornerRadius(const WindowSceneConfig::ConfigItem& item, float& out);
    bool ConfigAppWindowShadow(const WindowSceneConfig::ConfigItem& shadowConfig, WindowShadowConfig& outShadow);
    void ConfigDecor(const WindowSceneConfig::ConfigItem& decorConfig);
    void ConfigWindowAnimation(const WindowSceneConfig::ConfigItem& windowAnimationConfig);
    void ConfigStartingWindowAnimation(const WindowSceneConfig::ConfigItem& startingWindowConfig);
    sptr<SceneSession::SpecificSessionCallback> CreateSpecificSessionCallback();

    void RelayoutKeyBoard(sptr<SceneSession> sceneSession);
    void RestoreCallingSessionSizeIfNeed();
    void ResizeSoftInputCallingSessionIfNeed(const sptr<SceneSession>& sceneSession);

    sptr<AAFwk::SessionInfo> SetAbilitySessionInfo(const sptr<SceneSession>& scnSession);
    WSError DestroyDialogWithMainWindow(const sptr<SceneSession>& scnSession);
    sptr<SceneSession> FindMainWindowWithToken(sptr<IRemoteObject> targetToken);
    WSError UpdateParentSession(const sptr<SceneSession>& sceneSession, sptr<WindowSessionProperty> property);
    void UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing);
    void UpdateFocusableProperty(int32_t persistentId);
    std::vector<sptr<SceneSession>> GetSceneSessionVectorByType(WindowType type);
    bool UpdateSessionAvoidAreaIfNeed(const int32_t& persistentId,
        const AvoidArea& avoidArea, AvoidAreaType avoidAreaType);
    bool UpdateAvoidArea(const int32_t& persistentId);

    sptr<AppExecFwk::IBundleMgr> GetBundleManager();
    std::shared_ptr<Global::Resource::ResourceManager> CreateResourceManager(
        const AppExecFwk::AbilityInfo& abilityInfo);
    void GetStartPageFromResource(const AppExecFwk::AbilityInfo& abilityInfo, std::string& path, uint32_t& bgColor);
    std::string CreateCurve(
        const WindowSceneConfig::ConfigItem& curveConfig, const std::string& nodeName = "keyboardAnimation");

    bool CheckIsRemote(const std::string& deviceId);
    bool GetLocalDeviceId(std::string& localDeviceId);
    std::string AnonymizeDeviceId(const std::string& deviceId);
    int GetRemoteSessionInfos(const std::string& deviceId, int32_t numMax,
                              std::vector<SessionInfoBean> &sessionInfos);
    int GetRemoteSessionInfo(const std::string& deviceId, int32_t persistentId, SessionInfoBean& sessionInfo);

    WSError SetBrightness(const sptr<SceneSession>& sceneSession, float brightness);
    WSError UpdateBrightness(int32_t persistentId);
    void SetDisplayBrightness(float brightness);
    float GetDisplayBrightness() const;
    void HandleUpdateProperty(const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action,
        const sptr<SceneSession>& sceneSession);
    void NotifyWindowInfoChange(int32_t persistentId, WindowUpdateType type);
    bool FillWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos,
        const sptr<SceneSession>& sceneSession);
    std::vector<std::pair<uint64_t, bool>> GetWindowVisibilityChangeInfo(
        std::shared_ptr<RSOcclusionData> occlusionData);
    void WindowVisibilityChangeCallback(std::shared_ptr<RSOcclusionData> occlusiontionData);
    void WindowDestroyNotifyVisibility(const sptr<SceneSession>& sceneSession);
    void RegisterInputMethodShownFunc(const sptr<SceneSession>& sceneSession);
    void OnInputMethodShown(const int32_t& persistentId);
    void RegisterInputMethodHideFunc(const sptr<SceneSession>& sceneSession);
    void RegisterSessionExceptionFunc(const sptr<SceneSession>& sceneSession);
    bool IsSessionVisible(const sptr<SceneSession>& session);
    void DumpSessionInfo(const sptr<SceneSession>& session, std::ostringstream& oss);
    void DumpAllAppSessionInfo(std::ostringstream& oss);
    void DumpSessionElementInfo(const sptr<SceneSession>& session,
        const std::vector<std::string>& params, std::string& dumpInfo);
    void AddClientDeathRecipient(const sptr<ISessionStage>& sessionStage, const sptr<SceneSession>& sceneSession);
    void DestroySpecificSession(const sptr<IRemoteObject>& remoteObject);
    void CleanUserMap();

    sptr<RootSceneSession> rootSceneSession_;
    std::shared_ptr<AbilityRuntime::Context> rootSceneContext_;
    std::shared_mutex sceneSessionMapMutex_;
    std::map<int32_t, sptr<SceneSession>> sceneSessionMap_;
    sptr<ScbSessionHandler> scbSessionHandler_;
    std::shared_ptr<SessionListenerController> listenerController_;
    std::map<sptr<IRemoteObject>, int32_t> remoteObjectMap_;
    std::set<sptr<SceneSession>> avoidAreaListenerSessionSet_;
    std::map<int32_t, std::map<AvoidAreaType, AvoidArea>> lastUpdatedAvoidArea_;

    NotifyCreateSpecificSessionFunc createSpecificSessionFunc_;
    ProcessGestureNavigationEnabledChangeFunc gestureNavigationEnabledChangeFunc_;
    ProcessOutsideDownEventFunc outsideDownEventFunc_;
    DumpRootSceneElementInfoFunc dumpRootSceneFunc_;
    AppWindowSceneConfig appWindowSceneConfig_;
    SystemSessionConfig systemConfig_;
    int32_t focusedSessionId_ = INVALID_SESSION_ID;
    int32_t brightnessSessionId_ = INVALID_SESSION_ID;
    float displayBrightness_ = UNDEFINED_BRIGHTNESS;
    WSRect callingWindowRestoringRect_ = {0, 0, 0, 0};
    int32_t currentUserId_;

    std::shared_ptr<TaskScheduler> taskScheduler_;
    sptr<AppExecFwk::IBundleMgr> bundleMgr_;

    std::shared_ptr<AppExecFwk::EventRunner> eventLoop_;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_;
    bool isReportTaskStart_ = false;
    std::shared_ptr<RSOcclusionData> lastOcclusionData_ = std::make_shared<RSOcclusionData>();
    RSInterfaces& rsInterface_;
    void RegisterSessionStateChangeNotifyManagerFunc(sptr<SceneSession>& sceneSession);
    void OnSessionStateChange(int32_t persistentId, const SessionState& state);
    sptr<ISessionChangeListener> sessionListener_;
    sptr<SceneSession> FindSessionByToken(const sptr<IRemoteObject> &token);

    void CheckAndNotifyWaterMarkChangedResult(bool isAddingWaterMark);
    WSError NotifyWaterMarkFlagChangedResult(bool hasWaterMark);
    int32_t waterMarkSessionCount_ { 0 };
    WindowFocusChangedFunc windowFocusChangedFunc_;
    sptr<SceneSession> callingSession_ = nullptr;
    sptr<AgentDeathRecipient> windowDeath_ = new AgentDeathRecipient(
        std::bind(&SceneSessionManager::DestroySpecificSession, this, std::placeholders::_1));
    WSError ClearSession(sptr<SceneSession> sceneSession);
    bool IsSessionClearable(sptr<SceneSession> scnSession);
    void GetAllClearableSessions(std::vector<sptr<SceneSession>>& sessionVector);
    int GetRemoteSessionSnapshotInfo(const std::string& deviceId, int32_t sessionId,
                                     AAFwk::MissionSnapshot& sessionSnapshot);

    const int32_t BROKER_UID = 5528;
    const int32_t BROKER_RESERVE_UID = 5005;
    std::shared_mutex collaboratorMapLock_;
    std::unordered_map<int32_t, sptr<AAFwk::IAbilityManagerCollaborator>> collaboratorMap_;

    bool CheckCollaboratorType(int32_t type);
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H
