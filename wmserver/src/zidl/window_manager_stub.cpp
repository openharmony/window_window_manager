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

#include "zidl/window_manager_stub.h"
#include <ipc_skeleton.h>
#include "window_manager_hilog.h"


namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerStub"};
}

int32_t WindowManagerStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    switch (code) {
        case TRANS_ID_CREATE_WINDOW: {
            sptr<IRemoteObject> windowObject = data.ReadRemoteObject();
            sptr<IWindow> windowProxy = iface_cast<IWindow>(windowObject);
            sptr<WindowProperty> windowProperty = data.ReadStrongParcelable<WindowProperty>();
            std::shared_ptr<RSSurfaceNode> surfaceNode(data.ReadParcelable<RSSurfaceNode>());
            uint32_t windowId;
            WMError errCode = CreateWindow(windowProxy, windowProperty, surfaceNode, windowId);
            reply.WriteUint32(windowId);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case TRANS_ID_ADD_WINDOW: {
            sptr<WindowProperty> windowProperty = data.ReadStrongParcelable<WindowProperty>();
            WMError errCode = AddWindow(windowProperty);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case TRANS_ID_REMOVE_WINDOW: {
            uint32_t windowId = data.ReadUint32();
            WMError errCode = RemoveWindow(windowId);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case TRANS_ID_DESTROY_WINDOW: {
            uint32_t windowId = data.ReadUint32();
            WMError errCode = DestroyWindow(windowId);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case TRANS_ID_REQUEST_FOCUS: {
            uint32_t windowId = data.ReadUint32();
            WMError errCode = RequestFocus(windowId);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case TRANS_ID_SET_BACKGROUND_BLUR: {
            uint32_t windowId = data.ReadUint32();
            WindowBlurLevel level = static_cast<WindowBlurLevel>(data.ReadUint32());
            WMError errCode = SetWindowBackgroundBlur(windowId, level);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case TRANS_ID_SET_APLPHA: {
            uint32_t windowId = data.ReadUint32();
            float alpha = data.ReadFloat();
            WMError errCode = SetAlpha(windowId, alpha);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case TRANS_ID_SEND_ABILITY_TOKEN: {
            sptr<IRemoteObject> abilityToken = data.ReadRemoteObject();
            uint32_t windowId = data.ReadUint32();
            WMError errCode = SaveAbilityToken(abilityToken, windowId);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case TRANS_ID_GET_AVOID_AREA: {
            uint32_t windowId = data.ReadUint32();
            AvoidAreaType avoidAreaType = static_cast<AvoidAreaType>(data.ReadUint32());
            std::vector<Rect> avoidArea = GetAvoidAreaByType(windowId, avoidAreaType);

            // prepare reply data
            uint32_t avoidAreaNum = static_cast<uint32_t>(avoidArea.size());
            reply.WriteUint32(avoidAreaNum);
            for (uint32_t i = 0; i < avoidAreaNum; ++i) {
                reply.WriteInt32(avoidArea[i].posX_);
                reply.WriteInt32(avoidArea[i].posY_);
                reply.WriteUint32(avoidArea[i].width_);
                reply.WriteUint32(avoidArea[i].height_);
            }
            break;
        }
        case TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT: {
            WindowManagerAgentType type = static_cast<WindowManagerAgentType>(data.ReadUint32());
            sptr<IRemoteObject> windowManagerAgentObject = data.ReadRemoteObject();
            sptr<IWindowManagerAgent> windowManagerAgentProxy =
                iface_cast<IWindowManagerAgent>(windowManagerAgentObject);
            RegisterWindowManagerAgent(type, windowManagerAgentProxy);
            break;
        }
        case TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT: {
            WindowManagerAgentType type = static_cast<WindowManagerAgentType>(data.ReadUint32());
            sptr<IRemoteObject> windowManagerAgentObject = data.ReadRemoteObject();
            sptr<IWindowManagerAgent> windowManagerAgentProxy =
                iface_cast<IWindowManagerAgent>(windowManagerAgentObject);
            UnregisterWindowManagerAgent(type, windowManagerAgentProxy);
            break;
        }
        case TRANS_ID_PROCESS_POINT_DOWN: {
            uint32_t windowId = data.ReadUint32();
            bool isStartDrag = data.ReadBool();
            ProcessPointDown(windowId, isStartDrag);
            break;
        }
        case TRANS_ID_PROCESS_POINT_UP: {
            uint32_t windowId = data.ReadUint32();
            ProcessPointUp(windowId);
            break;
        }
        case TRANS_ID_GET_TOP_WINDOW_ID: {
            uint32_t mainWinId = data.ReadUint32();
            uint32_t topWinId;
            WMError errCode = GetTopWindowId(mainWinId, topWinId);
            reply.WriteUint32(topWinId);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case TRANS_ID_MINIMIZE_ALL_APP_WINDOWS: {
            MinimizeAllAppWindows(data.ReadUint64());
            break;
        }
        case TRANS_ID_MAXMIZE_WINDOW: {
            MaxmizeWindow(data.ReadUint32());
            break;
        }
        case TRANS_ID_UPDATE_LAYOUT_MODE: {
            DisplayId displayId = data.ReadUint64();
            WindowLayoutMode mode = static_cast<WindowLayoutMode>(data.ReadUint32());
            WMError errCode = SetWindowLayoutMode(displayId, mode);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case TRANS_ID_UPDATE_PROPERTY: {
            sptr<WindowProperty> windowProperty = data.ReadStrongParcelable<WindowProperty>();
            PropertyChangeAction action = static_cast<PropertyChangeAction>(data.ReadUint32());
            WMError errCode = UpdateProperty(windowProperty, action);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        default:
            WLOGFW("unknown transaction code");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS
