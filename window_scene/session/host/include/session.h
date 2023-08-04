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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_H

#include <mutex>
#include <set>
#include <string>
#include <vector>
#include <refbase.h>

#include "interfaces/include/ws_common.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "session/host/include/zidl/session_stub.h"
#include "session/host/include/scene_persistence.h"
#include "wm_common.h"
#include "occupied_area_change_info.h"
#include <ability_info.h>

namespace OHOS::MMI {
class PointerEvent;
class KeyEvent;
class AxisEvent;
} // namespace OHOS::MMI

namespace OHOS::Media {
class PixelMap;
} // namespace OHOS::Media

namespace OHOS::Rosen {
class RSSurfaceNode;
using NotifyPendingSessionActivationFunc = std::function<void(SessionInfo& info)>;
using NotifySessionStateChangeFunc = std::function<void(const SessionState& state)>;
using NotifySessionStateChangeNotifyManagerFunc = std::function<void(int32_t persistentId, const SessionState& state)>;
using NotifyBackPressedFunc = std::function<void()>;
using NotifySessionFocusableChangeFunc = std::function<void(const bool isFocusable)>;
using NotifySessionTouchableChangeFunc = std::function<void(const bool touchable)>;
using NotifyClickFunc = std::function<void()>;
using NotifyTerminateSessionFunc = std::function<void(const SessionInfo& info)>;
using NotifyTerminateSessionFuncNew = std::function<void(const SessionInfo& info, bool needStartCaller)>;
using NotifySessionExceptionFunc = std::function<void(const SessionInfo& info)>;
using NotifyPendingSessionToForegroundFunc = std::function<void(const SessionInfo& info)>;
using NotifyPendingSessionToBackgroundForDelegatorFunc = std::function<void(const SessionInfo& info)>;
using NotifyCallingSessionForegroundFunc = std::function<void(const int32_t& persistentId)>;
using NotifyCallingSessionBackgroundFunc = std::function<void()>;

class ILifecycleListener {
public:
    virtual void OnConnect() = 0;
    virtual void OnForeground() = 0;
    virtual void OnBackground() = 0;
    virtual void OnDisconnect() = 0;
    virtual void OnExtensionDied() = 0;
};

class Session : public SessionStub, public virtual RefBase {
public:
    explicit Session(const SessionInfo& info) : sessionInfo_(info) {}
    virtual ~Session() = default;

    int32_t GetPersistentId() const;
    int32_t GetParentPersistentId() const;
    void SetParentPersistentId(int32_t parentId);
    void SetSessionRect(const WSRect& rect);

    std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const;
    std::shared_ptr<Media::PixelMap> Snapshot();
    SessionState GetSessionState() const;
    SessionInfo& GetSessionInfo();
    sptr<WindowSessionProperty> GetSessionProperty() const;
    WSRect GetSessionRect() const;
    WindowType GetWindowType() const;
    float GetAspectRatio() const;
    WSError SetAspectRatio(float ratio) override;

    void SetWindowSessionProperty(const sptr<WindowSessionProperty>& property);
    sptr<WindowSessionProperty> GetWindowSessionProperty() const;

    virtual WSError SetActive(bool active);
    virtual WSError UpdateRect(const WSRect& rect, SizeChangeReason reason);

    void SetShowRecent(bool showRecent);
    bool GetShowRecent() const;
    void SetBufferAvailable(bool bufferAvailable);
    bool GetBufferAvailable() const;

    WSError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr) override;
    WSError ConnectImpl(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr);
    WSError Foreground(sptr<WindowSessionProperty> property) override;
    WSError Background() override;
    WSError Disconnect() override;

