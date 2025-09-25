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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H

#include <animation/rs_animation_timing_curve.h>
#include <animation/rs_animation_timing_protocol.h>
#include <animation/rs_symbol_animation.h>
#include <pipeline/rs_node_map.h>
#include <ui/rs_canvas_node.h>
#include <chrono>

#include "display_manager.h"
#include "session/host/include/session.h"
#include "session/host/include/move_drag_controller.h"
#include "session/host/include/pc_fold_screen_controller.h"
#include "vsync_station.h"
#include "wm_common.h"

namespace OHOS::PowerMgr {
class RunningLock;
}

namespace OHOS::Rosen {
namespace PARAM_KEY {
const std::string PARAM_MISSION_AFFINITY_KEY = "ohos.anco.param.missionAffinity";
const std::string PARAM_DMS_CONTINUE_SESSION_ID_KEY = "ohos.dms.continueSessionId";
const std::string PARAM_DMS_PERSISTENT_ID_KEY = "ohos.dms.persistentId";
}

class SceneSession;
class ScreenSession;

using NotifySessionLockStateChangeCallback = std::function<void(bool isLockedState)>;
using SpecificSessionCreateCallback =
  std::function<sptr<SceneSession>(const SessionInfo& info, sptr<WindowSessionProperty> property)>;
using SpecificSessionDestroyCallback = std::function<WSError(const int32_t& persistentId)>;
using CameraFloatSessionChangeCallback = std::function<void(uint32_t accessTokenId, bool isShowing)>;
using GetSceneSessionVectorByTypeAndDisplayIdCallback = std::function<std::vector<sptr<SceneSession>>(
    WindowType type, DisplayId displayId)>;
using GetSceneSessionVectorByTypeCallback = std::function<std::vector<sptr<SceneSession>>(WindowType type)>;
using UpdateAvoidAreaCallback = std::function<void(int32_t persistentId)>;
using UpdateOccupiedAreaIfNeedCallback = std::function<void(int32_t persistentId)>;
using NotifyWindowInfoUpdateCallback = std::function<void(int32_t persistentId, WindowUpdateType type)>;
using NotifyWindowPidChangeCallback = std::function<void(int32_t windowId, bool startMoving)>;
using NotifySessionTouchOutsideCallback = std::function<void(int32_t persistentId, DisplayId displayId)>;
using GetAINavigationBarArea = std::function<WSRect(uint64_t displayId)>;
using RecoveryCallback = std::function<void(int32_t persistentId, Rect rect)>;
using NotifyBindDialogSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifySessionPiPControlStatusChangeFunc = std::function<void(WsPiPControlType controlType,
    WsPiPControlStatus status)>;
using NotifyAutoStartPiPStatusChangeFunc = std::function<void(bool isAutoStart, uint32_t priority,
    uint32_t width, uint32_t height)>;
using NotifyUpdatePiPTemplateInfoFunc = std::function<void(PiPTemplateInfo& pipTemplateInfo)>;
using NotifySessionEventFunc = std::function<void(int32_t eventId, SessionEventParam param)>;
using NotifySessionTopmostChangeFunc = std::function<void(const bool topmost)>;
using NotifySubSessionZLevelChangeFunc = std::function<void(const int32_t zLevel)>;
using NotifySubModalTypeChangeFunc = std::function<void(SubWindowModalType subWindowModalType)>;
using NotifyMainModalTypeChangeFunc = std::function<void(bool isModal)>;
using NotifyRaiseToTopFunc = std::function<void()>;
using SetWindowPatternOpacityFunc = std::function<void(float opacity)>;
using NotifyIsCustomAnimationPlayingCallback = std::function<void(bool isFinish)>;
using NotifyWindowAnimationFlagChangeFunc = std::function<void(const bool flag)>;
using NotifySystemBarPropertyChangeFunc = std::function<void(
    const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)>;
using NotifyNeedAvoidFunc = std::function<void(bool status)>;
using NotifyShowWhenLockedFunc = std::function<void(bool showWhenLocked)>;
using NotifyReqOrientationChangeFunc = std::function<void(uint32_t orientation, bool needAnimation)>;
using NotifyRaiseAboveTargetFunc = std::function<void(int32_t subWindowId)>;
using NotifyForceHideChangeFunc = std::function<void(bool hide)>;
using NotifyTouchOutsideFunc = std::function<void()>;
using ClearCallbackMapFunc = std::function<void(bool needRemove)>;
using NotifyPrepareClosePiPSessionFunc = std::function<void()>;
using OnOutsideDownEvent = std::function<void(int32_t x, int32_t y)>;
using HandleSecureSessionShouldHideCallback = std::function<WSError(const sptr<SceneSession>& sceneSession)>;
using ClearDisplayStatusBarTemporarilyFlags = std::function<void()>;
using CameraSessionChangeCallback = std::function<void(uint32_t accessTokenId, bool isShowing)>;
using NotifyLandscapeMultiWindowSessionFunc = std::function<void(bool isLandscapeMultiWindow)>;
using NotifyKeyboardLayoutAdjustFunc = std::function<void(const KeyboardLayoutParams& params)>;
using NotifyKeyboarEffectOptionChangeFunc = std::function<void(const KeyboardEffectOption& effectOption)>;
using SessionChangeByActionNotifyManagerFunc = std::function<void(const sptr<SceneSession>& sceneSession,
    const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action)>;
using NotifyLayoutFullScreenChangeFunc = std::function<void(bool isLayoutFullScreen)>;
using NotifyDefaultDensityEnabledFunc = std::function<void(bool isDefaultDensityEnabled)>;
using NotifyTitleAndDockHoverShowChangeFunc = std::function<void(bool isTitleHoverShown,
    bool isDockHoverShown)>;
using NotifyRestoreMainWindowFunc = std::function<void()>;
using SetSkipSelfWhenShowOnVirtualScreenCallback = std::function<void(uint64_t surfaceNodeId, bool isSkip)>;
using SetSkipEventOnCastPlusCallback = std::function<void(int32_t persistentId, bool isSkip)>;
using NotifyForceSplitFunc = std::function<AppForceLandscapeConfig(const std::string& bundleName)>;
using GetHookWindowInfoFunc = std::function<HookWindowInfo(const std::string& bundleName)>;
using UpdatePrivateStateAndNotifyFunc = std::function<void(int32_t persistentId)>;
using UpdateScreenshotAppEventRegisteredFunc = std::function<void(int32_t persistentId, bool isRegister)>;
using PiPStateChangeCallback = std::function<void(const std::string& bundleName, bool isForeground)>;
using NotifyMainWindowTopmostChangeFunc = std::function<void(bool isTopmost)>;
using GetConstrainedModalExtWindowInfoFunc =
    std::function<std::optional<ExtensionWindowEventInfo>(const sptr<SceneSession>& sceneSession)>;
using NotifyPrivacyModeChangeFunc = std::function<void(bool isPrivacyMode)>;
using UpdateGestureBackEnabledCallback = std::function<void(int32_t persistentId)>;
using NotifyVisibleChangeFunc = std::function<void(int32_t persistentId)>;
using IsLastFrameLayoutFinishedFunc = std::function<WSError(bool& isLayoutFinished)>;
using IsAINavigationBarAvoidAreaValidFunc = std::function<bool(DisplayId displayId,
    const AvoidArea& avoidArea, int32_t sessionBottom)>;
using GetStatusBarDefaultVisibilityByDisplayIdFunc = std::function<bool(DisplayId displayId)>;
using NotifySetWindowRectAutoSaveFunc = std::function<void(bool enabled, bool isSaveBySpecifiedFlag)>;
using UpdateAppUseControlFunc = std::function<void(ControlAppType type, bool isNeedControl, bool isControlRecentOnly)>;
using NotifyAvoidAreaChangeCallback = std::function<void(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)>;
using NotifySetSupportedWindowModesFunc = std::function<void(
    std::vector<AppExecFwk::SupportWindowMode>&& supportedWindowModes)>;
using GetStatusBarAvoidHeightFunc = std::function<void(DisplayId displayId, WSRect& barArea)>;
using GetStatusBarConstantlyShowFunc = std::function<void(DisplayId displayId, bool& isVisible)>;
using NotifySetWindowCornerRadiusFunc = std::function<void(float cornerRadius)>;
using GetKeyboardOccupiedAreaWithRotationCallback =
    std::function<void(int32_t persistentId, Rotation rotation, std::vector<std::pair<bool, WSRect>>& avoidAreas)>;
using GetNextAvoidAreaRectInfoFunc = std::function<WSError(DisplayId displayId, AvoidAreaType type,
    std::pair<WSRect, WSRect>& nextSystemBarAvoidAreaRectInfo)>;
using NotifyFollowParentRectFunc = std::function<void(bool isFollow)>;
using NotifyWindowAnchorInfoChangeFunc = std::function<void(const WindowAnchorInfo& windowAnchorInfo)>;
using GetSceneSessionByIdCallback = std::function<sptr<SceneSession>(int32_t sessionId)>;
using NotifySetParentSessionFunc = std::function<void(int32_t oldParentWindowId, int32_t newParentWindowId)>;
using NotifyUpdateFlagFunc = std::function<void(const std::string& flag)>;
using GetStartWindowTypeFunc = std::function<void(const SessionInfo& info, std::string& startWindowType)>;
using NotifyRotationChangeFunc = std::function<void(int32_t persistentId, bool isRegister)>;
using NotifyHookSceneSessionActivationFunc = std::function<void(const sptr<SceneSession>& session, bool isNewWant)>;
using NotifySceneSessionDestructFunc = std::function<void(int32_t persistentId)>;
using NotifyFollowScreenChangeFunc = std::function<void(bool isFollowScreenChange)>;
using NotifyUseImplicitAnimationChangeFunc = std::function<void(bool useImplicit)>;
using NotifySetWindowShadowsFunc = std::function<void(const ShadowsInfo& shadowsInfo)>;
using NotifyWindowShadowEnableChangeFunc = std::function<void(bool windowShadowEnabled)>;
using NotifyWindowSystemBarPropertyChangeFunc = std::function<void(WindowType type,
    const SystemBarProperty& systemBarProperty)>;
using NotifySetSubWindowSourceFunc = std::function<void(SubWindowSource source)>;
using NotifyAnimateToFunc = std::function<void(const WindowAnimationProperty& animationProperty,
    const WindowAnimationOption& animationOption)>;
using GetAllAppUseControlMapFunc =
    std::function<std::unordered_map<std::string, std::unordered_map<ControlAppType, ControlInfo>>&()>;
using GetFbPanelWindowIdFunc =  std::function<WMError(uint32_t& windowId)>;
using FindScenePanelRsNodeByZOrderFunc = std::function<std::shared_ptr<Rosen::RSNode>(DisplayId displayId,
    uint32_t targetZOrder)>;
using ForceSplitFullScreenChangeCallback = std::function<void(uint32_t uid, bool isFullScreen)>;

struct UIExtensionTokenInfo {
    bool canShowOnLockScreen { false };
    uint32_t callingTokenId { 0 };
    sptr<IRemoteObject> abilityToken;
};

class SceneSession : public Session {
public:
    friend class HidumpController;
    friend class PcFoldScreenController;
    // callback for notify SceneSessionManager
    struct SpecificSessionCallback : public RefBase {
        SpecificSessionCreateCallback onCreate_;
        SpecificSessionDestroyCallback onDestroy_;
        ClearDisplayStatusBarTemporarilyFlags onClearDisplayStatusBarTemporarilyFlags_;
        CameraFloatSessionChangeCallback onCameraFloatSessionChange_;
        GetSceneSessionVectorByTypeAndDisplayIdCallback onGetSceneSessionVectorByTypeAndDisplayId_;
        GetSceneSessionVectorByTypeCallback onGetSceneSessionVectorByType_;
        UpdateAvoidAreaCallback onUpdateAvoidArea_;
        GetStatusBarDefaultVisibilityByDisplayIdFunc onGetStatusBarDefaultVisibilityByDisplayId_;
        UpdateOccupiedAreaIfNeedCallback onUpdateOccupiedAreaIfNeed_;
        NotifyWindowInfoUpdateCallback onWindowInfoUpdate_;
        NotifyWindowPidChangeCallback onWindowInputPidChangeCallback_;
        NotifySessionTouchOutsideCallback onSessionTouchOutside_;
        GetAINavigationBarArea onGetAINavigationBarArea_;
        GetNextAvoidAreaRectInfoFunc onGetNextAvoidAreaRectInfo_;
        OnOutsideDownEvent onOutsideDownEvent_;
        HandleSecureSessionShouldHideCallback onHandleSecureSessionShouldHide_;
        CameraSessionChangeCallback onCameraSessionChange_;
        SetSkipSelfWhenShowOnVirtualScreenCallback onSetSkipSelfWhenShowOnVirtualScreen_;
        SetSkipEventOnCastPlusCallback onSetSkipEventOnCastPlus_;
        PiPStateChangeCallback onPiPStateChange_;
        UpdateGestureBackEnabledCallback onUpdateGestureBackEnabled_;
        NotifyAvoidAreaChangeCallback onNotifyAvoidAreaChange_;
        NotifyWindowSystemBarPropertyChangeFunc onNotifyWindowSystemBarPropertyChangeFunc_;
        GetKeyboardOccupiedAreaWithRotationCallback onKeyboardRotationChange_;
        GetSceneSessionByIdCallback onGetSceneSessionByIdCallback_;
        NotifyFollowScreenChangeFunc onUpdateFollowScreenChange_;
    };

