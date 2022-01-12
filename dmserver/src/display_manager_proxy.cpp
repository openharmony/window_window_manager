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

#include "display_manager_proxy.h"

#include <cinttypes>
#include <ipc_types.h>

#include "window_manager_hilog.h"

#include <parcel.h>

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManagerProxy"};
}

DisplayId DisplayManagerProxy::GetDefaultDisplayId()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetDefaultDisplayId: remote is nullptr");
        return DISPLAY_ID_INVALD;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetDefaultDisplayId: WriteInterfaceToken failed");
        return DISPLAY_ID_INVALD;
    }
    if (remote->SendRequest(TRANS_ID_GET_DEFAULT_DISPLAY_ID, data, reply, option) != ERR_NONE) {
        WLOGFW("GetDefaultDisplayId: SendRequest failed");
        return DISPLAY_ID_INVALD;
    }

    DisplayId displayId = reply.ReadUint64();
    WLOGFI("DisplayManagerProxy::GetDefaultDisplayId %" PRIu64"", displayId);
    return displayId;
}

DisplayInfo DisplayManagerProxy::GetDisplayInfoById(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetDisplayInfoById: remote is nullptr");
        return DisplayInfo();
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetDisplayInfoById: WriteInterfaceToken failed");
        return DisplayInfo();
    }
    data.WriteUint64(static_cast<uint64_t>(displayId));
    if (remote->SendRequest(TRANS_ID_GET_DISPLAY_BY_ID, data, reply, option) != ERR_NONE) {
        WLOGFW("GetDisplayInfoById: SendRequest failed");
        return DisplayInfo();
    }

    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        WLOGFW("DisplayManagerProxy::GetDisplayInfoById SendRequest nullptr.");
        return DisplayInfo();
    }
    return *info;
}

DisplayId DisplayManagerProxy::CreateVirtualDisplay(const VirtualDisplayInfo &virtualDisplayInfo,
    sptr<Surface> surface)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("create virtual display: remote is nullptr");
        return DISPLAY_ID_INVALD;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("create virtual display: WriteInterfaceToken failed");
        return DISPLAY_ID_INVALD;
    }
    bool res = data.WriteParcelable(&virtualDisplayInfo) &&
        data.WriteRemoteObject(surface->GetProducer()->AsObject());
    if (!res) {
        return DISPLAY_ID_INVALD;
    }
    if (remote->SendRequest(TRANS_ID_CREATE_VIRTUAL_DISPLAY, data, reply, option) != ERR_NONE) {
        WLOGFW("create virtual display: SendRequest failed");
        return DISPLAY_ID_INVALD;
    }

    DisplayId displayId = reply.ReadUint64();
    WLOGFI("DisplayManagerProxy::CreateVirtualDisplay %" PRIu64"", displayId);
    return displayId;
}

bool DisplayManagerProxy::DestroyVirtualDisplay(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("destroy virtual display: remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("destroy virtual display: WriteInterfaceToken failed");
        return false;
    }
    data.WriteUint64(static_cast<uint64_t>(displayId));
    if (remote->SendRequest(TRANS_ID_DESTROY_VIRTUAL_DISPLAY, data, reply, option) != ERR_NONE) {
        WLOGFW("destroy virtual display: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

sptr<Media::PixelMap> DisplayManagerProxy::GetDispalySnapshot(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetDispalySnapshot: remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetDispalySnapshot: WriteInterfaceToken failed");
        return nullptr;
    }

    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write dispalyId failed");
        return nullptr;
    }

    if (remote->SendRequest(TRANS_ID_GET_DISPLAY_SNAPSHOT, data, reply, option) != ERR_NONE) {
        WLOGFW("GetDispalySnapshot: SendRequest failed");
        return nullptr;
    }

    sptr<Media::PixelMap> pixelMap = reply.ReadParcelable<Media::PixelMap>();
    if (pixelMap == nullptr) {
        WLOGFW("DisplayManagerProxy::GetDispalySnapshot SendRequest nullptr.");
        return nullptr;
    }
    return pixelMap;
}

void DisplayManagerProxy::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteRemoteObject(displayManagerAgent->AsObject())) {
        WLOGFE("Write IDisplayManagerAgent failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write DisplayManagerAgent type failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void DisplayManagerProxy::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteRemoteObject(displayManagerAgent->AsObject())) {
        WLOGFE("Write IWindowManagerAgent failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write DisplayManagerAgent type failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

bool DisplayManagerProxy::WakeUpBegin(PowerStateChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write PowerStateChangeReason failed");
        return false;
    }
    if (Remote()->SendRequest(TRANS_ID_WAKE_UP_BEGIN, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::WakeUpEnd()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (Remote()->SendRequest(TRANS_ID_WAKE_UP_END, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SuspendBegin(PowerStateChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write PowerStateChangeReason failed");
        return false;
    }
    if (Remote()->SendRequest(TRANS_ID_SUSPEND_BEGIN, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SuspendEnd()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (Remote()->SendRequest(TRANS_ID_SUSPEND_END, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SetScreenPowerForAll(DisplayPowerState state, PowerStateChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("Write DisplayPowerState failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write PowerStateChangeReason failed");
        return false;
    }
    if (Remote()->SendRequest(TRANS_ID_SET_SCREEN_POWER_FOR_ALL, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SetDisplayState(DisplayState state)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("Write DisplayState failed");
        return false;
    }
    if (Remote()->SendRequest(TRANS_ID_SET_DISPLAY_STATE, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

DisplayState DisplayManagerProxy::GetDisplayState(uint64_t displayId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DisplayState::UNKNOWN;
    }
    if (!data.WriteUint64(static_cast<uint32_t>(displayId))) {
        WLOGFE("Write displayId failed");
        return DisplayState::UNKNOWN;
    }
    if (Remote()->SendRequest(TRANS_ID_GET_DISPLAY_STATE, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return DisplayState::UNKNOWN;
    }
    return static_cast<DisplayState>(reply.ReadUint32());
}

void DisplayManagerProxy::NotifyDisplayEvent(DisplayEvent event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(event))) {
        WLOGFE("Write DisplayEvent failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_NOTIFY_DISPLAY_EVENT, data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return;
    }
}
} // namespace OHOS::Rosen