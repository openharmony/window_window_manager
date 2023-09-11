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
}

namespace {

/**
 * @tc.name: RegistrationTransitionCallback
 * @tc.desc: RegistrationTransitionCallback func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, RegistrationTransitionCallback, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback;
    FoldScreenStateMachine fsm;
    fsm.RegistrationTransitionCallback(callback);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: UnRegistrationTransitionCallback
 * @tc.desc: UnRegistrationTransitionCallback func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, UnRegistrationTransitionCallback, Function | SmallTest | Level1)
{
    std::shared_ptr<TransitionCallback> callback;
    FoldScreenStateMachine fsm;
    fsm.UnRegistrationTransitionCallback(callback);
    ASSERT_EQ(fsm.GetCurrentState(), fsm.currState_);
}

/**
 * @tc.name: TransitionTo
 * @tc.desc: TransitionTo func
 * @tc.type: FUNC
 */
HWTEST_F(FoldScreenStateMachineTest, TransitionTo, Function | SmallTest | Level1)
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
