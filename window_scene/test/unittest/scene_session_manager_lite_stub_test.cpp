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
}

}
}
}