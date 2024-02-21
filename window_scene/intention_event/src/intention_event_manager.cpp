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
#include "intention_event_manager.h"

#ifdef IMF_ENABLE
#include <input_method_controller.h>
#endif // IMF_ENABLE
#include "scene_session.h"
#include "session_helper.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "IntentionEventManager" };
std::shared_ptr<MMI::PointerEvent> g_lastMouseEvent = nullptr;
int32_t g_lastLeaveWindowId = -1;
constexpr int32_t DELAY_TIME = 15;

void LogPointInfo(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (pointerEvent == nullptr) {
        return;
    }

    uint32_t windowId = static_cast<uint32_t>(pointerEvent->GetTargetWindowId());
    WLOGFD("point source:%{public}d", pointerEvent->GetSourceType());
    auto actionId = pointerEvent->GetPointerId();
    int32_t action = pointerEvent->GetPointerAction();
    if (action == MMI::PointerEvent::POINTER_ACTION_MOVE) {
        return;
    }

    MMI::PointerEvent::PointerItem item;
    if (pointerEvent->GetPointerItem(actionId, item)) {
        WLOGFI("[WMSEvent] action point info : windowid: %{public}d, id:%{public}d, displayx:%{public}d, "
            "displayy:%{public}d, windowx:%{public}d, windowy :%{public}d, action :%{public}d pressure: "
            "%{public}f, tiltx :%{public}f, tiltY :%{public}f",
            windowId, actionId, item.GetDisplayX(), item.GetDisplayY(), item.GetWindowX(), item.GetWindowY(),
            pointerEvent->GetPointerAction(), item.GetPressure(), item.GetTiltX(), item.GetTiltY());
    }
    auto ids = pointerEvent->GetPointerIds();
    for (auto&& id :ids) {
        MMI::PointerEvent::PointerItem item;
        if (pointerEvent->GetPointerItem(id, item)) {
            WLOGFD("[WMSEvent] all point info: id: %{public}d, x:%{public}d, y:%{public}d, isPressend:%{public}d, "
                "pressure:%{public}f, tiltX:%{public}f, tiltY:%{public}f",
            actionId, item.GetWindowX(), item.GetWindowY(), item.IsPressed(), item.GetPressure(),
            item.GetTiltX(), item.GetTiltY());
        }
    }
}
} // namespace

IntentionEventManager::IntentionEventManager() {}
IntentionEventManager::~IntentionEventManager() {}

IntentionEventManager::InputEventListener::~InputEventListener()
{
    std::lock_guard<std::mutex> guard(mouseEventMutex_);
    g_lastMouseEvent = nullptr;
}

bool IntentionEventManager::EnableInputEventListener(Ace::UIContent* uiContent,
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler)
{
    if (uiContent == nullptr) {
        WLOGFE("EnableInputEventListener uiContent is null");
        return false;
    }
    if (eventHandler == nullptr) {
        WLOGFE("EnableInputEventListener eventHandler is null");
        return false;
    }
    auto listener =
        std::make_shared<IntentionEventManager::InputEventListener>(uiContent, eventHandler);
    MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(listener, eventHandler);
    return true;
}

void IntentionEventManager::InputEventListener::RegisterWindowChanged()
{}

