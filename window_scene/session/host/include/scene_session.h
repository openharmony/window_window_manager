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

#include <memory>
#include <mutex>
#include <shared_mutex>

#include "iremote_object.h"
#include "platform/image_native/pixel_map.h"
#include "session/host/include/session.h"
#include "session/host/include/move_drag_controller.h"
#include "wm_common.h"

namespace OHOS::PowerMgr {
    class RunningLock;
}

namespace OHOS::Rosen {
namespace PARAM_KEY {
    const std::string PARAM_MISSION_AFFINITY_KEY = "ohos.anco.param.missionAffinity";
}
class SceneSession;

using SpecificSessionCreateCallback =
    std::function<sptr<SceneSession>(const SessionInfo& info, sptr<WindowSessionProperty> property)>;
using SpecificSessionDestroyCallback = std::function<WSError(const int32_t& persistentId)>;
using CameraFloatSessionChangeCallback = std::function<void(uint32_t accessTokenId, bool isShowing)>;
using GetSceneSessionVectorByTypeCallback = std::function<std::vector<sptr<SceneSession>>(WindowType type)>;
using UpdateAvoidAreaCallback = std::function<void(const int32_t& persistentId)>;
using NotifyWindowInfoUpdateCallback = std::function<void(int32_t persistentId, WindowUpdateType type)>;
using NotifySessionTouchOutsideCallback = std::function<void(int32_t persistentId)>;
using GetAINavigationBarArea = std::function<WSRect()>;
using RecoveryCallback = std::function<void(int32_t persistentId, Rect rect)>;
using NotifyBindDialogSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifySessionRectChangeFunc = std::function<void(const WSRect& rect, const SizeChangeReason& reason)>;
using NotifySessionEventFunc = std::function<void(int32_t eventId)>;
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
class SceneSession : public Session {
public:
    // callback for notify SceneSessionManager
    struct SpecificSessionCallback : public RefBase {
        SpecificSessionCreateCallback onCreate_;
        SpecificSessionDestroyCallback onDestroy_;
        CameraFloatSessionChangeCallback onCameraFloatSessionChange_;
        GetSceneSessionVectorByTypeCallback onGetSceneSessionVectorByType_;
        UpdateAvoidAreaCallback onUpdateAvoidArea_;
        NotifyWindowInfoUpdateCallback onWindowInfoUpdate_;
        NotifySessionTouchOutsideCallback onSessionTouchOutside_;
        GetAINavigationBarArea onGetAINavigationBarArea_;
        RecoveryCallback onRecoveryPullPiPMainWindow_;
    };

    // callback for notify SceneBoard
    struct SessionChangeCallback : public RefBase {
        NotifyBindDialogSessionFunc onBindDialogTarget_;
        NotifySessionRectChangeFunc onRectChange_;
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
        int32_t pid = -1, int32_t uid = -1) override;
    WSError Reconnect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr, int32_t pid = -1,
        int32_t uid = -1);
    WSError Foreground(sptr<WindowSessionProperty> property) override;
    WSError Background() override;
    WSError Disconnect() override;

