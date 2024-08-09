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

#include "session_manager/include/fold_screen_controller/sensor_fold_state_manager/single_display_sensor_fold_state_manager.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class SingleDisplaySensorFoldStateManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SingleDisplaySensorFoldStateManagerTest::SetUpTestCase()
{
}

void SingleDisplaySensorFoldStateManagerTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void SingleDisplaySensorFoldStateManagerTest::SetUp()
{
}

void SingleDisplaySensorFoldStateManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: HandleAngleChange
 * @tc.desc: test function : HandleAngleChange
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorFoldStateManagerTest, HandleAngleChange, Function | SmallTest | Level1)
{
    float angel = 0;
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
HWTEST_F(SingleDisplaySensorFoldStateManagerTest, HandleHallChange, Function | SmallTest | Level1)
{
    float angel = 0;
    int hall = 0;
    sptr<FoldScreenPolicy> foldScreenPolicy = nullptr;
    SingleDisplaySensorFoldStateManager manager;
    manager.HandleHallChange(angel, hall, foldScreenPolicy);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UpdateSwitchScreenBoundaryForLargeFoldDevice
 * @tc.desc: test function : UpdateSwitchScreenBoundaryForLargeFoldDevice
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorFoldStateManagerTest, UpdateSwitchScreenBoundaryForLargeFoldDevice,
        Function | SmallTest | Level1)
{
    float angel = 0;
    int hall = 0;
    SingleDisplaySensorFoldStateManager manager;
    manager.UpdateSwitchScreenBoundaryForLargeFoldDevice(angel, hall);
    EXPECT_TRUE(true);

    angel = 91.0F;
    hall = 1;
    manager.UpdateSwitchScreenBoundaryForLargeFoldDevice(angel, hall);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetNextFoldState01
 * @tc.desc: test function : GetNextFoldState
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorFoldStateManagerTest, GetNextFoldState01, Function | SmallTest | Level1)
{
    float angel = -0.1;
    int hall = 0;
    SingleDisplaySensorFoldStateManager manager;
    auto result1 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result1), 0);

    manager.allowUserSensorForLargeFoldDevice = 0;
    angel = 90.0F;
    hall = 1;
    auto result2 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result2), 3);

    angel = 130.0F - 0.1;
    hall = 1;
    auto result3 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result3), 3);
    
    angel = 130.0F - 0.1;
    hall = 0;
    auto result4 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result4), 2);

    angel = 130.0F + 0.1;
    hall = 0;
    auto result5 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result5), 2);

    angel = 140.0F + 0.1;
    hall = 0;
    auto result6 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result6), 2);

    angel = 140.0F + 0.1;
    hall = 1;
    auto result7 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result7), 1);
}

/**
 * @tc.name: GetNextFoldState02
 * @tc.desc: test function : GetNextFoldState
 * @tc.type: FUNC
 */
HWTEST_F(SingleDisplaySensorFoldStateManagerTest, GetNextFoldState02, Function | SmallTest | Level1)
{
    SingleDisplaySensorFoldStateManager manager;
    manager.allowUserSensorForLargeFoldDevice = 1;
    float angel = 25.0F;
    int hall = 1;
    auto result1 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result1), 0);

    angel = 70.0F - 0.1;
    auto result2 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result2), 2);

    angel = 70.0F + 0.1;
    auto result3 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result3), 3);
    
    angel = 130.0F - 0.1;
    auto result4 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result4), 3);

    angel = 130.0F + 0.1;
    auto result5 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result5), 3);

    angel = 80.0F - 0.1;
    auto result6 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result6), 3);

    angel = 70.0F + 0.1;
    hall = 0;
    auto result7 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result7), 2);

    angel = 130.0F + 0.1;
    auto result8 = manager.GetNextFoldState(angel, hall);
    EXPECT_EQ(static_cast<int>(result8), 2);
}
}
}
}