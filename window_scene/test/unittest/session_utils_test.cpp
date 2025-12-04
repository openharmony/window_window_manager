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

#include "session_utils.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace SessionUtils {
class SessionUtilsTest : public Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SessionUtilsTest::SetUpTestCase() {}
void SessionUtilsTest::TearDownTestCase() {}

void SessionUtilsTest::SetUp() {}
void SessionUtilsTest::TearDown() {}

/**
 * @tc.name: TestIsAspectRatioValid
 * @tc.desc: Verify IsAspectRatioValid behavior in different scenarios
 * @tc.type: FUNC
 */
HWTEST_F(SessionUtilsTest, TestIsAspectRatioValid, TestSize.Level1)
{
    // Case 1: aspectRatio is near 0
    {
        WindowLimits limits(400, 400, 100, 100, FLT_MAX, 0.0f);
        WindowDecoration decor{0, 0, 0, 0};
        EXPECT_TRUE(IsAspectRatioValid(0.0f, limits, decor));
    }

    // Case 2: limits is invalid after trim
    {
        WindowLimits limits(100, 100, 200, 200, FLT_MAX, 0.0f);
        WindowDecoration decor{50, 50, 50, 50};
        EXPECT_FALSE(IsAspectRatioValid(1.0f, limits, decor));
    }

    // Case 3: aspectRatio < minRatio
    {
        WindowLimits limits(400, 200, 200, 100, FLT_MAX, 0.0f);
        WindowDecoration decor{0, 0, 0, 0};
        EXPECT_FALSE(IsAspectRatioValid(0.5f, limits, decor));
    }

    // Case 4: aspectRatio > maxRatio
    {
        WindowLimits limits(400, 200, 200, 100, FLT_MAX, 0.0f);
        WindowDecoration decor{0, 0, 0, 0};
        EXPECT_FALSE(IsAspectRatioValid(5.0f, limits, decor));
    }

    // Case 5: aspectRatio is valid
    {
        WindowLimits limits(400, 200, 200, 100, FLT_MAX, 0.0f);
        WindowDecoration decor{0, 0, 0, 0};
        EXPECT_TRUE(IsAspectRatioValid(2.0f, limits, decor));
    }
}

/**
 * @tc.name: TestAdjustLimitsByAspectRatio1
 * @tc.desc: Verify AdjustLimitsByAspectRatio behavior in different scenarios
 * @tc.type: FUNC
 */
HWTEST_F(SessionUtilsTest, TestAdjustLimitsByAspectRatio1, TestSize.Level1)
{
    WindowLimits baseLimits(400, 400, 100, 100, FLT_MAX, 0.0f);
    WindowDecoration noDecor {0, 0, 0, 0};

    // Case 1: aspectRatio == 0 (NearZero)
    {
        float aspectRatio = 0.0f;
        auto adjusted = AdjustLimitsByAspectRatio(baseLimits, noDecor, aspectRatio);
        EXPECT_EQ(adjusted.minWidth_, baseLimits.minWidth_);
        EXPECT_EQ(adjusted.minHeight_, baseLimits.minHeight_);
        EXPECT_EQ(adjusted.maxWidth_, baseLimits.maxWidth_);
        EXPECT_EQ(adjusted.maxHeight_, baseLimits.maxHeight_);
    }

    // Case 2: Trim -> invalid (min > max)
    {
        float aspectRatio = 1.0f;
        WindowLimits limits(50, 50, 100, 100, FLT_MAX, 0.0f);
        WindowDecoration decor {60, 60, 0, 0};
        auto adjusted = AdjustLimitsByAspectRatio(limits, decor, aspectRatio);
        EXPECT_EQ(adjusted.minWidth_, limits.minWidth_);
        EXPECT_EQ(adjusted.minHeight_, limits.minHeight_);
        EXPECT_EQ(adjusted.maxWidth_, limits.maxWidth_);
        EXPECT_EQ(adjusted.maxHeight_, limits.maxHeight_);
    }

    // Case 3: Adjust minWidth and maxHeight by aspect ratio
    {
        float aspectRatio = 2.0f;
        WindowLimits limits(400, 400, 50, 100, FLT_MAX, 0.0f);
        auto adjusted = AdjustLimitsByAspectRatio(limits, noDecor, aspectRatio);
        EXPECT_GE(adjusted.minWidth_, adjusted.minHeight_ * aspectRatio);
        EXPECT_LE(adjusted.maxHeight_, adjusted.maxWidth_ / aspectRatio);
    }

    // Case 4: Adjust minHeight and maxWidth by aspect ratio
    {
        float aspectRatio = 0.5f;
        WindowLimits limits(400, 400, 100, 50, FLT_MAX, 0.0f);
        auto adjusted = AdjustLimitsByAspectRatio(limits, noDecor, aspectRatio);
        EXPECT_GE(adjusted.minHeight_, adjusted.minWidth_ / aspectRatio);
        EXPECT_LE(adjusted.maxWidth_, adjusted.maxHeight_ * aspectRatio);
    }

    // Case 5: Adjusted is invalid (min > max) after aspect ratio adjustment
    {
        float aspectRatio = 4.0f;
        WindowLimits limits(400, 400, 200, 110, FLT_MAX, 0.0f);
        auto adjusted = AdjustLimitsByAspectRatio(limits, noDecor, aspectRatio);
        EXPECT_EQ(adjusted.minWidth_, limits.minWidth_);
        EXPECT_EQ(adjusted.minHeight_, limits.minHeight_);
        EXPECT_EQ(adjusted.maxWidth_, limits.maxWidth_);
        EXPECT_EQ(adjusted.maxHeight_, limits.maxHeight_);
    }
}

