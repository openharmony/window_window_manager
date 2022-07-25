/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "zidl/window_stub.h"
#include <vector>
#include "ipc_skeleton.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowStub"};
}

int WindowStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    WindowMessage msgId = static_cast<WindowMessage>(code);
    switch (msgId) {
        case WindowMessage::TRANS_ID_UPDATE_WINDOW_RECT: {
            struct Rect rect { data.ReadInt32(), data.ReadInt32(), data.ReadUint32(), data.ReadUint32() };
            bool decoStatus = data.ReadBool();
            WindowSizeChangeReason reason = static_cast<WindowSizeChangeReason>(data.ReadUint32());
            UpdateWindowRect(rect, decoStatus, reason);
            break;
        }
        case WindowMessage::TRANS_ID_UPDATE_WINDOW_MODE: {
            WindowMode mode = static_cast<WindowMode>(data.ReadUint32());
            UpdateWindowMode(mode);
            break;
        }
        case WindowMessage::TRANS_ID_UPDATE_MODE_SUPPORT_INFO: {
            uint32_t modeSupportInfo = data.ReadUint32();
            UpdateWindowModeSupportInfo(modeSupportInfo);
            break;
        }
        case WindowMessage::TRANS_ID_UPDATE_FOCUS_STATUS: {
            bool focused = data.ReadBool();
            UpdateFocusStatus(focused);
            break;
        }
        case WindowMessage::TRANS_ID_UPDATE_AVOID_AREA: {
            sptr<AvoidArea> avoidArea = data.ReadStrongParcelable<AvoidArea>();
            uint32_t type;
            if (!data.ReadUint32(type)) {
                return -1;
            }
            UpdateAvoidArea(avoidArea, static_cast<AvoidAreaType>(type));
            break;
        }
        case WindowMessage::TRANS_ID_UPDATE_WINDOW_STATE: {
            UpdateWindowState(static_cast<WindowState>(data.ReadUint32()));
            break;
        }
        case WindowMessage::TRANS_ID_UPDATE_DRAG_EVENT: {
            PointInfo point;
            point.x = data.ReadInt32();
            point.y = data.ReadInt32();
            DragEvent event = static_cast<DragEvent> (data.ReadUint32());
            UpdateWindowDragInfo(point, event);
            break;
        }
        case WindowMessage::TRANS_ID_UPDATE_DISPLAY_ID: {
            UpdateDisplayId(data.ReadUint64(), data.ReadUint64());
            break;
        }
        case WindowMessage::TRANS_ID_UPDATE_OCCUPIED_AREA: {
            sptr<OccupiedAreaChangeInfo> info = data.ReadParcelable<OccupiedAreaChangeInfo>();
            UpdateOccupiedAreaChangeInfo(info);
            break;
        }
        case WindowMessage::TRANS_ID_UPDATE_ACTIVE_STATUS: {
            bool isActive = data.ReadBool();
            UpdateActiveStatus(isActive);
            break;
        }
        case WindowMessage::TRANS_ID_GET_WINDOW_PROPERTY: {
            auto property = GetWindowProperty();
            reply.WriteParcelable(property.GetRefPtr());
            break;
        }
        case WindowMessage::TRANS_ID_NOTIFY_OUTSIDE_PRESSED: {
            NotifyTouchOutside();
            break;
        }
        case WindowMessage::TRANS_ID_NOTIFY_SCREEN_SHOT: {
            NotifyScreenshot();
            break;
        }
        case WindowMessage::TRANS_ID_NOTIFY_DESTROY: {
            NotifyDestroy();
            break;
        }
        case WindowMessage::TRANS_ID_DUMP_INFO: {
            std::vector<std::string> params;
            if (!data.ReadStringVector(&params)) {
                WLOGFE("Fail to read params");
            }
            std::vector<std::string> info;
            DumpInfo(params, info);
            if (!reply.WriteStringVector(info)) {
                WLOGFE("Fail to write info");
            }
            break;
        }
        default:
            break;
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS
