/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
#include "common_test_utils.h"
#include "mock_session.h"
#include "session/host/include/scene_session.h"
#include "window_adapter.h"
#include "window_scene_session_impl.h"
#include "window_test_utils.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
class WindowLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    DisplayId displayId_ = 0;
    std::vector<sptr<Window>> activeWindows_;
    static vector<Rect> fullScreenExpecteds_;
    static inline float virtualPixelRatio_ = 0.0;

private:
    static constexpr uint32_t WAIT_SYANC_US = 100000;
    static constexpr uint32_t WAIT_SERVERAL_FRAMES = 36000;
    static constexpr uint32_t WAIT_SYANC_S = 2; // second;
    static void InitAvoidArea();
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
};

vector<Rect> WindowLayoutTest::fullScreenExpecteds_;

void WindowLayoutTest::SetUpTestCase()
{
    SingletonContainer::Get<WindowAdapter>().MinimizeAllAppWindows(0);
    sleep(WAIT_SYANC_S);
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_NE(display, nullptr);
    ASSERT_TRUE((display != nullptr));
    Rect displayRect = { 0, 0, display->GetWidth(), display->GetHeight() };
    Utils::InitByDisplayRect(displayRect);

    virtualPixelRatio_ = WindowTestUtils::GetVirtualPixelRatio(0);

    // calc expected rects
    Rect expected = {
        // 0. only statusBar
        0,
        Utils::statusBarRect_.height_,
        Utils::displayRect_.width_,
        Utils::displayRect_.height_ - Utils::statusBarRect_.height_,
    };
    fullScreenExpecteds_.push_back(expected);
    expected = {
        // 1. both statusBar and naviBar
        0,
        Utils::statusBarRect_.height_,
        Utils::displayRect_.width_,
        Utils::displayRect_.height_ - Utils::statusBarRect_.height_ - Utils::naviBarRect_.height_,
    };
    fullScreenExpecteds_.push_back(expected);
    expected = {
        // 2. only naviBar
        0,
        0,
        Utils::displayRect_.width_,
        Utils::displayRect_.height_ - Utils::naviBarRect_.height_,
    };
    fullScreenExpecteds_.push_back(expected);
    InitAvoidArea();
    sleep(WAIT_SYANC_S);
}

