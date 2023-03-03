/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "utils/window_manager_service_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WmsUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WmsUtilsTest::SetUpTestCase()
{
}

void WmsUtilsTest::TearDownTestCase()
{
}

void WmsUtilsTest::SetUp()
{
}

void WmsUtilsTest::TearDown()
{
}
namespace {
/**
 * @tc.name: IsFixedOrientation01
 * @tc.desc: normal test
 * @tc.type: FUNC
 */
HWTEST_F(WmsUtilsTest, IsFixedOrientation01, Function | SmallTest | Level2)
{
    bool ret = WmsUtils::IsFixedOrientation(Orientation::HORIZONTAL, WindowMode::WINDOW_MODE_FULLSCREEN, 0);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: IsFixedOrientation02
 * @tc.desc: more then Orientation::REVERSE_HORIZONTAL
 * @tc.type: FUNC
 */
HWTEST_F(WmsUtilsTest, IsFixedOrientation02, Function | SmallTest | Level2)
{
    bool ret = WmsUtils::IsFixedOrientation(Orientation::SENSOR, WindowMode::WINDOW_MODE_FULLSCREEN, 0);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: IsFixedOrientation03
 * @tc.desc: less then Orientation::VERTICAL
 * @tc.type: FUNC
 */
HWTEST_F(WmsUtilsTest, IsFixedOrientation03, Function | SmallTest | Level2)
{
    bool ret = WmsUtils::IsFixedOrientation(Orientation::UNSPECIFIED, WindowMode::WINDOW_MODE_FULLSCREEN, 0);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: IsFixedOrientation04
 * @tc.desc: flags is WindowFlag::WINDOW_FLAG_NEED_AVOID
 * @tc.type: FUNC
 */
HWTEST_F(WmsUtilsTest, IsFixedOrientation04, Function | SmallTest | Level2)
{
    bool ret = WmsUtils::IsFixedOrientation(Orientation::HORIZONTAL, WindowMode::WINDOW_MODE_FULLSCREEN,
        static_cast<int32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: IsFixedOrientation05
 * @tc.desc: mode is not WindowMode::WINDOW_MODE_FULLSCREEN
 * @tc.type: FUNC
 */
HWTEST_F(WmsUtilsTest, IsFixedOrientation05, Function | SmallTest | Level2)
{
    bool ret = WmsUtils::IsFixedOrientation(Orientation::HORIZONTAL, WindowMode::WINDOW_MODE_FLOATING, 0);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: IsExpectedRotateLandscapeWindow01
 * @tc.desc: requestOrientation is not HORIZONTAL
 * @tc.type: FUNC
 */
HWTEST_F(WmsUtilsTest, IsExpectedRotateLandscapeWindow01, Function | SmallTest | Level2)
{
    bool ret = WmsUtils::IsExpectedRotateLandscapeWindow(Orientation::VERTICAL, DisplayOrientation::PORTRAIT, 0);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: IsExpectedRotatableWindow01
 * @tc.desc: flags is WindowFlag::WINDOW_FLAG_NEED_AVOID
 * @tc.type: FUNC
 */
HWTEST_F(WmsUtilsTest, IsExpectedRotatableWindow01, Function | SmallTest | Level2)
{
    bool ret = WmsUtils::IsExpectedRotatableWindow(Orientation::VERTICAL, DisplayOrientation::PORTRAIT,
        static_cast<int32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: IsExpectedRotatableWindow02
 * @tc.desc: disOrientation == currentOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WmsUtilsTest, IsExpectedRotatableWindow02, Function | SmallTest | Level2)
{
    bool ret = WmsUtils::IsExpectedRotatableWindow(Orientation::VERTICAL, DisplayOrientation::PORTRAIT, 0);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: IsExpectedRotatableWindow03
 * @tc.desc: disOrientation == currentOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WmsUtilsTest, IsExpectedRotatableWindow03, Function | SmallTest | Level2)
{
    bool ret = WmsUtils::IsExpectedRotatableWindow(Orientation::VERTICAL, DisplayOrientation::PORTRAIT, 0);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: IsExpectedRotatableWindow04
 * @tc.desc: disOrientation - currentOrientation == 2
 * @tc.type: FUNC
 */
HWTEST_F(WmsUtilsTest, IsExpectedRotatableWindow04, Function | SmallTest | Level2)
{
    bool ret = WmsUtils::IsExpectedRotatableWindow(Orientation::REVERSE_VERTICAL, DisplayOrientation::PORTRAIT, 0);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: IsExpectedRotatableWindow05
 * @tc.desc: disOrientation - currentOrientation == 2
 * @tc.type: FUNC
 */
HWTEST_F(WmsUtilsTest, IsExpectedRotatableWindow05, Function | SmallTest | Level2)
{
    bool ret = WmsUtils::IsExpectedRotatableWindow(Orientation::REVERSE_VERTICAL,
        DisplayOrientation::PORTRAIT, 0, false);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: IsExpectedRotatableWindow06
 * @tc.desc: mode is not WindowMode::WINDOW_MODE_FULLSCREEN
 * @tc.type: FUNC
 */
HWTEST_F(WmsUtilsTest, IsExpectedRotatableWindow06, Function | SmallTest | Level2)
{
    bool ret = WmsUtils::IsExpectedRotatableWindow(Orientation::REVERSE_VERTICAL,
        DisplayOrientation::PORTRAIT,  WindowMode::WINDOW_MODE_FLOATING, 0);
    ASSERT_FALSE(ret);
}
}
}
}