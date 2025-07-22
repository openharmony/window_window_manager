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
#include "session/host/include/zidl/session_ipc_interface_code.h"
#include "session/host/include/zidl/session_stub.h"
#include "session/host/include/session.h"
#include "sessionstubextension_fuzzer.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
}

void SessionStubTestExtensionCode(sptr<Session> sessionStub, MessageParcel& parcel)
{
    MessageParcel reply;
    MessageOption option;
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_TRANSFER_ABILITY_RESULT),
        parcel, reply, option);
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_TRANSFER_EXTENSION_DATA),
        parcel, reply, option);
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_DIED),
        parcel, reply, option);
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_NOTIFY_ASYNC_ON),
        parcel, reply, option);
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_NOTIFY_SYNC_ON),
        parcel, reply, option);
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_TRIGGER_BIND_MODAL_UI_EXTENSION),
        parcel, reply, option);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_NOTIFY_REPORT_ACCESSIBILITY_EVENT),
        parcel, reply, option);
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_TIMEOUT),
        parcel, reply, option);
    return;
}

void SessionStubTestPipCode(sptr<Session> sessionStub, MessageParcel& parcel)
{
    MessageParcel reply;
    MessageOption option;
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_NOTIFY_PIP_WINDOW_PREPARE_CLOSE),
        parcel, reply, option);
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_UPDATE_PIP_RECT),
        parcel, reply, option);
    return;
}

void SessionStubTestFbCode(sptr<Session> sessionStub, MessageParcel& parcel)
{
    MessageParcel reply;
    MessageOption option;
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_UPDATE_FLOATING_BALL),
        parcel, reply, option);
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_NOTIFY_FLOATING_BALL_PREPARE_CLOSE),
        parcel, reply, option);
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_START_FLOATING_BALL_MAIN_WINDOW),
        parcel, reply, option);
    parcel.RewindRead(0);
    sessionStub->OnRemoteRequest(
        static_cast<uint32_t>(Rosen::SessionInterfaceCode::TRANS_ID_GET_FLOATING_BALL_WINDOW_ID),
        parcel, reply, option);
    return;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    
    MessageParcel parcel;

    parcel.WriteInterfaceToken(SessionStub::GetDescriptor());
    parcel.WriteBuffer(data, size);
    parcel.RewindRead(0);

    SessionInfo info;
    info.abilityName_ = "stubConnectFuzzTest";
    info.bundleName_ = "stubConnectFuzzTest";
    sptr<Session> sessionStub = new (std::nothrow) Session(info);
    if (sessionStub == nullptr) {
        return false;
    }

    SessionStubTestExtensionCode(sessionStub, parcel);
    SessionStubTestPipCode(sessionStub, parcel);
    SessionStubTestFbCode(sessionStub, parcel);
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