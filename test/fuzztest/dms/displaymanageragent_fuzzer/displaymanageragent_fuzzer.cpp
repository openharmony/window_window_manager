/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "displaymanageragent_fuzzer.h"

#include "display_manager_agent_default.h"
#include "zidl/display_manager_agent_stub.h"
#include "zidl/idisplay_manager_agent.h"

namespace OHOS ::Rosen {
void DisplayManagerAgentFuzzTest(const uint8_t* data, size_t size)
{
    MessageParcel parcel;
    MessageParcel reply;
    MessageOption option;

    parcel.WriteInterfaceToken(DisplayManagerAgentStub::GetDescriptor());
    parcel.WriteBuffer(data, size);
    sptr<DisplayManagerAgentStub> stub = new DisplayManagerAgentDefault();

    for (uint32_t i = static_cast<uint32_t>(IDisplayManagerAgent::TRANS_ID_NOTIFY_DISPLAY_POWER_EVENT);
         i <= static_cast<uint32_t>(IDisplayManagerAgent::TRANS_ID_NOTIFY_ABNORMAL_SCREEN_CONNECT_CHANGED);
         i += 1) {
        parcel.RewindRead(0);
        stub->OnRemoteRequest(i, parcel, reply, option);
    }
}
} // namespace OHOS::Rosen

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DisplayManagerAgentFuzzTest(data, size);
    return 0;
}
