/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "zidl/window_manager_proxy.h"
#include <ipc_types.h>
#include <rs_iwindow_animation_controller.h>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerProxy"};
}


WMError WindowManagerProxy::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId, sptr<IRemoteObject> token)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteRemoteObject(window->AsObject())) {
        WLOGFE("Write IWindow failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteParcelable(property.GetRefPtr())) {
        WLOGFE("Write windowProperty failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteParcelable(surfaceNode.get())) {
        WLOGFE("Write windowProperty failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (token != nullptr) {
        if (!data.WriteRemoteObject(token)) {
            WLOGFE("Write abilityToken failed");
            return WMError::WM_ERROR_IPC_FAILED;
        }
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_CREATE_WINDOW),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }
    windowId = reply.ReadUint32();
    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError WindowManagerProxy::AddWindow(sptr<WindowProperty>& property)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteParcelable(property.GetRefPtr())) {
        WLOGFE("Write windowProperty failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_ADD_WINDOW),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }

    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError WindowManagerProxy::RemoveWindow(uint32_t windowId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(windowId)) {
        WLOGFE("Write windowId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_REMOVE_WINDOW),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }

    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError WindowManagerProxy::DestroyWindow(uint32_t windowId, bool /* onlySelf */)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(windowId)) {
        WLOGFE("Write windowId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_DESTROY_WINDOW),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }

    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError WindowManagerProxy::RequestFocus(uint32_t windowId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(windowId)) {
        WLOGFE("Write windowId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_REQUEST_FOCUS),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }

    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError WindowManagerProxy::SetWindowBackgroundBlur(uint32_t windowId, WindowBlurLevel level)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(windowId)) {
        WLOGFE("Write windowId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(level))) {
        WLOGFE("Write blur level failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_SET_BACKGROUND_BLUR),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }

    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError WindowManagerProxy::SetAlpha(uint32_t windowId, float alpha)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(windowId)) {
        WLOGFE("Write windowId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteFloat(alpha)) {
        WLOGFE("Write alpha failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_SET_APLPHA),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }

    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

std::vector<Rect> WindowManagerProxy::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    const uint32_t maxAvoidNum = 4;

    std::vector<Rect> avoidArea;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return avoidArea;
    }

    if (!data.WriteUint32(windowId)) {
        WLOGFE("Write windowId failed");
        return avoidArea;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write AvoidAreaType failed");
        return avoidArea;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_GET_AVOID_AREA),
        data, reply, option) != ERR_NONE) {
        return avoidArea;
    }

    uint32_t avoidNum = reply.ReadUint32();
    if (avoidNum != maxAvoidNum) {
        WLOGFE("Read len fail. AvoidArea size != 4");
        return avoidArea;
    }
    avoidArea.resize(avoidNum);
    if (avoidArea.size() < avoidNum) {
        WLOGFE("Fail to resize avoidArea.");
        return avoidArea;
    }
    bool readVectorRes = true;
    for (uint32_t i = 0; i < avoidNum; ++i) {
        readVectorRes = reply.ReadInt32(avoidArea[i].posX_) && reply.ReadInt32(avoidArea[i].posY_) &&
            reply.ReadUint32(avoidArea[i].width_) && reply.ReadUint32(avoidArea[i].height_);
        if (!readVectorRes) {
            WLOGFE("Fail to ReadInt32. index:%{public}u, nums:%{public}u", i, avoidNum);
            return avoidArea;
        }
    }
    return avoidArea;
}

void WindowManagerProxy::RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write type failed");
        return;
    }

    if (!data.WriteRemoteObject(windowManagerAgent->AsObject())) {
        WLOGFE("Write IWindowManagerAgent failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void WindowManagerProxy::UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write type failed");
        return;
    }

    if (!data.WriteRemoteObject(windowManagerAgent->AsObject())) {
        WLOGFE("Write IWindowManagerAgent failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

WMError WindowManagerProxy::SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (controller == nullptr) {
        WLOGFE("Failed to set window animation controller, controller is null!");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Failed to WriteInterfaceToken!");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteParcelable(controller->AsObject())) {
        WLOGFE("Failed to write controller!");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    auto error = Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_ANIMATION_SET_CONTROLLER),
        data, reply, option);
    if (error != ERR_NONE) {
        WLOGFE("Send request error: %{public}d", error);
        return WMError::WM_ERROR_IPC_FAILED;
    }

    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

void WindowManagerProxy::ProcessPointDown(uint32_t windowId, bool isStartDrag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(windowId)) {
        WLOGFE("Write windowId failed");
        return;
    }
    if (!data.WriteBool(isStartDrag)) {
        WLOGFE("Write bool isStartDrag failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_PROCESS_POINT_DOWN),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void WindowManagerProxy::ProcessPointUp(uint32_t windowId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(windowId)) {
        WLOGFE("Write windowId failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_PROCESS_POINT_UP),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void WindowManagerProxy::MinimizeAllAppWindows(DisplayId displayId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write displayId failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_MINIMIZE_ALL_APP_WINDOWS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void WindowManagerProxy::ToggleShownStateForAllAppWindow()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (Remote()->SendRequest(
        static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_TOGGLE_SHOWN_STATE_FOR_ALL_APP_WINDOWS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

WMError WindowManagerProxy::MaxmizeWindow(uint32_t windowId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(windowId)) {
        WLOGFE("Write windowId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_MAXMIZE_WINDOW),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }

    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError WindowManagerProxy::SetWindowLayoutMode(DisplayId displayId, WindowLayoutMode mode)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write displayId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(mode))) {
        WLOGFE("Write mode failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_UPDATE_LAYOUT_MODE),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }

    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError WindowManagerProxy::UpdateProperty(sptr<WindowProperty>& windowProperty, PropertyChangeAction action)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteParcelable(windowProperty.GetRefPtr())) {
        WLOGFE("Write windowProperty failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(action))) {
        WLOGFE("Write PropertyChangeAction failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_UPDATE_PROPERTY),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }

    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError WindowManagerProxy::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(mainWinId)) {
        WLOGFE("Write mainWinId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_GET_TOP_WINDOW_ID),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }
    topWinId = reply.ReadUint32();
    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError WindowManagerProxy::GetAccessibilityWindowInfo(sptr<AccessibilityWindowInfo>& windowInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteParcelable(windowInfo)) {
        WLOGFE("Write windowInfo failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_GET_ACCCESSIBILITY_WIDDOW_INFO_ID),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }
    windowInfo = reply.ReadParcelable<AccessibilityWindowInfo>();
    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError WindowManagerProxy::GetSystemDecorEnable(bool& isSystemDecorEnable)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteBool(isSystemDecorEnable)) {
        WLOGFE("Write bool isSystemDecorEnable failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_GET_SYSTEM_DECOR_ENABLE),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }
    isSystemDecorEnable = reply.ReadBool();
    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

void WindowManagerProxy::NotifyWindowTransition(WindowTransitionInfo from, WindowTransitionInfo to)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("Failed to WriteInterfaceToken!");
        return;
    }

    if (!data.WriteParcelable(&from)) {
        WLOGFE("Failed to write from ability window info!");
        return;
    }

    if (!data.WriteParcelable(&to)) {
        WLOGFE("Failed to write to ability window info!");
        return;
    }

    auto error = Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_NOTIFY_WINDOW_TRANSITION),
        data, reply, option);
    if (error != ERR_NONE) {
        WLOGFE("Send request error: %{public}d", static_cast<uint32_t>(error));
        return;
    }
}

WMError WindowManagerProxy::GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write displayId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_GET_FULLSCREEN_AND_SPLIT_HOT_ZONE),
        data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }

    WMError ret = static_cast<WMError>(reply.ReadInt32());
    if (ret == WMError::WM_OK) {
        hotZones.fullscreen_.posX_ = reply.ReadInt32();
        hotZones.fullscreen_.posY_ = reply.ReadInt32();
        hotZones.fullscreen_.width_ = reply.ReadUint32();
        hotZones.fullscreen_.height_ = reply.ReadUint32();

        hotZones.primary_.posX_ = reply.ReadInt32();
        hotZones.primary_.posY_ = reply.ReadInt32();
        hotZones.primary_.width_ = reply.ReadUint32();
        hotZones.primary_.height_ = reply.ReadUint32();

        hotZones.secondary_.posX_ = reply.ReadInt32();
        hotZones.secondary_.posY_ = reply.ReadInt32();
        hotZones.secondary_.width_ = reply.ReadUint32();
        hotZones.secondary_.height_ = reply.ReadUint32();
    }
    return ret;
}
} // namespace Rosen
} // namespace OHOS