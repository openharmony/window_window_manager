/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <ui/rs_surface_node.h>

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionLayoutTest::SetUpTestCase()
{
}

void SceneSessionLayoutTest::TearDownTestCase()
{
}

void SceneSessionLayoutTest::SetUp()
{
}

void SceneSessionLayoutTest::TearDown()
{
}

namespace {
/**
 * @tc.name: UpdateRect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, UpdateRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRect";
    info.bundleName_ = "UpdateRect";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();;
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);

    sceneSession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError result = sceneSession->UpdateRect(rect, reason, "SceneSessionLayoutTest");
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: NotifyClientToUpdateRect01
 * @tc.desc: NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, NotifyClientToUpdateRect01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRect01";
    info.bundleName_ = "NotifyClientToUpdateRect01";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = sptr<SessionStageMocker>::MakeSptr();
    ASSERT_NE(mockSessionStage, nullptr);
    sceneSession->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
    sceneSession->sessionStage_ = mockSessionStage;
    auto ret = sceneSession->NotifyClientToUpdateRect("SceneSessionLayoutTest", nullptr);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: UpdateRect1
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, UpdateRect1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRect1";
    info.bundleName_ = "UpdateRect1";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);

    sceneSession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError result = sceneSession->UpdateRect(rect, reason, "SceneSessionLayoutTest");
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->winRect_ = rect;
    result = sceneSession->UpdateRect(rect, reason, "SceneSessionLayoutTest");
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->reason_ = SizeChangeReason::DRAG_END;
    result = sceneSession->UpdateRect(rect, reason, "SceneSessionLayoutTest");
    ASSERT_EQ(result, WSError::WS_OK);

    WSRect rect2({0, 0, 0, 0});
    result = sceneSession->UpdateRect(rect2, reason, "SceneSessionLayoutTest");
    ASSERT_EQ(result, WSError::WS_OK);
}


/**
 * @tc.name: UpdateRectInner
 * @tc.desc: UpdateRectInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, UpdateRectInner01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRectInner01";
    info.bundleName_ = "UpdateRectInner01";
    info.screenId_ = 20;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    SessionUIParam uiParam;
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    sceneSession->SetForegroundInteractiveStatus(true);

    uiParam.needSync_ = true;
    uiParam.rect_ = {0, 0, 1, 1};

    sceneSession->winRect_ = {1, 1, 1, 1};
    sceneSession->isVisible_ = true;
    ASSERT_EQ(false, sceneSession->UpdateRectInner(uiParam, reason));
}

/**
 * @tc.name: UpdateRect01
 * @tc.desc: UpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, UpdateRect01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRect01";
    info.bundleName_ = "UpdateRect01";
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session->property_, nullptr);
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;

    WSRect rect = { 200, 200, 200, 200 };
    session->winRect_ = rect;
    session->SetClientRect(rect);
    EXPECT_EQ(session->UpdateRect(rect, reason, "SceneSessionLayoutTest"), WSError::WS_OK);

    rect.posX_ = 100;
    rect.posY_ = 100;
    rect.width_ = 800;
    rect.height_ = 800;
    session->winRect_ = rect;
    EXPECT_EQ(session->UpdateRect(rect, reason, "SceneSessionLayoutTest"), WSError::WS_OK);
}

/**
 * @tc.name: NotifyClientToUpdateRect
 * @tc.desc: NotifyClientToUpdateRect function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, NotifyClientToUpdateRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRect";
    info.bundleName_ = "NotifyClientToUpdateRect";
    info.isSystem_ = false;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->moveDragController_ = nullptr;
    session->isKeyboardPanelEnabled_ = false;
    session->reason_ = SizeChangeReason::UNDEFINED;
    session->Session::SetSessionState(SessionState::STATE_CONNECT);
    session->specificCallback_ = nullptr;
    session->reason_ = SizeChangeReason::DRAG;
    EXPECT_EQ(WSError::WS_OK, session->NotifyClientToUpdateRect("SceneSessionLayoutTest", nullptr));

    UpdateAvoidAreaCallback func = [](const int32_t& persistentId) {
        return;
    };
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    specificCallback->onUpdateAvoidArea_ = func;
    session->specificCallback_ = specificCallback;
    session->reason_ = SizeChangeReason::RECOVER;
    EXPECT_EQ(WSError::WS_OK, session->NotifyClientToUpdateRect("SceneSessionLayoutTest", nullptr));
}

/**
 * @tc.name: CheckAspectRatioValid
 * @tc.desc: CheckAspectRatioValid function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, CheckAspectRatioValid, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "CheckAspectRatioValid";
    info.bundleName_ = "CheckAspectRatioValid";
    info.isSystem_ = false;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    WindowLimits windowLimits;
    ASSERT_NE(session->GetSessionProperty(), nullptr);
    session->GetSessionProperty()->SetWindowLimits(windowLimits);

    SystemSessionConfig systemConfig;
    systemConfig.isSystemDecorEnable_ = false;
    session->SetSystemConfig(systemConfig);
    EXPECT_EQ(false, session->IsDecorEnable());

    windowLimits.minWidth_ = 0;
    windowLimits.minHeight_ = 0;
    EXPECT_EQ(WSError::WS_OK, session->SetAspectRatio(0.0f));

    windowLimits.minWidth_ = 1;
    windowLimits.maxHeight_ = 0;
    windowLimits.minHeight_ = 1;
    windowLimits.maxWidth_ = 0;
    EXPECT_EQ(WSError::WS_OK, session->SetAspectRatio(0.0f));

    windowLimits.maxHeight_ = 1;
    windowLimits.maxWidth_ = 1;
    EXPECT_EQ(WSError::WS_OK, session->SetAspectRatio(1.0f));

    windowLimits.maxHeight_ = 10000;
    windowLimits.minHeight_ = -10000;
    EXPECT_EQ(WSError::WS_OK, session->SetAspectRatio(0.0f));

    windowLimits.maxHeight_ = 10000;
    windowLimits.minHeight_ = -10000;
    EXPECT_EQ(WSError::WS_OK, session->SetAspectRatio(0.0f));

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(property, nullptr);
    WindowLimits limits = {8, 1, 6, 1, 1, 1.0f, 1.0f};
    property->SetWindowLimits(limits);
    session->SetSessionProperty(property);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, session->SetAspectRatio(0.1f));
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, session->SetAspectRatio(10.0f));
}

/**
 * @tc.name: CheckAspectRatioValid02
 * @tc.desc: CheckAspectRatioValid
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, CheckAspectRatioValid02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "CheckAspectRatioValid02";
    info.bundleName_ = "CheckAspectRatioValid02";
    info.isSystem_ = false;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    sceneSession->SetSessionProperty(property);

    WindowLimits windowLimits;
    ASSERT_NE(sceneSession->GetSessionProperty(), nullptr);
    sceneSession->GetSessionProperty()->SetWindowLimits(windowLimits);

    SystemSessionConfig systemConfig;
    systemConfig.isSystemDecorEnable_ = false;
    sceneSession->SetSystemConfig(systemConfig);
    EXPECT_EQ(false, sceneSession->IsDecorEnable());

    windowLimits.minWidth_ = 0;
    windowLimits.minHeight_ = 0;
    EXPECT_EQ(WSError::WS_OK, sceneSession->SetAspectRatio(0.0f));

    windowLimits.minWidth_ = 1;
    windowLimits.minHeight_ = 2;
    EXPECT_EQ(WSError::WS_OK, sceneSession->SetAspectRatio(0.0f));

    windowLimits.minWidth_ = 2;
    windowLimits.minHeight_ = 1;
    EXPECT_EQ(WSError::WS_OK, sceneSession->SetAspectRatio(0.0f));

    windowLimits.minWidth_ = 1;
    windowLimits.minHeight_ = 2;
    EXPECT_EQ(WSError::WS_OK, sceneSession->SetAspectRatio(1.0f));
}

/**
 * @tc.name: NotifyClientToUpdateRectTask
 * @tc.desc: NotifyClientToUpdateRectTask function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, NotifyClientToUpdateRectTask, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRectTask";
    info.bundleName_ = "NotifyClientToUpdateRectTask";
    info.isSystem_ = true;

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);

    session->moveDragController_ = nullptr;
    session->isKeyboardPanelEnabled_ = false;

    session->Session::UpdateSizeChangeReason(SizeChangeReason::UNDEFINED);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION,
        session->NotifyClientToUpdateRectTask("SceneSessionLayoutTest", nullptr));
    session->Session::UpdateSizeChangeReason(SizeChangeReason::MOVE);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION,
        session->NotifyClientToUpdateRectTask("SceneSessionLayoutTest", nullptr));
    session->Session::UpdateSizeChangeReason(SizeChangeReason::DRAG_MOVE);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION,
        session->NotifyClientToUpdateRectTask("SceneSessionLayoutTest", nullptr));
    session->Session::UpdateSizeChangeReason(SizeChangeReason::RESIZE);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION,
        session->NotifyClientToUpdateRectTask("SceneSessionLayoutTest", nullptr));
    session->Session::UpdateSizeChangeReason(SizeChangeReason::RECOVER);
    EXPECT_EQ(session->reason_, SizeChangeReason::RECOVER);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION,
        session->NotifyClientToUpdateRectTask("SceneSessionLayoutTest", nullptr));

    session->moveDragController_ = sptr<MoveDragController>::MakeSptr(2024, session->GetWindowType());
    session->moveDragController_->isStartDrag_ = true;
    session->moveDragController_->isStartMove_ = true;
    session->Session::UpdateSizeChangeReason(SizeChangeReason::MOVE);
    session->isKeyboardPanelEnabled_ = true;
    info.windowType_ = static_cast<uint32_t>(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION,
        session->NotifyClientToUpdateRectTask("SceneSessionLayoutTest", nullptr));
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION,
        session->NotifyClientToUpdateRectTask("SceneSessionLayoutTest", nullptr));

    session->Session::UpdateSizeChangeReason(SizeChangeReason::UNDEFINED);
    EXPECT_EQ(WSError::WS_ERROR_REPEAT_OPERATION,
        session->NotifyClientToUpdateRectTask("SceneSessionLayoutTest", nullptr));

    session->Session::UpdateSizeChangeReason(SizeChangeReason::MOVE);
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, 
        session->NotifyClientToUpdateRectTask("SceneSessionLayoutTest", nullptr));
    session->Session::UpdateSizeChangeReason(SizeChangeReason::DRAG_MOVE);
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION,
        session->NotifyClientToUpdateRectTask("SceneSessionLayoutTest", nullptr));
}

/**
 * @tc.name: HandleActionUpdateWindowLimits
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, HandleActionUpdateWindowLimits, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, sceneSession);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);

    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_ASPECT_RATIO;
    OHOS::Rosen::Session session(info);
    session.property_ = nullptr;
    sceneSession->HandleActionUpdateWindowLimits(property, action);
    session.property_ = sptr<WindowSessionProperty>::MakeSptr();
    sceneSession->HandleActionUpdateWindowLimits(property, action);
}

/**
 * @tc.name: SetAspectRatio1
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio1";
    info.bundleName_ = "SetAspectRatio1";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;
    sceneSession->property_ = nullptr;
    float ratio = 0.0001;
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}

/**
 * @tc.name: SetAspectRatio2
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio2, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio2";
    info.bundleName_ = "SetAspectRatio2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 0.0001;
    sceneSession->moveDragController_ = nullptr;
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(sceneSession->GetAspectRatio(), ratio);

    sceneSession->moveDragController_ = sptr<MoveDragController>::MakeSptr(0, sceneSession->GetWindowType());
    result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(sceneSession->GetAspectRatio(), ratio);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(sceneSession->GetAspectRatio(), ratio);
}

/**
 * @tc.name: SetAspectRatio3
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio3, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio3";
    info.bundleName_ = "SetAspectRatio3";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    WindowLimits limits;
    limits.maxHeight_ = 10;
    limits.minWidth_ = 1;
    limits.minHeight_ = 1;
    property->SetWindowLimits(limits);
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: SetAspectRatio4
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio4, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio4";
    info.bundleName_ = "SetAspectRatio4";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);

    float ratio = 0.1;
    WindowLimits limits;
    limits.maxHeight_ = 10;
    limits.minWidth_ = 10;
    property->SetWindowLimits(limits);
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: SetAspectRatio5
 * @tc.desc: test for aspectRatio NearZero
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio5, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio5";
    info.bundleName_ = "SetAspectRatio5";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 0.0001;
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(sceneSession->GetAspectRatio(), ratio);
}

/**
 * @tc.name: SetAspectRatio6
 * @tc.desc: test for aspectRatio is smaller than minWidth/maxHeight
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio6, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio6";
    info.bundleName_ = "SetAspectRatio6";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 2.5;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits;
    limits.maxWidth_ = 3000;
    limits.maxHeight_ = 3000;
    limits.minWidth_ = 2000;
    limits.minHeight_ = 2000;
    property->SetWindowLimits(limits);
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: SetAspectRatio7
 * @tc.desc: test for aspectRatio is smaller than minWidth/maxHeight
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio7, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio7";
    info.bundleName_ = "SetAspectRatio7";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits;
    limits.maxWidth_ = 3000;
    limits.maxHeight_ = 3000;
    limits.minWidth_ = 2000;
    limits.minHeight_ = 2000;
    property->SetWindowLimits(limits);
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: SetAspectRatio8
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio8, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio8";
    info.bundleName_ = "SetAspectRatio8";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits;
    limits.maxHeight_ = 10;
    limits.minWidth_ = 0;
    property->SetWindowLimits(limits);
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: SetAspectRatio9
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio9, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio9";
    info.bundleName_ = "SetAspectRatio9";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WindowLimits limits;
    limits.maxHeight_ = 10;
    limits.minWidth_ = 10;
    property->SetWindowLimits(limits);
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: SaveAspectRatio
 * @tc.desc: SaveAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, SaveAspectRatio, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession;
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    ASSERT_EQ(true, sceneSession->SaveAspectRatio(0.1));

    sceneSession->sessionInfo_.bundleName_ = "";
    sceneSession->sessionInfo_.moduleName_ = "";
    sceneSession->sessionInfo_.abilityName_ = "";
    ASSERT_EQ(false, sceneSession->SaveAspectRatio(0.1));
}

/**
 * @tc.name: AdjustRectByAspectRatio
 * @tc.desc: AdjustRectByAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, AdjustRectByAspectRatio, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession;
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    WSRect originalRect_ = { 0, 0, 0, 0 };
    ASSERT_EQ(false, sceneSession->AdjustRectByAspectRatio(originalRect_));
}

} // namespace
} // Rosen
} // OHOS