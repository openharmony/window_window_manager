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

#include "zidl/mock_session_manager_service_stub.h"

#include <ipc_skeleton.h>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MockSessionManagerServiceStub"};
}

int32_t MockSessionManagerServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    auto msgId = static_cast<MockSessionManagerServiceMessage>(code);
    WLOGFI("Receive MockSessionManagerServiceMessage = %{public}u", msgId);
    switch (msgId) {
        case MockSessionManagerServiceMessage::TRANS_ID_GET_SESSION_MANAGER_SERVICE: {
            sptr<IRemoteObject> remoteObject = GetSessionManagerService();
            reply.WriteRemoteObject(remoteObject);
            break;
        }
        case MockSessionManagerServiceMessage::TRANS_ID_NOTIFY_SCENE_BOARD_AVAILABLE: {
            NotifySceneBoardAvailable();
            break;
        }
        case MockSessionManagerServiceMessage::TRANS_ID_REGISTER_SESSION_MANAGER_RECOVER_LISTENER: {
            auto pid = data.ReadInt64();
            sptr<IRemoteObject> listenerObject = data.ReadRemoteObject();
            RegisterSessionManagerServiceRecoverListener(pid, listenerObject);
            break;
        }
        case MockSessionManagerServiceMessage::TRANS_ID_UNREGISTER_SESSION_MANAGER_RECOVER_LISTENER: {
            auto pid = data.ReadInt64();
            UnRegisterSessionManagerServiceRecoverListener(pid);
            break;
        }
        default:
            WLOGFW("unknown transaction code %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS