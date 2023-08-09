/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>
#include "ability_context_impl.h"
#include "window.h"
#include "window_manager.h"
#include "window_option.h"
#include "window_scene.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowDialogWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static inline std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_ = nullptr;
};

void WindowDialogWindowTest::SetUpTestCase()
{
}

void WindowDialogWindowTest::TearDownTestCase()
{
}

void WindowDialogWindowTest::SetUp()
{
}

void WindowDialogWindowTest::TearDown()
{
}

static sptr<WindowScene> CreateWindowScene()
{
    sptr<IWindowLifeCycle> listener = nullptr;
    WindowDialogWindowTest::abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();

    sptr<WindowScene> scene = new WindowScene();
    scene->Init(0, WindowDialogWindowTest::abilityContext_, listener);
    return scene;
}

static sptr<Window> CreateDialogWindow(sptr<WindowScene> scene, WindowType type, Rect rect, std::string name = "")
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(type);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect(rect);

    static int cnt = 0;
    std::string winName = (name == "") ? "DialogWindowTest" + std::to_string(cnt++) : name;

    return Window::Create(winName, option, scene->GetMainWindow()->GetContext());
}
} // namespace Rosen
} // namespace OHOS
