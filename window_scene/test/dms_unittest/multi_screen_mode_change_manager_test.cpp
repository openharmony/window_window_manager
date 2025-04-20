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

#include "multi_screen_mode_change_manager.h"
#include "screen_session_manager/include/screen_session_manager.h"
#include "display_manager_agent_default.h"
#include "common_test_utils.h"
#include "test_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class MultiScreenModeChangeManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void SetAceessTokenPermission(const std::string processName);
    static MultiScreenModeChangeManager& multiSMCM_;
    static ScreenSessionManager& ssm_;
};

MultiScreenModeChangeManager& MultiScreenModeChangeManagerTest::multiSMCM_ =
    MultiScreenModeChangeManager::GetInstance();

ScreenSessionManager& MultiScreenModeChangeManagerTest::ssm_ =
    ScreenSessionManager::GetInstance();

void MultiScreenModeChangeManagerTest::SetUpTestCase()
{
    CommonTestUtils::InjectTokenInfoByHapName(0, "com.ohos.systemui", 0);
    const char** perms = new const char *[1];
    perms[0] = "ohos.permission.CAPTURE_SCREEN";
    CommonTestUtils::SetAceessTokenPermission("foundation", perms, 1);
}

void MultiScreenModeChangeManagerTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void MultiScreenModeChangeManagerTest::SetUp()
{
}

void MultiScreenModeChangeManagerTest::TearDown()
{
}

namespace {

/**
 * @tc.name: InitMultiScreenModeChangeMap
 * @tc.desc: InitMultiScreenModeChangeMap func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, InitMultiScreenModeChangeMap, TestSize.Level1)
{
    multiSMCM_.InitMultiScreenModeChangeMap();
    multiSMCM_.HandleMultiScreenModeChangeMap_.clear();
    multiSMCM_.InitMultiScreenModeChangeMap();
    ASSERT_FALSE(multiSMCM_.HandleMultiScreenModeChangeMap_.empty());
}

/**
 * @tc.name: OnMultiScreenModeChangeRequest
 * @tc.desc: OnMultiScreenModeChangeRequest func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, OnMultiScreenModeChangeRequest, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = nullptr;
    sptr<ScreenSession> externalScreen = nullptr;
    std::string operateType = "mirror";
    auto ret = multiSMCM_.OnMultiScreenModeChangeRequest(innerScreen, externalScreen, operateType);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSMCM_.OnMultiScreenModeChangeRequest(innerScreen, externalScreen, operateType);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    innerScreen = ssm_.GetOrCreateScreenSession(0);
    externalScreen = nullptr;
    ret = multiSMCM_.OnMultiScreenModeChangeRequest(innerScreen, externalScreen, operateType);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSMCM_.OnMultiScreenModeChangeRequest(innerScreen, externalScreen, operateType);
    ASSERT_EQ(ret, DMError::DM_ERROR_INVALID_CALLING);
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: InitMultiScreenInnerMainExternalExtendModeModeChangeMap
 * @tc.desc: InitMultiScreenInnerMainExternalExtendModeModeChangeMap
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, InitMultiScreenInnerMainExternalExtendModeModeChangeMap, TestSize.Level1)
{
    ASSERT_FALSE(multiSMCM_.HandleMultiScreenInnerMainExternalExtendModeChangeMap_.empty());
    multiSMCM_.HandleMultiScreenInnerMainExternalExtendModeChangeMap_.clear();
    ASSERT_TRUE(multiSMCM_.HandleMultiScreenInnerMainExternalExtendModeChangeMap_.empty());
    multiSMCM_.InitMultiScreenInnerMainExternalExtendModeModeChangeMap();
    ASSERT_FALSE(multiSMCM_.HandleMultiScreenInnerMainExternalExtendModeChangeMap_.empty());
    multiSMCM_.InitMultiScreenInnerMainExternalExtendModeModeChangeMap();
}

/**
 * @tc.name: InitMultiScreenInnerMainExternalMirrorModeModeChangeMap
 * @tc.desc: InitMultiScreenInnerMainExternalMirrorModeModeChangeMap
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, InitMultiScreenInnerMainExternalMirrorModeModeChangeMap, TestSize.Level1)
{
    ASSERT_FALSE(multiSMCM_.HandleMultiScreenInnerMainExternalMirrorModeChangeMap_.empty());
    multiSMCM_.HandleMultiScreenInnerMainExternalMirrorModeChangeMap_.clear();
    ASSERT_TRUE(multiSMCM_.HandleMultiScreenInnerMainExternalMirrorModeChangeMap_.empty());
    multiSMCM_.InitMultiScreenInnerMainExternalMirrorModeModeChangeMap();
    ASSERT_FALSE(multiSMCM_.HandleMultiScreenInnerMainExternalMirrorModeChangeMap_.empty());
    multiSMCM_.InitMultiScreenInnerMainExternalMirrorModeModeChangeMap();
}

/**
 * @tc.name: InitMultiScreenInnerExtendExternalMainModeModeChangeMap
 * @tc.desc: InitMultiScreenInnerExtendExternalMainModeModeChangeMap
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, InitMultiScreenInnerExtendExternalMainModeModeChangeMap, TestSize.Level1)
{
    ASSERT_FALSE(multiSMCM_.HandleMultiScreenInnerExtendExternalMainModeChangeMap_.empty());
    multiSMCM_.HandleMultiScreenInnerExtendExternalMainModeChangeMap_.clear();
    ASSERT_TRUE(multiSMCM_.HandleMultiScreenInnerExtendExternalMainModeChangeMap_.empty());
    multiSMCM_.InitMultiScreenInnerExtendExternalMainModeModeChangeMap();
    ASSERT_FALSE(multiSMCM_.HandleMultiScreenInnerExtendExternalMainModeChangeMap_.empty());
    multiSMCM_.InitMultiScreenInnerExtendExternalMainModeModeChangeMap();
}

/**
 * @tc.name: InitMultiScreenInnerMirrorExternalMainModeModeChangeMap
 * @tc.desc: InitMultiScreenInnerMirrorExternalMainModeModeChangeMap
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, InitMultiScreenInnerMirrorExternalMainModeModeChangeMap, TestSize.Level1)
{
    ASSERT_FALSE(multiSMCM_.HandleMultiScreenInnerMirrorExternalMainModeChangeMap_.empty());
    multiSMCM_.HandleMultiScreenInnerMirrorExternalMainModeChangeMap_.clear();
    ASSERT_TRUE(multiSMCM_.HandleMultiScreenInnerMirrorExternalMainModeChangeMap_.empty());
    multiSMCM_.InitMultiScreenInnerMirrorExternalMainModeModeChangeMap();
    ASSERT_FALSE(multiSMCM_.HandleMultiScreenInnerMirrorExternalMainModeChangeMap_.empty());
    multiSMCM_.InitMultiScreenInnerMirrorExternalMainModeModeChangeMap();
}

/**
 * @tc.name: HandleInnerMainExternalExtendChange
 * @tc.desc: HandleInnerMainExternalExtendChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, HandleInnerMainExternalExtendChange, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = nullptr;
    sptr<ScreenSession> externalScreen = nullptr;
    ScreenCombination innerTargetCombination = ScreenCombination::SCREEN_MAIN;
    ScreenCombination externalTargetCombination = ScreenCombination::SCREEN_MIRROR;
    auto ret = multiSMCM_.HandleInnerMainExternalExtendChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSMCM_.HandleInnerMainExternalExtendChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    innerScreen = ssm_.GetDefaultScreenSession();
    externalScreen = nullptr;
    ret = multiSMCM_.HandleInnerMainExternalExtendChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSMCM_.HandleInnerMainExternalExtendChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    /* HandleInnerMainExternalExtendToInnerMainExternalMirror ssmClient null */
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    /* failed to find inner main external extend function handler! */
    innerTargetCombination = ScreenCombination::SCREEN_EXTEND;
    externalTargetCombination = ScreenCombination::SCREEN_MIRROR;
    ret = multiSMCM_.HandleInnerMainExternalExtendChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_INVALID_CALLING);
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: HandleInnerMainExternalMirrorChange
 * @tc.desc: HandleInnerMainExternalMirrorChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, HandleInnerMainExternalMirrorChange, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = nullptr;
    sptr<ScreenSession> externalScreen = nullptr;
    ScreenCombination innerTargetCombination = ScreenCombination::SCREEN_MAIN;
    ScreenCombination externalTargetCombination = ScreenCombination::SCREEN_EXTEND;
    auto ret = multiSMCM_.HandleInnerMainExternalMirrorChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSMCM_.HandleInnerMainExternalMirrorChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    innerScreen = ssm_.GetDefaultScreenSession();
    externalScreen = nullptr;
    ret = multiSMCM_.HandleInnerMainExternalMirrorChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    /* ssmClient null */
    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSMCM_.HandleInnerMainExternalMirrorChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);
   
    /* failed to find inner main external mirror function handler */
    innerTargetCombination = ScreenCombination::SCREEN_MAIN;
    externalTargetCombination = ScreenCombination::SCREEN_MIRROR;
    ret = multiSMCM_.HandleInnerMainExternalMirrorChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_INVALID_CALLING);
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: HandleInnerExtendExternalMainChange
 * @tc.desc: HandleInnerExtendExternalMainChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, HandleInnerExtendExternalMainChange, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = nullptr;
    sptr<ScreenSession> externalScreen = nullptr;
    ScreenCombination innerTargetCombination = ScreenCombination::SCREEN_MAIN;
    ScreenCombination externalTargetCombination = ScreenCombination::SCREEN_MIRROR;
    auto ret = multiSMCM_.HandleInnerExtendExternalMainChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSMCM_.HandleInnerExtendExternalMainChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    innerScreen = ssm_.GetDefaultScreenSession();
    externalScreen = nullptr;
    ret = multiSMCM_.HandleInnerExtendExternalMainChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSMCM_.HandleInnerExtendExternalMainChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    /* HandleInnerExtendExternalMainToInnerMainExternalMirror ssmclient null */
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    /* failed to find inner mirror external main function handler! */
    innerTargetCombination = ScreenCombination::SCREEN_EXTEND;
    externalTargetCombination = ScreenCombination::SCREEN_MAIN;
    ret = multiSMCM_.HandleInnerExtendExternalMainChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_INVALID_CALLING);
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: HandleInnerMirrorExternalMainChange
 * @tc.desc: HandleInnerMirrorExternalMainChange func
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, HandleInnerMirrorExternalMainChange, TestSize.Level1)
{
    sptr<ScreenSession> innerScreen = nullptr;
    sptr<ScreenSession> externalScreen = nullptr;
    ScreenCombination innerTargetCombination = ScreenCombination::SCREEN_MAIN;
    ScreenCombination externalTargetCombination = ScreenCombination::SCREEN_MIRROR;
    auto ret = multiSMCM_.HandleInnerMirrorExternalMainChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSMCM_.HandleInnerMirrorExternalMainChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    innerScreen = ssm_.GetDefaultScreenSession();
    externalScreen = nullptr;
    ret = multiSMCM_.HandleInnerMirrorExternalMainChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    externalScreen = ssm_.GetScreenSession(screenId);
    ret = multiSMCM_.HandleInnerMirrorExternalMainChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    /* HandleInnerMirrorExternalMainToInnerMainExternalMirror ssmClient null */
    ASSERT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    /* failed to find inner mirror external main function handler! */
    innerTargetCombination = ScreenCombination::SCREEN_MIRROR;
    externalTargetCombination = ScreenCombination::SCREEN_MAIN;
    ret = multiSMCM_.HandleInnerMirrorExternalMainChange(innerScreen, innerTargetCombination, externalScreen,
        externalTargetCombination);
    ASSERT_EQ(ret, DMError::DM_ERROR_INVALID_CALLING);
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: NotifyClientCreateExtendSessionOnly
 * @tc.desc: NotifyClientCreateExtendSessionOnly
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, NotifyClientCreateExtendSessionOnly, TestSize.Level1)
{
    sptr<IScreenSessionManagerClient> ssmClient = nullptr;
    sptr<ScreenSession> screenSession = nullptr;
    multiSMCM_.NotifyClientCreateExtendSessionOnly(ssmClient, screenSession);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    screenSession = ssm_.GetScreenSession(screenId);
    multiSMCM_.NotifyClientCreateExtendSessionOnly(ssmClient, screenSession);
    ASSERT_FALSE(screenSession->GetIsExtend());

    screenSession = nullptr;
    ssmClient = new TestClient();
    multiSMCM_.NotifyClientCreateExtendSessionOnly(ssmClient, screenSession);

    screenSession = ssm_.GetScreenSession(screenId);
    multiSMCM_.NotifyClientCreateExtendSessionOnly(ssmClient, screenSession);
    ASSERT_TRUE(screenSession->GetIsExtend());
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: NotifyClientCreateMirrorSessionOnly
 * @tc.desc: NotifyClientCreateMirrorSessionOnly
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, NotifyClientCreateMirrorSessionOnly, TestSize.Level1)
{
    sptr<IScreenSessionManagerClient> ssmClient = nullptr;
    sptr<ScreenSession> screenSession = nullptr;
    sptr<ScreenSession> mainSession = nullptr;
    multiSMCM_.NotifyClientCreateMirrorSessionOnly(ssmClient, screenSession, mainSession);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    screenSession = ssm_.GetScreenSession(screenId);
    multiSMCM_.NotifyClientCreateMirrorSessionOnly(ssmClient, screenSession, mainSession);
    ASSERT_FALSE(screenSession->GetIsExtend());

    mainSession = ssm_.GetDefaultScreenSession();
    multiSMCM_.NotifyClientCreateMirrorSessionOnly(ssmClient, screenSession, mainSession);
    ASSERT_FALSE(screenSession->GetIsExtend());

   
    screenSession = nullptr;
    multiSMCM_.NotifyClientCreateMirrorSessionOnly(ssmClient, screenSession, mainSession);

    screenSession = nullptr;
    ssmClient = new TestClient();
    multiSMCM_.NotifyClientCreateMirrorSessionOnly(ssmClient, screenSession, mainSession);

    screenSession = ssm_.GetScreenSession(screenId);
    multiSMCM_.NotifyClientCreateMirrorSessionOnly(ssmClient, screenSession, mainSession);
    ASSERT_TRUE(screenSession->GetIsExtend());
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: ScreenChangeToMirrorMode
 * @tc.desc: ScreenChangeToMirrorMode
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, ScreenChangeToMirrorMode, TestSize.Level1)
{
    sptr<IScreenSessionManagerClient> ssmClient = nullptr;
    sptr<ScreenSession> screenSession = nullptr;
    sptr<ScreenSession> mainSession = nullptr;
    multiSMCM_.ScreenChangeToMirrorMode(ssmClient, screenSession, mainSession);

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    screenSession = ssm_.GetScreenSession(screenId);
    multiSMCM_.ScreenChangeToMirrorMode(ssmClient, screenSession, mainSession);

    mainSession = ssm_.GetDefaultScreenSession();
    multiSMCM_.ScreenChangeToMirrorMode(ssmClient, screenSession, mainSession);
    ssmClient = new TestClient();
    multiSMCM_.ScreenChangeToMirrorMode(ssmClient, screenSession, mainSession);
    ASSERT_EQ(ScreenCombination::SCREEN_MIRROR, screenSession->GetScreenCombination());
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: ScreenDisplayNodeChangeNotify
 * @tc.desc: ScreenDisplayNodeChangeNotify
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, ScreenDisplayNodeChangeNotify, TestSize.Level1)
{
    sptr<IScreenSessionManagerClient> ssmClient = nullptr;
    sptr<ScreenSession> innerSession = ssm_.GetDefaultScreenSession();

    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> externalSession = ssm_.GetScreenSession(screenId);
    ssmClient = new TestClient();
    auto ret = multiSMCM_.ScreenDisplayNodeChangeNotify(ssmClient, innerSession, externalSession);
    ASSERT_EQ(DMError::DM_OK, ret);
    ssm_.DestroyVirtualScreen(screenId);
}

