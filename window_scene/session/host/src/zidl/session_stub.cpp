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

#include "parcel/accessibility_event_info_parcel.h"
#include "process_options.h"
#include "session/host/include/zidl/session_ipc_interface_code.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS::Accessibility {
class AccessibilityEventInfo;
}
namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionStub" };
} // namespace

int SessionStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    WLOGFD("Scene session on remote request!, code: %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_TRANSACTION_FAILED;
    }

    return ProcessRemoteRequest(code, data, reply, option);
}

int SessionStub::ProcessRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    switch (code) {
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_CONNECT):
            return HandleConnect(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_FOREGROUND):
            return HandleForeground(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKGROUND):
            return HandleBackground(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DISCONNECT):
            return HandleDisconnect(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SHOW):
            return HandleShow(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_HIDE):
            return HandleHide(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_DRAWING_COMPLETED):
            return HandleDrawingCompleted(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_RECTCHANGE_LISTENER_REGISTERED):
            return HandleUpdateRectChangeListenerRegistered(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SESSION_EVENT):
            return HandleSessionEvent(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SYSTEM_SESSION_EVENT):
            return HandleSystemSessionEvent(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_RECT):
            return HandleUpdateSessionRect(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_TO_APP_TOP):
            return HandleRaiseToAppTop(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_BACKPRESSED):
            return HandleBackPressed(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_MARK_PROCESSED):
            return HandleMarkProcessed(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_MAXIMIZE_MODE):
            return HandleSetGlobalMaximizeMode(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_MAXIMIZE_MODE):
            return HandleGetGlobalMaximizeMode(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NEED_AVOID):
            return HandleNeedAvoid(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_AVOID_AREA):
            return HandleGetAvoidAreaByType(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_ASPECT_RATIO):
            return HandleSetAspectRatio(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_WINDOW_ANIMATION_FLAG):
            return HandleSetWindowAnimationFlag(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_CUSTOM_ANIMATION):
            return HandleUpdateWindowSceneAfterCustomAnimation(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_LANDSCAPE_MULTI_WINDOW):
            return HandleSetLandscapeMultiWindow(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_ABOVE_TARGET):
            return HandleRaiseAboveTarget(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_RAISE_APP_MAIN_WINDOW):
            return HandleRaiseAppMainWindowToTop(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_CHANGE_SESSION_VISIBILITY_WITH_STATUS_BAR):
            return HandleChangeSessionVisibilityWithStatusBar(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_ACTIVE_PENDING_SESSION):
            return HandlePendingSessionActivation(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TERMINATE):
            return HandleTerminateSession(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_EXCEPTION):
            return HandleSessionException(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_PROCESS_POINT_DOWN_SESSION):
            return HandleProcessPointDownSession(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SEND_POINTEREVENT_FOR_MOVE_DRAG):
            return HandleSendPointerEvenForMoveDrag(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_SYSTEM_DRAG_ENABLE):
            return HandleSetSystemEnableDrag(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_KEYBOARD_SESSION_GRAVITY):
            return HandleSetKeyboardSessionGravity(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_CALLING_SESSION_ID):
            return HandleSetCallingSessionId(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_CUSTOM_DECOR_HEIGHT):
            return HandleSetCustomDecorHeight(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_SESSION_PROPERTY):
            return HandleUpdatePropertyByAction(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_ADJUST_KEYBOARD_LAYOUT):
            return HandleAdjustKeyboardLayout(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_ABILITY_RESULT):
            return HandleTransferAbilityResult(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRANSFER_EXTENSION_DATA):
            return HandleTransferExtensionData(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_ASYNC_ON):
            return HandleNotifyAsyncOn(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_SYNC_ON):
            return HandleNotifySyncOn(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_DIED):
            return HandleNotifyExtensionDied(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_EXTENSION_TIMEOUT):
            return HandleNotifyExtensionTimeout(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_TRIGGER_BIND_MODAL_UI_EXTENSION):
            return HandleTriggerBindModalUIExtension(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_REPORT_ACCESSIBILITY_EVENT):
            return HandleTransferAccessibilityEvent(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_NOTIFY_PIP_WINDOW_PREPARE_CLOSE):
            return HandleNotifyPiPWindowPrepareClose(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_PIP_RECT):
            return HandleUpdatePiPRect(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_UPDATE_PIP_CONTROL_STATUS):
            return HandleUpdatePiPControlStatus(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_LAYOUT_FULL_SCREEN_CHANGE):
            return HandleLayoutFullScreenChange(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_FORCE_LANDSCAPE_CONFIG):
            return HandleGetAppForceLandscapeConfig(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_STATUSBAR_HEIGHT):
            return HandleGetStatusBarHeight(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_SET_DIALOG_SESSION_BACKGESTURE_ENABLE):
            return HandleSetDialogSessionBackGestureEnabled(data, reply);
        case static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_FRAME_LAYOUT_FINISH):
            return HandleNotifyFrameLayoutFinish(data, reply);
        default:
            WLOGFE("Failed to find function handler!");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int SessionStub::HandleSetWindowAnimationFlag(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleSetWindowAnimationFlag!");
    bool isNeedWindowAnimationFlag = data.ReadBool();
    WSError errCode = UpdateWindowAnimationFlag(isNeedWindowAnimationFlag);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleForeground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("[WMSCom] Foreground!");
    sptr<WindowSessionProperty> property = nullptr;
    if (data.ReadBool()) {
        property = data.ReadStrongParcelable<WindowSessionProperty>();
        if (property == nullptr) {
            return ERR_INVALID_DATA;
        }
    } else {
        WLOGFW("[WMSCom] Property not exist!");
        property = sptr<WindowSessionProperty>::MakeSptr();
    }
    bool isFromClient = data.ReadBool();
    const WSError errCode = Foreground(property, isFromClient);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleBackground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("[WMSCom] Background!");
    bool isFromClient = data.ReadBool();
    const WSError errCode = Background(isFromClient);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleDisconnect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Disconnect!");
    bool isFromClient = data.ReadBool();
    WSError errCode = Disconnect(isFromClient);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleShow(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Show!");
    sptr<WindowSessionProperty> property = nullptr;
    if (data.ReadBool()) {
        property = data.ReadStrongParcelable<WindowSessionProperty>();
        if (property == nullptr) {
            return ERR_INVALID_DATA;
        }
    } else {
        WLOGFW("Property not exist!");
        property = sptr<WindowSessionProperty>::MakeSptr();
    }
    WSError errCode = Show(property);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleHide(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Hide!");
    WSError errCode = Hide();
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
        if (property == nullptr) {
            return ERR_INVALID_DATA;
        }
    } else {
        WLOGFW("Property not exist!");
    }

    sptr<IRemoteObject> token = nullptr;
    if (property && property->GetTokenState()) {
        token = data.ReadRemoteObject();
        if (token == nullptr) {
            return ERR_INVALID_DATA;
        }
    } else {
        WLOGI("accept token is nullptr");
    }
    std::string identityToken = data.ReadString();
    SystemSessionConfig systemConfig;
    WSError errCode = Connect(sessionStage, eventChannel, surfaceNode, systemConfig, property, token,
        identityToken);
    reply.WriteParcelable(&systemConfig);
    if (property) {
        reply.WriteInt32(property->GetPersistentId());
        reply.WriteUint64(property->GetDisplayId());
        bool needUpdate = property->GetIsNeedUpdateWindowMode();
        reply.WriteBool(needUpdate);
        if (needUpdate) {
            reply.WriteUint32(static_cast<uint32_t>(property->GetWindowMode()));
        }
        property->SetIsNeedUpdateWindowMode(false);

        Rect winRect = property->GetWindowRect();
        reply.WriteInt32(winRect.posX_);
        reply.WriteInt32(winRect.posY_);
        reply.WriteUint32(winRect.width_);
        reply.WriteUint32(winRect.height_);
        reply.WriteInt32(property->GetCollaboratorType());
        reply.WriteBool(property->GetFullScreenStart());
        reply.WriteBool(property->GetCompatibleModeInPc());
        reply.WriteBool(property->GetIsAppSupportPhoneInPc());
        reply.WriteBool(property->GetIsSupportDragInPcCompatibleMode());
        reply.WriteBool(property->GetIsPcAppInPad());
    }
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleNotifyFrameLayoutFinish(MessageParcel& data, MessageParcel& reply)
{
    WSError errCode = NotifyFrameLayoutFinishFromApp();
    reply.WriteInt32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleDrawingCompleted(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LIFE, "Called!");
    const WSError errCode = DrawingCompleted();
    reply.WriteInt32(static_cast<int32_t>(errCode));
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

int SessionStub::HandleSystemSessionEvent(MessageParcel& data, MessageParcel& reply)
{
    uint32_t eventId = data.ReadUint32();
    WLOGFD("HandleSystemSessionEvent eventId: %{public}d", eventId);
    WSError errCode = OnSystemSessionEvent(static_cast<SessionEvent>(eventId));
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleLayoutFullScreenChange(MessageParcel& data, MessageParcel& reply)
{
    bool isLayoutFullScreen = data.ReadBool();
    TLOGD(WmsLogTag::WMS_LAYOUT, "isLayoutFullScreen: %{public}d", isLayoutFullScreen);
    WSError errCode = OnLayoutFullScreenChange(isLayoutFullScreen);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleTerminateSession(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleTerminateSession");
    std::shared_ptr<AAFwk::Want> localWant(data.ReadParcelable<AAFwk::Want>());
    if (localWant == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "localWant is nullptr");
        return ERR_INVALID_VALUE;
    }
    sptr<AAFwk::SessionInfo> abilitySessionInfo = sptr<AAFwk::SessionInfo>::MakeSptr();
    abilitySessionInfo->want = *localWant;
    if (data.ReadBool()) {
        abilitySessionInfo->callerToken = data.ReadRemoteObject();
    }
    abilitySessionInfo->resultCode = data.ReadInt32();
    WSError errCode = TerminateSession(abilitySessionInfo);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleSessionException(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("run HandleSessionException");
    std::shared_ptr<AAFwk::Want> localWant(data.ReadParcelable<AAFwk::Want>());
    if (localWant == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "localWant is nullptr");
        return ERR_INVALID_VALUE;
    }
    sptr<AAFwk::SessionInfo> abilitySessionInfo = sptr<AAFwk::SessionInfo>::MakeSptr();
    abilitySessionInfo->want = *localWant;
    if (data.ReadBool()) {
        abilitySessionInfo->callerToken = data.ReadRemoteObject();
    }

    abilitySessionInfo->persistentId = data.ReadInt32();
    abilitySessionInfo->errorCode = data.ReadInt32();
    abilitySessionInfo->errorReason = data.ReadString();
    abilitySessionInfo->identityToken = data.ReadString();
    WSError errCode = NotifySessionException(abilitySessionInfo);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleChangeSessionVisibilityWithStatusBar(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleChangeSessionVisibilityWithStatusBar");
    sptr<AAFwk::Want> localWant = data.ReadParcelable<AAFwk::Want>();
    if (localWant == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "localWant is nullptr");
        return ERR_INVALID_VALUE;
    }
    sptr<AAFwk::SessionInfo> abilitySessionInfo = sptr<AAFwk::SessionInfo>::MakeSptr();
    abilitySessionInfo->want = *localWant;
    abilitySessionInfo->requestCode = data.ReadInt32();
    abilitySessionInfo->persistentId = data.ReadInt32();
    abilitySessionInfo->state = static_cast<AAFwk::CallToState>(data.ReadInt32());
    abilitySessionInfo->uiAbilityId = data.ReadInt64();
    abilitySessionInfo->callingTokenId = data.ReadUint32();
    abilitySessionInfo->reuse = data.ReadBool();
    abilitySessionInfo->processOptions =
        std::shared_ptr<AAFwk::ProcessOptions>(data.ReadParcelable<AAFwk::ProcessOptions>());
    if (data.ReadBool()) {
        abilitySessionInfo->callerToken = data.ReadRemoteObject();
    }
    if (data.ReadBool()) {
        abilitySessionInfo->startSetting.reset(data.ReadParcelable<AAFwk::AbilityStartSetting>());
    }
    bool visible = data.ReadBool();
    WSError errCode = ChangeSessionVisibilityWithStatusBar(abilitySessionInfo, visible);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandlePendingSessionActivation(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("PendingSessionActivation!");
    sptr<AAFwk::Want> localWant = data.ReadParcelable<AAFwk::Want>();
    if (localWant == nullptr) {
        TLOGE(WmsLogTag::WMS_LIFE, "localWant is nullptr");
        return ERR_INVALID_VALUE;
    }
    sptr<AAFwk::SessionInfo> abilitySessionInfo = sptr<AAFwk::SessionInfo>::MakeSptr();
    abilitySessionInfo->want = *localWant;
    abilitySessionInfo->requestCode = data.ReadInt32();
    abilitySessionInfo->persistentId = data.ReadInt32();
    abilitySessionInfo->state = static_cast<AAFwk::CallToState>(data.ReadInt32());
    abilitySessionInfo->uiAbilityId = data.ReadInt64();
    abilitySessionInfo->callingTokenId = data.ReadUint32();
    abilitySessionInfo->reuse = data.ReadBool();
    abilitySessionInfo->processOptions.reset(data.ReadParcelable<AAFwk::ProcessOptions>());
    abilitySessionInfo->canStartAbilityFromBackground = data.ReadBool();
    abilitySessionInfo->isAtomicService = data.ReadBool();
    abilitySessionInfo->isBackTransition = data.ReadBool();
    if (data.ReadBool()) {
        abilitySessionInfo->callerToken = data.ReadRemoteObject();
    }
    if (data.ReadBool()) {
        abilitySessionInfo->startSetting.reset(data.ReadParcelable<AAFwk::AbilityStartSetting>());
    }
    WSError errCode = PendingSessionActivation(abilitySessionInfo);
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
    WSError errCode = UpdateSessionRect(rect, reason);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleRaiseToAppTop(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("RaiseToAppTop!");
    WSError errCode = RaiseToAppTop();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleRaiseAboveTarget(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("RaiseAboveTarget!");
    auto subWindowId = data.ReadInt32();
    WSError errCode = RaiseAboveTarget(subWindowId);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleRaiseAppMainWindowToTop(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("RaiseAppMainWindowToTop!");
    WSError errCode = RaiseAppMainWindowToTop();
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

int SessionStub::HandleSetGlobalMaximizeMode(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleSetGlobalMaximizeMode!");
    auto mode = data.ReadUint32();
    WSError errCode = SetGlobalMaximizeMode(static_cast<MaximizeMode>(mode));
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleGetGlobalMaximizeMode(MessageParcel& data, MessageParcel& reply)
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

int SessionStub::HandleSetAspectRatio(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleSetAspectRatio!");
    float ratio = data.ReadFloat();
    WSError errCode = SetAspectRatio(ratio);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleUpdateWindowSceneAfterCustomAnimation(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleUpdateWindowSceneAfterCustomAnimation!");
    bool isAdd = data.ReadBool();
    WSError errCode = UpdateWindowSceneAfterCustomAnimation(isAdd);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleSetLandscapeMultiWindow(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_MULTI_WINDOW, "HandleSetLandscapeMultiWindow!");
    bool isLandscapeMultiWindow = data.ReadBool();
    const WSError errCode = SetLandscapeMultiWindow(isLandscapeMultiWindow);
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

int SessionStub::HandleNotifyExtensionTimeout(MessageParcel& data, MessageParcel& reply)
{
    int32_t errorCode = 0;
    if (!data.ReadInt32(errorCode)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Read eventId from parcel failed!");
        return ERR_INVALID_DATA;
    }
    NotifyExtensionTimeout(errorCode);
    return ERR_NONE;
}

int SessionStub::HandleTriggerBindModalUIExtension(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("called");
    TriggerBindModalUIExtension();
    return ERR_NONE;
}

int SessionStub::HandleTransferAccessibilityEvent(MessageParcel& data, MessageParcel& reply)
{
    sptr<Accessibility::AccessibilityEventInfoParcel> infoPtr =
        data.ReadStrongParcelable<Accessibility::AccessibilityEventInfoParcel>();
    if (infoPtr == nullptr) {
        return ERR_INVALID_DATA;
    }
    int64_t uiExtensionIdLevel = 0;
    if (!data.ReadInt64(uiExtensionIdLevel)) {
        WLOGFE("read uiExtensionIdLevel error");
        return ERR_INVALID_DATA;
    }
    NotifyTransferAccessibilityEvent(*infoPtr, uiExtensionIdLevel);
    return ERR_NONE;
}

int SessionStub::HandleNotifyPiPWindowPrepareClose(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_PIP, "HandleNotifyPiPWindowPrepareClose");
    NotifyPiPWindowPrepareClose();
    return ERR_NONE;
}

int SessionStub::HandleUpdatePiPRect(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_PIP, "HandleUpdatePiPRect!");
    Rect rect = {data.ReadInt32(), data.ReadInt32(), data.ReadUint32(), data.ReadUint32()};
    auto reason = static_cast<SizeChangeReason>(data.ReadInt32());
    WSError errCode = UpdatePiPRect(rect, reason);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleUpdatePiPControlStatus(MessageParcel& data, MessageParcel& reply)
{
    TLOGI(WmsLogTag::WMS_PIP, "called");
    uint32_t controlType = 0;
    int32_t status = 0;
    if (data.ReadUint32(controlType) && data.ReadInt32(status)) {
        WSError errCode = UpdatePiPControlStatus(static_cast<WsPiPControlType>(controlType),
            static_cast<WsPiPControlStatus>(status));
        reply.WriteInt32(static_cast<int32_t>(errCode));
        return ERR_NONE;
    } else {
        return ERR_INVALID_DATA;
    }
}

int SessionStub::HandleSetSystemEnableDrag(MessageParcel& data, MessageParcel& reply)
{
    bool enableDrag = data.ReadBool();
    TLOGI(WmsLogTag::WMS_LAYOUT, "handle, enableDrag: %{public}d", enableDrag);
    WSError errcode = SetSystemWindowEnableDrag(enableDrag);
    reply.WriteInt32(static_cast<int32_t>(errcode));
    return ERR_NONE;
}

int SessionStub::HandleProcessPointDownSession(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleProcessPointDownSession!");
    int32_t posX = data.ReadInt32();
    int32_t posY = data.ReadInt32();
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

int SessionStub::HandleUpdateRectChangeListenerRegistered(MessageParcel& data, MessageParcel& reply)
{
    bool isRegister = data.ReadBool();
    WSError errCode = UpdateRectChangeListenerRegistered(isRegister);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStub::HandleSetKeyboardSessionGravity(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "run HandleSetKeyboardSessionGravity!");
    SessionGravity gravity = static_cast<SessionGravity>(data.ReadUint32());
    uint32_t percent = data.ReadUint32();
    WSError ret = SetKeyboardSessionGravity(gravity, percent);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}

int SessionStub::HandleSetCallingSessionId(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "run HandleSetCallingSessionId!");
    uint32_t callingSessionId = data.ReadUint32();

    SetCallingSessionId(callingSessionId);
    reply.WriteInt32(static_cast<int32_t>(WSError::WS_OK));
    return ERR_NONE;
}

int SessionStub::HandleSetCustomDecorHeight(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "run HandleSetCustomDecorHeight!");
    int32_t height = data.ReadInt32();
    SetCustomDecorHeight(height);
    return ERR_NONE;
}

int SessionStub::HandleAdjustKeyboardLayout(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "run HandleAdjustKeyboardLayout!");
    sptr<KeyboardLayoutParams> keyboardLayoutParams = data.ReadParcelable<KeyboardLayoutParams>();
    if (keyboardLayoutParams == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "keyboardLayoutParams is nullptr.");
        return ERR_INVALID_DATA;
    }
    WSError ret = AdjustKeyboardLayout(*keyboardLayoutParams);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}

int SessionStub::HandleUpdatePropertyByAction(MessageParcel& data, MessageParcel& reply)
{
    auto action = static_cast<WSPropertyChangeAction>(data.ReadUint32());
    TLOGD(WmsLogTag::DEFAULT, "action:%{public}u", action);
    sptr<WindowSessionProperty> property = nullptr;
    if (data.ReadBool()) {
        property = sptr<WindowSessionProperty>::MakeSptr();
        if (property != nullptr) {
            property->Read(data, action);
        }
    } else {
        TLOGW(WmsLogTag::DEFAULT, "Property not exist!");
    }
    const WMError ret = UpdateSessionPropertyByAction(property, action);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}

int SessionStub::HandleGetAppForceLandscapeConfig(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::DEFAULT, "called");
    AppForceLandscapeConfig config;
    WMError ret = GetAppForceLandscapeConfig(config);
    reply.WriteParcelable(&config);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}

int SessionStub::HandleGetStatusBarHeight(MessageParcel& data, MessageParcel& reply)
{
    int32_t height = GetStatusBarHeight();
    TLOGD(WmsLogTag::WMS_IMMS, "StatusBarVectorHeight is %{public}d", height);
    reply.WriteInt32(height);
    return ERR_NONE;
}

int SessionStub::HandleSetDialogSessionBackGestureEnabled(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_DIALOG, "called");
    bool isEnabled = data.ReadBool();
    WSError ret = SetDialogSessionBackGestureEnabled(isEnabled);
    reply.WriteInt32(static_cast<int32_t>(ret));
    return ERR_NONE;
}
} // namespace OHOS::Rosen
