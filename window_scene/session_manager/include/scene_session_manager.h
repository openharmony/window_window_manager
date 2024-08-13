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

#include <cstdint>
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
#include "session/host/include/keyboard_session.h"
#include "session_manager/include/zidl/scene_session_manager_stub.h"
#include "wm_single_instance.h"
#include "window_scene_config.h"
#include "display_info.h"
#include "display_change_info.h"
#include "display_change_listener.h"
#include "app_debug_listener_interface.h"
#include "app_mgr_client.h"
#include "include/core/SkRegion.h"
#include "ability_info.h"
#include "screen_fold_data.h"

namespace OHOS::AAFwk {
class SessionInfo;
} // namespace OHOS::AAFwk

namespace OHOS::AppExecFwk {
class IBundleMgr;
struct AbilityInfo;
struct BundleInfo;
class LauncherService;
} // namespace OHOS::AppExecFwk

namespace OHOS::Global::Resource {
class ResourceManager;
} // namespace OHOS::Global::Resource

namespace OHOS::Rosen {
namespace AncoConsts {
    constexpr const char* ANCO_MISSION_ID = "ohos.anco.param.missionId";
    constexpr const char* ANCO_SESSION_ID = "ohos.anco.param.sessionId";
}
struct SCBAbilityInfo {
    AppExecFwk::AbilityInfo abilityInfo_;
    uint32_t sdkVersion_;
};
class SceneSession;
struct SecSurfaceInfo;
class RSUIExtensionData;
class AccessibilityWindowInfo;
class UnreliableWindowInfo;
using NotifyCreateSystemSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifyCreateKeyboardSessionFunc = std::function<void(const sptr<SceneSession>& keyboardSession,
    const sptr<SceneSession>& panelSession)>;
using NotifyCreateSubSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifyRecoverSceneSessionFunc =
    std::function<void(const sptr<SceneSession>& session, const SessionInfo& sessionInfo)>;
using ProcessStatusBarEnabledChangeFunc = std::function<void(bool enable, const std::string bundleName)>;
using ProcessGestureNavigationEnabledChangeFunc = std::function<void(bool enable, const std::string bundleName)>;
using ProcessOutsideDownEventFunc = std::function<void(int32_t x, int32_t y)>;
using ProcessShiftFocusFunc = std::function<void(int32_t persistentId)>;
using NotifySetFocusSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using DumpRootSceneElementInfoFunc = std::function<void(const std::vector<std::string>& params,
    std::vector<std::string>& infos)>;
using WindowChangedFunc = std::function<void(int32_t persistentId, WindowUpdateType type)>;
using TraverseFunc = std::function<bool(const sptr<SceneSession>& session)>;
using CmpFunc = std::function<bool(std::pair<int32_t, sptr<SceneSession>>& lhs,
    std::pair<int32_t, sptr<SceneSession>>& rhs)>;
using ProcessStartUIAbilityErrorFunc = std::function<void(int32_t startUIAbilityError)>;
using NotifySCBAfterUpdateFocusFunc = std::function<void()>;
using ProcessCallingSessionIdChangeFunc = std::function<void(uint32_t callingSessionId)>;
using FlushWindowInfoTask = std::function<void()>;
using ProcessVirtualPixelRatioChangeFunc = std::function<void(float density, const Rect& rect)>;
using DumpUITreeFunc = std::function<void(uint64_t, std::string& dumpInfo)>;
using RootSceneProcessBackEventFunc = std::function<void()>;

class AppAnrListener : public IRemoteStub<AppExecFwk::IAppDebugListener> {
public:
    void OnAppDebugStarted(const std::vector<AppExecFwk::AppDebugInfo>& debugInfos) override;
    void OnAppDebugStoped(const std::vector<AppExecFwk::AppDebugInfo>& debugInfos) override;
};

class DisplayChangeListener : public IDisplayChangeListener {
public:
    virtual void OnDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type) override;
    virtual void OnScreenshot(DisplayId displayId) override;
    virtual void OnImmersiveStateChange(bool& immersive) override;
    virtual void OnGetSurfaceNodeIdsFromMissionIds(std::vector<uint64_t>& missionIds,
        std::vector<uint64_t>& surfaceNodeIds) override;

    /*
     * Fold Screen Status Change Report
     */
    virtual void OnScreenFoldStatusChanged(const std::vector<std::string>& screenFoldInfo) override;
};

class SceneSessionManager : public SceneSessionManagerStub {
WM_DECLARE_SINGLE_INSTANCE_BASE(SceneSessionManager)
public:
    friend class AnomalyDetection;
    bool IsSessionVisible(const sptr<SceneSession>& session);
    bool IsSessionVisibleForeground(const sptr<SceneSession>& session);
    sptr<SceneSession> RequestSceneSession(const SessionInfo& sessionInfo,
        sptr<WindowSessionProperty> property = nullptr);
    void UpdateSceneSessionWant(const SessionInfo& sessionInfo);
    std::future<int32_t> RequestSceneSessionActivation(const sptr<SceneSession>& sceneSession, bool isNewActive);
    WSError RequestSceneSessionBackground(const sptr<SceneSession>& sceneSession, const bool isDelegator = false,
        const bool isToDesktop = false, const bool isSaveSnapshot = true);
    WSError RequestSceneSessionDestruction(const sptr<SceneSession>& sceneSession, bool needRemoveSession = true,
        bool isSaveSnapshot = true, const bool isForceClean = false);
    WSError RequestSceneSessionDestructionInner(sptr<SceneSession>& scnSession, sptr<AAFwk::SessionInfo> scnSessionInfo,
        const bool needRemoveSession, const bool isForceClean = false);
    void NotifyForegroundInteractiveStatus(const sptr<SceneSession>& sceneSession, bool interactive);
    WSError RequestSceneSessionByCall(const sptr<SceneSession>& sceneSession);
    void StartAbilityBySpecified(const SessionInfo& sessionInfo);

    void SetRootSceneContext(const std::weak_ptr<AbilityRuntime::Context>& contextWeak);
    sptr<RootSceneSession> GetRootSceneSession();
    WSRect GetRootSessionAvoidSessionRect(AvoidAreaType type);
    sptr<SceneSession> GetSceneSession(int32_t persistentId);
    void PostFlushWindowInfoTask(FlushWindowInfoTask &&task, const std::string taskName, const int delayTime);