/**
 * @tc.name: HandleInnerMainExternalExtendToInnerMainExternalMirrorChange
 * @tc.desc: HandleInnerMainExternalExtendToInnerMainExternalMirrorChange
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, HandleInnerMainExternalExtendToInnerMainExternalMirrorChange,
    TestSize.Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> innerSession = ssm_.GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    ScreenId screenId1 = ssm_.CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    sptr<ScreenSession> externalSession = ssm_.GetScreenSession(screenId1);

    innerSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    externalSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    DMError ret = multiSMCM_.HandleInnerMainExternalExtendToInnerMainExternalMirrorChange(innerSession,
        externalSession);
    ASSERT_EQ(DMError::DM_OK, ret);

    externalSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    ret = multiSMCM_.HandleInnerMainExternalExtendToInnerMainExternalMirrorChange(innerSession, externalSession);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ret);
    
    innerSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    externalSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    ret = multiSMCM_.HandleInnerMainExternalExtendToInnerMainExternalMirrorChange(innerSession, externalSession);

    innerSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    externalSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    sptr<IScreenSessionManagerClient> ssmClient = new TestClient();
    ssm_.SetClient(ssmClient);
    ret = multiSMCM_.HandleInnerMainExternalExtendToInnerMainExternalMirrorChange(innerSession, externalSession);
    ASSERT_EQ(DMError::DM_OK, ret);
    ssm_.DestroyVirtualScreen(screenId);
    ssm_.DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: HandleInnerMainExternalExtendToInnerExtendExternalMainChange
 * @tc.desc: HandleInnerMainExternalExtendToInnerExtendExternalMainChange
 * @tc.type: FUNC
 */
