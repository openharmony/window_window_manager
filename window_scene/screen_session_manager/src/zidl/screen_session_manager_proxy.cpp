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
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenSessionManagerProxy" };
}

sptr<DisplayInfo> OHOS::Rosen::ScreenSessionManagerProxy::GetDefaultDisplayInfo()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetDefaultDisplayInfo: remote is nullptr");
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

DMError ScreenSessionManagerProxy::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetScreenActiveMode: remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

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
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_ACTIVE_MODE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SetScreenActiveMode: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetVirtualPixelRatio: remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

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
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::SetVirtualPixelRatioSystem(ScreenId screenId, float virtualPixelRatio)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetVirtualPixelRatioSystem: remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

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
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO_SYSTEM),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::SetDefaultDensityDpi(ScreenId screenId, float virtualPixelRatio)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetDefaultDensity: remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(screenId) || !data.WriteFloat(virtualPixelRatio)) {
        WLOGFE("write screenId/virtualPixelRatio failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_DEFAULT_DENSITY_DPI),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::SetResolution(ScreenId screenId, uint32_t width, uint32_t height,
    float virtualPixelRatio)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetResolution: remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(screenId) || !data.WriteUint32(width) ||
        !data.WriteUint32(height) || !data.WriteFloat(virtualPixelRatio)) {
        WLOGFE("write screenId/width/height/virtualPixelRatio failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_RESOLUTION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::GetDensityInCurResolution(ScreenId screenId, float& virtualPixelRatio)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetDensityInCurResolution: remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("write screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DENSITY_IN_CURRENT_RESOLUTION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    virtualPixelRatio = reply.ReadFloat();
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

DMError ScreenSessionManagerProxy::GetPixelFormat(ScreenId screenId, GraphicPixelFormat& pixelFormat)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetPixelFormat: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("GetPixelFormat: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("GetPixelFormat: WriteUint64 uint64_t failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_PIXEL_FORMAT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetPixelFormat: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    pixelFormat = static_cast<GraphicPixelFormat>(reply.ReadUint32());
    return ret;
}

DMError ScreenSessionManagerProxy::SetPixelFormat(ScreenId screenId, GraphicPixelFormat pixelFormat)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetPixelFormat: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("SetPixelFormat: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId)) || !data.WriteInt32(pixelFormat)) {
        WLOGFW("SetPixelFormat: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_PIXEL_FORMAT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SetPixelFormat: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::GetSupportedHDRFormats(ScreenId screenId, std::vector<ScreenHDRFormat>& hdrFormats)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetSupportedHDRFormats: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("GetSupportedHDRFormats: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("GetSupportedHDRFormats: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_HDR_FORMAT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetSupportedHDRFormats: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    MarshallingHelper::UnmarshallingVectorObj<ScreenHDRFormat>(reply, hdrFormats,
        [](Parcel& parcel, ScreenHDRFormat& hdrFormat) {
            uint32_t value;
            bool res = parcel.ReadUint32(value);
            hdrFormat = static_cast<ScreenHDRFormat>(value);
            return res;
        }
    );
    return ret;
}

DMError ScreenSessionManagerProxy::GetScreenHDRFormat(ScreenId screenId, ScreenHDRFormat& hdrFormat)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetScreenHDRFormat: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("GetScreenHDRFormat: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("GetScreenHDRFormat: WriteUint64 uint64_t failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_HDR_FORMAT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetScreenHDRFormat: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    hdrFormat = static_cast<ScreenHDRFormat>(reply.ReadUint32());
    return ret;
}

DMError ScreenSessionManagerProxy::SetScreenHDRFormat(ScreenId screenId, int32_t modeIdx)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetScreenHDRFormat: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("SetScreenHDRFormat: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId)) || !data.WriteInt32(modeIdx)) {
        WLOGFW("SetScreenHDRFormat: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_HDR_FORMAT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SetScreenHDRFormat: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::GetSupportedColorSpaces(ScreenId screenId,
    std::vector<GraphicCM_ColorSpaceType>& colorSpaces)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetSupportedColorSpaces: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("GetSupportedColorSpaces: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("GetSupportedColorSpaces: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_SPACE),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetSupportedColorSpaces: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    MarshallingHelper::UnmarshallingVectorObj<GraphicCM_ColorSpaceType>(reply, colorSpaces,
        [](Parcel& parcel, GraphicCM_ColorSpaceType& color) {
            uint32_t value;
            bool res = parcel.ReadUint32(value);
            color = static_cast<GraphicCM_ColorSpaceType>(value);
            return res;
        }
    );
    return ret;
}

DMError ScreenSessionManagerProxy::GetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType& colorSpace)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetScreenColorSpace: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("GetScreenColorSpace: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("GetScreenColorSpace: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_SPACE),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetScreenColorSpace: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    colorSpace = static_cast<GraphicCM_ColorSpaceType>(reply.ReadUint32());
    return ret;
}

DMError ScreenSessionManagerProxy::SetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType colorSpace)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetScreenColorSpace: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("SetScreenColorSpace: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId)) || !data.WriteInt32(colorSpace)) {
        WLOGFW("SetScreenColorSpace: Write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_SPACE),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SetScreenColorSpace: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("RegisterDisplayManagerAgent: remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
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

DMError ScreenSessionManagerProxy::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("UnregisterDisplayManagerAgent: remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
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

bool OHOS::Rosen::ScreenSessionManagerProxy::WakeUpBegin(PowerStateChangeReason reason)
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

bool OHOS::Rosen::ScreenSessionManagerProxy::WakeUpEnd()
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

bool OHOS::Rosen::ScreenSessionManagerProxy::SuspendBegin(PowerStateChangeReason reason)
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

bool OHOS::Rosen::ScreenSessionManagerProxy::SuspendEnd()
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

ScreenId OHOS::Rosen::ScreenSessionManagerProxy::GetInternalScreenId()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[UL_POWER]GetInternalScreenId remote is nullptr");
        return SCREEN_ID_INVALID;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[UL_POWER]GetInternalScreenId: WriteInterfaceToken failed");
        return SCREEN_ID_INVALID;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_INTERNAL_SCREEN_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFW("[UL_POWER]GetInternalScreenId: SendRequest failed");
        return SCREEN_ID_INVALID;
    }
    return reply.ReadUint64();
}

bool OHOS::Rosen::ScreenSessionManagerProxy::SetScreenPowerById(ScreenId screenId, ScreenPowerState state,
    PowerStateChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[UL_POWER]SetScreenPowerById remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[UL_POWER]WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint64(screenId)) {
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
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_BY_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFW("[UL_POWER]SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool OHOS::Rosen::ScreenSessionManagerProxy::SetDisplayState(DisplayState state)
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

bool OHOS::Rosen::ScreenSessionManagerProxy::SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason)
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

bool OHOS::Rosen::ScreenSessionManagerProxy::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
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
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE),
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
        WLOGFE("[UL_POWER]NotifyDisplayEvent remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[UL_POWER]WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(event))) {
        WLOGFE("[UL_POWER]Write DisplayEvent failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("[UL_POWER]SendRequest failed");
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
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return ScreenPowerState::INVALID_STATE;
    }
    return static_cast<ScreenPowerState>(reply.ReadUint32());
}

ScreenPowerState OHOS::Rosen::ScreenSessionManagerProxy::GetScreenPower()
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
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER_AUTO),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return ScreenPowerState::INVALID_STATE;
    }
    return static_cast<ScreenPowerState>(reply.ReadUint32());
}

