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

#include "session/host/include/zidl/session_stub.h"

#include <ipc_types.h>
#include <ui/rs_surface_node.h>
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionStub"};
}

const std::map<uint32_t, SessionStubFunc> SessionStub::stubFuncMap_{
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_FOREGROUND), &SessionStub::HandleForeground),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_BACKGROUND), &SessionStub::HandleBackground),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_DISCONNECT), &SessionStub::HandleDisconnect),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_CONNECT), &SessionStub::HandleConnect),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_ACTIVE_PENDING_SESSION),
        &SessionStub::HandlePendingSessionActivation),
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_UPDATE_ACTIVE_STATUS),
        &SessionStub::HandleUpdateActivateStatus),

    // for scene only
    std::make_pair(static_cast<uint32_t>(SessionMessage::TRANS_ID_SESSION_EVENT), &SessionStub::HandleSessionEvent),
};

int SessionStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WLOGFD("Scene session on remote request!, code: %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_INVALID_STATE;
    }

    const auto func = stubFuncMap_.find(code);
    if (func == stubFuncMap_.end()) {
        WLOGFE("Failed to find function handler!");
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return (this->*(func->second))(data, reply);
}

int SessionStub::HandleForeground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Foreground!");
    WSError errCode = Foreground();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleBackground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Background!");
    WSError errCode = Background();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleDisconnect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Disconnect!");
    WSError errCode = Disconnect();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleConnect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Connect!");
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
    WSError errCode = Connect(sessionStage, eventChannel, surfaceNode, persistentId, property);
    reply.WriteUint64(persistentId);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleSessionEvent(MessageParcel& data, MessageParcel& reply)
{
    uint32_t eventId = data.ReadUint32();
    WLOGFD("HandleSessionEvent eventId: %{public}d", eventId);
    WSError errCode = OnSessionEvent(static_cast<SessionEvent>(eventId));
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandlePendingSessionActivation(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("PendingSessionActivation!");
    SessionInfo info;
    info.bundleName_ = data.ReadString();
    info.abilityName_ = data.ReadString();
    if (data.ReadBool()) {
        info.callerToken_ = data.ReadRemoteObject();
    }
    WSError errCode = PendingSessionActivation(info);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleUpdateActivateStatus(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleUpdateActivateStatus!");
    bool isActive = data.ReadBool();
    WSError errCode = UpdateActiveStatus(isActive);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}
} // namespace OHOS::Rosen
