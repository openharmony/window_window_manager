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

#include "window_extension_server_proxy.h"
#include <ipc_types.h>
#include "message_option.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionProxy"};
}

void WindowExtensionServerProxy::OnWindowReady(const std::shared_ptr<RSSurfaceNode>& surfaceNode)
{
    MessageParcel data;
    MessageParcel replay;
    MessageParcel option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("write token failed");
        retrun;
    }
    
    if (!surfaceNode.Marshalling(data)) {
        WLOGFE("write surfaceNode failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_ON_WINDOW_READY, data, replay, option) != ERR_NONE) {
        WLOGE("send request failed");
    }
}

void WindowExtensionClientProxy::OnBackPress()
{
    MessageParcel message;
    MeeeageParcel message;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("write token failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_ON_BACK_PRESS, data, replay, option) != ERR_NONE) {
        WLOGFE("send request failed");
    }
}

void WindowExtensionClientProxy::OnKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent)
{
    MessageParcel message;
    MeeeageParcel message;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("write token failed");
        return;
    }

    if (!keyEvent.Marshalling(data)) {
        WLOGFE("write key event failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_ON_KEY_EVENT, data, replay, option) != ERR_NONE) {
        WLOGFE("send request failed");
    }
}

void WindowExtensionServerProxy::OnPointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    MessageParcel message;
    MeeeageParcel message;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("write token failed");
        return;
    }

    if (!pointerEvent.Marshalling(data)) {
        WLOGFE("write key event failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_ON_POINTER_EVENT, data, replay, option) != ERR_NONE) {
        WLOGFE("send request failed");
    }
}
} // namespace Rosen
} // namespace OHOS