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

#include "session_manager/include/zidl/scene_session_manager_proxy.h"

#include <ipc_types.h>
#include <message_option.h>
#include <message_parcel.h>
#include <ui/rs_surface_node.h>

#include "marshalling_helper.h"
#include "window_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr int32_t CYCLE_LIMIT = 1000;
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SceneSessionManagerProxy"};
}
WSError SceneSessionManagerProxy::CreateAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, int32_t& persistentId, sptr<ISession>& session,
    SystemSessionConfig& systemConfig, sptr<IRemoteObject> token)
{
    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write InterfaceToken failed!");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(sessionStage->AsObject())) {
        WLOGFE("Write ISessionStage failed!");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(eventChannel->AsObject())) {
        WLOGFE("Write IWindowEventChannel failed!");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!surfaceNode || !surfaceNode->Marshalling(data)) {
        WLOGFE("Write surfaceNode failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (property) {
        if (!data.WriteBool(true) || !data.WriteParcelable(property.GetRefPtr())) {
            return WSError::WS_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }
    if (token != nullptr) {
        if (!data.WriteRemoteObject(token)) {
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_CREATE_AND_CONNECT_SPECIFIC_SESSION), data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    persistentId = reply.ReadInt32();
    sptr<IRemoteObject> sessionObject = reply.ReadRemoteObject();
    if (sessionObject == nullptr) {
        WLOGFE("ReadRemoteObject failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    session = iface_cast<ISession>(sessionObject);
    sptr<SystemSessionConfig> config = reply.ReadParcelable<SystemSessionConfig>();
    if (config) {
        systemConfig = *config;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SceneSessionManagerProxy::RecoverAndConnectSpecificSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<WindowSessionProperty> property, sptr<ISession>& session, sptr<IRemoteObject> token)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write InterfaceToken failed!");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(sessionStage->AsObject())) {
        WLOGFE("Write ISessionStage failed!");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(eventChannel->AsObject())) {
        WLOGFE("Write IWindowEventChannel failed!");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!surfaceNode->Marshalling(data)) {
        WLOGFE("Write surfaceNode failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (property) {
        if (!data.WriteBool(true) || !data.WriteParcelable(property.GetRefPtr())) {
            return WSError::WS_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }
    if (token != nullptr) {
        if (!data.WriteRemoteObject(token)) {
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_RECOVER_AND_CONNECT_SPECIFIC_SESSION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> sessionObject = reply.ReadRemoteObject();
    if (sessionObject == nullptr) {
        WLOGFE("ReadRemoteObject failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    session = iface_cast<ISession>(sessionObject);
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}
WSError SceneSessionManagerProxy::RecoverAndReconnectSceneSession(const sptr<ISessionStage>& sessionStage,
    const sptr<IWindowEventChannel>& eventChannel, const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    sptr<ISession>& session, sptr<WindowSessionProperty> property, sptr<IRemoteObject> token)
{
    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write InterfaceToken failed!");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(sessionStage->AsObject())) {
        WLOGFE("Write ISessionStage failed!");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteRemoteObject(eventChannel->AsObject())) {
        WLOGFE("Write IWindowEventChannel failed!");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!surfaceNode->Marshalling(data)) {
        WLOGFE("Write surfaceNode failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (property) {
        if (!data.WriteBool(true) || !data.WriteParcelable(property.GetRefPtr())) {
            return WSError::WS_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }
    if (token != nullptr) {
        if (!data.WriteRemoteObject(token)) {
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }

    if (Remote()->SendRequest(
        static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_RECOVER_AND_RECONNECT_SCENE_SESSION), data, reply,
        option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> sessionObject = reply.ReadRemoteObject();
    if (sessionObject == nullptr) {
        WLOGFE("ReadRemoteObject failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    session = iface_cast<ISession>(sessionObject);
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SceneSessionManagerProxy::DestroyAndDisconnectSpecificSession(const int32_t persistentId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_LIFE, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write persistentId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_DESTROY_AND_DISCONNECT_SPECIFIC_SESSION),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_LIFE, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SceneSessionManagerProxy::DestroyAndDisconnectSpecificSessionWithDetachCallback(const int32_t persistentId,
    const sptr<IRemoteObject>& callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_LIFE, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write persistentId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (callback == nullptr || !data.WriteRemoteObject(callback)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Write callback failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_DESTROY_AND_DISCONNECT_SPECIFIC_SESSION_WITH_DETACH_CALLBACK),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_LIFE, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WMError SceneSessionManagerProxy::UpdateSessionProperty(const sptr<WindowSessionProperty>& property,
    WSPropertyChangeAction action)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(action))) {
        WLOGFE("Write PropertyChangeAction failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (property) {
        if (!data.WriteBool(true) || !property->Write(data, action)) {
            WLOGFE("Write property failed");
            return WMError::WM_ERROR_IPC_FAILED;
        }
    } else {
        if (!data.WriteBool(false)) {
            WLOGFE("Write property failed");
            return WMError::WM_ERROR_IPC_FAILED;
        }
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_UPDATE_PROPERTY),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError SceneSessionManagerProxy::RequestFocusStatus(int32_t persistentId, bool isFocused, bool byForeground,
    FocusChangeReason reason)
{
    TLOGI(WmsLogTag::WMS_FOCUS, "SceneSessionManagerProxy::RequestFocusStatus id: %{public}d, focusState:\
        %{public}u, byForeground: %{public}d, reason: %{public}d", persistentId, isFocused, byForeground, reason);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        WLOGFE("Write persistentId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(isFocused)) {
        WLOGFE("Write isFocused failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_REQUEST_FOCUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WSError SceneSessionManagerProxy::RaiseWindowToTop(int32_t persistentId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        WLOGFE("Write persistentId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_RAISE_WINDOW_TO_TOP),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SceneSessionManagerProxy::BindDialogSessionTarget(uint64_t persistentId, sptr<IRemoteObject> targetToken)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint64(persistentId)) {
        WLOGFE("Write PropertyChangeAction failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (targetToken != nullptr) {
        if (!data.WriteRemoteObject(targetToken)) {
            WLOGFE("Write targetToken failed");
            return WSError::WS_ERROR_IPC_FAILED;
        }
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_BIND_DIALOG_TARGET),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WSError>(ret);
}

WSError SceneSessionManagerProxy::UpdateSessionAvoidAreaListener(int32_t& persistentId, bool haveListener)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        WLOGFE("Write persistentId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(haveListener)) {
        WLOGFE("Write avoid area listener failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_UPDATE_AVOIDAREA_LISTENER),
        data, reply, option) != ERR_NONE) {
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::UpdateSessionTouchOutsideListener(int32_t& persistentId, bool haveListener)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        WLOGFE("Write persistentId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(haveListener)) {
        WLOGFE("Write avoid area listener failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_UPDATE_TOUCHOUTSIDE_LISTENER),
        data, reply, option) != ERR_NONE) {
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::UpdateSessionWindowVisibilityListener(int32_t persistentId, bool haveListener)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        WLOGFE("Write persistentId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(haveListener)) {
        WLOGFE("Write avoid area listener failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_UPDATE_WINDOW_VISIBILITY_LISTENER),
        data, reply, option) != ERR_NONE) {
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WMError SceneSessionManagerProxy::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    MessageOption option;
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write InterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write type failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteRemoteObject(windowManagerAgent->AsObject())) {
        WLOGFE("Write IWindowManagerAgent failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    return static_cast<WMError>(reply.ReadInt32());
}

WMError SceneSessionManagerProxy::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    MessageParcel reply;
    MessageOption option;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write InterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write type failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteRemoteObject(windowManagerAgent->AsObject())) {
        WLOGFE("Write IWindowManagerAgent failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    return static_cast<WMError>(reply.ReadInt32());
}

WMError SceneSessionManagerProxy::SetGestureNavigaionEnabled(bool enable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write InterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteBool(enable)) {
        WLOGFE("Write enable failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_SET_GESTURE_NAVIGATION_ENABLED), data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

void SceneSessionManagerProxy::GetFocusWindowInfo(FocusChangeInfo& focusInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_FOCUS_SESSION_INFO),
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

WSError SceneSessionManagerProxy::SetSessionLabel(const sptr<IRemoteObject> &token, const std::string &label)
{
    WLOGFI("run SceneSessionManagerProxy::SetSessionLabel");
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

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_SET_SESSION_LABEL),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::SetSessionIcon(const sptr<IRemoteObject> &token,
    const std::shared_ptr<Media::PixelMap> &icon)
{
    WLOGFI("run SceneSessionManagerProxy::SetSessionIcon");
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

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_SET_SESSION_ICON),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::IsValidSessionIds(
    const std::vector<int32_t> &sessionIds, std::vector<bool> &results)
{
    WLOGFI("run SceneSessionManagerProxy::IsValidSessionIds");
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

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_IS_VALID_SESSION_IDS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    reply.ReadBoolVector(&results);
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::RegisterSessionListener(const sptr<ISessionChangeListener> sessionListener)
{
    WLOGFI("run SceneSessionManagerProxy::RegisterSessionListener");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (sessionListener == nullptr) {
        WLOGFE("sessionListener is null");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (!data.WriteRemoteObject(sessionListener->AsObject())) {
        WLOGFE("Write sessionListener failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(
        static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_REGISTER_SESSION_CHANGE_LISTENER), data, reply,
        option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

void SceneSessionManagerProxy::UnregisterSessionListener()
{
    WLOGFI("run SceneSessionManagerProxy::UnregisterSessionListener");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("UnregisterSessionListener WriteInterfaceToken failed");
        return;
    }

    if (Remote()->SendRequest(
        static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UNREGISTER_SESSION_CHANGE_LISTENER), data, reply,
        option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

WMError SceneSessionManagerProxy::GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos)
{
    MessageOption option;
    MessageParcel reply;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write InterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_GET_WINDOW_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!MarshallingHelper::UnmarshallingVectorParcelableObj<AccessibilityWindowInfo>(reply, infos)) {
        WLOGFE("read window info failed.");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    return static_cast<WMError>(reply.ReadUint32());
}

WSError SceneSessionManagerProxy::PendingSessionToForeground(const sptr<IRemoteObject> &token)
{
    WLOGFI("run SceneSessionManagerProxy::PendingSessionToForeground");
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

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_PENDING_SESSION_TO_FOREGROUND),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject> &token)
{
    WLOGFI("run SceneSessionManagerProxy::PendingSessionToBackgroundForDelegator");
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
        SceneSessionManagerMessage::TRANS_ID_PENDING_SESSION_TO_BACKGROUND_FOR_DELEGATOR),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::RegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFI("run SceneSessionManagerProxy::RegisterSessionListener");
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
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_REGISTER_SESSION_LISTENER),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::UnRegisterSessionListener(const sptr<ISessionListener>& listener)
{
    WLOGFI("run SceneSessionManagerProxy::UnRegisterSessionListener");
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
        static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UNREGISTER_SESSION_LISTENER),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::GetSessionInfos(const std::string& deviceId, int32_t numMax,
                                                  std::vector<SessionInfoBean>& sessionInfos)
{
    WLOGFI("run SceneSessionManagerProxy::GetSessionInfos");
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
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_MISSION_INFOS),
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

WSError SceneSessionManagerProxy::GetSessionInfo(const std::string& deviceId, int32_t persistentId,
                                                 SessionInfoBean& sessionInfo)
{
    WLOGFI("run SceneSessionManagerProxy::GetSessionInfo");
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
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_MISSION_INFO_BY_ID),
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

WSError SceneSessionManagerProxy::GetSessionInfoByContinueSessionId(
    const std::string& continueSessionId, SessionInfoBean& sessionInfo)
{
    TLOGD(WmsLogTag::WMS_LIFE, "run query session info");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_LIFE, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteString(continueSessionId)) {
        TLOGE(WmsLogTag::WMS_LIFE, "GetSessionInfoByContinueSessionId write continueSessionId failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_GET_SESSION_INFO_BY_CONTINUE_SESSION_ID),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_LIFE, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<SessionInfoBean> info(reply.ReadParcelable<SessionInfoBean>());
    if (info == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "read sessioninfo failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sessionInfo = *info;
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::DumpSessionAll(std::vector<std::string> &infos)
{
    WLOGFI("run SceneSessionManagerProxy::DumpSessionAll");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("DumpSessionAll write interface token failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_DUMP_SESSION_ALL),
        data, reply, option) != ERR_NONE) {
        WLOGFE("DumpSessionAll sendRequest failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!reply.ReadStringVector(&infos)) {
        WLOGFE("DumpSessionAll read session info failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::DumpSessionWithId(int32_t persistentId, std::vector<std::string> &infos)
{
    WLOGFI("run SceneSessionManagerProxy::DumpSessionWithId");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("DumpSessionWithId write interface token failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        WLOGFE("DumpSessionWithId write persistentId failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_DUMP_SESSION_WITH_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFE("DumpSessionWithId sendRequest failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!reply.ReadStringVector(&infos)) {
        WLOGFE("DumpSessionWithId read session info failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

template<typename T>
WSError SceneSessionManagerProxy::GetParcelableInfos(MessageParcel& reply, std::vector<T>& parcelableInfos)
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

WSError SceneSessionManagerProxy::TerminateSessionNew(const sptr<AAFwk::SessionInfo> abilitySessionInfo,
    bool needStartCaller, bool isFromBroker)
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
    if (!data.WriteBool(isFromBroker)) {
        WLOGFE("Write isFromBroker failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_TERMINATE_SESSION_NEW),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::GetFocusSessionToken(sptr<IRemoteObject> &token)
{
    WLOGFD("run SceneSessionManagerProxy::GetFocusSessionToken");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_FOCUS_SESSION_TOKEN),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    token = reply.ReadRemoteObject();
    if (token == nullptr) {
        WLOGFD("get token nullptr.");
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::GetFocusSessionElement(AppExecFwk::ElementName& element)
{
    WLOGFD("run SceneSessionManagerProxy::GetFocusSessionElement");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_FOCUS_SESSION_ELEMENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    sptr<AppExecFwk::ElementName> ret = reply.ReadParcelable<AppExecFwk::ElementName>();
    if (ret) {
        element = *ret;
    } else {
        WLOGFD("get element null.");
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WMError SceneSessionManagerProxy::CheckWindowId(int32_t windowId, int32_t &pid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Failed to write interfaceToken");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(windowId)) {
        WLOGFE("Failed to write windowId");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("remote is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_CHECK_WINDOW_ID),
        data, reply, option);
    if (ret != ERR_NONE) {
        WLOGFE("Send request failed, ret:%{public}d", ret);
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!reply.ReadInt32(pid)) {
        WLOGFE("Failed to read pid");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    return WMError::WM_OK;
}

WSError SceneSessionManagerProxy::GetSessionDumpInfo(const std::vector<std::string>& params, std::string& info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteStringVector(params)) {
        WLOGFE("Write params failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_SESSION_DUMP_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }

    auto infoSize = reply.ReadUint32();
    if (infoSize != 0) {
        const char* infoPtr = nullptr;
        infoPtr = reinterpret_cast<const char*>(reply.ReadRawData(infoSize));
        info = (infoPtr) ? std::string(infoPtr, infoSize) : "";
    }
    WLOGFD("GetSessionDumpInfo, infoSize: %{public}d", infoSize);
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::GetSessionSnapshot(const std::string& deviceId, int32_t persistentId,
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

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_SESSION_SNAPSHOT),
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

WSError SceneSessionManagerProxy::SetSessionContinueState(const sptr<IRemoteObject> &token,
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
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_SET_SESSION_CONTINUE_STATE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

void SceneSessionManagerProxy::NotifyDumpInfoResult(const std::vector<std::string>& info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken pfailed");
        return;
    }
    uint32_t vectorSize = static_cast<uint32_t>(info.size());
    if (!data.WriteUint32(vectorSize)) {
        WLOGFE("Write vector size failed");
        return;
    }
    for (const auto& elem : info) {
        const char* curInfo = elem.c_str();
        uint32_t curSize = static_cast<uint32_t>(strlen(curInfo));
        WLOGFD("NotifyDumpInfoResult infoSize: %{public}u", curSize);
        if (!data.WriteUint32(curSize)) {
            WLOGFE("Write info size failed");
            return;
        }
        if (curSize != 0) {
            if (!data.WriteRawData(curInfo, curSize)) {
                WLOGFE("Write info failed");
                return;
            }
        }
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_NOTIFY_DUMP_INFO_RESULT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return;
    }
}

WSError SceneSessionManagerProxy::LockSession(int32_t sessionId)
{
    WLOGFI("run SceneSessionManagerProxy::LockSession");
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
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_LOCK_SESSION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::UnlockSession(int32_t sessionId)
{
    WLOGFI("run SceneSessionManagerProxy::UnlockSession");
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
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UNLOCK_SESSION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::MoveSessionsToForeground(const std::vector<std::int32_t>& sessionIds,
    int32_t topSessionId)
{
    WLOGFI("run SceneSessionManagerProxy::MoveSessionsToForeground");
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
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_MOVE_MISSIONS_TO_FOREGROUND),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::MoveSessionsToBackground(const std::vector<std::int32_t>& sessionIds,
    std::vector<int32_t>& result)
{
    WLOGFI("run SceneSessionManagerProxy::MoveSessionsToBackground");
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
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_MOVE_MISSIONS_TO_BACKGROUND),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    reply.ReadInt32Vector(&result);
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::ClearSession(int32_t persistentId)
{
    WLOGFI("run SceneSessionManagerProxy::ClearSession");
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

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_CLEAR_SESSION),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::ClearAllSessions()
{
    WLOGFI("run SceneSessionManagerProxy::ClearSession");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("ClearAllSessions WriteInterfaceToken failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_CLEAR_ALL_SESSIONS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::RegisterIAbilityManagerCollaborator(int32_t type,
    const sptr<AAFwk::IAbilityManagerCollaborator> &impl)
{
    WLOGFI("run SceneSessionManagerProxy::RegisterIAbilityManagerCollaborator");
    if (!impl) {
        WLOGFE("impl is nullptr");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interface token failed.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteInt32(type)) {
        WLOGFE("type write failed.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteRemoteObject(impl->AsObject())) {
        WLOGFE("impl write failed.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_REGISTER_COLLABORATOR),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::UnregisterIAbilityManagerCollaborator(int32_t type)
{
    WLOGFI("run SceneSessionManagerProxy::UnregisterIAbilityManagerCollaborator");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interface token failed.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteInt32(type)) {
        WLOGFE("type write failed.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UNREGISTER_COLLABORATOR),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::NotifyWindowExtensionVisibilityChange(int32_t pid, int32_t uid, bool visible)
{
    WLOGFI("run SceneSessionManagerProxy::NotifyWindowExtensionVisibilityChange");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interface token failed.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (!data.WriteInt32(pid)) {
        WLOGFE("pid write failed.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (!data.WriteInt32(uid)) {
        WLOGFE("uid write failed.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }
    if (!data.WriteBool(visible)) {
        WLOGFE("pid write failed.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_NOTIFY_WINDOW_EXTENSION_VISIBILITY_CHANGE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WMError SceneSessionManagerProxy::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    WLOGFD("[GetTopWin] mainId: %{public}d", mainWinId);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interface token failed.");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(mainWinId)) {
        WLOGFE("Write mainWinId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_TOP_WINDOW_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    topWinId = reply.ReadUint32();
    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError SceneSessionManagerProxy::GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetVisibilityWindowInfo Write interfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_GET_VISIBILITY_WINDOW_INFO_ID), data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!MarshallingHelper::UnmarshallingVectorParcelableObj<WindowVisibilityInfo>(reply, infos)) {
        WLOGFE("read visibility window infos failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    return static_cast<WMError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::ShiftAppWindowFocus(int32_t sourcePersistentId, int32_t targetPersistentId)
{
    WLOGFD("run SceneSessionManagerProxy::ShiftAppWindowFocus");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Write interface token failed.");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (!data.WriteUint32(sourcePersistentId)) {
        WLOGFE("Write sourcePersistentId failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (!data.WriteUint32(targetPersistentId)) {
        WLOGFE("Write targetPersistentId failed");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_SHIFT_APP_WINDOW_FOCUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

void SceneSessionManagerProxy::AddExtensionWindowStageToSCB(const sptr<ISessionStage>& sessionStage,
    int32_t persistentId, int32_t parentId)
{
    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write InterfaceToken failed!");
        return;
    }
    if (!data.WriteRemoteObject(sessionStage->AsObject())) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write ISessionStage failed!");
        return;
    }
    if (!data.WriteInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write persistentId failed");
        return;
    }
    if (!data.WriteInt32(parentId)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write parentId failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
                              SceneSessionManagerMessage::TRANS_ID_ADD_EXTENSION_WINDOW_STAGE_TO_SCB),
                              data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_UIEXT, "SendRequest failed");
    }
}

WSError SceneSessionManagerProxy::AddOrRemoveSecureSession(int32_t persistentId, bool shouldHide)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_UIEXT, "WriteInterfaceToken failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write persistentId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(shouldHide)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write shouldHide failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
                              SceneSessionManagerMessage::TRANS_ID_ADD_OR_REMOVE_SECURE_SESSION),
                              data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_UIEXT, "SendRequest failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::UpdateExtWindowFlags(int32_t parentId, int32_t persistentId, uint32_t extWindowFlags,
    uint32_t extWindowActions)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "run SceneSessionManagerProxy::UpdateExtWindowFlags");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write interface token failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(parentId)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write parentId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write persistentId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(extWindowFlags)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write extWindowFlags failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(extWindowActions)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write extWindowActions failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(
        static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_UPDATE_EXTENSION_WINDOW_FLAGS),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_UIEXT, "SendRequest AddExtensionSessionInfo failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    return static_cast<WSError>(reply.ReadInt32());
}

WSError SceneSessionManagerProxy::GetHostWindowRect(int32_t hostWindowId, Rect& rect)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "run SceneSessionManagerProxy::GetHostWindowRect");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write interface token failed.");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(hostWindowId)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Write hostWindowId failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(
        static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_HOST_WINDOW_RECT),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_UIEXT, "SendRequest GetHostWindowRect failed");
        return WSError::WS_ERROR_IPC_FAILED;
    }
    auto posX = reply.ReadInt32();
    auto posY = reply.ReadInt32();
    auto width = reply.ReadUint32();
    auto height = reply.ReadUint32();
    rect = {posX, posY, width, height};
    return static_cast<WSError>(reply.ReadInt32());
}

WMError SceneSessionManagerProxy::GetCallingWindowWindowStatus(int32_t persistentId, WindowStatus& windowStatus)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Write windowId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_WINDOW_STATUS),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    auto ret = static_cast<WMError>(reply.ReadInt32());
    if (ret == WMError::WM_OK) {
        windowStatus = static_cast<WindowStatus>(reply.ReadUint32());
    }
    return ret;
}

WMError SceneSessionManagerProxy::GetCallingWindowRect(int32_t persistentId, Rect& rect)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteInt32(persistentId)) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Write windowId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(SceneSessionManagerMessage::TRANS_ID_GET_WINDOW_RECT),
        data, reply, option) != ERR_NONE) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "SendRequest failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    auto ret = static_cast<WMError>(reply.ReadInt32());
    if (ret == WMError::WM_OK) {
        rect.posX_ = reply.ReadInt32();
        rect.posY_ = reply.ReadInt32();
        rect.width_ = reply.ReadUint32();
        rect.height_ = reply.ReadUint32();
    }
    return ret;
}

WMError SceneSessionManagerProxy::GetWindowModeType(WindowModeType& windowModeType)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetWindowModeType Write interfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(
        SceneSessionManagerMessage::TRANS_ID_GET_WINDOW_MODE_TYPE), data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }
    windowModeType = static_cast<WindowModeType>(reply.ReadUint32());
    return static_cast<WMError>(reply.ReadInt32());
}
} // namespace OHOS::Rosen
