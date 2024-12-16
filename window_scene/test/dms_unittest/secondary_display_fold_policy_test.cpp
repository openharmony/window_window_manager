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

#include "screen_session_manager/include/fold_screen_controller/secondary_display_fold_policy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_US = 100000;
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
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    DisplayModeChangeReason reason = DisplayModeChangeReason::DEFAULT;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);

    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    policy.ChangeScreenDisplayMode(displayMode, reason);
    EXPECT_FALSE(policy.onBootAnimation_);

    displayMode = FoldDisplayMode::MAIN;
    policy.ChangeScreenDisplayMode(displayMode, reason);
    EXPECT_FALSE(policy.onBootAnimation_);

    displayMode = FoldDisplayMode::FULL;
    policy.ChangeScreenDisplayMode(displayMode, reason);
    EXPECT_FALSE(policy.onBootAnimation_);

    displayMode = FoldDisplayMode::GLOBAL_FULL;
    policy.ChangeScreenDisplayMode(displayMode, reason);
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: SendSensorResult
 * @tc.desc: test function : SendSensorResult
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, SendSensorResult, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);

    FoldStatus foldStatus = FoldStatus::UNKNOWN;
    policy.SendSensorResult(foldStatus);
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: SetOnBootAnimation
 * @tc.desc: test function : SetOnBootAnimation
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, SetOnBootAnimation, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);

    policy.SetOnBootAnimation(false);
    EXPECT_FALSE(policy.onBootAnimation_);

    policy.SetOnBootAnimation(true);
    EXPECT_TRUE(policy.onBootAnimation_);
}

/**
 * @tc.name: RecoverWhenBootAnimationExit
 * @tc.desc: test function : RecoverWhenBootAnimationExit
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, RecoverWhenBootAnimationExit, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);

    policy.currentDisplayMode_ = FoldDisplayMode::UNKNOWN;
    policy.RecoverWhenBootAnimationExit();
    EXPECT_FALSE(policy.onBootAnimation_);

    policy.currentDisplayMode_ = FoldDisplayMode::GLOBAL_FULL;
    policy.RecoverWhenBootAnimationExit();
    EXPECT_FALSE(policy.onBootAnimation_);

    policy.currentDisplayMode_ = FoldDisplayMode::FULL;
    policy.RecoverWhenBootAnimationExit();
    EXPECT_FALSE(policy.onBootAnimation_);

    policy.currentDisplayMode_ = FoldDisplayMode::MAIN;
    policy.RecoverWhenBootAnimationExit();
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: GetModeMatchStatus
 * @tc.desc: test function : GetModeMatchStatus
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, GetModeMatchStatus, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;

    policy.currentFoldStatus_ = FoldStatus::EXPAND;
    displayMode = policy.GetModeMatchStatus();
    EXPECT_EQ(FoldDisplayMode::FULL, displayMode);

    policy.currentFoldStatus_ = FoldStatus::FOLDED;
    displayMode = policy.GetModeMatchStatus();
    EXPECT_EQ(FoldDisplayMode::MAIN, displayMode);

    policy.currentFoldStatus_ = FoldStatus::HALF_FOLD;
    displayMode = policy.GetModeMatchStatus();
    EXPECT_EQ(FoldDisplayMode::FULL, displayMode);


    policy.currentFoldStatus_ = FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_EXPAND;
    displayMode = policy.GetModeMatchStatus();
    EXPECT_EQ(FoldDisplayMode::GLOBAL_FULL, displayMode);

    policy.currentFoldStatus_ = FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_HALF_FOLDED;
    displayMode = policy.GetModeMatchStatus();
    EXPECT_EQ(FoldDisplayMode::GLOBAL_FULL, displayMode);

    policy.currentFoldStatus_ = FoldStatus::FOLD_STATE_FOLDED_WITH_SECOND_EXPAND;
    displayMode = policy.GetModeMatchStatus();
    EXPECT_EQ(FoldDisplayMode::MAIN, displayMode);

    policy.currentFoldStatus_ = FoldStatus::FOLD_STATE_FOLDED_WITH_SECOND_HALF_FOLDED;
    displayMode = policy.GetModeMatchStatus();
    EXPECT_EQ(FoldDisplayMode::MAIN, displayMode);

    policy.currentFoldStatus_ = FoldStatus::FOLD_STATE_HALF_FOLDED_WITH_SECOND_EXPAND;
    displayMode = policy.GetModeMatchStatus();
    EXPECT_EQ(FoldDisplayMode::GLOBAL_FULL, displayMode);

    policy.currentFoldStatus_ = FoldStatus::FOLD_STATE_HALF_FOLDED_WITH_SECOND_HALF_FOLDED;
    displayMode = policy.GetModeMatchStatus();
    EXPECT_EQ(FoldDisplayMode::GLOBAL_FULL, displayMode);
}

/**
 * @tc.name: ChangeSuperScreenDisplayMode
 * @tc.desc: test function : ChangeSuperScreenDisplayMode
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, ChangeSuperScreenDisplayMode, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    sptr<ScreenSession> screenSession = new ScreenSession;
    FoldDisplayMode displayMode = policy.currentDisplayMode_;

    policy.onBootAnimation_ = true;
    policy.ChangeSuperScreenDisplayMode(screenSession, displayMode);
    EXPECT_TRUE(policy.onBootAnimation_);

    policy.ChangeSuperScreenDisplayMode(screenSession, displayMode);
    EXPECT_TRUE(policy.onBootAnimation_);
}

/**
 * @tc.name: SendPropertyChangeResult
 * @tc.desc: test function : SendPropertyChangeResult
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, SendPropertyChangeResult, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    sptr<ScreenSession> screenSession = new ScreenSession;
    ScreenId screenId = 0;
    FoldDisplayMode displayMode = policy.currentDisplayMode_;
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::UNDEFINED;

    policy.SendPropertyChangeResult(screenSession, screenId, reason, displayMode);
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: SetStatusFullActiveRectAndTpFeature
 * @tc.desc: test function : SetStatusFullActiveRectAndTpFeature
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, SetStatusFullActiveRectAndTpFeature, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    ScreenProperty property = policy.screenProperty_;

    policy.SetStatusFullActiveRectAndTpFeature(property);
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: SetStatusMainActiveRectAndTpFeature
 * @tc.desc: test function : SetStatusMainActiveRectAndTpFeature
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, SetStatusMainActiveRectAndTpFeature, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    ScreenProperty property = policy.screenProperty_;

    policy.SetStatusMainActiveRectAndTpFeature(property);
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: SetStatusGlobalFullActiveRectAndTpFeature
 * @tc.desc: test function : SetStatusGlobalFullActiveRectAndTpFeature
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, SetStatusGlobalFullActiveRectAndTpFeature, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    ScreenProperty property = policy.screenProperty_;

    policy.SetStatusGlobalFullActiveRectAndTpFeature(property);
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: ReportFoldDisplayModeChange
 * @tc.desc: test function : ReportFoldDisplayModeChange
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, ReportFoldDisplayModeChange, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);

    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    policy.ReportFoldDisplayModeChange(displayMode);
    EXPECT_EQ(FoldDisplayMode::UNKNOWN, displayMode);

    displayMode = FoldDisplayMode::FULL;
    policy.ReportFoldDisplayModeChange(displayMode);
    EXPECT_NE(FoldDisplayMode::UNKNOWN, displayMode);
}

/**
 * @tc.name: GetCurrentFoldCreaseRegion
 * @tc.desc: test function : GetCurrentFoldCreaseRegion
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, GetCurrentFoldCreaseRegion, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    
    sptr<FoldCreaseRegion> foldCreaseRegion;
    foldCreaseRegion = policy.GetCurrentFoldCreaseRegion();
    EXPECT_EQ(policy.currentFoldCreaseRegion_, foldCreaseRegion);
}

/**
 * @tc.name: InitScreenParams
 * @tc.desc: test function : InitScreenParams
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplayFoldPolicyTest, InitScreenParams, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SecondaryDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    
    policy.InitScreenParams();
    EXPECT_FALSE(policy.onBootAnimation_);
}
}
} // namespace Rosen
} // namespace OHOS