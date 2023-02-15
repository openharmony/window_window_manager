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

#ifndef OHOS_ORSEN_WINDOW_SESSION_STAGE_H
#define OHOS_ORSEN_WINDOW_SESSION_STAGE_H

#include <memory>
#include <mutex>
#include <refbase.h>
#include <vector>
#include "ws_common.h"
#include "zidl/session_interface.h"
#include "zidl/session_stage_stub.h"
#include "zidl/window_event_channel_interface.h"
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
    SessionStage(const sptr<ISession>& session);
    virtual ~SessionStage() = default;

    virtual WSError Connect();
    virtual WSError Foreground();
    virtual WSError Background();
    virtual WSError Disconnect();
    virtual WSError StartAbility(const WindowSession::AbilityInfo& info);
    virtual WSError SetActive(bool active) override;
    virtual WSError UpdateRect(const WSRect& rect, SizeChangeReason reason) override;

    // for scene session stage
    virtual WSError Recover() = 0;
    virtual WSError Maximum() = 0;

    bool RegisterSessionStageStateListener(const std::shared_ptr<ISessionStageStateListener>& listener);
    bool UnregisterSessionStageStateListener(const std::shared_ptr<ISessionStageStateListener>& listener);
    bool RegisterSizeChangeListener(const std::shared_ptr<ISizeChangeListener>& listener);
    bool UnregisterSizeChangeListener(const std::shared_ptr<ISizeChangeListener>& listener);

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
    std::recursive_mutex mutex_;
    std::vector<std::shared_ptr<ISessionStageStateListener>> sessionStageStateListeners_;
    std::vector<std::shared_ptr<ISizeChangeListener>> sizeChangeListeners_;
};
}
#endif // OHOS_ORSEN_WINDOW_SESSION_STAGE_H
