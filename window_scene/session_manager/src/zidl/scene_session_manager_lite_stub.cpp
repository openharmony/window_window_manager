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

#include "marshalling_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManagerLiteStub"};
constexpr uint32_t MAX_VECTOR_SIZE = 100;
constexpr uint32_t MAX_TOPN_INFO_SIZE = 200;
constexpr int32_t MAX_CONTROL_APP_INFO = 200;
}

int SceneSessionManagerLiteStub::OnRemoteRequest(uint32_t code,
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    WLOGFD("Scene session on remote request!, code: %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_TRANSACTION_FAILED;
    }

    return ProcessRemoteRequest(code, data, reply, option);
}

int SceneSessionManagerLiteStub::ProcessRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    switch (code) {
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_SET_SESSION_LABEL):
            return HandleSetSessionLabel(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_SET_SESSION_ICON):
            return HandleSetSessionIcon(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_IS_VALID_SESSION_IDS):
            return HandleIsValidSessionIds(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_PENDING_SESSION_TO_FOREGROUND):
            return HandlePendingSessionToForeground(data, reply);
        case static_cast<uint32_t>(
            SceneSessionManagerLiteMessage::TRANS_ID_PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR):
            return HandlePendingSessionToBackgroundForDelegator(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_FOCUS_SESSION_TOKEN):
            return HandleGetFocusSessionToken(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_FOCUS_SESSION_ELEMENT):
            return HandleGetFocusSessionElement(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_REGISTER_SESSION_LISTENER):
            return HandleRegisterSessionListener(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_UNREGISTER_SESSION_LISTENER):
            return HandleUnRegisterSessionListener(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_MISSION_INFOS):
            return HandleGetSessionInfos(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_MAIN_WINDOW_STATES_BY_PID):
            return HandleGetMainWindowStatesByPid(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_MISSION_INFO_BY_ID):
            return HandleGetSessionInfo(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_SESSION_INFO_BY_CONTINUE_SESSION_ID):
            return HandleGetSessionInfoByContinueSessionId(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_TERMINATE_SESSION_NEW):
            return HandleTerminateSessionNew(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_SESSION_SNAPSHOT):
            return HandleGetSessionSnapshot(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_SET_SESSION_CONTINUE_STATE):
            return HandleSetSessionContinueState(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_CLEAR_SESSION):
            return HandleClearSession(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_CLEAR_ALL_SESSIONS):
            return HandleClearAllSessions(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_LOCK_SESSION):
            return HandleLockSession(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_UNLOCK_SESSION):
            return HandleUnlockSession(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_MOVE_MISSIONS_TO_FOREGROUND):
            return HandleMoveSessionsToForeground(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_MOVE_MISSIONS_TO_BACKGROUND):
            return HandleMoveSessionsToBackground(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_FOCUS_SESSION_INFO):
            return HandleGetFocusSessionInfo(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT):
            return HandleRegisterWindowManagerAgent(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT):
            return HandleUnregisterWindowManagerAgent(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_CHECK_WINDOW_ID):
            return HandleCheckWindowId(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_UI_EXTENSION_CREATION_CHECK):
            return HandleCheckUIExtensionCreation(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_VISIBILITY_WINDOW_INFO_ID):
            return HandleGetVisibilityWindowInfo(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_WINDOW_MODE_TYPE):
            return HandleGetWindowModeType(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_RAISE_WINDOW_TO_TOP):
            return HandleRaiseWindowToTop(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_TOPN_MAIN_WINDOW_INFO):
            return HandleGetMainWinodowInfo(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_REGISTER_COLLABORATOR):
            return HandleRegisterCollaborator(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_UNREGISTER_COLLABORATOR):
            return HandleUnregisterCollaborator(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_ALL_MAIN_WINDOW_INFO):
            return HandleGetAllMainWindowInfos(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_CLEAR_MAIN_SESSIONS):
            return HandleClearMainSessions(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_WINDOW_STYLE_TYPE):
            return HandleGetWindowStyleType(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_TERMINATE_SESSION_BY_PERSISTENT_ID):
            return HandleTerminateSessionByPersistentId(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_CLOSE_TARGET_FLOAT_WINDOW):
            return HandleCloseTargetFloatWindow(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_CLOSE_TARGET_PIP_WINDOW):
            return HandleCloseTargetPiPWindow(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_CURRENT_PIP_WINDOW_INFO):
            return HandleGetCurrentPiPWindowInfo(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_ROOT_MAIN_WINDOW_ID):
            return HandleGetRootMainWindowId(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_WINDOW_INFO):
            return HandleGetAccessibilityWindowInfo(data, reply);
        case static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_NOTIFY_APP_USE_CONTROL_LIST):
            return HandleNotifyAppUseControlList(data, reply);
        default:
            WLOGFE("Failed to find function handler!");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int SceneSessionManagerLiteStub::HandleSetSessionLabel(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LIFE, "In");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    std::string label;
    if (!data.ReadString(label)) {
        TLOGD(WmsLogTag::WMS_LIFE, "Read label failed.");
        return ERR_INVALID_DATA;
    }
    WSError errCode = SetSessionLabel(token, label);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleSetSessionIcon(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleSetSessionIcon!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    std::shared_ptr<Media::PixelMap> icon(data.ReadParcelable<Media::PixelMap>());
    if (icon == nullptr) {
        WLOGFE("icon is null");
        return ERR_INVALID_DATA;
    }
    WSError errCode = SetSessionIcon(token, icon);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleIsValidSessionIds(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleIsValidSessionIds!");
    std::vector<int32_t> sessionIds;
    data.ReadInt32Vector(&sessionIds);
    std::vector<bool> results;
    WSError errCode = IsValidSessionIds(sessionIds, results);
    reply.WriteBoolVector(results);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandlePendingSessionToForeground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandlePendingSessionToForeground!");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    if (token == nullptr) {
        WLOGFE("token is nullptr");
        return ERR_INVALID_DATA;
    }
    WSError errCode = PendingSessionToForeground(token);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandlePendingSessionToBackgroundForDelegator(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LIFE, "run");
    sptr<IRemoteObject> token = data.ReadRemoteObject();
    if (token == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "token is nullptr");
        return ERR_INVALID_DATA;
    }
    bool shouldBackToCaller = true;
    if (!data.ReadBool(shouldBackToCaller)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read shouldBackToCaller failed");
        return ERR_INVALID_DATA;
    }
    WSError errCode = PendingSessionToBackgroundForDelegator(token, shouldBackToCaller);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleRegisterSessionListener(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleRegisterSessionListener!");
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    if (listener == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "listener is nullptr!");
        reply.WriteInt32(static_cast<int32_t>(WSError::WS_ERROR_INVALID_PARAM));
        return ERR_NONE;
    }
    WSError errCode = RegisterSessionListener(listener);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleUnRegisterSessionListener(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleUnRegisterSessionListener!");
    sptr<ISessionListener> listener = iface_cast<ISessionListener>(data.ReadRemoteObject());
    if (listener == nullptr) {
        reply.WriteInt32(static_cast<int32_t>(WSError::WS_ERROR_INVALID_PARAM));
        return ERR_NONE;
    }
    WSError errCode = UnRegisterSessionListener(listener);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetSessionInfos(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleGetSessionInfos!");
    std::u16string deviceIdU16;
    if (!data.ReadString16(deviceIdU16)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read deviceId fail");
        return ERR_TRANSACTION_FAILED;
    }
    std::string deviceId = Str16ToStr8(deviceIdU16);
    int32_t numMax = 0;
    if (!data.ReadInt32(numMax)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read numMax fail");
        return ERR_TRANSACTION_FAILED;
    }
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

int SceneSessionManagerLiteStub::HandleGetMainWindowStatesByPid(MessageParcel& data, MessageParcel& reply)
{
    int32_t pid = 0;
    if (!data.ReadInt32(pid)) {
        TLOGE(WmsLogTag::WMS_LIFE, "read pid fail");
        return ERR_INVALID_DATA;
    }
    std::vector<MainWindowState> windowStates;
    WSError errCode = GetMainWindowStatesByPid(pid, windowStates);
    if (!reply.WriteInt32(windowStates.size())) {
        TLOGE(WmsLogTag::WMS_LIFE, "write windowStates size fail");
        return ERR_INVALID_DATA;
    }
    for (auto& state : windowStates) {
        if (!reply.WriteParcelable(&state)) {
            TLOGE(WmsLogTag::WMS_LIFE, "write windowState fail");
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
    TLOGD(WmsLogTag::WMS_LIFE, "In!");
    SessionInfoBean info;
    std::u16string deviceIdU16;
    if (!data.ReadString16(deviceIdU16)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read deviceId fail");
        return ERR_TRANSACTION_FAILED;
    }
    std::string deviceId = Str16ToStr8(deviceIdU16);
    int32_t persistentId = 0;
    if (!data.ReadInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read persistentId fail");
        return ERR_TRANSACTION_FAILED;
    }
    WSError errCode = GetSessionInfo(deviceId, persistentId, info);
    if (!reply.WriteParcelable(&info)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write sessionInfo fail");
        return ERR_INVALID_DATA;
    }

    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write sessionInfo result fail");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetSessionInfoByContinueSessionId(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LIFE, "In!");
    std::string continueSessionId;
    if (!data.ReadString(continueSessionId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "read continueSessionId fail");
        return ERR_INVALID_DATA;
    }
    TLOGI(WmsLogTag::WMS_LIFE, "continueSessionId: %{public}s", continueSessionId.c_str());
    SessionInfoBean info;
    WSError errCode = GetSessionInfoByContinueSessionId(continueSessionId, info);
    if (!reply.WriteParcelable(&info)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Get sessionInfo by continueSessionId error");
        return ERR_INVALID_DATA;
    }

    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        TLOGE(WmsLogTag::WMS_LIFE, "Get sessionInfo by continueSessionId result error");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleTerminateSessionNew(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LIFE, "in");
    sptr<AAFwk::SessionInfo> abilitySessionInfo = data.ReadParcelable<AAFwk::SessionInfo>();
    if (abilitySessionInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "abilitySessionInfo is null");
        return ERR_INVALID_DATA;
    }
    bool needStartCaller = false;
    if (!data.ReadBool(needStartCaller)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read needStartCaller failed.");
        return ERR_INVALID_DATA;
    }
    bool isFromBroker = false;
    if (!data.ReadBool(isFromBroker)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read isFromBroker failed.");
        return ERR_INVALID_DATA;
    }
    WSError errCode = TerminateSessionNew(abilitySessionInfo, needStartCaller, isFromBroker);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetFocusSessionToken(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleGetFocusSessionToken!");
    sptr<IRemoteObject> token = nullptr;
    WSError errCode = GetFocusSessionToken(token);
    reply.WriteRemoteObject(token);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetFocusSessionElement(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleGetFocusSessionElement!");
    AppExecFwk::ElementName element;
    WSError errCode = GetFocusSessionElement(element);
    reply.WriteParcelable(&element);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleSetSessionContinueState(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleSetSessionContinueState");
    sptr <IRemoteObject> token = data.ReadRemoteObject();
    int32_t continueStateValue = 0;
    if (!data.ReadInt32(continueStateValue)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read continueState fail");
        return ERR_TRANSACTION_FAILED;
    }
    if (continueStateValue < static_cast<int32_t>(ContinueState::CONTINUESTATE_UNKNOWN) ||
        continueStateValue > static_cast<int32_t>(ContinueState::CONTINUESTATE_MAX)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Invalid continueState");
        return ERR_INVALID_DATA;
    }
    auto continueState = static_cast<ContinueState>(continueStateValue);
    const WSError &ret = SetSessionContinueState(token, continueState);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetSessionSnapshot(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_SYSTEM, "Handled!");
    std::u16string deviceIdData;
    if (!data.ReadString16(deviceIdData)) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "read deviceId fail");
        return ERR_INVALID_DATA;
    }
    std::string deviceId = Str16ToStr8(deviceIdData);
    int32_t persistentId = 0;
    if (!data.ReadInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "read persistentId fail");
        return ERR_INVALID_DATA;
    }
    bool isLowResolution = false;
    if (!data.ReadBool(isLowResolution)) {
        TLOGE(WmsLogTag::WMS_SYSTEM, "read isLowResolution fail");
        return ERR_INVALID_DATA;
    }
    std::shared_ptr<SessionSnapshot> snapshot = std::make_shared<SessionSnapshot>();
    WSError ret = GetSessionSnapshot(deviceId, persistentId, *snapshot, isLowResolution);
    reply.WriteParcelable(snapshot.get());
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleClearSession(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleClearSession!");
    int32_t persistentId = 0;
    if (!data.ReadInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read persistentId fail");
        return ERR_TRANSACTION_FAILED;
    }
    WSError ret = ClearSession(persistentId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleClearAllSessions(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleClearAllSessions!");
    WSError ret = ClearAllSessions();
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleLockSession(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleLockSession!");
    int32_t sessionId = 0;
    if (!data.ReadInt32(sessionId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read sessionId fail");
        return ERR_TRANSACTION_FAILED;
    }
    WSError ret = LockSession(sessionId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleUnlockSession(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleUnlockSession!");
    int32_t sessionId = 0;
    if (!data.ReadInt32(sessionId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read sessionId fail");
        return ERR_TRANSACTION_FAILED;
    }
    WSError ret = UnlockSession(sessionId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleMoveSessionsToForeground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleMoveSessionsToForeground!");
    std::vector<int32_t> sessionIds;
    data.ReadInt32Vector(&sessionIds);
    int32_t topSessionId = 0;
    if (!data.ReadInt32(topSessionId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read topSessionId fail");
        return ERR_TRANSACTION_FAILED;
    }
    const WSError &ret = MoveSessionsToForeground(sessionIds, topSessionId);
    reply.WriteUint32(static_cast<uint32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleMoveSessionsToBackground(MessageParcel& data, MessageParcel& reply)
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

int SceneSessionManagerLiteStub::HandleGetFocusSessionInfo(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run");
    FocusChangeInfo focusInfo;
    GetFocusWindowInfo(focusInfo);
    reply.WriteParcelable(&focusInfo);
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleCheckWindowId(MessageParcel& data, MessageParcel& reply)
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

int SceneSessionManagerLiteStub::HandleCheckUIExtensionCreation(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "UIExtOnLock: called");

    int32_t windowId = INVALID_WINDOW_ID;
    if (!data.ReadInt32(windowId)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "UIExtOnLock: Failed to get windowId");
        return ERR_INVALID_DATA;
    }

    uint32_t token = -1;
    if (!data.ReadUint32(token)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "UIExtOnLock: Failed to get token");
        return ERR_INVALID_DATA;
    }

    sptr<AppExecFwk::ElementName> element = data.ReadParcelable<AppExecFwk::ElementName>();
    if (!element) {
        TLOGE(WmsLogTag::WMS_UIEXT, "UIExtOnLock: Failed to get element");
        return ERR_INVALID_DATA;
    }

    int32_t pid = INVALID_PID;
    WMError errCode = CheckUIExtensionCreation(windowId, token, *element, pid);
    TLOGI(WmsLogTag::WMS_UIEXT, "UIExtOnLock: ret %{public}u", errCode);

    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        TLOGE(WmsLogTag::WMS_UIEXT, "UIExtOnLock: Failed to write errcode");
        return ERR_INVALID_DATA;
    }

    if (!reply.WriteInt32(pid)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "UIExtOnLock: Failed to write pid");
        return ERR_INVALID_DATA;
    }

    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleRegisterWindowManagerAgent(MessageParcel& data, MessageParcel& reply)
{
    uint32_t typeId = 0;
    if (!data.ReadUint32(typeId) ||
        typeId < static_cast<uint32_t>(WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS) ||
        typeId >= static_cast<uint32_t>(WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_END)) {
        return ERR_INVALID_DATA;
    }
    WindowManagerAgentType type = static_cast<WindowManagerAgentType>(typeId);
    TLOGI(WmsLogTag::DEFAULT, "type=%{public}u", typeId);
    sptr<IRemoteObject> windowManagerAgentObject = data.ReadRemoteObject();
    sptr<IWindowManagerAgent> windowManagerAgentProxy =
            iface_cast<IWindowManagerAgent>(windowManagerAgentObject);
    WMError errCode = RegisterWindowManagerAgent(type, windowManagerAgentProxy);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleUnregisterWindowManagerAgent(MessageParcel& data, MessageParcel& reply)
{
    uint32_t typeId = 0;
    if (!data.ReadUint32(typeId) ||
        typeId < static_cast<uint32_t>(WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS) ||
        typeId >= static_cast<uint32_t>(WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_END)) {
        return ERR_INVALID_DATA;
    }
    WindowManagerAgentType type = static_cast<WindowManagerAgentType>(typeId);
    TLOGI(WmsLogTag::DEFAULT, "type=%{public}u", typeId);
    sptr<IRemoteObject> windowManagerAgentObject = data.ReadRemoteObject();
    sptr<IWindowManagerAgent> windowManagerAgentProxy =
            iface_cast<IWindowManagerAgent>(windowManagerAgentObject);
    WMError errCode = UnregisterWindowManagerAgent(type, windowManagerAgentProxy);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetVisibilityWindowInfo(MessageParcel& data, MessageParcel& reply)
{
    std::vector<sptr<WindowVisibilityInfo>> infos;
    WMError errCode = GetVisibilityWindowInfo(infos);
    if (!MarshallingHelper::MarshallingVectorParcelableObj<WindowVisibilityInfo>(reply, infos)) {
        WLOGFE("Write visibility window infos failed");
        return -1;
    }
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetWindowModeType(MessageParcel& data, MessageParcel& reply)
{
    WindowModeType windowModeType = Rosen::WindowModeType::WINDOW_MODE_OTHER;
    WMError errCode = GetWindowModeType(windowModeType);
    WLOGFI("run HandleGetWindowModeType, windowModeType:%{public}d!", static_cast<int32_t>(windowModeType));
    if (!reply.WriteUint32(static_cast<int32_t>(windowModeType))) {
        WLOGE("Failed to WriteBool");
        return ERR_INVALID_DATA;
    }
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetMainWinodowInfo(MessageParcel& data, MessageParcel& reply)
{
    TLOGI(WmsLogTag::WMS_MAIN, "run HandleGetMainWinodowInfo lite");
    int32_t topN = 0;
    if (!data.ReadInt32(topN)) {
        TLOGE(WmsLogTag::WMS_MAIN, "failed to read topN");
        return ERR_INVALID_DATA;
    }
    TLOGD(WmsLogTag::WMS_MAIN, "topN :%{public}d", topN);
    std::vector<MainWindowInfo> topNInfos;
    WMError errCode = GetMainWindowInfos(topN, topNInfos);
    if ((topNInfos.size() <= 0) || (topNInfos.size() >= MAX_TOPN_INFO_SIZE)) {
        return ERR_INVALID_DATA;
    }
    reply.WriteInt32(topNInfos.size());
    for (auto& it : topNInfos) {
        if (!reply.WriteParcelable(&it)) {
            TLOGE(WmsLogTag::WMS_MAIN, "write topNinfo fail");
            return ERR_INVALID_DATA;
        }

        TLOGI(WmsLogTag::WMS_MAIN, "pid %{public}d, name %{public}s",
            it.pid_, it.bundleName_.c_str());
    }

    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        return ERR_INVALID_DATA;
    }

    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetAllMainWindowInfos(MessageParcel& data, MessageParcel& reply)
{
    std::vector<MainWindowInfo> infos;
    WMError errCode = GetAllMainWindowInfos(infos);
    reply.WriteInt32(infos.size());
    for (auto& info : infos) {
        if (!reply.WriteParcelable(&info)) {
            TLOGE(WmsLogTag::WMS_MAIN, "write main window info fail");
            return ERR_INVALID_DATA;
        }
    }
    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleClearMainSessions(MessageParcel& data, MessageParcel& reply)
{
    std::vector<int32_t> persistentIds;
    std::vector<int32_t> clearFailedIds;
    if (!data.ReadInt32Vector(&persistentIds)) {
        TLOGE(WmsLogTag::WMS_MAIN, "failed to read persistentIds.");
        return ERR_INVALID_DATA;
    }
    WMError errCode = ClearMainSessions(persistentIds, clearFailedIds);
    if (!reply.WriteInt32Vector(clearFailedIds)) {
        TLOGE(WmsLogTag::WMS_MAIN, "write clearFailedIds fail.");
        return ERR_INVALID_DATA;
    }
    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleRaiseWindowToTop(MessageParcel& data, MessageParcel& reply)
{
    auto persistentId = 0;
    if (!data.ReadInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_HIERARCHY, "read persistentId failed");
        return ERR_INVALID_DATA;
    }
    WSError errCode = RaiseWindowToTop(persistentId);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleRegisterCollaborator(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_MAIN, "called.");
    int32_t type = 0;
    if (!data.ReadInt32(type)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read type fail");
        return ERR_TRANSACTION_FAILED;
    }
    sptr<IRemoteObject> collaboratorObject = data.ReadRemoteObject();
    if (collaboratorObject == nullptr) {
        TLOGE(WmsLogTag::WMS_MAIN, "collaboratorObject is null.");
        return ERR_NULL_OBJECT;
    }
    sptr<AAFwk::IAbilityManagerCollaborator> collaborator =
        iface_cast<AAFwk::IAbilityManagerCollaborator>(collaboratorObject);
    WSError ret = RegisterIAbilityManagerCollaborator(type, collaborator);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleUnregisterCollaborator(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_MAIN, "called.");
    int32_t type = 0;
    if (!data.ReadInt32(type)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read type fail");
        return ERR_TRANSACTION_FAILED;
    }
    WSError ret = UnregisterIAbilityManagerCollaborator(type);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetWindowStyleType(MessageParcel& data, MessageParcel& reply)
{
    WindowStyleType windowStyleType = Rosen::WindowStyleType::WINDOW_STYLE_DEFAULT;
    WMError errCode = GetWindowStyleType(windowStyleType);
    TLOGI(WmsLogTag::WMS_MAIN, "windowStyleType:%{public}d!", static_cast<int32_t>(windowStyleType));
    if (!reply.WriteUint32(static_cast<int32_t>(windowStyleType))) {
        TLOGE(WmsLogTag::WMS_MAIN, "Failed to WriteBool");
        return ERR_INVALID_DATA;
    }
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleTerminateSessionByPersistentId(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LIFE, "In!");
    int32_t persistentId;
    if (!data.ReadInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "read persistentId failed");
        return ERR_INVALID_DATA;
    }
    WMError errCode = TerminateSessionByPersistentId(persistentId);
    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleCloseTargetFloatWindow(MessageParcel& data, MessageParcel& reply)
{
    std::string bundleName = data.ReadString();
    CloseTargetFloatWindow(bundleName);
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleCloseTargetPiPWindow(MessageParcel& data, MessageParcel& reply)
{
    std::string bundleName = data.ReadString();
    WMError errCode = CloseTargetPiPWindow(bundleName);
    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetCurrentPiPWindowInfo(MessageParcel& data, MessageParcel& reply)
{
    std::string bundleName;
    WMError errCode = GetCurrentPiPWindowInfo(bundleName);
    if (!reply.WriteInt32(static_cast<int32_t>(errCode))) {
        return ERR_INVALID_DATA;
    }
    if (!reply.WriteString(bundleName)) {
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetRootMainWindowId(MessageParcel &data, MessageParcel &reply)
{
    TLOGI(WmsLogTag::WMS_MAIN, "call");
    int32_t persistentId = INVALID_WINDOW_ID;
    if (!data.ReadInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_MAIN, "Failed to readInt32 windowId");
        return ERR_INVALID_DATA;
    }
    int32_t hostWindowId = INVALID_WINDOW_ID;
    WMError errCode = GetRootMainWindowId(persistentId, hostWindowId);
    if (errCode != WMError::WM_OK) {
        TLOGE(WmsLogTag::WMS_MAIN, "Failed to GetRootMainWindowId(%{public}d)", hostWindowId);
        return ERR_INVALID_DATA;
    }
    if (!reply.WriteInt32(hostWindowId)) {
        TLOGE(WmsLogTag::WMS_MAIN, "Failed to WriteInt32 hostWindowId");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int SceneSessionManagerLiteStub::HandleGetAccessibilityWindowInfo(MessageParcel& data, MessageParcel& reply)
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

int SceneSessionManagerLiteStub::HandleNotifyAppUseControlList(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LIFE, "In!");
    uint8_t controlType = 0;
    if (!data.ReadUint8(controlType) || controlType <= static_cast<uint8_t>(ControlAppType::CONTROL_APP_TYPE_BEGIN) ||
        controlType >= static_cast<uint8_t>(ControlAppType::CONTROL_APP_TYPE_END)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Failed to read controlType");
        return ERR_INVALID_DATA;
    }
    ControlAppType type = static_cast<ControlAppType>(controlType);

    int32_t userId;
    if (!data.ReadInt32(userId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Failed to read userId");
        return ERR_INVALID_DATA;
    }

    int32_t size = 0;
    if (!data.ReadInt32(size) || size < 0 || size > MAX_CONTROL_APP_INFO) {
        TLOGE(WmsLogTag::WMS_LIFE, "Read controlList size failed");
        return ERR_INVALID_DATA;
    }
    TLOGD(WmsLogTag::WMS_LIFE, "app control list size: %{public}d", size);
    std::vector<ControlAppInfo> controlList;
    controlList.resize(size);
    for (int32_t i = 0; i < size; i++) {
        if (!(data.ReadString(controlList[i].bundleName_) &&
              data.ReadInt32(controlList[i].appIndex_) &&
              data.ReadBool(controlList[i].isNeedControl_))) {
            TLOGE(WmsLogTag::WMS_LIFE, "Read controlList failed");
            return ERR_INVALID_DATA;
        }
    }

    WSError ret = NotifyAppUseControlList(type, userId, controlList);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}
} // namespace OHOS::Rosen
