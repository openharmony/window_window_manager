/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <system_ability_definition.h>

#include "fold_screen_controller/super_fold_state_manager.h"
#include "fold_screen_state_internel.h"
#include "screen_session_manager/include/screen_session_manager.h"
#include "display_manager_agent_default.h"
#include "screen_scene_config.h"
#define private public

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

namespace {
#define ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE if (!FoldScreenStateInternel::IsSuperFoldDisplayDevice()) {return;}
}

namespace {
    std::string g_errLog;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char* tag,
        const char* msg)
    {
        g_errLog += msg;
    }
}
    
class SuperFoldStateManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static ScreenSessionManager& ssm_;
};

ScreenSessionManager& SuperFoldStateManagerTest::ssm_ = ScreenSessionManager::GetInstance();

void SuperFoldStateManagerTest::SetUpTestCase()
{
}

void SuperFoldStateManagerTest::TearDownTestCase()
{
}

void SuperFoldStateManagerTest::SetUp()
{
}

void SuperFoldStateManagerTest::TearDown()
{
}

namespace {

/**
 * @tc.name: HandleSuperFoldStatusChange
 * @tc.desc: test function : ANGLE_CHANGE_EXPANDED
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange01, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatusChangeEvents events = SuperFoldStatusChangeEvents::ANGLE_CHANGE_EXPANDED;
    SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::HALF_FOLDED);
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().HandleSuperFoldStatusChange(events);
    };

    func();
    SuperFoldStatus curState = SuperFoldStateManager::GetInstance().GetCurrentStatus();
    SuperFoldStatus expState = SuperFoldStatus::EXPANDED;

    EXPECT_EQ(curState, expState);
}

/**
 * @tc.name: HandleSuperFoldStatusChange
 * @tc.desc: test function : ANGLE_CHANGE_HALF_FOLDED
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange02, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatusChangeEvents events = SuperFoldStatusChangeEvents::ANGLE_CHANGE_HALF_FOLDED;
    SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::FOLDED);
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().HandleSuperFoldStatusChange(events);
    };

    func();
    SuperFoldStatus curState = SuperFoldStateManager::GetInstance().GetCurrentStatus();
    SuperFoldStatus expState = SuperFoldStatus::HALF_FOLDED;

    EXPECT_EQ(curState, expState);
}

/**
 * @tc.name: HandleSuperFoldStatusChange
 * @tc.desc: test function : ANGLE_CHANGE_HALF_FOLDED
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange03, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatusChangeEvents events = SuperFoldStatusChangeEvents::ANGLE_CHANGE_HALF_FOLDED;
    SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::EXPANDED);
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().HandleSuperFoldStatusChange(events);
    };

    func();
    SuperFoldStatus curState = SuperFoldStateManager::GetInstance().GetCurrentStatus();
    SuperFoldStatus expState = SuperFoldStatus::HALF_FOLDED;

    EXPECT_EQ(curState, expState);
}

/**
 * @tc.name: HandleSuperFoldStatusChange
 * @tc.desc: test function : ANGLE_CHANGE_FOLDED
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange04, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatusChangeEvents events = SuperFoldStatusChangeEvents::ANGLE_CHANGE_FOLDED;
    SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::HALF_FOLDED);
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().HandleSuperFoldStatusChange(events);
    };

    func();
    SuperFoldStatus curState = SuperFoldStateManager::GetInstance().GetCurrentStatus();
    SuperFoldStatus expState = SuperFoldStatus::FOLDED;

    EXPECT_EQ(curState, expState);
}

/**
 * @tc.name: HandleSuperFoldStatusChange
 * @tc.desc: test function : KEYBOARD_ON
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange05, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatusChangeEvents events = SuperFoldStatusChangeEvents::KEYBOARD_ON;
    SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::HALF_FOLDED);
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().HandleSuperFoldStatusChange(events);
    };

    func();
    SuperFoldStatus curState = SuperFoldStateManager::GetInstance().GetCurrentStatus();
    SuperFoldStatus expState = SuperFoldStatus::KEYBOARD;

    EXPECT_EQ(curState, expState);
}

/**
 * @tc.name: HandleSuperFoldStatusChange
 * @tc.desc: test function : KEYBOARD_ON
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange06, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatusChangeEvents events = SuperFoldStatusChangeEvents::KEYBOARD_ON;
    SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::EXPANDED);
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().
            HandleSuperFoldStatusChange(SuperFoldStatusChangeEvents::ANGLE_CHANGE_HALF_FOLDED);
        SuperFoldStateManager::GetInstance().HandleSuperFoldStatusChange(events);
    };

    func();
    SuperFoldStatus curState = SuperFoldStateManager::GetInstance().GetCurrentStatus();
    SuperFoldStatus expState = SuperFoldStatus::KEYBOARD;

    EXPECT_EQ(curState, expState);
}

/**
 * @tc.name: HandleSuperFoldStatusChange
 * @tc.desc: test function : KEYBOARD_OFF
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange07, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatusChangeEvents events = SuperFoldStatusChangeEvents::KEYBOARD_OFF;
    SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::KEYBOARD);
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().HandleSuperFoldStatusChange(events);
    };

    func();
    SuperFoldStatus curState = SuperFoldStateManager::GetInstance().GetCurrentStatus();
    SuperFoldStatus expState = SuperFoldStatus::HALF_FOLDED;

    EXPECT_EQ(curState, expState);
}

/**
 * @tc.name: HandleSuperFoldStatusChange
 * @tc.desc: test function : SYSTEM_KEYBOARD_ON
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange08, Function | SmallTest | Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatusChangeEvents events = SuperFoldStatusChangeEvents::SYSTEM_KEYBOARD_ON;
    SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::HALF_FOLDED);
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().HandleSuperFoldStatusChange(events);
    };
 
    func();
    SuperFoldStatus curState = SuperFoldStateManager::GetInstance().GetCurrentStatus();
    SuperFoldStatus expState = SuperFoldStatus::HALF_FOLDED;
 
    EXPECT_EQ(curState, expState);
}
 
/**
 * @tc.name: HandleSuperFoldStatusChange
 * @tc.desc: test function : SYSTEM_KEYBOARD_OFF
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange09, Function | SmallTest | Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatusChangeEvents events = SuperFoldStatusChangeEvents::SYSTEM_KEYBOARD_OFF;
    SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::HALF_FOLDED);
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().HandleSuperFoldStatusChange(events);
    };
 
    func();
    SuperFoldStatus curState = SuperFoldStateManager::GetInstance().GetCurrentStatus();
    SuperFoldStatus expState = SuperFoldStatus::HALF_FOLDED;
 
    EXPECT_EQ(curState, expState);
}

/**
 * @tc.name: MatchSuperFoldStatusToFoldStatus
 * @tc.desc: test function : EXPANDED
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, MatchSuperFoldStatusToFoldStatus01, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatus superFoldStatus = SuperFoldStatus::EXPANDED;
    FoldStatus foldStatus;
    std::function<void()> func = [&]()
    {
        foldStatus = SuperFoldStateManager::GetInstance().MatchSuperFoldStatusToFoldStatus(superFoldStatus);
    };

    func();
    FoldStatus foldStatusExp = FoldStatus::EXPAND;
    EXPECT_EQ(foldStatus, foldStatusExp);
}

/**
 * @tc.name: MatchSuperFoldStatusToFoldStatus
 * @tc.desc: test function : HALF_FOLDED
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, MatchSuperFoldStatusToFoldStatus02, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatus superFoldStatus = SuperFoldStatus::HALF_FOLDED;
    FoldStatus foldStatus;
    std::function<void()> func = [&]()
    {
        foldStatus = SuperFoldStateManager::GetInstance().MatchSuperFoldStatusToFoldStatus(superFoldStatus);
    };

    func();
    FoldStatus foldStatusExp = FoldStatus::HALF_FOLD;
    EXPECT_EQ(foldStatus, foldStatusExp);
}

/**
 * @tc.name: MatchSuperFoldStatusToFoldStatus
 * @tc.desc: test function : FOLDED
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, MatchSuperFoldStatusToFoldStatus03, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatus superFoldStatus = SuperFoldStatus::FOLDED;
    FoldStatus foldStatus;
    std::function<void()> func = [&]()
    {
        foldStatus = SuperFoldStateManager::GetInstance().MatchSuperFoldStatusToFoldStatus(superFoldStatus);
    };

    func();
    FoldStatus foldStatusExp = FoldStatus::FOLDED;
    EXPECT_EQ(foldStatus, foldStatusExp);
}

/**
 * @tc.name: MatchSuperFoldStatusToFoldStatus
 * @tc.desc: test function : KEYBOARD
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, MatchSuperFoldStatusToFoldStatus04, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatus superFoldStatus = SuperFoldStatus::KEYBOARD;
    FoldStatus foldStatus;
    std::function<void()> func = [&]()
    {
        foldStatus = SuperFoldStateManager::GetInstance().MatchSuperFoldStatusToFoldStatus(superFoldStatus);
    };

    func();
    FoldStatus foldStatusExp = FoldStatus::HALF_FOLD;
    EXPECT_EQ(foldStatus, foldStatusExp);
}

/**
 * @tc.name: MatchSuperFoldStatusToFoldStatus
 * @tc.desc: test function : UNKNOWN
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, MatchSuperFoldStatusToFoldStatus05, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStatus superFoldStatus = SuperFoldStatus::UNKNOWN;
    FoldStatus foldStatus;
    std::function<void()> func = [&]()
    {
        foldStatus = SuperFoldStateManager::GetInstance().MatchSuperFoldStatusToFoldStatus(superFoldStatus);
    };

    func();
    FoldStatus foldStatusExp = FoldStatus::UNKNOWN;
    EXPECT_EQ(foldStatus, foldStatusExp);
}

/**
 * @tc.name: SetSystemKeyboardStatus
 * @tc.desc: SetSystemKeyboardStatus with true as parameter
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, SetSystemKeyboardStatus01, Function | SmallTest | Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::HALF_FOLDED);
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().SetSystemKeyboardStatus(true);
    };
 
    func();
    SuperFoldStatus curState = SuperFoldStateManager::GetInstance().GetCurrentStatus();
    SuperFoldStatus expState = SuperFoldStatus::HALF_FOLDED;
 
    EXPECT_EQ(curState, expState);
}
 
/**
 * @tc.name: SetSystemKeyboardStatus
 * @tc.desc: SetSystemKeyboardStatus with false as parameter
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, SetSystemKeyboardStatus02, Function | SmallTest | Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::HALF_FOLDED);
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().SetSystemKeyboardStatus(false);
    };
 
    func();
    SuperFoldStatus curState = SuperFoldStateManager::GetInstance().GetCurrentStatus();
    SuperFoldStatus expState = SuperFoldStatus::HALF_FOLDED;
 
    EXPECT_EQ(curState, expState);
}

/**
 * @tc.name: GetCurrentStatus_ShouldReturnKeyboard_WhenHalfScreenIsFalse
 * @tc.desc: Test GetCurrentStatus function when isHalfScreen_ is false.
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, GetCurrentStatus_ShouldReturnKeyboard_WhenHalfScreenIsFalse, TestSize.Level1)
{
    SuperFoldStateManager superFoldStateManager = SuperFoldStateManager();
    superFoldStateManager.SetCurrentStatus(SuperFoldStatus::UNKNOWN);
    superFoldStateManager.ChangeScreenState(false);

    SuperFoldStatus curState = superFoldStateManager.GetCurrentStatus();

    EXPECT_EQ(curState, SuperFoldStatus::UNKNOWN);
}

/**
* @tc.name  : GetFoldCreaseHeight
* @tc.desc  : GetFoldCreaseHeight
* @tc.type: FUNC
*/
HWTEST_F(SuperFoldStateManagerTest, GetFoldCreaseHeight, TestSize.Level1)
{
    SuperFoldStateManager superFoldStateManager = SuperFoldStateManager();
    const uint32_t DEFAULT_FOLD_REGION_HEIGHT = 82;
    superFoldStateManager.currentSuperFoldCreaseRegion_ = nullptr;
    uint32_t result = superFoldStateManager.GetFoldCreaseHeight();
    EXPECT_EQ(result, DEFAULT_FOLD_REGION_HEIGHT);
    std::vector<DMRect> rect = {};
    superFoldStateManager.currentSuperFoldCreaseRegion_ = sptr<FoldCreaseRegion>::MakeSptr(0, rect);
    result = superFoldStateManager.GetFoldCreaseHeight();
    EXPECT_EQ(result, DEFAULT_FOLD_REGION_HEIGHT);
    rect.push_back({0, 0,
        DEFAULT_FOLD_REGION_HEIGHT * 2, DEFAULT_FOLD_REGION_HEIGHT * 2
    });
    superFoldStateManager.currentSuperFoldCreaseRegion_ = sptr<FoldCreaseRegion>::MakeSptr(0, rect);
    result = superFoldStateManager.GetFoldCreaseHeight();
    EXPECT_EQ(result, DEFAULT_FOLD_REGION_HEIGHT * 2);
}
 
/**
* @tc.name  : RefreshMirrorRegionInner_NullptrScreenSession
* @tc.desc  : Test When null, return expect
* @tc.type: FUNC
*/
HWTEST_F(SuperFoldStateManagerTest, RefreshMirrorRegionInner_NullptrScreenSession, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    sptr<ScreenSession> mainScreenSession = nullptr;
    sptr<ScreenSession> secondarySession = nullptr;
    SuperFoldStateManager superFoldStateManager;
    ASSERT_EQ(superFoldStateManager.RefreshMirrorRegionInner(mainScreenSession, secondarySession),
        DMError::DM_ERROR_NULLPTR);
    mainScreenSession = sptr<ScreenSession>::MakeSptr();
    ASSERT_EQ(superFoldStateManager.RefreshMirrorRegionInner(mainScreenSession, secondarySession),
        DMError::DM_ERROR_NULLPTR);
}
 
/**
* @tc.name  : RefreshMirrorRegionInner_NormalCase
* @tc.desc  : Test When NormalCase, return DMError::OK
* @tc.type: FUNC
*/
HWTEST_F(SuperFoldStateManagerTest, RefreshMirrorRegionInner_NormalCase, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager superFoldStateManager;
    superFoldStateManager.SetCurrentStatus(SuperFoldStatus::UNKNOWN);
    sptr<ScreenSession> mainScreenSession = ssm_.GetOrCreateScreenSession(0);
    ScreenProperty mainScreenProperty;
    mainScreenProperty.bounds_.rect_ = { 0, 0, 100, 100 };
    mainScreenSession->SetScreenProperty(mainScreenProperty);
 
    sptr<ScreenSession> secondarySession = ssm_.GetOrCreateScreenSession(1);
    superFoldStateManager.SetCurrentStatus(SuperFoldStatus::EXPANDED);
    ScreenProperty secondaryScreenProperty;
    secondaryScreenProperty.bounds_.rect_ = { 0, 0, 100, 100 };
    secondarySession->SetScreenProperty(secondaryScreenProperty);
    EXPECT_EQ(superFoldStateManager.RefreshMirrorRegionInner(mainScreenSession, secondarySession), DMError::DM_OK);
}
 
/**
 * @tc.name  : RefreshExternalRegion_ShouldReturnOk_WhenNoExtendScreenConnected
 * @tc.desc  : When the extended screen is not connected, the function should return DM_OK and ignore the refresh.
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, RefreshExternalRegion_ShouldReturnOk_WhenNoExtendScreenConnected, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    ssm_.SetIsPhysicalExtendScreenConnected(false);
    SuperFoldStateManager superFoldStateManager;
    DMError result = superFoldStateManager.RefreshExternalRegion();
    ssm_.SetIsPhysicalExtendScreenConnected(true);
 
    sptr<ScreenSession> mainScreenSession = ssm_.GetOrCreateScreenSession(0);
    mainScreenSession->SetScreenCombination(ScreenCombination::SCREEN_MAIN);
    ScreenProperty mainScreenProperty;
    mainScreenProperty.bounds_.rect_ = { 0, 0, 100, 100 };
    mainScreenSession->SetScreenProperty(mainScreenProperty);
    result = superFoldStateManager.RefreshExternalRegion();
 
    sptr<IDisplayManagerAgent> displayManagerAgent = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption;
    virtualOption.name_ = "createVirtualOption";
    auto screenId = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption, displayManagerAgent->AsObject());
    auto secondarySession = ScreenSessionManager::GetInstance().GetScreenSession(screenId);
    
    secondarySession->SetScreenCombination(ScreenCombination::SCREEN_MIRROR);
    ScreenProperty secondaryScreenProperty;
    secondaryScreenProperty.bounds_.rect_ = { 0, 0, 100, 100 };
    secondarySession->SetScreenProperty(secondaryScreenProperty);
    result = superFoldStateManager.RefreshExternalRegion();
    EXPECT_EQ(result, DMError::DM_OK);
 
    sptr<IDisplayManagerAgent> displayManagerAgent1 = new(std::nothrow) DisplayManagerAgentDefault();
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "createVirtualOption";
    auto screenId1 = ScreenSessionManager::GetInstance().CreateVirtualScreen(
        virtualOption1, displayManagerAgent1->AsObject());
    auto thirdSession = ScreenSessionManager::GetInstance().GetScreenSession(screenId1);
    thirdSession->SetScreenCombination(ScreenCombination::SCREEN_EXTEND);
    ScreenProperty thirdScreenProperty;
    thirdScreenProperty.bounds_.rect_ = { 0, 0, 100, 100 };
    thirdSession->SetScreenProperty(thirdScreenProperty);
    result = superFoldStateManager.RefreshExternalRegion();
    EXPECT_EQ(result, DMError::DM_OK);
 
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId);
    ScreenSessionManager::GetInstance().DestroyVirtualScreen(screenId1);
}

/**
 * @tc.name: GetFoldCreaseRect01
 * @tc.desc: test function : GetFoldCreaseRect01
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, GetFoldCreaseRect01, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    std::vector<DMRect> foldCreaseRect;
    std::vector<int32_t> foldRect = {
        100, // posX
        200, // posY
        300, // width
        400  // height
    };
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().GetFoldCreaseRegion(true, false);
        SuperFoldStateManager::GetInstance().GetFoldCreaseRect(true, false, foldRect, foldCreaseRect);
    };
 
    func();
    EXPECT_TRUE(g_errLog.find("the current FoldCreaseRect is vertical") != std::string::npos);
    LOG_SetCallback(nullptr);
}
 
/**
 * @tc.name: GetFoldCreaseRect02
 * @tc.desc: test function : GetFoldCreaseRect02
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, GetFoldCreaseRect02, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    std::vector<DMRect> foldCreaseRect;
    std::vector<int32_t> foldRect = {
        100, // posX
        200, // posY
        300, // width
        400  // height
    };
    std::function<void()> func = [&]()
    {
        SuperFoldStateManager::GetInstance().GetFoldCreaseRegion(false, false);
        SuperFoldStateManager::GetInstance().GetFoldCreaseRect(false, false, foldRect, foldCreaseRect);
    };
 
    func();
    EXPECT_TRUE(g_errLog.find("the current FoldCreaseRect is horizontal") != std::string::npos);
    LOG_SetCallback(nullptr);
}
}
}
}