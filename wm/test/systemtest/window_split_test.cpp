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
    utils::TestWindowInfo fullInfo_;
    utils::TestWindowInfo splitInfo_;

private:
    static constexpr uint32_t SPLIT_TEST_SLEEP_S = 1; // split test sleep time
};

void WindowSplitTest::SetUpTestCase()
{
}

void WindowSplitTest::TearDownTestCase()
{
}

void WindowSplitTest::SetUp()
{
    fullInfo_ = {
        .name = "",
        .rect = utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };

    splitInfo_ = {
        .name = "",
        .rect = utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };

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
 * @tc.desc: one primary window and one fullscreen window, test mode change
 * @tc.type: FUNC
 */
HWTEST_F(WindowSplitTest, SplitWindow01, Function | MediumTest | Level3)
{
    fullInfo_.name  = "fullscreen.1";
    splitInfo_.name = "primary.1";
    splitInfo_.mode = WindowMode::WINDOW_MODE_SPLIT_PRIMARY;

    const sptr<Window>& fullWindow = utils::CreateTestWindow(fullInfo_);
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);

    const sptr<Window>& priWindow = utils::CreateTestWindow(splitInfo_);
    activeWindows_.push_back(priWindow);
    ASSERT_EQ(WMError::WM_OK, priWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);

    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, priWindow->GetMode());
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_SECONDARY, fullWindow->GetMode());

    ASSERT_EQ(WMError::WM_OK, priWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, fullWindow->GetMode());
    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
}

/**
 * @tc.name: SplitWindow02
 * @tc.desc: one secondary window and one fullscreen window, test mode change
 * @tc.type: FUNC
 */
HWTEST_F(WindowSplitTest, SplitWindow02, Function | MediumTest | Level3)
{
    fullInfo_.name  = "fullscreen.2";
    splitInfo_.name = "secondary.2";
    splitInfo_.mode = WindowMode::WINDOW_MODE_SPLIT_SECONDARY;

    const sptr<Window>& fullWindow = utils::CreateTestWindow(fullInfo_);
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);
    const sptr<Window>& secWindow = utils::CreateTestWindow(splitInfo_);
    activeWindows_.push_back(secWindow);
    ASSERT_EQ(WMError::WM_OK, secWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);

    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_SECONDARY, secWindow->GetMode());
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, fullWindow->GetMode());

    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, secWindow->GetMode());
    ASSERT_EQ(WMError::WM_OK, secWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
}

/**
 * @tc.name: SplitScreen03
 * @tc.desc: one primary window and one fullscreen window, test rects
 * @tc.type: FUNC
 */
HWTEST_F(WindowSplitTest, SplitScreen03, Function | MediumTest | Level3)
{
    fullInfo_.name  = "fullscreen.3";
    splitInfo_.name = "primary.3";
    splitInfo_.mode = WindowMode::WINDOW_MODE_SPLIT_PRIMARY;

    ASSERT_TRUE(utils::InitSplitRects());

    const sptr<Window>& fullWindow = utils::CreateTestWindow(fullInfo_);
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);
    const sptr<Window>& priWindow = utils::CreateTestWindow(splitInfo_);
    activeWindows_.push_back(priWindow);
    ASSERT_EQ(WMError::WM_OK, priWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);

    utils::UpdateSplitRects(fullWindow);

    ASSERT_TRUE(utils::RectEqualTo(fullWindow, utils::splitRects_.secondaryRect));
    ASSERT_TRUE(utils::RectEqualTo(priWindow, utils::splitRects_.primaryRect));

    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
    ASSERT_EQ(WMError::WM_OK, priWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
}

/**
 * @tc.name: SplitScreen04
 * @tc.desc: one secondary window and one fullscreen window, test rects
 * @tc.type: FUNC
 */
HWTEST_F(WindowSplitTest, SplitScreen04, Function | MediumTest | Level3)
{
    fullInfo_.name  = "fullscreen.4";
    splitInfo_.name = "secondary.4";
    splitInfo_.mode = WindowMode::WINDOW_MODE_SPLIT_SECONDARY;

    ASSERT_TRUE(utils::InitSplitRects());

    const sptr<Window>& fullWindow = utils::CreateTestWindow(fullInfo_);
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);
    const sptr<Window>& secWindow = utils::CreateTestWindow(splitInfo_);
    activeWindows_.push_back(secWindow);
    ASSERT_EQ(WMError::WM_OK, secWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);

    utils::UpdateSplitRects(fullWindow);

    ASSERT_TRUE(utils::RectEqualTo(fullWindow, utils::splitRects_.primaryRect));
    ASSERT_TRUE(utils::RectEqualTo(secWindow, utils::splitRects_.secondaryRect));

    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
    ASSERT_EQ(WMError::WM_OK, secWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
}

/**
 * @tc.name: SplitCreen05
 * @tc.desc: forbid dock slive to move test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSplitTest, SplitCreen05, Function | MediumTest | Level3)
{
    fullInfo_.name  = "fullscreen.5";
    splitInfo_.name = "primary.5";
    splitInfo_.mode = WindowMode::WINDOW_MODE_SPLIT_PRIMARY;

    const sptr<Window>& fullWindow = utils::CreateTestWindow(fullInfo_);
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);

    const sptr<Window>& priWindow = utils::CreateTestWindow(splitInfo_);
    activeWindows_.push_back(priWindow);
    ASSERT_EQ(WMError::WM_OK, priWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);

    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, priWindow->GetMode());
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_SECONDARY, fullWindow->GetMode());

    utils::TestWindowInfo dividerInfo;
    dividerInfo.name = "divider0";
    dividerInfo.type = WindowType::WINDOW_TYPE_DOCK_SLICE;
    dividerInfo.mode = WindowMode::WINDOW_MODE_FLOATING;
    dividerInfo.focusable_ = false;

    const sptr<Window>& divider = utils::CreateTestWindow(dividerInfo);
    activeWindows_.push_back(divider);
    ASSERT_EQ(WMError::WM_OK, divider->Show());
    sleep(SPLIT_TEST_SLEEP_S);

    Rect lastRect = divider->GetRect();
    WMError ret = divider->MoveTo(lastRect.posX_ + 10, lastRect.posY_ + 10);
    sleep(SPLIT_TEST_SLEEP_S);
    Rect rect = divider->GetRect();
    ASSERT_TRUE(lastRect.posX_ != rect.posX_ || lastRect.posY_ != rect.posY_);

    ret = fullWindow->AddWindowFlag(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE);
    sleep(SPLIT_TEST_SLEEP_S);
    ret = divider->MoveTo(rect.posX_ + 10, rect.posY_ + 10);
    sleep(SPLIT_TEST_SLEEP_S);
    Rect NewRect = divider->GetRect();
    ASSERT_TRUE(rect.posX_ == NewRect.posX_ && rect.posY_ == NewRect.posY_);

    ASSERT_EQ(WMError::WM_OK, priWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, fullWindow->GetMode());
    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
}
}
} // namespace Rosen
} // namespace OHOS
