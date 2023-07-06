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
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/session_manager_agent_controller.h"
#include "session_info.h"
#include "session/host/include/scene_session.h"
#include "window_manager_agent.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionManagerAgentControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SessionManagerAgentControllerTest::SetUpTestCase()
{
}

void SessionManagerAgentControllerTest::TearDownTestCase()
{
}

void SessionManagerAgentControllerTest::SetUp()
{
}

void SessionManagerAgentControllerTest::TearDown()
{
}

/**
 * @tc.name: RegisterWindowManagerAgent
 * @tc.desc: SesionManagerAgentController rigister window manager agent
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerAgentControllerTest, RegisterWindowManagerAgent, Function | SmallTest | Level3)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;

    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().RegisterWindowManagerAgent(
        windowManagerAgent, type));
    ASSERT_EQ(WMError::WM_OK, SessionManagerAgentController::GetInstance().UnregisterWindowManagerAgent(
        windowManagerAgent, type));
}

} // namespace Rosen
} // namespace OHOS

