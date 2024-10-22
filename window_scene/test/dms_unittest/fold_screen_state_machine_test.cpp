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
#include "interfaces/include/ws_common.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "window_manager_agent.h"
#include "session_manager.h"
#include "mission_info.h"
#include "fold_screen_controller/fold_screen_state_machine.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_US = 100000;
}

class FoldScreenStateMachineTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FoldScreenStateMachineTest::SetUpTestCase()
{
}

void FoldScreenStateMachineTest::TearDownTestCase()
{
}

void FoldScreenStateMachineTest::SetUp()
{
}

void FoldScreenStateMachineTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {

/**
 * @tc.name: RegistrationTransitionCallback01
 * @tc.desc: RegistrationTransitionCallback01 func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, RegistrationTransitionCallback01, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback;
    FoldScreenStateMachine fsm;
    fsm.RegistrationTransitionCallback(callback);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: RegistrationTransitionCallback02
 * @tc.desc: RegistrationTransitionCallback02 func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, RegistrationTransitionCallback02, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback = std::make_shared<TransitionCallback>();
    FoldScreenStateMachine fsm;
    fsm.RegistrationTransitionCallback(callback);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: RegistrationTransitionCallback03
 * @tc.desc: RegistrationTransitionCallback03 func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, RegistrationTransitionCallback03, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback = std::make_shared<TransitionCallback>();
    FoldScreenStateMachine fsm;
    fsm.callbacks_.push_back(callback);
    fsm.RegistrationTransitionCallback(callback);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: RegistrationTransitionCallback04
 * @tc.desc: RegistrationTransitionCallback04 func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, RegistrationTransitionCallback04, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback = std::make_shared<TransitionCallback>();
    std::shared_ptr<TransitionCallback> callback1 = std::make_shared<TransitionCallback>();
    FoldScreenStateMachine fsm;
    fsm.callbacks_.push_back(callback);
    fsm.callbacks_.push_back(callback1);
    fsm.RegistrationTransitionCallback(callback);
    fsm.RegistrationTransitionCallback(callback1);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: UnRegistrationTransitionCallback01
 * @tc.desc: UnRegistrationTransitionCallback01 func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, UnRegistrationTransitionCallback01, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback;
    FoldScreenStateMachine fsm;
    fsm.UnRegistrationTransitionCallback(callback);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: UnRegistrationTransitionCallback02
 * @tc.desc: UnRegistrationTransitionCallback02 func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, UnRegistrationTransitionCallback02, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback = std::make_shared<TransitionCallback>();
    FoldScreenStateMachine fsm;
    fsm.UnRegistrationTransitionCallback(callback);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: UnRegistrationTransitionCallback03
 * @tc.desc: UnRegistrationTransitionCallback03 func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, UnRegistrationTransitionCallback03, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback = std::make_shared<TransitionCallback>();
    FoldScreenStateMachine fsm;
    fsm.callbacks_.push_back(callback);
    fsm.UnRegistrationTransitionCallback(callback);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: UnRegistrationTransitionCallback04
 * @tc.desc: UnRegistrationTransitionCallback04 func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, UnRegistrationTransitionCallback04, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback = std::make_shared<TransitionCallback>();
    std::shared_ptr<TransitionCallback> callback1 = std::make_shared<TransitionCallback>();
    FoldScreenStateMachine fsm;
    fsm.callbacks_.push_back(callback);
    fsm.callbacks_.push_back(callback1);
    fsm.RegistrationTransitionCallback(callback);
    fsm.RegistrationTransitionCallback(callback1);
    fsm.UnRegistrationTransitionCallback(callback);
    fsm.UnRegistrationTransitionCallback(callback1);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: TransitionTo01
 * @tc.desc: TransitionTo01 func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, TransitionTo01, Function | SmallTest | Level1)
{
    FoldScreenState state = FoldScreenState::UNKNOWN;
    FoldScreenStateMachine fsm;
    fsm.TransitionTo(state);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
    state = FoldScreenState::FOLDED;
    fsm.TransitionTo(state);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: TransitionTo02
 * @tc.desc: TransitionTo02 func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, TransitionTo02, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback = std::make_shared<TransitionCallback>();
    FoldScreenState state = FoldScreenState::UNKNOWN;
    FoldScreenStateMachine fsm;
    fsm.callbacks_.push_back(callback);
    fsm.TransitionTo(state);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
    state = FoldScreenState::FOLDED;
    fsm.TransitionTo(state);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: TransitionTo03
 * @tc.desc: TransitionTo03 func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, TransitionTo03, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback = std::make_shared<TransitionCallback>();
    FoldScreenState state = FoldScreenState::HALF_FOLDED;
    FoldScreenStateMachine fsm;
    fsm.callbacks_.push_back(callback);
    fsm.TransitionTo(state);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
    state = FoldScreenState::FOLDED;
    fsm.TransitionTo(state);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: TransitionTo04
 * @tc.desc: TransitionTo04 func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, TransitionTo04, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback = std::make_shared<TransitionCallback>();
    FoldScreenState state = FoldScreenState::FULL;
    FoldScreenStateMachine fsm;
    fsm.callbacks_.push_back(callback);
    fsm.TransitionTo(state);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
    state = FoldScreenState::FOLDED;
    fsm.TransitionTo(state);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: GetCurrentState
 * @tc.desc: GetCurrentState func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, GetCurrentState, Function | SmallTest | Level1)
{
    FoldScreenStateMachine fsm;
    FoldScreenState state = fsm.GetCurrentState();
    ASSERT_EQ(state, fsm.currState_);
}

/**
 * @tc.name: GenStateMachineInfo
 * @tc.desc: GenStateMachineInfo func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, GenStateMachineInfo, Function | SmallTest | Level1)
{
    FoldScreenStateMachine fsm;
    std::string info = fsm.GenStateMachineInfo();
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

}
} // namespace Rosen
} // namespace OHOS