bool OHOS::Rosen::ScreenSessionManagerProxy::TryToCancelScreenOff()
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
        data.WriteInt32(virtualOption.flags_) && data.WriteBool(virtualOption.isForShot_) &&
        data.WriteUInt64Vector(virtualOption.missionIds_) &&
        data.WriteUint32(static_cast<uint32_t>(virtualOption.virtualScreenType_));
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

DMError ScreenSessionManagerProxy::AddVirtualScreenBlockList(const std::vector<int32_t>& persistentIds)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS, "remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "write token failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint64(persistentIds.size())) {
        TLOGE(WmsLogTag::DMS, "write size failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    for (const auto persistentId : persistentIds) {
        if (!data.WriteInt32(persistentId)) {
            TLOGE(WmsLogTag::DMS, "write persistentId failed");
            return DMError::DM_ERROR_IPC_FAILED;
        }
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_ADD_VIRTUAL_SCREEN_BLOCK_LIST),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::RemoveVirtualScreenBlockList(const std::vector<int32_t>& persistentIds)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS, "remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "write token failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint64(persistentIds.size())) {
        TLOGE(WmsLogTag::DMS, "write size failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    for (const auto persistentId : persistentIds) {
        if (!data.WriteInt32(persistentId)) {
            TLOGE(WmsLogTag::DMS, "write persistentId failed");
            return DMError::DM_ERROR_IPC_FAILED;
        }
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_REMOVE_VIRTUAL_SCREEN_BLOCK_LIST),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::SetScreenPrivacyMaskImage(ScreenId screenId,
    const std::shared_ptr<Media::PixelMap>& privacyMaskImg)
{
    WLOGFW("SCB: ScreenSessionManagerProxy::SetScreenPrivacyMaskImage: ENTER");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SCB: ScreenSessionManagerProxy::SetScreenPrivacyMaskImage: remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SCB: ScreenSessionManagerProxy::SetScreenPrivacyMaskImage: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (privacyMaskImg != nullptr) {
        if (!data.WriteBool(true) || !data.WriteParcelable(privacyMaskImg.get())) {
            WLOGFW("SCB: ScreenSessionManagerProxy::SetScreenPrivacyMaskImage: Write privacyMaskImg failed");
            return DMError::DM_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            WLOGFW("SCB: ScreenSessionManagerProxy::SetScreenPrivacyMaskImage: failed");
            return DMError::DM_ERROR_IPC_FAILED;
        }
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("SCB: ScreenSessionManagerProxy::SetScreenPrivacyMaskImage: Write screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_PRIVACY_MASKIMAGE),
        data, reply, option) != ERR_NONE) {
    WLOGFW("SCB: ScreenSessionManagerProxy::SetScreenPrivacyMaskImage: SendRequest failed");
    return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::SetVirtualMirrorScreenCanvasRotation(ScreenId screenId, bool canvasRotation)
{
    WLOGFW("SCB: ScreenSessionManagerProxy::SetVirtualMirrorScreenCanvasRotation: ENTER");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SCB: SetVirtualMirrorScreenCanvasRotation: remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SCB: SetVirtualMirrorScreenCanvasRotation: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUint64(static_cast<uint64_t>(screenId)) && data.WriteBool(canvasRotation);
    if (!res) {
        WLOGFW("SCB:SetVirtualMirrorScreenCanvasRotation: Write screenId/bufferRotation failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_CANVAS_ROTATION),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SCB:SetVirtualMirrorScreenCanvasRotation: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::SetVirtualMirrorScreenScaleMode(ScreenId screenId, ScreenScaleMode scaleMode)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetVirtualMirrorScreenScaleMode: remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (screenId == SCREEN_ID_INVALID) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUint64(static_cast<uint64_t>(screenId)) && data.WriteUint32(static_cast<uint32_t>(scaleMode));
    if (!res) {
        WLOGFE("Write screenId/scaleMode failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SCALE_MODE),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::ResizeVirtualScreen(ScreenId screenId, uint32_t width, uint32_t height)
{
    WLOGFI("ScreenSessionManagerProxy::ResizeVirtualScreen: ENTER");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("ScreenSessionManagerProxy::ResizeVirtualScreen: remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("ScreenSessionManagerProxy::ResizeVirtualScreen: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFE("ScreenSessionManagerProxy::ResizeVirtualScreen: WriteUnit64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(width)) {
        WLOGFE("ScreenSessionManagerProxy::ResizeVirtualScreen: WriteUnit32 width failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(height)) {
        WLOGFE("ScreenSessionManagerProxy::ResizeVirtualScreen: WriteUnit32 height failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_RESIZE_VIRTUAL_SCREEN),
        data, reply, option) != ERR_NONE) {
        WLOGFE("ScreenSessionManagerProxy::ResizeVirtualScreen fail: SendRequest failed");
        return DMError::DM_ERROR_NULLPTR;
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

DMError ScreenSessionManagerProxy::MakeMirrorForRecord(ScreenId mainScreenId,
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
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_MIRROR_FOR_RECORD),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SCB: ScreenSessionManagerProxy::MakeMirror: create mirror fail: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    screenGroupId = static_cast<ScreenId>(reply.ReadUint64());
    return ret;
}

DMError ScreenSessionManagerProxy::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenIds,
                                              DMRect mainScreenRegion, ScreenId& screenGroupId)
{
    WLOGFW("ScreenSessionManagerProxy::MakeMirror: ENTER");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("ScreenSessionManagerProxy::MakeMirror: create mirror fail: remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("ScreenSessionManagerProxy::MakeMirror: create mirror fail: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUint64(static_cast<uint64_t>(mainScreenId)) &&
        data.WriteUInt64Vector(mirrorScreenIds);
    if (!res) {
        WLOGFE("ScreenSessionManagerProxy::MakeMirror: create mirror fail: write screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(mainScreenRegion.posX_) || !data.WriteInt32(mainScreenRegion.posY_) ||
        !data.WriteUint32(mainScreenRegion.width_) || !data.WriteUint32(mainScreenRegion.height_)) {
        WLOGFE("ScreenSessionManagerProxy::MakeMirror: Write mainScreenRegion failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_MIRROR_WITH_REGION),
        data, reply, option) != ERR_NONE) {
        WLOGFW("ScreenSessionManagerProxy::MakeMirror: create mirror fail: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    screenGroupId = static_cast<ScreenId>(reply.ReadUint64());
    return ret;
}

DMError ScreenSessionManagerProxy::SetMultiScreenMode(ScreenId mainScreenId, ScreenId secondaryScreenId,
    MultiScreenMode screenMode)
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
    bool res = data.WriteUint64(static_cast<uint64_t>(mainScreenId)) &&
        data.WriteUint64(static_cast<uint64_t>(secondaryScreenId)) &&
        data.WriteUint32(static_cast<uint32_t>(screenMode));
    if (!res) {
        WLOGFE("data write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_MULTI_SCREEN_MODE_SWITCH),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    return ret;
}

DMError ScreenSessionManagerProxy::SetMultiScreenRelativePosition(MultiScreenPositionOptions mainScreenOptions,
    MultiScreenPositionOptions secondScreenOption)
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
    bool res = data.WriteUint64(mainScreenOptions.screenId_) &&
        data.WriteUint32(mainScreenOptions.startX_) && data.WriteUint32(mainScreenOptions.startY_) &&
        data.WriteUint64(secondScreenOption.screenId_) &&
        data.WriteUint32(secondScreenOption.startX_) && data.WriteUint32(secondScreenOption.startY_);
    if (!res) {
        WLOGFE("data write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_MULTI_SCREEN_POSITION),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    return ret;
}

DMError ScreenSessionManagerProxy::StopMirror(const std::vector<ScreenId>& mirrorScreenIds)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("StopMirror fail: remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("StopMirror fail: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUInt64Vector(mirrorScreenIds);
    if (!res) {
        WLOGFE("StopMirror fail: data write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_STOP_MIRROR),
        data, reply, option) != ERR_NONE) {
        WLOGFW("StopMirror fail: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::DisableMirror(bool disableOrNot)
{
    WLOGFI("SCB: ScreenSessionManagerProxy::DisableMirror %{public}d", disableOrNot);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisableMirror fail: remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("DisableMirror fail: WriteinterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteBool(disableOrNot)) {
        WLOGFE("DisableMirror fail: data write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_DISABLE_MIRROR),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisableMirror fail: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint,
                                              ScreenId& screenGroupId)
{
    WLOGFW("SCB: ScreenSessionManagerProxy::MakeExpand: ENTER");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SCB: ScreenSessionManagerProxy::MakeExpand: remote is null");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SCB: ScreenSessionManagerProxy::MakeExpand: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUInt64Vector(screenId)) {
        WLOGFE("SCB: ScreenSessionManagerProxy::MakeExpand: write screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!MarshallingHelper::MarshallingVectorObj<Point>(data, startPoint, [](Parcel& parcel, const Point& point) {
            return parcel.WriteInt32(point.posX_) && parcel.WriteInt32(point.posY_);
        })) {
        WLOGFE("SCB: ScreenSessionManagerProxy::MakeExpand: write startPoint failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_EXPAND),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SCB: ScreenSessionManagerProxy::MakeExpand: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    screenGroupId = static_cast<ScreenId>(reply.ReadUint64());
    return ret;
}

DMError ScreenSessionManagerProxy::StopExpand(const std::vector<ScreenId>& expandScreenIds)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("StopExpand fail: remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("StopExpand fail: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUInt64Vector(expandScreenIds);
    if (!res) {
        WLOGFE("StopExpand fail: data write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_STOP_EXPAND),
        data, reply, option) != ERR_NONE) {
        WLOGFW("StopExpand fail: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
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
    DmErrorCode* errorCode, bool isUseDma, bool isFullScreenCapture)
{
    WLOGFD("SCB: ScreenSessionManagerProxy::GetDisplaySnapshot enter");
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

    if (!data.WriteBool(isUseDma)) {
        WLOGFE("isUseDma fail: data write failed");
        return nullptr;
    }

    if (!data.WriteBool(isFullScreenCapture)) {
        WLOGFE("isFullScreenCapture fail: data write failed");
        return nullptr;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_SNAPSHOT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SCB: ScreenSessionManagerProxy::GetDisplaySnapshot: SendRequest failed");
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> pixelMap(reply.ReadParcelable<Media::PixelMap>());
    DmErrorCode replyErrorCode = static_cast<DmErrorCode>(reply.ReadInt32());
    if (errorCode) {
        *errorCode = replyErrorCode;
    }
    if (pixelMap == nullptr) {
        WLOGFW("SCB: ScreenSessionManagerProxy::GetDisplaySnapshot: SendRequest nullptr.");
        return nullptr;
    }
    return pixelMap;
}

std::shared_ptr<Media::PixelMap> ScreenSessionManagerProxy::GetSnapshotByPicker(Media::Rect &rect,
    DmErrorCode* errorCode)
{
    WLOGFD("enter");
    sptr<IRemoteObject> remote = Remote();
    *errorCode = DmErrorCode::DM_ERROR_SYSTEM_INNORMAL;
    if (remote == nullptr) {
        WLOGFE("remote is nullptr");
        return nullptr;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SNAPSHOT_BY_PICKER),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> pixelMap(reply.ReadParcelable<Media::PixelMap>()); // need't to check nullptr
    *errorCode = static_cast<DmErrorCode>(reply.ReadInt32());
    rect.left = reply.ReadInt32();
    rect.top = reply.ReadInt32();
    rect.width = reply.ReadInt32();
    rect.height = reply.ReadInt32();
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

sptr<DisplayInfo> ScreenSessionManagerProxy::GetVisibleAreaDisplayInfoById(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFW("WriteUint64 displayId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_VISIBLE_AREA_DISPLAY_INFO_BY_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return nullptr;
    }

    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        WLOGFW("SendRequest nullptr.");
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetAllDisplayIds: remote is null");
        return allDisplayIds;
    }

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
        WLOGFD("info modes is id: %{public}u, width: %{public}u, height: %{public}u, refreshRate: %{public}u",
            mode->id_, mode->width_, mode->height_, mode->refreshRate_);
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

DMError OHOS::Rosen::ScreenSessionManagerProxy::SetOrientation(ScreenId screenId, Orientation orientation)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("fail to set orientation: remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("fail to set orientation: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("fail to set orientation: Write screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(orientation))) {
        WLOGFW("fail to set orientation: Write orientation failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_ORIENTATION),
        data, reply, option) != ERR_NONE) {
        WLOGFW("fail to set orientation: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
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

DMError OHOS::Rosen::ScreenSessionManagerProxy::SetScreenRotationLockedFromJs(bool isLocked)
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
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED_FROM_JS),
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

sptr<CutoutInfo> ScreenSessionManagerProxy::GetCutoutInfoWithRotation(DisplayId displayId, int32_t rotation)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS, "remote is null!");
        return nullptr;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "with rotation : failed!");
        return nullptr;
    }
    if (!data.WriteUint64(displayId)) {
        TLOGE(WmsLogTag::DMS, "write displayId failed!");
        return nullptr;
    }
    if (!data.WriteInt32(rotation)) {
        TLOGE(WmsLogTag::DMS, "write rotation failed!");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_CUTOUT_INFO_WITH_ROTATION),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS, "GetCutoutInfoWithRotation failed");
        return nullptr;
    }
    sptr<CutoutInfo> info = reply.ReadParcelable<CutoutInfo>();
    return info;
}

DMError ScreenSessionManagerProxy::HasImmersiveWindow(ScreenId screenId, bool& immersive)
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
    if (!data.WriteUint64(screenId)) {
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_HAS_IMMERSIVE_WINDOW),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    immersive = reply.ReadBool();
    return ret;
}

bool OHOS::Rosen::ScreenSessionManagerProxy::ConvertScreenIdToRsScreenId(ScreenId screenId, ScreenId& rsScreenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is nullptr");
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
        return false;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_CONVERT_SCREENID_TO_RSSCREENID),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    bool ret = reply.ReadBool();
    rsScreenId = static_cast<ScreenId>(reply.ReadUint64());
    return ret;
}

DMError OHOS::Rosen::ScreenSessionManagerProxy::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
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

    if (!data.WriteUint64(displayId)) {
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_HAS_PRIVATE_WINDOW),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    hasPrivateWindow = reply.ReadBool();
    return ret;
}

void ScreenSessionManagerProxy::DumpAllScreensInfo(std::string& dumpInfo)
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
        WLOGFE("failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_DUMP_ALL_SCREEN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("failed");
        return;
    }
    dumpInfo = reply.ReadString();
}

void ScreenSessionManagerProxy::DumpSpecialScreenInfo(ScreenId id, std::string& dumpInfo)
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
        WLOGFE("failed");
        return;
    }
    if (!data.WriteUint64(id)) {
        WLOGFE("write ScreenId failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_DUMP_SPECIAL_SCREEN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("failed");
        return;
    }
    dumpInfo = reply.ReadString();
}

//Fold Screen
void ScreenSessionManagerProxy::SetFoldDisplayMode(const FoldDisplayMode displayMode)
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

//SetFoldDisplayModeFromJs add DMError to return DMErrorCode for js
DMError ScreenSessionManagerProxy::SetFoldDisplayModeFromJs(const FoldDisplayMode displayMode, std::string reason)
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
        WLOGFE("WriteInterfaceToken Failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(displayMode))) {
        WLOGFE("Write displayMode failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteString(reason)) {
        WLOGFE("Write reason failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_FOLD_DISPLAY_MODE_FROM_JS),
                            data, reply, option) != ERR_NONE) {
        WLOGFE("Send request failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    DMError ret = static_cast<DMError>(reply.ReadInt32());
    return ret;
}

void ScreenSessionManagerProxy::SetDisplayScale(ScreenId screenId, float scaleX, float scaleY, float pivotX,
    float pivotY)
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
    if (!(data.WriteUint64(static_cast<uint64_t>(screenId)) && data.WriteFloat(scaleX) && data.WriteFloat(scaleY) &&
        data.WriteFloat(pivotX) && data.WriteFloat(pivotY))) {
        WLOGFE("Write screen scale info failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_SET_DISPLAY_SCALE),
                            data, reply, option) != ERR_NONE) {
        WLOGFE("Send TRANS_ID_SCENE_BOARD_SET_DISPLAY_SCALE request failed");
    }
}

void ScreenSessionManagerProxy::SetFoldStatusLocked(bool locked)
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
    if (!data.WriteUint32(static_cast<uint32_t>(locked))) {
        WLOGFE("Write lock fold display status failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
                            DisplayManagerMessage::TRANS_ID_SCENE_BOARD_LOCK_FOLD_DISPLAY_STATUS),
                            data, reply, option) != ERR_NONE) {
        WLOGFE("Send TRANS_ID_SCENE_BOARD_LOCK_FOLD_DISPLAY_STATUS request failed");
    }
}

// SetFoldStatusLockedFromJs add DMError to return DMErrorCode for js
DMError ScreenSessionManagerProxy::SetFoldStatusLockedFromJs(bool locked)
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
        WLOGFE("WriteInterfaceToken Failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(locked))) {
        WLOGFE("Write lock fold display status failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
                            DisplayManagerMessage::TRANS_ID_SET_LOCK_FOLD_DISPLAY_STATUS_FROM_JS),
                            data, reply, option) != ERR_NONE) {
        WLOGFE("Send request failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    DMError ret = static_cast<DMError>(reply.ReadInt32());
    return ret;
}

void ScreenSessionManagerProxy::SetFoldStatusExpandAndLocked(bool locked)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS, "remote is null");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "WriteInterfaceToken Failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(locked))) {
        TLOGE(WmsLogTag::DMS, "Write lock fold display status failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
                            DisplayManagerMessage::TRANS_ID_SET_FOLD_STATUS_EXPAND_AND_LOCKED),
                            data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "Send TRANS_ID_SET_FOLD_STATUS_EXPAND_AND_LOCKED request failed");
    }
}

FoldDisplayMode ScreenSessionManagerProxy::GetFoldDisplayMode()
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

bool ScreenSessionManagerProxy::IsFoldable()
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
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_IS_FOLDABLE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool ScreenSessionManagerProxy::IsCaptured()
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
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_DEVICE_IS_CAPTURE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

FoldStatus ScreenSessionManagerProxy::GetFoldStatus()
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

SuperFoldStatus ScreenSessionManagerProxy::GetSuperFoldStatus()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is null");
        return SuperFoldStatus::UNKNOWN;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return SuperFoldStatus::UNKNOWN;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_SUPER_FOLD_STATUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return SuperFoldStatus::UNKNOWN;
    }
    return static_cast<SuperFoldStatus>(reply.ReadUint32());
}

float ScreenSessionManagerProxy::GetSuperRotation()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS, "remote is null");
        return -1.f;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "WriteInterfaceToken failed");
        return -1.f;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_SUPER_ROTATION),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return -1.f;
    }
    return reply.ReadFloat();
}

void ScreenSessionManagerProxy::SetLandscapeLockStatus(bool isLocked)
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
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteBool(isLocked)) {
        WLOGFE("Write isLocked failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_LANDSCAPE_LOCK_STATUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::SetForceCloseHdr(ScreenId screenId, bool isForceCloseHdr)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS, "remote is null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "WriteInterfaceToken failed");
        return;
    }
    if (!(data.WriteUint64(static_cast<uint64_t>(screenId)))) {
        TLOGE(WmsLogTag::DMS, "Write screenId failed");
        return;
    }
    if (!data.WriteBool(isForceCloseHdr)) {
        TLOGE(WmsLogTag::DMS, "Write isForceCloseHdr failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_FORCE_CLOSE_HDR),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return;
    }
}

ExtendScreenConnectStatus ScreenSessionManagerProxy::GetExtendScreenConnectStatus()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is null");
        return ExtendScreenConnectStatus::UNKNOWN;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return ExtendScreenConnectStatus::UNKNOWN;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_EXTEND_SCREEN_CONNECT_STATUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return ExtendScreenConnectStatus::UNKNOWN;
    }
    return static_cast<ExtendScreenConnectStatus>(reply.ReadUint32());
}

sptr<FoldCreaseRegion> ScreenSessionManagerProxy::GetCurrentFoldCreaseRegion()
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
    if (remote->SendRequest(
        static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_CURRENT_FOLD_CREASE_REGION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return nullptr;
    }
    return reply.ReadStrongParcelable<FoldCreaseRegion>();
}

DMError ScreenSessionManagerProxy::MakeUniqueScreen(const std::vector<ScreenId>& screenIds,
    std::vector<DisplayId>& displayIds)
{
    WLOGFI("ScreenSessionManagerProxy::MakeUniqueScreen");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("make unique screen failed: remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("MakeUniqueScreen writeInterfaceToken failed");
        return DMError::DM_ERROR_NULLPTR;
    }
    if (!data.WriteUint32(screenIds.size())) {
        WLOGFE("MakeUniqueScreen write screenIds size failed");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    bool res = data.WriteUInt64Vector(screenIds);
    if (!res) {
        WLOGFE("MakeUniqueScreen fail: write screens failed");
        return DMError::DM_ERROR_NULLPTR;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN),
        data, reply, option) != ERR_NONE) {
        WLOGFE("MakeUniqueScreen fail: SendRequest failed");
        return DMError::DM_ERROR_NULLPTR;
    }
    reply.ReadUInt64Vector(&displayIds);
    return static_cast<DMError>(reply.ReadInt32());
}

void ScreenSessionManagerProxy::SetClient(const sptr<IScreenSessionManagerClient>& client)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SetClient: remote is null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!client || !data.WriteRemoteObject(client->AsObject())) {
        WLOGFE("WriteRemoteObject failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_CLIENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::SwitchUser()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SwitchUser: remote is null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SWITCH_USER),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

ScreenProperty ScreenSessionManagerProxy::GetScreenProperty(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetScreenProperty: remote is null");
        return {};
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return {};
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return {};
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_PROPERTY),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return {};
    }
    ScreenProperty screenProperty;
    if (!RSMarshallingHelper::Unmarshalling(reply, screenProperty)) {
        WLOGFE("Read screenProperty failed");
        return {};
    }
    return screenProperty;
}

std::shared_ptr<RSDisplayNode> ScreenSessionManagerProxy::GetDisplayNode(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetDisplayNode: remote is null");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_NODE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return nullptr;
    }

    auto displayNode = RSDisplayNode::Unmarshalling(reply);
    if (!displayNode) {
        WLOGFE("displayNode is null");
        return nullptr;
    }
    return displayNode;
}

ScreenCombination ScreenSessionManagerProxy::GetScreenCombination(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetScreenCombination: remote is null");
        return ScreenCombination::SCREEN_ALONE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return ScreenCombination::SCREEN_ALONE;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return ScreenCombination::SCREEN_ALONE;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_COMBINATION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return ScreenCombination::SCREEN_ALONE;
    }
    return static_cast<ScreenCombination>(reply.ReadUint32());
}

void ScreenSessionManagerProxy::UpdateScreenDirectionInfo(ScreenId screenId, float screenComponentRotation,
    float rotation, float phyRotation, ScreenPropertyChangeType screenPropertyChangeType)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("UpdateScreenDirectionInfo: remote is null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return;
    }
    if (!data.WriteFloat(screenComponentRotation)) {
        WLOGFE("Write screenComponentRotation failed");
        return;
    }
    if (!data.WriteFloat(rotation)) {
        WLOGFE("Write rotation failed");
        return;
    }
    if (!data.WriteFloat(phyRotation)) {
        WLOGFE("Write phyRotation failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(screenPropertyChangeType))) {
        WLOGFE("Write screenPropertyChangeType failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_UPDATE_SCREEN_DIRECTION_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::UpdateScreenRotationProperty(ScreenId screenId, const RRect& bounds, float rotation,
    ScreenPropertyChangeType screenPropertyChangeType)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("UpdateScreenRotationProperty: remote is null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return;
    }
    if (!RSMarshallingHelper::Marshalling(data, bounds)) {
        WLOGFE("Write bounds failed");
        return;
    }
    if (!data.WriteFloat(rotation)) {
        WLOGFE("Write rotation failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(screenPropertyChangeType))) {
        WLOGFE("Write screenPropertyChangeType failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_UPDATE_SCREEN_ROTATION_PROPERTY),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

uint32_t ScreenSessionManagerProxy::GetCurvedCompressionArea()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetCurvedCompressionArea: remote is null");
        return 0;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return 0;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_CURVED_SCREEN_COMPRESSION_AREA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return 0;
    }

    return reply.ReadUint32();
}

ScreenProperty ScreenSessionManagerProxy::GetPhyScreenProperty(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetPhyScreenProperty: remote is null");
        return {};
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return {};
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return {};
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_PHY_SCREEN_PROPERTY),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return {};
    }
    ScreenProperty screenProperty;
    if (!RSMarshallingHelper::Unmarshalling(reply, screenProperty)) {
        WLOGFE("Read screenProperty failed");
        return {};
    }
    return screenProperty;
}

void ScreenSessionManagerProxy::NotifyDisplayChangeInfoChanged(const sptr<DisplayChangeInfo>& info)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("NotifyDisplayChangeInfoChanged: remote is null");
        return;
    }

    MessageOption option(MessageOption::TF_ASYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!info->Marshalling(data)) {
        WLOGFE("Write display change info failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_CHANGE_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::SetScreenPrivacyState(bool hasPrivate)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SetScreenPrivacyState: remote is null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteBool(hasPrivate)) {
        WLOGFE("Write hasPrivate failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_PRIVACY_STATE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::SetPrivacyStateByDisplayId(DisplayId id, bool hasPrivate)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SetPrivacyStateByDisplayId: remote is null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(id)) {
        WLOGFE("Write DisplayId failed");
        return;
    }
    if (!data.WriteBool(hasPrivate)) {
        WLOGFE("Write hasPrivate failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREENID_PRIVACY_STATE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::SetScreenPrivacyWindowList(DisplayId id, std::vector<std::string> privacyWindowList)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SetScreenPrivacyWindowList: remote is null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(id)) {
        WLOGFE("Write DisplayId failed");
        return;
    }
    if (!data.WriteStringVector(privacyWindowList)) {
        WLOGFE("Write privacyWindowList failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_PRIVACY_WINDOW_LIST),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::UpdateAvailableArea(ScreenId screenId, DMRect area)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("UpdateAvailableArea: remote is null");
        return;
    }

    MessageOption option(MessageOption::TF_ASYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return;
    }
    if (!data.WriteInt32(area.posX_) || !data.WriteInt32(area.posY_) || !data.WriteUint32(area.width_) ||
        !data.WriteInt32(area.height_)) {
        WLOGFE("Write area failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_UPDATE_AVAILABLE_AREA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::UpdateSuperFoldAvailableArea(ScreenId screenId, DMRect bArea, DMRect cArea)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("UpdateSuperFoldAvailableArea: remote is null");
        return;
    }

    MessageOption option(MessageOption::TF_ASYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return;
    }
    if (!data.WriteInt32(bArea.posX_) || !data.WriteInt32(bArea.posY_) || !data.WriteUint32(bArea.width_) ||
        !data.WriteUint32(bArea.height_)) {
        WLOGFE("Write bArea failed");
        return;
    }
    if (!data.WriteInt32(cArea.posX_) || !data.WriteInt32(cArea.posY_) || !data.WriteUint32(cArea.width_) ||
        !data.WriteUint32(cArea.height_)) {
        WLOGFE("Write cArea failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_UPDATE_SUPER_FOLD_AVAILABLE_AREA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::UpdateSuperFoldExpandAvailableArea(ScreenId screenId, DMRect area)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("UpdateSuperFoldExpandAvailableArea: remote is null");
        return;
    }

    MessageOption option(MessageOption::TF_ASYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return;
    }
    if (!data.WriteInt32(area.posX_) || !data.WriteInt32(area.posY_) || !data.WriteUint32(area.width_) ||
        !data.WriteUint32(area.height_)) {
        WLOGFE("Write area failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_UPDATE_SUPER_FOLD_EXPAND_AVAILABLE_AREA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

int32_t ScreenSessionManagerProxy::SetScreenOffDelayTime(int32_t delay)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SetScreenOffDelayTime: remote is null");
        return 0;
    }

    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return 0;
    }
    if (!data.WriteInt32(delay)) {
        WLOGFE("Write delay failed");
        return 0;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_OFF_DELAY_TIME),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return 0;
    }
    return reply.ReadInt32();
}

void ScreenSessionManagerProxy::SetCameraStatus(int32_t cameraStatus, int32_t cameraPosition)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SetCameraStatus: remote is null");
        return;
    }

    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteInt32(cameraStatus)) {
        WLOGFE("Write cameraStatus failed");
        return;
    }
    if (!data.WriteInt32(cameraPosition)) {
        WLOGFE("Write cameraPosition failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_CAMERA_STATUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

DMError ScreenSessionManagerProxy::GetAvailableArea(DisplayId displayId, DMRect& area)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetAvailableArea: remote is null");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageOption option;
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write displayId failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_AVAILABLE_AREA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    int32_t posX = reply.ReadInt32();
    int32_t posY = reply.ReadInt32();
    uint32_t width = reply.ReadUint32();
    uint32_t height = reply.ReadUint32();
    area = {posX, posY, width, height};
    return ret;
}

DMError ScreenSessionManagerProxy::GetExpandAvailableArea(DisplayId displayId, DMRect& area)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetExpandAvailableArea: remote is null");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageOption option;
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write displayId failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_EXPAND_AVAILABLE_AREA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    int32_t posX = reply.ReadInt32();
    int32_t posY = reply.ReadInt32();
    uint32_t width = reply.ReadUint32();
    uint32_t height = reply.ReadUint32();
    area = {posX, posY, width, height};
    return ret;
}

void ScreenSessionManagerProxy::NotifyFoldToExpandCompletion(bool foldToExpand)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("NotifyFoldToExpandCompletion: remote is null");
        return;
    }

    MessageOption option(MessageOption::TF_ASYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return ;
    }
    if (!data.WriteBool(foldToExpand)) {
        WLOGFE("Write foldToExpand failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_NOTIFY_FOLD_TO_EXPAND_COMPLETION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::RecordEventFromScb(std::string description, bool needRecordEvent)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("RecordEventFromScb: remote is null");
        return;
    }

    MessageOption option(MessageOption::TF_ASYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return ;
    }
    if (!data.WriteString(description)) {
        WLOGFE("Write description failed");
        return;
    }
    if (!data.WriteBool(needRecordEvent)) {
        WLOGFE("Write needRecordEvent failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_RECORD_EVENT_FROM_SCB),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

VirtualScreenFlag ScreenSessionManagerProxy::GetVirtualScreenFlag(ScreenId screenId)
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
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return VirtualScreenFlag::DEFAULT;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write screenId failed");
        return VirtualScreenFlag::DEFAULT;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_VIRTUAL_SCREEN_FLAG),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return VirtualScreenFlag::DEFAULT;
    }
    return static_cast<VirtualScreenFlag>(reply.ReadUint32());
}

DMError ScreenSessionManagerProxy::SetVirtualScreenFlag(ScreenId screenId, VirtualScreenFlag screenFlag)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SetVirtualScreenFlag: remote is null");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (screenId == SCREEN_ID_INVALID) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    if (screenFlag < VirtualScreenFlag::DEFAULT || screenFlag >= VirtualScreenFlag::MAX) {
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(screenId) || !data.WriteUint32(static_cast<uint32_t>(screenFlag))) {
        WLOGFE("Write screenId or screenFlag failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_FLAG),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DeviceScreenConfig ScreenSessionManagerProxy::GetDeviceScreenConfig()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetDeviceScreenConfig: remote is null");
        return {};
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "WriteInterfaceToken failed");
        return {};
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DEVICE_SCREEN_CONFIG),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return {};
    }
    DeviceScreenConfig deviceScreenConfig;
    if (!RSMarshallingHelper::Unmarshalling(reply, deviceScreenConfig)) {
        TLOGE(WmsLogTag::DMS, "Read deviceScreenConfig failed");
        return {};
    }
    return deviceScreenConfig;
}

DMError ScreenSessionManagerProxy::SetVirtualScreenRefreshRate(ScreenId screenId, uint32_t refreshInterval)
{
    WLOGFI("ScreenSessionManagerProxy::SetVirtualScreenRefreshRate: ENTER");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SetVirtualScreenRefreshRate: remote is null");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(screenId) || !data.WriteUint32(refreshInterval)) {
        WLOGFE("Write screenId or refreshInterval failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_REFRESH_RATE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

void ScreenSessionManagerProxy::SetVirtualScreenBlackList(ScreenId screenId, std::vector<uint64_t>& windowIdList,
    std::vector<uint64_t> surfaceIdList, std::vector<uint8_t> typeBlackList)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("Remote is nullptr");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFE("Write screenId failed");
        return;
    }
    if (!data.WriteUInt64Vector(windowIdList)) {
        WLOGFE("Write windowIdList failed");
        return;
    }
    if (!data.WriteUInt64Vector(surfaceIdList)) {
        WLOGFE("Write surfaceIdList failed");
        return;
    }
    if (!data.WriteUInt8Vector(typeBlackList)) {
        WLOGFE("Write typeBlackList failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_BLACK_LIST),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::SetVirtualDisplayMuteFlag(ScreenId screenId, bool muteFlag)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("Remote is nullptr");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFE("Write screenId failed");
        return;
    }
    if (!data.WriteBool(muteFlag)) {
        WLOGFE("Write muteFlag failed");
        return;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_DISPLAY_MUTE_FLAG),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::DisablePowerOffRenderControl(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("Remote is nullptr");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFE("Write screenId failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_DISABLE_POWEROFF_RENDER_CONTROL),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

DMError ScreenSessionManagerProxy::ProxyForFreeze(const std::set<int32_t>& pidList, bool isProxy)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("Remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    MessageParcel reply;
    MessageParcel data;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("proxy for freeze: failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteInt32(pidList.size())) {
        WLOGFE("proxy for freeze write date: failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }
    for (auto it = pidList.begin(); it != pidList.end(); it++) {
        if (!data.WriteInt32(*it)) {
            WLOGFE("proxy for freeze write date: failed");
            return DMError::DM_ERROR_WRITE_DATA_FAILED;
        }
    }
    if (!data.WriteBool(isProxy)) {
        WLOGFE("proxy for freeze write date: failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_PROXY_FOR_FREEZE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("proxy for freeze send request: failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError ScreenSessionManagerProxy::ResetAllFreezeStatus()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("Remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    MessageParcel reply;
    MessageParcel data;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_RESET_ALL_FREEZE_STATUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

void OHOS::Rosen::ScreenSessionManagerProxy::UpdateDisplayHookInfo(int32_t uid, bool enable, const DMHookInfo& hookInfo)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS, "remote is nullptr");
        return;
    }

    MessageOption option(MessageOption::TF_ASYNC);
    MessageParcel reply;
    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteInt32(uid)) {
        TLOGE(WmsLogTag::DMS, "Write uid failed");
        return;
    }

    if (!data.WriteBool(enable)) {
        TLOGE(WmsLogTag::DMS, "Write enable failed");
        return;
    }

    if (!data.WriteUint32(hookInfo.width_) || !data.WriteUint32(hookInfo.height_) ||
        !data.WriteFloat(hookInfo.density_) || !data.WriteUint32(hookInfo.rotation_) ||
        !data.WriteBool(hookInfo.enableHookRotation_)) {
        TLOGE(WmsLogTag::DMS, "Write hookInfo failed");
        return;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_HOOK_INFO),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::GetDisplayHookInfo(int32_t uid, DMHookInfo& hookInfo)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS, "remote is nullptr");
        return;
    }

    MessageOption option;
    MessageParcel reply;
    MessageParcel data;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteInt32(uid)) {
        TLOGE(WmsLogTag::DMS, "Write uid failed");
        return;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_HOOK_INFO),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return;
    }
    if (!reply.ReadUint32(hookInfo.width_) || !reply.ReadUint32(hookInfo.height_) ||
        !reply.ReadFloat(hookInfo.density_) || !reply.ReadUint32(hookInfo.rotation_)||
        !reply.ReadBool(hookInfo.enableHookRotation_)) {
        TLOGE(WmsLogTag::DMS, "reply write hookInfo failed!");
    }
}

DMError ScreenSessionManagerProxy::SetVirtualScreenSecurityExemption(ScreenId screenId, uint32_t pid,
    std::vector<uint64_t>& windowIdList)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("Remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    MessageParcel reply;
    MessageParcel data;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("proxy for freeze: failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("write date: failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }
    if (!data.WriteUint32(pid)) {
        WLOGFE("write date: failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }

    if (!data.WriteUInt64Vector(windowIdList)) {
        WLOGFE("write date: failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("send request: failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

std::vector<DisplayPhysicalResolution> ScreenSessionManagerProxy::GetAllDisplayPhysicalResolution()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS, "remote is nullptr");
        return std::vector<DisplayPhysicalResolution> {};
    }
    MessageOption option;
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "WriteInterfaceToken failed");
        return std::vector<DisplayPhysicalResolution> {};
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_ALL_PHYSICAL_DISPLAY_RESOLUTION),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return std::vector<DisplayPhysicalResolution> {};
    }
    std::vector<DisplayPhysicalResolution> allPhysicalSize;
    int32_t displayInfoSize = 0;
    bool readRet = reply.ReadInt32(displayInfoSize);
    if (!readRet || displayInfoSize <= 0) {
        return std::vector<DisplayPhysicalResolution> {};
    }
    for (int32_t i = 0; i < displayInfoSize; i++) {
        DisplayPhysicalResolution physicalItem;
        physicalItem.foldDisplayMode_ = static_cast<FoldDisplayMode>(reply.ReadUint32());
        physicalItem.physicalWidth_ = reply.ReadUint32();
        physicalItem.physicalHeight_ = reply.ReadUint32();
        allPhysicalSize.emplace_back(physicalItem);
    }
    return allPhysicalSize;
}

DMError ScreenSessionManagerProxy::GetDisplayCapability(std::string& capabilitInfo)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS, "remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_CAPABILITY),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    DMError ret = static_cast<DMError>(reply.ReadInt32());
    capabilitInfo = reply.ReadString();
    return ret;
}

