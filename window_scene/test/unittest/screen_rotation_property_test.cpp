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
#include <chrono>

#include <gtest/gtest.h>

#include "window_manager_hilog.h"
#include "screen_session_manager.h"

#include "session_manager/include/screen_rotation_property.h"

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
 * @tc.name: HandleSensorEventInput
 * @tc.desc: test function : HandleSensorEventInput
 * @tc.type: FUNC
 */
HWTEST_F(ScreenRotationPropertyTest, HandleSensorEventInput, Function | SmallTest | Level1)
{
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: HandleSensorEventInput start";
    ScreenRotationProperty::HandleSensorEventInput(DeviceRotation::INVALID);
    ScreenRotationProperty::HandleSensorEventInput(DeviceRotation::ROTATION_PORTRAIT);
    GTEST_LOG_(INFO) << "ScreenRotationPropertyTest: HandleSensorEventInput end";
}

}

}
}