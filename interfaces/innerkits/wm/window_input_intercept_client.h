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

#ifndef OHOS_ROSEN_WINDOW_INPUT_INTERCEPT_CLINET_H
#define OHOS_ROSEN_WINDOW_INPUT_INTERCEPT_CLINET_H

#include "window.h"
#include "window_input_intercept_consumer.h"

namespace OHOS {
namespace Rosen {
/**
 * @class WindowInputInterceptClient
 *
 * @brief Used by the GameControllerKit to intercept input events.
 */
class WindowInputInterceptClient {
public:
    /**
     * Registers the interception of input events.
     * @param deviceId deviceId
     * @param consumer Intercept consumer of the input event
     */
    static void RegisterInputEventIntercept(const int32_t deviceId,
                                            const std::shared_ptr<IInputEventInterceptConsumer> &consumer);

    /**
     * Unregister the interception of input events
     * @param deviceId deviceId
     */
    static void UnRegisterInputEventIntercept(const int32_t deviceId);

    /**
     * Send key event
     * @param keyEvent KeyEvent of Multi-Model Input.
     */
    static void SendInputEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent);

    /**
     * Send pointer event
     * @param pointerEvent PointerEvent of Multi-Model Input.
     */
    static void SendInputEvent(const std::shared_ptr<MMI::PointerEvent> &pointerEvent);
};
}
}

#endif //OHOS_ROSEN_WINDOW_INPUT_INTERCEPT_CLINET_H
