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
#include "iremote_object_mocker.h"
#include "mock_accesstoken_kit.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const int32_t CV_WAIT_SCREENOFF_MS = 1500;
const int32_t CV_WAIT_SCREENON_MS = 300;
const int32_t CV_WAIT_SCREENOFF_MS_MAX = 3000;
const uint32_t INVALID_DISPLAY_ORIENTATION = 99;
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
constexpr int32_t CAST_WIRED_PROJECTION_START = 1005;
constexpr int32_t CAST_WIRED_PROJECTION_STOP = 1007;
bool g_isPcDevice = ScreenSceneConfig::GetExternalScreenDefaultMode() == "none";
const bool IS_SUPPORT_PC_MODE = system::GetBoolParameter("const.window.support_window_pcmode_switch", false);
std::string logMsg;
void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
    const char *msg)
{
    logMsg = msg;
}
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
    sptr<ScreenSession> InitTestScreenSession(std::string name, ScreenId &screenId);
    DMHookInfo CreateDefaultHookInfo();
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

sptr<ScreenSession> ScreenSessionManagerTest::InitTestScreenSession(std::string name, ScreenId &screenId)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = name;
    screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    auto rsid = ssm_->screenIdManager_.ConvertToRsScreenId(screenId);
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(name, screenId, rsid, 0);
    return screenSession;
}

DMHookInfo ScreenSessionManagerTest::CreateDefaultHookInfo()
{
    uint32_t hookWidth = 500;
    uint32_t hookHeight = 700;
    float_t hookDensity = 3.0;
    uint32_t hookRotation = static_cast<uint32_t>(Rotation::ROTATION_0);
    uint32_t hookDisplayOrientation = static_cast<uint32_t>(DisplayOrientation::PORTRAIT);
    DMHookInfo dmHookInfo = { hookWidth, hookHeight, hookDensity, hookRotation, true, hookDisplayOrientation, true };
    return dmHookInfo;
}

