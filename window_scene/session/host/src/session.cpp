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
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "Session" };
const std::string UNDEFINED = "undefined";
} // namespace

Session::Session(const SessionInfo& info) : sessionInfo_(info)
{
    surfaceNode_ = CreateSurfaceNode(info.bundleName_);
    if (surfaceNode_ == nullptr) {
        WLOGFE("create surface node failed");
    }
}

void Session::SetPersistentId(uint64_t persistentId)
{
    persistentId_ = persistentId;
}

uint64_t Session::GetPersistentId() const
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
    return false;
}

bool Session::UnregisterLifecycleListener(const std::shared_ptr<ILifecycleListener>& listener)
{
    return false;
}

void Session::NotifyForeground()
{
}

void Session::NotifyBackground()
{
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
    return false;
}

RSSurfaceNode::SharedPtr Session::CreateSurfaceNode(std::string name)
{
    return nullptr;
}

WSError Session::UpdateRect(const WSRect& rect, SizeChangeReason reason)
{
    WLOGFI("session update rect: id: %{public}" PRIu64 ", rect[%{public}d, %{public}d, %{public}u, %{public}u], "\
        "reason:%{public}u", GetPersistentId(), rect.posX_, rect.posY_, rect.width_, rect.height_, reason);
    return WSError::WS_OK;
}

WSError Session::Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel)
{
    WLOGFI("Connect session, id: %{public}" PRIu64 ", state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()));
    return WSError::WS_OK;
}

WSError Session::Foreground()
{
    SessionState state = GetSessionState();
    WLOGFI("Foreground session, id: %{public}" PRIu64 ", state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(state));
    return WSError::WS_OK;
}

WSError Session::Background()
{
    SessionState state = GetSessionState();
    WLOGFI("Background session, id: %{public}" PRIu64 ", state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(state));
    return WSError::WS_OK;
}

WSError Session::Disconnect()
{
    SessionState state = GetSessionState();
    WLOGFI("Disconnect session, id: %{public}" PRIu64 ", state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(state));
    return WSError::WS_OK;
}

WSError Session::SetActive(bool active)
{
    SessionState state = GetSessionState();
    WLOGFI("Session update active: %{public}d, id: %{public}" PRIu64 ", state: %{public}u", active, GetPersistentId(),
        static_cast<uint32_t>(state));
    return WSError::WS_OK;
}

WSError Session::PendingSessionActivation(const SessionInfo& info)
{
    return WSError::WS_OK;
}

void Session::SetPendingSessionActivationEventListener(const NotifyPendingSessionActivationFunc& func)
{
    pendingSessionActivationFunc_ = func;
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
    WLOGFD("Session TransferPointEvent");
    return WSError::WS_OK;
}

WSError Session::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    WLOGFD("Session TransferPointEvent");
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
