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

#include "display_manager_proxy.h"

#include <cinttypes>
#include <ipc_types.h>
#include <parcel.h>
#include <ui/rs_surface_node.h>
#include "marshalling_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {

sptr<DisplayInfo> DisplayManagerProxy::GetDefaultDisplayInfo()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DEFAULT_DISPLAY_INFO),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return nullptr;
    }
    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest nullptr.");
    }
    return info;
}

sptr<DisplayInfo> DisplayManagerProxy::GetDisplayInfoById(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(displayId)) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 displayId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_ID),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return nullptr;
    }

    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest nullptr.");
        return nullptr;
    }
    return info;
}

sptr<DisplayInfo> DisplayManagerProxy::GetVisibleAreaDisplayInfoById(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(displayId)) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 displayId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_VISIBLE_AREA_DISPLAY_INFO_BY_ID),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return nullptr;
    }

    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest nullptr.");
        return nullptr;
    }
    return info;
}

sptr<DisplayInfo> DisplayManagerProxy::GetDisplayInfoByScreen(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "fail to get displayInfo by screenId: remote is null");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "fail to get displayInfo by screenId: WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(screenId)) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "fail to get displayInfo by screenId: WriteUint64 displayId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_SCREEN),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "fail to get displayInfo by screenId: SendRequest failed");
        return nullptr;
    }

    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "fail to get displayInfo by screenId: SendRequest null");
        return nullptr;
    }
    return info;
}

ScreenId DisplayManagerProxy::CreateVirtualScreen(VirtualScreenOption virtualOption,
    const sptr<IRemoteObject>& displayManagerAgent)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return SCREEN_ID_INVALID;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return SCREEN_ID_INVALID;
    }
    bool res = data.WriteString(virtualOption.name_) && data.WriteUint32(virtualOption.width_) &&
        data.WriteUint32(virtualOption.height_) && data.WriteFloat(virtualOption.density_) &&
        data.WriteInt32(virtualOption.flags_) && data.WriteBool(virtualOption.isForShot_) &&
        data.WriteUInt64Vector(virtualOption.missionIds_);
    if (virtualOption.surface_ != nullptr && virtualOption.surface_->GetProducer() != nullptr) {
        res = res &&
            data.WriteBool(true) &&
            data.WriteRemoteObject(virtualOption.surface_->GetProducer()->AsObject());
    } else {
        TLOGW(WmsLogTag::DMS_DMSERVER, "surface is nullptr");
        res = res && data.WriteBool(false);
    }
    if (displayManagerAgent != nullptr) {
        res = res &&
            data.WriteRemoteObject(displayManagerAgent);
    }
    if (!res) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "Write data failed");
        return SCREEN_ID_INVALID;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_CREATE_VIRTUAL_SCREEN),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return SCREEN_ID_INVALID;
    }

    ScreenId screenId = static_cast<ScreenId>(reply.ReadUint64());
    TLOGI(WmsLogTag::DMS_DMSERVER, "CreateVirtualScreen %" PRIu64"", screenId);
    return screenId;
}

DMError DisplayManagerProxy::DestroyVirtualScreen(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_DESTROY_VIRTUAL_SCREEN),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::SetVirtualScreenSurface(ScreenId screenId, sptr<IBufferProducer> surface)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUint64(static_cast<uint64_t>(screenId));
    if (surface != nullptr) {
        res = res &&
            data.WriteBool(true) &&
            data.WriteRemoteObject(surface->AsObject());
    } else {
        TLOGW(WmsLogTag::DMS_DMSERVER, "surface is nullptr");
        res = res && data.WriteBool(false);
    }
    if (!res) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "Write screenId/surface failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SURFACE),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::SetOrientation(ScreenId screenId, Orientation orientation)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "fail to set orientation: remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "fail to set orientation: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "fail to set orientation: Write screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(orientation))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "fail to set orientation: Write orientation failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_ORIENTATION),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "fail to set orientation: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

std::shared_ptr<Media::PixelMap> DisplayManagerProxy::GetDisplaySnapshot(DisplayId displayId,
    DmErrorCode* errorCode, bool isUseDma)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return nullptr;
    }

    if (!data.WriteUint64(displayId)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "Write displayId failed");
        return nullptr;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_SNAPSHOT),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> pixelMap(reply.ReadParcelable<Media::PixelMap>());
    DmErrorCode replyErrorCode = static_cast<DmErrorCode>(reply.ReadInt32());
    if (errorCode) {
        *errorCode = replyErrorCode;
    }
    if (pixelMap == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest nullptr.");
        return nullptr;
    }
    return pixelMap;
}

DMError DisplayManagerProxy::GetScreenSupportedColorGamuts(ScreenId screenId,
    std::vector<ScreenColorGamut>& colorGamuts)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_GAMUTS),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
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

DMError DisplayManagerProxy::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 uint64_t failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_GAMUT),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    colorGamut = static_cast<ScreenColorGamut>(reply.ReadUint32());
    return ret;
}

DMError DisplayManagerProxy::SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId)) || !data.WriteInt32(colorGamutIdx)) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "Write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_GAMUT),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_GAMUT_MAP),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    gamutMap = static_cast<ScreenGamutMap>(reply.ReadUint32());
    return ret;
}

DMError DisplayManagerProxy::SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId)) || !data.WriteUint32(static_cast<uint32_t>(gamutMap))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "Writ failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_GAMUT_MAP),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::SetScreenColorTransform(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_TRANSFORM),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::GetPixelFormat(ScreenId screenId, GraphicPixelFormat& pixelFormat)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 uint64_t failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_PIXEL_FORMAT),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    pixelFormat = static_cast<GraphicPixelFormat>(reply.ReadUint32());
    return ret;
}

DMError DisplayManagerProxy::SetPixelFormat(ScreenId screenId, GraphicPixelFormat pixelFormat)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId)) || !data.WriteInt32(pixelFormat)) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_PIXEL_FORMAT),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::GetSupportedHDRFormats(ScreenId screenId, std::vector<ScreenHDRFormat>& hdrFormats)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_HDR_FORMAT),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
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

DMError DisplayManagerProxy::GetScreenHDRFormat(ScreenId screenId, ScreenHDRFormat& hdrFormat)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 uint64_t failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_HDR_FORMAT),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    hdrFormat = static_cast<ScreenHDRFormat>(reply.ReadUint32());
    return ret;
}

DMError DisplayManagerProxy::SetScreenHDRFormat(ScreenId screenId, int32_t modeIdx)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId)) || !data.WriteInt32(modeIdx)) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_HDR_FORMAT),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::GetSupportedColorSpaces(ScreenId screenId,
    std::vector<GraphicCM_ColorSpaceType>& colorSpaces)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_SPACE),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
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

DMError DisplayManagerProxy::GetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType& colorSpace)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_SPACE),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    colorSpace = static_cast<GraphicCM_ColorSpaceType>(reply.ReadUint32());
    return ret;
}

DMError DisplayManagerProxy::SetScreenColorSpace(ScreenId screenId, GraphicCM_ColorSpaceType colorSpace)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId)) || !data.WriteInt32(colorSpace)) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "Write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_SPACE),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }

    if (!data.WriteRemoteObject(displayManagerAgent->AsObject())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "Write IDisplayManagerAgent failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "Write DisplayManagerAgent type failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }

    if (!data.WriteRemoteObject(displayManagerAgent->AsObject())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "Write IWindowManagerAgent failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "Write DisplayManagerAgent type failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

bool DisplayManagerProxy::WakeUpBegin(PowerStateChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]WakeUpBegin: remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]Write PowerStateChangeReason failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_WAKE_UP_BEGIN),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::WakeUpEnd()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]WakeUpEnd: remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]WriteInterfaceToken failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_WAKE_UP_END),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SuspendBegin(PowerStateChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SuspendBegin: remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]Write PowerStateChangeReason failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SUSPEND_BEGIN),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SuspendEnd()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SuspendEnd: remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]WriteInterfaceToken failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SUSPEND_END),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SetScreenPowerForAll: remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]Write ScreenPowerState failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]Write PowerStateChangeReason failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SetSpecifiedScreenPower(ScreenId screenId, ScreenPowerState state, PowerStateChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SetSpecifiedScreenPower: remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(screenId))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]Write ScreenId failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]Write ScreenPowerState failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]Write PowerStateChangeReason failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SPECIFIED_SCREEN_POWER),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

ScreenPowerState DisplayManagerProxy::GetScreenPower(ScreenId dmsScreenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return ScreenPowerState::INVALID_STATE;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return ScreenPowerState::INVALID_STATE;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(dmsScreenId))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "Write dmsScreenId failed");
        return ScreenPowerState::INVALID_STATE;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return ScreenPowerState::INVALID_STATE;
    }
    return static_cast<ScreenPowerState>(reply.ReadUint32());
}

bool DisplayManagerProxy::SetDisplayState(DisplayState state)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SetDisplayState: remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]Write DisplayState failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_DISPLAY_STATE),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

DisplayState DisplayManagerProxy::GetDisplayState(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DisplayState::UNKNOWN;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DisplayState::UNKNOWN;
    }
    if (!data.WriteUint64(displayId)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "Write displayId failed");
        return DisplayState::UNKNOWN;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DisplayState::UNKNOWN;
    }
    return static_cast<DisplayState>(reply.ReadUint32());
}

