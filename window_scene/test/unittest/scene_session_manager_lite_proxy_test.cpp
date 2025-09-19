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
#include "session_manager/include/zidl/pip_change_listener_stub.h"
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

private:
    sptr<MockIRemoteObject> iRemoteObjectMocker_ {nullptr};
    sptr<SceneSessionManagerLiteProxy> sceneSessionManagerLiteProxy_ {nullptr};
};
 
void sceneSessionManagerLiteProxyTest::SetUpTestCase()
{
}
 
void sceneSessionManagerLiteProxyTest::TearDownTestCase()
{
}
 
void sceneSessionManagerLiteProxyTest::SetUp()
{
    iRemoteObjectMocker_ = sptr<MockIRemoteObject>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker_, nullptr);
    sceneSessionManagerLiteProxy_ = sptr<SceneSessionManagerLiteProxy>::MakeSptr(iRemoteObjectMocker_);
    ASSERT_NE(sceneSessionManagerLiteProxy_, nullptr);
}
 
void sceneSessionManagerLiteProxyTest::TearDown()
{
    iRemoteObjectMocker_ = nullptr;
    sceneSessionManagerLiteProxy_ = nullptr;
    MockMessageParcel::ClearAllErrorFlag();
}
 
namespace {
/**
 * @tc.name: PendingSessionToBackground
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerLiteProxyTest, PendingSessionToBackground, TestSize.Level1)
{
    BackgroundParams params;
    sptr<IRemoteObject> token = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SceneSessionManagerLiteProxy> sceneSessionManagerLiteProxy =
        sptr<SceneSessionManagerLiteProxy>::MakeSptr(token);
    ASSERT_NE(sceneSessionManagerLiteProxy, nullptr);
    MockMessageParcel::ClearAllErrorFlag();
    WSError errCode = sceneSessionManagerLiteProxy->PendingSessionToBackground(token, params);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->PendingSessionToBackground(token, params);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    MockMessageParcel::SetWriteRemoteObjectErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->PendingSessionToBackground(token, params);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    MockMessageParcel::SetWriteRemoteObjectErrorFlag(false);
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->PendingSessionToBackground(token, params);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    MockMessageParcel::SetWriteRemoteObjectErrorFlag(false);
    MockMessageParcel::SetWriteInt32ErrorFlag(false);
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->PendingSessionToBackground(token, params);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    MockMessageParcel::SetWriteRemoteObjectErrorFlag(false);
    MockMessageParcel::SetWriteInt32ErrorFlag(false);
    MockMessageParcel::SetWriteBoolErrorFlag(false);
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->PendingSessionToBackground(token, params);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    MockMessageParcel::SetWriteRemoteObjectErrorFlag(false);
    MockMessageParcel::SetWriteInt32ErrorFlag(false);
    MockMessageParcel::SetWriteBoolErrorFlag(false);
    MockMessageParcel::SetWriteParcelableErrorFlag(false);
    errCode = sceneSessionManagerLiteProxy->PendingSessionToBackground(token, params);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    MockMessageParcel::SetWriteRemoteObjectErrorFlag(false);
    MockMessageParcel::SetWriteInt32ErrorFlag(false);
    MockMessageParcel::SetWriteBoolErrorFlag(false);
    MockMessageParcel::SetWriteParcelableErrorFlag(false);
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->PendingSessionToBackground(token, params);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::ClearAllErrorFlag();
}

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

/**
 * @tc.name: UpdateWindowModeByIdForUITest01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerLiteProxyTest, UpdateWindowModeByIdForUITest01, TestSize.Level1)
{
    sptr<MockIRemoteObject> iRemoteObjectMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<SceneSessionManagerLiteProxy> sceneSessionManagerLiteProxy =
        sptr<SceneSessionManagerLiteProxy>::MakeSptr(iRemoteObjectMocker);
    const int32_t windowId = 1;
    const int32_t updateMode = 1;
    MockMessageParcel::ClearAllErrorFlag();
    WMError errCode =
        sceneSessionManagerLiteProxy->UpdateWindowModeByIdForUITest(windowId, updateMode);
    EXPECT_EQ(errCode, WMError::WM_OK);
 
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->UpdateWindowModeByIdForUITest(windowId, updateMode);
    EXPECT_EQ(errCode, WMError::WM_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
 
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->UpdateWindowModeByIdForUITest(windowId, updateMode);
    EXPECT_EQ(errCode, WMError::WM_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInt32ErrorFlag(false);
 
    sptr<SceneSessionManagerLiteProxy> sceneSessionManagerLiteProxy2 =
        sptr<SceneSessionManagerLiteProxy>::MakeSptr(nullptr);
    errCode = sceneSessionManagerLiteProxy2->UpdateWindowModeByIdForUITest(windowId, updateMode);
    EXPECT_EQ(errCode, WMError::WM_ERROR_NULLPTR);
 
    iRemoteObjectMocker->SetRequestResult(1);
    errCode = sceneSessionManagerLiteProxy->UpdateWindowModeByIdForUITest(windowId, updateMode);
    EXPECT_EQ(errCode, WMError::WM_ERROR_IPC_FAILED);
    iRemoteObjectMocker->SetRequestResult(0);
 
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->UpdateWindowModeByIdForUITest(windowId, updateMode);
    MockMessageParcel::SetReadInt32ErrorFlag(false);
    EXPECT_EQ(errCode, WMError::WM_OK);
    MockMessageParcel::ClearAllErrorFlag();
}

/**
 * @tc.name: SendPointerEventForHover
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerLiteProxyTest, SendPointerEventForHover, TestSize.Level1)
{
    sptr<MockIRemoteObject> iRemoteObjectMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<SceneSessionManagerLiteProxy> sceneSessionManagerLiteProxy =
        sptr<SceneSessionManagerLiteProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sceneSessionManagerLiteProxy, nullptr);
    const int32_t persistentId = 1;
    bool shouldBackToCaller = true;
    MockMessageParcel::ClearAllErrorFlag();
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    WSError errCode = sceneSessionManagerLiteProxy->SendPointerEventForHover(pointerEvent);
    EXPECT_EQ(errCode, WSError::WS_OK);

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->SendPointerEventForHover(pointerEvent);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);

    iRemoteObjectMocker->SetRequestResult(1);
    errCode = sceneSessionManagerLiteProxy->SendPointerEventForHover(pointerEvent);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    iRemoteObjectMocker->SetRequestResult(0);

    MockMessageParcel::SetReadInt32ErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->SendPointerEventForHover(pointerEvent);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::SetReadInt32ErrorFlag(false);
}

/**
 * @tc.name: IsFocusWindowParent
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerLiteProxyTest, IsFocusWindowParent, TestSize.Level1)
{
    sptr<MockIRemoteObject> iRemoteObjectMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<SceneSessionManagerLiteProxy> sceneSessionManagerLiteProxy =
        sptr<SceneSessionManagerLiteProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sceneSessionManagerLiteProxy, nullptr);
    MockMessageParcel::ClearAllErrorFlag();
    sptr<IRemoteObject> token = nullptr;
    bool isParent = false;
    WSError errCode = sceneSessionManagerLiteProxy->IsFocusWindowParent(token, isParent);
    EXPECT_EQ(errCode, WSError::WS_ERROR_INVALID_PARAM);

    token = sptr<MockIRemoteObject>::MakeSptr();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->IsFocusWindowParent(token, isParent);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);

    iRemoteObjectMocker->SetRequestResult(1);
    errCode = sceneSessionManagerLiteProxy->IsFocusWindowParent(token, isParent);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    iRemoteObjectMocker->SetRequestResult(0);

    MockMessageParcel::SetReadBoolErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->IsFocusWindowParent(token, isParent);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::SetReadBoolErrorFlag(false);

    MockMessageParcel::SetReadInt32ErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->IsFocusWindowParent(token, isParent);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::SetReadInt32ErrorFlag(false);
}

/**
 * @tc.name: NotifyAppUseControlList
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerLiteProxyTest, NotifyAppUseControlList, TestSize.Level1)
{
    sptr<MockIRemoteObject> iRemoteObjectMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<SceneSessionManagerLiteProxy> sceneSessionManagerLiteProxy =
        sptr<SceneSessionManagerLiteProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sceneSessionManagerLiteProxy, nullptr);
    MockMessageParcel::ClearAllErrorFlag();
    ControlAppType type = ControlAppType::DLP;
    int32_t userId = 100;
    std::vector<AppUseControlInfo> controlList;
    AppUseControlInfo controlInfo;
    controlList.push_back(controlInfo);
    WMError errCode = sceneSessionManagerLiteProxy->NotifyAppUseControlList(type, userId, controlList);
    EXPECT_EQ(errCode, WSError::WS_OK);
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    errCode = sceneSessionManagerLiteProxy->NotifyAppUseControlList(type, userId, controlList);
    EXPECT_EQ(errCode, WSError::WS_ERROR_INVALID_PARAM);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, UnregPipChgListenerByScreenId_WriteTokenFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->UnregisterPipChgListenerByScreenId(1), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, UnregPipChgListenerByScreenId_WriteIntFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->UnregisterPipChgListenerByScreenId(1), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, UnregPipChgListenerByScreenId_ReadIntFailed, TestSize.Level1)
{
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->UnregisterPipChgListenerByScreenId(1), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, UnregPipChgListenerByScreenId_SendReqFailed, TestSize.Level1)
{
    iRemoteObjectMocker_->SetRequestResult(1);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->UnregisterPipChgListenerByScreenId(1), WMError::WM_ERROR_IPC_FAILED);
    iRemoteObjectMocker_->SetRequestResult(0);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, UnregPipChgListenerByScreenId_RemoteNullFailed, TestSize.Level1)
{
    sptr<SceneSessionManagerLiteProxy> liteProxyNullRemote = sptr<SceneSessionManagerLiteProxy>::MakeSptr(nullptr);
    ASSERT_NE(liteProxyNullRemote, nullptr);
    EXPECT_EQ(liteProxyNullRemote->UnregisterPipChgListenerByScreenId(1), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, RegisterPipChgListenerByScreenId_ShouldReturnOk_WhenNotReg, TestSize.Level1)
{
    EXPECT_EQ(sceneSessionManagerLiteProxy_->UnregisterPipChgListenerByScreenId(1), WMError::WM_OK);
}

class MockPipChgListener : public PipChangeListenerStub {
public:
    void OnPipStart(int32_t windowId) override {};
};

HWTEST_F(sceneSessionManagerLiteProxyTest, RegPipChgListenerByScreenId_Success, TestSize.Level1)
{
    sptr<IPipChangeListener> listener = sptr<MockPipChgListener>::MakeSptr();
    EXPECT_EQ(sceneSessionManagerLiteProxy_->RegisterPipChgListenerByScreenId(1, listener), WMError::WM_OK);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, UnregPipChgListenerByScreenId_ShouldReturnOk_WhenRegOK, TestSize.Level1)
{
    sptr<IPipChangeListener> listener = sptr<MockPipChgListener>::MakeSptr();
    EXPECT_EQ(sceneSessionManagerLiteProxy_->RegisterPipChgListenerByScreenId(1, listener), WMError::WM_OK);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->UnregisterPipChgListenerByScreenId(1), WMError::WM_OK);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, RegPipChgListenerByScreenId_WriteTokenFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    auto result = sceneSessionManagerLiteProxy_->RegisterPipChgListenerByScreenId(1, nullptr);
    EXPECT_EQ(result, WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, RegPipChgListenerByScreenId_WriteIntFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    auto result = sceneSessionManagerLiteProxy_->RegisterPipChgListenerByScreenId(1, nullptr);
    EXPECT_EQ(result, WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, RegPipChgListenerByScreenId_NullListenerFailed, TestSize.Level1)
{
    auto result = sceneSessionManagerLiteProxy_->RegisterPipChgListenerByScreenId(1, nullptr);
    EXPECT_EQ(result, WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, RegPipChgListenerByScreenId_WriteRemoteObjFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteRemoteObjectErrorFlag(true);
    sptr<IPipChangeListener> listener = sptr<MockPipChgListener>::MakeSptr();
    auto result = sceneSessionManagerLiteProxy_->RegisterPipChgListenerByScreenId(1, listener);
    EXPECT_EQ(result, WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, RegPipChgListenerByScreenId_SendReqFailed, TestSize.Level1)
{
    iRemoteObjectMocker_->SetRequestResult(1);
    sptr<IPipChangeListener> listener = sptr<MockPipChgListener>::MakeSptr();
    auto result = sceneSessionManagerLiteProxy_->RegisterPipChgListenerByScreenId(1, listener);
    EXPECT_EQ(result, WMError::WM_ERROR_IPC_FAILED);
    iRemoteObjectMocker_->SetRequestResult(0);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, RegPipChgListenerByScreenId_RemoteNullFailed, TestSize.Level1)
{
    sptr<SceneSessionManagerLiteProxy> liteProxyNullRemote = sptr<SceneSessionManagerLiteProxy>::MakeSptr(nullptr);
    ASSERT_NE(liteProxyNullRemote, nullptr);
    sptr<IPipChangeListener> listener = sptr<MockPipChgListener>::MakeSptr();
    EXPECT_EQ(liteProxyNullRemote->RegisterPipChgListenerByScreenId(1, listener), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, RegPipChgListenerByScreenId_ReadIntFailed, TestSize.Level1)
{
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    sptr<IPipChangeListener> listener = sptr<MockPipChgListener>::MakeSptr();
    auto result = sceneSessionManagerLiteProxy_->RegisterPipChgListenerByScreenId(1, listener);
    EXPECT_EQ(result, WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, SetPipEnableByScreenId_Success, TestSize.Level1)
{
    EXPECT_EQ(sceneSessionManagerLiteProxy_->SetPipEnableByScreenId(1, false), WMError::WM_OK);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, SetPipEnableByScreenId_WriteTokenFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->SetPipEnableByScreenId(1, false), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, SetPipEnableByScreenId_WriteIntFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->SetPipEnableByScreenId(1, false), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, SetPipEnableByScreenId_WriteBoolFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->SetPipEnableByScreenId(1, false), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, SetPipEnableByScreenId_SendReqFailed, TestSize.Level1)
{
    iRemoteObjectMocker_->SetRequestResult(1);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->SetPipEnableByScreenId(1, false), WMError::WM_ERROR_IPC_FAILED);
    iRemoteObjectMocker_->SetRequestResult(0);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, SetPipEnableByScreenId_RemoteNullFailed, TestSize.Level1)
{
    sptr<SceneSessionManagerLiteProxy> liteProxyNullRemote = sptr<SceneSessionManagerLiteProxy>::MakeSptr(nullptr);
    ASSERT_NE(liteProxyNullRemote, nullptr);
    EXPECT_EQ(liteProxyNullRemote->SetPipEnableByScreenId(1, false), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, SetPipEnableByScreenId_ReadIntFailed, TestSize.Level1)
{
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->SetPipEnableByScreenId(1, false), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, UnsetPipEnableByScreenId_Success, TestSize.Level1)
{
    EXPECT_EQ(sceneSessionManagerLiteProxy_->UnsetPipEnableByScreenId(1), WMError::WM_OK);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, UnsetPipEnableByScreenId_WriteTokenFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->UnsetPipEnableByScreenId(1), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, UnsetPipEnableByScreenId_WriteIntFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->UnsetPipEnableByScreenId(1), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, UnsetPipEnableByScreenId_SendReqFailed, TestSize.Level1)
{
    iRemoteObjectMocker_->SetRequestResult(1);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->UnsetPipEnableByScreenId(1), WMError::WM_ERROR_IPC_FAILED);
    iRemoteObjectMocker_->SetRequestResult(0);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, UnsetPipEnableByScreenId_RemoteNullFailed, TestSize.Level1)
{
    sptr<SceneSessionManagerLiteProxy> liteProxyNullRemote = sptr<SceneSessionManagerLiteProxy>::MakeSptr(nullptr);
    ASSERT_NE(liteProxyNullRemote, nullptr);
    EXPECT_EQ(liteProxyNullRemote->UnsetPipEnableByScreenId(1), WMError::WM_ERROR_IPC_FAILED);
}

HWTEST_F(sceneSessionManagerLiteProxyTest, UnsetPipEnableByScreenId_ReadIntFailed, TestSize.Level1)
{
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    EXPECT_EQ(sceneSessionManagerLiteProxy_->UnsetPipEnableByScreenId(1), WMError::WM_ERROR_IPC_FAILED);
}
}
}
}