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

#include "session/host/include/zidl/session_proxy.h"

#include "ability_start_setting.h"
#include <ipc_types.h>
#include <message_option.h>
#include <ui/rs_surface_node.h>

#include "accessibility_event_info_parcel.h"
#include "want.h"

#include "session/host/include/zidl/session_ipc_interface_code.h"
#include "window_manager_hilog.h"
namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionProxy" };
} // namespace

WSError SessionProxy::Foreground(sptr<WindowSessionProperty> property)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[WMSCom] WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (property) {
        if (!data.WriteBool(true) || !data.WriteParcelable(property.GetRefPtr())) {
            WLOGFE("[WMSCom] Write property failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            WLOGFE("[WMSCom] Write property failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_FOREGROUND),
        data, reply, option) != ERR_NONE) {
        WLOGFE("[WMSCom] SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::Background()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[WMSCom] WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKGROUND),
        data, reply, option) != ERR_NONE) {
        WLOGFE("[WMSCom] SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::Show(sptr<WindowSessionProperty> property)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (property) {
        if (!data.WriteBool(true) || !data.WriteParcelable(property.GetRefPtr())) {
            WLOGFE("Write property failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            WLOGFE("Write property failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SHOW),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::Hide()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_HIDE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::Disconnect()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DISCONNECT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
    sptr<WindowSessionProperty> property, sptr<IRemoteObject> token, int32_t pid, int32_t uid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(sessionStage->AsObject())) {
        WLOGFE("Write ISessionStage failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(eventChannel->AsObject())) {
        WLOGFE("Write IWindowEventChannel failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!surfaceNode->Marshalling(data)) {
        WLOGFE("Write surfaceNode failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (property) {
        if (!data.WriteBool(true) || !data.WriteParcelable(property.GetRefPtr())) {
            WLOGFE("Write property failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            WLOGFE("Write property failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }
    if (token != nullptr) {
        if (!data.WriteRemoteObject(token)) {
            WLOGFE("Write abilityToken failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_CONNECT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<SystemSessionConfig> config = reply.ReadParcelable<SystemSessionConfig>();
    if (config) {
        systemConfig = *config;
    }
    if (property) {
        property->SetPersistentId(reply.ReadInt32());
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::PendingSessionActivation(sptr<AAFwk::SessionInfo> abilitySessionInfo)
{
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is null");
        return WSError::WS_ERROR_INVALID_SESSION;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!WriteAbilitySessionInfoBasic(data, abilitySessionInfo)) {
        WLOGFE("WriteInterfaceToken or other param failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (abilitySessionInfo->callerToken) {
        if (!data.WriteBool(true) || !data.WriteRemoteObject(abilitySessionInfo->callerToken)) {
            WLOGFE("Write callerToken info failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            WLOGFE("Write has not callerToken info failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }
    if (abilitySessionInfo->startSetting) {
        if (!data.WriteBool(true) || !data.WriteParcelable(abilitySessionInfo->startSetting.get())) {
            WLOGFE("Write startSetting failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            WLOGFE("Write has not startSetting failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_ACTIVE_PENDING_SESSION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

bool SessionProxy::WriteAbilitySessionInfoBasic(MessageParcel& data, sptr<AAFwk::SessionInfo> abilitySessionInfo)
{
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is null");
        return false;
    }
    if (!data.WriteInterfaceToken(GetDescriptor()) ||
        !(data.WriteParcelable(&(abilitySessionInfo->want))) ||
        !data.WriteInt32(abilitySessionInfo->requestCode) ||
        !(data.WriteInt32(abilitySessionInfo->persistentId)) ||
        !(data.WriteInt32(static_cast<uint32_t>(abilitySessionInfo->state))) ||
        !(data.WriteInt64(abilitySessionInfo->uiAbilityId)) ||
        !data.WriteInt32(abilitySessionInfo->callingTokenId) ||
        !data.WriteBool(abilitySessionInfo->reuse)) {
        return false;
    }
    return true;
}

WSError SessionProxy::TerminateSession(const sptr<AAFwk::SessionInfo> abilitySessionInfo)
{
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is null");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteParcelable(&(abilitySessionInfo->want))) {
        WLOGFE("Write want info failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (abilitySessionInfo->callerToken) {
        if (!data.WriteBool(true) || !data.WriteRemoteObject(abilitySessionInfo->callerToken)) {
            WLOGFE("Write ability info failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            WLOGFE("Write ability info failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }
    if (!data.WriteInt32(abilitySessionInfo->resultCode)) {
        WLOGFE("Write resultCode info failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TERMINATE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::NotifySessionException(const sptr<AAFwk::SessionInfo> abilitySessionInfo)
{
    if (abilitySessionInfo == nullptr) {
        WLOGFE("abilitySessionInfo is null");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteParcelable(&(abilitySessionInfo->want))) {
        WLOGFE("Write want info failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (abilitySessionInfo->callerToken) {
        if (!data.WriteBool(true) || !data.WriteRemoteObject(abilitySessionInfo->callerToken)) {
            WLOGFE("Write ability info failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            WLOGFE("Write ability info failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }
    if (!data.WriteInt32(abilitySessionInfo->persistentId)) {
        WLOGFE("Write persistentId info failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(abilitySessionInfo->errorCode)) {
        WLOGFE("Write erroCode info failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteString(abilitySessionInfo->errorReason)) {
        WLOGFE("Write erroCode info failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_EXCEPTION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::UpdateActiveStatus(bool isActive)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!(data.WriteBool(isActive))) {
        WLOGFE("Write active status failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_ACTIVE_STATUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::OnSessionEvent(SessionEvent event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!(data.WriteUint32(static_cast<uint32_t>(event)))) {
        WLOGFE("Write event id failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SESSION_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason)
{
    WLOGFI("UpdateSessionRect [%{public}d, %{public}d, %{public}u, %{public}u]", rect.posX_, rect.posY_,
        rect.width_, rect.height_);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!((data.WriteInt32(static_cast<int32_t>(rect.posX_))) &&
        (data.WriteInt32(static_cast<int32_t>(rect.posY_))) &&
        (data.WriteUint32(static_cast<uint32_t>(rect.width_))) &&
        (data.WriteUint32(static_cast<uint32_t>(rect.height_))))) {
        WLOGFE("Write rect failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write SessionSizeChangeReason failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_RECT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::RaiseToAppTop()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_TO_APP_TOP),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::RaiseAboveTarget(int32_t subWindowId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(subWindowId)) {
        WLOGFE("Write subWindowId failed");
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_ABOVE_TARGET),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::OnNeedAvoid(bool status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!(data.WriteUint32(static_cast<uint32_t>(status)))) {
        WLOGFE("Write status failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NEED_AVOID),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

AvoidArea SessionProxy::GetAvoidAreaByType(AvoidAreaType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    AvoidArea avoidArea;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return avoidArea;
    }
    if (!(data.WriteUint32(static_cast<uint32_t>(type)))) {
        WLOGFE("Write type failed");
        return avoidArea;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_AVOID_AREA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return avoidArea;
    }
    sptr<AvoidArea> area = reply.ReadParcelable<AvoidArea>();
    if (area == nullptr) {
        return avoidArea;
    }
    return *area;
}

WSError SessionProxy::RequestSessionBack(bool needMoveToBackground)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(needMoveToBackground)) {
        WLOGFE("Write needMoveToBackground failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKPRESSED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::MarkProcessed(int32_t eventId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(eventId)) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_MARK_PROCESSED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError OHOS::Rosen::SessionProxy::SetGlobalMaximizeMode(MaximizeMode mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(mode))) {
        WLOGFE("Write uint32_t failed");
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_MAXIMIZE_MODE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::GetGlobalMaximizeMode(MaximizeMode& mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_MAXIMIZE_MODE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    mode = static_cast<MaximizeMode>(reply.ReadUint32());
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::SetSessionProperty(const sptr<WindowSessionProperty>& property)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteParcelable(property.GetRefPtr())) {
        WLOGFE("Write property failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_SESSION_PROPERTY),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::SetAspectRatio(float ratio)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteFloat(ratio)) {
        WLOGFE("Write ratio failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_ASPECT_RATIO),
                              data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::UpdateWindowSceneAfterCustomAnimation(bool isAdd)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(isAdd)) {
        WLOGFE("Write isAdd failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_CUSTOM_ANIMATION),
                              data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::TransferAbilityResult(uint32_t resultCode, const AAFwk::Want& want)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(resultCode)) {
        WLOGFE("resultCode write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteParcelable(&want)) {
        WLOGFE("want write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_ABILITY_RESULT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::TransferExtensionData(const AAFwk::WantParams& wantParams)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteParcelable(&wantParams)) {
        WLOGFE("wantParams write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_EXTENSION_DATA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

void SessionProxy::NotifyRemoteReady()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_REMOTE_READY),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void SessionProxy::NotifySyncOn()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_SYNC_ON),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void SessionProxy::NotifyAsyncOn()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_ASYNC_ON),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void SessionProxy::NotifyExtensionDied()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_DIED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

WSError SessionProxy::UpdateWindowAnimationFlag(bool needDefaultAnimationFlag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(needDefaultAnimationFlag)) {
        WLOGFE("wantParams write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_WINDOW_ANIMATION_FLAG),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::TransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
    const std::vector<int32_t>& uiExtensionIdLevelVec)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    AccessibilityEventInfoParcel infoParcel(info);
    if (!data.WriteParcelable(&infoParcel)) {
        WLOGFE("infoParcel write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32Vector(uiExtensionIdLevelVec)) {
        WLOGFE("idVec write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_REPORT_ACCESSIBILITY_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return WSError::WS_OK;
}

void SessionProxy::NotifyPiPWindowPrepareClose()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("writeInterfaceToken failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_PIP_WINDOW_PREPARE_CLOSE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

WSError SessionProxy::UpdatePiPRect(const uint32_t width, const uint32_t height, PiPRectUpdateReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("writeInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(width)) {
        WLOGFE("width write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(height)) {
        WLOGFE("height write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(static_cast<int32_t>(reason))) {
        WLOGFE("reason write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_PIP_RECT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::RecoveryPullPiPMainWindow(int32_t persistentId, const Rect& rect)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("writeInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(rect.posX_)) {
        WLOGFE("Write posX_ failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(rect.posY_)) {
        WLOGFE("Write posY_ failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(rect.width_)) {
        WLOGFE("Write width_ failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(rect.height_)) {
        WLOGFE("Write height_ failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RECOVERY_PULL_PIP_MAIN_WINDOW),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return WSError::WS_OK;
}
} // namespace OHOS::Rosen
