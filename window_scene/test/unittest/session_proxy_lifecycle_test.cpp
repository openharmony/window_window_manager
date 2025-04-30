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

#include "session_proxy.h"
#include "iremote_object_mocker.h"
#include <gtest/gtest.h>
#include "ws_common.h"

// using namespace FRAME_TRACE;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionProxyLifecycleTest : public testing::Test {
public:
    SessionProxyLifecycleTest() {}
    ~SessionProxyLifecycleTest() {}
};
namespace {

/**
 * @tc.name: Foreground
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyLifecycleTest, Foreground, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: Foreground start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    sptr<WindowSessionProperty> property;
    WSError res = sProxy->Foreground(property);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: Foreground end";
}

/**
 * @tc.name: Foreground
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyLifecycleTest, Foreground1, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: Foreground start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    WSError res = sProxy->Foreground(property);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: Foreground end";
}

/**
 * @tc.name: Background
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyLifecycleTest, Background, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: Background start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    WSError res = sProxy->Background();
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: Background end";
}

/**
 * @tc.name: Disconnect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyLifecycleTest, Disconnect, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: Disconnect start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    WSError res = sProxy->Disconnect();
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: Disconnect end";
}

/**
 * @tc.name: DrawingCompleted
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyLifecycleTest, DrawingCompleted, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: DrawingCompleted start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    WSError res = sProxy->DrawingCompleted();
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: DrawingCompleted end";
}

/**
 * @tc.name: RemoveStartingWindow
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyLifecycleTest, RemoveStartingWindow, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: RemoveStartingWindow start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    WSError res = sProxy->RemoveStartingWindow();
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: RemoveStartingWindow end";
}

/**
 * @tc.name: PendingSessionActivation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyLifecycleTest, PendingSessionActivation, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: PendingSessionActivation start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    sptr<AAFwk::SessionInfo> abilitySessionInfo = nullptr;
    WSError res = sProxy->PendingSessionActivation(abilitySessionInfo);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_SESSION);

    sptr<AAFwk::SessionInfo> abilitySessionInfo1 = sptr<AAFwk::SessionInfo>::MakeSptr();
    ASSERT_NE(abilitySessionInfo1, nullptr);
    res = sProxy->PendingSessionActivation(abilitySessionInfo1);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: PendingSessionActivation end";
}

/**
 * @tc.name: TerminateSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyLifecycleTest, TerminateSession, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: TerminateSession start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    sptr<AAFwk::SessionInfo> abilitySessionInfo = nullptr;
    WSError res = sProxy->TerminateSession(abilitySessionInfo);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_SESSION);

    sptr<AAFwk::SessionInfo> abilitySessionInfo1 = sptr<AAFwk::SessionInfo>::MakeSptr();
    ASSERT_NE(abilitySessionInfo1, nullptr);
    res = sProxy->TerminateSession(abilitySessionInfo1);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: TerminateSession end";
}

/**
 * @tc.name: NotifySessionException
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyLifecycleTest, NotifySessionException, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: NotifySessionException start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionProxy> sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    sptr<AAFwk::SessionInfo> abilitySessionInfo = nullptr;
    ExceptionInfo exceptionInfo;
    WSError res = sProxy->NotifySessionException(abilitySessionInfo, exceptionInfo);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_SESSION);

    sptr<AAFwk::SessionInfo> abilitySessionInfo1 = sptr<AAFwk::SessionInfo>::MakeSptr();
    ASSERT_NE(abilitySessionInfo1, nullptr);
    res = sProxy->NotifySessionException(abilitySessionInfo1, exceptionInfo);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "SessionProxyLifecycleTest: NotifySessionException end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS