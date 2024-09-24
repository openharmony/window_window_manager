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

#include "session/host/include/session.h"
#include "session/host/include/move_drag_controller.h"
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
namespace {
    constexpr int32_t MIN_DECOR_HEIGHT = 37;
    constexpr int32_t MAX_DECOR_HEIGHT = 112;
}
class SceneSession;
using SpecificSessionCreateCallback =
  std::function<sptr<SceneSession>(const SessionInfo& info, sptr<WindowSessionProperty> property)>;
using SpecificSessionDestroyCallback = std::function<WSError(const int32_t& persistentId)>;
using CameraFloatSessionChangeCallback = std::function<void(uint32_t accessTokenId, bool isShowing)>;
using GetSceneSessionVectorByTypeCallback = std::function<std::vector<sptr<SceneSession>>(
    WindowType type, uint64_t displayId)>;
using UpdateAvoidAreaCallback = std::function<void(const int32_t& persistentId)>;
using NotifyWindowInfoUpdateCallback = std::function<void(int32_t persistentId, WindowUpdateType type)>;
using NotifyWindowPidChangeCallback = std::function<void(int32_t windowId, bool startMoving)>;
using NotifySessionTouchOutsideCallback = std::function<void(int32_t persistentId)>;
using GetAINavigationBarArea = std::function<WSRect(uint64_t displayId)>;
using RecoveryCallback = std::function<void(int32_t persistentId, Rect rect)>;
using NotifyBindDialogSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifySessionPiPControlStatusChangeFunc = std::function<void(WsPiPControlType controlType,
    WsPiPControlStatus status)>;
using NotifySessionEventFunc = std::function<void(int32_t eventId, SessionEventParam param)>;
using NotifySessionTopmostChangeFunc = std::function<void(const bool topmost)>;
using NotifyRaiseToTopFunc = std::function<void()>;
using SetWindowPatternOpacityFunc = std::function<void(float opacity)>;
using NotifyIsCustomAnimationPlayingCallback = std::function<void(bool isFinish)>;
using NotifyWindowAnimationFlagChangeFunc = std::function<void(const bool flag)>;
using NotifySystemBarPropertyChangeFunc = std::function<void(
    const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)>;
using NotifyNeedAvoidFunc = std::function<void(bool status)>;
using NotifyShowWhenLockedFunc = std::function<void(bool showWhenLocked)>;
using NotifyReqOrientationChangeFunc = std::function<void(uint32_t orientation)>;
using NotifyRaiseAboveTargetFunc = std::function<void(int32_t subWindowId)>;
using NotifyForceHideChangeFunc = std::function<void(bool hide)>;
using NotifyTouchOutsideFunc = std::function<void()>;
using ClearCallbackMapFunc = std::function<void(bool needRemove, int32_t persistentId)>;
using NotifyPrepareClosePiPSessionFunc = std::function<void()>;
using OnOutsideDownEvent = std::function<void(int32_t x, int32_t y)>;
using HandleSecureSessionShouldHideCallback = std::function<WSError(const sptr<SceneSession>& sceneSession)>;
using SystemSessionBufferAvailableCallback = std::function<void()>;
using ClearDisplayStatusBarTemporarilyFlags = std::function<void()>;
using CameraSessionChangeCallback = std::function<void(uint32_t accessTokenId, bool isShowing)>;
using NotifyLandscapeMultiWindowSessionFunc = std::function<void(bool isLandscapeMultiWindow)>;
using NotifyKeyboardGravityChangeFunc = std::function<void(SessionGravity gravity)>;
using SessionChangeByActionNotifyManagerFunc = std::function<void(const sptr<SceneSession>& sceneSession,
    const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action)>;
using NotifyKeyboardLayoutAdjustFunc = std::function<void(const KeyboardLayoutParams& params)>;
using NotifyLayoutFullScreenChangeFunc = std::function<void(bool isLayoutFullScreen)>;
using SetSkipSelfWhenShowOnVirtualScreenCallback = std::function<void(uint64_t surfaceNodeId, bool isSkip)>;
using NotifyForceSplitFunc = std::function<AppForceLandscapeConfig(const std::string& bundleName)>;
using UpdatePrivateStateAndNotifyFunc = std::function<void(int32_t persistentId)>;
class SceneSession : public Session {
public:
    // callback for notify SceneSessionManager
    struct SpecificSessionCallback : public RefBase {
        SpecificSessionCreateCallback onCreate_;
        SpecificSessionDestroyCallback onDestroy_;
        ClearDisplayStatusBarTemporarilyFlags onClearDisplayStatusBarTemporarilyFlags_;
        CameraFloatSessionChangeCallback onCameraFloatSessionChange_;
        GetSceneSessionVectorByTypeCallback onGetSceneSessionVectorByType_;
        UpdateAvoidAreaCallback onUpdateAvoidArea_;
        NotifyWindowInfoUpdateCallback onWindowInfoUpdate_;
        NotifyWindowPidChangeCallback onWindowInputPidChangeCallback_;
        NotifySessionTouchOutsideCallback onSessionTouchOutside_;
        GetAINavigationBarArea onGetAINavigationBarArea_;
        OnOutsideDownEvent onOutsideDownEvent_;
        HandleSecureSessionShouldHideCallback onHandleSecureSessionShouldHide_;
        CameraSessionChangeCallback onCameraSessionChange_;
        SetSkipSelfWhenShowOnVirtualScreenCallback onSetSkipSelfWhenShowOnVirtualScreen_;
    };

    // callback for notify SceneBoard
    struct SessionChangeCallback : public RefBase {
        NotifyBindDialogSessionFunc onBindDialogTarget_;
        NotifySessionTopmostChangeFunc onSessionTopmostChange_;
        NotifyRaiseToTopFunc onRaiseToTop_;
        NotifySessionEventFunc OnSessionEvent_;
        NotifySystemBarPropertyChangeFunc OnSystemBarPropertyChange_;
        NotifyNeedAvoidFunc OnNeedAvoid_;
        NotifyIsCustomAnimationPlayingCallback onIsCustomAnimationPlaying_;
        NotifyWindowAnimationFlagChangeFunc onWindowAnimationFlagChange_;
        NotifyShowWhenLockedFunc OnShowWhenLocked_;
        NotifyReqOrientationChangeFunc OnRequestedOrientationChange_;
        NotifyRaiseAboveTargetFunc onRaiseAboveTarget_;
        NotifyForceHideChangeFunc OnForceHideChange_;
        NotifyTouchOutsideFunc OnTouchOutside_;
        ClearCallbackMapFunc clearCallbackFunc_;
        NotifyPrepareClosePiPSessionFunc onPrepareClosePiPSession_;
        NotifyLandscapeMultiWindowSessionFunc onSetLandscapeMultiWindowFunc_;
        NotifyLayoutFullScreenChangeFunc onLayoutFullScreenChangeFunc_;
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
    virtual WSError Reconnect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, sptr<WindowSessionProperty> property = nullptr,
        sptr<IRemoteObject> token = nullptr, int32_t pid = -1, int32_t uid = -1);
    WSError Foreground(sptr<WindowSessionProperty> property, bool isFromClient = false) override;
    WSError Background(bool isFromClient = false) override;
    virtual void RegisterBufferAvailableCallback(const SystemSessionBufferAvailableCallback& func) {};
    WSError BackgroundTask(const bool isSaveSnapshot = true);
    WSError Disconnect(bool isFromClient = false) override;
    void SetClientIdentityToken(const std::string& clientIdentityToken);
    virtual void BindKeyboardPanelSession(sptr<SceneSession> panelSession) {};
    virtual sptr<SceneSession> GetKeyboardPanelSession() const { return nullptr; };
    virtual void BindKeyboardSession(sptr<SceneSession> session) {};
    virtual sptr<SceneSession> GetKeyboardSession() const { return nullptr; };
    virtual SessionGravity GetKeyboardGravity() const { return SessionGravity::SESSION_GRAVITY_DEFAULT; };
    virtual void OnKeyboardPanelUpdated() {};

    WSError UpdateActiveStatus(bool isActive) override;
    WSError OnSessionEvent(SessionEvent event) override;
    WSError OnLayoutFullScreenChange(bool isLayoutFullScreen) override;
    WSError RaiseToAppTop() override;
    WSError UpdateSizeChangeReason(SizeChangeReason reason) override;
    virtual void OpenKeyboardSyncTransaction() {};
    virtual void CloseKeyboardSyncTransaction(const WSRect& keyboardPanelRect,
        bool isKeyboardShow, bool isRotating) {};
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) override;
    WSError UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason, bool isGlobal = false) override;
    WSError ChangeSessionVisibilityWithStatusBar(const sptr<AAFwk::SessionInfo> info, bool visible) override;
    WSError PendingSessionActivation(const sptr<AAFwk::SessionInfo> info) override;
    WSError TerminateSession(const sptr<AAFwk::SessionInfo> info) override;
    WSError NotifySessionException(
        const sptr<AAFwk::SessionInfo> info, bool needRemoveSession = false) override;
    WSError NotifySessionExceptionInner(
        const sptr<AAFwk::SessionInfo> info, bool needRemoveSession = false, bool isFromClient = false);
    WSError NotifyClientToUpdateRect(std::shared_ptr<RSTransaction> rsTransaction) override;
    WSError OnNeedAvoid(bool status) override;
    AvoidArea GetAvoidAreaByType(AvoidAreaType type) override;

    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent,
        bool needNotifyClient = true) override;
    WSError RequestSessionBack(bool needMoveToBackground) override;
    WSError SetAspectRatio(float ratio) override;
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
    WSError UpdatePiPRect(const Rect& rect, SizeChangeReason reason) override;
    WSError UpdatePiPControlStatus(WsPiPControlType controlType, WsPiPControlStatus status) override;
    void NotifyPiPWindowPrepareClose() override;
    void RequestHideKeyboard(bool isAppColdStart = false);
    WSError ProcessPointDownSession(int32_t posX, int32_t posY) override;
    WSError SendPointEventForMoveDrag(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    void NotifyOutsideDownEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    WSError NotifyFrameLayoutFinishFromApp(bool notifyListener, const WSRect& rect) override;
    void SetForegroundInteractiveStatus(bool interactive) override;
    WSError SetLandscapeMultiWindow(bool isLandscapeMultiWindow) override;

    WSError SetKeepScreenOn(bool keepScreenOn);
    void SetParentPersistentId(int32_t parentId);
    WSError SetTurnScreenOn(bool turnScreenOn);
    void SetPiPTemplateInfo(const PiPTemplateInfo& pipTemplateInfo);
    void SetPrivacyMode(bool isPrivacy);
    void SetSnapshotSkip(bool isSkip);
    void SetSystemSceneOcclusionAlpha(double alpha);
    void SetRequestedOrientation(Orientation orientation);
    void SetWindowAnimationFlag(bool needDefaultAnimationFlag);
    void SetCollaboratorType(int32_t collaboratorType);
    void SetLastSafeRect(WSRect rect);
    void SetOriPosYBeforeRaisedByKeyboard(int32_t posY);
    virtual WSError SetTopmost(bool topmost) { return WSError::WS_ERROR_INVALID_CALLING; }
    virtual bool IsTopmost() const { return false; }
    virtual bool IsModal() const { return false; }
    WSError SetSystemBarProperty(WindowType type, SystemBarProperty systemBarProperty);
    void SetIsStatusBarVisible(bool isVisible) { isStatusBarVisible_ = isVisible; }
    void SetAbilitySessionInfo(std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo);
    void SetWindowDragHotAreaListener(const NotifyWindowDragHotAreaFunc& func);
    void SetSessionEventParam(SessionEventParam param);
    void SetSessionRectChangeCallback(const NotifySessionRectChangeFunc& func);
    void SetKeyboardGravityChangeCallback(const NotifyKeyboardGravityChangeFunc& func);
    void SetAdjustKeyboardLayoutCallback(const NotifyKeyboardLayoutAdjustFunc& func);
    void SetSessionPiPControlStatusChangeCallback(const NotifySessionPiPControlStatusChangeFunc& func);
    void SetIsDisplayStatusBarTemporarily(bool isTemporary);
    void SetSkipDraw(bool skip);
    virtual void SetSkipSelfWhenShowOnVirtualScreen(bool isSkip);
    WMError SetUniqueDensityDpi(bool useUnique, float dpi);

    bool IsAnco() const override;
    void SetBlankFlag(bool isAddBlank) override;
    bool GetBlankFlag() const override;
    void SetBufferAvailableCallbackEnable(bool enable);
    bool GetBufferAvailableCallbackEnable() const override;
    int32_t GetCollaboratorType() const;
    WSRect GetLastSafeRect() const;
    WSRect GetSessionTargetRect() const;
    std::string GetUpdatedIconPath() const;
    std::string GetSessionSnapshotFilePath() const;
    int32_t GetParentPersistentId() const;
    int32_t GetMainSessionId();
    virtual int32_t GetMissionId() const { return persistentId_; };
    Orientation GetRequestedOrientation() const;
    std::vector<sptr<SceneSession>> GetSubSession() const;
    std::vector<sptr<SceneSession>> GetToastSession() const;
    std::shared_ptr<AppExecFwk::AbilityInfo> GetAbilityInfo() const;
    std::string GetWindowNameAllType() const;
    PiPTemplateInfo GetPiPTemplateInfo() const;
    SubWindowModalType GetSubWindowModalType() const;
    int32_t GetOriPosYBeforeRaisedByKeyboard() const;
    std::string GetClientIdentityToken() const;

    // Session recover
    bool IsRecovered() const { return isRecovered_; }
    void SetRecovered(bool isRecovered) { isRecovered_ = isRecovered; }

    bool IsVisible() const;
    bool IsDecorEnable() const;
    bool IsAppSession() const;
    bool IsTurnScreenOn() const;
    bool IsKeepScreenOn() const;
    bool IsShowWhenLocked() const;
    bool GetShowWhenLockedFlagValue() const;
    bool IsFloatingWindowAppType() const;
    bool IsNeedDefaultAnimation() const;
    bool IsDirtyWindow();
    void SetSystemTouchable(bool touchable) override;
    bool IsVisibleForAccessibility() const;
    bool GetIsDisplayStatusBarTemporarily() const;
    bool IsDeviceWakeupByApplication() const;
    void SetStartingWindowExitAnimationFlag(bool enable);
    bool NeedStartingWindowExitAnimation() const override;

    void NotifyWindowVisibility();
    WSError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) override;
    WSError OnShowWhenLocked(bool showWhenLocked);
    void SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap> &icon);
    void NotifyTouchOutside();
    bool CheckOutTouchOutsideRegister();
    void UpdateNativeVisibility(bool visible);
    void UpdateRotationAvoidArea();
    void DumpSessionElementInfo(const std::vector<std::string>& params);
    void NotifyForceHideChange(bool hide);
    WSError BindDialogSessionTarget(const sptr<SceneSession>& sceneSession);
    void DumpSessionInfo(std::vector<std::string> &info) const;
    bool AddSubSession(const sptr<SceneSession>& subSession);
    bool RemoveSubSession(int32_t persistentId);
    bool AddToastSession(const sptr<SceneSession>& toastSession);
    bool RemoveToastSession(int32_t persistentId);
    void NotifySessionForeground(uint32_t reason, bool withAnimation);
    void NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits);
    void RegisterSessionChangeCallback(const sptr<SceneSession::SessionChangeCallback>& sessionChangeCallback);
    void RegisterForceSplitListener(const NotifyForceSplitFunc& func);
    void SetUpdatePrivateStateAndNotifyFunc(const UpdatePrivateStateAndNotifyFunc& func);
    void ClearSpecificSessionCbMap();
    void SendPointerEventToUI(std::shared_ptr<MMI::PointerEvent> pointerEvent);
    bool SendKeyEventToUI(std::shared_ptr<MMI::KeyEvent> keyEvent, bool isPreImeEvent = false);
    bool IsStartMoving() const;
    void SetIsStartMoving(const bool startMoving);
    bool IsSystemSpecificSession() const;
    void SetIsSystemSpecificSession(bool isSystemSpecificSession);
    WSError SetPipActionEvent(const std::string& action, int32_t status);
    WSError SetPiPControlEvent(WsPiPControlType controlType, WsPiPControlStatus status);
    void SetShouldHideNonSecureWindows(bool shouldHide);
    void UpdateExtWindowFlags(int32_t extPersistentId, const ExtensionWindowFlags& extWindowFlags,
        const ExtensionWindowFlags& extWindowActions);
    ExtensionWindowFlags GetCombinedExtWindowFlags();
    void RemoveExtWindowFlags(int32_t extPersistentId);
    void ClearExtWindowFlags();
    void NotifyDisplayMove(DisplayId from, DisplayId to);
    void NotifySessionFullScreen(bool fullScreen);

    void SetSessionState(SessionState state) override;
    void UpdateSessionState(SessionState state) override;
    void SetForceHideState(ForceHideState forceHideState);
    ForceHideState GetForceHideState() const;
    bool IsTemporarilyShowWhenLocked() const;
    void SetTemporarilyShowWhenLocked(bool isTemporarilyShowWhenLocked);

    std::shared_ptr<PowerMgr::RunningLock> keepScreenLock_;

    static const wptr<SceneSession> GetEnterWindow();
    static void ClearEnterWindow();
    static MaximizeMode maximizeMode_;
    static uint32_t GetWindowDragHotAreaType(uint32_t type, int32_t pointerX, int32_t pointerY);
    static void AddOrUpdateWindowDragHotArea(uint32_t type, const WSRect& area);
    WSError UpdateRectChangeListenerRegistered(bool isRegister) override;
    int32_t GetCustomDecorHeight() override
    {
        return customDecorHeight_;
    }

    void SetCustomDecorHeight(int32_t height) override
    {
        if (height < MIN_DECOR_HEIGHT || height > MAX_DECOR_HEIGHT) {
            return;
        }
        customDecorHeight_ = height;
    }
    WMError UpdateSessionPropertyByAction(const sptr<WindowSessionProperty>& property,
        WSPropertyChangeAction action) override;
    void SetSessionChangeByActionNotifyManagerListener(const SessionChangeByActionNotifyManagerFunc& func);

    bool CheckGetAvoidAreaAvailable(AvoidAreaType type) override;
    void AddModalUIExtension(const ExtensionWindowEventInfo& extensionInfo);
    void RemoveModalUIExtension(int32_t persistentId);
    bool HasModalUIExtension();
    void UpdateModalUIExtension(const ExtensionWindowEventInfo& extensionInfo);
    ExtensionWindowEventInfo GetLastModalUIExtensionEventInfo();
    Vector2f GetPosition(bool useUIExtension);
    void AddUIExtSurfaceNodeId(uint64_t surfaceNodeId, int32_t persistentId);
    void RemoveUIExtSurfaceNodeId(int32_t persistentId);
    int32_t GetUIExtPersistentIdBySurfaceNodeId(uint64_t surfaceNodeId) const;
    bool IsFreeMultiWindowMode() const
    {
        return systemConfig_.IsFreeMultiWindowMode();
    }
    WMError GetAppForceLandscapeConfig(AppForceLandscapeConfig& config) override;
    void UnregisterSessionChangeListeners() override;
    bool IsPcOrPadEnableActivation() const;

protected:
    void NotifySessionRectChange(const WSRect& rect, const SizeChangeReason& reason = SizeChangeReason::UNDEFINED);
    void NotifyIsCustomAnimationPlaying(bool isPlaying);
    void SetMoveDragCallback();
    std::string GetRatioPreferenceKey();
    WSError NotifyClientToUpdateRectTask(std::shared_ptr<RSTransaction> rsTransaction);
    void MoveAndResizeKeyboard(const KeyboardLayoutParams& params,
        const sptr<WindowSessionProperty>& sessionProperty, bool isShow);
    bool GetScreenWidthAndHeightFromServer(const sptr<WindowSessionProperty>& sessionProperty,
        uint32_t &screenWidth, uint32_t &screenHeight);
    bool GetScreenWidthAndHeightFromClient(const sptr<WindowSessionProperty>& sessionProperty,
        uint32_t &screenWidth, uint32_t &screenHeight);
    std::string GetRectInfo(const WSRect& rect)
    {
        using std::to_string;
        return "[" + to_string(rect.width_) + ", " + to_string(rect.height_) + "; "
        + to_string(rect.posX_) + ", " + to_string(rect.posY_) + "]";
    }

    sptr<SpecificSessionCallback> specificCallback_ = nullptr;
    sptr<SessionChangeCallback> sessionChangeCallback_ = nullptr;
    sptr<MoveDragController> moveDragController_ = nullptr;
    sptr<SceneSession> keyboardPanelSession_ = nullptr;
    sptr<SceneSession> keyboardSession_ = nullptr;
    NotifyKeyboardGravityChangeFunc keyboardGravityChangeFunc_;
    NotifyKeyboardLayoutAdjustFunc adjustKeyboardLayoutFunc_;

