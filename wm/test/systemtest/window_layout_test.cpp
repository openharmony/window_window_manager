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
    int displayId_ = 0;
    std::vector<sptr<Window>> activeWindows_;
    static vector<Rect> fullScreenExpecteds_;
};

vector<Rect> WindowLayoutTest::fullScreenExpecteds_;

void WindowLayoutTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    if (display == nullptr) {
        printf("GetDefaultDisplay: failed!\n");
    } else {
        printf("GetDefaultDisplay: id %llu, w %d, h %d, fps %u\n", display->GetId(), display->GetWidth(),
            display->GetHeight(), display->GetFreshRate());
    }
    Rect screenRect = {0, 0, display->GetWidth(), display->GetHeight()};
    utils::InitByScreenRect(screenRect);
    // calc expected rects
    Rect expected = { // 0. only statusBar
        0,
        utils::statusBarRect_.height_,
        utils::screenRect_.width_,
        utils::screenRect_.height_ - utils::statusBarRect_.height_,
    };
    fullScreenExpecteds_.push_back(expected);
    expected = { // 1. both statusBar and naviBar
        0,
        utils::statusBarRect_.height_,
        utils::screenRect_.width_,
        utils::screenRect_.height_ - utils::statusBarRect_.height_ - utils::naviBarRect_.height_,
    };
    fullScreenExpecteds_.push_back(expected);
    expected = { // 2. only naviBar
        0,
        0,
        utils::screenRect_.width_,
        utils::screenRect_.height_ - utils::naviBarRect_.height_,
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
 * @tc.name: LayoutWindow02
 * @tc.desc: One FLOATING APP Window
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowLayoutTest, LayoutWindow02, Function | MediumTest | Level3)
{
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = utils::defaultAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& window = utils::CreateTestWindow(info);
    activeWindows_.push_back(window);

    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_TRUE(utils::RectEqualTo(window, utils::defaultAppRect_));
    ASSERT_EQ(WMError::WM_OK, window->Hide());
}

/**
 * @tc.name: LayoutWindow04
 * @tc.desc: One FLOATING APP Window & One StatusBar Window
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
 */
HWTEST_F(WindowLayoutTest, LayoutWindow04, Function | MediumTest | Level3)
{
    // app window
    utils::TestWindowInfo info = {
        .name = "main",
        .rect = utils::defaultAppRect_,
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
    ASSERT_TRUE(utils::RectEqualTo(appWin, utils::defaultAppRect_));
    ASSERT_EQ(WMError::WM_OK, statBar->Show());
    ASSERT_TRUE(utils::RectEqualTo(appWin, utils::defaultAppRect_));
    ASSERT_TRUE(utils::RectEqualTo(statBar, utils::statusBarRect_));
    ASSERT_EQ(WMError::WM_OK, statBar->Hide());
    ASSERT_TRUE(utils::RectEqualTo(appWin, utils::defaultAppRect_));
}

/**
 * @tc.name: LayoutWindow06
 * @tc.desc: StatusBar Window and NaviBar & Sys Window FULLSCRENN,NOT NEEDVOID,PARENTLIMIT
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
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
        .rect = utils::defaultAppRect_,
        .type = WindowType::WINDOW_TYPE_PANEL,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = false,
        .parentLimit = true,
        .parentName = "",
    };
    sptr<Window> sysWin = utils::CreateTestWindow(info);
    activeWindows_.push_back(sysWin);

    ASSERT_EQ(WMError::WM_OK, statBar->Show());
    ASSERT_TRUE(utils::RectEqualTo(statBar, utils::statusBarRect_));
    ASSERT_EQ(WMError::WM_OK, sysWin->Show());
    ASSERT_TRUE(utils::RectEqualTo(sysWin, utils::screenRect_));
    ASSERT_EQ(WMError::WM_OK, naviBar->Show());
    ASSERT_TRUE(utils::RectEqualTo(sysWin, utils::screenRect_));
    ASSERT_EQ(WMError::WM_OK, statBar->Hide());
    ASSERT_TRUE(utils::RectEqualTo(sysWin, utils::screenRect_));
}

/**
 * @tc.name: LayoutWindow07
 * @tc.desc: StatusBar Window and NaviBar & One Floating Sys Window
 * @tc.type: FUNC
 * @tc.require: AR000GGTVJ
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
        .rect = utils::defaultAppRect_,
        .type = WindowType::WINDOW_TYPE_PANEL,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = true,
        .parentName = "",
    };
    sptr<Window> sysWin = utils::CreateTestWindow(info);
    activeWindows_.push_back(sysWin);

    ASSERT_EQ(WMError::WM_OK, statBar->Show());
    ASSERT_TRUE(utils::RectEqualTo(statBar, utils::statusBarRect_));
    ASSERT_EQ(WMError::WM_OK, sysWin->Show());
    ASSERT_TRUE(utils::RectEqualTo(sysWin, utils::defaultAppRect_));
    ASSERT_EQ(WMError::WM_OK, naviBar->Show());
    ASSERT_TRUE(utils::RectEqualTo(sysWin, utils::defaultAppRect_));
    ASSERT_EQ(WMError::WM_OK, statBar->Hide());
    ASSERT_TRUE(utils::RectEqualTo(sysWin, utils::defaultAppRect_));
}
}
} // namespace Rosen
} // namespace OHOS
