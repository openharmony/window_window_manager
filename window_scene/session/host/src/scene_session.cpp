/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "scene_session.h"

#include "window_scene_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSession"};
}

SceneSession::SceneSession(const SceneAbilityInfo& info) : Session(info.bundleName_), abilityInfo_(info)
{
}

void SceneSession::SetStartSceneEventListener(const NotifyStartSceneFunc& func)
{
    startSceneFunc_ = func;
}

WSError SceneSession::Connect(const sptr<ISceneSessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel)
{
    WLOGFI("Connect session, id: %{public}u, state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()));
    if (GetSessionState() != SessionState::STATE_DISCONNECT) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    sceneSessionStage_ = sessionStage;
    windowEventChannel_ = eventChannel;
    if (sceneSessionStage_ == nullptr || windowEventChannel_ == nullptr) {
        WLOGFE("session stage or eventChannel is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    UpdateSessionState(SessionState::STATE_CONNECT);
    UpdateSessionRect(winRect_, SessionSizeChangeReason::SHOW);
    return WSError::WS_OK;
}

WSError SceneSession::Foreground()
{
    WLOGFI("Foreground session, id: %{public}u, state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()));
    if (GetSessionState() != SessionState::STATE_CONNECT) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    UpdateSessionState(SessionState::STATE_FOREGROUND);
    if (isActive_) {
        sceneSessionStage_->SetActive(true);
        UpdateSessionState(SessionState::STATE_ACTIVE);
    }
    return WSError::WS_OK;
}

WSError SceneSession::Background()
{
    WLOGFI("Background session, id: %{public}u, state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()));
    if (GetSessionState() < SessionState::STATE_FOREGROUND) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    SessionState state = GetSessionState();
    switch (state) {
        case SessionState::STATE_FOREGROUND:
            SetActive(true); // go through
        case SessionState::STATE_ACTIVE:
            SetActive(false); // go through
        default:
            break;
    }
    if (state == SessionState::STATE_INACTIVE) {
        UpdateSessionState(SessionState::STATE_BACKGROUND);
    }
    return WSError::WS_OK;
}

WSError SceneSession::Disconnect()
{
    WLOGFI("Disconnect session, id: %{public}u, state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()));
    Background();
    if (GetSessionState() == SessionState::STATE_BACKGROUND) {
        UpdateSessionState(SessionState::STATE_DISCONNECT);
    }
    return WSError::WS_OK;
}

// TODO: may be delete
WSError SceneSession::Minimize()
{
    WLOGFI("Minimize session, id: %{public}u, state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()));
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    // TODO: native->ts minimize
    return WSError::WS_OK;
}

// TODO: may be delete
WSError SceneSession::Close()
{
    WLOGFI("Close session, id: %{public}u, state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()));
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    // TODO: native->ts close
    return WSError::WS_OK;
}

// TODO: may be delete
WSError SceneSession::Recover()
{
    WLOGFI("Recover session, id: %{public}u, state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()));
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    // TODO: native->ts recover
    return WSError::WS_OK;
}

// TODO: may be delete
WSError SceneSession::Maximum()
{
    WLOGFI("Maximum session, id: %{public}u, state: %{public}u", GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()));
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    // TODO: native->ts maximum
    return WSError::WS_OK;
}

WSError SceneSession::StartScene(const SceneAbilityInfo& info, SessionOption sessionOption)
{
    if (startSceneFunc_) {
        startSceneFunc_(info, sessionOption);
    }
    return WSError::WS_OK;
}

WSError SceneSession::SetActive(bool active)
{
    WLOGFI("Session update active: %{public}d, id: %{public}u, state: %{public}u", active, GetPersistentId(),
        static_cast<uint32_t>(GetSessionState()));
    if (!IsSessionValid()) {
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    if (active == isActive_) {
        WLOGFI("Session active do not change: [%{public}d]", active);
        return WSError::WS_DO_NOTHING;
    }
    isActive_ = active;
    if (active && GetSessionState() == SessionState::STATE_FOREGROUND) {
        sceneSessionStage_->SetActive(true);
        UpdateSessionState(SessionState::STATE_ACTIVE);
    }
    if (!active && GetSessionState() == SessionState::STATE_ACTIVE) {
        sceneSessionStage_->SetActive(false);
        UpdateSessionState(SessionState::STATE_INACTIVE);
    }
    return WSError::WS_OK;
}

WSError SceneSession::UpdateSessionRect(const WSRect& rect, SessionSizeChangeReason reason)
{
    WLOGFI("session update rect: id: %{public}u, rect[%{public}d, %{public}d, %{public}u, %{public}u], "\
        "reason:%{public}u", GetPersistentId(), rect.posX_, rect.posY_, rect.width_, rect.height_, reason);
    if (!IsSessionValid()) {
        winRect_ = rect;
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    sceneSessionStage_->UpdateSessionRect(rect, reason);
    winRect_ = rect;
    return WSError::WS_OK;
}

const SceneAbilityInfo& SceneSession::GetAbilityInfo() const
{
    return abilityInfo_;
}
}
