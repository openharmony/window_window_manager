/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "window_stub.h"
#include <vector>
#include "ipc_skeleton.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowStub"};
    const uint32_t MAX_AVOID_NUM = 4;
}

int WindowStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    switch (code) {
        case TRANS_ID_UPDATE_WINDOW_RECT: {
            struct Rect rect { data.ReadInt32(), data.ReadInt32(), data.ReadUint32(), data.ReadUint32() };
            WindowSizeChangeReason reason = static_cast<WindowSizeChangeReason>(data.ReadUint32());
            UpdateWindowRect(rect, reason);
            break;
        }
        case TRANS_ID_UPDATE_WINDOW_MODE: {
            WindowMode mode = static_cast<WindowMode>(data.ReadUint32());
            UpdateWindowMode(mode);
            break;
        }
        case TRANS_ID_UPDATE_FOCUS_STATUS: {
            bool focused = data.ReadBool();
            UpdateFocusStatus(focused);
            break;
        }
        case TRANS_ID_UPDATE_AVOID_AREA: {
            std::vector<Rect> avoidArea;
            uint32_t len = data.ReadUint32();
            if (len != MAX_AVOID_NUM) {
                WLOGFE("Read len fail. AvoidArea size != 4");
                return -1;
            }
            avoidArea.resize(len);
            if (avoidArea.size() < len) {
                WLOGE("Fail to resize avoidArea.");
                return -1;
            }

            bool readVectorRes = true;
            for (uint32_t i = 0; i < len; ++i) {
                readVectorRes = data.ReadInt32(avoidArea[i].posX_) && data.ReadInt32(avoidArea[i].posY_) &&
                    data.ReadUint32(avoidArea[i].width_) && data.ReadUint32(avoidArea[i].height_);
                if (!readVectorRes) {
                    WLOGE("Fail to ReadInt32. index:%{public}u, nums:%{public}u", i, len);
                    return -1;
                }
            }
            UpdateAvoidArea(avoidArea);
            break;
        }
        case TRANS_ID_UPDATE_WINDOW_STATE: {
            UpdateWindowState(static_cast<WindowState>(data.ReadUint32()));
            break;
        }
        case TRANS_ID_UPDATE_DRAG_EVENT: {
            PointInfo point;
            point.x = data.ReadInt32();
            point.y = data.ReadInt32();
            DragEvent event = static_cast<DragEvent> (data.ReadUint32());
            UpdateWindowDragInfo(point, event);
            break;
        }
        case TRANS_ID_UPDATE_DISPLAY_ID: {
            UpdateDisplayId(data.ReadUint64(), data.ReadUint64());
            break;
        }
        case TRANS_ID_UPDATE_OCCUPIED_AREA: {
            sptr<OccupiedAreaChangeInfo> info = data.ReadParcelable<OccupiedAreaChangeInfo>();
            UpdateOccupiedAreaChangeInfo(info);
            break;
        }
        case TRANS_ID_UPDATE_ACTIVE_STATUS: {
            bool isActive = data.ReadBool();
            UpdateActiveStatus(isActive);
            break;
        }
        default:
            break;
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS
