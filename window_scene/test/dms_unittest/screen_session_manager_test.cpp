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

#include "screen_session_manager/include/screen_session_manager.h"
#include "display_manager_agent_default.h"
#include "iconsumer_surface.h"
#include "connection/screen_cast_connection.h"
#include "screen_scene_config.h"
#include <surface.h>
#include "scene_board_judgement.h"
#include "fold_screen_state_internel.h"
#include "common_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const int32_t CV_WAIT_SCREENOFF_MS = 1500;
const int32_t CV_WAIT_SCREENOFF_MS_MAX = 3000;
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
constexpr int32_t CAST_WIRED_PROJECTION_START = 1005;
constexpr int32_t CAST_WIRED_PROJECTION_STOP = 1007;
bool g_isPcDevice = ScreenSceneConfig::GetExternalScreenDefaultMode() == "none";
}
class ScreenSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<ScreenSessionManager> ssm_;

    ScreenId DEFAULT_SCREEN_ID {0};
    ScreenId VIRTUAL_SCREEN_ID {2};
    ScreenId VIRTUAL_SCREEN_RS_ID {100};
    void SetAceessTokenPermission(const std::string processName);
};

sptr<ScreenSessionManager> ScreenSessionManagerTest::ssm_ = nullptr;

void ScreenSessionManagerTest::SetUpTestCase()
{
    ssm_ = new ScreenSessionManager();
    CommonTestUtils::InjectTokenInfoByHapName(0, "com.ohos.systemui", 0);
    const char** perms = new const char *[1];
    perms[0] = "ohos.permission.CAPTURE_SCREEN";
    CommonTestUtils::SetAceessTokenPermission("foundation", perms, 1);
}

void ScreenSessionManagerTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void ScreenSessionManagerTest::SetUp()
{
}

void ScreenSessionManagerTest::TearDown()
{
    usleep(SLEEP_TIME_IN_US);
}

