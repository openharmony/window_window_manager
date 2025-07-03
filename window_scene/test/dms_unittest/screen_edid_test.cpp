/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <parameter.h>
#include <parameters.h>

#include "screen_session_manager/include/screen_edid_parse.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class ScreenEdidTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};


void ScreenEdidTest::SetUpTestCase()
{
}

void ScreenEdidTest::TearDownTestCase()
{
}

void ScreenEdidTest::SetUp()
{
}

void ScreenEdidTest::TearDown()
{
    usleep(SLEEP_TIME_IN_US);
}

namespace {
/**
 * @tc.name: LoadEdidPlugin01
 * @tc.desc: LoadEdidPlugin01 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenEdidTest, LoadEdidPlugin01, Function | SmallTest | Level3)
{
    bool result = LoadEdidPlugin();
    ASSERT_TRUE(result);
    // second enter another branch
    result = LoadEdidPlugin();
    ASSERT_TRUE(result);
    UnloadEdidPlugin();
}

/**
 * @tc.name: GetEdid01
 * @tc.desc: GetEdid01 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenEdidTest, GetEdid01, Function | SmallTest | Level3)
{
    ScreenId screenId = 1;
    BaseEdid edid;
    ASSERT_FALSE(GetEdid(screenId, edid));
}

/**
 * @tc.name: GetEdidCheckCode01
 * @tc.desc: GetEdidCheckCode01 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenEdidTest, GetEdidCheckCode01, Function | SmallTest | Level3)
{
    std::vector<uint8_t> edidData = {1, 2, 3, 4, 5};
    int32_t result = GetEdidCheckCode(edidData);
    int32_t invalid_code = -1;
    ASSERT_EQ(result, invalid_code);
}

/**
 * @tc.name: GetEdidCheckCode02
 * @tc.desc: GetEdidCheckCode02 test
 * @tc.type: FUNC
 */
 HWTEST_F(ScreenEdidTest, GetEdidCheckCode02, Function | SmallTest | Level3)
 {
    std::vector<uint8_t> edidData(20, 0);
    for (uint32_t i = 0; i < edidData.size(); i++) {
        edidData[i] = i + 1;
    }
    int32_t result = GetEdidCheckCode(edidData);
    int32_t expected = 135;
    ASSERT_EQ(result, expected);
}
}
}
}