    // func for change window scene pattern property
    struct SetWindowScenePatternFunc : public RefBase {
        SetWindowPatternOpacityFunc setOpacityFunc_;
    };

    SceneSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback);
    virtual ~SceneSession();

    WSError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr,
        const std::string& identityToken = "") override;
    WSError ConnectInner(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr,
        int32_t pid = -1, int32_t uid = -1, const std::string& identityToken = "") override;
    WSError Foreground(sptr<WindowSessionProperty> property, bool isFromClient = false,
        const std::string& identityToken = "") override;
    WSError Background(bool isFromClient = false, const std::string& identityToken = "") override;
    WSError BackgroundTask(const bool isSaveSnapshot = true, BackgroundReason reason = BackgroundReason::DEFAULT);
    WSError Disconnect(bool isFromClient = false, const std::string& identityToken = "") override;
    WSError DisconnectTask(bool isFromClient = false, bool isSaveSnapshot = true);
    void SetClientIdentityToken(const std::string& clientIdentityToken);
    virtual void BindKeyboardPanelSession(sptr<SceneSession> panelSession) {};
    virtual sptr<SceneSession> GetKeyboardPanelSession() const { return nullptr; };
    virtual void BindKeyboardSession(sptr<SceneSession> session) {};
    virtual sptr<SceneSession> GetKeyboardSession() const { return nullptr; };
    virtual SessionGravity GetKeyboardGravity() const { return SessionGravity::SESSION_GRAVITY_DEFAULT; };
    virtual uint32_t GetCallingSessionId() { return INVALID_SESSION_ID; };
    bool GetScreenWidthAndHeightFromClient(const sptr<WindowSessionProperty>& sessionProperty,
        uint32_t& screenWidth, uint32_t& screenHeight);
    sptr<SceneSession> GetSceneSessionById(int32_t sessionId) const;

    WSError UpdateActiveStatus(bool isActive) override;
    WSError OnSessionEvent(SessionEvent event) override;
    WSError SyncSessionEvent(SessionEvent event) override;
    WSError OnLayoutFullScreenChange(bool isLayoutFullScreen) override;
    WSError RaiseToAppTop() override;

    /*
     * Window Recover
     */
    virtual WSError Reconnect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, sptr<WindowSessionProperty> property = nullptr,
        sptr<IRemoteObject> token = nullptr, int32_t pid = -1, int32_t uid = -1);
    WSError ReconnectInner(sptr<WindowSessionProperty> property) REQUIRES(SCENE_GUARD);
    bool IsRecovered() const { return isRecovered_; }
    void SetRecovered(bool isRecovered) { isRecovered_ = isRecovered; }

    /*
     * Window Layout
     */
    WSError UpdateSizeChangeReason(SizeChangeReason reason) override;
    virtual void NotifySessionRectChange(const WSRect& rect,
        SizeChangeReason reason = SizeChangeReason::UNDEFINED, DisplayId displayId = DISPLAY_ID_INVALID,
        const RectAnimationConfig& rectAnimationConfig = {});
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason,
        const std::string& updateReason, const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) override;
    WSError UpdateSessionRect(const WSRect& rect, SizeChangeReason reason, bool isGlobal = false,
        bool isFromMoveToGlobal = false, const MoveConfiguration& moveConfiguration = {},
        const RectAnimationConfig& rectAnimationConfig = {}) override;
    WSError UpdateClientRect(const WSRect& rect) override;
    WSError UpdateGlobalDisplayRectFromClient(const WSRect& rect, SizeChangeReason reason) override;
    void NotifySingleHandTransformChange(const SingleHandTransform& singleHandTransform);
    WSRect GetSessionGlobalRectWithSingleHandScale();
    void UpdateSessionState(SessionState state) override;
    WSError NotifyClientToUpdateRect(const std::string& updateReason,
        std::shared_ptr<RSTransaction> rsTransaction) override;
    void SetWinRectWhenUpdateRect(const WSRect& rect);
    void RegisterNotifySurfaceBoundsChangeFunc(int32_t sessionId, NotifySurfaceBoundsChangeFunc&& func) override;
    void UnregisterNotifySurfaceBoundsChangeFunc(int32_t sessionId) override;
    void SetRequestRectWhenFollowParent(const WSRect& rect) { requestRectWhenFollowParent_ = rect; }
    WSRect GetRequestRectWhenFollowParent() const { return requestRectWhenFollowParent_; }
    void HandleCrossMoveTo(WSRect& globalRect);
    virtual void HandleCrossMoveToSurfaceNode(WSRect& globalRect) {}
    virtual bool IsNeedCrossDisplayRendering() const { return false; }
    virtual void HandleCrossSurfaceNodeByWindowAnchor(SizeChangeReason reason, DisplayId displayId) {}
    virtual void SetSurfaceBounds(const WSRect& rect, bool isGlobal, bool needFlush = true);

    virtual void OpenKeyboardSyncTransaction() {}
    virtual void CloseKeyboardSyncTransaction(const WSRect& keyboardPanelRect,
        bool isKeyboardShow, const WindowAnimationInfo& animationInfo) {}
    WSError ChangeSessionVisibilityWithStatusBar(const sptr<AAFwk::SessionInfo> info, bool visible) override;
    WSError PendingSessionActivation(const sptr<AAFwk::SessionInfo> info) override;
    WSError BatchPendingSessionsActivation(const std::vector<sptr<AAFwk::SessionInfo>>& abilitySessionInfos,
        const std::vector<PendingSessionActivationConfig>& configs = {}) override;
    WSError DoBatchPendingSessionsActivation(
        const std::vector<sptr<AAFwk::SessionInfo>>& abilitySessionInfos, sptr<SceneSession>& session,
        bool isFoundationCall, const std::vector<PendingSessionActivationConfig>& configs);
    bool DisallowActivationFromPendingBackground(bool isPcOrPadEnableActivation, bool isFoundationCall,
        bool canStartAbilityFromBackground, bool isFromAncoAndToAnco);
    WSError TerminateSession(const sptr<AAFwk::SessionInfo> info) override;
    WSError NotifySessionException(
        const sptr<AAFwk::SessionInfo> info, const ExceptionInfo& exceptionInfo) override;
    WSError NotifySessionExceptionInner(
        const sptr<AAFwk::SessionInfo> info, const ExceptionInfo& exceptionInfo,
        bool isFromClient = false, bool startFail = false);

    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        bool needNotifyClient = true, bool isExecuteDelayRaise = false) override;
    WSError TransferPointerEventInner(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        bool needNotifyClient = true, bool isExecuteDelayRaise = false);
    WSError RequestSessionBack(bool needMoveToBackground) override;
    WSError SetAspectRatio(float ratio) override;
    WSError SetContentAspectRatio(float ratio, bool isPersistent, bool needUpdateRect) override;
    WSError SetGlobalMaximizeMode(MaximizeMode mode) override;
    WSError GetGlobalMaximizeMode(MaximizeMode& mode) override;
    WSError UpdateWindowSceneAfterCustomAnimation(bool isAdd) override;
    WSError UpdateWindowAnimationFlag(bool needDefaultAnimationFlag) override;
    void SetZOrder(uint32_t zOrder) override;
    std::vector<Rect> GetTouchHotAreas() const override;
    void NotifyUILostFocus() override;
    void SetScale(float scaleX, float scaleY, float pivotX, float pivotY) override;
    void SetFloatingScale(float floatingScale) override;
    WSError RaiseAboveTarget(int32_t subWindowId) override;
    WSError RaiseMainWindowAboveTarget(int32_t targetId) override;
    std::shared_ptr<Rosen::RSNode> GetWindowDragMoveMountedNode(DisplayId displayId, uint32_t targetZOrder);

    /*
     * PiP Window
     */
    PiPTemplateInfo GetPiPTemplateInfo() const;
    void SetPiPTemplateInfo(const PiPTemplateInfo& pipTemplateInfo);
    WSError UpdatePiPRect(const Rect& rect, SizeChangeReason reason) override;
    WSError UpdatePiPControlStatus(WsPiPControlType controlType, WsPiPControlStatus status) override;
    WSError SetAutoStartPiP(bool isAutoStart, uint32_t priority, uint32_t width, uint32_t height) override;
    void SetUpdatePiPTemplateInfoCallback(NotifyUpdatePiPTemplateInfoFunc&& func);
    WSError UpdatePiPTemplateInfo(PiPTemplateInfo& pipTemplateInfo) override;
    void NotifyPiPWindowPrepareClose() override;
    void SetSessionPiPControlStatusChangeCallback(const NotifySessionPiPControlStatusChangeFunc& func);
    void SetAutoStartPiPStatusChangeCallback(const NotifyAutoStartPiPStatusChangeFunc& func);
    WSError SetPipActionEvent(const std::string& action, int32_t status);
    WSError SetPiPControlEvent(WsPiPControlType controlType, WsPiPControlStatus status);
    WSError NotifyPipWindowSizeChange(double width, double height, double scale);
    void RegisterProcessPrepareClosePiPCallback(NotifyPrepareClosePiPSessionFunc&& callback);

    void RequestHideKeyboard(bool isAppColdStart = false);
    WSError ProcessPointDownSession(int32_t posX, int32_t posY) override;
    WSError SendPointEventForMoveDrag(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        bool isExecuteDelayRaise = false) override;
    WSError SendPointerEventForHover(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void NotifyOutsideDownEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    WSError NotifyFrameLayoutFinishFromApp(bool notifyListener, const WSRect& rect) override;
    void SetForegroundInteractiveStatus(bool interactive) override;
    WSError SetLandscapeMultiWindow(bool isLandscapeMultiWindow) override;

    /*
     * Floating Ball Window
     */
    WMError UpdateFloatingBall(const FloatingBallTemplateInfo& fbTemplateInfo) override { return WMError::WM_OK; };
    WSError StopFloatingBall() override { return WSError::WS_OK; };
    WMError GetFloatingBallWindowId(uint32_t& windowId) override { return WMError::WM_OK; };
    WMError RestoreFbMainWindow(const std::shared_ptr<AAFwk::Want>& want) override { return WMError::WM_OK; };
    virtual WSError SendFbActionEvent(const std::string& action) { return WSError::WS_OK; };
    virtual FloatingBallTemplateInfo GetFbTemplateInfo() const { return fbTemplateInfo_; };
    virtual void SetFbTemplateInfo(const FloatingBallTemplateInfo& fbTemplateInfo) {};
    virtual uint32_t GetFbWindowId() const { return 0; };
    virtual void SetFloatingBallUpdateCallback(NotifyUpdateFloatingBallFunc&& func) {};
    virtual void SetFloatingBallStopCallback(NotifyStopFloatingBallFunc&& func) {};
    virtual void SetFloatingBallRestoreMainWindowCallback(NotifyRestoreFloatingBallMainWindowFunc&& func) {};
    virtual void RegisterGetFbPanelWindowIdFunc(GetFbPanelWindowIdFunc&& func) {};

    /*
     * Window Layout
     */
    WMError ActivateDragBySystem(bool activateDrag);
    WMError SetSystemWindowEnableDrag(bool enableDrag) override;
    WMError SetWindowEnableDragBySystem(bool enableDrag);
    WSError OnDefaultDensityEnabled(bool isDefaultDensityEnabled) override;
    WMError UpdateWindowModeForUITest(int32_t updateMode);
    void RegisterDefaultDensityEnabledCallback(NotifyDefaultDensityEnabledFunc&& callback);
    void SetSessionDisplayIdChangeCallback(NotifySessionDisplayIdChangeFunc&& func);
    bool IsMovable() const;
    bool IsDraggable() const;

    WSError SetKeepScreenOn(bool keepScreenOn);
    WSError SetViewKeepScreenOn(bool keepScreenOn);
    WSError SetWindowShadowEnabled(bool isEnabled);
    void SetParentPersistentId(int32_t parentId);
    WSError SetTurnScreenOn(bool turnScreenOn);
    void SetPrivacyMode(bool isPrivacy);
    WMError SetSnapshotSkip(bool isSkip);
    void SetSystemSceneOcclusionAlpha(double alpha);
    void ResetOcclusionAlpha();
    void SetSystemSceneForceUIFirst(bool forceUIFirst);
    void SetUIFirstSwitch(RSUIFirstSwitch uiFirstSwitch);
    void MarkSystemSceneUIFirst(bool isForced, bool isUIFirstEnabled);
    void SetRequestedOrientation(Orientation orientation, bool needAnimation = true);
    WSError SetDefaultRequestedOrientation(Orientation orientation);
    void SetWindowAnimationFlag(bool needDefaultAnimationFlag);
    void SetCollaboratorType(int32_t collaboratorType);
    void SetLastSafeRect(WSRect rect);
    void SetMovable(bool isMovable);
    void SetOriPosYBeforeRaisedByKeyboard(int32_t posY);
    void SetColorSpace(ColorSpace colorSpace);

    /*
     * Window Hierarchy
     */
    virtual WSError SetTopmost(bool topmost) { return WSError::WS_ERROR_INVALID_CALLING; }
    virtual bool IsTopmost() const { return false; }
    virtual WSError SetMainWindowTopmost(bool isTopmost) { return WSError::WS_ERROR_INVALID_CALLING; }
    virtual bool IsMainWindowTopmost() const { return false; }
    virtual WSError SetSubWindowZLevel(int32_t zLevel) { return WSError::WS_ERROR_INVALID_CALLING; }
    virtual int32_t GetSubWindowZLevel() const { return 0; }
    void SetMainWindowTopmostChangeCallback(NotifyMainWindowTopmostChangeFunc&& func);

    /*
     * Compatible Mode
     */
    bool IsInCompatScaleStatus() const;
    bool IsInCompatScaleMode();
    virtual void RegisterForceSplitFullScreenChangeCallback(ForceSplitFullScreenChangeCallback&& callback) {}
    virtual bool IsFullScreenInForceSplit() { return false; }

    /*
     * PC Window
     */
    void SetTitleAndDockHoverShowChangeCallback(NotifyTitleAndDockHoverShowChangeFunc&& func);
    void SetRestoreMainWindowCallback(NotifyRestoreMainWindowFunc&& func);
    void SetWindowRectAutoSaveCallback(NotifySetWindowRectAutoSaveFunc&& func);
    virtual bool IsModal() const { return false; }
    virtual bool IsApplicationModal() const { return false; }
    bool IsDialogWindow() const;
    WSError NotifySubModalTypeChange(SubWindowModalType subWindowModalType) override;
    void RegisterSubModalTypeChangeCallback(NotifySubModalTypeChangeFunc&& func);
    void RegisterMainModalTypeChangeCallback(NotifyMainModalTypeChangeFunc&& func);
    void RegisterSupportWindowModesCallback(NotifySetSupportedWindowModesFunc&& func);
    void CloneWindow(NodeId surfaceNodeId, bool needOffScreen);
    void AddSidebarBlur();
    void AddRSNodeModifier(bool isDark, const std::shared_ptr<RSBaseNode>& rsNode);
    void SetSidebarBlur(bool isDefaultSidebarBlur, bool isNeedAnimation);
    void ModifyRSAnimatableProperty(bool isDefaultSidebarBlur, bool isDark, bool isNeedAnimation);
    void SaveLastDensity();
    virtual bool IsFollowParentMultiScreenPolicy() const { return false; }
    void NotifyUpdateFlagCallback(NotifyUpdateFlagFunc&& func);
    void SetSidebarBlurMaximize(bool isMaximize);
    void ModifyRSAnimatablePropertyMaximize(bool isMaximize, bool isDark);
    void RegisterUseImplicitAnimationChangeCallback(NotifyUseImplicitAnimationChangeFunc&& func);
    WSError UseImplicitAnimation(bool useImplicit) override;

    /*
     * PC Window Layout
     */
    void SetIsLayoutFullScreen(bool isLayoutFullScreen);
    bool IsLayoutFullScreen() const;
    WSError StartMovingWithCoordinate(int32_t offsetX, int32_t offsetY,
        int32_t pointerPosX, int32_t pointerPosY, DisplayId displayId) override;

    /*
     * Sub Window
     */
    virtual void SetParentSessionCallback(NotifySetParentSessionFunc&& func) {}
    virtual WMError NotifySetParentSession(int32_t oldParentWindowId,
        int32_t newParentWindowId) { return WMError::WM_ERROR_INVALID_WINDOW; }
    void UpdateSubWindowLevel(uint32_t subWindowLevel);
    uint32_t GetMaxSubWindowLevel() const;
    void SetSubWindowOutlineEnabled(bool subWindowOutlineEnabled);
    bool IsSubWindowOutlineEnabled() const;
    WSError CloseSpecificScene();
    void SetSubWindowSourceFunc(NotifySetSubWindowSourceFunc&& func);
    WSError SetSubWindowSource(SubWindowSource source) override;

    /*
     * Window Immersive
     */
    WSError OnNeedAvoid(bool status) override;
    AvoidArea GetAvoidAreaByType(AvoidAreaType type, const WSRect& rect = WSRect::EMPTY_RECT,
        int32_t apiVersion = API_VERSION_INVALID) override;
    WSError GetAllAvoidAreas(std::map<AvoidAreaType, AvoidArea>& avoidAreas) override;
    WSError GetTargetOrientationConfigInfo(Orientation targetOrientation,
        const std::map<Rosen::WindowType, Rosen::SystemBarProperty>& properties) override;
    void SetSessionGetTargetOrientationConfigInfoCallback(
        const NotifySessionGetTargetOrientationConfigInfoFunc& func);
    WSError SetSystemBarProperty(WindowType type, SystemBarProperty systemBarProperty);
    void SetIsStatusBarVisible(bool isVisible);
    WSError SetIsStatusBarVisibleInner(bool isVisible);
    WSError HandleLayoutAvoidAreaUpdate(AvoidAreaType avoidArea = AvoidAreaType::TYPE_END);
    WSError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) override;
    void UpdateRotationAvoidArea();
    bool CheckGetAvoidAreaAvailable(AvoidAreaType type) override;
    bool CheckGetSubWindowAvoidAreaAvailable(WindowMode winMode, AvoidAreaType type);
    bool CheckGetMainWindowAvoidAreaAvailable(WindowMode winMode, AvoidAreaType type);
    bool CheckGetSystemWindowAvoidAreaAvailable();
    bool GetIsDisplayStatusBarTemporarily() const;
    void SetIsDisplayStatusBarTemporarily(bool isTemporary);
    void SetIsLastFrameLayoutFinishedFunc(IsLastFrameLayoutFinishedFunc&& func);
    void SetIsAINavigationBarAvoidAreaValidFunc(IsAINavigationBarAvoidAreaValidFunc&& func);
    void RetrieveStatusBarDefaultVisibility();
    void RegisterNeedAvoidCallback(NotifyNeedAvoidFunc&& callback);
    void RegisterSystemBarPropertyChangeCallback(NotifySystemBarPropertyChangeFunc&& callback);
    void MarkAvoidAreaAsDirty();
    virtual void RecalculatePanelRectForAvoidArea(WSRect& panelRect) {}
    void RegisterGetStatusBarAvoidHeightFunc(GetStatusBarAvoidHeightFunc&& func);
    void RegisterGetStatusBarConstantlyShowFunc(GetStatusBarConstantlyShowFunc&& func);
    void HookAvoidAreaInCompatibleMode(const WSRect& rect, AvoidAreaType avoidAreaType, AvoidArea& avoidArea) const;

    void SetAbilitySessionInfo(std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo);
    void SetWindowDragHotAreaListener(const NotifyWindowDragHotAreaFunc& func);
    void SetSessionEventParam(SessionEventParam param);
    void SetSessionRectChangeCallback(const NotifySessionRectChangeFunc& func);
    void SetAdjustKeyboardLayoutCallback(const NotifyKeyboardLayoutAdjustFunc& func);
    void SetSkipDraw(bool skip);
    virtual void SetSkipSelfWhenShowOnVirtualScreen(bool isSkip);
    virtual void SetSkipEventOnCastPlus(bool isSkip);
    WMError SetUniqueDensityDpi(bool useUnique, float dpi);

    bool IsAnco() const override;
    void SetBlank(bool isAddBlank) override;
    bool GetBlank() const override;
    void SetBufferAvailableCallbackEnable(bool enable);
    bool GetBufferAvailableCallbackEnable() const override;
    int32_t GetCollaboratorType() const;
    WSRect GetLastSafeRect() const;
    WSRect GetSessionTargetRectByDisplayId(DisplayId displayId) const;
    std::string GetUpdatedIconPath() const;
    int32_t GetParentPersistentId() const;
    int32_t GetMainSessionId();
    virtual int32_t GetMissionId() const { return persistentId_; };
    Orientation GetRequestedOrientation() const;
    std::vector<sptr<SceneSession>> GetSubSession() const;
    std::vector<sptr<SceneSession>> GetToastSession() const;
    std::shared_ptr<AppExecFwk::AbilityInfo> GetAbilityInfo() const;
    std::string GetWindowNameAllType() const;
    SubWindowModalType GetSubWindowModalType() const;
    int32_t GetOriPosYBeforeRaisedByKeyboard() const;
    std::string GetClientIdentityToken() const;

    /*
     * Window Watermark
     */
    void SetWatermarkEnabled(const std::string& watermarkName, bool isEnabled);

    bool IsDecorEnable() const;
    bool IsAppSession() const;
    bool IsAppOrLowerSystemSession() const;
    bool IsSystemSessionAboveApp() const;
    bool IsTurnScreenOn() const;
    bool IsKeepScreenOn() const;
    bool IsViewKeepScreenOn() const;
    bool GetWindowShadowEnabled() const;
    bool IsShowWhenLocked() const;
    bool GetShowWhenLockedFlagValue() const;
    bool IsFloatingWindowAppType() const;
    bool IsNeedDefaultAnimation() const;
    void SetSystemTouchable(bool touchable) override;
    bool IsVisibleForAccessibility() const;
    void SetStartingWindowExitAnimationFlag(bool enable);
    bool NeedStartingWindowExitAnimation() const override;

    void NotifyWindowVisibility();
    WSError OnShowWhenLocked(bool showWhenLocked);
    void SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap>& icon);
    void NotifyTouchOutside();
    bool CheckTouchOutsideCallbackRegistered();
    void UpdateNativeVisibility(bool visible);
    void DumpSessionElementInfo(const std::vector<std::string>& params);
    void NotifyForceHideChange(bool hide);
    WSError BindDialogSessionTarget(const sptr<SceneSession>& sceneSession);
    void DumpSessionInfo(std::vector<std::string>& info) const;
    bool AddSubSession(const sptr<SceneSession>& subSession);
    bool RemoveSubSession(int32_t persistentId);
    bool AddToastSession(const sptr<SceneSession>& toastSession);
    bool RemoveToastSession(int32_t persistentId);
    void NotifySessionForeground(uint32_t reason, bool withAnimation);
    void NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits);
    void RegisterForceSplitListener(const NotifyForceSplitFunc& func);

    /*
     * Dialog Window
     */
    void RegisterBindDialogSessionCallback(const NotifyBindDialogSessionFunc& callback);

    void SetUpdatePrivateStateAndNotifyFunc(const UpdatePrivateStateAndNotifyFunc& func);

    /*
     * Window Input Event
     */
    void RegisterTouchOutsideCallback(NotifyTouchOutsideFunc&& callback);
    void SetMousePointerDownEventStatus(bool mousePointerDownEventStatus);
    bool GetMousePointerDownEventStatus() const;
    void SetFingerPointerDownStatus(int32_t fingerId);
    void RemoveFingerPointerDownStatus(int32_t fingerId);
    std::unordered_set<int32_t> GetFingerPointerDownStatusList() const;

    /*
     * Window Rotation
     */
    void RegisterRequestedOrientationChangeCallback(NotifyReqOrientationChangeFunc&& callback);
    WSError NotifyRotationProperty(uint32_t rotation, uint32_t width, uint32_t height);
    WSError ConvertRotationToOrientation(uint32_t rotation, uint32_t& orientation);
    void RegisterUpdateRotationChangeListener(NotifyRotationChangeFunc&& callback);
    WSError UpdateRotationChangeRegistered(int32_t persistentId, bool isRegister) override;
    RotationChangeResult NotifyRotationChange(const RotationChangeInfo& rotationChangeInfo,
        bool isRestrictNotify = false);
    bool isRotationChangeCallbackRegistered = false;
    WSError SetCurrentRotation(int32_t currentRotation);
    void RegisterFollowScreenChangeCallback(NotifyFollowScreenChangeFunc&& callback);
    WSError UpdateFollowScreenChange(bool isFollowScreenChange);

    /*
     * Window Animation
     */
    void RegisterIsCustomAnimationPlayingCallback(NotifyIsCustomAnimationPlayingCallback&& callback);
    void RegisterDefaultAnimationFlagChangeCallback(NotifyWindowAnimationFlagChangeFunc&& callback);
    void RegisterAnimateToCallback(NotifyAnimateToFunc&& callback);
    WMError AnimateTo(const WindowAnimationProperty& animationProperty,
        const WindowAnimationOption& animationOption);
    WSError SetFrameRectForPartialZoomIn(const Rect& frameRect) override;

    /*
     * Window Visibility
     */
    void SetNotifyVisibleChangeFunc(const NotifyVisibleChangeFunc& func);

    /*
     * Window Hierarchy
     */
    void RegisterRaiseToTopCallback(NotifyRaiseToTopFunc&& callback);
    void RegisterRaiseAboveTargetCallback(NotifyRaiseAboveTargetFunc&& callback);
    void RegisterRaiseMainWindowAboveTargetCallback(NotifyRaiseMainWindowAboveTargetFunc&& callback);
    void RegisterSessionTopmostChangeCallback(NotifySessionTopmostChangeFunc&& callback);
    void RegisterSubSessionZLevelChangeCallback(NotifySubSessionZLevelChangeFunc&& callback);

    /*
     * Window Lifecycle
     */
    void ClearJsSceneSessionCbMap(bool needRemove); // ONLY Accessed on OS_sceneSession thread
    void ClearSpecificSessionCbMap();
    void RegisterShowWhenLockedCallback(NotifyShowWhenLockedFunc&& callback);
    void RegisterForceHideChangeCallback(NotifyForceHideChangeFunc&& callback);
    void RegisterClearCallbackMapCallback(ClearCallbackMapFunc&& callback);
    virtual WSError HideSync() { return WSError::WS_DO_NOTHING; }
    void RegisterUpdateAppUseControlCallback(UpdateAppUseControlFunc&& func);
    void NotifyUpdateAppUseControl(ControlAppType type, const ControlInfo& controlInfo) override;
    void UpdatePrivacyModeControlInfo();
    bool HasChildSessionInPrivacyMode();
    void SetVisibilityChangedDetectFunc(VisibilityChangedDetectFunc&& func);
    virtual void RegisterSessionLockStateChangeCallback(NotifySessionLockStateChangeCallback&& callback) {}
    virtual void NotifySessionLockStateChange(bool isLockedState) {}
    virtual void SetUpdateSessionLabelAndIconListener(NofitySessionLabelAndIconUpdatedFunc&& func) {}
    virtual WMError GetRouterStackInfo(std::string& routerStackInfo) const { return WMError::WM_OK; }
    bool UpdateInteractiveInner(bool interactive);
    void HookSceneSessionActivation(NotifyHookSceneSessionActivationFunc&& func);
    void SetSceneSessionDestructNotificationFunc(NotifySceneSessionDestructFunc&& func);
    void SetIsUserRequestedExit(bool isUserRequestedExit);
    void SetGetAllAppUseControlMapFunc(GetAllAppUseControlMapFunc&& callback);
    void UpdateLifecyclePausedInner();
    void CalculatedStartWindowType(SessionInfo& sessionInfo, bool hideStartWindow);

    void SendPointerEventToUI(std::shared_ptr<MMI::PointerEvent> pointerEvent);
    bool SendKeyEventToUI(std::shared_ptr<MMI::KeyEvent> keyEvent, bool isPreImeEvent = false);
    bool IsStartMoving() override;
    void SetIsStartMoving(bool startMoving);
    bool IsSystemSpecificSession() const;
    void SetIsSystemSpecificSession(bool isSystemSpecificSession);
    void SetShouldHideNonSecureWindows(bool shouldHide);
    void UpdateExtWindowFlags(int32_t extPersistentId, const ExtensionWindowFlags& extWindowFlags,
        const ExtensionWindowFlags& extWindowActions);
    ExtensionWindowFlags GetCombinedExtWindowFlags();
    void RemoveExtWindowFlags(int32_t extPersistentId);
    void ClearExtWindowFlags();
    void NotifyDisplayMove(DisplayId from, DisplayId to);
    void NotifySessionFullScreen(bool fullScreen);
    void SetDefaultDisplayIdIfNeed();

    void SetSessionState(SessionState state) override;
    void SetForceHideState(ForceHideState forceHideState);
    ForceHideState GetForceHideState() const;
    bool IsTemporarilyShowWhenLocked() const;
    void SetTemporarilyShowWhenLocked(bool isTemporarilyShowWhenLocked);

    std::shared_ptr<PowerMgr::RunningLock> keepScreenLock_;
    std::shared_ptr<PowerMgr::RunningLock> viewKeepScreenLock_;

    static MaximizeMode maximizeMode_;
    static uint32_t GetWindowDragHotAreaType(DisplayId displayId, uint32_t type, int32_t pointerX, int32_t pointerY);
    static void AddOrUpdateWindowDragHotArea(DisplayId displayId, uint32_t type, const WSRect& area);
    WSError UpdateRectChangeListenerRegistered(bool isRegister) override;
    WMError NotifyDisableDelegatorChange() override;
    virtual void SetRecentSessionState(RecentSessionInfo& info, const SessionState& state) {}
    void RegisterGetStartWindowConfigFunc(GetStartWindowTypeFunc&& func);

    /*
     * Window Decor
     */
    int32_t GetCustomDecorHeight() const;
    void SetCustomDecorHeight(int32_t height) override;
    WSError SetDecorVisible(bool isVisible) override;
    bool IsDecorVisible() const;
    WindowDecoration GetWindowDecoration() const;

    WMError UpdateSessionPropertyByAction(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action) override;
    void SetSessionChangeByActionNotifyManagerListener(const SessionChangeByActionNotifyManagerFunc& func);

    /*
     * UIExtension
     */
    bool IsShowOnLockScreen(uint32_t lockScreenZOrder);
    void AddExtensionTokenInfo(const UIExtensionTokenInfo& tokenInfo);
    void RemoveExtensionTokenInfo(const sptr<IRemoteObject>& abilityToken);
    void CheckExtensionOnLockScreenToClose();
    void CloseExtensionSync(const UIExtensionTokenInfo& tokenInfo);
    void OnNotifyAboveLockScreen();
    void AddNormalModalUIExtension(const ExtensionWindowEventInfo& extensionInfo);
    void RemoveNormalModalUIExtension(int32_t persistentId);
    void UpdateNormalModalUIExtension(const ExtensionWindowEventInfo& extensionInfo);
    std::optional<ExtensionWindowEventInfo> GetLastModalUIExtensionEventInfo();
    Vector2f GetSessionGlobalPosition(bool useUIExtension);
    void AddUIExtSurfaceNodeId(uint64_t surfaceNodeId, int32_t persistentId);
    void RemoveUIExtSurfaceNodeId(int32_t persistentId);
    int32_t GetUIExtPersistentIdBySurfaceNodeId(uint64_t surfaceNodeId) const;
    int32_t GetStatusBarHeight() override;
    int32_t GetDockHeight();
    bool IsFreeMultiWindowMode() const
    {
        return systemConfig_.IsFreeMultiWindowMode();
    }
    WMError GetAppForceLandscapeConfig(AppForceLandscapeConfig& config) override;

    // WMSPipeline-related: only accessed on SSM thread
    virtual void SyncScenePanelGlobalPosition(bool needSync) {}
    void SetNeedSyncSessionRect(bool needSync);
    // update visible session, return dirty flags
    uint32_t UpdateUIParam(const SessionUIParam& uiParam) REQUIRES(SCENE_GUARD);
    // update invisible session, return dirty flags
    uint32_t UpdateUIParam() REQUIRES(SCENE_GUARD);
    void SetPostProcessFocusState(PostProcessFocusState state);
    PostProcessFocusState GetPostProcessFocusState() const;
    void ResetPostProcessFocusState();
    void SetPostProcessProperty(bool state);
    bool GetPostProcessProperty() const;
    void PostProcessNotifyAvoidArea();
    bool IsImmersiveType() const;
    bool IsPcOrPadEnableActivation() const;
    static void RegisterGetConstrainedModalExtWindowInfo(GetConstrainedModalExtWindowInfoFunc&& callback);

    /*
     * Multi User
     */
    void SetMinimizedFlagByUserSwitch(bool isMinimized);
    bool IsMinimizedByUserSwitch() const;
    void UnregisterSessionChangeListeners() override;

    /*
     * Window ZOrder: PC
     */
    void SetPcScenePanel(bool isPcScenePanel) { isPcScenePanel_ = isPcScenePanel; }
    void UpdatePCZOrderAndMarkDirty(const uint32_t zOrder);

    /*
     * Multi Window
     */
    WSError SetSplitButtonVisible(bool isVisible);
    WSError SendContainerModalEvent(const std::string& eventName, const std::string& eventValue);
    WSError OnContainerModalEvent(const std::string& eventName, const std::string& eventValue) override;
    void RegisterSetLandscapeMultiWindowFunc(NotifyLandscapeMultiWindowSessionFunc&& func);

    /*
     * Move Drag
     */
    void SetAppDragResizeType(DragResizeType dragResizeType) { appDragResizeType_ = dragResizeType; }
    DragResizeType GetAppDragResizeType() const { return appDragResizeType_; }
    void RegisterSessionEventCallback(NotifySessionEventFunc&& callback);
    void SetWindowMovingCallback(NotifyWindowMovingFunc&& func);
    DMRect CalcRectForStatusBar();
    WSError SetMoveAvailableArea(DisplayId displayId);
    bool IsDragMoving() const override;
    bool IsDragZooming() const override;
    // KeyFrame
    WSError UpdateKeyFrameCloneNode(std::shared_ptr<RSCanvasNode>& rsCanvasNode,
        std::shared_ptr<RSTransaction>& rsTransaction) override;
    void SetKeyFramePolicy(const KeyFramePolicy& keyFramePolicy);
    WSError SetDragKeyFramePolicy(const KeyFramePolicy& keyFramePolicy) override;
    WSError KeyFrameAnimateEnd() override;

    /*
     * Window Layout
     */
    bool IsDirtyWindow();
    bool IsDirtyDragWindow();
    void ResetDirtyDragFlags();
    void ResetSizeChangeReasonIfDirty();
    void SetRequestNextVsyncFunc(RequestVsyncFunc&& func);
    void OnNextVsyncReceivedWhenDrag(const WSRect& globalRect,
        bool isGlobal, bool needFlush, bool needSetBoundsNextVsync);
    void RegisterLayoutFullScreenChangeCallback(NotifyLayoutFullScreenChangeFunc&& callback);
    bool SetFrameGravity(Gravity gravity);
    WSError GetCrossAxisState(CrossAxisState& state) override;
    virtual void UpdateCrossAxis();
    bool GetIsFollowParentLayout() const { return isFollowParentLayout_; }
    sptr<MoveDragController> GetMoveDragController() const { return moveDragController_; }
    void NotifyUpdateGravity();
    void SetWindowAnchorInfoChangeFunc(NotifyWindowAnchorInfoChangeFunc&& func);
    WSError SetWindowAnchorInfo(const WindowAnchorInfo& windowAnchorInfo) override;
    WindowAnchorInfo GetWindowAnchorInfo() const { return windowAnchorInfo_; }
    void CalcSubWindowRectByAnchor(const WSRect& parentRect, WSRect& subRect);
    bool IsAnyParentSessionDragMoving() const override;
    bool IsAnyParentSessionDragZooming() const override;
    bool IsCompatibleModeDirtyDragScaleWindow() const;
    void ResetCompatibleModeDragScaleFlags();
    void RegisterAppHookWindowInfoFunc(GetHookWindowInfoFunc&& func);
    WMError GetAppHookWindowInfoFromServer(HookWindowInfo& hookWindowInfo) override;
    void SetFindScenePanelRsNodeByZOrderFunc(FindScenePanelRsNodeByZOrderFunc&& func);

    /*
     * Gesture Back
     */
    bool GetGestureBackEnabled();
    bool GetEnableGestureBackHadSet();

    /*
     * PC Fold Screen
     */
    void UpdateFullScreenWaterfallMode(bool isWaterfallMode);
    void RegisterFullScreenWaterfallModeChangeCallback(std::function<void(bool isWaterfallMode)>&& func);
    void OnThrowSlipAnimationStateChange(bool isAnimating, bool isFullScreen);
    void RegisterThrowSlipAnimationStateChangeCallback(std::function<void(bool isAnimating, bool isFullScreen)>&& func);
    bool IsMissionHighlighted();
    bool IsPcFoldDevice();
    void MaskSupportEnterWaterfallMode();
    void SetSupportEnterWaterfallMode(bool isSupportEnter);
    void ThrowSlipDirectly(ThrowSlipMode throwSlipMode, const WSRectF& velocity);
    WSError GetWaterfallMode(bool& isWaterfallMode) override;

    /*
     * Keyboard
     */
    void SetIsSystemKeyboard(bool isSystemKeyboard);
    bool IsSystemKeyboard() const;
    void ActivateKeyboardAvoidArea(bool active, bool recalculateAvoid);
    bool IsKeyboardAvoidAreaActive() const;
    virtual void SetKeyboardEffectOptionChangeListener(const NotifyKeyboarEffectOptionChangeFunc& func) {};
    void GetKeyboardOccupiedAreaWithRotation(
        int32_t persistentId, Rotation rotation, std::vector<std::pair<bool, WSRect>>& avoidAreas);
    void NotifyKeyboardAnimationCompleted(bool isShowAnimation, const WSRect& beginRect, const WSRect& endRect);
    void NotifyKeyboardAnimationWillBegin(bool isKeyboardShow, const WSRect& beginRect, const WSRect& endRect,
        bool withAnimation, const std::shared_ptr<RSTransaction>& rsTransaction);
    void NotifyKeyboardWillShowRegistered(bool registered) override;
    void NotifyKeyboardWillHideRegistered(bool registered) override;
    void NotifyKeyboardDidShowRegistered(bool registered) override;
    void NotifyKeyboardDidHideRegistered(bool registered) override;
    bool isSubWindowResizingOrMoving_ = false;
    virtual void CalculateOccupiedAreaAfterUIRefresh() {}
    virtual WMError HandleActionUpdateKeyboardTouchHotArea(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action) { return WMError::WM_OK; }

    /*
     * Window Focus
     */
    bool IsSameMainSession(const sptr<SceneSession>& prevSession);
    void SetHighlightChangeNotifyFunc(const NotifyHighlightChangeFunc& func);
    void SetFollowParentRectFunc(NotifyFollowParentRectFunc&& func);
    WSError SetFollowParentWindowLayoutEnabled(bool isFollow) override;
    bool IsDelayFocusChange();
    virtual bool IsBlockingFocusWindowType() const;

    /*
     * Window Property
    */
    void SetWindowCornerRadiusCallback(NotifySetWindowCornerRadiusFunc&& func);
    WSError SetWindowCornerRadius(float cornerRadius) override;
    void SetPrivacyModeChangeNotifyFunc(NotifyPrivacyModeChangeFunc&& func);
    void SetIsAncoForFloatingWindow(bool isAncoForFloatingWindow);
    bool GetIsAncoForFloatingWindow() const;
    void SetWindowShadowsCallback(NotifySetWindowShadowsFunc&& func);
    WSError SetWindowShadows(const ShadowsInfo& shadowsInfo) override;
    virtual WSError GetTopNavDestinationName(std::string& topNavDestName);
    void RegisterWindowShadowEnableChangeCallback(NotifyWindowShadowEnableChangeFunc&& callback);
    void SetNotifyScreenshotAppEventRegisteredFunc(UpdateScreenshotAppEventRegisteredFunc&& func);
    WMError UpdateScreenshotAppEventRegistered(int32_t persistentId, bool isRegister) override;
    WMError UpdateAcrossDisplaysChangeRegistered(bool isRegister) override;
    WMError IsMainWindowFullScreenAcrossDisplays(bool& isAcrossDisplays) override;
    bool GetIsRegisterAcrossMultiDisplayChanged() const { return isRegisterAcrossDisplaysChanged_.load(); }
    WMError NotifySubSessionAcrossDisplaysChange(bool isAcrossDisplays);
    WMError NotifyFollowedParentWindowAcrossDisplaysChange(bool isAcrossDisplays);
    void NotifySessionAcrossDisplaysChange(const sptr<SceneSession>& sceneSession, bool isAcrossDisplays);
    WMError OnUpdateColorMode(const std::string& colorMode, bool hasDarkRes) override;
    std::string GetAbilityColorMode() const;

    /*
     * Window Pattern
    */
    void NotifyWindowAttachStateListenerRegistered(bool registered) override;
    WMError NotifySnapshotUpdate() override;

    /**
     * Window Transition Animation For PC
     */
    WSError SetWindowTransitionAnimation(WindowTransitionType transitionType,
        const TransitionAnimation& animation) override;
    void SetTransitionAnimationCallback(UpdateTransitionAnimationFunc&& func);

