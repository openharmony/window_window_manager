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

#include "screen_session_manager/include/fold_screen_controller/sensor_fold_state_manager/single_display_sensor_pocket_fold_state_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class SingleDisplaySensorPocketFoldStateManager : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SingleDisplaySensorPocketFoldStateManager::SetUpTestCase()
{
}

void SingleDisplaySensorPocketFoldStateManager::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void SingleDisplaySensorPocketFoldStateManager::SetUp()
{
}

void SingleDisplaySensorPocketFoldStateManager::TearDown()
{
}

namespace {
/**
 * @tc.name: HandleAngleChange
 * @tc.desc: test function : HandleAngleChange
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorPocketFoldStateManager, HandleAngleChange, Function | SmallTest | Level1)
{
    float angel = 0;
    int hall = 0;
    sptr<FoldScreenPolicy> foldScreenPolicy = nullptr;
    SingleDisplaySensorFoldStateManager manager;
    manager.HandleAngleChange(angel, hall, foldScreenPolicy);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: HandleAngleChange
 * @tc.desc: test function : HandleAngleChange
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorPocketFoldStateManager, HandleAngleChange, Function | SmallTest | Level1)
{
    float angel = 180;
    int hall = 0;
    sptr<FoldScreenPolicy> foldScreenPolicy = nullptr;
    SingleDisplaySensorFoldStateManager manager;
    manager.HandleAngleChange(angel, hall, foldScreenPolicy);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: HandleHallChange
 * @tc.desc: test function : HandleHallChange
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorPocketFoldStateManager, HandleHallChange, Function | SmallTest | Level1)
{
    float angel = 0;
    int hall = 0;
    sptr<FoldScreenPolicy> foldScreenPolicy = nullptr;
    SingleDisplaySensorFoldStateManager manager;
    manager.HandleHallChange(angel, hall, foldScreenPolicy);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: HandleHallChange
 * @tc.desc: test function : HandleHallChange
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorPocketFoldStateManager, HandleHallChange, Function | SmallTest | Level1)
{
    float angel = -1;
    int hall = 1;
    sptr<FoldScreenPolicy> foldScreenPolicy = nullptr;
    SingleDisplaySensorFoldStateManager manager;
    manager.HandleHallChange(angel, hall, foldScreenPolicy);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: HandleHallChange
 * @tc.desc: test function : HandleHallChange
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorPocketFoldStateManager, HandleHallChange, Function | SmallTest | Level1)
{
    float angel = 180;
    int hall = 1;
    sptr<FoldScreenPolicy> foldScreenPolicy = nullptr;
    SingleDisplaySensorFoldStateManager manager;
    manager.HandleHallChange(angel, hall, foldScreenPolicy);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: HandleHallChange
 * @tc.desc: test function : HandleHallChange
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorPocketFoldStateManager, HandleHallChange, Function | SmallTest | Level1)
{
    float angel = 0;
    int hall = 1;
    sptr<FoldScreenPolicy> foldScreenPolicy = nullptr;
    SingleDisplaySensorFoldStateManager manager;
    manager.HandleHallChange(angel, hall, foldScreenPolicy);
    EXPECT_TRUE(true);
}

HWTEST_F(SingleDisplaySensorPocketFoldStateManagerTest, HandleAngleChange, Function | SmallTest | Level3)
{
    SingleDisplaySensorPocketFoldStateManager mgr = SingleDisplaySensorPocketFoldStateManager();
    float angle = 0.0f;
    int hall = 0;
    sptr<FoldScreenPolicy> foldScreenPolicy = new FoldScreenPolicy();
    mgr.HandleAngleChange(angle, hall, foldScreenPolicy);
    ASSERT_TRUE(std::islessequal(angle, INWARD_FOLDED_THRESHOLD + ANGLE_BUFFER));
    ASSERT_FALSE(hall == HALL_THRESHOLD);

    hall = 1;
    mgr.HandleAngleChange(angle, hall, foldScreenPolicy);
    ASSERT_TRUE(std::islessequal(angle, INWARD_FOLDED_THRESHOLD + ANGLE_BUFFER));
    ASSERT_TRUE(hall == HALL_THRESHOLD);

    angle = 100.0f;
    mgr.HandleAngleChange(angle, hall, foldScreenPolicy);
    ASSERT_FALSE(std::islessequal(angle, INWARD_FOLDED_THRESHOLD + ANGLE_BUFFER));
    ASSERT_TRUE(hall == HALL_THRESHOLD);

    hall = 0;
    mgr.HandleAngleChange(angle, hall, foldScreenPolicy);
    ASSERT_FALSE(std::islessequal(angle, INWARD_FOLDED_THRESHOLD + ANGLE_BUFFER));
    ASSERT_FALSE(hall == HALL_THRESHOLD);
}

/**
 * @tc.name: HandleHallChange
 * @tc.desc: HandleHallChange
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorPocketFoldStateManagerTest, HandleHallChange, Function | SmallTest | Level3)
{
    SingleDisplaySensorPocketFoldStateManager mgr = SingleDisplaySensorPocketFoldStateManager();
    float angle = 0.0f;
    int hall = 1;
    sptr<FoldScreenPolicy> foldScreenPolicy = new FoldScreenPolicy();
    mgr.applicationStateObserver_ = nullptr;
    mgr.HandleHallChange(angle, hall, foldScreenPolicy);
    ASSERT_FALSE(mgr.applicationStateObserver_ != nullptr);
    ASSERT_TRUE(hall == HALL_THRESHOLD);

    mgr.applicationStateObserver_ = new ApplicationStateObserver();
    mgr.HandleHallChange(angle, hall, foldScreenPolicy);
    ASSERT_TRUE(mgr.applicationStateObserver_ != nullptr);
    ASSERT_TRUE(hall == HALL_THRESHOLD);

    mgr.applicationStateObserver_ = nullptr;
    hall = 0;
    mgr.HandleHallChange(angle, hall, foldScreenPolicy);
    ASSERT_FALSE(mgr.applicationStateObserver_ != nullptr);
    ASSERT_FALSE(hall == HALL_THRESHOLD);
}

/**
 * @tc.name: GetNextFoldState
 * @tc.desc: GetNextFoldState
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorPocketFoldStateManagerTest, GetNextFoldState, Function | SmallTest | Level3)
{
    SingleDisplaySensorPocketFoldStateManager mgr = SingleDisplaySensorPocketFoldStateManager();
    float angle = 200.0f;
    int hall = 1;
    FoldStatus ret = mgr.GetNextFoldState(angle, hall);
    ASSERT_TRUE(std::isgreaterequal(angle, INWARD_EXPAND_THRESHOLD + ANGLE_BUFFER));
    ASSERT_EQ(ret, FoldStatus::EXPAND);

    angle = 0.0f;
    ret = mgr.GetNextFoldState(angle, hall);
    ASSERT_TRUE(std::islessequal
    (angle, INWARD_FOLDED_LOWER_THRESHOLD + ANGLE_BUFFER));
    ASSERT_EQ(ret, FoldStatus::FOLDED);

    mgr.isHallSwitchApp_ = true;
    angle = 30.0f;
    ret = mgr.GetNextFoldState(angle, hall);
    ASSERT_TRUE(std::isgreaterequal(angle, INWARD_FOLDED_UPPER_THRESHOLD + ANGLE_BUFFER));
    ASSERT_TRUE(std::islessequal(angle, INWARD_HALF_FOLDED_MAX_THRESHOLD + ANGLE_BUFFER));
    ASSERT_EQ(ret, FoldStatus::HALF_FOLD);

    mgr.isHallSwitchApp_ = false;
    angle = 120.0f;
    ret = mgr.GetNextFoldState(angle, hall);
    ASSERT_TRUE(std::isgreaterequal(angle, INWARD_HALF_FOLDED_MIN_THRESHOLD + ANGLE_BUFFER));
    ASSERT_TRUE(std::islessequal(angle, INWARD_HALF_FOLDED_MAX_THRESHOLD + ANGLE_BUFFER));
    ASSERT_EQ(ret, FoldStatus::HALF_FOLD);
}

/**
 * @tc.name: RegisterApplicationStateObserver
 * @tc.desc: RegisterApplicationStateObserver
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorPocketFoldStateManagerTest, RegisterApplicationStateObserver, Function | SmallTest | Level3)
{
    SingleDisplaySensorPocketFoldStateManager mgr = SingleDisplaySensorPocketFoldStateManager();
    mgr.RegisterApplicationStateObserver();
    ASSERT_NE(mgr.applicationStateObserver_, nullptr);
}
}
}
}