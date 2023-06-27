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

#include "zidl/screen_session_manager_proxy.h"
#include "marshalling_helper.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenSessionManagerProxy"};
}

sptr<DisplayInfo> OHOS::Rosen::ScreenSessionManagerProxy::GetDefaultDisplayInfo()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DEFAULT_DISPLAY_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return nullptr;
    }

    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        WLOGFW("read display info failed, info is nullptr.");
    }
    return info;
}

DMError ScreenSessionManagerProxy::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SetScreenActiveMode: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(screenId) || !data.WriteUint32(modeId)) {
        WLOGFE("SetScreenActiveMode: write screenId/modeId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_ACTIVE_MODE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SetScreenActiveMode: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(screenId) || !data.WriteFloat(virtualPixelRatio)) {
        WLOGFE("write screenId/modeId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::GetScreenColorGamut: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("DisplayManagerProxy::GetScreenColorGamut: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("DisplayManagerProxy::GetScreenColorGamut: WriteUint64 uint64_t failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_GAMUT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::GetScreenColorGamut: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    colorGamut = static_cast<ScreenColorGamut>(reply.ReadUint32());
    return ret;
}

DMError ScreenSessionManagerProxy::SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::SetScreenColorGamut: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("DisplayManagerProxy::SetScreenColorGamut: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId)) || !data.WriteInt32(colorGamutIdx)) {
        WLOGFW("DisplayManagerProxy::SetScreenColorGamut: Write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_GAMUT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::SetScreenColorGamut: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::GetScreenGamutMap: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("DisplayManagerProxy::GetScreenGamutMap: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("DisplayManagerProxy::GetScreenGamutMap: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_GAMUT_MAP),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::GetScreenGamutMap: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    gamutMap = static_cast<ScreenGamutMap>(reply.ReadUint32());
    return ret;
}

DMError ScreenSessionManagerProxy::SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::SetScreenGamutMap: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("DisplayManagerProxy::SetScreenGamutMap: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId)) || !data.WriteUint32(static_cast<uint32_t>(gamutMap))) {
        WLOGFW("DisplayManagerProxy::SetScreenGamutMap: Writ failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_GAMUT_MAP),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::SetScreenGamutMap: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::SetScreenColorTransform(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::SetScreenColorTransform: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("DisplayManagerProxy::SetScreenColorTransform: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("DisplayManagerProxy::SetScreenColorTransform: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_TRANSFORM),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::SetScreenColorTransform: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }

    if (!data.WriteRemoteObject(displayManagerAgent->AsObject())) {
        WLOGFE("Write IDisplayManagerAgent failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write DisplayManagerAgent type failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }

    if (!data.WriteRemoteObject(displayManagerAgent->AsObject())) {
        WLOGFE("Write IWindowManagerAgent failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write DisplayManagerAgent type failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

bool OHOS::Rosen::ScreenSessionManagerProxy::WakeUpBegin(PowerStateChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("WakeUpBegin remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WakeUpBegin: WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("WakeUpBegin: Write PowerStateChangeReason failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_WAKE_UP_BEGIN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("WakeUpBegin: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool OHOS::Rosen::ScreenSessionManagerProxy::WakeUpEnd()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("WakeUpEnd remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WakeUpEnd: WriteInterfaceToken failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_WAKE_UP_END),
        data, reply, option) != ERR_NONE) {
        WLOGFW("WakeUpEnd: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool OHOS::Rosen::ScreenSessionManagerProxy::SuspendBegin(PowerStateChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SuspendBegin remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SuspendBegin: WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("SuspendBegin: Write PowerStateChangeReason failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SUSPEND_BEGIN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SuspendBegin: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool OHOS::Rosen::ScreenSessionManagerProxy::SuspendEnd()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SuspendEnd remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SuspendEnd: WriteInterfaceToken failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SUSPEND_END),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SuspendEnd: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool OHOS::Rosen::ScreenSessionManagerProxy::SetDisplayState(DisplayState state)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SetDisplayState remote is nullptr");
        return false;
    }

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
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_DISPLAY_STATE),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool OHOS::Rosen::ScreenSessionManagerProxy::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SetScreenPowerForAll remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("Write ScreenPowerState failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write PowerStateChangeReason failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

DisplayState OHOS::Rosen::ScreenSessionManagerProxy::GetDisplayState(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetDisplayState remote is nullptr");
        return DisplayState::UNKNOWN;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DisplayState::UNKNOWN;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write displayId failed");
        return DisplayState::UNKNOWN;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return DisplayState::UNKNOWN;
    }
    return static_cast<DisplayState>(reply.ReadUint32());
}

void OHOS::Rosen::ScreenSessionManagerProxy::NotifyDisplayEvent(DisplayEvent event)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("NotifyDisplayEvent remote is nullptr");
        return;
    }

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
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return;
    }
}

ScreenPowerState OHOS::Rosen::ScreenSessionManagerProxy::GetScreenPower(ScreenId dmsScreenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetScreenPower remote is nullptr");
        return ScreenPowerState::INVALID_STATE;
    }
    
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return ScreenPowerState::INVALID_STATE;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(dmsScreenId))) {
        WLOGFE("Write dmsScreenId failed");
        return ScreenPowerState::INVALID_STATE;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return ScreenPowerState::INVALID_STATE;
    }
    return static_cast<ScreenPowerState>(reply.ReadUint32());
}

ScreenId ScreenSessionManagerProxy::CreateVirtualScreen(VirtualScreenOption virtualOption,
                                                        const sptr<IRemoteObject>& displayManagerAgent)
{
    WLOGFI("SCB: ScreenSessionManagerProxy::CreateVirtualScreen: ENTER");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SCB: ScreenSessionManagerProxy::CreateVirtualScreen: remote is nullptr");
        return SCREEN_ID_INVALID;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SCB: ScreenSessionManagerProxy::CreateVirtualScreen: WriteInterfaceToken failed");
        return SCREEN_ID_INVALID;
    }
    bool res = data.WriteString(virtualOption.name_) && data.WriteUint32(virtualOption.width_) &&
        data.WriteUint32(virtualOption.height_) && data.WriteFloat(virtualOption.density_) &&
        data.WriteInt32(virtualOption.flags_) && data.WriteBool(virtualOption.isForShot_);
    if (virtualOption.surface_ != nullptr && virtualOption.surface_->GetProducer() != nullptr) {
        res = res &&
            data.WriteBool(true) &&
            data.WriteRemoteObject(virtualOption.surface_->GetProducer()->AsObject());
    } else {
        WLOGFW("SCB: ScreenSessionManagerProxy::CreateVirtualScreen: surface is nullptr");
        res = res && data.WriteBool(false);
    }
    if (displayManagerAgent != nullptr) {
        res = res &&
            data.WriteRemoteObject(displayManagerAgent);
    }
    if (!res) {
        WLOGFE("SCB: ScreenSessionManagerProxy::Write data failed");
        return SCREEN_ID_INVALID;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_CREATE_VIRTUAL_SCREEN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SCB: ScreenSessionManagerProxy::CreateVirtualScreen: SendRequest failed");
        return SCREEN_ID_INVALID;
    }

    ScreenId screenId = static_cast<ScreenId>(reply.ReadUint64());
    return screenId;
}

DMError ScreenSessionManagerProxy::SetVirtualScreenSurface(ScreenId screenId, sptr<IBufferProducer> surface)
{
    WLOGFW("SCB: ScreenSessionManagerProxy::SetVirtualScreenSurface: ENTER");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SCB: ScreenSessionManagerProxy::SetVirtualScreenSurface: remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SCB: ScreenSessionManagerProxy::SetVirtualScreenSurface: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUint64(static_cast<uint64_t>(screenId));
    if (surface != nullptr) {
        res = res &&
            data.WriteBool(true) &&
            data.WriteRemoteObject(surface->AsObject());
    } else {
        WLOGFW("SCB: ScreenSessionManagerProxy::SetVirtualScreenSurface: surface is nullptr");
        res = res && data.WriteBool(false);
    }
    if (!res) {
        WLOGFW("SCB: ScreenSessionManagerProxy::SetVirtualScreenSurface: Write screenId/surface failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SURFACE),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SCB: ScreenSessionManagerProxy::SetVirtualScreenSurface: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::DestroyVirtualScreen(ScreenId screenId)
{
    WLOGFW("SCB: ScreenSessionManagerProxy::DestroyVirtualScreen: ENTER");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SCB: ScreenSessionManagerProxy::DestroyVirtualScreen: remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SCB: ScreenSessionManagerProxy::DestroyVirtualScreen: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("SCB: ScreenSessionManagerProxy::DestroyVirtualScreen: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_DESTROY_VIRTUAL_SCREEN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SCB: ScreenSessionManagerProxy::DestroyVirtualScreen: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::MakeMirror(ScreenId mainScreenId,
                                              std::vector<ScreenId> mirrorScreenIds, ScreenId& screenGroupId)
{
    WLOGFW("SCB: ScreenSessionManagerProxy::MakeMirror: ENTER");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SCB: ScreenSessionManagerProxy::MakeMirror: create mirror fail: remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SCB: ScreenSessionManagerProxy::MakeMirror: create mirror fail: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUint64(static_cast<uint64_t>(mainScreenId)) &&
        data.WriteUInt64Vector(mirrorScreenIds);
    if (!res) {
        WLOGFE("SCB: ScreenSessionManagerProxy::MakeMirror: create mirror fail: data write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_MIRROR),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SCB: ScreenSessionManagerProxy::MakeMirror: create mirror fail: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    screenGroupId = static_cast<ScreenId>(reply.ReadUint64());
    return ret;
}

sptr<ScreenGroupInfo> ScreenSessionManagerProxy::GetScreenGroupInfoById(ScreenId screenId)
{
    WLOGFW("SCB: ScreenSessionManagerProxy::GetScreenGroupInfoById: ENTER!");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SCB: ScreenSessionManagerProxy::GetScreenGroupInfoById: remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SCB: ScreenSessionManagerProxy::GetScreenGroupInfoById: WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("SCB: ScreenSessionManagerProxy::GetScreenGroupInfoById: Write screenId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SCB: ScreenSessionManagerProxy::GetScreenGroupInfoById: SendRequest failed");
        return nullptr;
    }

    sptr<ScreenGroupInfo> info = reply.ReadStrongParcelable<ScreenGroupInfo>();
    if (info == nullptr) {
        WLOGFW("SCB: ScreenSessionManagerProxy::GetScreenGroupInfoById SendRequest nullptr.");
        return nullptr;
    }
    return info;
}

void ScreenSessionManagerProxy::RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens)
{
    WLOGFW("SCB: ScreenSessionManagerProxy::RemoveVirtualScreenFromGroup: ENTER!");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SCB: ScreenSessionManagerProxy::RemoveVirtualScreenFromGroup:remote is null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SCB: ScreenSessionManagerProxy::RemoveVirtualScreenFromGroup: WriteInterfaceToken failed");
        return;
    }
    bool res = data.WriteUInt64Vector(screens);
    if (!res) {
        WLOGFE("SCB: ScreenSessionManagerProxy::RemoveVirtualScreenFromGroup: write screens failed.");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
        DisplayManagerMessage::TRANS_ID_REMOVE_VIRTUAL_SCREEN_FROM_SCREEN_GROUP),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SCB: ScreenSessionManagerProxy::RemoveVirtualScreenFromGroup:: SendRequest failed");
    }
}

std::shared_ptr<Media::PixelMap> ScreenSessionManagerProxy::GetDisplaySnapshot(DisplayId displayId,
                                                                               DmErrorCode* errorCode)
{
    WLOGFW("SCB: ScreenSessionManagerProxy::GetDisplaySnapshot enter");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SCB: ScreenSessionManagerProxy::GetDisplaySnapshot: remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SCB: ScreenSessionManagerProxy::GetDisplaySnapshot: WriteInterfaceToken failed");
        return nullptr;
    }

    if (!data.WriteUint64(displayId)) {
        WLOGFE("SCB: ScreenSessionManagerProxy::GetDisplaySnapshot: Write displayId failed");
        return nullptr;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_SNAPSHOT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SCB: ScreenSessionManagerProxy::GetDisplaySnapshot: SendRequest failed");
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> pixelMap(reply.ReadParcelable<Media::PixelMap>());
    if (pixelMap == nullptr) {
        WLOGFW("SCB: ScreenSessionManagerProxy::GetDisplaySnapshot: SendRequest nullptr.");
        return nullptr;
    }
    return pixelMap;
}

sptr<DisplayInfo> ScreenSessionManagerProxy::GetDisplayInfoById(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetDisplayInfoById: remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetDisplayInfoById: WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFW("GetDisplayInfoById: WriteUint64 displayId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetDisplayInfoById: SendRequest failed");
        return nullptr;
    }

    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        WLOGFW("DisplayManagerProxy::GetDisplayInfoById SendRequest nullptr.");
        return nullptr;
    }
    return info;
}

sptr<DisplayInfo> ScreenSessionManagerProxy::GetDisplayInfoByScreen(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("fail to get displayInfo by screenId: remote is null");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("fail to get displayInfo by screenId: WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFW("fail to get displayInfo by screenId: WriteUint64 displayId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_SCREEN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("fail to get displayInfo by screenId: SendRequest failed");
        return nullptr;
    }

    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        WLOGFW("fail to get displayInfo by screenId: SendRequest null");
        return nullptr;
    }
    return info;
}

std::vector<DisplayId> ScreenSessionManagerProxy::GetAllDisplayIds()
{
    std::vector<DisplayId> allDisplayIds;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return allDisplayIds;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_ALL_DISPLAYIDS),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return allDisplayIds;
    }
    reply.ReadUInt64Vector(&allDisplayIds);
    return allDisplayIds;
}

sptr<ScreenInfo> ScreenSessionManagerProxy::GetScreenInfoById(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetScreenInfoById: remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetScreenInfoById: WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("GetScreenInfoById: Write screenId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetScreenInfoById: SendRequest failed");
        return nullptr;
    }

    sptr<ScreenInfo> info = reply.ReadStrongParcelable<ScreenInfo>();
    if (info == nullptr) {
        WLOGFW("GetScreenInfoById SendRequest nullptr.");
        return nullptr;
    }
    for (auto& mode : info->GetModes()) {
        WLOGFI("info modes is width: %{public}u, height: %{public}u, refreshRate: %{public}u",
            mode->width_, mode->height_, mode->refreshRate_);
    }
    return info;
}

