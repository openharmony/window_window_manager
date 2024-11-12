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

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SuperFoldStateManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

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
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange01, Function | SmallTest | Level1)
{
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
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange02, Function | SmallTest | Level1)
{
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
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange03, Function | SmallTest | Level1)
{
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
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange04, Function | SmallTest | Level1)
{
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
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange05, Function | SmallTest | Level1)
{
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
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange06, Function | SmallTest | Level1)
{
    SuperFoldStatusChangeEvents events = SuperFoldStatusChangeEvents::KEYBOARD_ON;
    SuperFoldStateManager::GetInstance().SetCurrentStatus(SuperFoldStatus::EXPANDED);
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
 * @tc.desc: test function : KEYBOARD_OFF
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, HandleSuperFoldStatusChange07, Function | SmallTest | Level1)
{
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
 * @tc.name: MatchSuperFoldStatusToFoldStatus
 * @tc.desc: test function : EXPANDED
 * @tc.type: FUNC
 */
HWTEST_F(SuperFoldStateManagerTest, MatchSuperFoldStatusToFoldStatus01, Function | SmallTest | Level1)
{
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
HWTEST_F(SuperFoldStateManagerTest, MatchSuperFoldStatusToFoldStatus02, Function | SmallTest | Level1)
{
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
HWTEST_F(SuperFoldStateManagerTest, MatchSuperFoldStatusToFoldStatus03, Function | SmallTest | Level1)
{
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
HWTEST_F(SuperFoldStateManagerTest, MatchSuperFoldStatusToFoldStatus04, Function | SmallTest | Level1)
{
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
HWTEST_F(SuperFoldStateManagerTest, MatchSuperFoldStatusToFoldStatus05, Function | SmallTest | Level1)
{
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

}
}
}