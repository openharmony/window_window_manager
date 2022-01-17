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
class WindowSplitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    std::vector<sptr<Window>> activeWindows_;

private:
    static constexpr uint32_t SPLIT_TEST_SPEEP_S = 1; // split test spleep time
};

void WindowSplitTest::SetUpTestCase()
{
}

void WindowSplitTest::TearDownTestCase()
{
}

void WindowSplitTest::SetUp()
{
    activeWindows_.clear();
}

void WindowSplitTest::TearDown()
{
    while (!activeWindows_.empty()) {
        ASSERT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
}

namespace {
/**
 * @tc.name: SplitWindow01
 * @tc.desc: one primary window and one fullscreen window
 * @tc.type: FUNC
 * @tc.require: AR000GGTV7
 */
HWTEST_F(WindowSplitTest, SplitWindow01, Function | MediumTest | Level3)
{
    utils::TestWindowInfo infoFullScreen = {
        .name = "fullscreen.1",
        .rect = utils::defaultAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    utils::TestWindowInfo infoPrimary = {
        .name = "primary.1",
        .rect = utils::defaultAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_SPLIT_PRIMARY,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& windowFullScreen = utils::CreateTestWindow(infoFullScreen);
    ASSERT_EQ(WMError::WM_OK, windowFullScreen->Show());
    sleep(SPLIT_TEST_SPEEP_S);
    activeWindows_.push_back(windowFullScreen);
    const sptr<Window>& windowPrimary = utils::CreateTestWindow(infoPrimary);
    ASSERT_EQ(WMError::WM_OK, windowPrimary->Show());
    sleep(SPLIT_TEST_SPEEP_S);
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, windowPrimary->GetMode());
    activeWindows_.push_back(windowPrimary);
    // show one split primary window
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_SECONDARY, windowFullScreen->GetMode());
    ASSERT_EQ(WMError::WM_OK, windowPrimary->Hide());
    sleep(SPLIT_TEST_SPEEP_S);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, windowFullScreen->GetMode());
    ASSERT_EQ(WMError::WM_OK, windowFullScreen->Hide());
    sleep(SPLIT_TEST_SPEEP_S);
}

/**
 * @tc.name: SplitWindow02
 * @tc.desc: one secondary window and one fullscreen window
 * @tc.type: FUNC
 * @tc.require: AR000GGTV7
 */
HWTEST_F(WindowSplitTest, SplitWindow02, Function | MediumTest | Level3)
{
    utils::TestWindowInfo infoFullScreen = {
        .name = "fullscreen.2",
        .rect = utils::defaultAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    utils::TestWindowInfo infoSecondary = {
        .name = "secondary.2",
        .rect = utils::defaultAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_SPLIT_SECONDARY,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };
    const sptr<Window>& windowFullScreen = utils::CreateTestWindow(infoFullScreen);
    ASSERT_EQ(WMError::WM_OK, windowFullScreen->Show());
    sleep(SPLIT_TEST_SPEEP_S);
    activeWindows_.push_back(windowFullScreen);
    const sptr<Window>& windowSecondary = utils::CreateTestWindow(infoSecondary);
    ASSERT_EQ(WMError::WM_OK, windowSecondary->Show());
    sleep(SPLIT_TEST_SPEEP_S);
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_SECONDARY, windowSecondary->GetMode());
    activeWindows_.push_back(windowSecondary);
    // show one split secondary window
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, windowFullScreen->GetMode());
    ASSERT_EQ(WMError::WM_OK, windowFullScreen->Hide());
    sleep(SPLIT_TEST_SPEEP_S);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, windowSecondary->GetMode());
    ASSERT_EQ(WMError::WM_OK, windowSecondary->Hide());
    sleep(SPLIT_TEST_SPEEP_S);
}
}
} // namespace Rosen
} // namespace OHOS