private:
    void NotifyAccessibilityVisibilityChange();
    void CalculateAvoidAreaRect(WSRect& rect, WSRect& avoidRect, AvoidArea& avoidArea) const;
    void GetSystemAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void GetCutoutAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void GetKeyboardAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void CalculateCombinedExtWindowFlags();
    void GetAINavigationBarArea(WSRect rect, AvoidArea& avoidArea) const;
    void HandleStyleEvent(MMI::WindowArea area) override;
    WSError HandleEnterWinwdowArea(int32_t windowX, int32_t windowY);

#ifdef DEVICE_STATUS_ENABLE
    void RotateDragWindow(std::shared_ptr<RSTransaction> rsTransaction);
#endif // DEVICE_STATUS_ENABLE
    void OnMoveDragCallback(const SizeChangeReason& reason);
    void HandleCompatibleModeMoveDrag(WSRect& rect, const SizeChangeReason& reason,
        bool isSupportDragInPcCompatibleMode);
    void FixRectByLimits(WindowLimits limits, WSRect& rect, float ratio, bool isDecor, float vpr);
    bool FixRectByAspectRatio(WSRect& rect);
    bool SaveAspectRatio(float ratio);
    void NotifyPropertyWhenConnect();
    WSError RaiseAppMainWindowToTop() override;
    void SetSurfaceBounds(const WSRect &rect);
    void UpdateWinRectForSystemBar(WSRect& rect);
    bool IsKeyboardNeedLeftOffset(bool isPhone, const sptr<WindowSessionProperty>& sessionProperty);
    bool UpdateInputMethodSessionRect(const WSRect& rect, WSRect& newWinRect, WSRect& newRequestRect);
    bool IsMovableWindowType();
    bool IsFullScreenMovable();
    void HandleCastScreenConnection(SessionInfo& info, sptr<SceneSession> session);
    void UpdateSessionRectInner(const WSRect& rect, const SizeChangeReason& reason);
    void FixKeyboardPositionByKeyboardPanel(sptr<SceneSession> panelSession, sptr<SceneSession> keyboardSession);
    WMError HandleUpdatePropertyByAction(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateTurnScreenOn(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateKeepScreenOn(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateFocusable(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateTouchable(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateSetBrightness(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateOrientation(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdatePrivacyMode(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateSnapshotSkip(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateMaximizeState(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateOtherProps(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateStatusProps(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateNavigationProps(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateNavigationIndicatorProps(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateFlags(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateMode(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateAnimationFlag(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateTouchHotArea(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateDecorEnable(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateWindowLimits(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateDragenabled(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateRaiseenabled(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateHideNonSystemFloatingWindows(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateTextfieldAvoidInfo(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateWindowMask(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateTopmost(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError HandleActionUpdateModeSupportInfo(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    WMError ProcessUpdatePropertyByAction(const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession, WSPropertyChangeAction action);
    void HandleSpecificSystemBarProperty(WindowType type, const sptr<WindowSessionProperty>& property,
        const sptr<SceneSession>& sceneSession);
    void SetWindowFlags(const sptr<SceneSession>& sceneSession,
        const sptr<WindowSessionProperty>& property);
    void NotifySessionChangeByActionNotifyManager(const sptr<SceneSession>& sceneSession,
        const sptr<WindowSessionProperty>& property, WSPropertyChangeAction action);

    NotifySessionPiPControlStatusChangeFunc sessionPiPControlStatusChangeFunc_;
    NotifyForceSplitFunc forceSplitFunc_;
    UpdatePrivateStateAndNotifyFunc updatePrivateStateAndNotifyFunc_;
    static wptr<SceneSession> enterSession_;
    static std::mutex enterSessionMutex_;
    int32_t collaboratorType_ = CollaboratorType::DEFAULT_TYPE;
    mutable std::mutex sessionChangeCbMutex_;
    WSRect lastSafeRect = { 0, 0, 0, 0 };
    std::vector<sptr<SceneSession>> subSession_;
    std::vector<sptr<SceneSession>> toastSession_;
    std::atomic_bool isDeviceWakeupByApplication_ { false };
    std::atomic_bool needStartingWindowExitAnimation_ { true };
    bool needDefaultAnimationFlag_ = true;
    PiPTemplateInfo pipTemplateInfo_ = {0, 0, {}};
    SessionEventParam sessionEventParam_ = { 0, 0 };
    std::atomic_bool isStartMoving_ { false };
    std::atomic_bool isVisibleForAccessibility_ { true };
    std::atomic_bool isDisplayStatusBarTemporarily_ { false };
    bool isSystemSpecificSession_ { false };
    std::atomic_bool shouldHideNonSecureWindows_ { false };
    std::shared_mutex combinedExtWindowFlagsMutex_;
    ExtensionWindowFlags combinedExtWindowFlags_ { 0 };
    std::map<int32_t, ExtensionWindowFlags> extWindowFlagsMap_;
    int32_t customDecorHeight_ = 0;
    ForceHideState forceHideState_ { ForceHideState::NOT_HIDDEN };
    static std::shared_mutex windowDragHotAreaMutex_;
    std::string clientIdentityToken_ = { "" };
    static std::map<uint32_t, WSRect> windowDragHotAreaMap_;
    SessionChangeByActionNotifyManagerFunc sessionChangeByActionNotifyManagerFunc_;
    int32_t oriPosYBeforeRaisedByKeyboard_ = 0;
    std::atomic_bool isTemporarilyShowWhenLocked_ { false };
    std::shared_mutex modalUIExtensionInfoListMutex_;
    std::vector<ExtensionWindowEventInfo> modalUIExtensionInfoList_;
    mutable std::shared_mutex uiExtNodeIdToPersistentIdMapMutex_;
    std::map<uint64_t, int32_t> uiExtNodeIdToPersistentIdMap_;
    bool isAddBlank_ = false;
    bool bufferAvailableCallbackEnable_ = false;
    bool isStatusBarVisible_ = true;

    // Session recover
    bool isRecovered_ = false;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H