namespace {
/**
 * @tc.name: RegisterDisplayManagerAgent
 * @tc.desc: RegisterDisplayManagerAgent test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RegisterDisplayManagerAgent, TestSize.Level1)
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
 * @tc.name: CanEnterCoordinationRecording
 * @tc.desc: CanEnterCoordinationRecording test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CanEnterCoordinationRecording, TestSize.Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->virtualScreenCount_ = 1;
    ssm_->hdmiScreenCount_ = 0;

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOptionRecord";
    virtualOption.virtualScreenType_ = VirtualScreenType::SCREEN_RECORDING;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    auto ret = ssm_->CanEnterCoordination();
    EXPECT_EQ(ret, DMError::DM_ERROR_NOT_SUPPORT_COOR_WHEN_RECORDING);
    ssm_->DestroyVirtualScreen(screenId)
}


/**
 * @tc.name: WakeupBegin
 * @tc.desc: WakeupBegin test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpBegin, TestSize.Level1)
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

    ScreenStateMachine::GetInstance().SetTransitionState(ScreenTransitionState::SCREEN_INIT);
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
 * @tc.name: WakeUp for Multi
 * @tc.desc: WakeUp for Multi test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpForMulti, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));
    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_COLLABORATION;
    ASSERT_TRUE(ssm_->WakeUpBegin(reason));
    usleep(SLEEP_TIME_IN_US);
    ASSERT_TRUE(ssm_->WakeUpEnd());
    ASSERT_FALSE(ssm_->isMultiScreenCollaboration_);
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: SuspendBegin
 * @tc.desc: SuspendBegin test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SuspendBegin, TestSize.Level1)
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
 * @tc.name: Suspend for Multi
 * @tc.desc: Suspend for Multi test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SuspendforMulti, Function | SmallTest | Level3)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);

    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    EXPECT_EQ(DMError::DM_OK, ssm_->RegisterDisplayManagerAgent(displayManagerAgent, type));
    PowerStateChangeReason reason = PowerStateChangeReason::STATE_CHANGE_REASON_COLLABORATION;
    ASSERT_TRUE(ssm_->SuspendBegin(reason));
    ASSERT_TRUE(ssm_->isMultiScreenCollaboration_);
    usleep(SLEEP_TIME_IN_US);
    ASSERT_TRUE(ssm_->SuspendEnd());
    ASSERT_FALSE(ssm_->isMultiScreenCollaboration_);
    EXPECT_EQ(DMError::DM_OK, ssm_->UnregisterDisplayManagerAgent(displayManagerAgent, type));
}

/**
 * @tc.name: GetInternalScreenId
 * @tc.desc: GetInternalScreenId test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetInternalScreenId, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerById01, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerById02, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerById03, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll01, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll02, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll03, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll04, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll05, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll06, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll07, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll08, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll09, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForAll10, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, ScreenChange, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, ScreenPower, TestSize.Level1)
{
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
 * @tc.name: SyncScreenPowerState_StateON
 * @tc.desc: SyncScreenPowerState State is ON
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SyncScreenPowerState_StateON, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    ASSERT_NE(ssm_, nullptr) << "ScreenSessionManager instance is null";

    ssm_->SyncScreenPowerState(ScreenPowerState::POWER_ON);

    EXPECT_TRUE(g_logMsg.find("force sync power state") != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: SyncScreenPowerState_StateOFF
 * @tc.desc: SyncScreenPowerState State is OFF
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SyncScreenPowerState_StateOFF, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    ASSERT_NE(ssm_, nullptr) << "ScreenSessionManager instance is null";

    ssm_->SyncScreenPowerState(ScreenPowerState::POWER_OFF);

    EXPECT_TRUE(g_logMsg.find("force sync power state") != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: GetScreenPower
 * @tc.desc: GetScreenPower screen power
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenPower, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, IsScreenRotationLocked, TestSize.Level1)
{
    bool isLocked = false;
    DisplayId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_EQ(DMError::DM_OK, ssm_->IsScreenRotationLocked(isLocked));
}

/**
 * @tc.name: CreateScreenProperty
 * @tc.desc: CreateScreenProperty test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CreateScreenProperty, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "testVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    ScreenProperty property;
    ssm_->isDensityDpiLoad_ = false;
    ssm_->CreateScreenProperty(screenId, property);
    ssm_->isDensityDpiLoad_ = true;
    ASSERT_EQ(0, screenSession->GetScreenProperty().GetRefreshRate());
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetInternalWidth
 * @tc.desc: GetInternalWidth test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetInternalWidth, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    auto  screenId = ssm_->GetInternalScreenId();
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    ASSERT_EQ(screenSession, nullptr);
    ssm_->GetInternalWidth();
}

/**
 * @tc.name: InitExtendScreen01
 * @tc.desc: InitExtendScreen01 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitExtendScreen01, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "testVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    sptr<ScreenSession> screenSession1 = ssm_->GetScreenSession(ssm_->GetDefaultScreenId());
    ScreenProperty property = screenSession->GetScreenProperty();
    ssm_->InitExtendScreenProperty(screenId, screenSession, property);
    ssm_->InitExtendScreenDensity(screenSession, property);
    EXPECT_EQ(property.GetDensityInCurResolution(), screenSession1->GetScreenProperty().GetDensityInCurResolution());
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: InitExtendScreen
 * @tc.desc: InitExtendScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitExtendScreen02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "testVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    sptr<ScreenSession> screenSession1 = ssm_->GetScreenSession(ssm_->GetDefaultScreenId());
    ScreenProperty property = screenSession->GetScreenProperty();
    screenSession->GetScreenProperty().SetScreenType(ScreenType::REAL);
    ssm_->InitExtendScreenDensity(screenSession, property);
    EXPECT_EQ(property.GetDensityInCurResolution(), screenSession1->GetScreenProperty().GetDensityInCurResolution());
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetExtendedScreenFallbackPlan
 * @tc.desc: SetExtendedScreenFallbackPlan test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetExtendedScreenFallbackPlan, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->SetExtendedScreenFallbackPlan(3030);
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "testVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    screenSession->SetIsInternal(true);
    ssm_->SetExtendedScreenFallbackPlan(screenId);

    screenSession->SetIsInternal(false);
    ScreenProperty screenProperty = screenSession->GetScreenProperty();
    screenProperty.SetCurrentOffScreenRendering(false);
    ssm_->SetExtendedScreenFallbackPlan(screenId);

    screenProperty.SetCurrentOffScreenRendering(true);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    ASSERT_EQ(screenSession->displayNode_, nullptr);

    screenSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    ASSERT_EQ(screenSession->displayNode_, nullptr);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: BlockSetDisplayState
 * @tc.desc: BlockSetDisplayState test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, BlockSetDisplayState, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    EXPECT_TRUE(ssm_->BlockSetDisplayState());
}

/**
 * @tc.name: TryToCancelScreenOff
 * @tc.desc: TryToCancelScreenOff test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TryToCancelScreenOff, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    bool ret = ssm_->sessionDisplayPowerController_->canCancelSuspendNotify_;
    bool ret1 = ssm_->gotScreenOffNotify_;
    bool ret2 = ssm_->needScreenOffNotify_;
    ssm_->sessionDisplayPowerController_->canCancelSuspendNotify_ = true;
    ASSERT_EQ(true, ssm_->TryToCancelScreenOff());

    ssm_->sessionDisplayPowerController_->canCancelSuspendNotify_ = false;
    ssm_->gotScreenOffNotify_ = true;
    ASSERT_EQ(false, ssm_->TryToCancelScreenOff());

    ssm_->gotScreenOffNotify_ = false;
    ssm_->needScreenOffNotify_ = false;
    ASSERT_EQ(false, ssm_->TryToCancelScreenOff());

    ssm_->needScreenOffNotify_ = true;
    ASSERT_EQ(true, ssm_->TryToCancelScreenOff());
    ssm_->sessionDisplayPowerController_->canCancelSuspendNotify_ = ret;
    ssm_->gotScreenOffNotify_ = ret1;
    ssm_->needScreenOffNotify_ = ret2;
}

/**
 * @tc.name: ForceSkipScreenOffAnimation
 * @tc.desc: ForceSkipScreenOffAnimation test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ForceSkipScreenOffAnimation, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    bool ret = ssm_->sessionDisplayPowerController_->canCancelSuspendNotify_;
    bool ret1 = ssm_->gotScreenOffNotify_;
    bool ret2 = ssm_->needScreenOffNotify_;
    ssm_->sessionDisplayPowerController_->canCancelSuspendNotify_ = true;
    ssm_->ForceSkipScreenOffAnimation();
    ASSERT_TRUE(ssm_->sessionDisplayPowerController_->skipScreenOffBlock_);

    ssm_->sessionDisplayPowerController_->canCancelSuspendNotify_ = false;
    ssm_->gotScreenOffNotify_ = true;
    ssm_->ForceSkipScreenOffAnimation();
    ASSERT_TRUE(ssm_->sessionDisplayPowerController_->skipScreenOffBlock_);

    ssm_->gotScreenOffNotify_ = false;
    ssm_->needScreenOffNotify_ = false;
    ssm_->ForceSkipScreenOffAnimation();
    ASSERT_TRUE(ssm_->sessionDisplayPowerController_->skipScreenOffBlock_);

    ssm_->needScreenOffNotify_ = true;
    ssm_->ForceSkipScreenOffAnimation();
    ASSERT_TRUE(ssm_->sessionDisplayPowerController_->skipScreenOffBlock_);
    ssm_->sessionDisplayPowerController_->canCancelSuspendNotify_ = ret;
    ssm_->gotScreenOffNotify_ = ret1;
    ssm_->needScreenOffNotify_ = ret2;
}

/**
 * @tc.name: SetOrientation
 * @tc.desc: SetOrientation test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetOrientation, TestSize.Level1)
{
    Orientation orientation = Orientation::HORIZONTAL;
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_EQ(DMError::DM_OK, ssm_->SetOrientation(id, orientation, false));
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->SetOrientation(SCREEN_ID_INVALID, orientation, false));
    Orientation invalidOrientation = Orientation{20};
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetOrientation(id, invalidOrientation, false));
}

/**
 * @tc.name: SetRotationFromWindow
 * @tc.desc: SetRotationFromWindow test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetRotationFromWindow, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetDisplaySnapshot, TestSize.Level1)
{
    DisplayId displayId(0);
    DmErrorCode* errorCode = nullptr;
    ssm_->GetDisplaySnapshot(displayId, errorCode, false, false);
    EXPECT_TRUE(1);
}

/**
 * @tc.name: VirtualScreen
 * @tc.desc: ScreenSesionManager virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, VirtualScreen, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, AutoRotate, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetScreenSession, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetDefaultScreenSession, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetDefaultDisplayInfo, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, HookDisplayInfoByUid, TestSize.Level1)
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
 * @tc.name: HookDisplayInfoByUid02
 * @tc.desc: HookDisplayInfo by uid
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HookDisplayInfoByUid02, TestSize.Level1)
{
    ScreenId screenId;
    sptr<ScreenSession> screenSession = InitTestScreenSession("HookDisplayInfoByUid02", screenId);
    ASSERT_NE(ssm_->GetScreenSession(screenId), nullptr);
    sptr<DisplayInfo> displayInfo = ssm_->GetDefaultDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    uint32_t uid = getuid();
    DMHookInfo dmHookInfo = CreateDefaultHookInfo();
    ssm_->displayHookMap_[uid] = dmHookInfo;
    EXPECT_NE(ssm_->displayHookMap_.find(uid), ssm_->displayHookMap_.end());
    displayInfo = ssm_->HookDisplayInfoByUid(displayInfo, screenSession);
    EXPECT_EQ(displayInfo->GetWidth(), dmHookInfo.width_);
    EXPECT_EQ(displayInfo->GetHeight(), dmHookInfo.height_);
    EXPECT_EQ(displayInfo->GetVirtualPixelRatio(), dmHookInfo.density_);
    EXPECT_EQ(static_cast<uint32_t>(displayInfo->GetRotation()), dmHookInfo.rotation_);
    EXPECT_EQ(static_cast<uint32_t>(displayInfo->GetDisplayOrientation()), dmHookInfo.displayOrientation_);
    ssm_->displayHookMap_.erase(uid);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: HookDisplayInfoByUid03
 * @tc.desc: HookDisplayInfo by uid
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HookDisplayInfoByUid03, TestSize.Level1)
{
    ScreenId screenId;
    sptr<ScreenSession> screenSession = InitTestScreenSession("HookDisplayInfoByUid03", screenId);
    ASSERT_NE(ssm_->GetScreenSession(screenId), nullptr);
    sptr<DisplayInfo> displayInfo = ssm_->GetDefaultDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    uint32_t uid = getuid();
    DMHookInfo dmHookInfo = CreateDefaultHookInfo();
    dmHookInfo.enableHookDisplayOrientation_ = false;
    dmHookInfo.displayOrientation_ = INVALID_DISPLAY_ORIENTATION;
    ssm_->displayHookMap_[uid] = dmHookInfo;
    EXPECT_NE(ssm_->displayHookMap_.find(uid), ssm_->displayHookMap_.end());
    displayInfo = ssm_->HookDisplayInfoByUid(displayInfo, screenSession);
    EXPECT_EQ(displayInfo->GetWidth(), dmHookInfo.width_);
    EXPECT_EQ(displayInfo->GetHeight(), dmHookInfo.height_);
    EXPECT_EQ(displayInfo->GetVirtualPixelRatio(), dmHookInfo.density_);
    EXPECT_EQ(static_cast<uint32_t>(displayInfo->GetRotation()), dmHookInfo.rotation_);
    EXPECT_NE(static_cast<uint32_t>(displayInfo->GetDisplayOrientation()), dmHookInfo.displayOrientation_);
    ssm_->displayHookMap_.erase(uid);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: HookDisplayInfoByUid03
 * @tc.desc: HookDisplayInfo by uid
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HookDisplayInfoByUid04, TestSize.Level1)
{
    ScreenId screenId;
    sptr<ScreenSession> screenSession = InitTestScreenSession("HookDisplayInfoByUid04", screenId);
    ASSERT_NE(ssm_->GetScreenSession(screenId), nullptr);
    sptr<DisplayInfo> displayInfo = ssm_->GetDefaultDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    uint32_t uid = getuid();
    DMHookInfo dmHookInfo = CreateDefaultHookInfo();
    dmHookInfo.displayOrientation_ = INVALID_DISPLAY_ORIENTATION;
    ssm_->displayHookMap_[uid] = dmHookInfo;
    EXPECT_NE(ssm_->displayHookMap_.find(uid), ssm_->displayHookMap_.end());
    displayInfo = ssm_->HookDisplayInfoByUid(displayInfo, screenSession);
    EXPECT_EQ(displayInfo->GetWidth(), dmHookInfo.width_);
    EXPECT_EQ(displayInfo->GetHeight(), dmHookInfo.height_);
    EXPECT_EQ(displayInfo->GetVirtualPixelRatio(), dmHookInfo.density_);
    EXPECT_EQ(static_cast<uint32_t>(displayInfo->GetRotation()), dmHookInfo.rotation_);
    EXPECT_NE(static_cast<uint32_t>(displayInfo->GetDisplayOrientation()), dmHookInfo.displayOrientation_);
    ssm_->displayHookMap_.erase(uid);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetDisplayInfoById
 * @tc.desc: GetDisplayInfoById virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayInfoById, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetDisplayInfoByScreen, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetScreenInfoById, TestSize.Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "GetScreenInfoById";
    ASSERT_EQ(ssm_->GetScreenInfoById(1), nullptr);
}

/**
 * @tc.name: NotifyScreenChanged
 * @tc.desc: NotifyScreenChanged virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyScreenChanged, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, NotifyDisplayEvent, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetScreenInfoByDisplayId, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetScreenModesByDisplayId, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, UpdateDisplayHookInfo001, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, UpdateDisplayHookInfo002, TestSize.Level1)
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
 * @tc.name: GetDisplayHookInfo
 * @tc.desc: GetDisplayHookInfo by uid
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayHookInfo, Function | SmallTest | Level2)
{
    int32_t uid = 10086;
    DMHookInfo hookInfo;
    hookInfo.enableHookRotation_ = true;
    hookInfo.rotation_ = false;
    hookInfo.density_ = 2.5f;
    hookInfo.width_ = 1920;
    hookInfo.height_ = 1080;

    ssm_->UpdateDisplayHookInfo(uid, true, hookInfo);

    DMHookInfo actualInfo;
    actualInfo.enableHookRotation_ = false;
    actualInfo.rotation_ = true;
    actualInfo.density_ = -1.0f;
    actualInfo.width_ = -1;
    actualInfo.height_ = -1;
    ssm_->GetDisplayHookInfo(uid, actualInfo);

    EXPECT_EQ(actualInfo.enableHookRotation_, hookInfo.enableHookRotation_);
    EXPECT_EQ(actualInfo.rotation_, hookInfo.rotation_);
    EXPECT_FLOAT_EQ(actualInfo.density_, hookInfo.density_);
    EXPECT_EQ(actualInfo.width_, hookInfo.width_);
    EXPECT_EQ(actualInfo.height_, hookInfo.height_);
}

/**
 * @tc.name: NotifyIsFullScreenInForceSplitMode
 * @tc.desc: NotifyIsFullScreenInForceSplitMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyIsFullScreenInForceSplitMode, Function | SmallTest | Level2)
{
    int32_t uid = 0;
    MockAccesstokenKit::MockIsSystemApp(false);
    ssm_->NotifyIsFullScreenInForceSplitMode(uid, true);
    MockAccesstokenKit::MockIsSystemApp(true);
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
HWTEST_F(ScreenSessionManagerTest, SetVirtualPixelRatio, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetVirtualPixelRatioSystem, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetDefaultDensityDpi, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetResolution, TestSize.Level1)
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

    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->SetResolution(screenId, 0, 0, 0.5));

    ScreenId invalidScreenId = -1;
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->SetResolution(invalidScreenId, 100, 100, 0.5));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetScreenColorGamut
 * @tc.desc: GetScreenColorGamut virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenColorGamut, TestSize.Level1)
{
#ifdef WM_SCREEN_COLOR_GAMUT_ENABLE
    ScreenColorGamut colorGamut = ScreenColorGamut::COLOR_GAMUT_SRGB;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->GetScreenColorGamut(1, colorGamut));
    DisplayId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, ssm_->GetScreenColorGamut(id, colorGamut));
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
HWTEST_F(ScreenSessionManagerTest, LoadScreenSceneXml, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetScreenGamutMap, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, MakeExpand, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, DeleteScreenId, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, HasRsScreenId, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, AddAsFirstScreenLocked, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, AddAsSuccedentScreenLocked, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetMirror, TestSize.Level1)
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
    ASSERT_EQ(DMError::DM_OK, ssm_->SetMirror(2, screens, DMRect::NONE(), {Rotation::ROTATION_0, false}));
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->SetMirror(9, screens, DMRect::NONE(), {Rotation::ROTATION_0, false}));
    ASSERT_EQ(DMError::DM_OK, ssm_->SetMirror(screenId, screens, DMRect::NONE(), {Rotation::ROTATION_0, false}));
    auto mirrorscreen = ssm_->GetScreenSession(screenId);
    ASSERT_TRUE(mirrorscreen != nullptr);
    mirrorscreen->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->SetMirror(screenId, screens, DMRect::NONE(),
        {Rotation::ROTATION_0, false}));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetAbstractScreenGroup
 * @tc.desc: GetAbstractScreenGroup virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAbstractScreenGroup, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, InitAbstractScreenModesInfo, TestSize.Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "InitAbstractScreenModesInfo";

    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_EQ(false, ssm_->InitAbstractScreenModesInfo(screenSession));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: AddToGroupLocked
 * @tc.desc: AddToGroupLocked virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddToGroupLocked, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, InitVirtualScreen, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, InitAndGetScreen, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, RemoveFromGroupLocked, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, CreateAndGetNewScreenId, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, AddScreenToGroup, TestSize.Level1)
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
    ssm_->AddScreenToGroup(group, addScreens, addChildPos, removeChildResMap, {Rotation::ROTATION_0, false});
    sptr<ScreenSession> screenSession =new  (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ssm_->InitAndGetScreen(2));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: GetRSDisplayNodeByScreenId
 * @tc.desc: GetRSDisplayNodeByScreenId virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetRSDisplayNodeByScreenId, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetAllDisplayIds, TestSize.Level0)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenGamutMap, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, StopExpand, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, OnScreenDisconnect, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenColorGamut, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenColorTransform, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenRotationLocked, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, UpdateScreenRotationProperty, TestSize.Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "UpdateScreenRotationProperty";
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;
    int rotation = 1;
    ScreenPropertyChangeType screenPropertyChangeType = ScreenPropertyChangeType::ROTATION_BEGIN;
    ScreenSessionManager::GetInstance().UpdateScreenRotationProperty(1, bounds, 1, screenPropertyChangeType, false);
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(virtualOption,
        displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    ScreenSessionManager::GetInstance().UpdateScreenRotationProperty(screenId, bounds, rotation,
        screenPropertyChangeType, false);
    screenPropertyChangeType = ScreenPropertyChangeType::ROTATION_END;
    ScreenSessionManager::GetInstance().UpdateScreenRotationProperty(screenId, bounds, rotation,
        screenPropertyChangeType, false);
    screenPropertyChangeType = ScreenPropertyChangeType::ROTATION_UPDATE_PROPERTY_ONLY;
    ScreenSessionManager::GetInstance().UpdateScreenRotationProperty(screenId, bounds, rotation,
        screenPropertyChangeType, true);
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession();
    ASSERT_NE(screenSession, ScreenSessionManager::GetInstance().InitAndGetScreen(2));
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: MakeUniqueScreen
 * @tc.desc: Make unique screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, MakeUniqueScreen, TestSize.Level1)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    vector<ScreenId> screenIds;
    screenIds.clear();
    std::vector<DisplayId> displayIds;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, ssm_->MakeUniqueScreen(screenIds, displayIds,
        UniqueScreenRotationOptions()));
#endif
}

/**
 * @tc.name: ConvertScreenIdToRsScreenId
 * @tc.desc: convert screen id to RS screen id
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ConvertScreenIdToRsScreenId, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, DisableMirror, TestSize.Level1)
{
    ASSERT_EQ(DMError::DM_OK, ssm_->DisableMirror(false));
}

/**
 * @tc.name: HasImmersiveWindow
 * @tc.desc: HasImmersiveWindow test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HasImmersiveWindow, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetSpecifiedScreenPower, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, NotifyFoldStatusChanged, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, NotifyPrivateWindowListChanged, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    ASSERT_NE(ssm_, nullptr) << "ScreenSessionManager instance is null";

    DisplayId id = 100;
    std::vector<std::string> privacyWindowList{"win0", "win1"};
    ssm_->NotifyPrivateWindowListChanged(id, privacyWindowList);

    EXPECT_TRUE(g_logMsg.find("agent is null") != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: SetPrivacyStateByDisplayId01
 * @tc.desc: SetPrivacyStateByDisplayId true test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetPrivacyStateByDisplayId01, TestSize.Level1)
{
    DisplayId id = 0;
    bool hasPrivate = true;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    std::unordered_map<DisplayId, bool> privacyBundleDisplayId = {{id, hasPrivate}};
    ssm_->SetPrivacyStateByDisplayId(privacyBundleDisplayId);
    bool curHasPrivate = false;
    ssm_->HasPrivateWindow(id, curHasPrivate);
    EXPECT_TRUE(curHasPrivate);
}

/**
 * @tc.name: SetPrivacyStateByDisplayId02
 * @tc.desc: SetPrivacyStateByDisplayId false test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetPrivacyStateByDisplayId02, TestSize.Level1)
{
    DisplayId id = 0;
    bool hasPrivate = false;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    std::unordered_map<DisplayId, bool> privacyBundleDisplayId = {{id, hasPrivate}};
    ssm_->SetPrivacyStateByDisplayId(privacyBundleDisplayId);
    bool curHasPrivate = false;
    ssm_->HasPrivateWindow(id, curHasPrivate);
    EXPECT_FALSE(curHasPrivate);
}

/**
 * @tc.name: SetScreenPrivacyWindowList
 * @tc.desc: SetScreenPrivacyWindowList test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPrivacyWindowList, Function | SmallTest | Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSystemApp(false);
    MockAccesstokenKit::MockIsSACalling(false);

    DisplayId invalidId = 999;
    std::vector<std::string> windowList{ "win0" };
    ScreenSessionManager::GetInstance().SetScreenPrivacyWindowList(invalidId, windowList);

    std::string expectedKeyword = "Permission Denied";
    bool found = (g_logMsg.find(expectedKeyword) != std::string::npos);
    EXPECT_TRUE(found) << "Expected log to contain '" << expectedKeyword
                       << "', but got: " << g_logMsg;

    MockAccesstokenKit::ChangeMockStateToInit();
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: GetAllScreenIds
 * @tc.desc: GetAllScreenIds screen power
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAllScreenIds, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetAllScreenInfos, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetScreenSupportedColorGamuts, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetPixelFormat, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetPixelFormat, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetSupportedHDRFormats, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetScreenHDRFormat, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenHDRFormat, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetSupportedColorSpaces, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetScreenColorSpace, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenColorSpace, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, HasPrivateWindow, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetAvailableArea01, TestSize.Level1)
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
 * @tc.name: GetAvailableArea
 * @tc.desc: GetAvailableArea test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAvailableArea02, TestSize.Level1)
{
    DMRect area;
    DisplayId id = 999;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_.clear();
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->GetAvailableArea(id, area));
    ssm_->screenSessionMap_.clear();
}


/**
 * @tc.name: GetExpandAvailableArea
 * @tc.desc: GetExpandAvailableArea test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetExpandAvailableArea02, TestSize.Level1)
{
    DMRect area;
    DisplayId id = 0;
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->GetExpandAvailableArea(id, area));
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_.clear();
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(DMError::DM_OK, ssm_->GetExpandAvailableArea(id, area));
    ssm_->screenSessionMap_.clear();
}

/**
 * @tc.name: ResetAllFreezeStatus
 * @tc.desc: ResetAllFreezeStatus test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ResetAllFreezeStatus, TestSize.Level1)
{
    EXPECT_EQ(DMError::DM_OK, ssm_->ResetAllFreezeStatus());
}

/**
 * @tc.name: SetVirtualScreenRefreshRate
 * @tc.desc: SetVirtualScreenRefreshRate test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenRefreshRate, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenFlag, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetVirtualScreenFlag, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, ResizeVirtualScreen, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetVirtualMirrorScreenScaleMode, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, StopMirror, TestSize.Level1)
{
    std::vector<ScreenId> mirrorScreenIds {0, 1, 2, 3, 4, 5};
    EXPECT_EQ(DMError::DM_OK, ssm_->StopMirror(mirrorScreenIds));
}

/**
 * @tc.name: GetDensityInCurResolution
 * @tc.desc: GetDensityInCurResolution screen power
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDensityInCurResolution, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenOffDelayTime, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetDeviceScreenConfig, TestSize.Level1)
{
    DeviceScreenConfig deviceScreenConfig = ssm_->GetDeviceScreenConfig();
    EXPECT_FALSE(deviceScreenConfig.rotationPolicy_.empty());
}

/**
 * @tc.name: SetVirtualScreenBlackList
 * @tc.desc: SetVirtualScreenBlackList test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenBlackList01, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenBlackList02, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetAllDisplayPhysicalResolution01, TestSize.Level1)
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
 * @tc.name: GetAllDisplayPhysicalResolution
 * @tc.desc: GetAllDisplayPhysicalResolution test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAllDisplayPhysicalResolution02, TestSize.Level1)
{
    ssm_->allDisplayPhysicalResolution_.clear();
    ssm_->allDisplayPhysicalResolution_.emplace_back(DisplayPhysicalResolution());
    std::vector<DisplayPhysicalResolution> result = ssm_->GetAllDisplayPhysicalResolution();
    EXPECT_TRUE(!result.empty());
    ssm_->allDisplayPhysicalResolution_.clear();
}

/**
 * @tc.name: GetAllDisplayPhysicalResolution
 * @tc.desc: GetAllDisplayPhysicalResolution test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAllDisplayPhysicalResolution03, TestSize.Level1)
{
    DisplayPhysicalResolution resolution;
    resolution.physicalWidth_ = 1920;
    resolution.physicalHeight_ = 1080;
    ssm_->allDisplayPhysicalResolution_.clear();
    ssm_->allDisplayPhysicalResolution_.push_back(resolution);
    std::vector<DisplayPhysicalResolution> result = ssm_->GetAllDisplayPhysicalResolution();
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].physicalWidth_, 1920);
    EXPECT_EQ(result[0].physicalHeight_, 1080);
    ssm_->allDisplayPhysicalResolution_.clear();
}

/**
 * @tc.name: GetAllDisplayPhysicalResolution
 * @tc.desc: GetAllDisplayPhysicalResolution test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetAllDisplayPhysicalResolution04, TestSize.Level1)
{
    ssm_->allDisplayPhysicalResolution_.clear();
    ssm_->allDisplayPhysicalResolution_.emplace_back(DisplayPhysicalResolution());
    ssm_->allDisplayPhysicalResolution_.back().foldDisplayMode_ = FoldDisplayMode::GLOBAL_FULL;
    std::vector<DisplayPhysicalResolution> resolutions = ssm_->GetAllDisplayPhysicalResolution();
    EXPECT_EQ(resolutions.back().foldDisplayMode_, FoldDisplayMode::FULL);
    ssm_->allDisplayPhysicalResolution_.clear();
}

/**
 * @tc.name: SetDisplayScale
 * @tc.desc: SetDisplayScale test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetDisplayScale, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, ScreenCastConnection, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, ReportFoldStatusToScb, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, DisablePowerOffRenderControl01, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, DisablePowerOffRenderControl02, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, CheckAndSendHiSysEvent, TestSize.Level1)
{
    std::string eventName =  "GET_DISPLAY_SNAPSHOT";
    std::string bundleName = "hmos.screenshot";
    ssm_->CheckAndSendHiSysEvent(eventName, bundleName);
    ASSERT_NE(ssm_, nullptr);
}

/**
 * @tc.name: ProxyForFreeze
 * @tc.desc: ProxyForFreeze
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ProxyForFreeze, TestSize.Level1)
{
    int32_t pid = 1;
    std::set<int32_t> unfreezedPidList = {1, 2, 3};
    std::set<DisplayManagerAgentType> pidAgentTypes = {DisplayManagerAgentType::SCREEN_EVENT_LISTENER};
    ScreenId screenId = 1050;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ssm_->NotifyUnfreezedAgents(pid, unfreezedPidList, pidAgentTypes, screenSession);

    ssm_->NotifyUnfreezed(unfreezedPidList, screenSession);

    std::set<int32_t> pidList = {1, 2, 3};
    DMError ret = ssm_->ProxyForFreeze(pidList, true);
    ASSERT_EQ(ret, DMError::DM_OK);

    ret = ssm_->ProxyForFreeze(pidList, false);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: GetOrCreateFakeScreenSession
 * @tc.desc: GetOrCreateFakeScreenSession
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetOrCreateFakeScreenSession, TestSize.Level1)
{
    ScreenId screenId = 1050;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    auto ret = ssm_->GetOrCreateFakeScreenSession(screenSession);
    ssm_->InitFakeScreenSession(screenSession);
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: SetVirtualScreenSecurityExemption
 * @tc.desc: SetVirtualScreenSecurityExemption
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenSecurityExemption, TestSize.Level1)
{
    ScreenId screenId = 0;
    uint32_t pid = 1111;
    std::vector<uint64_t> windowList = {11, 22, 33};
    auto ret = ssm_->SetVirtualScreenSecurityExemption(screenId, pid, windowList);
    ASSERT_EQ(ret, DMError::DM_ERROR_UNKNOWN);
}

/**
 * @tc.name: NotifyFoldToExpandCompletion
 * @tc.desc: NotifyFoldToExpandCompletion
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyFoldToExpandCompletion, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, UpdateAvailableArea01, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, UpdateAvailableArea02, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, UpdateAvailableArea03, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, NotifyAvailableAreaChanged01, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, TriggerFoldStatusChange01, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, NotifyFoldStatusChanged02, TestSize.Level1)
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

    if (FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        ASSERT_EQ(ssm_->foldScreenController_, nullptr);
        GTEST_SKIP();
    }
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
HWTEST_F(ScreenSessionManagerTest, Dump, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetDisplayNode01, TestSize.Level1)
{
    MockAccesstokenKit::MockIsSystemApp(true);
    ScreenId screenId = 1051;
    auto ret = ssm_->GetDisplayNode(screenId);
    ASSERT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetDisplayNode
 * @tc.desc: !screenSession = false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayNode02, TestSize.Level1)
{
    MockAccesstokenKit::MockIsSystemApp(true);
    ScreenId screenId = 1050;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(screenSession, nullptr);
    ssm_->screenSessionMap_[screenId] = screenSession;
    auto ret = ssm_->GetDisplayNode(screenId);
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: GetDisplayNode
 * @tc.desc: SystemCalling false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayNode03, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSystemApp(false);
    MockAccesstokenKit::MockIsSACalling(false);
    ScreenId screenId = 1050;
    ssm_->GetDisplayNode(screenId);
    EXPECT_TRUE(logMsg.find("Permission Denied") != std::string::npos);
}

/**
 * @tc.name: GetScreenProperty
 * @tc.desc: GetScreenProperty
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenProperty01, TestSize.Level0)
{
    MockAccesstokenKit::MockIsSystemApp(true);
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
HWTEST_F(ScreenSessionManagerTest, GetScreenProperty02, TestSize.Level0)
{
    MockAccesstokenKit::MockIsSystemApp(true);
    ScreenId screenId = 1050;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(screenSession, nullptr);
    ssm_->screenSessionMap_[screenId] = screenSession;
    ScreenProperty property = ssm_->GetScreenProperty(screenId);
    ASSERT_EQ(sizeof(property), sizeof(screenSession->property_));
}

/**
 * @tc.name: GetScreenProperty
 * @tc.desc: SystemCalling false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenProperty03, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSystemApp(false);
    MockAccesstokenKit::MockIsSACalling(false);
    ScreenId screenId = 1050;
    ssm_->GetScreenProperty(screenId);
    EXPECT_TRUE(logMsg.find("Permission Denied") != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: GetCurrentScreenPhyBounds
 * @tc.desc: GetCurrentScreenPhyBounds
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCurrentScreenPhyBounds01, TestSize.Level1)
{
#ifdef FOLD_ABILITY_ENABLE
    float phyWidth = 0.0f;
    float phyHeight = 0.0f;
    bool isReset = true;
    ScreenId screenId = 1050;
    ssm_->GetCurrentScreenPhyBounds(phyWidth, phyHeight, isReset, screenId);
    if (FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        ASSERT_EQ(isReset, true);
        GTEST_SKIP();
    } else {
        ASSERT_NE(isReset, true);
    }
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
 * @tc.name: GetCurrentScreenPhyBounds
 * @tc.desc: GetCurrentScreenPhyBounds
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCurrentScreenPhyBounds02, TestSize.Level1)
{
#ifdef FOLD_ABILITY_ENABLE
    float phyWidth = 0.0f;
    float phyHeight = 0.0f;
    bool isReset = true;
    ScreenId screenId = 0;
    ssm_->GetCurrentScreenPhyBounds(phyWidth, phyHeight, isReset, screenId);
    auto foldController = sptr<FoldScreenController>::MakeSptr(ssm_->displayInfoMutex_,
        ssm_->screenPowerTaskScheduler_);
    ASSERT_NE(foldController, nullptr);
    DisplayPhysicalResolution physicalSize_full;
    physicalSize_full.foldDisplayMode_ = FoldDisplayMode::FULL;
    physicalSize_full.physicalWidth_ = 2048;
    physicalSize_full.physicalHeight_ = 2232;
    DisplayPhysicalResolution physicalSize_main;
    physicalSize_main.foldDisplayMode_ = FoldDisplayMode::MAIN;
    physicalSize_main.physicalWidth_ = 1008;
    physicalSize_main.physicalHeight_ = 2232;
    DisplayPhysicalResolution physicalSize_global_full;
    physicalSize_global_full.foldDisplayMode_ = FoldDisplayMode::GLOBAL_FULL;
    physicalSize_global_full.physicalWidth_ = 3184;
    physicalSize_global_full.physicalHeight_ = 2232;
    ScreenSceneConfig::displayPhysicalResolution_.emplace_back(physicalSize_full);
    ScreenSceneConfig::displayPhysicalResolution_.emplace_back(physicalSize_main);
    ScreenSceneConfig::displayPhysicalResolution_.emplace_back(physicalSize_global_full);
    auto foldPolicy = foldController->GetFoldScreenPolicy(DisplayDeviceType::SECONDARY_DISPLAY_DEVICE);
    ASSERT_NE(foldPolicy, nullptr);
    foldPolicy->lastDisplayMode_ = FoldDisplayMode::GLOBAL_FULL;
    foldController->foldScreenPolicy_ = foldPolicy;
    ssm_->foldScreenController_ = foldController;
    ssm_->GetCurrentScreenPhyBounds(phyWidth, phyHeight, isReset, screenId);
    auto phyBounds = ssm_->GetPhyScreenProperty(0).GetPhyBounds();
    float phyWidthNow = phyBounds.rect_.width_;
    float phyHeightNow = phyBounds.rect_.width_;
    int32_t screenRotationOffSet = system::GetIntParameter<int32_t>("const.fold.screen_rotation.offset", 0);
    if (FoldScreenStateInternel::IsDualDisplayFoldDevice()) {
        EXPECT_EQ(phyWidth, phyWidthNow);
        EXPECT_EQ(phyHeight, phyHeightNow);
    } else if (screenRotationOffSet == 1 || screenRotationOffSet == 3) {
        EXPECT_EQ(phyHeight, phyHeightNow);
        EXPECT_EQ(phyWidth, phyWidthNow);
    } else {
        EXPECT_EQ(phyWidth, phyWidthNow);
        EXPECT_EQ(phyHeight, phyHeightNow);
    }
#endif
}

/**
 * @tc.name: PhyMirrorConnectWakeupScreen
 * @tc.desc: PhyMirrorConnectWakeupScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, PhyMirrorConnectWakeupScreen, TestSize.Level1)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    ssm_->PhyMirrorConnectWakeupScreen();
    ScreenSceneConfig::stringConfig_["externalScreenDefaultMode"] = "mirror";
    ssm_->PhyMirrorConnectWakeupScreen();
#endif
}

/**
 * @tc.name: GetIsCurrentInUseById01
 * @tc.desc: GetIsCurrentInUseById01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetIsCurrentInUseById01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId screenId = 50;
    auto res = ssm->GetIsCurrentInUseById(screenId);
    ASSERT_EQ(false, res);
}

/**
 * @tc.name: GetIsCurrentInUseById02
 * @tc.desc: GetIsCurrentInUseById02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetIsCurrentInUseById02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId screenId = 50;
    sptr<ScreenSession> screenSession = new ScreenSession();
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetIsCurrentInUse(false);
    ssm->screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    auto res = ssm->GetIsCurrentInUseById(screenId);
    ASSERT_EQ(false, res);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetIsCurrentInUseById03
 * @tc.desc: GetIsCurrentInUseById03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetIsCurrentInUseById03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId screenId = 50;
    sptr<ScreenSession> screenSession = new ScreenSession();
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetIsCurrentInUse(true);
    ssm->screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    auto res = ssm->GetIsCurrentInUseById(screenId);
    ASSERT_EQ(true, res);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: ReportHandleScreenEvent01
 * @tc.desc: ReportHandleScreenEvent01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ReportHandleScreenEvent01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenEvent screenEvent = ScreenEvent::CONNECTED;
    ScreenCombination screenCombination = ScreenCombination::SCREEN_MIRROR;
    ssm->ReportHandleScreenEvent(screenEvent, screenCombination);
}

/**
 * @tc.name: ReportHandleScreenEvent02
 * @tc.desc: ReportHandleScreenEvent02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ReportHandleScreenEvent02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenEvent screenEvent = ScreenEvent::CONNECTED;
    ScreenCombination screenCombination = ScreenCombination::SCREEN_EXTEND;
    ssm->ReportHandleScreenEvent(screenEvent, screenCombination);
}

/**
 * @tc.name: RegisterSettingWireCastObserver01
 * @tc.desc: RegisterSettingWireCastObserver01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RegisterSettingWireCastObserver01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(screenSession, nullptr);
    ssm->RegisterSettingWireCastObserver(screenSession);
}

/**
 * @tc.name: UnregisterSettingWireCastObserver01
 * @tc.desc: UnregisterSettingWireCastObserver01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UnregisterSettingWireCastObserver01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId screenId = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ssm->screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    ASSERT_EQ(screenSession, nullptr);
    ssm->UnregisterSettingWireCastObserver(screenId);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: UnregisterSettingWireCastObserver02
 * @tc.desc: UnregisterSettingWireCastObserver02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UnregisterSettingWireCastObserver02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId screenId = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    ssm->screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    ssm->UnregisterSettingWireCastObserver(60);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: UnregisterSettingWireCastObserver03
 * @tc.desc: UnregisterSettingWireCastObserver03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UnregisterSettingWireCastObserver03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId screenId = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    ssm->screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    ssm->UnregisterSettingWireCastObserver(50);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: IsPhysicalScreenAndInUse01
 * @tc.desc: IsPhysicalScreenAndInUse01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, IsPhysicalScreenAndInUse01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId screenId = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetIsCurrentInUse(true);
    ssm->screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    auto res = ssm->IsPhysicalScreenAndInUse(screenSession);
    ASSERT_EQ(true, res);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: IsPhysicalScreenAndInUse02
 * @tc.desc: IsPhysicalScreenAndInUse02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, IsPhysicalScreenAndInUse02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId screenId = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::VIRTUAL);
    screenSession->SetIsCurrentInUse(true);
    ssm->screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    auto res = ssm->IsPhysicalScreenAndInUse(screenSession);
    ASSERT_EQ(false, res);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: IsPhysicalScreenAndInUse03
 * @tc.desc: IsPhysicalScreenAndInUse03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, IsPhysicalScreenAndInUse03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId screenId = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetIsCurrentInUse(false);
    ssm->screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    auto res = ssm->IsPhysicalScreenAndInUse(screenSession);
    ASSERT_EQ(false, res);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: IsPhysicalScreenAndInUse04
 * @tc.desc: IsPhysicalScreenAndInUse04
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, IsPhysicalScreenAndInUse04, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId screenId = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::VIRTUAL);
    screenSession->SetIsCurrentInUse(false);
    ssm->screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    auto res = ssm->IsPhysicalScreenAndInUse(screenSession);
    ASSERT_EQ(false, res);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: IsPhysicalScreenAndInUse05
 * @tc.desc: IsPhysicalScreenAndInUse05
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, IsPhysicalScreenAndInUse05, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(screenSession, nullptr);
    auto res = ssm->IsPhysicalScreenAndInUse(screenSession);
    ASSERT_EQ(false, res);
}

/**
 * @tc.name: HookDisplayInfoByUid01
 * @tc.desc: HookDisplayInfo by uid01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HookDisplayInfoByUid01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    sptr<DisplayInfo> displayInfo = nullptr;
    ASSERT_EQ(displayInfo, nullptr);
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(screenSession, nullptr);
    auto res = ssm->HookDisplayInfoByUid(displayInfo, screenSession);
    ASSERT_EQ(res, nullptr);
}

/**
 * @tc.name: GetDisplayInfoById01
 * @tc.desc: GetDisplayInfoById01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayInfoById01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    DisplayId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(screenSession, nullptr);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetDisplayInfoById(id);
    ASSERT_EQ(res, nullptr);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetVisibleAreaDisplayInfoById01
 * @tc.desc: GetVisibleAreaDisplayInfoById01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetVisibleAreaDisplayInfoById01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    DisplayId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(screenSession, nullptr);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetVisibleAreaDisplayInfoById(id);
    ASSERT_EQ(res, nullptr);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetVisibleAreaDisplayInfoById02
 * @tc.desc: GetVisibleAreaDisplayInfoById02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetVisibleAreaDisplayInfoById02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    DisplayId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetVisibleAreaDisplayInfoById(id);
    ASSERT_NE(res, nullptr);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetDisplayInfoByScreen01
 * @tc.desc: GetDisplayInfoByScreen01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayInfoByScreen01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(screenSession, nullptr);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetDisplayInfoByScreen(id);
    ASSERT_EQ(res, nullptr);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetDisplayInfoByScreen02
 * @tc.desc: GetDisplayInfoByScreen02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayInfoByScreen02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetDisplayInfoByScreen(id);
    ASSERT_NE(res, nullptr);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetFakeDisplayId01
 * @tc.desc: GetFakeDisplayId01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetFakeDisplayId01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    sptr<ScreenSession> fakeScreenSession = nullptr;
    ASSERT_EQ(fakeScreenSession, nullptr);
    screenSession->SetFakeScreenSession(fakeScreenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetFakeDisplayId(screenSession);
    ASSERT_EQ(res, DISPLAY_ID_INVALID);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetFakeDisplayId02
 * @tc.desc: GetFakeDisplayId02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetFakeDisplayId02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    sptr<ScreenSession> fakeScreenSession = new ScreenSession(999, ScreenProperty(), 0);
    ASSERT_NE(nullptr, fakeScreenSession);
    screenSession->SetFakeScreenSession(fakeScreenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetFakeDisplayId(screenSession);
    ASSERT_NE(res, DISPLAY_ID_INVALID);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalculateXYPosition01
 * @tc.desc: CalculateXYPosition01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalculateXYPosition01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(screenSession, nullptr);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalculateXYPosition(screenSession);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalculateXYPosition02
 * @tc.desc: CalculateXYPosition02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalculateXYPosition02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetIsInternal(true);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalculateXYPosition(screenSession);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalculateXYPosition03
 * @tc.desc: CalculateXYPosition03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalculateXYPosition03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetIsInternal(false);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ScreenId internalId = 51;
    sptr<ScreenSession> internalSession = new ScreenSession(internalId, ScreenProperty(), 0);
    ASSERT_NE(nullptr, internalSession);
    internalSession->SetScreenType(ScreenType::REAL);
    internalSession->SetIsInternal(true);
    ssm->screenSessionMap_.insert(std::make_pair(internalId, internalSession));
    ssm->CalculateXYPosition(screenSession);
    ssm->screenSessionMap_.erase(50);
    ssm->screenSessionMap_.erase(51);
}

/**
 * @tc.name: NotifyAndPublishEvent01
 * @tc.desc: NotifyAndPublishEvent01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyAndPublishEvent01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    ASSERT_NE(nullptr, displayInfo);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    ssm->NotifyAndPublishEvent(displayInfo, id, screenSession);
}

/**
 * @tc.name: NotifyAndPublishEvent02
 * @tc.desc: NotifyAndPublishEvent02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyAndPublishEvent02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    ASSERT_NE(nullptr, displayInfo);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    ssm->NotifyAndPublishEvent(displayInfo, id, screenSession);
}

/**
 * @tc.name: NotifyAndPublishEvent03
 * @tc.desc: NotifyAndPublishEvent03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyAndPublishEvent03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    sptr<DisplayInfo> displayInfo = nullptr;
    ASSERT_EQ(nullptr, displayInfo);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    ssm->NotifyAndPublishEvent(displayInfo, id, screenSession);
}

/**
 * @tc.name: UpdateScreenDirectionInfo01
 * @tc.desc: UpdateScreenDirectionInfo01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateScreenDirectionInfo01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(screenSession, nullptr);
    ScreenDirectionInfo directionInfo;
    directionInfo.screenRotation_ = 0.0f;
    directionInfo.rotation_ = 0.0f;
    directionInfo.phyRotation_ = 0.0f;
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;
    ScreenPropertyChangeType screenPropertyChangeType = ScreenPropertyChangeType::ROTATION_END;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->UpdateScreenDirectionInfo(id, directionInfo, screenPropertyChangeType, bounds);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: UpdateScreenDirectionInfo02
 * @tc.desc: UpdateScreenDirectionInfo02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateScreenDirectionInfo02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(screenSession, nullptr);
    ScreenDirectionInfo directionInfo;
    directionInfo.screenRotation_ = 0.0f;
    directionInfo.rotation_ = 0.0f;
    directionInfo.phyRotation_ = 0.0f;
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;
    ScreenPropertyChangeType screenPropertyChangeType = ScreenPropertyChangeType::UNSPECIFIED;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->UpdateScreenDirectionInfo(id, directionInfo, screenPropertyChangeType, bounds);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: UpdateScreenDirectionInfo03
 * @tc.desc: UpdateScreenDirectionInfo03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateScreenDirectionInfo03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    ScreenDirectionInfo directionInfo;
    directionInfo.screenRotation_ = 0.0f;
    directionInfo.rotation_ = 0.0f;
    directionInfo.phyRotation_ = 0.0f;
    RRect bounds;
    bounds.rect_.width_ = 1344;
    bounds.rect_.height_ = 2772;
    ScreenPropertyChangeType screenPropertyChangeType = ScreenPropertyChangeType::UNSPECIFIED;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->UpdateScreenDirectionInfo(id, directionInfo, screenPropertyChangeType, bounds);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetRSDisplayNodeByScreenId01
 * @tc.desc: GetRSDisplayNodeByScreenId01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetRSDisplayNodeByScreenId01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(screenSession, nullptr);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetRSDisplayNodeByScreenId(id);
    ASSERT_EQ(res, nullptr);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetRSDisplayNodeByScreenId02
 * @tc.desc: GetRSDisplayNodeByScreenId02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetRSDisplayNodeByScreenId02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->ReleaseDisplayNode();
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetRSDisplayNodeByScreenId(id);
    ASSERT_EQ(res, nullptr);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: OnScreenConnect
 * @tc.desc: OnScreenConnect
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnScreenConnect, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    sptr<ScreenInfo> screenInfo = nullptr;
    ASSERT_EQ(nullptr, screenInfo);
    ssm->OnScreenConnect(screenInfo);
}

/**
 * @tc.name: TransferPropertyChangeTypeToString05
 * @tc.desc: TransferPropertyChangeTypeToString05
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TransferPropertyChangeTypeToString05, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    std::string screenType = ssm->TransferPropertyChangeTypeToString(
        ScreenPropertyChangeType::ROTATION_UPDATE_PROPERTY_ONLY_NOT_NOTIFY);
    std::string expectType = "ROTATION_UPDATE_PROPERTY_ONLY_NOT_NOTIFY";
    ASSERT_EQ(screenType, expectType);
}

/**
 * @tc.name: TransferPropertyChangeTypeToString06
 * @tc.desc: TransferPropertyChangeTypeToString06
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TransferPropertyChangeTypeToString06, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    std::string screenType = ssm->TransferPropertyChangeTypeToString(
        ScreenPropertyChangeType::UNDEFINED);
    std::string expectType = "UNDEFINED";
    ASSERT_EQ(screenType, expectType);
}

/**
 * @tc.name: UpdateDisplayScaleState01
 * @tc.desc: UpdateDisplayScaleState01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateDisplayScaleState01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(screenSession, nullptr);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->UpdateDisplayScaleState(id);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: UpdateDisplayScaleState02
 * @tc.desc: UpdateDisplayScaleState02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateDisplayScaleState02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    float scaleX = 3.0f;
    float scaleY = 3.0f;
    float pivotX = 3.0f;
    float pivotY = 3.0f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    screenSession->SetScreenScale(scaleX, scaleY, pivotX, pivotY, translateX, translateY);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->UpdateDisplayScaleState(id);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: SetDisplayScaleInner
 * @tc.desc: SetDisplayScaleInner
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetDisplayScaleInner, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = -0.5f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->SetDisplayScaleInner(id, scaleX, scaleY, pivotX, pivotY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnFoldableRotation01
 * @tc.desc: CalcDisplayNodeTranslateOnFoldableRotation01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnFoldableRotation01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = -0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnFoldableRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnFoldableRotation02
 * @tc.desc: CalcDisplayNodeTranslateOnFoldableRotation02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnFoldableRotation02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetRotation(Rotation::ROTATION_0);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnFoldableRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnFoldableRotation03
 * @tc.desc: CalcDisplayNodeTranslateOnFoldableRotation03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnFoldableRotation03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetRotation(Rotation::ROTATION_90);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnFoldableRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnFoldableRotation04
 * @tc.desc: CalcDisplayNodeTranslateOnFoldableRotation04
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnFoldableRotation04, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetRotation(Rotation::ROTATION_180);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnFoldableRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnFoldableRotation05
 * @tc.desc: CalcDisplayNodeTranslateOnFoldableRotation05
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnFoldableRotation05, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetRotation(Rotation::ROTATION_270);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnFoldableRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnPocketFoldRotation01
 * @tc.desc: CalcDisplayNodeTranslateOnPocketFoldRotation01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnPocketFoldRotation01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = -0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnPocketFoldRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnPocketFoldRotation02
 * @tc.desc: CalcDisplayNodeTranslateOnPocketFoldRotation02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnPocketFoldRotation02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetRotation(Rotation::ROTATION_0);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnPocketFoldRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnPocketFoldRotation03
 * @tc.desc: CalcDisplayNodeTranslateOnPocketFoldRotation03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnPocketFoldRotation03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetRotation(Rotation::ROTATION_90);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnPocketFoldRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnPocketFoldRotation04
 * @tc.desc: CalcDisplayNodeTranslateOnPocketFoldRotation04
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnPocketFoldRotation04, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetRotation(Rotation::ROTATION_180);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnPocketFoldRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnPocketFoldRotation05
 * @tc.desc: CalcDisplayNodeTranslateOnPocketFoldRotation05
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnPocketFoldRotation05, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetRotation(Rotation::ROTATION_270);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnPocketFoldRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnRotation01
 * @tc.desc: CalcDisplayNodeTranslateOnRotation01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnRotation01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = -0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnRotation02
 * @tc.desc: CalcDisplayNodeTranslateOnRotation02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnRotation02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetRotation(Rotation::ROTATION_90);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnRotation03
 * @tc.desc: CalcDisplayNodeTranslateOnRotation03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnRotation03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetRotation(Rotation::ROTATION_180);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CalcDisplayNodeTranslateOnRotation04
 * @tc.desc: CalcDisplayNodeTranslateOnRotation04
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalcDisplayNodeTranslateOnRotation04, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetRotation(Rotation::ROTATION_270);
    float scaleX = 0.5f;
    float scaleY = 3.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    float translateX = 0.0f;
    float translateY = 0.0f;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->CalcDisplayNodeTranslateOnRotation(screenSession, scaleX, scaleY, pivotX, pivotY, translateX,
        translateY);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: OnScreenChange
 * @tc.desc: OnScreenChange
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnScreenChange, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    ScreenEvent screenEvent = ScreenEvent::CONNECTED;
    ScreenChangeReason reason = ScreenChangeReason::HWCDEAD;
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->OnScreenChange(id, screenEvent, reason);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetCurrentInUseScreenNumber01
 * @tc.desc: GetCurrentInUseScreenNumber01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCurrentInUseScreenNumber01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetCurrentInUseScreenNumber();
    ASSERT_EQ(0, res);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetCurrentInUseScreenNumber02
 * @tc.desc: GetCurrentInUseScreenNumber02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCurrentInUseScreenNumber02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetIsCurrentInUse(false);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetCurrentInUseScreenNumber();
    ASSERT_EQ(0, res);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetCurrentInUseScreenNumber03
 * @tc.desc: GetCurrentInUseScreenNumber03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCurrentInUseScreenNumber03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetIsCurrentInUse(true);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetCurrentInUseScreenNumber();
    ASSERT_EQ(1, res);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: GetCurrentInUseScreenNumber04
 * @tc.desc: GetCurrentInUseScreenNumber04
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCurrentInUseScreenNumber04, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::UNDEFINED);
    screenSession->SetIsCurrentInUse(true);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    auto res = ssm->GetCurrentInUseScreenNumber();
    ASSERT_EQ(0, res);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: SetCastPrivacyFromSettingData01
 * @tc.desc: SetCastPrivacyFromSettingData01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetCastPrivacyFromSettingData01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->SetCastPrivacyFromSettingData();
    ssm->GetAllDisplayIds();
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CallRsSetScreenPowerStatusSyncForExtend01
 * @tc.desc: CallRsSetScreenPowerStatusSyncForExtend01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CallRsSetScreenPowerStatusSyncForExtend01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    std::vector<ScreenId> screenIds = {50};
    ScreenPowerStatus status = ScreenPowerStatus::POWER_STATUS_ON;
    ssm->CallRsSetScreenPowerStatusSyncForExtend(screenIds, status);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CallRsSetScreenPowerStatusSyncForExtend02
 * @tc.desc: CallRsSetScreenPowerStatusSyncForExtend02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CallRsSetScreenPowerStatusSyncForExtend02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::UNDEFINED);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    std::vector<ScreenId> screenIds = {50};
    ScreenPowerStatus status = ScreenPowerStatus::POWER_STATUS_ON;
    ssm->CallRsSetScreenPowerStatusSyncForExtend(screenIds, status);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CallRsSetScreenPowerStatusSyncForExtend03
 * @tc.desc: CallRsSetScreenPowerStatusSyncForExtend03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CallRsSetScreenPowerStatusSyncForExtend03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetIsInternal(true);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    std::vector<ScreenId> screenIds = {50};
    ScreenPowerStatus status = ScreenPowerStatus::POWER_STATUS_ON;
    ssm->CallRsSetScreenPowerStatusSyncForExtend(screenIds, status);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: CallRsSetScreenPowerStatusSyncForExtend04
 * @tc.desc: CallRsSetScreenPowerStatusSyncForExtend04
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CallRsSetScreenPowerStatusSyncForExtend04, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetIsInternal(false);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    std::vector<ScreenId> screenIds = {50};
    ScreenPowerStatus status = ScreenPowerStatus::POWER_STATUS_ON;
    ssm->CallRsSetScreenPowerStatusSyncForExtend(screenIds, status);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: SetExtendPixelRatio01
 * @tc.desc: SetExtendPixelRatio01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetExtendPixelRatio01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    float dpi = 1.0f;
    ssm->SetExtendPixelRatio(dpi);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->SetExtendPixelRatio(dpi);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: SetExtendPixelRatio02
 * @tc.desc: SetExtendPixelRatio02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetExtendPixelRatio02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    float dpi = 1.0f;
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetIsInternal(false);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->SetExtendPixelRatio(dpi);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: SetExtendPixelRatio03
 * @tc.desc: SetExtendPixelRatio03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetExtendPixelRatio03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    float dpi = 1.0f;
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetIsInternal(true);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->SetExtendPixelRatio(dpi);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: SetExtendPixelRatio04
 * @tc.desc: SetExtendPixelRatio04
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetExtendPixelRatio04, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    float dpi = 1.0f;
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::UNDEFINED);
    screenSession->SetIsInternal(true);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->SetExtendPixelRatio(dpi);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: SetRotation01
 * @tc.desc: SetRotation01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetRotation01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    Rotation rotationAfter = Rotation::ROTATION_0;
    bool isFromWindow = true;
    auto res = ssm->SetRotation(id, rotationAfter, isFromWindow);
    ASSERT_EQ(false, res);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: SetRotation02
 * @tc.desc: SetRotation02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetRotation02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetRotation(Rotation::ROTATION_90);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    Rotation rotationAfter = Rotation::ROTATION_0;
    bool isFromWindow = true;
    auto res = ssm->SetRotation(id, rotationAfter, isFromWindow);
    ASSERT_EQ(true, res);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: OnMakeExpand01
 * @tc.desc: OnMakeExpand01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnMakeExpand01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    std::vector<ScreenId> screenId = {1001, 1002};
    std::vector<Point> startPoint(1);
    auto res = ssm->OnMakeExpand(screenId, startPoint);
    ASSERT_EQ(false, res);
    ssm->screenSessionMap_.erase(0);
}

/**
 * @tc.name: OnMakeExpand02
 * @tc.desc: OnMakeExpand02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnMakeExpand02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->groupSmsId_ = 50;
    ScreenCombination combination =  ScreenCombination::SCREEN_ALONE;
    ScreenId rsId = 1002;
    std::string name = "ok";
    sptr<ScreenSessionGroup> groupSession = new ScreenSessionGroup(50, rsId, name, combination);
    ASSERT_NE(nullptr, groupSession);
    ssm->smsScreenGroupMap_.insert(std::make_pair(50, groupSession));
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    std::vector<ScreenId> screenId = {1001, 1002};
    std::vector<Point> startPoint(1);
    auto res = ssm->OnMakeExpand(screenId, startPoint);
    ASSERT_EQ(true, res);
    ssm->screenSessionMap_.erase(0);
    ssm->smsScreenGroupMap_.erase(50);
}

/**
 * @tc.name: IsExtendMode01
 * @tc.desc: IsExtendMode01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, IsExtendMode01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->IsExtendMode();
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: IsExtendMode02
 * @tc.desc: IsExtendMode02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, IsExtendMode02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    ssm->IsExtendMode();
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    ssm->IsExtendMode();
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    ssm->IsExtendMode();
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: UpdateCameraBackSelfie01
 * @tc.desc: UpdateCameraBackSelfie01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateCameraBackSelfie01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    bool isCameraBackSelfie = true;
    ssm->UpdateCameraBackSelfie(isCameraBackSelfie);
    ssm->UpdateCameraBackSelfie(true);
    ssm->screenSessionMap_.erase(0);
}

/**
 * @tc.name: UpdateCameraBackSelfie02
 * @tc.desc: UpdateCameraBackSelfie02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateCameraBackSelfie02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 0;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    bool isCameraBackSelfie = true;
    ssm->UpdateCameraBackSelfie(isCameraBackSelfie);
    ssm->UpdateCameraBackSelfie(false);
    ssm->screenSessionMap_.erase(0);
}

/**
 * @tc.name: HasCastEngineOrPhyMirror01
 * @tc.desc: HasCastEngineOrPhyMirror01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HasCastEngineOrPhyMirror01, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = nullptr;
    ASSERT_EQ(nullptr, screenSession);
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    std::vector<ScreenId> screenIdsToExclude = {100};
    ssm->HasCastEngineOrPhyMirror(screenIdsToExclude);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: HasCastEngineOrPhyMirror02
 * @tc.desc: HasCastEngineOrPhyMirror02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HasCastEngineOrPhyMirror02, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::VIRTUAL);
    screenSession->SetName("CastEngine");
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    std::vector<ScreenId> screenIdsToExclude = {100};
    auto res = ssm->HasCastEngineOrPhyMirror(screenIdsToExclude);
    ASSERT_EQ(true, res);
    std::vector<ScreenId> idsToExclude = {100, 50};
    ssm->HasCastEngineOrPhyMirror(idsToExclude);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: HasCastEngineOrPhyMirror03
 * @tc.desc: HasCastEngineOrPhyMirror03
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HasCastEngineOrPhyMirror03, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 50;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::VIRTUAL);
    screenSession->SetName("TestCastEngine");
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    std::vector<ScreenId> screenIdsToExclude = {100};
    auto res = ssm->HasCastEngineOrPhyMirror(screenIdsToExclude);
    ASSERT_EQ(false, res);
    screenSession->SetScreenType(ScreenType::REAL);
    ssm->HasCastEngineOrPhyMirror(screenIdsToExclude);
    ssm->screenSessionMap_.erase(50);
}

/**
 * @tc.name: HasCastEngineOrPhyMirror04
 * @tc.desc: HasCastEngineOrPhyMirror04
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HasCastEngineOrPhyMirror04, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ScreenId id = 9;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenType(ScreenType::VIRTUAL);
    screenSession->SetName("CastEngine");
    ssm->screenSessionMap_.insert(std::make_pair(id, screenSession));
    std::vector<ScreenId> screenIdsToExclude = {100};
    ssm->HasCastEngineOrPhyMirror(screenIdsToExclude);
    ssm->screenSessionMap_.erase(9);
}

/**
 * @tc.name: HotSwitch
 * @tc.desc: HotSwitch
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HotSwitch, Function | SmallTest | Level3)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    int32_t newUserId = 0;
    int32_t newScbPid = 1;
    ssm->HotSwitch(newUserId, newScbPid);
    sptr<IScreenSessionManagerClient> client = nullptr;
    ssm->clientProxyMap_.insert(std::make_pair(newUserId, client));
    ssm->HotSwitch(newUserId, newScbPid);
    ssm->clientProxyMap_.insert(std::make_pair(1, client));
    ssm->HotSwitch(1, newScbPid);
}

/**
 * @tc.name: SetVirtualScreenStatus
 * @tc.desc: SetVirtualScreenStatus test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenStatus, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetClient, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, SwitchUser, TestSize.Level1)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ssm->SwitchUser();
}

/**
 * @tc.name: SetScreenPrivacyMaskImage001
 * @tc.desc: SetScreenPrivacyMaskImage001
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPrivacyMaskImage001, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, ScbClientDeathCallback, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, NotifyClientProxyUpdateFoldDisplayMode, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, OnScreenRotationLockedChange, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, OnScreenOrientationChange, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, NotifyDisplayModeChanged, TestSize.Level1)
{
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    ssm->NotifyDisplayModeChanged(FoldDisplayMode::MAIN);
}

/**
 * @tc.name: SetMultiScreenMode
 * @tc.desc: MultiScreenMode::SCREEN_EXTEND
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenMode01, TestSize.Level1)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    MultiScreenMode screenMode = MultiScreenMode::SCREEN_EXTEND;
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    ASSERT_NE(screenSession, nullptr);
    screenSession->SetScreenType(ScreenType::REAL);

    auto ret = ssm_->SetMultiScreenMode(0, screenSession->GetRSScreenId(), screenMode);
    ASSERT_EQ(ret, DMError::DM_OK);
    ssm_->DestroyVirtualScreen(screenId);
#endif
}

/**
 * @tc.name: SetMultiScreenMode
 * @tc.desc: MultiScreenMode::SCREEN_MIRROR
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenMode02, TestSize.Level1)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    MultiScreenMode screenMode = MultiScreenMode::SCREEN_MIRROR;
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    ASSERT_NE(screenSession, nullptr);
    screenSession->SetScreenType(ScreenType::REAL);

    auto ret = ssm_->SetMultiScreenMode(0, screenSession->GetRSScreenId(), screenMode);
    ASSERT_EQ(ret, DMError::DM_OK);
    ssm_->DestroyVirtualScreen(screenId);
#endif
}

/**
 * @tc.name: SetMultiScreenMode
 * @tc.desc: operate mode error
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenMode03, TestSize.Level1)
{
#ifdef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    EXPECT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    ASSERT_NE(screenSession, nullptr);
    screenSession->SetScreenType(ScreenType::REAL);

    uint32_t testNum = 2;
    auto ret = ssm_->SetMultiScreenMode(0, screenSession->GetRSScreenId(), static_cast<MultiScreenMode>(testNum));
    ASSERT_EQ(ret, DMError::DM_OK);
    ssm_->DestroyVirtualScreen(screenId);
#endif
}

/**
 * @tc.name: SetMultiScreenRelativePosition
 * @tc.desc: ScreenSession is null
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenRelativePosition01, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenRelativePosition02, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenRelativePosition03, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenRelativePosition04, TestSize.Level1)
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
    if (FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        ASSERT_EQ(ret, DMError::DM_OK);
    } else {
        ASSERT_EQ(ret, DMError::DM_ERROR_INVALID_PARAM);
    }

    ssm_->DestroyVirtualScreen(screenId);
    ssm_->DestroyVirtualScreen(screenId1);
#endif
}

/**
 * @tc.name: SetMultiScreenRelativePosition
 * @tc.desc: INVALID_PARAM
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenRelativePosition05, TestSize.Level1)
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
    if (FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        ASSERT_EQ(ret, DMError::DM_OK);
    } else {
        ASSERT_EQ(ret, DMError::DM_ERROR_INVALID_PARAM);
    }

    ssm_->DestroyVirtualScreen(screenId);
    ssm_->DestroyVirtualScreen(screenId1);
#endif
}

/**
 * @tc.name: SetMultiScreenRelativePosition
 * @tc.desc: DisplayNode is null
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenRelativePosition06, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetCoordinationFlag, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, GetTentMode, TestSize.Level1)
{
    auto tentMode = ssm_->GetTentMode();
    ASSERT_EQ(tentMode, false);
}

/**
 * @tc.name: OnTentModeChanged
 * @tc.desc: Test change tent mode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnTentModeChanged, TestSize.Level1)
{
    bool isTentMode = false;
    ssm_->OnTentModeChanged(isTentMode);
    ASSERT_EQ(ssm_->GetTentMode(), false);
}

/**
 * @tc.name: GetScreenCapture
 * @tc.desc: GetScreenCapture
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenCapture, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, OnScreenCaptureNotify, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, GetPrimaryDisplayInfo, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ASSERT_NE(ssm_->GetPrimaryDisplayInfo(), nullptr);
}

/*
 * @tc.name: TransferTypeToString
 * @tc.desc: TransferTypeToString
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TransferTypeToString1, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, TransferTypeToString2, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, TransferTypeToString3, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, TransferPropertyChangeTypeToString1, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, TransferPropertyChangeTypeToString2, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, TransferPropertyChangeTypeToString3, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, TransferPropertyChangeTypeToString4, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, ConvertOffsetToCorrectRotation, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, ConfigureScreenSnapshotParams, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, RegisterRefreshRateChangeListener, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, FreeDisplayMirrorNodeInner, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetPowerStatus01, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetPowerStatus02, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetPowerStatus03, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetPowerStatus04, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetPowerStatus05, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetGotScreenOffAndWakeUpBlock, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetFoldStatus, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetLowTemp, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetScreenSkipProtectedWindow, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    ASSERT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption1";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    if (screenId != VIRTUAL_SCREEN_ID) {
        ASSERT_TRUE(screenId != VIRTUAL_SCREEN_ID);
    }
    const std::vector<ScreenId> screenIds = {screenId, 1234};
    bool isEnable = true;
    ASSERT_EQ(ssm_->SetScreenSkipProtectedWindow(screenIds, isEnable), DMError::DM_OK);
    isEnable = false;
    ASSERT_EQ(ssm_->SetScreenSkipProtectedWindow(screenIds, isEnable), DMError::DM_OK);
}

/**
 * @tc.name: GetDisplayCapability
 * @tc.desc: GetDisplayCapability test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetDisplayCapability, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetSecondaryDisplayCapability, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, GetSuperFoldCapability, TestSize.Level1)
{
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
HWTEST_F(ScreenSessionManagerTest, GetFoldableDeviceCapability, TestSize.Level1)
{
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
 HWTEST_F(ScreenSessionManagerTest, DoMakeUniqueScreenOld, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetCastPrivacyFromSettingData, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, SetCastPrivacyToRS, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, RegisterSettingWireCastObserver, TestSize.Level1)
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
HWTEST_F(ScreenSessionManagerTest, UnregisterSettingWireCastObserver, TestSize.Level1)
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

    uint32_t originValidWidth = screenSession->GetValidWidth();
    uint32_t originValidHeight = screenSession->GetValidHeight();
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
 * @tc.name: CalculateXYPosition
 * @tc.desc: CalculateXYPosition test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalculateXYPosition, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    ASSERT_NE(screenSession, nullptr);
    screenSession->SetScreenType(ScreenType::REAL);
    screenSession->SetIsInternal(true);
    int32_t x = screenSession->GetScreenProperty().GetX();
    EXPECT_EQ(0, x);
    int32_t y = screenSession->GetScreenProperty().GetY();
    EXPECT_EQ(0, y);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: IsSpecialApp
 * @tc.desc: IsSpecialApp
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, IsSpecialApp, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    auto ret = ssm_->IsSpecialApp();
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: IsScreenCasting
 * @tc.desc: IsScreenCasting
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, IsScreenCasting, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    ssm_->virtualScreenCount_ = 1;
    auto ret = ssm_->IsScreenCasting();
    ASSERT_EQ(ret, true);

    ssm_->virtualScreenCount_ = 0;
    ssm_->hdmiScreenCount_ = 0;
    ret = ssm_->IsScreenCasting();
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CanEnterCoordination01
 * @tc.desc: CanEnterCoordination01 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CanEnterCoordination01, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->virtualScreenCount_ = 0;
    ssm_->hdmiScreenCount_ = 1;
    auto ret = ssm_->CanEnterCoordination();
    EXPECT_EQ(ret, DMError::DM_ERROR_NOT_SUPPORT_COOR_WHEN_WIRED_CASTING);

    ssm_->virtualScreenCount_ = 0;
    ssm_->hdmiScreenCount_ = 0;
    auto ret = ssm_->CanEnterCoordination();
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: CanEnterCoordination02
 * @tc.desc: CanEnterCoordination02 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CanEnterCoordination02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->screenSessionMap_[50] = nullptr;
    ssm_->virtualScreenCount_ = 1;
    ssm_->hdmiScreenCount_ = 0;

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    virtualOption.virtualScreenType_ = VirtualScreenType::SCREEN_CASTING;
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    auto ret = ssm_->CanEnterCoordination();
    EXPECT_EQ(ret, DMError::DM_ERROR_NOT_SUPPORT_COOR_WHEN_WIRLESS_CASTING);
    ssm_->DestroyVirtualScreen(screenId)
}

/**
 * @tc.name: CanEnterCoordination03
 * @tc.desc: CanEnterCoordination03 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CanEnterCoordination03, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    auto oldMap = ssm_->screenSessionMap_;
    std::map<ScreenId, sptr<ScreenSession>> newScreenSessionMap_{};
    ssm_->screenSessionMap_ = newScreenSessionMap_;
    ssm_->virtualScreenCount_ = 1;
    ssm_->hdmiScreenCount_ = 0;
    auto ret = ssm_->CanEnterCoordination();
    EXPECT_EQ(ret, DMError::DM_OK);
    ssm_->screenSessionMap_ = oldMap;
}

/**
 * @tc.name: GetCameraPosition
 * @tc.desc: GetCameraPosition
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCameraPosition, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    ssm_->cameraPosition_ = 1;
    auto ret = ssm_->GetCameraPosition();
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: GetCameraStatus
 * @tc.desc: GetCameraStatus
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCameraStatus, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    ssm_->cameraStatus_ = 1;
    auto ret = ssm_->GetCameraStatus();
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: OnSecondaryReflexionChange
 * @tc.desc: OnSecondaryReflexionChange
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnSecondaryReflexionChange, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ASSERT_EQ(ssm_->clientProxy_, nullptr);

    ssm_->OnSecondaryReflexionChange(0, false);
    ASSERT_EQ(ssm_->cameraStatus_, 1);
}

/**
 * @tc.name: OnExtendScreenConnectStatusChange
 * @tc.desc: OnExtendScreenConnectStatusChange
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnExtendScreenConnectStatusChange, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ASSERT_EQ(ssm_->clientProxy_, nullptr);

    ExtendScreenConnectStatus status = ExtendScreenConnectStatus::UNKNOWN;
    ssm_->OnExtendScreenConnectStatusChange(0, status);
    ASSERT_EQ(ssm_->cameraStatus_, 1);
}

/**
 * @tc.name: OnSuperFoldStatusChange
 * @tc.desc: OnSuperFoldStatusChange
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnSuperFoldStatusChange, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ASSERT_EQ(ssm_->clientProxy_, nullptr);

    SuperFoldStatus status = SuperFoldStatus::UNKNOWN;
    ssm_->OnSuperFoldStatusChange(0, status);
    ASSERT_EQ(ssm_->cameraStatus_, 1);
}

/**
 * @tc.name: GetPrimaryDisplayInfo02
 * @tc.desc: GetPrimaryDisplayInfo02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPrimaryDisplayInfo02, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    ssm_->screenSessionMap_.insert(std::pair<ScreenId, sptr<ScreenSession>>(4, nullptr));
    auto ret = ssm_->GetPrimaryDisplayInfo();
    ASSERT_NE(ret, nullptr);

    auto screenSession = new ScreenSession;
    screenSession->SetIsExtend(true);
    ssm_->screenSessionMap_.insert(std::pair<ScreenId, sptr<ScreenSession>>(6, screenSession));
    ret = ssm_->GetPrimaryDisplayInfo();
    ASSERT_NE(ret, nullptr);

    screenSession->SetIsExtend(false);
    ret = ssm_->GetPrimaryDisplayInfo();
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: AddPermissionUsedRecord
 * @tc.desc: AddPermissionUsedRecord
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddPermissionUsedRecord, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    ssm_->AddPermissionUsedRecord("ohos.permission.CUSTOM_SCREEN_CAPTURE", 0, 1);
    ASSERT_EQ(ssm_->cameraStatus_, 1);
}

/**
 * @tc.name: SetVirtualScreenMaxRefreshRate
 * @tc.desc: SetVirtualScreenMaxRefreshRate
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenMaxRefreshRate, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    uint32_t actualRefreshRate = 0;

    auto ret = ssm_->SetVirtualScreenMaxRefreshRate(0, 120, actualRefreshRate);
    ASSERT_EQ(ret, DMError::DM_ERROR_INVALID_PARAM);

    ret = ssm_->SetVirtualScreenMaxRefreshRate(screenId, 120, actualRefreshRate);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: OnScreenExtendChange
 * @tc.desc: OnScreenExtendChange
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnScreenExtendChange, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    ssm_->OnScreenExtendChange(0, 12);
    ASSERT_EQ(ssm_->cameraStatus_, 1);
}

/**
 * @tc.name: GetSessionOption
 * @tc.desc: GetSessionOption
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetSessionOption, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    auto session = ssm_->GetScreenSession(0);
    auto ret = ssm_->GetSessionOption(session);
    ASSERT_EQ(ret.screenId_, 0);

    ret = ssm_->GetSessionOption(session, 0);
    ASSERT_EQ(ret.screenId_, 0);
}

/**
 * @tc.name: SetMultiScreenDefaultRelativePosition
 * @tc.desc: SetMultiScreenDefaultRelativePosition
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenDefaultRelativePosition, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    sptr<ScreenSession> screenSession1 = new ScreenSession(50, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession1);
    screenSession1->SetIsRealScreen(true);
    screenSession1->SetIsExtend(true);
    ssm_->screenSessionMap_[50] = screenSession1;

    sptr<ScreenSession> screenSession2 = new ScreenSession(51, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession2);
    screenSession2->SetIsRealScreen(false);
    ssm_->screenSessionMap_[51] = screenSession2;

    sptr<ScreenSession> screenSession3 = new ScreenSession(52, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession3);
    screenSession3->SetIsRealScreen(true);
    screenSession3->SetIsExtend(false);
    ssm_->screenSessionMap_[52] = screenSession3;

    sptr<ScreenSession> screenSession4 = nullptr;
    ASSERT_EQ(nullptr, screenSession4);
    ssm_->screenSessionMap_[53] = screenSession4;

    ssm_->SetMultiScreenDefaultRelativePosition();
    ASSERT_NE(ssm_->screenSessionMap_.empty(), true);
    ssm_->screenSessionMap_.erase(50);
    ssm_->screenSessionMap_.erase(51);
    ssm_->screenSessionMap_.erase(52);
    ssm_->screenSessionMap_.erase(53);
}

/**
 * @tc.name: ConvertEdidToString
 * @tc.desc: ConvertEdidToString
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ConvertEdidToString, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    struct BaseEdid edid = {
        .manufacturerName_ = "abc",
        .productCode_ = 101,
        .serialNumber_ = 1401,
        .weekOfManufactureOrModelYearFlag_ = 1,
        .yearOfManufactureOrModelYear_ = 2025,
    };
    std::string edidInfo = edid.manufacturerName_ + std::to_string(edid.productCode_)
        + std::to_string(edid.serialNumber_) + std::to_string(edid.weekOfManufactureOrModelYearFlag_)
        + std::to_string(edid.yearOfManufactureOrModelYear_);
    std::hash<std::string> hasher;
    std::size_t hashValue = hasher(edidInfo);
    std::ostringstream oss;
    oss << std::hex << std::uppercase << hashValue;

    auto str = ssm_->ConvertEdidToString(edid);
    ASSERT_EQ(str, oss.str());
}

/**
 * @tc.name: RecoverRestoredMultiScreenMode
 * @tc.desc: RecoverRestoredMultiScreenMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RecoverRestoredMultiScreenMode, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    sptr<ScreenSession> screenSession1 = new ScreenSession(50, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession1);
    screenSession1->SetScreenType(ScreenType::REAL);
    auto ret = ssm_->RecoverRestoredMultiScreenMode(screenSession1);
    ASSERT_EQ(ret, false);

    sptr<ScreenSession> screenSession2 = new ScreenSession(51, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession2);
    screenSession2->SetScreenType(ScreenType::VIRTUAL);
    ret = ssm_->RecoverRestoredMultiScreenMode(screenSession2);
    ASSERT_EQ(ret, true);
    ssm_->screenSessionMap_.erase(50);
    ssm_->screenSessionMap_.erase(51);
}

/**
 * @tc.name: CheckMultiScreenInfoMap
 * @tc.desc: CheckMultiScreenInfoMap
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CheckMultiScreenInfoMap, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    std::map<std::string, MultiScreenInfo> multiScreenInfoMap;
    ASSERT_EQ(multiScreenInfoMap.empty(), true);
    std::string serialNumber = "";
    auto ret = ssm_->CheckMultiScreenInfoMap(multiScreenInfoMap, serialNumber);
    ASSERT_EQ(ret, false);

    MultiScreenInfo info1;
    std::map<std::string, MultiScreenInfo> multiScreenInfoMap1;
    multiScreenInfoMap1["123"] = info1;
    ASSERT_EQ(multiScreenInfoMap1.empty(), false);
    std::string serialNumber1 = "";
    ret = ssm_->CheckMultiScreenInfoMap(multiScreenInfoMap1, serialNumber1);
    ASSERT_EQ(ret, false);

    MultiScreenInfo info2;
    std::map<std::string, MultiScreenInfo> multiScreenInfoMap2;
    multiScreenInfoMap2["123"] = info2;
    ASSERT_EQ(multiScreenInfoMap2.empty(), false);
    std::string serialNumber2 = "123";
    ret = ssm_->CheckMultiScreenInfoMap(multiScreenInfoMap2, serialNumber2);
    ASSERT_EQ(ret, true);

    MultiScreenInfo info3;
    std::map<std::string, MultiScreenInfo> multiScreenInfoMap3;
    multiScreenInfoMap3["123"] = info3;
    ASSERT_EQ(multiScreenInfoMap3.empty(), false);
    std::string serialNumber3 = "456";
    ret = ssm_->CheckMultiScreenInfoMap(multiScreenInfoMap3, serialNumber3);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: SetMultiScreenFrameControl
 * @tc.desc: SetMultiScreenFrameControl
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetMultiScreenFrameControl, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    sptr<ScreenSession> screenSession1 = new ScreenSession(50, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession1);
    screenSession1->SetScreenType(ScreenType::REAL);
    screenSession1->SetIsCurrentInUse(true);
    ssm_->screenSessionMap_[50] = screenSession1;

    sptr<ScreenSession> screenSession2 = new ScreenSession(51, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession2);
    screenSession2->SetScreenType(ScreenType::VIRTUAL);
    screenSession2->SetIsCurrentInUse(true);
    ssm_->screenSessionMap_[51] = screenSession2;

    ssm_->SetMultiScreenFrameControl();
    ASSERT_NE(ssm_->screenSessionMap_.empty(), true);

    sptr<ScreenSession> screenSession3 = new ScreenSession(52, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession3);
    screenSession3->SetScreenType(ScreenType::REAL);
    screenSession3->SetIsCurrentInUse(true);
    ssm_->screenSessionMap_[52] = screenSession3;

    ssm_->SetMultiScreenFrameControl();
    ASSERT_NE(ssm_->screenSessionMap_.empty(), true);
    ssm_->screenSessionMap_.erase(50);
    ssm_->screenSessionMap_.erase(51);
    ssm_->screenSessionMap_.erase(52);
}

/**
 * @tc.name: GetInternalScreenSession
 * @tc.desc: GetInternalScreenSession
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetInternalScreenSession, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    sptr<ScreenSession> screenSession1 = nullptr;
    ASSERT_EQ(nullptr, screenSession1);
    ssm_->screenSessionMap_[50] = screenSession1;

    sptr<ScreenSession> screenSession2 = new ScreenSession(51, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession2);
    screenSession2->SetScreenType(ScreenType::REAL);
    screenSession2->isInternal_ = false;
    ssm_->screenSessionMap_[51] = screenSession2;

    sptr<ScreenSession> screenSession3 = new ScreenSession(52, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession3);
    screenSession3->SetScreenType(ScreenType::VIRTUAL);
    screenSession3->isInternal_ = false;
    ssm_->screenSessionMap_[52] = screenSession3;

    sptr<ScreenSession> screenSession4 = new ScreenSession(53, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession4);
    screenSession4->SetScreenType(ScreenType::VIRTUAL);
    screenSession4->isInternal_ = true;
    ssm_->screenSessionMap_[53] = screenSession4;

    sptr<ScreenSession> screenSession5 = new ScreenSession(54, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession5);
    screenSession5->SetScreenType(ScreenType::REAL);
    screenSession5->isInternal_ = true;
    ssm_->screenSessionMap_[54] = screenSession5;

    auto ret = ssm_->GetInternalScreenSession();
    ASSERT_NE(nullptr, ret);
    ssm_->screenSessionMap_.erase(50);
    ssm_->screenSessionMap_.erase(51);
    ssm_->screenSessionMap_.erase(52);
    ssm_->screenSessionMap_.erase(53);
    ssm_->screenSessionMap_.erase(54);
}

/**
 * @tc.name: GetInternalAndExternalSession
 * @tc.desc: GetInternalAndExternalSession
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetInternalAndExternalSession, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    sptr<ScreenSession> screenSession1 = nullptr;
    ASSERT_EQ(nullptr, screenSession1);
    ssm_->screenSessionMap_[50] = screenSession1;

    sptr<ScreenSession> screenSession2 = new ScreenSession(51, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession2);
    screenSession2->SetIsCurrentInUse(false);
    ssm_->screenSessionMap_[51] = screenSession2;

    sptr<ScreenSession> screenSession3 = new ScreenSession(52, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession3);
    screenSession3->SetIsCurrentInUse(true);
    screenSession3->SetScreenType(ScreenType::VIRTUAL);
    ssm_->screenSessionMap_[52] = screenSession3;

    sptr<ScreenSession> screenSession4 = new ScreenSession(53, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession4);
    screenSession3->SetIsCurrentInUse(true);
    screenSession3->SetScreenType(ScreenType::REAL);
    screenSession3->isInternal_ = false;
    ssm_->screenSessionMap_[53] = screenSession4;

    sptr<ScreenSession> screenSession5 = new ScreenSession(54, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession5);
    screenSession5->SetIsCurrentInUse(true);
    screenSession5->SetScreenType(ScreenType::REAL);
    screenSession5->isInternal_ = true;
    ssm_->screenSessionMap_[54] = screenSession5;

    sptr<ScreenSession> externalSession = nullptr;
    sptr<ScreenSession> internalSession = nullptr;
    ssm_->GetInternalAndExternalSession(internalSession, externalSession);
    ASSERT_EQ(internalSession, screenSession5);
    ssm_->screenSessionMap_.erase(50);
    ssm_->screenSessionMap_.erase(51);
    ssm_->screenSessionMap_.erase(52);
    ssm_->screenSessionMap_.erase(53);
    ssm_->screenSessionMap_.erase(54);
}

/**
 * @tc.name: GetFakePhysicalScreenSession
 * @tc.desc: GetFakePhysicalScreenSession
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetFakePhysicalScreenSession, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);

    auto ret = ssm_->GetFakePhysicalScreenSession(50, 0, ScreenProperty());
    if (g_isPcDevice) {
        ASSERT_NE(nullptr, ret);
    } else {
        ASSERT_EQ(nullptr, ret);
    }
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

/**
 * @tc.name: RecoverRestoredMultiScreenMode01
 * @tc.desc: RecoverRestoredMultiScreenMode01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RecoverRestoredMultiScreenMode01, TestSize.Level1)
{
    ScreenId id = 1;
    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr(id, ScreenProperty(), 0);
    screenSession->GetScreenProperty().SetScreenType(ScreenType::REAL);
    EXPECT_FALSE(ScreenSessionManager::GetInstance().RecoverRestoredMultiScreenMode(screenSession));
}

/**
 * @tc.name: RecoverRestoredMultiScreenMode02
 * @tc.desc: RecoverRestoredMultiScreenMode02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RecoverRestoredMultiScreenMode02, TestSize.Level1)
{
    ScreenId id = 2;
    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr(id, ScreenProperty(), 0);
    screenSession->GetScreenProperty().SetScreenType(ScreenType::UNDEFINED);
    EXPECT_FALSE(ScreenSessionManager::GetInstance().RecoverRestoredMultiScreenMode(screenSession));
}

/**
 * @tc.name: TestIsFreezed_WhenPidNotExist
 * @tc.desc: Test IsFreezed function when agentPid does not exist in freezedPidList_
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TestIsFreezed_WhenPidNotExist, TestSize.Level1)
{
    int32_t agentPid = 1234;
    DisplayManagerAgentType agentType = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;

    EXPECT_FALSE(ScreenSessionManager::GetInstance().IsFreezed(agentPid, agentType));
}

/**
 * @tc.name: TestIsFreezed_WhenPidExistButAgentTypeNotExist
 * @tc.desc: Test IsFreezed function when agentPid exists in freezedPidList_ but not in pidAgentTypeMap_
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TestIsFreezed_WhenPidExistButAgentTypeNotExist, TestSize.Level1)
{
    int32_t agentPid = 1234;
    DisplayManagerAgentType agentType = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;

    ScreenSessionManager::GetInstance().freezedPidList_.insert(agentPid);
    EXPECT_TRUE(ScreenSessionManager::GetInstance().IsFreezed(agentPid, agentType));
    EXPECT_EQ(ScreenSessionManager::GetInstance().pidAgentTypeMap_[agentPid].count(agentType), 1);
}

/**
 * @tc.name: TestIsFreezed_WhenPidAndAgentTypeExist
 * @tc.desc: Test IsFreezed function when agentPid and agentType both exists
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TestIsFreezed_WhenPidAndAgentTypeExist, TestSize.Level1)
{
    int32_t agentPid = 1234;
    DisplayManagerAgentType agentType = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;

    ScreenSessionManager::GetInstance().freezedPidList_.insert(agentPid);
    ScreenSessionManager::GetInstance().pidAgentTypeMap_[agentPid] =
        std::set<DisplayManagerAgentType>({DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER});
    EXPECT_TRUE(ScreenSessionManager::GetInstance().IsFreezed(agentPid, agentType));
    EXPECT_EQ(ScreenSessionManager::GetInstance().pidAgentTypeMap_[agentPid].count(agentType), 1);
}

/**
 * @tc.name: SetScreenOnDelayTime
 * @tc.desc: SetScreenOnDelayTime test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenOnDelayTime, TestSize.Level1)
{
    int32_t delay = CV_WAIT_SCREENON_MS - 1;
    int32_t ret = ssm_->SetScreenOnDelayTime(delay);
    EXPECT_EQ(ret, delay);

    delay = CV_WAIT_SCREENON_MS + 1;
    ret = ssm_->SetScreenOnDelayTime(delay);
    EXPECT_EQ(ret, CV_WAIT_SCREENON_MS);
}

/**
 * @tc.name: ShouldReturnOkWhenMultiScreenNotEnabled
 * @tc.desc: ShouldReturnOkWhenMultiScreenNotEnabled test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ShouldReturnOkWhenMultiScreenNotEnabled, TestSize.Level1)
{
    EXPECT_EQ(ScreenSessionManager::GetInstance().VirtualScreenUniqueSwitch({}), DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: ShouldReturnOkWhenMultiScreenNotEnabled
 * @tc.desc: ShouldReturnOkWhenMultiScreenNotEnabled test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ShouldReturnNullPtrWhenDefaultScreenIsNull, TestSize.Level1)
{
#ifndef WM_MULTI_SCREEN_ENABLE
    ScreenSessionManager ssm;
    ssm = ScreenSessionManager::GetInstance();

    ssm.screenSessionMap_.erase(1100);
    ssm.defaultScreenId_ = 1100;
    EXPECT_EQ(ssm.VirtualScreenYniqueSwitch({}), DMError::DM_ERROR_NULLPTR);
#endif
}

/**
 * @tc.name: ShouldHandleExistingGroupSmsId
 * @tc.desc: ShouldHandleExistingGroupSmsId test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ShouldHandleExistingGroupSmsId, TestSize.Level1)
{
#ifndef WM_MULTI_SCREEN_ENABLE
    ScreenSessionManager ssm;
    ssm = ScreenSessionManager::GetInstance();

    Sptr<ScreenSession> screenSession = Sptr<ScreenSession>::MakeSptr();
    screenSession->groupSmsId_ = 1001;
    ssm.screenSessionMap_[1100] = screenSession;
    ssm.defaultScreenId_ = 1100;
    ssm.smsSessionGroupMap_[1001] = {};
    EXPECT_EQ(ssm.VirtualScreenYniqueSwitch({}), DMError::DM_OK);
    EXPECT_EQ(ssm.smsSessionGroupMap_.count(1), 0);
    ssm.screenSessionMap_.erase(1100);
#endif
}

/**
 * @tc.name: ShouldHandleNonExistingGroupSmsId
 * @tc.desc: ShouldHandleNonExistingGroupSmsId test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ShouldHandleNonExistingGroupSmsId, TestSize.Level1)
{
#ifndef WM_MULTI_SCREEN_ENABLE
    ScreenSessionManager ssm;
    ssm = ScreenSessionManager::GetInstance();

    Sptr<ScreenSession> screenSession = Sptr<ScreenSession>::MakeSptr();
    screenSession->groupSmsId_ = 1001;
    ssm.screenSessionMap_[1100] = screenSession;
    ssm.defaultScreenId_ = 1100;
    EXPECT_EQ(ssm.VirtualScreenYniqueSwitch({}), DMError::DM_OK);
    ssm.screenSessionMap_.erase(1100);
#endif
}

/**
 * @tc.name: SetRSScreenPowerStatus
 * @tc.desc: call RS power func, and notify brightness while screen on
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetRSScreenPowerStatus, TestSize.Level1)
{
    ScreenPowerState state;
    EXPECT_NE(ssm_, nullptr);

    ssm_->SetRSScreenPowerStatusExt(0, ScreenPowerStatus::POWER_STATUS_ON);
    state = ssm_->GetScreenPower(0);
    EXPECT_EQ(state, ScreenPowerState::POWER_ON);

    ssm_->SetRSScreenPowerStatusExt(0, ScreenPowerStatus::POWER_STATUS_OFF);
    state = ssm_->GetScreenPower(0);
    EXPECT_EQ(state, ScreenPowerState::POWER_OFF);
}

/**
 * @tc.name: GetScreenCombination01
 * @tc.desc: GetScreenCombination01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenCombination01, TestSize.Level1)
{
    ScreenId screenId = 1051;
    auto ret = ssm_->GetScreenCombination(screenId);
    EXPECT_EQ(ret, ScreenCombination::SCREEN_ALONE);
}

/**
 * @tc.name: GetScreenCombination02
 * @tc.desc: !screenSession = false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenCombination02, TestSize.Level1)
{
    ScreenId screenId = 1050;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    EXPECT_NE(screenSession, nullptr);
    ssm_->screenSessionMap_[screenId] = screenSession;
    auto ret = ssm_->GetScreenCombination(screenId);
    EXPECT_EQ(ret, ScreenCombination::SCREEN_ALONE);
}

/**
 * @tc.name: OnRemoteDied01
 * @tc.desc: OnRemoteDied_ShouldReturnFalse_WhenAgentIsNUllptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnRemoteDied01, TestSize.Level1)
{
    EXPECT_FALSE(ssm_->OnRemoteDied(nullptr));
}

/**
 * @tc.name: OnRemoteDied02
 * @tc.desc: OnRemoteDied_ShouldReturnTrue_WhenAgentNotFound
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnRemoteDied02, TestSize.Level1)
{
    sptr<IRemoteObject> agent = sptr<MockIRemoteObject>::MakeSptr();
    EXPECT_TRUE(ssm_->OnRemoteDied(agent));
}

/**
 * @tc.name: OnRemoteDied03
 * @tc.desc: OnRemoteDied_ShouldRemoveAgent_WhenAgentFoundAndNoScreens
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnRemoteDied03, TestSize.Level1)
{
    sptr<IRemoteObject> agent = sptr<MockIRemoteObject>::MakeSptr();
    ssm_->screenAgentMap_[agent] = {};

    EXPECT_TRUE(ssm_->OnRemoteDied(agent));
}

/**
 * @tc.name: GetExpandAvailableArea
 * @tc.desc: GetExpandAvailableArea test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetExpandAvailableArea, TestSize.Level1)
{
    DMRect area;
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ssm_->GetExpandAvailableArea(SCREEN_ID_INVALID, area));
    DisplayId id = 0;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(id, ScreenProperty(), 0);
    ssm_->screenSessionMap_[id] = screenSession;
    ASSERT_NE(nullptr, screenSession);
    EXPECT_EQ(DMError::DM_OK, ssm_->GetExpandAvailableArea(id, area));
}

/**
 * @tc.name: GetFakePhysicalScreenSession001
 * @tc.desc: Test that the function returns nullptr when g_isPcDevice is false
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetFakePhysicalScreenSession001, TestSize.Level1)
{
    ScreenId screenId = 1;
    ScreenId defScreenId = 2;
    ScreenProperty property;

    auto screenSession = ScreenSessionManager::GetInstance().GetFakePhysicalScreenSession(screenId, defScreenId,
        property);
    EXPECT_EQ(screenSession, nullptr);
}

/**
 * @tc.name: CreateFakePhysicalMirrorSessionInner
 * @tc.desc: CreateFakePhysicalMirrorSessionInner
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CreateFakePhysicalMirrorSessionInner, TestSize.Level1)
{
    ScreenId screenId = 1;
    ScreenId defScreenId = 2;
    ScreenProperty property;

    auto screenSession = ScreenSessionManager::GetInstance().CreateFakePhysicalMirrorSessionInner(screenId, defScreenId,
        property);
    EXPECT_EQ(screenSession, nullptr);
}

/**
 * @tc.name: GetPhysicalScreenSessionInner
 * @tc.desc: GetPhysicalScreenSessionInner
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPhysicalScreenSessionInner, TestSize.Level1)
{
    ScreenId screenId = 1;
    ScreenProperty property;

    auto screenSession = ScreenSessionManager::GetInstance().GetPhysicalScreenSessionInner(screenId, property);
    EXPECT_EQ(screenSession, nullptr);
}

/**
 * @tc.name: GetOrCreatePhysicalScreenSession
 * @tc.desc: Test scenario where no existing physical screen session exists and new session creation fails
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetOrCreatePhysicalScreenSession, TestSize.Level1)
{
    ScreenId screenId = 1;
    sptr<ScreenSession> result = ScreenSessionManager::GetInstance().GetOrCreatePhysicalScreenSession(screenId);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetScreenSessionByRsId01
 * @tc.desc: GetScreenSessionByRsId01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenSessionByRsId01, TestSize.Level1)
{
    ssm_->screenSessionMap_.erase(123);
    ScreenId rsScreenId = 123;
    sptr<ScreenSession> newSession = new ScreenSession();
    newSession->SetRSScreenId(rsScreenId);
    ssm_->screenSessionMap_[rsScreenId] = newSession;

    sptr<ScreenSession> result = ssm_->GetScreenSessionByRsId(rsScreenId);
    EXPECT_NE(result, nullptr);
    ssm_->screenSessionMap_.erase(123);
}

/**
 * @tc.name: GetScreenSessionByRsId02
 * @tc.desc: GetScreenSessionByRsId02
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenSessionByRsId02, TestSize.Level1)
{
    ssm_->screenSessionMap_.clear();
    ScreenId rsScreenId = 123;
    ssm_->screenSessionMap_[rsScreenId] = nullptr;

    sptr<ScreenSession> result = ssm_->GetScreenSessionByRsId(rsScreenId);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetPhysicalScreenSession001
 * @tc.desc: Test GetPhysicalScreenSession function when screenId is not found in the map
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPhysicalScreenSession001, TestSize.Level1)
{
    ScreenId screenId = 123;
    EXPECT_EQ(ssm_->GetPhysicalScreenSession(screenId), nullptr);
}

/**
 * @tc.name: GetPhysicalScreenSession002
 * @tc.desc: Test GetPhysicalScreenSession function when screenSessionMap is empty
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPhysicalScreenSession002, TestSize.Level1)
{
    ssm_->screenAgentMap_.clear();
    ScreenId screenId = 123;
    EXPECT_EQ(ssm_->GetPhysicalScreenSession(screenId), nullptr);
}

/**
 * @tc.name: OnScreenModeChange
 * @tc.desc: OnScreenModeChange
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnScreenModeChange, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ASSERT_EQ(ssm_->clientProxy_, nullptr);

    ScreenModeChangeEvent screenModeChangeEvent = ScreenModeChangeEvent::UNKNOWN;
    ssm_->OnScreenModeChange(screenModeChangeEvent);
}

/**
 * @tc.name: GetIsFoldStatusLocked/SetIsFoldStatusLocked
 * @tc.desc: GetIsFoldStatusLocked/SetIsFoldStatusLocked
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetIsFoldStatusLocked, Function | SmallTest | Level3)
{
    ssm_->SetIsFoldStatusLocked(true);
    EXPECT_EQ(ssm_->GetIsFoldStatusLocked(), true);

    ssm_->SetIsFoldStatusLocked(false);
    EXPECT_EQ(ssm_->GetIsFoldStatusLocked(), false);
}

/**
 * @tc.name: SetFoldStatusExpandAndLocked
 * @tc.desc: SetFoldStatusExpandAndLocked
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetFoldStatusExpandAndLocked, Function | SmallTest | Level3)
{
    if (!FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        GTEST_SKIP();
    }
    ssm_->SetFoldStatusExpandAndLocked(true);
    EXPECT_EQ(ssm_->GetIsFoldStatusLocked(), true);

    ssm_->SetFoldStatusExpandAndLocked(false);
    EXPECT_EQ(ssm_->GetIsFoldStatusLocked(), false);
}

/**
 * @tc.name: CheckMultiScreenInfoMap01
 * @tc.desc: Test CheckMultiScreenInfoMap function when the map is empty.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CheckMultiScreenInfoMap01, TestSize.Level1)
{
    std::map<std::string, MultiScreenInfo> emptyMap;
    EXPECT_FALSE(ScreenSessionManager::GetInstance().CheckMultiScreenInfoMap(emptyMap, "serial123"));
}

/**
 * @tc.name: CheckMultiScreenInfoMap02
 * @tc.desc: Test CheckMultiScreenInfoMap function when the serial number is empty.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CheckMultiScreenInfoMap02, TestSize.Level1)
{
    std::map<std::string, MultiScreenInfo> nonEmptyMap;
    nonEmptyMap["serial123"] = MultiScreenInfo();
    EXPECT_FALSE(ScreenSessionManager::GetInstance().CheckMultiScreenInfoMap(nonEmptyMap, ""));
}

/**
 * @tc.name: CheckMultiScreenInfoMap03
 * @tc.desc: Test CheckMultiScreenInfoMap function when the serial number is not found in the map.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CheckMultiScreenInfoMap03, TestSize.Level1)
{
    std::map<std::string, MultiScreenInfo> nonEmptyMap;
    nonEmptyMap["serial123"] = MultiScreenInfo();
    EXPECT_FALSE(ScreenSessionManager::GetInstance().CheckMultiScreenInfoMap(nonEmptyMap, "serial456"));
}

/**
 * @tc.name: CheckMultiScreenInfoMap04
 * @tc.desc: Test CHeckMultiScreenInfoMap function when all checks pass.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CheckMultiScreenInfoMap04, TestSize.Level1)
{
    std::map<std::string, MultiScreenInfo> nonEmptyMap;
    nonEmptyMap["serial123"] = MultiScreenInfo();
    EXPECT_TRUE(ScreenSessionManager::GetInstance().CheckMultiScreenInfoMap(nonEmptyMap, "serial123"));
}

/**
 * @tc.name: AdaptSuperHorizonalBoot
 * @tc.desc: AdaptSuperHorizonalBoot
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AdaptSuperHorizonalBoot, Function | SmallTest | Level3)
{
    if (!FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        GTEST_SKIP();
    }
    ScreenSessionConfig config;
    sptr<ScreenSession> session = new ScreenSession(config,
        ScreenSessionReason::CREATE_SESSION_FOR_REAL);
    ScreenId id = 0;
    
    ssm_->AdaptSuperHorizonalBoot(session, id);
    EXPECT_EQ(session->GetRotation(), Rotation::ROTATION_0);
}

/**
 * @tc.name: HandleSuperFoldStatusLocked
 * @tc.desc: HandleSuperFoldStatusLocked
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HandleSuperFoldStatusLocked, Function | SmallTest | Level3)
{
    if (!FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        GTEST_SKIP();
    }
    ssm_->HandleSuperFoldStatusLocked(true);
    EXPECT_EQ(ssm_->GetIsFoldStatusLocked(), true);

    ssm_->HandleSuperFoldStatusLocked(false);
    EXPECT_EQ(ssm_->GetIsFoldStatusLocked(), false);
}

/**
 * @tc.name: HandleMainScreenDisconnect
 * @tc.desc: HandleMainScreenDisconnect test
 * @tc.type: not main screen
 */
HWTEST_F(ScreenSessionManagerTest, HandleMainScreenDisconnect01, TestSize.Level1)
{
#ifndef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);

    ScreenId id = 1001;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);

