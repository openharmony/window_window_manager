/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "display_manager_lite_proxy.h"

#include <ipc_types.h>
#include <message_option.h>
#include <message_parcel.h>

#include "dm_common.h"
#include "marshalling_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerLiteProxy" };
}

DMError DisplayManagerLiteProxy::RegisterDisplayManagerAgent(
    const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is null");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WLOGFD("DisplayManagerLiteProxy::RegisterDisplayManagerAgent");
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("RegisterDisplayManagerAgent WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }

    if (displayManagerAgent == nullptr) {
        WLOGFE("IDisplayManagerAgent is null");
        return DMError::DM_ERROR_INVALID_PARAM;
    }

    if (!data.WriteRemoteObject(displayManagerAgent->AsObject())) {
        WLOGFE("Write IDisplayManagerAgent failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write DisplayManagerAgent type failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerLiteProxy::UnregisterDisplayManagerAgent(
    const sptr<IDisplayManagerAgent>& displayManagerAgent, DisplayManagerAgentType type)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is null");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    WLOGFD("DisplayManagerLiteProxy::UnregisterDisplayManagerAgent");
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("UnregisterDisplayManagerAgent WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }

    if (displayManagerAgent == nullptr) {
        WLOGFE("IDisplayManagerAgent is null");
        return DMError::DM_ERROR_INVALID_PARAM;
    }

    if (!data.WriteRemoteObject(displayManagerAgent->AsObject())) {
        WLOGFE("Write IWindowManagerAgent failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write DisplayManagerAgent type failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

FoldDisplayMode DisplayManagerLiteProxy::GetFoldDisplayMode()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is null");
        return FoldDisplayMode::UNKNOWN;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken Failed");
        return FoldDisplayMode::UNKNOWN;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_FOLD_DISPLAY_MODE),
                            data, reply, option) != ERR_NONE) {
        WLOGFE("Send TRANS_ID_SCENE_BOARD_GET_FOLD_DISPLAY_MODE request failed");
        return FoldDisplayMode::UNKNOWN;
    }
    return static_cast<FoldDisplayMode>(reply.ReadUint32());
}

void DisplayManagerLiteProxy::SetFoldDisplayMode(const FoldDisplayMode displayMode)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is null");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken Failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(displayMode))) {
        WLOGFE("Write displayMode failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_SET_FOLD_DISPLAY_MODE),
                            data, reply, option) != ERR_NONE) {
        WLOGFE("Send TRANS_ID_SCENE_BOARD_SET_FOLD_DISPLAY_MODE request failed");
    }
}

bool DisplayManagerLiteProxy::IsFoldable()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is null");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("IsFoldable WriteInterfaceToken failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_IS_FOLDABLE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

FoldStatus DisplayManagerLiteProxy::GetFoldStatus()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is null");
        return FoldStatus::UNKNOWN;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return FoldStatus::UNKNOWN;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_FOLD_STATUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return FoldStatus::UNKNOWN;
    }
    return static_cast<FoldStatus>(reply.ReadUint32());
}

sptr<DisplayInfo> OHOS::Rosen::DisplayManagerLiteProxy::GetDefaultDisplayInfo()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is null");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DEFAULT_DISPLAY_INFO),
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

sptr<DisplayInfo> DisplayManagerLiteProxy::GetDisplayInfoById(DisplayId displayId)
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

sptr<CutoutInfo> DisplayManagerLiteProxy::GetCutoutInfo(DisplayId displayId)
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

VirtualScreenFlag DisplayManagerLiteProxy::GetVirtualScreenFlag(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetVirtualScreenFlag: remote is null");
        return VirtualScreenFlag::DEFAULT;
    }
 
    if (screenId == SCREEN_ID_INVALID) {
        return VirtualScreenFlag::DEFAULT;
    }
    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel reply;
    MessageParcel data;
    WLOGFE("MessageParcel definded");
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("failed");
        return VirtualScreenFlag::DEFAULT;
    }
    WLOGFE("WriteInterfaceToken success");
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write failed");
        return VirtualScreenFlag::DEFAULT;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_VIRTUAL_SCREEN_FLAG),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return VirtualScreenFlag::DEFAULT;
    }
    return static_cast<VirtualScreenFlag>(reply.ReadUint32());
}