protected:
    void NotifyIsCustomAnimationPlaying(bool isPlaying);
    void SetMoveDragCallback();
    std::string GetRatioPreferenceKey();
    WSError NotifyClientToUpdateRectTask(const std::string& updateReason, std::shared_ptr<RSTransaction> rsTransaction);
    bool CheckPermissionWithPropertyAnimation(const sptr<WindowSessionProperty>& property) const;

    std::string GetRectInfo(const WSRect& rect)
    {
        using std::to_string;
        return "[" + to_string(rect.width_) + ", " + to_string(rect.height_) + "; "
        + to_string(rect.posX_) + ", " + to_string(rect.posY_) + "]";
    }

    /*
     * Window Pipeline
     */
    bool UpdateVisibilityInner(bool visibility) REQUIRES(SCENE_GUARD);
    virtual void NotifyClientToUpdateInteractive(bool interactive) {}
    bool PipelineNeedNotifyClientToUpdateRect() const;
    bool UpdateRectInner(const SessionUIParam& uiParam, SizeChangeReason reason);
    bool NotifyServerToUpdateRect(const SessionUIParam& uiParam, SizeChangeReason reason);
    bool UpdateScaleInner(float scaleX, float scaleY, float pivotX, float pivotY);
    bool UpdateZOrderInner(uint32_t zOrder);

    /*
     * Window Immersive
     */
    void CalculateAvoidAreaRect(const WSRect& rect, const WSRect& avoidRect, AvoidArea& avoidArea) const;
    virtual void NotifyClientToUpdateAvoidArea();
    bool PipelineNeedNotifyClientToUpdateAvoidArea(uint32_t dirty) const;
    NotifyNeedAvoidFunc onNeedAvoid_;
    NotifySystemBarPropertyChangeFunc onSystemBarPropertyChange_;
    GetStatusBarAvoidHeightFunc onGetStatusBarAvoidHeightFunc_;
    GetStatusBarConstantlyShowFunc onGetStatusBarConstantlyShowFunc_;
    void PrintAvoidAreaInfo(DisplayId displayId,
        AvoidAreaType type, const WSRect& winRect, const WSRect& avoidRect) const;
    void CalculateAvoidAreaByType(AvoidAreaType type,
        const WSRect& winRect, const WSRect& avoidRect, AvoidArea& avoidArea);

    /*
     * Gesture Back
     */
    void UpdateGestureBackEnabled() override;

    sptr<SpecificSessionCallback> specificCallback_ = nullptr;

    /*
     * Dialog window
     */
    NotifyBindDialogSessionFunc onBindDialogTarget_;

    sptr<SceneSession> keyboardPanelSession_ = nullptr;
    sptr<SceneSession> keyboardSession_ = nullptr;
    NotifyKeyboardLayoutAdjustFunc adjustKeyboardLayoutFunc_;

    /*
     * Window Hierarchy
     */
    NotifyMainWindowTopmostChangeFunc mainWindowTopmostChangeFunc_;
    NotifyRaiseToTopFunc onRaiseToTop_;
    NotifyRaiseAboveTargetFunc onRaiseAboveTarget_;
    NotifySessionTopmostChangeFunc onSessionTopmostChange_;
    NotifySubSessionZLevelChangeFunc onSubSessionZLevelChange_;

    /*
     * PC Window
     */
    NotifyTitleAndDockHoverShowChangeFunc onTitleAndDockHoverShowChangeFunc_;
    NotifyRestoreMainWindowFunc onRestoreMainWindowFunc_;
    NotifySetWindowRectAutoSaveFunc onSetWindowRectAutoSaveFunc_;
    NotifySubModalTypeChangeFunc onSubModalTypeChange_;
    NotifyMainModalTypeChangeFunc onMainModalTypeChange_;
    NotifySetSupportedWindowModesFunc onSetSupportedWindowModesFunc_;
    NotifyUpdateFlagFunc onUpdateFlagFunc_;
    NotifyUseImplicitAnimationChangeFunc useImplicitAnimationChangeFunc_;

    /*
     * PiP Window
     */
    NotifyPrepareClosePiPSessionFunc onPrepareClosePiPSession_;

    /*
     * Window Layout
     */
    NotifyDefaultDensityEnabledFunc onDefaultDensityEnabledFunc_;
    sptr<MoveDragController> moveDragController_ = nullptr;
    std::mutex displayIdSetDuringMoveToMutex_;
    std::set<uint64_t> displayIdSetDuringMoveTo_;
    NotifyFollowParentRectFunc followParentRectFunc_ = nullptr;
    std::mutex registerNotifySurfaceBoundsChangeMutex_;
    std::unordered_map<int32_t, NotifySurfaceBoundsChangeFunc> notifySurfaceBoundsChangeFuncMap_;
    bool isFollowParentLayout_ = false;
    NotifyWindowAnchorInfoChangeFunc onWindowAnchorInfoChangeFunc_ = nullptr;
    WindowAnchorInfo windowAnchorInfo_;
    FindScenePanelRsNodeByZOrderFunc findScenePanelRsNodeByZOrderFunc_;
    int32_t cloneNodeCount_ = 0;

    virtual void NotifySubSessionRectChangeByAnchor(const WSRect& parentRect,
        SizeChangeReason reason = SizeChangeReason::UNDEFINED, DisplayId displayId = DISPLAY_ID_INVALID) {}
    virtual void UpdateSessionRectInner(const WSRect& rect, SizeChangeReason reason,
        const MoveConfiguration& moveConfiguration, const RectAnimationConfig& rectAnimationConfig = {});
    void NotifySessionDisplayIdChange(uint64_t displayId);
    virtual void CheckAndMoveDisplayIdRecursively(uint64_t displayId);
    void SetShouldFollowParentWhenShow(bool shouldFollow) { shouldFollowParentWhenShow_ = shouldFollow; }
    bool GetShouldFollowParentWhenShow() const { return shouldFollowParentWhenShow_; }
    void CheckSubSessionShouldFollowParent(uint64_t displayId);
    bool IsNeedConvertToRelativeRect(SizeChangeReason reason = SizeChangeReason::UNDEFINED) const override;
    void SetRequestMoveConfiguration(const MoveConfiguration& config) { requestMoveConfiguration_ = config; }
    MoveConfiguration GetRequestMoveConfiguration() const { return requestMoveConfiguration_; }
    SubWindowSource subWindowSource_ = SubWindowSource::SUB_WINDOW_SOURCE_UNKNOWN;
    NotifySetSubWindowSourceFunc subWindowSourceFunc_ = nullptr;

    /*
     * Floating Ball Window
     */
    virtual void NotifyUpdateFloatingBall(const FloatingBallTemplateInfo& fbTemplateInfo) {};
    virtual void NotifyStopFloatingBall() {};
    virtual void NotifyRestoreFloatingBallMainWindow(const std::shared_ptr<AAFwk::Want>& want) {};
    std::mutex fbClickMutex_;
    uint8_t fbClickCnt_ {0};
    FloatingBallTemplateInfo fbTemplateInfo_ = {};

    /*
     * Window Lifecycle
     */
    bool isAppUseControl_ = false;
    NotifyShowWhenLockedFunc onShowWhenLockedFunc_;
    NotifyForceHideChangeFunc onForceHideChangeFunc_;
    ClearCallbackMapFunc clearCallbackMapFunc_;
    UpdateAppUseControlFunc onUpdateAppUseControlFunc_;
    std::unordered_map<ControlAppType, ControlInfo> appUseControlMap_;
    GetAllAppUseControlMapFunc onGetAllAppUseControlMapFunc_;

    /*
     * PC Fold Screen
     */
    bool IsFullScreenWaterfallMode();
    void SetFullScreenWaterfallMode(bool isFullScreenWaterfallMode);
    void UpdateWaterfallMode(SessionEvent event);
    sptr<PcFoldScreenController> pcFoldScreenController_ = nullptr;
    std::atomic<uint32_t> throwSlipToFullScreenAnimCount_ = 0;
    std::function<void(bool isAnimating, bool isFullScreen)> onThrowSlipAnimationStateChangeFunc_;

    /*
     * Compatible Mode
     */
    void HookStartMoveRect(WSRect& newRect, const WSRect& sessionRect);

    /*
     * UIExtension
     */
    static GetConstrainedModalExtWindowInfoFunc onGetConstrainedModalExtWindowInfoFunc_;

    /*
     * Multi Window
     */
    NotifyLandscapeMultiWindowSessionFunc onSetLandscapeMultiWindowFunc_;

    /*
     * Keyboard
     */
    virtual void EnableCallingSessionAvoidArea() {}
    virtual void RestoreCallingSession(uint32_t callingId, const std::shared_ptr<RSTransaction>& rsTransaction) {}
    bool keyboardAvoidAreaActive_ = true;

