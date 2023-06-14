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

#include "intention_event_proxy.h"

#include "hitrace_meter.h"
#include "iremote_object.h"
#include <message_option.h>
#include <message_parcel.h>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL { LOG_CORE, HILOG_DOMAIN_WINDOW, "IntentionEventProxy" };
} // namespace

WSError IntentionEventProxy::SetAnrObserver(sptr<IAnrObserver> observer)
{
    CALL_DEBUG_ENTER;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(IntentionEventProxy::GetDescriptor())) {
        WLOGFE("Failed to write descriptor");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (observer == nullptr) {
        WLOGFE("observer is nullptr");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    auto object = observer->AsObject();
    if (!data.WriteObject(object)) {
        WLOGFE("WriteObject failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is nullptr");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = remote->SendRequest(IIntentionEvent::SET_ANR_OBSERVER, data, reply, option);
    if (ret != static_cast<int32_t>(WSError::WS_OK)) {
        WLOGFE("Send request fail, ret:%{public}d", ret);
    }
    int32_t ret = reply.ReadUint32();
    return static_cast<WSError>(ret);
}
} // namespace Rosen
} // namespace OHOS
