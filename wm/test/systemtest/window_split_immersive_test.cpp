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
#include "avoid_area_controller.h"
#include "window_manager.h"
#include "window_test_utils.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using utils = WindowTestUtils;

class WindowSplitImmersiveTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    void IsShowTopBar(const sptr<Window>& top, bool isShow);
    void HideAndUnregister(const sptr<Window>& fullWindow, const sptr<Window>& priWindow, const sptr<Window>& top);

    std::vector<sptr<Window>> activeWindows_;
    utils::TestWindowInfo fullInfo_;
    utils::TestWindowInfo splitInfo_;

private:
    static constexpr uint32_t SPLIT_TEST_SLEEP_S = 1; // split test sleep time
};

void WindowSplitImmersiveTest::SetUpTestCase()
{
}

void WindowSplitImmersiveTest::TearDownTestCase()
{
}

void WindowSplitImmersiveTest::SetUp()
{
    fullInfo_ = {
        .name = "fullscreen.1",
        .rect = utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };

    splitInfo_ = {
        .name = "primary.1",
        .rect = utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = true,
        .parentLimit = false,
        .parentName = "",
    };

    activeWindows_.clear();
}

void WindowSplitImmersiveTest::TearDown()
{
    while (!activeWindows_.empty()) {
        ASSERT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
}

namespace {
/**
 * @tc.name: SplitImmersive01
 * @tc.desc: one primary window and one fullscreen window, test enter and out split immersive
 * @tc.type: FUNC
 */
HWTEST_F(WindowSplitImmersiveTest, SplitImmersive01, Function | MediumTest | Level3)
{
    // create fullscreen win and show
    const sptr<Window>& fullWindow = utils::CreateTestWindow(fullInfo_);
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);

    // enter split mode
    splitInfo_.mode = WindowMode::WINDOW_MODE_SPLIT_PRIMARY;
    const sptr<Window>& priWindow = utils::CreateTestWindow(splitInfo_);
    activeWindows_.push_back(priWindow);
    ASSERT_EQ(WMError::WM_OK, priWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);

    // check is enter split Immersive
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, priWindow->GetMode());
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_SECONDARY, fullWindow->GetMode());
    Rect immersivePriRect = priWindow->GetRect();
    ASSERT_EQ(0, immersivePriRect.posX_);
    ASSERT_EQ(0, immersivePriRect.posY_);
    sleep(SPLIT_TEST_SLEEP_S);

    // Exit split Mode.
    ASSERT_EQ(WMError::WM_OK, priWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);

    // check is out split Immersive
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, fullWindow->GetMode());
    Rect curFullScreenRect = fullWindow->GetRect();
    AvoidArea avoidArea;
    fullWindow->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM, avoidArea);
    ASSERT_EQ(avoidArea.topRect_.height_, curFullScreenRect.posY_);

    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
    ASSERT_EQ(WMError::WM_OK, priWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
}
}
} // namespace Rosen
} // namespace OHOS
