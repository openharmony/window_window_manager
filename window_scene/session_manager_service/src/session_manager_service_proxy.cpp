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
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionManagerServiceProxy" };
}

sptr<IRemoteObject> SessionManagerServiceProxy::GetSceneSessionManager()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }

    auto ret = Remote()->SendRequest(
        static_cast<uint32_t>(SessionManagerServiceMessage::TRANS_ID_GET_SCENE_SESSION_MANAGER),
        data, reply, option);
    if (ret != ERR_NONE) {
        WLOGFE("SendRequest failed, errorCode %{public}d", ret);
        return nullptr;
    }

    return reply.ReadRemoteObject();
}

sptr<IRemoteObject> SessionManagerServiceProxy::GetSceneSessionManagerLite()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return nullptr;
    }

    auto ret = Remote()->SendRequest(
        static_cast<uint32_t>(SessionManagerServiceMessage::TRANS_ID_GET_SCENE_SESSION_MANAGER_LITE),
        data, reply, option);
    if (ret != ERR_NONE) {
        WLOGFE("SendRequest failed, errorCode %{public}d", ret);
        return nullptr;
    }

    return reply.ReadRemoteObject();
}
} // namespace OHOS::Rosen
