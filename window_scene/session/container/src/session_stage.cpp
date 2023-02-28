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

#include "session/container/include/session_stage.h"

#include "session/container/include/window_event_channel.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionStage"};
}
SessionStage::SessionStage(const sptr<ISession>& session) : session_(session) {}

bool SessionStage::RegisterSessionStageStateListener(const std::shared_ptr<ISessionStageStateListener>& listener)
{
    return false;
}

bool SessionStage::UnregisterSessionStageStateListener(const std::shared_ptr<ISessionStageStateListener>& listener)
{
    return false;
}

bool SessionStage::RegisterSizeChangeListener(const std::shared_ptr<ISizeChangeListener>& listener)
{
    return false;
}

bool SessionStage::UnregisterSizeChangeListener(const std::shared_ptr<ISizeChangeListener>& listener)
{
    return false;
}

bool SessionStage::RegisterPointerEventListener(const std::shared_ptr<IPointerEventListener>& listener)
{
    return false;
}

bool SessionStage::UnregisterPointerEventListener(const std::shared_ptr<IPointerEventListener>& listener)
{
    return false;
}

bool SessionStage::RegisterKeyEventListener(const std::shared_ptr<IKeyEventListener>& listener)
{
    return false;
}

bool SessionStage::UnregisterKeyEventListener(const std::shared_ptr<IKeyEventListener>& listener)
{
    return false;
}

void SessionStage::NotifySizeChange(const WSRect& rect, SizeChangeReason reason)
{
}

void SessionStage::NotifyPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
}

void SessionStage::NotifyKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
}

WSError SessionStage::Connect()
{
    return WSError::WS_OK;
}

WSError SessionStage::Foreground()
{
    return WSError::WS_OK;
}

WSError SessionStage::Background()
{
    return WSError::WS_OK;
}

WSError SessionStage::Disconnect()
{
    return WSError::WS_OK;
}

WSError SessionStage::PendingSessionActivation(const SessionInfo& info)
{
    return WSError::WS_OK;
}

WSError SessionStage::SetActive(bool active)
{
    return WSError::WS_OK;
}

WSError SessionStage::UpdateRect(const WSRect& rect, SizeChangeReason reason)
{
    WLOGFI("update rect [%{public}d, %{public}d, %{public}u, %{public}u], reason:%{public}u", rect.posX_, rect.posY_,
        rect.width_, rect.height_, reason);
    return WSError::WS_OK;
}

WSError SessionStage::Recover()
{
    return WSError::WS_OK;
}

WSError SessionStage::Maximum()
{
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
