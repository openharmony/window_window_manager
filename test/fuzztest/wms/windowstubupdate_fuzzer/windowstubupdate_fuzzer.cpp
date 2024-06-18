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
#include "window.h"
#include "window_agent.h"
#include "window_impl.h"
#include "window_manager.h"
#include "windowstubupdate_fuzzer.h"

using namespace OHOS::Rosen;

namespace OHOS {
namespace {
constexpr size_t DATA_MIN_SIZE = 2;
}

void WindowStubUpdateTest(sptr<WindowAgent> windowStub, MessageParcel& parcel)
{
    MessageParcel reply;
    MessageOption option;
    parcel.RewindRead(0);
    windowStub->OnRemoteRequest(static_cast<uint32_t>(IWindow::WindowMessage::TRANS_ID_UPDATE_WINDOW_RECT),
        parcel, reply, option);
    parcel.RewindRead(0);
    windowStub->OnRemoteRequest(static_cast<uint32_t>(IWindow::WindowMessage::TRANS_ID_UPDATE_WINDOW_MODE),
        parcel, reply, option);
    parcel.RewindRead(0);
    windowStub->OnRemoteRequest(static_cast<uint32_t>(IWindow::WindowMessage::TRANS_ID_UPDATE_FOCUS_STATUS),
        parcel, reply, option);
    parcel.RewindRead(0);
    windowStub->OnRemoteRequest(static_cast<uint32_t>(IWindow::WindowMessage::TRANS_ID_UPDATE_MODE_SUPPORT_INFO),
        parcel, reply, option);
    parcel.RewindRead(0);
    windowStub->OnRemoteRequest(static_cast<uint32_t>(IWindow::WindowMessage::TRANS_ID_UPDATE_AVOID_AREA),
        parcel, reply, option);
    parcel.RewindRead(0);
    windowStub->OnRemoteRequest(static_cast<uint32_t>(IWindow::WindowMessage::TRANS_ID_UPDATE_WINDOW_STATE),
        parcel, reply, option);
    parcel.RewindRead(0);
    windowStub->OnRemoteRequest(static_cast<uint32_t>(IWindow::WindowMessage::TRANS_ID_UPDATE_DRAG_EVENT),
        parcel, reply, option);
    parcel.RewindRead(0);
    windowStub->OnRemoteRequest(static_cast<uint32_t>(IWindow::WindowMessage::TRANS_ID_UPDATE_DISPLAY_ID),
        parcel, reply, option);
    parcel.RewindRead(0);
    windowStub->OnRemoteRequest(static_cast<uint32_t>(IWindow::WindowMessage::TRANS_ID_UPDATE_OCCUPIED_AREA),
        parcel, reply, option);
    parcel.RewindRead(0);
    windowStub->OnRemoteRequest(static_cast<uint32_t>(IWindow::WindowMessage::TRANS_ID_UPDATE_OCCUPIED_AREA_AND_RECT),
        parcel, reply, option);
    parcel.RewindRead(0);
    windowStub->OnRemoteRequest(static_cast<uint32_t>(IWindow::WindowMessage::TRANS_ID_UPDATE_ACTIVE_STATUS),
        parcel, reply, option);
    parcel.RewindRead(0);
    windowStub->OnRemoteRequest(static_cast<uint32_t>(IWindow::WindowMessage::TRANS_ID_UPDATE_ZOOM_TRANSFORM),
        parcel, reply, option);
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < DATA_MIN_SIZE) {
        return false;
    }
    
    MessageParcel parcel;
    MessageParcel reply;
    MessageOption option;

    parcel.WriteInterfaceToken(WindowStub::GetDescriptor());
    parcel.WriteBuffer(data, size);

    sptr<WindowOption> windowOption = new(std::nothrow)WindowOption();
    if (windowOption == nullptr) {
        return false;
    }
    sptr<WindowImpl> window = new(std::nothrow)WindowImpl(windowOption);
    if (window == nullptr) {
        return false;
    }
    sptr<WindowAgent> windowStub = new(std::nothrow)WindowAgent(window);
    if (windowStub == nullptr) {
        return false;
    }
    
    WindowStubUpdateTest(windowStub, parcel);
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