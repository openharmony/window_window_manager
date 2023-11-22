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

#include "session/host/include/system_session.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SystemSession" };
} // namespace

SystemSession::SystemSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : SceneSession(info, specificCallback)
{
    WLOGFD("Create SystemSession");
}

SystemSession::~SystemSession()
{
    WLOGD("~SystemSession, id: %{public}d", GetPersistentId());
}

void SystemSession::UpdateCameraFloatWindowStatus(bool isShowing)
{
    if (GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA && specificCallback_ != nullptr) {
        WLOGFD("CameraFloat status: %{public}d, id: %{public}d", isShowing, GetPersistentId());
        specificCallback_->onCameraFloatSessionChange_(GetSessionProperty()->GetAccessTokenId(), isShowing);
    }
}

WSError SystemSession::Show(sptr<WindowSessionProperty> property)
{
    PostTask([weakThis = wptr(this), property]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WLOGFD("Show session, id: %{public}d", session->GetPersistentId());

        // use property from client
        if (property && property->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
            session->GetSessionProperty()->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
            session->NotifyIsCustomAnimationPlaying(true);
        }
        session->UpdateCameraFloatWindowStatus(true);
        auto ret = session->SceneSession::Foreground(property);
        return ret;
    });
    return WSError::WS_OK;
}

WSError SystemSession::Hide()
{
    PostTask([weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WLOGFD("Hide session, id: %{public}d", session->GetPersistentId());

        auto ret = session->SetActive(false);
        if (ret != WSError::WS_OK) {
            return ret;
        }
        // background will remove surfaceNode, custom not execute
        // not animation playing when already background; inactive may be animation playing
        if (session->GetSessionProperty() &&
            session->GetSessionProperty()->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
            session->NotifyIsCustomAnimationPlaying(true);
            return WSError::WS_OK;
        }
        session->UpdateCameraFloatWindowStatus(false);
        ret = session->SceneSession::Background();
        return ret;
    });
    return WSError::WS_OK;
}

WSError SystemSession::Disconnect()
{
    PostTask([weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        session->SceneSession::Disconnect();
        if (session->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
            session->NotifyCallingSessionBackground();
        }
        session->UpdateCameraFloatWindowStatus(false);
        return WSError::WS_OK;
    });
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
