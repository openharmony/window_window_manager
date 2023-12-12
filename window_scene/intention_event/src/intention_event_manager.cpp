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
#include "session_manager/include/scene_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "IntentionEventManager" };
std::shared_ptr<MMI::PointerEvent> g_lastMouseEvent = nullptr;
int32_t g_lastLeaveWindowId = -1;
constexpr int32_t DELAY_TIME = 15;

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
    listener->RegisterWindowChanged();
    return true;
}

void IntentionEventManager::InputEventListener::RegisterWindowChanged()
{
    SceneSessionManager::GetInstance().RegisterWindowChanged(
        [this](int32_t persistentId, WindowUpdateType type) {
            WLOGFD("Window changed, persistentId:%{public}d, type:%{public}d",
                persistentId, type);
            if (type == WindowUpdateType::WINDOW_UPDATE_BOUNDS) {
                this->ProcessEnterLeaveEventAsync();
            }
        }
    );
}

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
        enterSession->TransferPointerEvent(leavePointerEvent);
        g_lastLeaveWindowId = enterSession->GetPersistentId();

        auto enterPointerEvent = std::make_shared<MMI::PointerEvent>(*g_lastMouseEvent);
        enterPointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW);
        if (uiContent_ == nullptr) {
            WLOGFE("ProcessEnterLeaveEventAsync uiContent_ is null");
            return;
        }
        uiContent_->ProcessPointerEvent(enterPointerEvent);
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
        return;
    }

    int32_t action = pointerEvent->GetPointerAction();
    if (action != MMI::PointerEvent::POINTER_ACTION_MOVE) {
        WLOGFI("InputTracking id:%{public}d, EventListener OnInputEvent", pointerEvent->GetId());
    }
    if (action == MMI::PointerEvent::POINTER_ACTION_DOWN ||
        action == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        int32_t pointerId = pointerEvent->GetPointerId();
        MMI::PointerEvent::PointerItem pointerItem;
        if (!pointerEvent->GetPointerItem(pointerId, pointerItem)) {
            WLOGFE("OnInputEvent GetPointerItem failed, pointerId:%{public}d", pointerId);
        } else {
            SceneSessionManager::GetInstance().OnOutsideDownEvent(
                pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
        }
    }
    uiContent_->ProcessPointerEvent(pointerEvent);
    UpdateLastMouseEvent(pointerEvent);
}

void IntentionEventManager::InputEventListener::OnInputEvent(
    std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    auto focusedSessionId = SceneSessionManager::GetInstance().GetFocusedSession();
    if (focusedSessionId == INVALID_SESSION_ID) {
        WLOGFE("focusedSessionId is invalid");
        return;
    }
    auto focusedSceneSession = SceneSessionManager::GetInstance().GetSceneSession(focusedSessionId);
    if (focusedSceneSession == nullptr) {
        WLOGFE("focusedSceneSession is null");
        return;
    }
    WLOGFI("InputTracking id:%{public}d, EventListener OnInputEvent",
        keyEvent->GetId());
    if (focusedSceneSession->GetSessionInfo().isSystem_) {
        bool inputMethodHasProcessed = false;
#ifdef IMF_ENABLE
        bool isKeyboardEvent = IsKeyboardEvent(keyEvent);
        if (isKeyboardEvent) {
            WLOGD("dispatch keyEvent to input method");
            inputMethodHasProcessed =
                MiscServices::InputMethodController::GetInstance()->DispatchKeyEvent(keyEvent);
        }
#endif // IMF_ENABLE
        if (inputMethodHasProcessed) {
            WLOGD("Input method has processed key event");
            return;
        }
        WLOGFD("Syetem window scene, transfer key event to root scene");
        if (uiContent_ == nullptr) {
            WLOGFE("uiContent_ is null");
            return;
        }
        uiContent_->ProcessKeyEvent(keyEvent);
        return;
    }
    focusedSceneSession->TransferKeyEvent(keyEvent);
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
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is null");
        return;
    }
    uiContent_->ProcessAxisEvent(axisEvent);
}
}
} // namespace OHOS::Rosen