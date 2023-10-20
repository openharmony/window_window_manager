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

#include <mutex>
#include <shared_mutex>

#include "mission_snapshot.h"
#include "transaction/rs_interfaces.h"

#include "agent_death_recipient.h"
#include "common/include/task_scheduler.h"
#include "future.h"
#include "interfaces/include/ws_common.h"
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
struct BundleInfo;
} // namespace OHOS::AppExecFwk

namespace OHOS::Global::Resource {
class ResourceManager;
} // namespace OHOS::Global::Resource

namespace OHOS::Rosen {
namespace AncoConsts {
    constexpr const char* ANCO_MISSION_ID = "ohos.anco.param.missionId";
    constexpr const char* ANCO_SESSION_ID = "ohos.anco.param.sessionId";
}

class SceneSession;
class AccessibilityWindowInfo;
using NotifyCreateSpecificSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using ProcessStatusBarEnabledChangeFunc = std::function<void(bool enable)>;
using ProcessGestureNavigationEnabledChangeFunc = std::function<void(bool enable)>;
using ProcessOutsideDownEventFunc = std::function<void(int32_t x, int32_t y)>;
using NotifySetFocusSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using DumpRootSceneElementInfoFunc = std::function<void(const std::vector<std::string>& params,
    std::vector<std::string>& infos)>;
using ProcessVirtualPixelRatioChangeFunc = std::function<void(float density, const Rect& rect)>;
using WindowChangedFunc = std::function<void(int32_t persistentId, WindowUpdateType type)>;

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
    void UpdateSceneSessionWant(const SessionInfo& sessionInfo);
    std::future<int32_t> RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession, bool isNewActive);
    WSError RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession, const bool isDelegator = false);
    WSError RequestSceneSessionDestruction(
        const sptr<SceneSession>& sceneSession, const bool needRemoveSession = true);
    void NotifyForegroundInteractiveStatus(const sptr<SceneSession>& sceneSession, bool interactive);
    WSError RequestSceneSessionByCall(const sptr<SceneSession>& sceneSession);
    void StartAbilityBySpecified(const SessionInfo& sessionInfo);

    void SetRootSceneContext(const std::weak_ptr<AbilityRuntime::Context>& contextWeak);
    sptr<RootSceneSession> GetRootSceneSession();
    sptr<SceneSession> GetSceneSession(int32_t persistentId);
    sptr<SceneSession> GetSceneSessionByName(const std::string& bundleName,
        const std::string& moduleName, const std::string& abilityName, const int32_t appIndex);

    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
        sptr<IRemoteObject> token = nullptr);
    WSError DestroyAndDisconnectSpecificSession(const int32_t& persistentId);
    WMError UpdateProperty(sptr<WindowSessionProperty>& property, WSPropertyChangeAction action);
    void SetCreateSpecificSessionListener(const NotifyCreateSpecificSessionFunc& func);
    void SetStatusBarEnabledChangeListener(const ProcessStatusBarEnabledChangeFunc& func);
    void SetGestureNavigationEnabledChangeListener(const ProcessGestureNavigationEnabledChangeFunc& func);
    void SetDumpRootSceneElementInfoListener(const DumpRootSceneElementInfoFunc& func);
    void SetOutsideDownEventListener(const ProcessOutsideDownEventFunc& func);
    const AppWindowSceneConfig& GetWindowSceneConfig() const;
    WSError ProcessBackEvent();
    WSError BindDialogTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken);
    void GetStartupPage(const SessionInfo& sessionInfo, std::string& path, uint32_t& bgColor);
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
    WSError SendTouchEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, uint32_t zIndex);

    WSError SwitchUser(int32_t oldUserId, int32_t newUserId, std::string &fileDir);
    int32_t GetCurrentUserId() const;
    void StartWindowInfoReportLoop();
    void GetFocusWindowInfo(FocusChangeInfo& focusInfo);
    void NotifyCompleteFirstFrameDrawing(int32_t persistentId);
    void NotifySessionMovedToFront(int32_t persistentId);
    WSError SetSessionGravity(int32_t persistentId, SessionGravity gravity, uint32_t percent);
    WSError SetSessionLabel(const sptr<IRemoteObject> &token, const std::string &label);
    WSError SetSessionIcon(const sptr<IRemoteObject> &token, const std::shared_ptr<Media::PixelMap> &icon);
    WSError IsValidSessionIds(const std::vector<int32_t> &sessionIds, std::vector<bool> &results);
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
    WSError DumpSessionAll(std::vector<std::string> &infos);
    WSError DumpSessionWithId(int32_t persistentId, std::vector<std::string> &infos);
    WSError GetAllAbilityInfos(const AAFwk::Want &want, int32_t userId,
        std::vector<AppExecFwk::AbilityInfo> &abilityInfos);
    WSError PrepareTerminate(int32_t persistentId, bool& isPrepareTerminate);

    WSError TerminateSessionNew(const sptr<AAFwk::SessionInfo> info, bool needStartCaller);
    WSError UpdateSessionAvoidAreaListener(int32_t& persistentId, bool haveListener);
    WSError GetSessionSnapshot(const std::string& deviceId, int32_t persistentId,
                               SessionSnapshot& snapshot, bool isLowResolution);
    WSError SetSessionContinueState(const sptr<IRemoteObject> &token, const ContinueState& continueState);
    WSError ClearSession(int32_t persistentId);
    WSError ClearAllSessions();
    WSError LockSession(int32_t sessionId);
    WSError UnlockSession(int32_t sessionId);
    WSError MoveSessionsToForeground(const std::vector<int32_t>& sessionIds, int32_t topSessionId);
    WSError MoveSessionsToBackground(const std::vector<int32_t>& sessionIds, std::vector<int32_t>& result);

    std::map<int32_t, sptr<SceneSession>>& GetSessionMapByScreenId(ScreenId id);
    void UpdatePrivateStateAndNotify(uint32_t persistentId);
    void InitPersistentStorage();
    std::string GetSessionSnapshotFilePath(int32_t persistentId);
    void OnOutsideDownEvent(int32_t x, int32_t y);
    void NotifySessionTouchOutside(int32_t persistentId);

    WMError GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos);
    WSError SetWindowFlags(const sptr<SceneSession>& sceneSession, uint32_t flags);

    void NotifyOccupiedAreaChangeInfo(const sptr<SceneSession> callingSession,
        const WSRect& rect, const WSRect& occupiedArea);
    void OnScreenshot(DisplayId displayId);
    void NotifyDumpInfoResult(const std::vector<std::string>& info);
    void SetVirtualPixelRatioChangeListener(const ProcessVirtualPixelRatioChangeFunc& func);
    void ProcessVirtualPixelRatioChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type);

    RunnableFuture<std::vector<std::string>> dumpInfoFuture_;
    void RegisterWindowChanged(const WindowChangedFunc& func);

    WSError RegisterIAbilityManagerCollaborator(int32_t type, const sptr<AAFwk::IAbilityManagerCollaborator> &impl);
    WSError UnregisterIAbilityManagerCollaborator(int32_t type);

    WMError CheckWindowId(int32_t windowId, int32_t &pid);
    int GetSceneSessionPrivacyModeCount();
    bool CheckIfReuseSession(SessionInfo& sessionInfo);
    bool CheckCollaboratorType(int32_t type);
    sptr<SceneSession> FindSessionByAffinity(std::string affinity);
