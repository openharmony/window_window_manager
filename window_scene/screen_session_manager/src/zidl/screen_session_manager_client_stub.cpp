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
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenSessionManagerClientStub" };
} // namespace

const std::map<uint32_t, ScreenSessionManagerClientStub::StubFunc> ScreenSessionManagerClientStub::stubFuncMap_ {
    { static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_CONNECTION_CHANGED),
        &ScreenSessionManagerClientStub::HandleOnScreenConnectionChanged },
    { static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_PROPERTY_CHANGED),
        &ScreenSessionManagerClientStub::HandleOnPropertyChanged },
    { static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SENSOR_ROTATION_CHANGED),
        &ScreenSessionManagerClientStub::HandleOnSensorRotationChanged },
    { static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_ORIENTATION_CHANGED),
        &ScreenSessionManagerClientStub::HandleOnScreenOrientationChanged },
    { static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_ROTATION_LOCKED_CHANGED),
        &ScreenSessionManagerClientStub::HandleOnScreenRotationLockedChanged },
    { static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_DISPLAY_STATE_CHANGED),
        &ScreenSessionManagerClientStub::HandleOnDisplayStateChanged },
    { static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_SHOT),
        &ScreenSessionManagerClientStub::HandleOnScreenshot },
    { static_cast<uint32_t>(ScreenSessionManagerClientMessage::TRANS_ID_ON_IMMERSIVE_STATE_CHANGED),
        &ScreenSessionManagerClientStub::HandleOnImmersiveStateChanged },
};

int ScreenSessionManagerClientStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    auto iter = stubFuncMap_.find(code);
    if (iter == stubFuncMap_.end()) {
        WLOGFE("Failed to find function handler!");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return (this->*(iter->second))(data, reply);
}

int ScreenSessionManagerClientStub::HandleOnScreenConnectionChanged(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleOnScreenConnectionChanged");
    auto screenId = static_cast<ScreenId>(data.ReadUint64());
    auto screenEvent = static_cast<ScreenEvent>(data.ReadUint8());
    OnScreenConnectionChanged(screenId, screenEvent);
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
        auto displayInfo = data.ReadStrongParcelable<DisplayInfo>();
        displayInfoMap.emplace(displayId, displayInfo);
    }
    auto type = static_cast<DisplayStateChangeType>(data.ReadUint32());
    OnDisplayStateChanged(defaultDisplayId, displayInfo, displayInfoMap, type);
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
} // namespace OHOS::Rosen
