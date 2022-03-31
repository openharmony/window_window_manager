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
#include "window_test_utils.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using utils = WindowTestUtils;
class WindowInputMethodTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    utils::TestWindowInfo inputMethodWindowInfo_;
    utils::TestWindowInfo keyGuardWindowInfo_;
};

void WindowInputMethodTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    utils::InitByDisplayRect(displayRect);
}

void WindowInputMethodTest::TearDownTestCase()
{
}

void WindowInputMethodTest::SetUp()
{
    inputMethodWindowInfo_ = {
        .name = "",
        .rect = utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentName = "",
    };
    keyGuardWindowInfo_ = {
        .name = "",
        .rect = utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_KEYGUARD,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = false,
        .parentLimit = false,
        .parentName = "",
    };
}

void WindowInputMethodTest::TearDown()
{
}

namespace {
/**
 * @tc.name: InputMethodWindow01
 * @tc.desc: One InputMethod Floating Window
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputMethodTest, InputMethodWindow01, Function | MediumTest | Level3)
{
    inputMethodWindowInfo_.name = "input_method.1";
    const sptr<Window>& window = utils::CreateTestWindow(inputMethodWindowInfo_);
    ASSERT_EQ(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT, window->GetType());
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_OK, window->Hide());
}

/**
 * @tc.name: InputMethodWindow02
 * @tc.desc: One KeyGuard Window
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputMethodTest, InputMethodWindow02, Function | MediumTest | Level3)
{
    keyGuardWindowInfo_.name  = "keyGuard.1";
    const sptr<Window>& window = utils::CreateTestWindow(keyGuardWindowInfo_);
    ASSERT_EQ(WindowType::WINDOW_TYPE_KEYGUARD, window->GetType());
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_OK, window->Hide());
}

/**
 * @tc.name: InputMethodWindow03
 * @tc.desc: One InputMethod Floating Window & One KeyGuard Window
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputMethodTest, InputMethodWindow03, Function | MediumTest | Level3)
{
    inputMethodWindowInfo_.name = "input_method.2";
    keyGuardWindowInfo_.name  = "keyGuard.2";
    const sptr<Window>& inputMethodWindow = utils::CreateTestWindow(inputMethodWindowInfo_);
    const sptr<Window>& keyGuardWindow = utils::CreateTestWindow(keyGuardWindowInfo_);
    keyGuardWindow->Show();
    inputMethodWindow->Show();
    ASSERT_TRUE(utils::RectEqualTo(keyGuardWindow, utils::displayRect_));
    ASSERT_TRUE(utils::RectEqualTo(inputMethodWindow, utils::customAppRect_));
}
} // namespace
} // namespace Rosen
} // namespace OHOS
