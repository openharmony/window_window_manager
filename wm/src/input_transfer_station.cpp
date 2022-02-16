/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "input_transfer_station.h"
#include <window_manager_hilog.h>

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "InputTransferStation"};
}
WM_IMPLEMENT_SINGLE_INSTANCE(InputTransferStation)

void InputEventListener::OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const
{
    WLOGFI("OnInputEvent: receive keyEvent");
    if (keyEvent == nullptr) {
        WLOGE("OnInputEvent receive KeyEvent is nullptr");
        return;
    }
    uint32_t windowId = static_cast<uint32_t>(keyEvent->GetAgentWindowId());
    auto channel = InputTransferStation::GetInstance().GetInputChannel(windowId);
    if (channel == nullptr) {
        WLOGE("OnInputEvent channel is nullptr");
        return;
    }
    channel->HandleKeyEvent(keyEvent);
}

void InputEventListener::OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const
{
    WLOGFI("OnInputEvent: receive axisEvent");
    if (axisEvent == nullptr) {
        WLOGE("OnInputEvent receive axisEvent is nullptr");
        return;
    }
    axisEvent->MarkProcessed();
}

void InputEventListener::OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const
{
    WLOGFI("OnInputEvent: receive pointerEvent");
    if (pointerEvent == nullptr) {
        WLOGE("OnInputEvent receive pointerEvent is nullptr");
        return;
    }
    uint32_t windowId = pointerEvent->GetAgentWindowId();
    auto channel = InputTransferStation::GetInstance().GetInputChannel(windowId);
    if (channel == nullptr) {
        WLOGE("OnInputEvent channel is nullptr");
        return;
    }
    channel->HandlePointerEvent(pointerEvent);
}

void InputTransferStation::AddInputWindow(const sptr<Window>& window)
{
    WLOGFI("AddInputWindow: add window");
    uint32_t windowId = window->GetWindowId();
    sptr<WindowInputChannel> inputChannel = new WindowInputChannel(window);
    windowInputChannels_.insert(std::make_pair(windowId, inputChannel));
    if (!initInputListener_) {
        WLOGFI("init input listener");
        std::shared_ptr<MMI::IInputEventConsumer> listener = std::make_shared<InputEventListener>(InputEventListener());
        MMI::InputManager::GetInstance()->SetWindowInputEventConsumer(listener);
        inputListener_ = listener;
        initInputListener_ = true;
    }
}

void InputTransferStation::RemoveInputWindow(const sptr<Window>& window)
{
    WLOGFI("RemoveInputWindow: remove window");
    uint32_t windowId = window->GetWindowId();
    auto iter = windowInputChannels_.find(windowId);
    if (iter != windowInputChannels_.end()) {
        windowInputChannels_.erase(windowId);
    } else {
        WLOGE("RemoveInputWindow do not find windowId: %{public}d", windowId);
    }
}

void InputTransferStation::SetInputListener(uint32_t windowId, std::shared_ptr<MMI::IInputEventConsumer> &listener)
{
    auto channel = GetInputChannel(windowId);
    if (channel == nullptr) {
        WLOGE("SetInputListener channel is nullptr");
        return;
    }
    channel->SetInputListener(listener);
}

sptr<WindowInputChannel> InputTransferStation::GetInputChannel(uint32_t windowId)
{
    auto iter = windowInputChannels_.find(windowId);
    if (iter == windowInputChannels_.end()) {
        WLOGE("GetInputChannel do not find channel according to windowId: %{public}d", windowId);
        return nullptr;
    }
    return iter->second;
}
}
}
