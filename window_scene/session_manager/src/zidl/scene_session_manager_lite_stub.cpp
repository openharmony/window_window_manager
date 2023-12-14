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

#include "session_manager/include/zidl/scene_session_manager_lite_stub.h"

#include <ipc_types.h>
#include "marshalling_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManagerLiteStub"};
constexpr uint32_t MAX_VECTOR_SIZE = 100;
}

const std::map<uint32_t, SceneSessionManagerLiteStubFunc> SceneSessionManagerLiteStub::stubFuncMap_{
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_SET_SESSION_LABEL),
        &SceneSessionManagerLiteStub::HandleSetSessionLabel),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_SET_SESSION_ICON),
        &SceneSessionManagerLiteStub::HandleSetSessionIcon),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_IS_VALID_SESSION_IDS),
        &SceneSessionManagerLiteStub::HandleIsValidSessionIds),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_PENDING_SESSION_TO_FOREGROUND),
        &SceneSessionManagerLiteStub::HandlePendingSessionToForeground),
    std::make_pair(static_cast<uint32_t>(
        SceneSessionManagerLiteMessage::TRANS_ID_PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR),
        &SceneSessionManagerLiteStub::HandlePendingSessionToBackgroundForDelegator),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_FOCUS_SESSION_TOKEN),
        &SceneSessionManagerLiteStub::HandleGetFocusSessionToken),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_REGISTER_SESSION_LISTENER),
        &SceneSessionManagerLiteStub::HandleRegisterSessionListener),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_UNREGISTER_SESSION_LISTENER),
        &SceneSessionManagerLiteStub::HandleUnRegisterSessionListener),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_MISSION_INFOS),
        &SceneSessionManagerLiteStub::HandleGetSessionInfos),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_MISSION_INFO_BY_ID),
        &SceneSessionManagerLiteStub::HandleGetSessionInfo),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_TERMINATE_SESSION_NEW),
        &SceneSessionManagerLiteStub::HandleTerminateSessionNew),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_SESSION_SNAPSHOT),
        &SceneSessionManagerLiteStub::HandleGetSessionSnapshot),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_SET_SESSION_CONTINUE_STATE),
        &SceneSessionManagerLiteStub::HandleSetSessionContinueState),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_CLEAR_SESSION),
        &SceneSessionManagerLiteStub::HandleClearSession),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_CLEAR_ALL_SESSIONS),
        &SceneSessionManagerLiteStub::HandleClearAllSessions),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_LOCK_SESSION),
        &SceneSessionManagerLiteStub::HandleLockSession),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_UNLOCK_SESSION),
        &SceneSessionManagerLiteStub::HandleUnlockSession),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_MOVE_MISSIONS_TO_FOREGROUND),
        &SceneSessionManagerLiteStub::HandleMoveSessionsToForeground),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_MOVE_MISSIONS_TO_BACKGROUND),
        &SceneSessionManagerLiteStub::HandleMoveSessionsToBackground),
};

