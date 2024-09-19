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

#include "session/container/include/zidl/session_stage_stub.h"
#include "session/container/include/zidl/session_stage_ipc_interface_code.h"

#include <ipc_types.h>
#include <transaction/rs_transaction.h>

#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "SessionStageStub"};
}

int SessionStageStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    WLOGFD("Scene session stage on remote request!, code: %{public}u", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("Failed to check interface token!");
        return ERR_TRANSACTION_FAILED;
    }

    switch (code) {
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_SET_ACTIVE):
            return HandleSetActive(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SIZE_CHANGE):
            return HandleUpdateRect(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DENSITY_CHANGE):
            return HandleUpdateDensity(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_ORIENTATION_CHANGE):
            return HandleUpdateOrientation(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_UPDATE_SESSION_VIEWPORT_CONFIG):
            return HandleUpdateSessionViewportConfig(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_HANDLE_BACK_EVENT):
            return HandleBackEventInner(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DESTROY):
            return HandleNotifyDestroy(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_FOCUS_CHANGE):
            return HandleUpdateFocus(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TRANSFER_COMPONENT_DATA):
            return HandleNotifyTransferComponentData(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TRANSFER_COMPONENT_DATA_SYNC):
            return HandleNotifyTransferComponentDataSync(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_OCCUPIED_AREA_CHANGE_INFO):
            return HandleNotifyOccupiedAreaChange(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_UPDATE_AVOID_AREA):
            return HandleUpdateAvoidArea(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SCREEN_SHOT):
            return HandleNotifyScreenshot(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_DUMP_SESSSION_ELEMENT_INFO):
            return HandleDumpSessionElementInfo(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TOUCH_OUTSIDE):
            return HandleNotifyTouchOutside(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_WINDOW_MODE_CHANGE):
            return HandleUpdateWindowMode(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_FOREGROUND_INTERACTIVE_STATUS):
            return HandleNotifyForegroundInteractiveStatus(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_MAXIMIZE_MODE_CHANGE):
            return HandleUpdateMaximizeMode(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_CLOSE_EXIST_PIP_WINDOW):
            return HandleNotifyCloseExistPipWindow(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SESSION_FOREGROUND):
            return HandleNotifySessionForeground(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SESSION_BACKGROUND):
            return HandleNotifySessionBackground(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TITLE_POSITION_CHANGE):
            return HandleUpdateTitleInTargetPos(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DENSITY_FOLLOW_HOST):
            return HandleNotifyDensityFollowHost(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_WINDOW_VISIBILITY_CHANGE):
            return HandleNotifyWindowVisibilityChange(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_TRANSFORM_CHANGE):
            return HandleNotifyTransformChange(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DIALOG_STATE_CHANGE):
            return HandleNotifyDialogStateChange(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_SET_PIP_ACTION_EVENT):
            return HandleSetPipActionEvent(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_SET_PIP_CONTROL_EVENT):
            return HandleSetPiPControlEvent(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DISPLAYID_CHANGE):
            return HandleUpdateDisplayId(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DISPLAY_MOVE):
            return HandleNotifyDisplayMove(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SWITCH_FREEMULTIWINDOW):
            return HandleSwitchFreeMultiWindow(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_GET_UI_CONTENT_REMOTE_OBJ):
            return HandleGetUIContentRemoteObj(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_KEYBOARD_INFO_CHANGE):
            return HandleNotifyKeyboardPanelInfoChange(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_COMPATIBLE_FULLSCREEN_RECOVER):
            return HandleCompatibleFullScreenRecover(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_COMPATIBLE_FULLSCREEN_MINIMIZE):
            return HandleCompatibleFullScreenMinimize(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_COMPATIBLE_FULLSCREEN_CLOSE):
            return HandleCompatibleFullScreenClose(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_COMPATIBLE_MODE_ENABLE):
            return HandleNotifyCompatibleModeEnableInPad(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DENSITY_UNIQUE):
            return HandleSetUniqueVirtualPixelRatio(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_SESSION_FULLSCREEN):
            return HandleNotifySessionFullScreen(data, reply);
        case static_cast<uint32_t>(SessionStageInterfaceCode::TRANS_ID_NOTIFY_DUMP_INFO):
            return HandleNotifyDumpInfo(data, reply);
        default:
            WLOGFE("Failed to find function handler!");
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int SessionStageStub::HandleSetActive(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("SetActive!");
    bool active = data.ReadBool();
    WSError errCode = SetActive(active);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleUpdateRect(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("UpdateRect!");
    WSRect rect = { data.ReadInt32(), data.ReadInt32(), data.ReadUint32(), data.ReadUint32() };
    SizeChangeReason reason = static_cast<SizeChangeReason>(data.ReadUint32());
    bool hasRSTransaction = data.ReadBool();
    if (hasRSTransaction) {
        std::shared_ptr<RSTransaction> transaction(data.ReadParcelable<RSTransaction>());
        if (!transaction) {
            WLOGFE("transaction unMarsh failed");
            return -1;
        }
        WSError errCode = UpdateRect(rect, reason, transaction);
        reply.WriteUint32(static_cast<uint32_t>(errCode));
    } else {
        WSError errCode = UpdateRect(rect, reason);
        reply.WriteUint32(static_cast<uint32_t>(errCode));
    }
    return ERR_NONE;
}

int SessionStageStub::HandleUpdateDensity(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("UpdateDensity!");
    UpdateDensity();
    return ERR_NONE;
}

int SessionStageStub::HandleUpdateOrientation(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::DMS, "HandleUpdateOrientation!");
    WSError errCode = UpdateOrientation();
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleUpdateSessionViewportConfig(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "HandleUpdateSessionViewportConfig!");
    SessionViewportConfig config;
    if (!data.ReadBool(config.isDensityFollowHost_) || !data.ReadFloat(config.density_) ||
        !data.ReadUint64(config.displayId_) || !data.ReadInt32(config.orientation_) ||
        !data.ReadUint32(config.transform_)) {
        TLOGE(WmsLogTag::WMS_UIEXT, "Read HandleUpdateSessionViewportConfig data failed!");
        return ERR_INVALID_DATA;
    };
    UpdateSessionViewportConfig(config);
    return ERR_NONE;
}

