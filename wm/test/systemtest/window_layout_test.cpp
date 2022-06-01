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

// gtest
#include <gtest/gtest.h>
#include "window_manager.h"
#include "window_test_utils.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using utils = WindowTestUtils;
class WindowLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    DisplayId displayId_ = 0;
    std::vector<sptr<Window>> activeWindows_;
    static vector<Rect> fullScreenExpecteds_;
    static inline float virtualPixelRatio_ = 0.0;
private:
    static constexpr uint32_t WAIT_SYANC_US = 100000;
};

vector<Rect> WindowLayoutTest::fullScreenExpecteds_;

void WindowLayoutTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    utils::InitByDisplayRect(displayRect);

    virtualPixelRatio_ = WindowTestUtils::GetVirtualPixelRatio(0);

    // calc expected rects
    Rect expected = { // 0. only statusBar
        0,
        utils::statusBarRect_.height_,
        utils::displayRect_.width_,
        utils::displayRect_.height_ - utils::statusBarRect_.height_,
    };
    fullScreenExpecteds_.push_back(expected);
    expected = { // 1. both statusBar and naviBar
        0,
        utils::statusBarRect_.height_,
        utils::displayRect_.width_,
        utils::displayRect_.height_ - utils::statusBarRect_.height_ - utils::naviBarRect_.height_,
    };
    fullScreenExpecteds_.push_back(expected);
    expected = { // 2. only naviBar
        0,
        0,
        utils::displayRect_.width_,
        utils::displayRect_.height_ - utils::naviBarRect_.height_,
    };
    fullScreenExpecteds_.push_back(expected);
}

void WindowLayoutTest::TearDownTestCase()
{
}

void WindowLayoutTest::SetUp()
{
    activeWindows_.clear();
}

