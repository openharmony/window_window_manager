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

#include "parcel/accessibility_event_info_parcel.h"
#include "process_options.h"
#include "want.h"
#include "key_event.h"
#include "pointer_event.h"
#include "process_options.h"
#include "session/host/include/zidl/session_ipc_interface_code.h"
#include "window_manager_hilog.h"
namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionProxy" };
} // namespace

WSError SessionProxy::Foreground(sptr<WindowSessionProperty> property, bool isFromClient)
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
    if (!data.WriteBool(isFromClient)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write isFromClient failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[WMSCom] remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_FOREGROUND),
        data, reply, option) != ERR_NONE) {
        WLOGFE("[WMSCom] SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::Background(bool isFromClient)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("[WMSCom] WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(isFromClient)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write isFromClient failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("[WMSCom] remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKGROUND),
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

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SHOW),
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

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_HIDE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::Disconnect(bool isFromClient)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteBool(isFromClient)) {
        WLOGFE("Write isFromClient failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DISCONNECT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::Connect(const sptr<ISessionStage>& sessionStage, const sptr<IWindowEventChannel>& eventChannel,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, SystemSessionConfig& systemConfig,
    sptr<WindowSessionProperty> property, sptr<IRemoteObject> token,
    const std::string& identityToken)
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
    if (!surfaceNode || !surfaceNode->Marshalling(data)) {
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
    if (!data.WriteString(identityToken)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write identityToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_CONNECT),
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
        property->SetDisplayId(reply.ReadUint64());
        bool needUpdate = reply.ReadBool();
        property->SetIsNeedUpdateWindowMode(needUpdate);
        if (needUpdate) {
            property->SetWindowMode(static_cast<WindowMode>(reply.ReadUint32()));
        }
        Rect preRect = property->GetWindowRect();
        Rect rect = { reply.ReadInt32(), reply.ReadInt32(), reply.ReadUint32(), reply.ReadUint32() };
        TLOGI(WmsLogTag::WMS_LAYOUT, "updateRect when connect."
            "preRect:[%{public}d,%{public}d,%{public}u,%{public}u]"
            "rect:[%{public}d,%{public}d,%{public}u,%{public}u]",
            preRect.posX_, preRect.posY_, preRect.width_, preRect.height_,
            rect.posX_, rect.posY_, rect.width_, rect.height_);
        if (preRect.IsUninitializedRect() && !rect.IsUninitializedRect()) {
            property->SetWindowRect(rect);
        }
        property->SetCollaboratorType(reply.ReadInt32());
        property->SetFullScreenStart(reply.ReadBool());
        property->SetCompatibleModeInPc(reply.ReadBool());
        property->SetCompatibleWindowSizeInPc(reply.ReadInt32(), reply.ReadInt32(),
                                              reply.ReadInt32(), reply.ReadInt32());
        property->SetIsAppSupportPhoneInPc(reply.ReadBool());
        property->SetIsSupportDragInPcCompatibleMode(reply.ReadBool());
        property->SetIsPcAppInPad(reply.ReadBool());
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::DrawingCompleted()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_LIFE, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DRAWING_COMPLETED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_LIFE, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SessionProxy::ChangeSessionVisibilityWithStatusBar(sptr<AAFwk::SessionInfo> abilitySessionInfo, bool visible)
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
    data.WriteBool(visible);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
        SessionInterfaceCode::TRANS_ID_CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
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
    if (!data.WriteBool(abilitySessionInfo->canStartAbilityFromBackground)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write canStartAbilityFromBackground failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(abilitySessionInfo->isAtomicService) || !data.WriteBool(abilitySessionInfo->isBackTransition)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write isAtomicService or isBackTransition failed");
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_ACTIVE_PENDING_SESSION),
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
        !data.WriteBool(abilitySessionInfo->reuse) ||
        !data.WriteParcelable(abilitySessionInfo->processOptions.get())) {
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TERMINATE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::NotifySessionException(const sptr<AAFwk::SessionInfo> abilitySessionInfo, bool needRemoveSession)
{
    if (abilitySessionInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "abilitySessionInfo is null");
        return WSError::WS_ERROR_INVALID_SESSION;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_LIFE, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteParcelable(&(abilitySessionInfo->want))) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write want info failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (abilitySessionInfo->callerToken) {
        if (!data.WriteBool(true) || !data.WriteRemoteObject(abilitySessionInfo->callerToken)) {
            TLOGE(WmsLogTag::WMS_LIFE, "Write ability info failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            TLOGE(WmsLogTag::WMS_LIFE, "Write ability info failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }
    if (!data.WriteInt32(abilitySessionInfo->persistentId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write persistentId info failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(abilitySessionInfo->errorCode) ||
        !data.WriteString(abilitySessionInfo->errorReason)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write error info failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteString(abilitySessionInfo->identityToken)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write identity token info failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_EXCEPTION),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_LIFE, "SendRequest failed");
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SESSION_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::OnSystemSessionEvent(SessionEvent event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!(data.WriteInt32(static_cast<int32_t>(event)))) {
        WLOGFE("Write event id failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<int32_t>(SessionInterfaceCode::TRANS_ID_SYSTEM_SESSION_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::OnLayoutFullScreenChange(bool isLayoutFullScreen)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(isLayoutFullScreen)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Write isLayoutFullScreen failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_LAYOUT_FULL_SCREEN_CHANGE),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::UpdateSessionRect(const WSRect& rect, const SizeChangeReason& reason, bool isGlobal)
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

    if (!data.WriteBool(isGlobal)) {
        WLOGFE("Write bool failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_RECT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

/** @note @window.hierarchy */
WSError SessionProxy::RaiseToAppTop()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_TO_APP_TOP),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::NotifyFrameLayoutFinishFromApp(bool notifyListener, const WSRect& rect)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteBool(notifyListener)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Write notifyListener failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteInt32(rect.posX_) || !data.WriteInt32(rect.posY_) ||
        !data.WriteInt32(rect.width_) || !data.WriteInt32(rect.height_)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Write rect failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_FRAME_LAYOUT_FINISH),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return WSError::WS_OK;
}

/** @note @window.hierarchy */
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_ABOVE_TARGET),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::RaiseAppMainWindowToTop()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_APP_MAIN_WINDOW),
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NEED_AVOID),
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return avoidArea;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_AVOID_AREA),
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKPRESSED),
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_MARK_PROCESSED),
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_MAXIMIZE_MODE),
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_MAXIMIZE_MODE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    mode = static_cast<MaximizeMode>(reply.ReadUint32());
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_ASPECT_RATIO),
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_CUSTOM_ANIMATION),
                            data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::SetLandscapeMultiWindow(bool isLandscapeMultiWindow)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(isLandscapeMultiWindow)) {
        TLOGE(WmsLogTag::WMS_MULTI_WINDOW, "Write isLandscapeMultiWindow failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_LANDSCAPE_MULTI_WINDOW),
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_ABILITY_RESULT),
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_EXTENSION_DATA),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_SYNC_ON),
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_ASYNC_ON),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

