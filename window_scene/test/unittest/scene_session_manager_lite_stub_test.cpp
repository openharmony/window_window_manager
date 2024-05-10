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
#include <ipc_types.h>

#include "session_manager/include/zidl/scene_session_manager_lite_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MockSceneSessionManagerLiteStub : public SceneSessionManagerLiteStub {
    WSError SetSessionLabel(const sptr<IRemoteObject>& token, const std::string& label) override
    {
        return WSError::WS_OK;
    }
    WSError SetSessionIcon(const sptr<IRemoteObject>& token, const std::shared_ptr<Media::PixelMap>& icon) override
    {
        return WSError::WS_OK;
    }
    WSError IsValidSessionIds(const std::vector<int32_t>& sessionIds, std::vector<bool>& results) override
    {
        return WSError::WS_OK;
    }
    WSError PendingSessionToForeground(const sptr<IRemoteObject>& token) override
    {
        return WSError::WS_OK;
    }
    WSError PendingSessionToBackgroundForDelegator(const sptr<IRemoteObject>& token) override
    {
        return WSError::WS_OK;
    }
    WSError GetFocusSessionToken(sptr<IRemoteObject>& token) override
    {
        return WSError::WS_OK;
    }
    WSError GetFocusSessionElement(AppExecFwk::ElementName& element) override
    {
        return WSError::WS_OK;
    }
    WSError RegisterSessionListener(const sptr<ISessionListener>& listener) override
    {
        return WSError::WS_OK;
    }
    WSError UnRegisterSessionListener(const sptr<ISessionListener>& listener) override
    {
        return WSError::WS_OK;
    }
    WSError GetSessionInfos(const std::string& deviceId,
        int32_t numMax, std::vector<SessionInfoBean>& sessionInfos) override
    {
        return WSError::WS_OK;
    }
    WSError GetSessionInfo(const std::string& deviceId, int32_t persistentId, SessionInfoBean& sessionInfo) override
    {
        return WSError::WS_OK;
    }
    WSError GetSessionInfoByContinueSessionId(const std::string& continueSessionId,
        SessionInfoBean& sessionInfo) override
    {
        return WSError::WS_OK;
    }
    WSError SetSessionContinueState(const sptr<IRemoteObject>& token, const ContinueState& continueState) override
    {
        return WSError::WS_OK;
    }
    WSError TerminateSessionNew(
        const sptr<AAFwk::SessionInfo> info, bool needStartCaller, bool isFromBroker = false) override
    {
        return WSError::WS_OK;
    }
    WSError GetSessionSnapshot(const std::string& deviceId, int32_t persistentId,
        SessionSnapshot& snapshot, bool isLowResolution) override
    {
        return WSError::WS_OK;
    }
    WSError ClearSession(int32_t persistentId) override
    {
        return WSError::WS_OK;
    }
    WSError ClearAllSessions() override
    {
        return WSError::WS_OK;
    }
    WSError LockSession(int32_t sessionId) override
    {
        return WSError::WS_OK;
    }
    WSError UnlockSession(int32_t sessionId) override
    {
        return WSError::WS_OK;
    }
    WSError MoveSessionsToForeground(const std::vector<std::int32_t>& sessionIds, int32_t topSessionId) override
    {
        return WSError::WS_OK;
    }
    WSError MoveSessionsToBackground(const std::vector<std::int32_t>& sessionIds,
        std::vector<std::int32_t>& result) override
    {
        return WSError::WS_OK;
    }
    WMError RegisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override
    {
        return WMError::WM_OK;
    }
    WMError UnregisterWindowManagerAgent(WindowManagerAgentType type,
        const sptr<IWindowManagerAgent>& windowManagerAgent) override
    {
        return WMError::WM_OK;
    }
    void GetFocusWindowInfo(FocusChangeInfo& focusInfo) override
    {
    }
    WMError CheckWindowId(int32_t windowId, int32_t &pid) override
    {
        return WMError::WM_OK;
    }
    WMError GetMainWindowInfos(int32_t topNum, std::vector<MainWindowInfo>& topNInfo) override
    {
        MainWindowInfo mainWindowInfo;
        topNInfo.push_back(mainWindowInfo);
        return WMError::WM_OK;
    }
    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

class SceneSessionManagerLiteStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SceneSessionManagerLiteStub> sceneSessionManagerLiteStub_ = nullptr;
};

void SceneSessionManagerLiteStubTest::SetUpTestCase()
{
}

void SceneSessionManagerLiteStubTest::TearDownTestCase()
{
}

void SceneSessionManagerLiteStubTest::SetUp()
{
    sceneSessionManagerLiteStub_ = new MockSceneSessionManagerLiteStub();
    EXPECT_NE(nullptr, sceneSessionManagerLiteStub_);
}

void SceneSessionManagerLiteStubTest::TearDown()
{
    sceneSessionManagerLiteStub_ = nullptr;
}

