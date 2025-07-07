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
#include "window_session_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class WindowSessionPropertyRotationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void WindowSessionPropertyRotationTest::SetUpTestCase() {}

void WindowSessionPropertyRotationTest::TearDownTestCase() {}

namespace {
/**
 * @tc.name: SetRequestedOrientation
 * @tc.desc: SetRequestedOrientation test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyRotationTest, SetRequestedOrientation, TestSize.Level1)
{
    Orientation orientation = Orientation::REVERSE_HORIZONTAL;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    property->SetRequestedOrientation(orientation);
    Orientation ret = property->GetRequestedOrientation();
    ASSERT_EQ(ret, orientation);

    property->SetRequestedOrientation(Orientation::AUTO_ROTATION_UNSPECIFIED);
    Orientation ret1 = property->GetRequestedOrientation();
    ASSERT_EQ(ret1, Orientation::AUTO_ROTATION_UNSPECIFIED);

    property->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT);
    Orientation ret2 = property->GetRequestedOrientation();
    ASSERT_EQ(ret2, Orientation::USER_ROTATION_PORTRAIT);

    property->SetRequestedOrientation(Orientation::USER_ROTATION_LANDSCAPE);
    Orientation ret3 = property->GetRequestedOrientation();
    ASSERT_EQ(ret3, Orientation::USER_ROTATION_LANDSCAPE);

    property->SetRequestedOrientation(Orientation::USER_ROTATION_PORTRAIT_INVERTED);
    Orientation ret4 = property->GetRequestedOrientation();
    ASSERT_EQ(ret4, Orientation::USER_ROTATION_PORTRAIT_INVERTED);

    property->SetRequestedOrientation(Orientation::USER_ROTATION_LANDSCAPE_INVERTED);
    Orientation ret5 = property->GetRequestedOrientation();
    ASSERT_EQ(ret5, Orientation::USER_ROTATION_LANDSCAPE_INVERTED);

    property->SetRequestedOrientation(Orientation::FOLLOW_DESKTOP);
    Orientation ret6 = property->GetRequestedOrientation();
    ASSERT_EQ(ret6, Orientation::FOLLOW_DESKTOP);
}

/**
 * @tc.name: SetDefaultRequestedOrientation
 * @tc.desc: SetDefaultRequestedOrientation test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyRotationTest, SetDefaultRequestedOrientation, TestSize.Level1)
{
    Orientation orientation = Orientation::REVERSE_HORIZONTAL;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    property->SetDefaultRequestedOrientation(orientation);
    Orientation ret = property->GetDefaultRequestedOrientation();
    ASSERT_EQ(ret, orientation);
}
} // namespace
} // namespace Rosen
} // namespace OHOS