    WSError OnSessionEvent(SessionEvent event) override;
    WSError UpdateWindowSessionProperty(sptr<WindowSessionProperty> property) override;
    WSError OnNeedAvoid(bool status) override;
    WSError TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want) override;
    WSError TransferExtensionData(const AAFwk::WantParams& wantParams) override;
    void NotifyRemoteReady() override;
    void NotifyExtensionDied() override;
    void NotifyConnect();
    void NotifyForeground();
    void NotifyBackground();
    void NotifyDisconnect();

    virtual WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    virtual WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent);
    WSError TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed);
    WSError TransferFocusActiveEvent(bool isFocusActive);
    WSError TransferFocusStateEvent(bool focusState);

    bool RegisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener);
    bool UnregisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener);
    void SetPendingSessionActivationEventListener(const NotifyPendingSessionActivationFunc& func);

    WSError PendingSessionActivation(const sptr<AAFwk::SessionInfo> info) override;

    WSError TerminateSession(const sptr<AAFwk::SessionInfo> info) override;
    void SetTerminateSessionListener(const NotifyTerminateSessionFunc& func);
    WSError TerminateSessionNew(const sptr<AAFwk::SessionInfo> info, bool needStartCaller);
    void SetTerminateSessionListenerNew(const NotifyTerminateSessionFuncNew& func);
    void SetSessionExceptionListener(const NotifySessionExceptionFunc& func);
    WSError NotifySessionException(const sptr<AAFwk::SessionInfo> info) override;
    void SetSessionStateChangeListenser(const NotifySessionStateChangeFunc& func);
    void SetSessionStateChangeNotifyManagerListener(const NotifySessionStateChangeNotifyManagerFunc& func);
    void NotifySessionStateChange(const SessionState& state);
    WSError UpdateActiveStatus(bool isActive) override; // update active status from session_stage
    WSError RaiseToAppTop() override;
    WSError UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason) override;
    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
        sptr<IRemoteObject> token = nullptr) override;
    WSError DestroyAndDisconnectSpecificSession(const int32_t& persistentId) override;
    void SetSystemConfig(const SystemSessionConfig& systemConfig);
    void SetBackPressedListenser(const NotifyBackPressedFunc& func);
    WSError ProcessBackEvent(); // send back event to session_stage
    WSError RequestSessionBack() override; // receive back request from session_stage
    WSError MarkProcessed(int32_t eventId) override;

    sptr<ScenePersistence> GetScenePersistence() const;
    void SetSessionContinueState(const ContinueState& continueState);
    void SetParentSession(const sptr<Session>& session);
    void BindDialogToParentSession(const sptr<Session>& session);
    void RemoveDialogToParentSession(const sptr<Session>& session);
    std::vector<sptr<Session>> GetDialogVector() const;
    void NotifyTouchDialogTarget();
    WSError NotifyDestroy();

    void SetPendingSessionToForegroundListener(const NotifyPendingSessionToForegroundFunc& func);
    WSError PendingSessionToForeground();
    void SetPendingSessionToBackgroundForDelegatorListener(const NotifyPendingSessionToBackgroundForDelegatorFunc& func);
    WSError PendingSessionToBackgroundForDelegator();

    void SetSessionFocusableChangeListener(const NotifySessionFocusableChangeFunc& func);
    void SetSessionTouchableChangeListener(const NotifySessionTouchableChangeFunc& func);
    void SetClickListener(const NotifyClickFunc& func);
    void NotifySessionFocusableChange(bool isFocusable);
    void NotifySessionTouchableChange(bool touchable);
    void NotifyClick();
    WSError UpdateFocus(bool isFocused);
    WSError SetFocusable(bool isFocusable);
    bool NeedNotify() const;
    void SetNeedNotify(bool needNotify);
    bool GetFocusable() const;
    WSError SetTouchable(bool touchable);
    bool GetTouchable() const;
    WSError SetVisible(bool isVisible);
    bool GetVisible() const;
    WSError SetGlobalMaximizeMode(MaximizeMode mode) override;
    WSError GetGlobalMaximizeMode(MaximizeMode& mode) override;
    AvoidArea GetAvoidAreaByType(AvoidAreaType type) override;
    WSError SetBrightness(float brightness);
    float GetBrightness() const;
    void NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info);
    void SetRequestedOrientation(Orientation orientation);
    Orientation GetRequestedOrientation() const;

    bool IsSessionValid() const;
    bool IsActive() const;

    sptr<IRemoteObject> dialogTargetToken_ = nullptr;
    int32_t GetWindowId() const;
    void SetCallingPid(int32_t id);
    void SetCallingUid(int32_t id);
    int32_t GetCallingPid() const;
    int32_t GetCallingUid() const;
    void SetAbilityToken(sptr<IRemoteObject> token);
    sptr<IRemoteObject> GetAbilityToken() const;
    WindowMode GetWindowMode();
    virtual void SetZOrder(uint32_t zOrder);
    uint32_t GetZOrder();
    WSError UpdateWindowAnimationFlag(bool needDefaultAnimationFlag) override;
    WSError UpdateWindowSceneAfterCustomAnimation(bool isAdd) override;

    void SetNotifyCallingSessionForegroundFunc(const NotifyCallingSessionForegroundFunc& func);
    void NotifyCallingSessionForeground();
    void SetNotifyCallingSessionBackgroundFunc(const NotifyCallingSessionBackgroundFunc& func);
    void NotifyCallingSessionBackground();
    void NotifyScreenshot();
    virtual std::vector<Rect> GetTouchHotAreas() const
    {
        return std::vector<Rect>();
    }

