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

#ifndef OHOS_ROSEN_WINDOW_INNER_MANAGER_H
#define OHOS_ROSEN_WINDOW_INNER_MANAGER_H

#include <refbase.h>
#include "event_handler.h"
#include "event_runner.h"

#include "wm_common.h"
#include "wm_single_instance.h"

namespace OHOS {
namespace Rosen {
class WindowInnerManager : public RefBase {
WM_DECLARE_SINGLE_INSTANCE_BASE(WindowInnerManager);
using EventRunner = OHOS::AppExecFwk::EventRunner;
using EventHandler = OHOS::AppExecFwk::EventHandler;
public:
    void Start();
    void Stop();
    void CreateWindow(std::string name, WindowType type, Rect rect);
    void DestroyWindow();
public:
    enum class InnerWMRunningState {
        STATE_NOT_START,
        STATE_RUNNING,
    };
    ~WindowInnerManager();

private:
    WindowInnerManager();
    bool Init();
    void HandleCreateWindow(std::string name, WindowType type, Rect rect);
    void HandleDestroyWindow();

private:
    int32_t dialogId_ = -1;
    std::shared_ptr<EventHandler> eventHandler_;
    std::shared_ptr<EventRunner> eventLoop_;
    InnerWMRunningState state_;
    std::string dividerParams_ = "";
    const std::string INNER_WM_THREAD_NAME = "inner_window_manager";
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_INNER_MANAGER_H