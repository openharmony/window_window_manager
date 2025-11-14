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
#include "scene_board_judgement.h"

#include <functional>
#include <utility>

#include <axis_event.h>
#include <key_event.h>
#include <pointer_event.h>

#include "window_manager_hilog.h"
#include "session_permission.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventChannel" };
const std::set<int32_t> VALID_KEYCODE_FOR_CONSTRAINED_EMBEDDED_UIEXTENSION({ MMI::KeyEvent::KEYCODE_HOME,
    MMI::KeyEvent::KEYCODE_TAB, MMI::KeyEvent::KEYCODE_ESCAPE, MMI::KeyEvent::KEYCODE_DPAD_UP,
    MMI::KeyEvent::KEYCODE_DPAD_DOWN, MMI::KeyEvent::KEYCODE_DPAD_LEFT, MMI::KeyEvent::KEYCODE_DPAD_RIGHT,
    MMI::KeyEvent::KEYCODE_MOVE_HOME, MMI::KeyEvent::KEYCODE_MOVE_END });
constexpr int32_t SIZE_TWO = 2;
}

void WindowEventChannelListenerProxy::OnTransferKeyEventForConsumed(int32_t keyEventId, bool isPreImeEvent,
    bool isConsumed, WSError retCode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_EVENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteInt32(keyEventId)) {
        TLOGE(WmsLogTag::WMS_EVENT, "keyEventId write failed.");
        return;
    }
    if (!data.WriteBool(isPreImeEvent)) {
        TLOGE(WmsLogTag::WMS_EVENT, "isPreImeEvent write failed.");
        return;
    }
    if (!data.WriteBool(isConsumed)) {
        TLOGE(WmsLogTag::WMS_EVENT, "isConsumed write failed.");
        return;
    }
    if (!data.WriteInt32(static_cast<int32_t>(retCode))) {
        TLOGE(WmsLogTag::WMS_EVENT, "retCode write failed.");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "remote is null");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
        WindowEventChannelListenerMessage::TRANS_ID_ON_TRANSFER_KEY_EVENT_FOR_CONSUMED_ASYNC),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_EVENT, "SendRequest failed");
    }
}

void WindowEventChannel::SetIsUIExtension(bool isUIExtension)
{
    isUIExtension_ = isUIExtension;
}

