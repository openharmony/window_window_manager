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

#include "iremote_object_mocker.h"
#include "zidl/screen_session_manager_client_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenSessionManagerClientProxyTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObjectMocker> iRemoteObjectMocker_;
    sptr<ScreenSessionManagerClientProxy> screenSessionManagerClientProxy_;
};

void ScreenSessionManagerClientProxyTest::SetUp()
{
    iRemoteObjectMocker_ = new IRemoteObjectMocker();
    screenSessionManagerClientProxy_ = new ScreenSessionManagerClientProxy(iRemoteObjectMocker_);
}

void ScreenSessionManagerClientProxyTest::TearDown()
{
    screenSessionManagerClientProxy_ = nullptr;
}

/**
 * @tc.name: OnScreenConnectionChanged
 * @tc.desc: OnScreenConnectionChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, OnScreenConnectionChanged, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    ScreenEvent screenEvent = ScreenEvent::CONNECTED;
    ScreenId rsId = 0;
    std::string name;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->OnScreenConnectionChanged(screenId, screenEvent, rsId, name);
}

/**
 * @tc.name: OnPropertyChanged
 * @tc.desc: OnPropertyChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, OnPropertyChanged, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    ScreenProperty property;
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::UNDEFINED;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->OnPropertyChanged(screenId, property, reason);
}

/**
 * @tc.name: OnSensorRotationChanged
 * @tc.desc: OnSensorRotationChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, OnSensorRotationChanged, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    float sensorRotation = 0;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->OnSensorRotationChanged(screenId, sensorRotation);
}

/**
 * @tc.name: OnScreenOrientationChanged
 * @tc.desc: OnScreenOrientationChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, OnScreenOrientationChanged, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    float screenOrientation = 0;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->OnScreenOrientationChanged(screenId, screenOrientation);
}

/**
 * @tc.name: OnScreenRotationLockedChanged
 * @tc.desc: OnScreenRotationLockedChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, OnScreenRotationLockedChanged, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    bool isLocked = false;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->OnScreenRotationLockedChanged(screenId, isLocked);
}

/**
 * @tc.name: OnDisplayStateChanged
 * @tc.desc: OnDisplayStateChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, OnDisplayStateChanged, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap {
        { displayInfo->GetDisplayId(), displayInfo },
    };
    DisplayStateChangeType type = DisplayStateChangeType::BEFORE_SUSPEND;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->OnDisplayStateChanged(screenId, displayInfo, displayInfoMap, type);
}

/**
 * @tc.name: OnScreenshot
 * @tc.desc: OnScreenshot test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, OnScreenshot, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->OnScreenshot(screenId);
}

/**
 * @tc.name: OnImmersiveStateChanged
 * @tc.desc: OnImmersiveStateChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, OnImmersiveStateChanged, Function | SmallTest | Level2)
{
    bool immersive = false;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->OnImmersiveStateChanged(immersive);
}
} // namespace Rosen
} // namespace OHOS
