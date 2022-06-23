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

#include "window_effect_test.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
void WindowEffectTest::SetUpTestCase()
{
}

void WindowEffectTest::TearDownTestCase()
{
}

void WindowEffectTest::SetUp()
{
}

void WindowEffectTest::TearDown()
{
}

namespace {
/**
 * @tc.name: WindowEffect01
 * @tc.desc: windowOption: set window background blur / get window background blur
 * @tc.type: FUNC
 */
HWTEST_F(WindowEffectTest, WindowEffect01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowBackgroundBlur(WindowBlurLevel::WINDOW_BLUR_LOW);
    ASSERT_EQ(WindowBlurLevel::WINDOW_BLUR_LOW, option->GetWindowBackgroundBlur());
    option->SetWindowBackgroundBlur(WindowBlurLevel::WINDOW_BLUR_MEDIUM);
    ASSERT_EQ(WindowBlurLevel::WINDOW_BLUR_MEDIUM, option->GetWindowBackgroundBlur());
    option->SetWindowBackgroundBlur(WindowBlurLevel::WINDOW_BLUR_HIGH);
    ASSERT_EQ(WindowBlurLevel::WINDOW_BLUR_HIGH, option->GetWindowBackgroundBlur());
    option->SetWindowBackgroundBlur(WindowBlurLevel::WINDOW_BLUR_OFF);
    ASSERT_EQ(WindowBlurLevel::WINDOW_BLUR_OFF, option->GetWindowBackgroundBlur());
}

/**
 * @tc.name: WindowEffect02
 * @tc.desc: WindowImp: Create window with no default option, get and check Property
 * @tc.type: FUNC
 */
HWTEST_F(WindowEffectTest, WindowEffect02, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowBackgroundBlur(WindowBlurLevel::WINDOW_BLUR_LOW);
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));

    ASSERT_EQ(WindowBlurLevel::WINDOW_BLUR_LOW, window->GetWindowBackgroundBlur());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: WindowEffect03
 * @tc.desc: Create window with no default option, get and check Property
 * @tc.type: FUNC
 */
HWTEST_F(WindowEffectTest, WindowEffect03, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));
    window->SetAlpha(0.1f);
    ASSERT_EQ(0.1f, window->GetAlpha());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: WindowEffect04
 * @tc.desc: set window effect parameters throw window, and check parameters.
 * @tc.type: FUNC
 */
HWTEST_F(WindowEffectTest, WindowEffect04, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);

    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));

    EXPECT_CALL(m->Mock(), SetWindowBackgroundBlur(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetWindowBackgroundBlur(WindowBlurLevel::WINDOW_BLUR_LOW));
    ASSERT_EQ(WindowBlurLevel::WINDOW_BLUR_LOW, window->GetWindowBackgroundBlur());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    window->Destroy();
}

/**
 * @tc.name: WindowEffect05
 * @tc.desc: WindowImp: Create window with default option, get and check Property
 * @tc.type: FUNC
 */
HWTEST_F(WindowEffectTest, WindowEffect05, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowImpl> window = new WindowImpl(option);
    EXPECT_CALL(m->Mock(), CreateWindow(_, _, _, _, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(""));

    ASSERT_EQ(WindowBlurLevel::WINDOW_BLUR_OFF, window->GetWindowBackgroundBlur());
    ASSERT_EQ(1.0f, window->GetAlpha());

    EXPECT_CALL(m->Mock(), DestroyWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}
}
} // namespace Rosen
} // namespace OHOS
