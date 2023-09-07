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

#include "iremote_object.h"
#include "platform/image_native/pixel_map.h"
#include "session/host/include/session.h"
#include "session/host/include/move_drag_controller.h"
#include "wm_common.h"

namespace OHOS::PowerMgr {
    class RunningLock;
}

namespace OHOS::Rosen {
class SceneSession;

using SpecificSessionCreateCallback = std::function<sptr<SceneSession>(const SessionInfo& info, sptr<WindowSessionProperty> property)>;
using SpecificSessionDestroyCallback = std::function<WSError(const int32_t& persistentId)>;
using CameraFloatSessionChangeCallback = std::function<void(uint32_t accessTokenId, bool isShowing)>;
using GetSceneSessionVectorByTypeCallback = std::function<std::vector<sptr<SceneSession>>(WindowType type)>;
using UpdateAvoidAreaCallback = std::function<void(const int32_t& persistentId)>;
using NotifyWindowInfoUpdateCallback = std::function<void(int32_t persistentId, WindowUpdateType type)>;
using NotifySessionTouchOutsideCallback = std::function<void(int32_t persistentId)>;

using NotifyCreateSpecificSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifyBindDialogSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifySessionRectChangeFunc = std::function<void(const WSRect& rect, const SizeChangeReason& reason)>;
using NotifySessionEventFunc = std::function<void(int32_t eventId)>;
using NotifyRaiseToTopFunc = std::function<void()>;
using NotifyRaiseToTopForPointDownFunc = std::function<void()>;
using SetWindowPatternOpacityFunc = std::function<void(float opacity)>;
using NotifyIsCustomAnimationPlayingCallback = std::function<void(bool isFinish)>;
using NotifySystemBarPropertyChangeFunc = std::function<void(
    const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)>;
using NotifyNeedAvoidFunc = std::function<void(bool status)>;
using NotifyWindowAnimationFlagChangeFunc = std::function<void(const bool flag)>;
using NotifyShowWhenLockedFunc = std::function<void(bool showWhenLocked)>;
using NotifyReqOrientationChangeFunc = std::function<void(uint32_t orientation)>;
using NotifyRaiseAboveTargetFunc = std::function<void(int32_t subWindowId)>;
using NotifyForceHideChangeFunc = std::function<void(bool hide)>;
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
    };

    // callback for notify SceneBoard
    struct SessionChangeCallback : public RefBase {
        NotifyCreateSpecificSessionFunc onCreateSpecificSession_;
        NotifyBindDialogSessionFunc onBindDialogTarget_;
        NotifySessionRectChangeFunc onRectChange_;
        NotifyRaiseToTopFunc onRaiseToTop_;
        NotifyRaiseToTopForPointDownFunc onRaiseToTopForPointDown_;
        NotifySessionEventFunc OnSessionEvent_;
        NotifySystemBarPropertyChangeFunc OnSystemBarPropertyChange_;
        NotifyNeedAvoidFunc OnNeedAvoid_;
        NotifyWindowAnimationFlagChangeFunc onWindowAnimationFlagChange_;
        NotifyIsCustomAnimationPlayingCallback onIsCustomAnimationPlaying_;
        NotifyShowWhenLockedFunc OnShowWhenLocked_;
        NotifyReqOrientationChangeFunc OnRequestedOrientationChange_;
        NotifyRaiseAboveTargetFunc onRaiseAboveTarget_;
        NotifyForceHideChangeFunc OnForceHideChange_;
    };

    // func for change window scene pattern property
    struct SetWindowScenePatternFunc : public RefBase {
        SetWindowPatternOpacityFunc setOpacityFunc_;
    };

    SceneSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback);
    virtual ~SceneSession() = default;

    WSError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr,
        int32_t pid = -1, int32_t uid = -1) override;
    WSError Foreground(sptr<WindowSessionProperty> property) override;
    WSError Background() override;
    WSError Disconnect() override;

    WSError UpdateActiveStatus(bool isActive) override;
    WSError OnSessionEvent(SessionEvent event) override;
    WSError RaiseToAppTop() override;
    WSError RaiseToAppTopForPointDown();
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason) override;
    WSError UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason) override;
    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
        sptr<IRemoteObject> token = nullptr) override;
    WSError DestroyAndDisconnectSpecificSession(const int32_t& persistentId) override;
    WSError PendingSessionActivation(const sptr<AAFwk::SessionInfo> info) override;
    WSError TerminateSession(const sptr<AAFwk::SessionInfo> info) override;
    WSError NotifySessionException(const sptr<AAFwk::SessionInfo> info) override;

    WSError SetSystemBarProperty(WindowType type, SystemBarProperty systemBarProperty);
    WSError OnNeedAvoid(bool status) override;
    void CalculateAvoidAreaRect(WSRect& rect, WSRect& avoidRect, AvoidArea& avoidArea);
    void GetSystemAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void GetKeyboardAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    void GetCutoutAvoidArea(WSRect& rect, AvoidArea& avoidArea);
    AvoidArea GetAvoidAreaByType(AvoidAreaType type) override;
    WSError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type);
    WSError OnShowWhenLocked(bool showWhenLocked);
    bool IsShowWhenLocked() const;
    void RegisterSessionChangeCallback(const sptr<SceneSession::SessionChangeCallback>& sessionChangeCallback);

    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override;
    WSError RequestSessionBack(bool needMoveToBackground) override;

    WSError SetAspectRatio(float ratio) override;
    WSError SetGlobalMaximizeMode(MaximizeMode mode) override;
    WSError GetGlobalMaximizeMode(MaximizeMode& mode) override;

    std::string GetSessionSnapshotFilePath();
    void SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap> &icon);
    std::string GetUpdatedIconPath();
    void RegisterSetWindowPatternFunc(sptr<SetWindowScenePatternFunc> func)
    {
        setWindowScenePatternFunc_ = func;
    };
    WSError UpdateWindowSceneAfterCustomAnimation(bool isAdd) override;
    WSError UpdateWindowAnimationFlag(bool needDefaultAnimationFlag) override;
    void SetZOrder(uint32_t zOrder) override;
    std::vector<Rect> GetTouchHotAreas() const override;
    Rect GetHotAreaRect(int32_t action);
    WSError NotifyTouchOutside();

    static MaximizeMode maximizeMode_;

    void SetParentPersistentId(int32_t parentId);
    int32_t GetParentPersistentId() const;
    WSError SetTurnScreenOn(bool turnScreenOn);
    bool IsTurnScreenOn() const;
    WSError SetKeepScreenOn(bool keepScreenOn);
    bool IsKeepScreenOn() const;
    const std::string& GetWindowName() const;
    bool IsDecorEnable();
    void UpdateNativeVisibility(bool visible);
    void SetPrivacyMode(bool isPrivacy);
    bool IsVisible() const;
    bool IsFloatingWindowAppType() const;
    void DumpSessionElementInfo(const std::vector<std::string>& params);
    static const wptr<SceneSession> GetEnterWindow();
    static void ClearEnterWindow();
    void SetRequestedOrientation(Orientation orientation);
    void NotifyForceHideChange(bool hide);
    Orientation GetRequestedOrientation() const;
    WSError BindDialogTarget(const sptr<SceneSession>& sceneSession);
    void DumpSessionInfo(std::vector<std::string> &info) const;

    std::shared_ptr<PowerMgr::RunningLock> keepScreenLock_;

    int32_t GetCollaboratorType() const;
    void SetCollaboratorType(int32_t collaboratorType);
    std::shared_ptr<AppExecFwk::AbilityInfo> GetAbilityInfo();
    void SetAbilitySessionInfo(std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo);
    void SetSelfToken(sptr<IRemoteObject> selfToken);
    sptr<IRemoteObject> GetSelfToken();
    WSError RaiseAboveTarget(int32_t subWindowId) override;
    void SetSessionRectChangeCallback(const NotifySessionRectChangeFunc& func);
