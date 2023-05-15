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

#ifndef OHOS_ROSEN_ROOT_SCENE_H
#define OHOS_ROSEN_ROOT_SCENE_H

#include <mutex>

#include "window.h"

namespace OHOS::AppExecFwk {
class EventHandler;
} // namespace OHOS::AppExecFwk

namespace OHOS::Ace {
class UIContent;
} // namespace OHOS::Ace

namespace OHOS {
namespace Rosen {
class RootScene : public Window {
public:
    RootScene();
    virtual ~RootScene();

    void LoadContent(const std::string& contentUrl,
        NativeEngine* engine, NativeValue* storage, AbilityRuntime::Context* context);
    void UpdateViewportConfig(const Rect& rect, WindowSizeChangeReason reason);

    void RequestVsync(const std::shared_ptr<VsyncCallback>& vsyncCallback) override;

    void ConsumePointerEvent(const std::shared_ptr<MMI::PointerEvent>& inputEvent) override;
    void ConsumeKeyEvent(std::shared_ptr<MMI::KeyEvent>& inputEvent) override;

    void SetDisplayDensity(float density)
    {
        density_ = density;
    }

private:
    void RegisterInputEventListener();

    std::unique_ptr<Ace::UIContent> uiContent_;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_;

    std::recursive_mutex mutex_;

    float density_ = 1.0f;
};
} // namespace Rosen
} // namespace OHOS

#endif // OHOS_ROSEN_ROOT_SCENE_H