void WindowLayoutTest::TearDown()
{
    while (!activeWindows_.empty()) {
        ASSERT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
}

namespace {
/**
 * @tc.name: LayoutWindow01
 * @tc.desc: One FLOATING APP Window with on custom rect
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow01, Function | MediumTest | Level3)
{
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::TILE);
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::TILE);
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);

    utils::TestWindowInfo info = {
        .name = "main",
        .rect = {0, 0, 0, 0},
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& window = utils::CreateTestWindow(info);
    activeWindows_.push_back(window);
    Rect expect = utils::GetDefaultFloatingRect(window);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_TRUE(utils::RectEqualTo(window, expect));
    ASSERT_EQ(WMError::WM_OK, window->Hide());
}

/**
 * @tc.name: LayoutWindow02
 * @tc.desc: One FLOATING APP Window
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow02, Function | MediumTest | Level3)
{
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& window = utils::CreateTestWindow(info);
    activeWindows_.push_back(window);

    ASSERT_EQ(WMError::WM_OK, window->Show());
    Rect res = utils::GetFloatingLimitedRect(utils::customAppRect_, virtualPixelRatio_);
    if (window->IsDecorEnable()) {
        ASSERT_TRUE(utils::RectEqualTo(window, utils::GetDecorateRect(res, virtualPixelRatio_)));
    } else {
        ASSERT_TRUE(utils::RectEqualTo(window, res));
    }
    ASSERT_EQ(WMError::WM_OK, window->Hide());
}

/**
 * @tc.name: LayoutWindow04
 * @tc.desc: One FLOATING APP Window & One StatusBar Window
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow04, Function | MediumTest | Level3)
{
    // app window
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    sptr<Window> appWin = utils::CreateTestWindow(info);
    activeWindows_.push_back(appWin);

    // statusBar window
    sptr<Window> statBar = utils::CreateStatusBarWindow();
    activeWindows_.push_back(statBar);

    ASSERT_EQ(WMError::WM_OK, appWin->Show());
    Rect res = utils::GetFloatingLimitedRect(utils::customAppRect_, virtualPixelRatio_);
    if (appWin->IsDecorEnable()) {
        ASSERT_TRUE(utils::RectEqualTo(appWin, utils::GetDecorateRect(res, virtualPixelRatio_)));
    } else {
        ASSERT_TRUE(utils::RectEqualTo(appWin, res));
    }
    ASSERT_EQ(WMError::WM_OK, statBar->Show());
    if (appWin->IsDecorEnable()) {
        ASSERT_TRUE(utils::RectEqualTo(appWin, utils::GetDecorateRect(res, virtualPixelRatio_)));
    } else {
        ASSERT_TRUE(utils::RectEqualTo(appWin, res));
    }
    ASSERT_EQ(WMError::WM_OK, statBar->Hide());
    if (appWin->IsDecorEnable()) {
        ASSERT_TRUE(utils::RectEqualTo(appWin, utils::GetDecorateRect(res, virtualPixelRatio_)));
    } else {
        ASSERT_TRUE(utils::RectEqualTo(appWin, res));
    }
}

/**
 * @tc.name: LayoutWindow06
 * @tc.desc: StatusBar Window and NaviBar & Sys Window FULLSCRENN,NOT NEEDVOID,PARENTLIMIT
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow06, Function | MediumTest | Level3)
{
    // statusBar window
    sptr<Window> statBar = utils::CreateStatusBarWindow();
    activeWindows_.push_back(statBar);

    // naviBar window
    sptr<Window> naviBar = utils::CreateNavigationBarWindow();
    activeWindows_.push_back(naviBar);

    // sys window
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_PANEL,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = false,
        .parentLimit = true,
        .parentName = "",
    };
    sptr<Window> sysWin = utils::CreateTestWindow(info);
    activeWindows_.push_back(sysWin);

    ASSERT_EQ(WMError::WM_OK, statBar->Show());
    ASSERT_EQ(WMError::WM_OK, sysWin->Show());
    ASSERT_TRUE(utils::RectEqualTo(sysWin, utils::displayRect_));
    ASSERT_EQ(WMError::WM_OK, naviBar->Show());
    ASSERT_TRUE(utils::RectEqualTo(sysWin, utils::displayRect_));
    ASSERT_EQ(WMError::WM_OK, statBar->Hide());
    ASSERT_TRUE(utils::RectEqualTo(sysWin, utils::displayRect_));
}

/**
 * @tc.name: LayoutWindow07
 * @tc.desc: StatusBar Window and NaviBar & One Floating Sys Window
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow07, Function | MediumTest | Level3)
{
    // statusBar window
    sptr<Window> statBar = utils::CreateStatusBarWindow();
    activeWindows_.push_back(statBar);

    // naviBar window
    sptr<Window> naviBar = utils::CreateNavigationBarWindow();
    activeWindows_.push_back(naviBar);

    // sys window
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_PANEL,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = true,
        .parentName = "",
    };
    sptr<Window> sysWin = utils::CreateTestWindow(info);
    activeWindows_.push_back(sysWin);

    ASSERT_EQ(WMError::WM_OK, statBar->Show());
    ASSERT_EQ(WMError::WM_OK, sysWin->Show());
    ASSERT_TRUE(utils::RectEqualTo(sysWin, utils::customAppRect_));
    ASSERT_EQ(WMError::WM_OK, naviBar->Show());
    ASSERT_TRUE(utils::RectEqualTo(sysWin, utils::customAppRect_));
    ASSERT_EQ(WMError::WM_OK, statBar->Hide());
    ASSERT_TRUE(utils::RectEqualTo(sysWin, utils::customAppRect_));
}

/**
 * @tc.name: LayoutWindow08
 * @tc.desc: One FLOATING APP Window with on custom rect
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow08, Function | MediumTest | Level3)
{
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = {0, 0, 0, 0},
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& window = utils::CreateTestWindow(info);
    activeWindows_.push_back(window);
    Rect expect = utils::GetDefaultFloatingRect(window);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_TRUE(utils::RectEqualTo(window, expect));
    ASSERT_EQ(WMError::WM_OK, window->Hide());
}

/**
 * @tc.name: LayoutWindow09
 * @tc.desc: Add a floating and resize(2, 2)
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow09, Function | MediumTest | Level3)
{
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = {0, 0, 0, 0},
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& window = utils::CreateTestWindow(info);
    activeWindows_.push_back(window);
    Rect expect = utils::GetDefaultFloatingRect(window);

    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_TRUE(utils::RectEqualTo(window, expect));

    ASSERT_EQ(WMError::WM_OK, window->Resize(2u, 2u));        // 2: custom min size
    Rect finalExcept = { expect.posX_, expect.posY_, 2u, 2u}; // 2: custom min size
    finalExcept = utils::GetFloatingLimitedRect(finalExcept, virtualPixelRatio_);
    ASSERT_TRUE(utils::RectEqualTo(window, finalExcept));
    ASSERT_EQ(WMError::WM_OK, window->Hide());
}

/**
 * @tc.name: LayoutWindow10
 * @tc.desc: One FLOATING APP Window do max and recovery
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutWindow10, Function | MediumTest | Level3)
{
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = {0, 0, 0, 0},
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& window = utils::CreateTestWindow(info);
    activeWindows_.push_back(window);
    Rect expect = utils::GetDefaultFloatingRect(window);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_TRUE(utils::RectEqualTo(window, expect));
    ASSERT_EQ(WMError::WM_OK, window->Maximize());
    ASSERT_TRUE(utils::RectEqualTo(window, utils::displayRect_));
    ASSERT_EQ(WMError::WM_OK, window->Recover());
    ASSERT_TRUE(utils::RectEqualTo(window, expect));
    ASSERT_EQ(WMError::WM_OK, window->Minimize());
    ASSERT_EQ(WMError::WM_OK, window->Close());
}

/**
 * @tc.name: LayoutTile01
 * @tc.desc: One FLOATING APP Window into tile mode, show 4 new window
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutTile01, Function | MediumTest | Level3)
{
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = {0, 0, 0, 0},
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };

    const sptr<Window>& window = utils::CreateTestWindow(info);
    activeWindows_.push_back(window);
    Rect expect = utils::GetDefaultFloatingRect(window);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    // init tile window rects and get max tile window num
    utils::InitTileWindowRects(window);
    uint32_t maxTileNum = utils::GetMaxTileWinNum();

    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(utils::RectEqualTo(window, expect));
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::TILE);
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(utils::RectEqualTo(window, utils::singleTileRect_));

    info.name = "test1";
    const sptr<Window>& test1 = utils::CreateTestWindow(info);
    activeWindows_.push_back(test1);
    ASSERT_EQ(WMError::WM_OK, test1->Show());
    usleep(WAIT_SYANC_US);
    if (maxTileNum == 1) {
        ASSERT_TRUE(utils::RectEqualTo(test1, utils::singleTileRect_));
        WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
        return;
    }
    ASSERT_TRUE(utils::RectEqualTo(window, utils::doubleTileRects_[0]));
    ASSERT_TRUE(utils::RectEqualTo(test1, utils::doubleTileRects_[1]));

    info.name = "test2";
    const sptr<Window>& test2 = utils::CreateTestWindow(info);
    activeWindows_.push_back(test2);
    ASSERT_EQ(WMError::WM_OK, test2->Show());
    usleep(WAIT_SYANC_US);
    if (maxTileNum == 2) {
        ASSERT_TRUE(utils::RectEqualTo(test1, utils::doubleTileRects_[0]));
        ASSERT_TRUE(utils::RectEqualTo(test2, utils::doubleTileRects_[1]));
        WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
        return;
    }
    ASSERT_TRUE(utils::RectEqualTo(window, utils::tripleTileRects_[0]));
    ASSERT_TRUE(utils::RectEqualTo(test1, utils::tripleTileRects_[1]));
    ASSERT_TRUE(utils::RectEqualTo(test2, utils::tripleTileRects_[2])); // 2 is second rect idx
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
}

/**
 * @tc.name: LayoutTileNegative01
 * @tc.desc: negative test for tile window
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutTileNegative01, Function | MediumTest | Level3)
{
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = {-1, -100, -1, -100}, // -1, -100, -1, -100 is typical negative case nums
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& window = utils::CreateTestWindow(info);
    activeWindows_.push_back(window);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    // init tile window rects and get max tile window num
    utils::InitTileWindowRects(window);
    uint32_t maxTileNum = utils::GetMaxTileWinNum();

    usleep(WAIT_SYANC_US);
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::TILE);
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(utils::RectEqualTo(window, utils::singleTileRect_));

    info.name = "test1";
    const sptr<Window>& test1 = utils::CreateTestWindow(info);
    activeWindows_.push_back(test1);
    ASSERT_EQ(WMError::WM_OK, test1->Show());
    usleep(WAIT_SYANC_US);
    if (maxTileNum == 1) {
        ASSERT_TRUE(utils::RectEqualTo(test1, utils::singleTileRect_));
        WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
        return;
    }
    ASSERT_TRUE(utils::RectEqualTo(window, utils::doubleTileRects_[0]));
    ASSERT_TRUE(utils::RectEqualTo(test1, utils::doubleTileRects_[1]));

    info.name = "test2";
    const sptr<Window>& test2 = utils::CreateTestWindow(info);
    activeWindows_.push_back(test2);
    ASSERT_EQ(WMError::WM_OK, test2->Show());
    usleep(WAIT_SYANC_US);
    if (maxTileNum == 2) {
        ASSERT_TRUE(utils::RectEqualTo(test1, utils::doubleTileRects_[0]));
        ASSERT_TRUE(utils::RectEqualTo(test2, utils::doubleTileRects_[1]));
        WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
        return;
    }
    ASSERT_TRUE(utils::RectEqualTo(window, utils::tripleTileRects_[0]));
    ASSERT_TRUE(utils::RectEqualTo(test1, utils::tripleTileRects_[1]));
    ASSERT_TRUE(utils::RectEqualTo(test2, utils::tripleTileRects_[2])); // 2 is second rect idx
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
}

/**
 * @tc.name: LayoutTileNegative01
 * @tc.desc: move window out of the display
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutNegative01, Function | MediumTest | Level3)
{
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = {0, 0, 0, 0},
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& window = utils::CreateTestWindow(info);
    activeWindows_.push_back(window);
    Rect expect = utils::GetDefaultFloatingRect(window);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(utils::RectEqualTo(window, expect));
}

/**
 * @tc.name: LayoutNegative02
 * @tc.desc: resize window to negative size
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutTest, LayoutNegative02, Function | MediumTest | Level3)
{
    const uint32_t negativeW = 0;
    const uint32_t negativeH = 0;
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = {0, 0, 0, 0},
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& window = utils::CreateTestWindow(info);
    activeWindows_.push_back(window);
    Rect expect = utils::GetDefaultFloatingRect(window);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    usleep(WAIT_SYANC_US);
    ASSERT_TRUE(utils::RectEqualTo(window, expect));
    window->Resize(negativeW, negativeH);
    usleep(WAIT_SYANC_US);
    Rect expect2 = {expect.posX_, expect.posY_, negativeW, negativeH};
    expect2 = utils::CalcLimitedRect(expect2, virtualPixelRatio_);
    ASSERT_TRUE(utils::RectEqualTo(window, expect2));
}
}
} // namespace Rosen
} // namespace OHOS
