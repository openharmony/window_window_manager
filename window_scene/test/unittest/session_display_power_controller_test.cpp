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
#include "session_display_power_controller.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionDisplayPowerControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SessionDisplayPowerControllerTest::SetUpTestCase()
{
}

void SessionDisplayPowerControllerTest::TearDownTestCase()
{
}

void SessionDisplayPowerControllerTest::SetUp()
{
}

void SessionDisplayPowerControllerTest::TearDown()
{
}

namespace {

/**
 * @tc.name: NotifyDisplayEvent
 * @tc.desc: NotifyDisplayEvent_Unlock func
 * @tc.type: FUNC
 */
HWTEST_F(SessionDisplayPowerControllerTest, NotifyDisplayEvent_Unlock, Function | SmallTest | Level1)
{
    DisplayEvent event = DisplayEvent::UNLOCK;
    SessionDisplayPowerController controller([](DisplayId id, sptr<DisplayInfo> info, const std::map<DisplayId,
        sptr<DisplayInfo>>& infos, DisplayStateChangeType type) {
        EXPECT_EQ(id, DISPLAY_ID_INVALID);
        EXPECT_EQ(info, nullptr);
        EXPECT_TRUE(infos.empty());
        EXPECT_EQ(type, DisplayStateChangeType::BEFORE_UNLOCK);
    });
    controller.NotifyDisplayEvent(event);
}

/**
 * @tc.name: NotifyDisplayEvent
 * @tc.desc: NotifyDisplayEvent_KeyguardDrawn func
 * @tc.type: FUNC
 */
    HWTEST_F(SessionDisplayPowerControllerTest, NotifyDisplayEvent_KeyguardDrawn, Function | SmallTest | Level1)
    {
        DisplayEvent event = DisplayEvent::KEYGUARD_DRAWN;
        SessionDisplayPowerController controller([](DisplayId id, sptr<DisplayInfo> info, const std::map<DisplayId,
        sptr<DisplayInfo>>& infos, DisplayStateChangeType type) {
        EXPECT_TRUE(true);
    });
    controller.NotifyDisplayEvent(event);
}
/**
 * @tc.name: SetDisplayState
 * @tc.desc: SetDisplayState func
 * @tc.type: FUNC
 */
HWTEST_F(SessionDisplayPowerControllerTest, SetDisplayState, Function | SmallTest | Level1)
{
    DisplayState state = DisplayState::UNKNOWN;
    SessionDisplayPowerController controller([](DisplayId id, sptr<DisplayInfo> info, const std::map<DisplayId,
        sptr<DisplayInfo>>& infos, DisplayStateChangeType type) {
        EXPECT_TRUE(true);
    });
    bool result = controller.SetDisplayState(state);
    EXPECT_FALSE(result);
    state = DisplayState::ON;
    result = controller.SetDisplayState(state);
    EXPECT_TRUE(result);
    state = DisplayState::OFF;
    result = controller.SetDisplayState(state);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: SuspendBegin
 * @tc.desc: SuspendBegin func
 * @tc.type: FUNC
 */
HWTEST_F(SessionDisplayPowerControllerTest, SuspendBegin, Function | SmallTest | Level1)
{
    PowerStateChangeReason reason = PowerStateChangeReason::POWER_BUTTON;
    SessionDisplayPowerController controller([](DisplayId id, sptr<DisplayInfo> info, const std::map<DisplayId,
        sptr<DisplayInfo>>& infos, DisplayStateChangeType type) {
        EXPECT_TRUE(true);
    });
    EXPECT_TRUE(controller.SuspendBegin(reason));
}

}
} // namespace Rosen
} // namespace OHOS

