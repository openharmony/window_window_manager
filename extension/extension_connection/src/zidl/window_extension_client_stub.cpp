/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "window_extension_client_stub.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionClientStub"};
}
int WindowExtensionClientStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        // LCOV_EXCL_START
        WLOGFE("InterfaceToken check failed");
        return ERR_TRANSACTION_FAILED;
        // LCOV_EXCL_STOP
    }
    WLOGI(" code is %{public}d", code);
    switch (code) {
        case TRANS_ID_ON_WINDOW_READY: {
            std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(data);
            OnWindowReady(surfaceNode);
            break;
        }
        case TRANS_ID_ON_BACK_PRESS: {
            OnBackPress();
            break;
        }
        case TRANS_ID_ON_KEY_EVENT: {
            std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
            if (keyEvent == nullptr) {
                // LCOV_EXCL_START
                WLOGFE("create keyevent failed");
                break;
                // LCOV_EXCL_STOP
            }
            keyEvent->ReadFromParcel(data);
            OnKeyEvent(keyEvent);
            break;
        }
        case TRANS_ID_ON_POINTER_EVENT: {
            std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
            if (pointerEvent == nullptr) {
                // LCOV_EXCL_START
                WLOGFE("create pointer event failed");
                break;
                // LCOV_EXCL_STOP
            }
            pointerEvent->ReadFromParcel(data);
            OnPointerEvent(pointerEvent);
            break;
        }
        default: {
            // LCOV_EXCL_START
            WLOGFW("unknown transaction code %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            // LCOV_EXCL_STOP
        }
    }
    return ERR_NONE;
}
} // namespace Rosen
} // namespace OHOS