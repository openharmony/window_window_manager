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
#include "ipc_skeleton.h"
#include "window.h"
#include "window_manager.h"
#include "window_option.h"
#include "window_scene.h"
#include "window_test_utils.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class WindowSystemToastWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static inline float virtualPixelRatio_ = 1.0;
    static inline Rect displayRect_ {0, 0, 0, 0};
    static inline std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_ = nullptr;
};

void WindowSystemToastWindowTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    displayRect_.width_ = display->GetWidth();
    displayRect_.height_ = display->GetHeight();
    WindowTestUtils::InitByDisplayRect(displayRect_);
    virtualPixelRatio_ = WindowTestUtils::GetVirtualPixelRatio(0);
}

void WindowSystemToastWindowTest::TearDownTestCase()
{
}

void WindowSystemToastWindowTest::SetUp()
{
}

void WindowSystemToastWindowTest::TearDown()
{
}

static sptr<WindowScene> CreateWindowScene()
{
    sptr<IWindowLifeCycle> listener = nullptr;
    WindowSystemToastWindowTest::abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();

    sptr<WindowScene> scene = new WindowScene();
    scene->Init(0, WindowSystemToastWindowTest::abilityContext_, listener);
    return scene;
}

static sptr<Window> CreateSystemToastWindow(WindowType type, Rect rect, std::string name = "")
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(type);
    option->SetWindowRect(rect);

    static int cnt = 0;
    std::string winName = (name == "") ? "systemToastWindowTest" + std::to_string(cnt++) : name;

    return Window::Create(winName, option, WindowSystemToastWindowTest::abilityContext_);
}

static inline Rect GetRectWithVpr(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    auto vpr = WindowSystemToastWindowTest::virtualPixelRatio_;
    return {x, y, static_cast<uint32_t>(w * vpr), static_cast<uint32_t>(h * vpr)};
}

/**
 * @tc.name: SystemToastWindow01
 * @tc.desc: SystemToastWindow life cycle
 * @tc.type: FUNC
 */
HWTEST_F(WindowSystemToastWindowTest, SystemToastWindow01, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateSystemToastWindow(WindowType::WINDOW_TYPE_SYSTEM_TOAST, fltWindRect);
    ASSERT_NE(nullptr, fltWin);
    if (scene->GoForeground() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    }

    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());
    if (scene->GoForeground() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    } else {
        ASSERT_NE(WMError::WM_OK, scene->GoForeground());
    }

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
    }

/**
 * @tc.name: SystemToastWindow02
 * @tc.desc: SystemToastWindow life cycle, main window hide first
 * @tc.type: FUNC
 */
HWTEST_F(WindowSystemToastWindowTest, SystemToastWindow02, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateSystemToastWindow(WindowType::WINDOW_TYPE_SYSTEM_TOAST, fltWindRect);
    if (fltWin == nullptr) {
        return;
    }
    ASSERT_NE(nullptr, fltWin);
    if (scene->GoForeground() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    } else {
        ASSERT_NE(WMError::WM_OK, scene->GoForeground());
    }

    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    if (scene->GoForeground() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    } else {
        ASSERT_NE(WMError::WM_OK, scene->GoForeground());
    }

    if (scene->GetMainWindow() == nullptr) {
        return;
    }
    ASSERT_EQ(true, fltWin->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: SystemToastWindow03
 * @tc.desc: SystemToastWindow life cycle, app floating window hide first
 * @tc.type: FUNC
 */
HWTEST_F(WindowSystemToastWindowTest, SystemToastWindow03, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateSystemToastWindow(WindowType::WINDOW_TYPE_SYSTEM_TOAST, fltWindRect);
    if (fltWin == nullptr) {
        return;
    }
    ASSERT_NE(nullptr, fltWin);
    if (scene->GoForeground() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    } else {
        ASSERT_NE(WMError::WM_OK, scene->GoForeground());
    }

    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());
    ASSERT_EQ(false, fltWin->GetWindowState() == WindowState::STATE_SHOWN);
    if (scene->GetMainWindow() == nullptr) {
        return;
    }
    ASSERT_EQ(true, scene->GetMainWindow()->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: SystemToastWindow04
 * @tc.desc: SystemToastWindow life cycle, main window destroy first
 * @tc.type: FUNC
 */
HWTEST_F(WindowSystemToastWindowTest, SystemToastWindow04, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateSystemToastWindow(WindowType::WINDOW_TYPE_SYSTEM_TOAST, fltWindRect);
    if (fltWin == nullptr) {
        return;
    }
    ASSERT_NE(nullptr, fltWin);

    if (scene->GoForeground() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
        ASSERT_EQ(WMError::WM_OK, fltWin->Show());
        ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
    } else {
        ASSERT_NE(WMError::WM_OK, scene->GoForeground());
    }

    if (scene->GetMainWindow() == nullptr) {
        return;
    }
    ASSERT_EQ(nullptr, scene->GetMainWindow());
    ASSERT_EQ(false, fltWin->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
}
}
}