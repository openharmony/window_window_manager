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

#include <axis_event.h>
#include <key_event.h>
#include <pointer_event.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventChannel" };
}

WSError WindowEventChannel::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
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
    sessionStage_->NotifyPointerEvent(pointerEvent);
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

void WindowEventChannel::PrintKeyEvent(const std::shared_ptr<MMI::KeyEvent>& event)
{
    if (event == nullptr) {
        WLOGFE("event is nullptr");
        return;
    }
    std::vector<MMI::KeyEvent::KeyItem> eventItems = event->GetKeyItems();
    WLOGFD("KeyCode:%{public}d,KeyIntention:%{public}d,ActionTime:%{public}" PRId64
        ",ActionStartTime:%{public}" PRId64
        ",EventType:%{public}s,Flag:%{public}d,KeyAction:%{public}s,NumLock:%{public}d,"
        "CapsLock:%{public}d,ScrollLock:%{public}d,EventNumber:%{public}d,keyItemsCount:%{public}zu",
        event->GetKeyCode(), event->GetKeyIntention(), event->GetActionTime(), event->GetActionStartTime(),
        MMI::InputEvent::EventTypeToString(event->GetEventType()), event->GetFlag(),
        MMI::KeyEvent::ActionToString(event->GetKeyAction()),
        event->GetFunctionKey(MMI::KeyEvent::NUM_LOCK_FUNCTION_KEY),
        event->GetFunctionKey(MMI::KeyEvent::CAPS_LOCK_FUNCTION_KEY),
        event->GetFunctionKey(MMI::KeyEvent::SCROLL_LOCK_FUNCTION_KEY),
        event->GetId(), eventItems.size());
    for (const auto &item : eventItems) {
        WLOGFD("DeviceNumber:%{public}d,KeyCode:%{public}d,DownTime:%{public}" PRId64 ",IsPressed:%{public}d,"
            "GetUnicode:%{public}d", item.GetDeviceId(), item.GetKeyCode(), item.GetDownTime(), item.IsPressed(),
            item.GetUnicode());
    }
    std::vector<int32_t> pressedKeys = event->GetPressedKeys();
    std::vector<int32_t>::const_iterator cItr = pressedKeys.cbegin();
    if (cItr != pressedKeys.cend()) {
        std::string tmpStr = "Pressed keyCode: [" + std::to_string(*(cItr++));
        for (; cItr != pressedKeys.cend(); ++cItr) {
            tmpStr += ("," + std::to_string(*cItr));
        }
        WLOGFD("%{public}s]", tmpStr.c_str());
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
    WLOGFD("EventType:%{public}d,ActionTime:%{public}" PRId64 ",Action:%{public}d,"
        "ActionStartTime:%{public}" PRId64 ",Flag:%{public}d,PointerAction:%{public}s,"
        "SourceType:%{public}s,ButtonId:%{public}d,VerticalAxisValue:%{public}.2f,"
        "HorizontalAxisValue:%{public}.2f,PinchAxisValue:%{public}.2f,"
        "XAbsValue:%{public}.2f,YAbsValue:%{public}.2f,ZAbsValue:%{public}.2f,"
        "RzAbsValue:%{public}.2f,GasAbsValue:%{public}.2f,BrakeAbsValue:%{public}.2f,"
        "Hat0xAbsValue:%{public}.2f,Hat0yAbsValue:%{public}.2f,ThrottleAbsValue:%{public}.2f,"
        "PointerId:%{public}d,PointerCount:%{public}zu,EventNumber:%{public}d,"
        "BufferCount:%{public}zu,Buffer:%{public}s",
        MMI::InputEvent::EventTypeToString(event->GetEventType()), event->GetActionTime(), event->GetAction(),
        event->GetActionStartTime(), event->GetFlag(), event->DumpPointerAction(), event->DumpSourceType(),
        event->GetButtonId(), event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_SCROLL_VERTICAL),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_SCROLL_HORIZONTAL),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_PINCH),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_ABS_X),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_ABS_Y),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_ABS_Z),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_ABS_RZ),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_ABS_GAS),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_ABS_BRAKE),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_ABS_HAT0X),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_ABS_HAT0Y),
        event->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_ABS_THROTTLE), event->GetPointerId(), pointerIds.size(),
        event->GetId(), buffer.size(), str.c_str());

    for (const auto &pointerId : pointerIds) {
        MMI::PointerEvent::PointerItem item;
        if (!event->GetPointerItem(pointerId, item)) {
            WLOGFE("Invalid pointer: %{public}d.", pointerId);
            return;
        }
        WLOGFD("pointerId:%{public}d,DownTime:%{public}" PRId64 ",IsPressed:%{public}d,DisplayX:%{public}d,"
            "DisplayY:%{public}d,WindowX:%{public}d,WindowY:%{public}d,Width:%{public}d,Height:%{public}d,"
            "TiltX:%{public}.2f,TiltY:%{public}.2f,ToolDisplayX:%{public}d,ToolDisplayY:%{public}d,"
            "ToolWindowX:%{public}d,ToolWindowY:%{public}d,ToolWidth:%{public}d,ToolHeight:%{public}d,"
            "Pressure:%{public}.2f,ToolType:%{public}d,LongAxis:%{public}d,ShortAxis:%{public}d,RawDx:%{public}d,"
            "RawDy:%{public}d",
            pointerId, item.GetDownTime(), item.IsPressed(), item.GetDisplayX(), item.GetDisplayY(),
            item.GetWindowX(), item.GetWindowY(), item.GetWidth(), item.GetHeight(), item.GetTiltX(),
            item.GetTiltY(), item.GetToolDisplayX(), item.GetToolDisplayY(), item.GetToolWindowX(),
            item.GetToolWindowY(), item.GetToolWidth(), item.GetToolHeight(), item.GetPressure(),
            item.GetToolType(), item.GetLongAxis(), item.GetShortAxis(), item.GetRawDx(), item.GetRawDy());
    }
}
} // namespace OHOS::Rosen
