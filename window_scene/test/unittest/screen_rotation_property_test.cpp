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
    bool isLocked = true;
    DMError ret = ScreenRotationProperty::SetScreenRotationLocked(isLocked);
    ASSERT_EQ(DMError::DM_OK, ret);
    isLocked = false;
    ScreenRotationProperty::GetCurrentDisplayRotation();
    ScreenRotationProperty::ConvertDeviceToDisplayRotation(DeviceRotation::INVALID);
    ret = ScreenRotationProperty::SetScreenRotationLocked(isLocked);
    ASSERT_EQ(DMError::DM_OK, ret);
    ScreenRotationProperty::ConvertDeviceToDisplayRotation(DeviceRotation::ROTATION_LANDSCAPE);
    Orientation currentOrientation = ScreenRotationProperty::GetPreferredOrientation();
    ScreenRotationProperty::IsSensorRelatedOrientation(currentOrientation);
    ret = ScreenRotationProperty::SetScreenRotationLocked(isLocked);
    ScreenRotationProperty::IsSensorRelatedOrientation(Orientation::UNSPECIFIED);
    ret = ScreenRotationProperty::SetScreenRotationLocked(isLocked);
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
    uint32_t offset = -1;
    ScreenRotationProperty::SetDefaultDeviceRotationOffset(offset);
    offset = 0;
    ScreenRotationProperty::SetDefaultDeviceRotationOffset(offset);
    ASSERT_EQ(offset, ScreenRotationProperty::defaultDeviceRotationOffset_);
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
    Rotation targetRotation = ScreenRotationProperty::GetCurrentDisplayRotation();
    ScreenRotationProperty::SetScreenRotation(targetRotation);
    targetRotation = Rotation::ROTATION_90;
    ScreenRotationProperty::SetScreenRotation(targetRotation);
    ASSERT_EQ(false, ScreenSessionManager::GetInstance().SetRotationFromWindow(targetRotation));
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
    DeviceRotation deviceRotation = DeviceRotation::INVALID;
    Rotation rotation  = ScreenRotationProperty::ConvertDeviceToDisplayRotation(deviceRotation);
    ASSERT_EQ(ScreenRotationProperty::GetCurrentDisplayRotation(), rotation);
    deviceRotation = DeviceRotation::ROTATION_PORTRAIT;
    rotation  = ScreenRotationProperty::ConvertDeviceToDisplayRotation(deviceRotation);
    ASSERT_EQ(ScreenRotationProperty::deviceToDisplayRotationMap_.at(deviceRotation), rotation);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ConvertDeviceToDisplayRotation end";
}

