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
        case TRANS_ID_ON_SCREEN_CONNECT: {
            sptr<ScreenInfo> screenInfo = data.ReadParcelable<ScreenInfo>();
            OnScreenConnect(screenInfo);
            break;
        }
        case TRANS_ID_ON_SCREEN_DISCONNECT: {
            ScreenId screenId;
            if (!data.ReadUint64(screenId)) {
                WLOGFE("Read ScreenId failed");
                return -1;
            }
            OnScreenDisconnect(screenId);
            break;
        }
        case TRANS_ID_ON_SCREEN_CHANGED: {
            std::vector<const sptr<ScreenInfo>> screenInfos;
            uint32_t size;
            if (!data.ReadUint32(size)) {
                WLOGFE("Read ScreenChangeEvent failed");
                return -1;
            }

            for (uint32_t i = 0; i < size; i++) {
                screenInfos.push_back(data.ReadParcelable<ScreenInfo>());
            }

            uint32_t event;
            if (!data.ReadUint32(event)) {
                WLOGFE("Read ScreenChangeEvent failed");
                return -1;
            }
            OnScreenChange(screenInfos, static_cast<ScreenChangeEvent>(event));
            break;
        }
        case TRANS_ID_ON_DISPLAY_CONNECT: {
            sptr<DisplayInfo> displayInfo = data.ReadParcelable<DisplayInfo>();
            OnDisplayCreate(displayInfo);
            break;
        }
        case TRANS_ID_ON_DISPLAY_DISCONNECT: {
            DisplayId displayId;
            if (!data.ReadUint64(displayId)) {
                return -1;
                WLOGFE("Read DisplayId failed");
            }
            OnDisplayDestroy(displayId);
            break;
        }
        case TRANS_ID_ON_DISPLAY_CHANGED: {
            sptr<DisplayInfo> displayInfo = data.ReadParcelable<DisplayInfo>();
            uint32_t event;
            if (!data.ReadUint32(event)) {
                WLOGFE("Read DisplayChangeEvent failed");
                return -1;
            }
            OnDisplayChange(displayInfo, static_cast<DisplayChangeEvent>(event));
            break;
        }
        default:
            break;
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS
