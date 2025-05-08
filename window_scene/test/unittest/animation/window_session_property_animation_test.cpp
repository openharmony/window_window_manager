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

class WindowSessionPropertyAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void WindowSessionPropertyAnimationTest::SetUpTestCase() {}

void WindowSessionPropertyAnimationTest::TearDownTestCase() {}

namespace {
/**
 * @tc.name: SetWindowCornerRadius
 * @tc.desc: SetWindowCornerRadius
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyAnimationTest, SetWindowCornerRadius, TestSize.Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    float cornerRadius = 1.0f;
    property->SetWindowCornerRadius(cornerRadius);
    ASSERT_EQ(cornerRadius, property->GetWindowCornerRadius());
}
} // namespace
} // namespace Rosen
} // namespace OHOS