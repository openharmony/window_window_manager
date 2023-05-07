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

#include "session_manager_service_proxy.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SessionManagerServiceProxy"};
}

SessionManagerServiceProxy::SessionManagerServiceProxy(sptr<IRemoteObject>& remoteObject) : IRemoteProxy(remoteObject)
{
}

SessionManagerServiceProxy::~SessionManagerServiceProxy()
{
}

int SessionManagerServiceProxy::GetValueById(int id)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("GetValueById remote is nullptr");
        return -1;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetValueById: WriteInterfacetoken failed");
        return -1;
    }

    if (!data.WriteInt32(id)) {
        WLOGFE("GetValueById: Write id failed");
        return -1;
    }
    int ret = remote->SendRequest(static_cast<uint32_t>(SessionManagerServiceMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID),
        data, reply, option);
    if (ret != ERR_NONE) {
        WLOGFW("GetValueById: SendRequest failed, errorCode %{public}d", ret);
        return -1;
    }

    int value = reply.ReadInt32();
    WLOGFI("GetValueById: value: %{public}d", value);

    return value;
}
} // namespace OHOS::Rosen