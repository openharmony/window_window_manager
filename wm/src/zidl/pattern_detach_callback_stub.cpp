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

#include "zidl/pattern_detach_callback_stub.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

int PatternDetachCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    TLOGD(WmsLogTag::WMS_LIFE, "OnRemoteRequest code is %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TLOGE(WmsLogTag::WMS_LIFE, "InterfaceToken check failed");
        return ERR_INVALID_STATE;
    }
    PatternDetachCallbackMessage msgId = static_cast<PatternDetachCallbackMessage>(code);
    switch (msgId) {
        case PatternDetachCallbackMessage::TRANS_ID_PATTERN_ON_DETACH: {
            auto persisitentId = data.ReadInt32();
            OnPatternDetach(persisitentId);
            break;
        }
        default:
            TLOGW(WmsLogTag::WMS_LIFE, "Unknown transaction code %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_NONE;
}
} // namespace Rosen
} // namespace OHOS