public:
    std::shared_ptr<TaskScheduler> GetTaskScheduler() {return taskScheduler_;};
protected:
    SceneSessionManager();
    virtual ~SceneSessionManager() = default;

private:
    void Init();
    void InitPrepareTerminateConfig();
    void LoadWindowSceneXml();
    void ConfigWindowSceneXml();
    void ConfigWindowEffect(const WindowSceneConfig::ConfigItem& effectConfig);
    void ConfigKeyboardAnimation(const WindowSceneConfig::ConfigItem& animationConfig);
    void ConfigDefaultKeyboardAnimation();
    bool ConfigAppWindowCornerRadius(const WindowSceneConfig::ConfigItem& item, float& out);
    bool ConfigAppWindowShadow(const WindowSceneConfig::ConfigItem& shadowConfig, WindowShadowConfig& outShadow);
    void ConfigDecor(const WindowSceneConfig::ConfigItem& decorConfig);
    void ConfigWindowAnimation(const WindowSceneConfig::ConfigItem& windowAnimationConfig);
    void ConfigStartingWindowAnimation(const WindowSceneConfig::ConfigItem& startingWindowConfig);
    void ConfigWindowSizeLimits();
    void ConfigMainWindowSizeLimits(const WindowSceneConfig::ConfigItem& mainWindowSizeConifg);
    void ConfigSubWindowSizeLimits(const WindowSceneConfig::ConfigItem& subWindowSizeConifg);
    void ConfigSnapshotScale();

    std::tuple<std::string, std::vector<float>> CreateCurve(const WindowSceneConfig::ConfigItem& curveConfig);
    void LoadKeyboardAnimation(const WindowSceneConfig::ConfigItem& item, KeyboardSceneAnimationConfig& config);
    sptr<SceneSession::SpecificSessionCallback> CreateSpecificSessionCallback();
    void FillSessionInfo(sptr<SceneSession>& sceneSession);
    std::shared_ptr<AppExecFwk::AbilityInfo> QueryAbilityInfoFromBMS(const int32_t uId, const std::string& bundleName,
        const std::string& abilityName, const std::string& moduleName);

    void RelayoutKeyBoard(sptr<SceneSession> sceneSession);
    void RestoreCallingSessionSizeIfNeed();
    void ResizeSoftInputCallingSessionIfNeed(const sptr<SceneSession>& sceneSession, bool isInputUpdated = false);

    sptr<AAFwk::SessionInfo> SetAbilitySessionInfo(const sptr<SceneSession>& scnSession);
    WSError DestroyDialogWithMainWindow(const sptr<SceneSession>& scnSession);
    sptr<SceneSession> FindMainWindowWithToken(sptr<IRemoteObject> targetToken);
    WSError UpdateParentSessionForDialog(const sptr<SceneSession>& sceneSession, sptr<WindowSessionProperty> property);
    void UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing);
    void UpdateFocusableProperty(int32_t persistentId);
    std::vector<sptr<SceneSession>> GetSceneSessionVectorByType(WindowType type);
    bool UpdateSessionAvoidAreaIfNeed(const int32_t& persistentId,
        const sptr<SceneSession>& sceneSession, const AvoidArea& avoidArea, AvoidAreaType avoidAreaType);
    void UpdateAvoidSessionAvoidArea(WindowType type, bool& needUpdate);
    void UpdateNormalSessionAvoidArea(const int32_t& persistentId, sptr<SceneSession>& sceneSession, bool& needUpdate);
    void UpdateAvoidArea(const int32_t& persistentId);
    int32_t GetStatusBarHeight();

    sptr<AppExecFwk::IBundleMgr> GetBundleManager();
    std::shared_ptr<Global::Resource::ResourceManager> GetResourceManager(const AppExecFwk::AbilityInfo& abilityInfo);
    void GetStartupPageFromResource(const AppExecFwk::AbilityInfo& abilityInfo, std::string& path, uint32_t& bgColor);

    bool CheckIsRemote(const std::string& deviceId);
    bool GetLocalDeviceId(std::string& localDeviceId);
    std::string AnonymizeDeviceId(const std::string& deviceId);
    int GetRemoteSessionInfos(const std::string& deviceId, int32_t numMax,
                              std::vector<SessionInfoBean> &sessionInfos);
    int GetRemoteSessionInfo(const std::string& deviceId, int32_t persistentId, SessionInfoBean& sessionInfo);

    void PerformRegisterInRequestSceneSession(sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionActivationInner(sptr<SceneSession>& scnSession,
        bool isNewActive, const std::shared_ptr<std::promise<int32_t>>& promise);
    WSError SetBrightness(const sptr<SceneSession>& sceneSession, float brightness);
    WSError UpdateBrightness(int32_t persistentId);
    void SetDisplayBrightness(float brightness);
    float GetDisplayBrightness() const;
    WMError HandleUpdateProperty(const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action,
        const sptr<SceneSession>& sceneSession);
    void UpdateHideNonSystemFloatingWindows(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession);
    void UpdateForceHideState(const sptr<SceneSession>& sceneSession, const sptr<WindowSessionProperty>& property,
        bool add);
    void NotifyWindowInfoChange(int32_t persistentId, WindowUpdateType type);
    bool FillWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos,
        const sptr<SceneSession>& sceneSession);
    std::vector<std::pair<uint64_t, bool>> GetWindowVisibilityChangeInfo(
        std::shared_ptr<RSOcclusionData> occlusionData);
    void WindowVisibilityChangeCallback(std::shared_ptr<RSOcclusionData> occlusiontionData);
    sptr<SceneSession> SelectSesssionFromMap(const uint64_t& surfaceId);
    void WindowDestroyNotifyVisibility(const sptr<SceneSession>& sceneSession);
    void RegisterInputMethodUpdateFunc(const sptr<SceneSession>& sceneSession);
    void OnInputMethodUpdate(const int32_t& persistentId);
    void RegisterInputMethodShownFunc(const sptr<SceneSession>& sceneSession);
    void OnInputMethodShown(const int32_t& persistentId);
    void RegisterInputMethodHideFunc(const sptr<SceneSession>& sceneSession);
    void RegisterSessionExceptionFunc(const sptr<SceneSession>& sceneSession);
    void RegisterSessionSnapshotFunc(const sptr<SceneSession>& sceneSession);
    void NotifySessionForCallback(const sptr<SceneSession>& scnSession, const bool needRemoveSession);
    bool IsSessionVisible(const sptr<SceneSession>& session);
    void DumpSessionInfo(const sptr<SceneSession>& session, std::ostringstream& oss);
    void DumpAllAppSessionInfo(std::ostringstream& oss);
    void DumpSessionElementInfo(const sptr<SceneSession>& session,
        const std::vector<std::string>& params, std::string& dumpInfo);
    void AddClientDeathRecipient(const sptr<ISessionStage>& sessionStage, const sptr<SceneSession>& sceneSession);
    void DestroySpecificSession(const sptr<IRemoteObject>& remoteObject);
    void CleanUserMap();
    WSError GetAbilityInfosFromBundleInfo(std::vector<AppExecFwk::BundleInfo> &bundleInfos,
        std::vector<AppExecFwk::AbilityInfo> &abilityInfos);

    WMError UpdatePropertyDragEnabled(const sptr<WindowSessionProperty>& property,
                                      const sptr<SceneSession>& sceneSession);
    WMError UpdatePropertyRaiseEnabled(const sptr<WindowSessionProperty>& property,
                                       const sptr<SceneSession>& sceneSession);
    sptr<RootSceneSession> rootSceneSession_;
    std::weak_ptr<AbilityRuntime::Context> rootSceneContextWeak_;
    std::shared_mutex sceneSessionMapMutex_;
    std::map<int32_t, sptr<SceneSession>> sceneSessionMap_;
    std::map<int32_t, sptr<SceneSession>> systemTopSceneSessionMap_;
    std::map<int32_t, sptr<SceneSession>> nonSystemFloatSceneSessionMap_;
    sptr<ScbSessionHandler> scbSessionHandler_;
    std::shared_ptr<SessionListenerController> listenerController_;
    std::map<sptr<IRemoteObject>, int32_t> remoteObjectMap_;
    std::set<int32_t> avoidAreaListenerSessionSet_;
    std::map<int32_t, std::map<AvoidAreaType, AvoidArea>> lastUpdatedAvoidArea_;

    NotifyCreateSpecificSessionFunc createSpecificSessionFunc_;
    ProcessStatusBarEnabledChangeFunc statusBarEnabledChangeFunc_;
    ProcessGestureNavigationEnabledChangeFunc gestureNavigationEnabledChangeFunc_;
    ProcessOutsideDownEventFunc outsideDownEventFunc_;
    DumpRootSceneElementInfoFunc dumpRootSceneFunc_;
    ProcessVirtualPixelRatioChangeFunc processVirtualPixelRatioChangeFunc_ = nullptr;
    AppWindowSceneConfig appWindowSceneConfig_;
    SystemSessionConfig systemConfig_;
    float snapshotScale_ = 0.5;
    int32_t focusedSessionId_ = INVALID_SESSION_ID;
    int32_t brightnessSessionId_ = INVALID_SESSION_ID;
    float displayBrightness_ = UNDEFINED_BRIGHTNESS;
    bool isPrepareTerminateEnable_ {false};
    WSRect callingWindowRestoringRect_ = {0, 0, 0, 0};
    bool needUpdateSessionRect_ = false;
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
    void ProcessSubSessionForeground(sptr<SceneSession>& sceneSession);
    void ProcessSubSessionBackground(sptr<SceneSession>& sceneSession);
    sptr<ISessionChangeListener> sessionListener_;
    sptr<SceneSession> FindSessionByToken(const sptr<IRemoteObject> &token);

    void CheckAndNotifyWaterMarkChangedResult();
    WSError NotifyWaterMarkFlagChangedResult(bool hasWaterMark);
    void ProcessPreload(const AppExecFwk::AbilityInfo& abilityInfo) const;
    bool lastWaterMarkShowState_ { false };
    WindowChangedFunc WindowChangedFunc_;
    sptr<AgentDeathRecipient> windowDeath_ = new AgentDeathRecipient(
        std::bind(&SceneSessionManager::DestroySpecificSession, this, std::placeholders::_1));
    sptr<SceneSession> callingSession_ = nullptr;

    WSError ClearSession(sptr<SceneSession> sceneSession);
    bool IsSessionClearable(sptr<SceneSession> scnSession);
    void GetAllClearableSessions(std::vector<sptr<SceneSession>>& sessionVector);
    int GetRemoteSessionSnapshotInfo(const std::string& deviceId, int32_t sessionId,
                                     AAFwk::MissionSnapshot& sessionSnapshot);

    const int32_t BROKER_UID = 5557;
    const int32_t BROKER_RESERVE_UID = 5005;
    std::shared_mutex collaboratorMapLock_;
    std::unordered_map<int32_t, sptr<AAFwk::IAbilityManagerCollaborator>> collaboratorMap_;

    void NotifyStartAbility(int32_t collaboratorType, const SessionInfo& sessionInfo);
    void NotifySessionCreate(const sptr<SceneSession> sceneSession, const SessionInfo& sessionInfo);
    void NotifyLoadAbility(int32_t collaboratorType, sptr<AAFwk::SessionInfo> abilitySessionInfo,
        std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo);
    void NotifyUpdateSessionInfo(const sptr<SceneSession> sceneSession);
    void NotifyClearSession(int32_t collaboratorType, int32_t persistentId);
    void NotifyMoveSessionToForeground(int32_t collaboratorType, int32_t persistendId);
    void PreHandleCollaborator(sptr<SceneSession>& sceneSession);
    void NotifyCollaboratorAfterStart(sptr<SceneSession>& scnSession, sptr<AAFwk::SessionInfo>& scnSessionInfo);
    void UpdateCollaboratorSessionWant(sptr<SceneSession>& session);
    bool CheckSystemWindowPermission(const sptr<WindowSessionProperty>& property) const;
    void DestroySubSession(const sptr<SceneSession>& sceneSession);
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H
