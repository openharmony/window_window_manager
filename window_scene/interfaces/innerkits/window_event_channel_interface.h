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
 
#ifndef OHOS_WINDOW_EVENT_CHANNEL_INTERFACE_H
#define OHOS_WINDOW_EVENT_CHANNEL_INTERFACE_H

#include <iremote_broker.h>
#include "window_scene_common.h"

namespace OHOS::MMI {
    class PointerEvent;
    class KeyEvent;
    class AxisEvent;
}

namespace OHOS::Rosen {
class IWindowEventChannel : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IWindowEventChannel");

    enum class WindowEventChannelMessage : uint32_t {
        TRANS_ID_NOTIFY_SIZE_CHANGE,
        TRANS_ID_TRANSFER_KEY_EVENT,
        TRANS_ID_TRANSFER_POINTER_EVENT,
    };

    virtual WSError NotifySizeChange(const WSRect& rect) = 0;
    virtual WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) = 0;
    virtual WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) = 0;
};
}
#endif // OHOS_WINDOW_EVENT_CHANNEL_INTERFACE_H