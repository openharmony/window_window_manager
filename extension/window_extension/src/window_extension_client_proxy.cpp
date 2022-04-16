/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "window_extension_client_proxy.h"
#include <ipc_types.h>
#include "message_option.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionClientProxy"};
}

void WindowExtensionClientProxy::Resize(Rect rect)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("write interface token failed");
        return;
    }
    if (!(data.WriteInt32(rect.posX_) && data.WriteInt32(rect.posY_) &&
        data.WriteInt32(rect.height_) && data.WriteInt32(rect.width_))) {
        WLOGFE("write rect failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_RESIZE_WINDOW, data, reply, option) != ERR_NONE) {
        WLOGFE("send request failed");
        return;
    }
}

void WindowExtensionClientProxy::Hide()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("write interface token failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_HIDE_WINDOW, data, reply, option) != ERR_NONE) {
        WLOGFE("send request failed");
    }
}

void WindowExtensionClientProxy::Show()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("write interface token failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_SHOW_WINDOW, data, reply, option) != ERR_NONE) {
        WLOGFE("send request failed");
    }
}

void WindowExtensionClientProxy::ConnectToClient(sptr<IWindowExtensionServer>& token)
{
    MessageParcel data;
    MessageParcel replay;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("write interface token failed");
        return;
    }
    if (token->AsObject()) {
        WLOGFE("AsObject is null");
    }
    if (!data.WriteRemoteObject(token->AsObject())) {
        WLOGFE("write object failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_CONNECT_CLIENT, data, replay, option) != ERR_NONE) {
        WLOGFE("send request failed");
    }
}

void WindowExtensionClientProxy::RequestFocus()
{
    WLOGFI("called.");
}
} // namespace Rosen
} // namespace OHOS