void SessionProxy::NotifyExtensionDied()
{
    TLOGI(WmsLogTag::WMS_UIEXT, "NotifyExtensionDied called.");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_UIEXT, "WriteInterfaceToken failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "remote is null");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_DIED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_UIEXT, "SendRequest failed");
        return;
    }
}

void SessionProxy::NotifyExtensionTimeout(int32_t errorCode)
{
    TLOGI(WmsLogTag::WMS_UIEXT, "NotifyExtensionTimeout(errorCode:%{public}d) called.", errorCode);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_UIEXT, "WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteInt32(static_cast<int32_t>(errorCode))) {
        TLOGE(WmsLogTag::WMS_UIEXT, "errorCode write failed.");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_UIEXT, "remote is null");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_TIMEOUT),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_UIEXT, "SendRequest failed");
    }
}

void SessionProxy::TriggerBindModalUIExtension()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRIGGER_BIND_MODAL_UI_EXTENSION),
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_WINDOW_ANIMATION_FLAG),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::TransferAccessibilityEvent(const Accessibility::AccessibilityEventInfo& info,
    int64_t uiExtensionIdLevel)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    Accessibility::AccessibilityEventInfoParcel infoParcel(info);
    if (!data.WriteParcelable(&infoParcel)) {
        WLOGFE("infoParcel write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt64(uiExtensionIdLevel)) {
        WLOGFE("idVec write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_REPORT_ACCESSIBILITY_EVENT),
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
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_PIP_WINDOW_PREPARE_CLOSE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

WSError SessionProxy::UpdatePiPRect(const Rect& rect, SizeChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("writeInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(rect.posX_)) {
        WLOGFE("write posX_ failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(rect.posY_)) {
        WLOGFE("write posY_ failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(rect.width_)) {
        WLOGFE("write width_ failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(rect.height_)) {
        WLOGFE("write height_ failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(static_cast<int32_t>(reason))) {
        WLOGFE("reason write failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_PIP_RECT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::UpdatePiPControlStatus(WsPiPControlType controlType, WsPiPControlStatus status)
{
    TLOGI(WmsLogTag::WMS_PIP, "controlType:%{public}u, status:%{public}d", controlType, status);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_PIP, "writeInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(controlType))) {
        TLOGE(WmsLogTag::WMS_PIP, "Write controlType failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(static_cast<int32_t>(status))) {
        TLOGE(WmsLogTag::WMS_PIP, "write status failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_PIP, "remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_PIP_CONTROL_STATUS),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_PIP, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::ProcessPointDownSession(int32_t posX, int32_t posY)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("writeInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(posX)) {
        WLOGFE("width poX failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(posY)) {
        WLOGFE("width posY failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_PROCESS_POINT_DOWN_SESSION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SessionProxy::SendPointEventForMoveDrag(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("writeInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!pointerEvent->WriteToParcel(data)) {
        WLOGFE("width pointerEvent failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SEND_POINTEREVENT_FOR_MOVE_DRAG),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WMError SessionProxy::SetSystemWindowEnableDrag(bool enableDrag)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "enableDrag: %{public}d", enableDrag);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(enableDrag)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "write enableDrag failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "remote is null");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_SYSTEM_DRAG_ENABLE),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WSError SessionProxy::GetStartMoveFlag(bool& isMoving)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DEFAULT, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_START_MOVE_FLAG),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DEFAULT, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    isMoving = reply.ReadBool();
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::UpdateRectChangeListenerRegistered(bool isRegister)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(isRegister)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "write isRegister failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_RECTCHANGE_LISTENER_REGISTERED),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SessionProxy::SetKeyboardSessionGravity(SessionGravity gravity, uint32_t percent)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(gravity))) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Write gravity failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(percent)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Write percent failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_KEYBOARD_SESSION_GRAVITY),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

void SessionProxy::SetCallingSessionId(const uint32_t callingSessionId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "writeInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(callingSessionId)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Write callingSessionId failed.");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "remote is null");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_CALLING_SESSION_ID),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "SendRequest failed");
        return;
    }
}

void SessionProxy::SetCustomDecorHeight(int32_t height)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "writeInterfaceToken failed");
        return;
    }
    if (!data.WriteInt32(height)) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "Write height failed.");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "remote is null");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_CUSTOM_DECOR_HEIGHT),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_LAYOUT, "SendRequest failed");
        return;
    }
}

