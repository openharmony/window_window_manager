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

#include "session_manager/include/zidl/session_router_stack_listener_stub.h"
#include "window_manager_hilog.h"
#include "hilog_tag_wrapper.h"

namespace OHOS::Rosen {

int SessionRouterStackListenerStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TLOGE(WmsLogTag::WMS_LIFE, "local descriptor not equal to remote.");
        return ERR_INVALID_STATE;
    }
    return HandleOnGetRouterStackInfo(data, reply);
}

int SessionRouterStackListenerStub::HandleOnGetRouterStackInfo(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LIFE, "in");
    sptr<RouterStackInfo> routerStackInfo(data.ReadParcelable<RouterStackInfo>());
    if (!routerStackInfo) {
        TLOGE(WmsLogTag::WMS_LIFE, "Invalid routerStackInfo.");
        return ERR_INVALID_DATA;
    }
    SendRouterStackInfo(routerStackInfo);
    return ERR_NONE;
}
}
