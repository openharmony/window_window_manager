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

#include "screen_session_manager/include/fold_screen_controller/sensor_fold_state_manager/single_display_sensor_fold_state_manager.h"
#include "screen_session_manager/include/fold_screen_controller/secondary_display_fold_policy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
sptr<SecondaryDisplaySensorFoldStateManager> g_stateManager = new SecondaryDisplaySensorFoldStateManager();
std::shared_ptr<TaskScheduler> screenPowerTaskScheduler_ = std::make_shared<TaskScheduler>("test");
sptr<SecondaryDisplayFoldPolicy> g_policy =
    new SecondaryDisplayFoldPolicy(g_displayInfoMutex, screenPowerTaskScheduler_);
}
class SecondaryDisplaySensorFoldStateManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SecondaryDisplaySensorFoldStateManagerTest::SetUpTestCase()
{
}

void SecondaryDisplaySensorFoldStateManagerTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void SecondaryDisplaySensorFoldStateManagerTest::SetUp()
{
}

void SecondaryDisplaySensorFoldStateManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: HandleAngleChange
 * @tc.desc: test function : HandleAngleChange
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryDisplaySensorFoldStateManagerTest, HandleAngleChange, Function | SmallTest | Level1)
{
    std::vector<float> angles = { 180.0F, 180.0F };
    std::vector<uint16_t> halls = { 0, 0 };
    g_stateManager->HandleAngleChange(angles, halls, g_policy);
    EXPECT_TRUE(g_stateManager->GetCurrentState(), FoldStatus.EXPAND);
}
}
}
}