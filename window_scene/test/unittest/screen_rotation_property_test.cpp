/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "session_manager/include/screen_rotation_property.h"
#include <gtest/gtest.h>

#include "window_manager_hilog.h"
#include "screen_session_manager.h"
#include <chrono>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenRotationPropertyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void ScreenRotationPropertyTest::SetUpTestCase()
{
}

void ScreenRotationPropertyTest::TearDownTestCase()
{
}

void ScreenRotationPropertyTest::SetUp()
{
}

void ScreenRotationPropertyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Init
 * @tc.desc: test function : Init
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, Init, Function | SmallTest | Level1)
{
    int res = 0;
    ScreenRotationProperty::Init();
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: GetDefaultDisplayId
 * @tc.desc: test function : GetDefaultDisplayId
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, GetDefaultDisplayId, Function | SmallTest | Level1)
{
    ScreenRotationProperty::Init();
    int res = 0;
    ScreenRotationProperty::GetDefaultDisplayId();
    ASSERT_EQ(0, res);
}

/**
 * @tc.name: IsScreenRotationLocked
 * @tc.desc: test function : IsScreenRotationLocked
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, IsScreenRotationLocked, Function | SmallTest | Level1)
{
    ScreenRotationProperty::Init();
    ScreenRotationProperty::SetScreenRotationLocked(true);
    bool isLocked = ScreenRotationProperty::IsScreenRotationLocked();
    ASSERT_EQ(true, isLocked);
}

/**
 * @tc.name: SetScreenRotationLocked
 * @tc.desc: test function : SetScreenRotationLocked
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, SetScreenRotationLocked, Function | SmallTest | Level1)
{
    ScreenRotationProperty::Init();
    DMError ret = ScreenRotationProperty::SetScreenRotationLocked(true);
    ASSERT_EQ(DMError::DM_OK, ret);
    ret = ScreenRotationProperty::SetScreenRotationLocked(false);
    ASSERT_EQ(DMError::DM_OK, ret);

}

/**
 * @tc.name: SetDefaultDeviceRotationOffset
 * @tc.desc: test function : SetDefaultDeviceRotationOffset
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, SetDefaultDeviceRotationOffset, Function | SmallTest | Level1)
{
    ScreenRotationProperty::Init();
    uint32_t offset = 0;
    ScreenRotationProperty::SetDefaultDeviceRotationOffset(offset);
}

/**
 * @tc.name: GetPreferredOrientation
 * @tc.desc: test function : GetPreferredOrientation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, GetPreferredOrientation, Function | SmallTest | Level1)
{
    ScreenRotationProperty::Init();
    Orientation res = ScreenRotationProperty::GetPreferredOrientation();
    ASSERT_EQ(Orientation::SENSOR, res);
}

/**
 * @tc.name: SetScreenRotation
 * @tc.desc: test function : SetScreenRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, SetScreenRotation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: SetScreenRotation start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::SetScreenRotation(Rotation::ROTATION_0);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: SetScreenRotation end";
}

/**
 * @tc.name: IsSensorRelatedOrientation
 * @tc.desc: test function : IsSensorRelatedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, IsSensorRelatedOrientation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsSensorRelatedOrientation start";
    ScreenRotationProperty::Init();
    bool res = ScreenRotationProperty::IsSensorRelatedOrientation(Orientation::VERTICAL);
    ASSERT_EQ(false, res);
    res = ScreenRotationProperty::IsSensorRelatedOrientation(Orientation::SENSOR_VERTICAL);
    ASSERT_EQ(true, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsSensorRelatedOrientation end";
}

/**
 * @tc.name: ProcessSwitchToAutoRotation
 * @tc.desc: test function : ProcessSwitchToAutoRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessSwitchToAutoRotation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToAutoRotation start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::ProcessSwitchToAutoRotation(DeviceRotation::ROTATION_PORTRAIT);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToAutoRotation end";
}

/**
 * @tc.name: ProcessSwitchToAutoRotation
 * @tc.desc: test function : ProcessSwitchToAutoRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessSwitchToAutoRotationPortrait, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToAutoRotationPortrait start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::ProcessSwitchToAutoRotationPortrait(DeviceRotation::ROTATION_PORTRAIT);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToAutoRotationPortrait end";
}

/**
 * @tc.name: ProcessSwitchToAutoRotation
 * @tc.desc: test function : ProcessSwitchToAutoRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessSwitchToAutoRotationLandscape, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToAutoRotationLandscape start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::ProcessSwitchToAutoRotationLandscape(DeviceRotation::ROTATION_PORTRAIT);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToAutoRotationLandscape end";
}

/**
 * @tc.name: ProcessSwitchToAutoRotation
 * @tc.desc: test function : ProcessSwitchToAutoRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessSwitchToAutoRotationPortraitRestricted, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToAutoRotationPortraitRestricted start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::ProcessSwitchToAutoRotationPortraitRestricted();
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToAutoRotationPortraitRestricted end";
}

/**
 * @tc.name: ConvertSensorToDeviceRotation
 * @tc.desc: test function : ConvertSensorToDeviceRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ConvertSensorToDeviceRotation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ConvertSensorToDeviceRotation start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::ConvertSensorToDeviceRotation(SensorRotation::ROTATION_90);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ConvertSensorToDeviceRotation end";
}

}

}
}