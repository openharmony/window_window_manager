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

#include "session_manager/include/fold_screen_controller/secondary_display_fold_policy.h"
#include "session_manager/include/fold_screen_controller/secondary_fold_sensor_manager.h"
#include "session_manager/include/fold_screen_controller/fold_screen_sensor_manager.h"
#include "session_manager/include/fold_screen_controller/sensor_fold_state_manager/secondary_display_sensor_fold_state_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
std::recursive_mutex g_displayInfoMutex;
std::shared_ptr<TaskScheduler> screenPowerTaskScheduler_ = std::make_shared<TaskScheduler>("test");
sptr<SecondaryDisplayFoldPolicy> g_policy =
    new SecondaryDisplayFoldPolicy(g_displayInfoMutex, screenPowerTaskScheduler_);
sptr<SecondaryDisplaySensorFoldStateManager> g_stateManager = new SecondaryDisplaySensorFoldStateManager();
constexpr size_t SECONDARY_POSTURE_SIZE = 3;
constexpr size_t SECONDARY_HALL_SIZE = 2;
constexpr float CORRECT_POSTURE_BC = 90.0F;
constexpr float CORRECT_POSTURE_AB = 160.0F;
constexpr float CORRECT_POSTURE_AB_ANTI = 5.0F;
constexpr uint16_t CORRECT_HALL_BC = 0;
constexpr uint16_t CORRECT_HALL_AB = 0;
constexpr uint16_t FIRST_DATA = 0;
constexpr uint16_t SECOND_DATA = 1;
constexpr uint16_t THIRD_DATA = 2;
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
    EXPECT_FALSE(SecondaryFoldSensorManager::GetInstance().IsPostureUserCallbackInvalid());
}

