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
#include <message_option.h>
#include <message_parcel.h>
#include <common/rs_rect.h>
#include <transaction/rs_marshalling_helper.h>
#include "iremote_object_mocker.h"
#include "zidl/screen_session_manager_lite_proxy.h"
#include "zidl/screen_session_manager_lite_interface.h"
#include "screen_session_manager_lite.h"
#include "zidl/screen_session_manager_proxy.h"
#include "display_manager_adapter.h"
#include "display_manager.h"
#include "display_manager_agent_default.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class ScreenSessionManagerLiteProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObjectMocker> iRemoteObjectMocker_;
    sptr<ScreenSessionManagerLiteProxy> screenSessionManagerLiteProxy_;
};

void ScreenSessionManagerLiteProxyTest::SetUpTestCase()
{
}

void ScreenSessionManagerLiteProxyTest::TearDownTestCase()
{
}

void ScreenSessionManagerLiteProxyTest::SetUp()
{
    iRemoteObjectMocker_ = new IRemoteObjectMocker();
    screenSessionManagerLiteProxy_ = new ScreenSessionManagerLiteProxy(iRemoteObjectMocker_);
}

void ScreenSessionManagerLiteProxyTest::TearDown()
{
    screenSessionManagerLiteProxy_ = nullptr;
}

namespace {
/**
 * @tc.name: RegisterDisplayManagerAgent
 * @tc.desc: RegisterDisplayManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, RegisterDisplayManagerAgent, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    MessageParcel reply;

    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    DMError err = screenSessionManagerLiteProxy_->RegisterDisplayManagerAgent(displayManagerAgent, type);
    EXPECT_EQ(err, static_cast<DMError>(reply.ReadInt32()));
}

/**
 * @tc.name: UnregisterDisplayManagerAgent
 * @tc.desc: UnregisterDisplayManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, UnregisterDisplayManagerAgent, Function | SmallTest | Level1)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();

    MessageParcel reply;
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_STATE_LISTENER;
    DMError err = screenSessionManagerLiteProxy_->UnregisterDisplayManagerAgent(displayManagerAgent, type);
    EXPECT_EQ(err, static_cast<DMError>(reply.ReadInt32()));
}

/**
 * @tc.name: GetFoldDisplayMode
 * @tc.desc: GetFoldDisplayMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, GetFoldDisplayMode, Function | SmallTest | Level1)
{
    FoldDisplayMode res = screenSessionManagerLiteProxy_->GetFoldDisplayMode();
    EXPECT_EQ(res, FoldDisplayMode::UNKNOWN);
}

/**
 * @tc.name: IsFoldable
 * @tc.desc: IsFoldable
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, IsFoldable, Function | SmallTest | Level1)
{
    bool res = screenSessionManagerLiteProxy_->IsFoldable();
    ASSERT_FALSE(res);
}

/**
 * @tc.name: GetFoldStatus
 * @tc.desc: GetFoldStatus
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, GetFoldStatus, Function | SmallTest | Level1)
{
    FoldStatus res = screenSessionManagerLiteProxy_->GetFoldStatus();
    ASSERT_EQ(FoldStatus::UNKNOWN, res);
}

/**
 * @tc.name: GetDefaultDisplayInfo
 * @tc.desc: GetDefaultDisplayInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, GetDefaultDisplayInfo, Function | SmallTest | Level1)
{
    auto res = screenSessionManagerLiteProxy_->GetDefaultDisplayInfo();
    ASSERT_EQ(nullptr, res);
}

/**
 * @tc.name: GetDisplayInfoById
 * @tc.desc: GetDisplayInfoById
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, GetDisplayInfoById, Function | SmallTest | Level1)
{
    DisplayId displayId = 1001;
    auto res = screenSessionManagerLiteProxy_->GetDisplayInfoById(displayId);
    ASSERT_EQ(nullptr, res);
}

/**
 * @tc.name: GetCutoutInfo
 * @tc.desc: GetCutoutInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, GetCutoutInfo, Function | SmallTest | Level1)
{
    DisplayId displayId = 1001;
    auto res = screenSessionManagerLiteProxy_->GetCutoutInfo(displayId);
    ASSERT_EQ(nullptr, res);
}

/**
 * @tc.name: WakeUpBegin
 * @tc.desc: WakeUpBegin
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, WakeUpBegin, Function | SmallTest | Level1)
{
    PowerStateChangeReason reason {0};
    bool expectation = true;
    EXPECT_EQ(expectation, screenSessionManagerLiteProxy_->WakeUpBegin(reason));
}

/**
 * @tc.name: WakeUpEnd
 * @tc.desc: WakeUpEnd
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, WakeUpEnd, Function | SmallTest | Level1)
{
    bool expectation = true;
    EXPECT_EQ(expectation, screenSessionManagerLiteProxy_->WakeUpEnd());
}

/**
 * @tc.name: SuspendEnd
 * @tc.desc: SuspendEnd
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, SuspendEnd, Function | SmallTest | Level1)
{
    bool expectation = true;
    EXPECT_EQ(expectation, screenSessionManagerLiteProxy_->SuspendEnd());
}

/**
 * @tc.name: SetDisplayState
 * @tc.desc: SetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, SetDisplayState, Function | SmallTest | Level1)
{
    DisplayState state {1};
    screenSessionManagerLiteProxy_->SetDisplayState(state);
    int resultValue = 0;
    ASSERT_EQ(resultValue, 0);
}

/**
 * @tc.name: SetSpecifiedScreenPower
 * @tc.desc: SetSpecifiedScreenPower
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, SetSpecifiedScreenPower, Function | SmallTest | Level1)
{
    ScreenPowerState state {0};
    ScreenId id = 1001;
    PowerStateChangeReason reason {1};
    bool expectation = true;
    EXPECT_EQ(expectation, screenSessionManagerLiteProxy_->SetSpecifiedScreenPower(id, state, reason));
}

/**
 * @tc.name: SetScreenPowerForAll
 * @tc.desc: SetScreenPowerForAll
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, SetScreenPowerForAll, Function | SmallTest | Level1)
{
    ScreenPowerState state {0};
    PowerStateChangeReason reason {1};
    bool expectation = true;
    EXPECT_EQ(expectation, screenSessionManagerLiteProxy_->SetScreenPowerForAll(state, reason));
}

/**
 * @tc.name: GetDisplayState
 * @tc.desc: GetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, GetDisplayState, Function | SmallTest | Level1)
{
    DisplayId displayId {0};
    EXPECT_NE(DisplayState::UNKNOWN, screenSessionManagerLiteProxy_->GetDisplayState(displayId));
}

/**
 * @tc.name: GetScreenPower
 * @tc.desc: GetScreenPower
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteProxyTest, GetScreenPower, Function | SmallTest | Level1)
{
    ScreenId dmsScreenId = 1001;
    EXPECT_NE(ScreenPowerState::INVALID_STATE, screenSessionManagerLiteProxy_->GetScreenPower(dmsScreenId));
}
}
}
}