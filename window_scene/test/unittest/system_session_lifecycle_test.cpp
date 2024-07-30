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
#include "session/host/include/system_session.h"

#include "common/include/session_permission.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "session/host/include/session.h"
#include <ui/rs_surface_node.h>
#include "window_event_channel_base.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "pointer_event.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SystemSessionLifecycleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    SessionInfo info;
    sptr <SystemSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr <SystemSession> systemSession_;
private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
};

void SystemSessionLifecycleTest::SetUpTestCase()
{
}

void SystemSessionLifecycleTest::TearDownTestCase()
{
}

void SystemSessionLifecycleTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testSystemSession1";
    info.moduleName_ = "testSystemSession2";
    info.bundleName_ = "testSystemSession3";
    systemSession_ = new (std::nothrow) SystemSession(info, specificCallback);
    EXPECT_NE(nullptr, systemSession_);
}

void SystemSessionLifecycleTest::TearDown()
{
    systemSession_ = nullptr;
}

RSSurfaceNode::SharedPtr SystemSessionLifecycleTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "WindowSessionTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    return surfaceNode;
}

namespace {

/**
 * @tc.name: Show
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionLifecycleTest, Show01, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();

    ASSERT_TRUE((systemSession_ != nullptr));
    ASSERT_EQ(WSError::WS_OK, systemSession_->Show(property));
}

/**
 * @tc.name: Show
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionLifecycleTest, Show02, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_TRUE((property != nullptr));
    property->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    ASSERT_TRUE((systemSession_ != nullptr));
    systemSession_->SetSessionProperty(property);
    ASSERT_EQ(WSError::WS_OK, systemSession_->Show(property));
}

/**
 * @tc.name: Show
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionLifecycleTest, Show03, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_TRUE((property != nullptr));
    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    ASSERT_TRUE((systemSession_ != nullptr));
    systemSession_->SetSessionProperty(property);
    ASSERT_EQ(WSError::WS_OK, systemSession_->Show(property));
}

/**
 * @tc.name: Reconnect01
 * @tc.desc: check func Reconnect
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionLifecycleTest, Reconnect01, Function | SmallTest | Level1)
{
    auto surfaceNode = CreateRSSurfaceNode();
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    EXPECT_NE(nullptr, mockSessionStage);
    sptr<TestWindowEventChannel> testWindowEventChannel = new (std::nothrow) TestWindowEventChannel();
    EXPECT_NE(nullptr, testWindowEventChannel);

    auto result = systemSession_->Reconnect(nullptr, nullptr, nullptr, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = systemSession_->Reconnect(nullptr, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = systemSession_->Reconnect(mockSessionStage, nullptr, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = systemSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = systemSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_OK);


    property->windowState_ = WindowState::STATE_SHOWN;
    result = systemSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_OK);

    property->type_ = WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT;
    result = systemSession_->Reconnect(mockSessionStage, testWindowEventChannel, surfaceNode, property);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Hide
 * @tc.desc: test function : Hide
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionLifecycleTest, Hide, Function | SmallTest | Level1)
{
    ASSERT_TRUE(systemSession_ != nullptr);

    auto ret = systemSession_->Hide();
    ASSERT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: Disconnect
 * @tc.desc: test function : Disconnect
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionLifecycleTest, Disconnect, Function | SmallTest | Level1)
{
    ASSERT_TRUE(systemSession_ != nullptr);

    bool isFromClient = true;
    auto ret = systemSession_->Disconnect(isFromClient);
    ASSERT_EQ(WSError::WS_OK, ret);
}
}
}
}