private:
    void NotifyAccessibilityVisibilityChange();
    void CalculateCombinedExtWindowFlags();

    /*
     * Window Immersive
     */
    void SetSystemBarPropertyForRotation(const std::map<WindowType, SystemBarProperty>& properties);
    std::map<Rosen::WindowType, Rosen::SystemBarProperty>& GetSystemBarPropertyForRotation();
    void GetSystemAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void GetCutoutAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void GetKeyboardAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void GetAINavigationBarArea(WSRect& rect, AvoidArea& avoidArea);
    void PatchAINavigationBarArea(AvoidArea& avoidArea);
    AvoidArea GetAvoidAreaByTypeInner(AvoidAreaType type, const WSRect& rect = WSRect::EMPTY_RECT);
    WSError GetAvoidAreasByRotation(Rotation rotation, const WSRect& rect,
        const std::map<WindowType, SystemBarProperty>& properties, std::map<AvoidAreaType, AvoidArea>& avoidAreas);
    void GetSystemBarAvoidAreaByRotation(Rotation rotation, AvoidAreaType type, const WSRect& rect,
        const std::map<WindowType, SystemBarProperty>& properties, AvoidArea& avoidArea);
    void GetCutoutAvoidAreaByRotation(Rotation rotation, const WSRect& rect, AvoidArea& avoidArea);
    void GetKeyboardAvoidAreaByRotation(Rotation rotation, const WSRect& rect, AvoidArea& avoidArea);
    AvoidArea GetAvoidAreaByRotation(Rotation rotation, const WSRect& rect,
        const std::map<WindowType, SystemBarProperty>& properties, AvoidAreaType type);

    /*
     * Window Lifecycle
     */
    NotifySceneSessionDestructFunc notifySceneSessionDestructFunc_;
    bool CheckIdentityTokenIfMatched(const std::string& identityToken);
    bool CheckPidIfMatched();
    GetStartWindowTypeFunc getStartWindowConfigFunc_;
    StartWindowType startWindowType_ = StartWindowType::DEFAULT;

    // session lifecycle funcs
    WSError ForegroundTask(const sptr<WindowSessionProperty>& property);

    /*
     * Move Drag
     */
    virtual void HandleMoveDragSurfaceNode(SizeChangeReason reason);
    void OnMoveDragCallback(SizeChangeReason reason);
    bool DragResizeWhenEndFilter(SizeChangeReason reason);
    void HandleMoveDragEvent(SizeChangeReason reason);
    bool IsDragResizeScale(SizeChangeReason reason);
    void InitializeCrossMoveDrag();
    WSError InitializeMoveInputBar();
    void HandleMoveDragSurfaceBounds(WSRect& rect, WSRect& globalRect, SizeChangeReason reason);
    void HandleMoveDragEnd(WSRect& rect, SizeChangeReason reason);
    void WindowScaleTransfer(WSRect& rect, float scaleX, float scaleY);
    bool IsCompatibilityModeScale(float scaleX, float scaleY);
    void CompatibilityModeWindowScaleTransfer(WSRect& rect, bool isScale);
    void ThrowSlipToFullScreen(WSRect& endRect, WSRect& rect, int32_t statusBarHeight, int32_t dockHeight);
    bool MoveUnderInteriaAndNotifyRectChange(WSRect& rect, SizeChangeReason reason);
    void NotifyFullScreenAfterThrowSlip(const WSRect& rect);
    void SetDragResizeTypeDuringDrag(DragResizeType dragResizeType) { dragResizeTypeDuringDrag_ = dragResizeType; }
    DragResizeType GetDragResizeTypeDuringDrag() const { return dragResizeTypeDuringDrag_; }
    void HandleSessionDragEvent(SessionEvent event);
    NotifySessionEventFunc onSessionEvent_;
    void ProcessWindowMoving(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void HandleSubSessionCrossNode(SizeChangeReason reason);
    void RunAfterNVsyncs(uint32_t vsyncCount, Task&& task);
    void RestoreGravityWhenDragEnd();

    /*
     * Gesture Back
     */
    WMError SetGestureBackEnabled(bool isEnabled) override;

    /*
     * UIExtension
     */
    void UpdateAllModalUIExtensions(const WSRect& globalRect);

    /*
     * Window Property
     */
    NotifyWindowShadowEnableChangeFunc onWindowShadowEnableChangeFunc_;
    void NotifyPrivacyModeChange();

#ifdef DEVICE_STATUS_ENABLE
    void RotateDragWindow(std::shared_ptr<RSTransaction> rsTransaction);
#endif // DEVICE_STATUS_ENABLE
    void NotifyPropertyWhenConnect();
    WSError RaiseAppMainWindowToTop() override;
    void UpdateWinRectForSystemBar(WSRect& rect);
    bool IsMovableWindowType() const;
    bool IsFullScreenMovable() const;
    void HandleCastScreenConnection(SessionInfo& info, sptr<SceneSession> session);
    WMError HandleUpdatePropertyByAction(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateTurnScreenOn(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateKeepScreenOn(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateViewKeepScreenOn(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateFocusable(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateTouchable(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateSetBrightness(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateOrientation(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdatePrivacyMode(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateSnapshotSkip(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateMaximizeState(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateOtherProps(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateStatusProps(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateNavigationProps(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateNavigationIndicatorProps(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateFlags(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateMode(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateAnimationFlag(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateTouchHotArea(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateDecorEnable(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateWindowLimits(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateDragenabled(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateRaiseenabled(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateHideNonSystemFloatingWindows(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateTextfieldAvoidInfo(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateWindowMask(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateTopmost(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateMainWindowTopmost(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateSubWindowZLevel(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateWindowModeSupportType(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError ProcessUpdatePropertyByAction(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateAvoidAreaOption(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleBackgroundAlpha(const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action);
    WMError HandleActionUpdateExclusivelyHighlighted(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    WMError HandleActionUpdateFollowScreenChange(const sptr<WindowSessionProperty>& property,
            WSPropertyChangeAction action);
    WMError HandleActionUpdateWindowShadowEnabled(const sptr<WindowSessionProperty>& property,
            WSPropertyChangeAction action);
    void HandleSpecificSystemBarProperty(WindowType type, const sptr<WindowSessionProperty>& property);
    void SetWindowFlags(const sptr<WindowSessionProperty>& property);
    void NotifySessionChangeByActionNotifyManager(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action);
    void NotifyExtensionSecureLimitChange(bool isLimit);

    // window animation
    WSError SetFrameRectForPartialZoomInInner(const Rect& frameRect);

    /*
     * PiP Window
     */
    NotifySessionPiPControlStatusChangeFunc sessionPiPControlStatusChangeFunc_;
    NotifyAutoStartPiPStatusChangeFunc autoStartPiPStatusChangeFunc_;
    NotifyUpdatePiPTemplateInfoFunc updatePiPTemplateInfoCallbackFunc_;
    PiPTemplateInfo pipTemplateInfo_ = {};

    NotifyForceSplitFunc forceSplitFunc_;
    UpdatePrivateStateAndNotifyFunc updatePrivateStateAndNotifyFunc_;
    int32_t collaboratorType_ = CollaboratorType::DEFAULT_TYPE;
    WSRect lastSafeRect = { 0, 0, 0, 0 };
    std::vector<sptr<SceneSession>> subSession_;
    std::vector<sptr<SceneSession>> toastSession_;
    std::atomic_bool needStartingWindowExitAnimation_ { true };
    bool needDefaultAnimationFlag_ = true;
    bool isFollowScreenChange_ = false;
    SessionEventParam sessionEventParam_ = { 0, 0, 0, 0, 0 };
    std::atomic_bool isStartMoving_ { false };
    std::atomic_bool isVisibleForAccessibility_ { true };
    bool isSystemSpecificSession_ { false };

    /*
     * UIExtension
     */
    std::atomic_bool shouldHideNonSecureWindows_ { false };
    ExtensionWindowFlags combinedExtWindowFlags_ { 0 };
    std::map<int32_t, ExtensionWindowFlags> extWindowFlagsMap_;
    std::vector<UIExtensionTokenInfo> extensionTokenInfos_;

    /*
     * Window Layout
     */
    GetHookWindowInfoFunc getHookWindowInfoFunc_ = nullptr;
    bool SaveAspectRatio(float ratio);
    WSError UpdateRectForDrag(const WSRect& rect);
    void UpdateSessionRectPosYFromClient(SizeChangeReason reason, DisplayId& configDisplayId, WSRect& rect);
    void HandleSubSessionSurfaceNode(bool isAdd, DisplayId draggingOrMovingParentDisplayId);
    virtual void HandleSubSessionSurfaceNodeByWindowAnchor(SizeChangeReason reason, DisplayId displayId) {}
    virtual void AddSurfaceNodeToScreen(DisplayId draggingOrMovingParentDisplayId) {}
    virtual void RemoveSurfaceNodeFromScreen() {}
    void SetParentRect();
    WSRect GetGlobalOrWinRect();

    /*
     * Window Decor
     */
    mutable std::mutex customDecorHeightMutex_;
    int32_t customDecorHeight_ = 0;
    bool isDecorVisible_ = true;
    // guarded by customDecorHeightMutex_

    ForceHideState forceHideState_ { ForceHideState::NOT_HIDDEN };
    int32_t oriPosYBeforeRaisedByKeyboard_ = 0;
    std::atomic_bool isTemporarilyShowWhenLocked_ { false };
    std::shared_mutex modalUIExtensionInfoListMutex_;
    std::vector<ExtensionWindowEventInfo> modalUIExtensionInfoList_;
    mutable std::shared_mutex uiExtNodeIdToPersistentIdMapMutex_;
    std::map<uint64_t, int32_t> uiExtNodeIdToPersistentIdMap_;
    std::string clientIdentityToken_ = { "" };
    SessionChangeByActionNotifyManagerFunc sessionChangeByActionNotifyManagerFunc_;

    bool isAddBlank_ = false;
    bool bufferAvailableCallbackEnable_ = false;

    // WMSPipeline-related: only accessed on SSM thread
    PostProcessFocusState postProcessFocusState_;
    bool postProcessProperty_ { false };

    /*
     * Window Recover
     */
    bool isRecovered_ = false;

    /*
     * Multi User
     */
    bool isMinimizedByUserSwitch_ { false };

    /*
     * Window ZOrder: PC
     */
    bool isPcScenePanel_ { false };

    /*
     * Move Drag
     */
    static std::shared_mutex windowDragHotAreaMutex_;
    static std::map<uint64_t, std::map<uint32_t, WSRect>> windowDragHotAreaMap_;
    DragResizeType appDragResizeType_ = DragResizeType::RESIZE_TYPE_UNDEFINED;
    DragResizeType dragResizeTypeDuringDrag_ = DragResizeType::RESIZE_TYPE_UNDEFINED;
    NotifyWindowMovingFunc notifyWindowMovingFunc_;
    // KeyFrame
    void UpdateKeyFrameState(SizeChangeReason reason, const WSRect& rect);
    void InitKeyFrameState(const WSRect& rect);
    void RequestKeyFrameNextVsync(uint64_t requestStamp, uint64_t count);
    void OnKeyFrameNextVsync(uint64_t count);
    bool KeyFrameNotifyFilter(const WSRect& rect, SizeChangeReason reason);
    bool KeyFrameRectAlmostSame(const WSRect& rect1, const WSRect& rect2);
    std::mutex keyFrameMutex_;
    KeyFramePolicy keyFramePolicy_;
    std::shared_ptr<RSCanvasNode> keyFrameCloneNode_ = nullptr;
    bool keyFrameAnimating_ = false;
    uint64_t lastKeyFrameStamp_ = 0;
    WSRect lastKeyFrameRect_;
    uint64_t keyFrameVsyncRequestStamp_ = 0;
    uint64_t lastKeyFrameDragStamp_ = 0;
    WSRect lastKeyFrameDragRect_;
    bool keyFrameDragPauseNoticed_ = false;

    /*
     * Gesture Back
     */
    bool isEnableGestureBack_ { true };
    bool isEnableGestureBackHadSet_ { false };

    /*
     * Window Visibility
     */
    NotifyVisibleChangeFunc notifyVisibleChangeFunc_;

    /*
     * Window Input Event
     */
    NotifyTouchOutsideFunc onTouchOutside_;
    bool isMousePointerDownEventStatus_ { false };
    std::unordered_set<int32_t> fingerPointerDownStatusList_;

    /*
     * Window Rotation
     */
    NotifyReqOrientationChangeFunc onRequestedOrientationChange_;
    NotifyRotationChangeFunc onUpdateRotationChangeFunc_;
    bool GetFollowScreenChange() const;
    void SetFollowScreenChange(bool isFollowScreenChange);

    /*
     * Window Animation
     */
    NotifyIsCustomAnimationPlayingCallback onIsCustomAnimationPlaying_;
    NotifyWindowAnimationFlagChangeFunc onWindowAnimationFlagChange_;
    NotifyAnimateToFunc onAnimateTo_;
    bool isUIFirstEnabled_ = false;

    /*
     * Window Layout
     */
    void SetSurfaceBoundsWithAnimation(
        const std::pair<RSAnimationTimingProtocol, RSAnimationTimingCurve>& animationParam,
        const WSRect& rect, const std::function<void()>& finishCallback = nullptr, bool isGlobal = false);
    virtual void UpdateCrossAxisOfLayout(const WSRect& rect);
    NotifyLayoutFullScreenChangeFunc onLayoutFullScreenChangeFunc_;
    WSRect requestRectWhenFollowParent_;
    MoveConfiguration requestMoveConfiguration_;
    virtual void NotifySubAndDialogFollowRectChange(const WSRect& rect, bool isGlobal, bool needFlush) {};
    virtual void SetSubWindowBoundsDuringCross(const WSRect& parentRect, bool isGlobal, bool needFlush) {}
    std::atomic<bool> shouldFollowParentWhenShow_ = true;
    bool isDragging_ = false;
    std::atomic<bool> isCrossAxisOfLayout_ = false;
    std::atomic<uint32_t> crossAxisState_ = 0;

    /*
     * Window Immersive
     */
    std::map<Rosen::WindowType, Rosen::SystemBarProperty> targetSystemBarProperty_;
    std::atomic_bool isDisplayStatusBarTemporarily_ { false };
    IsLastFrameLayoutFinishedFunc isLastFrameLayoutFinishedFunc_;
    IsAINavigationBarAvoidAreaValidFunc isAINavigationBarAvoidAreaValid_;
    std::unordered_map<AvoidAreaType, std::tuple<DisplayId, WSRect, WSRect>> lastAvoidAreaInputParamtersMap_;

    /*
     * PC Window Layout
     */
    bool isLayoutFullScreen_ { false };
    bool isDefaultDensityEnabled_ = false;

    /**
     * Window pattern
     */
    void NotifyAddOrRemoveSnapshotWindow(bool interactive);

    /*
     * Window Property
     */
    std::unordered_set<std::string> containerColorList_;
    NotifySetWindowCornerRadiusFunc onSetWindowCornerRadiusFunc_;
    NotifyPrivacyModeChangeFunc privacyModeChangeNotifyFunc_;
    // Set true if either sessionProperty privacyMode or combinedExtWindowFlags_ privacyModeFlag is true.
    bool isPrivacyMode_ { false };
    bool isAncoForFloatingWindow_ = false;
    bool subWindowOutlineEnabled_ = false;
    std::atomic_bool isRegisterAcrossDisplaysChanged_ = false;
    std::string colorMode_;
    bool hasDarkRes_ = false;
    mutable std::mutex colorModeMutex_;
    NotifySetWindowShadowsFunc onSetWindowShadowsFunc_;
    UpdateScreenshotAppEventRegisteredFunc updateScreenshotAppEventRegisteredFunc_;

    /*
     * PC Window Sidebar Blur
     */
    std::shared_ptr<Rosen::RSAnimatableProperty<float>> blurRadiusValue_;
    std::shared_ptr<Rosen::RSAnimatableProperty<float>> blurSaturationValue_;
    std::shared_ptr<Rosen::RSAnimatableProperty<float>> blurBrightnessValue_;
    std::shared_ptr<Rosen::RSAnimatableProperty<Rosen::RSColor>> blurMaskColorValue_;

   /*
    * Window Lifecycle
    */
    NotifyHookSceneSessionActivationFunc hookSceneSessionActivationFunc_;
    bool isUserRequestedExit_ = false;

    /**
     * Window Transition Animation For PC
     */
    UpdateTransitionAnimationFunc updateTransitionAnimationFunc_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H
