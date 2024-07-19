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

#include "zidl/screen_session_manager_client_stub.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DMS_SCREEN_CLIENT,
                                          "ScreenSessionManagerClientStub" };
} // namespace

int ScreenSessionManagerClientStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }
    ScreenSessionManagerClientMessage msgId = static_cast<ScreenSessionManagerClientMessage>(code);
    switch (msgId) {
        case ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_CONNECTION_CHANGED: {
            return HandleOnScreenConnectionChanged(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_ON_PROPERTY_CHANGED: {
            return HandleOnPropertyChanged(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_ON_POWER_STATUS_CHANGED: {
            return HandleOnPowerStatusChanged(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_ON_SENSOR_ROTATION_CHANGED: {
            return HandleOnSensorRotationChanged(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_ORIENTATION_CHANGED: {
            return HandleOnScreenOrientationChanged(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_ROTATION_LOCKED_CHANGED: {
            return HandleOnScreenRotationLockedChanged(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_ON_DISPLAY_STATE_CHANGED: {
            return HandleOnDisplayStateChanged(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_SHOT: {
            return HandleOnScreenshot(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_ON_IMMERSIVE_STATE_CHANGED: {
            return HandleOnImmersiveStateChanged(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_SET_DISPLAY_NODE_SCREEN_ID: {
            return HandleOnSetDisplayNodeScreenId(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_GET_SURFACENODEID_FROM_MISSIONID: {
            return HandleOnGetSurfaceNodeIdsFromMissionIdsChanged(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_SET_FOLD_DISPLAY_MODE: {
            return HandleOnUpdateFoldDisplayMode(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_ON_SWITCH_USER_CMD: {
            return HandleSwitchUserCallback(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO_SYSTEM: {
            return HandleSetVirtualPixelRatioSystem(data, reply);
        }
        case ScreenSessionManagerClientMessage::TRANS_ID_ON_FOLDSTATUS_CHANGED_REPORT_UE: {
            return HandleOnFoldStatusChangedReportUE(data, reply);
        }
        default: {
            WLOGFE("Failed to find function handler!");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }

    return 0;
}

int ScreenSessionManagerClientStub::HandleSwitchUserCallback(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleSwitchUserCallback");
    std::vector<int32_t> oldScbPids;
    data.ReadInt32Vector(&oldScbPids);
    int32_t currentScbPid = data.ReadInt32();
    SwitchUserCallback(oldScbPids, currentScbPid);
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnScreenConnectionChanged(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleOnScreenConnectionChanged");
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    auto screenEvent = static_cast<ScreenEvent>(data.ReadUint8());
    auto rsId = static_cast<ScreenId>(data.ReadUint64());
    auto name = data.ReadString();
    OnScreenConnectionChanged(screenId, screenEvent, rsId, name);
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnPropertyChanged(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleOnPropertyChanged");
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    ScreenProperty property;
    if (!RSMarshallingHelper::Unmarshalling(data, property)) {
        WLOGFE("Read property failed");
        return ERR_INVALID_DATA;
    }
    auto reason = static_cast<ScreenPropertyChangeReason>(data.ReadUint32());
    OnPropertyChanged(screenId, property, reason);
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnPowerStatusChanged(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleOnPowerStatusChanged");
    auto event = static_cast<DisplayPowerEvent>(data.ReadUint32());
    auto status = static_cast<EventStatus>(data.ReadUint32());
    auto reason = static_cast<PowerStateChangeReason>(data.ReadUint32());
    OnPowerStatusChanged(event, status, reason);
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnSensorRotationChanged(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleOnSensorRotationChanged");
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    auto sensorRotation = data.ReadFloat();
    OnSensorRotationChanged(screenId, sensorRotation);
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnScreenOrientationChanged(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleOnScreenOrientationChanged");
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    auto screenOrientation = data.ReadFloat();
    OnScreenOrientationChanged(screenId, screenOrientation);
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnScreenRotationLockedChanged(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleOnScreenRotationLockedChanged");
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    auto isLocked = data.ReadBool();
    OnScreenRotationLockedChanged(screenId, isLocked);
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnDisplayStateChanged(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleOnDisplayStateChanged");
    auto defaultDisplayId = static_cast<DisplayId>(data.ReadUint64());
    auto displayInfo = data.ReadStrongParcelable<DisplayInfo>();
    auto mapSize = data.ReadUint32();
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    while (mapSize--) {
        auto displayId = data.ReadUint64();
        auto displayInfoValue = data.ReadStrongParcelable<DisplayInfo>();
        displayInfoMap.emplace(displayId, displayInfoValue);
    }
    auto type = static_cast<DisplayStateChangeType>(data.ReadUint32());
    OnDisplayStateChanged(defaultDisplayId, displayInfo, displayInfoMap, type);
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnUpdateFoldDisplayMode(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleOnUpdateFoldDisplayMode");
    auto foldDisplayMode = static_cast<FoldDisplayMode>(data.ReadUint32());
    OnUpdateFoldDisplayMode(foldDisplayMode);
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnGetSurfaceNodeIdsFromMissionIdsChanged(MessageParcel& data,
    MessageParcel& reply)
{
    std::vector<uint64_t> missionIds;
    data.ReadUInt64Vector(&missionIds);
    std::vector<uint64_t> surfaceNodeIds;
    data.ReadUInt64Vector(&surfaceNodeIds);
    OnGetSurfaceNodeIdsFromMissionIdsChanged(missionIds, surfaceNodeIds);
    if (!reply.WriteUInt64Vector(surfaceNodeIds)) {
        WLOGFE("Write surfaceNodeIds failed");
        return ERR_TRANSACTION_FAILED;
    }
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnScreenshot(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleOnScreenshot");
    auto displayId = static_cast<DisplayId>(data.ReadUint64());
    OnScreenshot(displayId);
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnImmersiveStateChanged(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleOnImmersiveStateChanged");
    bool immersive = false;
    OnImmersiveStateChanged(immersive);
    if (!reply.WriteBool(immersive)) {
        WLOGFE("Write immersive failed");
        return ERR_TRANSACTION_FAILED;
    }
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnSetDisplayNodeScreenId(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleOnSetDisplayNodeScreenId");
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    auto displayNodeScreenId = static_cast<ScreenId>(data.ReadUint64());
    SetDisplayNodeScreenId(screenId, displayNodeScreenId);
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleSetVirtualPixelRatioSystem(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleSetVirtualPixelRatioSystem");
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    float virtualPixelRatio = data.ReadFloat();
    SetVirtualPixelRatioSystem(screenId, virtualPixelRatio);
    return ERR_NONE;
}

int ScreenSessionManagerClientStub::HandleOnFoldStatusChangedReportUE(MessageParcel& data, MessageParcel& reply)
{
    std::vector<std::string> screenFoldInfo;
    data.ReadStringVector(&screenFoldInfo);
    OnFoldStatusChangedReportUE(screenFoldInfo);
    return ERR_NONE;
}
} // namespace OHOS::Rosen