/**
 * @tc.name: TestAdjustLimitsByAspectRatio2
 * @tc.desc: Verify AdjustLimitsByAspectRatio behavior in different scenarios
 * @tc.type: FUNC
 */
HWTEST_F(SessionUtilsTest, TestAdjustLimitsByAspectRatio2, TestSize.Level1)
{
    // Case 5: With decoration (expand + trim check)
    WindowLimits limits(500, 500, 100, 100, FLT_MAX, 0.0f);
    WindowDecoration decor {10, 20, 10, 20};
    auto adjusted = AdjustLimitsByAspectRatio(limits, decor, 1.0f);
    EXPECT_EQ(adjusted.minWidth_, 100);
    EXPECT_EQ(adjusted.minHeight_, 120);
    EXPECT_EQ(adjusted.maxWidth_, 480);
    EXPECT_EQ(adjusted.maxHeight_, 500);
}

/**
 * @tc.name: TestAdjustRectByAspectRatio
 * @tc.desc: Verify AdjustRectByAspectRatio behavior in different scenarios
 * @tc.type: FUNC
 */
HWTEST_F(SessionUtilsTest, TestAdjustRectByAspectRatio, TestSize.Level1)
{
    WSRect rect {0, 0, 200, 100};
    WindowDecoration noDecor {0, 0, 0, 0};
    WindowLimits limits(400, 400, 50, 50, FLT_MAX, 0.0f);

    // Case 1: aspectRatio == 0 (NearZero)
    {
        auto adjusted = AdjustRectByAspectRatio(rect, limits, noDecor, 0.0f, 5);
        EXPECT_EQ(adjusted, rect);
    }

    // Case 2: Trim -> invalid (min > max)
    {
        WindowLimits smallLimits(50, 50, 100, 100, FLT_MAX, 0.0f); // invalid after trim
        WindowDecoration decor {20, 20, 0, 0};
        auto adjusted = AdjustRectByAspectRatio(rect, smallLimits, decor, 2.0f, 5);
        EXPECT_EQ(adjusted, rect);
    }

    // Case 3: minW > maxW (aspect ratio conflicts with limits)
    {
        WindowLimits conflictLimits(100, 100, 90, 90, FLT_MAX, 0.0f);
        auto adjusted = AdjustRectByAspectRatio(rect, conflictLimits, noDecor, 10.0f, 5);
        EXPECT_EQ(adjusted, rect);
    }

    // Case 4: Normal adjust, but difference <= tolerancePx
    {
        WSRect rectNear {0, 0, 200, 100};
        WindowLimits limitsOk(400, 400, 50, 50, FLT_MAX, 0.0f);
        auto adjusted = AdjustRectByAspectRatio(rectNear, limitsOk, noDecor, 2.0f, 50);
        EXPECT_EQ(adjusted, rectNear);
    }

    // Case 5: Normal adjust, difference > tolerancePx
    {
        WSRect rectFar {0, 0, 300, 100};
        WindowLimits limitsOk(400, 400, 50, 50, FLT_MAX, 0.0f);
        auto adjusted = AdjustRectByAspectRatio(rectFar, limitsOk, noDecor, 2.0f, 1);
        EXPECT_EQ(adjusted.width_, 300);
        EXPECT_EQ(adjusted.height_, 150);
    }
}

/**
 * @tc.name: ShiftDecimalDigit
 * @tc.desc: Verify ShiftDecimalDigit
 * @tc.type: FUNC
 */
HWTEST_F(SessionUtilsTest, ShiftDecimalDigit, TestSize.Level1)
{
    EXPECT_EQ(ShiftDecimalDigit(4, 2), 400);
}
} // namespace SessionUtils
} // namespace Rosen
} // namespace OHOS
