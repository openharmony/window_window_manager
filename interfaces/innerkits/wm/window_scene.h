/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNERKITS_WINDOW_SCENE_H
#define INTERFACES_INNERKITS_WINDOW_SCENE_H

#include <refbase.h>
#include <iremote_object.h>

#include "window.h"
#include "window_option.h"

namespace OHOS::AppExecFwk {
    class Configuration;
}

namespace OHOS {
namespace Rosen {
class WindowScene : public RefBase {
public:
    static const DisplayId DEFAULT_DISPLAY_ID = 0;
    static const std::string MAIN_WINDOW_ID;

    WindowScene() = default;
    ~WindowScene();

    WMError Init(DisplayId displayId, const std::shared_ptr<AbilityRuntime::Context>& context,
        sptr<IWindowLifeCycle>& listener, sptr<WindowOption> option = nullptr);

    sptr<Window> CreateWindow(const std::string& windowName, sptr<WindowOption>& option) const;

    const sptr<Window>& GetMainWindow() const;

    WMError GoForeground();
    WMError GoBackground() const;

    WMError RequestFocus() const;
    void UpdateConfiguration(const std::shared_ptr<AppExecFwk::Configuration>& configuration);
    WMError SetSystemBarProperty(WindowType type, const SystemBarProperty& property) const;

    std::string GetContentInfo() const;

private:
    static inline std::atomic<uint32_t> count { 0 };
    sptr<Window> mainWindow_ = nullptr;
    DisplayId displayId_ = DEFAULT_DISPLAY_ID;

    std::shared_ptr<AbilityRuntime::Context> context_ = nullptr;
    std::string GenerateMainWindowName(const std::shared_ptr<AbilityRuntime::Context>& context) const;
};
} // namespace Rosen
} // namespace OHOS
#endif // INTERFACES_INNERKITS_WINDOW_SCENE_H