void IntentionEventManager::InputEventListener::ProcessEnterLeaveEventAsync()
{
    auto task = [this]() {
        std::lock_guard<std::mutex> guard(mouseEventMutex_);
        if ((g_lastMouseEvent == nullptr) ||
            (g_lastMouseEvent->GetButtonId() != MMI::PointerEvent::BUTTON_NONE &&
            g_lastMouseEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_BUTTON_UP)) {
            return;
        }
        auto enterSession = SceneSession::GetEnterWindow().promote();
        if (enterSession == nullptr) {
            WLOGFE("Enter session is null, do not reissuing enter leave events");
            return;
        }
        if (g_lastLeaveWindowId == enterSession->GetPersistentId()) {
            WLOGFI("g_lastLeaveWindowId:%{public}d equal enterSession id",
                g_lastLeaveWindowId);
            return;
        }

        WLOGFD("Reissue enter leave, enter persistentId:%{public}d",
            enterSession->GetPersistentId());
        auto leavePointerEvent = std::make_shared<MMI::PointerEvent>(*g_lastMouseEvent);
        leavePointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW);
        WSError ret = enterSession->TransferPointerEvent(leavePointerEvent);
        if (ret != WSError::WS_OK && leavePointerEvent != nullptr) {
            leavePointerEvent->MarkProcessed();
        }
        g_lastLeaveWindowId = enterSession->GetPersistentId();

        auto enterPointerEvent = std::make_shared<MMI::PointerEvent>(*g_lastMouseEvent);
        if (enterPointerEvent == nullptr) {
            WLOGFE("The enter pointer event is nullptr");
            return;
        }
        enterPointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW);
        if (uiContent_ == nullptr) {
            WLOGFE("ProcessEnterLeaveEventAsync uiContent_ is null");
            return;
        }
        if (!(uiContent_->ProcessPointerEvent(enterPointerEvent))) {
            WLOGFE("The UI content consumes pointer event failed");
            enterPointerEvent->MarkProcessed();
        }
    };
    auto eventHandler = weakEventConsumer_.lock();
    if (eventHandler == nullptr) {
        WLOGFE("ProcessEnterLeaveEventAsync eventHandler is null");
        return;
    }
    eventHandler->PostTask(std::move(task), "wms:ProcessEventLeaveEventAsync",
        DELAY_TIME, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void IntentionEventManager::InputEventListener::UpdateLastMouseEvent(
    std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    if (pointerEvent == nullptr) {
        WLOGFE("UpdateLastMouseEvent pointerEvent is null");
        return;
    }
    g_lastLeaveWindowId = -1;
    if ((pointerEvent->GetSourceType() == MMI::PointerEvent::SOURCE_TYPE_MOUSE) &&
        (pointerEvent->GetPointerAction() != MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW)) {
        std::lock_guard<std::mutex> guard(mouseEventMutex_);
        g_lastMouseEvent = std::make_shared<MMI::PointerEvent>(*pointerEvent);
    } else if (g_lastMouseEvent != nullptr) {
        WLOGFD("Clear last mouse event");
        std::lock_guard<std::mutex> guard(mouseEventMutex_);
        g_lastMouseEvent = nullptr;
        SceneSession::ClearEnterWindow();
    }
}

void IntentionEventManager::InputEventListener::OnInputEvent(
    std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    if (pointerEvent == nullptr) {
        WLOGFE("OnInputEvent pointerEvent is null");
        return;
    }
    if (uiContent_ == nullptr) {
        WLOGFE("OnInputEvent uiContent_ is null");
        pointerEvent->MarkProcessed();
        return;
    }
    if (!SceneSessionManager::GetInstance().IsInputEventEnabled()) {
        WLOGFD("OnInputEvent is disabled temporarily");
        pointerEvent->MarkProcessed();
        return;
    }

    LogPointInfo(pointerEvent);

    int32_t action = pointerEvent->GetPointerAction();
    if (action != MMI::PointerEvent::POINTER_ACTION_MOVE) {
        WLOGFI("InputTracking id:%{public}d, EventListener OnInputEvent", pointerEvent->GetId());
    }

    uint32_t windowId = static_cast<uint32_t>(pointerEvent->GetTargetWindowId());
    auto sceneSession = SceneSessionManager::GetInstance().GetSceneSession(windowId);
    if (sceneSession == nullptr) {
        WLOGFE("The scene session is nullptr");
        pointerEvent->MarkProcessed();
        return;
    }

    if (sceneSession->GetSessionInfo().isSystem_) {
        WLOGI("[WMSEvent] InputEventListener::OnInputEvent id:%{public}d, wid:%{public}u windowName:%{public}s "
            "action = %{public}d", pointerEvent->GetId(), windowId, sceneSession->GetSessionInfo().abilityName_.c_str(),
            action);
        sceneSession->SendPointerEventToUI(pointerEvent);

        // notify touchOutside and touchDown event
        MMI::PointerEvent::PointerItem pointerItem;
        if (pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem)) {
            sceneSession->ProcessPointDownSession(pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
        }
    } else {
        // transfer pointer event for move and drag
        WSError ret = sceneSession->TransferPointerEvent(pointerEvent);
        if (sceneSession->GetWindowType() == WindowType::WINDOW_TYPE_SYSTEM_FLOAT) {
            sceneSession->NotifyOutsideDownEvent(pointerEvent);
        }
        if ((ret != WSError::WS_OK || static_cast<int32_t>(getprocpid()) != sceneSession->GetCallingPid()) &&
            pointerEvent != nullptr) {
            pointerEvent->MarkProcessed();
        }
    }
    UpdateLastMouseEvent(pointerEvent);
}

void IntentionEventManager::InputEventListener::DispatchKeyEventCallback(
    int32_t focusedSessionId, std::shared_ptr<MMI::KeyEvent> keyEvent, bool consumed) const
{
    if (keyEvent == nullptr) {
        WLOGFW("keyEvent is null, focusedSessionId:%{public}" PRId32
            ", consumed:%{public}" PRId32, focusedSessionId, consumed);
        return;
    }

    if (consumed) {
        WLOGD("Input method has processed key event, id:%{public}" PRId32 ", focusedSessionId:%{public}" PRId32,
            keyEvent->GetId(), focusedSessionId);
        return;
    }

    auto focusedSceneSession = SceneSessionManager::GetInstance().GetSceneSession(focusedSessionId);
    if (focusedSceneSession == nullptr) {
        WLOGFE("focusedSceneSession is null");
        keyEvent->MarkProcessed();
        return;
    }

    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is null");
        keyEvent->MarkProcessed();
        return;
    }

    focusedSceneSession->SendKeyEventToUI(keyEvent);
}

