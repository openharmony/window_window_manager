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

#include <hisysevent.h>
#include "screen_session_manager/include/fold_screen_controller/secondary_display_fold_policy.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager.h"
#include "fold_screen_state_internel.h"

#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_US = 100000;
std::recursive_mutex g_displayInfoMutex;
std::shared_ptr<TaskScheduler> screenPowerTaskScheduler_ = std::make_shared<TaskScheduler>("test");
sptr<SecondaryDisplayFoldPolicy> g_policy =
    new SecondaryDisplayFoldPolicy(g_displayInfoMutex, screenPowerTaskScheduler_);
}

class SecondaryDisplayFoldPolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SecondaryDisplayFoldPolicyTest::SetUpTestCase()
{
}

void SecondaryDisplayFoldPolicyTest::TearDownTestCase()
{
}

void SecondaryDisplayFoldPolicyTest::SetUp()
{
}

void SecondaryDisplayFoldPolicyTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {
/**
 * @tc.name: ChangeScreenDisplayMode
 * @tc.desc: test function : ChangeScreenDisplayMode
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, ChangeScreenDisplayMode, Function | SmallTest | Level3)
{
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    g_policy->ChangeScreenDisplayMode(displayMode);
    EXPECT_FALSE(g_policy->currentDisplayMode_ == FoldDisplayMode::UNKNOWN);

    displayMode = FoldDisplayMode::FULL;
    g_policy->ChangeScreenDisplayMode(displayMode);
    EXPECT_FALSE(g_policy->currentDisplayMode_ == FoldDisplayMode::FULL);
}
}
} // namespace Rosen
} // namespace OHOS