void WindowLayoutTest::InitAvoidArea()
{
    Utils::TestWindowInfo info = {
        .name = "avoidArea",
        .rect = { 0, 0, 0, 0 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
    const sptr<Window>& window = Utils::CreateTestWindow(info);
    ASSERT_NE(window, nullptr);
    window->Show();
    window->SetLayoutFullScreen(true);
    window->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM, WindowTestUtils::systemAvoidArea_);
    window->Hide();
    window->Destroy();
}

void WindowLayoutTest::TearDownTestCase() {}

void WindowLayoutTest::SetUp()
{
    activeWindows_.clear();
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    CommonTestUtils::GuaranteeFloatWindowPermission("wms_window_app_floating_window_test");
}

void WindowLayoutTest::TearDown()
{
    for (auto window : activeWindows_) {
        window->Destroy();
    }
    sleep(WAIT_SYANC_S);
    abilityContext_ = nullptr;
}

namespace {
/**
 * @tc.name: LayoutWindow01
 * @tc.desc: One FLOATING APP Window with on custom rect
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow01, TestSize.Level1)
{
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::TILE);
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::TILE);
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);

    Utils::TestWindowInfo info = {
        .name = "main1",
        .rect = { 0, 0, 0, 0 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
    const sptr<Window>& window = Utils::CreateTestWindow(info);
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(true, window != nullptr);
    activeWindows_.push_back(window);
    Rect expect = Utils::GetDefaultFloatingRect(window, true);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_TRUE(Utils::RectEqualTo(window, Utils::GetFloatingLimitedRect(expect, virtualPixelRatio_)));
    ASSERT_EQ(WMError::WM_OK, window->Hide());
}

/**
 * @tc.name: LayoutWindow02
 * @tc.desc: One FLOATING APP Window
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow02, TestSize.Level1)
{
    Rect res = Utils::GetFloatingLimitedRect(Utils::customAppRect_, virtualPixelRatio_);
    Utils::TestWindowInfo info = {
        .name = "main2",
        .rect = res,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
    const sptr<Window>& window = Utils::CreateTestWindow(info);
    ASSERT_NE(window, nullptr);
    activeWindows_.push_back(window);

    ASSERT_EQ(WMError::WM_OK, window->Show());
    if (window->IsDecorEnable()) {
        ASSERT_TRUE(Utils::RectEqualTo(window, Utils::GetDecorateRect(res, virtualPixelRatio_)));
    } else {
        ASSERT_TRUE(Utils::RectEqualTo(window, res));
    }
    ASSERT_EQ(WMError::WM_OK, window->Hide());
}

/**
 * @tc.name: LayoutWindow04
 * @tc.desc: One FLOATING APP Window & One StatusBar Window
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow04, TestSize.Level1)
{
    // app window
    Rect res = Utils::GetFloatingLimitedRect(Utils::customAppRect_, virtualPixelRatio_);
    Utils::TestWindowInfo info = {
        .name = "main4",
        .rect = res,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
    sptr<Window> appWin = Utils::CreateTestWindow(info);
    ASSERT_NE(appWin, nullptr);

    activeWindows_.push_back(appWin);

    // statusBar window
    sptr<Window> statBar = Utils::CreateStatusBarWindow();
    activeWindows_.push_back(statBar);

    ASSERT_EQ(WMError::WM_OK, appWin->Show());
    if (appWin->IsDecorEnable()) {
        ASSERT_TRUE(Utils::RectEqualTo(appWin, Utils::GetDecorateRect(res, virtualPixelRatio_)));
    } else {
        ASSERT_TRUE(Utils::RectEqualTo(appWin, res));
    }
    ASSERT_EQ(WMError::WM_OK, statBar->Show());
    if (appWin->IsDecorEnable()) {
        ASSERT_TRUE(Utils::RectEqualTo(appWin, Utils::GetDecorateRect(res, virtualPixelRatio_)));
    } else {
        ASSERT_TRUE(Utils::RectEqualTo(appWin, res));
    }
    ASSERT_EQ(WMError::WM_OK, statBar->Hide());
    if (appWin->IsDecorEnable()) {
        ASSERT_TRUE(Utils::RectEqualTo(appWin, Utils::GetDecorateRect(res, virtualPixelRatio_)));
    } else {
        ASSERT_TRUE(Utils::RectEqualTo(appWin, res));
    }
}

/**
 * @tc.name: LayoutWindow06
 * @tc.desc: StatusBar Window and NaviBar & Sys Window FULLSCRENN,NOT NEEDVOID,PARENTLIMIT
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow06, TestSize.Level1)
{
    sptr<Window> statBar = Utils::CreateStatusBarWindow();
    ASSERT_NE(statBar, nullptr);

    activeWindows_.push_back(statBar);
    sptr<Window> naviBar = Utils::CreateNavigationBarWindow();
    activeWindows_.push_back(naviBar);
    Utils::TestWindowInfo info = {
        .name = "main6",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_PANEL,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = false,
        .parentLimit = true,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
    sptr<Window> sysWin = Utils::CreateTestWindow(info);
    ASSERT_NE(sysWin, nullptr);
    activeWindows_.push_back(sysWin);
    if (statBar->Show() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, statBar->Show());
    } else if (statBar->Show() == WMError::WM_ERROR_INVALID_WINDOW) {
        ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, statBar->Show());
    }
    sysWin->Show();
    if (Utils::RectEqualTo(sysWin, Utils::displayRect_)) {
        ASSERT_TRUE(Utils::RectEqualTo(sysWin, Utils::displayRect_));
    }
    if (Utils::RectEqualTo(sysWin, Utils::displayRect_)) {
        ASSERT_TRUE(Utils::RectEqualTo(sysWin, Utils::displayRect_));
    } else {
        ASSERT_FALSE(Utils::RectEqualTo(sysWin, Utils::displayRect_));
    }
    if (WMError::WM_OK == naviBar->Show()) {
        ASSERT_EQ(WMError::WM_OK, naviBar->Show());
    }
    if (Utils::RectEqualTo(sysWin, Utils::displayRect_)) {
        ASSERT_TRUE(Utils::RectEqualTo(sysWin, Utils::displayRect_));
    }
    if (WMError::WM_OK == statBar->Hide()) {
        ASSERT_EQ(WMError::WM_OK, statBar->Hide());
    }
    if (Utils::RectEqualTo(sysWin, Utils::displayRect_)) {
        ASSERT_TRUE(Utils::RectEqualTo(sysWin, Utils::displayRect_));
    }
}

/**
 * @tc.name: LayoutWindow07
 * @tc.desc: StatusBar Window and NaviBar & One Floating Sys Window
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow07, TestSize.Level1)
{
    // statusBar window
    sptr<Window> statBar = Utils::CreateStatusBarWindow();
    ASSERT_NE(statBar, nullptr);
    activeWindows_.push_back(statBar);

    // naviBar window
    sptr<Window> naviBar = Utils::CreateNavigationBarWindow();
    ASSERT_NE(naviBar, nullptr);
    activeWindows_.push_back(naviBar);
    // sys window
    Utils::TestWindowInfo info = {
        .name = "main7",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_PANEL,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = true,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
    sptr<Window> sysWin = Utils::CreateTestWindow(info);
    ASSERT_NE(sysWin, nullptr);
    activeWindows_.push_back(sysWin);
    if (statBar->Show() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, statBar->Show());
    } else if (statBar->Show() == WMError::WM_ERROR_INVALID_WINDOW) {
        ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, statBar->Show());
    }
    sysWin->Show();

    ASSERT_TRUE(Utils::RectEqualTo(sysWin, Utils::customAppRect_));

    if (WMError::WM_OK == naviBar->Show()) {
        ASSERT_EQ(WMError::WM_OK, naviBar->Show());
    } else {
        ASSERT_NE(WMError::WM_OK, naviBar->Show());
    }

    ASSERT_TRUE(Utils::RectEqualTo(sysWin, Utils::customAppRect_));
    if (statBar->Hide() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, statBar->Hide());
    } else if (statBar->Hide() == WMError::WM_ERROR_INVALID_WINDOW) {
        ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, statBar->Hide());
    }
    ASSERT_TRUE(Utils::RectEqualTo(sysWin, Utils::customAppRect_));
}

/**
 * @tc.name: LayoutWindow08
 * @tc.desc: One FLOATING APP Window with on custom rect
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow08, TestSize.Level1)
{
    Utils::TestWindowInfo info = {
        .name = "main8",
        .rect = { 0, 0, 0, 0 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
    const sptr<Window>& window = Utils::CreateTestWindow(info);
    ASSERT_NE(window, nullptr);

    activeWindows_.push_back(window);
    Rect expect = Utils::GetDefaultFloatingRect(window, true);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(Utils::RectEqualTo(window, expect));
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    usleep(WAIT_SYANC_US);
}

/**
 * @tc.name: LayoutWindow09
 * @tc.desc: Add a floating and resize(2, 2)
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow09, TestSize.Level1)
{
    Utils::TestWindowInfo info = {
        .name = "main9",
        .rect = { 0, 0, 0, 0 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
    const sptr<Window>& window = Utils::CreateTestWindow(info);
    ASSERT_NE(window, nullptr);
    activeWindows_.push_back(window);
    Rect expect = Utils::GetDefaultFloatingRect(window, true);

    ASSERT_EQ(WMError::WM_OK, window->Show());
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(Utils::RectEqualTo(window, expect));

    ASSERT_EQ(WMError::WM_OK, window->Resize(2u, 2u));         // 2: custom min size
    Rect finalExcept = { expect.posX_, expect.posY_, 2u, 2u }; // 2: custom min size
    finalExcept = Utils::GetFloatingLimitedRect(finalExcept, virtualPixelRatio_);
    ASSERT_TRUE(Utils::RectEqualTo(window, finalExcept));
    ASSERT_EQ(WMError::WM_OK, window->Hide());
}

/**
 * @tc.name: LayoutWindow10
 * @tc.desc: One FLOATING APP Window do max and recovery
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow10, TestSize.Level1)
{
    Utils::TestWindowInfo info = {
        .name = "main10",
        .rect = { 0, 0, 0, 0 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
    const sptr<Window>& window = Utils::CreateTestWindow(info);
    ASSERT_NE(window, nullptr);

    activeWindows_.push_back(window);
    Rect expect = Utils::GetDefaultFloatingRect(window, true);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(Utils::RectEqualTo(window, expect));
    ASSERT_EQ(WMError::WM_OK, window->Maximize());
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(Utils::RectEqualTo(window, Utils::displayRect_));
    ASSERT_EQ(WMError::WM_OK, window->Recover());
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(Utils::RectEqualTo(window, expect));
    ASSERT_EQ(WMError::WM_OK, window->Minimize());
    usleep(WAIT_SYANC_US);
    ASSERT_EQ(WMError::WM_OK, window->Close());
}

/**
 * @tc.name: LayoutTile01
 * @tc.desc: One FLOATING APP Window into tile mode, show 4 new window
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutTile01, TestSize.Level1)
{
    Utils::TestWindowInfo info = {
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, .mode = WindowMode::WINDOW_MODE_FLOATING, .needAvoid = true,
        .parentId = INVALID_WINDOW_ID, .parentLimit = false, .name = "mainTile1", .rect = {0, 0, 0, 0},
    };
    const sptr<Window>& window = Utils::CreateTestWindow(info);
    ASSERT_NE(window, nullptr);
    activeWindows_.push_back(window);
    Rect expect = Utils::GetDefaultFloatingRect(window, true);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    usleep(WAIT_SYANC_US);
    // init tile window rects and get max tile window num
    Utils::InitTileWindowRects(window, false);
    uint32_t maxTileNum = Utils::GetMaxTileWinNum();
    EXPECT_FALSE(maxTileNum < 1);
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(Utils::RectEqualTo(window, expect));
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::TILE);
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(Utils::RectEqualTo(window, Utils::singleTileRect_));

    info.name = "test1";
    const sptr<Window>& test1 = Utils::CreateTestWindow(info);
    ASSERT_NE(nullptr, test1);
    activeWindows_.push_back(test1);
    ASSERT_EQ(WMError::WM_OK, test1->Show());
    usleep(WAIT_SYANC_US);
    EXPECT_NE(maxTileNum, 1);
    ASSERT_TRUE(Utils::RectEqualTo(window, Utils::doubleTileRects_[0]));
    ASSERT_TRUE(Utils::RectEqualTo(test1, Utils::doubleTileRects_[1]));

    info.name = "test2";
    const sptr<Window>& test2 = Utils::CreateTestWindow(info);
    ASSERT_NE(nullptr, test2);
    activeWindows_.push_back(test2);
    ASSERT_EQ(WMError::WM_OK, test2->Show());
    usleep(WAIT_SYANC_US);
    if (maxTileNum == 2) {
        ASSERT_TRUE(Utils::RectEqualTo(test1, Utils::doubleTileRects_[0]));
        ASSERT_TRUE(Utils::RectEqualTo(test2, Utils::doubleTileRects_[1]));
    } else {
        ASSERT_TRUE(Utils::RectEqualTo(window, Utils::tripleTileRects_[0]));
        ASSERT_TRUE(Utils::RectEqualTo(test1, Utils::tripleTileRects_[1]));
        ASSERT_TRUE(Utils::RectEqualTo(test2, Utils::tripleTileRects_[2])); // 2 is second rect idx
    }
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
}

/**
 * @tc.name: LayoutTileNegative01
 * @tc.desc: negative test for tile window
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutTileNegative01, TestSize.Level1)
{
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
    Utils::TestWindowInfo info = {
        .name = "mainTileNegative1", .rect = {-1, -100, -1, -100}, // -1, -100, -1, -100 is typical negative case nums
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true, .parentLimit = false, .parentId = INVALID_WINDOW_ID,
    };
    const sptr<Window>& window = Utils::CreateTestWindow(info);
    ASSERT_NE(window, nullptr);

    activeWindows_.push_back(window);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    usleep(WAIT_SYANC_US);
    // init tile window rects and get max tile window num
    Utils::InitTileWindowRects(window, false);
    uint32_t maxTileNum = Utils::GetMaxTileWinNum();
    EXPECT_FALSE(maxTileNum < 1);

    usleep(WAIT_SYANC_US);
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::TILE);
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(Utils::RectEqualTo(window, Utils::singleTileRect_));

    info.name = "test1";
    const sptr<Window>& test1 = Utils::CreateTestWindow(info);
    ASSERT_NE(nullptr, test1);
    activeWindows_.push_back(test1);
    ASSERT_EQ(WMError::WM_OK, test1->Show());
    usleep(WAIT_SYANC_US);
    EXPECT_NE(maxTileNum, 1);
    ASSERT_TRUE(Utils::RectEqualTo(window, Utils::doubleTileRects_[0]));
    ASSERT_TRUE(Utils::RectEqualTo(test1, Utils::doubleTileRects_[1]));

    info.name = "test2";
    const sptr<Window>& test2 = Utils::CreateTestWindow(info);
    ASSERT_NE(nullptr, test2);
    activeWindows_.push_back(test2);
    ASSERT_EQ(WMError::WM_OK, test2->Show());
    usleep(WAIT_SYANC_US);
    if (maxTileNum == 2) {
        ASSERT_TRUE(Utils::RectEqualTo(test1, Utils::doubleTileRects_[0]));
        ASSERT_TRUE(Utils::RectEqualTo(test2, Utils::doubleTileRects_[1]));
    } else {
        ASSERT_TRUE(Utils::RectEqualTo(window, Utils::tripleTileRects_[0]));
        ASSERT_TRUE(Utils::RectEqualTo(test1, Utils::tripleTileRects_[1]));
        ASSERT_TRUE(Utils::RectEqualTo(test2, Utils::tripleTileRects_[2])); // 2 is second rect idx
    }
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
}

/**
 * @tc.name: LayoutTileNegative01
 * @tc.desc: move window out of the display
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutNegative01, TestSize.Level1)
{
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
    Utils::TestWindowInfo info = {
        .name = "mainNegative1",
        .rect = { 0, 0, 0, 0 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
    const sptr<Window>& window = Utils::CreateTestWindow(info);
    ASSERT_NE(window, nullptr);

    activeWindows_.push_back(window);
    Rect expect = Utils::GetDefaultFloatingRect(window, true);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(Utils::RectEqualTo(window, expect));
}

/**
 * @tc.name: LayoutNegative02
 * @tc.desc: resize window to negative size
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutNegative02, TestSize.Level1)
{
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
    const uint32_t negativeW = 0;
    const uint32_t negativeH = 0;
    Utils::TestWindowInfo info = {
        .name = "mainNegative2",
        .rect = { 0, 0, 0, 0 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
    const sptr<Window>& window = Utils::CreateTestWindow(info);
    ASSERT_NE(window, nullptr);

    activeWindows_.push_back(window);
    Rect expect = Utils::GetDefaultFloatingRect(window, true);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(Utils::RectEqualTo(window, expect));
    window->Resize(negativeW, negativeH);
    usleep(WAIT_SYANC_US);
    Rect expect2 = { expect.posX_, expect.posY_, negativeW, negativeH };
    expect2 = Utils::CalcLimitedRect(expect2, virtualPixelRatio_);
    ASSERT_TRUE(Utils::RectEqualTo(window, expect2));
}

} // namespace
} // namespace Rosen
} // namespace OHOS
