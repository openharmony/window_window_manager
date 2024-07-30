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
#include "session/host/include/main_session.h"
#include "mock/mock_session_stage.h"
#include "window_event_channel_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MainSessionLifecycleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    SessionInfo info;
    sptr <MainSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr <MainSession> mainSession_;
private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
};

void MainSessionLifecycleTest::SetUpTestCase()
{
}

void MainSessionLifecycleTest::TearDownTestCase()
{
}

void MainSessionLifecycleTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testMainSession1";
    info.moduleName_ = "testMainSession2";
    info.bundleName_ = "testMainSession3";
    mainSession_ = new (std::nothrow) MainSession(info, specificCallback);
    EXPECT_NE(nullptr, mainSession_);
}

void MainSessionLifecycleTest::TearDown()
{
    mainSession_ = nullptr;
}

RSSurfaceNode::SharedPtr MainSessionLifecycleTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "WindowSessionTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    return surfaceNode;
}

namespace {

/**
 * @tc.name: Reconnect01
 * @tc.desc: check func Reconnect
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionLifecycleTest, Reconnect01, Function | SmallTest | Level1)
{
    auto surfaceNode = CreateRSSurfaceNode();
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    sptr<TestWindowEventChannel> testWindowEventChannel = new (std::nothrow) TestWindowEventChannel();
    EXPECT_NE(nullptr, testWindowEventChannel);

    auto result = mainSession_->Reconnect(nullptr, nullptr, nullptr, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = mainSession_->Reconnect(nullptr, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = mainSession_->Reconnect(mockSessionStage, nullptr, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = mainSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_OK);

    property->SetWindowState(WindowState::STATE_HIDDEN);
    result = mainSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: NotifyForegroundInteractiveStatus01
 * @tc.desc: check func NotifyForegroundInteractiveStatus
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionLifecycleTest, NotifyForegroundInteractiveStatus01, Function | SmallTest | Level2)
{
    mainSession_->isVisible_ = true;
    mainSession_->SetSessionState(SessionState::STATE_DISCONNECT);
    mainSession_->NotifyForegroundInteractiveStatus(true);
    mainSession_->NotifyForegroundInteractiveStatus(false);
    ASSERT_EQ(WSError::WS_OK, mainSession_->SetFocusable(false));

    mainSession_->isVisible_ = false;
    mainSession_->SetSessionState(SessionState::STATE_ACTIVE);
    mainSession_->NotifyForegroundInteractiveStatus(true);
    mainSession_->NotifyForegroundInteractiveStatus(false);
    ASSERT_EQ(WSError::WS_OK, mainSession_->SetFocusable(false));

    mainSession_->isVisible_ = false;
    mainSession_->SetSessionState(SessionState::STATE_FOREGROUND);
    mainSession_->NotifyForegroundInteractiveStatus(true);
    mainSession_->NotifyForegroundInteractiveStatus(false);
    ASSERT_EQ(WSError::WS_OK, mainSession_->SetFocusable(false));
}
}
}
}