private:
    void HandleStyleEvent(MMI::WindowArea area) override;
    WSError HandleEnterWinwdowArea(int32_t windowX, int32_t windowY);
    WSError HandlePointerStyle(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);

    void UpdateCameraFloatWindowStatus(bool isShowing);
    void NotifySessionRectChange(const WSRect& rect, const SizeChangeReason& reason = SizeChangeReason::UNDEFINED);
    void SetMoveDragCallback();
    void OnMoveDragCallback(const SizeChangeReason& reason);
    bool FixRectByAspectRatio(WSRect& rect);
    std::string GetRatioPreferenceKey();
    bool SaveAspectRatio(float ratio);
    void NotifyIsCustomAnimationPlaying(bool isPlaying);
    void NotifyPropertyWhenConnect();
    void SetSurfaceBounds(const WSRect& rect);
    void UpdateWinRectForSystemBar(WSRect& rect);
    bool UpdateInputMethodSessionRect(const WSRect& rect, WSRect& newWinRect, WSRect& newRequestRect);

    sptr<SpecificSessionCallback> specificCallback_ = nullptr;
    sptr<SessionChangeCallback> sessionChangeCallback_ = nullptr;
    sptr<MoveDragController> moveDragController_ = nullptr;
    sptr<SetWindowScenePatternFunc> setWindowScenePatternFunc_ = nullptr;
    NotifySessionRectChangeFunc sessionRectChangeFunc_;
    bool isVisible_ = false;
    static wptr<SceneSession> enterSession_;
    static std::mutex enterSessionMutex_;
    mutable std::mutex sessionChangeCbMutex_;
    int32_t collaboratorType_ = CollaboratorType::DEFAULT_TYPE;
    sptr<IRemoteObject> selfToken_ = nullptr;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H
