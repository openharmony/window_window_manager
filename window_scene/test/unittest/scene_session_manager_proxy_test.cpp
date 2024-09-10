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
#include "mock/mock_session.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
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
 * @tc.name: CreateAndConnectSpecificSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, CreateAndConnectSpecificSession, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<ISessionStage> sessionStage = new (std::nothrow) SessionStageMocker();
    sptr<IWindowEventChannel> eventChannel = new (std::nothrow) WindowEventChannelMocker(sessionStage);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> node = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    int32_t persistentId = 0;
    SessionInfo info;
    sptr<ISession> session = new (std::nothrow) SessionMocker(info);
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token = new (std::nothrow) IRemoteObjectMocker();

    sceneSessionManagerProxy_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property,
        persistentId, session, systemConfig, token);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: CreateAndConnectSpecificSession2
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, CreateAndConnectSpecificSession2, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<ISessionStage> sessionStage = new (std::nothrow) SessionStageMocker();
    sptr<IWindowEventChannel> eventChannel = new (std::nothrow) WindowEventChannelMocker(sessionStage);
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    int32_t persistentId = 0;
    SessionInfo info;
    sptr<ISession> session = new (std::nothrow) SessionMocker(info);
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token = new (std::nothrow) IRemoteObjectMocker();

    sceneSessionManagerProxy_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property,
        persistentId, session, systemConfig, token);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: CreateAndConnectSpecificSession3
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, CreateAndConnectSpecificSession3, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<ISessionStage> sessionStage = new (std::nothrow) SessionStageMocker();
    sptr<IWindowEventChannel> eventChannel = new (std::nothrow) WindowEventChannelMocker(sessionStage);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> node = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    sptr<WindowSessionProperty> property = nullptr;
    int32_t persistentId = 0;
    SessionInfo info;
    sptr<ISession> session = new (std::nothrow) SessionMocker(info);
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token = new (std::nothrow) IRemoteObjectMocker();

    sceneSessionManagerProxy_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property,
        persistentId, session, systemConfig, token);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: CreateAndConnectSpecificSession4
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, CreateAndConnectSpecificSession4, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<ISessionStage> sessionStage = new (std::nothrow) SessionStageMocker();
    sptr<IWindowEventChannel> eventChannel = new (std::nothrow) WindowEventChannelMocker(sessionStage);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> node = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    int32_t persistentId = 0;
    SessionInfo info;
    sptr<ISession> session = new (std::nothrow) SessionMocker(info);
    SystemSessionConfig systemConfig;
    sptr<IRemoteObject> token = nullptr;

    sceneSessionManagerProxy_->CreateAndConnectSpecificSession(sessionStage, eventChannel, node, property,
        persistentId, session, systemConfig, token);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: RecoverAndConnectSpecificSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RecoverAndConnectSpecificSession, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<ISessionStage> sessionStage = new (std::nothrow) SessionStageMocker();
    sptr<IWindowEventChannel> eventChannel = new (std::nothrow) WindowEventChannelMocker(sessionStage);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> node = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    SessionInfo info;
    sptr<ISession> session = new (std::nothrow) SessionMocker(info);
    sptr<IRemoteObject> token = new (std::nothrow) IRemoteObjectMocker();

    sceneSessionManagerProxy_->RecoverAndConnectSpecificSession(sessionStage, eventChannel, node, property,
        session, token);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: RecoverAndConnectSpecificSession2
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RecoverAndConnectSpecificSession2, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<ISessionStage> sessionStage = new (std::nothrow) SessionStageMocker();
    sptr<IWindowEventChannel> eventChannel = new (std::nothrow) WindowEventChannelMocker(sessionStage);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> node = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    sptr<WindowSessionProperty> property = nullptr;
    SessionInfo info;
    sptr<ISession> session = new (std::nothrow) SessionMocker(info);
    sptr<IRemoteObject> token = new (std::nothrow) IRemoteObjectMocker();

    sceneSessionManagerProxy_->RecoverAndConnectSpecificSession(sessionStage, eventChannel, node, property,
        session, token);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: RecoverAndConnectSpecificSession3
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RecoverAndConnectSpecificSession3, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<ISessionStage> sessionStage = new (std::nothrow) SessionStageMocker();
    sptr<IWindowEventChannel> eventChannel = new (std::nothrow) WindowEventChannelMocker(sessionStage);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> node = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    SessionInfo info;
    sptr<ISession> session = new (std::nothrow) SessionMocker(info);
    sptr<IRemoteObject> token = nullptr;

    sceneSessionManagerProxy_->RecoverAndConnectSpecificSession(sessionStage, eventChannel, node, property,
        session, token);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: RecoverAndReconnectSceneSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RecoverAndReconnectSceneSession, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<ISessionStage> sessionStage = new (std::nothrow) SessionStageMocker();
    sptr<IWindowEventChannel> eventChannel = new (std::nothrow) WindowEventChannelMocker(sessionStage);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> node = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    SessionInfo info;
    sptr<ISession> session = new (std::nothrow) SessionMocker(info);
    sptr<IRemoteObject> token = new (std::nothrow) IRemoteObjectMocker();

    sceneSessionManagerProxy_->RecoverAndReconnectSceneSession(sessionStage, eventChannel, node, session,
        property, token);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: RecoverAndReconnectSceneSession2
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RecoverAndReconnectSceneSession2, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<ISessionStage> sessionStage = new (std::nothrow) SessionStageMocker();
    sptr<IWindowEventChannel> eventChannel = new (std::nothrow) WindowEventChannelMocker(sessionStage);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> node = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    sptr<WindowSessionProperty> property = nullptr;
    SessionInfo info;
    sptr<ISession> session = new (std::nothrow) SessionMocker(info);
    sptr<IRemoteObject> token = new (std::nothrow) IRemoteObjectMocker();

    sceneSessionManagerProxy_->RecoverAndReconnectSceneSession(sessionStage, eventChannel, node, session,
        property, token);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: RecoverAndReconnectSceneSession3
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RecoverAndReconnectSceneSession3, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<ISessionStage> sessionStage = new (std::nothrow) SessionStageMocker();
    sptr<IWindowEventChannel> eventChannel = new (std::nothrow) WindowEventChannelMocker(sessionStage);
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    std::shared_ptr<RSSurfaceNode> node = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    SessionInfo info;
    sptr<ISession> session = new (std::nothrow) SessionMocker(info);
    sptr<IRemoteObject> token = nullptr;

    sceneSessionManagerProxy_->RecoverAndReconnectSceneSession(sessionStage, eventChannel, node, session,
        property, token);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, DestroyAndDisconnectSpecificSession, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sceneSessionManagerProxy_->DestroyAndDisconnectSpecificSession(0);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: DestroyAndDisconnectSpecificSessionWithDetachCallback
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(
    sceneSessionManagerProxyTest,
    DestroyAndDisconnectSpecificSessionWithDetachCallback,
    Function | SmallTest | Level2
)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<IRemoteObject> callback = new (std::nothrow) IRemoteObjectMocker();

    sceneSessionManagerProxy_->DestroyAndDisconnectSpecificSessionWithDetachCallback(0, callback);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: UpdateSessionTouchOutsideListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, UpdateSessionTouchOutsideListener, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    int32_t persistentId = 0;
    sceneSessionManagerProxy_->UpdateSessionTouchOutsideListener(persistentId, true);
    sceneSessionManagerProxy_->UpdateSessionTouchOutsideListener(persistentId, false);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: SetGestureNavigationEnabled
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, SetGestureNavigationEnabled, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sceneSessionManagerProxy_->SetGestureNavigationEnabled(true);
    sceneSessionManagerProxy_->SetGestureNavigationEnabled(false);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: GetFocusWindowInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetFocusWindowInfo, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    FocusChangeInfo focusInfo{};
    sceneSessionManagerProxy_->GetFocusWindowInfo(focusInfo);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: SetSessionIcon
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, SetSessionIcon, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<IRemoteObject> token = new (std::nothrow) IRemoteObjectMocker();
    std::shared_ptr<Media::PixelMap> icon = nullptr;
    sceneSessionManagerProxy_->SetSessionIcon(token, icon);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: GetAccessibilityWindowInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetAccessibilityWindowInfo, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    std::vector<sptr<AccessibilityWindowInfo>> infos{};
    sceneSessionManagerProxy_->GetAccessibilityWindowInfo(infos);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: GetUnreliableWindowInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetUnreliableWindowInfo, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    std::vector<sptr<UnreliableWindowInfo>> infos{};
    sceneSessionManagerProxy_->GetUnreliableWindowInfo(0, infos);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: GetSessionInfos
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetSessionInfos, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    std::string deviceId;
    std::vector<SessionInfoBean> sessionInfos{};
    sceneSessionManagerProxy_->GetSessionInfos(deviceId, 0, sessionInfos);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: GetSessionInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetSessionInfo, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    std::string deviceId;
    SessionInfoBean sessionInfo;
    sceneSessionManagerProxy_->GetSessionInfo(deviceId, 0, sessionInfo);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->RegisterWindowManagerAgent(type, windowManagerAgent));
    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->UnregisterWindowManagerAgent(type, windowManagerAgent));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: UpdateModalExtensionRect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, UpdateModalExtensionRect, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy =
        sptr<SceneSessionManagerProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sceneSessionManagerProxy, nullptr);

    sptr<IRemoteObject> token = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(token, nullptr);
    Rect rect { 1, 2, 3, 4 };
    sceneSessionManagerProxy->UpdateModalExtensionRect(token, rect);
    sceneSessionManagerProxy->UpdateModalExtensionRect(nullptr, rect);
    sceneSessionManagerProxy = nullptr;
}

