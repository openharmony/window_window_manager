/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "window_input_channel.h"
#include <input_method_controller.h>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowInputChannel"};
}
WindowInputChannel::WindowInputChannel(const sptr<Window>& window)
{
    window_ = window;
    callback_->onCallback = std::bind(&WindowInputChannel::OnVsync, this, std::placeholders::_1);
}

void WindowInputChannel::HandleKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    if (keyEvent == nullptr) {
        WLOGFE("keyEvent is nullptr");
        return;
    }
    WLOGFI("Receive key event, windowId: %{public}d, keyCode: %{public}d",
        window_->GetWindowId(), keyEvent->GetKeyCode());
    bool isKeyboardEvent = IsKeyboardEvent(keyEvent);
    bool inputMethodHasProcessed = false;
    if (isKeyboardEvent) {
        WLOGFI("dispatch keyEvent to input method");
        inputMethodHasProcessed = MiscServices::InputMethodController::GetInstance()->dispatchKeyEvent(keyEvent);
    }
    if (!isKeyboardEvent || !inputMethodHasProcessed) {
        WLOGFI("dispatch keyEvent to ACE");
        if (inputListener_ != nullptr) {
            inputListener_->OnInputEvent(keyEvent);
            return;
        }
        window_->ConsumeKeyEvent(keyEvent);
    }
    keyEvent->MarkProcessed();
}

void WindowInputChannel::HandlePointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (pointerEvent == nullptr) {
        WLOGFE("pointerEvent is nullptr");
        return;
    }
    if (inputListener_ != nullptr) {
        inputListener_->OnInputEvent(pointerEvent);
        return;
    }
    if (pointerEvent->GetPointerAction() == MMI::PointerEvent::POINTER_ACTION_MOVE) {
        std::shared_ptr<MMI::PointerEvent> pointerEventTemp;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            pointerEventTemp = moveEvent_;
            moveEvent_ = pointerEvent;
        }
        VsyncStation::GetInstance().RequestVsync(VsyncStation::CallbackType::CALLBACK_INPUT, callback_);
        WLOGFI("Receive move event, windowId: %{public}d, action: %{public}d",
            window_->GetWindowId(), pointerEvent->GetPointerAction());
        if (pointerEventTemp != nullptr) {
            pointerEventTemp->MarkProcessed();
        }
    } else {
        WLOGFI("Dispatch non-move event, windowId: %{public}d, action: %{public}d",
            window_->GetWindowId(), pointerEvent->GetPointerAction());
        window_->ConsumePointerEvent(pointerEvent);
        pointerEvent->MarkProcessed();
    }
}

void WindowInputChannel::OnVsync(int64_t timeStamp)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        pointerEvent = moveEvent_;
        moveEvent_ = nullptr;
    }
    if (pointerEvent == nullptr) {
        WLOGFE("moveEvent_ is nullptr");
        return;
    }
    WLOGFI("Dispatch move event, windowId: %{public}d, action: %{public}d",
        window_->GetWindowId(), pointerEvent->GetPointerAction());
    window_->ConsumePointerEvent(pointerEvent);
    pointerEvent->MarkProcessed();
}

void WindowInputChannel::SetInputListener(std::shared_ptr<MMI::IInputEventConsumer>& listener)
{
    inputListener_ = listener;
}

bool WindowInputChannel::IsKeyboardEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const
{
    int32_t keyCode = keyEvent->GetKeyCode();
    bool isKeyFN = (keyCode == MMI::KeyEvent::KEYCODE_FN);
    bool isKeyboard = (keyCode >= MMI::KeyEvent::KEYCODE_0 && keyCode <= MMI::KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN);
    WLOGFI("isKeyFN: %{public}d, isKeyboard: %{public}d", isKeyFN, isKeyboard);
    return (isKeyFN || isKeyboard);
}
}
}