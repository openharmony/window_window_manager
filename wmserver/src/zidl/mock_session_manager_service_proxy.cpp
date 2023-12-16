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

#include <cinttypes>
#include "zidl/mock_session_manager_service_proxy.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MockSessionManagerServiceProxy"};
}

sptr<IRemoteObject> MockSessionManagerServiceProxy::GetSessionManagerService()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        MockSessionManagerServiceMessage::TRANS_ID_GET_SESSION_MANAGER_SERVICE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = reply.ReadRemoteObject();
    return remoteObject;
}

void MockSessionManagerServiceProxy::NotifySceneBoardAvailable()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[RECOVER] WriteInterfaceToken failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        MockSessionManagerServiceMessage::TRANS_ID_NOTIFY_SCENE_BOARD_AVAILABLE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("[RECOVER] SendRequest failed");
        return;
    }
}

void MockSessionManagerServiceProxy::RegisterSessionManagerServiceRecoverListener(
    int64_t pid, const sptr<IRemoteObject>& listener)
{
    WLOGFD("[RECOVER] RegisterSessionManagerServiceRecoverListener pid = %{public}" PRId64, pid);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[RECOVER] WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteInt64(pid)) {
        WLOGFE("[RECOVER] Write pid failed");
        return;
    }

    if (!data.WriteRemoteObject(listener)) {
        WLOGFE("[RECOVER] WriteRemoteObject listener failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        MockSessionManagerServiceMessage::TRANS_ID_REGISTER_SESSION_MANAGER_RECOVER_LISTENER),
        data, reply, option) != ERR_NONE) {
        WLOGFE("[RECOVER] SendRequest failed");
        return;
    }
}

void MockSessionManagerServiceProxy::UnRegisterSessionManagerServiceRecoverListener(int64_t pid)
{
    WLOGFD("[RECOVER] UnRegisterSessionManagerServiceRecoverListener pid = %{public}" PRId64, pid);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[RECOVER] WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteInt64(pid)) {
        WLOGFE("[RECOVER] Write pid failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        MockSessionManagerServiceMessage::TRANS_ID_UNREGISTER_SESSION_MANAGER_RECOVER_LISTENER),
        data, reply, option) != ERR_NONE) {
        WLOGFE("[RECOVER] SendRequest failed");
        return;
    }
}

} // namespace Rosen
} // namespace OHOS