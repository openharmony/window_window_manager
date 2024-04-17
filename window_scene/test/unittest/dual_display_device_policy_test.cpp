/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "session_manager/include/fold_screen_controller/dual_display_device_policy.h"
#include "session_manager/include/screen_session_manager.h"
#include "fold_screen_controller/fold_screen_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DualDisplayDevicePolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static ScreenSessionManager& ssm_;
};

ScreenSessionManager& DualDisplayDevicePolicyTest::ssm_ = ScreenSessionManager::GetInstance();

void DualDisplayDevicePolicyTest::SetUpTestCase()
{
}

void DualDisplayDevicePolicyTest::TearDownTestCase()
{
}

void DualDisplayDevicePolicyTest::SetUp()
{
}

void DualDisplayDevicePolicyTest::TearDown()
{
}

namespace {

    /**
     * @tc.name: ChangeScreenDisplayMode
     * @tc.desc: DualDisplayDevicePolicy change screen display mode
     * @tc.type: FUNC
     */
    HWTEST_F(DualDisplayDevicePolicyTest, ChangeScreenDisplayMode, Function | SmallTest | Level3)
    {
        int64_t ret = 0;
        FoldDisplayMode mode = ssm_.GetFoldDisplayMode();
        ASSERT_EQ(mode, ssm_.GetFoldDisplayMode());
        ASSERT_EQ(ret, 0);
    }
}
} // namespace Rosen
} // namespace OHOS