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
class WindowEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    utils::TestWindowInfo fullScreenAppInfo_;
};

void WindowEffectTest::SetUpTestCase()
{
}

void WindowEffectTest::TearDownTestCase()
{
}

void WindowEffectTest::SetUp()
{
    fullScreenAppInfo_ = {
            .name = "FullWindow",
            .rect = utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
            .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
            .needAvoid = false,
            .parentLimit = false,
            .parentName = "",
    };
}

void WindowEffectTest::TearDown()
{
}

namespace {
/**
 * @tc.name: WindowEffect01
 * @tc.desc: SetWindowBackgroundBlur | GetWindowBackgroundBlur
 * @tc.type: FUNC
 */
HWTEST_F(WindowEffectTest, WindowEffect01, Function | MediumTest | Level3)
{
    const sptr<Window>& window = utils::CreateTestWindow(fullScreenAppInfo_);

    ASSERT_EQ(WMError::WM_OK, window->SetWindowBackgroundBlur(WindowBlurLevel::WINDOW_BLUR_LOW));
    ASSERT_EQ(WindowBlurLevel::WINDOW_BLUR_LOW, window->GetWindowBackgroundBlur());
    window->Destroy();
}

/**
 * @tc.name: WindowEffect02
 * @tc.desc: SetAlpha | GetAlpha
 * @tc.type: FUNC
 */
HWTEST_F(WindowEffectTest, WindowEffect02, Function | MediumTest | Level3)
{
    const sptr<Window>& window = utils::CreateTestWindow(fullScreenAppInfo_);

    ASSERT_EQ(WMError::WM_OK, window->SetAlpha(1.0f));
    ASSERT_EQ(1.0f, window->GetAlpha());

    window->Destroy();
}
} // namespace
} // namespace Rosen
} // namespace OHOS
