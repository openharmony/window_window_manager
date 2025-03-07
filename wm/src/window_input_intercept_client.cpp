/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "window_input_intercept_client.h"
#include "input_transfer_station.h"
#include "window_input_intercept.h"

namespace OHOS {
namespace Rosen {

void WindowInputInterceptClient::RegisterInputEventIntercept(const int32_t deviceId,
    const std::shared_ptr<IInputEventInterceptConsumer> &consumer)
{
    WindowInputIntercept::GetInstance().RegisterInputEventIntercept(deviceId, consumer);
}

void WindowInputInterceptClient::UnRegisterInputEventIntercept(const int32_t deviceId)
{
    WindowInputIntercept::GetInstance().UnRegisterInputEventIntercept(deviceId);
}

void WindowInputInterceptClient::SendInputEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent)
{
    InputTransferStation::GetInstance().HandleInputEvent(keyEvent);
}

void WindowInputInterceptClient::SendInputEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent)
{
    InputTransferStation::GetInstance().HandleInputEvent(pointerEvent);
}
}
}