    ssm_->HandleMainScreenDisconnect(screenSession);
    EXPECT_EQ(screenSession->GetScreenCombination(), ScreenCombination::SCREEN_MIRROR);
#endif
}

/**
 * @tc.name: HandleMainScreenDisconnect
 * @tc.desc: HandleMainScreenDisconnect test
 * @tc.type: main screen
 */
HWTEST_F(ScreenSessionManagerTest, HandleMainScreenDisconnect02, TestSize.Level1)
{
#ifndef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);

    ScreenId id = 1001;
    sptr<ScreenSession> screenSession = new ScreenSession(id, ScreenProperty(), 0);
    ASSERT_NE(nullptr, screenSession);
    screenSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);

    ssm_->HandleMainScreenDisconnect(screenSession);
    if (!g_isPcDevice) {
        EXPECT_EQ(screenSession->GetScreenCombination(), ScreenCombination::SCREEN_MAIN);
    } else {
        EXPECT_EQ(ssm_->GetIsOuterOnlyMode(), false);
    }
#endif
}

/**
 * @tc.name: HandleMainScreenDisconnect
 * @tc.desc: HandleMainScreenDisconnect test
 * @tc.type: main screen
 */
HWTEST_F(ScreenSessionManagerTest, HandleMainScreenDisconnect03, TestSize.Level1)
{
#ifndef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);

    ScreenId innerId = 1000;
    ScreenId extendId = 1001;
    ScreenSessionConfig innerConfig = {
        .screenId = innerId,
        .rsId = extendId,
        .defaultScreenId = innerId,
    };
    sptr<ScreenSession> innerSession = new ScreenSession(innerConfig,
        ScreenSessionReason::CREATE_SESSION_FOR_REAL);
    ASSERT_NE(nullptr, innerSession);
    innerSession->SetIsInternal(true);
    innerSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    innerSession->SetIsCurrentInUse(true);
    ssm_->screenSessionMap_.insert(std::make_pair(innerId, innerSession));

    ScreenSessionConfig extendConfig = {
        .screenId = extendId,
        .rsId = innerId,
        .defaultScreenId = innerId,
    };
    sptr<ScreenSession> extendSession = new ScreenSession(extendConfig,
        ScreenSessionReason::CREATE_SESSION_FOR_REAL);
    ASSERT_NE(nullptr, extendSession);
    extendSession->SetIsInternal(false);
    extendSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    extendSession->SetIsCurrentInUse(true);
    ssm_->screenSessionMap_.insert(std::make_pair(extendId, extendSession));

    ssm_->HandleMainScreenDisconnect(extendSession);
    if (!g_isPcDevice) {
        EXPECT_EQ(extendSession->GetScreenCombination(), ScreenCombination::SCREEN_MAIN);
    } else {
        EXPECT_EQ(extendSession->GetScreenCombination(), ScreenCombination::SCREEN_MIRROR);
    }
    ssm_->screenSessionMap_.erase(innerId);
    ssm_->screenSessionMap_.erase(extendId);
#endif
}