namespace {
/**
 * @tc.name: RegisterDisplayManagerAgent
 * @tc.desc: RegisterDisplayManagerAgent test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RegisterDisplayManagerAgent, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    DisplayManagerAgentType type = DisplayManagerAgentType::SCREEN_EVENT_LISTENER;
    EXPECT_NE(DMError::DM_ERROR_NOT_SYSTEM_APP, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));
    EXPECT_NE(DMError::DM_ERROR_NOT_SYSTEM_APP, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));

    type = DisplayManagerAgentType::PRIVATE_WINDOW_LISTENER;
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->RegisterDisplayManagerAgent(nullptr, type));
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->UnregisterDisplayManagerAgent(nullptr, type));

    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: WakeupBegin
 * @tc.desc: WakeupBegin test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpBegin, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_POWER_KEY;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_SWITCH;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_SUCCESS;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_ON;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_START_DREAM;
    ASSERT_EQ(false, ssm_->WakeUpBegin(reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_END_DREAM;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: WakeupBegin01
 * @tc.desc: WakeupBegin01 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpBegin01, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::POWER_BUTTON;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: WakeupBegin02
 * @tc.desc: WakeupBegin02 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpBegin02, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_INIT;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: WakeupBegin03
 * @tc.desc: WakeupBegin03 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpBegin03, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_TIMEOUT;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: WakeupBegin04
 * @tc.desc: WakeupBegin04 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpBegin04, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_RUNNING_LOCK;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: WakeupBegin05
 * @tc.desc: WakeupBegin05 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpBegin05, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_BATTERY;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: WakeupBegin06
 * @tc.desc: WakeupBegin06 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpBegin06, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_THERMAL;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: WakeupBegin07
 * @tc.desc: WakeupBegin07 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpBegin07, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_WORK;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: WakeupBegin08
 * @tc.desc: WakeupBegin08 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpBegin08, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_SYSTEM;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: WakeupBegin09
 * @tc.desc: WakeupBegin09 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpBegin09, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_APPLICATION;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: WakeupBegin10
 * @tc.desc: WakeupBegin10 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpBegin10, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_SETTINGS;
    ASSERT_EQ(true, ssm_->WakeUpBegin(reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SuspendBegin
 * @tc.desc: SuspendBegin test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SuspendBegin, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_HARD_KEY;
    ASSERT_EQ(true, ssm_->SuspendBegin(reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_OFF;
    ASSERT_EQ(true, ssm_->SuspendBegin(reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_START_DREAM;
    ASSERT_EQ(true, ssm_->SuspendBegin(reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_END_DREAM;
    ASSERT_EQ(false, ssm_->SuspendBegin(reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: GetInternalScreenId
 * @tc.desc: GetInternalScreenId test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetInternalScreenId, Function | SmallTest | Level3)
{
    ASSERT_EQ(SCREEN_ID_INVALID, ssm_->GetInternalScreenId());

    ScreenSessionConfig config;
    sptr<ScreenSession> session = new ScreenSession(config,
        ScreenSessionReason::CREATE_SESSION_FOR_REAL);
    session->SetIsInternal(true);
    ScreenId id = 0;
    ssm_->screenSessionMap_.insert(std::make_pair(id, session));
    ASSERT_EQ(0, ssm_->GetInternalScreenId());

    ssm_->screenSessionMap_.erase(0);
}

/**
 * @tc.name: SetScreenPowerById01
 * @tc.desc: SetScreenPowerById test POWER_ON
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerById01, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_POWER_KEY;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerById(screenId, state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerById02
 * @tc.desc: SetScreenPowerById test POWER_OFF
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerById02, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_POWER_KEY;
    ScreenPowerState state = ScreenPowerState::POWER_OFF;
    ASSERT_EQ(true, ssm_->SetScreenPowerById(screenId, state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerById03
 * @tc.desc: SetScreenPowerById test INVALID_STATE
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerById03, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_POWER_KEY;
    ScreenPowerState state = ScreenPowerState::INVALID_STATE;
    ASSERT_EQ(false, ssm_->SetScreenPowerById(screenId, state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerForAll
 * @tc.desc: SetScreenPowerForAll test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_POWER_KEY;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_SWITCH;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_SUCCESS;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_HARD_KEY;
    state = ScreenPowerState::POWER_OFF;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_OFF;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerForAll01
 * @tc.desc: SetScreenPowerForAll01 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll01, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_INIT;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerForAll02
 * @tc.desc: SetScreenPowerForAll02 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll02, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_RUNNING_LOCK;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerForAll03
 * @tc.desc: SetScreenPowerForAll03 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll03, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_BATTERY;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerForAll04
 * @tc.desc: SetScreenPowerForAll04 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll04, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_THERMAL;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerForAll05
 * @tc.desc: SetScreenPowerForAll05 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll05, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_WORK;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerForAll06
 * @tc.desc: SetScreenPowerForAll06 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll06, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_SYSTEM;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerForAll07
 * @tc.desc: SetScreenPowerForAll07 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll07, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_APPLICATION;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerForAll08
 * @tc.desc: SetScreenPowerForAll08 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll08, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_SETTINGS;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerForAll09
 * @tc.desc: SetScreenPowerForAll09 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll09, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_HARD_KEY;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SetScreenPowerForAll10
 * @tc.desc: SetScreenPowerForAll10 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll10, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_TOUCH;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ASSERT_EQ(true, ssm_->SetScreenPowerForAll(state, reason));

    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: ScreenChange
 * @tc.desc: ScreenChange test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ScreenChange, Function | SmallTest | Level3)
{
    ScreenEvent screenEvent = ScreenEvent::CONNECTED;
    ssm_->OnVirtualScreenChange(DEFAULT_SCREEN_ID, screenEvent);
    ssm_->OnVirtualScreenChange(VIRTUAL_SCREEN_ID, screenEvent);
    ssm_->OnScreenChange(DEFAULT_SCREEN_ID, screenEvent);
    ssm_->OnScreenChange(VIRTUAL_SCREEN_ID, screenEvent);
    EXPECT_TRUE(1);

    screenEvent = ScreenEvent::DISCONNECTED;
    ssm_->OnVirtualScreenChange(DEFAULT_SCREEN_ID, screenEvent);
    ssm_->OnVirtualScreenChange(VIRTUAL_SCREEN_ID, screenEvent);
    ssm_->OnScreenChange(DEFAULT_SCREEN_ID, screenEvent);
    ssm_->OnScreenChange(VIRTUAL_SCREEN_ID, screenEvent);
    EXPECT_TRUE(1);
}

/**
 * @tc.name: ScreenPower
 * @tc.desc: ScreenSesionManager screen power
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ScreenPower, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    PowerStateChangeReason reason = PowerStateChangeReason::POWER_BUTTON;
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    DisplayState displayState = DisplayState::ON;

    ASSERT_EQ(false, ssm_->WakeUpBegin(reason));
    ASSERT_EQ(false, ssm_->WakeUpEnd());

    ASSERT_EQ(false, ssm_->SuspendBegin(reason));
    ASSERT_EQ(false, ssm_->SuspendEnd());

    ASSERT_EQ(false, ssm_->SetScreenPowerForAll(state, reason));

    DisplayId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_EQ(true, ssm_->SetDisplayState(displayState));
    ASSERT_EQ(DisplayState::ON, ssm_->GetDisplayState(0));
}

/**
 * @tc.name: GetScreenPower
 * @tc.desc: GetScreenPower screen power
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenPower, Function | SmallTest | Level3)
{
    DisplayId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ssm_->GetScreenPower(0);
    EXPECT_TRUE(1);
}

/**
 * @tc.name: IsScreenRotationLocked
 * @tc.desc: IsScreenRotationLocked test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, IsScreenRotationLocked, Function | SmallTest | Level3)
{
    bool isLocked = false;
    DisplayId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_EQ(DMError::DM_OK, ssm_->IsScreenRotationLocked(isLocked));
}

/**
 * @tc.name: SetOrientation
 * @tc.desc: SetOrientation test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetOrientation, Function | SmallTest | Level3)
{
    Orientation orientation = Orientation::HORIZONTAL;
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_EQ(DMError::DM_OK, ssm_->SetOrientation(id, orientation));
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->SetOrientation(SCREEN_ID_INVALID, orientation));
    Orientation invalidOrientation = Orientation{20};
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetOrientation(id, invalidOrientation));
}

/**
 * @tc.name: SetRotationFromWindow
 * @tc.desc: SetRotationFromWindow test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetRotationFromWindow, Function | SmallTest | Level3)
{
    Rotation targetRotation = Rotation::ROTATION_0;
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_EQ(ssm_->SetRotationFromWindow(targetRotation), ssm_->SetRotation(id, targetRotation, true));
}

/**
 * @tc.name: GetDisplaySnapshot
 * @tc.desc: ScreenSesionManager screen shot
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplaySnapshot, Function | SmallTest | Level3)
{
    DisplayId displayId(0);
    DmErrorCode* errorCode = nullptr;
    ssm_->GetDisplaySnapshot(displayId, errorCode, false);
    EXPECT_TRUE(1);
}

/**
 * @tc.name: VirtualScreen
 * @tc.desc: ScreenSesionManager virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, VirtualScreen, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "testVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());

    std::vector<ScreenId> mirrorScreenIds;
    ScreenId mainScreenId(DEFAULT_SCREEN_ID);
    ScreenId screenGroupId{1};
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->MakeMirror(mainScreenId, mirrorScreenIds, screenGroupId));
    mirrorScreenIds.push_back(VIRTUAL_SCREEN_ID);
    ASSERT_NE(DMError::DM_OK, ssm_->MakeMirror(mainScreenId, mirrorScreenIds, screenGroupId));

    mirrorScreenIds.push_back(screenId);
    ASSERT_EQ(DMError::DM_OK, ssm_->MakeMirror(mainScreenId, mirrorScreenIds, screenGroupId));

    auto result1 = ssm_->SetVirtualScreenSurface(screenId, nullptr);
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, result1);
    sptr<IConsumerSurface> surface = OHOS::IConsumerSurface::Create();
    auto result2 = ssm_->SetVirtualScreenSurface(screenId, surface->GetProducer());
    ASSERT_EQ(DMError::DM_OK, result2);
    ASSERT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
}

/**
 * @tc.name: AutoRotate
 * @tc.desc: ScreenSesionManager SetVirtualMirrorScreenCanvasRotation test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AutoRotate, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "testAutoRotate";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }

    auto result1 = ssm_->SetVirtualMirrorScreenCanvasRotation(screenId, true);
    ASSERT_EQ(DMError::DM_OK, result1);
    auto result2 = ssm_->SetVirtualMirrorScreenCanvasRotation(screenId, false);
    ASSERT_EQ(DMError::DM_OK, result2);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetScreenSession
 * @tc.desc: GetScreenSession virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenSession, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetScreenSession";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    auto rsid = ssm_->screenIdManager_.ConvertToRsScreenId(screenId);
    sptr<ScreenSession> screenSession =
        new (std::nothrow) ScreenSession("GetScreenSession", screenId, rsid, 0);
    ASSERT_NE(ssm_->GetScreenSession(screenId), screenSession);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetDefaultScreenSession
 * @tc.desc: GetDefaultScreenSession virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDefaultScreenSession, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetDefaultScreenSession";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    auto rsid = ssm_->screenIdManager_.ConvertToRsScreenId(screenId);
    sptr<ScreenSession> screenSession =
        new (std::nothrow) ScreenSession("GetDefaultScreenSession", screenId, rsid, 0);
    ASSERT_NE(ssm_->GetDefaultScreenSession(), screenSession);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetDefaultDisplayInfo
 * @tc.desc: GetDefaultDisplayInfo virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDefaultDisplayInfo, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetDefaultDisplayInfo";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    auto rsid = ssm_->screenIdManager_.ConvertToRsScreenId(screenId);
    sptr<ScreenSession> screenSession =
        new (std::nothrow) ScreenSession("GetDefaultDisplayInfo", screenId, rsid, 0);
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    if (ssm_->GetScreenSession(screenId) == nullptr) {
        ASSERT_EQ(ssm_->GetDefaultDisplayInfo(), nullptr);
    }
    ASSERT_NE(ssm_->GetScreenSession(screenId), nullptr);
    ASSERT_NE(ssm_->GetDefaultDisplayInfo(), displayInfo);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: HookDisplayInfoByUid
 * @tc.desc: HookDisplayInfo by uid
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HookDisplayInfoByUid, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetDefaultScreenSession";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    auto rsid = ssm_->screenIdManager_.ConvertToRsScreenId(screenId);
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession("GetDefaultDisplayInfo", screenId, rsid, 0);
    sptr<DisplayInfo> displayInfo = ssm_->GetDefaultDisplayInfo();
    ASSERT_NE(ssm_->GetScreenSession(screenId), nullptr);
    ASSERT_NE(displayInfo, nullptr);
    uint32_t uid = 20020001;
    DMHookInfo dmHookInfo;
    std::map<uint32_t, DMHookInfo> displayHookMap = {};
    displayHookMap[uid] = dmHookInfo;
    ASSERT_EQ(displayHookMap.find(uid) != displayHookMap.end(), true);
    displayInfo->SetWidth(100);
    ASSERT_EQ(displayInfo->GetWidth(), 100);
    displayInfo->SetHeight(100);
    ASSERT_EQ(displayInfo->GetHeight(), 100);
    displayInfo->SetVirtualPixelRatio(1.0);
    ASSERT_EQ(displayInfo->GetVirtualPixelRatio(), 1.0);
    displayInfo->SetRotation(Rotation::ROTATION_0);
    ASSERT_EQ(displayInfo->GetRotation(), Rotation::ROTATION_0);
    displayInfo->SetDisplayOrientation(DisplayOrientation::PORTRAIT);
    ASSERT_EQ(displayInfo->GetDisplayOrientation(), DisplayOrientation::PORTRAIT);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetDisplayInfoById
 * @tc.desc: GetDisplayInfoById virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayInfoById, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetDisplayInfoById";
    ASSERT_EQ(ssm_->GetDisplayInfoById(1), nullptr);
}

/**
 * @tc.name: GetDisplayInfoByScreen
 * @tc.desc: GetDisplayInfoByScreen virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayInfoByScreen, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetDisplayInfoByScreen";
    ASSERT_EQ(ssm_->GetDisplayInfoByScreen(1), nullptr);
}

/**
 * @tc.name: GetScreenInfoById
 * @tc.desc: GetScreenInfoById virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenInfoById, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetScreenInfoById";
    ASSERT_EQ(ssm_->GetScreenInfoById(1), nullptr);
}

/**
 * @tc.name: SetScreenActiveMode
 * @tc.desc: SetScreenActiveMode virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenActiveMode, Function | SmallTest | Level3)
{
#ifdef WM_SCREEN_ACTIVE_MODE_ENABLE
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetScreenActiveMode";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    ASSERT_EQ(ssm_->SetScreenActiveMode(screenId, 0), DMError::DM_OK);
    ssm_->DestroyVirtualScreen(screenId);
#endif
}

/**
 * @tc.name: NotifyScreenChanged
 * @tc.desc: NotifyScreenChanged virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyScreenChanged, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "NotifyScreenChanged";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenInfo> screenInfo;
    ssm_->NotifyScreenChanged(screenInfo, ScreenChangeEvent::UPDATE_ORIENTATION);
    screenInfo = new ScreenInfo();
    ssm_->NotifyScreenChanged(screenInfo, ScreenChangeEvent::UPDATE_ORIENTATION);
    ASSERT_EQ(ssm_->SetScreenActiveMode(screenId, 0), DMError::DM_OK);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: NotifyDisplayEvent
 * @tc.desc: NotifyDisplayEvent  virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyDisplayEvent, Function | SmallTest | Level3)
{
    DisplayEvent event = DisplayEvent::KEYGUARD_DRAWN;
    ssm_->NotifyDisplayEvent(event);
    ASSERT_EQ(ssm_->keyguardDrawnDone_, true);

    event = DisplayEvent::SCREEN_LOCK_SUSPEND;
    ssm_->NotifyDisplayEvent(event);
    ASSERT_EQ(ssm_->gotScreenOffNotify_, true);

    event = DisplayEvent::SCREEN_LOCK_OFF;
    ssm_->NotifyDisplayEvent(event);
    ASSERT_EQ(ssm_->gotScreenOffNotify_, true);

    event = DisplayEvent::SCREEN_LOCK_FINGERPRINT;
    ssm_->NotifyDisplayEvent(event);
    ASSERT_EQ(ssm_->gotScreenlockFingerprint_, true);

    ssm_->gotScreenOffNotify_ = false;
    event = DisplayEvent::SCREEN_LOCK_DOZE_FINISH;
    ssm_->NotifyDisplayEvent(event);
    ASSERT_EQ(ssm_->gotScreenOffNotify_, true);
}

/**
 * @tc.name: GetScreenInfoByDisplayId
 * @tc.desc: GetScreenInfoByDisplayId  virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenInfoByDisplayId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetScreenInfoByDisplayId";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenInfo> screenInfo;
    screenInfo = ssm_->GetScreenInfoByDisplayId(screenId);
    ASSERT_NE(screenInfo, nullptr);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetScreenModesByDisplayId
 * @tc.desc: GetScreenModesByDisplayId  virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenModesByDisplayId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetScreenModesByDisplayId";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<SupportedScreenModes> screenModes;
    screenModes = ssm_->GetScreenModesByDisplayId(screenId);
    ASSERT_NE(screenModes, nullptr);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: UpdateDisplayHookInfo001
 * @tc.desc: UpdateDisplayHookInfo by uid
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateDisplayHookInfo001, Function | SmallTest | Level3)
{
    uint32_t uid = 20020001;
    DMHookInfo dmHookInfo;
    dmHookInfo.width_ = 100;
    dmHookInfo.height_ = 100;
    dmHookInfo.density_ = 1.0;
    std::map<uint32_t, DMHookInfo> displayHookMap = {};
    displayHookMap[uid] = dmHookInfo;
    ASSERT_EQ(displayHookMap.find(uid) != displayHookMap.end(), true);
    auto info = displayHookMap[uid];
    ASSERT_EQ(info.width_, 100);
    ASSERT_EQ(info.height_, 100);
    ASSERT_EQ(info.density_, 1.0);
}

/**
 * @tc.name: UpdateDisplayHookInfo002
 * @tc.desc: UpdateDisplayHookInfo by uid
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateDisplayHookInfo002, Function | SmallTest | Level3)
{
    uint32_t uid = 20020001;
    DMHookInfo dmHookInfo;
    dmHookInfo.width_ = 100;
    dmHookInfo.height_ = 100;
    dmHookInfo.density_ = 1.0;
    std::map<uint32_t, DMHookInfo> displayHookMap = {};
    displayHookMap[uid] = dmHookInfo;
    displayHookMap.erase(uid);
    ASSERT_EQ(displayHookMap.find(uid) != displayHookMap.end(), false);
}

/**
 * @tc.name: NotifyIsFullScreenInForceSplitMode
 * @tc.desc: NotifyIsFullScreenInForceSplitMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyIsFullScreenInForceSplitMode, Function | SmallTest | Level2)
{
    int32_t uid = 0;
    ssm_->NotifyIsFullScreenInForceSplitMode(uid, true);
    DMHookInfo hookInfo;
    hookInfo.enableHookRotation_ = true;
    hookInfo.rotation_ = true;
    hookInfo.density_ = 1.1;
    hookInfo.width_ = 100;
    hookInfo.height_ = 200;
    ssm_->UpdateDisplayHookInfo(uid, true, hookInfo);
    DMHookInfo hookInfo2;
    ssm_->GetDisplayHookInfo(uid, hookInfo2);
    ASSERT_EQ(hookInfo2.width_, 0);

    uid = 100;
    ssm_->NotifyIsFullScreenInForceSplitMode(uid, true);
    ssm_->UpdateDisplayHookInfo(uid, true, hookInfo);
    ssm_->NotifyIsFullScreenInForceSplitMode(uid, true);
    DMHookInfo hookInfo3;
    ssm_->GetDisplayHookInfo(uid, hookInfo3);
    ASSERT_TRUE(hookInfo3.isFullScreenInForceSplit_);
}

/**
 * @tc.name: SetVirtualPixelRatio
 * @tc.desc: SetVirtualPixelRatio virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualPixelRatio, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetVirtualPixelRatio";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    ASSERT_EQ(DMError::DM_OK, ssm_->SetVirtualPixelRatio(screenId, 0.1));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetVirtualPixelRatioSystem
 * @tc.desc: SetVirtualPixelRatioSystem virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualPixelRatioSystem, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetVirtualPixelRatioSystem";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    ASSERT_EQ(DMError::DM_OK, ssm_->SetVirtualPixelRatioSystem(screenId, 0.1));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetDefaultDensityDpi
 * @tc.desc: SetDefaultDensityDpi virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetDefaultDensityDpi, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetDefaultDensityDpi";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    ASSERT_EQ(DMError::DM_OK, ssm_->SetDefaultDensityDpi(screenId, 0.1));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetResolution
 * @tc.desc: SetResolution virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetResolution, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetResolution";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ssm_->screenSessionMap_[screenId] = screenSession;
    sptr<SupportedScreenModes> mode = new SupportedScreenModes();
    mode->width_ = 1;
    mode->height_ = 1;
    screenSession->modes_ = {mode};
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetResolution(screenId, 100, 100, 0.5));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetScreenColorGamut
 * @tc.desc: GetScreenColorGamut virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenColorGamut, Function | SmallTest | Level3)
{
#ifdef WM_SCREEN_COLOR_GAMUT_ENABLE
    ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->GetScreenColorGamut(1, colorGamut));
    DisplayId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(DMError::DM_OK, ssm_->GetScreenColorGamut(id, colorGamut));
    } else {
        ASSERT_NE(DMError::DM_OK, ssm_->GetScreenColorGamut(id, colorGamut));
    }
#endif
}

/**
 * @tc.name: LoadScreenSceneXml
 * @tc.desc: LoadScreenSceneXml virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, LoadScreenSceneXml, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "LoadScreenSceneXml";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    ssm_->LoadScreenSceneXml();
    ASSERT_EQ(ssm_->SetScreenActiveMode(screenId, 0), DMError::DM_OK);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetScreenGamutMap
 * @tc.desc: GetScreenGamutMap virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenGamutMap, Function | SmallTest | Level3)
{
#ifdef WM_SCREEN_COLOR_GAMUT_ENABLE
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetScreenGamutMap";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    auto rsid = ssm_->screenIdManager_.ConvertToRsScreenId(screenId);
    sptr<ScreenSession> screenSession =
        new (std::nothrow) ScreenSession("GetScreenGamutMap", screenId, rsid, 0);
    ScreenGamutMap gamutMap;
    ASSERT_EQ(DMError::DM_OK, ssm_->GetScreenGamutMap(screenId, gamutMap));
    ssm_->DestroyVirtualScreen(screenId);
#endif
}

/**
 * @tc.name: MakeExpand
 * @tc.desc: MakeExpand virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, MakeExpand, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "MakeExpand";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    std::vector<ScreenId> mirrorScreenIds;
    std::vector<Point> startPoints;
    ScreenId screenGroupId2 = DISPLAY_ID_INVALID;
    ASSERT_NE(DMError::DM_ERROR_RENDER_SERVICE_FAILED, ssm_->MakeExpand(mirrorScreenIds, startPoints, screenGroupId2));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: DeleteScreenId
 * @tc.desc: DeleteScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, DeleteScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "DeleteScreenId";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ScreenSessionManager::ScreenIdManager sim;
    ASSERT_EQ(false, sim.DeleteScreenId(99));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: HasRsScreenId
 * @tc.desc: HasRsScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HasRsScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "HasRsScreenId";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ScreenSessionManager::ScreenIdManager sim;
    ASSERT_EQ(false, sim.HasRsScreenId(99));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: AddAsFirstScreenLocked
 * @tc.desc: AddAsFirstScreenLocked virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddAsFirstScreenLocked, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "AddAsFirstScreenLocked";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> newScreen = new (std::nothrow) ScreenSession();
    sptr<ScreenSessionGroup> sessiongroup=nullptr;
    ASSERT_NE(sessiongroup, ssm_->AddAsFirstScreenLocked(newScreen));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: AddAsSuccedentScreenLocked
 * @tc.desc: AddAsSuccedentScreenLocked virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddAsSuccedentScreenLocked, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "AddAsSuccedentScreenLocked";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> newScreen;
    sptr<ScreenSessionGroup> sessiongroup=nullptr;
    ASSERT_EQ(sessiongroup, ssm_->AddAsSuccedentScreenLocked(newScreen));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetMirror
 * @tc.desc: SetMirror virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMirror, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetMirror";

    std::vector<ScreenId> screens{0, 1, 2, 3, 4, 5, 6, 7};
    sptr<ScreenSession> screenSession = nullptr;
    screenSession = new (std::nothrow) ScreenSession("GetScreenGamutMap", 2, 2, 3);
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMap_{
        {2, screenSession},
    };
    ssm_->screenSessionMap_ = screenSessionMap_;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    auto screen = ssm_->GetScreenSession(2);
    screen->GetScreenProperty().SetScreenType(ScreenType::REAL);
    ASSERT_EQ(DMError::DM_OK, ssm_->SetMirror(2, screens, DMRect::NONE()));
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->SetMirror(9, screens, DMRect::NONE()));
    ASSERT_EQ(DMError::DM_OK, ssm_->SetMirror(screenId, screens, DMRect::NONE()));
    auto mirrorscreen = ssm_->GetScreenSession(screenId);
    ASSERT_TRUE(mirrorscreen != nullptr);
    mirrorscreen->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->SetMirror(screenId, screens, DMRect::NONE()));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetAbstractScreenGroup
 * @tc.desc: GetAbstractScreenGroup virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAbstractScreenGroup, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetAbstractScreenGroup";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSessionGroup> sreengroup=nullptr;
    ASSERT_EQ(sreengroup, ssm_->GetAbstractScreenGroup(2));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: InitAbstractScreenModesInfo
 * @tc.desc: InitAbstractScreenModesInfo virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitAbstractScreenModesInfo, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "InitAbstractScreenModesInfo";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_EQ(true, ssm_->InitAbstractScreenModesInfo(screenSession));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: AddToGroupLocked
 * @tc.desc: AddToGroupLocked virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddToGroupLocked, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "AddToGroupLocked";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> newScreen=nullptr;
    sptr<ScreenSessionGroup> sessiongroup=nullptr;
    ASSERT_EQ(sessiongroup, ssm_->AddToGroupLocked(newScreen));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: InitVirtualScreen
 * @tc.desc: InitVirtualScreen virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitVirtualScreen, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "InitVirtualScreen";
    sptr<ScreenSession> screenSession;
    VirtualScreenOption option;
    ASSERT_NE(screenSession, ssm_->InitVirtualScreen(1, 2, option));
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ssm_->InitVirtualScreen(1, 2, option));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: InitAndGetScreen
 * @tc.desc: InitAndGetScreen virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitAndGetScreen, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "InitAndGetScreen";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ssm_->InitAndGetScreen(2));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: RemoveFromGroupLocked
 * @tc.desc: RemoveFromGroupLocked virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RemoveFromGroupLocked, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "RemoveFromGroupLocked";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> newScreen = new (std::nothrow) ScreenSession();
    sptr<ScreenSessionGroup> sessiongroup;
    ASSERT_EQ(sessiongroup, ssm_->RemoveFromGroupLocked(newScreen));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: CreateAndGetNewScreenId
 * @tc.desc: CreateAndGetNewScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CreateAndGetNewScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "CreateAndGetNewScreenId";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    ScreenSessionManager::ScreenIdManager sim;
    ASSERT_EQ(1000, sim.CreateAndGetNewScreenId(screenId));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: AddScreenToGroup
 * @tc.desc: AddScreenToGroup virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddScreenToGroup, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "AddScreenToGroup";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    const std::vector<ScreenId> addScreens;
    const std::vector<Point> addChildPos;
    std::map<ScreenId, bool> removeChildResMap;
    sptr<ScreenSessionGroup> group;
    ssm_->AddScreenToGroup(group, addScreens, addChildPos, removeChildResMap);
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ssm_->InitAndGetScreen(2));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetRSDisplayNodeByScreenId
 * @tc.desc: GetRSDisplayNodeByScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetRSDisplayNodeByScreenId, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetRSDisplayNodeByScreenId";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ssm_->GetRSDisplayNodeByScreenId(2);
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ssm_->InitAndGetScreen(2));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetAllDisplayIds
 * @tc.desc: GetAllDisplayIds virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAllDisplayIds, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetAllDisplayIds";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    std::vector<DisplayId>  displays(2);
    ASSERT_NE(displays, ssm_->GetAllDisplayIds());
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetScreenGamutMap
 * @tc.desc: SetScreenGamutMap virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenGamutMap, Function | SmallTest | Level3)
{
#ifdef WM_SCREEN_COLOR_GAMUT_ENABLE
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetScreenGamutMap";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    auto rsid = ssm_->screenIdManager_.ConvertToRsScreenId(screenId);
    sptr<ScreenSession> screenSession =
        new (std::nothrow) ScreenSession("SetScreenGamutMap", screenId, rsid, 0);
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM,
              ssm_->SetScreenGamutMap(SCREEN_ID_INVALID, ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION));
    ASSERT_EQ(DMError::DM_OK, ssm_->SetScreenGamutMap(screenId, ScreenGamutMap::GAMUT_MAP_EXTENSION));
    ssm_->DestroyVirtualScreen(screenId);
#endif
}

/**
 * @tc.name: StopExpand
 * @tc.desc: StopExpand virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, StopExpand, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "StopExpand";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    std::vector<ScreenId> expandScreenIds {0, 1, 2, 3, 4, 5};
    ASSERT_NE(DMError::DM_ERROR_NOT_SYSTEM_APP, ssm_->StopExpand(expandScreenIds));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: OnScreenDisconnect
 * @tc.desc: OnScreenDisconnect virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnScreenDisconnect, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "OnScreenDisconnect";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ssm_->OnScreenDisconnect(1);
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ssm_->InitAndGetScreen(2));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetScreenColorGamut
 * @tc.desc: SetScreenColorGamut virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenColorGamut, Function | SmallTest | Level3)
{
#ifdef WM_SCREEN_COLOR_GAMUT_ENABLE
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetScreenColorGamut";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    auto rsid = ssm_->screenIdManager_.ConvertToRsScreenId(screenId);
    sptr<ScreenSession> screenSession =
        new (std::nothrow) ScreenSession("SetScreenColorGamut", screenId, rsid, 0);
    ASSERT_EQ(DMError::DM_OK, ssm_->SetScreenColorGamut(screenId, 2));
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetScreenColorGamut(SCREEN_ID_INVALID, 2));
    ssm_->DestroyVirtualScreen(screenId);
#endif
}

/**
 * @tc.name: SetScreenColorTransform
 * @tc.desc: SetScreenColorTransform virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenColorTransform, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "SetScreenColorTransform";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    auto rsid = ssm_->screenIdManager_.ConvertToRsScreenId(screenId);
    sptr<ScreenSession> screenSession =
        new (std::nothrow) ScreenSession("SetScreenColorTransform", screenId, rsid, 0);
    ASSERT_EQ(DMError::DM_OK, ssm_->SetScreenColorTransform(screenId));
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetScreenColorTransform(SCREEN_ID_INVALID));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetScreenRotationLocked
 * @tc.desc: SetScreenRotationLocked virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenRotationLocked, Function | SmallTest | Level3)
{
    ScreenId id = 0;
    ssm_->screenSessionMap_[id] = nullptr;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetScreenRotationLocked(false));
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_EQ(DMError::DM_OK, ssm_->SetScreenRotationLocked(false));
}

/**
 * @tc.name: UpdateScreenRotationProperty
 * @tc.desc: UpdateScreenRotationProperty virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateScreenRotationProperty, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "UpdateScreenRotationProperty";
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;
    int rotation = 1;
    ScreenPropertyChangeType screenPropertyChangeType = ScreenPropertyChangeType::ROTATION_BEGIN;
    ScreenSessionManager::GetInstance().UpdateScreenRotationProperty(1, bounds, 1, screenPropertyChangeType);
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(virtualOption,
        displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ScreenSessionManager::GetInstance().UpdateScreenRotationProperty(1, bounds, rotation,
        screenPropertyChangeType);
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ScreenSessionManager::GetInstance().InitAndGetScreen(2));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: MakeUniqueScreen
 * @tc.desc: Make unique screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, MakeUniqueScreen, Function | SmallTest | Level3)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    vector<ScreenId> screenIds;
    screenIds.clear();
    std::vector<DisplayId> displayIds;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->MakeUniqueScreen(screenIds, displayIds));
#endif
}

/**
 * @tc.name: ConvertScreenIdToRsScreenId
 * @tc.desc: convert screen id to RS screen id
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ConvertScreenIdToRsScreenId, Function | SmallTest | Level3)
{
    ScreenId rsScreenId = VIRTUAL_SCREEN_RS_ID;
    ssm_->screenIdManager_.rs2SmsScreenIdMap_.erase(rsScreenId);
    ScreenId screenId = ssm_->screenIdManager_.CreateAndGetNewScreenId(rsScreenId);
    ASSERT_EQ(true, ssm_->screenIdManager_.HasRsScreenId(rsScreenId));
    ScreenId tmpRsScreenId = SCREEN_ID_INVALID;
    ASSERT_EQ(true, ssm_->ConvertScreenIdToRsScreenId(screenId, tmpRsScreenId));
    ASSERT_EQ(tmpRsScreenId, rsScreenId);
    ssm_->screenIdManager_.DeleteScreenId(screenId);
    ASSERT_EQ(false, ssm_->ConvertScreenIdToRsScreenId(screenId, tmpRsScreenId));
}

/**
 * @tc.name: DisableMirror
 * @tc.desc: DisableMirror test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, DisableMirror, Function | SmallTest | Level3)
{
    ASSERT_EQ(DMError::DM_OK, ssm_->DisableMirror(false));
}

/**
 * @tc.name: HasImmersiveWindow
 * @tc.desc: HasImmersiveWindow test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HasImmersiveWindow, Function | SmallTest | Level3)
{
    bool immersive = false;
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->HasImmersiveWindow(0u, immersive));
    immersive = true;
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->HasImmersiveWindow(0u, immersive));
}

/**
 * @tc.name: SetSpecifiedScreenPower
 * @tc.desc: ScreenSessionManager screen power
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetSpecifiedScreenPower, Function | SmallTest | Level3)
{
    ScreenId mainScreenId(DEFAULT_SCREEN_ID);
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    PowerStateChangeReason reason = PowerStateChangeReason::POWER_BUTTON;
    ASSERT_EQ(false, ssm_->SetSpecifiedScreenPower(mainScreenId, state, reason));

    reason = PowerStateChangeReason::STATE_CHANGE_REASON_COLLABORATION;
    EXPECT_TRUE(ssm_->SetSpecifiedScreenPower(mainScreenId, state, reason));
}

/**
 * @tc.name: NotifyFoldStatusChanged
 * @tc.desc: ScreenSessionManager notify foldStatus changed
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyFoldStatusChanged, Function | SmallTest | Level3)
{
    const std::string& dumpParam = "-p";
    if (ssm_ != nullptr)
    {
        int errCode = ssm_->NotifyFoldStatusChanged(dumpParam);
        ASSERT_EQ(errCode, 0);
    } else {
        ASSERT_EQ(1, 0);
    }
}

/**
 * @tc.name: NotifyPrivateWindowListChanged
 * @tc.desc: ScreenSessionManager notify PrivateWindowList changed
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyPrivateWindowListChanged, Function | SmallTest | Level3)
{
    DisplayId id = 0;
    std::vector<std::string> privacyWindowList{"win0", "win1"};
    if (ssm_ != nullptr)
    {
        ssm_->NotifyPrivateWindowListChanged(id, privacyWindowList);
        ASSERT_EQ(0, 0);
    } else {
        ASSERT_EQ(1, 0);
    }
}

/**
 * @tc.name: SetPrivacyStateByDisplayId01
 * @tc.desc: SetPrivacyStateByDisplayId true test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetPrivacyStateByDisplayId01, Function | SmallTest | Level3)
{
    DisplayId id = 0;
    bool hasPrivate = true;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    ssm_->SetPrivacyStateByDisplayId(id, hasPrivate);
    bool result = screenSession->HasPrivateSessionForeground();
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: SetPrivacyStateByDisplayId02
 * @tc.desc: SetPrivacyStateByDisplayId false test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetPrivacyStateByDisplayId02, Function | SmallTest | Level3)
{
    DisplayId id = 0;
    bool hasPrivate = false;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    ssm_->SetPrivacyStateByDisplayId(id, hasPrivate);
    bool result = screenSession->HasPrivateSessionForeground();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: SetScreenPrivacyWindowList
 * @tc.desc: SetScreenPrivacyWindowList test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPrivacyWindowList, Function | SmallTest | Level3)
{
    DisplayId id = 0;
    std::vector<std::string> privacyWindowList{"win0", "win1"};
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    ssm_->SetScreenPrivacyWindowList(id, privacyWindowList);
    ASSERT_EQ(0, 0);
}

/**
 * @tc.name: GetAllScreenIds
 * @tc.desc: GetAllScreenIds screen power
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAllScreenIds, Function | SmallTest | Level3)
{
    sptr<ScreenSession> screenSession = new ScreenSession();
    ASSERT_NE(nullptr, screenSession);
    ssm_->screenSessionMap_.insert(std::make_pair(1, screenSession));
    ssm_->GetAllScreenIds();
    EXPECT_TRUE(1);
}

/**
 * @tc.name: GetAllScreenInfos
 * @tc.desc: GetAllScreenInfos test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAllScreenInfos, Function | SmallTest | Level3)
{
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    std::vector<sptr<ScreenInfo>> screenInfos;
    EXPECT_EQ(DMError::DM_OK, ssm_->GetAllScreenInfos(screenInfos));
}

/**
 * @tc.name: GetScreenSupportedColorGamuts
 * @tc.desc: GetScreenSupportedColorGamuts test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenSupportedColorGamuts, Function | SmallTest | Level3)
{
    std::vector<ScreenColorGamut> colorGamuts;
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->GetScreenSupportedColorGamuts(SCREEN_ID_INVALID, colorGamuts));
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(ssm_->GetScreenSupportedColorGamuts(id, colorGamuts),
        screenSession->GetScreenSupportedColorGamuts(colorGamuts));
}

/**
 * @tc.name: GetPixelFormat
 * @tc.desc: GetPixelFormat test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPixelFormat, Function | SmallTest | Level3)
{
    GraphicPixelFormat pixelFormat;
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->GetPixelFormat(SCREEN_ID_INVALID, pixelFormat));
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(ssm_->GetPixelFormat(id, pixelFormat), screenSession->GetPixelFormat(pixelFormat));
}

/**
 * @tc.name: SetPixelFormat
 * @tc.desc: SetPixelFormat test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetPixelFormat, Function | SmallTest | Level3)
{
    GraphicPixelFormat pixelFormat = GraphicPixelFormat{GRAPHIC_PIXEL_FMT_CLUT8};
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetPixelFormat(SCREEN_ID_INVALID, pixelFormat));
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(ssm_->SetPixelFormat(id, pixelFormat), screenSession->SetPixelFormat(pixelFormat));
}

/**
 * @tc.name: GetSupportedHDRFormats
 * @tc.desc: GetSupportedHDRFormats test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetSupportedHDRFormats, Function | SmallTest | Level3)
{
#ifdef WM_SCREEN_HDR_FORMAT_ENABLE
    std::vector<ScreenHDRFormat> hdrFormats;
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->GetSupportedHDRFormats(SCREEN_ID_INVALID, hdrFormats));
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(ssm_->GetSupportedHDRFormats(id, hdrFormats), screenSession->GetSupportedHDRFormats(hdrFormats));
#endif
}

/**
 * @tc.name: GetScreenHDRFormat
 * @tc.desc: GetScreenHDRFormat test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenHDRFormat, Function | SmallTest | Level3)
{
#ifdef WM_SCREEN_HDR_FORMAT_ENABLE
    ScreenHDRFormat hdrFormat;
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->GetScreenHDRFormat(SCREEN_ID_INVALID, hdrFormat));
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(ssm_->GetScreenHDRFormat(id, hdrFormat), screenSession->GetScreenHDRFormat(hdrFormat));
#endif
}

/**
 * @tc.name: SetScreenHDRFormat
 * @tc.desc: SetScreenHDRFormat test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenHDRFormat, Function | SmallTest | Level3)
{
#ifdef WM_SCREEN_HDR_FORMAT_ENABLE
    int32_t modeIdx {0};
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetScreenHDRFormat(SCREEN_ID_INVALID, modeIdx));
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(ssm_->SetScreenHDRFormat(id, modeIdx), screenSession->SetScreenHDRFormat(modeIdx));
#endif
}

/**
 * @tc.name: GetSupportedColorSpaces
 * @tc.desc: GetSupportedColorSpaces test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetSupportedColorSpaces, Function | SmallTest | Level3)
{
#ifdef WM_SCREEN_COLOR_SPACE_ENABLE
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->GetSupportedColorSpaces(SCREEN_ID_INVALID, colorSpaces));
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(ssm_->GetSupportedColorSpaces(id, colorSpaces), screenSession->GetSupportedColorSpaces(colorSpaces));
#endif
}

/**
 * @tc.name: GetScreenColorSpace
 * @tc.desc: GetScreenColorSpace test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenColorSpace, Function | SmallTest | Level3)
{
#ifdef WM_SCREEN_COLOR_SPACE_ENABLE
    GraphicCM_ColorSpaceType colorSpace;
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->GetScreenColorSpace(SCREEN_ID_INVALID, colorSpace));
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(ssm_->GetScreenColorSpace(id, colorSpace), screenSession->GetScreenColorSpace(colorSpace));
#endif
}

/**
 * @tc.name: SetScreenColorSpace
 * @tc.desc: SetScreenColorSpace test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenColorSpace, Function | SmallTest | Level3)
{
#ifdef WM_SCREEN_COLOR_SPACE_ENABLE
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType{GRAPHIC_CM_COLORSPACE_NONE};
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetScreenColorSpace(SCREEN_ID_INVALID, colorSpace));
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(ssm_->SetScreenColorSpace(id, colorSpace), screenSession->SetScreenColorSpace(colorSpace));
#endif
}

/**
 * @tc.name: HasPrivateWindow
 * @tc.desc: HasPrivateWindow test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HasPrivateWindow, Function | SmallTest | Level3)
{
    bool hasPrivateWindow;
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->HasPrivateWindow(SCREEN_ID_INVALID, hasPrivateWindow));
    DisplayId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(DMError::DM_OK, ssm_->HasPrivateWindow(id, hasPrivateWindow));
}

/**
 * @tc.name: GetAvailableArea
 * @tc.desc: GetAvailableArea test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAvailableArea, Function | SmallTest | Level3)
{
    DMRect area;
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->GetAvailableArea(SCREEN_ID_INVALID, area));
    DisplayId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(DMError::DM_OK, ssm_->GetAvailableArea(id, area));
}

/**
 * @tc.name: ResetAllFreezeStatus
 * @tc.desc: ResetAllFreezeStatus test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ResetAllFreezeStatus, Function | SmallTest | Level3)
{
    EXPECT_EQ(DMError::DM_OK, ssm_->ResetAllFreezeStatus());
}

/**
 * @tc.name: SetVirtualScreenRefreshRate
 * @tc.desc: SetVirtualScreenRefreshRate test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenRefreshRate, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    uint32_t refreshInterval {2};
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetVirtualScreenRefreshRate(id, refreshInterval));
    EXPECT_EQ(DMError::DM_OK, ssm_->SetVirtualScreenRefreshRate(screenId, refreshInterval));
    uint32_t invalidRefreshInterval {0};
    EXPECT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetVirtualScreenRefreshRate(screenId, invalidRefreshInterval));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetVirtualScreenFlag
 * @tc.desc: SetVirtualScreenFlag test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenFlag, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenFlag screenFlag = VirtualScreenFlag::DEFAULT;
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    EXPECT_EQ(DMError::DM_OK, ssm_->SetVirtualScreenFlag(screenId, screenFlag));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetVirtualScreenFlag
 * @tc.desc: GetVirtualScreenFlag test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetVirtualScreenFlag, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenFlag screenFlag = VirtualScreenFlag::DEFAULT;
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);

    EXPECT_EQ(DMError::DM_OK, ssm_->SetVirtualScreenFlag(screenId, screenFlag));
    EXPECT_EQ(screenFlag, ssm_->GetVirtualScreenFlag(screenId));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: ResizeVirtualScreen
 * @tc.desc: ResizeVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ResizeVirtualScreen, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);

    uint32_t width {100};
    uint32_t height {100};
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    if (screenSession->GetDisplayNode() != nullptr) {
        EXPECT_EQ(DMError::DM_OK, ssm_->ResizeVirtualScreen(screenId, width, height));
    }
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetVirtualMirrorScreenScaleMode
 * @tc.desc: SetVirtualMirrorScreenScaleMode test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualMirrorScreenScaleMode, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ScreenScaleMode scaleMode = ScreenScaleMode::FILL_MODE;
    EXPECT_EQ(DMError::DM_OK, ssm_->SetVirtualMirrorScreenScaleMode(screenId, scaleMode));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: StopMirror
 * @tc.desc: StopMirror test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, StopMirror, Function | SmallTest | Level3)
{
    std::vector<ScreenId> mirrorScreenIds {0, 1, 2, 3, 4, 5};
    EXPECT_EQ(DMError::DM_OK, ssm_->StopMirror(mirrorScreenIds));
}

/**
 * @tc.name: GetDensityInCurResolution
 * @tc.desc: GetDensityInCurResolution screen power
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDensityInCurResolution, Function | SmallTest | Level3)
{
    sptr<ScreenSession> screenSession = new ScreenSession();
    ASSERT_NE(nullptr, screenSession);
    ssm_->screenSessionMap_.insert(std::make_pair(1, screenSession));
    ScreenId screenId = 100;
    float x = 3.14;
    auto res = ssm_->GetDensityInCurResolution(screenId, x);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, res);
    screenId = 1;
    res = ssm_->GetDensityInCurResolution(screenId, x);
    EXPECT_EQ(DMError::DM_OK, res);
}

/**
 * @tc.name: SetScreenOffDelayTime
 * @tc.desc: SetScreenOffDelayTime test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenOffDelayTime, Function | SmallTest | Level3)
{
    int32_t delay = CV_WAIT_SCREENOFF_MS - 1;
    int32_t ret = ssm_->SetScreenOffDelayTime(delay);
    EXPECT_EQ(ret, CV_WAIT_SCREENOFF_MS);

    delay = CV_WAIT_SCREENOFF_MS + 1;
    ret = ssm_->SetScreenOffDelayTime(delay);
    EXPECT_EQ(ret, delay);

    delay = CV_WAIT_SCREENOFF_MS_MAX - 1;
    ret = ssm_->SetScreenOffDelayTime(delay);
    EXPECT_EQ(ret, delay);

    delay = CV_WAIT_SCREENOFF_MS_MAX + 1;
    ret = ssm_->SetScreenOffDelayTime(delay);
    EXPECT_EQ(ret, CV_WAIT_SCREENOFF_MS_MAX);
}

/**
 * @tc.name: GetDeviceScreenConfig
 * @tc.desc: GetDeviceScreenConfig test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDeviceScreenConfig, Function | SmallTest | Level3)
{
    DeviceScreenConfig deviceScreenConfig = ssm_->GetDeviceScreenConfig();
    EXPECT_FALSE(deviceScreenConfig.rotationPolicy_.empty());
}

/**
 * @tc.name: SetVirtualScreenBlackList
 * @tc.desc: SetVirtualScreenBlackList test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenBlackList01, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::SCREEN_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    std::vector<uint64_t> windowId = {10, 20, 30};
    ssm_->SetVirtualScreenBlackList(screenId, windowId);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetVirtualScreenBlackList
 * @tc.desc: ConvertScreenIdToRsScreenId = false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenBlackList02, Function | SmallTest | Level3)
{
    ScreenId screenId = 1010;
    ScreenId rsScreenId = SCREEN_ID_INVALID;
    ASSERT_FALSE(ssm_->ConvertScreenIdToRsScreenId(screenId, rsScreenId));
    std::vector<uint64_t> windowId = {10, 20, 30};
    ssm_->SetVirtualScreenBlackList(screenId, windowId);
}

/**
 * @tc.name: SetVirtualDisplayMuteFlag
 * @tc.desc: SetVirtualDisplayMuteFlag test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualDisplayMuteFlag01, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::SCREEN_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));

    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    bool muteFlag = false;
    ssm_->SetVirtualDisplayMuteFlag(screenId, muteFlag);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetVirtualDisplayMuteFlag
 * @tc.desc: ConvertScreenIdToRsScreenId = false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualDisplayMuteFlag02, Function | SmallTest | Level3)
{
    ScreenId screenId = 1010;
    ScreenId rsScreenId = SCREEN_ID_INVALID;
    ASSERT_FALSE(ssm_->ConvertScreenIdToRsScreenId(screenId, rsScreenId));
    bool muteFlag = false;
    ssm_->SetVirtualDisplayMuteFlag(screenId, muteFlag);
}

/**
 * @tc.name: GetAllDisplayPhysicalResolution
 * @tc.desc: GetAllDisplayPhysicalResolution test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAllDisplayPhysicalResolution, Function | SmallTest | Level3)
{
    std::vector<DisplayPhysicalResolution> allSize {};
    if (ssm_ != nullptr) {
        allSize = ssm_->GetAllDisplayPhysicalResolution();
        ASSERT_TRUE(!allSize.empty());
    } else {
        ASSERT_TRUE(allSize.empty());
    }
}

/**
 * @tc.name: SetDisplayScale
 * @tc.desc: SetDisplayScale test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetDisplayScale, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ScreenId fakeScreenId = static_cast<ScreenId>(-1);
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    ssm_->SetDisplayScale(fakeScreenId, scaleX, scaleY, pivotX, pivotY);
    ssm_->SetDisplayScale(ssm_->GetDefaultScreenId(), scaleX, scaleY, pivotX, pivotY);
}

/**
 * @tc.name: ScreenCastConnection
 * @tc.desc: ScreenCastConnection test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ScreenCastConnection, Function | SmallTest | Level3)
{
    std::string castBundleName = "testCastBundleName";
    std::string castAbilityName = "testCastAbilityName";

    ScreenCastConnection::GetInstance().SetBundleName("");
    ScreenCastConnection::GetInstance().SetAbilityName("");
    EXPECT_EQ(ScreenCastConnection::GetInstance().GetBundleName(), "");
    EXPECT_EQ(ScreenCastConnection::GetInstance().GetAbilityName(), "");

    EXPECT_EQ(ScreenCastConnection::GetInstance().CastConnectExtension(1), false);

    ScreenCastConnection::GetInstance().SetBundleName(castBundleName);
    ScreenCastConnection::GetInstance().SetAbilityName(castAbilityName);
    EXPECT_EQ(ScreenCastConnection::GetInstance().GetBundleName(), castBundleName);
    EXPECT_EQ(ScreenCastConnection::GetInstance().GetAbilityName(), castAbilityName);

    MessageParcel data;
    MessageParcel reply;
    ScreenCastConnection::GetInstance().SendMessageToCastService(CAST_WIRED_PROJECTION_START, data, reply);
    ScreenCastConnection::GetInstance().SendMessageToCastService(CAST_WIRED_PROJECTION_STOP, data, reply);
    ScreenCastConnection::GetInstance().CastDisconnectExtension();
    EXPECT_EQ(ScreenCastConnection::GetInstance().IsConnectedSync(), false);
}

/**
 * @tc.name: ReportFoldStatusToScb
 * @tc.desc: clientProxy_ = nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ReportFoldStatusToScb, Function | SmallTest | Level3)
{
    ssm_->clientProxy_ = nullptr;
    FoldStatus currentStatus = FoldStatus::FOLDED;
    FoldStatus nextStatus = FoldStatus::HALF_FOLD;
    int32_t duration = 1;
    float postureAngle = 1.2;
    std::vector<std::string> screenFoldInfo {std::to_string(static_cast<int32_t>(currentStatus)),
        std::to_string(static_cast<int32_t>(nextStatus)), std::to_string(duration), std::to_string(postureAngle)};
    ssm_->ReportFoldStatusToScb(screenFoldInfo);
    ASSERT_NE(ssm_, nullptr);
}

/**
 * @tc.name: DisablePowerOffRenderControl
 * @tc.desc: ConvertScreenIdToRsScreenId = true
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, DisablePowerOffRenderControl01, Function | SmallTest | Level3)
{
    ScreenId screenId = 1050;
    ScreenId rsScreenId = SCREEN_ID_INVALID;
    ScreenId rsScreenId1 = 1060;
    ssm_->screenIdManager_.sms2RsScreenIdMap_[screenId] = rsScreenId1;
    ASSERT_TRUE(ssm_->ConvertScreenIdToRsScreenId(screenId, rsScreenId));
    ssm_->DisablePowerOffRenderControl(screenId);
}

/**
 * @tc.name: DisablePowerOffRenderControl
 * @tc.desc: ConvertScreenIdToRsScreenId = false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, DisablePowerOffRenderControl02, Function | SmallTest | Level3)
{
    ScreenId screenId = 1010;
    ScreenId rsScreenId = SCREEN_ID_INVALID;
    ASSERT_FALSE(ssm_->ConvertScreenIdToRsScreenId(screenId, rsScreenId));
    ssm_->DisablePowerOffRenderControl(screenId);
}

/**
 * @tc.name: CheckAndSendHiSysEvent
 * @tc.desc: eventName != "CREATE_VIRTUAL_SCREEN"
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CheckAndSendHiSysEvent, Function | SmallTest | Level3)
{
    std::string eventName =  "GET_DISPLAY_SNAPSHOT";
    std::string bundleName = "hmos.screenshot";
    ssm_->CheckAndSendHiSysEvent(eventName, bundleName);
    ASSERT_NE(ssm_, nullptr);
}

/**
 * @tc.name: NotifyFoldToExpandCompletion
 * @tc.desc: NotifyFoldToExpandCompletion
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyFoldToExpandCompletion, Function | SmallTest | Level3)
{
    bool foldToExpand = false;
    ssm_->NotifyFoldToExpandCompletion(foldToExpand);

    foldToExpand = true;
    ssm_->NotifyFoldToExpandCompletion(foldToExpand);
    ASSERT_NE(ssm_, nullptr);
}

/**
 * @tc.name: UpdateAvailableArea
 * @tc.desc: screenSession == nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateAvailableArea01, Function | SmallTest | Level3)
{
    ScreenId screenId = 1050;
    DMRect area = DMRect{};
    ssm_->UpdateAvailableArea(screenId, area);
    ASSERT_NE(ssm_, nullptr);
}

/**
 * @tc.name: UpdateAvailableArea
 * @tc.desc: screenSession != nullptr screenSession->UpdateAvailableArea(area) is true
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateAvailableArea02, Function | SmallTest | Level3)
{
    ScreenId screenId = 1050;
    DMRect area{0, 0, 600, 900};
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(screenSession, nullptr);
    ssm_->screenSessionMap_[screenId] = screenSession;
    auto screenSession1 = ssm_->GetScreenSession(screenId);
    ASSERT_EQ(screenSession1, screenSession);
    ASSERT_TRUE(screenSession->UpdateAvailableArea(area));
    ssm_->UpdateAvailableArea(screenId, area);
}

/**
 * @tc.name: UpdateAvailableArea
 * @tc.desc: screenSession != nullptr screenSession->UpdateAvailableArea(area) is false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateAvailableArea03, Function | SmallTest | Level3)
{
    ScreenId screenId = 1050;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(screenSession, nullptr);
    DMRect area = screenSession->GetAvailableArea();
    ssm_->screenSessionMap_[screenId] = screenSession;
    ASSERT_FALSE(screenSession->UpdateAvailableArea(area));
    ssm_->UpdateAvailableArea(screenId, area);
}

/**
 * @tc.name: NotifyAvailableAreaChanged
 * @tc.desc: agents.empty()
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyAvailableAreaChanged01, Function | SmallTest | Level3)
{
    DMRect area = DMRect{};
    DisplayId displayId = 0;
    ASSERT_NE(ssm_, nullptr);
    ssm_->NotifyAvailableAreaChanged(area, displayId);
}

/**
 * @tc.name: TriggerFoldStatusChange01
 * @tc.desc: test interface TriggerFoldStatusChange
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TriggerFoldStatusChange01, Function | SmallTest | Level3)
{
#ifdef FOLD_ABILITY_ENABLE
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }
    ASSERT_NE(ssm_, nullptr);
    FoldStatus foldStatus = FoldStatus::EXPAND;
    ssm_->TriggerFoldStatusChange(foldStatus);
    FoldStatus res = ssm_->GetFoldStatus();
    EXPECT_EQ(res, foldStatus);

    foldStatus = FoldStatus::FOLDED;
    ssm_->TriggerFoldStatusChange(foldStatus);
    res = ssm_->GetFoldStatus();
    EXPECT_EQ(res, foldStatus);

    foldStatus = FoldStatus::HALF_FOLD;
    ssm_->TriggerFoldStatusChange(foldStatus);
    res = ssm_->GetFoldStatus();
    EXPECT_EQ(res, foldStatus);

    foldStatus = FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_EXPAND;
    ssm_->TriggerFoldStatusChange(foldStatus);
    res = ssm_->GetFoldStatus();
    EXPECT_EQ(res, foldStatus);

    foldStatus = FoldStatus::FOLD_STATE_EXPAND_WITH_SECOND_HALF_FOLDED;
    ssm_->TriggerFoldStatusChange(foldStatus);
    res = ssm_->GetFoldStatus();
    EXPECT_EQ(res, foldStatus);

    foldStatus = FoldStatus::FOLD_STATE_FOLDED_WITH_SECOND_EXPAND;
    ssm_->TriggerFoldStatusChange(foldStatus);
    res = ssm_->GetFoldStatus();
    EXPECT_EQ(res, foldStatus);

    foldStatus = FoldStatus::FOLD_STATE_FOLDED_WITH_SECOND_HALF_FOLDED;
    ssm_->TriggerFoldStatusChange(foldStatus);
    res = ssm_->GetFoldStatus();
    EXPECT_EQ(res, foldStatus);

    foldStatus = FoldStatus::FOLD_STATE_HALF_FOLDED_WITH_SECOND_EXPAND;
    ssm_->TriggerFoldStatusChange(foldStatus);
    res = ssm_->GetFoldStatus();
    EXPECT_EQ(res, foldStatus);

    foldStatus = FoldStatus::FOLD_STATE_HALF_FOLDED_WITH_SECOND_HALF_FOLDED;
    ssm_->TriggerFoldStatusChange(foldStatus);
    res = ssm_->GetFoldStatus();
    EXPECT_EQ(res, foldStatus);
#endif
}

/**
 * @tc.name: NotifyFoldStatusChanged
 * @tc.desc: NotifyFoldStatusChanged
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyFoldStatusChanged02, Function | SmallTest | Level3)
{
#ifdef FOLD_ABILITY_ENABLE
    std::string statusParam;
    auto ret = ssm_->NotifyFoldStatusChanged(statusParam);
    ASSERT_EQ(ret, -1);

    statusParam =  "-z";     // STATUS_FOLD_HALF
    ret = ssm_->NotifyFoldStatusChanged(statusParam);
    ASSERT_EQ(ret, 0);

    statusParam = "-y";     // STATUS_EXPAND
    ret = ssm_->NotifyFoldStatusChanged(statusParam);
    ASSERT_EQ(ret, 0);

    statusParam = "-coor";     // ARG_FOLD_DISPLAY_COOR
    ret = ssm_->NotifyFoldStatusChanged(statusParam);

    if (!(ssm_->IsFoldable())) {
        ssm_->foldScreenController_ = new FoldScreenController(
            ssm_->displayInfoMutex_, ssm_->screenPowerTaskScheduler_);
    }
    ASSERT_NE(ssm_->foldScreenController_, nullptr);
    statusParam = "-y";
    ssm_->HandleFoldScreenPowerInit();
    ret = ssm_->NotifyFoldStatusChanged(statusParam);
    ASSERT_EQ(ret, 0);
#endif
}

/**
 * @tc.name: Dump
 * @tc.desc: Dump
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, Dump, Function | SmallTest | Level3)
{
    int fd = 2;
    std::vector<std::u16string> args;
    int ret = ssm_->Dump(fd, args);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: GetDisplayNode
 * @tc.desc: GetDisplayNode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayNode01, Function | SmallTest | Level3)
{
    ScreenId screenId = 1051;
    auto ret = ssm_->GetDisplayNode(screenId);
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetDisplayNode
 * @tc.desc: !screenSession = false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayNode02, Function | SmallTest | Level3)
{
    ScreenId screenId = 1050;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(screenSession, nullptr);
    ssm_->screenSessionMap_[screenId] = screenSession;
    auto ret = ssm_->GetDisplayNode(screenId);
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: GetScreenProperty
 * @tc.desc: GetScreenProperty
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenProperty01, Function | SmallTest | Level3)
{
    ScreenId screenId = 2000;
    ASSERT_EQ(ssm_->GetScreenSession(screenId), nullptr);
    auto ret = ssm_->GetScreenProperty(screenId);
    ASSERT_EQ(sizeof(ret), sizeof(ScreenProperty));
}

/**
 * @tc.name: GetScreenProperty
 * @tc.desc: !screenSession = false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenProperty02, Function | SmallTest | Level3)
{
    ScreenId screenId = 1050;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(screenSession, nullptr);
    ssm_->screenSessionMap_[screenId] = screenSession;
    ScreenProperty property = ssm_->GetScreenProperty(screenId);
    ASSERT_EQ(sizeof(property), sizeof(screenSession->property_));
}

/**
 * @tc.name: GetCurrentScreenPhyBounds
 * @tc.desc: GetCurrentScreenPhyBounds
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCurrentScreenPhyBounds01, Function | SmallTest | Level3)
{
#ifdef FOLD_ABILITY_ENABLE
    float phyWidth = 0.0f;
    float phyHeight = 0.0f;
    bool isReset = true;
    ScreenId screenId = 1050;
    ssm_->GetCurrentScreenPhyBounds(phyWidth, phyHeight, isReset, screenId);
    ASSERT_NE(isReset, true);
    if (!(ssm_->IsFoldable())) {
        ssm_->foldScreenController_ = new FoldScreenController(
            ssm_->displayInfoMutex_, ssm_->screenPowerTaskScheduler_);
    }

    ASSERT_NE(ssm_->foldScreenController_, nullptr);
    ssm_->GetCurrentScreenPhyBounds(phyWidth, phyHeight, isReset, screenId);
    ASSERT_FALSE(isReset);
#endif
}

/**
 * @tc.name: PhyMirrorConnectWakeupScreen
 * @tc.desc: PhyMirrorConnectWakeupScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, PhyMirrorConnectWakeupScreen, Function | SmallTest | Level3)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    ssm_->PhyMirrorConnectWakeupScreen();
    ScreenSceneConfig::stringConfig_["externalScreenDefaultMode"] = "mirror";
    ssm_->PhyMirrorConnectWakeupScreen();
#endif
}

/**
 * @tc.name: SetVirtualScreenStatus
 * @tc.desc: SetVirtualScreenStatus test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenStatus, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ASSERT_FALSE(ssm_->SetVirtualScreenStatus(SCREEN_ID_INVALID, VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE));

    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());

    ASSERT_TRUE(ssm_->SetVirtualScreenStatus(screenId, VirtualScreenStatus::VIRTUAL_SCREEN_PAUSE));
    EXPECT_EQ(DMError::DM_OK, ssm_->DestroyVirtualScreen(screenId));
}

/**
 * @tc.name: SetClient
 * @tc.desc: SetClient
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetClient, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    sptr<IScreenSessionManagerClient> client = nullptr;
    ssm->SetClient(client);
}

/**
 * @tc.name: SwitchUser
 * @tc.desc: SwitchUser
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SwitchUser, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ssm->SwitchUser();
}

/**
 * @tc.name: SetScreenPrivacyMaskImage001
 * @tc.desc: SetScreenPrivacyMaskImage001
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPrivacyMaskImage001, Function | SmallTest | Level3)
{
    ScreenId screenId = DEFAULT_SCREEN_ID;
    auto ret = ssm_->SetScreenPrivacyMaskImage(screenId, nullptr);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: ScbClientDeathCallback
 * @tc.desc: ScbClientDeathCallback
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ScbClientDeathCallback, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    int32_t deathScbPid = ssm->currentScbPId_;
    ssm->currentScbPId_ = -1;     // INVALID_SCB_PID
    ssm->ScbClientDeathCallback(deathScbPid);
    ASSERT_EQ(ssm->clientProxy_, nullptr);

    deathScbPid = ssm->currentScbPId_;
    ssm->currentScbPId_ = 0;
    ssm->ScbClientDeathCallback(deathScbPid);
    ASSERT_EQ(ssm->clientProxy_, nullptr);

    deathScbPid = 0;
    ssm->currentScbPId_ = -1;     // INVALID_SCB_PID
    ssm->ScbClientDeathCallback(deathScbPid);
    ASSERT_EQ(ssm->clientProxy_, nullptr);

    deathScbPid = 0;
    ssm->currentScbPId_ = 0;
    ssm->ScbClientDeathCallback(deathScbPid);
    ASSERT_EQ(ssm->clientProxy_, nullptr);
}

/**
 * @tc.name: NotifyClientProxyUpdateFoldDisplayMode?
 * @tc.desc: NotifyClientProxyUpdateFoldDisplayMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyClientProxyUpdateFoldDisplayMode, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    FoldDisplayMode displayMode = FoldDisplayMode::FULL;
    ASSERT_EQ(ssm->clientProxy_, nullptr);
    ssm->NotifyClientProxyUpdateFoldDisplayMode(displayMode);
}

/**
 * @tc.name: OnScreenRotationLockedChange
 * @tc.desc: OnScreenRotationLockedChange
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnScreenRotationLockedChange, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    bool isLocked = false;
    ScreenId screenId = 1050;
    ASSERT_EQ(ssm->clientProxy_, nullptr);
    ssm->OnScreenRotationLockedChange(isLocked, screenId);
}

/**
 * @tc.name: OnScreenOrientationChange
 * @tc.desc: OnScreenOrientationChange
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnScreenOrientationChange, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    float screenOrientation = 75.2f;
    ScreenId screenId = 1050;
    ASSERT_EQ(ssm->clientProxy_, nullptr);
    ssm->OnScreenOrientationChange(screenOrientation, screenId);
}

/**
 * @tc.name: NotifyDisplayModeChanged
 * @tc.desc: NotifyDisplayModeChanged
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyDisplayModeChanged, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ssm->NotifyDisplayModeChanged(FoldDisplayMode::MAIN);
}

/**
 * @tc.name: SetMultiScreenMode
 * @tc.desc: MultiScreenMode::SCREEN_EXTEND
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenMode01, Function | SmallTest | Level3)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    MultiScreenMode screenMode = MultiScreenMode::SCREEN_EXTEND;

    auto ret = ssm_->SetMultiScreenMode(0, screenId, screenMode);
    ASSERT_EQ(ret, DMError::DM_OK);
    ssm_->DestroyVirtualScreen(screenId);
#endif
}

/**
 * @tc.name: SetMultiScreenMode
 * @tc.desc: MultiScreenMode::SCREEN_MIRROR
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenMode02, Function | SmallTest | Level3)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    MultiScreenMode screenMode = MultiScreenMode::SCREEN_MIRROR;

    auto ret = ssm_->SetMultiScreenMode(0, screenId, screenMode);
    ASSERT_EQ(ret, DMError::DM_OK);
    ssm_->DestroyVirtualScreen(screenId);
#endif
}

/**
 * @tc.name: SetMultiScreenMode
 * @tc.desc: operate mode error
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenMode03, Function | SmallTest | Level3)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());

    uint32_t testNum = 2;
    auto ret = ssm_->SetMultiScreenMode(0, screenId, static_cast<MultiScreenMode>(testNum));
    ASSERT_EQ(ret, DMError::DM_OK);
    ssm_->DestroyVirtualScreen(screenId);
#endif
}

/**
 * @tc.name: SetMultiScreenRelativePosition
 * @tc.desc: ScreenSession is null
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenRelativePosition01, Function | SmallTest | Level3)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    ScreenId testId = 2060;
    ScreenId testId1 = 3060;
    MultiScreenPositionOptions mainScreenOptions = {testId, 100, 100};
    MultiScreenPositionOptions secondScreenOption = {testId1, 100, 100};
    auto ret = ssm_->SetMultiScreenRelativePosition(mainScreenOptions, secondScreenOption);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);
#endif
}

/**
 * @tc.name: SetMultiScreenRelativePosition
 * @tc.desc: B is located on the right side of A
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenRelativePosition02, Function | SmallTest | Level3)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    virtualOption.width_ = 200;
    virtualOption.height_ = 100;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent1, nullptr);
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    virtualOption1.width_ = 200;
    virtualOption1.height_ = 100;
    auto screenId1 = ssm_->CreateVirtualScreen(virtualOption1, displayManagerAgent1->AsObject());

    MultiScreenPositionOptions mainScreenOptions = {screenId, 0, 0};
    MultiScreenPositionOptions secondScreenOption = {screenId1, 200, 50};
    auto ret = ssm_->SetMultiScreenRelativePosition(mainScreenOptions, secondScreenOption);
    ASSERT_EQ(ret, DMError::DM_OK);

    ssm_->DestroyVirtualScreen(screenId);
    ssm_->DestroyVirtualScreen(screenId1);
#endif
}

/**
 * @tc.name: SetMultiScreenRelativePosition
 * @tc.desc: B is located below A
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenRelativePosition03, Function | SmallTest | Level3)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    virtualOption.width_ = 200;
    virtualOption.height_ = 100;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent1, nullptr);
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    virtualOption1.width_ = 200;
    virtualOption1.height_ = 100;
    auto screenId1 = ssm_->CreateVirtualScreen(virtualOption1, displayManagerAgent1->AsObject());

    MultiScreenPositionOptions mainScreenOptions = {screenId, 0, 0};
    MultiScreenPositionOptions secondScreenOption = {screenId1, 100, 100};
    auto ret = ssm_->SetMultiScreenRelativePosition(mainScreenOptions, secondScreenOption);
    ASSERT_EQ(ret, DMError::DM_OK);

    ssm_->DestroyVirtualScreen(screenId);
    ssm_->DestroyVirtualScreen(screenId1);
#endif
}

/**
 * @tc.name: SetMultiScreenRelativePosition
 * @tc.desc: INVALID_PARAM
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenRelativePosition04, Function | SmallTest | Level3)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    virtualOption.width_ = 200;
    virtualOption.height_ = 100;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent1, nullptr);
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    virtualOption1.width_ = 200;
    virtualOption1.height_ = 100;
    auto screenId1 = ssm_->CreateVirtualScreen(virtualOption1, displayManagerAgent1->AsObject());

    MultiScreenPositionOptions mainScreenOptions = {screenId, 0, 0};
    MultiScreenPositionOptions secondScreenOption = {screenId1, 100, 50};
    auto ret = ssm_->SetMultiScreenRelativePosition(mainScreenOptions, secondScreenOption);
    ASSERT_EQ(ret, DMError::DM_ERROR_INVALID_PARAM);

    ssm_->DestroyVirtualScreen(screenId);
    ssm_->DestroyVirtualScreen(screenId1);
#endif
}

/**
 * @tc.name: SetMultiScreenRelativePosition
 * @tc.desc: INVALID_PARAM
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenRelativePosition05, Function | SmallTest | Level3)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    virtualOption.width_ = 200;
    virtualOption.height_ = 100;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent1, nullptr);
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    virtualOption1.width_ = 200;
    virtualOption1.height_ = 100;
    auto screenId1 = ssm_->CreateVirtualScreen(virtualOption1, displayManagerAgent1->AsObject());

    MultiScreenPositionOptions mainScreenOptions = {screenId, 0, 0};
    MultiScreenPositionOptions secondScreenOption = {screenId1, 200, 100};
    auto ret = ssm_->SetMultiScreenRelativePosition(mainScreenOptions, secondScreenOption);
    ASSERT_EQ(ret, DMError::DM_ERROR_INVALID_PARAM);

    ssm_->DestroyVirtualScreen(screenId);
    ssm_->DestroyVirtualScreen(screenId1);
#endif
}

/**
 * @tc.name: SetMultiScreenRelativePosition
 * @tc.desc: DisplayNode is null
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenRelativePosition06, Function | SmallTest | Level3)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent1, nullptr);
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ssm_->CreateVirtualScreen(virtualOption1, displayManagerAgent1->AsObject());

    MultiScreenPositionOptions mainScreenOptions = {screenId, 0, 100};
    MultiScreenPositionOptions secondScreenOption = {screenId1, 100, 100};

    sptr<ScreenSession> secondScreenSession = ssm_->GetScreenSession(screenId1);
    secondScreenSession->ReleaseDisplayNode();
    auto ret = ssm_->SetMultiScreenRelativePosition(mainScreenOptions, secondScreenOption);
    ASSERT_EQ(ret, DMError::DM_OK);

    sptr<ScreenSession> firstScreenSession = ssm_->GetScreenSession(screenId);
    firstScreenSession->ReleaseDisplayNode();
    ret = ssm_->SetMultiScreenRelativePosition(mainScreenOptions, secondScreenOption);

    ssm_->DestroyVirtualScreen(screenId);
    ssm_->DestroyVirtualScreen(screenId1);
#endif
}

/**
 * @tc.name: SetCoordinationFlag
 * @tc.desc: SetCoordinationFlag
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetCoordinationFlag, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ASSERT_EQ(ssm->isCoordinationFlag_, false);
    ssm->SetCoordinationFlag(true);
    ASSERT_EQ(ssm->isCoordinationFlag_, true);
}

/**
 * @tc.name: GetTentMode
 * @tc.desc: Test get tent mode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetTentMode, Function | SmallTest | Level1)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    auto tentMode = ssm_->GetTentMode();
    ASSERT_EQ(tentMode, false);
}

/**
 * @tc.name: OnTentModeChanged
 * @tc.desc: Test change tent mode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnTentModeChanged, Function | SmallTest | Level1)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    bool isTentMode = false;
    ssm_->OnTentModeChanged(isTentMode);
    ASSERT_EQ(ssm_->GetTentMode(), false);
}

/**
 * @tc.name: GetScreenCapture
 * @tc.desc: GetScreenCapture
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenCapture, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    CaptureOption option;
    option.displayId_ = 0;
    DmErrorCode errCode;
    std::shared_ptr<Media::PixelMap> bitMap = ssm->GetScreenCapture(option, &errCode);
    if (errCode == DmErrorCode::DM_OK) {
        ASSERT_NE(bitMap, nullptr);
    } else {
        ASSERT_EQ(bitMap, nullptr);
    }
}

/**
 * @tc.name: OnScreenCaptureNotify
 * @tc.desc: OnScreenCaptureNotify
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnScreenCaptureNotify, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId screenId = 0;
    int32_t uid = 0;
    std::string clientName = "test";
    ssm->OnScreenCaptureNotify(screenId, uid, clientName);
}

/**
 * @tc.name: GetPrimaryDisplayInfo
 * @tc.desc: GetPrimaryDisplayInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPrimaryDisplayInfo, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ASSERT_NE(ssm_->GetPrimaryDisplayInfo(), nullptr);
}

/*
 * @tc.name: TransferTypeToString
 * @tc.desc: TransferTypeToString
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TransferTypeToString1, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    std::string screenType = ssm->TransferTypeToString(ScreenType::REAL);
    std::string expectType = "REAL";
    ASSERT_EQ(screenType, expectType);
}

/**
 * @tc.name: TransferTypeToString
 * @tc.desc: TransferTypeToString
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TransferTypeToString2, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    std::string screenType = ssm->TransferTypeToString(ScreenType::VIRTUAL);
    std::string expectType = "VIRTUAL";
    ASSERT_EQ(screenType, expectType);
}

/**
 * @tc.name: TransferTypeToString
 * @tc.desc: TransferTypeToString
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TransferTypeToString3, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    std::string screenType = ssm->TransferTypeToString(ScreenType::UNDEFINED);
    std::string expectType = "UNDEFINED";
    ASSERT_EQ(screenType, expectType);
}

/**
 * @tc.name: TransferPropertyChangeTypeToString
 * @tc.desc: TransferPropertyChangeTypeToString
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TransferPropertyChangeTypeToString1, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    std::string screenType = ssm->TransferPropertyChangeTypeToString(ScreenPropertyChangeType::UNSPECIFIED);
    std::string expectType = "UNSPECIFIED";
    ASSERT_EQ(screenType, expectType);
}

/**
 * @tc.name: TransferPropertyChangeTypeToString
 * @tc.desc: TransferPropertyChangeTypeToString
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TransferPropertyChangeTypeToString2, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    std::string screenType = ssm->TransferPropertyChangeTypeToString(ScreenPropertyChangeType::ROTATION_BEGIN);
    std::string expectType = "ROTATION_BEGIN";
    ASSERT_EQ(screenType, expectType);
}

/**
 * @tc.name: TransferPropertyChangeTypeToString
 * @tc.desc: TransferPropertyChangeTypeToString
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TransferPropertyChangeTypeToString3, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    std::string screenType = ssm->TransferPropertyChangeTypeToString(ScreenPropertyChangeType::ROTATION_END);
    std::string expectType = "ROTATION_END";
    ASSERT_EQ(screenType, expectType);
}

/**
 * @tc.name: TransferPropertyChangeTypeToString
 * @tc.desc: TransferPropertyChangeTypeToString
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TransferPropertyChangeTypeToString4, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    std::string screenType = ssm->TransferPropertyChangeTypeToString(
        ScreenPropertyChangeType::ROTATION_UPDATE_PROPERTY_ONLY);
    std::string expectType = "ROTATION_UPDATE_PROPERTY_ONLY";
    ASSERT_EQ(screenType, expectType);
}

/**
 * @tc.name: ConvertOffsetToCorrectRotation
 * @tc.desc: ConvertOffsetToCorrectRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ConvertOffsetToCorrectRotation, Function | SmallTest | Level3)
{
    int32_t phyOffset = 90;
    ASSERT_EQ(ssm_->ConvertOffsetToCorrectRotation(phyOffset), ScreenRotation::ROTATION_270);
    phyOffset = 180;
    ASSERT_EQ(ssm_->ConvertOffsetToCorrectRotation(phyOffset), ScreenRotation::ROTATION_180);
    phyOffset = 270;
    ASSERT_EQ(ssm_->ConvertOffsetToCorrectRotation(phyOffset), ScreenRotation::ROTATION_90);
    phyOffset = 0;
    ASSERT_EQ(ssm_->ConvertOffsetToCorrectRotation(phyOffset), ScreenRotation::ROTATION_0);
}

/**
 * @tc.name: ConfigureScreenSnapshotParams
 * @tc.desc: ConfigureScreenSnapshotParams
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ConfigureScreenSnapshotParams, Function | SmallTest | Level3)
{
    ssm_->OnStart();
    auto stringConfig = ScreenSceneConfig::GetStringConfig();
    if (g_isPcDevice) {
        ASSERT_EQ(stringConfig.count("screenSnapshotBundleName"), 1);
    } else {
        ASSERT_EQ(stringConfig.count("screenSnapshotBundleName"), 0);
    }
    ssm_->ConfigureScreenSnapshotParams();
}

/**
 * @tc.name: RegisterRefreshRateChangeListener
 * @tc.desc: RegisterRefreshRateChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RegisterRefreshRateChangeListener, Function | SmallTest | Level3)
{
    ssm_->RegisterRefreshRateChangeListener();
    std::string ret = ssm_->screenEventTracker_.recordInfos_.back().info;
    ASSERT_NE(ret, "Dms RefreshRateChange register failed.");
}

/**
 * @tc.name: FreeDisplayMirrorNodeInner
 * @tc.desc: FreeDisplayMirrorNodeInner
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, FreeDisplayMirrorNodeInner, Function | SmallTest | Level3)
{
    sptr<ScreenSession> mirrorSession = nullptr;
    ssm_->FreeDisplayMirrorNodeInner(mirrorSession);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    ASSERT_EQ(ssm_->GetScreenSession(screenId)->GetDisplayNode(), nullptr);
}

/**
 * @tc.name: GetPowerStatus
 * @tc.desc: GetPowerStatus test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPowerStatus01, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ScreenPowerStatus status;
    ssm_->GetPowerStatus(ScreenPowerState::POWER_ON, PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT, status);
    ASSERT_EQ(status, ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);
    ssm_->GetPowerStatus(ScreenPowerState::POWER_ON, PowerStateChangeReason::POWER_BUTTON, status);
    ASSERT_EQ(status, ScreenPowerStatus::POWER_STATUS_ON);
}

/**
 * @tc.name: GetPowerStatus
 * @tc.desc: GetPowerStatus test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPowerStatus02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ScreenPowerStatus status;
    ssm_->GetPowerStatus(ScreenPowerState::POWER_OFF,
        PowerStateChangeReason::STATE_CHANGE_REASON_PRE_BRIGHT_AUTH_FAIL_SCREEN_OFF, status);
    ASSERT_EQ(status, ScreenPowerStatus::POWER_STATUS_OFF_ADVANCED);
    ssm_->GetPowerStatus(ScreenPowerState::POWER_OFF, PowerStateChangeReason::POWER_BUTTON, status);
    ASSERT_EQ(status, ScreenPowerStatus::POWER_STATUS_OFF);
}

/**
 * @tc.name: GetPowerStatus
 * @tc.desc: GetPowerStatus test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPowerStatus03, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ScreenPowerStatus status;
    ssm_->GetPowerStatus(ScreenPowerState::POWER_SUSPEND, PowerStateChangeReason::POWER_BUTTON, status);
    ASSERT_EQ(status, ScreenPowerStatus::POWER_STATUS_SUSPEND);
}

/**
 * @tc.name: GetPowerStatus
 * @tc.desc: GetPowerStatus test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPowerStatus04, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ScreenPowerStatus status;
    ssm_->GetPowerStatus(ScreenPowerState::POWER_DOZE, PowerStateChangeReason::POWER_BUTTON, status);
    ASSERT_EQ(status, ScreenPowerStatus::POWER_STATUS_DOZE);
}

/**
 * @tc.name: GetPowerStatus
 * @tc.desc: GetPowerStatus test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPowerStatus05, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ScreenPowerStatus status;
    ssm_->GetPowerStatus(ScreenPowerState::POWER_DOZE_SUSPEND, PowerStateChangeReason::POWER_BUTTON, status);
    ASSERT_EQ(status, ScreenPowerStatus::POWER_STATUS_DOZE_SUSPEND);
}

/**
 * @tc.name: SetGotScreenOffAndWakeUpBlock
 * @tc.desc: SetGotScreenOffAndWakeUpBlock test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetGotScreenOffAndWakeUpBlock, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->SetGotScreenOffAndWakeUpBlock();
    ASSERT_TRUE(ssm_->gotScreenOffNotify_);
    ssm_->needScreenOffNotify_ = true;
    ssm_->SetGotScreenOffAndWakeUpBlock();
    ASSERT_FALSE(ssm_->needScreenOffNotify_);
}

/**
 * @tc.name: GetFoldStatus
 * @tc.desc: GetFoldStatus test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetFoldStatus, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    auto status = ssm_->GetFoldStatus();
    if (ssm_->IsFoldable()) {
        EXPECT_NE(FoldStatus::UNKNOWN, status);
    } else {
        EXPECT_EQ(FoldStatus::UNKNOWN, status);
    }
}

/**
 * @tc.name: SetLowTemp
 * @tc.desc: SetLowTemp test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetLowTemp, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->SetLowTemp(LowTempMode::LOW_TEMP_OFF);
    ASSERT_EQ(ssm_->lowTemp_, LowTempMode::LOW_TEMP_OFF);
    ssm_->SetLowTemp(LowTempMode::LOW_TEMP_ON);
    ASSERT_EQ(ssm_->lowTemp_, LowTempMode::LOW_TEMP_ON);
}

/**
 * @tc.name: SetScreenSkipProtectedWindow
 * @tc.desc: SetScreenSkipProtectedWindow test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenSkipProtectedWindow, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    ASSERT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption1";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    const std::vector<ScreenId> screenIds = {screenId, 1002};
    bool isEnable = true;
    ASSERT_EQ(ssm_->SetScreenSkipProtectedWindow(screenIds, isEnable), DMError::DM_OK);
    isEnable = false;
    ASSERT_EQ(ssm_->SetScreenSkipProtectedWindow(screenIds, isEnable), DMError::DM_OK);
}

/**
 * @tc.name: SetScreenSkipProtectedWindowInner
 * @tc.desc: SetScreenSkipProtectedWindowInner test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenSkipProtectedWindowInner, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    ASSERT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption1";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    virtualOption.name_ = "createVirtualOption2";
    auto screenId2 = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId2 != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId2 != VIRTUAL_SCREEN_ID);
    }
    auto screenSession = ssm_->GetScreenSession(screenId);
    screenSession->SetShareProtect(true);
    ssm_->SetScreenSkipProtectedWindowInner();
}

/**
 * @tc.name: GetDisplayCapability
 * @tc.desc: GetDisplayCapability test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayCapability, Function | SmallTest | Level3)
{
    std::string info {""};
    if (ssm_ != nullptr) {
        EXPECT_EQ(DMError::DM_OK, ssm_->GetDisplayCapability(info));
        ASSERT_NE(info, "");
    } else {
        ASSERT_EQ(info, "");
    }
}

/**
 * @tc.name: GetSecondaryDisplayCapability
 * @tc.desc: GetSecondaryDisplayCapability test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetSecondaryDisplayCapability, Function | SmallTest | Level3)
{
    std::string info {""};
    if (ssm_ != nullptr) {
        EXPECT_EQ(DMError::DM_OK, ssm_->GetSecondaryDisplayCapability(info));
        ASSERT_NE(info, "");
    } else {
        ASSERT_EQ(info, "");
    }
}

/**
 * @tc.name: GetSuperFoldCapability
 * @tc.desc: GetSuperFoldCapability test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetSuperFoldCapability, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    std::string info {""};
    if (ssm_ != nullptr) {
        EXPECT_EQ(DMError::DM_OK, ssm_->GetSuperFoldCapability(info));
        ASSERT_NE(info, "");
    } else {
        ASSERT_EQ(info, "");
    }
}

/**
 * @tc.name: GetFoldableDeviceCapability
 * @tc.desc: GetFoldableDeviceCapability test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetFoldableDeviceCapability, Function | SmallTest | Level3)
{
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        GTEST_SKIP() << "SceneBoard is not enabled, skipping test.";
    }
    std::string info {""};
    if (ssm_ != nullptr) {
        EXPECT_EQ(DMError::DM_OK, ssm_->GetFoldableDeviceCapability(info));
        ASSERT_NE(info, "");
    } else {
        ASSERT_EQ(info, "");
    }
}

/**
 * @tc.name: DoMakeUniqueScreenOld
 * @tc.desc: DoMakeUniqueScreenOld test
 * @tc.type: FUNC
 */
 HWTEST_F(ScreenSessionManagerTest, DoMakeUniqueScreenOld, Function | SmallTest | Level3)
 {
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    ASSERT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    std::vector<ScreenId> allUniqueScreenIds = {screenId, 99};
    std::vector<DisplayId> displayIds = {};
    ssm_->DoMakeUniqueScreenOld(allUniqueScreenIds, displayIds, false);
    ASSERT_EQ(displayIds.size(), 1);
}

/**
 * @tc.name: SetCastPrivacyFromSettingData
 * @tc.desc: SetCastPrivacyFromSettingData test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetCastPrivacyFromSettingData, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    ASSERT_NE(displayManagerAgent, nullptr);
    ScreenId id = 2;
    sptr<ScreenSession> newSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(newSession, nullptr);
    ssm_->screenSessionMap_[id] = newSession;
    ssm_->SetCastPrivacyFromSettingData();
}

/**
 * @tc.name: SetCastPrivacyToRS
 * @tc.desc: SetCastPrivacyToRS test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetCastPrivacyToRS, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    ASSERT_NE(displayManagerAgent, nullptr);
    sptr<ScreenSession> defScreen = ssm_->GetScreenSession(DEFAULT_SCREEN_ID);
    ASSERT_EQ(ssm_->SetCastPrivacyToRS(defScreen, true), false);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto virtualScreenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> virtualSession = ssm_->GetScreenSession(virtualScreenId);
    ASSERT_EQ(ssm_->SetCastPrivacyToRS(virtualSession, true), false);
    ScreenId id = 2;
    sptr<ScreenSession> newSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(newSession, nullptr);
    newSession->GetScreenProperty().SetScreenType(ScreenType::REAL);
    ASSERT_EQ(ssm_->SetCastPrivacyToRS(newSession, true), true);
}

/**
 * @tc.name: RegisterSettingWireCastObserver
 * @tc.desc: RegisterSettingWireCastObserver
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RegisterSettingWireCastObserver, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    ASSERT_NE(displayManagerAgent, nullptr);
    ScreenId id = 2;
    sptr<ScreenSession> newSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(newSession, nullptr);
    ssm_->screenSessionMap_[id] = newSession;
    ssm_->RegisterSettingWireCastObserver(newSession);
}

/**
 * @tc.name: UnregisterSettingWireCastObserver
 * @tc.desc: UnregisterSettingWireCastObserver
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UnregisterSettingWireCastObserver, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    ASSERT_NE(displayManagerAgent, nullptr);
    ScreenId id = 2;
    sptr<ScreenSession> newSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(newSession, nullptr);
    ssm_->screenSessionMap_[id] = newSession;
    ssm_->UnregisterSettingWireCastObserver(id);
}

/**
 * @tc.name: MultiScreenChangeOuter
 * @tc.desc: MultiScreenChangeOuter
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, MultiScreenChangeOuter, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    EXPECT_EQ(ssm_->clientProxy_, nullptr);
    std::string outerFlag = "2";
    ssm_->MultiScreenChangeOuter(outerFlag);
    outerFlag = "0";
    ssm_->MultiScreenChangeOuter(outerFlag);
    outerFlag = "1";
    ssm_->MultiScreenChangeOuter(outerFlag);
}

/**
 * @tc.name: UpdateValidArea
 * @tc.desc: UpdateValidArea
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateValidArea, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->UpdateValidArea(2000, 800, 1000);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);

    int32_t originValidWidth = screenSession->GetValidWidth();
    int32_t originValidHeight = screenSession->GetValidHeight();
    ssm_->UpdateValidArea(screenId, 800, 1000);
    EXPECT_EQ(800, screenSession->GetValidWidth());
    EXPECT_EQ(1000, screenSession->GetValidHeight());
    ssm_->UpdateValidArea(screenId, originValidWidth, originValidHeight);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetIsRealScreen
 * @tc.desc: GetIsRealScreen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetIsRealScreen, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    EXPECT_EQ(ssm_->GetIsRealScreen(2000), false);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    screenSession->SetIsRealScreen(true);
    ASSERT_EQ(ssm_->GetIsRealScreen(screenId), true);
    screenSession->SetIsRealScreen(false);
    ASSERT_EQ(ssm_->GetIsRealScreen(screenId), false);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetSystemKeyboardStatus
 * @tc.desc: SetSystemKeyboardStatus with true as parameter
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetSystemKeyboardStatus01, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    auto ret = ssm_->SetSystemKeyboardStatus(true);
    ASSERT_NE(ret, DMError::DM_ERROR_UNKNOWN);
}
 
/**
 * @tc.name: SetSystemKeyboardStatus
 * @tc.desc: SetSystemKeyboardStatus with false as parameter
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetSystemKeyboardStatus02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    auto ret = ssm_->SetSystemKeyboardStatus(false);
    ASSERT_NE(ret, DMError::DM_ERROR_UNKNOWN);
}

/**
 * @tc.name: NotifyCastWhenSwitchScbNode
 * @tc.desc: NotifyCastWhenSwitchScbNode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyCastWhenSwitchScbNode, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    ASSERT_NE(displayManagerAgent, nullptr);

    ScreenId id = 2;
    sptr<ScreenSession> newSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    newSession ->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    ASSERT_NE(newSession, nullptr);
    ssm_->screenSessionMap_[id] = newSession;
    ScreenId id1 = 3;
    sptr<ScreenSession> newSession1 = new (std::nothrow) ScreenSession(id1, ScreenProperty(), 0);
    newSession1 ->SetScreenCombination(ScreenCombination::SCREEN_UNIQUE);
    ASSERT_NE(newSession1, nullptr);
    ssm_->screenSessionMap_[id1] = newSession1;
    ScreenId id2 = 4;
    sptr<ScreenSession> newSession2 = nullptr;
    ssm_->screenSessionMap_[id2] = newSession2;

    ssm_->NotifyCastWhenSwitchScbNode();
}
}
} // namespace Rosen
} // namespace OHOS