namespace {
/**
 * @tc.name: OnRemoteRequest
 * @tc.desc: test function : OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, OnRemoteRequest, Function | SmallTest | Level1)
{
    uint32_t code = static_cast<uint32_t>(SceneSessionManagerLiteStub::
        SceneSessionManagerLiteMessage::TRANS_ID_SET_SESSION_LABEL);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(u"OpenHarmeny");
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_INVALID_STATE, res);
    data.WriteInterfaceToken(SceneSessionManagerLiteStub::GetDescriptor());
    res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::OnRemoteRequest(1000, data, reply, option);
    EXPECT_EQ(IPC_STUB_UNKNOW_TRANS_ERR, res);
    data.WriteInterfaceToken(SceneSessionManagerLiteStub::GetDescriptor());
    res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleSetSessionIcon
 * @tc.desc: test function : HandleSetSessionIcon
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleSetSessionIcon, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleSetSessionIcon(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleIsValidSessionIds
 * @tc.desc: test function : HandleIsValidSessionIds
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleIsValidSessionIds, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleIsValidSessionIds(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandlePendingSessionToForeground
 * @tc.desc: test function : HandlePendingSessionToForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandlePendingSessionToForeground, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandlePendingSessionToForeground(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandlePendingSessionToBackgroundForDelegator
 * @tc.desc: test function : HandlePendingSessionToBackgroundForDelegator
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandlePendingSessionToBackgroundForDelegator, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandlePendingSessionToBackgroundForDelegator(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleRegisterSessionListener
 * @tc.desc: test function : HandleRegisterSessionListener
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleRegisterSessionListener, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleRegisterSessionListener(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleUnRegisterSessionListener
 * @tc.desc: test function : HandleUnRegisterSessionListener
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleUnRegisterSessionListener, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleUnRegisterSessionListener(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleGetSessionInfos
 * @tc.desc: test function : HandleGetSessionInfos
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleGetSessionInfos, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleUnRegisterSessionListener(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleGetSessionInfo
 * @tc.desc: test function : HandleGetSessionInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleGetSessionInfo, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleGetSessionInfo(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleGetSessionInfoByContinueSessionId
 * @tc.desc: test function : HandleGetSessionInfoByContinueSessionId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleGetSessionInfoByContinueSessionId, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleGetSessionInfoByContinueSessionId(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleTerminateSessionNew
 * @tc.desc: test function : HandleTerminateSessionNew
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleTerminateSessionNew, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleTerminateSessionNew(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleGetFocusSessionToken
 * @tc.desc: test function : HandleGetFocusSessionToken
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleGetFocusSessionToken, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleGetFocusSessionToken(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleGetFocusSessionElement
 * @tc.desc: test function : HandleGetFocusSessionElement
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleGetFocusSessionElement, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleGetFocusSessionElement(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleSetSessionContinueState
 * @tc.desc: test function : HandleSetSessionContinueState
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleSetSessionContinueState, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleSetSessionContinueState(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleGetSessionSnapshot
 * @tc.desc: test function : HandleGetSessionSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleGetSessionSnapshot, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleGetSessionSnapshot(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleClearSession
 * @tc.desc: test function : HandleClearSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleClearSession, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleClearSession(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleClearAllSessions
 * @tc.desc: test function : HandleClearAllSessions
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleClearAllSessions, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleClearAllSessions(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleLockSession
 * @tc.desc: test function : HandleLockSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleLockSession, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleLockSession(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleUnlockSession
 * @tc.desc: test function : HandleUnlockSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleUnlockSession, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleUnlockSession(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleMoveSessionsToForeground
 * @tc.desc: test function : HandleMoveSessionsToForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleMoveSessionsToForeground, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleMoveSessionsToForeground(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleMoveSessionsToBackground
 * @tc.desc: test function : HandleMoveSessionsToBackground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleMoveSessionsToBackground, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleMoveSessionsToBackground(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleGetFocusSessionInfo
 * @tc.desc: test function : HandleGetFocusSessionInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleGetFocusSessionInfo, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleGetFocusSessionInfo(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleCheckWindowId
 * @tc.desc: test function : HandleCheckWindowId
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleCheckWindowId, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t numMax = 100;
    data.WriteInt32(numMax);
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleCheckWindowId(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleRegisterWindowManagerAgent
 * @tc.desc: test function : HandleRegisterWindowManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleRegisterWindowManagerAgent, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleRegisterWindowManagerAgent(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleUnregisterWindowManagerAgent
 * @tc.desc: test function : HandleUnregisterWindowManagerAgent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleUnregisterWindowManagerAgent, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleUnregisterWindowManagerAgent(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleGetVisibilityWindowInfo
 * @tc.desc: test function : HandleGetVisibilityWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleGetVisibilityWindowInfo, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleGetVisibilityWindowInfo(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

/**
 * @tc.name: HandleGetMainWinodowInfo
 * @tc.desc: test function : HandleGetMainWinodowInfo
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerLiteStubTest, HandleGetMainWinodowInfo, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t numMax = 100;
    data.WriteInt32(numMax);
    auto res = sceneSessionManagerLiteStub_->
        SceneSessionManagerLiteStub::HandleGetMainWinodowInfo(data, reply);
    EXPECT_EQ(ERR_NONE, res);
}

}
}
}