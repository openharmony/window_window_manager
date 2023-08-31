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

#include <axis_event.h>
#include <key_event.h>
#include <pointer_event.h>

#include "anr_handler.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventChannel" };
}

WSError WindowEventChannel::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    WLOGFD("WindowEventChannel receive key event");
    PrintKeyEvent(keyEvent);
    bool isConsumed = false;
    return TransferKeyEventForConsumed(keyEvent, isConsumed);
}

WSError WindowEventChannel::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    WLOGFD("WindowEventChannel receive pointer event");
    PrintPointerEvent(pointerEvent);
    if (!sessionStage_) {
        WLOGFE("session stage is null!");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (pointerEvent == nullptr) {
        WLOGFE("PointerEvent is null!");
        return WSError::WS_ERROR_NULLPTR;
    }
    auto pointerAction = pointerEvent->GetPointerAction();
    if (pointerAction == MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_PULL_IN_WINDOW ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_PULL_OUT_WINDOW) {
        WLOGFD("Dispatch by skipping receipt, action:%{public}s, eventId:%{public}d, persistentId:%{public}d",
            pointerEvent->DumpPointerAction(), pointerEvent->GetId(), sessionStage_->GetPersistentId());
    } else {
        DelayedSingleton<ANRHandler>::GetInstance()->SetSessionStage(pointerEvent->GetId(), sessionStage_);
        pointerEvent->SetProcessedCallback(dispatchCallback_);
        WLOGFD("Dispatch normally, action:%{public}s, eventId:%{public}d, persistentId:%{public}d",
            pointerEvent->DumpPointerAction(), pointerEvent->GetId(), sessionStage_->GetPersistentId());
    }
    sessionStage_->NotifyPointerEvent(pointerEvent);
    return WSError::WS_OK;
}

WSError WindowEventChannel::TransferBackpressedEventForConsumed(bool& isConsumed)
{
    WLOGFD("WindowEventChannel receive backpressed event");
    if (!sessionStage_) {
        WLOGFE("session stage is null!");
        return WSError::WS_ERROR_NULLPTR;
    }
    sessionStage_->NotifyBackpressedEvent(isConsumed);
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
    DelayedSingleton<ANRHandler>::GetInstance()->SetSessionStage(keyEvent->GetId(), sessionStage_);
    if (keyEvent != nullptr) {
        WLOGFD("SetProcessedCallback enter");
        keyEvent->SetProcessedCallback(dispatchCallback_);
        WLOGFD("SetProcessedCallback leave");
    }
    sessionStage_->NotifyKeyEvent(keyEvent, isConsumed);
    keyEvent->MarkProcessed();
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
    DelayedSingleton<ANRHandler>::GetInstance()->HandleEventConsumed(eventId, actionTime);
}

void WindowEventChannel::PrintKeyEvent(const std::shared_ptr<MMI::KeyEvent>& event)
{
    if (event == nullptr) {
        WLOGFE("event is nullptr");
        return;
    }
    std::vector<MMI::KeyEvent::KeyItem> eventItems = event->GetKeyItems();
    WLOGFD("KeyCode:%{public}d,KeyAction:%{public}s,keyItemsCount:%{public}zu", event->GetKeyCode(),
        MMI::KeyEvent::ActionToString(event->GetKeyAction()), eventItems.size());
    for (const auto &item : eventItems) {
        WLOGFD("KeyCode:%{public}d,IsPressed:%{public}d,GetUnicode:%{public}d",
            item.GetKeyCode(), item.IsPressed(), item.GetUnicode());
    }
}

void WindowEventChannel::PrintPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event)
{
    if (event == nullptr) {
        WLOGFE("event is nullptr");
        return;
    }
    std::vector<int32_t> pointerIds = event->GetPointerIds();
    std::string str;
    std::vector<uint8_t> buffer = event->GetBuffer();
    for (const auto &buff : buffer) {
        str += std::to_string(buff);
    }
    WLOGFD("PointerAction:%{public}s,SourceType:%{public}s,ButtonId:%{public}d,"
        "VerticalAxisValue:%{public}.2f,HorizontalAxisValue:%{public}.2f,"
        "PointerId:%{public}d,PointerCount:%{public}zu,EventNumber:%{public}d,"
        "BufferCount:%{public}zu,Buffer:%{public}s",
        event->DumpPointerAction(), event->DumpSourceType(), event->GetButtonId(),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_SCROLL_VERTICAL),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL),
        event->GetPointerId(), pointerIds.size(), event->GetId(), buffer.size(), str.c_str());

    for (const auto &pointerId : pointerIds) {
        MMI::PointerEvent::PointerItem item;
        if (!event->GetPointerItem(pointerId, item)) {
            WLOGFE("Invalid pointer: %{public}d.", pointerId);
            return;
        }
        WLOGFD("pointerId:%{public}d,DownTime:%{public}" PRId64 ",IsPressed:%{public}d,"
            "DisplayX:%{public}d,DisplayY:%{public}d,WindowX:%{public}d,WindowY:%{public}d,",
            pointerId, item.GetDownTime(), item.IsPressed(), item.GetDisplayX(), item.GetDisplayY(),
            item.GetWindowX(), item.GetWindowY());
    }
}

WSError WindowEventChannel::TransferFocusState(bool focusState)
{
    WLOGFD("WindowEventChannel receive focus state event: %{public}d", static_cast<int>(focusState));
    if (!sessionStage_) {
        WLOGFE("session stage is null!");
        return WSError::WS_ERROR_NULLPTR;
    }
    sessionStage_->NotifyFocusStateEvent(focusState);
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
