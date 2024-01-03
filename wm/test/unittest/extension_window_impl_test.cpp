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

#include "extension_window_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ExtensionWindowImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ExtensionWindowImplTest::SetUpTestCase()
{
}

void ExtensionWindowImplTest::TearDownTestCase()
{
}

void ExtensionWindowImplTest::SetUp()
{
}

void ExtensionWindowImplTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetAvoidAreaByType
 * @tc.desc: GetAvoidAreaByType Test
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionWindowImplTest, GetAvoidAreaByType, Function | SmallTest | Level2)
{
    sptr<Window> window = new Window();
    ExtensionWindowImpl extensionWindowImpl(window);
    AvoidAreaType type = AvoidAreaType::TYPE_SYSTEM;
    AvoidArea avoidArea;
    ASSERT_EQ(WMError::WM_OK, extensionWindowImpl.GetAvoidAreaByType(type, avoidArea));
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}
}
} // namespace Rosen
} // namespace OHOS