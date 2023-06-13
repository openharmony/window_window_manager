/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "anr_observer_stub.h"

#include <message_parcel.h>

#include "window_manager_hilog.h"
#include "ws_common.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "AnrObserverStub" };
} // namespace

int32_t AnrObserverStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    WLOGFD("cmd = %{public}u, flags= %{public}d", code, option.GetFlags());
    std::u16string descripter = AnrObserverStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        WLOGFE("AnrObserverStub::OnRemoteRequest failed, descriptor mismatch");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (code == static_cast<int32_t>(IAnrObserver::NOTIFY_ANR)) {
        return OnAnrStub(data);
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t AnrObserverStub::OnAnrStub(MessageParcel &data)
{
    CALL_DEBUG_ENTER;
    int32_t applicationPid;
    if (!data.ReadInt32(applicationPid)) {
        WLOGFE("Read eventId from parcel failed!");
        return ERR_INVALID_DATA;
    }
    return OnAnr(applicationPid);
}
} // namespace Rosen
} // namespace OHOS