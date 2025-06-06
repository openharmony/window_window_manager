/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "multi_screen_power_change_manager.h"
#include "screen_session_manager/include/screen_session_manager.h"
#include "display_manager_agent_default.h"
#include "zidl/screen_session_manager_client_interface.h"
#include "common_test_utils.h"
#include "test_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class MultiScreenPowerChangeManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void SetAceessTokenPermission(const std::string processName);
    static MultiScreenPowerChangeManager& multiSPCM_;
    static ScreenSessionManager& ssm_;
};

MultiScreenPowerChangeManager& MultiScreenPowerChangeManagerTest::multiSPCM_ =
    MultiScreenPowerChangeManager::GetInstance();

ScreenSessionManager& MultiScreenPowerChangeManagerTest::ssm_ =
    ScreenSessionManager::GetInstance();

void MultiScreenPowerChangeManagerTest::SetUpTestCase()
{
    CommonTestUtils::InjectTokenInfoByHapName(0, "com.ohos.systemui", 0);
    const char** perms = new const char *[1];
    perms[0] = "ohos.permission.CAPTURE_SCREEN";
    CommonTestUtils::SetAceessTokenPermission("foundation", perms, 1);
}

void MultiScreenPowerChangeManagerTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void MultiScreenPowerChangeManagerTest::SetUp()
{
}

void MultiScreenPowerChangeManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: InitMultiScreenPowerChangeMap
 * @tc.desc: InitMultiScreenPowerChangeMap func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, InitMultiScreenPowerChangeMap, TestSize.Level1)
{
    multiSPCM_.handleScreenPowerChangeMap_.clear();
    ASSERT_TRUE(multiSPCM_.handleScreenPowerChangeMap_.empty());
    multiSPCM_.InitMultiScreenPowerChangeMap();
    ASSERT_FALSE(multiSPCM_.handleScreenPowerChangeMap_.empty());
    multiSPCM_.InitMultiScreenPowerChangeMap();
}

