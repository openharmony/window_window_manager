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

#include "session_manager/include/scene_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "IntentionEventManager" };
} // namespace

IntentionEventManager::IntentionEventManager() {}
IntentionEventManager::~IntentionEventManager() {}

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
    auto listener = std::make_shared<IntentionEventManager::InputEventListener>(uiContent);
    MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(listener, eventHandler);
    return true;
}

void IntentionEventManager::InputEventListener::OnInputEvent(
    std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    if (uiContent_ == nullptr) {
        WLOGFE("uiContent_ is null");
        return;
    }
    if (pointerEvent != nullptr) {
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
            }
        }
    }
    uiContent_->ProcessPointerEvent(pointerEvent);
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