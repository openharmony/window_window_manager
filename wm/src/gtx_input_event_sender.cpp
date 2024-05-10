/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtx_input_event_sender.h"

namespace OHOS {

GtxInputEventSender& GtxInputEventSender::GetInstance()
{
    static GtxInputEventSender instance;
    return instance;
}

void GtxInputEventSender::GetTouchEvent(GtxTouchEventInfo& touchEvent)
{
    std::unique_lock<std::mutex> lock(mEventMutex);
    touchEvent = mEvent;
}

void GtxInputEventSender::SetTouchEvent(Rosen::Rect rect,
    std::shared_ptr<MMI::PointerEvent> pointerEvent)
{
    if (!mIsEnable.load()) {
        return;
    }
    if (pointerEvent == nullptr) {
        return;
    }

    std::unique_lock<std::mutex> lock(mEventMutex);
    mEvent = {};
    mEvent.windowId = pointerEvent->GetAgentWindowId();
    mEvent.pointerId = pointerEvent->GetPointerId();
    mEvent.extent = { rect.posX_, rect.posY_, rect.width_, rect.height_ };

    std::vector<int32_t> pointIndex = pointerEvent->GetPointerIds();
    mEvent.numPoints = std::min(pointIndex.size(), static_cast<size_t>(GTX_MAX_TOUCH_POINTS_NUMBER));

    for (uint32_t i = 0; i < mEvent.numPoints; i++) {
        GtxTouchPoint& dstTouchPoint = mEvent.touchPoints[i];
        MMI::PointerEvent::PointerItem srcTouchPoint;
        pointerEvent->GetPointerItem(pointIndex[i], srcTouchPoint);

        dstTouchPoint.id = pointIndex[i];
        dstTouchPoint.screenX = srcTouchPoint.GetDisplayX();
        dstTouchPoint.screenY = srcTouchPoint.GetDisplayY();
        dstTouchPoint.x = srcTouchPoint.GetWindowX();
        dstTouchPoint.y = srcTouchPoint.GetWindowY();
        dstTouchPoint.isPressed = srcTouchPoint.IsPressed();
        dstTouchPoint.pressure = srcTouchPoint.GetPressure();
    }
}

extern "C" {
__attribute__((visibility("default"))) void SetGtxTouchEventStatus(bool isEnable)
{
    OHOS::GtxInputEventSender::GetInstance().SetOpt(isEnable);
}

__attribute__((visibility("default"))) void GetGtxTouchEvent(OHOS::GtxTouchEventInfo& touchEvent)
{
    OHOS::GtxInputEventSender::GetInstance().GetTouchEvent(touchEvent);
}
}

} // namespace OHOS
