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

#ifndef OHOS_WINDOW_SCENE_SESSION_WINDOW_EVENT_CHANNEL_INTERFACE_H
#define OHOS_WINDOW_SCENE_SESSION_WINDOW_EVENT_CHANNEL_INTERFACE_H

#include <iremote_broker.h>
#include <list>

#include "interfaces/include/ws_common.h"

namespace OHOS::MMI {
    class PointerEvent;
    class KeyEvent;
    class AxisEvent;
} // namespace OHOS::MMI
namespace OHOS::Accessibility {
    class AccessibilityElementInfo;
}
namespace OHOS::Rosen {
class IWindowEventChannel : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IWindowEventChannel");

    virtual WSError TransferKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) = 0;
    virtual WSError TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) = 0;

    // transfer sync key event for weather consumed
    virtual WSError TransferBackpressedEventForConsumed(bool& isConsumed) = 0;
    virtual WSError TransferKeyEventForConsumed(const std::shared_ptr<MMI::KeyEvent>& keyEvent, bool& isConsumed) = 0;
    virtual WSError TransferFocusActiveEvent(bool isFocusActive) = 0;
    virtual WSError TransferFocusState(bool focusState) = 0;
    virtual WSError TransferSearchElementInfo(int32_t elementId, int32_t mode, int32_t baseParent,
        std::list<Accessibility::AccessibilityElementInfo>& infos) = 0;
    virtual WSError TransferSearchElementInfosByText(int32_t elementId, const std::string& text, int32_t baseParent,
        std::list<Accessibility::AccessibilityElementInfo>& infos) = 0;
    virtual WSError TransferFindFocusedElementInfo(int32_t elementId, int32_t focusType, int32_t baseParent,
        Accessibility::AccessibilityElementInfo& info) = 0;
    virtual WSError TransferFocusMoveSearch(int32_t elementId, int32_t direction, int32_t baseParent,
        Accessibility::AccessibilityElementInfo& info) = 0;
};
} // namespace OHOS::Rosen
#endif // OHOS_WINDOW_SCENE_SESSION_WINDOW_EVENT_CHANNEL_INTERFACE_H
