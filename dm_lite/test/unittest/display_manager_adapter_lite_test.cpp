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
#include "display_manager_adapter_lite.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayManagerAdapterLiteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayManagerAdapterLiteTest::SetUpTestCase()
{
}

void DisplayManagerAdapterLiteTest::TearDownTestCase()
{
}

void DisplayManagerAdapterLiteTest::SetUp()
{
}

void DisplayManagerAdapterLiteTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteDied
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, OnRemoteDied, TestSize.Level1)
{
    sptr<IRemoteObject::DeathRecipient> dmsDeath_ = nullptr;
    dmsDeath_ = new(std::nothrow) DMSDeathRecipientLite(SingletonContainer::Get<DisplayManagerAdapterLite>());
    dmsDeath_->OnRemoteDied(nullptr);
    ASSERT_NE(dmsDeath_, nullptr);
}

/**
 * @tc.name: OnRemoteDied01
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, OnRemoteDied01, TestSize.Level1)
{
    sptr<IRemoteObject::DeathRecipient> dmsDeath_ = nullptr;
    dmsDeath_ = new(std::nothrow) DMSDeathRecipientLite(SingletonContainer::Get<DisplayManagerAdapterLite>());
    SingletonContainer::Get<DisplayManagerAdapterLite>().InitDMSProxy();
    sptr<IRemoteObject> obj =
        SingletonContainer::Get<DisplayManagerAdapterLite>().displayManagerServiceProxy_->AsObject();
    wptr<IRemoteObject> wptrDeath = obj;
    dmsDeath_->OnRemoteDied(wptrDeath);
    ASSERT_NE(dmsDeath_, nullptr);
}

/**
 * @tc.name: Clear
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, Clear, TestSize.Level1)
{
    SingletonContainer::Get<DisplayManagerAdapterLite>().InitDMSProxy();
    SingletonContainer::Get<DisplayManagerAdapterLite>().Clear();
    ASSERT_FALSE(SingletonContainer::Get<DisplayManagerAdapterLite>().isProxyValid_);
}

/**
 * @tc.name: Clear01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, Clear01, TestSize.Level1)
{
    SingletonContainer::Get<DisplayManagerAdapterLite>().InitDMSProxy();
    SingletonContainer::Get<DisplayManagerAdapterLite>().displayManagerServiceProxy_ = nullptr;
    SingletonContainer::Get<DisplayManagerAdapterLite>().Clear();
    ASSERT_FALSE(SingletonContainer::Get<DisplayManagerAdapterLite>().isProxyValid_);
}

/**
 * @tc.name: WakeUpEnd
 * @tc.desc: test WakeUpEnd
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, WakeUpEnd, TestSize.Level1)
{
    bool ret = SingletonContainer::Get<DisplayManagerAdapterLite>().WakeUpEnd();
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: SuspendEnd
 * @tc.desc: test SuspendEnd
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, SuspendEnd, TestSize.Level1)
{
    bool ret = SingletonContainer::Get<DisplayManagerAdapterLite>().SuspendEnd();
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: SetDisplayState
 * @tc.desc: test SetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, SetDisplayState, TestSize.Level1)
{
    DisplayState state = DisplayState{1};
    bool ret = SingletonContainer::Get<DisplayManagerAdapterLite>().SetDisplayState(state);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_TRUE(ret);
    } else {
        ASSERT_FALSE(ret);
    }
}

/**
 * @tc.name: GetVirtualScreenFlag
 * @tc.desc: test GetVirtualScreenFlag
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, GetVirtualScreenFlag, TestSize.Level1)
{
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        uint64_t screenId = 0;
        auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().GetVirtualScreenFlag(screenId);
        ASSERT_EQ(ret, VirtualScreenFlag::DEFAULT);
    }
}

/**
 * @tc.name: SetSystemKeyboardStatus
 * @tc.desc: SetSystemKeyboardStatus with true as parameter
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, SetSystemKeyboardStatus01, TestSize.Level1)
{
    auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().SetSystemKeyboardStatus(true);
    ASSERT_NE(ret, DMError::DM_OK);
}
 
/**
 * @tc.name: SetSystemKeyboardStatus
 * @tc.desc: SetSystemKeyboardStatus with false as parameter
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, SetSystemKeyboardStatus02, TestSize.Level1)
{
    auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().SetSystemKeyboardStatus(false);
    ASSERT_NE(ret, DMError::DM_OK);
}

/**
 * @tc.name: SetSpecifiedScreenPowerPiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, SetSpecifiedScreenPowerPiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    uint64_t screenId = 0;
    auto ret = SingletonContainer::Get<ScreenManagerAdapterLite>().SetSpecifiedScreenPower(screenId,
        ScreenPowerState::POWER_ON, PowerStateChangeReason::POWER_BUTTON);
    EXPECT_TRUE(ret);
#undef SCREENLESS_ENABLE
}

/**
 * @tc.name: GetScreenPowerPiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, GetScreenPowerPiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    uint64_t screenId = 0;
    auto ret = SingletonContainer::Get<ScreenManagerAdapterLite>().GetScreenPower(screenId);
    EXPECT_NE(ret, ScreenPowerState::INVALID_STATE);
#undef SCREENLESS_ENABLE
}

/**
 * @tc.name: SetScreenBrightnessPiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, SetScreenBrightnessPiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    uint64_t screenId = 0;
    auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().SetScreenBrightness(screenId, 0);
    EXPECT_TRUE(ret);
#undef SCREENLESS_ENABLE
}

/**
 * @tc.name: SetDisplayStatePiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, SetDisplayStatePiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().SetDisplayState(DisplayState::ON);
    EXPECT_TRUE(ret);
#undef SCREENLESS_ENABLE
}

/**
 * @tc.name: SetScreenPowerForAllPiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, SetScreenPowerForAllPiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    auto ret = SingletonContainer::Get<ScreenManagerAdapterLite>().SetScreenPowerForAll(ScreenPowerState::POWER_ON,
        PowerStateChangeReason::POWER_BUTTON);
    EXPECT_TRUE(ret);
#undef SCREENLESS_ENABLE
}

/**
 * @tc.name: SuspendEndPiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, SuspendEndPiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().SuspendEnd();
    EXPECT_TRUE(ret);
#undef SCREENLESS_ENABLE
}

/**
 * @tc.name: SetScreenPowerByIdPiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, SetScreenPowerByIdPiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    uint64_t screenId = 0;
    auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().SetScreenPowerById(screenId,
        ScreenPowerState::POWER_ON, PowerStateChangeReason::POWER_BUTTON);
    EXPECT_TRUE(ret);
#undef SCREENLESS_ENABLE
}

/**
 * @tc.name: SuspendBeginPiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, SuspendBeginPiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().SuspendBegin(PowerStateChangeReason::POWER_BUTTON);
    EXPECT_TRUE(ret);
#undef SCREENLESS_ENABLE
}

/**
 * @tc.name: WakeUpEndPiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, WakeUpEndPiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().WakeUpEnd();
    EXPECT_TRUE(ret);
#undef SCREENLESS_ENABLE
}

/**
 * @tc.name: GetScreenBrightnessPiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, GetScreenBrightnessPiling, TestSize.Level1)
{
    uint32_t b1 = SingletonContainer::Get<DisplayManagerAdapterLite>().GetScreenBrightness(0);
    uint32_t b2 = SingletonContainer::Get<DisplayManagerAdapterLite>().GetScreenBrightness(0);
    
    EXPECT_GE(b1, 0u);
    EXPECT_LE(b1, 10000u);
    EXPECT_EQ(b1, b2);
}

/**
 * @tc.name: TryToCancelScreenOffPiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, TryToCancelScreenOffPiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().TryToCancelScreenOff();
    EXPECT_TRUE(ret);
#undef SCREENLESS_ENABLE
}

/**
 * @tc.name: IsFoldablePiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, IsFoldablePiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().IsFoldable();
    EXPECT_FALSE(ret);
#undef SCREENLESS_ENABLE
}


/**
 * @tc.name: GetInternalScreenIdPiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, GetInternalScreenIdPiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().GetInternalScreenId();
    EXPECT_EQ(ret, 0);
#undef SCREENLESS_ENABLE
}

/**
 * @tc.name: WakeUpBeginPiling
 * @tc.desc: test piling success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, WakeUpBeginPiling, TestSize.Level1)
{
#define SCREENLESS_ENABLE
    auto ret = SingletonContainer::Get<DisplayManagerAdapterLite>().WakeUpBegin(PowerStateChangeReason::POWER_BUTTON);
    EXPECT_TRUE(ret);
#undef SCREENLESS_ENABLE
}

/**
 * @tc.name: GetPhysicalScreenIds
 * @tc.desc: GetPhysicalScreenIds
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, GetPhysicalScreenIds, TestSize.Level1)
{
    std::vector<ScreenId> screenIds;
    auto ret = SingletonContainer::Get<ScreenManagerAdapterLite>().GetPhysicalScreenIds(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: SetResolution
 * @tc.desc: SetResolution
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterLiteTest, SetResolution, TestSize.Level1)
{
    ScreenId id = 0;
    uint32_t width = 1080;
    uint32_t height = 2400;
    float vpr = 2.8;
    auto ret = SingletonContainer::Get<ScreenManagerAdapterLite>().SetResolution(id, width, height, vpr);
    EXPECT_EQ(ret, DMError::DM_OK);
}
}
}
}
