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
#include "dm_virtual_screen_option.h"

using namespace testing::ext;

namespace OHOS::Rosen {
/**
 * @tc.name: MarshallingUnmarshalling_DefaultValues
 * @tc.desc: Marshalling and unmarshalling test with default values
 * @tc.type: FUNC
 */
HWTEST(DmVirtualScreenOptionTest, MarshallingUnmarshalling_DefaultValues, TestSize.Level1)
{
    DmVirtualScreenOption dmVirtualScreenOption;
    Parcel parcel;
    bool ret = dmVirtualScreenOption.Marshalling(parcel);
    ASSERT_TRUE(ret);

    sptr<DmVirtualScreenOption> option = DmVirtualScreenOption::Unmarshalling(parcel);
    ASSERT_NE(option, nullptr);
    EXPECT_EQ(option->GetOption().name_, "");
    EXPECT_EQ(option->GetOption().width_, 0);
    EXPECT_EQ(option->GetOption().height_, 0);
}

/**
 * @tc.name: MarshallingUnmarshalling_CustomValues
 * @tc.desc: Marshalling and unmarshalling test with custom values
 * @tc.type: FUNC
 */
HWTEST(DmVirtualScreenOptionTest, MarshallingUnmarshalling_CustomValues, TestSize.Level1)
{
    VirtualScreenOption virtualOption{};
    virtualOption.name_ = "testVirtualOption";
    virtualOption.width_ = 100;
    virtualOption.height_ = 200;
    DmVirtualScreenOption dmVirtualScreenOption(virtualOption);

    Parcel parcel;
    bool ret = dmVirtualScreenOption.Marshalling(parcel);
    ASSERT_TRUE(ret);

    sptr<DmVirtualScreenOption> option = DmVirtualScreenOption::Unmarshalling(parcel);
    ASSERT_NE(option, nullptr);
    EXPECT_EQ(option->GetOption().name_, virtualOption.name_);
    EXPECT_EQ(option->GetOption().width_, virtualOption.width_);
    EXPECT_EQ(option->GetOption().height_, virtualOption.height_);
}
} // namespace OHOS::Rosen
