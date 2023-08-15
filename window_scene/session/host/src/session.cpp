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

#include "ability_info.h"
#include "ipc_skeleton.h"
#include "key_event.h"
#include "pointer_event.h"

#include "anr_manager.h"
#include "foundation/ability/ability_base/interfaces/kits/native/want/include/want.h"
#include "interfaces/include/ws_common.h"
#include <surface_capture_future.h>
#include <transaction/rs_interfaces.h>
#include <ui/rs_surface_node.h>
#include "util.h"
#include <want.h>

#include "ability_start_setting.h"
#include "window_manager_hilog.h"
#include "session_helper.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "Session" };
std::atomic<int32_t> g_persistentId = INVALID_SESSION_ID;
std::set<int32_t> g_persistentIdSet;
} // namespace

int32_t Session::GetPersistentId() const
{
    return persistentId_;
}

int32_t Session::GetParentPersistentId() const
{
    if (property_ != nullptr) {
        return property_->GetParentPersistentId();
    }
    return INVALID_SESSION_ID;
}

void Session::SetParentPersistentId(int32_t parentId)
{
    if (property_ == nullptr) {
        return;
    }
    property_->SetParentPersistentId(parentId);
}

void Session::SetWindowSessionProperty(const sptr<WindowSessionProperty>& property)
{
    property_ = property;
}

sptr<WindowSessionProperty> Session::GetWindowSessionProperty() const
{
    return property_;
}

std::shared_ptr<RSSurfaceNode> Session::GetSurfaceNode() const
{
    return surfaceNode_;
}

std::shared_ptr<Media::PixelMap> Session::GetSnapshot() const
{
    return snapshot_;
}

SessionInfo& Session::GetSessionInfo()
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

void Session::NotifyDisconnect()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) {
            listener.lock()->OnDisconnect();
        }
    }
}

void Session::NotifyExtensionDied()
{
    auto lifecycleListeners = GetListeners<ILifecycleListener>();
    for (auto& listener : lifecycleListeners) {
        if (!listener.expired()) {
            listener.lock()->OnExtensionDied();
        }
    }
}

float Session::GetAspectRatio() const
{
    return aspectRatio_;
}

WSError Session::SetAspectRatio(float ratio)
{
    aspectRatio_ = ratio;
    return WSError::WS_OK;
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

void Session::UpdateSessionFocusable(bool isFocusable)
{
    property_->SetFocusable(isFocusable);
    NotifySessionFocusableChange(isFocusable);
}

void Session::UpdateSessionTouchable(bool touchable)
{
    property_->SetTouchable(touchable);
    NotifySessionTouchableChange(touchable);
}

WSError Session::SetFocusable(bool isFocusable)
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (isFocusable == property_->GetFocusable()) {
        WLOGFD("Session focusable do not change: [%{public}d]", isFocusable);
        return WSError::WS_DO_NOTHING;
    }
    UpdateSessionFocusable(isFocusable);
    return WSError::WS_OK;
}

bool Session::GetFocusable() const
{
    if (property_ != nullptr) {
        return property_->GetFocusable();
    }
    WLOGFD("property is null");
    return true;
}

void Session::SetNeedNotify(bool needNotify)
{
    needNotify_ = needNotify;
}

bool Session::NeedNotify() const
{
    return needNotify_;
}

WSError Session::SetTouchable(bool touchable)
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    UpdateSessionTouchable(touchable);
    return WSError::WS_OK;
}

bool Session::GetTouchable() const
{
    return property_->GetTouchable();
}

WSError Session::SetVisible(bool isVisible)
{
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    isVisible_ = isVisible;
    return WSError::WS_OK;
}

bool Session::GetVisible() const
{
    return isVisible_;
}

int32_t Session::GetWindowId() const
{
    return GetPersistentId();
}

void Session::SetCallingPid(int32_t id)
{
    callingPid_ = id;
}

void Session::SetCallingUid(int32_t id)
{
    callingUid_ = id;
}

int32_t Session::GetCallingPid() const
{
    return callingPid_;
}

int32_t Session::GetCallingUid() const
{
    return callingUid_;
}

void Session::SetAbilityToken(sptr<IRemoteObject> token)
{
    abilityToken_ = token;
}

