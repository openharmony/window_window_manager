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

// gtest
#include <gtest/gtest.h>
#include "ability_context_impl.h"
#include "window.h"
#include "window_option.h"
#include "window_scene.h"
#include "window_test_utils.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowAppFloatingWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static inline float virtualPixelRatio_ = 1.0;
    static inline std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_ = nullptr;
};

void WindowAppFloatingWindowTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    WindowTestUtils::InitByDisplayRect(displayRect);
    virtualPixelRatio_ = WindowTestUtils::GetVirtualPixelRatio(0);
}

void WindowAppFloatingWindowTest::TearDownTestCase()
{
}

void WindowAppFloatingWindowTest::SetUp()
{
}

void WindowAppFloatingWindowTest::TearDown()
{
}

static sptr<WindowScene> CreateWindowScene()
{
    sptr<IWindowLifeCycle> listener = nullptr;
    WindowAppFloatingWindowTest::abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();

    sptr<WindowScene> scene = new WindowScene();
    scene->Init(0, WindowAppFloatingWindowTest::abilityContext_, listener);
    return scene;
}

static sptr<Window> CreateAppFloatingWindow(WindowType type, Rect rect, std::string name = "")
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(type);
    option->SetWindowRect(rect);

    static int cnt = 0;
    std::string winName = (name == "") ? "FloatingWindowTest" + std::to_string(cnt++) : name;

    return Window::Create(winName, option, WindowAppFloatingWindowTest::abilityContext_);
}

static inline Rect GetRectWithVpr(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    auto vpr = WindowAppFloatingWindowTest::virtualPixelRatio_;
    return {x, y, static_cast<uint32_t>(w * vpr), static_cast<uint32_t>(h * vpr)};
}

/**
 * @tc.name: AppFloatingWindow01
 * @tc.desc: AppFloatingWindow life cycle
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow01, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow02
 * @tc.desc: AppFloatingWindow life cycle, main window hide first
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow02, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    ASSERT_EQ(false, scene->GetMainWindow()->GetShowState());
    ASSERT_EQ(true, fltWin->GetShowState());
    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow03
 * @tc.desc: AppFloatingWindow life cycle, app floating window hide first
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow03, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());
    ASSERT_EQ(false, fltWin->GetShowState());
    ASSERT_EQ(true, scene->GetMainWindow()->GetShowState());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow04
 * @tc.desc: AppFloatingWindow life cycle, main window destroy first
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow04, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
    ASSERT_EQ(nullptr, scene->GetMainWindow());
    ASSERT_EQ(false, fltWin->GetShowState());
    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
}

/**
 * @tc.name: AppFloatingWindow05
 * @tc.desc: Camera AppFloatingWindow life cycle
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow05, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow06
 * @tc.desc: Camera AppFloatingWindow life cycle, main window hide first
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow06, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    ASSERT_EQ(false, scene->GetMainWindow()->GetShowState());
    ASSERT_EQ(true, fltWin->GetShowState());
    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow07
 * @tc.desc: Camera AppFloatingWindow life cycle, app floating window hide first
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow07, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());
    ASSERT_EQ(false, fltWin->GetShowState());
    ASSERT_EQ(true, scene->GetMainWindow()->GetShowState());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow08
 * @tc.desc: Camera AppFloatingWindow life cycle, main window destroy first
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow08, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
    ASSERT_EQ(nullptr, scene->GetMainWindow());
    ASSERT_EQ(false, fltWin->GetShowState());
    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
}

/**
 * @tc.name: AppFloatingWindow09
 * @tc.desc: Camera AppFloatingWindow rect check
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow09, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(10, 20, 10, 10);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    Rect exceptRect = WindowTestUtils::GetFloatingLimitedRect(fltWindRect, virtualPixelRatio_);
    ASSERT_TRUE(WindowTestUtils::RectEqualTo(fltWin, exceptRect));

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}
} // namespace Rosen
} // namespace OHOS
