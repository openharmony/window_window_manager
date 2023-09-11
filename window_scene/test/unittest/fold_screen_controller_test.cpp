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

#include "session_manager/include/fold_screen_controller/fold_screen_controller.h"
#include "session_manager/include/screen_session_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class FoldScreenControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static ScreenSessionManager& ssm_;
};

ScreenSessionManager& FoldScreenControllerTest::ssm_ = ScreenSessionManager::GetInstance();

void FoldScreenControllerTest::SetUpTestCase()
{
}

void FoldScreenControllerTest::TearDownTestCase()
{
}

void FoldScreenControllerTest::SetUp()
{
}

void FoldScreenControllerTest::TearDown()
{
}

namespace {

    /**
     * @tc.name: SetFoldDisplayMode
     * @tc.desc: FoldScreenController set display mode
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, SetDisplayMode, Function | SmallTest | Level3)
    {
        ssm_.SetFoldDisplayMode(FoldDisplayMode::MAIN);
        ASSERT_EQ(FoldDisplayMode::UNKNOWN, ssm_.GetFoldDisplayMode());

        ssm_.SetFoldDisplayMode(FoldDisplayMode::FULL);
        ASSERT_EQ(FoldDisplayMode::UNKNOWN, ssm_.GetFoldDisplayMode());
    }

    /**
     * @tc.name: GetFoldDisplayMode
     * @tc.desc: GetFoldDisplayMode get display mode
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, GetFoldDisplayMode, Function | SmallTest | Level3)
    {
        ssm_.SetFoldDisplayMode(FoldDisplayMode::MAIN);
        ASSERT_EQ(FoldDisplayMode::UNKNOWN, ssm_.GetFoldDisplayMode());

        ssm_.SetFoldDisplayMode(FoldDisplayMode::FULL);
        ASSERT_EQ(FoldDisplayMode::UNKNOWN, ssm_.GetFoldDisplayMode());
    }

    /**
     * @tc.name: IsFoldable
     * @tc.desc: FoldScreenController whether is foldable device or not
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, IsFoldable, Function | SmallTest | Level3)
    {
        ASSERT_EQ(true, ssm_.IsFoldable());
    }

    /**
     * @tc.name: GetFoldStatus
     * @tc.desc: FoldScreenController get device fold status
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, GetFoldStatus, Function | SmallTest | Level3)
    {
        ssm_.SetFoldDisplayMode(FoldDisplayMode::MAIN);
        ASSERT_EQ(FoldStatus::UNKNOWN, ssm_.GetFoldStatus());

        ssm_.SetFoldDisplayMode(FoldDisplayMode::FULL);
        ASSERT_EQ(FoldStatus::UNKNOWN, ssm_.GetFoldStatus());
    }

    /**
     * @tc.name: GetCurrentFoldCreaseRegion
     * @tc.desc: FoldScreenController get crease region
     * @tc.type: FUNC
     */
    HWTEST_F(FoldScreenControllerTest, GetCurrentFoldCreaseRegion, Function | SmallTest | Level3)
    {
        ASSERT_EQ(nullptr, ssm_.GetCurrentFoldCreaseRegion());
    }
}
} // namespace Rosen
} // namespace OHOS

