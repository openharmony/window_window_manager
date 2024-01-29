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

#include <gtest/gtest.h>
#include <transaction/rs_marshalling_helper.h>

#include "screen_session_manager_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenSessionManagerClientStubTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    sptr<ScreenSessionManagerClientStub> screenSessionManagerClientStub_;
};

void ScreenSessionManagerClientStubTest::SetUp()
{
    screenSessionManagerClientStub_ = new ScreenSessionManagerClient();
}

void ScreenSessionManagerClientStubTest::TearDown()
{
    screenSessionManagerClientStub_ = nullptr;
}

/**
 * @tc.name: HandleOnScreenConnectionChanged
 * @tc.desc: HandleOnScreenConnectionChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientStubTest, HandleOnScreenConnectionChanged, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInterfaceToken(ScreenSessionManagerClientStub::GetDescriptor());
    ScreenId screenId = 0;
    data.WriteUint64(screenId);
    ScreenEvent screenEvent = ScreenEvent::CONNECTED;
    data.WriteUint8(static_cast<uint8_t>(screenEvent));
    ScreenId rsId = 0;
    data.WriteUint64(rsId);
    std::string name;
    data.WriteString(name);

    int ret = screenSessionManagerClientStub_->HandleOnScreenConnectionChanged(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: HandleOnPropertyChanged
 * @tc.desc: HandleOnPropertyChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientStubTest, HandleOnPropertyChanged, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInterfaceToken(ScreenSessionManagerClientStub::GetDescriptor());
    ScreenId screenId = 0;
    data.WriteUint64(screenId);
    ScreenProperty property;
    RSMarshallingHelper::Marshalling(data, property);
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::UNDEFINED;
    data.WriteUint32(static_cast<uint32_t>(reason));

    int ret = screenSessionManagerClientStub_->HandleOnPropertyChanged(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: HandleOnSensorRotationChanged
 * @tc.desc: HandleOnSensorRotationChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientStubTest, HandleOnSensorRotationChanged, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInterfaceToken(ScreenSessionManagerClientStub::GetDescriptor());
    ScreenId screenId = 0;
    data.WriteUint64(screenId);
    float sensorRotation = 0;
    data.WriteFloat(sensorRotation);

    int ret = screenSessionManagerClientStub_->HandleOnSensorRotationChanged(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: HandleOnScreenOrientationChanged
 * @tc.desc: HandleOnScreenOrientationChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientStubTest, HandleOnScreenOrientationChanged, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInterfaceToken(ScreenSessionManagerClientStub::GetDescriptor());
    ScreenId screenId = 0;
    data.WriteUint64(screenId);
    float screenOrientation = 0;
    data.WriteFloat(screenOrientation);

    int ret = screenSessionManagerClientStub_->HandleOnScreenOrientationChanged(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: HandleOnScreenRotationLockedChanged
 * @tc.desc: HandleOnScreenRotationLockedChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientStubTest, HandleOnScreenRotationLockedChanged, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInterfaceToken(ScreenSessionManagerClientStub::GetDescriptor());
    ScreenId screenId = 0;
    data.WriteUint64(screenId);
    bool isLocked = false;
    data.WriteBool(isLocked);

    int ret = screenSessionManagerClientStub_->HandleOnScreenRotationLockedChanged(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: HandleOnDisplayStateChanged
 * @tc.desc: HandleOnDisplayStateChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientStubTest, HandleOnDisplayStateChanged, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInterfaceToken(ScreenSessionManagerClientStub::GetDescriptor());
    ScreenId screenId = 0;
    data.WriteUint64(screenId);
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    data.WriteStrongParcelable(displayInfo);
    data.WriteUint32(0);
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;
    data.WriteUint32(static_cast<uint32_t>(type));

    int ret = screenSessionManagerClientStub_->HandleOnDisplayStateChanged(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: HandleOnScreenshot
 * @tc.desc: HandleOnScreenshot test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientStubTest, HandleOnScreenshot, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInterfaceToken(ScreenSessionManagerClientStub::GetDescriptor());
    ScreenId screenId = 0;
    data.WriteUint64(screenId);

    int ret = screenSessionManagerClientStub_->HandleOnScreenshot(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: HandleOnImmersiveStateChanged
 * @tc.desc: HandleOnImmersiveStateChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientStubTest, HandleOnImmersiveStateChanged, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInterfaceToken(ScreenSessionManagerClientStub::GetDescriptor());

    int ret = screenSessionManagerClientStub_->HandleOnImmersiveStateChanged(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: HandleOnPowerStatusChanged
 * @tc.desc: HandleOnPowerStatusChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientStubTest, HandleOnPowerStatusChanged, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInterfaceToken(ScreenSessionManagerClientStub::GetDescriptor());

    int ret = screenSessionManagerClientStub_->HandleOnPowerStatusChanged(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: HandleOnGetSurfaceNodeIdsFromMissionIdsChanged
 * @tc.desc: HandleOnGetSurfaceNodeIdsFromMissionIdsChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientStubTest, HandleOnGetSurfaceNodeIdsFromMissionIdsChanged, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInterfaceToken(ScreenSessionManagerClientStub::GetDescriptor());

    int ret = screenSessionManagerClientStub_->HandleOnGetSurfaceNodeIdsFromMissionIdsChanged(data, reply);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: HandleOnSetDisplayNodeScreenId
 * @tc.desc: HandleOnSetDisplayNodeScreenId test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientStubTest, HandleOnSetDisplayNodeScreenId, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInterfaceToken(ScreenSessionManagerClientStub::GetDescriptor());

    int ret = screenSessionManagerClientStub_->HandleOnSetDisplayNodeScreenId(data, reply);
    EXPECT_EQ(ret, 0);
}
} // namespace Rosen
} // namespace OHOS
