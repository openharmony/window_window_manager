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

#ifndef OHOS_SESSION_INTERFACE_H
#define OHOS_SESSION_INTERFACE_H

#include <i_input_event_consumer.h>
#include "window_scene_common.h"

namespace OHOS::Rosen {
class SessionStage {
public:
    SessionStage() = default;
    ~SessionStage() = default;

    WSError Connect() const;
    WSError Foreground() const;
    WSError Background() const;
    WSError Disconnect() const;
    bool RegisterInputEventConsumer(const std::shared_ptr<IInputEventConsumer>& inputEventConsumer);

private:
    std::shared_ptr<IInputEventConsumer> inputEventConsumer_;

};
}
#endif // OHOS_SESSION_INTERFACE_H