sptr<IRemoteObject> Session::GetAbilityToken() const
{
    return abilityToken_;
}

WSError Session::SetBrightness(float brightness)
{
    if (!property_) {
        return WSError::WS_ERROR_NULLPTR;
    }
    property_->SetBrightness(brightness);
    return WSError::WS_OK;
}

float Session::GetBrightness() const
{
    if (!property_) {
        return UNDEFINED_BRIGHTNESS;
    }
    return property_->GetBrightness();
}

bool Session::IsSessionValid() const
{
    bool res = state_ > SessionState::STATE_DISCONNECT && state_ < SessionState::STATE_END;
    if (!res) {
        WLOGFI("session is already destroyed or not created! id: %{public}d state: %{public}u",
            GetPersistentId(), state_);
    }
    return res;
}

bool Session::IsActive() const
{
    return isActive_;
}

WSError Session::UpdateRect(const WSRect& rect, SizeChangeReason reason)
{
    WLOGFI("session update rect: id: %{public}d, rect[%{public}d, %{public}d, %{public}u, %{public}u], "\
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
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig, sptr<WindowSessionProperty> property, sptr<IRemoteObject> token)
{
    callingPid_ = IPCSkeleton::GetCallingPid();
    callingUid_ = IPCSkeleton::GetCallingUid();
    return ConnectImpl(sessionStage, eventChannel, surfaceNode, systemConfig, property, token);
}

WSError Session::ConnectImpl(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig, sptr<WindowSessionProperty> property, sptr<IRemoteObject> token)
{
    WLOGFI("Connect session, id: %{public}d, state: %{public}u", GetPersistentId(),
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
    abilityToken_ = token;
    systemConfig = systemConfig_;
    if (property) {
        property->SetPersistentId(GetPersistentId());
    }
    property_ = property;

    UpdateSessionState(SessionState::STATE_CONNECT);
    // once update rect before connect, update again when connect
    UpdateRect(winRect_, SizeChangeReason::UNDEFINED);
    NotifyConnect();
    callingBundleName_ = DelayedSingleton<ANRManager>::GetInstance()->GetBundleName(callingPid_, callingUid_);
    DelayedSingleton<ANRManager>::GetInstance()->SetApplicationInfo(persistentId_, callingPid_, callingBundleName_);
    return WSError::WS_OK;
}

WSError Session::UpdateWindowSessionProperty(sptr<WindowSessionProperty> property)
{
    property_ = property;
    return WSError::WS_OK;
}

WSError Session::Foreground(sptr<WindowSessionProperty> property)
{
    HandleDialogForeground();
    SessionState state = GetSessionState();
    WLOGFI("Foreground session, id: %{public}d, state: %{public}" PRIu32"", GetPersistentId(),
        static_cast<uint32_t>(state));
    if (state != SessionState::STATE_CONNECT && state != SessionState::STATE_BACKGROUND) {
        WLOGFE("state invalid!");
        return WSError::WS_ERROR_INVALID_SESSION;
    }

    UpdateSessionState(SessionState::STATE_FOREGROUND);
    if (!isActive_) {
        SetActive(true);
    }

    if (GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        NotifyCallingSessionForeground();
    }
    return WSError::WS_OK;
}

void Session::NotifyCallingSessionForeground()
{
    if (notifyCallingSessionForegroundFunc_) {
        WLOGFI("Notify calling window that input method shown");
        notifyCallingSessionForegroundFunc_(persistentId_);
    }
}

void Session::HandleDialogBackground()
{
    const auto& type = GetWindowType();
    if (type < WindowType::APP_MAIN_WINDOW_BASE || type >= WindowType::APP_MAIN_WINDOW_END) {
        WLOGFD("Current session is not main window, id: %{public}d, type: %{public}d", GetPersistentId(), type);
        return;
    }
    for (const auto& dialog : dialogVec_) {
        if (dialog == nullptr) {
            continue;
        }
        WLOGFD("Background dialog, id: %{public}d, dialogId: %{public}d", GetPersistentId(), dialog->GetPersistentId());
        dialog->SetActive(false);
        dialog->Background();
    }
}

void Session::HandleDialogForeground()
{
    const auto& type = GetWindowType();
    if (type < WindowType::APP_MAIN_WINDOW_BASE || type >= WindowType::APP_MAIN_WINDOW_END) {
        WLOGFD("Current session is not main window, id: %{public}d, type: %{public}d", GetPersistentId(), type);
    }
    for (const auto& dialog : dialogVec_) {
        if (dialog == nullptr) {
            continue;
        }
        WLOGFD("Foreground dialog, id: %{public}d, dialogId: %{public}d", GetPersistentId(), dialog->GetPersistentId());
        dialog->Foreground(dialog->GetSessionProperty());
    }
}

WSError Session::Background()
{
    HandleDialogBackground();
    SessionState state = GetSessionState();
    WLOGFI("Background session, id: %{public}d, state: %{public}" PRIu32"", GetPersistentId(),
        static_cast<uint32_t>(state));
    if (state != SessionState::STATE_INACTIVE) {
        WLOGFE("state invalid!");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    UpdateSessionState(SessionState::STATE_BACKGROUND);
    if (GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        NotifyCallingSessionBackground();
    }
    return WSError::WS_OK;
}

void Session::NotifyCallingSessionBackground()
{
    if (notifyCallingSessionBackgroundFunc_) {
        WLOGFI("Notify calling window that input method hide");
        notifyCallingSessionBackgroundFunc_();
    }
}

WSError Session::Disconnect()
{
    SessionState state = GetSessionState();
    WLOGFI("Disconnect session, id: %{public}d, state: %{public}" PRIu32"", GetPersistentId(),
        static_cast<uint32_t>(state));
    if (state == SessionState::STATE_ACTIVE) {
        snapshot_ = Snapshot();
        if (scenePersistence_ && snapshot_) {
            scenePersistence_->SaveSnapshot(snapshot_);
        }
    }
    UpdateSessionState(SessionState::STATE_BACKGROUND);
    UpdateSessionState(SessionState::STATE_DISCONNECT);
    NotifyDisconnect();
    snapshot_.reset();
    DelayedSingleton<ANRManager>::GetInstance()->OnSessionLost(persistentId_);
    return WSError::WS_OK;
}

WSError Session::SetActive(bool active)
{
    SessionState state = GetSessionState();
    WLOGFI("Session update active: %{public}d, id: %{public}d, state: %{public}" PRIu32"",
        active, GetPersistentId(), static_cast<uint32_t>(state));
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
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is null");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    sessionInfo_.startMethod = OHOS::Rosen::StartMethod::START_CALL;

    SessionInfo info;
    info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
    info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
    info.moduleName_ = abilitySessionInfo->want.GetModuleName();
    info.persistentId_ = abilitySessionInfo->persistentId;
    info.callerPersistentId_ = GetPersistentId();
    info.callState_ = static_cast<uint32_t>(abilitySessionInfo->state);
    info.uiAbilityId_ = abilitySessionInfo->uiAbilityId;
    info.want = new AAFwk::Want(abilitySessionInfo->want);
    info.requestCode = abilitySessionInfo->requestCode;
    info.callerToken_ = abilitySessionInfo->callerToken;
    info.startSetting = abilitySessionInfo->startSetting;
    info.callingTokenId_ = abilitySessionInfo->callingTokenId;
    info.reuse = abilitySessionInfo->reuse;
    WLOGFI("PendingSessionActivation:bundleName %{public}s, moduleName:%{public}s, abilityName:%{public}s",
        info.bundleName_.c_str(), info.moduleName_.c_str(), info.abilityName_.c_str());
    WLOGFI("PendingSessionActivation callState:%{public}d, want persistentId: %{public}d, callingTokenId:%{public}d, \
        uiAbilityId: %{public}" PRIu64 "", info.callState_, info.persistentId_, info.callingTokenId_,
        info.uiAbilityId_);
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
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is null");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    SessionInfo info;
    info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
    info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
    info.callerToken_ = abilitySessionInfo->callerToken;
    info.persistentId_ = static_cast<int32_t>(abilitySessionInfo->persistentId);
    sessionInfo_.want = new AAFwk::Want(abilitySessionInfo->want);
    sessionInfo_.resultCode = abilitySessionInfo->resultCode;
    if (terminateSessionFunc_) {
        terminateSessionFunc_(info);
    }
    return WSError::WS_OK;
}

void Session::SetTerminateSessionListener(const NotifyTerminateSessionFunc& func)
{
    terminateSessionFunc_ = func;
}

WSError Session::TerminateSessionNew(const sptr<AAFwk::SessionInfo> abilitySessionInfo, bool needStartCaller)
{
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is null");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    SessionInfo info;
    info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
    info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
    info.callerToken_ = abilitySessionInfo->callerToken;
    info.persistentId_ = static_cast<int32_t>(abilitySessionInfo->persistentId);
    sessionInfo_.want = new AAFwk::Want(abilitySessionInfo->want);
    sessionInfo_.resultCode = abilitySessionInfo->resultCode;
    if (terminateSessionFuncNew_) {
        terminateSessionFuncNew_(info, needStartCaller);
    }
    return WSError::WS_OK;
}

void Session::SetTerminateSessionListenerNew(const NotifyTerminateSessionFuncNew& func)
{
    terminateSessionFuncNew_ = func;
}

WSError Session::TerminateSessionTotal(const sptr<AAFwk::SessionInfo> abilitySessionInfo, TerminateType terminateType)
{
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is null");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    SessionInfo info;
    info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
    info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
    info.callerToken_ = abilitySessionInfo->callerToken;
    info.persistentId_ = static_cast<int32_t>(abilitySessionInfo->persistentId);
    sessionInfo_.want = new AAFwk::Want(abilitySessionInfo->want);
    sessionInfo_.resultCode = abilitySessionInfo->resultCode;
    if (terminateSessionFuncTotal_) {
        terminateSessionFuncTotal_(info, terminateType);
    }
    return WSError::WS_OK;
}

void Session::SetTerminateSessionListenerTotal(const NotifyTerminateSessionFuncTotal& func)
{
    terminateSessionFuncTotal_ = func;
}

WSError Session::Clear()
{
    SessionInfo info = GetSessionInfo();
    if (terminateSessionFuncTotal_) {
        terminateSessionFuncTotal_(info, TerminateType::CLOSE_AND_CLEAR_MULTITASK);
    }
    return WSError::WS_OK;
}

WSError Session::NotifySessionException(const sptr<AAFwk::SessionInfo> abilitySessionInfo)
{
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is null");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    SessionInfo info;
    info.abilityName_ = abilitySessionInfo->want.GetElement().GetAbilityName();
    info.bundleName_ = abilitySessionInfo->want.GetElement().GetBundleName();
    info.callerToken_ = abilitySessionInfo->callerToken;
    info.errorCode = abilitySessionInfo->errorCode;
    info.errorReason = abilitySessionInfo->errorReason;
    info.persistentId_ = static_cast<int32_t>(abilitySessionInfo->persistentId);
    sessionInfo_.want = new AAFwk::Want(abilitySessionInfo->want);
    sessionInfo_.errorCode = abilitySessionInfo->errorCode;
    sessionInfo_.errorReason = abilitySessionInfo->errorReason;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!sessionExceptionFuncs_.empty()) {
        for (std::shared_ptr<NotifySessionExceptionFunc> funcSptr: sessionExceptionFuncs_) {
            auto sessionExceptionFunc = *funcSptr;
            sessionExceptionFunc(info);
        }
    }
    return WSError::WS_OK;
}

void Session::SetSessionExceptionListener(const NotifySessionExceptionFunc& func)
{
    if (func == nullptr) {
        WLOGFE("func is nullptr");
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    std::shared_ptr<NotifySessionExceptionFunc> funcSptr = std::make_shared<NotifySessionExceptionFunc>(func);
    if (std::find(sessionExceptionFuncs_.begin(), sessionExceptionFuncs_.end(), funcSptr) !=
        sessionExceptionFuncs_.end()) {
        WLOGFW("func already regitered");
        return;
    }
    sessionExceptionFuncs_.emplace_back(funcSptr);
}

void Session::SetPendingSessionToForegroundListener(const NotifyPendingSessionToForegroundFunc& func)
{
    pendingSessionToForegroundFunc_ = func;
}

WSError Session::PendingSessionToForeground()
{
    WLOGFI("run PendingSessionToForeground");
    SessionInfo info = GetSessionInfo();
    if (pendingSessionActivationFunc_) {
        pendingSessionActivationFunc_(info);
    }
    return WSError::WS_OK;
}

void Session::SetPendingSessionToBackgroundForDelegatorListener(
    const NotifyPendingSessionToBackgroundForDelegatorFunc& func)
{
    pendingSessionToBackgroundForDelegatorFunc_ = func;
}

WSError Session::PendingSessionToBackgroundForDelegator()
{
    WLOGFI("run PendingSessionToBackgroundForDelegator");
    SessionInfo info = GetSessionInfo();
    if (pendingSessionToBackgroundForDelegatorFunc_) {
        pendingSessionToBackgroundForDelegatorFunc_(info);
    }
    return WSError::WS_OK;
}

void Session::SetNotifyCallingSessionForegroundFunc(const NotifyCallingSessionForegroundFunc& func)
{
    notifyCallingSessionForegroundFunc_ = func;
}

void Session::SetNotifyCallingSessionBackgroundFunc(const NotifyCallingSessionBackgroundFunc& func)
{
    notifyCallingSessionBackgroundFunc_ = func;
}

void Session::NotifyTouchDialogTarget()
{
    if (!sessionStage_) {
        return;
    }
    sessionStage_->NotifyTouchDialogTarget();
}

void Session::NotifyScreenshot()
{
    if (!sessionStage_) {
        return;
    }
    sessionStage_->NotifyScreenshot();
}

WSError Session::NotifyDestroy()
{
    if (!sessionStage_) {
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->NotifyDestroy();
}

void Session::SetSessionContinueState(const ContinueState& continueState)
{
    auto sessionInfo = GetSessionInfo();
    WLOGFD("SetSessionContinueState, continueState : %{public}d", static_cast<int32_t>(continueState));
    sessionInfo.continueState = continueState;
}

void Session::SetParentSession(const sptr<Session>& session)
{
    parentSession_ = session;
}

void Session::BindDialogToParentSession(const sptr<Session>& session)
{
    auto iter = std::find(dialogVec_.begin(), dialogVec_.end(), session);
    if (iter != dialogVec_.end()) {
        WLOGFW("Dialog is existed in parentVec, id: %{public}d, parentId: %{public}d",
            session->GetPersistentId(), GetPersistentId());
        return;
    }
    dialogVec_.push_back(session);
    WLOGFD("Bind dialog success, id: %{public}d, parentId: %{public}d",
        session->GetPersistentId(), GetPersistentId());
}

void Session::RemoveDialogToParentSession(const sptr<Session>& session)
{
    auto iter = std::find(dialogVec_.begin(), dialogVec_.end(), session);
    if (iter != dialogVec_.end()) {
        WLOGFD("Remove dialog success, id: %{public}d, parentId: %{public}d",
            session->GetPersistentId(), GetPersistentId());
        dialogVec_.erase(iter);
    }
    WLOGFW("Remove dialog failed, id: %{public}d, parentId: %{public}d",
        session->GetPersistentId(), GetPersistentId());
}

std::vector<sptr<Session>> Session::GetDialogVector() const
{
    return dialogVec_;
}

bool Session::CheckDialogOnForeground()
{
    if (dialogVec_.empty()) {
        return false;
    }
    for (auto dialogSession : dialogVec_) {
        if (dialogSession && dialogSession->GetSessionState() == SessionState::STATE_ACTIVE) {
            dialogSession->NotifyTouchDialogTarget();
            WLOGFD("Notify touch dialog window");
            return true;
        }
    }
    return false;
}

WSError Session::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        if (CheckDialogOnForeground()) {
            WLOGFD("Has dialog on foreground, not transfer pointer event");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
    } else if (GetWindowType() == WindowType::WINDOW_TYPE_APP_SUB_WINDOW) {
        if (parentSession_ && parentSession_->CheckDialogOnForeground()) {
            WLOGFD("Its main window has dialog on foreground, not transfer pointer event");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
    }
    if (pointerEvent == nullptr) {
        WLOGFE("PointerEvent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    WLOGFD("Session TransferPointEvent, eventId:%{public}d, persistentId:%{public}d, "
        "bundleName:%{public}s, pid:%{public}d",
        pointerEvent->GetId(), persistentId_, callingBundleName_.c_str(), callingPid_);
    if (DelayedSingleton<ANRManager>::GetInstance()->IsANRTriggered(persistentId_)) {
        WLOGFD("The pointerEvent does not report normally, bundleName:%{public}s not response, pid:%{public}d",
            callingBundleName_.c_str(), callingPid_);
        return WSError::WS_DO_NOTHING;
    }
    auto action = pointerEvent->GetPointerAction();
    if (!isFocused_ && GetFocusable() && (action == MMI::PointerEvent::POINTER_ACTION_DOWN ||
    action == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN)) {
        NotifyClick();
    }
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (WSError ret = windowEventChannel_->TransferPointerEvent(pointerEvent); ret != WSError::WS_OK) {
        WLOGFE("TransferPointer failed");
        return ret;
    }
    auto pointerAction = pointerEvent->GetPointerAction();
    if (pointerAction == MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_PULL_IN_WINDOW ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_PULL_OUT_WINDOW) {
        WLOGFD("Action:%{public}s, eventId:%{public}d, report without timer",
        pointerEvent->DumpPointerAction(), pointerEvent->GetId());
    } else {
        DelayedSingleton<ANRManager>::GetInstance()->AddTimer(pointerEvent->GetId(), persistentId_);
    }
    return WSError::WS_OK;
}

WSError Session::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        if (CheckDialogOnForeground()) {
            WLOGFD("Has dialog on foreground, not transfer pointer event");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
    } else if (GetWindowType() == WindowType::WINDOW_TYPE_APP_SUB_WINDOW) {
        if (parentSession_ && parentSession_->CheckDialogOnForeground()) {
            WLOGFD("Its main window has dialog on foreground, not transfer pointer event");
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
    }
    if (keyEvent == nullptr) {
        WLOGFE("KeyEvent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    WLOGFD("Session TransferKeyEvent, eventId:%{public}d, persistentId:%{public}d, "
        "bundleName:%{public}s, pid:%{public}d",
        keyEvent->GetId(), persistentId_, callingBundleName_.c_str(), callingPid_);
    if (DelayedSingleton<ANRManager>::GetInstance()->IsANRTriggered(persistentId_)) {
        WLOGFD("The keyEvent does not report normally, bundleName:%{public}s not response, pid:%{public}d",
            callingBundleName_.c_str(), callingPid_);
        return WSError::WS_DO_NOTHING;
    }
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    WLOGD("TransferKeyEvent, id: %{public}d", persistentId_);
    if (WSError ret = windowEventChannel_->TransferKeyEvent(keyEvent); ret != WSError::WS_OK) {
        WLOGFE("TransferKeyEvent failed");
        return ret;
    }
    DelayedSingleton<ANRManager>::GetInstance()->AddTimer(keyEvent->GetId(), persistentId_);
    return WSError::WS_OK;
}

WSError Session::TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed)
{
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (keyEvent == nullptr) {
        WLOGFE("KeyEvent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferKeyEventForConsumed(keyEvent, isConsumed);
}

WSError Session::TransferFocusActiveEvent(bool isFocusActive)
{
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferFocusActiveEvent(isFocusActive);
}

WSError Session::TransferFocusStateEvent(bool focusState)
{
    if (!windowEventChannel_) {
        WLOGFE("windowEventChannel_ is null");
        return WSError::WS_ERROR_NULLPTR;
    }
    return windowEventChannel_->TransferFocusState(focusState);
}

std::shared_ptr<Media::PixelMap> Session::Snapshot()
{
    auto callback = std::make_shared<SurfaceCaptureFuture>();
    constexpr float scale = 0.5;
    bool ret = RSInterfaces::GetInstance().TakeSurfaceCapture(surfaceNode_, callback, scale, scale);
    if (!ret) {
        WLOGFE("TakeSurfaceCapture failed");
        return nullptr;
    }
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
    NotifySessionStateChange(state_);
}

void Session::SetSessionStateChangeNotifyManagerListener(const NotifySessionStateChangeNotifyManagerFunc& func)
{
    sessionStateChangeNotifyManagerFunc_ = func;
    NotifySessionStateChange(state_);
}

void Session::NotifySessionStateChange(const SessionState& state)
{
    WLOGFI("state: %{public}u", static_cast<uint32_t>(state));
    if (sessionStateChangeFunc_) {
        sessionStateChangeFunc_(state);
    }
    if (sessionStateChangeNotifyManagerFunc_) {
        sessionStateChangeNotifyManagerFunc_(GetPersistentId(), state);
    }
}

void Session::SetSessionFocusableChangeListener(const NotifySessionFocusableChangeFunc& func)
{
    sessionFocusableChangeFunc_ = func;
    sessionFocusableChangeFunc_(GetFocusable());
}

void Session::SetSessionTouchableChangeListener(const NotifySessionTouchableChangeFunc& func)
{
    sessionTouchableChangeFunc_ = func;
    sessionTouchableChangeFunc_(GetTouchable());
}

void Session::SetClickListener(const NotifyClickFunc& func)
{
    clickFunc_ = func;
}

void Session::NotifySessionFocusableChange(bool isFocusable)
{
    WLOGFI("Notify session focusable change: %{public}u", isFocusable);
    if (sessionFocusableChangeFunc_) {
        sessionFocusableChangeFunc_(isFocusable);
    }
}

void Session::NotifySessionTouchableChange(bool touchable)
{
    WLOGFI("Notify session touchable change: %{public}u", touchable);
    if (sessionTouchableChangeFunc_) {
        sessionTouchableChangeFunc_(touchable);
    }
}

void Session::NotifyClick()
{
    WLOGFI("Notify click");
    if (clickFunc_) {
        clickFunc_();
    }
}

WSError Session::UpdateFocus(bool isFocused)
{
    WLOGFI("Session update focus id: %{public}d", GetPersistentId());
    if (isFocused_ == isFocused) {
        WLOGFD("Session focus do not change: [%{public}d]", isFocused);
        return WSError::WS_DO_NOTHING;
    }
    isFocused_ = isFocused;
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    sessionStage_->UpdateFocus(isFocused);

    return WSError::WS_OK;
}

WSError Session::UpdateWindowMode(WindowMode mode)
{
    WLOGFI("Session update window mode, id: %{public}d, mode: %{public}d", GetPersistentId(),
        static_cast<int32_t>(mode));
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    return sessionStage_->UpdateWindowMode(mode);
}

void Session::SetSessionRect(const WSRect& rect)
{
    winRect_ = rect;
}

WSRect Session::GetSessionRect() const
{
    return winRect_;
}

void Session::SetSessionRequestRect(const WSRect& rect)
{
    if (property_ == nullptr) {
        WLOGFD("id: %{public}d property is nullptr", persistentId_);
        return;
    }
    property_->SetRequestRect(SessionHelper::TransferToRect(rect));
    WLOGFD("is: %{public}d, rect: [%{public}d, %{public}d, %{public}u, %{public}u]", persistentId_,
        rect.posX_, rect.posY_, rect.width_, rect.height_);
}

WSRect Session::GetSessionRequestRect() const
{
    WSRect rect;
    if (property_ == nullptr) {
        WLOGFD("id: %{public}d property is nullptr", persistentId_);
        return rect;
    }
    rect = SessionHelper::TransferToWSRect(property_->GetRequestRect());
    WLOGFD("is: %{public}d, rect: [%{public}d, %{public}d, %{public}u, %{public}u]", persistentId_,
        rect.posX_, rect.posY_, rect.width_, rect.height_);
    return rect;
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

WSError Session::OnNeedAvoid(bool status)
{
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
    sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session, sptr<IRemoteObject> token)
{
    return WSError::WS_OK;
}

WSError Session::DestroyAndDisconnectSpecificSession(const int32_t& persistentId)
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
    int32_t persistentId = GetPersistentId();
    WLOGFI("persistentId:%{public}d, eventId:%{public}d", persistentId, eventId);
    DelayedSingleton<ANRManager>::GetInstance()->MarkProcessed(eventId, persistentId);
    return WSError::WS_OK;
}

void Session::GeneratePersistentId(bool isExtension, const SessionInfo& sessionInfo)
{
    if (sessionInfo.persistentId_ != INVALID_SESSION_ID) {
        g_persistentIdSet.insert(sessionInfo.persistentId_);
        persistentId_ = sessionInfo.persistentId_;
        return;
    }

    if (g_persistentId == INVALID_SESSION_ID) {
        g_persistentId++; // init non system session id from 2
    }

    g_persistentId++;
    while (g_persistentIdSet.count(g_persistentId)) {
        g_persistentId++;
    }
    persistentId_ = isExtension ? static_cast<uint32_t>(
        g_persistentId.load()) | 0x40000000 : static_cast<uint32_t>(g_persistentId.load()) & 0x3fffffff;
    g_persistentIdSet.insert(g_persistentId);
}

sptr<ScenePersistence> Session::GetScenePersistence() const
{
    return scenePersistence_;
}

WSError Session::SetGlobalMaximizeMode(MaximizeMode mode)
{
    WLOGFD("Session SetGlobalMaximizeMode");
    return WSError::WS_OK;
}

WSError Session::GetGlobalMaximizeMode(MaximizeMode& mode)
{
    WLOGFD("Session GetGlobalMaximizeMode");
    return WSError::WS_OK;
}

AvoidArea Session::GetAvoidAreaByType(AvoidAreaType type)
{
    AvoidArea avoidArea;
    return avoidArea;
}

WSError Session::TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want)
{
    return WSError::WS_OK;
}

WSError Session::TransferExtensionData(const AAFwk::WantParams& wantParams)
{
    return WSError::WS_OK;
}

void Session::NotifyOccupiedAreaChangeInfo(sptr<OccupiedAreaChangeInfo> info)
{
    if (!sessionStage_) {
        WLOGFE("session stage is nullptr");
        return;
    }
    sessionStage_->NotifyOccupiedAreaChangeInfo(info);
}

void Session::NotifyRemoteReady()
{
    return;
}

WindowMode Session::GetWindowMode()
{
    if (property_ == nullptr) {
        WLOGFW("null property.");
        return WindowMode::WINDOW_MODE_UNDEFINED;
    }
    return property_->GetWindowMode();
}

void Session::SetZOrder(uint32_t zOrder)
{
    zOrder_ = zOrder;
}

uint32_t Session::GetZOrder() const
{
    return zOrder_;
}

void Session::SetUINodeId(uint32_t uiNodeId)
{
    uiNodeId_ = uiNodeId;
}

uint32_t Session::GetUINodeId() const
{
    return uiNodeId_;
}

WSError Session::UpdateWindowAnimationFlag(bool needDefaultAnimationFlag)
{
    WLOGFD("UpdateWindowAnimationFlag");
    return WSError::WS_OK;
}

WSError Session::UpdateWindowSceneAfterCustomAnimation(bool isAdd)
{
    return WSError::WS_OK;
}

void Session::SetShowRecent(bool showRecent)
{
    showRecent_ = showRecent;
}

bool Session::GetShowRecent() const
{
    return showRecent_;
}

void Session::SetBufferAvailable(bool bufferAvailable)
{
    bufferAvailable_ = bufferAvailable;
}

bool Session::GetBufferAvailable() const
{
    return bufferAvailable_;
}

int32_t Session::GetCollaboratorType() const
{
    return collaboratorType_;
}

void Session::SetCollaboratorType(int32_t collaboratorType)
{
    collaboratorType_ = collaboratorType;
}

std::shared_ptr<AppExecFwk::AbilityInfo> Session::GetAbilityInfo()
{
    SessionInfo& sessionInfo = GetSessionInfo();
    return sessionInfo.abilityInfo;
}

void Session::SetAbilitySessionInfo(std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo)
{
    SessionInfo& sessionInfo = GetSessionInfo();
    sessionInfo.abilityInfo = abilityInfo;
}

void Session::UpdateBrokerPersistentId(int32_t persistendId)
{
    brokerPersistentId_ = persistendId;
}

int32_t Session::GetBrokerPersistentId()
{
    return brokerPersistentId_;
}

} // namespace OHOS::Rosen
