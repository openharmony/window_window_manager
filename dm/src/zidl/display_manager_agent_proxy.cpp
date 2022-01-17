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

#include "zidl/display_manager_agent_proxy.h"
#include <ipc_types.h>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "DisplayManagerAgentProxy"};
}

void DisplayManagerAgentProxy::NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(event))) {
        WLOGFE("Write event failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(status))) {
        WLOGFE("Write status failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_NOTIFY_DISPLAY_POWER_EVENT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void DisplayManagerAgentProxy::NotifyDisplayStateChanged(DisplayState state)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("Write DisplayState failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_NOTIFY_DISPLAY_STATE_CHANGED, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}
} // namespace Rosen
} // namespace OHOS