bool DisplayManagerProxy::TryToCancelScreenOff()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]TryToCancelScreenOff: remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]WriteInterfaceToken failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_TRY_TO_CANCEL_SCREEN_OFF),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

std::vector<DisplayId> DisplayManagerProxy::GetAllDisplayIds()
{
    std::vector<DisplayId> allDisplayIds;
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return allDisplayIds;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return allDisplayIds;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_ALL_DISPLAYIDS),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return allDisplayIds;
    }
    reply.ReadUInt64Vector(&allDisplayIds);
    return allDisplayIds;
}

sptr<CutoutInfo> DisplayManagerProxy::GetCutoutInfo(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is null");
        return nullptr;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "GetCutoutInfo failed");
        return nullptr;
    }
    if (!data.WriteUint64(displayId)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write displayId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_CUTOUT_INFO),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "GetCutoutInfo failed");
        return nullptr;
    }
    sptr<CutoutInfo> info = reply.ReadParcelable<CutoutInfo>();
    return info;
}

DMError DisplayManagerProxy::AddSurfaceNodeToDisplay(DisplayId displayId,
    std::shared_ptr<class RSSurfaceNode>& surfaceNode, bool onTop)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint64(displayId)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write displayId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (surfaceNode == nullptr || !surfaceNode->Marshalling(data)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "Write windowProperty failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_ADD_SURFACE_NODE),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "Send request failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadUint32());
    return ret;
}

DMError DisplayManagerProxy::RemoveSurfaceNodeFromDisplay(DisplayId displayId,
    std::shared_ptr<class RSSurfaceNode>& surfaceNode)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint64(displayId)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write displayId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (surfaceNode == nullptr || !surfaceNode->Marshalling(data)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "Write windowProperty failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_REMOVE_SURFACE_NODE),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "Send request failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadUint32());
    return ret;
}

DMError DisplayManagerProxy::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint64(displayId)) {
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_HAS_PRIVATE_WINDOW),
        data, reply, option) != ERR_NONE) {
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    hasPrivateWindow = reply.ReadBool();
    return ret;
}

void DisplayManagerProxy::NotifyDisplayEvent(DisplayEvent event)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(event))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "[UL_POWER]Write DisplayEvent failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_EVENT),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "[UL_POWER]SendRequest failed");
        return;
    }
}

bool DisplayManagerProxy::SetFreeze(std::vector<DisplayId> displayIds, bool isFreeze)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUInt64Vector(displayIds)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write displayId failed.");
        return false;
    }
    if (!data.WriteBool(isFreeze)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write freeze flag failed.");
        return false;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_FREEZE_EVENT),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return false;
    }
    return true;
}

DMError DisplayManagerProxy::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId,
                                        ScreenId& screenGroupId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUint64(static_cast<uint64_t>(mainScreenId)) &&
        data.WriteUInt64Vector(mirrorScreenId);
    if (!res) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "data write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_MIRROR),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    screenGroupId = static_cast<ScreenId>(reply.ReadUint64());
    return ret;
}

DMError DisplayManagerProxy::StopMirror(const std::vector<ScreenId>& mirrorScreenIds)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUInt64Vector(mirrorScreenIds);
    if (!res) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "data write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_STOP_MIRROR),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

sptr<ScreenInfo> DisplayManagerProxy::GetScreenInfoById(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(screenId)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "Write screenId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return nullptr;
    }

    sptr<ScreenInfo> info = reply.ReadStrongParcelable<ScreenInfo>();
    if (info == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest nullptr.");
        return nullptr;
    }
    for (auto& mode : info->GetModes()) {
        TLOGI(WmsLogTag::DMS_DMSERVER, "info modes is id: %{public}u, width: %{public}u, height: %{public}u, "
            "refreshRate: %{public}u", mode->id_, mode->width_, mode->height_, mode->refreshRate_);
    }
    return info;
}

sptr<ScreenGroupInfo> DisplayManagerProxy::GetScreenGroupInfoById(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(screenId)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "Write screenId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return nullptr;
    }

    sptr<ScreenGroupInfo> info = reply.ReadStrongParcelable<ScreenGroupInfo>();
    if (info == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest nullptr.");
        return nullptr;
    }
    return info;
}

DMError DisplayManagerProxy::GetAllScreenInfos(std::vector<sptr<ScreenInfo>>& screenInfos)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_ALL_SCREEN_INFOS),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    static_cast<void>(MarshallingHelper::UnmarshallingVectorParcelableObj<ScreenInfo>(reply, screenInfos));
    return ret;
}

