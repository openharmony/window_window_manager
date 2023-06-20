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

#include "session/container/include/window_event_channel.h"

#include <functional>
#include <utility>

#include "unistd.h"

#include <axis_event.h>
#include <key_event.h>
#include <pointer_event.h>

#include "anr_handler.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventChannel" };
constexpr int32_t DELAY_TO_TRIGGER_ANR = 5;
}

WSError WindowEventChannel::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    bool isConsumed = false;
    return TransferKeyEventForConsumed(keyEvent, isConsumed);
}

WSError WindowEventChannel::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    WLOGFD("WindowEventChannel receive pointer event");
    if (!sessionStage_) {
        WLOGFE("session stage is null!");
        return WSError::WS_ERROR_NULLPTR;
    }
    // 这个可以考虑优化，在应用进程启动或应用窗口被创建的时候只初始化一遍就可以，不必每次都设置
    ANRHDL->SetSessionStage(sessionStage_);
    if (pointerEvent != nullptr) {
        WLOGFD("SetProcessedCallback enter");
        pointerEvent->SetProcessedCallback(dispatchCallback_);
        WLOGFD("SetProcessedCallback leave");
    }
    static auto checkInAnrRegin = [](const std::shared_ptr<MMI::PointerEvent> pointerEvent) -> bool {
        WLOGFD("Here in checkInAnrRegin");
        std::pair<int32_t, int32_t> leftUp {0, 0};
        std::pair<int32_t, int32_t> rightDown {300, 300};
        if (pointerEvent == nullptr) {
            return false;
        }
        int32_t pointerId = pointerEvent->GetPointerId();
        MMI::PointerEvent::PointerItem pointerItem;
        if (!pointerEvent->GetPointerItem(pointerId, pointerItem)) {
            WLOGFE("Can't find pointer item, pointer:%{public}d", pointerId);
            return false;
        }
        int32_t displayX = pointerItem.GetDisplayX();
        int32_t displayY = pointerItem.GetDisplayY();
        return (displayX >= leftUp.first && displayX < rightDown.first &&
                displayY >= leftUp.second && displayY < rightDown.second);
    };
    if (checkInAnrRegin(pointerEvent)) {
        WLOGFD("The pointerEvent eventId: %{public}d in anr regin, sleep 5 seconds to trigger anr", pointerEvent->GetId());
        sleep(DELAY_TO_TRIGGER_ANR);
    }
    sessionStage_->NotifyPointerEvent(pointerEvent); // sessionStage_ 就是 windowSessionImpl,windowSessionImpl 里有 hostSession_
    return WSError::WS_OK;
}

WSError WindowEventChannel::TransferKeyEventForConsumed(
    const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed)
{
    WLOGFD("WindowEventChannel receive key event");
    if (!sessionStage_) {
        WLOGFE("session stage is null!");
        return WSError::WS_ERROR_NULLPTR;
    }
    sessionStage_->NotifyKeyEvent(keyEvent, isConsumed);
    return WSError::WS_OK;
}

WSError WindowEventChannel::TransferFocusActiveEvent(bool isFocusActive)
{
    WLOGFD("WindowEventChannel receive focus active event");
    if (!sessionStage_) {
        WLOGFE("session stage is null!");
        return WSError::WS_ERROR_NULLPTR;
    }
    sessionStage_->NotifyFocusActiveEvent(isFocusActive);
    return WSError::WS_OK;
}

void WindowEventChannel::OnDispatchEventProcessed(int32_t eventId, int64_t actionTime)
{
    CALL_DEBUG_ENTER;
    ANRHDL->SetLastProcessedEventId(eventId, actionTime);
}

} // namespace OHOS::Rosen
