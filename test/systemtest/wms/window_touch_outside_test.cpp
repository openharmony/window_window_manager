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
#include "window_impl.h"
#include "window_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
const int WAIT_CALLBACK_US = 10000;  // 10000 us

class WindowTouchOutsideTestListener : public ITouchOutsideListener {
public:
    void OnTouchOutside() const override
    {
        isTouchOutside_ = true;
    }
    mutable bool isTouchOutside_ { false };
};

class WindowTouchOutsideTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<WindowTouchOutsideTestListener> windowlistener1_;
    static sptr<WindowTouchOutsideTestListener> windowlistener2_;
    Utils::TestWindowInfo firstWindowInfo_;
    Utils::TestWindowInfo secondWindowInfo_;
    Utils::TestWindowInfo thirdWindowInfo_;
};

sptr<WindowTouchOutsideTestListener> WindowTouchOutsideTest::windowlistener1_ =
    new WindowTouchOutsideTestListener();
sptr<WindowTouchOutsideTestListener> WindowTouchOutsideTest::windowlistener2_ =
    new WindowTouchOutsideTestListener();

void WindowTouchOutsideTest::SetUp()
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

void WindowTouchOutsideTest::TearDown()
{
    windowlistener1_->isTouchOutside_ = false;
    windowlistener2_->isTouchOutside_ = false;
}

void WindowTouchOutsideTest::SetUpTestCase()
{
}

void WindowTouchOutsideTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: onTouchInside
 * @tc.desc: can't not receive a inside touch event
 * @tc.type: FUNC
 */
HWTEST_F(WindowTouchOutsideTest, onTouchInside, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName(firstWindowInfo_.name);
    option->SetWindowMode(firstWindowInfo_.mode);
    option->SetWindowType(firstWindowInfo_.type);
    option->SetWindowRect(firstWindowInfo_.rect);
    auto firstWindow = new WindowImpl(option);
    firstWindow->Create("");
    firstWindow->RegisterTouchOutsideListener(windowlistener1_);
    firstWindow->Show();
    SingletonContainer::Get<WindowAdapter>().ProcessPointDown(firstWindow->GetWindowId(),
        firstWindow->property_, firstWindow->moveDragProperty_);
    usleep(WAIT_CALLBACK_US);
    ASSERT_TRUE(!windowlistener1_->isTouchOutside_);
    firstWindow->Destroy();
}

/**
 * @tc.name: onTouchOutside
 * @tc.desc: received an outside touch event when window state is show
 * @tc.type: FUNC
 */
HWTEST_F(WindowTouchOutsideTest, onTouchOutside, Function | MediumTest | Level3)
{
    const sptr<Window> &firstWindow = Utils::CreateTestWindow(firstWindowInfo_);
    firstWindow->RegisterTouchOutsideListener(windowlistener1_);

    sptr<WindowOption> secondOption = new WindowOption();
    secondOption->SetWindowName(secondWindowInfo_.name);
    secondOption->SetWindowMode(secondWindowInfo_.mode);
    secondOption->SetWindowType(secondWindowInfo_.type);
    secondOption->SetWindowRect(secondWindowInfo_.rect);
    auto secondWindow = new WindowImpl(secondOption);
    secondWindow->Create("");
    firstWindow->Show();
    secondWindow->Show();
    SingletonContainer::Get<WindowAdapter>().ProcessPointDown(secondWindow->GetWindowId(),
        secondWindow->property_, secondWindow->moveDragProperty_);
    usleep(WAIT_CALLBACK_US);
    ASSERT_TRUE(windowlistener1_->isTouchOutside_);
    firstWindow->Destroy();
    secondWindow->Destroy();
}

/**
 * @tc.name: onTouchOutsideNotShow
 * @tc.desc: If the window is not in the show state, the touch outside event cannot be received
 * @tc.type: FUNC
 */
HWTEST_F(WindowTouchOutsideTest, onTouchOutsideNotShow, Function | MediumTest | Level3)
{
    const sptr<Window> &firstWindow = Utils::CreateTestWindow(firstWindowInfo_);
    firstWindow->RegisterTouchOutsideListener(windowlistener1_);

    sptr<WindowOption> secondOption = new WindowOption();
    secondOption->SetWindowName(secondWindowInfo_.name);
    secondOption->SetWindowMode(secondWindowInfo_.mode);
    secondOption->SetWindowType(secondWindowInfo_.type);
    secondOption->SetWindowRect(secondWindowInfo_.rect);
    auto secondWindow = new WindowImpl(secondOption);
    secondWindow->Create("");
    secondWindow->Show();
    SingletonContainer::Get<WindowAdapter>().ProcessPointDown(secondWindow->GetWindowId(),
        secondWindow->property_, secondWindow->moveDragProperty_);
    usleep(WAIT_CALLBACK_US);
    ASSERT_TRUE(!windowlistener1_->isTouchOutside_);
    firstWindow->Destroy();
    secondWindow->Destroy();
}

/**
 * @tc.name: onTouchOutsideForAllWindow
 * @tc.desc: All windows can receive the touch outside event
 * @tc.type: FUNC
 */
HWTEST_F(WindowTouchOutsideTest, onTouchOutsideForAllWindow, Function | MediumTest | Level3)
{
    const sptr<Window> &firstWindow = Utils::CreateTestWindow(firstWindowInfo_);
    firstWindow->RegisterTouchOutsideListener(windowlistener1_);
    const sptr<Window> &secondWindow = Utils::CreateTestWindow(secondWindowInfo_);
    firstWindow->RegisterTouchOutsideListener(windowlistener2_);

    firstWindow->Show();
    secondWindow->Show();

    sptr<WindowOption> thirdOption = new WindowOption();
    thirdOption->SetWindowName(thirdWindowInfo_.name);
    thirdOption->SetWindowMode(thirdWindowInfo_.mode);
    thirdOption->SetWindowType(thirdWindowInfo_.type);
    thirdOption->SetWindowRect(thirdWindowInfo_.rect);
    auto thirdWindow = new WindowImpl(thirdOption);
    thirdWindow->Create("");
    thirdWindow->Show();
    SingletonContainer::Get<WindowAdapter>().ProcessPointDown(thirdWindow->GetWindowId(),
        thirdWindow->property_, thirdWindow->moveDragProperty_);
    usleep(WAIT_CALLBACK_US);
    ASSERT_TRUE(windowlistener1_->isTouchOutside_);
    ASSERT_TRUE(windowlistener2_->isTouchOutside_);
    firstWindow->Destroy();
    secondWindow->Destroy();
    thirdWindow->Destroy();
}
} // namespace
} // Rosen
} // OHOS