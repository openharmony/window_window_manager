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

#ifndef OHOS_WINDOW_SCENE_WINDOW_EVENT_CHANNEL_STUB_H
#define OHOS_WINDOW_SCENE_WINDOW_EVENT_CHANNEL_STUB_H

#include <map>
#include <iremote_stub.h>
#include "session/container/include/zidl/window_event_channel_interface.h"

namespace OHOS::Rosen {
class WindowEventChannelStub;
using WindowEventChannelStubFunc = int (WindowEventChannelStub::*)(MessageParcel& data, MessageParcel& reply);

class WindowEventChannelStub : public IRemoteStub<IWindowEventChannel> {
public:
    WindowEventChannelStub() = default;
    ~WindowEventChannelStub() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
        MessageOption &option) override;

private:
    static const std::map<uint32_t, WindowEventChannelStubFunc> stubFuncMap_;

    int HandleTransferBackpressedEvent(MessageParcel& data, MessageParcel& reply);
    int HandleTransferKeyEvent(MessageParcel& data, MessageParcel& reply);
    int HandleTransferPointerEvent(MessageParcel& data, MessageParcel& reply);
    int HandleGetApplicationPid(MessageParcel& data, MessageParcel& reply);
    int HandleTransferFocusActiveEvent(MessageParcel& data, MessageParcel& reply);
    int HandleTransferFocusStateEvent(MessageParcel& data, MessageParcel& reply);
    int HandleTransferSearchElementInfo(MessageParcel& data, MessageParcel& reply);
    int HandleTransferSearchElementInfosByText(MessageParcel& data, MessageParcel& reply);
    int HandleTransferFindFocusedElementInfo(MessageParcel& data, MessageParcel& reply);
    int HandleTransferFocusMoveSearch(MessageParcel& data, MessageParcel& reply);
    int HandleTransferExecuteAction(MessageParcel& data, MessageParcel& reply);
};
} // namespace OHOS::Rosen
#endif // OHOS_WINDOW_SCENE_WINDOW_EVENT_CHANNEL_STUB_H
