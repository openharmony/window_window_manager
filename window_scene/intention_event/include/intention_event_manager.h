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

#ifndef OHOS_ROSEN_WINDOW_SCENE_INTENTION_EVENT_MANAGER_H
#define OHOS_ROSEN_WINDOW_SCENE_INTENTION_EVENT_MANAGER_H

#include <mutex>

#include "input_manager.h"
#include "singleton.h"
#include "ui_content.h"

namespace OHOS::AppExecFwk {
class EventHandler;
} // namespace OHOS::AppExecFwk

namespace OHOS::Rosen {
class IntentionEventManager {
    DECLARE_DELAYED_SINGLETON(IntentionEventManager);
public:
    DISALLOW_COPY_AND_MOVE(IntentionEventManager);
    bool EnableInputEventListener(Ace::UIContent* uiContent,
        std::shared_ptr<AppExecFwk::EventHandler>);

private:
class InputEventListener : public MMI::IInputEventConsumer {
public:
    InputEventListener(Ace::UIContent* uiContent, std::shared_ptr<AppExecFwk::EventHandler>
        eventHandler): uiContent_(uiContent), weakEventConsumer_(eventHandler) {}
    virtual ~InputEventListener();
    void OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const override;
    void OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const override;
    void OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const override;
    void RegisterWindowFocusChanged();

private:
    void UpdateLastMouseEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const;
    void ProcessEnterLeaveEvent();
    Ace::UIContent* uiContent_ = nullptr;
    std::weak_ptr<AppExecFwk::EventHandler> weakEventConsumer_;
    mutable std::mutex mouseEventMutex_;
};
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_INTENTION_EVENT_MANAGER_H