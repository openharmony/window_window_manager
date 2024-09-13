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

#include "zidl/screen_session_manager_lite_stub.h"
#include "dm_common.h"
#include <ipc_skeleton.h>

#include "marshalling_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenSessionManagerLiteStub"};
}

int32_t ScreenSessionManagerLiteStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    WLOGFD("OnRemoteRequest code is %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return ERR_TRANSACTION_FAILED;
    }
    ScreenManagerLiteMessage msgId = static_cast<ScreenManagerLiteMessage>(code);
    switch (msgId) {
        case ScreenManagerLiteMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT:
            HandleRegisterDisplayManagerAgent(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT:
            HandleUnRegisterDisplayManagerAgent(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_SCENE_BOARD_GET_FOLD_DISPLAY_MODE:
            HandleGetFoldDisplayMode(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_SCENE_BOARD_SET_FOLD_DISPLAY_MODE:
            HandleSetFoldDisplayMode(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_SCENE_BOARD_IS_FOLDABLE:
            HandleIsFoldable(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_SCENE_BOARD_GET_FOLD_STATUS:
            HandleGetFoldStatus(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_GET_DEFAULT_DISPLAY_INFO:
            HandleGetDefaultDisplayInfo(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_GET_DISPLAY_BY_ID:
            HandleGetDisplayById(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_GET_CUTOUT_INFO:
            HandleGetCutoutInfo(data, reply);
            break;
        default:
            return RemoteRequest(code, data, reply, option);
    }
    return 0;
}

int ScreenSessionManagerLiteStub::RemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    ScreenManagerLiteMessage msgId = static_cast<ScreenManagerLiteMessage>(code);
    switch (msgId) {
        case ScreenManagerLiteMessage::TRANS_ID_WAKE_UP_BEGIN:
            HandleWakeUpBegin(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_WAKE_UP_END:
            HandleWakeUpEnd(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_SUSPEND_BEGIN:
            HandleSuspendBegin(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_SUSPEND_END:
            HandleSuspendEnd(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_SET_SPECIFIED_SCREEN_POWER:
            HandleSetSpecifiedScreenPower(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL:
            HandleSetScreenPowerForAll(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_GET_SCREEN_POWER:
            HandleGetScreenPower(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_SET_DISPLAY_STATE:
            HandleSetDisplayState(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_GET_DISPLAY_STATE:
            HandleGetDisplayState(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_GET_ALL_DISPLAYIDS:
            HandleGetAllDisplayIds(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_SET_SCREEN_BRIGHTNESS:
            HandleSetScreenBrightness(data, reply);
            break;
        case ScreenManagerLiteMessage::TRANS_ID_GET_SCREEN_BRIGHTNESS:
            HandleGetScreenBrightness(data, reply);
            break;
        default:
            WLOGFW("unknown transaction code");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}

int ScreenSessionManagerLiteStub::HandleRegisterDisplayManagerAgent(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleRegisterDisplayManagerAgent!");
    auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
    if (agent == nullptr) {
        return ERR_INVALID_DATA;
    }
    auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
    DMError ret = RegisterDisplayManagerAgent(agent, type);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleUnRegisterDisplayManagerAgent(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleUnRegisterDisplayManagerAgent!");
    auto agent = iface_cast<IDisplayManagerAgent>(data.ReadRemoteObject());
    if (agent == nullptr) {
        return ERR_INVALID_DATA;
    }
    auto type = static_cast<DisplayManagerAgentType>(data.ReadUint32());
    DMError ret = UnregisterDisplayManagerAgent(agent, type);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleGetFoldDisplayMode(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleGetFoldDisplayMode!");
    FoldDisplayMode displayMode = GetFoldDisplayMode();
    reply.WriteUint32(static_cast<uint32_t>(displayMode));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleSetFoldDisplayMode(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleSetFoldDisplayMode!");
    FoldDisplayMode displayMode = static_cast<FoldDisplayMode>(data.ReadUint32());
    SetFoldDisplayMode(displayMode);
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleIsFoldable(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleIsFoldable!");
    reply.WriteBool(IsFoldable());
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleGetFoldStatus(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleGetFoldStatus!");
    reply.WriteUint32(static_cast<uint32_t>(GetFoldStatus()));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleGetDefaultDisplayInfo(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleGetDefaultDisplayInfo!");
    auto info = GetDefaultDisplayInfo();
    reply.WriteParcelable(info);
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleGetDisplayById(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleGetDisplayById!");
    DisplayId displayId = data.ReadUint64();
    auto info = GetDisplayInfoById(displayId);
    reply.WriteParcelable(info);
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleGetCutoutInfo(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleGetCutoutInfo!");
    DisplayId displayId = static_cast<DisplayId>(data.ReadUint64());
    sptr<CutoutInfo> cutoutInfo = GetCutoutInfo(displayId);
    reply.WriteParcelable(cutoutInfo);
    return ERR_NONE;
}
/*
 * used by powermgr
 */
int ScreenSessionManagerLiteStub::HandleWakeUpBegin(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleWakeUpBegin!");
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(WakeUpBegin(reason));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleWakeUpEnd(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleWakeUpEnd!");
    reply.WriteBool(WakeUpEnd());
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleSuspendBegin(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleSuspendBegin!");
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SuspendBegin(reason));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleSuspendEnd(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleSuspendEnd!");
    reply.WriteBool(SuspendEnd());
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleSetSpecifiedScreenPower(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleSetSpecifiedScreenPower!");
    ScreenId screenId = static_cast<ScreenId>(data.ReadUint32());
    ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SetSpecifiedScreenPower(screenId, state, reason));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleSetScreenPowerForAll(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleSetScreenPowerForAll!");
    ScreenPowerState state = static_cast<ScreenPowerState>(data.ReadUint32());
    PowerStateChangeReason reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    reply.WriteBool(SetScreenPowerForAll(state, reason));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleGetScreenPower(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleGetScreenPower!");
    ScreenId dmsScreenId;
    if (!data.ReadUint64(dmsScreenId)) {
        WLOGFE("fail to read dmsScreenId.");
        return ERR_INVALID_DATA;
    }
    reply.WriteUint32(static_cast<uint32_t>(GetScreenPower(dmsScreenId)));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleSetDisplayState(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleSetDisplayState!");
    DisplayState state = static_cast<DisplayState>(data.ReadUint32());
    reply.WriteBool(SetDisplayState(state));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleGetDisplayState(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleGetDisplayState!");
    DisplayState state = GetDisplayState(data.ReadUint64());
    reply.WriteUint32(static_cast<uint32_t>(state));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleSetScreenBrightness(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleSetScreenBrightness!");
    uint64_t screenId = data.ReadUint64();
    uint32_t level = data.ReadUint64();
    reply.WriteBool(SetScreenBrightness(screenId, level));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleGetScreenBrightness(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleGetScreenBrightness!");
    uint64_t screenId = data.ReadUint64();
    reply.WriteUint32(GetScreenBrightness(screenId));
    return ERR_NONE;
}

int ScreenSessionManagerLiteStub::HandleGetAllDisplayIds(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleGetAllDisplayIds!");
    std::vector<DisplayId> allDisplayIds = GetAllDisplayIds();
    reply.WriteUInt64Vector(allDisplayIds);
    return ERR_NONE;
}

} // namespace OHOS::Rosen
