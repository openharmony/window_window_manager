/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
class WindowGamutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    utils::TestWindowInfo fullScreenAppInfo_;
};

void WindowGamutTest::SetUpTestCase()
{
}

void WindowGamutTest::TearDownTestCase()
{
}

void WindowGamutTest::SetUp()
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

void WindowGamutTest::TearDown()
{
}

namespace {
/**
 * @tc.name: IsSupportWideGamut01
 * @tc.desc: IsSupportWideGamut
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowGamutTest, IsSupportWideGamut01, Function | MediumTest | Level3)
{
    const sptr<Window>& window = utils::CreateTestWindow(fullScreenAppInfo_);

    ASSERT_EQ(true, window->IsSupportWideGamut());

    window->Destroy();
}

/**
 * @tc.name: SetGetColorSpace01
 * @tc.desc: Set and Get ColorSpace
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(WindowGamutTest, SetGetColorSpace01, Function | MediumTest | Level3)
{
    const sptr<Window>& window = utils::CreateTestWindow(fullScreenAppInfo_);

    window->SetColorSpace(ColorSpace::COLOR_SPACE_DEFAULT);
    window->SetColorSpace(ColorSpace::COLOR_SPACE_WIDE_GAMUT);
    ColorSpace invalidColorSpace =
        static_cast<ColorSpace>(static_cast<uint32_t>(ColorSpace::COLOR_SPACE_WIDE_GAMUT) + 1);
    window->SetColorSpace(invalidColorSpace);  // invalid param
    ASSERT_EQ(ColorSpace::COLOR_SPACE_DEFAULT, window->GetColorSpace());

    window->Destroy();
}
} // namespace
} // namespace Rosen
} // namespace OHOS