int SessionStageStub::HandleBackEventInner(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleBackEventInner!");
    WSError errCode = HandleBackEvent();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyDestroy(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Notify Destroy");
    WSError errCode = NotifyDestroy();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyCloseExistPipWindow(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_PIP, "Notify Pip AlreadyExists");
    WSError errCode = NotifyCloseExistPipWindow();
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleUpdateFocus(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("UpdateFocus!");
    bool isFocused = data.ReadBool();
    WSError errCode = UpdateFocus(isFocused);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyTransferComponentData(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleNotifyTransferComponentData!");
    std::shared_ptr<AAFwk::WantParams> wantParams(data.ReadParcelable<AAFwk::WantParams>());
    if (wantParams == nullptr) {
        WLOGFE("wantParams is nullptr");
        return ERR_INVALID_VALUE;
    }
    WSError errCode = NotifyTransferComponentData(*wantParams);
    reply.WriteUint32(static_cast<uint32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyTransferComponentDataSync(MessageParcel& data, MessageParcel& reply)
{
    std::shared_ptr<AAFwk::WantParams> wantParams(data.ReadParcelable<AAFwk::WantParams>());
    if (wantParams == nullptr) {
        WLOGFE("wantParams is nullptr");
        return static_cast<int>(WSErrorCode::WS_ERROR_TRANSFER_DATA_FAILED);
    }
    AAFwk::WantParams reWantParams;
    WSErrorCode errCode = NotifyTransferComponentDataSync(*wantParams, reWantParams);
    if (errCode != WSErrorCode::WS_OK) {
        return static_cast<int>(errCode);
    }
    if (!reply.WriteParcelable(&reWantParams)) {
        WLOGFE("reWantParams write failed.");
        return static_cast<int>(WSErrorCode::WS_ERROR_TRANSFER_DATA_FAILED);
    }
    return static_cast<int>(WSErrorCode::WS_OK);
}

int SessionStageStub::HandleNotifyOccupiedAreaChange(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "HandleNotifyOccupiedAreaChangeInfo!");
    sptr<OccupiedAreaChangeInfo> info(data.ReadParcelable<OccupiedAreaChangeInfo>());
    if (info == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "Occupied info is nullptr");
        return ERR_INVALID_VALUE;
    }

    bool hasRSTransaction = data.ReadBool();
    if (hasRSTransaction) {
        std::shared_ptr<RSTransaction> transaction(data.ReadParcelable<RSTransaction>());
        if (!transaction) {
            TLOGE(WmsLogTag::WMS_KEYBOARD, "transaction unMarsh failed");
            return ERR_INVALID_VALUE;
        }
        NotifyOccupiedAreaChangeInfo(info, transaction);
    } else {
        NotifyOccupiedAreaChangeInfo(info);
    }

    return ERR_NONE;
}

int SessionStageStub::HandleUpdateAvoidArea(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleUpdateAvoidArea!");
    sptr<AvoidArea> avoidArea = data.ReadStrongParcelable<AvoidArea>();
    if (!avoidArea) {
        return ERR_INVALID_VALUE;
    }
    uint32_t type;
    if (!data.ReadUint32(type)) {
        return ERR_INVALID_VALUE;
    }
    UpdateAvoidArea(avoidArea, static_cast<AvoidAreaType>(type));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyScreenshot(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("Notify Screen shot!");
    NotifyScreenshot();
    return ERR_NONE;
}

int SessionStageStub::HandleDumpSessionElementInfo(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleDumpSessionElementInfo!");
    std::vector<std::string> params;
    if (!data.ReadStringVector(&params)) {
        WLOGFE("Fail to read params");
        return -1;
    }
    DumpSessionElementInfo(params);
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyTouchOutside(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleNotifyTouchOutside!");
    NotifyTouchOutside();
    return ERR_NONE;
}

int SessionStageStub::HandleUpdateWindowMode(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleUpdateWindowMode!");
    WindowMode mode = static_cast<WindowMode>(data.ReadUint32());
    WSError errCode = UpdateWindowMode(mode);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyWindowVisibilityChange(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleNotifyWindowVisibilityChange!");
    bool isVisible = data.ReadBool();
    WSError errCode = NotifyWindowVisibility(isVisible);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyForegroundInteractiveStatus(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("NotifyForegroundInteractiveStatus!");
    bool interactive = data.ReadBool();
    NotifyForegroundInteractiveStatus(interactive);
    return ERR_NONE;
}

int SessionStageStub::HandleUpdateMaximizeMode(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleUpdateMaximizeMode!");
    MaximizeMode mode = static_cast<MaximizeMode>(data.ReadUint32());
    WSError errCode = UpdateMaximizeMode(mode);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifySessionForeground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleNotifySessionForeground");
    uint32_t reason = data.ReadUint32();
    bool withAnimation = data.ReadBool();
    NotifySessionForeground(reason, withAnimation);
    return ERR_NONE;
}

int SessionStageStub::HandleNotifySessionFullScreen(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "called");
    bool fullScreen = data.ReadBool();
    NotifySessionFullScreen(fullScreen);
    return ERR_NONE;
}

int SessionStageStub::HandleNotifySessionBackground(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleNotifySessionBackground");
    uint32_t reason = data.ReadUint32();
    bool withAnimation = data.ReadBool();
    bool isFromInnerkits = data.ReadBool();
    NotifySessionBackground(reason, withAnimation, isFromInnerkits);
    return ERR_NONE;
}

int SessionStageStub::HandleUpdateTitleInTargetPos(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleUpdateTitleInTargetPos!");
    bool isShow = data.ReadBool();
    int32_t height = data.ReadInt32();
    WSError errCode = UpdateTitleInTargetPos(isShow, height);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyTransformChange(MessageParcel& data, MessageParcel& reply)
{
    WLOGFD("HandleNotifyTransformChange!");
    Transform transform;
    transform.Unmarshalling(data);
    NotifyTransformChange(transform);
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyDensityFollowHost(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_UIEXT, "HandleNotifyDensityFollowHost");
    bool isFollowHost = data.ReadBool();
    float densityValue = data.ReadFloat();
    NotifyDensityFollowHost(isFollowHost, densityValue);
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyDialogStateChange(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleNotifyDialogStateChange!");
    bool isForeground = data.ReadBool();
    NotifyDialogStateChange(isForeground);
    return ERR_NONE;
}

int SessionStageStub::HandleSetPipActionEvent(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_PIP, "HandleSetPipActionEvent");
    std::string action = data.ReadString();
    if (action.empty()) {
        TLOGE(WmsLogTag::WMS_PIP, "SessionStageStub pip action event is nullptr");
        return ERR_INVALID_VALUE;
    }
    int32_t status;
    if (!data.ReadInt32(status)) {
        return ERR_INVALID_VALUE;
    }
    SetPipActionEvent(action, status);
    return ERR_NONE;
}

int SessionStageStub::HandleSetPiPControlEvent(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_PIP, "called");
    uint32_t controlType;
    if (!data.ReadUint32(controlType)) {
        return ERR_INVALID_VALUE;
    }
    int32_t status;
    if (!data.ReadInt32(status)) {
        return ERR_INVALID_VALUE;
    }
    SetPiPControlEvent(static_cast<WsPiPControlType>(controlType), static_cast<WsPiPControlStatus>(status));
    return ERR_NONE;
}

int SessionStageStub::HandleUpdateDisplayId(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("UpdateDisplayId!");
    uint64_t displayId = data.ReadUint64();
    WSError errCode = UpdateDisplayId(displayId);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyDisplayMove(MessageParcel& data, MessageParcel& reply)
{
    WLOGD("HandleNotifyDisplayMove!");
    DisplayId from = static_cast<DisplayId>(data.ReadUint64());
    DisplayId to = static_cast<DisplayId>(data.ReadUint64());
    NotifyDisplayMove(from, to);
    return ERR_NONE;
}

int SessionStageStub::HandleSwitchFreeMultiWindow(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LAYOUT, "HandleSwitchFreeMultiWindow!");
    bool enable = data.ReadBool();
    WSError errCode = SwitchFreeMultiWindow(enable);
    reply.WriteInt32(static_cast<int32_t>(errCode));

    return ERR_NONE;
}

int SessionStageStub::HandleGetUIContentRemoteObj(MessageParcel& data, MessageParcel& reply)
{
    TLOGI(WmsLogTag::DEFAULT, "Called");
    sptr<IRemoteObject> uiContentRemoteObj;
    WSError errCode = GetUIContentRemoteObj(uiContentRemoteObj);
    reply.WriteRemoteObject(uiContentRemoteObj);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyKeyboardPanelInfoChange(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_KEYBOARD, "HandleNotifyKeyboardPanelInfoChange!");
    sptr<KeyboardPanelInfo> keyboardPanelInfo = data.ReadParcelable<KeyboardPanelInfo>();
    if (keyboardPanelInfo == nullptr) {
        TLOGE(WmsLogTag::WMS_KEYBOARD, "keyboardPanelInfo is nullptr!");
        return ERR_INVALID_VALUE;
    }
    NotifyKeyboardPanelInfoChange(*keyboardPanelInfo);

    return ERR_NONE;
}

int SessionStageStub::HandleCompatibleFullScreenRecover(MessageParcel& data, MessageParcel& reply)
{
    WSError errCode = CompatibleFullScreenRecover();
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleCompatibleFullScreenMinimize(MessageParcel& data, MessageParcel& reply)
{
    WSError errCode = CompatibleFullScreenMinimize();
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleCompatibleFullScreenClose(MessageParcel& data, MessageParcel& reply)
{
    WSError errCode = CompatibleFullScreenClose();
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyCompatibleModeEnableInPad(MessageParcel& data, MessageParcel& reply)
{
    bool enable = data.ReadBool();
    WSError errCode = NotifyCompatibleModeEnableInPad(enable);
    reply.WriteInt32(static_cast<int32_t>(errCode));
    return ERR_NONE;
}

int SessionStageStub::HandleSetUniqueVirtualPixelRatio(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::DEFAULT, "HandleSetUniqueVirtualPixelRatio!");
    bool useUniqueDensity = data.ReadBool();
    float densityValue = data.ReadFloat();
    SetUniqueVirtualPixelRatio(useUniqueDensity, densityValue);
    return ERR_NONE;
}

int SessionStageStub::HandleNotifyDumpInfo(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::DEFAULT, "HandleNotifyDumpInfo!");
    std::vector<std::string> params;
    if (!data.ReadStringVector(&params)) {
        TLOGE(WmsLogTag::DEFAULT, "Failed to read string vector");
        return ERR_INVALID_VALUE;
    }
    std::vector<std::string> info;
    WSError errCode = NotifyDumpInfo(params, info);
    if (!reply.WriteStringVector(info) || !reply.WriteInt32(static_cast<int32_t>(errCode))) {
        TLOGE(WmsLogTag::DEFAULT, "HandleNotifyDumpInfo write info failed");
        return ERR_TRANSACTION_FAILED;
    }
    return ERR_NONE;
}
} // namespace OHOS::Rosen