    WSError UpdateActiveStatus(bool isActive) override;
    WSError OnSessionEvent(SessionEvent event) override;
    WSError RaiseToAppTop() override;
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason,
        const std::shared_ptr<RSTransaction>& rsTransaction = nullptr) override;
    WSError UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason) override;
    WSError PendingSessionActivation(const sptr<AAFwk::SessionInfo> info) override;
    WSError TerminateSession(const sptr<AAFwk::SessionInfo> info) override;
    WSError NotifySessionException(const sptr<AAFwk::SessionInfo> info) override;
    WSError NotifyClientToUpdateRect(std::shared_ptr<RSTransaction> rsTransaction) override;
    WSError OnNeedAvoid(bool status) override;
    AvoidArea GetAvoidAreaByType(AvoidAreaType type) override;
    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    WSError RequestSessionBack(bool needMoveToBackground) override;
    WSError SetAspectRatio(float ratio) override;
    WSError SetGlobalMaximizeMode(MaximizeMode mode) override;
    WSError GetGlobalMaximizeMode(MaximizeMode& mode) override;
    WSError UpdateWindowSceneAfterCustomAnimation(bool isAdd) override;
    WSError UpdateWindowAnimationFlag(bool needDefaultAnimationFlag) override;
    void SetZOrder(uint32_t zOrder) override;
    std::vector<Rect> GetTouchHotAreas() const override;
    void SetFloatingScale(float floatingScale) override;
    WSError RaiseAboveTarget(int32_t subWindowId) override;
    WSError SetTextFieldAvoidInfo(double textFieldPositionY, double textFieldHeight) override;
    WSError UpdatePiPRect(uint32_t width, uint32_t height, PiPRectUpdateReason reason) override;
    void NotifyPiPWindowPrepareClose() override;
    WSError RecoveryPullPiPMainWindow(int32_t persistentId, const Rect& rect) override;
    void SetScale(float scaleX, float scaleY, float pivotX, float pivotY) override;
    void RequestHideKeyboard();

    WSError SetKeepScreenOn(bool keepScreenOn);
    void SetParentPersistentId(int32_t parentId);
    WSError SetTurnScreenOn(bool turnScreenOn);
    void SetPrivacyMode(bool isPrivacy);
    void SetSystemSceneOcclusionAlpha(double alpha);
    void SetRequestedOrientation(Orientation orientation);
    void SetWindowAnimationFlag(bool needDefaultAnimationFlag);
    void SetCollaboratorType(int32_t collaboratorType);
    void SetSelfToken(sptr<IRemoteObject> selfToken);
    void SetLastSafeRect(WSRect rect);
    WSError SetSystemBarProperty(WindowType type, SystemBarProperty systemBarProperty);
    void SetAbilitySessionInfo(std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo);
    void SetWindowDragHotAreaListener(const NotifyWindowDragHotAreaFunc& func);
    void SetSessionRectChangeCallback(const NotifySessionRectChangeFunc& func);

    int32_t GetCollaboratorType() const;
    sptr<IRemoteObject> GetSelfToken() const;
    WSRect GetLastSafeRect() const;
    WSRect GetSessionTargetRect() const;
    std::string GetUpdatedIconPath() const;
    std::string GetSessionSnapshotFilePath() const;
    int32_t GetParentPersistentId() const;
    const std::string& GetWindowName() const;
    Orientation GetRequestedOrientation() const;
    void GetAINavigationBarArea(WSRect rect, AvoidArea& avoidArea) const;
    std::vector<sptr<SceneSession>> GetSubSession() const;
    std::shared_ptr<AppExecFwk::AbilityInfo> GetAbilityInfo() const;

    bool IsVisible() const;
    bool IsDecorEnable() const;
    bool IsAppSession() const;
    bool IsTurnScreenOn() const;
    bool IsKeepScreenOn() const;
    bool IsShowWhenLocked() const;
    bool IsFloatingWindowAppType() const;
    bool IsNeedDefaultAnimation() const;
    bool IsDirtyWindow();
    void NotifyUILostFocus() override;

    WSError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type);
    WSError OnShowWhenLocked(bool showWhenLocked);
    void SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap> &icon);
    void NotifyTouchOutside();
    void NotifyWindowVisibility();
    bool CheckOutTouchOutsideRegister();
    void UpdateNativeVisibility(bool visible);
    void UpdateRotationAvoidArea();
    void DumpSessionElementInfo(const std::vector<std::string>& params);
    void NotifyForceHideChange(bool hide);
    WSError BindDialogSessionTarget(const sptr<SceneSession>& sceneSession);
    void DumpSessionInfo(std::vector<std::string> &info) const;
    bool AddSubSession(const sptr<SceneSession>& subSession);
    bool RemoveSubSession(int32_t persistentId);
    void NotifySessionForeground(uint32_t reason, bool withAnimation);
    void NotifySessionBackground(uint32_t reason, bool withAnimation, bool isFromInnerkits);
    void RegisterSessionChangeCallback(const sptr<SceneSession::SessionChangeCallback>& sessionChangeCallback);
    WSError UpdateSizeChangeReason(SizeChangeReason reason);
    void ClearSpecificSessionCbMap();

    double textFieldPositionY_ = 0.0;
    double textFieldHeight_ = 0.0;
    std::shared_ptr<PowerMgr::RunningLock> keepScreenLock_;

    static const wptr<SceneSession> GetEnterWindow();
    static void ClearEnterWindow();
    static MaximizeMode maximizeMode_;
    static std::map<int32_t, WSRect> windowDragHotAreaMap_;

protected:
    void NotifyIsCustomAnimationPlaying(bool isPlaying);
    void SetMoveDragCallback();
    std::string GetRatioPreferenceKey();

    std::string GetRectInfo(const WSRect& rect)
    {
        using std::to_string;
        return "[" + to_string(rect.width_) + ", " + to_string(rect.height_) + "; "
        + to_string(rect.posX_) + ", " + to_string(rect.posY_) + "]";
    }

    sptr<SpecificSessionCallback> specificCallback_ = nullptr;
    sptr<SessionChangeCallback> sessionChangeCallback_ = nullptr;
    sptr<MoveDragController> moveDragController_ = nullptr;

private:
    void CalculateAvoidAreaRect(WSRect& rect, WSRect& avoidRect, AvoidArea& avoidArea) const;
    void GetSystemAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void GetCutoutAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void GetKeyboardAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void HandleStyleEvent(MMI::WindowArea area) override;
    WSError HandleEnterWinwdowArea(int32_t windowX, int32_t windowY);
    WSError HandlePointerStyle(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    void NotifySessionRectChange(const WSRect& rect, const SizeChangeReason& reason = SizeChangeReason::UNDEFINED);
    void OnMoveDragCallback(const SizeChangeReason& reason);
    void FixRectByLimits(WindowLimits limits, WSRect& rect, float ratio, bool isDecor, float vpr);
    bool FixRectByAspectRatio(WSRect& rect);
    bool SaveAspectRatio(float ratio);
    void NotifyPropertyWhenConnect();
    WSError RaiseAppMainWindowToTop() override;
    void SetSurfaceBounds(const WSRect &rect);
    void UpdateWinRectForSystemBar(WSRect& rect);
    bool UpdateInputMethodSessionRect(const WSRect& rect, WSRect& newWinRect, WSRect& newRequestRect);
    void OnPiPMoveCallback(const WSRect& rect, const SizeChangeReason& reason);
    bool InitPiPRectInfo();
    void ClearPiPRectPivotInfo();
    void SavePiPRectInfo();
    void GetNewPiPRect(const uint32_t displayWidth, const uint32_t displayHeight, Rect& rect);
    void ProcessUpdatePiPRect(SizeChangeReason reason);

    NotifySessionRectChangeFunc sessionRectChangeFunc_;
    bool isVisible_ = false;
    static wptr<SceneSession> enterSession_;
    static std::mutex enterSessionMutex_;
    mutable std::mutex sessionChangeCbMutex_;
    int32_t collaboratorType_ = CollaboratorType::DEFAULT_TYPE;
    sptr<IRemoteObject> selfToken_ = nullptr;
    WSRect lastSafeRect = { 0, 0, 0, 0 };
    std::vector<sptr<SceneSession>> subSession_;
    bool needDefaultAnimationFlag_ = true;
    PiPRectInfo pipRectInfo_;
    SizeChangeReason reason_ = SizeChangeReason::UNDEFINED;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H
