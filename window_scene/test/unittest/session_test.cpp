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
#include "session/host/include/session.h"
#include "session/host/include/extension_session.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    sptr<Session> session_ = nullptr;
};

void WindowSessionTest::SetUpTestCase()
{
}

void WindowSessionTest::TearDownTestCase()
{
}

void WindowSessionTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testSession1";
    info.bundleName_ = "testSession2";
    session_ = new (std::nothrow) Session(info);
    EXPECT_NE(nullptr, session_);
}

void WindowSessionTest::TearDown()
{
    session_ = nullptr;
}

namespace {
/**
 * @tc.name: SetActive01
 * @tc.desc: set session active
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(WindowSessionTest, SetActive01, Function | SmallTest | Level2)
{
    sptr<ISession> sessionToken = nullptr;
    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker(sessionToken);
    EXPECT_NE(nullptr, mockSessionStage);
    EXPECT_CALL(*(mockSessionStage), SetActive(_)).WillOnce(Return(WSError::WS_OK));
    EXPECT_CALL(*(mockSessionStage), UpdateRect(_, _)).Times(1).WillOnce(Return(WSError::WS_OK));
    session_->sessionStage_ = mockSessionStage;
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, session_->SetActive(true));

    sptr<WindowEventChannelMocker> mockEventChannel = new(std::nothrow) WindowEventChannelMocker(mockSessionStage);
    EXPECT_NE(nullptr, mockEventChannel);
    ASSERT_EQ(WSError::WS_OK, session_->Connect(mockSessionStage, mockEventChannel));
    ASSERT_EQ(WSError::WS_OK, session_->SetActive(true));
    ASSERT_EQ(false, session_->isActive_);

    session_->UpdateSessionState(SessionState::STATE_FOREGROUND);
    ASSERT_EQ(WSError::WS_OK, session_->SetActive(true));
    ASSERT_EQ(true, session_->isActive_);
}

/**
 * @tc.name: UpdateRect01
 * @tc.desc: update rect
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(WindowSessionTest, UpdateRect01, Function | SmallTest | Level2)
{
    sptr<ISession> sessionToken = nullptr;
    sptr<SessionStageMocker> mockSessionStage = new(std::nothrow) SessionStageMocker(sessionToken);
    EXPECT_NE(nullptr, mockSessionStage);
    session_->sessionStage_ = mockSessionStage;
    EXPECT_CALL(*(mockSessionStage), UpdateRect(_, _)).Times(1).WillOnce(Return(WSError::WS_OK));

    WSRect rect = {0, 0, 0, 0};
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, session_->UpdateRect(rect, SizeChangeReason::SHOW));
    sptr<WindowEventChannelMocker> mockEventChannel = new(std::nothrow) WindowEventChannelMocker(mockSessionStage);
    EXPECT_NE(nullptr, mockEventChannel);
    ASSERT_EQ(WSError::WS_OK, session_->Connect(mockSessionStage, mockEventChannel));

    rect = {0, 0, 100, 100};
    EXPECT_CALL(*(mockSessionStage), UpdateRect(_, _)).Times(1).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WSError::WS_OK, session_->UpdateRect(rect, SizeChangeReason::SHOW));
    ASSERT_EQ(rect, session_->winRect_);
}
}
} // namespace Rosen
} // namespace OHOS
