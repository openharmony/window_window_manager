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

#include "pointer_event.h"
#include "key_event.h"

#include "anr_manager.h"
#include "interfaces/include/ws_common.h"
#include "surface_capture_future.h"
#include <transaction/rs_interfaces.h>
#include <ui/rs_surface_node.h>
#include "util.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "Session" };
} // namespace

Session::Session(const SessionInfo& info) : sessionInfo_(info)
{
}

Session::~Session()
{
    WLOGD("~Session");
}

void Session::SetPersistentId(uint64_t persistentId)
{
    persistentId_ = persistentId;
}

uint64_t Session::GetPersistentId() const
{
    return persistentId_;
}

uint64_t Session::GetParentPersistentId() const
{
    if (property_ != nullptr) {
        WLOGFD("GetParentPersistentId, id:%{public}" PRIu64"", property_->GetParentPersistentId());
        return property_->GetParentPersistentId();
    }
    return INVALID_SESSION_ID;
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
    return UnregisterListenerLocked(lifecycleListeners_, listener);
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
        return false;
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
        [listener](std::shared_ptr<T> registeredListener) { return registeredListener == listener; }),
        holder.end());
    return true;
}

void Session::NotifyConnect()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) {
            listener.lock()->OnConnect();
        }
    }
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
    NotifySessionStateChange(state);
}

bool Session::IsSessionValid() const
{
    bool res = state_ > SessionState::STATE_DISCONNECT && state_ < SessionState::STATE_END;
    if (!res) {
        WLOGFI("session is already destroyed or not created! id: %{public}" PRIu64 " state: %{public}u",
            GetPersistentId(), state_);
    }
    return res;
}

