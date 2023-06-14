/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "intention_event_stub.h"

#include "message_parcel.h"

#include "intention_event_service.h"
#include "window_manager_hilog.h"
#include "ws_common.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, HILOG_DOMAIN_WINDOW, "IntentionEventStub" };
using ConnFunc = int32_t (IntentionEventStub::*)(MessageParcel& data, MessageParcel& reply);
} // namespace

int32_t IntentionEventStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    WLOGFD("cmd = %{public}d, flags = %{public}d", code, option.GetFlags());
    std::u16string descriptor = IntentionEventStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        WLOGFE("IntentionEventStub::OnRemoteRequest failed, descriptor is not matched");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    const std::map<int32_t, ConnFunc> mapConnFunc {
        {IIntentionEvent::SET_ANR_OBSERVER, &IntentionEventStub::SetAnrObserver}
    };
    auto it = mapConnFunc.find(code);
    if (it != mapConnFunc.end()) {
        return (this->*it->second)(data, reply);
    }
    WLOGFE("Unknown code:%{public}u", code);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t IntentionEventStub::SetAnrObserverStub(MessageParcel& data, MessageParcel& reply)
{
    sptr<IRemoteObject> obj = data.ReadRemoteObject();
    if (obj == nullptr) {
        WLOGFE("Read remote obj failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IAnrObserver> observer = iface_cast<IAnrObserver>(obj);
    if (observer == nullptr) {
        WLOGFE("Read remote obj failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    WLOGFD("Read callback successfully");
    SetAnrObserver(observer);
    return RET_OK;
}

} // namespace Rosen
} // namespace OHOS