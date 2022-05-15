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

#include "zidl/window_proxy.h"
#include <ipc_types.h>
#include "message_option.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowProxy"};
}

void WindowProxy::UpdateWindowRect(const struct Rect& rect, bool decoStatus, WindowSizeChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!(data.WriteInt32(rect.posX_) && data.WriteInt32(rect.posY_) &&
        data.WriteUint32(rect.width_) && data.WriteUint32(rect.height_))) {
        WLOGFE("Write WindowRect failed");
        return;
    }
    if (!data.WriteBool(decoStatus)) {
        WLOGFE("Write deco status failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write WindowSizeChangeReason failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowMessage::TRANS_ID_UPDATE_WINDOW_RECT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
    return;
}

void WindowProxy::UpdateWindowMode(WindowMode mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(mode))) {
        WLOGFE("Write WindowMode failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowMessage::TRANS_ID_UPDATE_WINDOW_MODE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
    return;
}

void WindowProxy::UpdateFocusStatus(bool focused)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteBool(focused)) {
        WLOGFE("Write Focus failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowMessage::TRANS_ID_UPDATE_FOCUS_STATUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
    return;
}

void WindowProxy::UpdateAvoidArea(const std::vector<Rect>& avoidArea)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    uint32_t len = static_cast<uint32_t>(avoidArea.size());
    if (!data.WriteUint32(len)) {
        WLOGFE("Write UpdateAvoidArea Vector Size failed");
        return;
    }

    for (auto avoid : avoidArea) {
        if (!(data.WriteInt32(avoid.posX_) && data.WriteInt32(avoid.posY_) &&
            data.WriteUint32(avoid.width_) && data.WriteUint32(avoid.height_))) {
            WLOGFE("Write WindowRect failed");
            return;
        }
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowMessage::TRANS_ID_UPDATE_AVOID_AREA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
    return;
}

void WindowProxy::UpdateWindowState(WindowState state)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("Write isStopped");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowMessage::TRANS_ID_UPDATE_WINDOW_STATE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void WindowProxy::UpdateWindowDragInfo(const PointInfo& point, DragEvent event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!(data.WriteInt32(point.x) and data.WriteInt32(point.y))) {
        WLOGFE("Write pos failed");
        return;
    }
    if (!data.WriteInt32(static_cast<uint32_t>(event))) {
        WLOGFE("Write event failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowMessage::TRANS_ID_UPDATE_DRAG_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest TRANS_ID_UPDATE_DRAG_EVENT failed");
    }
}

void WindowProxy::UpdateDisplayId(DisplayId from, DisplayId to)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!(data.WriteUint64(from) and data.WriteUint64(to))) {
        WLOGFE("Write displayid failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowMessage::TRANS_ID_UPDATE_DISPLAY_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest TRANS_ID_UPDATE_DISPLAY_ID failed");
    }
}

void WindowProxy::UpdateOccupiedAreaChangeInfo(const sptr<OccupiedAreaChangeInfo>& info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteParcelable(info)) {
        WLOGFE("Write OccupiedAreaChangeInfo failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowMessage::TRANS_ID_UPDATE_OCCUPIED_AREA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void WindowProxy::UpdateActiveStatus(bool isActive)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteBool(isActive)) {
        WLOGFE("Write Focus failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowMessage::TRANS_ID_UPDATE_ACTIVE_STATUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
    return;
}

sptr<WindowProperty> WindowProxy::GetWindowProperty()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }
    uint32_t requestCode = static_cast<uint32_t>(WindowMessage::TRANS_ID_GET_WINDOW_PROPERTY);
    if (Remote()->SendRequest(requestCode, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
    return reply.ReadParcelable<WindowProperty>();
}
} // namespace Rosen
} // namespace OHOS

