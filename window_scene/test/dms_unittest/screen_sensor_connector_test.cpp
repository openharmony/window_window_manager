/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <parameters.h>
#include "mock_parameters.h"
#include "window_manager_hilog.h"
#include "screen_session_manager.h"

#define private public
#define protected public
#include "screen_sensor_connector.h"
#undef private
#undef protected

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_US = 100000;
const std::string SMART_ROTATION_PARAM = "const.window.device.default_rotation_sensor";
constexpr int32_t DISABLE_SMART_ROTATION = 0;
constexpr int32_t ENABLE_SMART_ROTATION = 1;
}

class ScreenSensorConnectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenSensorConnectorTest::SetUpTestCase() {}

void ScreenSensorConnectorTest::TearDownTestCase() {}

void ScreenSensorConnectorTest::SetUp() {}

void ScreenSensorConnectorTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {

HWTEST_F(ScreenSensorConnectorTest, SubscribeRotationSensorTest001, TestSize.Level1)
{
    ScreenSensorConnector::SubscribeRotationSensor();
    ScreenSensorConnector::UnsubscribeRotationSensor();
}

HWTEST_F(ScreenSensorConnectorTest, SubscribeRotationSensorTest002, TestSize.Level1)
{
    ScreenSensorConnector::SubscribeRotationSensor();
    ScreenSensorConnector::SubscribeRotationSensor();
    ScreenSensorConnector::UnsubscribeRotationSensor();
}

HWTEST_F(ScreenSensorConnectorTest, UnsubscribeRotationSensorTest001, TestSize.Level1)
{
    ScreenSensorConnector::UnsubscribeRotationSensor();
}

HWTEST_F(ScreenSensorConnectorTest, SubscribeTentSensorTest001, TestSize.Level1)
{
    ScreenSensorConnector::SubscribeTentSensor();
    ScreenSensorConnector::UnsubscribeTentSensor();
}

HWTEST_F(ScreenSensorConnectorTest, UnsubscribeTentSensorTest001, TestSize.Level1)
{
    ScreenSensorConnector::UnsubscribeTentSensor();
}

#ifdef WM_SUBSCRIBE_MOTION_ENABLE
HWTEST_F(ScreenSensorConnectorTest, SmartRotationDisabledTest001, TestSize.Level1)
{
    system::SetIntParameter(SMART_ROTATION_PARAM, DISABLE_SMART_ROTATION);
    
    MotionSubscriber::isMotionSensorSubscribed_ = false;
    MotionSubscriber::SubscribeMotionSensor();
    
    EXPECT_TRUE(MotionSubscriber::isMotionSensorSubscribed_);
    
    MotionSubscriber::UnsubscribeMotionSensor();
    EXPECT_FALSE(MotionSubscriber::isMotionSensorSubscribed_);
}

HWTEST_F(ScreenSensorConnectorTest, SmartRotationEnabledTest001, TestSize.Level1)
{
    system::SetIntParameter(SMART_ROTATION_PARAM, ENABLE_SMART_ROTATION);
    
    MotionSubscriber::isMotionSensorSubscribed_ = false;
    MotionSubscriber::SubscribeMotionSensor();
    
    EXPECT_TRUE(MotionSubscriber::isMotionSensorSubscribed_);
    
    MotionSubscriber::UnsubscribeMotionSensor();
    EXPECT_FALSE(MotionSubscriber::isMotionSensorSubscribed_);
    
    system::SetIntParameter(SMART_ROTATION_PARAM, DISABLE_SMART_ROTATION);
}

HWTEST_F(ScreenSensorConnectorTest, SmartRotationSubscribeAlreadySubscribedTest001, TestSize.Level1)
{
    system::SetIntParameter(SMART_ROTATION_PARAM, ENABLE_SMART_ROTATION);
    
    MotionSubscriber::isMotionSensorSubscribed_ = true;
    MotionSubscriber::SubscribeMotionSensor();
    
    EXPECT_TRUE(MotionSubscriber::isMotionSensorSubscribed_);
    
    MotionSubscriber::UnsubscribeMotionSensor();
    system::SetIntParameter(SMART_ROTATION_PARAM, DISABLE_SMART_ROTATION);
}

HWTEST_F(ScreenSensorConnectorTest, SmartRotationUnsubscribeNotSubscribedTest001, TestSize.Level1)
{
    system::SetIntParameter(SMART_ROTATION_PARAM, ENABLE_SMART_ROTATION);
    
    MotionSubscriber::isMotionSensorSubscribed_ = false;
    MotionSubscriber::UnsubscribeMotionSensor();
    
    EXPECT_FALSE(MotionSubscriber::isMotionSensorSubscribed_);
    
    system::SetIntParameter(SMART_ROTATION_PARAM, DISABLE_SMART_ROTATION);
}
#endif

}

}
}