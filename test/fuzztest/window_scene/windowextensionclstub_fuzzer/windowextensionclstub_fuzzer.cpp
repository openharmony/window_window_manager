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
#include "window_extension_client_interface.h"
#include "window_extension_client_stub.h"
#include "window_extension_client_stub_impl.h"
#include "windowextensionclstub_fuzzer.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    constexpr size_t DATA_MIN_SIZE = 2;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    
    MessageParcel parcel;
    MessageParcel reply;
    MessageOption option;

    parcel.WriteInterfaceToken(WindowExtensionClientStub::GetDescriptor());
    parcel.WriteBuffer(data, size);
    parcel.RewindRead(0);
    sptr<IWindowExtensionCallback> componentCallback;
    std::shared_ptr<WindowExtensionClientStubImpl> extensionStubClient = std::make_shared<WindowExtensionClientStubImpl>(componentCallback);
    extensionStubClient->OnRemoteRequest(
        static_cast<uint32_t>(IWindowExtensionClient::TRANS_ID_ON_WINDOW_READY),
        parcel, reply, option);
    extensionStubClient->OnRemoteRequest(
        static_cast<uint32_t>(IWindowExtensionClient::TRANS_ID_ON_BACK_PRESS),
        parcel, reply, option);
    extensionStubClient->OnRemoteRequest(
        static_cast<uint32_t>(IWindowExtensionClient::TRANS_ID_ON_KEY_EVENT),
        parcel, reply, option);
    extensionStubClient->OnRemoteRequest(
        static_cast<uint32_t>(IWindowExtensionClient::TRANS_ID_ON_POINTER_EVENT),
        parcel, reply, option);
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