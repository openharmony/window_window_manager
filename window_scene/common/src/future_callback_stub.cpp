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

#include "common/include/future_callback_stub.h"
#include "window_manager_hilog.h"
namespace OHOS {
namespace Rosen {
int FutureCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "OnRemoteRequest code is %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "InterfaceToken check failed");
        return ERR_INVALID_STATE;
    }

    return ProcessRemoteRequest(code, data, reply, option);
}

int FutureCallbackStub::ProcessRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    switch (code) {
        case static_cast<uint32_t>(FutureCallbackMessage::TRANS_ID_UPDATE_SESSION_RECT):
            return HandleUpdateSessionRect(data, reply);
        default:
            TLOGE(WmsLogTag::WMS_LAYOUT, "Failed to find function handler!");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int FutureCallbackStub::HandleUpdateSessionRect(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "UpdateSessionRect!");
    WSRect rect = { data.ReadInt32(), data.ReadInt32(), data.ReadUint32(), data.ReadUint32() };
    OnUpdateSessionRect(rect);
    return ERR_NONE;
}
} // namespace Rosen
} // namespace OHOS