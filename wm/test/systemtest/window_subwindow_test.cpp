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

// gtest
#include <gtest/gtest.h>
#include <ability_context.h>
#include "window.h"
#include "window_life_cycle_interface.h"
#include "window_option.h"
#include "window_scene.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSubWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowSubWindowTest::SetUpTestCase()
{
}

void WindowSubWindowTest::TearDownTestCase()
{
}

void WindowSubWindowTest::SetUp()
{
}

void WindowSubWindowTest::TearDown()
{
}

static sptr<WindowScene> CreateWindowScene()
{
    sptr<IWindowLifeCycle> listener = nullptr;
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;

    sptr<WindowScene> scene = new WindowScene();
    scene->Init(0, abilityContext, listener);
    return scene;
}

static sptr<Window> CreateSubWindow(sptr<WindowScene> scene, WindowType type,
    WindowMode mode, struct Rect rect, uint32_t flags)
{
    sptr<WindowOption> subOp = new WindowOption();
    subOp->SetWindowType(type);
    subOp->SetWindowMode(mode);
    subOp->SetWindowRect(rect);
    subOp->SetWindowFlags(flags);

    static int cnt = 0;
    return scene->CreateWindow("SubWindow" + std::to_string(cnt++), subOp);
}

/**
 * @tc.name: SubWindow01
 * @tc.desc: FullScreen Main Window + Floating SubWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSubWindowTest, SubWindow01, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = 0;
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
        WindowMode::WINDOW_MODE_FLOATING, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
}

/**
 * @tc.name: SubWindow02
 * @tc.desc: FullScreen Main Window + Floating SubWindow & Parent Limit work
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSubWindowTest, SubWindow02, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
        WindowMode::WINDOW_MODE_FLOATING, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
}

/**
 * @tc.name: SubWindow03
 * @tc.desc: FullScreen Main Window + Floating MediaWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSubWindowTest, SubWindow03, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 2000, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_MEDIA,
        WindowMode::WINDOW_MODE_FLOATING, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
}

/**
 * @tc.name: SubWindow04
 * @tc.desc: FullScreen Main Window + Floating MediaWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSubWindowTest, SubWindow04, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 2000, 3000, 2000};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_MEDIA,
        WindowMode::WINDOW_MODE_FLOATING, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
}

/**
 * @tc.name: SubWindow05
 * @tc.desc: FullScreen Main Window + Floating MediaWindow + Floating SubWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSubWindowTest, SubWindow05, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_MEDIA,
        WindowMode::WINDOW_MODE_FLOATING, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    sptr<Window> subWindow2 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
        WindowMode::WINDOW_MODE_FLOATING, rect, flags);
    ASSERT_NE(nullptr, subWindow2);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow2->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, subWindow2->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
}

/**
 * @tc.name: SubWindow06
 * @tc.desc: FullScreen Main Window + FullScreen SubWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSubWindowTest, SubWindow06, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
        WindowMode::WINDOW_MODE_FULLSCREEN, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
}

/**
 * @tc.name: SubWindow07
 * @tc.desc: FullScreen Main Window + Floating SubWindow & MainWindow Fisrt GoBackground
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSubWindowTest, SubWindow07, Function | MediumTest | Level4)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
        WindowMode::WINDOW_MODE_FLOATING, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
}

/**
 * @tc.name: SubWindow08
 * @tc.desc: FullScreen Main Window + Floating SubWindow & only show SubWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSubWindowTest, SubWindow08, Function | MediumTest | Level4)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
        WindowMode::WINDOW_MODE_FLOATING, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, subWindow->Show());
}

/**
 * @tc.name: SubWindow09
 * @tc.desc: FullScreen Main Window + Floating SubWindow & first destroy SubWindow, then destroy MainWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSubWindowTest, SubWindow09, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
        WindowMode::WINDOW_MODE_FLOATING, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    ASSERT_EQ(WMError::WM_OK, subWindow->Destroy());
}

/**
 * @tc.name: SubWindow10
 * @tc.desc: FullScreen Main Window + Floating SubWindow & first destroy MainWindow, then destroy SubWindow
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowSubWindowTest, SubWindow10, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
        WindowMode::WINDOW_MODE_FLOATING, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    sptr<Window> mainWindow = scene->GetMainWindow();
    ASSERT_EQ(WMError::WM_OK, mainWindow->Destroy());
    ASSERT_EQ(WMError::WM_ERROR_DESTROYED_OBJECT, subWindow->Destroy());
}
} // namespace Rosen
} // namespace OHOS
