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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_STAGE_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_STAGE_H

#include <memory>
#include <mutex>
#include <refbase.h>
#include <vector>

#include "interfaces/include/ws_common.h"
#include "session/container/include/zidl/session_stage_stub.h"
#include "session/container/include/zidl/window_event_channel_interface.h"
#include "session/host/include/zidl/session_interface.h"

namespace OHOS::MMI {
class PointerEvent;
class KeyEvent;
class AxisEvent;
} // namespace OHOS::MMI

namespace OHOS::Rosen {
class ISessionStageStateListener {
public:
    virtual void AfterForeground() = 0;
    virtual void AfterBackground() = 0;
    virtual void AfterActive() = 0;
    virtual void AfterInactive() = 0;
};

class ISizeChangeListener {
public:
    virtual void OnSizeChange(WSRect rect, SizeChangeReason reason) = 0;
};

class IPointerEventListener {
public:
    virtual void OnPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) = 0;
};

class IKeyEventListener {
public:
    virtual void OnKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) = 0;
};

class SessionStage : public SessionStageStub, public virtual RefBase {
public:
    SessionStage(const sptr<ISession>& session);
    virtual ~SessionStage() = default;

    virtual WSError Connect();
    virtual WSError Foreground();
    virtual WSError Background();
    virtual WSError Disconnect();
    virtual WSError PendingSessionActivation(const SessionInfo& info);
    // for scene session stage
    virtual WSError Recover();
    virtual WSError Maximum();

    // IPC
    WSError SetActive(bool active) override;
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason) override;

    bool RegisterSessionStageStateListener(const std::shared_ptr<ISessionStageStateListener>& listener);
    bool UnregisterSessionStageStateListener(const std::shared_ptr<ISessionStageStateListener>& listener);
    bool RegisterSizeChangeListener(const std::shared_ptr<ISizeChangeListener>& listener);
    bool UnregisterSizeChangeListener(const std::shared_ptr<ISizeChangeListener>& listener);

    bool RegisterPointerEventListener(const std::shared_ptr<IPointerEventListener>& listener);
    bool UnregisterPointerEventListener(const std::shared_ptr<IPointerEventListener>& listener);
    bool RegisterKeyEventListener(const std::shared_ptr<IKeyEventListener>& listener);
    bool UnregisterKeyEventListener(const std::shared_ptr<IKeyEventListener>& listener);

    // for window event
    void NotifyPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void NotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent);

protected:
    void NotifySizeChange(const WSRect& rect, SizeChangeReason reason);
    sptr<ISession> session_;

private:
    std::recursive_mutex mutex_;
    std::vector<std::shared_ptr<IPointerEventListener>> pointerEventListeners_;
    std::vector<std::shared_ptr<IKeyEventListener>> keyEventListeners_;
    std::vector<std::shared_ptr<ISessionStageStateListener>> sessionStageStateListeners_;
    std::vector<std::shared_ptr<ISizeChangeListener>> sizeChangeListeners_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_STAGE_H
