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
#include "dm_common.h"

using namespace testing::ext;

namespace OHOS::Rosen {
/**
 * @tc.name: MarshallingUnmarshalling_DefaultValues
 * @tc.desc: Marshalling and unmarshalling test with default values
 * @tc.type: FUNC
 */
HWTEST(PointTest, MarshallingUnmarshalling_DefaultValues, TestSize.Level1)
{
    Point point1;
    Parcel parcel;
    bool ret = point1.Marshalling(parcel);
    ASSERT_TRUE(ret);

    sptr<Point> point2 = Point::Unmarshalling(parcel);
    ASSERT_NE(point2, nullptr);
    EXPECT_EQ(point2->posX_, 0);
    EXPECT_EQ(point2->posY_, 0);
}

/**
 * @tc.name: MarshallingUnmarshalling_CustomValues
 * @tc.desc: Marshalling and unmarshalling test with custom values
 * @tc.type: FUNC
 */
HWTEST(PointTest, MarshallingUnmarshalling_CustomValues, TestSize.Level1)
{
    Point point1(100, 200);
    Parcel parcel;
    bool ret = point1.Marshalling(parcel);
    ASSERT_TRUE(ret);

    sptr<Point> point2 = Point::Unmarshalling(parcel);
    ASSERT_NE(point2, nullptr);
    EXPECT_EQ(point2->posX_, 100);
    EXPECT_EQ(point2->posY_, 200);
}
} // namespace OHOS::Rosen
