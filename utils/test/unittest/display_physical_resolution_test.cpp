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
HWTEST(DisplayPhysicalResolutionTest, MarshallingUnmarshalling_DefaultValues, TestSize.Level1)
{
    DisplayPhysicalResolution displayPhysicalResolution1;
    Parcel parcel;
    bool ret = displayPhysicalResolution1.Marshalling(parcel);
    ASSERT_TRUE(ret);

    sptr<DisplayPhysicalResolution> displayPhysicalResolution2 = DisplayPhysicalResolution::Unmarshalling(parcel);
    ASSERT_NE(displayPhysicalResolution2, nullptr);
    EXPECT_EQ(displayPhysicalResolution2->foldDisplayMode_, FoldDisplayMode::UNKNOWN);
    EXPECT_EQ(displayPhysicalResolution2->physicalWidth_, 0);
    EXPECT_EQ(displayPhysicalResolution2->physicalHeight_, 0);
}

/**
 * @tc.name: MarshallingUnmarshalling_CustomValues
 * @tc.desc: Marshalling and unmarshalling test with custom values
 * @tc.type: FUNC
 */
HWTEST(DisplayPhysicalResolutionTest, MarshallingUnmarshalling_CustomValues, TestSize.Level1)
{
    DisplayPhysicalResolution dmVirtualScreenOption1(FoldDisplayMode::MAIN, 1920, 1080);

    Parcel parcel;
    bool ret = dmVirtualScreenOption1.Marshalling(parcel);
    ASSERT_TRUE(ret);

    sptr<DisplayPhysicalResolution> displayPhysicalResolution2 = DisplayPhysicalResolution::Unmarshalling(parcel);
    ASSERT_NE(displayPhysicalResolution2, nullptr);
    EXPECT_EQ(displayPhysicalResolution2->foldDisplayMode_, FoldDisplayMode::MAIN);
    EXPECT_EQ(displayPhysicalResolution2->physicalWidth_, 1920);
    EXPECT_EQ(displayPhysicalResolution2->physicalHeight_, 1080);
}
} // namespace OHOS::Rosen