void WindowEventChannel::SetUIExtensionUsage(UIExtensionUsage uiExtensionUsage)
{
    uiExtensionUsage_ = uiExtensionUsage;
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
    if (SceneBoardJudgement::IsSceneBoardEnabled() && isUIExtension_ &&
        (uiExtensionUsage_ == UIExtensionUsage::MODAL ||
        uiExtensionUsage_ == UIExtensionUsage::CONSTRAINED_EMBEDDED)) {
        if (!SessionPermission::IsSystemCalling()) {
            TLOGE(WmsLogTag::WMS_EVENT, "Point event blocked because of modal/constrained UIExtension:%{public}u",
                uiExtensionUsage_);
            return WSError::WS_ERROR_INVALID_PERMISSION;
        } else {
            TLOGW(WmsLogTag::WMS_EVENT, "SystemCalling UIExtension:%{public}u", uiExtensionUsage_);
        }
    }
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
        WLOGFI("InputTracking id:%{public}d, Dispatch by skipping receipt, action:%{public}s,"
            " persistentId:%{public}d", pointerEvent->GetId(),
            pointerEvent->DumpPointerAction(), sessionStage_->GetPersistentId());
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
    const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed, bool isPreImeEvent)
{
    WLOGFD("WindowEventChannel receive key event");
    if (!sessionStage_) {
        WLOGFE("session stage is null!");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (keyEvent == nullptr) {
        WLOGFE("keyEvent is nullptr");
        return WSError::WS_ERROR_NULLPTR;
    }
    if (SceneBoardJudgement::IsSceneBoardEnabled() && isUIExtension_ && IsUIExtensionKeyEventBlocked(keyEvent)) {
        return WSError::WS_ERROR_INVALID_PERMISSION;
    }
    if (isPreImeEvent) {
        isConsumed = sessionStage_->NotifyOnKeyPreImeEvent(keyEvent);
        TLOGI(WmsLogTag::WMS_EVENT, "NotifyOnKeyPreImeEvent id:%{public}d isConsumed:%{public}d",
            keyEvent->GetId(), static_cast<int>(isConsumed));
        return WSError::WS_OK;
    }
    sessionStage_->NotifyKeyEvent(keyEvent, isConsumed);
    keyEvent->MarkProcessed();
    return WSError::WS_OK;
}

bool WindowEventChannel::IsUIExtensionKeyEventBlocked(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (uiExtensionUsage_ == UIExtensionUsage::MODAL) {
        if (!SessionPermission::IsSystemCalling()) {
            TLOGE(WmsLogTag::WMS_EVENT, "Unsupported keyCode due to Modal UIExtension.");
            return true;
        } else {
            TLOGW(WmsLogTag::WMS_EVENT, "SystemCalling UIExtension.");
            return false;
        }
    }
    if (uiExtensionUsage_ == UIExtensionUsage::CONSTRAINED_EMBEDDED) {
        auto keyCode = keyEvent->GetKeyCode();
        if (VALID_KEYCODE_FOR_CONSTRAINED_EMBEDDED_UIEXTENSION.find(keyCode) ==
            VALID_KEYCODE_FOR_CONSTRAINED_EMBEDDED_UIEXTENSION.end()) {
            TLOGE(WmsLogTag::WMS_EVENT, "Unsupported keyCode due to Constrained embedded UIExtension.");
            return true;
        }
        auto pressedKeys = keyEvent->GetPressedKeys();
        if (pressedKeys.size() == SIZE_TWO && keyCode == MMI::KeyEvent::KEYCODE_TAB &&
            (pressedKeys[0] == MMI::KeyEvent::KEYCODE_SHIFT_LEFT ||
            pressedKeys[0] == MMI::KeyEvent::KEYCODE_SHIFT_RIGHT)) {
            // only allows combined keys SHIFT+TAB
            return false;
        } else if (pressedKeys.size() >= SIZE_TWO) {
            TLOGE(WmsLogTag::WMS_EVENT, "Invalid size of PressedKeys due to Constrained embedded UIExtension.");
            return true;
        }
    }
    return false;
}

WSError WindowEventChannel::TransferKeyEventForConsumedAsync(
    const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool isPreImeEvent, const sptr<IRemoteObject>& listener)
{
    bool isConsumed = false;
    auto ret = TransferKeyEventForConsumed(keyEvent, isConsumed, isPreImeEvent);
    auto channelListener = iface_cast<IWindowEventChannelListener>(listener);
    if (channelListener == nullptr) {
        TLOGE(WmsLogTag::WMS_EVENT, "listener is null.");
        return ret;
    }

    auto keyEventId = keyEvent->GetId();
    TLOGD(WmsLogTag::WMS_EVENT, "finished with isConsumed:%{public}d ret:%{public}d at PreIme:%{public}d id:%{public}d",
        isConsumed, ret, isPreImeEvent, keyEventId);
    channelListener->OnTransferKeyEventForConsumed(keyEventId, isPreImeEvent, isConsumed, ret);
    return ret;
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
    WLOGFD("KeyAction:%{public}s,keyItemsCount:%{public}zu",
        MMI::KeyEvent::ActionToString(event->GetKeyAction()), eventItems.size());
    for (const auto &item : eventItems) {
        WLOGFD("IsPressed:%{public}d,GetUnicode:%{public}d", item.IsPressed(), item.GetUnicode());
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
    auto pointerAction = event->GetPointerAction();
    if (pointerAction == MMI::PointerEvent::POINTER_ACTION_MOVE ||
        pointerAction == MMI::PointerEvent::POINTER_ACTION_PULL_MOVE) {
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
            TLOGD(WmsLogTag::WMS_EVENT, "pointerId:%{public}d,DownTime:%{public}" PRId64 ",IsPressed:%{public}d,"
                "DisplayX:%{private}d,DisplayY:%{private}d,WindowX:%{private}d,WindowY:%{private}d",
                pointerId, item.GetDownTime(), item.IsPressed(), item.GetDisplayX(), item.GetDisplayY(),
                item.GetWindowX(), item.GetWindowY());
        }
    } else {
        PrintInfoPointerEvent(event);
    }
}

void WindowEventChannel::PrintInfoPointerEvent(const std::shared_ptr<MMI::PointerEvent>& event)
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
    WLOGFI("PointerAction:%{public}s,SourceType:%{public}s,ButtonId:%{public}d,"
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
        WLOGFI("pointerId:%{public}d,DownTime:%{public}" PRId64 ",IsPressed:%{public}d",
            pointerId, item.GetDownTime(), item.IsPressed());
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

WSError WindowEventChannel::TransferAccessibilityHoverEvent(float pointX, float pointY, int32_t sourceType,
    int32_t eventType, int64_t timeMs)
{
    if (!sessionStage_) {
        TLOGE(WmsLogTag::WMS_UIEXT, "session stage is null.");
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->NotifyAccessibilityHoverEvent(pointX, pointY, sourceType, eventType, timeMs);
}

WSError WindowEventChannel::TransferAccessibilityChildTreeRegister(
    uint32_t windowId, int32_t treeId, int64_t accessibilityId)
{
    if (!sessionStage_) {
        TLOGE(WmsLogTag::WMS_UIEXT, "session stage is null.");
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->NotifyAccessibilityChildTreeRegister(windowId, treeId, accessibilityId);
}

WSError WindowEventChannel::TransferAccessibilityChildTreeUnregister()
{
    if (!sessionStage_) {
        TLOGE(WmsLogTag::WMS_UIEXT, "session stage is null.");
        return WSError::WS_ERROR_NULLPTR;
    }
    return sessionStage_->NotifyAccessibilityChildTreeUnregister();
}

WSError WindowEventChannel::TransferAccessibilityDumpChildInfo(
    const std::vector<std::string>& params, std::vector<std::string>& info)
{
    if (!sessionStage_) {
        TLOGE(WmsLogTag::WMS_UIEXT, "session stage is null.");
        return WSError::WS_ERROR_NULLPTR;
    }
#ifdef ACCESSIBILITY_DUMP_FOR_TEST
    return sessionStage_->NotifyAccessibilityDumpChildInfo(params, info);
#else
    info.emplace_back("not support in user build variant");
    return WSError::WS_OK;
#endif
}
} // namespace OHOS::Rosen