DMError DisplayManagerProxy::MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint,
                                        ScreenId& screenGroupId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is null");
        return DMError::DM_ERROR_IPC_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUInt64Vector(screenId)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!MarshallingHelper::MarshallingVectorObj<Point>(data, startPoint, [](Parcel& parcel, const Point& point) {
            return parcel.WriteInt32(point.posX_) && parcel.WriteInt32(point.posY_);
        })) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write startPoint failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_EXPAND),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    screenGroupId = static_cast<ScreenId>(reply.ReadUint64());
    return ret;
}

DMError DisplayManagerProxy::StopExpand(const std::vector<ScreenId>& expandScreenIds)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUInt64Vector(expandScreenIds);
    if (!res) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "data write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_STOP_EXPAND),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

void DisplayManagerProxy::RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "cancel make mirror or expand fail: remote is null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "cancel make mirror or expand fail: WriteInterfaceToken failed");
        return;
    }
    bool res = data.WriteUInt64Vector(screens);
    if (!res) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "cancel make mirror or expand fail: write screens failed.");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
        DisplayManagerMessage::TRANS_ID_REMOVE_VIRTUAL_SCREEN_FROM_SCREEN_GROUP),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "cancel make mirror or expand fail: SendRequest failed");
    }
}

DMError DisplayManagerProxy::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(screenId) || !data.WriteUint32(modeId)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write screenId/modeId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_ACTIVE_MODE),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SetVirtualPixelRatio: remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(screenId) || !data.WriteFloat(virtualPixelRatio)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write screenId/modeId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::SetResolution(ScreenId screenId, uint32_t width, uint32_t height, float virtualPixelRatio)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SetResolution: remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(screenId) || !data.WriteUint32(width) ||
        !data.WriteUint32(height) || !data.WriteFloat(virtualPixelRatio)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write screenId/width/height/virtualPixelRatio failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_RESOLUTION),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::GetDensityInCurResolution(ScreenId screenId, float& virtualPixelRatio)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "GetDensityInCurResolution: remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(screenId)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DENSITY_IN_CURRENT_RESOLUTION),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    virtualPixelRatio = reply.ReadFloat();
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::IsScreenRotationLocked(bool& isLocked)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_IS_SCREEN_ROTATION_LOCKED),
        data, reply, option) != ERR_NONE) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    isLocked = reply.ReadBool();
    return ret;
}

DMError DisplayManagerProxy::SetScreenRotationLocked(bool isLocked)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteBool(isLocked)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write isLocked failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::SetScreenRotationLockedFromJs(bool isLocked)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteBool(isLocked)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write isLocked failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED_FROM_JS),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::ResizeVirtualScreen(ScreenId screenId, uint32_t width, uint32_t height)
{
    TLOGI(WmsLogTag::DMS_DMSERVER, "ENTER");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteUnit64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(width)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteUnit32 width failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(height)) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteUnit32 height failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_RESIZE_VIRTUAL_SCREEN),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return DMError::DM_ERROR_NULLPTR;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::MakeUniqueScreen(const std::vector<ScreenId>& screenIds,
    std::vector<DisplayId>& displayIds)
{
    TLOGI(WmsLogTag::DMS_DMSERVER, "enter");
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGW(WmsLogTag::DMS_DMSERVER, "remote is null");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "writeInterfaceToken failed");
        return DMError::DM_ERROR_NULLPTR;
    }
    if (!data.WriteUint32(screenIds.size())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write screenIds size failed");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    bool res = data.WriteUInt64Vector(screenIds);
    if (!res) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "write screens failed");
        return DMError::DM_ERROR_NULLPTR;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "fSendRequest failed");
        return DMError::DM_ERROR_NULLPTR;
    }
    reply.ReadUInt64Vector(&displayIds);
    return static_cast<DMError>(reply.ReadInt32());
}

std::vector<DisplayPhysicalResolution> DisplayManagerProxy::GetAllDisplayPhysicalResolution()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "remote is nullptr");
        return std::vector<DisplayPhysicalResolution> {};
    }
    MessageOption option;
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "WriteInterfaceToken failed");
        return std::vector<DisplayPhysicalResolution> {};
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_ALL_PHYSICAL_DISPLAY_RESOLUTION),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "SendRequest failed");
        return std::vector<DisplayPhysicalResolution> {};
    }
    std::vector<DisplayPhysicalResolution> allPhysicalSize;
    int32_t displayInfoSize = 0;
    bool readRet = reply.ReadInt32(displayInfoSize);
    if (!readRet || displayInfoSize <= 0) {
        TLOGE(WmsLogTag::DMS_DMSERVER, "read failed");
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
} // namespace OHOS::Rosen