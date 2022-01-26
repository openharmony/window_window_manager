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

#include "window_proxy.h"
#include <ipc_types.h>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "WindowProxy"};
}

void WindowProxy::UpdateWindowProperty(const WindowProperty& windowProperty)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is nullptr");
        return;
    }
}

void WindowProxy::UpdateWindowRect(const struct Rect& rect)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!(data.WriteInt32(rect.posX_) && data.WriteInt32(rect.posY_) &&
        data.WriteUint32(rect.width_) && data.WriteUint32(rect.height_))) {
        WLOGFE("Write WindowRect failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_UPDATE_WINDOW_RECT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
    return;
}

void WindowProxy::UpdateWindowMode(WindowMode mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(mode))) {
        WLOGFE("Write WindowMode failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_UPDATE_WINDOW_MODE, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
    return;
}

void WindowProxy::UpdateFocusStatus(bool focused)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteBool(focused)) {
        WLOGFE("Write Focus failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_UPDATE_FOCUS_STATUS, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
    return;
}


void WindowProxy::UpdateAvoidArea(const std::vector<Rect>& avoidArea)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
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

    if (Remote()->SendRequest(TRANS_ID_UPDATE_AVOID_AREA, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
    return;
}

void WindowProxy::UpdateWindowState(WindowState state)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("Write isStopped");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_UPDATE_WINDOW_STATE, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
    return;
}
} // namespace Rosen
} // namespace OHOS

