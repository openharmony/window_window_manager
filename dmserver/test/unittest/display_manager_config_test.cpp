/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "display_manager_config.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayManagerConfigTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void DisplayManagerConfigTest::SetUpTestCase()
{
}

void DisplayManagerConfigTest::TearDownTestCase()
{
}

void DisplayManagerConfigTest::SetUp()
{
}

void DisplayManagerConfigTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Demo
 * @tc.desc: Demo
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerConfigTest, Demo, Function | SmallTest | Level1)
{
    ASSERT_EQ(true, true);
}
}
} // namespace Rosen
} // namespace OHOS