/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "window_manager_proxy.h"
#include <ipc_types.h>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerProxy"};
}


WMError WindowManagerProxy::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId)
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

    if (Remote()->SendRequest(TRANS_ID_CREATE_WINDOW, data, reply, option) != ERR_NONE) {
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

    if (Remote()->SendRequest(TRANS_ID_ADD_WINDOW, data, reply, option) != ERR_NONE) {
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

    if (Remote()->SendRequest(TRANS_ID_REMOVE_WINDOW, data, reply, option) != ERR_NONE) {
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

    if (Remote()->SendRequest(TRANS_ID_DESTROY_WINDOW, data, reply, option) != ERR_NONE) {
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
    if (Remote()->SendRequest(TRANS_ID_REQUEST_FOCUS, data, reply, option) != ERR_NONE) {
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
    if (Remote()->SendRequest(TRANS_ID_SET_BACKGROUND_BLUR, data, reply, option) != ERR_NONE) {
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
    if (Remote()->SendRequest(TRANS_ID_SET_APLPHA, data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }

    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

WMError WindowManagerProxy::SaveAbilityToken(const sptr<IRemoteObject>& abilityToken, uint32_t windowId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteRemoteObject(abilityToken)) {
        WLOGFE("Write abilityToken failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint32(windowId)) {
        WLOGFE("Write windowId failed");
        return WMError::WM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(TRANS_ID_SEND_ABILITY_TOKEN, data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }

    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}

void WindowManagerProxy::SetBrightness(uint32_t windowId, float brightness)
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
    if (!data.WriteFloat(brightness)) {
        WLOGFE("Write brightness failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerMessage::TRANS_ID_SET_BRIGHTNESS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

std::vector<Rect> WindowManagerProxy::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

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

    if (Remote()->SendRequest(TRANS_ID_GET_AVOID_AREA, data, reply, option) != ERR_NONE) {
        return avoidArea;
    }

    uint32_t avoidNum = reply.ReadUint32();
    for (uint32_t i = 0; i < avoidNum; ++i) {
        int32_t x = reply.ReadInt32();
        int32_t y = reply.ReadInt32();
        uint32_t w = reply.ReadUint32();
        uint32_t h = reply.ReadUint32();
        Rect rect = {x, y, w, h};
        avoidArea.push_back(rect);
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

    if (Remote()->SendRequest(TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT, data, reply, option) != ERR_NONE) {
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

    if (Remote()->SendRequest(TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
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
    if (Remote()->SendRequest(TRANS_ID_PROCESS_POINT_DOWN, data, reply, option) != ERR_NONE) {
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
    if (Remote()->SendRequest(TRANS_ID_PROCESS_POINT_UP, data, reply, option) != ERR_NONE) {
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
    if (Remote()->SendRequest(TRANS_ID_MINIMIZE_ALL_APP_WINDOWS, data, reply, option) != ERR_NONE) {
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
    if (Remote()->SendRequest(TRANS_ID_MAXMIZE_WINDOW, data, reply, option) != ERR_NONE) {
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
    if (Remote()->SendRequest(TRANS_ID_UPDATE_LAYOUT_MODE, data, reply, option) != ERR_NONE) {
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

    if (Remote()->SendRequest(TRANS_ID_UPDATE_PROPERTY, data, reply, option) != ERR_NONE) {
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

    if (Remote()->SendRequest(TRANS_ID_GET_TOP_WINDOW_ID, data, reply, option) != ERR_NONE) {
        return WMError::WM_ERROR_IPC_FAILED;
    }
    topWinId = reply.ReadUint32();
    int32_t ret = reply.ReadInt32();
    return static_cast<WMError>(ret);
}
} // namespace Rosen
} // namespace OHOS