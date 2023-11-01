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
#include "screen_property.h"
#include <gtest/gtest.h>

// using namespace FRAME_TRACE;
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class ScreenPropertyTest : public testing::Test {
  public:
    ScreenPropertyTest() {}
    ~ScreenPropertyTest() {}
};

namespace {
/**
 * @tc.name: SetScreenRotation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetScreenRotation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetScreenRotation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    int64_t ret = 0;
    Rotation rotation = Rotation::ROTATION_0;
    property->SetScreenRotation(rotation);

    rotation = Rotation::ROTATION_90;
    property->SetScreenRotation(rotation);

    rotation = Rotation::ROTATION_180;
    property->SetScreenRotation(rotation);

    rotation = Rotation::ROTATION_270;
    property->SetScreenRotation(rotation);
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetScreenRotation end";
}

/**
 * @tc.name: UpdateVirtualPixelRatio
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, UpdateVirtualPixelRatio, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: UpdateVirtualPixelRatio start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    int64_t ret = 0;
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;

    property->UpdateVirtualPixelRatio(bounds);

    bounds.rect_.height_ = 1111;
    property->UpdateVirtualPixelRatio(bounds);

    bounds.rect_.width_ = 1111;
    bounds.rect_.height_ = 2772;
    property->UpdateVirtualPixelRatio(bounds);

    bounds.rect_.width_ = 1111;
    bounds.rect_.height_ = 1111;
    property->UpdateVirtualPixelRatio(bounds);
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenPropertyTest: UpdateVirtualPixelRatio end";
}

/**
 * @tc.name: SetBounds
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetBounds, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetBounds start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    int64_t ret = 0;
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;

    uint32_t phyWidth = UINT32_MAX;
    property->SetPhyWidth(phyWidth);
    uint32_t phyHeigth = UINT32_MAX;
    property->SetPhyHeight(phyHeigth);
    property->SetBounds(bounds);

    bounds.rect_.width_ = 2772;
    bounds.rect_.height_ = 1344;

    uint32_t phyWidth1 = 2772;
    property->SetPhyWidth(phyWidth1);
    uint32_t phyHeigth1 = 1344;
    property->SetPhyHeight(phyHeigth1);
    property->SetBounds(bounds);
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetBounds end";
}

/**
 * @tc.name: CalculateXYDpi
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, CalculateXYDpi, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: CalculateXYDpi start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    uint32_t phyWidth = 0;
    uint32_t phyHeight = 0;
    int ret = 0;
    property->CalculateXYDpi(phyWidth, phyHeight);
    phyWidth = 1;
    phyHeight = 1;
    property->CalculateXYDpi(phyWidth, phyHeight);
    ASSERT_EQ(ret, 0);
    GTEST_LOG_(INFO) << "ScreenPropertyTest: CalculateXYDpi end";
}

/**
 * @tc.name: SetOffsetX
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetOffsetX, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOffsetX start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    int32_t offsetX = 0;
    property->SetOffsetX(offsetX);
    int32_t ret = property->GetOffsetX();
    ASSERT_EQ(ret, offsetX);
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOffsetX end";
}

/**
 * @tc.name: SetOffsetY
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetOffsetY, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOffsetY start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    int32_t offsetY = 0;
    property->SetOffsetY(offsetY);
    int32_t ret = property->GetOffsetY();
    ASSERT_EQ(ret, offsetY);
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOffsetY end";
}

/**
 * @tc.name: SetOffset
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetOffset, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOffset start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    property->SetOffset(offsetX, offsetY);
    int32_t ret_x = property->GetOffsetX();
    int32_t ret_y = property->GetOffsetY();
    ASSERT_EQ(ret_x, offsetX);
    ASSERT_EQ(ret_y, offsetY);
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetOffset end";
}

/**
 * @tc.name: SetScreenRequestedOrientation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenPropertyTest, SetScreenRequestedOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetScreenRequestedOrientation start";
    ScreenProperty* property = new(std::nothrow) ScreenProperty();
    Orientation orientation = Orientation::UNSPECIFIED;
    property->SetScreenRequestedOrientation(orientation);
    Orientation ret = property->GetScreenRequestedOrientation();
    ASSERT_EQ(ret, orientation);
    GTEST_LOG_(INFO) << "ScreenPropertyTest: SetScreenRequestedOrientation end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS
