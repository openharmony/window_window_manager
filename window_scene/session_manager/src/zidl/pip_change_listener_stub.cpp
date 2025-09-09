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

#include "session_manager/include/zidl/pip_change_listener_stub.h"
#include "window_manager_hilog.h"
#include "hilog_tag_wrapper.h"

namespace OHOS::Rosen {

int PipChangeListenerStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
                                           MessageOption& option)
{
    TLOGD(WmsLogTag::WMS_PIP, "In");
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TLOGE(WmsLogTag::WMS_PIP, "local descriptor not equal to remote.");
        return ERR_TRANSACTION_FAILED;
    }
    return HandleOnPipChange(data, reply);
}

int PipChangeListenerStub::HandleOnPipChange(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_PIP, "In");
    int32_t windowId = 0;
    if (!data.ReadInt32(windowId)) {
        TLOGE(WmsLogTag::WMS_PIP, "Failed to read windowId");
        return ERR_INVALID_DATA;
    }
    OnPipStart(windowId);
    return ERR_NONE;
}
}