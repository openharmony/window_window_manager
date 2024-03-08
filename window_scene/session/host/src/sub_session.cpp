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

#include "session/host/include/sub_session.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SubSession" };
} // namespace

SubSession::SubSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
    : SceneSession(info, specificCallback)
{
    moveDragController_ = new (std::nothrow) MoveDragController(GetPersistentId());
    SetMoveDragCallback();
    WLOGFD("[WMSLife] Create SubSession");
}

SubSession::~SubSession()
{
    WLOGD("[WMSLife] ~SubSession, id: %{public}d", GetPersistentId());
}

WSError SubSession::Show(sptr<WindowSessionProperty> property)
{
    auto task = [weakThis = wptr(this), property]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("[WMSSub] session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WLOGFI("[WMSLife] Show session, id: %{public}d", session->GetPersistentId());

        // use property from client
        if (property && property->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM)) {
            session->GetSessionProperty()->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
            session->NotifyIsCustomAnimationPlaying(true);
        }
        auto ret = session->SceneSession::Foreground(property);
        return ret;
    };
    PostTask(task, "Show");
    return WSError::WS_OK;
}

WSError SubSession::Hide()
{
    auto task = [weakThis = wptr(this)]() {
        auto session = weakThis.promote();
        if (!session) {
            WLOGFE("[WMSSub] session is null");
            return WSError::WS_ERROR_DESTROYED_OBJECT;
        }
        WLOGFI("[WMSLife] Hide session, id: %{public}d", session->GetPersistentId());
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
        ret = session->SceneSession::Background();
        return ret;
    };
    PostTask(task, "Hide");
    return WSError::WS_OK;
}

WSError SubSession::ProcessPointDownSession(int32_t posX, int32_t posY)
{
    const auto& id = GetPersistentId();
    WLOGFI("id: %{public}d, type: %{public}d", id, GetWindowType());
    if (parentSession_ && parentSession_->CheckDialogOnForeground()) {
        WLOGFI("Has dialog foreground, id: %{public}d, type: %{public}d", id, GetWindowType());
        return WSError::WS_OK;
    }

    if (parentSession_ && parentSession_->CheckModalSubWindowOnForeground()) {
        return WSError::WS_OK;
    }

    if (GetSessionProperty() && GetSessionProperty()->GetRaiseEnabled()) {
        RaiseToAppTopForPointDown();
    }
    PresentFocusIfPointDown();
    return SceneSession::ProcessPointDownSession(posX, posY);
}
} // namespace OHOS::Rosen