/**
 * @tc.name: ProcessModalExtensionPointDown
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, ProcessModalExtensionPointDown, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy =
        sptr<SceneSessionManagerProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sceneSessionManagerProxy, nullptr);

    sptr<IRemoteObject> token = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(token, nullptr);
    sceneSessionManagerProxy->ProcessModalExtensionPointDown(token, 0, 0);
    sceneSessionManagerProxy->ProcessModalExtensionPointDown(nullptr, 0, 0);
    sceneSessionManagerProxy = nullptr;
}

/**
 * @tc.name: AddExtensionWindowStageToSCB
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, AddExtensionWindowStageToSCB, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy =
        sptr<SceneSessionManagerProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sceneSessionManagerProxy, nullptr);

    sptr<ISessionStage> sessionStage = sptr<SessionStageMocker>::MakeSptr();
    ASSERT_NE(sessionStage, nullptr);
    sptr<IRemoteObject> token = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(token, nullptr);
    sceneSessionManagerProxy->AddExtensionWindowStageToSCB(sessionStage, token, 12345);
    sceneSessionManagerProxy = nullptr;
}

/**
 * @tc.name: RemoveExtensionWindowStageFromSCB
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, RemoveExtensionWindowStageFromSCB, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy =
        sptr<SceneSessionManagerProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sceneSessionManagerProxy, nullptr);

    sptr<ISessionStage> sessionStage = sptr<SessionStageMocker>::MakeSptr();
    ASSERT_NE(sessionStage, nullptr);
    sptr<IRemoteObject> token = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(token, nullptr);
    sceneSessionManagerProxy->RemoveExtensionWindowStageFromSCB(sessionStage, token);
    sceneSessionManagerProxy = nullptr;
}

/**
 * @tc.name: AddOrRemoveSecureSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, AddOrRemoveSecureSession001, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    int32_t persistentId = 12345;
    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->AddOrRemoveSecureSession(persistentId, true));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: UpdateExtWindowFlags
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, UpdateExtWindowFlags, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<IRemoteObject> token = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(token, nullptr);
    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->UpdateExtWindowFlags(token, 7, 7));
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, sceneSessionManagerProxy_->UpdateExtWindowFlags(nullptr, 7, 7));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: GetSessionInfoByContinueSessionId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetSessionInfoByContinueSessionId, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->RaiseWindowToTop(persistendId));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: BindDialogSessionTarget
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, BindDialogSessionTarget, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<IRemoteObject> targetToken = new (std::nothrow) IRemoteObjectMocker();
    sceneSessionManagerProxy_->BindDialogSessionTarget(0, targetToken);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
}

/**
 * @tc.name: BindDialogSessionTarget2
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, BindDialogSessionTarget2, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    sptr<IRemoteObject> targetToken = nullptr;
    sceneSessionManagerProxy_->BindDialogSessionTarget(0, targetToken);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);
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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->IsValidSessionIds(sessionIds, results));
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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    ASSERT_EQ(WSError::WS_ERROR_INVALID_PARAM, sceneSessionManagerProxy_->UnRegisterSessionListener(listener));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: GetSnapshotByWindowId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetSnapshotByWindowId, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy = new SceneSessionManagerProxy(iRemoteObjectMocker);
    ASSERT_NE(sceneSessionManagerProxy, nullptr);
    int32_t windowId = -1;
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    WMError ret = sceneSessionManagerProxy->GetSnapshotByWindowId(windowId, pixelMap);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: GetSessionSnapshotById
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetSessionSnapshotById, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy = new SceneSessionManagerProxy(iRemoteObjectMocker);
    ASSERT_NE(sceneSessionManagerProxy, nullptr);
    int32_t windowId = -1;
    SessionSnapshot snapshot;
    WMError ret = sceneSessionManagerProxy->GetSessionSnapshotById(windowId, snapshot);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/*
 * @tc.name: TerminateSessionNew
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, TerminateSessionNew, Function | SmallTest | Level2)
{
    sptr<ISessionListener> listener = nullptr;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, sceneSessionManagerProxy_->TerminateSessionNew(nullptr, true, true));
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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->LockSession(0));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: GetUIContentRemoteObj
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetUIContentRemoteObj, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy = new SceneSessionManagerProxy(iRemoteObjectMocker);
    sptr<IRemoteObject> remoteObj;
    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, sceneSessionManagerProxy->GetUIContentRemoteObj(1, remoteObj));
}

/**
 * @tc.name: ClearSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, ClearSession, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    uint32_t topWinId = 1;
    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->GetTopWindowId(0, topWinId));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: GetWindowStyleType
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetWindowStyleType, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
            new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    WindowStyleType styleType;
    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->GetWindowStyleType(styleType));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: GetProcessSurfaceNodeIdByPersistentId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyTest, GetProcessSurfaceNodeIdByPersistentId, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
            new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    int32_t pid = 123;
    std::vector<int32_t> persistentIds = {1, 2, 3};
    std::vector<uint64_t> surfaceNodeIds;
    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->GetProcessSurfaceNodeIdByPersistentId(
        pid, persistentIds, surfaceNodeIds));
    sceneSessionManagerProxy_ = nullptr;
}

}  // namespace
}
}
