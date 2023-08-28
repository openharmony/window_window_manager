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
#include "marshalling_helper.h"
#include "session/host/include/scene_session.h"
#include "window_manager.h"
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
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_IS_VALID_SESSION_IDS),
        &SceneSessionManagerStub::HandleIsValidSessionIds),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_REGISTER_SESSION_CHANGE_LISTENER),
        &SceneSessionManagerStub::HandleRegisterSessionChangeListener),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UNREGISTER_SESSION_CHANGE_LISTENER),
        &SceneSessionManagerStub::HandleUnRegisterSessionChangeListener),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_PENDING_SESSION_TO_FOREGROUND),
        &SceneSessionManagerStub::HandlePendingSessionToForeground),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR),
        &SceneSessionManagerStub::HandlePendingSessionToBackgroundForDelegator),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_FOCUS_SESSION_TOKEN),
        &SceneSessionManagerStub::HandleGetFocusSessionToken),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_CHECK_WINDOW_ID),
        &SceneSessionManagerStub::HandleCheckWindowId),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_SET_GESTURE_NAVIGATION_ENABLED),
        &SceneSessionManagerStub::HandleSetGestureNavigationEnabled),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_WINDOW_INFO),
        &SceneSessionManagerStub::HandleGetAccessibilityWindowInfo),

    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_REGISTER_SESSION_LISTENER),
        &SceneSessionManagerStub::HandleRegisterSessionListener),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UNREGISTER_SESSION_LISTENER),
        &SceneSessionManagerStub::HandleUnRegisterSessionListener),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_MISSION_INFOS),
        &SceneSessionManagerStub::HandleGetSessionInfos),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_MISSION_INFO_BY_ID),
        &SceneSessionManagerStub::HandleGetSessionInfo),

    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_DUMP_SESSION_ALL),
        &SceneSessionManagerStub::HandleDumpSessionAll),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_DUMP_SESSION_WITH_ID),
        &SceneSessionManagerStub::HandleDumpSessionWithId),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_TERMINATE_SESSION_NEW),
        &SceneSessionManagerStub::HandleTerminateSessionNew),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UPDATE_AVOIDAREA_LISTENER),
        &SceneSessionManagerStub::HandleUpdateSessionAvoidAreaListener),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_SESSION_DUMP_INFO),
        &SceneSessionManagerStub::HandleGetSessionDump),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_BIND_DIALOG_TARGET),
        &SceneSessionManagerStub::HandleBindDialogTarget),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_SESSION_SNAPSHOT),
        &SceneSessionManagerStub::HandleGetSessionSnapshot),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_SESSION_DUMP_INFO),
        &SceneSessionManagerStub::HandleGetSessionDump),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_NOTIFY_DUMP_INFO_RESULT),
        &SceneSessionManagerStub::HandleNotifyDumpInfoResult),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_SET_SESSION_CONTINUE_STATE),
        &SceneSessionManagerStub::HandleSetSessionContinueState),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_SET_SESSION_GRAVITY),
        &SceneSessionManagerStub::HandleSetSessionGravity),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_CLEAR_SESSION),
        &SceneSessionManagerStub::HandleClearSession),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_CLEAR_ALL_SESSIONS),
        &SceneSessionManagerStub::HandleClearAllSessions),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_LOCK_SESSION),
        &SceneSessionManagerStub::HandleLockSession),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UNLOCK_SESSION),
        &SceneSessionManagerStub::HandleUnlockSession),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_MOVE_MISSIONS_TO_FOREGROUND),
        &SceneSessionManagerStub::HandleMoveSessionsToForeground),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_MOVE_MISSIONS_TO_BACKGROUND),
        &SceneSessionManagerStub::HandleMoveSessionsToBackground),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_REGISTER_COLLABORATOR),
        &SceneSessionManagerStub::HandleRegisterCollaborator),
    std::make_pair(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UNREGISTER_COLLABORATOR),
        &SceneSessionManagerStub::HandleUnregisterCollaborator),
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

    sptr<IRemoteObject> token = nullptr;
    if (property && property->GetTokenState()) {
        token = data.ReadRemoteObject();
    } else {
        WLOGI("accept token is nullptr");
    }

    auto persistentId = INVALID_SESSION_ID;
    sptr<ISession> sceneSession;
    CreateAndConnectSpecificSession(sessionStage, eventChannel, surfaceNode,
        property, persistentId, sceneSession, token);
    if (sceneSession== nullptr) {
        return ERR_INVALID_STATE;
    }
    reply.WriteInt32(persistentId);
    reply.WriteRemoteObject(sceneSession->AsObject());
    reply.WriteUint32(static_cast<uint32_t>(WSError::WS_OK));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleDestroyAndDisconnectSpcificSession(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleDestroyAndDisconnectSpcificSession!");
    auto persistentId = data.ReadInt32();
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

int SceneSessionManagerStub::HandleIsValidSessionIds(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleIsValidSessionIds!");
    std::vector<int32_t> sessionIds;
    data.ReadInt32Vector(&sessionIds);
    std::vector<bool> results;
    reply.WriteBoolVector(results);
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleRegisterSessionChangeListener(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleRegisterSessionChangeListener!");
    sptr<ISessionChangeListener> listener = iface_cast<ISessionChangeListener>(data.ReadRemoteObject());
    WSError errCode = RegisterSessionListener(listener);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleUnRegisterSessionChangeListener(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleUnRegisterSessionChangeListener!");
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

int SceneSessionManagerStub::HandleRegisterSessionListener(MessageParcel& data, MessageParcel& reply)
{
    WLOGFI("run HandleRegisterSessionListener!");
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    WSError errCode = RegisterSessionListener(listener);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleUnRegisterSessionListener(MessageParcel& data, MessageParcel& reply)
{
    WLOGFI("run HandleUnRegisterSessionListener!");
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    WSError errCode = UnRegisterSessionListener(listener);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleGetSessionInfos(MessageParcel& data, MessageParcel& reply)
{
    WLOGFI("run HandleGetSessionInfos!");
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

int SceneSessionManagerStub::HandleGetSessionInfo(MessageParcel& data, MessageParcel& reply)
{
    WLOGFI("run HandleGetSessionInfo!");
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

int SceneSessionManagerStub::HandleDumpSessionAll(MessageParcel& data, MessageParcel& reply)
{
    WLOGFI("run HandleDumpSessionAll!");
    std::vector<std::string> infos;
    WSError errCode = DumpSessionAll(infos);
    if (!reply.WriteStringVector(infos)) {
        WLOGFE("HandleDumpSessionAll write info failed.");
        return ERR_TRANSACTION_FAILED;
    }

    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        WLOGFE("HandleDumpSessionAll write errcode failed.");
        return ERR_TRANSACTION_FAILED;
    }
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleDumpSessionWithId(MessageParcel& data, MessageParcel& reply)
{
    WLOGFI("run HandleDumpSessionWithId!");
    int32_t persistentId = data.ReadInt32();
    std::vector<std::string> infos;
    WSError errCode = DumpSessionWithId(persistentId, infos);
    if (!reply.WriteStringVector(infos)) {
        WLOGFE("HandleDumpSessionWithId write info failed.");
        return ERR_TRANSACTION_FAILED;
    }

    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        WLOGFE("HandleDumpSessionWithId write errcode failed.");
        return ERR_TRANSACTION_FAILED;
    }
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleTerminateSessionNew(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleTerminateSessionNew");
    sptr<AAFwk::SessionInfo> abilitySessionInfo = data.ReadParcelable<AAFwk::SessionInfo>();
    bool needStartCaller = data.ReadBool();
    const WSError& errCode = TerminateSessionNew(abilitySessionInfo, needStartCaller);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleGetFocusSessionToken(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleGetFocusSessionToken!");
    sptr<IRemoteObject> token = nullptr;
    const WSError& errCode = GetFocusSessionToken(token);
    reply.WriteRemoteObject(token);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleCheckWindowId(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleCheckWindowId!");
    int32_t windowId = INVALID_WINDOW_ID;
    if (!data.ReadInt32(windowId)) {
        WLOGE("Failed to readInt32 windowId");
        return ERR_INVALID_DATA;
    }
    int32_t pid = INVALID_PID;
    WMError errCode = CheckWindowId(windowId, pid);
    if (errCode != WMError::WM_OK) {
        WLOGE("Failed to checkWindowId(%{public}d)", pid);
        return ERR_INVALID_DATA;
    }
    if (!reply.WriteInt32(pid)) {
        WLOGE("Failed to WriteInt32 pid");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleSetGestureNavigationEnabled(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleSetGestureNavigationEnabled!");
    bool enable = data.ReadBool();
    const WMError &ret = SetGestureNavigaionEnabled(enable);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleGetAccessibilityWindowInfo(MessageParcel &data, MessageParcel &reply)
{
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    WMError errCode = GetAccessibilityWindowInfo(infos);
    if (!MarshallingHelper::MarshallingVectorParcelableObj<AccessibilityWindowInfo>(reply, infos)) {
        WLOGFE("Write window infos failed.");
        return ERR_TRANSACTION_FAILED;
    }
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleSetSessionContinueState(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("HandleSetSessionContinueState");
    sptr <IRemoteObject> token = data.ReadRemoteObject();
    auto continueState = static_cast<ContinueState>(data.ReadInt32());
    const WSError &ret = SetSessionContinueState(token, continueState);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleSetSessionGravity(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleSetSessionGravity!");
    auto persistentId = data.ReadInt32();
    SessionGravity gravity = static_cast<SessionGravity>(data.ReadUint32());
    uint32_t percent = data.ReadUint32();
    WSError ret = SetSessionGravity(persistentId, gravity, percent);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleGetSessionDump(MessageParcel &data, MessageParcel &reply)
{
    std::vector<std::string> params;
    if (!data.ReadStringVector(&params)) {
        WLOGFE("Fail to read params");
        return -1;
    }
    std::string dumpInfo;
    WSError errCode = GetSessionDumpInfo(params, dumpInfo);
    const char* info = dumpInfo.c_str();
    uint32_t infoSize = static_cast<uint32_t>(strlen(info));
    WLOGFI("HandleGetSessionDump, infoSize: %{public}d", infoSize);
    reply.WriteUint32(infoSize);
    reply.WriteRawData(info, infoSize);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleUpdateSessionAvoidAreaListener(MessageParcel& data, MessageParcel& reply)
{
    auto persistentId = data.ReadInt32();
    bool haveAvoidAreaListener = data.ReadBool();
    WSError errCode = UpdateSessionAvoidAreaListener(persistentId, haveAvoidAreaListener);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleBindDialogTarget(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleBindDialogTarget!");
    auto persistentId = data.ReadUint64();
    sptr<IRemoteObject> remoteObject = data.ReadRemoteObject();
    const WSError& ret = BindDialogTarget(persistentId, remoteObject);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleGetSessionSnapshot(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleGetSessionSnapshot!");
    std::string deviceId = Str16ToStr8(data.ReadString16());
    int32_t persistentId = data.ReadInt32();
    bool isLowResolution = data.ReadBool();
    std::shared_ptr<Media::PixelMap> snapshot = std::make_shared<Media::PixelMap>();
    const WSError& ret = GetSessionSnapshot(deviceId, persistentId, snapshot, isLowResolution);
    reply.WriteParcelable(snapshot.get());
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleNotifyDumpInfoResult(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("HandleNotifyDumpInfoResult");
    std::vector<std::string> info;
    uint32_t vectorSize = data.ReadUint32();
    for (uint32_t i = 0; i < vectorSize; i++) {
        uint32_t curSize = data.ReadUint32();
        info.emplace_back(reinterpret_cast<const char*>(data.ReadRawData(curSize)));
        WLOGFD("HandleNotifyDumpInfoResult count: %{public}u, infoSize: %{public}u", i, curSize);
    }
    NotifyDumpInfoResult(info);
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleClearSession(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleClearSession!");
    int32_t persistentId = data.ReadInt32();
    const WSError& ret = ClearSession(persistentId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleClearAllSessions(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleClearAllSessions!");
    const WSError& ret = ClearAllSessions();
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleLockSession(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleLockSession!");
    int32_t sessionId = data.ReadInt32();
    const WSError& ret = LockSession(sessionId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}
int SceneSessionManagerStub::HandleUnlockSession(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleUnlockSession!");
    int32_t sessionId = data.ReadInt32();
    const WSError& ret = UnlockSession(sessionId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}
int SceneSessionManagerStub::HandleMoveSessionsToForeground(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleMoveSessionsToForeground!");
    std::vector<int32_t> sessionIds;
    data.ReadInt32Vector(&sessionIds);
    const WSError &ret = MoveSessionsToForeground(sessionIds);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}
int SceneSessionManagerStub::HandleMoveSessionsToBackground(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleMoveSessionsToBackground!");
    std::vector<int32_t> sessionIds;
    data.ReadInt32Vector(&sessionIds);
    const WSError &ret = MoveSessionsToBackground(sessionIds);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleRegisterCollaborator(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleRegisterCollaborator!");
    int32_t type = data.ReadInt32();
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator = iface_cast<AAFwk::IAbilityManagerCollaborator>(data.ReadRemoteObject());
    const WSError& ret = RegisterIAbilityManagerCollaborator(type, collaborator);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerStub::HandleUnregisterCollaborator(MessageParcel &data, MessageParcel &reply)
{
    WLOGFI("run HandleUnregisterCollaborator!");
    int32_t type = data.ReadInt32();
    const WSError& ret = UnregisterIAbilityManagerCollaborator(type);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}
} // namespace OHOS::Rosen
