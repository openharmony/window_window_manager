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
#include <parameter.h>
#include <parameters.h>

#include "display_manager_agent_default.h"
#include "screen_session_manager/include/screen_session_manager.h"
#include "screen_scene_config.h"
#include "screen_setting_helper.h"
#include "fold_screen_state_internel.h"
#include "mock/mock_accesstoken_kit.h"
#include "window_manager_hilog.h"
#include "test_client.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
constexpr uint32_t M_STATUS_WIDTH = 1008;
constexpr uint32_t F_STATUS_WIDTH = 2048;
constexpr uint32_t G_STATUS_WIDTH = 3184;
const ScreenId SCREENID = 1000;
constexpr uint32_t EXCEPTION_DPI = 10;
constexpr uint32_t PC_MODE_DPI = 304;
constexpr ScreenId SCREEN_ID_FULL = 0;
constexpr ScreenId SCREEN_ID_MAIN = 5;
const bool CORRECTION_ENABLE = system::GetIntParameter<int32_t>("const.system.sensor_correction_enable", 0) == 1;
bool g_isPcDevice = ScreenSceneConfig::GetExternalScreenDefaultMode() == "none";
}
namespace {
    std::string g_errLog;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char* tag,
        const char* msg)
    {
        g_errLog += msg;
    }
}
class ScreenSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<ScreenSessionManager> ssm_;
};

sptr<ScreenSessionManager> ScreenSessionManagerTest::ssm_ = nullptr;

void ScreenSessionManagerTest::SetUpTestCase()
{
    ssm_ = new ScreenSessionManager();
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
 * @tc.name: SetScreenPowerForFold01
 * @tc.desc: SetScreenPowerForFold test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForFold01, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsFoldScreenDevice()) {
        GTEST_SKIP();
    }
    // 内屏预上电
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<ScreenSessionManager> ssm = sptr<ScreenSessionManager>::MakeSptr();
    ssm->SetRSScreenPowerStatus(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);

    // 下电
    ssm->lastPowerForAllStatus_.store(ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);
    ssm->lastScreenId_.store(SCREEN_ID_FULL);
    ssm->SetScreenPowerForFold(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_OFF);
    if (FoldScreenStateInternel::IsSingleDisplayFoldDevice()) {
        EXPECT_TRUE(g_errLog.find("set advancedOn powerStatus off") != std::string::npos);
    } else {
        EXPECT_TRUE(g_errLog.find("set advancedOn powerStatus off") == std::string::npos);
    }
}

/**
 * @tc.name: SetScreenPowerForFold02
 * @tc.desc: SetScreenPowerForFold test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForFold02, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsFoldScreenDevice()) {
        GTEST_SKIP();
    }
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<ScreenSessionManager> ssm = sptr<ScreenSessionManager>::MakeSptr();
    ssm->SetRSScreenPowerStatus(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);

    ssm->lastPowerForAllStatus_.store(ScreenPowerStatus::POWER_STATUS_OFF);
    ssm->lastScreenId_.store(SCREEN_ID_FULL);
    ssm->SetScreenPowerForFold(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_OFF);
    EXPECT_TRUE(g_errLog.find("set advancedOn powerStatus off") == std::string::npos);
}

/**
 * @tc.name: SetScreenPowerForFold03
 * @tc.desc: SetScreenPowerForFold test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPowerForFold03, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsFoldScreenDevice()) {
        GTEST_SKIP();
    }
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<ScreenSessionManager> ssm = sptr<ScreenSessionManager>::MakeSptr();
    ssm->SetRSScreenPowerStatus(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);

    ssm->lastPowerForAllStatus_.store(ScreenPowerStatus::POWER_STATUS_ON_ADVANCED);
    ssm->lastScreenId_.store(SCREEN_ID_MAIN);
    ssm->SetScreenPowerForFold(SCREEN_ID_FULL, ScreenPowerStatus::POWER_STATUS_OFF);
    EXPECT_TRUE(g_errLog.find("set advancedOn powerStatus off") == std::string::npos);
}

/**
 * @tc.name: SwitchScrollParam01
 * @tc.desc: SwitchScrollParam test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SwitchScrollParam01, TestSize.Level1)
{
    ScreenSceneConfig::scrollableParams_.clear();
    vector<FoldDisplayMode> displayModeALL = {
        FoldDisplayMode::SUB,
        FoldDisplayMode::MAIN,
        FoldDisplayMode::FULL,
        FoldDisplayMode::UNKNOWN,
        FoldDisplayMode::COORDINATION,
    };
    std::map<FoldDisplayMode, std::string> scrollVelocityScaleParam = {
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::SUB, "1.0"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::MAIN, "1.1"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::FULL, "1.2"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::UNKNOWN, "1.3"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::COORDINATION, "1.4")
    };
    std::map<FoldDisplayMode, std::string> scrollFrictionParam = {
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::SUB, "1.0"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::MAIN, "2.0"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::FULL, "3.0"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::UNKNOWN, "4.0"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::COORDINATION, "5.0"),
    };
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    system::SetParameter("persist.scrollable.velocityScale", "0");
    system::SetParameter("persist.scrollable.friction", "0");
    std::string ret1, ret2;
    for (FoldDisplayMode displayMode : displayModeALL) {
        ScrollableParam scrollableParam;
        scrollableParam.velocityScale_ = scrollVelocityScaleParam.count(displayMode) ?
            scrollVelocityScaleParam[displayMode] : "0";
        scrollableParam.friction_ = scrollFrictionParam.count(displayMode) ?
            scrollFrictionParam[displayMode] : "0";
        ScreenSceneConfig::scrollableParams_[displayMode] = scrollableParam;
        ssm->SwitchScrollParam(displayMode);
        ret1 = system::GetParameter("persist.scrollable.velocityScale", "0");
        ret2 = system::GetParameter("persist.scrollable.friction", "0");
        EXPECT_NE(ret1, "");
        EXPECT_NE(ret2, "");
    }
}

/**
 * @tc.name: SwitchScrollParam02
 * @tc.desc: SwitchScrollParam test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SwitchScrollParam02, TestSize.Level1)
{
    ScreenSceneConfig::scrollableParams_.clear();
    vector<FoldDisplayMode> displayModeALL = {
        FoldDisplayMode::SUB,
        FoldDisplayMode::MAIN,
        FoldDisplayMode::FULL,
        FoldDisplayMode::UNKNOWN,
        FoldDisplayMode::COORDINATION,
    };
    std::map<FoldDisplayMode, std::string> scrollVelocityScaleParam = {
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::FULL, "2.0"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::MAIN, "main"),
        pair<FoldDisplayMode, std::string>(FoldDisplayMode::UNKNOWN, "!!"),
    };
    std::map<FoldDisplayMode, std::string> scrollFrictionParam;
    ScreenSessionManager* ssm = new ScreenSessionManager();
    ASSERT_NE(ssm, nullptr);
    system::SetParameter("persist.scrollable.velocityScale", "0");
    system::SetParameter("persist.scrollable.friction", "0");
    std::string ret1, ret2;
    for (FoldDisplayMode displayMode : displayModeALL) {
        ScrollableParam scrollableParam;
        scrollableParam.velocityScale_ = scrollVelocityScaleParam.count(displayMode) ?
            scrollVelocityScaleParam[displayMode] : "0";
        scrollableParam.friction_ = scrollFrictionParam.count(displayMode) ?
            scrollFrictionParam[displayMode] : "0";
        ScreenSceneConfig::scrollableParams_[displayMode] = scrollableParam;
        ssm->SwitchScrollParam(displayMode);
        ret1 = system::GetParameter("persist.scrollable.velocityScale", "0");
        ret2 = system::GetParameter("persist.scrollable.friction", "0");
        EXPECT_NE(ret1, "");
        EXPECT_NE(ret2, "");
    }
}

/**
 * @tc.name: WakeUpPictureFrameBlock
 * @tc.desc: WakeUpPictureFrameBlock test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WakeUpPictureFrameBlock, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->pictureFrameReady_ = false;
    ssm_->pictureFrameBreak_ = false;
    ssm_->WakeUpPictureFrameBlock(DisplayEvent::SCREEN_LOCK_OFF);
    ASSERT_EQ(ssm_->pictureFrameReady_, false);
    ASSERT_EQ(ssm_->pictureFrameBreak_, false);
    ssm_->WakeUpPictureFrameBlock(DisplayEvent::SCREEN_LOCK_START_DREAM);
    ASSERT_EQ(ssm_->pictureFrameReady_, true);
    ssm_->WakeUpPictureFrameBlock(DisplayEvent::SCREEN_LOCK_END_DREAM);
    ASSERT_EQ(ssm_->pictureFrameBreak_, true);
}

/**
 * @tc.name: AddVirtualScreenBlockList
 * @tc.desc: AddVirtualScreenBlockList test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddVirtualScreenBlockList, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    std::vector<int32_t> persistentIds {0, 1, 2};
    ASSERT_EQ(DMError::DM_OK, ssm_->AddVirtualScreenBlockList(persistentIds));
}

/**
 * @tc.name: RemoveVirtualScreenBlockList
 * @tc.desc: RemoveVirtualScreenBlockList test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RemoveVirtualScreenBlockList, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    std::vector<int32_t> persistentIds {0, 1, 2};
    ASSERT_EQ(DMError::DM_OK, ssm_->RemoveVirtualScreenBlockList(persistentIds));
}

/**
 * @tc.name: BlockScreenWaitPictureFrameByCV
 * @tc.desc: BlockScreenWaitPictureFrameByCV test
 * @tc.type: FUNC
 */
 HWTEST_F(ScreenSessionManagerTest, BlockScreenWaitPictureFrameByCV, Function | SmallTest | Level3)
 {
     ASSERT_NE(ssm_, nullptr);
     ssm_->pictureFrameReady_ = true;
     ssm_->pictureFrameBreak_ = true;
     bool result = ssm_->BlockScreenWaitPictureFrameByCV(true);
     ASSERT_EQ(result, true);
     result = ssm_->BlockScreenWaitPictureFrameByCV(false);
     ASSERT_EQ(result, true);

     ASSERT_EQ(ssm_->pictureFrameReady_, false);
     ASSERT_EQ(ssm_->pictureFrameBreak_, false);
}