    sptr<SceneSession> GetSceneSessionByName(const std::string& bundleName,
        const std::string& moduleName, const std::string& abilityName, const int32_t appIndex);

    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
        SystemSessionConfig& systemConfig, sptr<IRemoteObject> token = nullptr) override;
    WSError RecoverAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, sptr<ISession>& session, sptr<IRemoteObject> token = nullptr) override;
    WSError RecoverAndReconnectSceneSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<ISession>& session, sptr<WindowSessionProperty> property = nullptr,
        sptr<IRemoteObject> token = nullptr) override;
    WSError DestroyAndDisconnectSpecificSession(const int32_t persistentId) override;
    WSError DestroyAndDisconnectSpecificSessionWithDetachCallback(const int32_t persistentId,
        const sptr<IRemoteObject>& callback) override;
    void SetCreateSystemSessionListener(const NotifyCreateSystemSessionFunc& func);
    void SetCreateKeyboardSessionListener(const NotifyCreateKeyboardSessionFunc& func);
    void SetStatusBarEnabledChangeListener(const ProcessStatusBarEnabledChangeFunc& func);
    void SetStartUIAbilityErrorListener(const ProcessStartUIAbilityErrorFunc& func);
    void SetRecoverSceneSessionListener(const NotifyRecoverSceneSessionFunc& func);
    void SetGestureNavigationEnabledChangeListener(const ProcessGestureNavigationEnabledChangeFunc& func);
    void SetDumpRootSceneElementInfoListener(const DumpRootSceneElementInfoFunc& func);
    void SetOutsideDownEventListener(const ProcessOutsideDownEventFunc& func);
    void SetShiftFocusListener(const ProcessShiftFocusFunc& func);
    void SetSCBFocusedListener(const NotifySCBAfterUpdateFocusFunc& func);
    void SetSCBUnfocusedListener(const NotifySCBAfterUpdateFocusFunc& func);
    void SetCallingSessionIdSessionListenser(const ProcessCallingSessionIdChangeFunc& func);
    void SetDumpUITreeFunc(const DumpUITreeFunc& func);
    const AppWindowSceneConfig& GetWindowSceneConfig() const;
    WSError ProcessBackEvent();
    WSError BindDialogSessionTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken) override;
    void GetStartupPage(const SessionInfo& sessionInfo, std::string& path, uint32_t& bgColor);
    WMError SetGestureNavigaionEnabled(bool enable) override;
    WMError RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;
    WMError UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override;

    WSError SetFocusedSessionId(int32_t persistentId);
    int32_t GetFocusedSessionId() const;
    FocusChangeReason GetFocusChangeReason() const { return focusChangeReason_; }
    WSError GetAllSessionDumpInfo(std::string& info);
    WSError GetSpecifiedSessionDumpInfo(std::string& dumpInfo, const std::vector<std::string>& params,
        const std::string& strId);
    WSError GetSCBDebugDumpInfo(std::string& dumpInfo, const std::vector<std::string>& params);
    WSError GetSessionDumpInfo(const std::vector<std::string>& params, std::string& info) override;
    WMError RequestFocusStatus(int32_t persistentId, bool isFocused, bool byForeground = true,
        FocusChangeReason reason = FocusChangeReason::DEFAULT) override;
    WMError RequestFocusStatusBySCB(int32_t persistentId, bool isFocused, bool byForeground = true,
        FocusChangeReason reason = FocusChangeReason::DEFAULT);
    void RequestAllAppSessionUnfocus();
    WSError UpdateFocus(int32_t persistentId, bool isFocused);
    WSError UpdateWindowMode(int32_t persistentId, int32_t windowMode);
    WSError SendTouchEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, uint32_t zIndex);
    WSError RaiseWindowToTop(int32_t persistentId) override;
    void SetScreenLocked(const bool isScreenLocked);
    bool IsScreenLocked() const;

    WSError InitUserInfo(int32_t userId, std::string& fileDir);
    void NotifySwitchingUser(const bool isUserActive);
    int32_t GetCurrentUserId() const;
    void StartWindowInfoReportLoop();
    void GetFocusWindowInfo(FocusChangeInfo& focusInfo) override;
    void NotifyCompleteFirstFrameDrawing(int32_t persistentId);
    void NotifySessionMovedToFront(int32_t persistentId);
    WSError SetSessionLabel(const sptr<IRemoteObject>& token, const std::string& label) override;
    WSError SetSessionIcon(const sptr<IRemoteObject>& token, const std::shared_ptr<Media::PixelMap>& icon) override;
    WSError IsValidSessionIds(const std::vector<int32_t>& sessionIds, std::vector<bool>& results) override;
    void HandleTurnScreenOn(const sptr<SceneSession>& sceneSession);
    void HandleKeepScreenOn(const sptr<SceneSession>& sceneSession, bool requireLock);
    void InitWithRenderServiceAdded();
    WSError PendingSessionToForeground(const sptr<IRemoteObject>& token) override;
    WSError PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject>& token) override;
    WSError GetFocusSessionToken(sptr<IRemoteObject>& token) override;
    WSError GetFocusSessionElement(AppExecFwk::ElementName& element) override;
    WSError RegisterSessionListener(const sptr<ISessionListener>& listener) override;
    WSError UnRegisterSessionListener(const sptr<ISessionListener>& listener) override;
    WSError GetSessionInfos(const std::string& deviceId, int32_t numMax,
        std::vector<SessionInfoBean>& sessionInfos) override;
    WSError GetSessionInfo(const std::string& deviceId, int32_t persistentId, SessionInfoBean& sessionInfo) override;
    WSError GetSessionInfoByContinueSessionId(const std::string& continueSessionId,
        SessionInfoBean& sessionInfo) override;
    WSError DumpSessionAll(std::vector<std::string>& infos) override;
    WSError DumpSessionWithId(int32_t persistentId, std::vector<std::string>& infos) override;
    WSError GetAllAbilityInfos(const AAFwk::Want& want, int32_t userId,
        std::vector<SCBAbilityInfo>& scbAbilityInfos);
    WSError PrepareTerminate(int32_t persistentId, bool& isPrepareTerminate);
    WSError GetIsLayoutFullScreen(bool& isLayoutFullScreen);

    WSError TerminateSessionNew(
        const sptr<AAFwk::SessionInfo> info, bool needStartCaller, bool isFromBroker = false) override;
    WSError UpdateSessionAvoidAreaListener(int32_t& persistentId, bool haveListener) override;
    WSError UpdateSessionTouchOutsideListener(int32_t& persistentId, bool haveListener) override;
    WSError GetSessionSnapshot(const std::string& deviceId, int32_t persistentId,
                               SessionSnapshot& snapshot, bool isLowResolution) override;
    WMError GetSessionSnapshotById(int32_t persistentId, SessionSnapshot& snapshot) override;
    WSError SetVmaCacheStatus(bool flag);
    WSError GetUIContentRemoteObj(int32_t persistentId, sptr<IRemoteObject>& uiContentRemoteObj) override;
    WSError SetSessionContinueState(const sptr<IRemoteObject>& token, const ContinueState& continueState) override;
    WSError ClearSession(int32_t persistentId) override;
    WSError ClearAllSessions() override;
    WSError LockSession(int32_t sessionId) override;
    WSError UnlockSession(int32_t sessionId) override;
    WSError MoveSessionsToForeground(const std::vector<int32_t>& sessionIds, int32_t topSessionId) override;
    WSError MoveSessionsToBackground(const std::vector<int32_t>& sessionIds, std::vector<int32_t>& result) override;
    WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId) override;

    std::map<int32_t, sptr<SceneSession>>& GetSessionMapByScreenId(ScreenId id);
    void UpdatePrivateStateAndNotify(uint32_t persistentId);
    void InitPersistentStorage();
    std::string GetSessionSnapshotFilePath(int32_t persistentId);
    void OnOutsideDownEvent(int32_t x, int32_t y);
    void NotifySessionTouchOutside(int32_t persistentId);

    WMError GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos) override;
    WMError GetUnreliableWindowInfo(int32_t windowId,
        std::vector<sptr<UnreliableWindowInfo>>& infos) override;
    WSError SetWindowFlags(const sptr<SceneSession>& sceneSession, const sptr<WindowSessionProperty>& property);

    void OnScreenshot(DisplayId displayId);
    void NotifyDumpInfoResult(const std::vector<std::string>& info) override;
    void SetVirtualPixelRatioChangeListener(const ProcessVirtualPixelRatioChangeFunc& func);
    void ProcessVirtualPixelRatioChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type);
    void ProcessUpdateRotationChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type);

    void SetRootSceneProcessBackEventFunc(const RootSceneProcessBackEventFunc& processBackEventFunc);
    void RegisterWindowChanged(const WindowChangedFunc& func);

    WSError RegisterIAbilityManagerCollaborator(int32_t type,
        const sptr<AAFwk::IAbilityManagerCollaborator>& impl) override;
    WSError UnregisterIAbilityManagerCollaborator(int32_t type) override;

    bool IsInputEventEnabled();
    void SetEnableInputEvent(bool enabled);
    void UpdateRecoveredSessionInfo(const std::vector<int32_t>& recoveredPersistentIds);
    void SetAlivePersistentIds(const std::vector<int32_t>& alivePersistentIds);
    void NotifyRecoveringFinished();

    WMError CheckWindowId(int32_t windowId, int32_t& pid) override;
    void GetSceneSessionPrivacyModeBundles(DisplayId displayId, std::unordered_set<std::string>& privacyBundles);
    BrokerStates CheckIfReuseSession(SessionInfo& sessionInfo);
    bool CheckCollaboratorType(int32_t type);
    sptr<SceneSession> FindSessionByAffinity(std::string affinity);
    void AddWindowDragHotArea(uint32_t type, WSRect& area);
    void PreloadInLakeApp(const std::string& bundleName);
    WSError UpdateMaximizeMode(int32_t persistentId, bool isMaximize);
    bool GetImmersiveState();
    WSError UpdateSessionDisplayId(int32_t persistentId, uint64_t screenId);
    WSError NotifyStackEmpty(int32_t persistentId);
    void NotifySessionUpdate(const SessionInfo& sessionInfo, ActionType type,
        ScreenId fromScreenId = SCREEN_ID_INVALID);
    WSError NotifyAINavigationBarShowStatus(bool isVisible, WSRect barArea, uint64_t displayId);
    WSRect GetAINavigationBarArea(uint64_t displayId);
    WMError GetSurfaceNodeIdsFromMissionIds(std::vector<uint64_t>& missionIds,
        std::vector<uint64_t>& surfaceNodeIds);
    WSError UpdateTitleInTargetPos(int32_t persistentId, bool isShow, int32_t height);
    void RegisterCreateSubSessionListener(int32_t persistentId, const NotifyCreateSubSessionFunc& func);
    void UnregisterCreateSubSessionListener(int32_t persistentId);

    WSError NotifyWindowExtensionVisibilityChange(int32_t pid, int32_t uid, bool visible) override;
    void DealwithVisibilityChange(const std::vector<std::pair<uint64_t, WindowVisibilityState>>& visibilityChangeInfos);
    void DealwithDrawingContentChange(const std::vector<std::pair<uint64_t, bool>>& drawingChangeInfos);
    void NotifyUpdateRectAfterLayout();
    void FlushUIParams(ScreenId screenId, std::unordered_map<int32_t, SessionUIParam>&& uiParams);
    WSError UpdateSessionWindowVisibilityListener(int32_t persistentId, bool haveListener) override;
    WMError SetSystemAnimatedScenes(SystemAnimatedSceneType sceneType);
    WSError ShiftAppWindowFocus(int32_t sourcePersistentId, int32_t targetPersistentId) override;
    std::shared_ptr<Media::PixelMap> GetSessionSnapshotPixelMap(const int32_t persistentId, const float scaleParam);
    void RequestInputMethodCloseKeyboard(int32_t persistentId);
    WMError GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos) override;
    const std::map<int32_t, sptr<SceneSession>> GetSceneSessionMap();
    void GetAllSceneSession(std::vector<sptr<SceneSession>>& sceneSessions);
    void GetAllWindowVisibilityInfos(std::vector<std::pair<int32_t, uint32_t>>& windowVisibilityInfos);
    void FlushWindowInfoToMMI(const bool forceFlush = false);
    int32_t StartUIAbilityBySCB(sptr<AAFwk::SessionInfo>& abilitySessionInfo);
    int32_t StartUIAbilityBySCB(sptr<SceneSession>& sceneSessions);
    int32_t ChangeUIAbilityVisibilityBySCB(sptr<SceneSession>& sceneSessions, bool visibility);
    void AddExtensionWindowStageToSCB(const sptr<ISessionStage>& sessionStage,
        const sptr<IRemoteObject>& token, uint64_t surfaceNodeId) override;
    void RemoveExtensionWindowStageFromSCB(const sptr<ISessionStage>& sessionStage,
        const sptr<IRemoteObject>& token) override;
    void UpdateModalExtensionRect(const sptr<IRemoteObject>& token, Rect rect) override;
    void ProcessModalExtensionPointDown(const sptr<IRemoteObject>& token, int32_t posX, int32_t posY) override;
    WSError AddOrRemoveSecureSession(int32_t persistentId, bool shouldHide) override;
    WSError CheckExtWindowFlagsPermission(ExtensionWindowFlags& actions) const;
    WSError UpdateExtWindowFlags(const sptr<IRemoteObject>& token, uint32_t extWindowFlags,
        uint32_t extWindowActions) override;
    void CheckSceneZOrder();
    WSError GetHostWindowRect(int32_t hostWindowId, Rect& rect) override;
    WMError GetCallingWindowWindowStatus(int32_t persistentId, WindowStatus& windowStatus) override;
    WMError GetCallingWindowRect(int32_t persistentId, Rect& rect) override;
    WMError GetWindowModeType(WindowModeType& windowModeType) override;

    int32_t ReclaimPurgeableCleanMem();
    void OnBundleUpdated(const std::string& bundleName, int userId);
    void OnConfigurationUpdated(const std::shared_ptr<AppExecFwk::Configuration>& configuration);

    std::shared_ptr<TaskScheduler> GetTaskScheduler() {return taskScheduler_;};
    int32_t GetCustomDecorHeight(int32_t persistentId);
    WSError SwitchFreeMultiWindow(bool enable);
    WSError GetFreeMultiWindowEnableState(bool& enable) override;
    const SystemSessionConfig& GetSystemSessionConfig() const;
    void ClearDisplayStatusBarTemporarilyFlags();
    WSError NotifyEnterRecentTask(bool enterRecent);
    WMError GetMainWindowInfos(int32_t topNum, std::vector<MainWindowInfo>& topNInfo);
    WMError GetAllMainWindowInfos(std::vector<MainWindowInfo>& infos) const;
    WMError ClearMainSessions(const std::vector<int32_t>& persistentIds, std::vector<int32_t>& clearFailedIds);
    WMError UpdateDisplayHookInfo(int32_t uid, uint32_t width, uint32_t height, float_t density, bool enable);
    void InitScheduleUtils();
    void ProcessDisplayScale(sptr<DisplayInfo> displayInfo);

    /*
     * Fold Screen Status Change Report
     */
    WMError ReportScreenFoldStatusChange(const std::vector<std::string>& screenFoldInfo);

    void UpdateSecSurfaceInfo(std::shared_ptr<RSUIExtensionData> secExtensionData, uint64_t userid);
    WMError GetWindowStyleType(WindowStyleType& windowStyletype) override;
    WSError SetAppForceLandscapeConfig(const std::string& bundleName, const AppForceLandscapeConfig& config);
    AppForceLandscapeConfig GetAppForceLandscapeConfig(const std::string& bundleName);
    WMError TerminateSessionByPersistentId(int32_t persistentId);
    WMError GetProcessSurfaceNodeIdByPersistentId(const int32_t pid,
        const std::vector<int32_t>& persistentIds, std::vector<uint64_t>& surfaceNodeIds) override;