/**
 * @tc.name: ResetInternalScreenSession
 * @tc.desc: ResetInternalScreenSession test
 * @tc.type: session null
 */
HWTEST_F(ScreenSessionManagerTest, ResetInternalScreenSession01, TestSize.Level1)
{
#ifndef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);

    sptr<ScreenSession> innerSession = nullptr;
    sptr<ScreenSession> extendSession = nullptr;

    ssm_->ResetInternalScreenSession(innerSession, extendSession);
    EXPECT_EQ(nullptr, innerSession);
    EXPECT_EQ(nullptr, extendSession);
#endif
}

/**
 * @tc.name: ResetInternalScreenSession
 * @tc.desc: ResetInternalScreenSession test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ResetInternalScreenSession02, TestSize.Level1)
{
#ifndef WM_MULTI_SCREEN_ENABLE
    ASSERT_NE(ssm_, nullptr);

    ScreenId innerId = 1000;
    ScreenId extendId = 1001;
    ScreenSessionConfig innerConfig = {
        .screenId = innerId,
        .rsId = extendId,
        .defaultScreenId = innerId,
    };
    sptr<ScreenSession> innerSession = new ScreenSession(innerConfig,
        ScreenSessionReason::CREATE_SESSION_FOR_REAL);
    ASSERT_NE(nullptr, innerSession);
    innerSession->SetIsInternal(true);
    innerSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    ssm_->screenSessionMap_.insert(std::make_pair(innerId, innerSession));

    ScreenSessionConfig extendConfig = {
        .screenId = extendId,
        .rsId = innerId,
        .defaultScreenId = innerId,
    };
    sptr<ScreenSession> extendSession = new ScreenSession(extendConfig,
        ScreenSessionReason::CREATE_SESSION_FOR_REAL);
    ASSERT_NE(nullptr, extendSession);
    extendSession->SetIsInternal(false);
    extendSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    ssm_->screenSessionMap_.insert(std::make_pair(extendId, extendSession));

    ssm_->ResetInternalScreenSession(innerSession, extendSession);
    EXPECT_EQ(innerSession->GetIsInternal(), false);
    EXPECT_EQ(extendSession->GetIsInternal(), true);
    ssm_->screenSessionMap_.erase(innerId);
    ssm_->screenSessionMap_.erase(extendId);
#endif
}

/**
 * @tc.name: NotifyCreatedScreen
 * @tc.desc: NotifyCreatedScreen test
 * @tc.type: NotifyCreatedScreen test
 */
