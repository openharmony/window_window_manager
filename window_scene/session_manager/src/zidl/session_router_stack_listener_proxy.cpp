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

#include "session_manager/include/zidl/session_router_stack_listener_proxy.h"
#include "hilog_tag_wrapper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {

void SessionRouterStackListenerProxy::SendRouterStackInfo(const sptr<RouterStackInfo>& routerStackInfo)
{
    OnGetRouterStackInfo(routerStackInfo);
}

void SessionRouterStackListenerProxy::OnGetRouterStackInfo(const sptr<RouterStackInfo>& routerStackInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write interfaceToken failed");
        return;
    }
    if (!data.WriteParcelable(routerStackInfo)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write routerStackInfo failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "remote is null");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
        SessionRouterStackListenerMessage::TRANS_ID_GET_ROUTER_STACK_INFO), data, reply, option) != NO_ERROR) {
        TLOGE(WmsLogTag::WMS_LIFE, "SendRequest failed.");
        return;
    }
}
} // namespace OHOS::Rosen
