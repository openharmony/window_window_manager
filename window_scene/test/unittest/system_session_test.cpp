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
#include "session/host/include/system_session.h"

#include "common/include/session_permission.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "session/host/include/session.h"
#include <ui/rs_surface_node.h>
#include "window_event_channel_base.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "pointer_event.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SystemSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    SessionInfo info;
    sptr<SystemSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<SystemSession> systemSession_;
private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
};

void SystemSessionTest::SetUpTestCase()
{
}

void SystemSessionTest::TearDownTestCase()
{
}

void SystemSessionTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testSystemSession1";
    info.moduleName_ = "testSystemSession2";
    info.bundleName_ = "testSystemSession3";
    systemSession_ = new (std::nothrow) SystemSession(info, specificCallback);
    EXPECT_NE(nullptr, systemSession_);
}

void SystemSessionTest::TearDown()
{
    systemSession_ = nullptr;
}

RSSurfaceNode::SharedPtr SystemSessionTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "WindowSessionTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    return surfaceNode;
}

namespace {

/**
 * @tc.name: TransferKeyEvent01
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, TransferKeyEvent01, Function | SmallTest | Level1)
{
    systemSession_->state_ = SessionState::STATE_END;

    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, systemSession_->TransferKeyEvent(nullptr));
}

/**
 * @tc.name: TransferKeyEvent02
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, TransferKeyEvent02, Function | SmallTest | Level1)
{
    systemSession_->state_ = SessionState::STATE_CONNECT;
    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;

    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, systemSession_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: TransferKeyEvent03
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, TransferKeyEvent03, Function | SmallTest | Level1)
{
    systemSession_->state_ = SessionState::STATE_CONNECT;
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();

    sptr<WindowSessionProperty> windowSessionProperty = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(nullptr, windowSessionProperty);
    systemSession_->property_ = windowSessionProperty;
    EXPECT_NE(WSError::WS_OK, systemSession_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: CheckKeyEventDispatch01
 * @tc.desc: check func CheckKeyEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, CheckKeyEventDispatch01, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    systemSession_->isRSVisible_ = false;
    systemSession_->winRect_ = {1, 1, 1, 1};
    ASSERT_EQ(false, systemSession_->CheckKeyEventDispatch(keyEvent));

    systemSession_->isRSVisible_ = true;
    systemSession_->winRect_ = {1, 1, 1, 1};
    ASSERT_EQ(false, systemSession_->CheckKeyEventDispatch(keyEvent));

    systemSession_->winRect_ = {1, 1, 1, 0};
    ASSERT_EQ(false, systemSession_->CheckKeyEventDispatch(keyEvent));

    systemSession_->isRSVisible_ = true;
    systemSession_->winRect_ = {1, 1, 0, 0};
    ASSERT_EQ(false, systemSession_->CheckKeyEventDispatch(keyEvent));
}

/**
 * @tc.name: CheckKeyEventDispatch02
 * @tc.desc: check func CheckKeyEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, CheckKeyEventDispatch02, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent;
    systemSession_->isRSVisible_ = true;
    systemSession_->winRect_ = {1, 1, 0, 0};

    SessionInfo info;
    info.abilityName_ = "ParamSystemSession1";
    info.moduleName_ = "ParamSystemSession2";
    info.bundleName_ = "ParamSystemSession3";
    sptr<Session> session = new (std::nothrow) Session(info);
    systemSession_->parentSession_ = session;

    systemSession_->parentSession_->state_ = SessionState::STATE_CONNECT;
    systemSession_->state_ = SessionState::STATE_FOREGROUND;
    ASSERT_EQ(false, systemSession_->CheckKeyEventDispatch(keyEvent));

    systemSession_->state_ = SessionState::STATE_ACTIVE;
    ASSERT_EQ(false, systemSession_->CheckKeyEventDispatch(keyEvent));

    systemSession_->state_ = SessionState::STATE_CONNECT;
    systemSession_->parentSession_->state_ = SessionState::STATE_FOREGROUND;
    ASSERT_EQ(false, systemSession_->CheckKeyEventDispatch(keyEvent));

    systemSession_->parentSession_->state_ = SessionState::STATE_ACTIVE;
    ASSERT_EQ(false, systemSession_->CheckKeyEventDispatch(keyEvent));

    systemSession_->parentSession_->state_ = SessionState::STATE_CONNECT;
    systemSession_->state_ = SessionState::STATE_CONNECT;
    ASSERT_EQ(false, systemSession_->CheckKeyEventDispatch(keyEvent));

    systemSession_->parentSession_->state_ = SessionState::STATE_FOREGROUND;
    systemSession_->state_ = SessionState::STATE_FOREGROUND;
    ASSERT_EQ(false, systemSession_->CheckKeyEventDispatch(keyEvent));
}


/**
 * @tc.name: ProcessBackEvent01
 * @tc.desc: check func ProcessBackEvent
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, ProcessBackEvent01, Function | SmallTest | Level1)
{
    systemSession_->state_ = SessionState::STATE_END;

    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, systemSession_->ProcessBackEvent());
}

/**
 * @tc.name: NotifyClientToUpdateRect01
 * @tc.desc: check func NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, NotifyClientToUpdateRect01, Function | SmallTest | Level1)
{
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    systemSession_->sessionStage_ = mockSessionStage;
    auto ret = systemSession_->NotifyClientToUpdateRect(nullptr);
    ASSERT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: CheckPointerEventDispatch
 * @tc.desc: check func CheckPointerEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, CheckPointerEventDispatch, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent_ =  MMI::PointerEvent::Create();
    SessionInfo info;
    info.abilityName_ = "CheckPointerEventDispatch";
    info.bundleName_ = "CheckPointerEventDispatchBundleName";
    info.windowType_ = 2122;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    sptr<SystemSession> sysSession =
        new (std::nothrow) SystemSession(info, specificCallback_);
    sysSession->SetSessionState(SessionState::STATE_FOREGROUND);
    bool ret1 = sysSession->CheckPointerEventDispatch(pointerEvent_);
    ASSERT_EQ(true, ret1);
}

/**
 * @tc.name: UpdatePointerArea
 * @tc.desc: check func UpdatePointerArea
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, UpdatePointerArea, Function | SmallTest | Level1)
{
    WSRect rect = { 1, 1, 1, 1 };
    SessionInfo info;
    info.abilityName_ = "UpdatePointerArea";
    info.bundleName_ = "UpdatePointerAreaBundleName";
    info.windowType_ = 2122;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    sptr<SystemSession> sysSession =
        new (std::nothrow) SystemSession(info, specificCallback_);
    sysSession->UpdatePointerArea(rect);
    ASSERT_NE(sysSession->preRect_, rect);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetDecorEnable(true);
    sysSession->property_ = property;
    sysSession->UpdatePointerArea(rect);
    ASSERT_EQ(sysSession->preRect_, rect);
}

/**
 * @tc.name: ProcessPointDownSession
 * @tc.desc: test function : ProcessPointDownSession
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, ProcessPointDownSession, Function | SmallTest | Level1)
{
    ASSERT_TRUE(systemSession_ != nullptr);

    int32_t posX = 2;
    int32_t posY = 3;
    auto ret = systemSession_->ProcessPointDownSession(posX, posY);
    ASSERT_EQ(WSError::WS_OK, ret);

    sptr<WindowSessionProperty> windowSessionProperty = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(nullptr, windowSessionProperty);

    systemSession_->property_ = windowSessionProperty;
    ret = systemSession_->ProcessPointDownSession(posX, posY);
    ASSERT_EQ(WSError::WS_OK, ret);

    windowSessionProperty->raiseEnabled_ = false;
    ret = systemSession_->ProcessPointDownSession(posX, posY);
    ASSERT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: GetMissionId
 * @tc.desc: test function : GetMissionId
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, GetMissionId, Function | SmallTest | Level1)
{
    ASSERT_TRUE(systemSession_ != nullptr);
    SessionInfo info;
    info.abilityName_ = "testSystemSession1";
    info.moduleName_ = "testSystemSession2";
    info.bundleName_ = "testSystemSession3";
    sptr<Session> session = new (std::nothrow) Session(info);
    systemSession_->parentSession_ = session;
    auto ret = systemSession_->GetMissionId();
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: RectCheck
 * @tc.desc: test function : RectCheck
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, RectCheck, Function | SmallTest | Level1)
{
    ASSERT_TRUE(systemSession_ != nullptr);
    SessionInfo info;
    info.abilityName_ = "testRectCheck";
    info.moduleName_ = "testRectCheck";
    info.bundleName_ = "testRectCheck";
    sptr<Session> session = new (std::nothrow) Session(info);
    EXPECT_NE(nullptr, session);
    systemSession_->parentSession_ = session;
    uint32_t curWidth = 100;
    uint32_t curHeight = 200;
    systemSession_->RectCheck(curWidth, curHeight);
}

/**
 * @tc.name: SetDialogSessionBackGestureEnabled01
 * @tc.desc: test function : SetDialogSessionBackGestureEnabled
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, SetDialogSessionBackGestureEnabled01, Function | SmallTest | Level1)
{
    ASSERT_TRUE(systemSession_ != nullptr);
    SessionInfo info;
    info.abilityName_ = "SetDialogSessionBackGestureEnabled";
    info.moduleName_ = "SetDialogSessionBackGestureEnabled";
    info.bundleName_ = "SetDialogSessionBackGestureEnabled";
    sptr<Session> session = new (std::nothrow) Session(info);
    EXPECT_NE(nullptr, session);

    systemSession_->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    auto ret = systemSession_->SetDialogSessionBackGestureEnabled(true);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_CALLING);
}

/**
 * @tc.name: SetDialogSessionBackGestureEnabled02
 * @tc.desc: test function : SetDialogSessionBackGestureEnabled
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, SetDialogSessionBackGestureEnabled02, Function | SmallTest | Level1)
{
    ASSERT_TRUE(systemSession_ != nullptr);
    SessionInfo info;
    info.abilityName_ = "SetDialogSessionBackGestureEnabled02";
    info.moduleName_ = "SetDialogSessionBackGestureEnabled02";
    info.bundleName_ = "SetDialogSessionBackGestureEnabled02";
    sptr<Session> session = new (std::nothrow) Session(info);
    EXPECT_NE(nullptr, session);

    systemSession_->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    auto ret = systemSession_->SetDialogSessionBackGestureEnabled(true);
    ASSERT_EQ(ret, WSError::WS_OK);
}

} // namespace
} // namespace Rosen
} // namespace OHOS
