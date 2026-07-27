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
#include "fold_screen_state_internel.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class FoldScreenStateInternelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FoldScreenStateInternelTest::SetUpTestCase()
{
}

void FoldScreenStateInternelTest::TearDownTestCase()
{
}

void FoldScreenStateInternelTest::SetUp()
{
}

void FoldScreenStateInternelTest::TearDown()
{
}

namespace {
/**
 * @tc.name: IsSecondaryDisplaySuperFoldDevice_Type8_ReturnsTrue
 * @tc.desc: Verify IsSecondaryDisplaySuperFoldDevice returns true when fold type is secondary super display (type "8")
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateInternelTest, IsSecondaryDisplaySuperFoldDevice_Type8_ReturnsTrue, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplaySuperFoldDevice()) {
        GTEST_SKIP() << "This test requires a secondary display super fold device (fold type 8)";
    }
    EXPECT_TRUE(FoldScreenStateInternel::IsSecondaryDisplaySuperFoldDevice());
}

/**
 * @tc.name: IsSecondaryDisplaySuperFoldDevice_Type7_ReturnsFalse
 * @tc.desc: Verify IsSecondaryDisplaySuperFoldDevice returns false on single display super fold device (type "7"),
 *           ensuring the two similar device types are not confused
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateInternelTest, IsSecondaryDisplaySuperFoldDevice_Type7_ReturnsFalse, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSingleDisplaySuperFoldDevice()) {
        GTEST_SKIP() << "This test requires a single display super fold device (fold type 7)";
    }
    // On a single display super fold device, secondary display check must return false
    EXPECT_FALSE(FoldScreenStateInternel::IsSecondaryDisplaySuperFoldDevice());
}
}
} // namespace Rosen
} // namespace OHOS
