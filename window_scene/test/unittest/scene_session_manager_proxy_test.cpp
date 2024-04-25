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
#include "iremote_object_mocker.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "session_manager/include/zidl/scene_session_manager_proxy.h"
#include "window_manager_agent.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class sceneSessionManagerProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObject> iRemoteObjectMocker;
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_;
};

void sceneSessionManagerProxyTest::SetUpTestCase()
{
}

void sceneSessionManagerProxyTest::TearDownTestCase()
{
}

void sceneSessionManagerProxyTest::SetUp()
{
}

void sceneSessionManagerProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: RegisterWindowManagerAgent01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RegisterWindowManagerAgent01, Function | SmallTest | Level2)
{
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->RegisterWindowManagerAgent(type, windowManagerAgent));
    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->UnregisterWindowManagerAgent(type, windowManagerAgent));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: UpdateSessionWindowVisibilityListener001
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, UpdateSessionWindowVisibilityListener001, Function | SmallTest | Level2)
{
    int32_t persistentId = 0;
    bool haveListener = true;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->UpdateSessionWindowVisibilityListener(persistentId,
        haveListener));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: AddOrRemoveSecureSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, AddOrRemoveSecureSession001, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy = new SceneSessionManagerProxy(iRemoteObjectMocker);

    int32_t persistentId = 12345;
    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy->AddOrRemoveSecureSession(persistentId, true));
    sceneSessionManagerProxy = nullptr;
}

/**
 * @tc.name: UpdateExtWindowFlags
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, UpdateExtWindowFlags, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy = new SceneSessionManagerProxy(iRemoteObjectMocker);

    int32_t parentId = 1234;
    int32_t persistentId = 12345;
    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy->UpdateExtWindowFlags(parentId, persistentId, 7, 7));
    sceneSessionManagerProxy = nullptr;
}

/**
 * @tc.name: GetSessionInfoByContinueSessionId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetSessionInfoByContinueSessionId, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    std::string continueSessionId = "test_01";
    SessionInfoBean missionInfo;
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED,
        sceneSessionManagerProxy_->GetSessionInfoByContinueSessionId(continueSessionId, missionInfo));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: RequestFocusStatus01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RequestFocusStatus01, Function | SmallTest | Level2)
{
    int32_t persistendId = 0;
    bool isFocused = true;
    bool byForeground = true;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->RequestFocusStatus(persistendId, isFocused, byForeground));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: RaiseWindowToTop
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RaiseWindowToTop, Function | SmallTest | Level2)
{
    int32_t persistendId = 0;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->RaiseWindowToTop(persistendId));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: UpdateSessionAvoidAreaListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, UpdateSessionAvoidAreaListener, Function | SmallTest | Level2)
{
    int32_t persistendId = 0;
    bool haveListener = true;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->UpdateSessionAvoidAreaListener(persistendId,
        haveListener));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: SetSessionLabel
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, SetSessionLabel, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> token = nullptr;
    std::string label = "SetSessionLabel";
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, sceneSessionManagerProxy_->SetSessionLabel(token, label));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: IsValidSessionIds
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, IsValidSessionIds, Function | SmallTest | Level2)
{
    std::vector<int32_t> sessionIds;
    std::vector<bool> results;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->IsValidSessionIds(sessionIds, results));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: PendingSessionToForeground
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, PendingSessionToForeground, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> token = nullptr;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, sceneSessionManagerProxy_->PendingSessionToForeground(token));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: PendingSessionToBackgroundForDelegator
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, PendingSessionToBackgroundForDelegator, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> token = nullptr;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, sceneSessionManagerProxy_->PendingSessionToBackgroundForDelegator(token));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: RegisterSessionListener01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RegisterSessionListener01, Function | SmallTest | Level2)
{
    sptr<ISessionListener> listener = nullptr;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_ERROR_INVALID_PARAM, sceneSessionManagerProxy_->RegisterSessionListener(listener));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: UnRegisterSessionListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, UnRegisterSessionListener, Function | SmallTest | Level2)
{
    sptr<ISessionListener> listener = nullptr;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_ERROR_INVALID_PARAM, sceneSessionManagerProxy_->UnRegisterSessionListener(listener));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: GetSessionDumpInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetSessionDumpInfo, Function | SmallTest | Level2)
{
    std::vector<std::string> params;
    std::string info = "info";
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->GetSessionDumpInfo(params, info));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: LockSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, LockSession, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->LockSession(0));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: ClearSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, ClearSession, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->LockSession(0));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: RegisterIAbilityManagerCollaborator
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RegisterIAbilityManagerCollaborator, Function | SmallTest | Level2)
{
    sptr<AAFwk::IAbilityManagerCollaborator> impl = nullptr;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_ERROR_INVALID_PARAM, sceneSessionManagerProxy_->RegisterIAbilityManagerCollaborator(0, impl));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: NotifyWindowExtensionVisibilityChange
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, NotifyWindowExtensionVisibilityChange, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->NotifyWindowExtensionVisibilityChange(0, 0, true));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: GetTopWindowId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetTopWindowId, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    uint32_t topWinId = 1;
    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->GetTopWindowId(0, topWinId));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: GetVisibilityWindowInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetVisibilityWindowInfo, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ = new SceneSessionManagerProxy(iRemoteObjectMocker);

    std::vector<sptr<WindowVisibilityInfo>> infos;
    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->GetVisibilityWindowInfo(infos));
    sceneSessionManagerProxy_ = nullptr;
}
}
}
}
