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

#include "anr_observer_proxy.h"

#include <message_parcel.h>

#include "iremote_object.h"
#include "message_option.h"

#include "window_manager_hilog.h"

#include "ws_common.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "AnrObserverProxy" };
}  // namespace

int32_t AnrObserverProxy::OnAnr(int32_t pid)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("Get remote object failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(AnrObserverProxy::GetDescriptor())) {
        WLOGFE("Write descriptor failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
     if (!data.WriteInt32(pid)) {
        WLOGFE("Write int32_t failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = remote->SendRequest(static_cast<int32_t>(IAnrObserver::NOTIFY_ANR), data, reply, option);
    if (ret != static_cast<int32_t>(WSError::WS_OK)) {
        WLOGFE("SendRequest is failed, error code: %{public}d", ret);
    }
    return ret;
}
} // Rosen
} // OHOS