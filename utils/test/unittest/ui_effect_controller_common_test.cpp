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

#include "ui_effect_controller_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class UIEffectControllerCommonTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void UIEffectControllerCommonTest::SetUpTestCase() {}

void UIEffectControllerCommonTest::TearDownTestCase() {}

void UIEffectControllerCommonTest::SetUp() {}

void UIEffectControllerCommonTest::TearDown() {}

void UIEffectControllerCommonTest::TearDownTestCase() {}
namespace {
/**
 * @tc.name: WriteAndReadParcelable
 * @tc.desc: WriteAndReadParcelable test
 * @tc.type: FUNC
 */
HWTEST_F(UIEffectControllerCommonTest, WriteAndReadParcelable, TestSize.Level1)
{
    UIEffectParams param;
    Parcel parcelWrite;
    EXPECT_EQ(param.Marshalling(parcel), false);
    param.backgroundFilter_ = sptr<Filter>::MakeSptr();
    EXPECT_EQ(param.Marshalling(parcel), false);
    param.effect_ = sptr<VisualEffect>::MakeSptr();
    EXPECT_EQ(param.Marshalling(parcel), true);
    EXPECT_EQ(UIEffectParams::Unmarshalling(parcelRead), nullptr);
    sptr<Filter> backgroundFilter = sptr<Filter>::MakeSptr();
    parcelRead.WriteStrongParcelable(backgroundFilter);
    EXPECT_EQ(UIEffectParams::Unmarshalling(parcelRead), nullptr);
    pstr<VisualEffect> effect = sptr<VisualEffect>::MakeSptr();
    parcelRead.WriteStrongParcelable(effect);
    EXPECT_NE(UIEffectParams::Unmarshalling(parcelRead), nullptr);
}

/**
 * @tc.name: WrapObjectDestruction
 * @tc.desc: WrapObjectDestruction test
 * @tc.type: FUNC
 */
HWTEST_F(UIEffectControllerCommonTest, WrapObjectDestruction, TestSize.Level1)
{
    int* data = new int(10);
    WrapObjectDestructor(nullptr, data, nullptr);
    EXPECT_EQ(data, nullptr);
    int* data2 = nullptr;
    WrapObjectDestructor(nullptr, data, nullptr);
    EXPECT_EQ(data, nullptr);
}
}
} // namespace OHOS::Rosen