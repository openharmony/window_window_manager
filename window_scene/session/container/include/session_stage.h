/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_SESSION_STAGE_H
#define OHOS_SESSION_STAGE_H

#include <memory>
#include <mutex>
#include <i_input_event_consumer.h>
#include "window_scene_common.h"
#include "zidl/window_event_channel_interface.h"

namespace OHOS::Rosen {
class ISessionStateListener {
public:
    virtual void AfterForeground() = 0;
    virtual void AfterBackground() = 0;
    virtual void AfterActive() = 0;
    virtual void AfterInactive() = 0;
};

class SessionStage {
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
    ~SessionStage() = default;

    virtual WSError Connect(const sptr<IWindowEventChannel>& eventChannel) = 0;
    virtual WSError Foreground() = 0;
    virtual WSError Background() = 0;
    virtual WSError Disconnect() = 0;

    bool RegisterSessionStateListener(const std::shared_ptr<ISessionStateListener>& listener);
    bool UnregisterLifeCycleListener(const std::shared_ptr<ISessionStateListener>& listener);
    // bool RegisterInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer);

protected:
    inline void NotifyAfterForeground()
    {
        auto sessionStateListeners = GetListeners<ISessionStateListener>();
        CALL_SESSION_STATE_LISTENER(AfterForeground, sessionStateListeners);
    }

    inline void NotifyAfterBackground()
    {
        auto sessionStateListeners = GetListeners<ISessionStateListener>();
        CALL_SESSION_STATE_LISTENER(AfterBackground, sessionStateListeners);
    }

    inline void NotifyAfterActive()
    {
        auto sessionStateListeners = GetListeners<ISessionStateListener>();
        CALL_SESSION_STATE_LISTENER(AfterActive, sessionStateListeners);
    }

    inline void NotifyAfterInactive(bool needNotifyUiContent = true)
    {
        auto sessionStateListeners = GetListeners<ISessionStateListener>();
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
    inline EnableIfSame<T, ISessionStateListener, std::vector<std::weak_ptr<ISessionStateListener>>> GetListeners()
    {
        std::vector<std::weak_ptr<ISessionStateListener>> sessionStateListeners;
        {
            std::lock_guard<std::recursive_mutex> lock(mutex_);
            for (auto& listener : sessionStateListeners_) {
                sessionStateListeners.push_back(listener);
            }
        }
        return sessionStateListeners;
    }
    std::recursive_mutex mutex_;
    std::vector<std::shared_ptr<ISessionStateListener>> sessionStateListeners_;
};
}
#endif // OHOS_SESSION_STAGE_H
