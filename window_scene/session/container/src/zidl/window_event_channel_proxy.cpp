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
#include "session/container/include/parcel/accessibility_element_info_parcel.h"

#include <axis_event.h>
#include <ipc_types.h>
#include <key_event.h>
#include <message_option.h>
#include <message_parcel.h>
#include <pointer_event.h>
#include <vector>

#include "accessibility_element_info_parcel.h"
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
    const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed)
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

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_KEY_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    isConsumed = reply.ReadBool();
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

WSError GetElementInfos(MessageParcel& reply, std::list<Accessibility::AccessibilityElementInfo>& infos)
{
    int32_t count = 0;
    if (!reply.ReadInt32(count)) {
        WLOGFE("GetElementInfos failed to read count");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    infos.clear();
    for (int i = 0; i < count; i++) {
        sptr<AccessibilityElementInfoParcel> infoPtr =
            reply.ReadStrongParcelable<AccessibilityElementInfoParcel>();
        if (infoPtr != nullptr) {
            infos.push_back(*infoPtr);
        }
    }
    WLOGFD("GetElementInfos end");
    return WSError::WS_OK;
}

WSError WindowEventChannelProxy::TransferSearchElementInfo(int32_t elementId, int32_t mode, int32_t baseParent,
    std::list<Accessibility::AccessibilityElementInfo>& infos)
{
    WLOGFD("TransferSearchElementInfo begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(elementId)) {
        WLOGFE("Write elementId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(mode)) {
        WLOGFE("Write mode failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(baseParent)) {
        WLOGFE("Write baseParent failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_SEARCH_ELEMENT_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    WLOGFD("TransferSearchElementInfo end");
    return GetElementInfos(reply, infos);
}

WSError WindowEventChannelProxy::TransferSearchElementInfosByText(int32_t elementId, const std::string& text,
    int32_t baseParent, std::list<Accessibility::AccessibilityElementInfo>& infos)
{
    WLOGFD("TransferSearchElementInfosByText begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(elementId)) {
        WLOGFE("Write elementId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteString(text)) {
        WLOGFE("Write text failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(baseParent)) {
        WLOGFE("Write baseParent failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(
        static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_SEARCH_ELEMENT_INFO_BY_TEXT), data, reply,
        option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    WLOGFD("TransferSearchElementInfosByText end");
    return GetElementInfos(reply, infos);
}

WSError GetElementInfo(MessageParcel& reply, Accessibility::AccessibilityElementInfo& info)
{
    WLOGFD("GetElementInfo begin");
    sptr<AccessibilityElementInfoParcel> infoPtr =
        reply.ReadStrongParcelable<AccessibilityElementInfoParcel>();
    if (infoPtr != nullptr) {
        info = *infoPtr;
    }
    WLOGFD("GetElementInfo end");
    return WSError::WS_OK;
}

WSError WindowEventChannelProxy::TransferFindFocusedElementInfo(int32_t elementId, int32_t focusType,
    int32_t baseParent, Accessibility::AccessibilityElementInfo& info)
{
    WLOGFD("TransferFindFocusedElementInfo begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(elementId)) {
        WLOGFE("Write elementId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(focusType)) {
        WLOGFE("Write focusType failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(baseParent)) {
        WLOGFE("Write baseParent failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(
        static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_FIND_FOCUSED_ELEMENT_INFO), data, reply,
        option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    WLOGFD("TransferFindFocusedElementInfo end");
    return GetElementInfo(reply, info);
}

WSError WindowEventChannelProxy::TransferFocusMoveSearch(int32_t elementId, int32_t direction, int32_t baseParent,
    Accessibility::AccessibilityElementInfo& info)
{
    WLOGFD("TransferFocusMoveSearch begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(elementId)) {
        WLOGFE("Write elementId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(direction)) {
        WLOGFE("Write direction failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(baseParent)) {
        WLOGFE("Write baseParent failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowEventInterfaceCode::TRANS_ID_TRANSFER_FOCUS_MOVE_SEARCH),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    WLOGFD("TransferFocusMoveSearch end");
    return GetElementInfo(reply, info);
}

} // namespace OHOS::Rosen