HWTEST_F(ScreenSessionManagerTest, NotifyCreatedScreen, TestSize.Level1)
{
    logMsg.clear();
    if (!g_isPcDevice) {
        GTEST_SKIP();
    }
    ASSERT_NE(ssm_, nullptr);
    LOG_SetCallback(MyLogCallback);
    ScreenId screenId = 1001;
    sptr<ScreenSession> screenSession = new ScreenSession(screenId, ScreenProperty(), 0);
    if (screenSession->GetScreenCombination() == ScreenCombination::SCREEN_MIRROR) {
        screenSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    }
    ssm_->NotifyCreatedScreen(screenSession);
    if (FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        EXPECT_TRUE(logMsg.find("super fold device, change by rotation.") == std::string::npos);
    } else {
        EXPECT_FALSE(logMsg.find("super fold device, change by rotation.") == std::string::npos);
    }
}

/**
 * @tc.name: SetPrimaryDisplaySystemDpi
 * @tc.desc: SetPrimaryDisplaySystemDpi
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetPrimaryDisplaySystemDpi, Function | SmallTest | Level3)
{
    DMError ret = ssm_->SetPrimaryDisplaySystemDpi(2.2);
    EXPECT_EQ(DMError::DM_OK, ret);
}

/**
 * @tc.name: HandleSwitchPcMode
 * @tc.desc: HandleSwitchPcMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HandleSwitchPcMode, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    if (!IS_SUPPORT_PC_MODE) {
        bool isPcDevice = ssm_->HandleSwitchPcMode();
        ASSERT_EQ(isPcDevice, g_isPcDevice);
        GTEST_SKIP();
    }
    bool isPcMode = system::GetBoolParameter("persist.sceneboard.ispcmode", false);
    bool isPcDevice = ssm_->HandleSwitchPcMode();
    if (isPcMode) {
        EXPECT_TRUE(isPcDevice);
    } else {
        EXPECT_FALSE(isPcDevice);
    }
}

/**
 * @tc.name: SwitchModeHandleExternalScreen
 * @tc.desc: SwitchModeHandleExternalScreen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SwitchModeHandleExternalScreen01, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = sptr<DisplayManagerAgentDefault>::MakeSptr();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "testVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    ASSERT_NE(screenSession, nullptr);
    ssm_->SwitchModeHandleExternalScreen(false);
    EXPECT_NE(screenSession->GetScreenCombination(), ScreenCombination::SCREEN_MIRROR);
    screenSession->SetIsRealScreen(true);
    ssm_->screenSessionMap_.insert(std::make_pair(777, nullptr));
    sptr<IDisplayManagerAgent> displayManagerAgent1 = sptr<DisplayManagerAgentDefault>::MakeSptr();
    ASSERT_NE(displayManagerAgent1, nullptr);
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption2";
    auto virtualScreenId = ssm_->CreateVirtualScreen(virtualOption1, displayManagerAgent1->AsObject());
    ssm_->SwitchModeHandleExternalScreen(false);
    EXPECT_EQ(screenSession->GetName(), "CastEngine");
    screenSession->SetIsRealScreen(false);
    ssm_->DestroyVirtualScreen(screenId);
    ssm_->DestroyVirtualScreen(virtualScreenId);
}

/**
 * @tc.name: SwitchModeHandleExternalScreen
 * @tc.desc: SwitchModeHandleExternalScreen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SwitchModeHandleExternalScreen02, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->screenSessionMap_.clear();
    sptr<IDisplayManagerAgent> displayManagerAgent = sptr<DisplayManagerAgentDefault>::MakeSptr();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "testVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    ASSERT_NE(screenSession, nullptr);
    screenSession->SetIsRealScreen(true);
    ssm_->SwitchModeHandleExternalScreen(true);
    EXPECT_NE(screenSession->GetScreenCombination(), ScreenCombination::SCREEN_MIRROR);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: CreateVirtualScreen
 * @tc.desc: CreateVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CreateVirtualScreen, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = sptr<DisplayManagerAgentDefault>::MakeSptr();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "CastEngine";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    ASSERT_NE(screenSession, nullptr);
    EXPECT_EQ(screenSession->GetVirtualScreenFlag(), VirtualScreenFlag::CAST);
    ssm_->DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: SetScreenOffsetFeatureTest
 * @tc.desc: SetScreenOffsetInner
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenOffsetFeatureTest, Function | SmallTest | Level3)
{
    ScreenId screenId = 0;
    EXPECT_TRUE(ssm_->SetScreenOffset(screenId, 0.0F, 0.0F));
    EXPECT_TRUE(ssm_->SetScreenOffset(screenId, 100.0F, 100.0F));
    screenId = -1;
    EXPECT_FALSE(ssm_->SetScreenOffset(screenId, 0.0F, 0.0F));
    EXPECT_FALSE(ssm_->SetScreenOffset(screenId, 100.0F, 100.0F));
}

+/**
+ * @tc.name: SetLapTopLidOpenStatus
+ * @tc.desc: test function : SetLapTopLidOpenStatus
+ * @tc.type: FUNC
+ */
HWTEST_F(ScreenSessionManagerTest, SetLapTopLidOpenStatus, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);

    ssm_->SetLapTopLidOpenStatus(true);
    bool isOpened = ssm_->IsLapTopLidOpen();
    EXPECT_EQ(true, isOpened);
}