protected:
    void GeneratePersistentId(const bool isExtension, const SessionInfo& sessionInfo);
    void UpdateSessionState(SessionState state);
    void UpdateSessionFocusable(bool isFocusable);
    void UpdateSessionTouchable(bool touchable);

    bool isActive_ = false;
    bool isFocused_ = false;
    float aspectRatio_ = 0.0f;
    WSRect winRect_;
    sptr<ISessionStage> sessionStage_;
    SessionInfo sessionInfo_;
    NotifyPendingSessionActivationFunc pendingSessionActivationFunc_;
    NotifySessionStateChangeFunc sessionStateChangeFunc_;
    NotifySessionStateChangeNotifyManagerFunc sessionStateChangeNotifyManagerFunc_;
    NotifyBackPressedFunc backPressedFunc_;
    NotifySessionFocusableChangeFunc sessionFocusableChangeFunc_;
    NotifySessionTouchableChangeFunc sessionTouchableChangeFunc_;
    NotifyClickFunc clickFunc_;
    NotifyTerminateSessionFunc terminateSessionFunc_;
    NotifyTerminateSessionFuncNew terminateSessionFuncNew_;
    NotifySessionExceptionFunc sessionExceptionFunc_;
    NotifyPendingSessionToForegroundFunc pendingSessionToForegroundFunc_;
    NotifyPendingSessionToBackgroundForDelegatorFunc pendingSessionToBackgroundForDelegatorFunc_;
    NotifyCallingSessionForegroundFunc notifyCallingSessionForegroundFunc_;
    NotifyCallingSessionBackgroundFunc notifyCallingSessionBackgroundFunc_;
    sptr<WindowSessionProperty> property_ = nullptr;
    SystemSessionConfig systemConfig_;
    sptr<ScenePersistence> scenePersistence_ = nullptr;
    uint32_t zOrder_ = 0;

private:
    void FillSessionInfo(SessionInfo& sessionInfo);
    sptr<AppExecFwk::AbilityInfo> QueryAbilityInfoFromBMS(const int32_t uId, const std::string& bundleName,
        const std::string& abilityName, const std::string& moduleName);
    bool CheckDialogOnForeground();

    template<typename T>
    bool RegisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener);
    template<typename T>
    bool UnregisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener);

    template<typename T1, typename T2, typename Ret>
    using EnableIfSame = typename std::enable_if<std::is_same_v<T1, T2>, Ret>::type;
    template<typename T>
    inline EnableIfSame<T, ILifecycleListener, std::vector<std::weak_ptr<ILifecycleListener>>> GetListeners()
    {
        std::vector<std::weak_ptr<ILifecycleListener>> lifecycleListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            for (auto& listener : lifecycleListeners_) {
                lifecycleListeners.push_back(listener);
            }
        }
        return lifecycleListeners;
    }

    std::shared_ptr<RSSurfaceNode> CreateSurfaceNode(const std::string& name);

    int32_t persistentId_ = INVALID_SESSION_ID;
    static std::atomic<int32_t> sessionId_;
    static std::set<int32_t> persistIdSet_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
    SessionState state_ = SessionState::STATE_DISCONNECT;
    bool showRecent_ = false;
    bool bufferAvailable_ = false;

    std::recursive_mutex mutex_;
    std::vector<std::shared_ptr<ILifecycleListener>> lifecycleListeners_;
    sptr<IWindowEventChannel> windowEventChannel_ = nullptr;

    std::vector<sptr<Session>> dialogVec_;
    sptr<Session> parentSession_;

    int32_t callingPid_ = { 0 };
    int32_t callingUid_ = { 0 };
    std::string callingBundleName_ { "unknow" };
    bool isVisible_ {false};
    bool needNotify_ {true};
    sptr<IRemoteObject> abilityToken_ = nullptr;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_H
