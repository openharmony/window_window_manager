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
#include "session/host/include/scene_session.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSession" };
}

SceneSession::SceneSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : Session(info)
{
    specificCallback_ = specificCallback;
}

WSError SceneSession::OnSessionEvent(SessionEvent event)
{
    WLOGFD("SceneSession OnSessionEvent event: %{public}d", static_cast<int32_t>(event));
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->OnSessionEvent_) {
        sessionChangeCallback_->OnSessionEvent_(static_cast<uint32_t>(event));
    }
    return WSError::WS_OK;
}

void SceneSession::RegisterSessionChangeCallback(const sptr<SceneSession::SessionChangeCallback>&
    sessionChangeCallback)
{
    sessionChangeCallback_ = sessionChangeCallback;
}

WSError SceneSession::UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason)
{
    WLOGFI("UpdateSessionRect [%{public}d, %{public}d, %{public}u, %{public}u]", rect.posX_, rect.posY_,
        rect.width_, rect.height_);
    SetSessionRect(rect);
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onRectChange_) {
        sessionChangeCallback_->onRectChange_(rect);
    }
    UpdateRect(rect, reason);
    return WSError::WS_OK;
}

WSError SceneSession::RaiseToAppTop()
{
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onRaiseToTop_) {
        sessionChangeCallback_->onRaiseToTop_();
    }
    return WSError::WS_OK;
}

WSError SceneSession::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, uint64_t& persistentId, sptr<ISession>& session)
{
    WLOGFI("CreateAndConnectSpecificSession id: %{public}" PRIu64 "", GetPersistentId());
    sptr<SceneSession> sceneSession;
    if (specificCallback_ != nullptr) {
        sceneSession = specificCallback_->onCreate_(sessionInfo_);
    }
    if (sceneSession == nullptr) {
        return WSError::WS_ERROR_NULLPTR;
    }
    // connect specific session and sessionStage
    WSError errCode = sceneSession->Connect(sessionStage, eventChannel, surfaceNode, persistentId, property);
    if (sessionChangeCallback_ != nullptr && sessionChangeCallback_->onCreateSpecificSession_) {
        sessionChangeCallback_->onCreateSpecificSession_(sceneSession);
    }
    session = sceneSession;
    return errCode;
}

WSError SceneSession::DestroyAndDisconnectSpecificSession(const uint64_t& persistentId)
{
    WSError ret = WSError::WS_OK;
    if (specificCallback_ != nullptr) {
        ret = specificCallback_->onDestroy_(persistentId);
    }
    return ret;
}
} // namespace OHOS::Rosen
