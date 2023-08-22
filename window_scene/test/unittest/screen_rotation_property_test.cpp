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

/**
 * @tc.name: ProcessSwitchToAutoRotationLandscapeRestricted
 * @tc.desc: test function : ProcessSwitchToAutoRotationLandscapeRestricted
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessSwitchToAutoRotationLandscapeRestricted, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToAutoRotationLandscapeRestricted start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::ProcessSwitchToAutoRotationLandscapeRestricted();
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToAutoRotationLandscapeRestricted end";
}

/**
 * @tc.name: ConvertRotationToDisplayOrientation
 * @tc.desc: test function : ConvertRotationToDisplayOrientation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ConvertRotationToDisplayOrientation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ConvertRotationToDisplayOrientation start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::ConvertRotationToDisplayOrientation(Rotation::ROTATION_90);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ConvertSensorToDeviceRotation end";
}

/**
 * @tc.name: ConvertDeviceToDisplayRotation
 * @tc.desc: test function : ConvertDeviceToDisplayRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ConvertDeviceToDisplayRotation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ConvertDeviceToDisplayRotation start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_PORTRAIT);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ConvertDeviceToDisplayRotation end";
}

/**
 * @tc.name: ProcessRotationMapping
 * @tc.desc: test function : ProcessRotationMapping
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessRotationMapping, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessRotationMapping start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::ProcessRotationMapping();
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessRotationMapping end";
}

HWTEST_F(ScreenRotationPropertyTest, ProcessSwitchToSensorRelatedOrientation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToSensorRelatedOrientation start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::ProcessSwitchToSensorRelatedOrientation(Orientation::BEGIN, DeviceRotation::ROTATION_PORTRAIT);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToSensorRelatedOrientation end";
}

/**
 * @tc.name: GetCurrentDisplayRotation
 * @tc.desc: test function : GetCurrentDisplayRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, GetCurrentDisplayRotation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: GetCurrentDisplayRotation start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::GetCurrentDisplayRotation();
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: GetCurrentDisplayRotation end";
}

/**
 * @tc.name: HandleSensorEventInput
 * @tc.desc: test function : HandleSensorEventInput
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, HandleSensorEventInput, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: HandleSensorEventInput start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::HandleSensorEventInput(DeviceRotation::INVALID);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: HandleSensorEventInput end";
}

/**
 * @tc.name: ProcessSwitchToSensorUnrelatedOrientation
 * @tc.desc: test function : ProcessSwitchToSensorUnrelatedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessSwitchToSensorUnrelatedOrientation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToSensorUnrelatedOrientation start";
    ScreenRotationProperty::Init();
    int32_t res = 0;
    ScreenRotationProperty::ProcessSwitchToSensorUnrelatedOrientation(ScreenRotationProperty::lastOrientationType_);
    ASSERT_EQ(0, res);
    ScreenRotationProperty::ProcessSwitchToSensorUnrelatedOrientation(Orientation::UNSPECIFIED);
    ASSERT_EQ(0, res);
    ScreenRotationProperty::ProcessSwitchToSensorUnrelatedOrientation(Orientation::VERTICAL);
    ASSERT_EQ(0, res);
    ScreenRotationProperty::ProcessSwitchToSensorUnrelatedOrientation(Orientation::REVERSE_VERTICAL);
    ASSERT_EQ(0, res);
    ScreenRotationProperty::ProcessSwitchToSensorUnrelatedOrientation(Orientation::HORIZONTAL);
    ASSERT_EQ(0, res);
    ScreenRotationProperty::ProcessSwitchToSensorUnrelatedOrientation(Orientation::REVERSE_HORIZONTAL);
    ASSERT_EQ(0, res);
    ScreenRotationProperty::ProcessSwitchToSensorUnrelatedOrientation(Orientation::BEGIN);
    ASSERT_EQ(0, res);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToSensorUnrelatedOrientation end";
}

/**
 * @tc.name: ProcessAutoRotationPortraitOrientation
 * @tc.desc: ProcessAutoRotationPortraitOrientation func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessAutoRotationPortraitOrientation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessAutoRotationPortraitOrientation start";
    ScreenRotationProperty::Init();
    DeviceRotation sensorRotationConverted = DeviceRotation::ROTATION_LANDSCAPE;
    auto result = ScreenRotationProperty::ProcessAutoRotationPortraitOrientation(sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::currentDisplayRotation_, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessAutoRotationPortraitOrientation end";
}

/**
 * @tc.name: ProcessAutoRotationLandscapeOrientation
 * @tc.desc: ProcessAutoRotationLandscapeOrientation func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessAutoRotationLandscapeOrientation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessAutoRotationLandscapeOrientation start";
    ScreenRotationProperty::Init();
    DeviceRotation sensorRotationConverted = DeviceRotation::ROTATION_LANDSCAPE;
    auto result = ScreenRotationProperty::ProcessAutoRotationLandscapeOrientation(sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::currentDisplayRotation_, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessAutoRotationLandscapeOrientation end";
}

/**
 * @tc.name: CalcDeviceRotation
 * @tc.desc: CalcDeviceRotation func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, CalcDeviceRotation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: CalcDeviceRotation start";
    ScreenRotationProperty::Init();
    SensorRotation sensorRotation = SensorRotation::INVALID;
    auto result = ScreenRotationProperty::CalcDeviceRotation(sensorRotation);
    ASSERT_EQ(DeviceRotation::INVALID, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: CalcTargetDisplayRotation end";
}

/**
 * @tc.name: CalcTargetDisplayRotation
 * @tc.desc: CalcTargetDisplayRotation func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, CalcTargetDisplayRotation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: CalcTargetDisplayRotation start";
    ScreenRotationProperty::Init();
    Orientation requestedOrientation = Orientation::VERTICAL;
    DeviceRotation sensorRotationConverted = DeviceRotation::ROTATION_LANDSCAPE;
    auto result = ScreenRotationProperty::CalcTargetDisplayRotation(requestedOrientation,sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::lastSensorDecidedRotation_, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: CalcTargetDisplayRotation end";
}

/**
 * @tc.name: IsCurrentDisplayVertical
 * @tc.desc: IsCurrentDisplayVertical func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, IsCurrentDisplayVertical, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsCurrentDisplayVertical start";
    ScreenRotationProperty::Init();
    auto result = ScreenRotationProperty::IsCurrentDisplayVertical();
    ASSERT_EQ(false, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsCurrentDisplayVertical end";
}

/**
 * @tc.name: IsDeviceRotationVertical
 * @tc.desc: IsDeviceRotationVertical func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, IsDeviceRotationVertical, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsDeviceRotationVertical start";
    ScreenRotationProperty::Init();
    DeviceRotation deviceRotation = DeviceRotation::ROTATION_PORTRAIT;
    auto result = ScreenRotationProperty::IsDeviceRotationVertical(deviceRotation);
    ASSERT_EQ(true, result);
    deviceRotation = DeviceRotation::ROTATION_PORTRAIT_INVERTED;
    result = ScreenRotationProperty::IsDeviceRotationVertical(deviceRotation);
    ASSERT_EQ(true, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsDeviceRotationVertical end";
}

/**
 * @tc.name: IsDeviceRotationHorizontal
 * @tc.desc: IsDeviceRotationHorizontal func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, IsDeviceRotationHorizontal, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsDeviceRotationHorizontal start";
    ScreenRotationProperty::Init();
    DeviceRotation deviceRotation = DeviceRotation::ROTATION_LANDSCAPE;
    auto result = ScreenRotationProperty::IsDeviceRotationHorizontal(deviceRotation);
    ASSERT_EQ(true, result);
    deviceRotation = DeviceRotation::ROTATION_LANDSCAPE_INVERTED;
    result = ScreenRotationProperty::IsDeviceRotationHorizontal(deviceRotation);
    ASSERT_EQ(true, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsDeviceRotationHorizontal end";
}

/**
 * @tc.name: IsCurrentDisplayHorizontal
 * @tc.desc: IsCurrentDisplayHorizontal func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, IsCurrentDisplayHorizontal, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsCurrentDisplayHorizontal start";
    ScreenRotationProperty::Init();
    auto result = ScreenRotationProperty::IsCurrentDisplayHorizontal();
    ASSERT_EQ(true, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsCurrentDisplayHorizontal end";
}

/**
 * @tc.name: IsDefaultDisplayRotationPortrait
 * @tc.desc: IsDefaultDisplayRotationPortrait func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, IsDefaultDisplayRotationPortrait, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsDefaultDisplayRotationPortrait start";
    ScreenRotationProperty::Init();
    auto result = ScreenRotationProperty::IsDefaultDisplayRotationPortrait();
    ASSERT_EQ(true, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsDefaultDisplayRotationPortrait end";
}

/**
 * @tc.name: IsDisplayRotationVertical
 * @tc.desc: IsDisplayRotationVertical func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, IsDisplayRotationVertical, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsDisplayRotationVertical start";
    ScreenRotationProperty::Init();
    Rotation rotation = Rotation::ROTATION_0;
    auto result = ScreenRotationProperty::IsDisplayRotationVertical(rotation);
    ASSERT_EQ(true, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsDisplayRotationVertical end";
}

/**
 * @tc.name: IsDisplayRotationHorizontal
 * @tc.desc: IsDisplayRotationHorizontal func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, IsDisplayRotationHorizontal, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsDisplayRotationHorizontal start";
    ScreenRotationProperty::Init();
    Rotation rotation = Rotation::ROTATION_0;
    auto result = ScreenRotationProperty::IsDisplayRotationHorizontal(rotation);
    ASSERT_EQ(false, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: IsDisplayRotationHorizontal end";
}

/**
 * @tc.name: ProcessOrientationSwitch
 * @tc.desc: ProcessOrientationSwitch func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessOrientationSwitch, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessOrientationSwitch start";
    ScreenRotationProperty::Init();
    Orientation orientation = Orientation::BEGIN;
    int res = 0;
    ScreenRotationProperty::ProcessOrientationSwitch(orientation);
    ASSERT_EQ(res, 0);
    orientation = Orientation::SENSOR;
    ScreenRotationProperty::ProcessOrientationSwitch(orientation);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessOrientationSwitch start";
}

}

}
}