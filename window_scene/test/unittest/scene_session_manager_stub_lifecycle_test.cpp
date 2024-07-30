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

#include "iremote_object_mocker.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "session/container/include/window_event_channel.h"
#include "window_manager_agent.h"
#include "zidl/scene_session_manager_stub.h"
#include "zidl/window_manager_agent_interface.h"
#include "pattern_detach_callback.h"
#include "test/mock/mock_session_stage.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionManagerStubLifecycleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SceneSessionManagerStub> stub_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

void SceneSessionManagerStubLifecycleTest::SetUpTestCase()
{
}

void SceneSessionManagerStubLifecycleTest::TearDownTestCase()
{
}

void SceneSessionManagerStubLifecycleTest::SetUp()
{
    stub_ = new SceneSessionManager();
}

void SceneSessionManagerStubLifecycleTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

namespace {
/**
 * @tc.name: HandleRecoverAndReconnectSceneSession
 * @tc.desc: test HandleRecoverAndReconnectSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubLifecycleTest, HandleRecoverAndReconnectSceneSession, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    sptr<ISessionStage> sessionStage = new SessionStageMocker();
    ASSERT_NE(nullptr, sessionStage);
    data.WriteRemoteObject(sessionStage->AsObject());
    sptr<IWindowEventChannel> eventChannel = new WindowEventChannel(sessionStage);
    ASSERT_NE(nullptr, eventChannel);
    data.WriteRemoteObject(eventChannel->AsObject());
    struct RSSurfaceNodeConfig surfaceNodeConfig;
    surfaceNodeConfig.SurfaceNodeName = "SurfaceNode";
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(surfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    surfaceNode->Marshalling(data);
    data.WriteBool(false);
    stub_->HandleRecoverAndReconnectSceneSession(data, reply);

    data.WriteRemoteObject(sessionStage->AsObject());
    data.WriteRemoteObject(eventChannel->AsObject());
    surfaceNode->Marshalling(data);
    data.WriteBool(true);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    property->SetTokenState(true);
    data.WriteStrongParcelable(property);
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    ASSERT_NE(nullptr, windowManagerAgent);
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    int res = stub_->HandleRecoverAndReconnectSceneSession(data, reply);
    EXPECT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: HandlePendingSessionToForeground
 * @tc.desc: test HandlePendingSessionToForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubLifecycleTest, HandlePendingSessionToForeground, Function | SmallTest | Level2)
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
HWTEST_F(
    SceneSessionManagerStubLifecycleTest,
    HandlePendingSessionToBackgroundForDelegator,
    Function | SmallTest | Level2
)
{
    MessageParcel data;
    MessageParcel reply;

    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    int res = stub_->HandlePendingSessionToBackgroundForDelegator(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleTerminateSessionNew
 * @tc.desc: test HandleTerminateSessionNew
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubLifecycleTest, HandleTerminateSessionNew, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(false);

    int res = stub_->HandleTerminateSessionNew(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleSetSessionContinueState
 * @tc.desc: test HandleSetSessionContinueState
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubLifecycleTest, HandleSetSessionContinueState, Function | SmallTest | Level2)
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
 * @tc.name: HandleClearSession
 * @tc.desc: test HandleClearSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubLifecycleTest, HandleClearSession, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t persistentId = 65535;
    data.WriteInt32(persistentId);

    int res = stub_->HandleClearSession(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleClearAllSessions
 * @tc.desc: test HandleClearAllSessions
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubLifecycleTest, HandleClearAllSessions, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int res = stub_->HandleClearAllSessions(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleLockSession
 * @tc.desc: test HandleLockSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubLifecycleTest, HandleLockSession, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t sessionId = 65535;
    data.WriteInt32(sessionId);

    int res = stub_->HandleLockSession(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleUnlockSession
 * @tc.desc: test HandleUnlockSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubLifecycleTest, HandleUnlockSession, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t sessionId = 65535;
    data.WriteInt32(sessionId);

    int res = stub_->HandleUnlockSession(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleMoveSessionsToForeground
 * @tc.desc: test HandleMoveSessionsToForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubLifecycleTest, HandleMoveSessionsToForeground, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    std::vector<int32_t> sessionIds = {1, 2, 3, 15, 1423};
    data.WriteInt32Vector(sessionIds);
    int32_t topSessionId = 1;
    data.WriteInt32(topSessionId);

    int res = stub_->HandleMoveSessionsToForeground(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleMoveSessionsToBackground
 * @tc.desc: test HandleMoveSessionsToBackground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubLifecycleTest, HandleMoveSessionsToBackground, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    std::vector<int32_t> sessionIds = {1, 2, 3, 15, 1423};
    data.WriteInt32Vector(sessionIds);
    std::vector<int32_t> result = {1, 2, 3, 15, 1423};
    data.WriteInt32Vector(result);

    int res = stub_->HandleMoveSessionsToBackground(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleUpdateSessionWindowVisibilityListener
 * @tc.desc: test HandleUpdateSessionWindowVisibilityListener
 * @tc.type: FUNC
 */
HWTEST_F(
    SceneSessionManagerStubLifecycleTest,
    HandleUpdateSessionWindowVisibilityListener,
    Function | SmallTest | Level2
)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(0);
    data.WriteBool(true);

    int res = stub_->HandleUpdateSessionWindowVisibilityListener(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetVisibilityWindowInfo
 * @tc.desc: test HandleGetVisibilityWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubLifecycleTest, HandleGetVisibilityWindowInfo, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int res = stub_->HandleGetVisibilityWindowInfo(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}
}
}
}