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

#include <gtest/gtest.h>
#include <pointer_event.h>
#include <ui/rs_surface_node.h>

#include "display_manager.h"
#include "input_event.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "pointer_event.h"
#include "session/host/include/main_session.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/system_session.h"
#include "window_helper.h"
#include "wm_common.h"

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

private:
    sptr<SessionStageMocker> mockSessionStage_ = nullptr;
};

void SceneSessionLayoutTest::SetUpTestCase() {}

void SceneSessionLayoutTest::TearDownTestCase() {}

void SceneSessionLayoutTest::SetUp()
{
    mockSessionStage_ = sptr<SessionStageMocker>::MakeSptr();
}

void SceneSessionLayoutTest::TearDown() {}

namespace {
/**
 * @tc.name: UpdateRect01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, UpdateRect01, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRect01";
    info.bundleName_ = "UpdateRect01";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);

    sceneSession->SetSessionProperty(property);
    WSRect rect({ 1, 1, 1, 1 });
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError result = sceneSession->UpdateRect(rect, reason, "SceneSessionLayoutTest");
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateRect02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, UpdateRect02, TestSize.Level0)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRect02";
    info.bundleName_ = "UpdateRect02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);

    sceneSession->SetSessionProperty(property);
    WSRect rect({ 1, 1, 1, 1 });
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError result = sceneSession->UpdateRect(rect, reason, "SceneSessionLayoutTest");
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->GetLayoutController()->SetSessionRect(rect);
    result = sceneSession->UpdateRect(rect, reason, "SceneSessionLayoutTest");
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->Session::UpdateSizeChangeReason(SizeChangeReason::DRAG_END);
    result = sceneSession->UpdateRect(rect, reason, "SceneSessionLayoutTest");
    ASSERT_EQ(result, WSError::WS_OK);

    WSRect rect2({ 0, 0, 0, 0 });
    result = sceneSession->UpdateRect(rect2, reason, "SceneSessionLayoutTest");
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateRect03
 * @tc.desc: UpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, UpdateRect03, TestSize.Level0)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRect03";
    info.bundleName_ = "UpdateRect03";
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;

    WSRect rect = { 200, 200, 200, 200 };
    session->GetLayoutController()->SetSessionRect(rect);
    session->SetClientRect(rect);
    EXPECT_EQ(session->UpdateRect(rect, reason, "SceneSessionLayoutTest"), WSError::WS_OK);

    rect.posX_ = 100;
    rect.posY_ = 100;
    rect.width_ = 800;
    rect.height_ = 800;
    session->GetLayoutController()->SetSessionRect(rect);
    EXPECT_EQ(session->UpdateRect(rect, reason, "SceneSessionLayoutTest"), WSError::WS_OK);
}

/**
 * @tc.name: NotifyClientToUpdateRect01
 * @tc.desc: NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, NotifyClientToUpdateRect01, TestSize.Level0)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRect01";
    info.bundleName_ = "NotifyClientToUpdateRect01";
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<SessionStageMocker> mockSessionStage = sptr<SessionStageMocker>::MakeSptr();
    sceneSession->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
    sceneSession->sessionStage_ = mockSessionStage;
    auto ret = sceneSession->NotifyClientToUpdateRect("SceneSessionLayoutTest", nullptr);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: UpdateRectInner
 * @tc.desc: UpdateRectInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, UpdateRectInner01, TestSize.Level0)
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
    uiParam.rect_ = { 0, 0, 1, 1 };

    sceneSession->GetLayoutController()->SetSessionRect({ 1, 1, 1, 1 });
    sceneSession->isVisible_ = true;
    ASSERT_EQ(false, sceneSession->UpdateRectInner(uiParam, reason));
}

/**
 * @tc.name: UpdateRectInner02
 * @tc.desc: UpdateRectInner02
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, UpdateRectInner02, TestSize.Level0)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRectInner02";
    info.bundleName_ = "UpdateRectInner02";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    SessionUIParam uiParam;
    sceneSession->isVisible_ = true;
    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    sceneSession->SetForegroundInteractiveStatus(true);

    sceneSession->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
    sceneSession->isSubWindowResizingOrMoving_ = true;
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ASSERT_EQ(true, sceneSession->UpdateRectInner(uiParam, reason));
    ASSERT_EQ(true, sceneSession->isSubWindowResizingOrMoving_);

    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ASSERT_EQ(true, sceneSession->UpdateRectInner(uiParam, reason));
    ASSERT_EQ(false, sceneSession->isSubWindowResizingOrMoving_);
}

/**
 * @tc.name: NotifyClientToUpdateRect
 * @tc.desc: NotifyClientToUpdateRect function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, NotifyClientToUpdateRect, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRect";
    info.bundleName_ = "NotifyClientToUpdateRect";
    info.isSystem_ = false;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    session->moveDragController_ = nullptr;
    session->isKeyboardPanelEnabled_ = false;
    session->Session::UpdateSizeChangeReason(SizeChangeReason::UNDEFINED);
    session->Session::SetSessionState(SessionState::STATE_CONNECT);
    session->specificCallback_ = nullptr;
    session->Session::UpdateSizeChangeReason(SizeChangeReason::DRAG);
    EXPECT_EQ(WSError::WS_OK, session->NotifyClientToUpdateRect("SceneSessionLayoutTest", nullptr));

    UpdateAvoidAreaCallback func = [](const int32_t& persistentId) { return; };
    auto specificCallback = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    specificCallback->onUpdateAvoidArea_ = func;
    session->specificCallback_ = specificCallback;
    session->Session::UpdateSizeChangeReason(SizeChangeReason::RECOVER);
    EXPECT_EQ(WSError::WS_OK, session->NotifyClientToUpdateRect("SceneSessionLayoutTest", nullptr));
}

/**
 * @tc.name: CheckAspectRatioValid
 * @tc.desc: CheckAspectRatioValid function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, CheckAspectRatioValid, TestSize.Level0)
{
    SessionInfo info;
    info.abilityName_ = "CheckAspectRatioValid";
    info.bundleName_ = "CheckAspectRatioValid";
    info.isSystem_ = false;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
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
    WindowLimits limits = { 8, 1, 6, 1, 1, 1.0f, 1.0f };
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
HWTEST_F(SceneSessionLayoutTest, CheckAspectRatioValid02, TestSize.Level0)
{
    SessionInfo info;
    info.abilityName_ = "CheckAspectRatioValid02";
    info.bundleName_ = "CheckAspectRatioValid02";
    info.isSystem_ = false;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    sceneSession->SetSessionProperty(property);

    WindowLimits windowLimits;
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
HWTEST_F(SceneSessionLayoutTest, NotifyClientToUpdateRectTask, TestSize.Level0)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRectTask";
    info.bundleName_ = "NotifyClientToUpdateRectTask";
    info.isSystem_ = true;

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
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
    EXPECT_EQ(session->GetSizeChangeReason(), SizeChangeReason::RECOVER);
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
HWTEST_F(SceneSessionLayoutTest, HandleActionUpdateWindowLimits, TestSize.Level1)
{
    SessionInfo info;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();

    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS;
    WMError res = sceneSession->HandleActionUpdateWindowLimits(property, action);
    EXPECT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: SetAspectRatio2
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio2, TestSize.Level0)
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
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio3, TestSize.Level1)
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
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio4, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio4";
    info.bundleName_ = "SetAspectRatio4";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
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
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio5, TestSize.Level0)
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
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio6, TestSize.Level0)
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
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio7, TestSize.Level0)
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
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio8, TestSize.Level1)
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
HWTEST_F(SceneSessionLayoutTest, SetAspectRatio9, TestSize.Level1)
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
HWTEST_F(SceneSessionLayoutTest, SaveAspectRatio, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession;
    sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_EQ(true, sceneSession->SaveAspectRatio(0.1));

    sceneSession->sessionInfo_.bundleName_ = "";
    sceneSession->sessionInfo_.moduleName_ = "";
    sceneSession->sessionInfo_.abilityName_ = "";
    ASSERT_EQ(false, sceneSession->SaveAspectRatio(0.1));
}

/**
 * To test the function call
 *
 * @tc.name: ActivateDragBySystem
 * @tc.desc: ActivateDragBySystem function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, ActivateDragBySystem, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "ActivateDragBySystem";
    info.bundleName_ = "ActivateDragBySystem";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    auto ret = sceneSession->ActivateDragBySystem(true);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * To test the drag activated settings, and validate the draggable results.
 * Expect the results:
 * enableDrag: true, dragActivated: true => true
 * enableDrag: false, dragActivated: true => false
 * enableDrag: true, dragActivated: false => false
 * enableDrag: false, dragActivated: false => false
 *
 * @tc.name: CheckDragActivatedSettings
 * @tc.desc: CheckDragActivatedSettings
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, CheckDragActivatedSettings, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "CheckDragActivatedSettings";
    info.bundleName_ = "CheckDragActivatedSettings";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    sceneSession->ActivateDragBySystem(true);
    sceneSession->GetSessionProperty()->SetDragEnabled(true);
    ASSERT_EQ(true, sceneSession->IsDragAccessible());

    sceneSession->GetSessionProperty()->SetDragEnabled(false);
    ASSERT_EQ(false, sceneSession->IsDragAccessible());

    sceneSession->ActivateDragBySystem(false);
    sceneSession->GetSessionProperty()->SetDragEnabled(true);
    ASSERT_EQ(false, sceneSession->IsDragAccessible());

    sceneSession->GetSessionProperty()->SetDragEnabled(false);
    ASSERT_EQ(false, sceneSession->IsDragAccessible());
}

/**
 * @tc.name: NotifySingleHandTransformChange
 * @tc.desc: NotifySingleHandTransformChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, NotifySingleHandTransformChange, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "NotifySingleHandTransformChange";
    info.bundleName_ = "NotifySingleHandTransformChange";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    SingleHandTransform testTransform;
    sceneSession->state_ = SessionState::STATE_BACKGROUND;
    sceneSession->NotifySingleHandTransformChange(testTransform);

    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    mockSessionStage_ = nullptr;
    sceneSession->sessionStage_ = mockSessionStage_;
    sceneSession->NotifySingleHandTransformChange(testTransform);

    mockSessionStage_ = sptr<SessionStageMocker>::MakeSptr();
    sceneSession->sessionStage_ = mockSessionStage_;
    sceneSession->NotifySingleHandTransformChange(testTransform);
    ASSERT_NE(100, testTransform.posX);
}

/**
 * @tc.name: GetSessionGlobalRectWithSingleHandScale
 * @tc.desc: GetSessionGlobalRectWithSingleHandScale
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, GetSessionGlobalRectWithSingleHandScale, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetSessionGlobalRectWithSingleHandScale";
    info.bundleName_ = "GetSessionGlobalRectWithSingleHandScale";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    SingleHandTransform testTransform;
    WSRect globalRect = { 100, 100, 800, 800 };
    sceneSession->SetSessionGlobalRect(globalRect);
    sceneSession->SetSingleHandTransform(testTransform);
    ASSERT_EQ(sceneSession->GetSessionGlobalRect().posX_,
              sceneSession->GetSessionGlobalRectWithSingleHandScale().posX_);

    testTransform.posX = 10;
    testTransform.posY = 10;
    testTransform.scaleX = 0.75f;
    testTransform.scaleY = 0.75f;
    sceneSession->SetSingleHandTransform(testTransform);
    ASSERT_NE(sceneSession->GetSessionGlobalRect().posX_,
              sceneSession->GetSessionGlobalRectWithSingleHandScale().posX_);
}

/**
 * @tc.name: IsNeedConvertToRelativeRect
 * @tc.desc: IsNeedConvertToRelativeRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, IsNeedConvertToRelativeRect, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "IsNeedConvertToRelativeRect";
    info.bundleName_ = "IsNeedConvertToRelativeRect";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->moveDragController_ =
        sptr<MoveDragController>::MakeSptr(sceneSession->GetPersistentId(), sceneSession->GetWindowType());
    info.abilityName_ = "IsNeedConvertToRelativeRect_subSession";
    info.bundleName_ = "IsNeedConvertToRelativeRect_subSession";
    sptr<SceneSession> subSceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    subSceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSession->subSession_.emplace_back(subSceneSession);
    subSceneSession->parentSession_ = sceneSession;

    EXPECT_EQ(sceneSession->IsNeedConvertToRelativeRect(SizeChangeReason::MAXIMIZE), false);
    EXPECT_EQ(sceneSession->IsNeedConvertToRelativeRect(SizeChangeReason::DRAG_MOVE), true);

    sceneSession->moveDragController_->isStartMove_ = true;
    sceneSession->moveDragController_->isStartDrag_ = false;
    EXPECT_EQ(subSceneSession->IsNeedConvertToRelativeRect(SizeChangeReason::MAXIMIZE), false);

    sceneSession->moveDragController_->isStartMove_ = false;
    sceneSession->moveDragController_->isStartDrag_ = true;
    EXPECT_EQ(subSceneSession->IsNeedConvertToRelativeRect(SizeChangeReason::MAXIMIZE), false);

    subSceneSession->windowAnchorInfo_.isAnchorEnabled_ = true;
    sceneSession->moveDragController_->isStartMove_ = true;
    sceneSession->moveDragController_->isStartDrag_ = false;
    EXPECT_EQ(subSceneSession->IsNeedConvertToRelativeRect(SizeChangeReason::MAXIMIZE), true);
}

/**
 * @tc.name: IsDraggable
 * @tc.desc: IsDraggable when moveDragController_ is nullptr or not.
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, IsDraggable, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "IsDraggable";
    info.bundleName_ = "IsDraggable";
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    sceneSession->GetSessionProperty()->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sceneSession->GetSessionProperty()->SetDragEnabled(true);
    sceneSession->SetDragActivated(true);

    sceneSession->moveDragController_ = nullptr;
    EXPECT_EQ(sceneSession->IsDraggable(), false);

    sceneSession->moveDragController_ =
        sptr<MoveDragController>::MakeSptr(sceneSession->GetPersistentId(), sceneSession->GetWindowType());
    EXPECT_EQ(sceneSession->IsDraggable(), true);
}

/**
 * @tc.name: IsMovable
 * @tc.desc: Test IsMovable when AnchorEnabled or AnchorDisabled
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, IsMovable, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "IsMovable";
    info.bundleName_ = "IsMovable";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSession->moveDragController_ = sptr<MoveDragController>::MakeSptr(2025, WindowType::WINDOW_TYPE_FLOAT);
    EXPECT_EQ(WSError::WS_OK, sceneSession->UpdateFocus(true));
    sceneSession->moveDragController_->isStartMove_ = false;
    sceneSession->moveDragController_->isStartDrag_ = false;
    sceneSession->moveDragController_->hasPointDown_ = true;
    sceneSession->moveDragController_->isMovable_ = true;

    sceneSession->windowAnchorInfo_.isAnchorEnabled_ = true;
    EXPECT_EQ(false, sceneSession->IsMovable());
    sceneSession->windowAnchorInfo_.isAnchorEnabled_ = false;
    EXPECT_EQ(true, sceneSession->IsMovable());
}

/**
 * @tc.name: IsAnyParentSessionDragMoving
 * @tc.desc: IsAnyParentSessionDragMoving
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, IsAnyParentSessionDragMoving, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "IsAnyParentSessionDragMoving";
    info.bundleName_ = "IsAnyParentSessionDragMoving";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->moveDragController_ =
        sptr<MoveDragController>::MakeSptr(sceneSession->GetPersistentId(), sceneSession->GetWindowType());
    info.abilityName_ = "IsAnyParentSessionDragMoving_subSession";
    info.bundleName_ = "IsAnyParentSessionDragMoving_subSession";
    sptr<SceneSession> subSceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    subSceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);

    subSceneSession->parentSession_ = nullptr;
    ASSERT_EQ(subSceneSession->IsAnyParentSessionDragMoving(), false);
    sceneSession->moveDragController_->isStartMove_ = true;
    ASSERT_EQ(sceneSession->IsAnyParentSessionDragMoving(), true);

    sceneSession->subSession_.emplace_back(subSceneSession);
    subSceneSession->parentSession_ = sceneSession;
    ASSERT_EQ(subSceneSession->IsAnyParentSessionDragMoving(), true);
}

/**
 * @tc.name: IsAnyParentSessionDragZooming
 * @tc.desc: IsAnyParentSessionDragZooming
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, IsAnyParentSessionDragZooming, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "IsAnyParentSessionDragZooming";
    info.bundleName_ = "IsAnyParentSessionDragZooming";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->moveDragController_ =
        sptr<MoveDragController>::MakeSptr(sceneSession->GetPersistentId(), sceneSession->GetWindowType());
    info.abilityName_ = "IsAnyParentSessionDragZooming_subSession";
    info.bundleName_ = "IsAnyParentSessionDragZooming_subSession";
    sptr<SceneSession> subSceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    subSceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);

    subSceneSession->parentSession_ = nullptr;
    ASSERT_EQ(subSceneSession->IsAnyParentSessionDragZooming(), false);
    sceneSession->moveDragController_->isStartDrag_ = true;
    ASSERT_EQ(sceneSession->IsAnyParentSessionDragZooming(), true);

    sceneSession->subSession_.emplace_back(subSceneSession);
    subSceneSession->parentSession_ = sceneSession;
    ASSERT_EQ(subSceneSession->IsAnyParentSessionDragZooming(), true);
}

/**
 * @tc.name: HandleSubSessionSurfaceNode
 * @tc.desc: HandleSubSessionSurfaceNode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, HandleSubSessionSurfaceNode, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "HandleSubSessionSurfaceNode";
    info.bundleName_ = "HandleSubSessionSurfaceNode";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sceneSession->moveDragController_ =
        sptr<MoveDragController>::MakeSptr(sceneSession->GetPersistentId(), sceneSession->GetWindowType());
    info.abilityName_ = "HandleSubSessionSurfaceNode_subSession";
    info.bundleName_ = "HandleSubSessionSurfaceNode_subSession";
    sptr<SubSession> subSceneSession = sptr<SubSession>::MakeSptr(info, nullptr);
    subSceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSession->subSession_.emplace_back(subSceneSession);
    subSceneSession->parentSession_ = sceneSession;
    sceneSession->displayIdSetDuringMoveTo_.insert(888);
    subSceneSession->displayIdSetDuringMoveTo_.insert(888);

    subSceneSession->NotifyFollowParentMultiScreenPolicy(false);
    sceneSession->HandleSubSessionSurfaceNode(false, 0);
    ASSERT_NE(0, subSceneSession->displayIdSetDuringMoveTo_.size());
    subSceneSession->NotifyFollowParentMultiScreenPolicy(true);
    sceneSession->HandleSubSessionSurfaceNode(true, 0);
    ASSERT_NE(0, subSceneSession->displayIdSetDuringMoveTo_.size());
    sceneSession->HandleSubSessionSurfaceNode(false, 0);
    ASSERT_EQ(0, subSceneSession->displayIdSetDuringMoveTo_.size());
}

/**
 * @tc.name: HandleSubSessionCrossNode
 * @tc.desc: HandleSubSessionCrossNode
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, HandleSubSessionCrossNode, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "HandleSubSessionCrossNode";
    info.bundleName_ = "HandleSubSessionCrossNode";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    info.abilityName_ = "HandleSubSessionCrossNode_subSession";
    info.bundleName_ = "HandleSubSessionCrossNode_subSession";
    sptr<SceneSession> subSceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    subSceneSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);

    sceneSession->SetDragStart(true);
    sceneSession->HandleSubSessionCrossNode(SizeChangeReason::UNDEFINED);
    ASSERT_EQ(sceneSession->IsDragStart(), true);
    sceneSession->HandleSubSessionCrossNode(SizeChangeReason::DRAG);
    ASSERT_EQ(sceneSession->IsDragStart(), true);
    sceneSession->HandleSubSessionCrossNode(SizeChangeReason::DRAG_END);
    ASSERT_EQ(sceneSession->IsDragStart(), false);

    sceneSession->HandleSubSessionCrossNode(SizeChangeReason::DRAG);
    ASSERT_EQ(sceneSession->IsDragStart(), true);
    sceneSession->subSession_.emplace_back(subSceneSession);
    subSceneSession->parentSession_ = sceneSession;
    sceneSession->HandleSubSessionCrossNode(SizeChangeReason::DRAG_END);
    ASSERT_EQ(sceneSession->IsDragStart(), false);
    sceneSession->HandleSubSessionCrossNode(SizeChangeReason::DRAG_MOVE);
    ASSERT_EQ(sceneSession->IsDragStart(), true);
}

/**
 * @tc.name: UpdateWindowModeForUITest01
 * @tc.desc: UpdateWindowModeForUITest
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionLayoutTest, UpdateWindowModeForUITest01, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "UpdateWindowModeForUITest";
    info.bundleName_ = "UpdateWindowModeForUITest";
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCallback);

    const int32_t updateMode = 1;
    WMError errCode = sceneSession->UpdateWindowModeForUITest(updateMode);
    EXPECT_EQ(errCode, WMError::WM_ERROR_NULLPTR);

    sptr<SessionStageMocker> mockSessionStage = sptr<SessionStageMocker>::MakeSptr();
    sceneSession->sessionStage_ = mockSessionStage;
    errCode = sceneSession->UpdateWindowModeForUITest(updateMode);
    EXPECT_EQ(errCode, WMError::WM_OK);
}
} // namespace
} // namespace Rosen
} // namespace OHOS