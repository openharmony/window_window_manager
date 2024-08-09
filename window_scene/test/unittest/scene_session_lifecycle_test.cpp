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

#include "window_helper.h"
#include "display_manager.h"
#include "pointer_event.h"

#include <gtest/gtest.h>
#include "key_event.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/system_session.h"
#include "session/host/include/main_session.h"
#include "wm_common.h"
#include "mock/mock_session_stage.h"
#include "input_event.h"
#include <pointer_event.h>
#include "ui/rs_surface_node.h"
#include "session/container/include/window_event_channel.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionLifecycleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr <SceneSession> sceneSession;
    SessionInfo info;
};

void SceneSessionLifecycleTest::SetUpTestCase()
{
}

void SceneSessionLifecycleTest::TearDownTestCase()
{
}

void SceneSessionLifecycleTest::SetUp()
{
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
}

void SceneSessionLifecycleTest::TearDown()
{
}

namespace {

/**
 * @tc.name: Foreground01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Foreground01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    int resultValue = 0;
    sptr<SceneSession> sceneSession;

    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    auto result = sceneSession->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);
    specificCallback->onCreate_ = [&resultValue, specificCallback](const SessionInfo &info,
        sptr<WindowSessionProperty> property) -> sptr<SceneSession>
    {
        sptr<SceneSession> scensessionreturn = new (std::nothrow) SceneSession(info, specificCallback);
        EXPECT_NE(scensessionreturn, nullptr);
        resultValue = 1;
        return scensessionreturn;
    };
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->UpdateSessionState(SessionState::STATE_DISCONNECT);
    sceneSession->isActive_ = true;
    result = sceneSession->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Foreground02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Foreground02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Foreground02";
    info.bundleName_ = "Foreground02";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession;

    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;
    sptr<WindowSessionProperty> property = nullptr;
    auto result = sceneSession->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: ForegroundAndBackground03
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Foreground03, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Foreground03";
    info.bundleName_ = "Foreground03";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession;

    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    property->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
    auto result = sceneSession->Foreground(property);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Foreground04
 * @tc.desc: Foreground04 function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Foreground04, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Foreground04";
    info.bundleName_ = "Foreground04";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    sptr<WindowSessionProperty> property = nullptr;
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));

    session->property_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));

    info.windowType_ = static_cast<uint32_t>(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    sptr<SceneSession> session1 = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_EQ(WSError::WS_OK, session1->Foreground(property, true));
    EXPECT_EQ(WSError::WS_OK, session1->Foreground(property, false));
}

/**
 * @tc.name: Foreground05
 * @tc.desc: Foreground05 function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Foreground05, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Foreground05";
    info.bundleName_ = "Foreground05";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    sptr<WindowSessionProperty> property = nullptr;
    session->Session::SetSessionState(SessionState::STATE_CONNECT);
    session->Session::isActive_ = true;
    session->SetLeashWinSurfaceNode(nullptr);
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));

    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    session->specificCallback_ = specificCallback;
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));

    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->SetLeashWinSurfaceNode(surfaceNode);
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));

    session->SetSessionProperty(property);
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));
}

/**
 * @tc.name: Foreground06
 * @tc.desc: Foreground06 function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Foreground06, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Foreground06";
    info.bundleName_ = "Foreground06";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    sptr<WindowSessionProperty> property = nullptr;
    session->property_ = property;
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));

    session->SetLeashWinSurfaceNode(nullptr);
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));

    sptr<WindowSessionProperty> property2 = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property2, nullptr);
    property2->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property2, false));

    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->SetLeashWinSurfaceNode(surfaceNode);
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property2, false));

    sptr<WindowSessionProperty> property3 = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property3, nullptr);
    property3->SetWindowType(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    session->SetSessionProperty(property3);
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));
}

/**
 * @tc.name: Background01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Background01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "Background01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    int resultValue = 0;
    sptr<SceneSession> sceneSession;

    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;
    auto result = sceneSession->Background();
    ASSERT_EQ(result, WSError::WS_OK);
    specificCallback->onCreate_ = [&resultValue, specificCallback](const SessionInfo &info,
        sptr<WindowSessionProperty> property) -> sptr<SceneSession>
    {
        sptr<SceneSession> scensessionreturn = new (std::nothrow) SceneSession(info, specificCallback);
        EXPECT_NE(scensessionreturn, nullptr);
        resultValue = 1;
        return scensessionreturn;
    };
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->UpdateSessionState(SessionState::STATE_CONNECT);
    sceneSession->isActive_ = true;
    result = sceneSession->Background();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Background02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Background02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background02";
    info.bundleName_ = "Background02";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    property->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
    sceneSession->SetSessionProperty(property);
    sceneSession->isActive_ = true;
    auto result = sceneSession->Background();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Background3
 * @tc.desc:  * @tc.name: Background
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Background3, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background3";
    info.bundleName_ = "Background3";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, sceneSession);

    sceneSession->specificCallback_ = new (std::nothrow) SceneSession::SpecificSessionCallback();
    WindowType windowType = WindowType::APP_MAIN_WINDOW_BASE;
    OHOS::Rosen::WindowHelper::IsMainWindow(windowType);
}

/**
 * @tc.name: Background04
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Background04, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background04";
    info.bundleName_ = "Background04";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, sceneSession);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
    sceneSession->SetSessionProperty(property);
    EXPECT_EQ(WSError::WS_OK, sceneSession->Background(true));

    sptr<WindowSessionProperty> property2 = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property2, nullptr);
    property2->SetWindowType(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    sceneSession->SetSessionProperty(property2);
    EXPECT_EQ(WSError::WS_OK, sceneSession->Background(false));
    EXPECT_EQ(WSError::WS_OK, sceneSession->Background(true));
}

/**
 * @tc.name: BackgroundTask01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, BackgroundTask01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "BackgroundTask01";
    info.bundleName_ = "BackgroundTask01";
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    int resultValue = 0;
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;
    auto result = sceneSession->BackgroundTask();
    ASSERT_EQ(result, WSError::WS_OK);
    sceneSession->isActive_ = true;
    result = sceneSession->BackgroundTask(false);
    ASSERT_EQ(result, WSError::WS_OK);
    specificCallback->onCreate_ =
    [&resultValue, specificCallback](const SessionInfo& info,
                                     sptr<WindowSessionProperty> property) -> sptr<SceneSession>
    {
        sptr<SceneSession> sceneSessionReturn = new (std::nothrow) SceneSession(info, specificCallback);
        EXPECT_NE(sceneSessionReturn, nullptr);
        resultValue = 1;
        return sceneSessionReturn;
    };
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->UpdateSessionState(SessionState::STATE_CONNECT);
    sceneSession->isActive_ = true;
    result = sceneSession->BackgroundTask();
    ASSERT_EQ(result, WSError::WS_OK);
    sceneSession->UpdateSessionState(SessionState::STATE_CONNECT);
    sceneSession->isActive_ = true;
    result = sceneSession->BackgroundTask(false);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: BackgroundTask02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, BackgroundTask02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "BackgroundTask02";
    info.bundleName_ = "BackgroundTask02";
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    property->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
    sceneSession->SetSessionProperty(property);
    sceneSession->isActive_ = true;
    auto result = sceneSession->BackgroundTask();
    ASSERT_EQ(result, WSError::WS_OK);
    sceneSession->isActive_ = true;
    result = sceneSession->BackgroundTask(false);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: BackgroundTask03
 * @tc.desc: BackgroundTask03 function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, BackgroundTask03, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "BackgroundTask03";
    info.bundleName_ = "BackgroundTask03";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    sptr<WindowSessionProperty> property = nullptr;
    session->SetSessionState(SessionState::STATE_BACKGROUND);
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(false));

    session->SetSessionState(SessionState::STATE_CONNECT);
    session->Session::SetSessionState(SessionState::STATE_CONNECT);
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(false));

    session->Session::SetSessionState(SessionState::STATE_INACTIVE);
    info.windowType_ = static_cast<uint32_t>(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(false));

    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    session->specificCallback_ = specificCallback;
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(false));

    session->scenePersistence_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(true));

    session->scenePersistence_ = sptr<ScenePersistence>::MakeSptr("Foreground02", 1);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->surfaceNode_ = surfaceNode;
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(true));

    sptr<WindowSessionProperty> property2 = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property2, nullptr);
    property2->SetWindowType(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    session->SetSessionProperty(property2);
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(false));
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(true));
}

/**
 * @tc.name: DisconnectTask01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, DisconnectTask01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "DisconnectTask01";
    info.bundleName_ = "DisconnectTask01";
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCallback, nullptr);

    int resultValue = 0;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;
    auto result = sceneSession->DisconnectTask(true, true);
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_EQ(result, WSError::WS_OK);
    sceneSession->isActive_ = true;
    result = sceneSession->DisconnectTask(false, true);
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_EQ(result, WSError::WS_OK);
    specificCallback->onCreate_ =
    [&resultValue, specificCallback](const SessionInfo& info,
                                     sptr<WindowSessionProperty> property) -> sptr<SceneSession> {
        sptr<SceneSession> sceneSessionReturn = new (std::nothrow) SceneSession(info, specificCallback);
        EXPECT_NE(sceneSessionReturn, nullptr);
        resultValue = 1;
        return sceneSessionReturn;
    };
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->UpdateSessionState(SessionState::STATE_CONNECT);
    sceneSession->isActive_ = true;
    result = sceneSession->DisconnectTask(true, true);
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_EQ(result, WSError::WS_OK);
    sceneSession->UpdateSessionState(SessionState::STATE_CONNECT);
    sceneSession->isActive_ = true;
    result = sceneSession->DisconnectTask(false, true);
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: DisconnectTask02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, DisconnectTask02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "DisconnectTask02";
    info.bundleName_ = "DisconnectTask02";
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(property, nullptr);
    property->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::CUSTOM));
    sceneSession->SetSessionProperty(property);
    sceneSession->isActive_ = true;
    auto result = sceneSession->DisconnectTask(true, true);
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_EQ(result, WSError::WS_OK);
    sceneSession->isActive_ = true;
    result = sceneSession->DisconnectTask(false, true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Disconnect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Disconnect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Disconnect";
    info.bundleName_ = "Disconnect";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    sceneSession->isActive_ = true;
    auto result = sceneSession->Disconnect();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Disconnect2
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Disconnect2, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Disconnect2";
    info.bundleName_ = "Disconnect2";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    sceneSession->isActive_ = true;

    auto result = sceneSession->Disconnect(true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Disconnect3
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Disconnect3, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Disconnect3";
    info.bundleName_ = "Disconnect3";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    sceneSession->isActive_ = true;

    auto result = sceneSession->Disconnect(true);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->specificCallback_ = nullptr;
    result = sceneSession->Disconnect(false);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->needSnapshot_ = true;
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);

    result = sceneSession->Disconnect(false);
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->DisconnectTask(false, false);
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->DisconnectTask(false, true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateActiveStatus01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, UpdateActiveStatus01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateActiveStatus01";
    info.bundleName_ = "UpdateActiveStatus01";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    sceneSession->isActive_ = true;
    auto result = sceneSession->UpdateActiveStatus(true);
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->UpdateActiveStatus(false);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateActiveStatus02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, UpdateActiveStatus02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateActiveStatus02";
    info.bundleName_ = "UpdateActiveStatus02";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    sceneSession->isActive_ = false;
    auto result = sceneSession->UpdateActiveStatus(true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateActiveStatus03
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, UpdateActiveStatus03, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateActiveStatus03";
    info.bundleName_ = "UpdateActiveStatus03";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    sceneSession->isActive_ = true;
    sceneSession->state_ = SessionState::STATE_DISCONNECT;

    auto result = sceneSession->UpdateActiveStatus(false);
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->UpdateActiveStatus(true);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->isActive_ = false;
    result = sceneSession->UpdateActiveStatus(false);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->isActive_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    result = sceneSession->UpdateActiveStatus(true);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->isActive_ = true;
    sceneSession->state_ = SessionState::STATE_ACTIVE;
    result = sceneSession->UpdateActiveStatus(false);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->isActive_ = false;
    result = sceneSession->UpdateActiveStatus(true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateActiveStatus04
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, UpdateActiveStatus04, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateActiveStatus04";
    info.bundleName_ = "UpdateActiveStatus04";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    sceneSession->isActive_ = true;
    sceneSession->state_ = SessionState::STATE_ACTIVE;

    auto result = sceneSession->UpdateActiveStatus(true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Connect
 * @tc.desc: Connect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Connect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.bundleName_ = "Connect";
    info.abilityName_ = "Connect1";
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<ISessionStage> sessionStage = nullptr;
    sptr<IWindowEventChannel> eventChannel = nullptr;
    std::shared_ptr<RSSurfaceNode> surfaceNode = nullptr;
    SystemSessionConfig systemConfig;
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    sptr<IRemoteObject> token;
    WSError res = sceneSession->Connect(sessionStage, eventChannel,
                                       surfaceNode, systemConfig, property, token);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: ConnectInner01
 * @tc.desc: ConnectInner01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, ConnectInner01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.bundleName_ = "ConnectInner01";
    info.abilityName_ = "ConnectInner01";
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    SystemSessionConfig systemConfig;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sceneSession->clientIdentityToken_ = "session1";

    auto result = sceneSession->ConnectInner(mockSessionStage, nullptr, nullptr, systemConfig,
        property, nullptr, -1, -1, "session2");
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->ConnectInner(mockSessionStage, nullptr, nullptr, systemConfig,
        property, nullptr, -1, -1, "session1");
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    result = sceneSession->ConnectInner(mockSessionStage, nullptr, nullptr, systemConfig,
        property, nullptr, -1, -1);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: ConnectInner02
 * @tc.desc: ConnectInner02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, ConnectInner02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.bundleName_ = "ConnectInner02";
    info.abilityName_ = "ConnectInner02";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    SystemSessionConfig systemConfig;
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(property, nullptr);
    sceneSession->SetSessionState(SessionState::STATE_CONNECT);
    sceneSession->Session::isTerminating_ = false;
    auto result = sceneSession->ConnectInner(mockSessionStage, nullptr, nullptr, systemConfig,
        property, nullptr);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_SESSION);

    sptr<IWindowEventChannel> eventChannel = new WindowEventChannel(mockSessionStage);
    ASSERT_NE(eventChannel, nullptr);
    sceneSession->SetSessionState(SessionState::STATE_DISCONNECT);
    result = sceneSession->ConnectInner(mockSessionStage, eventChannel, nullptr, systemConfig,
        property, nullptr);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Reconnect
 * @tc.desc: Reconnect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, Reconnect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.bundleName_ = "Reconnect";
    info.abilityName_ = "Reconnect1";
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<ISessionStage> sessionStage = nullptr;
    sptr<IWindowEventChannel> eventChannel = nullptr;
    std::shared_ptr<RSSurfaceNode> surfaceNode = nullptr;
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    sptr<IRemoteObject> token;
    int32_t pid = -1;
    int32_t uid = -1;
    WSError res =
            sceneSession->Reconnect(sessionStage, eventChannel, surfaceNode, property, token, pid, uid);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: PendingSessionActivation
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, PendingSessionActivation, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "PendingSessionActivation";
    info.bundleName_ = "PendingSessionActivation";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    uint32_t p = 10;
    property->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, p);
    sceneSession->SetSessionProperty(property);

    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();

    sptr<AAFwk::SessionInfo> info1 = nullptr;
    WSError result = sceneSession->PendingSessionActivation(info1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);

    result = sceneSession->PendingSessionActivation(abilitySessionInfo);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: TerminateSession
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, TerminateSession, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "TerminateSession";
    info.bundleName_ = "TerminateSession";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    uint32_t p = 10;
    property->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, p);
    sceneSession->SetSessionProperty(property);

    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();

    sptr<AAFwk::SessionInfo> info1 = nullptr;
    WSError result = sceneSession->TerminateSession(info1);
    ASSERT_EQ(result, WSError::WS_OK);

    result = sceneSession->TerminateSession(abilitySessionInfo);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: TerminateSession01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, TerminateSession01, Function | SmallTest | Level2)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();
    ASSERT_NE(nullptr, abilitySessionInfo);
    OHOS::Rosen::Session session(info);
    session.isTerminating_ = true;
    sceneSession->TerminateSession(abilitySessionInfo);
}

/**
 * @tc.name: NotifySessionException
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, NotifySessionException, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifySessionException";
    info.bundleName_ = "NotifySessionException";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    uint32_t p = 10;
    property->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, p);
    sceneSession->SetSessionProperty(property);

    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();

    sptr<AAFwk::SessionInfo> info1 = nullptr;
    WSError result = sceneSession->NotifySessionException(info1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);

    result = sceneSession->NotifySessionException(abilitySessionInfo);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: NotifySessionException01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, NotifySessionException01, Function | SmallTest | Level2)
{
    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();
    ASSERT_NE(nullptr, abilitySessionInfo);
    bool needRemoveSession = true;
    OHOS::Rosen::Session session(info);
    session.isTerminating_ = true;
    sceneSession->NotifySessionException(abilitySessionInfo, needRemoveSession);
    sceneSession->GetLastSafeRect();
    WSRect rect;
    sceneSession->SetLastSafeRect(rect);
}

/**
 * @tc.name: NotifySessionForeground01
 * @tc.desc: NotifySessionForeground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, NotifySessionForeground, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    uint32_t reason = 1;
    bool withAnimation = true;
    int ret = 1;

    sceneSession->sessionStage_ = mockSessionStage;
    sceneSession->NotifySessionForeground(reason, withAnimation);
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: NotifySessionFullScreen01
 * @tc.desc: NotifySessionFullScreen
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, NotifySessionFullScreen, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "FullScreen01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    bool fullScreen = true;
    int ret = 1;
    sceneSession->sessionStage_ = mockSessionStage;
    sceneSession->NotifySessionFullScreen(fullScreen);
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: NotifySessionBackground01
 * @tc.desc: NotifySessionBackground
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLifecycleTest, NotifySessionBackground, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
            new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    uint32_t reason = 1;
    bool withAnimation = true;
    bool isFromInnerkits = true;
    int ret = 1;

    sceneSession->sessionStage_ = mockSessionStage;
    sceneSession->NotifySessionBackground(reason, withAnimation, isFromInnerkits);
    ASSERT_EQ(ret, 1);
}
}
}
}