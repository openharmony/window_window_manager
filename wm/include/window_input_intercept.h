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

#ifndef OHOS_ROSEN_WINDOW_INPUT_INTERCEPT_H
#define OHOS_ROSEN_WINDOW_INPUT_INTERCEPT_H

#include "input_manager.h"
#include "window_input_intercept_consumer.h"
#include "wm_single_instance.h"
#include <memory>
#include <mutex>

namespace OHOS {
namespace Rosen {

class WindowInputIntercept {
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowInputIntercept);
public:
    void RegisterInputEventIntercept(const int32_t deviceId,
                                     const std::shared_ptr<IInputEventInterceptConsumer> &consumer);
    void UnRegisterInputEventIntercept(const int32_t deviceId);
    bool IsInputIntercept(const std::shared_ptr<MMI::KeyEvent> &keyEvent);
    bool IsInputIntercept(const std::shared_ptr<MMI::PointerEvent> &pointerEvent);
public:
    WindowInputIntercept() = default;
    ~WindowInputIntercept() = default;

private:
    std::unordered_map<int32_t, std::shared_ptr<IInputEventInterceptConsumer>> inputIntercept_;
    std::mutex registerMutex_;
};
}
}

#endif //OHOS_ROSEN_WINDOW_INPUT_INTERCEPT_H
