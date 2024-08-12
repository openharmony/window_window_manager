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

#include <cstddef>
#include <cstdint>
#include <parcel.h>
#include <securec.h>

#include <iremote_stub.h>
#include "message_option.h"
#include "message_parcel.h"
#include "marshalling_helper.h"
#include "screen_session_manager_client_interface.h"
#include "screen_session_manager_client_stub.h"
#include "screensessionmanagerclientstub_fuzzer.h"
#include "screen_session_manager_client.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
}
template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}
void ScreenSessionMgrClientStubTest1(MessageParcel &parcel, MessageParcel &reply, MessageOption &option)
{
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_CONNECTION_CHANGED), parcel, reply, option);
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_ON_PROPERTY_CHANGED), parcel, reply, option);
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_ON_POWER_STATUS_CHANGED), parcel, reply, option);
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_ON_SENSOR_ROTATION_CHANGED), parcel, reply, option);
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_ORIENTATION_CHANGED), parcel, reply, option);
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_ROTATION_LOCKED_CHANGED), parcel, reply, option);
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_ON_DISPLAY_STATE_CHANGED), parcel, reply, option);
    return;
}
void ScreenSessionMgrClientStubTest2(MessageParcel &parcel, MessageParcel &reply, MessageOption &option)
{
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_ON_SCREEN_SHOT), parcel, reply, option);
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_ON_IMMERSIVE_STATE_CHANGED), parcel, reply, option);
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_SET_DISPLAY_NODE_SCREEN_ID), parcel, reply, option);
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_GET_SURFACENODEID_FROM_MISSIONID), parcel, reply, option);
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_SET_FOLD_DISPLAY_MODE), parcel, reply, option);
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_ON_SWITCH_USER_CMD), parcel, reply, option);
    parcel.RewindRead(0);
    ScreenSessionManagerClient::GetInstance().OnRemoteRequest(
        static_cast<uint32_t>(Rosen::IScreenSessionManagerClient::
        ScreenSessionManagerClientMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO_SYSTEM), parcel, reply, option);
    return;
}
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    
    MessageParcel parcel;
    MessageParcel reply;
    MessageOption option;

    parcel.WriteInterfaceToken(ScreenSessionManagerClientStub::GetDescriptor());
    parcel.WriteBuffer(data, size);

    ScreenSessionMgrClientStubTest1(parcel, reply, option);
    ScreenSessionMgrClientStubTest2(parcel, reply, option);
    return true;
}
} // namespace.OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}