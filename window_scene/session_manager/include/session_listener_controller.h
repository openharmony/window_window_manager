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

#ifndef OHOS_SESSION_MANAGER_SESSION_LISTENER_CONTROLLER_H
#define OHOS_SESSION_MANAGER_SESSION_LISTENER_CONTROLLER_H

#include <mutex>
#include <vector>
#include <list>

#include "mission_listener_interface.h"
#include "ws_common.h"

namespace OHOS {
namespace Rosen {
using ISessionListener = AAFwk::IMissionListener;
class SessionListenerController : public std::enable_shared_from_this<SessionListenerController> {
public:
    SessionListenerController() = default;
    ~SessionListenerController() = default;

    WSError AddSessionListener(const sptr<ISessionListener>& listener);

    void DelSessionListener(const sptr<ISessionListener>& listener);

    void NotifySessionCreated(int32_t persistentId);

    void NotifySessionDestroyed(int32_t persistentId);

    void NotifySessionSnapshotChanged(int32_t persistentId);

    void NotifySessionMovedToFront(int32_t persistentId);

    void NotifySessionFocused(int32_t persistentId);

    void NotifySessionUnfocused(int32_t persistentId);

    void NotifySessionIconChanged(int32_t persistentId, const std::shared_ptr<OHOS::Media::PixelMap>& icon);

    void NotifySessionClosed(int32_t persistentId);

    void NotifySessionLabelUpdated(int32_t persistentId);

    void HandleUnInstallApp(const std::list<int32_t>& sessions);

private:
    void OnListenerDied(const wptr<IRemoteObject>& remote);

    template<typename F, typename... Args>
    void CallListeners(F func, Args&&... args);

    class ListenerDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        using ListenerDiedHandler = std::function<void(const wptr<IRemoteObject>&)>;
        explicit ListenerDeathRecipient(ListenerDiedHandler handler) : diedHandler_(std::move(handler)) {}
        ~ListenerDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remote) final
        {
            if (diedHandler_) {
                diedHandler_(remote);
            }
        }

    private:
        ListenerDiedHandler diedHandler_;
    };

private:
    std::mutex listenerLock_;
    std::vector<sptr<ISessionListener>> sessionListeners_;
    sptr<IRemoteObject::DeathRecipient> listenerDeathRecipient_;
};
}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_SESSION_MANAGER_SESSION_LISTENER_CONTROLLER_H
