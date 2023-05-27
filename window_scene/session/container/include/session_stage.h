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
    virtual void OnSizeChange(const WSRect& rect, SizeChangeReason reason) = 0;
};

class IInputEventListener {
public:
    virtual void OnPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) = 0;
    virtual void OnKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) = 0;
    virtual void OnAxisEvent(const std::shared_ptr<MMI::AxisEvent>& axisEvent) = 0;
};

class SessionStage : public SessionStageStub, public virtual RefBase {
#define CALL_SESSION_STATE_LISTENER(sessionStateCb, listeners) \
    do {                                                       \
        for (auto& listener : (listeners)) {                   \
            if (!listener.expired()) {                         \
                listener.lock()->sessionStateCb();             \
            }                                                  \
        }                                                      \
    } while (0)

#define CALL_INPUT_EVENT_LISTENER(listeners, inputCb, event)    \
    do {                                                        \
        for (auto& listener : (listeners)) {                    \
            if (!listener.expired()) {                          \
                listener.lock()->inputCb(event);                \
            }                                                   \
        }                                                       \
    } while (0)

public:
    explicit SessionStage(const sptr<ISession>& session);
    virtual ~SessionStage() = default;

    virtual WSError Connect(const std::shared_ptr<RSSurfaceNode>& surfaceNode);
    virtual WSError Foreground();
    virtual WSError Background();
    virtual WSError Disconnect();
    virtual WSError PendingSessionActivation(const SessionInfo& info);
    // for scene session stage
    WSError SetActive(bool active) override;
    WSError UpdateRect(const WSRect& rect, SizeChangeReason reason) override;

    bool RegisterSessionStageStateListener(const std::shared_ptr<ISessionStageStateListener>& listener);
    bool UnregisterSessionStageStateListener(const std::shared_ptr<ISessionStageStateListener>& listener);

    bool RegisterSizeChangeListener(const std::shared_ptr<ISizeChangeListener>& listener);
    bool UnregisterSizeChangeListener(const std::shared_ptr<ISizeChangeListener>& listener);

    bool RegisterInputEventListener(const std::shared_ptr<IInputEventListener>& listener);
    bool UnregisterInputEventListener(const std::shared_ptr<IInputEventListener>& listener);

    inline void NotifyPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) override
    {
        auto listeners = GetListeners<IInputEventListener>();
        CALL_INPUT_EVENT_LISTENER(listeners, OnPointerEvent, pointerEvent);
    }

    inline void NotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override
    {
        auto listeners = GetListeners<IInputEventListener>();
        CALL_INPUT_EVENT_LISTENER(listeners, OnKeyEvent, keyEvent);
    }

    inline void NotifyAxisEvent(const std::shared_ptr<MMI::AxisEvent>& axisEvent)
    {
        auto listeners = GetListeners<IInputEventListener>();
        CALL_INPUT_EVENT_LISTENER(listeners, OnAxisEvent, axisEvent);
    }

protected:
    void NotifySizeChange(const WSRect& rect, SizeChangeReason reason)
    {
        auto sizeChangeListeners = GetListeners<ISizeChangeListener>();
        for (auto& listener : sizeChangeListeners) {
            if (!listener.expired()) {
                listener.lock()->OnSizeChange(rect, reason);
            }
        }
    }

    inline void NotifyAfterForeground()
    {
        auto sessionStateListeners = GetListeners<ISessionStageStateListener>();
        CALL_SESSION_STATE_LISTENER(AfterForeground, sessionStateListeners);
    }

    inline void NotifyAfterBackground()
    {
        auto sessionStateListeners = GetListeners<ISessionStageStateListener>();
        CALL_SESSION_STATE_LISTENER(AfterBackground, sessionStateListeners);
    }

    inline void NotifyAfterActive()
    {
        auto sessionStateListeners = GetListeners<ISessionStageStateListener>();
        CALL_SESSION_STATE_LISTENER(AfterActive, sessionStateListeners);
    }

    inline void NotifyAfterInactive(bool needNotifyUiContent = true)
    {
        auto sessionStateListeners = GetListeners<ISessionStageStateListener>();
        CALL_SESSION_STATE_LISTENER(AfterInactive, sessionStateListeners);
    }

    sptr<ISession> session_;

private:
    template<typename T>
    bool RegisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener);
    template<typename T>
    bool UnregisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener);

    template<typename T1, typename T2, typename Ret>
    using EnableIfSame = typename std::enable_if<std::is_same_v<T1, T2>, Ret>::type;

    template<typename T>
    inline EnableIfSame<T, ISessionStageStateListener, std::vector<std::weak_ptr<ISessionStageStateListener>>>
    GetListeners()
    {
        std::vector<std::weak_ptr<ISessionStageStateListener>> sessionStageStateListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            for (auto& listener : sessionStageStateListeners_) {
                sessionStageStateListeners.push_back(listener);
            }
        }
        return sessionStageStateListeners;
    }

    template<typename T>
    inline EnableIfSame<T, ISizeChangeListener, std::vector<std::weak_ptr<ISizeChangeListener>>> GetListeners()
    {
        std::vector<std::weak_ptr<ISizeChangeListener>> sizeChangeListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            for (auto& listener : sizeChangeListeners_) {
                sizeChangeListeners.push_back(listener);
            }
        }
        return sizeChangeListeners;
    }

    template<typename T>
    inline EnableIfSame<T, IInputEventListener, std::vector<std::weak_ptr<IInputEventListener>>> GetListeners()
    {
        std::vector<std::weak_ptr<IInputEventListener>> inputEventListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            for (auto& listener : inputEventListeners_) {
                inputEventListeners.push_back(listener);
            }
        }
        return inputEventListeners;
    }

    std::recursive_mutex mutex_;
    std::vector<std::shared_ptr<ISessionStageStateListener>> sessionStageStateListeners_;
    std::vector<std::shared_ptr<ISizeChangeListener>> sizeChangeListeners_;
    std::vector<std::shared_ptr<IInputEventListener>> inputEventListeners_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_STAGE_H
