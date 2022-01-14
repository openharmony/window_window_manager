/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "zidl/display_manager_agent_stub.h"
#include "ipc_skeleton.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManagerAgentStub"};
}

int32_t DisplayManagerAgentStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    WLOGFI("code:%{public}d", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    switch (code) {
        case TRANS_ID_NOTIFY_DISPLAY_POWER_EVENT: {
            DisplayPowerEvent event = static_cast<DisplayPowerEvent>(data.ReadUint32());
            EventStatus status = static_cast<EventStatus>(data.ReadUint32());
            NotifyDisplayPowerEvent(event, status);
            break;
        }
        case TRANS_ID_NOTIFY_DISPLAY_STATE_CHANGED: {
            DisplayState state = static_cast<DisplayState>(data.ReadUint32());
            NotifyDisplayStateChanged(state);
            break;
        }
        default:
            break;
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS
