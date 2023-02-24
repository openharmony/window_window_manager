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
#include <refbase.h>
#include <string>
#include <vector>

#include "interfaces/include/ws_common.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "session/host/include/zidl/session_stub.h"

namespace OHOS::MMI {
class PointerEvent;
class KeyEvent;
class AxisEvent;
} // namespace OHOS::MMI

namespace OHOS::Rosen {
class RSSurfaceNode;
using NotifyPendingSessionActivationFunc = std::function<void(const SessionInfo& info)>;

class ILifecycleListener {
public:
    virtual void OnForeground() = 0;
    virtual void OnBackground() = 0;
};

class Session : public SessionStub, public virtual RefBase {
public:
    Session(const SessionInfo& info);
    virtual ~Session() = default;

    void SetPersistentId(uint64_t persistentId);
    uint64_t GetPersistentId() const;
    std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const;

    virtual WSError SetActive(bool active);
    virtual WSError UpdateRect(const WSRect& rect, SizeChangeReason reason);

    virtual WSError Connect(
        const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel) override;
    virtual WSError Foreground() override;
    virtual WSError Background() override;
    virtual WSError Disconnect() override;
    virtual WSError PendingSessionActivation(const SessionInfo& info) override;

    virtual WSError Recover() override;
    virtual WSError Maximum() override;

    void NotifyForeground();
    void NotifyBackground();

    // for window event
    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent);

    bool RegisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener);
    bool UnregisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener);

    const SessionInfo& GetSessionInfo() const;
    void SetPendingSessionActivationEventListener(const NotifyPendingSessionActivationFunc& func);

protected:
    SessionState GetSessionState() const;
    void UpdateSessionState(SessionState state);
    bool IsSessionValid() const;
    bool isActive_ = false;
    WSRect winRect_;
    sptr<ISessionStage> sessionStage_;
    SessionInfo sessionInfo_;
    NotifyPendingSessionActivationFunc pendingSessionActivationFunc_;

private:
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

    std::shared_ptr<RSSurfaceNode> CreateSurfaceNode(std::string name);

    uint64_t persistentId_ = INVALID_SESSION_ID;
    std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
    SessionState state_ = SessionState::STATE_DISCONNECT;

    std::recursive_mutex mutex_;
    std::vector<std::shared_ptr<ILifecycleListener>> lifecycleListeners_;
    sptr<IWindowEventChannel> windowEventChannel_ = nullptr;
};
} // namespace OHOS::Rosen
  //
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_H
