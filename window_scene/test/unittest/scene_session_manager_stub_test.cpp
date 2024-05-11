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
 * @tc.name: HandleCreateAndConnectSpecificSession
 * @tc.desc: test HandleCreateAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleCreateAndConnectSpecificSession, Function | SmallTest | Level2)
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
    stub_->HandleCreateAndConnectSpecificSession(data, reply);

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

    int res = stub_->HandleCreateAndConnectSpecificSession(data, reply);
    EXPECT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: HandleRecoverAndConnectSpecificSession
 * @tc.desc: test HandleRecoverAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleRecoverAndConnectSpecificSession, Function | SmallTest | Level2)
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
    stub_->HandleRecoverAndConnectSpecificSession(data, reply);

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

    int res = stub_->HandleRecoverAndConnectSpecificSession(data, reply);
    EXPECT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: HandleRecoverAndReconnectSceneSession
 * @tc.desc: test HandleRecoverAndReconnectSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleRecoverAndReconnectSceneSession, Function | SmallTest | Level2)
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
 * @tc.name: HandleDestroyAndDisconnectSpcificSessionWithDetachCallback
 * @tc.desc: test HandleDestroyAndDisconnectSpcificSessionWithDetachCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleDestroyAndDisconnectSpcificSessionWithDetachCallback,
    Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<PatternDetachCallback> callback = new PatternDetachCallback();
    data.WriteRemoteObject(callback->AsObject());

    int res = stub_->HandleDestroyAndDisconnectSpcificSession(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleUpdateProperty
 * @tc.desc: test HandleUpdateProperty
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleUpdateProperty, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_RECT;
    data.WriteUint32(static_cast<uint32_t>(action));
    bool isPropertyExit = false;
    data.WriteBool(isPropertyExit);
    stub_->HandleUpdateProperty(data, reply);

    isPropertyExit = true;
    data.WriteBool(isPropertyExit);
    int res = stub_->HandleUpdateProperty(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleRequestFocusStatus
 * @tc.desc: test HandleRequestFocusStatus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleRequestFocusStatus, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t persistentId = 65535;
    data.WriteInt32(persistentId);
    bool isFocused = true;
    data.WriteBool(isFocused);

    int res = stub_->HandleRequestFocusStatus(data, reply);
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
 * @tc.name: HandleGetFocusSessionElement
 * @tc.desc: test HandleGetFocusSessionElement
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetFocusSessionElement, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int res = stub_->HandleGetFocusSessionElement(data, reply);
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
 * @tc.name: HandleSetSessionIcon
 * @tc.desc: test HandleSetSessionIcon
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleSetSessionIcon, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    int res = stub_->HandleSetSessionIcon(data, reply);
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
 * @tc.name: HandleUnRegisterSessionChangeListener
 * @tc.desc: test HandleUnRegisterSessionChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleUnRegisterSessionChangeListener, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int res = stub_->HandleUnRegisterSessionChangeListener(data, reply);
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
 * @tc.name: HandleGetSessionInfos
 * @tc.desc: test HandleGetSessionInfos
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetSessionInfos, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    data.WriteString16(static_cast<std::u16string>(u"123"));
    int32_t numMax = 100;
    data.WriteInt32(numMax);

    int res = stub_->HandleGetSessionInfos(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetSessionInfo
 * @tc.desc: test HandleGetSessionInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetSessionInfo, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    data.WriteString16(static_cast<std::u16string>(u"123"));
    int32_t persistentId = 65535;
    data.WriteInt32(persistentId);

    int res = stub_->HandleGetSessionInfo(data, reply);
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
 * @tc.name: HandleCheckWindowId
 * @tc.desc: test HandleCheckWindowId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleCheckWindowId, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t windowId = 1000;
    SessionInfo info;
    info.abilityName_ = "HandleCheckWindowId";
    info.bundleName_ = "HandleCheckWindowId1";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    SceneSessionManager::GetInstance().sceneSessionMap_[windowId] = sceneSession;
    data.WriteInt32(windowId);

    int res = stub_->HandleCheckWindowId(data, reply);
    EXPECT_EQ(res, ERR_INVALID_DATA);
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
 * @tc.name: HandleGetSessionDump
 * @tc.desc: test HandleGetSessionDump
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetSessionDump, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    std::vector<std::string> params = {"-a"};
    data.WriteStringVector(params);
    stub_->HandleGetSessionDump(data, reply);

    params.clear();
    params.push_back("-w");
    params.push_back("23456");
    data.WriteStringVector(params);
    stub_->HandleGetSessionDump(data, reply);

    params.clear();
    data.WriteStringVector(params);

    int res = stub_->HandleGetSessionDump(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleUpdateSessionAvoidAreaListener
 * @tc.desc: test HandleUpdateSessionAvoidAreaListener
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleUpdateSessionAvoidAreaListener, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t persistentId = 65535;
    data.WriteInt32(persistentId);
    bool haveAvoidAreaListener = false;
    data.WriteBool(haveAvoidAreaListener);

    int res = stub_->HandleUpdateSessionAvoidAreaListener(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetSessionSnapshot
 * @tc.desc: test HandleGetSessionSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetSessionSnapshot, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    data.WriteString16(static_cast<std::u16string>(u"123"));
    int32_t persistentId = 65535;
    data.WriteInt32(persistentId);
    bool isLowResolution = false;
    data.WriteBool(isLowResolution);

    int res = stub_->HandleGetSessionSnapshot(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleBindDialogTarget
 * @tc.desc: test HandleBindDialogTarget
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleBindDialogTarget, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    uint64_t persistentId = 65535;
    data.WriteUint64(persistentId);
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    int res = stub_->HandleBindDialogTarget(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleNotifyDumpInfoResult
 * @tc.desc: test HandleNotifyDumpInfoResult
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleNotifyDumpInfoResult, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    uint32_t vectorSize = 128;
    data.WriteUint32(vectorSize);
    stub_->HandleNotifyDumpInfoResult(data, reply);

    std::vector<std::string> info = {"-a", "-b123", "-c3456789", ""};
    vectorSize = static_cast<uint32_t>(info.size());
    data.WriteUint32(vectorSize);
    uint32_t curSize;
    for (const auto& elem : info) {
        const char* curInfo = elem.c_str();
        curSize = static_cast<uint32_t>(strlen(curInfo));
        data.WriteUint32(curSize);
        if (curSize != 0) {
            data.WriteRawData(curInfo, curSize);
        }
    }

    int res = stub_->HandleNotifyDumpInfoResult(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleClearSession
 * @tc.desc: test HandleClearSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleClearSession, Function | SmallTest | Level2)
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
HWTEST_F(SceneSessionManagerStubTest, HandleClearAllSessions, Function | SmallTest | Level2)
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
HWTEST_F(SceneSessionManagerStubTest, HandleLockSession, Function | SmallTest | Level2)
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
HWTEST_F(SceneSessionManagerStubTest, HandleUnlockSession, Function | SmallTest | Level2)
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
HWTEST_F(SceneSessionManagerStubTest, HandleMoveSessionsToForeground, Function | SmallTest | Level2)
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
HWTEST_F(SceneSessionManagerStubTest, HandleMoveSessionsToBackground, Function | SmallTest | Level2)
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
 * @tc.name: HandleUnregisterCollaborator
 * @tc.desc: test HandleUnregisterCollaborator
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleUnregisterCollaborator, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t type = CollaboratorType::RESERVE_TYPE;
    data.WriteInt32(type);

    int res = stub_->HandleUnregisterCollaborator(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleUpdateSessionTouchOutsideListener
 * @tc.desc: test HandleUpdateSessionTouchOutsideListener
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleUpdateSessionTouchOutsideListener, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t persistentId = 65535;
    data.WriteInt32(persistentId);
    bool haveAvoidAreaListener = true;
    data.WriteBool(haveAvoidAreaListener);

    int res = stub_->HandleUpdateSessionTouchOutsideListener(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleRaiseWindowToTop
 * @tc.desc: test HandleRaiseWindowToTop
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleRaiseWindowToTop, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t persistentId = 65535;
    data.WriteInt32(persistentId);

    int res = stub_->HandleRaiseWindowToTop(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleNotifyWindowExtensionVisibilityChange
 * @tc.desc: test HandleNotifyWindowExtensionVisibilityChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleNotifyWindowExtensionVisibilityChange, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t pid = 65535;
    data.WriteInt32(pid);
    int32_t uid = 12345;
    data.WriteInt32(uid);
    bool visible = true;
    data.WriteBool(visible);

    int res = stub_->HandleNotifyWindowExtensionVisibilityChange(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetTopWindowId
 * @tc.desc: test HandleGetTopWindowId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetTopWindowId, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    uint32_t mainWinId = 65535;
    data.WriteUint32(mainWinId);

    int res = stub_->HandleGetTopWindowId(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleUpdateSessionWindowVisibilityListener
 * @tc.desc: test HandleUpdateSessionWindowVisibilityListener
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleUpdateSessionWindowVisibilityListener, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(0);
    data.WriteBool(true);

    int res = stub_->HandleUpdateSessionWindowVisibilityListener(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleShiftAppWindowFocus
 * @tc.desc: test HandleShiftAppWindowFocus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleShiftAppWindowFocus, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t sourcePersistentId = 12345;
    data.WriteInt32(sourcePersistentId);
    int32_t targetPersistentId = 65535;
    data.WriteInt32(targetPersistentId);

    int res = stub_->HandleShiftAppWindowFocus(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetVisibilityWindowInfo
 * @tc.desc: test HandleGetVisibilityWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetVisibilityWindowInfo, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int res = stub_->HandleGetVisibilityWindowInfo(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleAddExtensionWindowStageToSCB
 * @tc.desc: test HandleAddExtensionWindowStageToSCB
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleAddExtensionWindowStageToSCB, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    sptr<ISessionStage> sessionStage = new SessionStageMocker();
    data.WriteRemoteObject(sessionStage->AsObject());

    int32_t persistentId = 65535;
    data.WriteInt32(persistentId);
    int32_t parentId = 12345;
    data.WriteInt32(parentId);

    int res = stub_->HandleAddExtensionWindowStageToSCB(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleAddOrRemoveSecureSession
 * @tc.desc: test HandleAddOrRemoveSecureSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleAddOrRemoveSecureSession, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(12345);
    data.WriteBool(true);

    int res = stub_->HandleAddOrRemoveSecureSession(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetSessionInfoByContinueSessionId
 * @tc.desc: test HandleGetSessionInfoByContinueSessionId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetSessionInfoByContinueSessionId, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteString("test_01");

    int res = stub_->HandleGetSessionInfoByContinueSessionId(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleUpdateExtWindowFlags
 * @tc.desc: test HandleUpdateExtWindowFlags
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleUpdateExtWindowFlags, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(1234);
    data.WriteInt32(12345);
    data.WriteInt32(7);
    data.WriteInt32(7);

    int res = stub_->HandleUpdateExtWindowFlags(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetHostWindowRect
 * @tc.desc: test HandleGetHostWindowRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetHostWindowRect, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t hostWindowId = 65535;
    data.WriteInt32(hostWindowId);

    int res = stub_->HandleGetHostWindowRect(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: HandleGetCallingWindowWindowStatus
 * @tc.desc: test HandleGetCallingWindowWindowStatus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetCallingWindowWindowStatus, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t persistentId = 65535;
    data.WriteInt32(persistentId);

    int res = stub_->HandleGetCallingWindowWindowStatus(data, reply);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

/**
 * @tc.name: HandleGetCallingWindowRect
 * @tc.desc: test HandleGetCallingWindowRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, HandleGetCallingWindowRect, Function | SmallTest | Level2)
{
    if (stub_ == nullptr) {
        return;
    }

    MessageParcel data;
    MessageParcel reply;

    int32_t persistentId = 65535;
    data.WriteInt32(persistentId);

    int res = stub_->HandleGetCallingWindowRect(data, reply);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

}
}
}