bool ScreenSessionManagerProxy::SetVirtualScreenStatus(ScreenId screenId, VirtualScreenStatus screenStatus)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS, "remote is nullptr");
        return false;
    }

    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "WriteInterfaceToken failed");
        return false;
    }

    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGE(WmsLogTag::DMS, "Write screenId failed");
        return false;
    }

    if (!data.WriteInt32(static_cast<int32_t>(screenStatus))) {
        TLOGE(WmsLogTag::DMS, "Write screenStatus failed");
        return false;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_STATUS),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return false;
    }

    return reply.ReadBool();
}

DMError ScreenSessionManagerProxy::SetVirtualScreenMaxRefreshRate(ScreenId id, uint32_t refreshRate,
    uint32_t& actualRefreshRate)
{
    WLOGFI("ENTER");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(id))) {
        WLOGFE("WriteUnit64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(refreshRate)) {
        WLOGFE("WriteUnit32 width failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_MAX_REFRESHRATE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return DMError::DM_ERROR_NULLPTR;
    }
    actualRefreshRate = reply.ReadUint32();
    return static_cast<DMError>(reply.ReadInt32());
}

std::shared_ptr<Media::PixelMap> ScreenSessionManagerProxy::GetScreenCapture(const CaptureOption& captureOption,
    DmErrorCode* errorCode)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is nullptr");
        return nullptr;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(captureOption.displayId_) ||
        !data.WriteBool(captureOption.isNeedNotify_) || !data.WriteBool(captureOption.isNeedPointer_)) {
        WLOGFE("Write displayId or isNeedNotify or isNeedPointer failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_CAPTURE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> pixelMap(reply.ReadParcelable<Media::PixelMap>());
    DmErrorCode replyErrorCode = static_cast<DmErrorCode>(reply.ReadInt32());
    if (errorCode) {
        *errorCode = replyErrorCode;
    }
    if (pixelMap == nullptr) {
        WLOGFE("Send pixelMap nullptr.");
        return nullptr;
    }
    return pixelMap;
}

sptr<DisplayInfo> ScreenSessionManagerProxy::GetPrimaryDisplayInfo()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetPrimaryDisplayInfo: remote is nullptr");
        return nullptr;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_PRIMARY_DISPLAY_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return nullptr;
    }

    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        WLOGFE("get display info.");
    }
    return info;
}

