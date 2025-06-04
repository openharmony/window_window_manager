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

#include "window_coordinate_helper.h"

#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowCoordinateHelperTest : public Test {
public:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: TestComputeScaledRect
 * @tc.desc: Test scaling with pivot (centered) and uniform scale
 * @tc.type: FUNC
 */
HWTEST_F(WindowCoordinateHelperTest, TestComputeScaledRect, TestSize.Level1)
{
    Rect rect { 100, 100, 100, 100 };
    // center point scaling
    Transform transform;
    transform.pivotX_ = 0.5f;
    transform.pivotY_ = 0.5f;
    transform.scaleX_ = 0.5f;
    transform.scaleY_ = 0.5f;

    Rect expected { 125, 125, 50, 50 };
    Rect result = WindowCoordinateHelper::ComputeScaledRect(rect, transform);
    ASSERT_EQ(result, expected);
}

/**
 * @tc.name: TestLocalToExternal
 * @tc.desc: Convert local to external position
 * @tc.type: FUNC
 */
HWTEST_F(WindowCoordinateHelperTest, TestLocalToExternal, TestSize.Level1)
{
    Rect window { 50, 100, 300, 200 };
    Position local { 10, 20 };

    Position result = WindowCoordinateHelper::LocalToExternal(window, local);
    ASSERT_EQ(result.x, 60);
    ASSERT_EQ(result.y, 120);
}

/**
 * @tc.name: TestLocalToExternalWithTransform
 * @tc.desc: Convert local to external position with transform
 * @tc.type: FUNC
 */
HWTEST_F(WindowCoordinateHelperTest, TestLocalToExternalWithTransform, TestSize.Level1)
{
    Position local { 10, 20 };
    Rect window { 100, 100, 100, 100 };
    // center point scaling
    Transform transform;
    transform.pivotX_ = 0.5f;
    transform.pivotY_ = 0.5f;
    transform.scaleX_ = 0.5f;
    transform.scaleY_ = 0.5f;


    Position result = WindowCoordinateHelper::LocalToExternal(window, transform, local);
    // scaled rect = (125, 125, 50, 50)
    // scaled position = (5, 10)
    ASSERT_EQ(result.x, 130);
    ASSERT_EQ(result.y, 135);
}

/**
 * @tc.name: TestExternalToLocal
 * @tc.desc: Convert external to local position
 * @tc.type: FUNC
 */
HWTEST_F(WindowCoordinateHelperTest, TestExternalToLocal, TestSize.Level1)
{
    Rect window { 100, 200, 300, 200 };
    Position external { 130, 250 };

    Position result = WindowCoordinateHelper::ExternalToLocal(window, external);
    ASSERT_EQ(result.x, 30);
    ASSERT_EQ(result.y, 50);
}

/**
 * @tc.name: TestExternalToLocalWithTransform
 * @tc.desc: Convert external to local position with transform
 * @tc.type: FUNC
 */
HWTEST_F(WindowCoordinateHelperTest, TestExternalToLocalWithTransform, TestSize.Level1)
{
    Position external { 130, 135 };
    Rect window { 100, 100, 100, 100 };
    // center point scaling
    Transform transform;
    transform.pivotX_ = 0.5f;
    transform.pivotY_ = 0.5f;
    transform.scaleX_ = 0.5f;
    transform.scaleY_ = 0.5f;

    Position result = WindowCoordinateHelper::ExternalToLocal(window, transform, external);
    // scaled rect = (125, 125, 50, 50)
    ASSERT_EQ(result.x, 10);
    ASSERT_EQ(result.y, 20);
}

/**
 * @tc.name: TestExternalToLocalWithZeroScale
 * @tc.desc: Convert position when transform scale is zero (should fallback to 0,0)
 * @tc.type: FUNC
 */
HWTEST_F(WindowCoordinateHelperTest, TestExternalToLocalWithZeroScale, TestSize.Level1)
{
    Position external { 130, 135 };
    Rect window { 100, 100, 100, 100 };
    Transform transform;
    transform.pivotX_ = 0.5f;
    transform.pivotY_ = 0.5f;
    transform.scaleX_ = 0.0f;
    transform.scaleY_ = 0.0f;

    Position result = WindowCoordinateHelper::ExternalToLocal(window, transform, external);
    ASSERT_EQ(result.x, 0);
    ASSERT_EQ(result.y, 0);
}
} // namespace Rosen
} // namespace OHOS