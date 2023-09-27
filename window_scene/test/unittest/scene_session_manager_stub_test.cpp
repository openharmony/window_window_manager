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
#include <message_option.h>
#include <message_parcel.h>
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "window_manager_agent.h"
#include "zidl/scene_session_manager_stub.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionManagerStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SceneSessionManagerStub> stub_;
};

void SceneSessionManagerStubTest::SetUpTestCase()
{
}

void SceneSessionManagerStubTest::TearDownTestCase()
{
}

void SceneSessionManagerStubTest::SetUp()
{
    stub_ = new SceneSessionManager();
}

void SceneSessionManagerStubTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteRequest01
 * @tc.desc: test TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, OnRemoteRequest01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(SceneSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        ISceneSessionManager::SceneSessionManagerMessage::TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest02
 * @tc.desc: test TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, OnRemoteRequest02, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(SceneSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        ISceneSessionManager::SceneSessionManagerMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: HandleDestroyAndDisconnectSpcificSession
 * @tc.desc: test HandleDestroyAndDisconnectSpcificSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleDestroyAndDisconnectSpcificSession, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));

    int res = stub_->HandleDestroyAndDisconnectSpcificSession(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleRegisterWindowManagerAgent
 * @tc.desc: test HandleRegisterWindowManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleRegisterWindowManagerAgent, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    int res = stub_->HandleRegisterWindowManagerAgent(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleUnregisterWindowManagerAgent
 * @tc.desc: test HandleUnregisterWindowManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleUnregisterWindowManagerAgent, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    int res = stub_->HandleUnregisterWindowManagerAgent(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetFocusSessionInfo
 * @tc.desc: test HandleGetFocusSessionInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetFocusSessionInfo, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    int res = stub_->HandleGetFocusSessionInfo(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleSetSessionLabel
 * @tc.desc: test HandleSetSessionLabel
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleSetSessionLabel, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteString(static_cast<string>("123"));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    int res = stub_->HandleSetSessionLabel(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleIsValidSessionIds
 * @tc.desc: test HandleIsValidSessionIds
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleIsValidSessionIds, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    std::vector<int32_t> points {0, 0};
    data.WriteInt32Vector(points);
    int res = stub_->HandleIsValidSessionIds(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandlePendingSessionToForeground
 * @tc.desc: test HandlePendingSessionToForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandlePendingSessionToForeground, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    int res = stub_->HandlePendingSessionToForeground(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandlePendingSessionToBackgroundForDelegator
 * @tc.desc: test HandlePendingSessionToBackgroundForDelegator
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandlePendingSessionToBackgroundForDelegator, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    int res = stub_->HandlePendingSessionToBackgroundForDelegator(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleDumpSessionAll
 * @tc.desc: test HandleDumpSessionAll
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleDumpSessionAll, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    int res = stub_->HandleDumpSessionAll(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleDumpSessionWithId
 * @tc.desc: test HandleDumpSessionWithId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleDumpSessionWithId, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    int32_t x = 1;
    data.WriteInt32(x);

    int res = stub_->HandleDumpSessionWithId(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleTerminateSessionNew
 * @tc.desc: test HandleTerminateSessionNew
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleTerminateSessionNew, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(false);

    int res = stub_->HandleTerminateSessionNew(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetFocusSessionToken
 * @tc.desc: test HandleGetFocusSessionToken
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetFocusSessionToken, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    int res = stub_->HandleGetFocusSessionToken(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleSetGestureNavigationEnabled
 * @tc.desc: test HandleSetGestureNavigationEnabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleSetGestureNavigationEnabled, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(false);

    int res = stub_->HandleSetGestureNavigationEnabled(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetAccessibilityWindowInfo
 * @tc.desc: test HandleGetAccessibilityWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetAccessibilityWindowInfo, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));

    int res = stub_->HandleGetAccessibilityWindowInfo(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleSetSessionContinueState
 * @tc.desc: test HandleSetSessionContinueState
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleSetSessionContinueState, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    int32_t x = 1;
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());
    data.WriteInt32(x);

    int res = stub_->HandleSetSessionContinueState(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleSetSessionGravity
 * @tc.desc: test HandleSetSessionGravity
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleSetSessionGravity, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(1);
    data.WriteUint32(0);
    data.WriteUint32(10);

    int res = stub_->HandleSetSessionGravity(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

}
}
}