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
#include "mock/mock_session.h"
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
constexpr int WAIT_ASYNC_US = 1000000;
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
    sptr<SystemSession> GetSystemSession(const std::string& name);
    sptr<SceneSession> GetSceneSession(const std::string& name);
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

sptr<SystemSession> SystemSessionTest::GetSystemSession(const std::string& name)
{
    SessionInfo info;
    info.abilityName_ = name;
    info.moduleName_ = name;
    info.bundleName_ = name;
    auto sysSession = sptr<SystemSession>::MakeSptr(info, nullptr);
    return sysSession;
}

sptr<SceneSession> SystemSessionTest::GetSceneSession(const std::string& name)
{
    SessionInfo info;
    info.abilityName_ = name;
    info.moduleName_ = name;
    info.bundleName_ = name;
    auto sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    return sceneSession;
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

/**
 * @tc.name: UpdateCameraWindowStatus01
 * @tc.desc: test function : UpdateCameraWindowStatus
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, UpdateCameraWindowStatus01, Function | SmallTest | Level1)
{
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    ASSERT_NE(specificCallback, nullptr);
    bool result = false;

    systemSession_->UpdateCameraWindowStatus(true);
    ASSERT_EQ(result, false);

    systemSession_->specificCallback_ = specificCallback;
    systemSession_->UpdateCameraWindowStatus(true);
    ASSERT_EQ(result, false);

    systemSession_->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    systemSession_->UpdateCameraWindowStatus(true);
    ASSERT_EQ(result, false);

    systemSession_->specificCallback_->onCameraFloatSessionChange_ =
        [&result] (uint32_t accessTokenId, bool isShowing) {
            result = isShowing;
        };
    systemSession_->UpdateCameraWindowStatus(true);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: UpdateCameraWindowStatus02
 * @tc.desc: test function : UpdateCameraWindowStatus
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, UpdateCameraWindowStatus02, Function | SmallTest | Level1)
{
    auto sysSession = GetSystemSession("UpdateCameraWindowStatus02");
    ASSERT_NE(sysSession, nullptr);
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    ASSERT_NE(specificCallback, nullptr);
    bool result = false;
    sysSession->specificCallback_ = specificCallback;
    
    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    sysSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sysSession->UpdateCameraWindowStatus(true);
    ASSERT_EQ(result, false);

    sysSession->specificCallback_->onCameraSessionChange_ =
        [&result](uint32_t accessTokenId, bool isShowing) {
            result = isShowing;
        };

    result = false;
    sysSession->pipTemplateInfo_.pipTemplateType =
        static_cast<uint32_t>(PiPTemplateType::VIDEO_CALL);
    sysSession->UpdateCameraWindowStatus(true);
    ASSERT_EQ(result, true);

    result = false;
    sysSession->pipTemplateInfo_.pipTemplateType =
        static_cast<uint32_t>(PiPTemplateType::VIDEO_MEETING);
    sysSession->UpdateCameraWindowStatus(true);
    ASSERT_EQ(result, true);

    result = false;
    sysSession->pipTemplateInfo_.pipTemplateType =
        static_cast<uint32_t>(PiPTemplateType::VIDEO_LIVE);
    sysSession->UpdateCameraWindowStatus(true);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: Show01
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, Show01, Function | SmallTest | Level1)
{
    // for CheckPermissionWithPropertyAnimation;
    auto windowProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowProperty, nullptr);
    windowProperty->animationFlag_ = static_cast<uint32_t>(WindowAnimation::CUSTOM);

    // for TOAST or FLOAT permission
    systemSession_->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    systemSession_->Show(windowProperty);
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(systemSession_->property_->animationFlag_, static_cast<uint32_t>(WindowAnimation::CUSTOM));
}

/**
 * @tc.name: Hide01
 * @tc.desc: test function : Hide
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, Hide01, Function | SmallTest | Level1)
{
    auto sysSession = GetSystemSession("Hide01");
    ASSERT_NE(sysSession, nullptr);

    // for CheckPermissionWithPropertyAnimation;
    auto windowProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowProperty, nullptr);
    windowProperty->animationFlag_ = static_cast<uint32_t>(WindowAnimation::CUSTOM);

    sysSession->property_ = windowProperty;
    // for TOAST or FLOAT permission
    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_FLOAT);

    // for IsSessionValid
    sysSession->sessionInfo_.isSystem_ = false;
    sysSession->state_ = SessionState::STATE_CONNECT;
    sysSession->isActive_ = true;

    auto ret = sysSession->Hide();
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(ret, WSError::WS_OK);

    windowProperty->animationFlag_ = static_cast<uint32_t>(WindowAnimation::DEFAULT);
    ret = sysSession->Hide();
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(ret, WSError::WS_OK);

    ret = sysSession->Hide();
    sysSession->property_ = nullptr;
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: ProcessPointDownSession01
 * @tc.desc: test function : ProcessPointDownSession
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, ProcessPointDownSession01, Function | SmallTest | Level1)
{
    auto sysSession = GetSystemSession("ProcessPointDownSession01");
    ASSERT_NE(sysSession, nullptr);
    sysSession->persistentId_ = 1;
    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sysSession->state_ = SessionState::STATE_ACTIVE;

    auto parentSesssion = GetSceneSession("parentSession");
    ASSERT_NE(parentSesssion, nullptr);
    sysSession->parentSession_ = parentSesssion;

    parentSesssion->dialogVec_.push_back(sysSession);
    auto ret = sysSession->ProcessPointDownSession(0, 0);
    ASSERT_EQ(ret, WSError::WS_OK);

    // for IsTopDialog true
    auto topDialog = GetSystemSession("TopDialogSession");
    ASSERT_NE(topDialog, nullptr);
    topDialog->persistentId_ = 2;
    topDialog->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    topDialog->state_ = SessionState::STATE_ACTIVE;
    parentSesssion->dialogVec_.push_back(topDialog);

    ret = sysSession->ProcessPointDownSession(0, 0);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: ProcessPointDownSession02
 * @tc.desc: test function : ProcessPointDownSession
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, ProcessPointDownSession02, Function | SmallTest | Level1)
{
    auto sysSession = GetSystemSession("ProcessPointDownSession02");
    ASSERT_NE(sysSession, nullptr);
    sysSession->persistentId_ = 1;
    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sysSession->state_ = SessionState::STATE_ACTIVE;

    sysSession->property_->raiseEnabled_ = false;
    auto ret = sysSession->ProcessPointDownSession(0, 0);
    ASSERT_EQ(ret, WSError::WS_OK);

    sysSession->property_->raiseEnabled_ = true;
    ret = sysSession->ProcessPointDownSession(0, 0);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: TransferKeyEvent04
 * @tc.desc: test function : TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, TransferKeyEvent04, Function | SmallTest | Level1)
{
    auto sysSession = GetSystemSession("TransferKeyEvent04");
    ASSERT_NE(sysSession, nullptr);
    sysSession->persistentId_ = 1;
    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sysSession->state_ = SessionState::STATE_ACTIVE;

    std::shared_ptr<MMI::KeyEvent> keyEvent;

    auto ret = sysSession->TransferKeyEvent(nullptr);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);

    ret = sysSession->TransferKeyEvent(keyEvent);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: TransferKeyEvent05
 * @tc.desc: test function : TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, TransferKeyEvent05, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();

    auto sysSession = GetSystemSession("TransferKeyEvent05");
    ASSERT_NE(sysSession, nullptr);
    sysSession->persistentId_ = 1;
    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sysSession->state_ = SessionState::STATE_ACTIVE;

    auto ret = sysSession->TransferKeyEvent(keyEvent);
    ASSERT_EQ(ret, WSError::WS_DO_NOTHING);

    auto parentSesssion = GetSceneSession("parentSession");
    ASSERT_NE(parentSesssion, nullptr);
    sysSession->parentSession_ = parentSesssion;

    // for CheckKeyEventDispatch return true;
    parentSesssion->state_ = SessionState::STATE_FOREGROUND;
    ret = sysSession->TransferKeyEvent(keyEvent);
    ASSERT_EQ(ret, WSError::WS_DO_NOTHING);

    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_BACK);
    ret = sysSession->TransferKeyEvent(keyEvent);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_PERMISSION);

    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_HOME);
    parentSesssion->dialogVec_.push_back(sysSession);
    ret = sysSession->TransferKeyEvent(keyEvent);
    ASSERT_EQ(ret, WSError::WS_DO_NOTHING);

    auto topDialog = GetSystemSession("TopDialogSession");
    ASSERT_NE(topDialog, nullptr);
    topDialog->persistentId_ = 2;
    topDialog->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    topDialog->state_ = SessionState::STATE_ACTIVE;
    parentSesssion->dialogVec_.push_back(topDialog);
    ret = sysSession->TransferKeyEvent(keyEvent);
    ASSERT_EQ(ret, WSError::WS_ERROR_INVALID_PERMISSION);
}

/**
 * @tc.name: ProcessBackEvent02
 * @tc.desc: test function : ProcessBackEvent
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, ProcessBackEvent02, Function | SmallTest | Level1)
{
    auto sysSession = GetSystemSession("ProcessBackEvent02");
    ASSERT_NE(sysSession, nullptr);
    sysSession->persistentId_ = 1;
    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sysSession->state_ = SessionState::STATE_ACTIVE;

    sysSession->dialogSessionBackGestureEnabled_ = false;
    auto ret = sysSession->ProcessBackEvent();
    ASSERT_EQ(ret, WSError::WS_OK);

    sysSession->dialogSessionBackGestureEnabled_ = true;
    ret = sysSession->ProcessBackEvent();
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);

    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ret = sysSession->ProcessBackEvent();
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: CheckKeyEventDispatch03
 * @tc.desc: test function : CheckKeyEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, CheckKeyEventDispatch03, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    auto sysSession = GetSystemSession("CheckKeyEventDispatch03");
    ASSERT_NE(sysSession, nullptr);
    sysSession->persistentId_ = 1;
    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    sysSession->state_ = SessionState::STATE_ACTIVE;
    sysSession->isRSVisible_ = false;

    auto ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);

    sysSession->isRSVisible_ = true;
    sysSession->winRect_.width_ = 0;
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);

    sysSession->isRSVisible_ = true;
    sysSession->winRect_.width_ = 1;
    sysSession->winRect_.height_ = 0;
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);

    sysSession->isRSVisible_ = true;
    sysSession->winRect_.width_ = 1;
    sysSession->winRect_.height_ = 1;
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: CheckKeyEventDispatch04
 * @tc.desc: test function : CheckKeyEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, CheckKeyEventDispatch04, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    auto sysSession = GetSystemSession("CheckKeyEventDispatch04");
    ASSERT_NE(sysSession, nullptr);
    sysSession->persistentId_ = 1;
    sysSession->isRSVisible_ = true;
    sysSession->winRect_.width_ = 1;
    sysSession->winRect_.height_ = 1;
    sysSession->state_ = SessionState::STATE_DISCONNECT;

    auto parentSesssion = GetSceneSession("parentSession");
    ASSERT_NE(parentSesssion, nullptr);
    sysSession->parentSession_ = parentSesssion;

    parentSesssion->state_ = SessionState::STATE_FOREGROUND;
    auto ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);

    parentSesssion->state_ = SessionState::STATE_ACTIVE;
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);

    parentSesssion->state_ = SessionState::STATE_DISCONNECT;
    sysSession->state_ = SessionState::STATE_FOREGROUND;
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);

    sysSession->state_ = SessionState::STATE_ACTIVE;
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);

    parentSesssion->state_ = SessionState::STATE_DISCONNECT;
    sysSession->state_ = SessionState::STATE_DISCONNECT;
    ret = sysSession->CheckKeyEventDispatch(keyEvent);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: NotifyClientToUpdateRect02
 * @tc.desc: test function : NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, NotifyClientToUpdateRect02, Function | SmallTest | Level1)
{
    auto sysSession = GetSystemSession("NotifyClientToUpdateRect02");
    ASSERT_NE(sysSession, nullptr);
    sysSession->persistentId_ = 1;
    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    sysSession->state_ = SessionState::STATE_ACTIVE;

    //for NotifyClientToUpdateRectTask
    sysSession->isKeyboardPanelEnabled_ = true;

    sysSession->dirtyFlags_ = 0;
    sysSession->reason_ = SizeChangeReason::MAXIMIZE;
    sysSession->NotifyClientToUpdateRect(nullptr);
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(sysSession->reason_, SizeChangeReason::UNDEFINED);

    sysSession->reason_ = SizeChangeReason::DRAG;
    sysSession->NotifyClientToUpdateRect(nullptr);
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(sysSession->reason_, SizeChangeReason::DRAG);
}

/**
 * @tc.name: NotifyClientToUpdateRect03
 * @tc.desc: test function : NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, NotifyClientToUpdateRect03, Function | SmallTest | Level1)
{
    auto sysSession = GetSystemSession("NotifyClientToUpdateRect03");
    ASSERT_NE(sysSession, nullptr);
    sysSession->persistentId_ = 1;
    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    sysSession->state_ = SessionState::STATE_ACTIVE;

    sysSession->isKeyboardPanelEnabled_ = true;
    sysSession->reason_ = SizeChangeReason::MAXIMIZE;

    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    ASSERT_NE(specificCallback, nullptr);
    sysSession->specificCallback_ = specificCallback;
    sysSession->NotifyClientToUpdateRect(nullptr);
    usleep(WAIT_ASYNC_US);
    EXPECT_EQ(sysSession->reason_, SizeChangeReason::UNDEFINED);

    sysSession->reason_ = SizeChangeReason::MAXIMIZE;
    sysSession->specificCallback_->onUpdateAvoidArea_ = [](const int32_t& persistentId) {};
    sysSession->NotifyClientToUpdateRect(nullptr);
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(sysSession->reason_, SizeChangeReason::UNDEFINED);
}

/**
 * @tc.name: IsVisibleForeground01
 * @tc.desc: test function : IsVisibleForeground
 * @tc.type: FUNC
 */
HWTEST_F(SystemSessionTest, IsVisibleForeground01, Function | SmallTest | Level1)
{
    auto sysSession = GetSystemSession("IsVisibleForeground01");
    ASSERT_NE(sysSession, nullptr);
    sysSession->persistentId_ = 1;
    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    sysSession->state_ = SessionState::STATE_ACTIVE;
    sysSession->isVisible_ = false;

    auto ret = sysSession->IsVisibleForeground();
    ASSERT_EQ(ret, false);

    sysSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ret = sysSession->IsVisibleForeground();
    ASSERT_EQ(ret, false);

    auto parentSesssion = GetSceneSession("parentSession");
    ASSERT_NE(parentSesssion, nullptr);
    sysSession->parentSession_ = parentSesssion;
    parentSesssion->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ret = sysSession->IsVisibleForeground();
    ASSERT_EQ(ret, false);

    parentSesssion->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ret = sysSession->IsVisibleForeground();
    ASSERT_EQ(ret, false);
}

} // namespace
} // namespace Rosen
} // namespace OHOS
