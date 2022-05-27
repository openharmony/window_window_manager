/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "singleton_container.h"
#include "wm_common.h"
#include "window_adapter.h"
#include "window_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using utils = WindowTestUtils;
const int WAIT_CALLBACK_US = 10000;  // 10000 us

class WindowOutsidePressedTestListener : public IOutsidePressedListener {
public:
    void OnOutsidePressed() override
    {
        isOutsidePressed_ = true;
    }
    bool isOutsidePressed_ { false };
};

class WindowOutsidePressedTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<WindowOutsidePressedTestListener> windowlistener1_;
    static sptr<WindowOutsidePressedTestListener> windowlistener2_;
    utils::TestWindowInfo firstWindowInfo_;
    utils::TestWindowInfo secondWindowInfo_;
    utils::TestWindowInfo thirdWindowInfo_;
};

sptr<WindowOutsidePressedTestListener> WindowOutsidePressedTest::windowlistener1_ =
    new WindowOutsidePressedTestListener();
sptr<WindowOutsidePressedTestListener> WindowOutsidePressedTest::windowlistener2_ =
    new WindowOutsidePressedTestListener();

void WindowOutsidePressedTest::SetUp()
{
        firstWindowInfo_ = {
        .name = "firstWindow",
        .rect = { 100, 100, 200, 200 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentName = "",
    };

    secondWindowInfo_ = {
        .name = "secondWindow",
        .rect = { 400, 400, 200, 200 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentName = "",
    };

    thirdWindowInfo_ = {
        .name = "thirdWindow",
        .rect = { 400, 400, 200, 200 },
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentName = "",
    };
}

void WindowOutsidePressedTest::TearDown()
{
    windowlistener1_->isOutsidePressed_ = false;
    windowlistener2_->isOutsidePressed_ = false;
}

void WindowOutsidePressedTest::SetUpTestCase()
{
}

void WindowOutsidePressedTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: onIutsidePressed
 * @tc.desc: can't not receive a insidepress event
 * @tc.type: FUNC
 */
HWTEST_F(WindowOutsidePressedTest, onIutsidePressed, Function | MediumTest | Level3)
{
    const sptr<Window> &firstWindow = utils::CreateTestWindow(firstWindowInfo_);
    firstWindow->RegisterOutsidePressedListener(windowlistener1_);
    firstWindow->Show();
    SingletonContainer::Get<WindowAdapter>().ProcessPointDown(firstWindow->GetWindowId());
    usleep(WAIT_CALLBACK_US);
    ASSERT_TRUE(!windowlistener1_->isOutsidePressed_);
    firstWindow->Destroy();
}

/**
 * @tc.name: onOutsidePressed
 * @tc.desc: received an outside pressed event when window state is show
 * @tc.type: FUNC
 */
HWTEST_F(WindowOutsidePressedTest, onOutsidePressed, Function | MediumTest | Level3)
{
    const sptr<Window> &firstWindow = utils::CreateTestWindow(firstWindowInfo_);
    firstWindow->RegisterOutsidePressedListener(windowlistener1_);
    const sptr<Window> &secondWindow = utils::CreateTestWindow(secondWindowInfo_);
    firstWindow->Show();
    secondWindow->Show();
    SingletonContainer::Get<WindowAdapter>().ProcessPointDown(secondWindow->GetWindowId());
    usleep(WAIT_CALLBACK_US);
    ASSERT_TRUE(windowlistener1_->isOutsidePressed_);
    firstWindow->Destroy();
    secondWindow->Destroy();
}

/**
 * @tc.name: onOutsidePressedNotShow
 * @tc.desc: If the window is not in the show state, the outpress event cannot be received
 * @tc.type: FUNC
 */
HWTEST_F(WindowOutsidePressedTest, onOutsidePressedNotShow, Function | MediumTest | Level3)
{
    const sptr<Window> &firstWindow = utils::CreateTestWindow(firstWindowInfo_);
    firstWindow->RegisterOutsidePressedListener(windowlistener1_);
    const sptr<Window> &secondWindow = utils::CreateTestWindow(secondWindowInfo_);
    secondWindow->Show();
    SingletonContainer::Get<WindowAdapter>().ProcessPointDown(secondWindow->GetWindowId());
    usleep(WAIT_CALLBACK_US);
    ASSERT_TRUE(!windowlistener1_->isOutsidePressed_);
    firstWindow->Destroy();
    secondWindow->Destroy();
}

/**
 * @tc.name: onOutsidePressedForAllWindow
 * @tc.desc: All windows can receive the outpress event
 * @tc.type: FUNC
 */
HWTEST_F(WindowOutsidePressedTest, onOutsidePressedForAllWindow, Function | MediumTest | Level3)
{
    const sptr<Window> &firstWindow = utils::CreateTestWindow(firstWindowInfo_);
    firstWindow->RegisterOutsidePressedListener(windowlistener1_);
    const sptr<Window> &secondWindow = utils::CreateTestWindow(secondWindowInfo_);
    firstWindow->RegisterOutsidePressedListener(windowlistener2_);
    firstWindow->Show();
    secondWindow->Show();
    const sptr<Window> &thirdWindow = utils::CreateTestWindow(thirdWindowInfo_);
    thirdWindow->Show();
    SingletonContainer::Get<WindowAdapter>().ProcessPointDown(thirdWindow->GetWindowId());
    usleep(WAIT_CALLBACK_US);
    ASSERT_TRUE(windowlistener1_->isOutsidePressed_);
    ASSERT_TRUE(windowlistener2_->isOutsidePressed_);
    firstWindow->Destroy();
    secondWindow->Destroy();
    thirdWindow->Destroy();
}
} // namespace
} // Rosen
} // OHOS