/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <memory>

#include "hilog_tag_wrapper.h"
#include "session_manager/include/zidl/session_lifecycle_listener_stub.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {

int SessionLifecycleListenerStub::OnRemoteRequest(
    uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        TLOGE(WmsLogTag::WMS_LIFE, "local descriptor not equal to remote.");
        return ERR_INVALID_STATE;
    }
    return HandleOnLifecycleEvent(data, reply);
}

int SessionLifecycleListenerStub::HandleOnLifecycleEvent(MessageParcel& data, MessageParcel& reply)
{
    TLOGD(WmsLogTag::WMS_LIFE, "in");
    int32_t event = 0;
    if (!data.ReadInt32(event)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Failed to read event");
        return ERR_INVALID_DATA;
    }
    if (event < static_cast<int32_t>(SessionLifecycleEvent::CREATED) ||
        event >= static_cast<int32_t>(SessionLifecycleEvent::EVENT_END)) {
        TLOGE(WmsLogTag::WMS_LIFE, "Invalid lifecycle event");
        return ERR_INVALID_DATA;
    }
    std::unique_ptr<LifecycleEventPayload> payload(data.ReadParcelable<LifecycleEventPayload>());
    if (!payload) {
        TLOGE(WmsLogTag::WMS_LIFE, "Invalid payload");
        return ERR_INVALID_DATA;
    }
    OnLifecycleEvent(static_cast<SessionLifecycleEvent>(event), *payload);
    return 0;
}
}
