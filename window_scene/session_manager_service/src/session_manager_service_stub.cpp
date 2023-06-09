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

#include "session_manager_service_stub.h"

#include <ipc_skeleton.h>

#include "marshalling_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionManagerServiceStub"};
}

int32_t SessionManagerServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    WLOGFD("OnRemoteRequest code is %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }

    SessionManagerServiceMessage msgId = static_cast<SessionManagerServiceMessage>(code);
    switch (msgId) {
        case SessionManagerServiceMessage::TRANS_ID_SCREEN_BASE: {
            break;
        }
        case SessionManagerServiceMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID: {
            int id = data.ReadInt32();
            int value = GetValueById(id);
            reply.WriteInt32(value);
            break;
        }
        case SessionManagerServiceMessage::TRANS_ID_GET_SCENE_SESSION_MANAGER: {
            reply.WriteRemoteObject(GetSceneSessionManager());
            break;
        }
        case SessionManagerServiceMessage::TRANS_ID_GET_SCREEN_SESSION_MANAGER_SERVICE: {
            sptr<IRemoteObject> remoteObj = GetScreenSessionManagerService();
            reply.WriteRemoteObject(remoteObj);
            break;
        }
        case SessionManagerServiceMessage::TRANS_ID_GET_SCREEN_LOCK_MANAGER_SERVICE: {
            reply.WriteRemoteObject(GetScreenLockManagerService());
            break;
        }
        default: {
            WLOGFW("unknown transaction code.");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return 0;
}

} // namespace OHOS::Rosen