int SceneSessionManagerLiteStub::OnRemoteRequest(uint32_t code,
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

int SceneSessionManagerLiteStub::HandleSetSessionLabel(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleSetSessionLabel!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    std::string label = data.ReadString();
    WSError errCode = SetSessionLabel(token, label);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleSetSessionIcon(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleSetSessionIcon!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    std::shared_ptr<Media::PixelMap> icon(data.ReadParcelable<Media::PixelMap>());
    WSError errCode = SetSessionIcon(token, icon);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleIsValidSessionIds(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleIsValidSessionIds!");
    std::vector<int32_t> sessionIds;
    data.ReadInt32Vector(&sessionIds);
    std::vector<bool> results;
    reply.WriteBoolVector(results);
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandlePendingSessionToForeground(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandlePendingSessionToForeground!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    const WSError& errCode = PendingSessionToForeground(token);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandlePendingSessionToBackgroundForDelegator(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandlePendingSessionToBackground!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    const WSError& errCode = PendingSessionToBackgroundForDelegator(token);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleRegisterSessionListener(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleRegisterSessionListener!");
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    WSError errCode = RegisterSessionListener(listener);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleUnRegisterSessionListener(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleUnRegisterSessionListener!");
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    WSError errCode = UnRegisterSessionListener(listener);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetSessionInfos(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleGetSessionInfos!");
    std::string deviceId = Str16ToStr8(data.ReadString16());
    int numMax = data.ReadInt32();
    std::vector<SessionInfoBean> missionInfos;
    WSError errCode = GetSessionInfos(deviceId, numMax, missionInfos);
    reply.WriteInt32(missionInfos.size());
    for (auto& it : missionInfos) {
        if (!reply.WriteParcelable(&it)) {
            WLOGFE("GetSessionInfos error");
            return ERR_INVALID_DATA;
        }
    }
    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetSessionInfo(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleGetSessionInfo!");
    SessionInfoBean info;
    std::string deviceId = Str16ToStr8(data.ReadString16());
    int32_t persistentId = data.ReadInt32();
    WSError errCode = GetSessionInfo(deviceId, persistentId, info);
    if (!reply.WriteParcelable(&info)) {
        WLOGFE("GetSessionInfo error");
        return ERR_INVALID_DATA;
    }

    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        WLOGFE("GetSessionInfo result error");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleTerminateSessionNew(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleTerminateSessionNew");
    sptr<AAFwk::SessionInfo> abilitySessionInfo = data.ReadParcelable<AAFwk::SessionInfo>();
    bool needStartCaller = data.ReadBool();
    const WSError& errCode = TerminateSessionNew(abilitySessionInfo, needStartCaller);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetFocusSessionToken(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleGetFocusSessionToken!");
    sptr<IRemoteObject> token = nullptr;
    const WSError& errCode = GetFocusSessionToken(token);
    reply.WriteRemoteObject(token);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleSetSessionContinueState(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("HandleSetSessionContinueState");
    sptr <IRemoteObject> token = data.ReadRemoteObject();
    auto continueState = static_cast<ContinueState>(data.ReadInt32());
    const WSError &ret = SetSessionContinueState(token, continueState);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetSessionSnapshot(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleGetSessionSnapshot!");
    std::string deviceId = Str16ToStr8(data.ReadString16());
    int32_t persistentId = data.ReadInt32();
    bool isLowResolution = data.ReadBool();
    std::shared_ptr<SessionSnapshot> snapshot = std::make_shared<SessionSnapshot>();
    const WSError& ret = GetSessionSnapshot(deviceId, persistentId, *snapshot, isLowResolution);
    reply.WriteParcelable(snapshot.get());
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleClearSession(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleClearSession!");
    int32_t persistentId = data.ReadInt32();
    const WSError& ret = ClearSession(persistentId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleClearAllSessions(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleClearAllSessions!");
    const WSError& ret = ClearAllSessions();
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleLockSession(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleLockSession!");
    int32_t sessionId = data.ReadInt32();
    const WSError& ret = LockSession(sessionId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleUnlockSession(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleUnlockSession!");
    int32_t sessionId = data.ReadInt32();
    const WSError& ret = UnlockSession(sessionId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleMoveSessionsToForeground(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleMoveSessionsToForeground!");
    std::vector<int32_t> sessionIds;
    data.ReadInt32Vector(&sessionIds);
    int32_t topSessionId = data.ReadInt32();
    const WSError &ret = MoveSessionsToForeground(sessionIds, topSessionId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleMoveSessionsToBackground(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("run HandleMoveSessionsToBackground!");
    std::vector<int32_t> sessionIds;
    data.ReadInt32Vector(&sessionIds);
    std::vector<int32_t> result;
    data.ReadInt32Vector(&result);
    const WSError &ret = MoveSessionsToBackground(sessionIds, result);
    reply.WriteInt32Vector(result);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}
} // namespace OHOS::Rosen
