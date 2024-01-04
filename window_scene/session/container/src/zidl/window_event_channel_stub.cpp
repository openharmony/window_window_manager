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
#include "session/container/include/zidl/window_event_ipc_interface_code.h"

#include <axis_event.h>
#include <ipc_types.h>
#include <key_event.h>
#include <pointer_event.h>

#include "accessibility_element_info.h"
#include "accessibility_element_info_parcel.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventChannelStub"};
constexpr int32_t MAX_ARGUMENTS_KEY_SIZE = 1000;
}

const std::map<uint32_t, WindowEventChannelStubFunc> WindowEventChannelStub::stubFuncMap_{
    std::make_pair(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_KEY_EVENT),
        &WindowEventChannelStub::HandleTransferKeyEvent),
    std::make_pair(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_POINTER_EVENT),
        &WindowEventChannelStub::HandleTransferPointerEvent),
    std::make_pair(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_FOCUS_ACTIVE_EVENT),
        &WindowEventChannelStub::HandleTransferFocusActiveEvent),
    std::make_pair(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_FOCUS_STATE_EVENT),
        &WindowEventChannelStub::HandleTransferFocusStateEvent),
    std::make_pair(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_BACKPRESSED_EVENT),
        &WindowEventChannelStub::HandleTransferBackpressedEvent),
    std::make_pair(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_SEARCH_ELEMENT_INFO),
        &WindowEventChannelStub::HandleTransferSearchElementInfo),
    std::make_pair(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_SEARCH_ELEMENT_INFO_BY_TEXT),
        &WindowEventChannelStub::HandleTransferSearchElementInfosByText),
    std::make_pair(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_FIND_FOCUSED_ELEMENT_INFO),
        &WindowEventChannelStub::HandleTransferFindFocusedElementInfo),
    std::make_pair(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_FOCUS_MOVE_SEARCH),
        &WindowEventChannelStub::HandleTransferFocusMoveSearch),
    std::make_pair(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_EXECUTE_ACTION),
        &WindowEventChannelStub::HandleTransferExecuteAction),
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