/*
 * used by powermgr
 */
bool DisplayManagerLiteProxy::WakeUpBegin(PowerStateChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[UL_POWER]WakeUpBegin remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[UL_POWER]WakeUpBegin: WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("[UL_POWER]WakeUpBegin: Write PowerStateChangeReason failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_WAKE_UP_BEGIN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("[UL_POWER]WakeUpBegin: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerLiteProxy::WakeUpEnd()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[UL_POWER]WakeUpEnd remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[UL_POWER]WakeUpEnd: WriteInterfaceToken failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_WAKE_UP_END),
        data, reply, option) != ERR_NONE) {
        WLOGFW("[UL_POWER]WakeUpEnd: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerLiteProxy::SuspendBegin(PowerStateChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[UL_POWER]SuspendBegin remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[UL_POWER]SuspendBegin: WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("[UL_POWER]SuspendBegin: Write PowerStateChangeReason failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SUSPEND_BEGIN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("[UL_POWER]SuspendBegin: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerLiteProxy::SuspendEnd()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[UL_POWER]SuspendEnd remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[UL_POWER]SuspendEnd: WriteInterfaceToken failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SUSPEND_END),
        data, reply, option) != ERR_NONE) {
        WLOGFW("[UL_POWER]SuspendEnd: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerLiteProxy::SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state,
    PowerStateChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[UL_POWER]SetSpecifiedScreenPower remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[UL_POWER]WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(screenId))) {
        WLOGFE("[UL_POWER]Write ScreenId failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("[UL_POWER]Write ScreenPowerState failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("[UL_POWER]Write PowerStateChangeReason failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SPECIFIED_SCREEN_POWER),
        data, reply, option) != ERR_NONE) {
        WLOGFW("[UL_POWER]SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerLiteProxy::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[UL_POWER]SetScreenPowerForAll remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[UL_POWER]WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("[UL_POWER]Write ScreenPowerState failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("[UL_POWER]Write PowerStateChangeReason failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL),
        data, reply, option) != ERR_NONE) {
        WLOGFW("[UL_POWER]SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

ScreenPowerState DisplayManagerLiteProxy::GetScreenPower(ScreenId dmsScreenId)
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
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return ScreenPowerState::INVALID_STATE;
    }
    return static_cast<ScreenPowerState>(reply.ReadUint32());
}

bool DisplayManagerLiteProxy::SetDisplayState(DisplayState state)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[UL_POWER]SetDisplayState remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[UL_POWER]WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("[UL_POWER]Write DisplayState failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_DISPLAY_STATE),
        data, reply, option) != ERR_NONE) {
        WLOGFW("[UL_POWER]SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

DisplayState DisplayManagerLiteProxy::GetDisplayState(DisplayId displayId)
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
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return DisplayState::UNKNOWN;
    }
    return static_cast<DisplayState>(reply.ReadUint32());
}

bool DisplayManagerLiteProxy::TryToCancelScreenOff()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[UL_POWER]TryToCancelScreenOff remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[UL_POWER]TryToCancelScreenOff: WriteInterfaceToken failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_TRY_TO_CANCEL_SCREEN_OFF),
        data, reply, option) != ERR_NONE) {
        WLOGFW("[UL_POWER]TryToCancelScreenOff: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerLiteProxy::SetScreenBrightness(uint64_t screenId, uint32_t level)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SetScreenBrightness remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return false;
    }
    if (!data.WriteUint64(level)) {
        WLOGFE("Write level failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_BRIGHTNESS),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

uint32_t DisplayManagerLiteProxy::GetScreenBrightness(uint64_t screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetScreenBrightness remote is nullptr");
        return 0;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return 0;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFE("Write screenId failed");
        return 0;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_BRIGHTNESS),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return 0;
    }
    return reply.ReadUint32();
}

std::vector<DisplayId> DisplayManagerLiteProxy::GetAllDisplayIds()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[UL_POWER]GetAllDisplayIds remote is nullptr");
        return {};
    }

    std::vector<DisplayId> allDisplayIds;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return allDisplayIds;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_ALL_DISPLAYIDS),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return allDisplayIds;
    }
    reply.ReadUInt64Vector(&allDisplayIds);
    return allDisplayIds;
}

} // namespace OHOS::Rosen