protected:
    SceneSessionManager();
    virtual ~SceneSessionManager();

private:
    std::atomic<bool> enterRecent_ { false };
    bool isKeyboardPanelEnabled_ = false;
    static sptr<SceneSessionManager> CreateInstance();
    void Init();
    void RegisterAppListener();
    void InitPrepareTerminateConfig();
    void LoadWindowSceneXml();
    void ConfigWindowSceneXml();
    void ConfigWindowSceneXml(const WindowSceneConfig::ConfigItem& config);
    void ConfigWindowEffect(const WindowSceneConfig::ConfigItem& effectConfig);
    void ConfigWindowImmersive(const WindowSceneConfig::ConfigItem& immersiveConfig);
    void ConfigKeyboardAnimation(const WindowSceneConfig::ConfigItem& animationConfig);
    void ConfigDefaultKeyboardAnimation();
    bool ConfigAppWindowCornerRadius(const WindowSceneConfig::ConfigItem& item, float& out);
    bool ConfigAppWindowShadow(const WindowSceneConfig::ConfigItem& shadowConfig, WindowShadowConfig& outShadow);
    bool ConfigStatusBar(const WindowSceneConfig::ConfigItem& config, StatusBarConfig& statusBarConfig);
    void ConfigSystemUIStatusBar(const WindowSceneConfig::ConfigItem& statusBarConfig);
    void ConfigDecor(const WindowSceneConfig::ConfigItem& decorConfig, bool mainConfig = true);
    void ConfigWindowAnimation(const WindowSceneConfig::ConfigItem& windowAnimationConfig);
    void ConfigStartingWindowAnimation(const WindowSceneConfig::ConfigItem& startingWindowConfig);
    void ConfigWindowSizeLimits();
    void ConfigMainWindowSizeLimits(const WindowSceneConfig::ConfigItem& mainWindowSizeConifg);
    void ConfigSubWindowSizeLimits(const WindowSceneConfig::ConfigItem& subWindowSizeConifg);
    void ConfigSnapshotScale();
    void ConfigFreeMultiWindow();
    void LoadFreeMultiWindowConfig(bool enable);

    std::tuple<std::string, std::vector<float>> CreateCurve(const WindowSceneConfig::ConfigItem& curveConfig);
    void LoadKeyboardAnimation(const WindowSceneConfig::ConfigItem& item, KeyboardSceneAnimationConfig& config);
    sptr<SceneSession::SpecificSessionCallback> CreateSpecificSessionCallback();
    sptr<KeyboardSession::KeyboardSessionCallback> CreateKeyboardSessionCallback();
    void FillSessionInfo(sptr<SceneSession>& sceneSession);
    std::shared_ptr<AppExecFwk::AbilityInfo> QueryAbilityInfoFromBMS(const int32_t uId, const std::string& bundleName,
        const std::string& abilityName, const std::string& moduleName);
    std::vector<sptr<SceneSession>> GetSubSceneSession(int32_t parentWindowId);
    void RemoveDuplicateSubSession(const std::vector<std::pair<uint64_t, WindowVisibilityState>>& visibilityChangeInfo,
        std::vector<sptr<SceneSession>>& subSessions);
    void SetSessionVisibilityInfo(const sptr<SceneSession>& session, WindowVisibilityState visibleState,
        std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos, std::string& visibilityInfo);
    void UpdateSubWindowVisibility(const sptr<SceneSession>& session, WindowVisibilityState visibleState,
        const std::vector<std::pair<uint64_t, WindowVisibilityState>>& visibilityChangeInfo,
        std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfos, std::string& visibilityInfo);

    void PostProcessFocus();
    void PostProcessProperty();
    std::vector<std::pair<int32_t, sptr<SceneSession>>> GetSceneSessionVector(CmpFunc cmp);
    void TraverseSessionTree(TraverseFunc func, bool isFromTopToBottom);
    void TraverseSessionTreeFromTopToBottom(TraverseFunc func);
    void TraverseSessionTreeFromBottomToTop(TraverseFunc func);
    WSError RequestSessionFocus(int32_t persistentId, bool byForeground = true,
        FocusChangeReason reason = FocusChangeReason::DEFAULT);
    WSError RequestSessionFocusImmediately(int32_t persistentId);
    WSError RequestSessionUnfocus(int32_t persistentId, FocusChangeReason reason = FocusChangeReason::DEFAULT);
    WSError RequestAllAppSessionUnfocusInner();
    WSError RequestFocusBasicCheck(int32_t persistentId);
    WSError RequestFocusSpecificCheck(sptr<SceneSession>& sceneSession, bool byForeground,
        FocusChangeReason reason = FocusChangeReason::DEFAULT);
    bool CheckTopmostWindowFocus(sptr<SceneSession>& focusedSession, sptr<SceneSession>& sceneSession);
    bool CheckRequestFocusImmdediately(sptr<SceneSession>& sceneSession);
    bool CheckFocusIsDownThroughBlockingType(sptr<SceneSession>& requestSceneSession,
        sptr<SceneSession>& focusedSession, bool includingAppSession);
    bool CheckClickFocusIsDownThroughFullScreen(const sptr<SceneSession>& focusedSession,
        const sptr<SceneSession>& sceneSession, FocusChangeReason reason);
    bool CheckParentSessionVisible(const sptr<SceneSession>& session);
    void InitSceneSession(sptr<SceneSession>& sceneSession, const SessionInfo& sessionInfo,
        const sptr<WindowSessionProperty>& property);

    sptr<SceneSession> GetNextFocusableSession(int32_t persistentId);
    sptr<SceneSession> GetTopNearestBlockingFocusSession(uint32_t zOrder, bool includingAppSession);
    sptr<SceneSession> GetTopFocusableNonAppSession();
    WSError ShiftFocus(sptr<SceneSession>& nextSession, FocusChangeReason reason = FocusChangeReason::DEFAULT);
    void UpdateFocusStatus(sptr<SceneSession>& sceneSession, bool isFocused);
    void NotifyFocusStatus(sptr<SceneSession>& sceneSession, bool isFocused);
    int32_t NotifyRssThawApp(const int32_t uid, const std::string& bundleName,
        const std::string& reason);
    void NotifyFocusStatusByMission(sptr<SceneSession>& prevSession, sptr<SceneSession>& currSession);
    void NotifyUnFocusedByMission(sptr<SceneSession>& sceneSession);
    bool MissionChanged(sptr<SceneSession>& prevSession, sptr<SceneSession>& currSession);
    std::string GetAllSessionFocusInfo();
    void RegisterRequestFocusStatusNotifyManagerFunc(sptr<SceneSession>& sceneSession);
    void RegisterGetStateFromManagerFunc(sptr<SceneSession>& sceneSession);
    void RegisterSessionChangeByActionNotifyManagerFunc(sptr<SceneSession>& sceneSession);

    sptr<AAFwk::SessionInfo> SetAbilitySessionInfo(const sptr<SceneSession>& scnSession);
    WSError DestroyDialogWithMainWindow(const sptr<SceneSession>& scnSession);
    sptr<SceneSession> FindMainWindowWithToken(sptr<IRemoteObject> targetToken);
    WSError UpdateParentSessionForDialog(const sptr<SceneSession>& sceneSession, sptr<WindowSessionProperty> property);
    void UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing);
    void UpdateFocusableProperty(int32_t persistentId);
    WMError UpdateTopmostProperty(const sptr<WindowSessionProperty>& property, const sptr<SceneSession>& sceneSession);
    std::vector<sptr<SceneSession>> GetSceneSessionVectorByType(WindowType type, uint64_t displayId);
    bool UpdateSessionAvoidAreaIfNeed(const int32_t& persistentId,
        const sptr<SceneSession>& sceneSession, const AvoidArea& avoidArea, AvoidAreaType avoidAreaType);
    void UpdateAvoidSessionAvoidArea(WindowType type, bool& needUpdate);
    void UpdateNormalSessionAvoidArea(const int32_t& persistentId, sptr<SceneSession>& sceneSession, bool& needUpdate);
    void UpdateAvoidArea(const int32_t persistentId);
    void NotifyMMIWindowPidChange(int32_t windowId, bool startMoving);

    sptr<AppExecFwk::IBundleMgr> GetBundleManager();
    std::shared_ptr<Global::Resource::ResourceManager> GetResourceManager(const AppExecFwk::AbilityInfo& abilityInfo);
    bool GetStartupPageFromResource(const AppExecFwk::AbilityInfo& abilityInfo, std::string& path, uint32_t& bgColor);
    bool GetStartingWindowInfoFromCache(const SessionInfo& sessionInfo, std::string& path, uint32_t& bgColor);
    void CacheStartingWindowInfo(
        const AppExecFwk::AbilityInfo& abilityInfo, const std::string& path, const uint32_t& bgColor);

    bool CheckIsRemote(const std::string& deviceId);
    bool GetLocalDeviceId(std::string& localDeviceId);
    std::string AnonymizeDeviceId(const std::string& deviceId);
    int GetRemoteSessionInfos(const std::string& deviceId, int32_t numMax,
                              std::vector<SessionInfoBean>& sessionInfos);
    int GetRemoteSessionInfo(const std::string& deviceId, int32_t persistentId, SessionInfoBean& sessionInfo);
    WSError GetTotalUITreeInfo(const std::string& strId, std::string& dumpInfo);

    void PerformRegisterInRequestSceneSession(sptr<SceneSession>& sceneSession);
    WSError RequestSceneSessionActivationInner(sptr<SceneSession>& scnSession,
        bool isNewActive, const std::shared_ptr<std::promise<int32_t>>& promise);
    WSError SetBrightness(const sptr<SceneSession>& sceneSession, float brightness);
    WSError UpdateBrightness(int32_t persistentId);
    void SetDisplayBrightness(float brightness);
    float GetDisplayBrightness() const;
    void HandleSpecificSystemBarProperty(WindowType type, const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession);
    void HandleHideNonSystemFloatingWindows(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession);
    void UpdateForceHideState(const sptr<SceneSession>& sceneSession, const sptr<WindowSessionProperty>& property,
        bool add);
    void NotifyWindowInfoChange(int32_t persistentId, WindowUpdateType type);
    void NotifyWindowInfoChangeFromSession(int32_t persistentid);
    bool FillWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos,
        const sptr<SceneSession>& sceneSession);
    std::vector<std::pair<uint64_t, WindowVisibilityState>> GetWindowVisibilityChangeInfo(
        std::vector<std::pair<uint64_t, WindowVisibilityState>>& currVisibleData);
    std::vector<std::pair<uint64_t, bool>> GetWindowDrawingContentChangeInfo(
        std::vector<std::pair<uint64_t, bool>> currDrawingContentData);
    void GetWindowLayerChangeInfo(std::shared_ptr<RSOcclusionData> occlusionData,
        std::vector<std::pair<uint64_t, WindowVisibilityState>>& currVisibleData,
        std::vector<std::pair<uint64_t, bool>>& currDrawingContentData);
    void WindowLayerInfoChangeCallback(std::shared_ptr<RSOcclusionData> occlusiontionData);
    sptr<SceneSession> SelectSesssionFromMap(const uint64_t& surfaceId);
    void WindowDestroyNotifyVisibility(const sptr<SceneSession>& sceneSession);
    void RegisterSessionExceptionFunc(const sptr<SceneSession>& sceneSession);
    void RegisterSessionSnapshotFunc(const sptr<SceneSession>& sceneSession);
    void NotifySessionForCallback(const sptr<SceneSession>& scnSession, const bool needRemoveSession);
    void DumpSessionInfo(const sptr<SceneSession>& session, std::ostringstream& oss);
    void DumpSessionElementInfo(const sptr<SceneSession>& session,
        const std::vector<std::string>& params, std::string& dumpInfo);
    void DumpAllSessionFocusableInfo(int32_t persistentId);
    void AddClientDeathRecipient(const sptr<ISessionStage>& sessionStage, const sptr<SceneSession>& sceneSession);
    void DestroySpecificSession(const sptr<IRemoteObject>& remoteObject);
    bool GetExtensionWindowIds(const sptr<IRemoteObject>& token, int32_t& persistentId, int32_t& parentId);
    void DestroyExtensionSession(const sptr<IRemoteObject>& remoteExtSession);
    void EraseSceneSessionMapById(int32_t persistentId);
    WSError GetAbilityInfosFromBundleInfo(std::vector<AppExecFwk::BundleInfo>& bundleInfos,
        std::vector<SCBAbilityInfo>& scbAbilityInfos);
    void GetOrientationFromResourceManager(AppExecFwk::AbilityInfo& abilityInfo);
    void UpdatePrivateStateAndNotifyForAllScreens();

    WMError UpdatePropertyDragEnabled(const sptr<WindowSessionProperty>& property,
                                      const sptr<SceneSession>& sceneSession);
    WMError UpdatePropertyRaiseEnabled(const sptr<WindowSessionProperty>& property,
                                       const sptr<SceneSession>& sceneSession);
    void ClosePipWindowIfExist(WindowType type);
    void NotifySessionAINavigationBarChange(int32_t persistentId);
    void ReportWindowProfileInfos();
    void UpdateCameraWindowStatus(uint32_t accessTokenId, bool isShowing);
    void removeFailRecoveredSession();
    void GetAllSceneSessionForAccessibility(std::vector<sptr<SceneSession>>& sceneSessionList);
    void FillAccessibilityInfo(std::vector<sptr<SceneSession>>& sceneSessionList,
        std::vector<sptr<AccessibilityWindowInfo>>& accessibilityInfo);
    void FilterSceneSessionCovered(std::vector<sptr<SceneSession>>& sceneSessionList);
    void NotifyAllAccessibilityInfo();
    void SetSkipSelfWhenShowOnVirtualScreen(uint64_t surfaceNodeId, bool isSkip);
    void RegisterSecSurfaceInfoListener();

    /*
     * User Switch
     */
    bool IsPcSceneSessionLifecycle(const sptr<SceneSession>& sceneSession);
    bool IsNeedChangeLifeCycleOnUserSwitch(const sptr<SceneSession>& sceneSession, int32_t pid);
    WSError StartOrMinimizeUIAbilityBySCB(const sptr<SceneSession>& sceneSession, bool isUserActive);

    sptr<RootSceneSession> rootSceneSession_;
    std::weak_ptr<AbilityRuntime::Context> rootSceneContextWeak_;
    mutable std::shared_mutex sceneSessionMapMutex_;
    std::map<int32_t, sptr<SceneSession>> sceneSessionMap_;
    std::map<int32_t, sptr<SceneSession>> systemTopSceneSessionMap_;
    std::map<int32_t, sptr<SceneSession>> nonSystemFloatSceneSessionMap_;
    sptr<ScbSessionHandler> scbSessionHandler_;
    std::shared_ptr<SessionListenerController> listenerController_;
    std::map<sptr<IRemoteObject>, int32_t> remoteObjectMap_;
    std::map<sptr<IRemoteObject>, sptr<IRemoteObject>> remoteExtSessionMap_;
    std::map<sptr<IRemoteObject>, ExtensionWindowAbilityInfo> extSessionInfoMap_;
    std::set<int32_t> avoidAreaListenerSessionSet_;
    std::set<int32_t> touchOutsideListenerSessionSet_;
    std::set<int32_t> windowVisibilityListenerSessionSet_;
    ExtensionWindowFlags combinedExtWindowFlags_ { 0 };
    std::map<int32_t, ExtensionWindowFlags> extWindowFlagsMap_;
    std::set<int32_t> failRecoveredPersistentIdSet_;
    std::map<int32_t, std::map<AvoidAreaType, AvoidArea>> lastUpdatedAvoidArea_;

    NotifyCreateSystemSessionFunc createSystemSessionFunc_;
    NotifyCreateKeyboardSessionFunc createKeyboardSessionFunc_;
    std::map<int32_t, NotifyCreateSubSessionFunc> createSubSessionFuncMap_;
    std::map<int32_t, std::vector<sptr<SceneSession>>> recoverSubSessionCacheMap_;
    bool recoveringFinished_ = false;
    NotifyRecoverSceneSessionFunc recoverSceneSessionFunc_;
    ProcessStatusBarEnabledChangeFunc statusBarEnabledChangeFunc_;
    ProcessGestureNavigationEnabledChangeFunc gestureNavigationEnabledChangeFunc_;
    ProcessOutsideDownEventFunc outsideDownEventFunc_;
    DumpRootSceneElementInfoFunc dumpRootSceneFunc_;
    ProcessShiftFocusFunc shiftFocusFunc_;
    NotifySCBAfterUpdateFocusFunc notifySCBAfterFocusedFunc_;
    NotifySCBAfterUpdateFocusFunc notifySCBAfterUnfocusedFunc_;
    ProcessCallingSessionIdChangeFunc callingSessionIdChangeFunc_;
    ProcessStartUIAbilityErrorFunc startUIAbilityErrorFunc_;
    DumpUITreeFunc dumpUITreeFunc_;
    ProcessVirtualPixelRatioChangeFunc processVirtualPixelRatioChangeFunc_ = nullptr;
    AppWindowSceneConfig appWindowSceneConfig_;
    SystemSessionConfig systemConfig_;
    FocusChangeReason focusChangeReason_ = FocusChangeReason::DEFAULT;
    float snapshotScale_ = 0.5;
    int32_t focusedSessionId_ = INVALID_SESSION_ID;
    int32_t lastFocusedSessionId_ = INVALID_SESSION_ID;
    int32_t brightnessSessionId_ = INVALID_SESSION_ID;
    float displayBrightness_ = UNDEFINED_BRIGHTNESS;
    bool isScreenLocked_ {false};
    bool needBlockNotifyFocusStatusUntilForeground_ {false};
    bool needBlockNotifyUnfocusStatus_ {false};
    bool isPrepareTerminateEnable_ {false};
    bool openDebugTrace {false};
    int32_t currentUserId_;
    std::atomic<bool> enableInputEvent_ = true;
    std::vector<int32_t> alivePersistentIds_ = {};
    std::vector<VisibleWindowNumInfo> lastInfo_ = {};
    std::shared_mutex lastInfoMutex_;

    std::shared_ptr<TaskScheduler> taskScheduler_;
    sptr<AppExecFwk::IBundleMgr> bundleMgr_;
    sptr<AppAnrListener> appAnrListener_;
    sptr<AppExecFwk::LauncherService> launcherService_;
    std::shared_mutex startingWindowMapMutex_;
    const size_t MAX_CACHE_COUNT = 100;
    std::map<std::string, std::map<std::string, StartingWindowInfo>> startingWindowMap_;
    std::unordered_map<std::string, AppForceLandscapeConfig> appForceLandscapeMap_;
    std::shared_mutex appForceLandscapeMutex_;

    std::mutex privacyBundleMapMutex_;
    std::unordered_map<DisplayId, std::unordered_set<std::string>> privacyBundleMap_;

    bool isAINavigationBarVisible_ = false;
    std::shared_mutex currAINavigationBarAreaMapMutex_;
    std::map<uint64_t, WSRect> currAINavigationBarAreaMap_;
    WindowModeType lastWindowModeType_ { WindowModeType::WINDOW_MODE_OTHER };

    // displayRegionMap_ stores the screen display area for AccessibilityNotification,
    // the read and write operations must be performed in the same thread, current is in task thread.
    std::unordered_map<DisplayId, std::shared_ptr<SkRegion>> displayRegionMap_;
    std::shared_ptr<SkRegion> GetDisplayRegion(DisplayId displayId);
    void UpdateDisplayRegion(const sptr<DisplayInfo>& displayInfo);

    std::shared_ptr<AppExecFwk::EventRunner> eventLoop_;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_;
    bool isReportTaskStart_ = false;
    std::vector<std::pair<uint64_t, WindowVisibilityState> > lastVisibleData_;
    RSInterfaces& rsInterface_;
    void ClearUnrecoveredSessions(const std::vector<int32_t>& recoveredPersistentIds);
    SessionInfo RecoverSessionInfo(const sptr<WindowSessionProperty>& property);
    bool IsNeedRecover(const int32_t persistentId);
    void RegisterSessionStateChangeNotifyManagerFunc(sptr<SceneSession>& sceneSession);
    void RegisterSessionInfoChangeNotifyManagerFunc(sptr<SceneSession>& sceneSession);
    void OnSessionStateChange(int32_t persistentId, const SessionState& state);
    void ProcessFocusWhenForeground(sptr<SceneSession>& sceneSession);
    void ProcessSubSessionForeground(sptr<SceneSession>& sceneSession);
    void ProcessSubSessionBackground(sptr<SceneSession>& sceneSession);
    WSError ProcessDialogRequestFocusImmdediately(sptr<SceneSession>& sceneSession);
    WSError ProcessModalTopmostRequestFocusImmdediately(sptr<SceneSession>& sceneSession);
    sptr<SceneSession> FindSessionByToken(const sptr<IRemoteObject>& token);

    void CheckAndNotifyWaterMarkChangedResult();
    WSError NotifyWaterMarkFlagChangedResult(bool hasWaterMark);
    void ProcessPreload(const AppExecFwk::AbilityInfo& abilityInfo) const;
    std::atomic_bool shouldHideNonSecureFloatingWindows_ { false };
    std::atomic_bool specialExtWindowHasPrivacyMode_ { false };
    bool lastWaterMarkShowState_ { false };
    WindowChangedFunc WindowChangedFunc_;
    sptr<AgentDeathRecipient> windowDeath_ = new AgentDeathRecipient(
        [this](const sptr<IRemoteObject>& remoteObject) { this->DestroySpecificSession(remoteObject); });
    sptr<AgentDeathRecipient> extensionDeath_ = new AgentDeathRecipient(
        [this](const sptr<IRemoteObject>& remoteExtSession) { this->DestroyExtensionSession(remoteExtSession); });

    WSError ClearSession(sptr<SceneSession> sceneSession);
    bool IsSessionClearable(sptr<SceneSession> scnSession);
    void GetAllClearableSessions(std::vector<sptr<SceneSession>>& sessionVector);
    int GetRemoteSessionSnapshotInfo(const std::string& deviceId, int32_t sessionId,
                                     AAFwk::MissionSnapshot& sessionSnapshot);
    sptr<AAFwk::IAbilityManagerCollaborator> GetCollaboratorByType(int32_t collaboratorType);

    const int32_t BROKER_UID = 5557;
    const int32_t BROKER_RESERVE_UID = 5005;
    std::shared_mutex collaboratorMapLock_;
    std::unordered_map<int32_t, sptr<AAFwk::IAbilityManagerCollaborator>> collaboratorMap_;
    std::atomic<int64_t> containerStartAbilityTime { 0 };
    std::vector<uint64_t> skipSurfaceNodeIds_;

    std::atomic_bool processingFlushUIParams_ { false };

    BrokerStates NotifyStartAbility(
        int32_t collaboratorType, const SessionInfo& sessionInfo, int32_t persistentId = 0);
    void NotifySessionCreate(const sptr<SceneSession> sceneSession, const SessionInfo& sessionInfo);
    void NotifyLoadAbility(int32_t collaboratorType, sptr<AAFwk::SessionInfo> abilitySessionInfo,
        std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo);
    void NotifyUpdateSessionInfo(const sptr<SceneSession> sceneSession);
    void NotifyClearSession(int32_t collaboratorType, int32_t persistentId);
    void NotifyMoveSessionToForeground(int32_t collaboratorType, int32_t persistentId);
    bool PreHandleCollaborator(sptr<SceneSession>& sceneSession, int32_t persistentId = 0);
    void NotifyCollaboratorAfterStart(sptr<SceneSession>& scnSession, sptr<AAFwk::SessionInfo>& scnSessionInfo);
    void UpdateCollaboratorSessionWant(sptr<SceneSession>& session, int32_t persistentId = 0);
    bool CheckSystemWindowPermission(const sptr<WindowSessionProperty>& property);
    bool CheckModalSubWindowPermission(const sptr<WindowSessionProperty>& property);
    bool CheckPiPPriority(const PiPTemplateInfo& pipTemplateInfo);
    bool isEnablePiPCreate(const sptr<WindowSessionProperty>& property);
    void DestroySubSession(const sptr<SceneSession>& sceneSession);
    void DestroyToastSession(const sptr<SceneSession>& sceneSession);
    void NotifySessionForeground(const sptr<SceneSession>& session, uint32_t reason, bool withAnimation);
    void NotifySessionBackground(const sptr<SceneSession>& session, uint32_t reason, bool withAnimation,
                                bool isFromInnerkits);
    void NotifyCreateSubSession(int32_t persistentId, sptr<SceneSession> session);
    void NotifyCreateToastSession(int32_t persistentId, sptr<SceneSession> session);
    void CacheSubSessionForRecovering(sptr<SceneSession> sceneSession, const sptr<WindowSessionProperty>& property);
    void RecoverCachedSubSession(int32_t persistentId);
    void NotifySessionUnfocusedToClient(int32_t persistentId);
    void NotifyCreateSpecificSession(sptr<SceneSession> session,
        sptr<WindowSessionProperty> property, const WindowType& type);
    void OnSCBSystemSessionBufferAvailable(const WindowType type);
    sptr<SceneSession> CreateSceneSession(const SessionInfo& sessionInfo, sptr<WindowSessionProperty> property);
    void CreateKeyboardPanelSession(sptr<SceneSession> keyboardSession);
    bool GetPreWindowDrawingState(uint64_t windowId, int32_t& pid, bool currentDrawingContentState);
    bool GetProcessDrawingState(uint64_t windowId, int32_t pid, bool currentDrawingContentState);
    WSError DestroyAndDisconnectSpecificSessionInner(const int32_t persistentId);
    WSError GetAppMainSceneSession(sptr<SceneSession>& sceneSession, int32_t persistentId);
    void CalculateCombinedExtWindowFlags();
    void UpdateSpecialExtWindowFlags(int32_t persistentId, ExtensionWindowFlags flags, ExtensionWindowFlags actions);
    void HideNonSecureFloatingWindows();
    void HideNonSecureSubWindows(const sptr<SceneSession>& sceneSession);
    WSError HandleSecureSessionShouldHide(const sptr<SceneSession>& sceneSession);
    void HandleSpecialExtWindowFlagsChange(int32_t persistentId, ExtensionWindowFlags extWindowFlags,
        ExtensionWindowFlags extWindowActions);
    void HandleCastScreenDisConnection(const sptr<SceneSession> sceneSession);
    void ProcessWindowModeType();
    WindowModeType CheckWindowModeType();
    void NotifyRSSWindowModeTypeUpdate();
    void CacVisibleWindowNum();
    bool IsVectorSame(const std::vector<VisibleWindowNumInfo>& lastInfo,
        const std::vector<VisibleWindowNumInfo>& currentInfo);
    bool IsKeyboardForeground();
    WindowStatus GetWindowStatus(WindowMode mode, SessionState sessionState,
        const sptr<WindowSessionProperty>& property);
    void DeleteStateDetectTask();
    bool JudgeNeedNotifyPrivacyInfo(DisplayId displayId, const std::unordered_set<std::string>& privacyBundles);
    WSError CheckSessionPropertyOnRecovery(const sptr<WindowSessionProperty>& property, bool isSpecificSession);
    
    /*
     * Fold Screen Status Change Report
     */
    WMError MakeScreenFoldData(const std::vector<std::string>& screenFoldInfo, ScreenFoldData& screenFoldData);
    WMError CheckAndReportScreenFoldStatus(ScreenFoldData& data);
    WMError ReportScreenFoldStatus(const ScreenFoldData& data);

    RunnableFuture<std::vector<std::string>> dumpInfoFuture_;

    std::condition_variable nextFlushCompletedCV_;
    std::mutex nextFlushCompletedMutex_;
    RootSceneProcessBackEventFunc rootSceneProcessBackEventFunc_ = nullptr;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_MANAGER_H