void IntentionEventManager::InputEventListener::OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    if (keyEvent == nullptr) {
        WLOGFE("The key event is nullptr");
        return;
    }
    if (!SceneSessionManager::GetInstance().IsInputEventEnabled()) {
        WLOGFD("OnInputEvent is disabled temporarily");
        keyEvent->MarkProcessed();
        return;
    }
    auto focusedSessionId = SceneSessionManager::GetInstance().GetFocusedSession();
    if (focusedSessionId == INVALID_SESSION_ID) {
        WLOGFE("focusedSessionId is invalid");
        keyEvent->MarkProcessed();
        return;
    }
    auto focusedSceneSession = SceneSessionManager::GetInstance().GetSceneSession(focusedSessionId);
    if (focusedSceneSession == nullptr) {
        WLOGFE("focusedSceneSession is null");
        keyEvent->MarkProcessed();
        return;
    }
    auto isSystem = focusedSceneSession->GetSessionInfo().isSystem_;
    WLOGFI("EventListener OnInputEvent InputTracking id:%{public}d, focusedSessionId:%{public}d, isSystem:%{public}d",
        keyEvent->GetId(), focusedSessionId, isSystem);
    if (!isSystem) {
        WSError ret = focusedSceneSession->TransferKeyEvent(keyEvent);
        if ((ret != WSError::WS_OK || static_cast<int32_t>(getprocpid()) != focusedSceneSession->GetCallingPid()) &&
            keyEvent != nullptr) {
            keyEvent->MarkProcessed();
        }
        return;
    }
#ifdef IMF_ENABLE
    bool isKeyboardEvent = IsKeyboardEvent(keyEvent);
    if (isKeyboardEvent) {
        WLOGD("Async dispatch keyEvent to input method");
        auto callback = [this, focusedSessionId] (std::shared_ptr<MMI::KeyEvent>& keyEvent, bool consumed) {
            this->DispatchKeyEventCallback(focusedSessionId, keyEvent, consumed);
        };
        auto ret = MiscServices::InputMethodController::GetInstance()->DispatchKeyEvent(keyEvent, callback);
        if (ret != 0) {
            WLOGFE("DispatchKeyEvent failed, ret:%{public}d, id:%{public}d, focusedSessionId:%{public}d",
                ret, keyEvent->GetId(), focusedSessionId);
            keyEvent->MarkProcessed();
        }
        return;
    }
#endif // IMF_ENABLE
    WLOGFD("Syetem window scene, transfer key event to root scene");
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is null");
        keyEvent->MarkProcessed();
        return;
    }
    focusedSceneSession->SendKeyEventToUI(keyEvent);
}

bool IntentionEventManager::InputEventListener::IsKeyboardEvent(
    const std::shared_ptr<MMI::KeyEvent>& keyEvent) const
{
    int32_t keyCode = keyEvent->GetKeyCode();
    bool isKeyFN = (keyCode == MMI::KeyEvent::KEYCODE_FN);
    bool isKeyBack = (keyCode == MMI::KeyEvent::KEYCODE_BACK);
    bool isKeyboard = (keyCode >= MMI::KeyEvent::KEYCODE_0 && keyCode <= MMI::KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN);
    WLOGI("isKeyFN: %{public}d, isKeyboard: %{public}d", isKeyFN, isKeyboard);
    return (isKeyFN || isKeyboard || isKeyBack);
}

void IntentionEventManager::InputEventListener::OnInputEvent(
    std::shared_ptr<MMI::AxisEvent> axisEvent) const
{
    if (axisEvent == nullptr) {
        WLOGFE("axisEvent is nullptr");
    }
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is null");
        axisEvent->MarkProcessed();
        return;
    }
    if (!(uiContent_->ProcessAxisEvent(axisEvent))) {
        WLOGFI("The UI content consumes the axis event failed.");
        axisEvent->MarkProcessed();
    }
}
}
} // namespace OHOS::Rosen
