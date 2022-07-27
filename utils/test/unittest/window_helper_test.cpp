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

#include <gtest/gtest.h>

#include "window_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowHelperTest::SetUpTestCase()
{
}

void WindowHelperTest::TearDownTestCase()
{
}

void WindowHelperTest::SetUp()
{
}

void WindowHelperTest::TearDown()
{
}

namespace {
/**
 * @tc.name: WindowTypeWindowMode
 * @tc.desc: window type/mode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowHelperTest, WindowTypeWindowMode, Function | SmallTest | Level1)
{
    ASSERT_EQ(true, WindowHelper::IsMainFullScreenWindow(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
                                                         WindowMode::WINDOW_MODE_FULLSCREEN));
    ASSERT_EQ(false, WindowHelper::IsMainFullScreenWindow(WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
                                                          WindowMode::WINDOW_MODE_FULLSCREEN));
    ASSERT_EQ(false, WindowHelper::IsMainFullScreenWindow(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
                                                          WindowMode::WINDOW_MODE_SPLIT_PRIMARY));

    ASSERT_EQ(true, WindowHelper::IsFloatingWindow(WindowMode::WINDOW_MODE_FLOATING));
    ASSERT_EQ(false, WindowHelper::IsFloatingWindow(WindowMode::WINDOW_MODE_FULLSCREEN));

    ASSERT_EQ(true, WindowHelper::IsFullScreenWindow(WindowMode::WINDOW_MODE_FULLSCREEN));
    ASSERT_EQ(false, WindowHelper::IsFullScreenWindow(WindowMode::WINDOW_MODE_FLOATING));
}

/**
 * @tc.name: WindowModeSupport
 * @tc.desc: window mode supported test
 * @tc.type: FUNC
 */
HWTEST_F(WindowHelperTest, WindowModeSupport, Function | SmallTest | Level1)
{
    ASSERT_EQ(true, WindowHelper::IsWindowModeSupported(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL,
                                                        WindowMode::WINDOW_MODE_FULLSCREEN));
    ASSERT_EQ(true, WindowHelper::IsWindowModeSupported(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL,
                                                        WindowMode::WINDOW_MODE_FLOATING));
    ASSERT_EQ(true, WindowHelper::IsWindowModeSupported(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL,
                                                        WindowMode::WINDOW_MODE_SPLIT_PRIMARY));
    ASSERT_EQ(true, WindowHelper::IsWindowModeSupported(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL,
                                                        WindowMode::WINDOW_MODE_SPLIT_SECONDARY));
    ASSERT_EQ(true, WindowHelper::IsWindowModeSupported(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL,
                                                        WindowMode::WINDOW_MODE_PIP));
    ASSERT_EQ(true, WindowHelper::IsWindowModeSupported(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL,
                                                        WindowMode::WINDOW_MODE_UNDEFINED));
}

/**
 * @tc.name: WindowRect
 * @tc.desc: rect test
 * @tc.type: FUNC
 */
HWTEST_F(WindowHelperTest, WindowRect, Function | SmallTest | Level1)
{
    Rect rect0 = {0, 0, 0, 0};
    ASSERT_EQ(true, WindowHelper::IsEmptyRect(rect0));

    Rect rect1 = {0, 0, 1, 1};
    ASSERT_EQ(false, WindowHelper::IsEmptyRect(rect1));
}

/**
 * @tc.name: WindowStringUtil
 * @tc.desc: string test
 * @tc.type: FUNC
 */
HWTEST_F(WindowHelperTest, WindowStringUtil, Function | SmallTest | Level1)
{
    ASSERT_EQ(true, WindowHelper::IsNumber("123"));
    ASSERT_EQ(false, WindowHelper::IsNumber("1a3"));
    ASSERT_EQ(false, WindowHelper::IsNumber(""));

    ASSERT_EQ(true, WindowHelper::IsFloatingNumber("1.23"));
    ASSERT_EQ(true, WindowHelper::IsFloatingNumber(".123"));
    ASSERT_EQ(false, WindowHelper::IsFloatingNumber("1a3"));
    ASSERT_EQ(false, WindowHelper::IsFloatingNumber("123.."));

    std::vector<std::string> vec = WindowHelper::Split("123a123a123", "a");
    for (size_t i = 0; i < vec.size(); i++) {
        if (vec[i].compare("123")) {
            ASSERT_TRUE(false);
        }
    }
    ASSERT_TRUE(true);
}
}
} // namespace Rosen
} // namespace OHOS