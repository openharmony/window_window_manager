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
#include "fold_screen_state_internel.h"
#include "../mock/mock_accesstoken_kit.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
constexpr uint32_t M_STATUS_WIDTH = 1008;
constexpr uint32_t F_STATUS_WIDTH = 2048;
constexpr uint32_t G_STATUS_WIDTH = 3184;
}
namespace {
    std::string g_errLog;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char* tag,
        const char* msg)
    {
        g_errLog = msg;
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
 * @tc.name: GetCutoutInfoWithRotation01
 * @tc.desc: GetCutoutInfoWithRotation test with controller nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCutoutInfoWithRotation01, Function | SmallTest | Level3)
{
    DisplayId id = 0;
    int32_t rotation = 0;
    ScreenSessionManager::GetInstance().screenCutoutController_ = nullptr;
    auto cutoutInfo = ScreenSessionManager::GetInstance().GetCutoutInfoWithRotation(id, rotation);
    ASSERT_EQ(cutoutInfo, nullptr);
}

/**
 * @tc.name: GetCutoutInfoWithRotation02
 * @tc.desc: GetCutoutInfoWithRotation test with controller not nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCutoutInfoWithRotation02, Function | SmallTest | Level3)
{
    DisplayId id = 0;
    int32_t rotation = 0;
    auto cutoutInfo = ssm_->GetCutoutInfoWithRotation(id, rotation);
    ASSERT_NE(cutoutInfo, nullptr);
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
 * @tc.name: GetCutoutInfoWithRotation03
 * @tc.desc: GetCutoutInfoWithRotation test with controller is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCutoutInfoWithRotatio03, Function | SmallTest | Level3)
{
    DisplayId id = 0;
    int32_t rotation = 0;
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(false);
    auto cutoutInfo = ssm_->GetCutoutInfoWithRotation(id, rotation);
    EXPECT_EQ(cutoutInfo, nullptr);
}
 
/**
 * @tc.name: GetCutoutInfoWithRotation04
 * @tc.desc: GetCutoutInfoWithRotation test with controller is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCutoutInfoWithRotatio04, Function | SmallTest | Level3)
{
    DisplayId id = 0;
    int32_t rotation = 0;
    MockAccesstokenKit::MockIsSACalling(false);
    MockAccesstokenKit::MockIsSystemApp(true);
    auto cutoutInfo = ssm_->GetCutoutInfoWithRotation(id, rotation);
    EXPECT_NE(cutoutInfo, nullptr);
}
 
/**
 * @tc.name: GetCutoutInfoWithRotation05
 * @tc.desc: GetCutoutInfoWithRotation test with controller is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerTest, GetCutoutInfoWithRotatio05, Function | SmallTest | Level3)
{
    DisplayId id = 0;
    int32_t rotation = 0;
    MockAccesstokenKit::MockIsSACalling(true);
    MockAccesstokenKit::MockIsSystemApp(true);
    auto cutoutInfo = ssm_->GetCutoutInfoWithRotation(id, rotation);
    EXPECT_NE(cutoutInfo, nullptr);
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
}
}
}