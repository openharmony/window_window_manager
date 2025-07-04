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
    superFoldStateManager.currentSuperFoldCreaseRegion_ = new FoldCreaseRegion(0, rect);
    result = superFoldStateManager.GetFoldCreaseHeight();
    EXPECT_EQ(result, DEFAULT_FOLD_REGION_HEIGHT);
    rect.push_back({0, 0,
        DEFAULT_FOLD_REGION_HEIGHT * 2, DEFAULT_FOLD_REGION_HEIGHT * 2
    });
    superFoldStateManager.currentSuperFoldCreaseRegion_ = new FoldCreaseRegion(0, rect);
    result = superFoldStateManager.GetFoldCreaseHeight();
    EXPECT_EQ(result, DEFAULT_FOLD_REGION_HEIGHT * 2);
}
 
/**
* @tc.name  : RefreshActiveRegion_HeightZero
* @tc.desc  : Test When currentSuperFoldCreaseRegion_ is nullptr, return defalut
* @tc.type: FUNC
*/
HWTEST_F(SuperFoldStateManagerTest, RefreshActiveRegion_HeightZero, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    DMRect mirrorRegion = {100, 100, 0, 0};
    SuperFoldStateManager superFoldStateManager = SuperFoldStateManager();
    sptr<ScreenSession> innerScreen = new ScreenSession();
    auto result = superFoldStateManager.RefreshActiveRegion(mirrorRegion, innerScreen, 0);
    EXPECT_EQ(result, DMError::DM_ERROR_INVALID_PARAM);
    result = superFoldStateManager.RefreshActiveRegion(mirrorRegion, innerScreen, 100);
    EXPECT_EQ(result, DMError::DM_ERROR_INVALID_PARAM);
}
 
/**
* @tc.name  : RefreshActiveRegion_ExpandedStatus
* @tc.desc  : Test When ExpandedStatus, return ok
* @tc.type: FUNC
*/
HWTEST_F(SuperFoldStateManagerTest, RefreshActiveRegion_ExpandedStatus, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    DMRect mirrorRegion = {100, 100, 100, 100};
    SuperFoldStateManager superFoldStateManager = SuperFoldStateManager();
    superFoldStateManager.SetCurrentStatus(SuperFoldStatus::EXPANDED);
    sptr<ScreenSession> innerScreen = nullptr;
    innerScreen = ssm_.GetOrCreateScreenSession(0);
    auto result = superFoldStateManager.RefreshActiveRegion(mirrorRegion, innerScreen, 100);
    EXPECT_EQ(result, DMError::DM_OK);
}
 
/**
* @tc.name  : RefreshActiveRegion_NonExpandedStatus
* @tc.desc  : Test When NonExpandedStatus, return ok
* @tc.type: FUNC
*/
HWTEST_F(SuperFoldStateManagerTest, RefreshActiveRegion_NonExpandedStatus, TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    DMRect mirrorRegion = {100, 100, 100, 100};
    SuperFoldStateManager superFoldStateManager = SuperFoldStateManager();
    superFoldStateManager.SetCurrentStatus(SuperFoldStatus::UNKNOWN);
    sptr<ScreenSession> innerScreen = nullptr;
    innerScreen = ssm_.GetOrCreateScreenSession(0);
    auto result = superFoldStateManager.RefreshActiveRegion(mirrorRegion, innerScreen, 100);
    EXPECT_EQ(result, DMError::DM_OK);
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
    SuperFoldStateManager superFoldStateManager = SuperFoldStateManager();
    ASSERT_EQ(superFoldStateManager.RefreshMirrorRegionInner(mainScreenSession, secondarySession),
        DMError::DM_ERROR_NULLPTR);
    mainScreenSession = new ScreenSession();
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
    SuperFoldStateManager superFoldStateManager = SuperFoldStateManager();
    superFoldStateManager.SetCurrentStatus(SuperFoldStatus::UNKNOWN);
    sptr<ScreenSession> mainScreenSession = ssm_.GetOrCreateScreenSession(0);
    ScreenProperty mainScreenProperty;
    mainScreenProperty.bounds_.rect_ = { 0, 0, 100, 100 };
    mainScreenSession->SetScreenProperty(mainScreenProperty);
 
    sptr<ScreenSession> secondarySession = ssm_.GetOrCreateScreenSession(1);
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
    ssm_.SetIsExtendScreenConnected(false);
    SuperFoldStateManager superFoldStateManager = SuperFoldStateManager();
    DMError result = superFoldStateManager.RefreshExternalRegion();
    ssm_.SetIsExtendScreenConnected(true);
 
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
 * @tc.name  : RefreshScreenRelativePosition_ShouldUpdatePosition_WhenNull
 * @tc.desc  : Test that the screen position is correctly updated when session is null
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, RefreshScreenRelativePosition_ShouldUpdatePosition_WhenNull,
    TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager manager = SuperFoldStateManager();
    sptr<ScreenSession> mainScreenSession = nullptr;
    sptr<ScreenSession> secondarySession = nullptr;
    DMError result = manager.RefreshScreenRelativePosition(mainScreenSession, secondarySession);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, result);
 
    mainScreenSession = new ScreenSession();
    result = manager.RefreshScreenRelativePosition(mainScreenSession, secondarySession);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, result);
}
 
