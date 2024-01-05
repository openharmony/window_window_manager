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

#include "ability_start_setting.h"
#include <ipc_types.h>
#include <ui/rs_surface_node.h>
#include "want.h"
#include "pointer_event.h"
#include "key_event.h"

#include "accessibility_event_info_parcel.h"
#include "session/host/include/zidl/session_ipc_interface_code.h"
#include "window_manager_hilog.h"

namespace OHOS::Accessibility {
class AccessibilityEventInfo;
}
namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionStub" };
} // namespace

const std::map<uint32_t, SessionStubFunc> SessionStub::stubFuncMap_ {
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_CONNECT),
        &SessionStub::HandleConnect),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_FOREGROUND),
        &SessionStub::HandleForeground),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKGROUND),
        &SessionStub::HandleBackground),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DISCONNECT),
        &SessionStub::HandleDisconnect),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SHOW),
        &SessionStub::HandleShow),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_HIDE),
        &SessionStub::HandleHide),

    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_ACTIVE_STATUS),
        &SessionStub::HandleUpdateActivateStatus),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SESSION_EVENT),
        &SessionStub::HandleSessionEvent),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_RECT),
        &SessionStub::HandleUpdateSessionRect),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_TO_APP_TOP),
        &SessionStub::HandleRaiseToAppTop),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKPRESSED),
        &SessionStub::HandleBackPressed),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_MARK_PROCESSED),
        &SessionStub::HandleMarkProcessed),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_MAXIMIZE_MODE),
        &SessionStub::HandleSetGlobalMaximizeMode),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_MAXIMIZE_MODE),
        &SessionStub::HandleGetGlobalMaximizeMode),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NEED_AVOID),
        &SessionStub::HandleNeedAvoid),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_AVOID_AREA),
        &SessionStub::HandleGetAvoidAreaByType),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_SESSION_PROPERTY),
        &SessionStub::HandleSetSessionProperty),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_ASPECT_RATIO),
        &SessionStub::HandleSetAspectRatio),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_WINDOW_ANIMATION_FLAG),
        &SessionStub::HandleSetWindowAnimationFlag),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_CUSTOM_ANIMATION),
        &SessionStub::HandleUpdateWindowSceneAfterCustomAnimation),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_ABOVE_TARGET),
        &SessionStub::HandleRaiseAboveTarget),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_APP_MAIN_WINDOW),
        &SessionStub::HandleRaiseAppMainWindowToTop),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_ACTIVE_PENDING_SESSION),
        &SessionStub::HandlePendingSessionActivation),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TERMINATE),
        &SessionStub::HandleTerminateSession),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_EXCEPTION),
        &SessionStub::HandleSessionException),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_PROCESS_POINT_DOWN_SESSION),
        &SessionStub::HandleProcessPointDownSession),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SEND_POINTEREVENT_FOR_MOVE_DRAG),
        &SessionStub::HandleSendPointerEvenForMoveDrag),

    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_ABILITY_RESULT),
        &SessionStub::HandleTransferAbilityResult),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_EXTENSION_DATA),
        &SessionStub::HandleTransferExtensionData),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_REMOTE_READY),
        &SessionStub::HandleNotifyRemoteReady),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_ASYNC_ON),
        &SessionStub::HandleNotifyAsyncOn),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_SYNC_ON),
        &SessionStub::HandleNotifySyncOn),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_DIED),
        &SessionStub::HandleNotifyExtensionDied),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_REPORT_ACCESSIBILITY_EVENT),
        &SessionStub::HandleTransferAccessibilityEvent),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_PIP_WINDOW_PREPARE_CLOSE),
        &SessionStub::HandleNotifyPiPWindowPrepareClose),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_PIP_RECT),
        &SessionStub::HandleUpdatePiPRect),
    std::make_pair(static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RECOVERY_PULL_PIP_MAIN_WINDOW),
        &SessionStub::HandleRecoveryPullPiPMainWindow)
};

int SessionStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
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

int SessionStub::HandleSetWindowAnimationFlag(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleSetWindowAnimationFlag!");
    bool isNeedWindowAnimationFlag = data.ReadBool();
    const WSError& errCode = UpdateWindowAnimationFlag(isNeedWindowAnimationFlag);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleForeground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("[WMSCom] Foreground!");
    sptr<WindowSessionProperty> property = nullptr;
    if (data.ReadBool()) {
        property = data.ReadStrongParcelable<WindowSessionProperty>();
    } else {
        WLOGFW("[WMSCom] Property not exist!");
        property = new WindowSessionProperty();
    }
    const WSError& errCode = Foreground(property);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleBackground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("[WMSCom] Background!");
    const WSError& errCode = Background();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleDisconnect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Disconnect!");
    const WSError& errCode = Disconnect();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleShow(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Show!");
    sptr<WindowSessionProperty> property = nullptr;
    if (data.ReadBool()) {
        property = data.ReadStrongParcelable<WindowSessionProperty>();
    } else {
        WLOGFW("Property not exist!");
        property = new WindowSessionProperty();
    }
    const WSError& errCode = Show(property);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleHide(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Hide!");
    const WSError& errCode = Hide();
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

    sptr<IRemoteObject> token = nullptr;
    if (property && property->GetTokenState()) {
        token = data.ReadRemoteObject();
    } else {
        WLOGI("accept token is nullptr");
    }
    SystemSessionConfig systemConfig;
    WSError errCode = Connect(sessionStage, eventChannel, surfaceNode, systemConfig, property, token);
    reply.WriteParcelable(&systemConfig);
    if (property) {
        reply.WriteInt32(property->GetPersistentId());
        bool needUpdate = property->GetIsNeedUpdateWindowMode();
        reply.WriteBool(needUpdate);
        if (needUpdate) {
            reply.WriteUint32(static_cast<uint32_t>(property->GetWindowMode()));
        }
        property->SetIsNeedUpdateWindowMode(false);
    }
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

int SessionStub::HandleTerminateSession(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleTerminateSession");
    sptr<AAFwk::SessionInfo> abilitySessionInfo(new AAFwk::SessionInfo());
    abilitySessionInfo->want = *(data.ReadParcelable<AAFwk::Want>());
    if (data.ReadBool()) {
        abilitySessionInfo->callerToken = data.ReadRemoteObject();
    }
    abilitySessionInfo->resultCode = data.ReadInt32();
    const WSError& errCode = TerminateSession(abilitySessionInfo);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleSessionException(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleSessionException");
    sptr<AAFwk::SessionInfo> abilitySessionInfo(new AAFwk::SessionInfo());
    abilitySessionInfo->want = *(data.ReadParcelable<AAFwk::Want>());
    if (data.ReadBool()) {
        abilitySessionInfo->callerToken = data.ReadRemoteObject();
    }

    abilitySessionInfo->persistentId = data.ReadInt32();
    abilitySessionInfo->errorCode = data.ReadInt32();
    abilitySessionInfo->errorReason = data.ReadString();
    const WSError& errCode = NotifySessionException(abilitySessionInfo);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandlePendingSessionActivation(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("PendingSessionActivation!");
    sptr<AAFwk::SessionInfo> abilitySessionInfo(new AAFwk::SessionInfo());
    abilitySessionInfo->want = *(data.ReadParcelable<AAFwk::Want>());
    abilitySessionInfo->requestCode = data.ReadInt32();
    abilitySessionInfo->persistentId = data.ReadInt32();
    abilitySessionInfo->state = static_cast<AAFwk::CallToState>(data.ReadInt32());
    abilitySessionInfo->uiAbilityId = data.ReadInt64();
    abilitySessionInfo->callingTokenId = data.ReadUint32();
    abilitySessionInfo->reuse = data.ReadBool();
    if (data.ReadBool()) {
        abilitySessionInfo->callerToken = data.ReadRemoteObject();
    }
    if (data.ReadBool()) {
        abilitySessionInfo->startSetting.reset(data.ReadParcelable<AAFwk::AbilityStartSetting>());
    }
    const WSError& errCode = PendingSessionActivation(abilitySessionInfo);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleUpdateActivateStatus(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleUpdateActivateStatus!");
    bool isActive = data.ReadBool();
    const WSError& errCode = UpdateActiveStatus(isActive);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleUpdateSessionRect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleUpdateSessionRect!");
    auto posX = data.ReadInt32();
    auto posY = data.ReadInt32();
    auto width = data.ReadUint32();
    auto height = data.ReadUint32();
    WSRect rect = {posX, posY, width, height};
    WLOGFI("HandleUpdateSessionRect [%{public}d, %{public}d, %{public}u, %{public}u]", posX, posY,
        width, height);
    const SizeChangeReason& reason = static_cast<SizeChangeReason>(data.ReadUint32());
    const WSError& errCode = UpdateSessionRect(rect, reason);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleRaiseToAppTop(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("RaiseToAppTop!");
    const WSError& errCode = RaiseToAppTop();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleRaiseAboveTarget(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("RaiseAboveTarget!");
    auto subWindowId = data.ReadInt32();
    const WSError& errCode = RaiseAboveTarget(subWindowId);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleRaiseAppMainWindowToTop(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("RaiseAppMainWindowToTop!");
    const WSError& errCode = RaiseAppMainWindowToTop();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleBackPressed(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleBackPressed!");
    bool needMoveToBackground = false;
    if (!data.ReadBool(needMoveToBackground)) {
        WLOGFE("Read needMoveToBackground from parcel failed!");
        return ERR_INVALID_DATA;
    }
    WSError errCode = RequestSessionBack(needMoveToBackground);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleMarkProcessed(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleMarkProcessed!");
    int32_t eventId = 0;
    if (!data.ReadInt32(eventId)) {
        WLOGFE("Read eventId from parcel failed!");
        return ERR_INVALID_DATA;
    }
    WSError errCode = MarkProcessed(eventId);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleSetGlobalMaximizeMode(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("HandleSetGlobalMaximizeMode!");
    auto mode = data.ReadUint32();
    WSError errCode = SetGlobalMaximizeMode(static_cast<MaximizeMode>(mode));
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleGetGlobalMaximizeMode(MessageParcel &data, MessageParcel &reply)
{
    WLOGFD("HandleGetGlobalMaximizeMode!");
    MaximizeMode mode = MaximizeMode::MODE_FULL_FILL;
    WSError errCode = GetGlobalMaximizeMode(mode);
    reply.WriteUint32(static_cast<uint32_t>(mode));
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleNeedAvoid(MessageParcel& data, MessageParcel& reply)
{
    bool status = static_cast<bool>(data.ReadUint32());
    WLOGFD("HandleNeedAvoid status:%{public}d", static_cast<int32_t>(status));
    WSError errCode = OnNeedAvoid(status);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleGetAvoidAreaByType(MessageParcel& data, MessageParcel& reply)
{
    AvoidAreaType type = static_cast<AvoidAreaType>(data.ReadUint32());
    WLOGFD("HandleGetAvoidArea type:%{public}d", static_cast<int32_t>(type));
    AvoidArea avoidArea = GetAvoidAreaByType(type);
    reply.WriteParcelable(&avoidArea);
    return ERR_NONE;
}

int SessionStub::HandleSetSessionProperty(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleSetSessionProperty!");
    auto property = data.ReadStrongParcelable<WindowSessionProperty>();
    auto errCode = SetSessionProperty(property);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleSetAspectRatio(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleSetAspectRatio!");
    float ratio = data.ReadFloat();
    const WSError& errCode = SetAspectRatio(ratio);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleUpdateWindowSceneAfterCustomAnimation(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleUpdateWindowSceneAfterCustomAnimation!");
    bool isAdd = data.ReadBool();
    const WSError& errCode = UpdateWindowSceneAfterCustomAnimation(isAdd);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleTransferAbilityResult(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleTransferAbilityResult!");
    uint32_t resultCode = data.ReadUint32();
    std::shared_ptr<AAFwk::Want> want(data.ReadParcelable<AAFwk::Want>());
    if (want == nullptr) {
        WLOGFE("want is nullptr");
        return ERR_INVALID_VALUE;
    }
    WSError errCode = TransferAbilityResult(resultCode, *want);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleTransferExtensionData(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleTransferExtensionData!");
    std::shared_ptr<AAFwk::WantParams> wantParams(data.ReadParcelable<AAFwk::WantParams>());
    if (wantParams == nullptr) {
        WLOGFE("wantParams is nullptr");
        return ERR_INVALID_VALUE;
    }
    WSError errCode = TransferExtensionData(*wantParams);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleNotifyRemoteReady(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleNotifyRemoteReady!");
    NotifyRemoteReady();
    return ERR_NONE;
}

int SessionStub::HandleNotifySyncOn(MessageParcel& data, MessageParcel& reply)
{
    NotifySyncOn();
    return ERR_NONE;
}

int SessionStub::HandleNotifyAsyncOn(MessageParcel& data, MessageParcel& reply)
{
    NotifyAsyncOn();
    return ERR_NONE;
}

int SessionStub::HandleNotifyExtensionDied(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("called");
    NotifyExtensionDied();
    return ERR_NONE;
}

int SessionStub::HandleTransferAccessibilityEvent(MessageParcel& data, MessageParcel& reply)
{
    sptr<Accessibility::AccessibilityEventInfoParcel> infoPtr =
        data.ReadStrongParcelable<Accessibility::AccessibilityEventInfoParcel>();
    int32_t uiExtensionIdLevel = 0;
    if (!data.ReadInt32(uiExtensionIdLevel)) {
        WLOGFE("read idVect error");
        return ERR_INVALID_DATA;
    }
    NotifyTransferAccessibilityEvent(*infoPtr, uiExtensionIdLevel);
    return ERR_NONE;
}

int SessionStub::HandleNotifyPiPWindowPrepareClose(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleNotifyPiPWindowPrepareClose");
    NotifyPiPWindowPrepareClose();
    return ERR_NONE;
}

int SessionStub::HandleUpdatePiPRect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleUpdatePiPRect!");
    uint32_t width = data.ReadUint32();
    uint32_t height = data.ReadUint32();
    auto reason = static_cast<PiPRectUpdateReason>(data.ReadInt32());
    WSError errCode = UpdatePiPRect(width, height, reason);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleRecoveryPullPiPMainWindow(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleNotifyRecoveryPullPiPMainWindow");
    int32_t persistentId = 0;
    if (!data.ReadInt32(persistentId)) {
        WLOGFE("Read eventId from parcel failed!");
        return ERR_INVALID_DATA;
    }
    Rect rect = {data.ReadInt32(), data.ReadInt32(), data.ReadUint32(), data.ReadUint32()};
    WSError errCode = RecoveryPullPiPMainWindow(persistentId, rect);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleProcessPointDownSession(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleProcessPointDownSession!");
    uint32_t posX = data.ReadInt32();
    uint32_t posY = data.ReadInt32();
    WSError errCode = ProcessPointDownSession(posX, posY);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleSendPointerEvenForMoveDrag(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleSendPointerEvenForMoveDrag!");
    auto pointerEvent = MMI::PointerEvent::Create();
    if (!pointerEvent->ReadFromParcel(data)) {
        WLOGFE("Read pointer event failed");
        return -1;
    }
    WSError errCode = SendPointEventForMoveDrag(pointerEvent);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}
} // namespace OHOS::Rosen
