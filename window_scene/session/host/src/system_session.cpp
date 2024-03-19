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
#include "common/include/session_permission.h"
#include "session/host/include/session.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SystemSession" };
} // namespace

SystemSession::SystemSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : SceneSession(info, specificCallback)
{
    TLOGD(WmsLogTag::WMS_LIFE, "Create SystemSession");
    SetMoveDragCallback();
}

SystemSession::~SystemSession()
{
    TLOGD(WmsLogTag::WMS_LIFE, " ~SystemSession, id: %{public}d", GetPersistentId());
}

void SystemSession::UpdateCameraFloatWindowStatus(bool isShowing)
{
    if (GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA && specificCallback_ != nullptr) {
        TLOGI(WmsLogTag::WMS_SYSTEM, "CameraFloat status: %{public}d, id: %{public}d", isShowing, GetPersistentId());
        specificCallback_->onCameraFloatSessionChange_(GetSessionProperty()->GetAccessTokenId(), isShowing);
    }
}

WSError SystemSession::Show(sptr<WindowSessionProperty> property)
{
    auto type = GetWindowType();
    if (((type == WindowType::WINDOW_TYPE_TOAST) || (type == WindowType::WINDOW_TYPE_FLOAT)) &&
        !SessionPermission::IsSystemCalling()) {
        auto parentSession = GetParentSession();
        if (parentSession == nullptr) {
            WLOGFW("parent session is null");
            return WSError::WS_ERROR_INVALID_PARENT;
        }
        if (!parentSession->IsSessionForeground()) {
            WLOGFW("parent session is not in foreground");
            return WSError::WS_ERROR_INVALID_OPERATION;
        }
    }
    auto task = [weakThis = wptr(this), property]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGI(WmsLogTag::WMS_LIFE, "Show session, id: %{public}d", session->GetPersistentId());

        // use property from client
        if (property && property->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
            session->GetSessionProperty()->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
            session->NotifyIsCustomAnimationPlaying(true);
        }
        session->UpdateCameraFloatWindowStatus(true);
        auto ret = session->SceneSession::Foreground(property);
        return ret;
    };
    PostTask(task, "Show");
    return WSError::WS_OK;
}

WSError SystemSession::Hide()
{
    auto type = GetWindowType();
    if (WindowHelper::IsSystemWindow(type) && Session::NeedSystemPermission(type)) {
        if (!SessionPermission::IsSystemCalling()) {
            TLOGE(WmsLogTag::WMS_LIFE, "Hide permission denied id: %{public}d type:%{public}u",
                GetPersistentId(), type);
            return WSError::WS_ERROR_INVALID_PERMISSION;
        }
    }
    auto task = [weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_LIFE, "session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGI(WmsLogTag::WMS_LIFE, "Hide session, id: %{public}d", session->GetPersistentId());

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
    };
    PostTask(task, "Hide");
    return WSError::WS_OK;
}

WSError SystemSession::Disconnect(bool isFromClient)
{
    auto task = [weakThis = wptr(this), isFromClient]() {
        auto session = weakThis.promote();
        if (!session) {
            TLOGE(WmsLogTag::WMS_LIFE, "session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        TLOGI(WmsLogTag::WMS_LIFE, "Disconnect session, id: %{public}d", session->GetPersistentId());
        session->SceneSession::Disconnect(isFromClient);
        if (session->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
            session->NotifyCallingSessionBackground();
        }
        session->UpdateCameraFloatWindowStatus(false);
        return WSError::WS_OK;
    };
    PostTask(task, "Disconnect");
    return WSError::WS_OK;
}

WSError SystemSession::ProcessPointDownSession(int32_t posX, int32_t posY)
{
    const auto& id = GetPersistentId();
    const auto& type = GetWindowType();
    WLOGFI("id: %{public}d, type: %{public}d", id, type);
    if (parentSession_ && parentSession_->CheckDialogOnForeground()) {
        WLOGFI("Parent has dialog foreground, id: %{public}d, type: %{public}d", id, type);
        parentSession_->HandlePointDownDialog();
        if (!IsTopDialog()) {
            return WSError::WS_OK;
        }
    }
    if (type == WindowType::WINDOW_TYPE_DIALOG && GetSessionProperty() && GetSessionProperty()->GetRaiseEnabled()) {
        RaiseToAppTopForPointDown();
    }
    PresentFocusIfPointDown();
    return SceneSession::ProcessPointDownSession(posX, posY);
}
} // namespace OHOS::Rosen