/**
 * @tc.name: OnMultiScreenPowerChangeRequest
 * @tc.desc: OnMultiScreenPowerChangeRequest func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, OnMultiScreenPowerChangeRequest, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = nullptr;
    sptr<ScreenSession> externalScreen = nullptr;
    MultiScreenPowerSwitchType switchStatus = MultiScreenPowerSwitchType::SCREEN_SWITCH_ON;
    auto ret = multiSPCM_.OnMultiScreenPowerChangeRequest(innerScreen, externalScreen, switchStatus);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSPCM_.OnMultiScreenPowerChangeRequest(innerScreen, externalScreen, switchStatus);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    innerScreen = ssm_.GetDefaultScreenSession();
    externalScreen = nullptr;
    ret = multiSPCM_.OnMultiScreenPowerChangeRequest(innerScreen, externalScreen, switchStatus);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSPCM_.OnMultiScreenPowerChangeRequest(innerScreen, externalScreen, switchStatus);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    switchStatus = MultiScreenPowerSwitchType::SCREEN_SWITCH_OFF;
    ret = multiSPCM_.OnMultiScreenPowerChangeRequest(innerScreen, externalScreen, switchStatus);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    switchStatus = MultiScreenPowerSwitchType::SCREEN_SWITCH_EXTERNAL;
    ret = multiSPCM_.OnMultiScreenPowerChangeRequest(innerScreen, externalScreen, switchStatus);
    ASSERT_NE(ret, DMError::DM_ERROR_INVALID_CALLING);
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: HandleScreenOnChange
 * @tc.desc: HandleScreenOnChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, HandleScreenOnChange, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = nullptr;
    sptr<ScreenSession> externalScreen = nullptr;
    auto ret = multiSPCM_.HandleScreenOnChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSPCM_.HandleScreenOnChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    innerScreen = ssm_.GetOrCreateScreenSession(0);
    externalScreen = nullptr;
    ret = multiSPCM_.HandleScreenOnChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSPCM_.HandleScreenOnChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: HandleScreenOffChange
 * @tc.desc: HandleScreenOffChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, HandleScreenOffChange, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = nullptr;
    sptr<ScreenSession> externalScreen = nullptr;
    auto ret = multiSPCM_.HandleScreenOffChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSPCM_.HandleScreenOffChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    innerScreen = ssm_.GetOrCreateScreenSession(0);
    externalScreen = nullptr;
    ret = multiSPCM_.HandleScreenOffChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSPCM_.HandleScreenOffChange(innerScreen, externalScreen);
    ASSERT_NE(ret, DMError::DM_ERROR_NULLPTR);
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: InitRecoveryMultiScreenModeChangeMap
 * @tc.desc: InitRecoveryMultiScreenModeChangeMap func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, InitRecoveryMultiScreenModeChangeMap, TestSize.Level1)
{
    ASSERT_FALSE(multiSPCM_.handleScreenPowerChangeMap_.empty());
    multiSPCM_.handleScreenPowerChangeMap_.clear();
    ASSERT_TRUE(multiSPCM_.handleScreenPowerChangeMap_.empty());
    multiSPCM_.InitRecoveryMultiScreenModeChangeMap();
}

/**
 * @tc.name: InitMultiScreenModeOffChangeMap
 * @tc.desc: InitMultiScreenModeOffChangeMap func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, InitMultiScreenModeOffChangeMap, TestSize.Level1)
{
    multiSPCM_.handleScreenPowerChangeMap_.clear();
    ASSERT_TRUE(multiSPCM_.handleScreenPowerChangeMap_.empty());
    multiSPCM_.InitMultiScreenModeOffChangeMap();
}

/**
 * @tc.name: HandleScreenOnlyExternalModeChange
 * @tc.desc: HandleScreenOnlyExternalModeChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, HandleScreenOnlyExternalModeChange, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = nullptr;
    sptr<ScreenSession> externalScreen = nullptr;
    auto ret = multiSPCM_.HandleScreenOnlyExternalModeChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSPCM_.HandleScreenOnlyExternalModeChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    innerScreen = ssm_.GetOrCreateScreenSession(0);
    externalScreen = nullptr;
    ret = multiSPCM_.HandleScreenOnlyExternalModeChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSPCM_.HandleScreenOnlyExternalModeChange(innerScreen, externalScreen);
    ASSERT_NE(ret, DMError::DM_ERROR_NULLPTR);
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: ScreenDisplayNodeRemove
 * @tc.desc: ScreenDisplayNodeRemove func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, ScreenDisplayNodeRemove, TestSize.Level1)
{
    sptr<ScreenSession> screenScreen = new ScreenSession();
    ASSERT_NE(nullptr, screenScreen);
    multiSPCM_.ScreenDisplayNodeRemove(screenScreen);
}

/**
 * @tc.name: HandleInnerMirrorExternalMainChange
 * @tc.desc: HandleInnerMirrorExternalMainChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, HandleInnerMirrorExternalMainChange, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = nullptr;
    sptr<ScreenSession> externalScreen = nullptr;
    auto ret = multiSPCM_.HandleInnerMirrorExternalMainChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    innerScreen = ssm_.GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    ScreenId screenId1 = ssm_.CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    externalScreen = ssm_.GetScreenSession(screenId1);
    innerScreen->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    externalScreen->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    sptr<IScreenSessionManagerClient> ssmClient = new ScreenSessionManagerClientTest();
    ssm_.SetClient(ssmClient);
    ret = multiSPCM_.HandleInnerMirrorExternalMainChange(innerScreen, externalScreen);
    ASSERT_EQ(DMError::DM_OK, ret);
    ssm_.DestroyVirtualScreen(screenId);
    ssm_.DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: HandleInnerMainExternalExtendChange
 * @tc.desc: HandleInnerMainExternalExtendChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, HandleInnerMainExternalExtendChange, TestSize.Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> innerScreen = ssm_.GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    ScreenId screenId1 = ssm_.CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    sptr<ScreenSession> externalScreen = ssm_.GetScreenSession(screenId1);

    innerScreen->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    externalScreen->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    sptr<IScreenSessionManagerClient> ssmClient = new ScreenSessionManagerClientTest();
    ssm_.SetClient(ssmClient);
    auto ret = multiSPCM_.HandleInnerMainExternalExtendChange(innerScreen, externalScreen);
    ASSERT_EQ(DMError::DM_OK, ret);
    ssm_.DestroyVirtualScreen(screenId);
    ssm_.DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: HandleInnerMainExternalMirrorChange
 * @tc.desc: HandleInnerMainExternalMirrorChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, HandleInnerMainExternalMirrorChange, TestSize.Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> innerScreen = ssm_.GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    ScreenId screenId1 = ssm_.CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    sptr<ScreenSession> externalScreen = ssm_.GetScreenSession(screenId1);
    innerScreen->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    externalScreen->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    sptr<IScreenSessionManagerClient> ssmClient = new ScreenSessionManagerClientTest();
    ssm_.SetClient(ssmClient);
    auto ret = multiSPCM_.HandleInnerMainExternalMirrorChange(innerScreen, externalScreen);
    ASSERT_EQ(DMError::DM_OK, ret);
    ssm_.DestroyVirtualScreen(screenId);
    ssm_.DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: HandleInnerExtendExternalMainChange
 * @tc.desc: HandleInnerExtendExternalMainChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, HandleInnerExtendExternalMainChange, TestSize.Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> innerScreen = ssm_.GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    ScreenId screenId1 = ssm_.CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    sptr<ScreenSession> externalScreen = ssm_.GetScreenSession(screenId1);
    innerScreen->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    externalScreen->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    sptr<IScreenSessionManagerClient> ssmClient = new ScreenSessionManagerClientTest();
    ssm_.SetClient(ssmClient);
    auto ret = multiSPCM_.HandleInnerExtendExternalMainChange(innerScreen, externalScreen);
    ASSERT_EQ(DMError::DM_OK, ret);
    ssm_.DestroyVirtualScreen(screenId);
    ssm_.DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: HandleRecoveryInnerMainExternalExtendChange
 * @tc.desc: HandleRecoveryInnerMainExternalExtendChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, HandleRecoveryInnerMainExternalExtendChange, TestSize.Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> innerScreen = ssm_.GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    ScreenId screenId1 = ssm_.CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    sptr<ScreenSession> externalScreen = ssm_.GetScreenSession(screenId1);
    innerScreen->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    externalScreen->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    sptr<IScreenSessionManagerClient> ssmClient = new ScreenSessionManagerClientTest();
    ssm_.SetClient(ssmClient);
    auto ret = multiSPCM_.HandleRecoveryInnerMainExternalExtendChange(innerScreen, externalScreen);
    ASSERT_EQ(DMError::DM_OK, ret);
    ssm_.DestroyVirtualScreen(screenId);
    ssm_.DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: HandleRecoveryInnerMainExternalMirrorChange
 * @tc.desc: HandleRecoveryInnerMainExternalMirrorChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, HandleRecoveryInnerMainExternalMirrorChange, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = new ScreenSession();
    sptr<ScreenSession> externalScreen = new ScreenSession();
    auto ret = multiSPCM_.HandleRecoveryInnerMainExternalMirrorChange(innerScreen, externalScreen);
    ASSERT_NE(ret, DMError::DM_ERROR_NULLPTR);
}

/**
 * @tc.name: HandleRecoveryInnerExtendExternalMainChange
 * @tc.desc: HandleRecoveryInnerExtendExternalMainChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, HandleRecoveryInnerExtendExternalMainChange, TestSize.Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> innerScreen = ssm_.GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    ScreenId screenId1 = ssm_.CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    sptr<ScreenSession> externalScreen = ssm_.GetScreenSession(screenId1);
    innerScreen->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    externalScreen->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    sptr<IScreenSessionManagerClient> ssmClient = new ScreenSessionManagerClientTest();
    ssm_.SetClient(ssmClient);
    auto ret = multiSPCM_.HandleRecoveryInnerExtendExternalMainChange(innerScreen, externalScreen);
    ASSERT_EQ(DMError::DM_OK, ret);
    ssm_.DestroyVirtualScreen(screenId);
    ssm_.DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: ScreenToExtendChange
 * @tc.desc: ScreenToExtendChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, ScreenToExtendChange, TestSize.Level1)
{
    sptr<IScreenSessionManagerClient> ssmClient = ScreenSessionManager::GetInstance().GetClientProxy();
    sptr<ScreenSession> screenSession = nullptr;
    multiSPCM_.ScreenToExtendChange(ssmClient, screenSession);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    screenSession = ssm_.GetScreenSession(screenId);
    multiSPCM_.ScreenToExtendChange(ssmClient, screenSession);
    ASSERT_NE(ssmClient, nullptr);
}

/**
 * @tc.name: CreateExternalScreenDisplayNodeOnly
 * @tc.desc: CreateExternalScreenDisplayNodeOnly func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, CreateExternalScreenDisplayNodeOnly, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = new ScreenSession();
    sptr<ScreenSession> externalScreen = new ScreenSession();
    ScreenCombination combination = ScreenCombination::SCREEN_MIRROR;
    ASSERT_NE(nullptr, innerScreen);
    ASSERT_NE(nullptr, externalScreen);
    multiSPCM_.CreateExternalScreenDisplayNodeOnly(innerScreen, externalScreen, combination);
    combination = ScreenCombination::SCREEN_EXPAND;
    multiSPCM_.CreateExternalScreenDisplayNodeOnly(innerScreen, externalScreen, combination);
    ASSERT_NE(nullptr, innerScreen);
}

/**
 * @tc.name: OnRecoveryScreenModeByPowerChange
 * @tc.desc: OnRecoveryScreenModeByPowerChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, OnRecoveryScreenModeByPowerChange, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = nullptr;
    sptr<ScreenSession> externalScreen = nullptr;
    auto ret = multiSPCM_.OnRecoveryScreenModeByPowerChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSPCM_.OnRecoveryScreenModeByPowerChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    innerScreen = ssm_.GetDefaultScreenSession();
    externalScreen = nullptr;
    ret = multiSPCM_.OnRecoveryScreenModeByPowerChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSPCM_.OnRecoveryScreenModeByPowerChange(innerScreen, externalScreen);
}

/**
 * @tc.name: HandleRecoveryInnerMirrorExternalMainChange
 * @tc.desc: HandleRecoveryInnerMirrorExternalMainChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenPowerChangeManagerTest, HandleRecoveryInnerMirrorExternalMainChange, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = new ScreenSession();
    sptr<ScreenSession> externalScreen = new ScreenSession();
    auto ret = multiSPCM_.HandleRecoveryInnerMirrorExternalMainChange(innerScreen, externalScreen);
    ASSERT_EQ(ret, DMError::DM_OK);
}
}
} // namespace Rosen
} // namespace OHOS