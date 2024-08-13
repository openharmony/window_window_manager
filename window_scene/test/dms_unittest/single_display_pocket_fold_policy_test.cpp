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
#include "fold_screen_controller/single_display_pocket_fold_policy.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager.h"

#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t SLEEP_TIME_US = 100000;
    const ScreenId SCREEN_ID_FULL = 0;
}

class SingleDisplayPocketFoldPolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SingleDisplayPocketFoldPolicyTest::SetUpTestCase()
{
}

void SingleDisplayPocketFoldPolicyTest::TearDownTestCase()
{
}

void SingleDisplayPocketFoldPolicyTest::SetUp()
{
}

void SingleDisplayPocketFoldPolicyTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {
    /**
     * @tc.name: ChangeScreenDisplayMode
     * @tc.desc: test function : ChangeScreenDisplayMode
     * @tc.type: FUNC
     */
    HWTEST_F(SingleDisplayPocketFoldPolicyTest, ChangeScreenDisplayMode, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        SingleDisplayPocketFoldPolicy policy = SingleDisplayPocketFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        FoldDisplayMode displayMode = FoldDisplayMode::MAIN;
        policy.currentDisplayMode_ = FoldDisplayMode::FULL;
        policy.ChangeScreenDisplayMode(displayMode);
        ASSERT_EQ(ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_FULL), nullptr);
    }

    /**
     * @tc.name: SendSensorResult
     * @tc.desc: test function : SendSensorResult
     * @tc.type: FUNC
     */
    HWTEST_F(SingleDisplayPocketFoldPolicyTest, SendSensorResult, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        SingleDisplayPocketFoldPolicy policy = SingleDisplayPocketFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        FoldStatus foldStatue = FoldStatus::EXPAND;
        policy.SendSensorResult(foldStatue);
        ASSERT_EQ(policy.currentDisplayMode_, FoldDisplayMode::UNKNOWN);

        policy.currentDisplayMode_ = FoldDisplayMode::FULL;
        policy.SendSensorResult(foldStatue);
        ASSERT_EQ(policy.currentDisplayMode_, FoldDisplayMode::FULL);
    }

    /**
     * @tc.name: SetOnBootAnimation
     * @tc.desc: test function : SetOnBootAnimation
     * @tc.type: FUNC
     */
    HWTEST_F(SingleDisplayPocketFoldPolicyTest, SetOnBootAnimation, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        SingleDisplayPocketFoldPolicy policy = SingleDisplayPocketFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        bool onBootAnimation = true;
        policy.SetOnBootAnimation(onBootAnimation);
        ASSERT_TRUE(policy.onBootAnimation_);

        onBootAnimation = false;
        policy.SetOnBootAnimation(onBootAnimation);
        ASSERT_FALSE(policy.onBootAnimation_);
    }

    /**
     * @tc.name: RecoverWhenBootAnimationExit
     * @tc.desc: test function : RecoverWhenBootAnimationExit
     * @tc.type: FUNC
     */
    HWTEST_F(SingleDisplayPocketFoldPolicyTest, RecoverWhenBootAnimationExit, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        SingleDisplayPocketFoldPolicy policy = SingleDisplayPocketFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        policy.RecoverWhenBootAnimationExit();
        ASSERT_EQ(policy.currentDisplayMode_, FoldDisplayMode::UNKNOWN);

        policy.currentDisplayMode_ = FoldDisplayMode::FULL;
        policy.RecoverWhenBootAnimationExit();
        ASSERT_EQ(policy.currentDisplayMode_, FoldDisplayMode::FULL);
    }

    /**
     * @tc.name: TriggerScreenDisplayModeUpdate
     * @tc.desc: test function : TriggerScreenDisplayModeUpdate
     * @tc.type: FUNC
     */
    HWTEST_F(SingleDisplayPocketFoldPolicyTest, TriggerScreenDisplayModeUpdate, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        SingleDisplayPocketFoldPolicy policy = SingleDisplayPocketFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        FoldDisplayMode displayMode = FoldDisplayMode::MAIN;
        policy.TriggerScreenDisplayModeUpdate(displayMode);
        ASSERT_EQ(ScreenSessionManager::GetInstance().GetScreenSession(SCREEN_ID_FULL), nullptr);
    }

    /**
     * @tc.name: UpdateForPhyScreenPropertyChange
     * @tc.desc: test function : UpdateForPhyScreenPropertyChange
     * @tc.type: FUNC
     */
    HWTEST_F(SingleDisplayPocketFoldPolicyTest, UpdateForPhyScreenPropertyChange, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        SingleDisplayPocketFoldPolicy policy = SingleDisplayPocketFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        policy.UpdateForPhyScreenPropertyChange();
        ASSERT_EQ(policy.currentDisplayMode_, FoldDisplayMode::UNKNOWN);

        policy.currentDisplayMode_ = FoldDisplayMode::FULL;
        policy.UpdateForPhyScreenPropertyChange();
        ASSERT_EQ(policy.currentDisplayMode_, FoldDisplayMode::FULL);
    }

    /**
     * @tc.name: GetModeMatchStatus
     * @tc.desc: test function : GetModeMatchStatus
     * @tc.type: FUNC
     */
    HWTEST_F(SingleDisplayPocketFoldPolicyTest, GetModeMatchStatus, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        SingleDisplayPocketFoldPolicy policy = SingleDisplayPocketFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        FoldDisplayMode ret = policy.GetModeMatchStatus();
        ASSERT_EQ(ret, FoldDisplayMode::UNKNOWN);

        policy.currentFoldStatus_ = FoldStatus::EXPAND;
        ret = policy.GetModeMatchStatus();
        ASSERT_EQ(ret, FoldDisplayMode::FULL);

        policy.currentFoldStatus_ = FoldStatus::FOLDED;
        ret = policy.GetModeMatchStatus();
        ASSERT_EQ(ret, FoldDisplayMode::MAIN);

        policy.currentFoldStatus_ = FoldStatus::HALF_FOLD;
        ret = policy.GetModeMatchStatus();
        ASSERT_EQ(ret, FoldDisplayMode::FULL);
    }

    /**
     * @tc.name: ReportFoldDisplayModeChange
     * @tc.desc: test function : ReportFoldDisplayModeChange
     * @tc.type: FUNC
     */
    HWTEST_F(SingleDisplayPocketFoldPolicyTest, ReportFoldDisplayModeChange, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        SingleDisplayPocketFoldPolicy policy = SingleDisplayPocketFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
        policy.ReportFoldDisplayModeChange(displayMode);
        ASSERT_EQ(policy.currentDisplayMode_, FoldDisplayMode::UNKNOWN);
    }

    /**
     * @tc.name: ReportFoldStatusChangeBegin
     * @tc.desc: test function : ReportFoldStatusChangeBegin
     * @tc.type: FUNC
     */
    HWTEST_F(SingleDisplayPocketFoldPolicyTest, ReportFoldStatusChangeBegin, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        SingleDisplayPocketFoldPolicy policy = SingleDisplayPocketFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        int32_t offScreen = 0;
        int32_t onScreen = 1;
        policy.ReportFoldStatusChangeBegin(offScreen, onScreen);
        ASSERT_EQ(policy.currentDisplayMode_, FoldDisplayMode::UNKNOWN);
    }

    /**
     * @tc.name: ChangeScreenDisplayModeToMain
     * @tc.desc: test function : ChangeScreenDisplayModeToMain
     * @tc.type: FUNC
     */
    HWTEST_F(SingleDisplayPocketFoldPolicyTest, ChangeScreenDisplayModeToMain, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        SingleDisplayPocketFoldPolicy policy = SingleDisplayPocketFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        sptr<ScreenSession> screenSession = new ScreenSession();
        policy.onBootAnimation_ = true;
        policy.ChangeScreenDisplayModeToMain(screenSession);
        ASSERT_TRUE(policy.onBootAnimation_);
    }

    /**
     * @tc.name: ChangeScreenDisplayModeToFull
     * @tc.desc: test function : ChangeScreenDisplayModeToFull
     * @tc.type: FUNC
     */
    HWTEST_F(SingleDisplayPocketFoldPolicyTest, ChangeScreenDisplayModeToFull, Function | SmallTest | Level3)
    {
        std::recursive_mutex mutex;
        SingleDisplayPocketFoldPolicy policy = SingleDisplayPocketFoldPolicy(mutex, std::shared_ptr<TaskScheduler>());
        sptr<ScreenSession> screenSession = new ScreenSession();
        policy.onBootAnimation_ = true;
        policy.ChangeScreenDisplayModeToFull(screenSession);
        ASSERT_TRUE(policy.onBootAnimation_);
    }
}
} // namespace Rosen
} // namespace OHOS