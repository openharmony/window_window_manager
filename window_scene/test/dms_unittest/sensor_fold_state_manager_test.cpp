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

#include "fold_screen_controller/fold_screen_policy.h"
#include "fold_screen_controller/sensor_fold_state_manager/sensor_fold_state_manager.h"
#include "fold_screen_state_internel.h"
#include "session_manager/include/screen_session_manager.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_US = 100000;
}

class SensorFoldStateManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SensorFoldStateManagerTest::SetUpTestCase()
{
}

void SensorFoldStateManagerTest::TearDownTestCase()
{
}

void SensorFoldStateManagerTest::SetUp()
{
}

void SensorFoldStateManagerTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {

/**
 * @tc.name: HandleSensorChange
 * @tc.desc: HandleSensorChange
 * @tc.type: FUNC
 */
HWTEST_F(SensorFoldStateManagerTest, HandleSensorChange, Function | SmallTest | Level3)
{
    SensorFoldStateManager mgr = SensorFoldStateManager();
    FoldStatus nextState = FoldStatus::UNKNOWN;
    float angle = 0.0f;
    sptr<FoldScreenPolicy> foldScreenPolicy = new FoldScreenPolicy();
    mgr.HandleSensorChange(nextState, angle, foldScreenPolicy);
    ASSERT_EQ(mgr.mState_, FoldStatus::UNKNOWN);

    mgr.mState_ = FoldStatus::EXPAND;
    mgr.HandleSensorChange(nextState, angle, foldScreenPolicy);
    ASSERT_EQ(mgr.mState_, FoldStatus::EXPAND);

    nextState = FoldStatus::EXPAND;
    mgr.HandleSensorChange(nextState, angle, foldScreenPolicy);
    ASSERT_EQ(mgr.mState_, FoldStatus::EXPAND);
}

/**
 * @tc.name: ReportNotifyFoldStatusChange
 * @tc.desc: ReportNotifyFoldStatusChange
 * @tc.type: FUNC
 */
HWTEST_F(SensorFoldStateManagerTest, ReportNotifyFoldStatusChange, Function | SmallTest | Level3)
{
    SensorFoldStateManager mgr = SensorFoldStateManager();
    int32_t currentStatus = 0;
    int32_t nextStatus = 1;
    float postureAngle = 0.0f;
    mgr.ReportNotifyFoldStatusChange(currentStatus, nextStatus, postureAngle);
    ASSERT_EQ(mgr.GetCurrentState(), FoldStatus::UNKNOWN);
}

/**
 * @tc.name: ClearState
 * @tc.desc: ClearState
 * @tc.type: FUNC
 */
HWTEST_F(SensorFoldStateManagerTest, ClearState, Function | SmallTest | Level3)
{
    SensorFoldStateManager mgr = SensorFoldStateManager();
    sptr<FoldScreenPolicy> foldScreenPolicy = new FoldScreenPolicy();
    mgr.ClearState(foldScreenPolicy);
    ASSERT_EQ(mgr.GetCurrentState(), FoldStatus::UNKNOWN);
}

/**
 * @tc.name: NotifyReportFoldStatusToScb
 * @tc.desc: NotifyReportFoldStatusToScb
 * @tc.type: FUNC
 */
HWTEST_F(SensorFoldStateManagerTest, NotifyReportFoldStatusToScb, Function | SmallTest | Level3)
{
    SensorFoldStateManager mgr = SensorFoldStateManager();
    FoldStatus currentStatus = FoldStatus::UNKNOWN;
    FoldStatus nextStatus = FoldStatus::EXPAND;
    float postureAngle = 0.0f;
    mgr.NotifyReportFoldStatusToScb(currentStatus, nextStatus, postureAngle);
    ASSERT_EQ(mgr.GetCurrentState(), FoldStatus::UNKNOWN);
}
}
} // namespace Rosen
} // namespace OHOS