/**
 * @tc.name: ProcessRotationMapping
 * @tc.desc: test function : ProcessRotationMapping
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessRotationMapping, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessRotationMapping_PORTRAIT start";
    ScreenRotationProperty::defaultDeviceRotation_ = 0;
    ScreenRotationProperty::ProcessRotationMapping();
    ASSERT_EQ(Rotation::ROTATION_0,
        ScreenRotationProperty::deviceToDisplayRotationMap_.at(DeviceRotation::ROTATION_PORTRAIT));
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessRotationMapping_PORTRAIT end";
}

/**
 * @tc.name: ProcessSwitchToSensorRelatedOrientation
 * @tc.desc: test function : ProcessSwitchToSensorRelatedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, ProcessSwitchToSensorRelatedOrientation, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToSensorRelatedOrientation start";
    ScreenRotationProperty::Init();
    Orientation orientation = Orientation::AUTO_ROTATION_RESTRICTED;
    DeviceRotation sensorRotationConverted = DeviceRotation::INVALID;
    ScreenRotationProperty::isScreenRotationLocked_ = true;
    ScreenRotationProperty::ProcessSwitchToSensorRelatedOrientation(orientation, sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::GetCurrentDisplayRotation(), ScreenRotationProperty::rotationLockedRotation_);
    orientation = Orientation::SENSOR;
    sensorRotationConverted = DeviceRotation::ROTATION_PORTRAIT;
    ScreenRotationProperty::ProcessSwitchToSensorRelatedOrientation(orientation, sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::deviceToDisplayRotationMap_.at(sensorRotationConverted),
        ScreenRotationProperty::ConvertDeviceToDisplayRotation(sensorRotationConverted));
    orientation = Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED;
    ScreenRotationProperty::ProcessSwitchToSensorRelatedOrientation(orientation, sensorRotationConverted);
    orientation = Orientation::SENSOR_VERTICAL;
    ScreenRotationProperty::ProcessSwitchToSensorRelatedOrientation(orientation, sensorRotationConverted);
    orientation = Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED;
    ScreenRotationProperty::ProcessSwitchToSensorRelatedOrientation(orientation, sensorRotationConverted);
    orientation = Orientation::SENSOR_HORIZONTAL;
    ScreenRotationProperty::ProcessSwitchToSensorRelatedOrientation(orientation, sensorRotationConverted);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: ProcessSwitchToSensorRelatedOrientation end";
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
    DeviceRotation deviceRotation = DeviceRotation::INVALID;
    ScreenRotationProperty::HandleSensorEventInput(deviceRotation);
    deviceRotation = DeviceRotation::ROTATION_PORTRAIT;
    ScreenSessionManager *screenSessionManager = new ScreenSessionManager;
    auto defaultDisplayInfo = screenSessionManager->GetDefaultDisplayInfo();
    ScreenRotationProperty::HandleSensorEventInput(deviceRotation);
    Orientation orientation = Orientation::BEGIN;
    ScreenRotationProperty::IsSensorRelatedOrientation(orientation);
    ScreenRotationProperty::HandleSensorEventInput(deviceRotation);
    orientation = Orientation::SENSOR;
    ScreenRotationProperty::GetCurrentDisplayRotation();
    ScreenRotationProperty::ConvertDeviceToDisplayRotation(deviceRotation);
    ScreenRotationProperty::HandleSensorEventInput(deviceRotation);
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
    ASSERT_EQ(ScreenRotationProperty::GetCurrentDisplayRotation(), result);
    sensorRotationConverted = DeviceRotation::ROTATION_PORTRAIT;
    result = ScreenRotationProperty::ProcessAutoRotationPortraitOrientation(sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::ConvertDeviceToDisplayRotation(sensorRotationConverted), result);
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
    DeviceRotation sensorRotationConverted = DeviceRotation::ROTATION_PORTRAIT;
    auto result = ScreenRotationProperty::ProcessAutoRotationPortraitOrientation(sensorRotationConverted);
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
    sensorRotation = SensorRotation::ROTATION_0;
    ScreenRotationProperty::defaultDeviceRotationOffset_ = 90;
    ScreenRotationProperty::defaultDeviceRotation_ = 1;
    result = ScreenRotationProperty::CalcDeviceRotation(sensorRotation);
    ASSERT_EQ(DeviceRotation::ROTATION_PORTRAIT_INVERTED, result);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: CalcDeviceRotation end";
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
    Orientation requestedOrientation = Orientation::SENSOR;
    DeviceRotation sensorRotationConverted = DeviceRotation::ROTATION_LANDSCAPE;
    auto result = ScreenRotationProperty::CalcTargetDisplayRotation(requestedOrientation, sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::ConvertDeviceToDisplayRotation(sensorRotationConverted), result);
    requestedOrientation = Orientation::SENSOR_VERTICAL;
    result = ScreenRotationProperty::CalcTargetDisplayRotation(requestedOrientation, sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::ProcessAutoRotationPortraitOrientation(sensorRotationConverted), result);
    requestedOrientation = Orientation::SENSOR_HORIZONTAL;
    result = ScreenRotationProperty::CalcTargetDisplayRotation(requestedOrientation, sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::ProcessAutoRotationLandscapeOrientation(sensorRotationConverted), result);
    requestedOrientation = Orientation::AUTO_ROTATION_RESTRICTED;
    ScreenRotationProperty::isScreenRotationLocked_ = true;
    result = ScreenRotationProperty::CalcTargetDisplayRotation(requestedOrientation, sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::GetCurrentDisplayRotation(), result);
    ScreenRotationProperty::isScreenRotationLocked_ = false;
    result = ScreenRotationProperty::CalcTargetDisplayRotation(requestedOrientation, sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::ConvertDeviceToDisplayRotation(sensorRotationConverted), result);
    requestedOrientation = Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED;
    ScreenRotationProperty::isScreenRotationLocked_ = true;
    result = ScreenRotationProperty::CalcTargetDisplayRotation(requestedOrientation, sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::GetCurrentDisplayRotation(), result);
    ScreenRotationProperty::isScreenRotationLocked_ = false;
    result = ScreenRotationProperty::CalcTargetDisplayRotation(requestedOrientation, sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::ProcessAutoRotationPortraitOrientation(sensorRotationConverted), result);
    requestedOrientation = Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED;
    ScreenRotationProperty::isScreenRotationLocked_ = true;
    result = ScreenRotationProperty::CalcTargetDisplayRotation(requestedOrientation, sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::GetCurrentDisplayRotation(), result);
    ScreenRotationProperty::isScreenRotationLocked_ = false;
    result = ScreenRotationProperty::CalcTargetDisplayRotation(requestedOrientation, sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::ProcessAutoRotationLandscapeOrientation(sensorRotationConverted), result);
    requestedOrientation = Orientation::LOCKED;
    result = ScreenRotationProperty::CalcTargetDisplayRotation(requestedOrientation, sensorRotationConverted);
    ASSERT_EQ(ScreenRotationProperty::GetCurrentDisplayRotation(), result);
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
    ASSERT_EQ(true, result);
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
    ASSERT_EQ(false, result);
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
    Rotation rotation = Rotation::ROTATION_90;
    auto result = ScreenRotationProperty::IsDisplayRotationHorizontal(rotation);
    ASSERT_EQ(true, result);
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