std::shared_ptr<Media::PixelMap> ScreenSessionManagerProxy::GetDisplaySnapshotWithOption(
    const CaptureOption& captureOption, DmErrorCode* errorCode)
{
    WLOGFD("enter");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(captureOption.displayId_) ||
        !data.WriteBool(captureOption.isNeedNotify_) || !data.WriteBool(captureOption.isNeedPointer_) ||
        !data.WriteBool(captureOption.isFullScreenCapture_)) {
        WLOGFE("Write displayId or isNeedNotify or isNeedPointer failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_SNAPSHOT_WITH_OPTION),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> pixelMap(reply.ReadParcelable<Media::PixelMap>());
    DmErrorCode replyErrorCode = static_cast<DmErrorCode>(reply.ReadInt32());
    if (errorCode) {
        *errorCode = replyErrorCode;
    }
    if (pixelMap == nullptr) {
        WLOGFW("SendRequest nullptr.");
        return nullptr;
    }
    return pixelMap;
}

int32_t ScreenSessionManagerProxy::SetScreenOnDelayTime(int32_t delay)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("SetScreenOnDelayTime: remote is null");
        return 0;
    }

    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return 0;
    }
    if (!data.WriteInt32(delay)) {
        WLOGFE("Write delay failed");
        return 0;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_ON_DELAY_TIME),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return 0;
    }
    return reply.ReadInt32();
}

