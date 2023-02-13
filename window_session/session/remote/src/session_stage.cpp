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

#include "session_stage.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionStage"};
}

bool SessionStage::RegisterSessionStageStateListener(const std::shared_ptr<ISessionStageStateListener>& listener)
{
    return RegisterListenerLocked(sessionStageStateListeners_, listener);
}

bool SessionStage::UnregisterSessionStageStateListener(const std::shared_ptr<ISessionStageStateListener>& listener)
{
    return UnregisterListenerLocked(sessionStageStateListeners_, listener);
}

bool SessionStage::RegisterSizeChangeListener(const std::shared_ptr<ISizeChangeListener>& listener)
{
    return RegisterListenerLocked(sizeChangeListeners_, listener);
}

bool SessionStage::UnregisterSizeChangeListener(const std::shared_ptr<ISizeChangeListener>& listener)
{
    return UnregisterListenerLocked(sizeChangeListeners_, listener);
}

template<typename T>
bool SessionStage::RegisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener is nullptr");
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (std::find(holder.begin(), holder.end(), listener) != holder.end()) {
        WLOGFE("Listener already registered");
        return true;
    }
    holder.emplace_back(listener);
    return true;
}

template<typename T>
bool SessionStage::UnregisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener)
{
    if (listener == nullptr) {
        WLOGFE("listener could not be null");
        return false;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    holder.erase(std::remove_if(holder.begin(), holder.end(),
        [listener](std::shared_ptr<T> registeredListener) {
            return registeredListener == listener;
        }), holder.end());
    return true;
}

void SessionStage::NotifySizeChange(const WSRect& rect, SizeChangeReason reason)
{
    auto sizeChangeListeners = GetListeners<ISizeChangeListener>();
    for (auto& listener : sizeChangeListeners) {
        if (!listener.expired()) {
            listener.lock()->OnSizeChange(rect, reason);
        }
    }
}

WSError SessionStage::Connect()
{
    if (session_ == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    sptr<SessionStage> sessionStage(this);
    return session_->Connect(sessionStage);
}

WSError SessionStage::Foreground()
{
    if (session_ == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    WSError res = session_->Foreground();
    if (res == WSError::WS_OK) {
        NotifyAfterForeground();
    }
    return res;
}

WSError SessionStage::Background()
{
    if (session_ == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    NotifyAfterBackground();
    return WSError::WS_OK;
}

WSError SessionStage::Disconnect()
{
    if (session_ == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    return WSError::WS_OK;
}

WSError SessionStage::StartAbility(const AbilityInfo& info)
{
    if (session_ == nullptr) {
        WLOGFE("session is invalid");
        return WSError::WS_ERROR_NULLPTR;
    }
    return session_->StartAbility(info);
}
}