/**
 * @tc.name  : RefreshScreenRelativePosition_ShouldUpdatePosition_Normal
 * @tc.desc  : Test that the screen position is correctly updated when session is Normal
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, RefreshScreenRelativePosition_ShouldUpdatePosition_Normal,
    TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager manager = SuperFoldStateManager();
    sptr<ScreenSession> mainScreenSession = new ScreenSession();
    sptr<ScreenSession> secondarySession = new ScreenSession();
    DMError result = manager.RefreshScreenRelativePosition(mainScreenSession, secondarySession);
    EXPECT_EQ(DMError::DM_OK, result);
}
 
/**
 * @tc.name: RefreshScreenRelativePositionInner
 * @tc.desc: RefreshScreenRelativePositionInner
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, RefreshScreenRelativePositionInner, TestSize.Level0)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager manager = SuperFoldStateManager();
    MultiScreenPositionOptions mainScreenOptions = {0, 0, 0};
    MultiScreenPositionOptions secondScreenOption = {1, 100, 50};
    Drawing::Rect p1 = {0, 0, 100, 200};
    Drawing::Rect p2 = {100, 50, 200, 250};
    manager.SetCurrentStatus(SuperFoldStatus::EXPANDED);
    auto result = manager.RefreshScreenRelativePositionInner(mainScreenOptions, secondScreenOption, p1, p2);
    EXPECT_EQ(result, DMError::DM_OK);
    EXPECT_EQ(mainScreenOptions.startX_, 0);
    EXPECT_EQ(secondScreenOption.startX_, 200);
}
/**
 * @tc.name  : CalculateScreenRelativePosition_ShouldSetRightDirection_WhenSecondRectIsRightOfMain
 * @tc.desc  : Test when the second screen is on the right side of the main screen, the direction set to RIGHT
* @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, CalculateScreenRelativePosition_ShouldSetRightDirection_WhenSecondRectIsRightOfMain,
    TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager manager = SuperFoldStateManager();
    int32_t mainStartX = 0, mainStartY = 0;
    int32_t p1Width = 100, p1Height = 100;
    int32_t p2Width = 50, p2Height = 100;
    int32_t secondStartX = mainStartX + p1Width, secondStartY = 0;
    ScreenDirectionType p2Direction;
    Drawing::Rect p1 = {mainStartX, mainStartY, mainStartX + p1Width, mainStartY + p1Height};
    Drawing::Rect p2 = {secondStartX, secondStartY, secondStartX + p2Width, secondStartX + p2Height};
    auto result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
    mainStartX = 0;
    mainStartY = p2Height;
    secondStartX = mainStartX + p1Width;
    secondStartY = 0;
    p1 = {mainStartX, mainStartY, mainStartX + p1Width, mainStartY + p1Height};
    p2 = {secondStartX, secondStartY, secondStartX + p2Width, secondStartX + p2Height};
    result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
 
    mainStartX = 0;
    mainStartY = 0;
    secondStartX = mainStartX + p1Width;
    secondStartY = p1Height;
    p1 = {mainStartX, mainStartY, mainStartX + p1Width, mainStartY + p1Height};
    p2 = {secondStartX, secondStartY, secondStartX + p2Width, secondStartX + p2Height};
    result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
 
    EXPECT_EQ(result, DMError::DM_OK);
}
 
/**
 * @tc.name  : CalculateScreenRelativePosition_ShouldSetLeftDirection_WhenSecondRectIsLeftOfMain
 * @tc.desc  : Test the orientation set to LEFT when the second screen is on the left side of the main screen
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, CalculateScreenRelativePosition_ShouldSetLeftDirection_WhenSecondRectIsLeftOfMain,
    TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager manager = SuperFoldStateManager();
    int32_t p1Width = 100, p1Height = 100;
    int32_t p2Width = 50, p2Height = 100;
 
    int32_t secondStartX = 0, secondStartY = 0;
    int32_t mainStartX = secondStartX + p2Width, mainStartY = secondStartY;
    ScreenDirectionType p2Direction;
    Drawing::Rect p1 = {mainStartX, mainStartY, mainStartX + p1Width, mainStartY + p1Height};
    Drawing::Rect p2 = {secondStartX, secondStartY, secondStartX + p2Width, secondStartX + p2Height};
    auto result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
 
    mainStartX = 0;
    mainStartY = p1Height;
    secondStartX = mainStartX + p1Width;
    secondStartY = 0;
    p1 = {mainStartX, mainStartY, mainStartX + p1Width, mainStartY + p1Height};
    p2 = {secondStartX, secondStartY, secondStartX + p2Width, secondStartX + p2Height};
    result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
    
    mainStartX = 0;
    mainStartY = 0;
    secondStartX = mainStartX + p1Width;
    secondStartY = p1Height;
    p1 = {mainStartX, mainStartY, mainStartX + p1Width, mainStartY + p1Height};
    p2 = {secondStartX, secondStartY, secondStartX + p2Width, secondStartX + p2Height};
    result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
 
    EXPECT_EQ(result, DMError::DM_OK);
}
 
/**
 * @tc.name  : CalculateScreenRelativePosition_ShouldSetTopDirection_WhenSecondRectIsAboveMain
 * @tc.desc  : When the second screen area is above the main screen, the orientation should be set to TOP
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, CalculateScreenRelativePosition_ShouldSetTopDirection_WhenSecondRectIsAboveMain,
    TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager manager = SuperFoldStateManager();
    int32_t p1Width = 100, p1Height = 100;
    int32_t p2Width = 50, p2Height = 100;
    int32_t secondStartX = p1Width / 2, secondStartY = 0;
    int32_t mainStartX = secondStartX, mainStartY = secondStartY + p2Height;
    ScreenDirectionType p2Direction;
    Drawing::Rect p1 = {mainStartX, mainStartY, mainStartX + p1Width, mainStartY + p1Height};
    Drawing::Rect p2 = {secondStartX, secondStartY, secondStartX + p2Width, secondStartX + p2Height};
 
    auto result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
    
    mainStartX = p2Width - 1;
    mainStartY = p2Height;
    secondStartX = 0;
    secondStartY = 0;
    p1 = {mainStartX, mainStartY, mainStartX + p1Width, mainStartY + p1Height};
    p2 = {secondStartX, secondStartY, secondStartX + p2Width, secondStartX + p2Height};
    result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
    
    mainStartX = 0;
    mainStartY = p2Height;
    secondStartX = p1Width - 1;
    secondStartY = 0;
    p1 = {mainStartX, mainStartY, mainStartX + p1Width, mainStartY + p1Height};
    p2 = {secondStartX, secondStartY, secondStartX + p2Width, secondStartX + p2Height};
    result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
 
    EXPECT_EQ(result, DMError::DM_OK);
}
 
/**
 * @tc.name  : CalculateScreenRelativePosition_ShouldSetBottomDirection_WhenSecondRectIsBelowMain
 * @tc.desc  : When the second screen area is below the main screen, the orientation should be set to BOTTOM
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, CalculateScreenRelativePosition_ShouldSetBottomDirection_WhenSecondRectIsBelowMain,
    TestSize.Level1)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager manager = SuperFoldStateManager();
    int32_t p1Width = 100, p1Height = 100;
    int32_t p2Width = 50, p2Height = 100;
    int32_t mainStartX = 0, mainStartY = 0;
    int32_t secondStartX = p1Width / 2, secondStartY = mainStartY + p1Height;
    ScreenDirectionType p2Direction;
    Drawing::Rect p1 = {mainStartX, mainStartY, mainStartX + p1Width, mainStartY + p1Height};
    Drawing::Rect p2 = {secondStartX, secondStartY, secondStartX + p2Width, secondStartX + p2Height};
 
    auto result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
    
    mainStartX = 0;
    mainStartY = 0;
    secondStartX = p1Width - 1;
    secondStartY = p1Height;
    p1 = {mainStartX, mainStartY, mainStartX + p1Width, mainStartY + p1Height};
    p2 = {secondStartX, secondStartY, secondStartX + p2Width, secondStartX + p2Height};
    result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
    
    mainStartX = p2Width -1;
    mainStartY = 0;
    secondStartX = 0;
    secondStartY = p1Height;
    p1 = {mainStartX, mainStartY, mainStartX + p1Width, mainStartY + p1Height};
    p2 = {secondStartX, secondStartY, secondStartX + p2Width, secondStartX + p2Height};
    result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
 
    EXPECT_EQ(result, DMError::DM_OK);
}
 
/**
 * @tc.name  : CalculateScreenRelativePosition_ShouldSetDirectionLeft_WhenP2LeftOfP1
 * @tc.number: CalculateScreenRelativePositionTest_001
 * @tc.desc  : When p2 is to the left of p1, set p2Direction to LEFT
 */