DMError ScreenSessionManagerProxy::SetScreenSkipProtectedWindow(const std::vector<ScreenId>& screenIds, bool isEnable)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS, "remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUInt64Vector(screenIds)) {
        TLOGE(WmsLogTag::DMS, "Write screenIds failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }
    if (!data.WriteBool(isEnable)) {
        TLOGE(WmsLogTag::DMS, "Write isEnable failed");
        return DMError::DM_ERROR_WRITE_DATA_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_SKIP_PROTECTED_WINDOW),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

bool ScreenSessionManagerProxy::GetIsRealScreen(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS, "remote is nullptr");
        return false;
    }
    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGE(WmsLogTag::DMS, "Write screenId failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_IS_REAL_SCREEN),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

void ScreenSessionManagerProxy::SetDefaultMultiScreenModeWhenSwitchUser()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return;
    }

    MessageParcel reply;
    MessageParcel data;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_DEFAULT_MODE_WHEN_SWITCH_USER),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
    return;
}

void ScreenSessionManagerProxy::NotifyExtendScreenCreateFinish()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return;
    }

    MessageParcel reply;
    MessageParcel data;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_NOTIFY_EXTEND_SCREEN_CREATE_FINISH),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::NotifyExtendScreenDestroyFinish()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return;
    }

    MessageParcel reply;
    MessageParcel data;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_NOTIFY_EXTEND_SCREEN_DESTROY_FINISH),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void ScreenSessionManagerProxy::NotifyScreenMaskAppear()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return;
    }

    MessageParcel reply;
    MessageParcel data;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_NOTIFY_SCREEN_MASK_APPEAR),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

DMError ScreenSessionManagerProxy::GetScreenAreaOfDisplayArea(DisplayId displayId, const DMRect& displayArea,
    ScreenId& screenId, DMRect& screenArea)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS, "remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(displayId))) {
        TLOGE(WmsLogTag::DMS, "Write displayId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(displayArea.posX_) || !data.WriteInt32(displayArea.posY_) ||
        !data.WriteUint32(displayArea.width_) || !data.WriteUint32(displayArea.height_)) {
        TLOGE(WmsLogTag::DMS, "Write displayArea failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_AREA_OF_DISPLAY_AREA),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    screenId = static_cast<ScreenId>(reply.ReadUint64());
    int32_t posX = reply.ReadInt32();
    int32_t posY = reply.ReadInt32();
    uint32_t width = reply.ReadUint32();
    uint32_t height = reply.ReadUint32();
    screenArea = { posX, posY, width, height };
    return ret;
}
} // namespace OHOS::Rosen
