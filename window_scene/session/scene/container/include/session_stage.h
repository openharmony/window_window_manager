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

#ifndef OHOS_ORSEN_WINDOW_SCENE_SESSION_STAGE_H
#define OHOS_ORSEN_WINDOW_SCENE_SESSION_STAGE_H

#include <memory>
#include <mutex>
#include <refbase.h>
#include <vector>

// #include <i_input_event_consumer.h>
#include "utils/include/window_scene_common.h"

namespace OHOS::Rosen {
class ISessionStageStateListener {
public:
    virtual void AfterForeground() = 0;
    virtual void AfterBackground() = 0;
    virtual void AfterActive() = 0;
    virtual void AfterInactive() = 0;
};

class ISessionChangeListener {
public:
    virtual void OnSizeChange(WSRect rect, SessionSizeChangeReason reason) = 0;
};

class SessionStage : public virtual RefBase {
#define CALL_SESSION_STATE_LISTENER(sessionStateCb, listeners) \
    do {                                                       \
        for (auto& listener : (listeners)) {                   \
            if (!listener.expired()) {                         \
                listener.lock()->sessionStateCb();             \
            }                                                  \
        }                                                      \
    } while (0)

public:
    SessionStage() = default;
    virtual ~SessionStage() = default;

    virtual WSError Connect() = 0;
    virtual WSError Foreground() = 0;
    virtual WSError Background() = 0;
    virtual WSError Disconnect() = 0;

    bool RegisterSessionStageStateListener(const std::shared_ptr<ISessionStageStateListener>& listener);
    bool UnregisterSessionStageStateListener(const std::shared_ptr<ISessionStageStateListener>& listener);
    bool RegisterSessionChangeListener(const std::shared_ptr<ISessionChangeListener>& listener);
    bool UnregisterSessionSizeChangeListener(const std::shared_ptr<ISessionChangeListener>& listener);
    // bool RegisterInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer);

protected:
    void NotifySizeChange(const WSRect& rect, SessionSizeChangeReason reason);
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

private:
    template<typename T> bool RegisterListenerLocked(std::vector<std::shared_ptr<T>>& holder,
        const std::shared_ptr<T>& listener);
    template<typename T> bool UnregisterListenerLocked(std::vector<std::shared_ptr<T>>& holder,
        const std::shared_ptr<T>& listener);
    
    template<typename T1, typename T2, typename Ret>
    using EnableIfSame = typename std::enable_if<std::is_same_v<T1, T2>, Ret>::type;
    template<typename T>
    inline EnableIfSame<T, ISessionStageStateListener,
        std::vector<std::weak_ptr<ISessionStageStateListener>>> GetListeners()
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
    inline EnableIfSame<T, ISessionChangeListener, std::vector<std::weak_ptr<ISessionChangeListener>>> GetListeners()
    {
        std::vector<std::weak_ptr<ISessionChangeListener>> sessionChangeListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            for (auto& listener : sessionChangeListeners_) {
                sessionChangeListeners.push_back(listener);
            }
        }
        return sessionChangeListeners;
    }
    std::recursive_mutex mutex_;
    std::vector<std::shared_ptr<ISessionStageStateListener>> sessionStageStateListeners_;
    std::vector<std::shared_ptr<ISessionChangeListener>> sessionChangeListeners_;
};
}
#endif // OHOS_ORSEN_WINDOW_SCENE_SESSION_STAGE_H