/**
 * @tc.name: DestroyExtendVirtualScreen001
 * @tc.desc: DestroyExtendVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, DestroyExtendVirtualScreen001, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    ASSERT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto virtualScreenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());

    sptr<ScreenSession> screenSession = ssm_->GetScreenSession(virtualScreenId);
    ASSERT_NE(screenSession, nullptr);
    screenSession->SetScreenType(ScreenType::VIRTUAL);
    ssm_->DestroyExtendVirtualScreen();
    screenSession->SetIsExtendVirtual(true);
    ssm_->DestroyExtendVirtualScreen();
    ssm_->DestroyVirtualScreen(virtualScreenId);
}

/**
 * @tc.name: CreateExtendVirtualScreen001
 * @tc.desc: CreateExtendVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CreateExtendVirtualScreen001, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    ASSERT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto virtualScreenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());

    sptr<ScreenSession> virtualScreenSession = ssm_->GetScreenSession(virtualScreenId);
    virtualScreenSession->SetScreenType(ScreenType::REAL);
    ssm_->CreateExtendVirtualScreen(virtualScreenSession->GetRSScreenId());
    ssm_->DestroyVirtualScreen(virtualScreenId);
}

/**
 * @tc.name: IsPhysicalExtendScreenInUse001
 * @tc.desc: CreateExtendVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, IsPhysicalExtendScreenInUse001, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    ASSERT_NE(displayManagerAgent, nullptr);
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto virtualScreenId = ssm_->CreateVirtualScreen(virtualOption, displayManagerAgent->AsObject());

    sptr<ScreenSession> virtualScreenSession = ssm_->GetScreenSession(virtualScreenId);
    ASSERT_NE(virtualScreenSession, nullptr);

    virtualScreenSession->SetScreenType(ScreenType::VIRTUAL);
    DMError ret = ssm_->IsPhysicalExtendScreenInUse(virtualScreenSession->GetRSScreenId(), 100);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    VirtualScreenOption physicalOption;
    physicalOption.name_ = "createPhysicalOption";
    auto physicalScreenId = ssm_->CreateVirtualScreen(physicalOption, displayManagerAgent->AsObject());
    sptr<ScreenSession> physicalScreenSession = ssm_->GetScreenSession(physicalScreenId);
    ASSERT_NE(physicalScreenSession, nullptr);

    physicalScreenSession->SetScreenType(ScreenType::REAL);
    physicalScreenSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    DMError ret1 = ssm_->IsPhysicalExtendScreenInUse(virtualScreenSession->GetRSScreenId(),
        physicalScreenSession->GetRSScreenId());
    EXPECT_EQ(ret1, DMError::DM_OK);

    physicalScreenSession->SetScreenType(ScreenType::VIRTUAL);
    DMError ret2 = ssm_->IsPhysicalExtendScreenInUse(virtualScreenSession->GetRSScreenId(),
        physicalScreenSession->GetRSScreenId());
    EXPECT_EQ(ret2, DMError::DM_ERROR_UNKNOWN);

    ssm_->DestroyVirtualScreen(virtualScreenId);
    ssm_->DestroyVirtualScreen(physicalScreenId);
}

/**
 * @tc.name: SetForceCloseHdr01
 * @tc.desc: SetForceCloseHdr test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetForceCloseHdr01, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    ScreenId screenId = 0;
    bool isForceCloseHdr = true;
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(false);
    ssm_->SetForceCloseHdr(screenId, isForceCloseHdr);
    EXPECT_TRUE(g_errLog.find("permission denied!") != std::string::npos);
}

/**
 * @tc.name: SetForceCloseHdr02
 * @tc.desc: SetForceCloseHdr test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetForceCloseHdr02, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    ScreenId screenId = 0;
    bool isForceCloseHdr = true;
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(true);
    ssm_->SetForceCloseHdr(screenId, isForceCloseHdr);
    EXPECT_TRUE(g_errLog.find("permission denied!") == std::string::npos);
}

/**
 * @tc.name: SetForceCloseHdr03
 * @tc.desc: SetForceCloseHdr test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetForceCloseHdr03, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    ScreenId screenId = 0;
    bool isForceCloseHdr = true;
    MockAccesstokenKit::MockIsSACalling(true);
    MockAccesstokenKit::MockIsSystemApp(true);
    ssm_->SetForceCloseHdr(screenId, isForceCloseHdr);
    EXPECT_TRUE(g_errLog.find("permission denied!") == std::string::npos);
}

/**
 * @tc.name: SetDefaultMultiScreenModeWhenSwitchUser01
 * @tc.desc: SetDefaultMultiScreenModeWhenSwitchUser test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetDefaultMultiScreenModeWhenSwitchUser01, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(false);
    ssm_->SetDefaultMultiScreenModeWhenSwitchUser();
    EXPECT_TRUE(g_errLog.find("permission denied!") != std::string::npos);
}

/**
 * @tc.name: SetDefaultMultiScreenModeWhenSwitchUser02
 * @tc.desc: SetDefaultMultiScreenModeWhenSwitchUser test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetDefaultMultiScreenModeWhenSwitchUser02, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(true);
    ssm_->SetDefaultMultiScreenModeWhenSwitchUser();
    EXPECT_TRUE(g_errLog.find("permission denied!") == std::string::npos);
}

/**
 * @tc.name: SetDefaultMultiScreenModeWhenSwitchUser03
 * @tc.desc: SetDefaultMultiScreenModeWhenSwitchUser test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetDefaultMultiScreenModeWhenSwitchUser03, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(true);
    MockAccesstokenKit::MockIsSystemApp(true);
    ssm_->SetDefaultMultiScreenModeWhenSwitchUser();
    EXPECT_TRUE(g_errLog.find("permission denied!") == std::string::npos);
}

/**
 * @tc.name: NotifyExtendScreenCreateFinish01
 * @tc.desc: NotifyExtendScreenCreateFinish test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyExtendScreenCreateFinish01, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(false);
    ssm_->NotifyExtendScreenCreateFinish();
    EXPECT_TRUE(g_errLog.find("permission denied!") != std::string::npos);
}

/**
 * @tc.name: NotifyExtendScreenCreateFinish02
 * @tc.desc: NotifyExtendScreenCreateFinish test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyExtendScreenCreateFinish02, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(true);
    ssm_->NotifyExtendScreenCreateFinish();
    EXPECT_TRUE(g_errLog.find("permission denied!") == std::string::npos);
}

/**
 * @tc.name: NotifyExtendScreenCreateFinish03
 * @tc.desc: NotifyExtendScreenCreateFinish test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyExtendScreenCreateFinish03, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(true);
    MockAccesstokenKit::MockIsSystemApp(true);
    ssm_->NotifyExtendScreenCreateFinish();
    EXPECT_TRUE(g_errLog.find("permission denied!") == std::string::npos);
}

/**
 * @tc.name: NotifyExtendScreenDestroyFinish01
 * @tc.desc: NotifyExtendScreenDestroyFinish test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyExtendScreenDestroyFinish01, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(false);
    ssm_->NotifyExtendScreenDestroyFinish();
    EXPECT_TRUE(g_errLog.find("permission denied!") != std::string::npos);
}

/**
 * @tc.name: NotifyExtendScreenDestroyFinish02
 * @tc.desc: NotifyExtendScreenDestroyFinish test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyExtendScreenDestroyFinish02, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(true);
    ssm_->NotifyExtendScreenDestroyFinish();
    EXPECT_TRUE(g_errLog.find("permission denied!") == std::string::npos);
}

/**
 * @tc.name: NotifyExtendScreenDestroyFinish03
 * @tc.desc: NotifyExtendScreenDestroyFinish test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyExtendScreenDestroyFinish03, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(true);
    MockAccesstokenKit::MockIsSystemApp(true);
    ssm_->NotifyExtendScreenDestroyFinish();
    EXPECT_TRUE(g_errLog.find("permission denied!") == std::string::npos);
}

/**
 * @tc.name: SetFoldStatusExpandAndLocked01
 * @tc.desc: SetFoldStatusExpandAndLocked test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetFoldStatusExpandAndLocked01, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(false);
    bool isLocked = true;
    ssm_->SetFoldStatusExpandAndLocked(isLocked);
    EXPECT_TRUE(g_errLog.find("permission denied!") != std::string::npos);
}

/**
 * @tc.name: SetFoldStatusExpandAndLocked02
 * @tc.desc: SetFoldStatusExpandAndLocked test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetFoldStatusExpandAndLocked02, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(true);
    bool isLocked = true;
    ssm_->SetFoldStatusExpandAndLocked(isLocked);
    EXPECT_TRUE(g_errLog.find("permission denied!") == std::string::npos);
}

/**
 * @tc.name: SetFoldStatusExpandAndLocked03
 * @tc.desc: SetFoldStatusExpandAndLocked test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetFoldStatusExpandAndLocke03, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(true);
    MockAccesstokenKit::MockIsSystemApp(true);
    bool isLocked = true;
    ssm_->SetFoldStatusExpandAndLocked(isLocked);
    EXPECT_TRUE(g_errLog.find("permission denied!") == std::string::npos);
}

/**
 * @tc.name: OnBeforeScreenPropertyChange
 * @tc.desc: OnBeforeScreenPropertyChange
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnBeforeScreenPropertyChange, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->OnBeforeScreenPropertyChange(FoldStatus::UNKNOWN);
    ASSERT_EQ(ssm_->clientProxy_, nullptr);
}

/**
 * @tc.name: ScbStatusRecoveryWhenSwitchUser
 * @tc.desc: ScbStatusRecoveryWhenSwitchUser
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ScbStatusRecoveryWhenSwitchUser, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    if (FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        ScreenId id = ssm_->GetDefaultScreenId();
        sptr<ScreenSession> screenSession = ssm_->GetOrCreateScreenSession(id);
        ScreenProperty screenProperty = screenSession->GetScreenProperty();
        uint32_t currentWidth = screenProperty.GetBounds().rect_.GetWidth();
        std::vector<int32_t> oldScbPids_ = { 100 };
        int32_t newScbPid = 101;
        ssm_->ScbStatusRecoveryWhenSwitchUser(oldScbPids_, newScbPid);
        FoldDisplayMode mode = ssm_->GetFoldDisplayMode();
        if (mode == FoldDisplayMode::MAIN) {
            EXPECT_EQ(currentWidth, M_STATUS_WIDTH);
        } else if (mode == FoldDisplayMode::FULL) {
            EXPECT_EQ(currentWidth, F_STATUS_WIDTH);
        } else {
            EXPECT_EQ(currentWidth, G_STATUS_WIDTH);
        }
    }
}

/**
 * @tc.name: GetScreenAreaOfDisplayArea
 * @tc.desc: GetScreenAreaOfDisplayArea
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetScreenAreaOfDisplayArea, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->screenSessionMap_.clear();
    DisplayId displayId = 0;
    DMRect displayArea = { 0, 0, 1, 1 };
    ScreenId screenId = 0;
    DMRect screenArea = DMRect::NONE();
    sptr<ScreenSession> screenSession = nullptr;
    auto ret = ssm_->GetScreenAreaOfDisplayArea(displayId, displayArea, screenId, screenArea);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);

    ScreenProperty property = ScreenProperty();
    screenSession = new (std::nothrow) ScreenSession(0, property, 0);
    ASSERT_NE(screenSession, nullptr);
    ssm_->screenSessionMap_.clear();
    ssm_->screenSessionMap_.insert(std::make_pair(0, screenSession));
    ret = ssm_->GetScreenAreaOfDisplayArea(displayId, displayArea, screenId, screenArea);
    EXPECT_EQ(ret, DMError::DM_ERROR_INVALID_PARAM);

    property.SetOffsetX(0);
    property.SetOffsetY(0);
    property.SetCreaseRect({100, 100, 100, 100});
    property.SetBounds({{0, 0, 100, 100}, 0, 0});
    property.SetPhyBounds({{0, 100, 0, 100}, 0, 0});
    displayArea = { 0, 0, 0, 0 };
    screenSession->screenId_ = 0;
    ret = ssm_->GetScreenAreaOfDisplayArea(displayId, displayArea, screenId, screenArea);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: CalculateRotatedDisplay1
 * @tc.desc: CalculateRotatedDisplay1
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalculateRotatedDisplay1, Function | SmallTest | Level3)
{
    if (FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }

    ASSERT_NE(ssm_, nullptr);
    DMRect screenRegion = {0, 0, 100, 200};
    DMRect displayRegion = {0, 0, 200, 100};
    DMRect displayArea = {20, 10, 30, 40};

    Rotation rotation = Rotation::ROTATION_0;
    ssm_->CalculateRotatedDisplay(rotation, screenRegion, displayRegion, displayArea);
    DMRect expectedRect = {0, 0, 200, 100};
    EXPECT_EQ(displayRegion, expectedRect);
    expectedRect = {20, 10, 30, 40};
    EXPECT_EQ(displayArea, expectedRect);

    rotation = Rotation::ROTATION_90;
    displayRegion = {0, 0, 200, 100};
    displayArea = {20, 10, 30, 40};
    ssm_->CalculateRotatedDisplay(rotation, screenRegion, displayRegion, displayArea);
    expectedRect = {0, 0, 100, 200};
    EXPECT_EQ(displayRegion, expectedRect);
    expectedRect = {10, 150, 40, 30};
    EXPECT_EQ(displayArea, expectedRect);

    rotation = Rotation::ROTATION_180;
    displayRegion = {0, 0, 100, 200};
    displayArea = {20, 10, 30, 40};
    ssm_->CalculateRotatedDisplay(rotation, screenRegion, displayRegion, displayArea);
    expectedRect = {0, 0, 100, 200};
    EXPECT_EQ(displayRegion, expectedRect);
    expectedRect = {50, 150, 30, 40};
    EXPECT_EQ(displayArea, expectedRect);

    rotation = Rotation::ROTATION_270;
    displayRegion = {0, 0, 200, 100};
    displayArea = {20, 10, 30, 40};
    ssm_->CalculateRotatedDisplay(rotation, screenRegion, displayRegion, displayArea);
    expectedRect = {0, 0, 100, 200};
    EXPECT_EQ(displayRegion, expectedRect);
    expectedRect = {50, 20, 40, 30};
    EXPECT_EQ(displayArea, expectedRect);
}

/**
 * @tc.name: CalculateRotatedDisplay2
 * @tc.desc: CalculateRotatedDisplay2
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalculateRotatedDisplay2, Function | SmallTest | Level3)
{
    if (!FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        return;
    }

    ASSERT_NE(ssm_, nullptr);
    DMRect screenRegion = {0, 0, 100, 200};
    DMRect displayRegion = {0, 0, 200, 100};
    DMRect displayArea = {20, 10, 30, 40};

    Rotation rotation = Rotation::ROTATION_90;
    ssm_->CalculateRotatedDisplay(rotation, screenRegion, displayRegion, displayArea);
    DMRect expectedRect = {0, 0, 200, 100};
    EXPECT_EQ(displayRegion, expectedRect);
    expectedRect = {20, 10, 30, 40};
    EXPECT_EQ(displayArea, expectedRect);

    rotation = Rotation::ROTATION_180;
    displayRegion = {0, 0, 200, 100};
    displayArea = {20, 10, 30, 40};
    ssm_->CalculateRotatedDisplay(rotation, screenRegion, displayRegion, displayArea);
    expectedRect = {0, 0, 100, 200};
    EXPECT_EQ(displayRegion, expectedRect);
    expectedRect = {10, 150, 40, 30};
    EXPECT_EQ(displayArea, expectedRect);

    rotation = Rotation::ROTATION_270;
    displayRegion = {0, 0, 100, 200};
    displayArea = {20, 10, 30, 40};
    ssm_->CalculateRotatedDisplay(rotation, screenRegion, displayRegion, displayArea);
    expectedRect = {0, 0, 100, 200};
    EXPECT_EQ(displayRegion, expectedRect);
    expectedRect = {50, 150, 30, 40};
    EXPECT_EQ(displayArea, expectedRect);

    rotation = Rotation::ROTATION_0;
    displayRegion = {0, 0, 200, 100};
    displayArea = {20, 10, 30, 40};
    ssm_->CalculateRotatedDisplay(rotation, screenRegion, displayRegion, displayArea);
    expectedRect = {0, 0, 100, 200};
    EXPECT_EQ(displayRegion, expectedRect);
    expectedRect = {50, 20, 40, 30};
    EXPECT_EQ(displayArea, expectedRect);
}

/**
 * @tc.name: CalculateScreenArea
 * @tc.desc: CalculateScreenArea
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CalculateScreenArea, Function | SmallTest | Level3)
{
    if (FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        ASSERT_NE(ssm_, nullptr);
        DMRect displayRegion = DMRect::NONE();
        DMRect displayArea = {10, 20, 30, 40};
        DMRect screenRegion = DMRect::NONE();
        DMRect screenArea = DMRect::NONE();
        ssm_->CalculateScreenArea(displayRegion, displayArea, screenRegion, screenArea);
        EXPECT_EQ(screenArea, displayArea);
        return;
    }

    ASSERT_NE(ssm_, nullptr);
    DMRect displayRegion = {0, 0, 50, 100};
    DMRect displayArea = {10, 20, 30, 40};
    DMRect screenRegion = {0, 0, 50, 100};
    DMRect screenArea = DMRect::NONE();
    ssm_->CalculateScreenArea(displayRegion, displayArea, screenRegion, screenArea);
    EXPECT_EQ(screenArea, displayArea);

    screenRegion = {0, 0, 100, 200};
    ssm_->CalculateScreenArea(displayRegion, displayArea, screenRegion, screenArea);
    DMRect expectedRect = {20, 40, 60, 80};
    EXPECT_EQ(screenArea, expectedRect);
}

/**
 * @tc.name: ConvertIntToRotation
 * @tc.desc: ConvertIntToRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ConvertIntToRotation, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    int32_t rotation = 0;
    Rotation targetRotation = Rotation::ROTATION_0;
    targetRotation = ssm_->ConvertIntToRotation(rotation);
    EXPECT_EQ(targetRotation, Rotation::ROTATION_0);
    rotation = 100;
    targetRotation = ssm_->ConvertIntToRotation(rotation);
    EXPECT_EQ(targetRotation, Rotation::ROTATION_0);
    rotation = 90;
    targetRotation = ssm_->ConvertIntToRotation(rotation);
    EXPECT_EQ(targetRotation, Rotation::ROTATION_90);
    rotation = 180;
    targetRotation = ssm_->ConvertIntToRotation(rotation);
    EXPECT_EQ(targetRotation, Rotation::ROTATION_180);
    rotation = 270;
    targetRotation = ssm_->ConvertIntToRotation(rotation);
    EXPECT_EQ(targetRotation, Rotation::ROTATION_270);
}

/**
 * @tc.name: GetPhysicalScreenIds
 * @tc.desc: GetPhysicalScreenIds
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetPhysicalScreenIds, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    std::vector<ScreenId> screenIds;
    auto ret = ssm_->GetPhysicalScreenIds(screenIds);
    EXPECT_EQ(ret, DMError::DM_OK);

    ScreenId screenId = 11;
    sptr<ScreenSession> screenSession = nullptr;
    ssm_->screenSessionMap_.insert(std::make_pair(screenId, screenSession));
    std::vector<ScreenId> screenIds1;
    auto ret1 = ssm_->GetPhysicalScreenIds(screenIds1);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: SetVirtualScreenAutoRotation01
 * @tc.desc: SetVirtualScreenAutoRotation test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenAutoRotation01, Function | SmallTest | Level3)
{
    ScreenId screenId = 0;
    bool enable = true;
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSASystemApp(false);
    auto ret = ssm_->SetVirtualScreenAutoRotation(screenId, enable);
    EXPECT_EQ(ret, DMError::DM_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: SetVirtualScreenAutoRotation02
 * @tc.desc: SetVirtualScreenAutoRotation test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenAutoRotation02, Function | SmallTest | Level3)
{
    ScreenId screenId = 0;
    bool enable = true;
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSASystemApp(true);
    auto ret = ssm_->SetVirtualScreenAutoRotation(screenId, enable);
    EXPECT_EQ(ret, DMError::DM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: SetVirtualScreenAutoRotation03
 * @tc.desc: SetVirtualScreenAutoRotation test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetVirtualScreenAutoRotation03, Function | SmallTest | Level3)
{
    ScreenId screenId = 0;
    bool enable = true;
    MockAccesstokenKit::MockIsSACalling(true);
    MockAccesstokenKit::MockIsSASystemApp(true);
    auto ret = ssm_->SetVirtualScreenAutoRotation(screenId, enable);
    EXPECT_EQ(ret, DMError::DM_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: CreateScreenProperty
 * @tc.desc: CreateScreenProperty test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, CreateScreenProperty, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ScreenId screenId = 0;
    ScreenProperty property = ScreenProperty();
    EXPECT_EQ(DISPLAY_GROUP_ID_INVALID, property.GetDisplayGroupId());
    EXPECT_EQ(SCREEN_ID_INVALID, property.GetMainDisplayIdOfGroup());

    ssm_->CreateScreenProperty(screenId, property);
    EXPECT_EQ(DISPLAY_GROUP_ID_DEFAULT, property.GetDisplayGroupId());
    EXPECT_EQ(MAIN_SCREEN_ID_DEFAULT, property.GetMainDisplayIdOfGroup());

    property.SetDisplayGroupId(1);
    property.SetMainDisplayIdOfGroup(1);
    ssm_->CreateScreenProperty(screenId, property);
    EXPECT_EQ(1, property.GetDisplayGroupId());
    EXPECT_EQ(1, property.GetMainDisplayIdOfGroup());
}

/**
 * @tc.name: InitScreenProperty
 * @tc.desc: InitScreenProperty test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitScreenProperty, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ScreenId screenId = 0;
    ScreenProperty property = ScreenProperty();
    RSScreenModeInfo screenModeInfo = RSScreenModeInfo();
    screenModeInfo.SetScreenWidth(100);
    screenModeInfo.SetScreenHeight(200);
    RSScreenCapability screenCapability = RSScreenCapability();

    ssm_->InitScreenProperty(screenId, screenModeInfo, screenCapability, property);
    EXPECT_EQ(0, property.GetScreenAreaOffsetX());
    EXPECT_EQ(0, property.GetScreenAreaOffsetY());
    EXPECT_EQ(100, property.GetScreenAreaWidth());
    EXPECT_EQ(200, property.GetScreenAreaHeight());
}

/**
 * @tc.name: InitExtendScreenProperty
 * @tc.desc: InitExtendScreenProperty test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitExtendScreenProperty, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    ScreenId screenId = 0;
    ScreenProperty property = ScreenProperty();
    property.SetBounds({{0, 0, 4000, 4000}, 0, 0});
    ScreenSceneConfig::enableConfig_["isSupportOffScreenRendering"] = true;
    ScreenSceneConfig::offScreenPPIThreshold_ = 100;
    ScreenSceneConfig::stringConfig_["externalScreenDefaultMode"] = "none";

    ssm_->Init();
    sptr<ScreenSession> screenSession = ssm_->GetScreenSessionInner(screenId, property);
    ssm_->InitExtendScreenProperty(screenId, screenSession, property);
    EXPECT_EQ(3840, screenSession->GetValidWidth());

    property.SetBounds({{0, 0, 1000, 1000}, 0, 0});
    ssm_->InitExtendScreenProperty(screenId, screenSession, property);
    EXPECT_EQ(3120, screenSession->GetValidWidth());
}

/**
 * @tc.name: GetOrCreateScreenSession
 * @tc.desc: GetOrCreateScreenSession
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetOrCreateScreenSession, Function | SmallTest | Level3)
{
    if (FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        sptr<ScreenSession> session = ssm_->GetOrCreateScreenSession(SCREENID);
        EXPECT_NE(session, nullptr);
    }
}

/**
 * @tc.name: SetScreenPrivacyWindowTagSwitch001
 * @tc.desc: SetScreenPrivacyWindowTagSwitch test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPrivacyWindowTagSwitch001, TestSize.Level1)
{
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(false);
    ScreenId mainScreenId = 0;
    std::vector<std::string> privacyWindowTag{"test1", "test2"};
    auto ret = ssm_->SetScreenPrivacyWindowTagSwitch(mainScreenId, privacyWindowTag, true);
    EXPECT_EQ(ret, DMError::DM_ERROR_NOT_SYSTEM_APP);
}

/**
 * @tc.name: SetScreenPrivacyWindowTagSwitch002
 * @tc.desc: SetScreenPrivacyWindowTagSwitch test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetScreenPrivacyWindowTagSwitch002, TestSize.Level1)
{
    MockAccesstokenKit::MockIsSACalling(true);
    MockAccesstokenKit::MockIsSystemApp(true);
    ScreenId mainScreenId = 0;
    ScreenId invalidScreenId = -1;
    std::vector<std::string> privacyWindowTag{"test1", "test2"};
    auto ret = ssm_->SetScreenPrivacyWindowTagSwitch(invalidScreenId, privacyWindowTag, true);
    EXPECT_EQ(ret, DMError::DM_ERROR_NULLPTR);
    ret = ssm_->SetScreenPrivacyWindowTagSwitch(mainScreenId, privacyWindowTag, true);
    EXPECT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: UpdateSuperFoldRefreshRate
 * @tc.desc: UpdateSuperFoldRefreshRate test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateSuperFoldRefreshRate, TestSize.Level1)
{
    if (!FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        GTEST_SKIP();
    }
    g_errLog.clear();
    uint32_t tempRefreshRate = 60;
    sptr<ScreenSession> screenSession = nullptr;
    ssm_->UpdateSuperFoldRefreshRate(screenSession, tempRefreshRate);
    EXPECT_TRUE(g_errLog.find("screenSession is nullptr") != std::string::npos);

    ScreenId screenId = 1050;
    DMRect area{0, 0, 600, 800};
    screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    sptr<ScreenSession> fakescreenSession = nullptr;
    screenSession->SetFakeScreenSession(fakescreenSession);
    ssm_->UpdateSuperFoldRefreshRate(screenSession, tempRefreshRate);
    EXPECT_TRUE(g_errLog.find("fakeScreenSession is nullptr") != std::string::npos);
    g_errLog.clear();
}

/**
 * @tc.name: OnVerticalChangeBoundsWhenSwitchUser
 * @tc.desc: OnVerticalChangeBoundsWhenSwitchUser test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, OnVerticalChangeBoundsWhenSwitchUser, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ScreenId screenId = 1050;
    DMRect area{0, 0, 600, 800};
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(screenSession, nullptr);
    ScreenProperty screenProperty = screenSession->GetScreenProperty();
    auto rotation = screenProperty.GetRotation();
    constexpr float SECONDARY_ROTATION_90 = 90.0F;
    screenProperty.SetRotation(SECONDARY_ROTATION_90);
    RRect bounds = screenProperty.GetBounds();
    ssm_->OnVerticalChangeBoundsWhenSwitchUser(screenSession, FoldDisplayMode::UNKNOWN);
    RRect afterbounds = screenProperty.GetBounds();
    EXPECT_EQ(bounds.rect_.GetHeight(), bounds.rect_.GetWidth());
}

/**
 * @tc.name: SetLandscapeLockStatus01
 * @tc.desc: SetLandscapeLockStatus01 test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetLandscapeLockStatus01, TestSize.Level1)
{
    g_errLog.clear();
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(false);
    ssm_->SetLandscapeLockStatus(true);
    EXPECT_TRUE(g_errLog.find("permission denied!") != std::string::npos);
}

/**
 * @tc.name: NotifyDisplayChangedByUid
 * @tc.desc: NotifyDisplayChangedByUid test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyDisplayChangedByUid, TestSize.Level1)
{
    g_errLog.clear();
    ASSERT_NE(ssm_, nullptr);
    ssm_->screenSessionMap_.clear();
    ScreenId screenId = 1050;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(screenSession, nullptr);
    ssm_->screenSessionMap_[screenId] = screenSession;
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMapCopy = ssm_->screenSessionMap_;
    ssm_->NotifyDisplayChangedByUid(screenSessionMapCopy, DisplayChangeEvent::DISPLAY_SIZE_CHANGED, 2002);
    ssm_->screenSessionMap_.clear();
    ssm_->screenSessionMap_[1051] = nullptr;
    std::map<ScreenId, sptr<ScreenSession>> screenSessionMapCopy1 = ssm_->screenSessionMap_;
    ssm_->NotifyDisplayChangedByUid(screenSessionMapCopy1, DisplayChangeEvent::DISPLAY_SIZE_CHANGED, 2002);
    EXPECT_TRUE(g_errLog.find("screenSession is nullptr") != std::string::npos);
}

/**
 * @tc.name: NotifyDisplayChangedByUidInner
 * @tc.desc: NotifyDisplayChangedByUidInner test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyDisplayChangedByUidInner, TestSize.Level1)
{
    g_errLog.clear();
    ASSERT_NE(ssm_, nullptr);
    ScreenId screenId = 1050;
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ASSERT_NE(screenSession, nullptr);
    ssm_->NotifyDisplayChangedByUidInner(screenSession->ConvertToDisplayInfo(),
        DisplayChangeEvent::DISPLAY_SIZE_CHANGED, 2002);
    EXPECT_TRUE(g_errLog.find("notify end") != std::string::npos);
}

/**
 * @tc.name: SynchronizePowerStatusPermissionDenied
 * @tc.desc: SynchronizePowerStatus test permission denied.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SynchronizePowerStatusPermissionDenied, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(false);
    ScreenPowerState state = ScreenPowerState::POWER_ON;
    ssm_->SynchronizePowerStatus(state);
    EXPECT_TRUE(g_errLog.find("permission denied!") != std::string::npos);
}

/**
 * @tc.name: SynchronizePowerStatusNotSupport
 * @tc.desc: SynchronizePowerStatus not support state.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SynchronizePowerStatusNotSupport, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(true);
    MockAccesstokenKit::MockIsSystemApp(true);
    ScreenPowerState state = ScreenPowerState::INVALID_STATE;
    ssm_->SynchronizePowerStatus(state);
    EXPECT_TRUE(g_errLog.find("not support!") != std::string::npos);
}

/**
 * @tc.name: SynchronizePowerStatusOk
 * @tc.desc: SynchronizePowerStatus test ok.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SynchronizePowerStatusOk, Function | SmallTest | Level3)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSACalling(true);
    MockAccesstokenKit::MockIsSystemApp(true);
    ScreenPowerState state = ScreenPowerState::POWER_ON;
#define FOLD_ABILITY_ENABLE
#define POWERMGR_DISPLAY_MANAGER_ENABLE
    ssm_->SynchronizePowerStatus(state);
#undef POWERMGR_DISPLAY_MANAGER_ENABLE
#undef FOLD_ABILITY_ENABLE
    EXPECT_TRUE(g_errLog.find("notify brightness") != std::string::npos);
}

/**
 * @tc.name: GetSuperFoldStatus
 * @tc.desc: GetSuperFoldStatus test SystemCalling false.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetSuperFoldStatus01, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSystemApp(false);
    MockAccesstokenKit::MockIsSACalling(false);
    SuperFoldStatus superFoldStatus = ssm_->GetSuperFoldStatus();
    EXPECT_TRUE(g_errLog.find("Permission Denied") != std::string::npos);
    EXPECT_EQ(superFoldStatus, SuperFoldStatus::UNKNOWN);
}

/**
 * @tc.name: GetSuperFoldStatus
 * @tc.desc: GetSuperFoldStatus test SystemCalling true.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetSuperFoldStatus02, TestSize.Level1)
{
    MockAccesstokenKit::MockIsSystemApp(true);
#define FOLD_ABILITY_ENABLE
    SuperFoldStatus superFoldStatus = ssm_->GetSuperFoldStatus();
    SuperFoldStatus status = SuperFoldStateManager::GetInstance().GetCurrentStatus();
#undef FOLD_ABILITY_ENABLE
    if (!FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        EXPECT_EQ(superFoldStatus, SuperFoldStatus::UNKNOWN);
    } else {
        EXPECT_EQ(superFoldStatus, status);
    }
}

/**
 * @tc.name: NotifyScreenMaskAppear
 * @tc.desc: NotifyScreenMaskAppear test SystemCalling false.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyScreenMaskAppear01, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSystemApp(false);
    MockAccesstokenKit::MockIsSACalling(false);
    ssm_->NotifyScreenMaskAppear();
    EXPECT_TRUE(g_errLog.find("Permission Denied") != std::string::npos);
}

/**
 * @tc.name: NotifyScreenMaskAppear
 * @tc.desc: NotifyScreenMaskAppear test SystemCalling true.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, NotifyScreenMaskAppear02, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    MockAccesstokenKit::MockIsSystemApp(true);
    ssm_->NotifyScreenMaskAppear();
    bool hasLog = g_errLog.find("not pc device") != std::string::npos ||
        g_errLog.find("screen mask appeared") != std::string::npos;
    EXPECT_TRUE(hasLog);
}

/**
 * @tc.name: TryToCancelScreenOff01
 * @tc.desc: TryToCancelScreenOff test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, TryToCancelScreenOff01, TestSize.Level1)
{
    g_errLog.clear();
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(false);
    MockSessionPermission::MockIsStarByHdcd(false);
    ssm_->TryToCancelScreenOff();
    EXPECT_TRUE(g_errLog.find("Permission denied!") != std::string::npos);
}

/**
 * @tc.name: WaitUpdateAvailableAreaForPc01
 * @tc.desc: WaitUpdateAvailableAreaForPc test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WaitUpdateAvailableAreaForPc01, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    bool temp = ScreenSessionManager::GetInstance().GetPcStatus();
    ScreenSessionManager::GetInstance().SetPcStatus(true);
    ScreenSessionManager::GetInstance().WaitUpdateAvailableAreaForPc();
    EXPECT_TRUE(g_errLog.find("need wait update available area") == std::string::npos);
    g_errLog.clear();
    ScreenSessionManager::GetInstance().SetPcStatus(temp);
}

/**
 * @tc.name: WaitUpdateAvailableAreaForPc02
 * @tc.desc: WaitUpdateAvailableAreaForPc test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, WaitUpdateAvailableAreaForPc02, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    bool temp = ScreenSessionManager::GetInstance().GetPcStatus();
    ScreenSessionManager::GetInstance().SetPcStatus(true);
    ScreenSessionManager::GetInstance().needWaitAvailableArea_ = true;
    ScreenSessionManager::GetInstance().WaitUpdateAvailableAreaForPc();
    EXPECT_TRUE(g_errLog.find("wait update available area timeout") != std::string::npos);
    g_errLog.clear();
    ScreenSessionManager::GetInstance().SetPcStatus(temp);
}

/**
 * @tc.name: UpdateAvailableAreaWhenDisplayAdd01
 * @tc.desc: UpdateAvailableArea WhenDisplayAdd notify all
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateAvailableArea02, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    bool temp = ssm_->GetPcStatus();
    ssm_->SetPcStatus(true);
    ssm_->needWaitAvailableArea_ = true;

    ScreenId screenId = 1050;
    DMRect area{0, 0, 600, 900};
    DMRect area2{0, 0, 600, 800};
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ssm_->screenSessionMap_[screenId] = screenSession;
    auto screenSession1 = ssm_->GetScreenSession(screenId);
    EXPECT_EQ(screenSession1, screenSession);
    EXPECT_TRUE(screenSession->UpdateAvailableArea(area));
    ssm_->UpdateAvailableArea(screenId, area2);
    EXPECT_FALSE(ssm_->needWaitAvailableArea_);
    g_errLog.clear();
    ssm_->SetPcStatus(temp);
}

/**
 * @tc.name: UpdateAvailableAreaWhenDisplayAdd02
 * @tc.desc: UpdateAvailableArea WhenDisplayAdd not notify all
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateAvailableArea02, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    bool temp = ssm_->GetPcStatus();
    ssm_->SetPcStatus(true);
    ssm_->needWaitAvailableArea_ = false;

    ScreenId screenId = 1050;
    DMRect area{0, 0, 600, 800};
    sptr<ScreenSession> screenSession = new (std::nothrow) ScreenSession(screenId, ScreenProperty(), 0);
    ssm_->screenSessionMap_[screenId] = screenSession;
    ssm_->UpdateAvailableArea(screenId, area);
    auto screenSession1 = ssm_->GetScreenSession(screenId);
    EXPECT_EQ(screenSession1->GetAvailabelArea(), area);
    EXPECT_FALSE(ssm_->needWaitAvailableArea_);
    g_errLog.clear();
    ssm_->SetPcStatus(temp);
}

/**
 * @tc.name: ConfigureDpi01
 * @tc.desc: ConfigureDpi01
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ConfigureDpi01, Function | SmallTest | Level3)
{
    system::SetParameter("persist.sceneboard.ispcmode", "true");
    ScreenSceneConfig::intNumbersConfig_["pcModeDpi"].clear();
    ScreenSceneConfig::intNumbersConfig_["pcModeDpi"].emplace_back(EXCEPTION_DPI);
    ScreenSessionManager::GetInstance().ConfigureDpi();
    EXPECT_FALSE(ScreenSessionManager::GetInstance().densityDpi_ ==
        static_cast<float>(EXCEPTION_DPI) / BASELINE_DENSITY);

    ScreenSceneConfig::intNumbersConfig_["pcModeDpi"].clear();
    ScreenSceneConfig::intNumbersConfig_["pcModeDpi"].emplace_back(PC_MODE_DPI);
    ScreenSessionManager::GetInstance().ConfigureDpi();
    EXPECT_TRUE(ScreenSessionManager::GetInstance().densityDpi_ ==
        static_cast<float>(PC_MODE_DPI) / BASELINE_DENSITY);
    system::SetParameter("persist.sceneboard.ispcmode", "false");
}

/**
 * @tc.name: GetCancelSuspendStatus01
 * @tc.desc: GetCancelSuspendStatus01_BothFalse
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCancelSuspendStatus01, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->sessionDisplayPowerController_->needCancelNotify_ = false;
    ssm_->sessionDisplayPowerController_->canceledSuspend_ = false;
    EXPECT_FALSE(ssm_->GetCancelSuspendStatus());
}

/**
 * @tc.name: GetCancelSuspendStatus02
 * @tc.desc: GetCancelSuspendStatus02_NeedCancelNotifyTrue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCancelSuspendStatus02, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->sessionDisplayPowerController_->needCancelNotify_ = true;
    ssm_->sessionDisplayPowerController_->canceledSuspend_ = false;
    EXPECT_TRUE(ssm_->GetCancelSuspendStatus());
}

/**
 * @tc.name: GetCancelSuspendStatus03
 * @tc.desc: GetCancelSuspendStatus03_CanceledSuspendTrue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCancelSuspendStatus03, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->sessionDisplayPowerController_->needCancelNotify_ = false;
    ssm_->sessionDisplayPowerController_->canceledSuspend_ = true;
    EXPECT_TRUE(ssm_->GetCancelSuspendStatus());
}

/**
 * @tc.name: GetCancelSuspendStatus04
 * @tc.desc: GetCancelSuspendStatus04_BothTrue
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCancelSuspendStatus04, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    ssm_->sessionDisplayPowerController_->needCancelNotify_ = true;
    ssm_->sessionDisplayPowerController_->canceledSuspend_ = true;
    EXPECT_TRUE(ssm_->GetCancelSuspendStatus());
}

/**
 * @tc.name: HasSameScreenCastInfo
 * @tc.desc: HasSameScreenCastInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HasSameScreenCastInfo01, TestSize.Level1) {
    ASSERT_NE(ssm_, nullptr);
    ScreenId screenId = 1660;
    ScreenId mainScreenId = 0;
    EXPECT_FALSE(ssm_->HasSameScreenCastInfo(screenId, mainScreenId, ScreenCombination::SCREEN_MIRROR));
    ssm_->SetScreenCastInfo(screenId, mainScreenId, ScreenCombination::SCREEN_MIRROR);
    EXPECT_TRUE(ssm_->HasSameScreenCastInfo(screenId, mainScreenId, ScreenCombination::SCREEN_MIRROR));
    ssm_->RemoveScreenCastInfo(screenId);
    EXPECT_FALSE(ssm_->HasSameScreenCastInfo(screenId, mainScreenId, ScreenCombination::SCREEN_MIRROR));
}
 
/**
 * @tc.name: HasSameScreenCastInfo
 * @tc.desc: HasSameScreenCastInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HasSameScreenCastInfo02, TestSize.Level1) {
    ASSERT_NE(ssm_, nullptr);
    ScreenId screenId = 1660;
    ScreenId mainScreenId1 = 0;
    ScreenId mainScreenId2 = 1;
    ssm_->SetScreenCastInfo(screenId, mainScreenId1, ScreenCombination::SCREEN_MIRROR);
    EXPECT_FALSE(ssm_->HasSameScreenCastInfo(mainScreenId1, mainScreenId2, ScreenCombination::SCREEN_MIRROR));
    EXPECT_FALSE(ssm_->HasSameScreenCastInfo(screenId, mainScreenId2, ScreenCombination::SCREEN_MIRROR));
    EXPECT_FALSE(ssm_->HasSameScreenCastInfo(screenId, mainScreenId1, ScreenCombination::SCREEN_EXPAND));
    EXPECT_TRUE(ssm_->HasSameScreenCastInfo(screenId, mainScreenId1, ScreenCombination::SCREEN_MIRROR));
    ssm_->RemoveScreenCastInfo(screenId);
}
 
/**
 * @tc.name: RemoveScreenCastInfo
 * @tc.desc: RemoveScreenCastInfo
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RemoveScreenCastInfo, TestSize.Level1) {
    ASSERT_NE(ssm_, nullptr);
    ScreenId screenId = 1660;
    ScreenId mainScreenId = 0;
    ScreenId screenIdNotExist = 1080;
    ssm_->SetScreenCastInfo(screenId, mainScreenId, ScreenCombination::SCREEN_MIRROR);
    EXPECT_TRUE(ssm_->HasSameScreenCastInfo(screenId, mainScreenId, ScreenCombination::SCREEN_MIRROR));
    ssm_->RemoveScreenCastInfo(screenIdNotExist);
    EXPECT_TRUE(ssm_->HasSameScreenCastInfo(screenId, mainScreenId, ScreenCombination::SCREEN_MIRROR));
    ssm_->RemoveScreenCastInfo(screenId);
    EXPECT_FALSE(ssm_->HasSameScreenCastInfo(screenId, mainScreenId, ScreenCombination::SCREEN_MIRROR));
}
 
/**
 * @tc.name: ChangeScreenGroup
 * @tc.desc: ChangeScreenGroup test screenCastInfo has same
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ChangeScreenGroup, TestSize.Level1) {
    ASSERT_NE(ssm_, nullptr);
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    ScreenId groupId = 1;
    ScreenId screenId = 4080;
    ScreenId mainScreenId = 0;
    sptr<ScreenSessionGroup> group =
        sptr<ScreenSessionGroup>::MakeSptr(groupId, SCREEN_ID_INVALID, "test", ScreenCombination::SCREEN_MIRROR);
    group->mirrorScreenId_ = mainScreenId;
    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr(screenId, ScreenProperty(), 0);
    ssm_->screenSessionMap_[screenId] = screenSession;
    std::vector<ScreenId> screens;
    screens.emplace_back(screenId);
    DMRect region = { 0, 0, 1, 1 };
    Point point;
    std::vector<Point> startPoints;
    startPoints.insert(startPoints.begin(), screens.size(), point);
    ssm_->ChangeScreenGroup(group, screens, startPoints, true, ScreenCombination::SCREEN_MIRROR, region);
    EXPECT_TRUE(g_errLog.find("has not same cast info") != std::string::npos);
    g_errLog.clear();
    ssm_->SetScreenCastInfo(screenId, mainScreenId, ScreenCombination::SCREEN_MIRROR);
    ssm_->ChangeScreenGroup(group, screens, startPoints, true, ScreenCombination::SCREEN_MIRROR, region);
    EXPECT_FALSE(g_errLog.find("has not same cast info") != std::string::npos);
    g_errLog.clear();
    ssm_->RemoveScreenCastInfo(screenId);
}

/**
 * @tc.name: ChangeMirrorScreenConfig
 * @tc.desc: ChangeMirrorScreenConfig
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ChangeMirrorScreenConfig, TestSize.Level1) {
    ASSERT_NE(ssm_, nullptr);
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    ScreenId groupId = 1;
    ScreenId screenId = 4080;
    ScreenId mainScreenId = 0;
    sptr<ScreenSessionGroup> group =
        sptr<ScreenSessionGroup>::MakeSptr(groupId, SCREEN_ID_INVALID, "test", ScreenCombination::SCREEN_MIRROR);
    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr(screenId, ScreenProperty(), 0);
    ssm_->screenSessionMap_[screenId] = screenSession;
    group->mirrorScreenId_ = mainScreenId;
    DMRect region = { 0, 0, 1, 1 };
    ssm_->ChangeMirrorScreenConfig(group, region, screenSession);
    EXPECT_TRUE(g_errLog.find("convert to rs id failed") != std::string::npos);
    g_errLog.clear();
    ssm_->screenIdManager_.UpdateScreenId(screenId, screenId);
    ssm_->ChangeMirrorScreenConfig(group, region, screenSession);
    EXPECT_TRUE(g_errLog.find("with region") != std::string::npos);
    g_errLog.clear();
    group = nullptr;
    ssm_->ChangeMirrorScreenConfig(group, region, screenSession);
    EXPECT_FALSE(g_errLog.find("with region") != std::string::npos);
    g_errLog.clear();
    screenSession = nullptr;
    ssm_->ChangeMirrorScreenConfig(group, region, screenSession);
    EXPECT_FALSE(g_errLog.find("with region") != std::string::npos);
    g_errLog.clear();
}

/*
 * @tc.name: RegisterSettingDuringCallStateObserver
 * @tc.desc: RegisterSettingDuringCallStateObserver
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RegisterSettingDuringCallStateObserver, Function | SmallTest | Level3)
{
    if (g_isPcDevice) {
        GTEST_SKIP();
    }
    ASSERT_NE(ssm_, nullptr);
    ssm_->RegisterSettingDuringCallStateObserver();
    if (FoldScreenStateInternel::IsDualDisplayFoldDevice() && ScreenSceneConfig::IsSupportDuringCall()) {
        ASSERT_NE(ScreenSettingHelper::duringCallStateObserver_, nullptr);
    } else {
        ASSERT_EQ(ScreenSettingHelper::duringCallStateObserver_, nullptr);
    }
}

/**
 * @tc.name: UpdateDuringCallState
 * @tc.desc: UpdateDuringCallState
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, UpdateDuringCallState, Function | SmallTest | Level3)
{
    ASSERT_NE(ssm_, nullptr);
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    ssm_->UpdateDuringCallState();
    if (FoldScreenStateInternel::IsDualDisplayFoldDevice() && ScreenSceneConfig::IsSupportDuringCall()) {
        ASSERT_EQ(ssm_->duringCallState_, 0);
    } else {
        EXPECT_TRUE(g_errLog.find("get setting during call state failed") == std::string::npos);
    }
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: SetDuringCallState
 * @tc.desc: SetDuringCallState
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetDuringCallState, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    bool value = false;
    ssm_->SetDuringCallState(value);
    EXPECT_TRUE(g_errLog.find("set during call state to") != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: DisconnectScreenIfScreenInfoNull
 * @tc.desc: DisconnectScreenIfScreenInfoNull
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, DisconnectScreenIfScreenInfoNull01, TestSize.Level1) {
    ASSERT_NE(ssm_, nullptr);
#define FOLD_ABILITY_ENABLE
    if (FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        sptr<ScreenSession> session = ssm_->GetOrCreateScreenSession(SCREENID);
        EXPECT_NE(session, nullptr);
        ssm_->SetClient(nullptr);
        ASSERT_EQ(ssm_->GetClientProxy(), nullptr);
        ssm_->DisConnectScreenIfScreenInfoNull(session);
    }
#undef FOLD_ABILITY_ENABLE
}

/**
 * @tc.name: SetDefaultScreenModeWhenCreateMirror
 * @tc.desc: SetDefaultScreenModeWhenCreateMirror
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetDefaultScreenModeWhenCreateMirror, TestSize.Level1) {
    ASSERT_NE(ssm_, nullptr);
#define FOLD_ABILITY_ENABLE
    if (FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        sptr<ScreenSession> session = nullptr;
        ssm_->SetDefaultScreenModeWhenCreateMirror(session);
        session = ssm_->GetOrCreateScreenSession(0);
        auto mode = session->GetScreenCombination();
        ssm_->SetDefaultScreenModeWhenCreateMirror(session);
        ASSERT_EQ(session->GetScreenCombination(), mode);
    }
#undef FOLD_ABILITY_ENABLE
}

/**
 * @tc.name: GetOldDisplayModeRotation
 * @tc.desc: GetOldDisplayModeRotation
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetOldDisplayModeRotation, TestSize.Level1)
{
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
    foldPolicy->lastDisplayMode_ = FoldDisplayMode::MAIN;
    foldController->foldScreenPolicy_ = foldPolicy;
    ssm_->foldScreenController_ = foldController;
    std::unordered_map<FoldDisplayMode, int32_t> rotationCorrectionMap;
    rotationCorrectionMap.insert({FoldDisplayMode::GLOBAL_FULL, 3});
    ssm_->rotationCorrectionMap_ = rotationCorrectionMap;
    Rotation rotation = ssm_->GetOldDisplayModeRotation(FoldDisplayMode::MAIN, Rotation::ROTATION_0);
    EXPECT_EQ(rotation, Rotation::ROTATION_0);
    rotation = ssm_->GetOldDisplayModeRotation(FoldDisplayMode::GLOBAL_FULL, Rotation::ROTATION_0);
    if (CORRECTION_ENABLE) {
        EXPECT_EQ(rotation, Rotation::ROTATION_90);
    } else {
        EXPECT_EQ(rotation, Rotation::ROTATION_0);
    }
}
 
/**
 * @tc.name: SwapScreenWeightAndHeight
 * @tc.desc: SwapScreenWeightAndHeight
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SwapScreenWeightAndHeight, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    sptr<ScreenSession> screenSession = nullptr;
    ssm_->SwapScreenWeightAndHeight(screenSession);
    EXPECT_TRUE(g_errLog.find("screenSession is null") != std::string::npos);
    screenSession = sptr<ScreenSession>::MakeSptr();
    ScreenProperty property;
    RRect bounds;
    bounds.rect_.width_ = 100;
    bounds.rect_.height_ = 200;
    property.SetBounds(bounds);
    screenSession->SetScreenProperty(property);
    ssm_->SwapScreenWeightAndHeight(screenSession);
    auto afterBounds = screenSession->GetScreenProperty().GetBounds();
    EXPECT_EQ(afterBounds.rect_.width_, bounds.rect_.height_);
    EXPECT_EQ(afterBounds.rect_.height_, bounds.rect_.width_);
    g_errLog.clear();
}
 
/**
 * @tc.name: HandleScreenRotationAndBoundsWhenSetClient
 * @tc.desc: HandleScreenRotationAndBoundsWhenSetClient
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HandleScreenRotationAndBoundsWhenSetClient, TestSize.Level1)
{
    auto foldController = sptr<FoldScreenController>::MakeSptr(ssm_->displayInfoMutex_,
        ssm_->screenPowerTaskScheduler_);
    ASSERT_NE(foldController, nullptr);
    auto foldPolicy = foldController->GetFoldScreenPolicy(DisplayDeviceType::SINGLE_DISPLAY_DEVICE);
    ASSERT_NE(foldPolicy, nullptr);
    foldPolicy->lastDisplayMode_ = FoldDisplayMode::GLOBAL_FULL;
    foldController->foldScreenPolicy_ = foldPolicy;
    ssm_->foldScreenController_ = foldController;
    std::unordered_map<FoldDisplayMode, int32_t> rotationCorrectionMap;
    rotationCorrectionMap.insert({FoldDisplayMode::GLOBAL_FULL, 3});
    ssm_->rotationCorrectionMap_ = rotationCorrectionMap;
    ScreenId id = 123;
    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr(id, ScreenProperty(), 0);
    if (FoldScreenStateInternel::IsSecondaryDisplayFoldDevice()) {
        screenSession->SetRotation(Rotation::ROTATION_0);
        ssm_->HandleScreenRotationAndBoundsWhenSetClient(screenSession);
        EXPECT_EQ(screenSession->GetRotation(), Rotation::ROTATION_0);
        screenSession->SetRotation(Rotation::ROTATION_180);
        ssm_->HandleScreenRotationAndBoundsWhenSetClient(screenSession);
        EXPECT_EQ(screenSession->GetRotation(), Rotation::ROTATION_0);
        screenSession->SetRotation(Rotation::ROTATION_270);
        ssm_->HandleScreenRotationAndBoundsWhenSetClient(screenSession);
        EXPECT_EQ(screenSession->GetRotation(), Rotation::ROTATION_0);
    } else {
        screenSession->SetRotation(Rotation::ROTATION_270);
        ssm_->HandleScreenRotationAndBoundsWhenSetClient(screenSession);
        EXPECT_EQ(screenSession->GetRotation(), Rotation::ROTATION_270);
        screenSession->SetRotation(Rotation::ROTATION_270);
        foldPolicy->lastDisplayMode_ = FoldDisplayMode::UNKNOWN;
        ssm_->HandleScreenRotationAndBoundsWhenSetClient(screenSession);
        EXPECT_EQ(screenSession->GetRotation(), Rotation::ROTATION_270);
        screenSession->SetRotation(Rotation::ROTATION_0);
        ssm_->HandleScreenRotationAndBoundsWhenSetClient(screenSession);
        EXPECT_EQ(screenSession->GetRotation(), Rotation::ROTATION_0);
    }
}
 
/**
 * @tc.name: RemoveRotationCorrection
 * @tc.desc: RemoveRotationCorrection
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RemoveRotationCorrection, TestSize.Level1)
{
    auto afterRotation = ssm_->RemoveRotationCorrection(Rotation::ROTATION_270);
    EXPECT_EQ(afterRotation, Rotation::ROTATION_270);
}
 
/**
 * @tc.name: GetConfigCorrectionByDisplayMode
 * @tc.desc: GetConfigCorrectionByDisplayMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetConfigCorrectionByDisplayMode, TestSize.Level1)
{
    ssm_->rotationCorrectionMap_.clear();
    bool correctionEnable = system::GetIntParameter<int32_t>("const.system.sensor_correction_enable", 0) == 1;
    if (correctionEnable) {
        auto rotation = ssm_->GetConfigCorrectionByDisplayMode(FoldDisplayMode::MAIN);
        EXPECT_EQ(rotation, Rotation::ROTATION_0);
        std::unordered_map<FoldDisplayMode, int32_t> rotationCorrectionMap;
        rotationCorrectionMap.insert({FoldDisplayMode::MAIN, 3});
        ssm_->rotationCorrectionMap_ = rotationCorrectionMap;
        rotation = ssm_->GetConfigCorrectionByDisplayMode(FoldDisplayMode::MAIN);
        EXPECT_EQ(rotation, Rotation::ROTATION_270);
 
    } else {
        auto rotation = ssm_->GetConfigCorrectionByDisplayMode(FoldDisplayMode::MAIN);
        EXPECT_EQ(rotation, Rotation::ROTATION_0);
    }
}
 
/**
 * @tc.name: InitRotationCorrectionMap
 * @tc.desc: rotationCorrectionMap not null
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitRotationCorrectionMap01, TestSize.Level1)
{
    ssm_->rotationCorrectionMap_.clear();
    std::unordered_map<FoldDisplayMode, int32_t> rotationCorrectionMap;
    rotationCorrectionMap.insert({FoldDisplayMode::MAIN, 3});
    ssm_->rotationCorrectionMap_ = rotationCorrectionMap;
    ssm_->InitRotationCorrectionMap("");
    EXPECT_TRUE(ssm_->rotationCorrectionMap_.find(FoldDisplayMode::MAIN) != ssm_->rotationCorrectionMap_.end());
    ssm_->rotationCorrectionMap_.clear();
    std::string config = "1,0;2,0;5,3;";
    ssm_->InitRotationCorrectionMap(config);
    EXPECT_EQ(ssm_->rotationCorrectionMap_[FoldDisplayMode::GLOBAL_FULL], 3);
}
 
/**
 * @tc.name: InitRotationCorrectionMap
 * @tc.desc: split failed
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitRotationCorrectionMap02, TestSize.Level1)
{
    ssm_->rotationCorrectionMap_.clear();
    ssm_->InitRotationCorrectionMap("");
    EXPECT_TRUE(ssm_->rotationCorrectionMap_.empty());
    ssm_->rotationCorrectionMap_.clear();
    std::string config = ";;";
    ssm_->InitRotationCorrectionMap(config);
    EXPECT_TRUE(ssm_->rotationCorrectionMap_.empty());
}
 
/**
 * @tc.name: InitRotationCorrectionMap
 * @tc.desc: InitRotationCorrectionMap
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, InitRotationCorrectionMap03, TestSize.Level1)
{
    ssm_->rotationCorrectionMap_.clear();
    std::string config = "1,0,1;1,0;2,0;5,3;3,a;a,a";
    ssm_->InitRotationCorrectionMap(config);
    EXPECT_EQ(ssm_->rotationCorrectionMap_[FoldDisplayMode::GLOBAL_FULL], 3);
}

/**
 * @tc.name: RecoverDefaultScreenModeInner
 * @tc.desc: RecoverDefaultScreenModeInner
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RecoverDefaultScreenModeInner, TestSize.Level1) {
    ASSERT_NE(ssm_, nullptr);
#define FOLD_ABILITY_ENABLE
    if (FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {
        ScreenId innerRsId = 0;
        ScreenId externalRsId = 11;
        ssm_->SetIsFoldStatusLocked(true);
        EXPECT_EQ(ssm_->GetIsFoldStatusLocked(), true);
        ssm_->RecoverDefaultScreenModeInner(innerRsId, externalRsId);
        SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::EXPANDED);
        innerRsId = 1;
        externalRsId = 12;
        ssm_->RecoverDefaultScreenModeInner(innerRsId, externalRsId);
    }
#undef FOLD_ABILITY_ENABLE
}

/**
 * @tc.name: AddOrUpdateUserDisplayNode
 * @tc.desc: AddOrUpdateUserDisplayNode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddOrUpdateUserDisplayNode, TestSize.Level1)
{
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayNode>(config);
    ASSERT_NE(ssm_, nullptr);
    ssm_->AddOrUpdateUserDisplayNode(100, 1, node);

    auto userIt = ssm_->userDisplayNodeMap_.find(100);
    ASSERT_NE(userIt, ssm_->userDisplayNodeMap_.end());
    
    auto& screenMap = userIt->second;
    auto screenIt = screenMap.find(1);
    ASSERT_NE(screenIt, screenMap.end());
    EXPECT_EQ(screenIt->second, node);
}

/**
 * @tc.name: RemoveUserDisplayNode
 * @tc.desc: RemoveUserDisplayNode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RemoveUserDisplayNode, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    int32_t user1 = 100;
    int32_t user2 = 200;
    ScreenId screen1 = 1;
    ScreenId screen2 = 2;
    RSDisplayNodeConfig config;
    auto node1 = std::make_shared<RSDisplayNode>(config);
    auto node2 = std::make_shared<RSDisplayNode>(config);
    ssm_->userDisplayNodeMap_[user1].insert_or_assign(screen1, node1);
    ssm_->RemoveUserDisplayNode(user2, screen1);
    EXPECT_EQ(ssm_->userDisplayNodeMap_.size(), 1);
    
    ssm_->userDisplayNodeMap_[user2].insert_or_assign(screen2, node2);
    ssm_->RemoveUserDisplayNode(user2, screen1);
    EXPECT_EQ(ssm_->userDisplayNodeMap_.size(), 2);

    ssm_->userDisplayNodeMap_[user2].insert_or_assign(screen1, node2);
    ssm_->RemoveUserDisplayNode(user2, screen1);
    auto findRes = ssm_->userDisplayNodeMap_.find(user2);
    EXPECT_NE(findRes, ssm_->userDisplayNodeMap_.end());
    EXPECT_EQ(findRes->second.find(screen1), findRes->second.end());

    ssm_->RemoveUserDisplayNode(user2, screen2);
    EXPECT_EQ(ssm_->userDisplayNodeMap_.find(user2), ssm_->userDisplayNodeMap_.end());
}

/**
 * @tc.name: GetUserDisplayNodeMap
 * @tc.desc: GetUserDisplayNodeMap
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetUserDisplayNodeMap, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    int32_t user1 = 100;
    ScreenId screen1 = 1;
    RSDisplayNodeConfig config;
    auto node1 = std::make_shared<RSDisplayNode>(config);
    ssm_->userDisplayNodeMap_[user1].insert_or_assign(screen1, node1);
    auto res = ssm_->GetUserDisplayNodeMap(user1);
    auto it = res.find(screen1);
    ASSERT_NE(it, res.end());
    ASSERT_EQ(it->second, node1);
}

/**
 * @tc.name: SwitchUserDealUserDisplayNode
 * @tc.desc: SwitchUserDealUserDisplayNode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SwitchUserDealUserDisplayNode, TestSize.Level1)
{
    ASSERT_NE(ssm_, nullptr);
    int32_t userId = 100;
    ScreenId sc1 = 1;
    ScreenId sc2 = 2;
    ScreenId sc3 = 3;
    ScreenId sc4 = 4;

    auto realSession1 = sptr<ScreenSession>::MakeSptr();
    realSession1->SetScreenId(sc1);
    realSession1->SetIsRealScreen(true);
    
    auto realSession2 = sptr<ScreenSession>::MakeSptr();
    realSession2->SetScreenId(sc2);
    realSession2->SetIsRealScreen(true);
    
    auto virtualSession = sptr<ScreenSession>::MakeSptr();
    virtualSession->SetScreenId(sc3);
    virtualSession->SetIsRealScreen(false);

    ssm_->screenSessionMap_[sc1] = realSession1;
    ssm_->screenSessionMap_[sc2] = realSession2;
    ssm_->screenSessionMap_[sc3] = virtualSession;

    auto& userNodeMap = ssm_->userDisplayNodeMap_[userId];
    RSDisplayNodeConfig config;
    auto presetNode = std::make_shared<RSDisplayNode>(config);
    userNodeMap[sc1] = presetNode;
    userNodeMap[sc4] = presetNode;
    ssm_->SwitchUserDealUserDisplayNode(userId);

    // 验证结果
    auto& resultMap = ssm_->userDisplayNodeMap_[userId];
    EXPECT_EQ(resultMap[sc1], presetNode);
    EXPECT_NE(resultMap.find(sc2), resultMap.end());
    EXPECT_NE(resultMap[sc2], nullptr);
    EXPECT_NE(resultMap[sc2], presetNode);
    EXPECT_EQ(resultMap.find(sc4), resultMap.end());
    EXPECT_EQ(resultMap.find(sc3), resultMap.end());
}

/**
 * @tc.name: AddUserDisplayNodeOnTree
 * @tc.desc: AddUserDisplayNodeOnTree
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, AddUserDisplayNodeOnTree, TestSize.Level1)
{
    g_errLog.clear();
    int32_t userId = 100;
    ScreenId screenId1 = 1;
    ScreenId screenId2 = 2;
    RSDisplayNodeConfig config;
    auto node1 = std::make_shared<RSDisplayNode>(config);
    std::shared_ptr<RSDisplayNode> node2 = nullptr;
    ssm_->userDisplayNodeMap_[userId].insert_or_assign(screenId1, node1);
    ssm_->userDisplayNodeMap_[userId].insert_or_assign(screenId2, node2);
    ssm_->AddUserDisplayNodeOnTree(userId);
    
    EXPECT_TRUE(g_errLog.find("userId: ") != std::string::npos);
    g_errLog.clear();
}

/**
 * @tc.name: RemoveUserDisplayNodeFromTree
 * @tc.desc: RemoveUserDisplayNodeFromTree
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, RemoveUserDisplayNodeFromTree, TestSize.Level1)
{
    int32_t userId = 100;
    ScreenId screenId1 = 1;
    ScreenId screenId2 = 2;
    RSDisplayNodeConfig config;
    auto node1 = std::make_shared<RSDisplayNode>(config);
    std::shared_ptr<RSDisplayNode> node2 = nullptr;
    ssm_->userDisplayNodeMap_[userId].insert_or_assign(screenId1, node1);
    ssm_->userDisplayNodeMap_[userId].insert_or_assign(screenId2, node2);
    ssm_->AddUserDisplayNodeOnTree(userId);
    g_errLog.clear();
    ssm_->RemoveUserDisplayNodeFromTree(userId);
    EXPECT_TRUE(g_errLog.find("userId: ") != std::string::npos);
    g_errLog.clear();
}

/**
 * @tc.name: SetUserDisplayNodePositionZ
 * @tc.desc: SetUserDisplayNodePositionZ
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, SetUserDisplayNodePositionZ, TestSize.Level1)
{
    g_errLog.clear();
    int32_t userId = 100;
    float positionZ = 2.0f;
    ScreenId screenId1 = 1;
    ScreenId screenId2 = 2;
    RSDisplayNodeConfig config;
    auto node1 = std::make_shared<RSDisplayNode>(config);
    std::shared_ptr<RSDisplayNode> node2 = nullptr;
    ssm_->userDisplayNodeMap_[userId].insert_or_assign(screenId1, node1);
    ssm_->userDisplayNodeMap_[userId].insert_or_assign(screenId2, node2);
    ssm_->SetUserDisplayNodePositionZ(userId, positionZ);
    
    ASSERT_TRUE(g_errLog.find("positionZ:") != std::string::npos);
    g_errLog.clear();
}

/**
 * @tc.name: HandleNewUserDisplayNode
 * @tc.desc: HandleNewUserDisplayNode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, HandleNewUserDisplayNode, TestSize.Level1) {
    ASSERT_NE(ssm_, nullptr);
    int32_t oldUserId = 100;
    int32_t newUserId = 200;
    ssm_->currentUserId_ = oldUserId;
    g_errLog.clear();

    RSDisplayNodeConfig config;
    auto oldUserNode = std::make_shared<Rosen::RSDisplayNode>(config);
    auto newUserNode = std::make_shared<Rosen::RSDisplayNode>(config);

    ssm_->userDisplayNodeMap_[oldUserId][1] = oldUserNode;
    float position = 5.0f;
    oldUserNode->SetPositionZ(position);
    newUserNode->SetPositionZ(position);
    ssm_->HandleNewUserDisplayNode(newUserId, false);
    EXPECT_TRUE(g_errLog.find("deal with userDisplayNode") != std::string::npos);
    g_errLog.clear();
    EXPECT_TRUE(ssm_->userDisplayNodeMap_[newUserId].size() > 0);
}

/**
 * @tc.name: ScreenConnectHandleFoldScreen
 * @tc.desc: ScreenConnectHandleFoldScreen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, ScreenConnectHandleFoldScreen, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    ScreenId screenId = 0;
    ssm_->isCoordinationFlag_ = false;
    ScreenEvent screenEvent = ScreenEvent::CONNECTED;
    sptr<ScreenSession> session = nullptr;
    ssm_->ScreenConnectHandleFoldScreen(screenId, session, false, screenEvent);
    EXPECT_FALSE(g_errLog.find("event: connect") != std::string::npos);
    g_errLog.clear();``````````

    session = sptr<ScreenSession>::MakeSptr();
    ssm_->clientProxy_ = nullptr;
    ssm_->ScreenConnectHandleFoldScreen(screenId, session, false, screenEvent);
    EXPECT_FALSE(g_errLog.find("event: connect") != std::string::npos);
    g_errLog.clear();
 
    ssm_->clientProxy_ = sptr<ScreenSessionManagerClientTest>::MakeSptr();
    ssm_->ScreenConnectHandleFoldScreen(screenId, session, false, screenEvent);
    EXPECT_TRUE(g_errLog.find("event: connect") != std::string::npos);
    g_errLog.clear();
 
    screenId = 1;
    ssm_->ScreenConnectHandleFoldScreen(screenId, session, true, screenEvent);
    EXPECT_FALSE(g_errLog.find("event: connect") != std::string::npos);
    g_errLog.clear();
 
    screenId = SCREEN_ID_MAIN;
    ssm_->ScreenConnectHandleFoldScreen(screenId, session, true, screenEvent);
    EXPECT_FALSE(g_errLog.find("event: connect") != std::string::npos);
    g_errLog.clear();
    
    ssm_->isCoordinationFlag_ = true;
    ssm_->ScreenConnectHandleFoldScreen(screenId, session, true, screenEvent);
    EXPECT_TRUE(g_errLog.find("event: connect") != std::string::npos);
    g_errLog.clear();
}
}
}
}