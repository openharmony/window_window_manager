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
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class ScreenSessionManagerClientProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObjectMocker> iRemoteObjectMocker_;
    sptr<ScreenSessionManagerClientProxy> screenSessionManagerClientProxy_;
};

void ScreenSessionManagerClientProxyTest::SetUpTestCase()
{
}

void ScreenSessionManagerClientProxyTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

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
    screenSessionManagerClientProxy_->OnScreenConnectionChanged(screenId, screenEvent, rsId, name, false);
}

/**
 * @tc.name: SwitchUserCallback
 * @tc.desc: SwitchUserCallback test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, SwitchUserCallback, Function | SmallTest | Level2)
{
    std::vector<int32_t> oldScbPids {0, 1};
    int32_t currentScbPid = 0;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->SwitchUserCallback(oldScbPids, currentScbPid);
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
 * @tc.name: OnPowerStatusChanged
 * @tc.desc: OnPowerStatusChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, OnPowerStatusChanged, Function | SmallTest | Level2)
{
    DisplayPowerEvent event = DisplayPowerEvent::DISPLAY_ON;
    EventStatus status = EventStatus::BEGIN;
    PowerStateChangeReason reason = PowerStateChangeReason::POWER_BUTTON;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->OnPowerStatusChanged(event, status, reason);
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
 * @tc.name: OnGetSurfaceNodeIdsFromMissionIdsChanged
 * @tc.desc: OnGetSurfaceNodeIdsFromMissionIdsChanged test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, OnGetSurfaceNodeIdsFromMissionIdsChanged, Function | SmallTest | Level2)
{
    std::vector<uint64_t> missionIds = {0, 1};
    std::vector<uint64_t> surfaceNodeIds;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->OnGetSurfaceNodeIdsFromMissionIdsChanged(missionIds, surfaceNodeIds);
}

/**
 * @tc.name: OnUpdateFoldDisplayMode
 * @tc.desc: OnUpdateFoldDisplayMode test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, OnUpdateFoldDisplayMode, Function | SmallTest | Level2)
{
    FoldDisplayMode displayMode = FoldDisplayMode::MAIN;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->OnUpdateFoldDisplayMode(displayMode);
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

/**
 * @tc.name: SetDisplayNodeScreenId
 * @tc.desc: SetDisplayNodeScreenId test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, SetDisplayNodeScreenId, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    ScreenId displayNodeScreenId = 0;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->SetDisplayNodeScreenId(screenId, displayNodeScreenId);
}

/**
 * @tc.name: SetVirtualPixelRatioSystem
 * @tc.desc: SetVirtualPixelRatioSystem test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerClientProxyTest, SetVirtualPixelRatioSystem, Function | SmallTest | Level2)
{
    ScreenId screenId = 0;
    float virtualPixelRatio = 1.0f;

    ASSERT_TRUE(screenSessionManagerClientProxy_ != nullptr);
    screenSessionManagerClientProxy_->SetVirtualPixelRatioSystem(screenId, virtualPixelRatio);
}
} // namespace Rosen
} // namespace OHOS