WSError SessionProxy::AdjustKeyboardLayout(const KeyboardLayoutParams& params)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteParcelable(&params)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "keyboard layout params write failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_ADJUST_KEYBOARD_LAYOUT),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WMError SessionProxy::UpdateSessionPropertyByAction(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (action == WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON) {
        option.SetFlags(MessageOption::TF_ASYNC);
    }
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DEFAULT, "WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(action))) {
        TLOGE(WmsLogTag::DEFAULT, "Write PropertyChangeAction failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (property) {
        if (!data.WriteBool(true) || !property->Write(data, action)) {
            TLOGE(WmsLogTag::DEFAULT, "Write property failed");
            return WMError::WM_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            TLOGE(WmsLogTag::DEFAULT, "Write property failed");
            return WMError::WM_ERROR_IPC_FAILED;
        }
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "remote is null");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
        SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_PROPERTY),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DEFAULT, "SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError SessionProxy::GetAppForceLandscapeConfig(AppForceLandscapeConfig& config)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DEFAULT, "WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "remote is null");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
        SessionInterfaceCode::TRANS_ID_GET_FORCE_LANDSCAPE_CONFIG),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DEFAULT, "SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    sptr<AppForceLandscapeConfig> replyConfig = reply.ReadParcelable<AppForceLandscapeConfig>();
    if (replyConfig) {
        config = *replyConfig;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

int32_t SessionProxy::GetStatusBarHeight()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    int32_t height = 0;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::DEFAULT, "WriteInterfaceToken failed");
        return height;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::DEFAULT, "remote is null");
        return height;
    }
    if (remote->SendRequest(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_STATUSBAR_HEIGHT),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::DEFAULT, "SendRequest failed");
        return height;
    }
    height = reply.ReadInt32();
    return height;
}

WSError SessionProxy::SetDialogSessionBackGestureEnabled(bool isEnabled)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_DIALOG, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(isEnabled)) {
        TLOGE(WmsLogTag::WMS_DIALOG, "Write isEnabled failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        TLOGE(WmsLogTag::WMS_DIALOG, "remote is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(
        static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_DIALOG_SESSION_BACKGESTURE_ENABLE),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_DIALOG, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}
} // namespace OHOS::Rosen
