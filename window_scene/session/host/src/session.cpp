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

#include "session/host/include/session.h"

#include <ui/rs_surface_node.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "Session"};
    const std::string UNDEFINED = "undefined";
}

Session::Session(const SessionInfo& info) : sessionInfo_(info)
{
    surfaceNode_ = CreateSurfaceNode(info.bundleName_);
    if (surfaceNode_ == nullptr) {
        WLOGFE("create surface node failed");
    }
}

void Session::SetPersistentId(uint32_t persistentId)
{
    persistentId_ = persistentId;
}

uint32_t Session::GetPersistentId() const
{
    return persistentId_;
}

std::shared_ptr<RSSurfaceNode> Session::GetSurfaceNode() const
{
    return surfaceNode_;
}

const SessionInfo& Session::GetSessionInfo() const
{
    return sessionInfo_;
}

bool Session::RegisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener)
{
    return RegisterListenerLocked(lifecycleListeners_, listener);
}

bool Session::UnregisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener)
{
    return RegisterListenerLocked(lifecycleListeners_, listener);
}

template<typename T>
bool Session::RegisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener)
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
bool Session::UnregisterListenerLocked(std::vector<std::shared_ptr<T>>& holder, const std::shared_ptr<T>& listener)
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

void Session::NotifyForeground()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) {
            listener.lock()->OnForeground();
        }
    }
}

void Session::NotifyBackground()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) {
            listener.lock()->OnBackground();
        }
    }
}

SessionState Session::GetSessionState() const
{
    return state_;
}

void Session::UpdateSessionState(SessionState state)
{
    state_ = state;
}

bool Session::IsSessionValid() const
{
    bool res = state_ > SessionState::STATE_DISCONNECT;
    if (!res) {
        WLOGFI("session is already destroyed or not created! id: %{public}u state: %{public}u",
            GetPersistentId(), state_);
    }
    return res;
}

RSSurfaceNode::SharedPtr Session::CreateSurfaceNode(std::string name)
{
    if (name.empty()) {
        WLOGFI("name is empty");
        name = UNDEFINED + std::to_string(persistentId_++);
    } else {
        std::string surfaceNodeName = name + std::to_string(persistentId_++);
        std::size_t pos = surfaceNodeName.find_last_of('.');
        name = (pos == std::string::npos) ? surfaceNodeName : surfaceNodeName.substr(pos + 1); // skip '.'
    }
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = name;
    return RSSurfaceNode::Create(rsSurfaceNodeConfig);
}

WSError Session::UpdateRect(const WSRect& rect, SizeChangeReason reason)
{
    WLOGFI("session update rect: id: %{public}u, rect[%{public}d, %{public}d, %{public}u, %{public}u], "\
        "reason:%{public}u", GetPersistentId(), rect.posX_, rect.posY_, rect.width_, rect.height_, reason);
    if (!IsSessionValid()) {
        winRect_ = rect;
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    sessionStage_->UpdateRect(rect, reason);
    winRect_ = rect;
    return WSError::WS_OK;
}


WSError Session::Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel)
{
    WLOGFI("Connect session, id: %{public}u, state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()));
    if (GetSessionState() != SessionState::STATE_DISCONNECT) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    sessionStage_ = sessionStage;
    if (sessionStage_ == nullptr) {
        WLOGFE("session stage is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    windowEventChannel_ = eventChannel;
    if (sessionStage_ == nullptr || windowEventChannel_ == nullptr) {
        WLOGFE("session stage or eventChannel is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    UpdateSessionState(SessionState::STATE_CONNECT);
    // once update rect before connect, update again when connect
    UpdateRect(winRect_, SizeChangeReason::SHOW);
    return WSError::WS_OK;
}

WSError Session::Foreground()
{
    SessionState state = GetSessionState();
    WLOGFI("Foreground session, id: %{public}u, state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(state));
    if (state != SessionState::STATE_CONNECT && state != SessionState::STATE_BACKGROUND) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    UpdateSessionState(SessionState::STATE_FOREGROUND);
    if (!isActive_) {
        sessionStage_->SetActive(true);
        UpdateSessionState(SessionState::STATE_ACTIVE);
    }
    return WSError::WS_OK;
}

WSError Session::Background()
{
    SessionState state = GetSessionState();
    WLOGFI("Background session, id: %{public}u, state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(state));
    if (state < SessionState::STATE_INACTIVE) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }

    UpdateSessionState(SessionState::STATE_BACKGROUND);
    return WSError::WS_OK;
}

WSError Session::Disconnect()
{
    SessionState state = GetSessionState();
    WLOGFI("Disconnect session, id: %{public}u, state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(state));
    Background();
    if (GetSessionState() == SessionState::STATE_BACKGROUND) {
        UpdateSessionState(SessionState::STATE_DISCONNECT);
    }
    return WSError::WS_OK;
}

WSError Session::SetActive(bool active)
{
    SessionState state = GetSessionState();
    WLOGFI("Session update active: %{public}d, id: %{public}u, state: %{public}u", active, GetPersistentId(),
        static_cast<uint32_t>(state));
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (active == isActive_) {
        WLOGFI("Session active do not change: [%{public}d]", active);
        return WSError::WS_DO_NOTHING;
    }
    isActive_ = active;
    if (active && GetSessionState() == SessionState::STATE_FOREGROUND) {
        sessionStage_->SetActive(true);
        UpdateSessionState(SessionState::STATE_ACTIVE);
    }
    if (!active && GetSessionState() == SessionState::STATE_ACTIVE) {
        sessionStage_->SetActive(false);
        UpdateSessionState(SessionState::STATE_INACTIVE);
    }
    return WSError::WS_OK;
}

WSError Session::StartPendingSessionActivation(const SessionInfo& info)
{
    if (startPendingSessionActivationFunc_) {
        startPendingSessionActivationFunc_(info);
    }
    return WSError::WS_OK;
}

void Session::SetStartPendingSessionActivationEventListener(const NotifyStartPendingSessionActivationFunc& func)
{
    startPendingSessionActivationFunc_ = func;
}

WSError Session::Recover()
{
    return WSError::WS_OK;
}

WSError Session::Maximum()
{
    return WSError::WS_OK;
}

// for window event
WSError Session::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    WLOGFI("Session TransferPointEvent");
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    windowEventChannel_->TransferPointerEvent(pointerEvent);
    return WSError::WS_OK;
}

WSError Session::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    WLOGFI("Session TransferPointEvent");
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    windowEventChannel_->TransferKeyEvent(keyEvent);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
