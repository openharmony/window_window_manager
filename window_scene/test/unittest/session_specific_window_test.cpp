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

#include "interfaces/include/ws_common.h"
#include "iremote_object_mocker.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "session_info.h"
#include "session_manager.h"
#include "session_manager/include/scene_session_manager.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/main_session.h"
#include "window_manager_agent.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SessionSpecificWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SceneSessionManager> ssm_;

private:
    sptr<Session> session_ = nullptr;
    static constexpr uint32_t WAIT_SYNC_IN_NS = 500000;

    sptr<SessionStageMocker> mockSessionStage_ = nullptr;
    sptr<WindowEventChannelMocker> mockEventChannel_ = nullptr;
};

void SessionSpecificWindowTest::SetUpTestCase() {}

void SessionSpecificWindowTest::TearDownTestCase() {}

void SessionSpecificWindowTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    session_ = sptr<Session>::MakeSptr(info);
    ssm_ = sptr<SceneSessionManager>::MakeSptr();

    mockSessionStage_ = sptr<SessionStageMocker>::MakeSptr();
    ASSERT_NE(mockSessionStage_, nullptr);

    mockEventChannel_ = sptr<WindowEventChannelMocker>::MakeSptr(mockSessionStage_);
    ASSERT_NE(mockEventChannel_, nullptr);
}

void SessionSpecificWindowTest::TearDown()
{
    session_ = nullptr;
    usleep(WAIT_SYNC_IN_NS);
}

namespace {

/**
 * @tc.name: HandleSubWindowClick06
 * @tc.desc: HandleSubWindowClick
 * @tc.type: FUNC
 */
HWTEST_F(SessionSpecificWindowTest, HandleSubWindowClick06, Function | SmallTest | Level2)
{
    ASSERT_NE(session_, nullptr);
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.moduleName_ = "testSession2";
    info.bundleName_ = "testSession3";
    sptr<Session> subSession = sptr<Session>::MakeSptr(info);
    subSession->SetParentSession(session_);
    auto property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_IS_MODAL));
    subSession->SetSessionProperty(property);
    auto result = session_->HandleSubWindowClick(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    EXPECT_EQ(result, WSError::WS_OK);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