DMError ScreenSessionManagerProxy::GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetAllScreenInfos: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetAllScreenInfos: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_ALL_SCREEN_INFOS),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetAllScreenInfos: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    MarshallingHelper::UnmarshallingVectorParcelableObj<ScreenInfo>(reply, screenInfos);
    return ret;
}

DMError ScreenSessionManagerProxy::GetScreenSupportedColorGamuts(ScreenId screenId,
    std::vector<ScreenColorGamut>& colorGamuts)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::GetScreenSupportedColorGamuts: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("DisplayManagerProxy::GetScreenSupportedColorGamuts: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("DisplayManagerProxy::GetScreenSupportedColorGamuts: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_GAMUTS),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::GetScreenSupportedColorGamuts: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    MarshallingHelper::UnmarshallingVectorObj<ScreenColorGamut>(reply, colorGamuts,
        [](Parcel& parcel, ScreenColorGamut& color) {
            uint32_t value;
            bool res = parcel.ReadUint32(value);
            color = static_cast<ScreenColorGamut>(value);
            return res;
        }
    );
    return ret;
}

DMError OHOS::Rosen::ScreenSessionManagerProxy::SetScreenRotationLocked(bool isLocked)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteBool(isLocked)) {
        WLOGFE("write isLocked failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError OHOS::Rosen::ScreenSessionManagerProxy::IsScreenRotationLocked(bool& isLocked)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_IS_SCREEN_ROTATION_LOCKED),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    isLocked = reply.ReadBool();
    return ret;
}

sptr<CutoutInfo> ScreenSessionManagerProxy::GetCutoutInfo(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("get cutout info : remote is null");
        return nullptr;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("get cutout info : failed");
        return nullptr;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFE("get cutout info: write displayId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_CUTOUT_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetCutoutInfo: GetCutoutInfo failed");
        return nullptr;
    }
    sptr<CutoutInfo> info = reply.ReadParcelable<CutoutInfo>();
    return info;
}
} // namespace OHOS::Rosen
