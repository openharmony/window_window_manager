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
    virtual void OnForeground() {};
    virtual void OnBackground() {};
};
class Session : public SessionStub, public virtual RefBase {
public:
    explicit Session(const SessionInfo& info);
    virtual ~Session() = default;

    void SetPersistentId(uint64_t persistentId);
    uint64_t GetPersistentId() const;
    std::shared_ptr<RSSurfaceNode> GetSurfaceNode() const;

    virtual WSError SetActive(bool active);
    virtual WSError UpdateRect(const WSRect& rect, SizeChangeReason reason);

    WSError Connect(
        const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel) override;
    WSError Foreground() override;
    WSError Background() override;
    WSError Disconnect() override;
    WSError PendingSessionActivation(const SessionInfo& info) override;

    WSError Recover() override;
    WSError Maximize() override;

    // for window event
    WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent);

    const SessionInfo& GetSessionInfo() const;
    void SetPendingSessionActivationEventListener(const NotifyPendingSessionActivationFunc& func);

protected:
    SessionState GetSessionState() const;
    void UpdateSessionState(SessionState state);
    bool IsSessionValid() const;
    bool isActive_ = false;
    WSRect winRect_ {0, 0, 0, 0};
    sptr<ISessionStage> sessionStage_;
    SessionInfo sessionInfo_;
    NotifyPendingSessionActivationFunc pendingSessionActivationFunc_;

private:
    std::shared_ptr<RSSurfaceNode> CreateSurfaceNode(std::string name);

    uint64_t persistentId_ = INVALID_SESSION_ID;
    std::shared_ptr<RSSurfaceNode> surfaceNode_ = nullptr;
    SessionState state_ = SessionState::STATE_DISCONNECT;

    sptr<IWindowEventChannel> windowEventChannel_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_H
