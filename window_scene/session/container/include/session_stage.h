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
    virtual void AfterForeground() {};
    virtual void AfterBackground() {};
    virtual void AfterActive() {};
    virtual void AfterInactive() {};
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
#define CALL_SESSION_STATE_LISTENER(sessionStateCb, listeners) \
    do {                                                       \
        for (auto& listener : (listeners)) {                   \
            if (!listener.expired()) {                         \
                listener.lock()->sessionStateCb();             \
            }                                                  \
        }                                                      \
    } while (0)

public:
    explicit SessionStage(const sptr<ISession>& session);
    virtual ~SessionStage() = default;

    virtual WSError Connect();
    virtual WSError Foreground();
    virtual WSError Background();
    virtual WSError Disconnect();
    virtual WSError PendingSessionActivation(const SessionInfo& info);
    // for scene session stage
    virtual WSError Recover();
    virtual WSError Maximize();

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
    inline EnableIfSame<T, IPointerEventListener, std::vector<std::weak_ptr<IPointerEventListener>>> GetListeners()
    {
        std::vector<std::weak_ptr<IPointerEventListener>> pointerEventListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            for (auto& listener : pointerEventListeners_) {
                pointerEventListeners.push_back(listener);
            }
        }
        return pointerEventListeners;
    }

    template<typename T>
    inline EnableIfSame<T, IKeyEventListener, std::vector<std::weak_ptr<IKeyEventListener>>> GetListeners()
    {
        std::vector<std::weak_ptr<IKeyEventListener>> keyEventListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            for (auto& listener : keyEventListeners_) {
                keyEventListeners.push_back(listener);
            }
        }
        return keyEventListeners;
    }

    std::recursive_mutex mutex_;
    std::vector<std::shared_ptr<IPointerEventListener>> pointerEventListeners_;
    std::vector<std::shared_ptr<IKeyEventListener>> keyEventListeners_;
    std::vector<std::shared_ptr<ISessionStageStateListener>> sessionStageStateListeners_;
    std::vector<std::shared_ptr<ISizeChangeListener>> sizeChangeListeners_;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_STAGE_H
