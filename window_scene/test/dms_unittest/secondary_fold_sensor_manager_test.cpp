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
#include "screen_session_manager/include/fold_screen_controller/sensor_fold_state_manager/secondary_display_sensor_fold_state_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
std::shared_ptr<TaskScheduler> screenPowerTaskScheduler_ = std::make_shared<TaskScheduler>("test");
sptr<SecondaryDisplayFoldPolicy> g_policy =
    new SecondaryDisplayFoldPolicy(g_displayInfoMutex, screenPowerTaskScheduler_);
sptr<SecondaryDisplaySensorFoldStateManager> g_stateManager = new SecondaryDisplaySensorFoldStateManager();
}
class SecondaryFoldSensorManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SecondaryFoldSensorManagerTest::SetUpTestCase()
{
    SecondaryFoldSensorManager::GetInstance().SetFoldScreenPolicy(g_policy);
    SecondaryFoldSensorManager::GetInstance().SetSensorFoldStateManager(g_stateManager);
}

void SecondaryFoldSensorManagerTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void SecondaryFoldSensorManagerTest::SetUp()
{
}

void SecondaryFoldSensorManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: RegisterPostureCallback01
 * @tc.desc: test function : RegisterPostureCallback
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, RegisterPostureCallback01, Function | SmallTest | Level3)
{
    SecondaryFoldSensorManager::GetInstance().RegisterPostureCallback();
    EXPECT_NE(SecondaryFoldSensorManager::GetInstance().IsPostureUserCallbackNull(), nullptr);
}

/**
 * @tc.name: UnRegisterPostureCallback01
 * @tc.desc: test function : UnRegisterPostureCallback
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, UnRegisterPostureCallback01, Function | SmallTest | Level3)
{
    SecondaryFoldSensorManager::GetInstance().UnRegisterPostureCallback();
    EXPECT_NE(SecondaryFoldSensorManager::GetInstance().IsPostureUserCallbackNull(), nullptr);
}

/**
 * @tc.name: RegisterHallCallback01
 * @tc.desc: test function : RegisterHallCallback
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, RegisterHallCallback01, Function | SmallTest | Level3)
{
    SecondaryFoldSensorManager::GetInstance().RegisterHallCallback();
    EXPECT_NE(SecondaryFoldSensorManager::GetInstance().IsHallUserCallbackNull(), nullptr);
}

/**
 * @tc.name: UnRegisterHallCallback01
 * @tc.desc: test function : UnRegisterHallCallback
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, UnRegisterHallCallback01, Function | SmallTest | Level3)
{
    SecondaryFoldSensorManager::GetInstance().UnRegisterHallCallback();
    EXPECT_NE(SecondaryFoldSensorManager::GetInstance().IsHallUserCallbackNull(), nullptr);
}
}
}
}