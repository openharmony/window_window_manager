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

#include "session_manager/include/zidl/scene_session_manager_stub.h"

#include <ipc_types.h>
#include <ui/rs_surface_node.h>
#include "session/host/include/scene_session.h"
#include "window_manager_agent_proxy.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManagerStub"};
}

const std::map<uint32_t, SceneSessionManagerStubFunc> SceneSessionManagerStub::stubFuncMap_{
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_CREATE_AND_CONNECT_SPECIFIC_SESSION),
        &SceneSessionManagerStub::HandleCreateAndConnectSpecificSession),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_DESTROY_AND_DISCONNECT_SPECIFIC_SESSION),
        &SceneSessionManagerStub::HandleDestroyAndDisconnectSpcificSession),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UPDATE_PROPERTY),
        &SceneSessionManagerStub::HandleUpdateProperty),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT),
        &SceneSessionManagerStub::HandleRegisterWindowManagerAgent),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT),
        &SceneSessionManagerStub::HandleUnregisterWindowManagerAgent),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_FOCUS_SESSION_INFO),
        &SceneSessionManagerStub::HandleGetFocusSessionInfo),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_SET_SESSION_LABEL),
        &SceneSessionManagerStub::HandleSetSessionLabel),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_SET_SESSION_ICON),
        &SceneSessionManagerStub::HandleSetSessionIcon),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_REGISTER_SESSION_LISTENER),
        &SceneSessionManagerStub::HandleRegisterSessionListener),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UNREGISTER_SESSION_LISTENER),
        &SceneSessionManagerStub::HandleUnregisterSessionListener),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_PENDING_SESSION_TO_FOREGROUND),
        &SceneSessionManagerStub::HandlePendingSessionToForeground),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR),
        &SceneSessionManagerStub::HandlePendingSessionToBackgroundForDelegator),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_FOCUS_SESSION_TOKEN),
        &SceneSessionManagerStub::HandleGetFocusSessionToken),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_SET_GESTURE_NAVIGATION_ENABLED),
        &SceneSessionManagerStub::HandleSetGestureNavigationEnabled)
};

int SceneSessionManagerStub::OnRemoteRequest(uint32_t code,
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WLOGFD("Scene session on remote request!, code: %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    const auto& func = stubFuncMap_.find(code);
    if (func == stubFuncMap_.end()) {
        WLOGFE("Failed to find function handler!");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return (this->*(func->second))(data, reply);
}

int SceneSessionManagerStub::HandleCreateAndConnectSpecificSession(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleCreateAndConnectSpecificSession!");
    sptr<IRemoteObject> sessionStageObject = data.ReadRemoteObject();
    sptr<ISessionStage> sessionStage = iface_cast<ISessionStage>(sessionStageObject);
    sptr<IRemoteObject> eventChannelObject = data.ReadRemoteObject();
    sptr<IWindowEventChannel> eventChannel = iface_cast<IWindowEventChannel>(eventChannelObject);
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(data);
    if (sessionStage == nullptr || eventChannel == nullptr || surfaceNode == nullptr) {
        WLOGFE("Failed to read scene session stage object or event channel object!");
        return ERR_INVALID_DATA;
    }

    sptr<WindowSessionProperty> property = nullptr;
    if (data.ReadBool()) {
        property = data.ReadStrongParcelable<WindowSessionProperty>();
    } else {
        WLOGFW("Property not exist!");
    }
    uint64_t persistentId = INVALID_SESSION_ID;
    sptr<ISession> sceneSession;
    CreateAndConnectSpecificSession(sessionStage, eventChannel, surfaceNode,
        property, persistentId, sceneSession);
    if (sceneSession== nullptr) {
        return ERR_INVALID_STATE;
    }
    reply.WriteUint64(persistentId);
    reply.WriteRemoteObject(sceneSession->AsObject());
    reply.WriteUint32(static_cast<uint32_t>(WSError::WS_OK));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleDestroyAndDisconnectSpcificSession(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleDestroyAndDisconnectSpcificSession!");
    uint64_t persistentId = data.ReadUint64();
    const WSError& ret = DestroyAndDisconnectSpecificSession(persistentId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleUpdateProperty(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleUpdateProperty!");
    auto action = static_cast<WSPropertyChangeAction>(data.ReadUint32());
    sptr<WindowSessionProperty> property = nullptr;
    if (data.ReadBool()) {
        property = data.ReadStrongParcelable<WindowSessionProperty>();
    } else {
        WLOGFW("Property not exist!");
    }
    const WSError& ret = UpdateProperty(property, action);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleRegisterWindowManagerAgent(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleRegisterWindowManagerAgent!");
    auto type = static_cast<WindowManagerAgentType>(data.ReadUint32());
    sptr<IRemoteObject> windowManagerAgentObject = data.ReadRemoteObject();
    sptr<IWindowManagerAgent> windowManagerAgentProxy =
        iface_cast<IWindowManagerAgent>(windowManagerAgentObject);
    WMError errCode = RegisterWindowManagerAgent(type, windowManagerAgentProxy);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleUnregisterWindowManagerAgent(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleUnregisterWindowManagerAgent!");
    auto type = static_cast<WindowManagerAgentType>(data.ReadUint32());
    sptr<IRemoteObject> windowManagerAgentObject = data.ReadRemoteObject();
    sptr<IWindowManagerAgent> windowManagerAgentProxy =
        iface_cast<IWindowManagerAgent>(windowManagerAgentObject);
    WMError errCode = UnregisterWindowManagerAgent(type, windowManagerAgentProxy);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleGetFocusSessionInfo(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleGetFocusSessionInfo!");
    FocusChangeInfo focusInfo;
    GetFocusWindowInfo(focusInfo);
    reply.WriteParcelable(&focusInfo);
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleSetSessionLabel(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleSetSessionLabel!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    std::string label = data.ReadString();
    WSError errCode = SetSessionLabel(token, label);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleSetSessionIcon(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleSetSessionIcon!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    std::shared_ptr<Media::PixelMap> icon(data.ReadParcelable<Media::PixelMap>());
    WSError errCode = SetSessionIcon(token, icon);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleRegisterSessionListener(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleRegisterSessionListener!");
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    WSError errCode = RegisterSessionListener(listener);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleUnregisterSessionListener(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleUnregisterSessionListener!");
    UnregisterSessionListener();
    return ERR_NONE;
}

int SceneSessionManagerStub::HandlePendingSessionToForeground(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandlePendingSessionToForeground!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    const WSError& errCode = PendingSessionToForeground(token);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandlePendingSessionToBackgroundForDelegator(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandlePendingSessionToBackground!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    const WSError& errCode = PendingSessionToBackgroundForDelegator(token);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleGetFocusSessionToken(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleGetFocusSessionToken!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    const WSError& errCode = GetFocusSessionToken(token);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleSetGestureNavigationEnabled(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleGetFocusSessionToken!");
    bool enable = data.ReadBool();
    const WMError &ret = SetGestureNavigaionEnabled(enable);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}
} // namespace OHOS::Rosen