/**
 * @tc.name: UnRegisterPostureCallback01
 * @tc.desc: test function : UnRegisterPostureCallback
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, UnRegisterPostureCallback01, Function | SmallTest | Level3)
{
    SecondaryFoldSensorManager::GetInstance().UnRegisterPostureCallback();
    EXPECT_FALSE(SecondaryFoldSensorManager::GetInstance().IsPostureUserCallbackInvalid());
}

/**
 * @tc.name: RegisterHallCallback01
 * @tc.desc: test function : RegisterHallCallback
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, RegisterHallCallback01, Function | SmallTest | Level3)
{
    SecondaryFoldSensorManager::GetInstance().RegisterHallCallback();
    EXPECT_FALSE(SecondaryFoldSensorManager::GetInstance().IsHallUserCallbackInvalid());
}

/**
 * @tc.name: UnRegisterHallCallback01
 * @tc.desc: test function : UnRegisterHallCallback
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, UnRegisterHallCallback01, Function | SmallTest | Level3)
{
    SecondaryFoldSensorManager::GetInstance().UnRegisterHallCallback();
    EXPECT_FALSE(SecondaryFoldSensorManager::GetInstance().IsHallUserCallbackInvalid());
}

static std::vector<float> HandlePostureData(float postureBc, float postureAb, float postureAbAnti)
{
    FoldScreenSensorManager::PostureDataSecondary postureData = {
        .postureBc = postureBc,
        .postureAb = postureAb,
        .postureAbAnti = postureAbAnti,
    };
    SensorEvent postureEvent = {
        .dataLen = sizeof(FoldScreenSensorManager::PostureDataSecondary),
        .data = reinterpret_cast<uint8_t *>(&postureData),
    };
    OHOS::Rosen::SecondaryFoldSensorManager::GetInstance().HandlePostureData(&postureEvent);
    std::vector<float> postures = SecondaryFoldSensorManager::GetInstance().GetGlobalAngle();
    return postures;
}

/**
 * @tc.name: HandlePostureData01
 * @tc.desc: test function : HandlePostureData
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, HandlePostureData01, Function | SmallTest | Level3)
{
    OHOS::Rosen::SecondaryFoldSensorManager::GetInstance().HandlePostureData(nullptr);
    std::vector<float> postures = SecondaryFoldSensorManager::GetInstance().GetGlobalAngle();
    EXPECT_EQ(postures.size(), SECONDARY_POSTURE_SIZE);
    EXPECT_EQ(postures[FIRST_DATA], -1.0F);
    EXPECT_EQ(postures[SECOND_DATA], -1.0F);
    EXPECT_EQ(postures[THIRD_DATA], -1.0F);
}

/**
 * @tc.name: HandlePostureData02
 * @tc.desc: test function : HandlePostureData
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, HandlePostureData02, Function | SmallTest | Level3)
{
    SensorEvent postureEvent = {
        .dataLen = sizeof(PostureData),
        .data = nullptr,
    };
    OHOS::Rosen::SecondaryFoldSensorManager::GetInstance().HandlePostureData(&postureEvent);
    std::vector<float> postures = SecondaryFoldSensorManager::GetInstance().GetGlobalAngle();
    EXPECT_EQ(postures.size(), SECONDARY_POSTURE_SIZE);
    EXPECT_EQ(postures[FIRST_DATA], -1.0F);
    EXPECT_EQ(postures[SECOND_DATA], -1.0F);
    EXPECT_EQ(postures[THIRD_DATA], -1.0F);
}

/**
 * @tc.name: HandlePostureData03
 * @tc.desc: test function : HandlePostureData
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, HandlePostureData03, Function | SmallTest | Level3)
{
    float postureBc = 181.0F;
    float postureAb = 160.0F;
    float postureAbAnti = -1.0F;
    std::vector<float> postures = HandlePostureData(postureBc, postureAb, postureAbAnti);
    EXPECT_EQ(postures.size(), SECONDARY_POSTURE_SIZE);
    EXPECT_EQ(postures[FIRST_DATA], postureBc);
    EXPECT_EQ(postures[SECOND_DATA], postureAb);
    EXPECT_EQ(postures[THIRD_DATA], postureAbAnti);
}

/**
 * @tc.name: HandlePostureData04
 * @tc.desc: test function : HandlePostureData
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, HandlePostureData04, Function | SmallTest | Level3)
{
    std::vector<float> postures = HandlePostureData(CORRECT_POSTURE_BC, CORRECT_POSTURE_AB, CORRECT_POSTURE_AB_ANTI);
    EXPECT_EQ(postures.size(), SECONDARY_POSTURE_SIZE);
    EXPECT_EQ(postures[FIRST_DATA], CORRECT_POSTURE_BC);
    EXPECT_EQ(postures[SECOND_DATA], CORRECT_POSTURE_AB);
    EXPECT_EQ(postures[THIRD_DATA], CORRECT_POSTURE_AB_ANTI);
}

static std::vector<uint16_t> HandleHallDataExt(uint16_t hallBc, uint16_t hallAb)
{
    FoldScreenSensorManager::EXTHALLData hallData = {
        .flag = 26,
        .hall = hallBc,
        .hallAb = hallAb,
    };
    SensorEvent hallEvent = {
        .dataLen = sizeof(FoldScreenSensorManager::EXTHALLData),
        .data = reinterpret_cast<uint8_t *>(&hallData),
    };
    OHOS::Rosen::SecondaryFoldSensorManager::GetInstance().HandleHallDataExt(&hallEvent);
    std::vector<uint16_t> halls = SecondaryFoldSensorManager::GetInstance().GetGlobalHall();
    return halls;
}

/**
 * @tc.name: HandleHallDataExt01
 * @tc.desc: test function : HandleHallDataExt
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, HandleHallDataExt01, Function | SmallTest | Level3)
{
    OHOS::Rosen::SecondaryFoldSensorManager::GetInstance().HandleHallDataExt(nullptr);
    std::vector<uint16_t> halls = SecondaryFoldSensorManager::GetInstance().GetGlobalHall();
    EXPECT_EQ(halls.size(), SECONDARY_HALL_SIZE);
    EXPECT_EQ(halls[FIRST_DATA], USHRT_MAX);
    EXPECT_EQ(halls[SECOND_DATA], USHRT_MAX);
}

/**
 * @tc.name: HandleHallDataExt02
 * @tc.desc: test function : HandleHallDataExt
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, HandleHallDataExt02, Function | SmallTest | Level3)
{
    SensorEvent hallEvent = {
        .dataLen = sizeof(FoldScreenSensorManager::EXTHALLData),
        .data = nullptr,
    };
    OHOS::Rosen::SecondaryFoldSensorManager::GetInstance().HandleHallDataExt(&hallEvent);
    std::vector<uint16_t> halls = SecondaryFoldSensorManager::GetInstance().GetGlobalHall();
    EXPECT_EQ(halls.size(), SECONDARY_HALL_SIZE);
    EXPECT_EQ(halls[FIRST_DATA], USHRT_MAX);
    EXPECT_EQ(halls[SECOND_DATA], USHRT_MAX);
}

/**
 * @tc.name: HandleHallDataExt03
 * @tc.desc: test function : HandleHallDataExt
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, HandleHallDataExt03, Function | SmallTest | Level3)
{
    uint16_t hallBc = 2;
    uint16_t hallAb = 0;
    std::vector<uint16_t> halls = HandleHallDataExt(hallBc, hallAb);
    EXPECT_EQ(halls.size(), SECONDARY_HALL_SIZE);
    EXPECT_EQ(halls[FIRST_DATA], hallBc);
    EXPECT_EQ(halls[SECOND_DATA], hallAb);
}

/**
 * @tc.name: HandleHallDataExt04
 * @tc.desc: test function : HandleHallDataExt
 * @tc.type: FUNC
 */
HWTEST_F(SecondaryFoldSensorManagerTest, HandleHallDataExt04, Function | SmallTest | Level3)
{
    std::vector<uint16_t> halls = HandleHallDataExt(CORRECT_HALL_BC, CORRECT_HALL_AB);
    EXPECT_EQ(halls.size(), SECONDARY_HALL_SIZE);
    EXPECT_EQ(halls[FIRST_DATA], CORRECT_HALL_BC);
    EXPECT_EQ(halls[SECOND_DATA], CORRECT_HALL_AB);
}
}
}
}