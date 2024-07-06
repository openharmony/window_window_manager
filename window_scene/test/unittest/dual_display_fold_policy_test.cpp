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

#include "fold_screen_controller/dual_display_fold_policy.h"
#include "fold_screen_controller/fold_screen_controller.h"
#include "session_manager/include/screen_session_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t SLEEP_TIME_US = 100000;
}

class DualDisplayFoldPolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static ScreenSessionManager& ssm_;
};

ScreenSessionManager& DualDisplayFoldPolicyTest::ssm_ = ScreenSessionManager::GetInstance();

void DualDisplayFoldPolicyTest::SetUpTestCase()
{
}

void DualDisplayFoldPolicyTest::TearDownTestCase()
{
}

void DualDisplayFoldPolicyTest::SetUp()
{
}

void DualDisplayFoldPolicyTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {

    /**
     * @tc.name: ChangeScreenDisplayMode
     * @tc.desc: ChangeScreenDisplayMode
     * @tc.type: FUNC
     */
    HWTEST_F(DualDisplayFoldPolicyTest, ChangeScreenDisplayMode, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        DualDisplayFoldPolicy dualDisplayFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        dualDisplayFoldPolicy.ChangeScreenDisplayMode(FoldDisplayMode::UNKNOWN);
        FoldDisplayMode mode = ssm_.GetFoldDisplayMode();
        ASSERT_EQ(mode, ssm_.GetFoldDisplayMode());

        dualDisplayFoldPolicy.ChangeScreenDisplayMode(FoldDisplayMode::SUB);
        mode = ssm_.GetFoldDisplayMode();
        ASSERT_EQ(mode, ssm_.GetFoldDisplayMode());

        dualDisplayFoldPolicy.ChangeScreenDisplayMode(FoldDisplayMode::MAIN);
        mode = ssm_.GetFoldDisplayMode();
        ASSERT_EQ(mode, ssm_.GetFoldDisplayMode());

        dualDisplayFoldPolicy.ChangeScreenDisplayMode(FoldDisplayMode::COORDINATION);
        mode = ssm_.GetFoldDisplayMode();
        ASSERT_EQ(mode, ssm_.GetFoldDisplayMode());
    }

    /**
     * @tc.name: SendSensorResult
     * @tc.desc: SendSensorResult
     * @tc.type: FUNC
     */
    HWTEST_F(DualDisplayFoldPolicyTest, SendSensorResult, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        DualDisplayFoldPolicy dualDisplayFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        dualDisplayFoldPolicy.SendSensorResult(FoldStatus::UNKNOWN);
        FoldDisplayMode mode = ssm_.GetFoldDisplayMode();
        ASSERT_EQ(mode, ssm_.GetFoldDisplayMode());
    }

    /**
     * @tc.name: SetOnBootAnimation
     * @tc.desc: SetOnBootAnimation
     * @tc.type: FUNC
     */
    HWTEST_F(DualDisplayFoldPolicyTest, SetOnBootAnimation, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        DualDisplayFoldPolicy dualDisplayFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        dualDisplayFoldPolicy.SetOnBootAnimation(true);
        ASSERT_TRUE(dualDisplayFoldPolicy.onBootAnimation_);

        dualDisplayFoldPolicy.SetOnBootAnimation(false);
        ASSERT_FALSE(dualDisplayFoldPolicy.onBootAnimation_);
    }

    /**
     * @tc.name: RecoverWhenBootAnimationExit
     * @tc.desc: RecoverWhenBootAnimationExit
     * @tc.type: FUNC
     */
    HWTEST_F(DualDisplayFoldPolicyTest, RecoverWhenBootAnimationExit, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        DualDisplayFoldPolicy dualDisplayFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        dualDisplayFoldPolicy.RecoverWhenBootAnimationExit();
        FoldDisplayMode mode = ssm_.GetFoldDisplayMode();
        ASSERT_EQ(mode, ssm_.GetFoldDisplayMode());
    }

    /**
     * @tc.name: UpdateForPhyScreenPropertyChange
     * @tc.desc: UpdateForPhyScreenPropertyChange
     * @tc.type: FUNC
     */
    HWTEST_F(DualDisplayFoldPolicyTest, UpdateForPhyScreenPropertyChange, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        DualDisplayFoldPolicy dualDisplayFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        dualDisplayFoldPolicy.UpdateForPhyScreenPropertyChange();
        FoldDisplayMode mode = ssm_.GetFoldDisplayMode();
        ASSERT_EQ(mode, ssm_.GetFoldDisplayMode());
    }

    /**
     * @tc.name: GetModeMatchStatus
     * @tc.desc: GetModeMatchStatus
     * @tc.type: FUNC
     */
    HWTEST_F(DualDisplayFoldPolicyTest, GetModeMatchStatus, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        DualDisplayFoldPolicy dualDisplayFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        FoldDisplayMode ret = dualDisplayFoldPolicy.GetModeMatchStatus();
        ASSERT_EQ(ret, FoldDisplayMode::UNKNOWN);

        dualDisplayFoldPolicy.currentFoldStatus_ = FoldStatus::EXPAND;
        ret = dualDisplayFoldPolicy.GetModeMatchStatus();
        ASSERT_EQ(ret, FoldDisplayMode::MAIN);

        dualDisplayFoldPolicy.currentFoldStatus_ = FoldStatus::FOLDED;
        ret = dualDisplayFoldPolicy.GetModeMatchStatus();
        ASSERT_EQ(ret, FoldDisplayMode::SUB);

        dualDisplayFoldPolicy.currentFoldStatus_ = FoldStatus::HALF_FOLD;
        ret = dualDisplayFoldPolicy.GetModeMatchStatus();
        ASSERT_EQ(ret, FoldDisplayMode::MAIN);
    }

    /**
     * @tc.name: ReportFoldDisplayModeChange
     * @tc.desc: ReportFoldDisplayModeChange
     * @tc.type: FUNC
     */
    HWTEST_F(DualDisplayFoldPolicyTest, ReportFoldDisplayModeChange, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        DualDisplayFoldPolicy dualDisplayFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        dualDisplayFoldPolicy.ReportFoldDisplayModeChange(FoldDisplayMode::FULL);
        FoldDisplayMode mode = ssm_.GetFoldDisplayMode();
        ASSERT_EQ(mode, ssm_.GetFoldDisplayMode());
    }

    /**
     * @tc.name: ReportFoldStatusChangeBegin
     * @tc.desc: ReportFoldStatusChangeBegin
     * @tc.type: FUNC
     */
    HWTEST_F(DualDisplayFoldPolicyTest, ReportFoldStatusChangeBegin, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        DualDisplayFoldPolicy dualDisplayFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        int32_t offScreen = 0;
        int32_t onScreen = 1;
        dualDisplayFoldPolicy.ReportFoldStatusChangeBegin(offScreen, onScreen);
        FoldDisplayMode mode = ssm_.GetFoldDisplayMode();
        ASSERT_EQ(mode, ssm_.GetFoldDisplayMode());
    }

    /**
     * @tc.name: ChangeScreenDisplayModeOnBootAnimation
     * @tc.desc: ChangeScreenDisplayModeOnBootAnimation
     * @tc.type: FUNC
     */
    HWTEST_F(DualDisplayFoldPolicyTest, ChangeScreenDisplayModeOnBootAnimation, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        DualDisplayFoldPolicy dualDisplayFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        sptr<ScreenSession> screenSession = new ScreenSession();
        ScreenId screenId = 0;
        std::string threadName = "test";
        dualDisplayFoldPolicy.screenPowerTaskScheduler_ = std::make_shared<TaskScheduler>(threadName);
        dualDisplayFoldPolicy.ChangeScreenDisplayModeOnBootAnimation(screenSession, screenId);
        ASSERT_EQ(screenId, dualDisplayFoldPolicy.screenId_);
    }

    /**
     * @tc.name: AddOrRemoveDisplayNodeToTree
     * @tc.desc: AddOrRemoveDisplayNodeToTree
     * @tc.type: FUNC
     */
    HWTEST_F(DualDisplayFoldPolicyTest, AddOrRemoveDisplayNodeToTree, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        DualDisplayFoldPolicy dualDisplayFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        sptr<ScreenSession> screenSession = new ScreenSession();
        ScreenId screenId = 0;
        int32_t command = 1;
        dualDisplayFoldPolicy.AddOrRemoveDisplayNodeToTree(screenId, command);
        FoldDisplayMode mode = ssm_.GetFoldDisplayMode();
        ASSERT_EQ(mode, ssm_.GetFoldDisplayMode());
    }
}
} // namespace Rosen
} // namespace OHOS