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
using NotifySessionStateChangeNotifyManagerFunc = std::function<void(uint64_t persistentId)>;
using NotifyBackPressedFunc = std::function<void()>;
using NotifySessionFocusableChangeFunc = std::function<void(const bool isFocusable)>;
using NotifyClickFunc = std::function<void()>;
using NotifyTerminateSessionFunc = std::function<void(const SessionInfo& info)>;
using NotifySessionExceptionFunc = std::function<void(const SessionInfo& info)>;

class ILifecycleListener {
public:
    virtual void OnConnect() = 0;
    virtual void OnForeground() = 0;
    virtual void OnBackground() = 0;
    virtual void OnDisconnect() = 0;
};

class Session : public SessionStub, public virtual RefBase {
public:
    explicit Session(const SessionInfo& info) : sessionInfo_(info) {}
    virtual ~Session() = default;

    uint64_t GetPersistentId() const;
    uint64_t GetParentPersistentId() const;
    void SetSessionRect(const WSRect& rect);

    std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const;
    std::shared_ptr<Media::PixelMap> GetSnapshot() const;
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

    WSError Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
        const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
        sptr<WindowSessionProperty> property = nullptr, sptr<IRemoteObject> token = nullptr) override;
    WSError Foreground() override;
    WSError Background() override;
    WSError Disconnect() override;

    WSError OnSessionEvent(SessionEvent event) override;
    WSError UpdateWindowSessionProperty(sptr<WindowSessionProperty> property) override;
    WSError OnNeedAvoid(bool status) override;
    void NotifyConnect();
    void NotifyForeground();
    void NotifyBackground();
    void NotifyDisconnect();

    virtual WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    virtual WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent);
    WSError TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed);
    WSError TransferFocusActiveEvent(bool isFocusActive);

    bool RegisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener);
    bool UnregisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener);
    void SetPendingSessionActivationEventListener(const NotifyPendingSessionActivationFunc& func);

    WSError PendingSessionActivation(const sptr<AAFwk::SessionInfo> info) override;

    void SetTerminateSessionListener(const NotifyTerminateSessionFunc& func);
    WSError TerminateSession(const sptr<AAFwk::SessionInfo> info) override;
    void SetSessionExceptionListener(const NotifyTerminateSessionFunc& func);
    WSError NotifySessionException(const sptr<AAFwk::SessionInfo> info) override;
    void SetSessionStateChangeListenser(const NotifySessionStateChangeFunc& func);
    void SetSessionStateChangeNotifyManagerListener(const NotifySessionStateChangeNotifyManagerFunc& func);
    void NotifySessionStateChange(const SessionState& state);
    WSError UpdateActiveStatus(bool isActive) override; // update active status from session_stage
    WSError RaiseToAppTop() override;
    WSError UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason) override;
    WSError CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
        const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
        sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session) override;
    WSError DestroyAndDisconnectSpecificSession(const uint64_t& persistentId) override;
    void SetSystemConfig(const SystemSessionConfig& systemConfig);
    void SetBackPressedListenser(const NotifyBackPressedFunc& func);
    WSError ProcessBackEvent(); // send back event to session_stage
    WSError RequestSessionBack() override; // receive back request from session_stage
    sptr<ScenePersistence> GetScenePersistence() const;
    void SetParentSession(const sptr<Session>& session);
    void BindDialogToParentSession(const sptr<Session>& session);
    void RemoveDialogToParentSession(const sptr<Session>& session);
    std::vector<sptr<Session>> GetDialogVector() const;
    void NotifyTouchDialogTarget();
    WSError NotifyDestroy();

    void SetSessionFocusableChangeListener(const NotifySessionFocusableChangeFunc& func);
    void SetClickListener(const NotifyClickFunc& func);
    void NotifySessionFocusableChange(bool isFocusable);
    void NotifyClick();
    WSError UpdateFocus(bool isFocused);
    WSError SetFocusable(bool isFocusable);
    bool GetFocusable() const;
    WSError SetTouchable(bool touchable);
    bool GetTouchable() const;
    WSError SetGlobalMaximizeMode(MaximizeMode mode) override;
    WSError GetGlobalMaximizeMode(MaximizeMode& mode) override;
    AvoidArea GetAvoidAreaByType(AvoidAreaType type) override;
    WSError SetBrightness(float brightness);
    float GetBrightness() const;

    bool IsSessionValid() const;

    uint32_t GetWindowId() const;
    int32_t GetCallingPid() const;
    int32_t GetCallingUid() const;
    sptr<IRemoteObject> GetAbilityToken() const;
protected:
    void GeneratePersistentId(const bool isExtension, const SessionInfo& sessionInfo);
    void UpdateSessionState(SessionState state);
    void UpdateSessionFocusable(bool isFocusable);

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
    NotifyClickFunc clickFunc_;
    NotifyTerminateSessionFunc terminateSessionFunc_;
    NotifySessionExceptionFunc sessionExceptionFunc_;
    sptr<WindowSessionProperty> property_ = nullptr;
    SystemSessionConfig systemConfig_;
    sptr<ScenePersistence> scenePersistence_ = nullptr;

private:
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
    std::shared_ptr<Media::PixelMap> Snapshot();

    uint64_t persistentId_ = INVALID_SESSION_ID;
    static std::atomic<uint32_t> sessionId_;
    static std::set<uint32_t> persistIdSet_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
    SessionState state_ = SessionState::STATE_DISCONNECT;

    std::recursive_mutex mutex_;
    std::vector<std::shared_ptr<ILifecycleListener>> lifecycleListeners_;
    sptr<IWindowEventChannel> windowEventChannel_ = nullptr;

    std::shared_ptr<Media::PixelMap> snapshot_;

    std::vector<sptr<Session>> dialogVec_;
    sptr<Session> parentSession_;

    int32_t callingPid_ = { 0 };
    int32_t callingUid_ = { 0 };
    sptr<IRemoteObject> abilityToken_ = nullptr;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_H