+/**
+ * @tc.name: InitSecondaryDisplayPhysicalParams
+ * @tc.desc: test function : InitSecondaryDisplayPhysicalParams
+ * @tc.type: FUNC
+ */
HWTEST_F(ScreenSessionManagerTest, InitSecondaryDisplayPhysicalParams, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }
    ASSERT_NE(ssm_, nullptr);
    ssm_->InitSecondaryDisplayPhysicalParams();
    EXPECT_FALSE(ssm_->screenParams_.empty());
}

/**
 * @tc.name: GetPhyScreenId
 * @tc.desc: test function : GetPhyScreenId
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPhyScreenId, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        GTEST_SKIP();
    }
 
    ScreenId screenId = 0;
    ssm_->SetCoordinationFlag(true);
    ASSERT_EQ(ssm_->GetPhyScreenId(screenId), screenId);
 
    screenId = 5;
    ASSERT_EQ(ssm_->GetPhyScreenId(screenId), 0);
}
 
/**
 * @tc.name: UpdateCoordinationRefreshRate
 * @tc.desc: test function : UpdateCoordinationRefreshRate
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateCoordinationRefreshRate, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        GTEST_SKIP();
    }
    uint32_t refreshRate = 60;
    ssm_->SetCoordinationFlag(false);
    ssm_->UpdateCoordinationRefreshRate(refreshRate);
    EXPECT_FALSE(ssm_->GetScreenSession(5));
 
    refreshRate = 90;
    ssm_->SetCoordinationFlag(true);
    ssm_->UpdateCoordinationRefreshRate(refreshRate);
    EXPECT_TRUE(ssm_->GetScreenSession(0));
}

/**
@tc.name: SyncScreenPropertyChangedToServer
@tc.desc: test function : SyncScreenPropertyChangedToServer
@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerTest, SyncScreenPropertyChangedToServer, TestSize.Level1)
{
    ScreenId screenId = 1;
    ScreenProperty screenProperty;
    sptr<ScreenSession> screenSession = nullptr;
    ssm_->screenSessionMap_.erase(screenId);
    ssm_->screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    DMError ret = ssm_->SyncScreenPropertyChangedToServer(screenId, screenProperty);
    EXPECT_EQ(DMError::DM_ERROR_NULLPTR, ret);
    screenSession = new ScreenSession(screenId, screenProperty, 0);
    ssm_->screenSessionMap_.erase(screenId);
    ssm_->screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    std::function<void(sptr<ScreenSession> screenSession, SuperFoldStatusChangeEvents changeEvent)> func =
        [](sptr<ScreenSession> screenSession, SuperFoldStatusChangeEvents changeEvent) { return; };
    ssm_->SetPropertyChangedCallback(func);
    ret = ssm_->SyncScreenPropertyChangedToServer(screenId, screenProperty);
    EXPECT_EQ(DMError::DM_OK, ret);
}
/**
@tc.name: SetPropertyChangedCallback
@tc.desc: test function : SetPropertyChangedCallback
@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerTest, SetPropertyChangedCallback, TestSize.Level1)
{
    std::function<void(sptr<ScreenSession> screenSession, SuperFoldStatusChangeEvents changeEvent)> func = nullptr;
    ssm_->SetPropertyChangedCallback(func);
    EXPECT_EQ(ssm_->propertyChangedCallback_, nullptr);
    std::function<void(sptr<ScreenSession> screenSession, SuperFoldStatusChangeEvents changeEvent)> func1 =
        [](sptr<ScreenSession> screenSession, SuperFoldStatusChangeEvents changeEvent) { return; };
    ssm_->SetPropertyChangedCallback(func1);
    EXPECT_NE(ssm_->propertyChangedCallback_, nullptr);
}
/**
@tc.name: OnFoldPropertyChange
@tc.desc: test function : OnFoldPropertyChange
@tc.type: FUNC
*/
HWTEST_F(ScreenSessionManagerTest, OnFoldPropertyChange, TestSize.Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    ScreenId screenId = 1;
    ScreenProperty screenProperty;
    ScreenPropertyChangeReason reason = ScreenPropertyChangeReason::UNDEFINED;
    FoldDisplayMode displayMode = FoldDisplayMode::UNKNOWN;
    ssm_->clientProxy_ = nullptr;
    ssm_->OnFoldPropertyChange(screenId, screenProperty, reason, displayMode);
    EXPECT_TRUE(g_logMsg.find("clientProxy_ is null") != std::string::npos);
    g_logMsg.clear();

    ssm_->clientProxy_ = sptr<ScreenSessionManagerClientTest>::MakeSptr();
    ssm_->OnFoldPropertyChange(screenId, screenProperty, reason, displayMode);
}

/**
 * @tc.name: SetConfigForInputmethod
 * @tc.desc: SetConfigForInputmethod
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetConfigForInputmethod, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "testVirtualOption";
    auto screenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());
    VirtualScreenOption option;
    option.supportsFocus_ = false;
    option.supportsInput_ = false;
    ssm_->SetConfigForInputmethod(screenId, option);
    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(screenId);
    EXPECT_EQ(screenSession->GetSupportsFocus(), false);
    EXPECT_EQ(screenSession->GetSupportsInput(), false);

    option.supportsFocus_ = true;
    option.supportsInput_ = true;
    ssm_->SetConfigForInputmethod(screenId, option);
    screenSession = ssm_->GetScreenSession(screenId);
    EXPECT_EQ(screenSession->GetSupportsFocus(), true);
    EXPECT_EQ(screenSession->GetSupportsInput(), true);

    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    screenId++;
    ssm_->SetConfigForInputmethod(screenId, option);
    EXPECT_TRUE(logMsg.find("screenSession is nullptr!") != std::string::npos);
    LOG_SetCallback(nullptr);
}
}
} // namespace Rosen
} // namespace OHOS
