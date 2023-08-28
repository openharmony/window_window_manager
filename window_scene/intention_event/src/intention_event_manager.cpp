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

#include "scene_session.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "IntentionEventManager" };
std::shared_ptr<MMI::PointerEvent> g_lastMouseEvent = nullptr;
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
        WLOGFE("uiContent is null");
        return false;
    }
    if (eventHandler == nullptr) {
        WLOGFE("eventHandler is null");
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
                auto enterSession = SceneSession::GetEnterWindow().promote();
                if (enterSession == nullptr) {
                    WLOGFE("Enter session is null, do not reissuing enter leave events");
                    return;
                }
                this->ProcessEnterLeaveEventAsync();
            }
        }
    );
}

void IntentionEventManager::InputEventListener::ProcessEnterLeaveEventAsync()
{
    auto task = [this]() {
        std::lock_guard<std::mutex> guard(mouseEventMutex_);
        if (g_lastMouseEvent == nullptr) {
            return;
        }
        auto pointerEvent = std::make_shared<MMI::PointerEvent>(*g_lastMouseEvent);
        pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
        pointerEvent->SetButtonId(MMI::PointerEvent::BUTTON_NONE);
        if (uiContent_ == nullptr) {
            WLOGFE("uiContent_ is null");
            return;
        }
        uiContent_->ProcessPointerEvent(pointerEvent);
    };
    auto eventHandler = weakEventConsumer_.lock();
    if (eventHandler == nullptr) {
        WLOGFE("eventHandler is null");
        return;
    }
    eventHandler->PostTask(std::move(task), DELAY_TIME, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void IntentionEventManager::InputEventListener::UpdateLastMouseEvent(
    std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    if (pointerEvent == nullptr) {
        WLOGFE("pointerEvent is null");
        return;
    }
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
        WLOGFE("pointerEvent is null");
        return;
    }
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is null");
        return;
    }

    int32_t action = pointerEvent->GetPointerAction();
    if (action == MMI::PointerEvent::POINTER_ACTION_DOWN ||
        action == MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN) {
        int32_t pointerId = pointerEvent->GetPointerId();
        MMI::PointerEvent::PointerItem pointerItem;
        if (!pointerEvent->GetPointerItem(pointerId, pointerItem)) {
            WLOGFE("uiContent_ is null");
        } else {
            SceneSessionManager::GetInstance().OnOutsideDownEvent(
                pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
            SceneSessionManager::GetInstance().NotifySessionTouchOutside(
                action, pointerItem.GetDisplayX(), pointerItem.GetDisplayY());
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
    focusedSceneSession->TransferKeyEvent(keyEvent);
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