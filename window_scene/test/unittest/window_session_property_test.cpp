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

#include <gtest/gtest.h>
#include "window_session_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class WindowSessionPropertyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
};

void WindowSessionPropertyTest::SetUpTestCase()
{
}

void WindowSessionPropertyTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: SetDragEnabled001
 * @tc.desc: SetDragEnabled and GetDragEnabled to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetDragEnabled001, Function | SmallTest | Level2)
{
    WindowSessionProperty* property = new WindowSessionProperty();
    ASSERT_EQ(property->GetDragEnabled(), true);
    property->SetDragEnabled(false);
    ASSERT_EQ(property->GetDragEnabled(), false);
}

/**
 * @tc.name: SetRaiseEnabled001
 * @tc.desc: SetRaiseEnabled and GetRaiseEnabled to check the value
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionPropertyTest, SetRaiseEnabled001, Function | SmallTest | Level2)
{
    WindowSessionProperty* property = new WindowSessionProperty();
    ASSERT_EQ(property->GetRaiseEnabled(), true);
    property->SetRaiseEnabled(false);
    ASSERT_EQ(property->GetRaiseEnabled(), false);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