HWTEST_F(SuperFoldStateManagerTest, CalculateScreenRelativePosition_ShouldSetDirectionLeft_WhenP2LeftOfP1,
    TestSize.Level0)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager manager = SuperFoldStateManager();
    int32_t mainStartX = 0;
    int32_t mainStartY = 0;
    int32_t secondStartX = 0;
    int32_t secondStartY = 0;
    Drawing::Rect p1 = {0, 0, 100, 100};
    Drawing::Rect p2 = {-50, 0, 50, 100};
    ScreenDirectionType p2Direction = ScreenDirectionType::LEFT;
    int32_t p1Width = 100;
    int32_t p1Height = 100;
    int32_t p2Width = 100;
    int32_t p2Height = 100;
 
    DMError result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
 
    EXPECT_EQ(result, DMError::DM_OK);
    EXPECT_EQ(p2Direction, ScreenDirectionType::LEFT);
    EXPECT_EQ(secondStartX, mainStartX - p2Width);
    EXPECT_EQ(secondStartY, mainStartY);
}
 
/**
 * @tc.name  : CalculateScreenRelativePosition_ShouldSetDirectionRight_WhenP2RightOfP1
 * @tc.number: CalculateScreenRelativePositionTest_002
 * @tc.desc  : When p2 is to the right of p1, set p2Direction to RIGHT
 */
