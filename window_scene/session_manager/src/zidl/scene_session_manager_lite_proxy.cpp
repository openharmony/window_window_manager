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

#include "session_manager/include/zidl/scene_session_manager_lite_proxy.h"

#include <ipc_types.h>
#include <message_option.h>
#include <message_parcel.h>


#include "marshalling_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr int32_t CYCLE_LIMIT = 1000;
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManagerLiteProxy"};
}

WSError SceneSessionManagerLiteProxy::SetSessionLabel(const sptr<IRemoteObject> &token, const std::string &label)
{
    WLOGFD("run SceneSessionManagerLiteProxy::SetSessionLabel");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(token)) {
        WLOGFE("Write token failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteString(label)) {
        WLOGFE("Write label failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_SET_SESSION_LABEL),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::SetSessionIcon(const sptr<IRemoteObject> &token,
    const std::shared_ptr<Media::PixelMap> &icon)
{
    WLOGFD("run SceneSessionManagerLiteProxy::SetSessionIcon");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(token)) {
        WLOGFE("Write token failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteParcelable(icon.get())) {
        WLOGFE("Write icon failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_SET_SESSION_ICON),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::IsValidSessionIds(
    const std::vector<int32_t> &sessionIds, std::vector<bool> &results)
{
    WLOGFD("run SceneSessionManagerLiteProxy::IsValidSessionIds");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32Vector(sessionIds)) {
        WLOGFE("Write sessionIds failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_IS_VALID_SESSION_IDS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    reply.ReadBoolVector(&results);
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::PendingSessionToForeground(const sptr<IRemoteObject> &token)
{
    WLOGFD("run SceneSessionManagerLiteProxy::PendingSessionToForeground");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteRemoteObject(token)) {
        WLOGFE("Write token failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerLiteMessage::TRANS_ID_PENDING_SESSION_TO_FOREGROUND),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject> &token)
{
    WLOGFD("run SceneSessionManagerLiteProxy::PendingSessionToBackgroundForDelegator");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteRemoteObject(token)) {
        WLOGFE("Write token failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerLiteMessage::TRANS_ID_PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::RegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFD("run SceneSessionManagerLiteProxy::RegisterSessionListener");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (listener == nullptr) {
        WLOGFE("register mission listener, listener is nullptr");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(listener->AsObject())) {
        WLOGFE("write mission listener failed when register mission listener.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerLiteMessage::TRANS_ID_REGISTER_SESSION_LISTENER),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::UnRegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFD("run SceneSessionManagerLiteProxy::UnRegisterSessionListener");
    if (listener == nullptr) {
        WLOGFE("unregister mission listener, listener is nullptr");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(listener->AsObject())) {
        WLOGFE("write mission listener failed when unregister mission listener.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(
        static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_UNREGISTER_SESSION_LISTENER),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::GetSessionInfos(const std::string& deviceId, int32_t numMax,
    std::vector<SessionInfoBean>& sessionInfos)
{
    WLOGFD("run SceneSessionManagerLiteProxy::GetSessionInfos");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteString16(Str8ToStr16(deviceId))) {
        WLOGFE("GetSessionInfos write deviceId failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(numMax)) {
        WLOGFE("GetSessionInfos numMax write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_MISSION_INFOS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    WSError error = GetParcelableInfos(reply, sessionInfos);
    if (error != WSError::WS_OK) {
        WLOGFE("GetSessionInfos error");
        return error;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::GetSessionInfo(const std::string& deviceId, int32_t persistentId,
    SessionInfoBean& sessionInfo)
{
    WLOGFD("run SceneSessionManagerLiteProxy::GetSessionInfo");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteString16(Str8ToStr16(deviceId))) {
        WLOGFE("GetSessionInfo write deviceId failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        WLOGFE("GetSessionInfo write persistentId failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_MISSION_INFO_BY_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    std::unique_ptr<SessionInfoBean> info(reply.ReadParcelable<SessionInfoBean>());
    if (info == nullptr) {
        WLOGFE("read missioninfo failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sessionInfo = *info;
    return static_cast<WSError>(reply.ReadInt32());
}

template<typename T>
WSError SceneSessionManagerLiteProxy::GetParcelableInfos(MessageParcel& reply, std::vector<T>& parcelableInfos)
{
    int32_t infoSize = reply.ReadInt32();
    if (infoSize > CYCLE_LIMIT) {
        WLOGFE("infoSize is too large");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    for (int32_t i = 0; i < infoSize; i++) {
        std::unique_ptr<T> info(reply.ReadParcelable<T>());
        if (!info) {
            WLOGFE("Read Parcelable infos failed.");
            return WSError::WS_ERROR_IPC_FAILED;
        }
        parcelableInfos.emplace_back(*info);
    }
    return WSError::WS_OK;
}

WSError SceneSessionManagerLiteProxy::TerminateSessionNew(const sptr<AAFwk::SessionInfo> abilitySessionInfo,
    bool needStartCaller)
{
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is null");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteParcelable(abilitySessionInfo)) {
        WLOGFE("write abilitySessionInfo failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(needStartCaller)) {
        WLOGFE("Write needStartCaller failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_TERMINATE_SESSION_NEW),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::GetFocusSessionToken(sptr<IRemoteObject> &token)
{
    WLOGFD("run SceneSessionManagerLiteProxy::GetFocusSessionToken");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_FOCUS_SESSION_TOKEN),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    token = reply.ReadRemoteObject();
    if (token == nullptr) {
        WLOGFE("get token nullptr.");
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::GetSessionSnapshot(const std::string& deviceId, int32_t persistentId,
    SessionSnapshot& snapshot, bool isLowResolution)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteString16(Str8ToStr16(deviceId))) {
        WLOGFE("Write deviceId failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        WLOGFE("Write persistentId failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (!data.WriteBool(isLowResolution)) {
        WLOGFE("Write isLowResolution failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_SESSION_SNAPSHOT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    std::unique_ptr<SessionSnapshot> info(reply.ReadParcelable<SessionSnapshot>());
    if (info) {
        snapshot = *info;
    } else {
        WLOGFW("Read SessionSnapshot is null.");
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::SetSessionContinueState(const sptr<IRemoteObject> &token,
    const ContinueState& continueState)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteRemoteObject(token)) {
        WLOGFE("Write token failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(static_cast<int32_t>(continueState))) {
        WLOGFE("Write continueState failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerLiteMessage::TRANS_ID_SET_SESSION_CONTINUE_STATE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::LockSession(int32_t sessionId)
{
    WLOGFD("run SceneSessionManagerLiteProxy::LockSession");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interface token failed.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteInt32(sessionId)) {
        WLOGFE("Write persistentId failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_LOCK_SESSION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::UnlockSession(int32_t sessionId)
{
    WLOGFD("run SceneSessionManagerLiteProxy::UnlockSession");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interface token failed.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteInt32(sessionId)) {
        WLOGFE("Write persistentId failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_UNLOCK_SESSION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::MoveSessionsToForeground(const std::vector<std::int32_t>& sessionIds,
    int32_t topSessionId)
{
    WLOGFD("run SceneSessionManagerLiteProxy::MoveSessionsToForeground");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteInt32Vector(sessionIds)) {
        WLOGFE("Write sessionIds failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteInt32(topSessionId)) {
        WLOGFE("Write topSessionId failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerLiteMessage::TRANS_ID_MOVE_MISSIONS_TO_FOREGROUND),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::MoveSessionsToBackground(const std::vector<std::int32_t>& sessionIds,
    std::vector<int32_t>& result)
{
    WLOGFD("run SceneSessionManagerLiteProxy::MoveSessionsToBackground");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteInt32Vector(sessionIds)) {
        WLOGFE("Write sessionIds failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteInt32Vector(result)) {
        WLOGFE("Write result failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerLiteMessage::TRANS_ID_MOVE_MISSIONS_TO_BACKGROUND),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    reply.ReadInt32Vector(&result);
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::ClearSession(int32_t persistentId)
{
    WLOGFD("run SceneSessionManagerLiteProxy::ClearSession");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("ClearSession WriteInterfaceToken failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (!data.WriteInt32(persistentId)) {
        WLOGFE("Write persistentId failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_CLEAR_SESSION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerLiteProxy::ClearAllSessions()
{
    WLOGFD("run SceneSessionManagerLiteProxy::ClearSession");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("ClearAllSessions WriteInterfaceToken failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_CLEAR_ALL_SESSIONS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

void SceneSessionManagerLiteProxy::GetFocusWindowInfo(FocusChangeInfo& focusInfo)
{
    WLOGFI("get focus Winow info lite proxy");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerLiteMessage::TRANS_ID_GET_FOCUS_SESSION_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
    sptr<FocusChangeInfo> info = reply.ReadParcelable<FocusChangeInfo>();
    if (info) {
        focusInfo = *info;
    } else {
        WLOGFE("info is null.");
    }
}

} // namespace OHOS::Rosen
