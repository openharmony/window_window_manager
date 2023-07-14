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

#include "session/container/include/zidl/window_event_channel_stub.h"

#include <axis_event.h>
#include <ipc_types.h>
#include <key_event.h>
#include <pointer_event.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventChannelStub"};
}

const std::map<uint32_t, WindowEventChannelStubFunc> WindowEventChannelStub::stubFuncMap_{
    std::make_pair(static_cast<uint32_t>(WindowEventChannelMessage::TRANS_ID_TRANSFER_KEY_EVENT),
        &WindowEventChannelStub::HandleTransferKeyEvent),
    std::make_pair(static_cast<uint32_t>(WindowEventChannelMessage::TRANS_ID_TRANSFER_POINTER_EVENT),
        &WindowEventChannelStub::HandleTransferPointerEvent),
    std::make_pair(static_cast<uint32_t>(WindowEventChannelMessage::TRANS_ID_TRANSFER_FOCUS_ACTIVE_EVENT),
        &WindowEventChannelStub::HandleTransferFocusActiveEvent),
    std::make_pair(static_cast<uint32_t>(WindowEventChannelMessage::TRANS_ID_TRANSFER_FOCUS_WINDOW_ID_EVENT),
        &WindowEventChannelStub::HandleTransferFocusWindowIdEvent),
    std::make_pair(static_cast<uint32_t>(WindowEventChannelMessage::TRANS_ID_TRANSFER_FOCUS_STATE_EVENT),
        &WindowEventChannelStub::HandleTransferFocusStateEvent)
};

int WindowEventChannelStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
    MessageParcel &reply, MessageOption &option)
{
    WLOGFD("Window event channel on remote request!, code: %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    const auto func = stubFuncMap_.find(code);
    if (func == stubFuncMap_.end()) {
        WLOGFE("Failed to find function handler!");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return (this->*(func->second))(data, reply);
}

int WindowEventChannelStub::HandleTransferKeyEvent(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("TransferKeyEvent!");
    auto keyEvent = MMI::KeyEvent::Create();
    if (keyEvent == nullptr) {
        WLOGFE("Failed to create key event!");
        return ERR_INVALID_DATA;
    }
    if (!keyEvent->ReadFromParcel(data)) {
        WLOGFE("Read Key Event failed");
        return ERR_INVALID_DATA;
    }
    bool isConsumed = false;
    WSError errCode = TransferKeyEventForConsumed(keyEvent, isConsumed);

    reply.WriteBool(isConsumed);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int WindowEventChannelStub::HandleTransferPointerEvent(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("TransferPointerEvent!");
    auto pointerEvent = MMI::PointerEvent::Create();
    if (pointerEvent == nullptr) {
        WLOGFE("Failed to create pointer event!");
        return ERR_INVALID_DATA;
    }
    if (!pointerEvent->ReadFromParcel(data)) {
        WLOGFE("Read Pointer Event failed");
        return ERR_INVALID_DATA;
    }
    WSError errCode = TransferPointerEvent(pointerEvent);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int WindowEventChannelStub::HandleTransferFocusActiveEvent(MessageParcel& data, MessageParcel& reply)
{
    bool isFocusActive = data.ReadBool();
    WSError errCode = TransferFocusActiveEvent(isFocusActive);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int WindowEventChannelStub::HandleTransferFocusWindowIdEvent(MessageParcel& data, MessageParcel& reply)
{
    uint32_t focusWindowId = data.ReadUint32();
    WSError errCode = TransferFocusWindowId(focusWindowId);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int WindowEventChannelStub::HandleTransferFocusStateEvent(MessageParcel& data, MessageParcel& reply)
{
    bool focusState = data.ReadBool();
    WSError errCode = TransferFocusState(focusState);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}
}