WSError Session::UpdateRect(const WSRect& rect, SizeChangeReason reason)
{
    WLOGFI("session update rect: id: %{public}" PRIu64 ", rect[%{public}d, %{public}d, %{public}u, %{public}u], "\
        "reason:%{public}u", GetPersistentId(), rect.posX_, rect.posY_, rect.width_, rect.height_, reason);
    if (!IsSessionValid()) {
        winRect_ = rect;
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    winRect_ = rect;
    sessionStage_->UpdateRect(rect, reason);
    return WSError::WS_OK;
}

WSError Session::Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig, sptr<WindowSessionProperty> property)
{
    WLOGFI("Connect session, id: %{public}" PRIu64 ", state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()));
    if (GetSessionState() != SessionState::STATE_DISCONNECT) {
        WLOGFE("state is not disconnect!");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (sessionStage == nullptr || eventChannel == nullptr) {
        WLOGFE("session stage or eventChannel is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    sessionStage_ = sessionStage;
    windowEventChannel_ = eventChannel;
    surfaceNode_ = surfaceNode;
    systemConfig = systemConfig_;
    if (property) {
        property->SetPersistentId(GetPersistentId());
    }
    property_ = property;

    UpdateSessionState(SessionState::STATE_CONNECT);
    // once update rect before connect, update again when connect
    UpdateRect(winRect_, SizeChangeReason::UNDEFINED);
    NotifyConnect();
    return WSError::WS_OK;
}

WSError Session::Foreground()
{
    SessionState state = GetSessionState();
    WLOGFI("Foreground session, id: %{public}" PRIu64 ", state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(state));
    if (state != SessionState::STATE_CONNECT && state != SessionState::STATE_BACKGROUND) {
        WLOGFE("state invalid!");
        return WSError::WS_ERROR_INVALID_SESSION;
    }

    UpdateSessionState(SessionState::STATE_FOREGROUND);
    if (!isActive_) {
        SetActive(true);
    }
    NotifyForeground();
    return WSError::WS_OK;
}

WSError Session::Background()
{
    SessionState state = GetSessionState();
    WLOGFI("Background session, id: %{public}" PRIu64 ", state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(state));
    if (state < SessionState::STATE_INACTIVE) { // only STATE_INACTIVE can transfer to background
        WLOGFE("state invalid!");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    UpdateSessionState(SessionState::STATE_BACKGROUND);

    snapshot_ = Snapshot();
    NotifyBackground();
    return WSError::WS_OK;
}

WSError Session::Disconnect()
{
    SessionState state = GetSessionState();
    WLOGFI("Disconnect session, id: %{public}" PRIu64 ", state: %{public}u", GetPersistentId(),
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
    WLOGFI("Session update active: %{public}d, id: %{public}" PRIu64 ", state: %{public}u", active, GetPersistentId(),
        static_cast<uint32_t>(state));
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (active == isActive_) {
        WLOGFD("Session active do not change: [%{public}d]", active);
        return WSError::WS_DO_NOTHING;
    }
    if (active && GetSessionState() == SessionState::STATE_FOREGROUND) {
        sessionStage_->SetActive(true);
        UpdateSessionState(SessionState::STATE_ACTIVE);
        isActive_ = active;
    }
    if (!active && GetSessionState() == SessionState::STATE_ACTIVE) {
        sessionStage_->SetActive(false);
        UpdateSessionState(SessionState::STATE_INACTIVE);
        isActive_ = active;
    }
    return WSError::WS_OK;
}

WSError Session::PendingSessionActivation(const sptr<AAFwk::SessionInfo> abilitySessionInfo)
{
    SessionInfo info;
    info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
    info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
    info.moduleName_ = abilitySessionInfo->want.GetModuleName();
    info.callerToken_ = abilitySessionInfo->callerToken;
    if (pendingSessionActivationFunc_) {
        pendingSessionActivationFunc_(info);
    }
    return WSError::WS_OK;
}

void Session::SetPendingSessionActivationEventListener(const NotifyPendingSessionActivationFunc& func)
{
    pendingSessionActivationFunc_ = func;
}

void Session::SetBackPressedListenser(const NotifyBackPressedFunc& func)
{
    backPressedFunc_ = func;
}

WSError Session::TerminateSession(const sptr<AAFwk::SessionInfo> abilitySessionInfo)
{
    SessionInfo info;
    info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
    info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
    info.callerToken_ = abilitySessionInfo->callerToken;
    if (terminateSessionFunc_) {
        terminateSessionFunc_(info);
    }
    return WSError::WS_OK;
}

void Session::SetTerminateSessionListener(const NotifyTerminateSessionFunc& func)
{
    terminateSessionFunc_ = func;
}

WSError Session::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    WLOGFD("Session TransferPointEvent, Id: %{public}" PRIu64 "", persistentId_);
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    
    auto currentTime = GetSysClockTime();
    if (ANRMgr->IsANRTriggered(currentTime, persistentId_)) {
        WLOGFD("The pointer event does not report normally, application not response");
        return WSError::WS_DO_NOTHING;
    }
    if (WSError ret = windowEventChannel_->TransferPointerEvent(pointerEvent); ret != WSError::WS_OK) {
        WLOGFE("TransferPointer failed");
        return ret;
    }
    // 这里执行添加定时器的逻辑， 需要persistentId_,然后把一些状态信息存储到eventStage
    ANRMgr->AddTimer(pointerEvent->GetId(), currentTime, persistentId_);
    if (ANRMgr->GetPidByPersistentId(persistentId_) == -1) {
        ANRMgr->SetApplicationPid(persistentId_, windowEventChannel_->GetApplicationPid());
    }
    return WSError::WS_OK;
}

WSError Session::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    WLOGFD("Session TransferKeyEvent");
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferKeyEvent(keyEvent);
}

std::shared_ptr<Media::PixelMap> Session::GetSnapshot() const
{
    return snapshot_;
}

std::shared_ptr<Media::PixelMap> Session::Snapshot()
{
    auto callback = std::make_shared<SurfaceCaptureFuture>();
    RSInterfaces::GetInstance().TakeSurfaceCapture(surfaceNode_, callback);
    auto pixelMap = callback->GetResult(2000); // wait for <= 2000ms
    if (pixelMap != nullptr) {
        WLOGFD("Save pixelMap WxH = %{public}dx%{public}d", pixelMap->GetWidth(), pixelMap->GetHeight());
    } else {
        WLOGFE("Failed to get pixelMap, return nullptr");
    }
    return pixelMap;
}

void Session::SetSessionStateChangeListenser(const NotifySessionStateChangeFunc& func)
{
    sessionStateChangeFunc_ = func;
}

void Session::NotifySessionStateChange(const SessionState& state)
{
    WLOGFI("state: %{public}u", static_cast<uint32_t>(state));
    if (sessionStateChangeFunc_) {
        sessionStateChangeFunc_(state);
    }
}

void Session::SetSessionRect(const WSRect& rect)
{
    winRect_ = rect;
}
WSRect Session::GetSessionRect() const
{
    return winRect_;
}

WSError Session::UpdateActiveStatus(bool isActive)
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (isActive == isActive_) {
        WLOGFD("Session active do not change: [%{public}d]", isActive);
        return WSError::WS_DO_NOTHING;
    }
    WSError ret = WSError::WS_DO_NOTHING;

    if (isActive && GetSessionState() == SessionState::STATE_FOREGROUND) {
        UpdateSessionState(SessionState::STATE_ACTIVE);
        isActive_ = isActive;
        ret = WSError::WS_OK;
    }
    if (!isActive && GetSessionState() == SessionState::STATE_ACTIVE) {
        UpdateSessionState(SessionState::STATE_INACTIVE);
        isActive_ = isActive;
        ret = WSError::WS_OK;
    }
    WLOGFD("UpdateActiveStatus, isActive: %{public}d, state: %{public}u", isActive_,
        static_cast<uint32_t>(state_));
    return ret;
}

WSError Session::OnSessionEvent(SessionEvent event)
{
    WLOGFD("Session OnSessionEvent");
    return WSError::WS_OK;
}

WSError Session::UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason)
{
    WLOGFD("UpdateSessionRect");
    return WSError::WS_OK;
}

WSError Session::RaiseToAppTop()
{
    return WSError::WS_OK;
}

WSError Session::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session)
{
    return WSError::WS_OK;
}

WSError Session::DestroyAndDisconnectSpecificSession(const uint64_t& persistentId)
{
    return WSError::WS_OK;
}

sptr<WindowSessionProperty> Session::GetSessionProperty() const
{
    return property_;
}

WindowType Session::GetWindowType() const
{
    if (property_ != nullptr) {
        WLOGFD("Type:%{public}" PRIu32 "", static_cast<uint32_t>(property_->GetWindowType()));
        return property_->GetWindowType();
    }
    return WindowType::WINDOW_TYPE_APP_MAIN_WINDOW;
}

void Session::SetSystemConfig(const SystemSessionConfig& systemConfig)
{
    systemConfig_ = systemConfig;
}

WSError Session::RequestSessionBack()
{
    if (!backPressedFunc_) {
        WLOGFW("Session didn't register back event consumer!");
        return WSError::WS_DO_NOTHING;
    }
    backPressedFunc_();
    return WSError::WS_OK;
}

WSError Session::ProcessBackEvent()
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    return sessionStage_->HandleBackEvent();
}

WSError Session::MarkProcessed(int32_t eventId)
{
    WLOGFI("WLD>>> Here in Session::MarkProcessed!");
    int32_t persistentId = GetPersistentId();
    WLOGFI("WLD>>> persistentId:%{public}d, eventId:%{public}d", persistentId, eventId);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
