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

#include "session_listener_controller.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionListenerController"};
}

WSError SessionListenerController::AddSessionListener(const sptr<ISessionListener>& listener)
{
    if (!listener) {
        WLOGFE("listener is invalid");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    std::lock_guard guard(listenerLock_);
    auto it = std::find_if(sessionListeners_.begin(), sessionListeners_.end(),
                           [&listener](const sptr<ISessionListener>& item) {
                               return item && item->AsObject() == listener->AsObject();
                           });
    if (it != sessionListeners_.end()) {
        WLOGFW("listener was already added");
        return WSError::WS_OK;
    }

    if (!listenerDeathRecipient_) {
        auto task = [weakThis = weak_from_this()](const wptr<IRemoteObject>& remote) {
            if (auto controller = weakThis.lock()) {
                controller->OnListenerDied(remote);
            }
        };
        listenerDeathRecipient_ = sptr<ListenerDeathRecipient>::MakeSptr(task);
    }
    if (auto listenerObject = listener->AsObject(); listenerObject && listenerObject->IsProxyObject()) {
        listenerObject->AddDeathRecipient(listenerDeathRecipient_);
    }
    sessionListeners_.emplace_back(listener);
    WLOGFI("success");
    return WSError::WS_OK;
}

void SessionListenerController::DelSessionListener(const sptr<ISessionListener>& listener)
{
    if (!listener) {
        WLOGFE("listener is invalid");
        return;
    }

    std::lock_guard guard(listenerLock_);
    auto it = std::find_if(sessionListeners_.begin(), sessionListeners_.end(),
                           [&listener](const sptr<ISessionListener>& item) {
                               return item && item->AsObject() == listener->AsObject();
                           });
    if (it != sessionListeners_.end()) {
        WLOGFI("success");
        if (auto listenerObject = listener->AsObject(); listenerObject && listenerObject->IsProxyObject()) {
            listenerObject->RemoveDeathRecipient(listenerDeathRecipient_);
        }
        sessionListeners_.erase(it);
    }
}

void SessionListenerController::NotifySessionCreated(int32_t persistentId)
{
    if (persistentId == -1) {
        return;
    }
    WLOGFI("Id:%{public}d", persistentId);
    CallListeners(&ISessionListener::OnMissionCreated, persistentId);
}

void SessionListenerController::NotifySessionDestroyed(int32_t persistentId)
{
    if (persistentId == -1) {
        return;
    }
    WLOGFI("Id:%{public}d", persistentId);
    CallListeners(&ISessionListener::OnMissionDestroyed, persistentId);
}

void SessionListenerController::HandleUnInstallApp(const std::list<int32_t>& sessions)
{
    for (auto id : sessions) {
        CallListeners(&ISessionListener::OnMissionDestroyed, id);
    }
}

void SessionListenerController::NotifySessionSnapshotChanged(int32_t persistentId)
{
    if (persistentId == -1) {
        return;
    }
    WLOGFI("Id:%{public}d", persistentId);
    CallListeners(&ISessionListener::OnMissionSnapshotChanged, persistentId);
}

void SessionListenerController::NotifySessionMovedToFront(int32_t persistentId)
{
    if (persistentId == -1) {
        return;
    }
    WLOGFI("Id:%{public}d", persistentId);
    CallListeners(&ISessionListener::OnMissionMovedToFront, persistentId);
}

void SessionListenerController::NotifySessionFocused(int32_t persistentId)
{
    if (persistentId == -1) {
        return;
    }
    TLOGI(WmsLogTag::WMS_FOCUS, "Id:%{public}d", persistentId);
    CallListeners(&ISessionListener::OnMissionFocused, persistentId);
}

void SessionListenerController::NotifySessionUnfocused(int32_t persistentId)
{
    if (persistentId == -1) {
        return;
    }
    TLOGI(WmsLogTag::WMS_FOCUS, "Id:%{public}d", persistentId);
    CallListeners(&ISessionListener::OnMissionUnfocused, persistentId);
}

void SessionListenerController::NotifySessionIconChanged(int32_t persistentId,
                                                         const std::shared_ptr<OHOS::Media::PixelMap>& icon)
{
    if (persistentId == -1) {
        return;
    }
    WLOGFI("Id:%{public}d", persistentId);
    CallListeners(&ISessionListener::OnMissionIconUpdated, persistentId, icon);
}

void SessionListenerController::NotifySessionClosed(int32_t persistentId)
{
    if (persistentId == -1) {
        return;
    }
    WLOGFI("Id:%{public}d", persistentId);
    CallListeners(&ISessionListener::OnMissionClosed, persistentId);
}

void SessionListenerController::NotifySessionLabelUpdated(int32_t persistentId)
{
    if (persistentId == -1) {
        return;
    }
    WLOGFI("Id:%{public}d", persistentId);
    CallListeners(&ISessionListener::OnMissionLabelUpdated, persistentId);
}

void SessionListenerController::OnListenerDied(const wptr<IRemoteObject>& remote)
{
    WLOGFD("in");
    auto remoteObj = remote.promote();
    if (!remoteObj) {
        WLOGFD("invalid remote object");
        return;
    }
    remoteObj->RemoveDeathRecipient(listenerDeathRecipient_);

    std::lock_guard guard(listenerLock_);
    auto it = std::find_if(sessionListeners_.begin(), sessionListeners_.end(),
                           [&remoteObj](const sptr<ISessionListener>& item) {
                               return item && item->AsObject() == remoteObj;
                           });
    if (it != sessionListeners_.end()) {
        WLOGFI("SessionListener removed on died");
        sessionListeners_.erase(it);
    }
}

template<typename F, typename... Args>
void SessionListenerController::CallListeners(F func, Args&&... args)
{
    std::lock_guard guard(listenerLock_);
    WLOGFD("size:%{public}d", static_cast<int32_t>(sessionListeners_.size()));
    for (auto listener : sessionListeners_) {
        if (listener) {
            (listener->*func)(std::forward<Args>(args)...);
        }
    }
}

} // namespace Rosen
} // namespace OHOS
