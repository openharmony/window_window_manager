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

#include "zidl/screen_session_manager_client_proxy.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {

void ScreenSessionManagerClientProxy::OnScreenConnectionChanged(SessionOption SessionOption, ScreenEvent screenEvent)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(SessionOption.rsId_)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write rsId failed");
        return;
    }
    if (!data.WriteString(SessionOption.name_)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write name failed");
        return;
    }
    if (!data.WriteBool(SessionOption.isExtend_)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write isExtended failed");
        return;
    }
    if (!data.WriteString(SessionOption.innerName_)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write innerName failed");
        return;
    }
    if (!data.WriteUint64(SessionOption.screenId_)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (!data.WriteUint8(static_cast<uint8_t>(screenEvent))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenEvent failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_CONNECTION_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::SwitchUserCallback(std::vector<int32_t> oldScbPids, int32_t currentScbPid)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteInt32Vector(oldScbPids)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write oldScbPids failed");
        return;
    }
    if (!data.WriteInt32(currentScbPid)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write currentScbPid failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SWITCH_USER_CMD),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnScreenExtendChanged(ScreenId mainScreenId, ScreenId extendScreenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(mainScreenId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (!data.WriteUint64(extendScreenId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenOrientation failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_EXTEND_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnPropertyChanged(ScreenId screenId,
    const ScreenProperty& property, ScreenPropertyChangeReason reason)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (!RSMarshallingHelper::Marshalling(data, property)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write property failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write reason failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_PROPERTY_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnPowerStatusChanged(DisplayPowerEvent event, EventStatus status,
    PowerStateChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(event))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write event failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(status))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write status failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write reason failed");
        return;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed, Remote is nullptr");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_POWER_STATUS_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnSensorRotationChanged(ScreenId screenId, float sensorRotation)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (!data.WriteFloat(sensorRotation)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write sensorRotation failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SENSOR_ROTATION_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnHoverStatusChanged(ScreenId screenId, int32_t hoverStatus, bool needRotate)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (!data.WriteInt32(hoverStatus)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write hoverStatus failed");
        return;
    }
    if (!data.WriteBool(needRotate)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write needRotate failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_HOVER_STATUS_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnScreenOrientationChanged(ScreenId screenId, float screenOrientation)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (!data.WriteFloat(screenOrientation)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenOrientation failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_ORIENTATION_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnScreenRotationLockedChanged(ScreenId screenId, bool isLocked)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }
    
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (!data.WriteBool(isLocked)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write isLocked failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_ROTATION_LOCKED_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnDisplayStateChanged(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(defaultDisplayId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write defaultDisplayId failed");
        return;
    }
    if (!data.WriteStrongParcelable(displayInfo)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write displayInfo failed");
        return;
    }
    auto mapSize = static_cast<uint32_t>(displayInfoMap.size());
    if (!data.WriteUint32(mapSize)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write mapSize failed");
        return;
    }
    for (auto [id, info] : displayInfoMap) {
        if (!data.WriteUint64(id)) {
            TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write id failed");
            return;
        }
        if (!data.WriteStrongParcelable(info)) {
            TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write info failed");
            return;
        }
    }
    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write type failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_DISPLAY_STATE_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnGetSurfaceNodeIdsFromMissionIdsChanged(std::vector<uint64_t>& missionIds,
    std::vector<uint64_t>& surfaceNodeIds, bool isBlackList)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUInt64Vector(missionIds)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write missionIds failed");
        return;
    }
    if (!data.WriteUInt64Vector(surfaceNodeIds)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write surfaceNodeIds failed");
        return;
    }
    if (!data.WriteBool(isBlackList)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write isBlackList failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
        ScreenSessionManagerClientMessage::TRANS_ID_GET_SURFACENODEID_FROM_MISSIONID),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
    reply.ReadUInt64Vector(&surfaceNodeIds);
}

void ScreenSessionManagerClientProxy::OnUpdateFoldDisplayMode(FoldDisplayMode displayMode)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(displayMode))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write displayMode failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_SET_FOLD_DISPLAY_MODE),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnScreenshot(DisplayId displayId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(displayId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write displayId failed");
        return;
    }
    auto remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed, Remote is nullptr");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_SHOT),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnImmersiveStateChanged(ScreenId screenId, bool& immersive)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_IMMERSIVE_STATE_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
    immersive = reply.ReadBool();
}

void ScreenSessionManagerClientProxy::SetDisplayNodeScreenId(ScreenId screenId, ScreenId displayNodeScreenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (!data.WriteUint64(displayNodeScreenId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write displayNodeScreenId failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_SET_DISPLAY_NODE_SCREEN_ID),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::SetVirtualPixelRatioSystem(ScreenId screenId, float virtualPixelRatio)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId) || !data.WriteFloat(virtualPixelRatio)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId/virtualPixelRatio failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO_SYSTEM),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnFoldStatusChangedReportUE(const std::vector<std::string>& screenFoldInfo)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }
    
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteStringVector(screenFoldInfo)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenFoldInfo failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
        ScreenSessionManagerClientMessage::TRANS_ID_ON_FOLDSTATUS_CHANGED_REPORT_UE),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::ScreenCaptureNotify(ScreenId mainScreenId, int32_t uid,
    const std::string& clientName)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(mainScreenId) || !data.WriteInt32(uid) || !data.WriteString(clientName)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId or uid or client failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_CAPTURE_NOTIFY),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnCameraBackSelfieChanged(ScreenId screenId, bool isCameraBackSelfie)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(screenId)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (!data.WriteBool(isCameraBackSelfie)) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write isCameraBackSelfie failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_CAMERA_BACKSELFIE_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnSuperFoldStatusChanged(ScreenId screenId, SuperFoldStatus superFoldStatus)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(superFoldStatus))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write superFoldStatus failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SUPER_FOLD_STATUS_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnExtendScreenConnectStatusChanged(ScreenId screenId,
    ExtendScreenConnectStatus extendScreenConnectStatus)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(extendScreenConnectStatus))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write extendScreenConnectStatus failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_EXTEND_SCREEN_CONNECT_STATUS_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}

void ScreenSessionManagerClientProxy::OnSecondaryReflexionChanged(ScreenId screenId, bool isSecondaryReflexion)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "remote is nullptr");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write screenId failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(isSecondaryReflexion))) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "Write secondaryReflexion failed");
        return;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SECONDARY_REFLEXION_CHANGED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DMS_SSM_CLIENT, "SendRequest failed");
        return;
    }
}
} // namespace OHOS::Rosen
