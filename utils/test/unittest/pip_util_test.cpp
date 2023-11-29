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
#include "pip_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class PiPUtilTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PiPUtilTest::SetUpTestCase()
{
}

void PiPUtilTest::TearDownTestCase()
{
}

void PiPUtilTest::SetUp()
{
}

void PiPUtilTest::TearDown()
{
}

namespace {
/**
 * @tc.name: UpdateRectPivot
 * @tc.desc: UpdateRectPivot test
 * @tc.type: FUNC
 */
HWTEST_F(PiPUtilTest, UpdateRectPivot, Function | SmallTest | Level2)
{
    auto pivot1 = PiPScalePivot::UNDEFINED;
    PiPUtil::UpdateRectPivot(10, 20, 100, pivot1);
    ASSERT_EQ(PiPScalePivot::START, pivot1);

    auto pivot2 = PiPScalePivot::UNDEFINED;
    PiPUtil::UpdateRectPivot(40, 20, 100, pivot2);
    ASSERT_EQ(PiPScalePivot::MIDDLE, pivot2);

    auto pivot3 = PiPScalePivot::UNDEFINED;
    PiPUtil::UpdateRectPivot(70, 20, 100, pivot3);
    ASSERT_EQ(PiPScalePivot::END, pivot3);
}

/**
 * @tc.name: GetRectByPivot
 * @tc.desc: GetRectByPivot test
 * @tc.type: FUNC
 */
HWTEST_F(PiPUtilTest, GetRectByPivot, Function | SmallTest | Level2)
{
    auto pivot1 = PiPScalePivot::UNDEFINED;
    int32_t start1 = 100;
    PiPUtil::GetRectByPivot(start1, 100, 200, 300, pivot1);
    ASSERT_EQ(100, start1);

    auto pivot2 = PiPScalePivot::UNDEFINED;
    int32_t start2 = 100;
    PiPUtil::GetRectByPivot(start2, 100, 200, 300, pivot2);
    ASSERT_EQ(100, start2);

    auto pivot3 = PiPScalePivot::UNDEFINED;
    int32_t start3 = 100;
    PiPUtil::GetRectByPivot(start3, 100, 200, 300, pivot3);
    ASSERT_EQ(100, start3);
}

/**
 * @tc.name: GetRectByScale
 * @tc.desc: GetRectByScale test
 * @tc.type: FUNC
 */
HWTEST_F(PiPUtilTest, GetRectByScale, Function | SmallTest | Level2)
{
    Rect rect1 = {0, 0, 100, 100};
    auto level1 = PiPScaleLevel::PIP_SCALE_LEVEL_SMALLEST;
    PiPUtil::GetRectByScale(200, 200, level1, rect1);
    ASSERT_EQ(60, rect1.width_);
    ASSERT_EQ(60, rect1.height_);

    Rect rect2 = {0, 0, 100, 100};
    auto level2 = PiPScaleLevel::PIP_SCALE_LEVEL_SMALLEST;
    PiPUtil::GetRectByScale(200, 200, level2, rect2);
    ASSERT_EQ(60, rect2.width_);
    ASSERT_EQ(60, rect2.height_);
}

/**
 * @tc.name: GetValidRect
 * @tc.desc: GetValidRect test
 * @tc.type: FUNC
 */
HWTEST_F(PiPUtilTest, GetValidRect, Function | SmallTest | Level2)
{
    Rect rect1 = {0, PiPUtil::SAFE_PADDING_VERTICAL_TOP, 100, 100};
    ASSERT_EQ(true, PiPUtil::GetValidRect(1000, 1000, rect1));
    ASSERT_EQ(PiPUtil::SAFE_PADDING_HORIZONTAL_VP, rect1.posX_);
    ASSERT_EQ(PiPUtil::SAFE_PADDING_VERTICAL_TOP, rect1.posY_);

    Rect rect2 = {1000, PiPUtil::SAFE_PADDING_VERTICAL_TOP, 100, 100};
    ASSERT_EQ(true, PiPUtil::GetValidRect(1000, 1000, rect2));
    ASSERT_EQ(1000 - rect2.width_ - PiPUtil::SAFE_PADDING_HORIZONTAL_VP, rect2.posX_);
    ASSERT_EQ(PiPUtil::SAFE_PADDING_VERTICAL_TOP, rect2.posY_);
    
    Rect rect3 = {PiPUtil::SAFE_PADDING_HORIZONTAL_VP, 0, 100, 100};
    ASSERT_EQ(true, PiPUtil::GetValidRect(1000, 1000, rect3));
    ASSERT_EQ(PiPUtil::SAFE_PADDING_HORIZONTAL_VP, rect3.posX_);
    ASSERT_EQ(PiPUtil::SAFE_PADDING_VERTICAL_TOP, rect3.posY_);

    Rect rect4 = {PiPUtil::SAFE_PADDING_HORIZONTAL_VP, 1000, 100, 100};
    ASSERT_EQ(true, PiPUtil::GetValidRect(1000, 1000, rect4));
    ASSERT_EQ(PiPUtil::SAFE_PADDING_HORIZONTAL_VP, rect4.posX_);
    ASSERT_EQ(1000 - 100 - PiPUtil::SAFE_PADDING_VERTICAL_BOTTOM, rect4.posY_);

    Rect rect5 = {PiPUtil::SAFE_PADDING_HORIZONTAL_VP, PiPUtil::SAFE_PADDING_VERTICAL_TOP, 100, 100};
    ASSERT_EQ(false, PiPUtil::GetValidRect(1000, 1000, rect5));
    ASSERT_EQ(PiPUtil::SAFE_PADDING_HORIZONTAL_VP, rect5.posX_);
    ASSERT_EQ(PiPUtil::SAFE_PADDING_VERTICAL_TOP, rect5.posY_);
}
}
} // namespace Rosen
} // namespace OHOS