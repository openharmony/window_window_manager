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

#include <mutex>

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
using UpdateAvoidAreaCallback = std::function<bool(const int32_t& persistentId)>;
using NotifyWindowInfoUpdateCallback = std::function<void(int32_t persistentId, WindowUpdateType type)>;

using NotifyCreateSpecificSessionFunc = std::function<void(const sptr<SceneSession>& session)>;
using NotifySessionRectChangeFunc = std::function<void(const WSRect& rect)>;
using NotifySessionEventFunc = std::function<void(int32_t eventId)>;
using NotifyRaiseToTopFunc = std::function<void()>;
using SetWindowPatternOpacityFunc = std::function<void(float opacity)>;
using NotifyIsCustomAnimationPlayingCallback = std::function<void(bool isFinish)>;
using NotifySystemBarPropertyChangeFunc = std::function<void(
    const std::unordered_map<WindowType, SystemBarProperty>& propertyMap)>;
using NotifyNeedAvoidFunc = std::function<void(bool status)>;
using NotifyWindowAnimationFlagChangeFunc = std::function<void(const bool flag)>;
using NotifyShowWhenLockedFunc = std::function<void(bool showWhenLocked)>;
using NotifyReqOrientationChangeFunc = std::function<void(uint32_t orientation)>;
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
    };

    // callback for notify SceneBoard
    struct SessionChangeCallback : public RefBase {
        NotifyCreateSpecificSessionFunc onCreateSpecificSession_;
        NotifySessionRectChangeFunc onRectChange_;
        NotifyRaiseToTopFunc onRaiseToTop_;
        NotifySessionEventFunc OnSessionEvent_;
        NotifySystemBarPropertyChangeFunc OnSystemBarPropertyChange_;
        NotifyNeedAvoidFunc OnNeedAvoid_;
        NotifyWindowAnimationFlagChangeFunc onWindowAnimationFlagChange_;
        NotifyIsCustomAnimationPlayingCallback onIsCustomAnimationPlaying_;
        NotifyShowWhenLockedFunc OnShowWhenLocked_;
        NotifyReqOrientationChangeFunc OnRequestedOrientationChange_;
    };

    // func for change window scene pattern property
    struct SetWindowScenePatternFunc : public RefBase {
        SetWindowPatternOpacityFunc setOpacityFunc_;
    };

    SceneSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback);
    ~SceneSession() = default;

    WSError Foreground(sptr<WindowSessionProperty> property) override;
    WSError Background() override;
    WSError UpdateWindowAnimationFlag(bool needDefaultAnimationFlag) override;

    WSError OnSessionEvent(SessionEvent event) override;
    WSError RaiseToAppTop() override;
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason) override;
    WSError UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason) override;
    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
        sptr<IRemoteObject> token = nullptr) override;
    WSError DestroyAndDisconnectSpecificSession(const int32_t& persistentId) override;
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
    WSError SetAspectRatio(float ratio) override;
    WSError SetGlobalMaximizeMode(MaximizeMode mode) override;
    WSError GetGlobalMaximizeMode(MaximizeMode& mode) override;
    std::string GetSessionSnapshotFilePath();
    void RegisterSetWindowPatternFunc(sptr<SetWindowScenePatternFunc> func)
    {
        setWindowScenePatternFunc_ = func;
    };
    WSError UpdateWindowSceneAfterCustomAnimation(bool isAdd) override;
    void SetZOrder(uint32_t zOrder) override;
    std::vector<Rect> GetTouchHotAreas() const override;
    Rect GetHotAreaRect(int32_t action);
    WSError NotifyTouchOutside();

    static MaximizeMode maximizeMode_;

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
    Orientation GetRequestedOrientation() const;

    std::shared_ptr<PowerMgr::RunningLock> keepScreenLock_;
private:
    void UpdateCameraFloatWindowStatus(bool isShowing);
    void NotifySessionRectChange(const WSRect& rect);
    void SetSessionRectChangeCallback();
    void OnSessionRectChange();
    bool FixRectByAspectRatio(WSRect& rect);
    std::string GetRatioPreferenceKey();
    bool SaveAspectRatio(float ratio);
    void NotifyIsCustomAnimatiomPlaying(bool isPlaying);
    sptr<SpecificSessionCallback> specificCallback_ = nullptr;
    sptr<SessionChangeCallback> sessionChangeCallback_ = nullptr;
    sptr<MoveDragController> moveDragController_ = nullptr;
    sptr<SetWindowScenePatternFunc> setWindowScenePatternFunc_ = nullptr;
    bool isVisible_ = false;
    static wptr<SceneSession> enterSession_;
    static std::mutex enterSessionMutex_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_SESSION_H