HWTEST_F(SuperFoldStateManagerTest, CalculateScreenRelativePosition_ShouldSetDirectionRight_WhenP2RightOfP1,
    TestSize.Level0)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager manager = SuperFoldStateManager();
    int32_t mainStartX = 0;
    int32_t mainStartY = 0;
    int32_t secondStartX = 0;
    int32_t secondStartY = 0;
    Drawing::Rect p1 = {0, 0, 100, 100};
    Drawing::Rect p2 = {150, 0, 250, 100};
    ScreenDirectionType p2Direction = ScreenDirectionType::RIGHT;
    int32_t p1Width = 100;
    int32_t p1Height = 100;
    int32_t p2Width = 100;
    int32_t p2Height = 100;
 
    DMError result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
 
    EXPECT_EQ(result, DMError::DM_OK);
    EXPECT_EQ(p2Direction, ScreenDirectionType::RIGHT);
    EXPECT_EQ(secondStartX, mainStartX + p1Width);
    EXPECT_EQ(secondStartY, mainStartY);
}
 
/**
 * @tc.name  : CalculateScreenRelativePosition_ShouldSetDirectionTop_WhenP2AboveP1
 * @tc.number: CalculateScreenRelativePositionTest_003
 * @tc.desc  : When p2 is above p1, p2Direction should be set to TOP
 */
HWTEST_F(SuperFoldStateManagerTest, CalculateScreenRelativePosition_ShouldSetDirectionTop_WhenP2AboveP1,
    TestSize.Level0)
{
    ONLY_FOR_SUPERFOLD_DISPLAY_DEVICE
    SuperFoldStateManager manager = SuperFoldStateManager();
    int32_t mainStartX = 0;
    int32_t mainStartY = 0;
    int32_t secondStartX = 0;
    int32_t secondStartY = 0;
    Drawing::Rect p1 = {0, 0, 100, 100};
    Drawing::Rect p2 = {0, -50, 100, 50};
    ScreenDirectionType p2Direction = ScreenDirectionType::TOP;
    int32_t p1Width = 100;
    int32_t p1Height = 100;
    int32_t p2Width = 100;
    int32_t p2Height = 100;
 
    DMError result = manager.CalculateScreenRelativePosition(p1, p2, secondStartX, secondStartY, mainStartX, mainStartY);
 
    EXPECT_EQ(result, DMError::DM_OK);
    EXPECT_EQ(p2Direction, ScreenDirectionType::TOP);
    EXPECT_EQ(secondStartX, mainStartX);
    EXPECT_EQ(secondStartY, mainStartY - p2Height);
}
}
}
}