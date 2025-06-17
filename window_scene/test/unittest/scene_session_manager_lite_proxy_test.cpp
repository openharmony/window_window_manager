/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
 
#include "iremote_object_mocker.h"
#include "ws_common.h"
#include "mock_message_parcel.h"
#include "mock/mock_session.h"
#include "mock/mock_session_stage.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_lite_interface.h"
#include "session_manager/include/zidl/scene_session_manager_lite_proxy.h"
#include "session_manager/include/zidl/session_router_stack_listener_stub.h"
#include "window_manager_agent.h"
 
using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class sceneSessionManagerLiteProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObject> iRemoteObjectMocker;
};
 
void sceneSessionManagerLiteProxyTest::SetUpTestCase()
{
}
 
void sceneSessionManagerLiteProxyTest::TearDownTestCase()
{
}
 
void sceneSessionManagerLiteProxyTest::SetUp()
{
}
 
void sceneSessionManagerLiteProxyTest::TearDown()
{
}
 
namespace {
/**
 * @tc.name: PendingSessionToBackgroundByPersistentId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerLiteProxyTest, PendingSessionToBackgroundByPersistentId, TestSize.Level1)
{
    sptr<MockIRemoteObject> iRemoteObjectMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<SceneSessionManagerLiteProxy> sceneSessionManagerLiteProxy =
        sptr<SceneSessionManagerLiteProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sceneSessionManagerLiteProxy, nullptr);
    const int32_t persistentId = 1;
    bool shouldBackToCaller = true;
    MockMessageParcel::ClearAllErrorFlag();
    WSError errCode =
        sceneSessionManagerLiteProxy->PendingSessionToBackgroundByPersistentId(persistentId, shouldBackToCaller);
    EXPECT_EQ(errCode, WSError::WS_OK);
 
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->PendingSessionToBackgroundByPersistentId(persistentId, shouldBackToCaller);
    EXPECT_EQ(errCode, WSError::WS_ERROR_INVALID_PARAM);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
 
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->PendingSessionToBackgroundByPersistentId(persistentId, shouldBackToCaller);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInt32ErrorFlag(false);
 
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->PendingSessionToBackgroundByPersistentId(persistentId, shouldBackToCaller);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteBoolErrorFlag(false);
 
    sptr<SceneSessionManagerLiteProxy> sceneSessionManagerLiteProxy2 =
        sptr<SceneSessionManagerLiteProxy>::MakeSptr(nullptr);
    errCode = sceneSessionManagerLiteProxy2->PendingSessionToBackgroundByPersistentId(persistentId, shouldBackToCaller);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
 
    iRemoteObjectMocker->SetRequestResult(1);
    errCode = sceneSessionManagerLiteProxy->PendingSessionToBackgroundByPersistentId(persistentId, shouldBackToCaller);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    iRemoteObjectMocker->SetRequestResult(0);
 
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->PendingSessionToBackgroundByPersistentId(persistentId, shouldBackToCaller);
    MockMessageParcel::SetReadInt32ErrorFlag(false);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
}
}