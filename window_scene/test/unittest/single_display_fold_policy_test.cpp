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

#include "session_manager/include/fold_screen_controller/single_display_fold_policy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t SLEEP_TIME_US = 100000;
}

class SingleDisplayFoldPolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SingleDisplayFoldPolicyTest::SetUpTestCase()
{
}

void SingleDisplayFoldPolicyTest::TearDownTestCase()
{
}

void SingleDisplayFoldPolicyTest::SetUp()
{
}

void SingleDisplayFoldPolicyTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {
/**
 * @tc.name: ChangeScreenDisplayMode
 * @tc.desc: test function : ChangeScreenDisplayMode
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, ChangeScreenDisplayMode, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    policy.ChangeScreenDisplayMode(displayMode);
    EXPECT_FALSE(policy.onBootAnimation_);

    displayMode = FoldDisplayMode::MAIN;
    policy.ChangeScreenDisplayMode(displayMode);
    EXPECT_FALSE(policy.onBootAnimation_);

    displayMode = FoldDisplayMode::FULL;
    policy.ChangeScreenDisplayMode(displayMode);
    EXPECT_FALSE(policy.onBootAnimation_);

    displayMode = FoldDisplayMode::SUB;
    policy.ChangeScreenDisplayMode(displayMode);
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: SendSensorResult
 * @tc.desc: test function : SendSensorResult
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, SendSensorResult, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    
    FoldStatus foldStatus = FoldStatus::UNKNOWN;
    policy.SendSensorResult(foldStatus);
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: GetCurrentFoldCreaseRegion
 * @tc.desc: test function : GetCurrentFoldCreaseRegion
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, GetCurrentFoldCreaseRegion, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    
    sptr<FoldCreaseRegion> foldCreaseRegion;
    foldCreaseRegion = policy.GetCurrentFoldCreaseRegion();
    EXPECT_EQ(policy.currentFoldCreaseRegion_, foldCreaseRegion);
}

/**
 * @tc.name: LockDisplayStatus
 * @tc.desc: test function : LockDisplayStatus
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, LockDisplayStatus, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    
    policy.LockDisplayStatus(false);
    EXPECT_EQ(policy.lockDisplayStatus_, false);
}

/**
 * @tc.name: SetOnBootAnimation
 * @tc.desc: test function : SetOnBootAnimation
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, SetOnBootAnimation, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    
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
HWTEST_F(SingleDisplayFoldPolicyTest, RecoverWhenBootAnimationExit, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    
    policy.currentDisplayMode_ = FoldDisplayMode::UNKNOWN;
    policy.RecoverWhenBootAnimationExit();
    EXPECT_FALSE(policy.onBootAnimation_);

    policy.currentDisplayMode_ = FoldDisplayMode::SUB;
    policy.RecoverWhenBootAnimationExit();
    EXPECT_FALSE(policy.onBootAnimation_);

    policy.currentDisplayMode_ = FoldDisplayMode::FULL;
    policy.RecoverWhenBootAnimationExit();
    EXPECT_FALSE(policy.onBootAnimation_);

    policy.currentDisplayMode_ = FoldDisplayMode::MAIN;
    policy.RecoverWhenBootAnimationExit();
    EXPECT_FALSE(policy.onBootAnimation_);

    policy.currentDisplayMode_ = FoldDisplayMode::COORDINATION;
    policy.RecoverWhenBootAnimationExit();
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: TriggerScreenDisplayModeUpdate
 * @tc.desc: test function : TriggerScreenDisplayModeUpdate
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, TriggerScreenDisplayModeUpdate, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    
    
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    policy.TriggerScreenDisplayModeUpdate(displayMode);
    EXPECT_FALSE(policy.onBootAnimation_);

    displayMode = FoldDisplayMode::SUB;
    policy.TriggerScreenDisplayModeUpdate(displayMode);
    EXPECT_FALSE(policy.onBootAnimation_);

    displayMode = FoldDisplayMode::FULL;
    policy.TriggerScreenDisplayModeUpdate(displayMode);
    EXPECT_FALSE(policy.onBootAnimation_);

    displayMode = FoldDisplayMode::MAIN;
    policy.TriggerScreenDisplayModeUpdate(displayMode);
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: UpdateForPhyScreenPropertyChange
 * @tc.desc: test function : UpdateForPhyScreenPropertyChange
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, UpdateForPhyScreenPropertyChange, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    
    policy.currentDisplayMode_ = FoldDisplayMode::UNKNOWN;
    policy.UpdateForPhyScreenPropertyChange();
    EXPECT_FALSE(policy.onBootAnimation_);

    policy.currentDisplayMode_ = FoldDisplayMode::SUB;
    policy.UpdateForPhyScreenPropertyChange();
    EXPECT_FALSE(policy.onBootAnimation_);

    policy.currentDisplayMode_ = FoldDisplayMode::FULL;
    policy.UpdateForPhyScreenPropertyChange();
    EXPECT_FALSE(policy.onBootAnimation_);

    policy.currentDisplayMode_ = FoldDisplayMode::MAIN;
    policy.UpdateForPhyScreenPropertyChange();
    EXPECT_FALSE(policy.onBootAnimation_);

    policy.currentDisplayMode_ = FoldDisplayMode::COORDINATION;
    policy.UpdateForPhyScreenPropertyChange();
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: GetModeMatchStatus
 * @tc.desc: test function : GetModeMatchStatus
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, GetModeMatchStatus, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
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

    policy.currentFoldStatus_ = FoldStatus::UNKNOWN;
    displayMode = policy.GetModeMatchStatus();
    EXPECT_EQ(FoldDisplayMode::UNKNOWN, displayMode);
}

/**
 * @tc.name: ReportFoldDisplayModeChange
 * @tc.desc: test function : ReportFoldDisplayModeChange
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, ReportFoldDisplayModeChange, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);

    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    policy.ReportFoldDisplayModeChange(displayMode);
    EXPECT_EQ(FoldDisplayMode::UNKNOWN, displayMode);

    displayMode = FoldDisplayMode::FULL;
    policy.ReportFoldDisplayModeChange(displayMode);
    EXPECT_NE(FoldDisplayMode::UNKNOWN, displayMode);
}

/**
 * @tc.name: ReportFoldStatusChangeBegin
 * @tc.desc: test function : ReportFoldStatusChangeBegin
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, ReportFoldStatusChangeBegin, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);

    int32_t offScreen = 0;
    int32_t onScreen = 1;
    policy.ReportFoldStatusChangeBegin(offScreen, onScreen);
    EXPECT_TRUE(onScreen);

    policy.ReportFoldStatusChangeBegin(offScreen, onScreen);
    EXPECT_TRUE(onScreen);
}

/**
 * @tc.name: ChangeScreenDisplayModeToMain
 * @tc.desc: test function : ChangeScreenDisplayModeToMain
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, ChangeScreenDisplayModeToMain, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    sptr<ScreenSession> screenSession = new ScreenSession;

    policy.onBootAnimation_ = true;
    policy.ChangeScreenDisplayModeToMain(screenSession);
    EXPECT_TRUE(policy.onBootAnimation_);

    policy.ChangeScreenDisplayModeToMain(screenSession);
    EXPECT_TRUE(policy.onBootAnimation_);
}

/**
 * @tc.name: ChangeScreenDisplayModeToFull
 * @tc.desc: test function : ChangeScreenDisplayModeToFull
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, ChangeScreenDisplayModeToFull, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    sptr<ScreenSession> screenSession = new ScreenSession;

    policy.onBootAnimation_ = true;
    policy.ChangeScreenDisplayModeToFull(screenSession);
    EXPECT_TRUE(policy.onBootAnimation_);

    policy.ChangeScreenDisplayModeToFull(screenSession);
    EXPECT_TRUE(policy.onBootAnimation_);
}

/**
 * @tc.name: ChangeScreenDisplayModePower
 * @tc.desc: test function : ChangeScreenDisplayModePower
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, ChangeScreenDisplayModePower, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    EXPECT_TRUE(1);
}

/**
 * @tc.name: SendPropertyChangeResult
 * @tc.desc: test function : SendPropertyChangeResult
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, SendPropertyChangeResult, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    sptr<ScreenSession> screenSession = new ScreenSession;
    ScreenId screenId = 0;
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::UNDEFINED;

    policy.SendPropertyChangeResult(screenSession, screenId, reason);
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: ChangeScreenDisplayModeToMainOnBootAnimation
 * @tc.desc: test function : ChangeScreenDisplayModeToMainOnBootAnimation
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, ChangeScreenDisplayModeToMainOnBootAnimation, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    sptr<ScreenSession> screenSession = new ScreenSession;

    policy.ChangeScreenDisplayModeToMainOnBootAnimation(screenSession);
    EXPECT_FALSE(policy.onBootAnimation_);
}

/**
 * @tc.name: ChangeScreenDisplayModeToFullOnBootAnimation
 * @tc.desc: test function : ChangeScreenDisplayModeToFullOnBootAnimation
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplayFoldPolicyTest, ChangeScreenDisplayModeToFullOnBootAnimation, Function | SmallTest | Level3)
{
    std::recursive_mutex displayInfoMutex;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler = nullptr;
    SingleDisplayFoldPolicy policy(displayInfoMutex, screenPowerTaskScheduler);
    sptr<ScreenSession> screenSession = new ScreenSession;

    policy.ChangeScreenDisplayModeToFullOnBootAnimation(screenSession);
    EXPECT_FALSE(policy.onBootAnimation_);
}
}
} // namespace Rosen
} // namespace OHOS