int WindowEventChannelStub::HandleTransferBackpressedEvent(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("TransferBackpressedEvent!");
    bool isConsumed = false;
    WSError errCode = TransferBackpressedEventForConsumed(isConsumed);

    reply.WriteBool(isConsumed);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
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

int WindowEventChannelStub::HandleTransferFocusStateEvent(MessageParcel& data, MessageParcel& reply)
{
    bool focusState = data.ReadBool();
    WSError errCode = TransferFocusState(focusState);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int WindowEventChannelStub::HandleTransferSearchElementInfo(MessageParcel& data, MessageParcel& reply)
{
    int64_t elementId = 0;
    if (!data.ReadInt64(elementId)) {
        WLOGFE("Parameter elementId is invalid!");
        return ERR_INVALID_DATA;
    }
    int32_t mode = 0;
    if (!data.ReadInt32(mode)) {
        WLOGFE("Parameter mode is invalid!");
        return ERR_INVALID_DATA;
    }
    int64_t baseParent = 0;
    if (!data.ReadInt64(baseParent)) {
        WLOGFE("Parameter baseParent is invalid!");
        return ERR_INVALID_DATA;
    }
    std::list<Accessibility::AccessibilityElementInfo> infos;
    WSError errCode = TransferSearchElementInfo(elementId, mode, baseParent, infos);
    if (errCode != WSError::WS_OK) {
        WLOGFE("Failed to TransferSearchElementInfo:%{public}d", static_cast<uint32_t>(errCode));
        return static_cast<uint32_t>(errCode);
    }
    int64_t count = static_cast<int64_t>(infos.size());
    if (!reply.WriteInt64(count)) {
        WLOGFE("Failed to write count!");
        return ERR_INVALID_DATA;
    }
    for (auto &info : infos) {
        Accessibility::AccessibilityElementInfoParcel infoParcel(info);
        if (!reply.WriteParcelable(&infoParcel)) {
            WLOGFE("Failed to WriteParcelable info");
            return ERR_INVALID_DATA;
        }
    }
    return ERR_NONE;
}

int WindowEventChannelStub::HandleTransferSearchElementInfosByText(MessageParcel& data, MessageParcel& reply)
{
    int64_t elementId = 0;
    if (!data.ReadInt64(elementId)) {
        WLOGFE("Parameter elementId is invalid!");
        return ERR_INVALID_DATA;
    }
    std::string text;
    if (!data.ReadString(text)) {
        WLOGFE("Parameter text is invalid!");
        return ERR_INVALID_DATA;
    }
    int64_t baseParent = 0;
    if (!data.ReadInt64(baseParent)) {
        WLOGFE("Parameter baseParent is invalid!");
        return ERR_INVALID_DATA;
    }
    std::list<Accessibility::AccessibilityElementInfo> infos;
    WSError errCode = TransferSearchElementInfosByText(elementId, text, baseParent, infos);
    if (errCode != WSError::WS_OK) {
        WLOGFE("Failed to HandleTransferSearchElementInfosByText:%{public}d", static_cast<uint32_t>(errCode));
        return static_cast<uint32_t>(errCode);
    }
    int64_t count = static_cast<int64_t>(infos.size());
    if (!reply.WriteInt64(count)) {
        WLOGFE("Failed to write count!");
        return ERR_INVALID_DATA;
    }
    for (auto &info : infos) {
        Accessibility::AccessibilityElementInfoParcel infoParcel(info);
        if (!reply.WriteParcelable(&infoParcel)) {
            WLOGFE("Failed to WriteParcelable info");
            return ERR_INVALID_DATA;
        }
    }
    return ERR_NONE;
}

int WindowEventChannelStub::HandleTransferFindFocusedElementInfo(MessageParcel& data, MessageParcel& reply)
{
    int64_t elementId = 0;
    if (!data.ReadInt64(elementId)) {
        WLOGFE("Parameter elementId is invalid!");
        return ERR_INVALID_DATA;
    }
    int32_t focusType = 0;
    if (!data.ReadInt32(focusType)) {
        WLOGFE("Parameter focusType is invalid!");
        return ERR_INVALID_DATA;
    }
    int64_t baseParent = 0;
    if (!data.ReadInt64(baseParent)) {
        WLOGFE("Parameter baseParent is invalid!");
        return ERR_INVALID_DATA;
    }
    Accessibility::AccessibilityElementInfo info;
    WSError errCode = TransferFindFocusedElementInfo(elementId, focusType, baseParent, info);
    if (errCode != WSError::WS_OK) {
        WLOGFE("Failed to TransferFindFocusedElementInfo:%{public}d", static_cast<uint32_t>(errCode));
        return static_cast<uint32_t>(errCode);
    }
    Accessibility::AccessibilityElementInfoParcel infoParcel(info);
    if (!reply.WriteParcelable(&infoParcel)) {
        WLOGFE("Failed to WriteParcelable info");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int WindowEventChannelStub::HandleTransferFocusMoveSearch(MessageParcel& data, MessageParcel& reply)
{
    int64_t elementId = 0;
    if (!data.ReadInt64(elementId)) {
        WLOGFE("Parameter elementId is invalid!");
        return ERR_INVALID_DATA;
    }
    int32_t direction = 0;
    if (!data.ReadInt32(direction)) {
        WLOGFE("Parameter direction is invalid!");
        return ERR_INVALID_DATA;
    }
    int64_t baseParent = 0;
    if (!data.ReadInt64(baseParent)) {
        WLOGFE("Parameter baseParent is invalid!");
        return ERR_INVALID_DATA;
    }
    Accessibility::AccessibilityElementInfo info;
    WSError errCode = TransferFocusMoveSearch(elementId, direction, baseParent, info);
    if (errCode != WSError::WS_OK) {
        WLOGFE("Failed to TransferFocusMoveSearch:%{public}d", static_cast<uint32_t>(errCode));
        return static_cast<uint32_t>(errCode);
    }
    Accessibility::AccessibilityElementInfoParcel infoParcel(info);
    if (!reply.WriteParcelable(&infoParcel)) {
        WLOGFE("Failed to WriteParcelable info");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int WindowEventChannelStub::HandleTransferExecuteAction(MessageParcel& data, MessageParcel& reply)
{
    int64_t elementId = 0;
    if (!data.ReadInt64(elementId)) {
        WLOGFE("Parameter elementId is invalid!");
        return ERR_INVALID_DATA;
    }
    int32_t action = 0;
    if (!data.ReadInt32(action)) {
        WLOGFE("Parameter action is invalid!");
        return ERR_INVALID_DATA;
    }
    std::vector<std::string> actionArgumentsKey;
    std::vector<std::string> actionArgumentsValue;
    std::map<std::string, std::string> actionArguments;
    if (!data.ReadStringVector(&actionArgumentsKey)) {
        WLOGFE("ReadStringVector actionArgumentsKey failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.ReadStringVector(&actionArgumentsValue)) {
        WLOGFE("ReadStringVector actionArgumentsValue failed");
        return ERR_INVALID_VALUE;
    }
    if (actionArgumentsKey.size() != actionArgumentsValue.size()) {
        WLOGFE("Read actionArguments failed.");
        return ERR_INVALID_VALUE;
    }
    if (actionArgumentsKey.size() > MAX_ARGUMENTS_KEY_SIZE) {
        WLOGFE("ActionArguments over max size");
        return ERR_INVALID_VALUE;
    }
    for (size_t i = 0; i < actionArgumentsKey.size(); i++) {
        actionArguments.insert(make_pair(actionArgumentsKey[i], actionArgumentsValue[i]));
    }
    int64_t baseParent = 0;
    if (!data.ReadInt64(baseParent)) {
        WLOGFE("Parameter baseParent is invalid!");
        return ERR_INVALID_DATA;
    }
    WSError errCode = TransferExecuteAction(elementId, actionArguments, action, baseParent);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}
}
