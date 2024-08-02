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

#include "session/container/include/zidl/window_event_channel_proxy.h"
#include "session/container/include/zidl/window_event_ipc_interface_code.h"

#include <axis_event.h>
#include <ipc_types.h>
#include <key_event.h>
#include <message_option.h>
#include <message_parcel.h>
#include <pointer_event.h>
#include <vector>

#include "parcel/accessibility_element_info_parcel.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventChannelProxy"};
}

WSError WindowEventChannelProxy::TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!keyEvent->WriteToParcel(data)) {
        WLOGFE("Failed to write key event");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    bool isPreImeEvent = false;
    if (!data.WriteBool(isPreImeEvent)) {
        WLOGFE("Write bool failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_KEY_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    reply.ReadBool();
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError WindowEventChannelProxy::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!pointerEvent->WriteToParcel(data)) {
        WLOGFE("Failed to write pointer event");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_POINTER_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError WindowEventChannelProxy::TransferBackpressedEventForConsumed(bool& isConsumed)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_BACKPRESSED_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    isConsumed = reply.ReadBool();
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError WindowEventChannelProxy::TransferKeyEventForConsumed(
    const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed, bool isPreImeEvent)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!keyEvent->WriteToParcel(data)) {
        WLOGFE("Failed to write key event");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(isPreImeEvent)) {
        WLOGFE("Write bool failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_KEY_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    isConsumed = reply.ReadBool();
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError WindowEventChannelProxy::TransferKeyEventForConsumedAsync(
    const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool isPreImeEvent, const sptr<IRemoteObject>& listener)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_EVENT, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!keyEvent->WriteToParcel(data)) {
        TLOGE(WmsLogTag::WMS_EVENT, "Failed to write key event");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteBool(isPreImeEvent)) {
        TLOGE(WmsLogTag::WMS_EVENT, "Write isPreImeEvent failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteRemoteObject(listener)) {
        TLOGE(WmsLogTag::WMS_EVENT, "WriteRemoteObject listener failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_KEY_EVENT_ASYNC),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_EVENT, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError WindowEventChannelProxy::TransferFocusActiveEvent(bool isFocusActive)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(isFocusActive)) {
        WLOGFE("Write bool failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_FOCUS_ACTIVE_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError WindowEventChannelProxy::TransferFocusState(bool focusState)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(focusState)) {
        WLOGFE("Write focusState failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_FOCUS_STATE_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError WindowEventChannelProxy::TransferAccessibilityHoverEvent(float pointX, float pointY, int32_t sourceType,
    int32_t eventType, int64_t timeMs)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_UIEXT, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteFloat(pointX) ||
        !data.WriteFloat(pointY) ||
        !data.WriteInt32(sourceType) ||
        !data.WriteInt32(eventType) ||
        !data.WriteInt64(timeMs)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write TransferAccessibilityHoverEvent data failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(
        static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_ACCESSIBILITY_HOVER_EVENT),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_UIEXT, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError WindowEventChannelProxy::TransferAccessibilityChildTreeRegister(
    uint32_t windowId, int32_t treeId, int64_t accessibilityId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_UIEXT, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(windowId)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "write windowId fail, action error");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(treeId)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "write treeId fail, action error");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt64(accessibilityId)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "write accessibilityId fail, action error");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    int error = Remote()->SendRequest(
        static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_ACCESSIBILITY_CHILD_TREE_REGISTER),
        data, reply, option);
    if (error != ERR_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to SendRequest: %{public}d", error);
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError WindowEventChannelProxy::TransferAccessibilityChildTreeUnregister()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_UIEXT, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int error = Remote()->SendRequest(
        static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_ACCESSIBILITY_CHILD_TREE_UNREGISTER),
        data, reply, option);
    if (error != ERR_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to SendRequest: %{public}d", error);
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError WindowEventChannelProxy::TransferAccessibilityDumpChildInfo(
    const std::vector<std::string>& params, std::vector<std::string>& info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_UIEXT, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteStringVector(params)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to write params");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int error = Remote()->SendRequest(
        static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_ACCESSIBILITY_DUMP_CHILD_INFO),
        data, reply, option);
    if (error != ERR_OK) {
        TLOGE(WmsLogTag::WMS_UIEXT, "failed to SendRequest: %{public}d", error);
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!reply.ReadStringVector(&info)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Read reply info failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}
} // namespace OHOS::Rosen