HWTEST_F(MultiScreenModeChangeManagerTest, HandleInnerMainExternalExtendToInnerExtendExternalMainChange,
    TestSize.Level1)
{
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    ScreenId screenId = ssm_.CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> innerSession = ssm_.GetScreenSession(screenId);

    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    ScreenId screenId1 = ssm_.CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    sptr<ScreenSession> externalSession = ssm_.GetScreenSession(screenId1);

    ssm_.clientProxy_ = nullptr;
    innerSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    externalSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    DMError ret = multiSMCM_.HandleInnerMainExternalExtendToInnerExtendExternalMainChange(innerSession,
        externalSession);
    ASSERT_EQ(DMError::DM_OK, ret);

    externalSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    ret = multiSMCM_.HandleInnerMainExternalExtendToInnerExtendExternalMainChange(innerSession, externalSession);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, ret);
    
    innerSession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    externalSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    ret = multiSMCM_.HandleInnerMainExternalExtendToInnerExtendExternalMainChange(innerSession, externalSession);

    innerSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    externalSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    sptr<IScreenSessionManagerClient> ssmClient = new TestClient();
    ssm_.SetClient(ssmClient);
    ret = multiSMCM_.HandleInnerMainExternalExtendToInnerExtendExternalMainChange(innerSession, externalSession);
    ASSERT_EQ(DMError::DM_OK, ret);
    ssm_.DestroyVirtualScreen(screenId);
    ssm_.DestroyVirtualScreen(screenId1);
}
}
} // namespace Rosen
} // namespace OHOS
