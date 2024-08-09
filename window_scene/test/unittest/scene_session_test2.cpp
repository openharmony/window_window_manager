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
#include "ui/rs_surface_node.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionTest2 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionTest2::SetUpTestCase()
{
}

void SceneSessionTest2::TearDownTestCase()
{
}

void SceneSessionTest2::SetUp()
{
}

void SceneSessionTest2::TearDown()
{
}

namespace {
/**
 * @tc.name: RaiseAboveTarget
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, RaiseAboveTarget, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "RaiseAboveTarget";
    info.bundleName_ = "RaiseAboveTarget";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    uint32_t p = 10;
    property->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, p);

    sceneSession->SetSessionProperty(property);
    WSError result = sceneSession->RaiseAboveTarget(0);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    EXPECT_NE(sceneSession->sessionChangeCallback_, nullptr);
    sceneSession->sessionChangeCallback_->onRaiseAboveTarget_ = nullptr;
    result = sceneSession->RaiseAboveTarget(0);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: BindDialogSessionTarget
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, BindDialogSessionTarget, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "BindDialogSessionTarget";
    info.bundleName_ = "BindDialogSessionTarget";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    uint32_t p = 10;
    property->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, p);
    scensession->SetSessionProperty(property);

    sptr<SceneSession> sceneSession = nullptr;
    WSError result = scensession->BindDialogSessionTarget(sceneSession);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<SceneSession> sceneSession1 = scensession;
    result = scensession->BindDialogSessionTarget(sceneSession1);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: NotifyPropertyWhenConnect1
 * @tc.desc: NotifyPropertyWhenConnect1
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, NotifyPropertyWhenConnect1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ability";
    info.bundleName_ = "bundle";
    info.moduleName_ = "module";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    int ret = 1;
    std::string key = info.bundleName_ + info.moduleName_ + info.abilityName_;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    scensession->NotifyPropertyWhenConnect();

    sptr<WindowSessionProperty> property = nullptr;
    scensession->NotifyPropertyWhenConnect();
    ASSERT_EQ(ret, 1);
}

/**
 * @tc.name: GetSystemAvoidArea
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, GetSystemAvoidArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetSystemAvoidArea";
    info.bundleName_ = "GetSystemAvoidArea";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    int32_t p = 10;
    property->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));

    scensession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    AvoidArea avoidArea;
    scensession->GetSystemAvoidArea(rect, avoidArea);
    ASSERT_EQ(p, 10);
}

/**
 * @tc.name: HandleEnterWinwdowArea1
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, HandleEnterWinwdowArea1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleEnterWinwdowArea1";
    info.bundleName_ = "HandleEnterWinwdowArea1";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    property->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));

    scensession->SetSessionProperty(property);
    WSError result = scensession->HandleEnterWinwdowArea(-1, -1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);

    result = scensession->HandleEnterWinwdowArea(1, -1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);

    result = scensession->HandleEnterWinwdowArea(-1, 1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
}

/**
 * @tc.name: HandleEnterWinwdowArea2
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, HandleEnterWinwdowArea2, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleEnterWinwdowArea2";
    info.bundleName_ = "HandleEnterWinwdowArea2";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    scensession->SetSessionProperty(property);
    WSError result = scensession->HandleEnterWinwdowArea(1, 1);
    ASSERT_EQ(result, WSError::WS_OK);

    scensession->sessionInfo_.isSystem_ = true;
    result = scensession->HandleEnterWinwdowArea(1, 1);
    result = scensession->HandleEnterWinwdowArea(1, 1);
    ASSERT_EQ(result, WSError::WS_OK);

    property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::ABOVE_APP_SYSTEM_WINDOW_END);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    scensession->SetSessionProperty(property);
    result = scensession->HandleEnterWinwdowArea(1, 1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_TYPE);

    property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    scensession->SetSessionProperty(property);
    result = scensession->HandleEnterWinwdowArea(1, 1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_TYPE);

    property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    property->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    scensession->SetSessionProperty(property);
    result = scensession->HandleEnterWinwdowArea(1, 1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_TYPE);
}

/**
 * @tc.name: HandlePointerStyle
 * @tc.desc: HandlePointerStyle
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, HandlePointerStyle, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    ASSERT_EQ(scensession->HandlePointerStyle(pointerEvent), WSError::WS_ERROR_NULLPTR);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetPersistentId(11);
    scensession->property_ = property;

    std::shared_ptr<MMI::PointerEvent> pointerEvent_ = MMI::PointerEvent::Create();
    ASSERT_EQ(scensession->HandlePointerStyle(pointerEvent_), WSError::WS_DO_NOTHING);

    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UNKNOWN);
    pointerEvent_->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    ASSERT_EQ(scensession->HandlePointerStyle(pointerEvent_), WSError::WS_DO_NOTHING);

    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->SetButtonId(MMI::PointerEvent::MOUSE_BUTTON_LEFT);
    ASSERT_EQ(scensession->HandlePointerStyle(pointerEvent_), WSError::WS_DO_NOTHING);

    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UNKNOWN);
    pointerEvent_->SetButtonId(MMI::PointerEvent::BUTTON_NONE);
    ASSERT_EQ(scensession->HandlePointerStyle(pointerEvent_), WSError::WS_DO_NOTHING);

    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->SetButtonId(MMI::PointerEvent::BUTTON_NONE);
    ASSERT_EQ(scensession->HandlePointerStyle(pointerEvent_), WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: TransferPointerEvent
 * @tc.desc: TransferPointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, TransferPointerEvent01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsFloatingWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    ASSERT_EQ(scensession->TransferPointerEvent(pointerEvent), WSError::WS_ERROR_NULLPTR);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetPersistentId(11);
    scensession->property_ = property;

    std::shared_ptr<MMI::PointerEvent> pointerEvent_ = MMI::PointerEvent::Create();
    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW);
    ASSERT_EQ(scensession->TransferPointerEvent(pointerEvent_),
        WSError::WS_ERROR_INVALID_SESSION);

    scensession->sessionInfo_.isSystem_ = true;
    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    ASSERT_EQ(scensession->TransferPointerEvent(pointerEvent_),
    WSError::WS_ERROR_NULLPTR);

    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW);
    ASSERT_EQ(scensession->TransferPointerEvent(pointerEvent_),
    WSError::WS_ERROR_NULLPTR);

    scensession->sessionInfo_.isSystem_ = false;
    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    ASSERT_EQ(scensession->TransferPointerEvent(pointerEvent_),
    WSError::WS_ERROR_INVALID_SESSION);

    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW);
    ASSERT_EQ(scensession->TransferPointerEvent(pointerEvent_),
    WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: TransferPointerEvent
 * @tc.desc: TransferPointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, TransferPointerEvent02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Background01";
    info.bundleName_ = "IsSubWindowAppType";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    ASSERT_EQ(scensession->TransferPointerEvent(pointerEvent), WSError::WS_ERROR_NULLPTR);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    property->SetPersistentId(11);
    scensession->property_ = property;

    std::shared_ptr<MMI::PointerEvent> pointerEvent_ = MMI::PointerEvent::Create();
    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(scensession->TransferPointerEvent(pointerEvent_), WSError::WS_ERROR_INVALID_SESSION);
}

/**
 * @tc.name: RequestSessionBack
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, RequestSessionBack, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "RequestSessionBack";
    info.bundleName_ = "RequestSessionBack";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    uint32_t p = 10;
    property->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, p);
    scensession->SetSessionProperty(property);

    WSError result = scensession->RequestSessionBack(true);
    ASSERT_EQ(result, WSError::WS_OK);

    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    EXPECT_NE(nullptr, surfaceNode);
    scensession->SetLeashWinSurfaceNode(surfaceNode);
    result = scensession->RequestSessionBack(true);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: SetParentPersistentId
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetParentPersistentId, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetParentPersistentId";
    info.bundleName_ = "SetParentPersistentId";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    scensession->SetParentPersistentId(0);
    int32_t result = scensession->GetParentPersistentId();
    ASSERT_EQ(result, 0);

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);

    scensession->SetSessionProperty(property);
    scensession->SetParentPersistentId(0);
    result = scensession->GetParentPersistentId();
    ASSERT_EQ(result, 0);

    scensession->SetSessionProperty(nullptr);
    scensession->SetParentPersistentId(0);
    ASSERT_EQ(0, scensession->GetParentPersistentId());
}

/**
 * @tc.name: GetSessionSnapshotFilePath
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, GetSessionSnapshotFilePath, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetSessionSnapshotFilePath";
    info.bundleName_ = "GetSessionSnapshotFilePath";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    scensession->SetSessionProperty(property);

    std::string result = scensession->GetSessionSnapshotFilePath();
    ASSERT_NE(result, "0");
}

/**
 * @tc.name: AddSubSession
 * @tc.desc: AddSubSession Test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, AddSubSession, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifySessionException";
    info.bundleName_ = "NotifySessionException";

    sptr<SceneSession> session = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(session, nullptr);

    sptr<SceneSession> subSession = nullptr;
    bool res = session->AddSubSession(subSession);
    ASSERT_EQ(res, false);

    subSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(subSession, nullptr);

    res = session->AddSubSession(subSession);
    ASSERT_EQ(res, true);

    res = session->AddSubSession(subSession);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: RemoveSubSession
 * @tc.desc: RemoveSubSession Test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, RemoveSubSession, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifySessionException";
    info.bundleName_ = "NotifySessionException";

    sptr<SceneSession> session = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(session, nullptr);

    bool res = session->RemoveSubSession(0);
    ASSERT_EQ(res, false);

    sptr<SceneSession> subSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(subSession, nullptr);

    res = session->AddSubSession(subSession);
    ASSERT_EQ(res, true);

    res = session->RemoveSubSession(subSession->GetPersistentId());
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: NotifyClientToUpdateRect01
 * @tc.desc: NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, NotifyClientToUpdateRect01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRect01";
    info.bundleName_ = "NotifyClientToUpdateRect01";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    scensession->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
    scensession->sessionStage_ = mockSessionStage;
    auto ret = scensession->NotifyClientToUpdateRect(nullptr);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: UpdateSizeChangeReason01
 * @tc.desc: UpdateSizeChangeReason01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, UpdateSizeChangeReason01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateSizeChangeReason01";
    info.bundleName_ = "UpdateSizeChangeReason01";
    info.windowType_ = 1;
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    scensession->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
    scensession->sessionStage_ = mockSessionStage;
    scensession->UpdateSizeChangeReason(SizeChangeReason::ROTATION);
    ASSERT_EQ(scensession->reason_, SizeChangeReason::ROTATION);
}

/**
 * @tc.name: SetMinimizedFlagByUserSwitch/IsMinimizedByUserSwitch
 * @tc.desc: SetMinimizedFlagByUserSwitch/IsMinimizedByUserSwitch Test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetMinimizedFlagByUserSwitch, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetMinimizedFlagByUserSwitch";
    info.bundleName_ = "SetMinimizedFlagByUserSwitch";

    sptr<SceneSession> session = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(session, nullptr);

    bool isMinimized = true;
    session->SetMinimizedFlagByUserSwitch(isMinimized);
    bool ret = session->IsMinimizedByUserSwitch();
    ASSERT_EQ(ret, true);

    isMinimized = false;
    session->SetMinimizedFlagByUserSwitch(isMinimized);
    ret = session->IsMinimizedByUserSwitch();
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: UpdatePiPRect01
 * @tc.desc: UpdatePiPRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, UpdatePiPRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdatePiPRect01";
    info.bundleName_ = "UpdatePiPRect";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scenesession;
    scenesession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scenesession, nullptr);
    scenesession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    scenesession->SetSessionProperty(property);

    Rect rect = {0, 0, 800, 600};
    SizeChangeReason reason = SizeChangeReason::PIP_START;
    WSError result = scenesession->UpdatePiPRect(rect, reason);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdatePiPControlStatus
 * @tc.desc: UpdatePiPControlStatus
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, UpdatePiPControlStatus, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdatePiPControlStatus";
    info.bundleName_ = "UpdatePiPControlStatus";
    auto sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    auto property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    sceneSession->SetSessionProperty(property);

    auto controlType = WsPiPControlType::VIDEO_PLAY_PAUSE;
    auto status = WsPiPControlStatus::PLAY;
    WSError result = sceneSession->UpdatePiPControlStatus(controlType, status);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: SetScale
 * @tc.desc: SetScale
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetScale, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetScale";
    info.bundleName_ = "SetScale";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    int resultValue = 0;
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->SetScale(1.0f, 1.0f, 0.0f, 0.0f);
    ASSERT_EQ(0, resultValue);
}

/**
 * @tc.name: RequestHideKeyboard
 * @tc.desc:  * @tc.name: RequestHideKeyboard
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, RequestHideKeyboard, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "RequestHideKeyboard";
    info.bundleName_ = "RequestHideKeyboard";
    sptr<Rosen::ISession> session_;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    int resultValue = 0;
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->RequestHideKeyboard();
    ASSERT_EQ(0, resultValue);
}

/**
 * @tc.name: SetIsDisplayStatusBarTemporarily
 * @tc.desc:  * @tc.name: SetIsDisplayStatusBarTemporarily
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetIsDisplayStatusBarTemporarily, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetIsDisplayStatusBarTemporarily";
    info.bundleName_ = "SetIsDisplayStatusBarTemporarily";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->SetIsDisplayStatusBarTemporarily(true);
    ASSERT_EQ(true, sceneSession->GetIsDisplayStatusBarTemporarily());
    sceneSession->SetIsDisplayStatusBarTemporarily(false);
    ASSERT_EQ(false, sceneSession->GetIsDisplayStatusBarTemporarily());
}

/**
 * @tc.name: UpdateAvoidArea
 * @tc.desc:  * @tc.name: UpdateAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, UpdateAvoidArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateAvoidArea";
    info.bundleName_ = "UpdateAvoidArea";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    scensession->sessionStage_ = nullptr;
    WSError result = scensession->UpdateAvoidArea(nullptr, AvoidAreaType::TYPE_SYSTEM);
    EXPECT_EQ(WSError::WS_ERROR_NULLPTR, result);

    scensession->sessionStage_ = new SessionStageMocker();
    EXPECT_NE(nullptr, scensession->sessionStage_);
    result = scensession->UpdateAvoidArea(nullptr, AvoidAreaType::TYPE_SYSTEM);
    EXPECT_EQ(WSError::WS_OK, result);
}

/**
 * @tc.name: SaveUpdatedIcon
 * @tc.desc:  * @tc.name: SaveUpdatedIcon
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SaveUpdatedIcon, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SaveUpdatedIcon";
    info.bundleName_ = "SaveUpdatedIcon";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);

    scensession->scenePersistence_ = new ScenePersistence("OpenHarmony", 1);
    EXPECT_NE(nullptr, scensession->scenePersistence_);
    scensession->SaveUpdatedIcon(nullptr);
}

/**
 * @tc.name: NotifyTouchOutside
 * @tc.desc:  * @tc.name: NotifyTouchOutside
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, NotifyTouchOutside, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyTouchOutside";
    info.bundleName_ = "NotifyTouchOutside";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);

    scensession->sessionStage_ = new SessionStageMocker();
    EXPECT_NE(nullptr, scensession->sessionStage_);
    scensession->NotifyTouchOutside();

    scensession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    EXPECT_NE(nullptr, scensession->sessionChangeCallback_);
    auto func = [scensession]() {
        scensession->SaveUpdatedIcon(nullptr);
    };
    scensession->sessionChangeCallback_->OnTouchOutside_ = func;
    EXPECT_NE(nullptr, &func);
    scensession->sessionStage_ = nullptr;
    scensession->NotifyTouchOutside();

    scensession->sessionChangeCallback_->OnTouchOutside_ = nullptr;
    scensession->sessionStage_ = nullptr;
    scensession->NotifyTouchOutside();
}

/**
 * @tc.name: CheckOutTouchOutsideRegister
 * @tc.desc:  * @tc.name: CheckOutTouchOutsideRegister
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, CheckOutTouchOutsideRegister, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "CheckOutTouchOutsideRegister";
    info.bundleName_ = "CheckOutTouchOutsideRegister";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);

    scensession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    EXPECT_NE(nullptr, scensession->sessionChangeCallback_);
    auto func = [scensession]() {
        scensession->NotifyWindowVisibility();
    };
    scensession->sessionChangeCallback_->OnTouchOutside_ = func;
    bool result = scensession->CheckOutTouchOutsideRegister();
    EXPECT_EQ(true, result);

    scensession->sessionChangeCallback_->OnTouchOutside_ = nullptr;
    result = scensession->CheckOutTouchOutsideRegister();
    EXPECT_EQ(false, result);

    scensession->sessionChangeCallback_ = nullptr;
    result = scensession->CheckOutTouchOutsideRegister();
    EXPECT_EQ(false, result);
}

/**
 * @tc.name: UpdateRotationAvoidArea
 * @tc.desc:  * @tc.name: UpdateRotationAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, UpdateRotationAvoidArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRotationAvoidArea";
    info.bundleName_ = "UpdateRotationAvoidArea";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    scensession->specificCallback_ = new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, scensession->specificCallback_);
    auto func = [scensession](const int32_t& persistentId) {
        scensession->SetParentPersistentId(persistentId);
        scensession->GetParentPersistentId();
    };
    scensession->specificCallback_->onUpdateAvoidArea_ = func;
    sptr<Session> session;
    session = new (std::nothrow) Session(info);
    int result = session->GetPersistentId();
    EXPECT_EQ(0, result);
    scensession->UpdateRotationAvoidArea();

    scensession->specificCallback_ = nullptr;
    scensession->UpdateRotationAvoidArea();
}

/**
 * @tc.name: NotifyForceHideChange
 * @tc.desc:  * @tc.name: NotifyForceHideChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, NotifyForceHideChange, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyForceHideChange";
    info.bundleName_ = "NotifyForceHideChange";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    scensession->NotifyForceHideChange(true);

    sptr<Session> session;
    session = new (std::nothrow) Session(info);
    scensession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    auto func = [scensession](bool hide) {
        scensession->SetPrivacyMode(hide);
    };
    scensession->sessionChangeCallback_->OnForceHideChange_ = func;
    EXPECT_NE(nullptr, &func);
    scensession->NotifyForceHideChange(true);

    scensession->SetSessionProperty(nullptr);
    scensession->NotifyForceHideChange(true);
    ASSERT_EQ(scensession->property_->forceHide_, false);
}

/**
 * @tc.name: RegisterSessionChangeCallback
 * @tc.desc:  * @tc.name: RegisterSessionChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, RegisterSessionChangeCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "RegisterSessionChangeCallback";
    info.bundleName_ = "RegisterSessionChangeCallback";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    sptr<SceneSession::SessionChangeCallback> callback = new SceneSession::SessionChangeCallback();
    EXPECT_NE(nullptr, callback);
    scensession->RegisterSessionChangeCallback(callback);
}

/**
 * @tc.name: ClearSpecificSessionCbMap
 * @tc.desc:  * @tc.name: ClearSpecificSessionCbMap
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, ClearSpecificSessionCbMap, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ClearSpecificSessionCbMap";
    info.bundleName_ = "ClearSpecificSessionCbMap";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, scensession);
    scensession->ClearSpecificSessionCbMap();
}

/**
 * @tc.name: SendPointerEventToUI
 * @tc.desc:  * @tc.name: SendPointerEventToUI
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SendPointerEventToUI, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SendPointerEventToUI";
    info.bundleName_ = "SendPointerEventToUI";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, scensession);
    auto pointerEventFunc = [scensession](std::shared_ptr<MMI::PointerEvent> pointerEvent) {
        scensession->NotifyOutsideDownEvent(pointerEvent);
    };
    scensession->systemSessionPointerEventFunc_ = pointerEventFunc;
    std::shared_ptr<MMI::PointerEvent> pointerEvent =
         MMI::PointerEvent::Create();
    scensession->SendPointerEventToUI(pointerEvent);
    EXPECT_NE(nullptr, pointerEvent);
}

/**
 * @tc.name: SetFloatingScale
 * @tc.desc:  * @tc.name: SetFloatingScale
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetFloatingScale, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetFloatingScale";
    info.bundleName_ = "SetFloatingScale";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    scensession->specificCallback_ = new SceneSession::SpecificSessionCallback();
    auto windowInfoUpdateFun = [scensession](int32_t persistentId, WindowUpdateType type) {
        if (WindowUpdateType::WINDOW_UPDATE_PROPERTY == type) {
            scensession->SetCollaboratorType(persistentId);
            return;
        } else {
            persistentId++;
            scensession->SetParentPersistentId(persistentId);
            return;
        }
    };
    auto updateAvoidAreaFun = [scensession](const int32_t& persistentId) {
        bool result = scensession->RemoveSubSession(persistentId);
        scensession->SetWindowAnimationFlag(result);
    };
    scensession->specificCallback_->onWindowInfoUpdate_ = windowInfoUpdateFun;
    scensession->specificCallback_->onUpdateAvoidArea_ = updateAvoidAreaFun;
    scensession->SetFloatingScale(3.14f);
    EXPECT_EQ(3.14f, scensession->floatingScale_);

    scensession->floatingScale_ = 3.0f;
    scensession->SetFloatingScale(3.0f);
    EXPECT_EQ(3.0f, scensession->floatingScale_);
}

/**
 * @tc.name: ProcessPointDownSession
 * @tc.desc:  * @tc.name: ProcessPointDownSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, ProcessPointDownSession, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ProcessPointDownSession";
    info.bundleName_ = "ProcessPointDownSession";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    scensession->specificCallback_ = new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, scensession->specificCallback_);
    auto sessionTouchOutsideFun = [scensession](int32_t persistentId) {
        scensession->SetCollaboratorType(persistentId);
    };
    auto outsideDownEventFun = [scensession](int32_t x, int32_t y) {
        int z = x + y;
        scensession->SetCollaboratorType(z);
    };
    scensession->specificCallback_->onSessionTouchOutside_ = sessionTouchOutsideFun;
    scensession->specificCallback_->onOutsideDownEvent_ = outsideDownEventFun;
    EXPECT_EQ(WSError::WS_OK, scensession->ProcessPointDownSession(3, 4));

    scensession->specificCallback_->onSessionTouchOutside_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, scensession->ProcessPointDownSession(3, 4));

    scensession->sessionInfo_.bundleName_ = "SCBGestureBack";
    scensession->specificCallback_->onOutsideDownEvent_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, scensession->ProcessPointDownSession(3, 4));
}

/**
 * @tc.name: SetSelfToken
 * @tc.desc:  * @tc.name: SetSelfToken
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetSelfToken, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSelfToken";
    info.bundleName_ = "SetSelfToken";
    sptr<SceneSession> scensession;
    sptr<SceneSession::SpecificSessionCallback> specificSession =
        new SceneSession::SpecificSessionCallback();
    scensession = new (std::nothrow) SceneSession(info, specificSession);
    EXPECT_NE(nullptr, scensession);
    scensession->SetSessionState(SessionState::STATE_DISCONNECT);
    scensession->UpdateSessionState(SessionState::STATE_CONNECT);
    scensession->isVisible_ = true;
    EXPECT_EQ(true, scensession->IsVisibleForAccessibility());
    scensession->SetSystemTouchable(false);
    EXPECT_EQ(false, scensession->IsVisibleForAccessibility());
    scensession->SetForegroundInteractiveStatus(true);
    scensession->NotifyAccessibilityVisibilityChange();
    auto windowInfoUpdateFun = [scensession](int32_t persistentId, WindowUpdateType type) {
        if (WindowUpdateType::WINDOW_UPDATE_PROPERTY == type) {
            scensession->SetCollaboratorType(persistentId);
            return;
        } else {
            persistentId++;
            scensession->SetParentPersistentId(persistentId);
            return;
        }
    };
    scensession->specificCallback_->onWindowInfoUpdate_ = windowInfoUpdateFun;
    scensession->NotifyAccessibilityVisibilityChange();
    EXPECT_NE(nullptr, scensession->specificCallback_);
}

/**
 * @tc.name: SetLastSafeRect
 * @tc.desc:  * @tc.name: SetLastSafeRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetLastSafeRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetLastSafeRect";
    info.bundleName_ = "SetLastSafeRect";
    sptr<SceneSession> scensession;
    sptr<SceneSession::SpecificSessionCallback> specificSession =
        new SceneSession::SpecificSessionCallback();
    scensession = new (std::nothrow) SceneSession(info, specificSession);
    WSRect rect = {3, 4, 5, 6};
    scensession->SetLastSafeRect(rect);
    WSRect result = scensession->GetLastSafeRect();
    EXPECT_EQ(3, result.posX_);
    EXPECT_EQ(4, result.posY_);
    EXPECT_EQ(5, result.width_);
    EXPECT_EQ(6, result.height_);
}

/**
 * @tc.name: GetSessionTargetRect
 * @tc.desc:  * @tc.name: GetSessionTargetRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, GetSessionTargetRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetSubSession";
    info.bundleName_ = "GetSessionTargetRect";
    sptr<SceneSession> scensession;
    sptr<SceneSession::SpecificSessionCallback> specificSession =
        new SceneSession::SpecificSessionCallback();
    scensession = new (std::nothrow) SceneSession(info, specificSession);
    bool res = scensession->AddSubSession(scensession);
    EXPECT_EQ(true, res);
    EXPECT_EQ(scensession, (scensession->GetSubSession())[0]);
    scensession->moveDragController_ = new MoveDragController(1024);
    WSRect rectResult = scensession->GetSessionTargetRect();
    EXPECT_EQ(0, rectResult.posX_);
    EXPECT_EQ(0, rectResult.width_);
    auto dragHotAreaFunc = [scensession](int32_t type, const SizeChangeReason& reason) {
        if (SizeChangeReason::END == reason) {
            GTEST_LOG_(INFO) << "type = " << type;
        }
        return;
    };
    scensession->SetWindowDragHotAreaListener(dragHotAreaFunc);
    EXPECT_NE(nullptr,  scensession->moveDragController_);
}

/*
 * @tc.name: SetPipActionEvent
 * @tc.desc:  * @tc.name: SetPipActionEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetPipActionEvent, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetPipActionEvent";
    info.bundleName_ = "SetPipActionEvent";
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    scensession->SetSessionProperty(property);
    WSError res = scensession->SetPipActionEvent("close", 0);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_TYPE);

    property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    property->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    scensession->SetSessionProperty(property);
    res = scensession->SetPipActionEvent("close", 0);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
}

/*
 * @tc.name: SetPiPControlEvent
 * @tc.desc:  * @tc.name: SetPiPControlEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetPiPControlEvent, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetPiPControlEvent";
    info.bundleName_ = "SetPiPControlEvent";
    auto sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    auto property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    auto controlType = WsPiPControlType::VIDEO_PLAY_PAUSE;
    auto status = WsPiPControlStatus::PLAY;
    WSError res = sceneSession->SetPiPControlEvent(controlType, status);
    ASSERT_EQ(res, WSError::WS_ERROR_INVALID_TYPE);

    auto sessionStage = sptr<SessionStageMocker>::MakeSptr();
    ASSERT_NE(sessionStage, nullptr);

    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    property->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sceneSession->SetSessionProperty(property);
    res = sceneSession->SetPiPControlEvent(controlType, status);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);

    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sceneSession->property_ = property;
    ASSERT_EQ(sceneSession->SetPiPControlEvent(controlType, status), WSError::WS_ERROR_INVALID_TYPE);

    sceneSession->sessionStage_ = sptr<SessionStageMocker>::MakeSptr();
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    property->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sceneSession->property_ = property;
    ASSERT_EQ(sceneSession->SetPiPControlEvent(controlType, status), WSError::WS_OK);
}

/**
 * @tc.name: SetShouldHideNonSecureWindows
 * @tc.desc: SetShouldHideNonSecureWindows
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetShouldHideNonSecureWindows, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetShouldHideNonSecureWindows";
    info.bundleName_ = "SetShouldHideNonSecureWindows";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    EXPECT_FALSE(sceneSession->shouldHideNonSecureWindows_.load());
    sceneSession->SetShouldHideNonSecureWindows(true);
    EXPECT_TRUE(sceneSession->shouldHideNonSecureWindows_.load());
}

/**
 * @tc.name: UpdateExtWindowFlags
 * @tc.desc: update uiextension window flags
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, UpdateExtWindowFlags, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateExtWindowFlags";
    info.bundleName_ = "UpdateExtWindowFlags";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    EXPECT_TRUE(sceneSession->extWindowFlagsMap_.empty());
    int32_t persistentId = 12345;
    ExtensionWindowFlags flags(7);
    ExtensionWindowFlags actions(7);
    sceneSession->UpdateExtWindowFlags(persistentId, flags, actions);
    EXPECT_EQ(sceneSession->extWindowFlagsMap_.size(), 1);
    EXPECT_EQ(sceneSession->extWindowFlagsMap_.begin()->first, persistentId);
    EXPECT_EQ(sceneSession->extWindowFlagsMap_.begin()->second.bitData, 7);
    flags.bitData = 0;
    actions.bitData = 3;
    sceneSession->UpdateExtWindowFlags(persistentId, flags, actions);
    EXPECT_EQ(sceneSession->extWindowFlagsMap_.size(), 1);
    EXPECT_EQ(sceneSession->extWindowFlagsMap_.begin()->first, persistentId);
    EXPECT_EQ(sceneSession->extWindowFlagsMap_.begin()->second.bitData, 4);
    actions.bitData = 4;
    sceneSession->UpdateExtWindowFlags(persistentId, flags, actions);
    EXPECT_TRUE(sceneSession->extWindowFlagsMap_.empty());
}

/**
 * @tc.name: GetCombinedExtWindowFlags
 * @tc.desc: get combined uiextension window flags
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, GetCombinedExtWindowFlags, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetCombinedExtWindowFlags";
    info.bundleName_ = "GetCombinedExtWindowFlags";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    auto combinedExtWindowFlags = sceneSession->GetCombinedExtWindowFlags();
    EXPECT_EQ(combinedExtWindowFlags.bitData, 0);
    sceneSession->UpdateExtWindowFlags(1234, ExtensionWindowFlags(3), ExtensionWindowFlags(3));
    sceneSession->UpdateExtWindowFlags(5678, ExtensionWindowFlags(4), ExtensionWindowFlags(4));

    sceneSession->state_ = SessionState::STATE_FOREGROUND;
    combinedExtWindowFlags = sceneSession->GetCombinedExtWindowFlags();
    EXPECT_EQ(combinedExtWindowFlags.bitData, 7);

    sceneSession->state_ = SessionState::STATE_BACKGROUND;
    combinedExtWindowFlags = sceneSession->GetCombinedExtWindowFlags();
    EXPECT_EQ(combinedExtWindowFlags.bitData, 6);
}

/**
 * @tc.name: RemoveExtWindowFlags
 * @tc.desc: remove uiextension window flags
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, RemoveExtWindowFlags, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "RemoveExtWindowFlags";
    info.bundleName_ = "RemoveExtWindowFlags";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    EXPECT_TRUE(sceneSession->extWindowFlagsMap_.empty());
    sceneSession->UpdateExtWindowFlags(1234, ExtensionWindowFlags(3), ExtensionWindowFlags(3));
    EXPECT_EQ(sceneSession->extWindowFlagsMap_.size(), 1);
    sceneSession->RemoveExtWindowFlags(1234);
    EXPECT_TRUE(sceneSession->extWindowFlagsMap_.empty());
}

/**
 * @tc.name: ClearExtWindowFlags
 * @tc.desc: clear uiextension window flags
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, ClearExtWindowFlags, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ClearExtWindowFlags";
    info.bundleName_ = "ClearExtWindowFlags";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    EXPECT_TRUE(sceneSession->extWindowFlagsMap_.empty());
    sceneSession->UpdateExtWindowFlags(1234, ExtensionWindowFlags(3), ExtensionWindowFlags(3));
    sceneSession->UpdateExtWindowFlags(5678, ExtensionWindowFlags(4), ExtensionWindowFlags(4));
    EXPECT_EQ(sceneSession->extWindowFlagsMap_.size(), 2);
    sceneSession->ClearExtWindowFlags();
    EXPECT_TRUE(sceneSession->extWindowFlagsMap_.empty());
}

/**
 * @tc.name: CalculateCombinedExtWindowFlags
 * @tc.desc: calculate combined uiextension window flags
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, CalculateCombinedExtWindowFlags, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "CalculateCombinedExtWindowFlags";
    info.bundleName_ = "CalculateCombinedExtWindowFlags";

    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    EXPECT_EQ(sceneSession->combinedExtWindowFlags_.bitData, 0);
    sceneSession->UpdateExtWindowFlags(1234, ExtensionWindowFlags(3), ExtensionWindowFlags(3));
    sceneSession->UpdateExtWindowFlags(5678, ExtensionWindowFlags(4), ExtensionWindowFlags(4));
    sceneSession->CalculateCombinedExtWindowFlags();
    EXPECT_EQ(sceneSession->combinedExtWindowFlags_.bitData, 7);
}

/**
 * @tc.name: ChangeSessionVisibilityWithStatusBar
 * @tc.desc:  * @tc.name: ChangeSessionVisibilityWithStatusBar
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, ChangeSessionVisibilityWithStatusBar, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ChangeSessionVisibilityWithStatusBar";
    info.bundleName_ = "ChangeSessionVisibilityWithStatusBar";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<AAFwk::SessionInfo> info1 = nullptr;
    WSError result = sceneSession->ChangeSessionVisibilityWithStatusBar(info1, false);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);

    sptr<AAFwk::SessionInfo> abilitySessionInfo = new AAFwk::SessionInfo();
    result = sceneSession->ChangeSessionVisibilityWithStatusBar(abilitySessionInfo, false);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PERMISSION);
    delete sceneSession;
}

/**
 * @tc.name: SetForceHideState
 * @tc.desc: SetForceHideState
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetForceHideState, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetForceHideState";
    info.bundleName_ = "SetForceHideState";
    sptr<SceneSession> sceneSession;
    sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sceneSession->SetForceHideState(ForceHideState::NOT_HIDDEN);
    bool hide = sceneSession->GetForceHideState();
    ASSERT_EQ(hide, ForceHideState::NOT_HIDDEN);
    sceneSession->SetForceHideState(ForceHideState::HIDDEN_WHEN_FOCUSED);
    hide = sceneSession->GetForceHideState();
    ASSERT_EQ(hide, ForceHideState::HIDDEN_WHEN_FOCUSED);
}

/**
 * @tc.name: ClearSpecificSessionCbMap
 * @tc.desc:  * @tc.name: ClearSpecificSessionCbMap
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, ClearSpecificSessionCbMap01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ClearSpecificSessionCbMap01";
    info.bundleName_ = "ClearSpecificSessionCbMap01";
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, scensession);
    sptr<Session> session;
    session = new (std::nothrow) Session(info);
    scensession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    scensession->ClearSpecificSessionCbMap();

    bool isFromClient = true;
    scensession->needSnapshot_ = true;
    scensession->specificCallback_ = new (std::nothrow) SceneSession::SpecificSessionCallback();
    scensession->Disconnect(isFromClient);

    bool isActive = false;
    scensession->UpdateActiveStatus(isActive);
    isActive = true;
    Session ssession(info);
    ssession.state_ = SessionState::STATE_FOREGROUND;
    isActive = false;
    ssession.state_ = SessionState::STATE_ACTIVE;
}

/**
 * @tc.name: OnSessionEvent01
 * @tc.desc:  * @tc.name: OnSessionEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, OnSessionEvent01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnSessionEvent01";
    info.bundleName_ = "OnSessionEvent01";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    SessionEvent event = SessionEvent::EVENT_START_MOVE;
    sceneSession->moveDragController_ = new MoveDragController(1);
    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    sceneSession->OnSessionEvent(event);
    sceneSession->moveDragController_->isStartDrag_ = true;
    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    EXPECT_NE(sceneSession->sessionChangeCallback_, nullptr);
    auto result = sceneSession->OnSessionEvent(event);
    ASSERT_EQ(result, WSError::WS_OK);

    event = SessionEvent::EVENT_END_MOVE;
    ASSERT_EQ(sceneSession->OnSessionEvent(event), WSError::WS_OK);
}

/**
 * @tc.name: SetSessionRectChangeCallback
 * @tc.desc:  * @tc.name: SetSessionRectChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetSessionRectChangeCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSessionRectChangeCallback";
    info.bundleName_ = "SetSessionRectChangeCallback";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    NotifySessionRectChangeFunc func;
    sceneSession->SetSessionRectChangeCallback(func);
    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();

    sceneSession->RaiseToAppTop();
    sceneSession = nullptr;
    GTEST_LOG_(INFO) << "2";
    sceneSession->BindDialogSessionTarget(sceneSession);

    Session ssession(info);
    ssession.property_ = nullptr;
}

/**
 * @tc.name: SetSessionPiPControlStatusChangeCallback
 * @tc.desc:  * @tc.name: SetSessionPiPControlStatusChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetSessionPiPControlStatusChangeCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSessionPiPControlStatusChangeCallback";
    info.bundleName_ = "SetSessionPiPControlStatusChangeCallback";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    NotifySessionPiPControlStatusChangeFunc func;
    sceneSession->SetSessionPiPControlStatusChangeCallback(func);
}

/**
 * @tc.name: RaiseAppMainWindowToTop
 * @tc.desc:  * @tc.name: RaiseAppMainWindowToTop
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, RaiseAppMainWindowToTop, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "RaiseAppMainWindowToTop";
    info.bundleName_ = "RaiseAppMainWindowToTop";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sceneSession->RaiseAppMainWindowToTop();
    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    bool status = true;
    sceneSession->OnNeedAvoid(status);

    bool showWhenLocked = true;
    sceneSession->OnShowWhenLocked(showWhenLocked);
    sceneSession->NotifyPropertyWhenConnect();

    sceneSession->focusedOnShow_ = false;
    sceneSession->RaiseAppMainWindowToTop();
}

/**
 * @tc.name: GetKeyboardAvoidArea
 * @tc.desc:  * @tc.name: GetKeyboardAvoidArea01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, GetKeyboardAvoidArea01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetKeyboardAvoidArea";
    info.bundleName_ = "GetKeyboardAvoidArea";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    WSRect rect;
    AvoidArea avoidArea;
    GTEST_LOG_(INFO) << "1";
    sceneSession->GetKeyboardAvoidArea(rect, avoidArea);
}

/**
 * @tc.name: GetCutoutAvoidArea
 * @tc.desc:  * @tc.name: GetCutoutAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, GetCutoutAvoidArea01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetCutoutAvoidArea";
    info.bundleName_ = "GetCutoutAvoidArea";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    WSRect rect;
    AvoidArea avoidArea;
    DisplayManager displayManager;
    Session ssession(info);
    auto display = DisplayManager::GetInstance().GetDisplayById(
        ssession.GetSessionProperty()->GetDisplayId());
    sceneSession->GetCutoutAvoidArea(rect, avoidArea);
}

/**
 * @tc.name: GetAINavigationBarArea
 * @tc.desc:  * @tc.name: GetAINavigationBarArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, GetAINavigationBarArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetAINavigationBarArea";
    info.bundleName_ = "GetAINavigationBarArea";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    WSRect rect;
    AvoidArea avoidArea;
    sceneSession->GetAINavigationBarArea(rect, avoidArea);

    sceneSession->SetIsDisplayStatusBarTemporarily(true);
    sceneSession->GetAINavigationBarArea(rect, avoidArea);
    ASSERT_EQ(sceneSession->GetIsDisplayStatusBarTemporarily(), true);

    auto property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(property, nullptr);
    property->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sceneSession->property_ = property;
    sceneSession->GetAINavigationBarArea(rect, avoidArea);

    sceneSession->SetSessionProperty(nullptr);
    sceneSession->GetAINavigationBarArea(rect, avoidArea);

    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sceneSession->SetSessionProperty(property);
    sceneSession->specificCallback_->onGetAINavigationBarArea_ = [](uint64_t displayId) {
        WSRect rect = {1, 1, 1, 1};
        return rect;
    };
    sceneSession->GetAINavigationBarArea(rect, avoidArea);
}

/**
 * @tc.name: HandlePointerStyle01
 * @tc.desc:  * @tc.name: HandlePointerStyle
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, HandlePointerStyle01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandlePointerStyle";
    info.bundleName_ = "HandlePointerStyle";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    OHOS::Rosen::DisplayManager displayManager;
    MMI::PointerEvent::PointerItem pointerItem;
}

/**
 * @tc.name: TransferPointerEvent
 * @tc.desc:  * @tc.name: TransferPointerEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, TransferPointerEvent03, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "TransferPointerEvent";
    info.bundleName_ = "TransferPointerEvent";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    sceneSession->specificCallback_ = new (std::nothrow) SceneSession::SpecificSessionCallback();

    sceneSession->TransferPointerEvent(pointerEvent);

    WindowLimits limits;
    WSRect rect;
    float ratio = 0.0;
    bool isDecor = true;
    float vpr = 0.0;
    sceneSession->FixRectByLimits(limits, rect, ratio, isDecor, vpr);
    sceneSession->SetPipActionEvent("pointerEvent", 0);

    auto property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sceneSession->SetSessionProperty(property);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_TYPE, sceneSession->SetPipActionEvent("pointerEvent", 0));

    sceneSession->sessionStage_ = sptr<SessionStageMocker>::MakeSptr();
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    property->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    ASSERT_EQ(WSError::WS_OK, sceneSession->SetPipActionEvent("pointerEvent", 0));

    sceneSession->FixRectByLimits(limits, rect, ratio, false, vpr);
}

/**
 * @tc.name: OnMoveDragCallback
 * @tc.desc:  * @tc.name: OnMoveDragCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, OnMoveDragCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnMoveDragCallback";
    info.bundleName_ = "OnMoveDragCallback";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    Session session(info);
    WSRect rect;
    sceneSession->UpdateWinRectForSystemBar(rect);
    sceneSession->SetSurfaceBounds(rect);
    sceneSession->GetWindowNameAllType();
    session.scenePersistence_ = new ScenePersistence("aa", 0);
    sceneSession->GetUpdatedIconPath();

    bool visible = true;
    sceneSession->UpdateNativeVisibility(visible);

    sceneSession->scenePersistence_ = new (std::nothrow) ScenePersistence(info.bundleName_, 0);
    EXPECT_NE(sceneSession->scenePersistence_, nullptr);
    sceneSession->GetUpdatedIconPath();
}

/**
 * @tc.name: OnMoveDragCallback
 * @tc.desc:  * @tc.name: OnMoveDragCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, OnMoveDragCallback01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnMoveDragCallback01";
    info.bundleName_ = "OnMoveDragCallback01";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    double alpha = 0.5;
    Session session(info);

    sceneSession->SetSystemSceneOcclusionAlpha(alpha);
    sceneSession->IsNeedDefaultAnimation();
    bool isPlaying = true;
    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    sceneSession->NotifyIsCustomAnimationPlaying(isPlaying);

    sptr<AAFwk::SessionInfo> abilitySessionInfo = nullptr;
    bool visible = true;
    sceneSession->ChangeSessionVisibilityWithStatusBar(abilitySessionInfo, visible);
}

/**
 * @tc.name: OnMoveDragCallback
 * @tc.desc:  * @tc.name: OnMoveDragCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, OnMoveDragCallback02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnMoveDragCallback02";
    info.bundleName_ = "OnMoveDragCallback02";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<SceneSession> ssession = nullptr;
    sceneSession->HandleCastScreenConnection(info, ssession);

    Session session(info);
    sptr<AAFwk::SessionInfo> abilitySessionInfo = nullptr;
    session.isTerminating_ = true;
    sceneSession->TerminateSession(abilitySessionInfo);

    bool needRemoveSession = true;
    session.sessionExceptionFunc_ = std::make_shared<NotifySessionExceptionFunc>();
    session.jsSceneSessionExceptionFunc_ = std::make_shared<NotifySessionExceptionFunc>();
    sceneSession->NotifySessionException(abilitySessionInfo, needRemoveSession);

    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    sceneSession->NotifyPiPWindowPrepareClose();

    bool isLandscapeMultiWindow = true;
    sceneSession->SetLandscapeMultiWindow(isLandscapeMultiWindow);

    std::shared_ptr<MMI::KeyEvent> keyEvent = std::make_shared<MMI::KeyEvent>(MMI::KeyEvent::KEYCODE_BACK);
    bool isPreImeEvent = true;
    sceneSession->SendKeyEventToUI(keyEvent, isPreImeEvent);
    sceneSession->IsDirtyWindow();
    sceneSession->moveDragController_ = new MoveDragController(0);
    sceneSession->NotifyUILostFocus();
}

/**
 * @tc.name: IsStartMoving
 * @tc.desc:  * @tc.name: IsStartMoving
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, IsStartMoving, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "IsStartMoving";
    info.bundleName_ = "IsStartMoving";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sceneSession->IsStartMoving();
    bool startMoving = true;
    sceneSession->SetIsStartMoving(startMoving);
    ExtensionWindowFlags extWindowActions;
    sceneSession->UpdateExtWindowFlags(1, 0, extWindowActions);
    DisplayId from = 0;
    DisplayId to = 0;
    sceneSession->NotifyDisplayMove(from, to);
    sceneSession->RemoveExtWindowFlags(0);
    sceneSession->ClearExtWindowFlags();
    bool isRegister = true;
    sceneSession->UpdateRectChangeListenerRegistered(isRegister);
}

/**
 * @tc.name: IsSystemSpecificSession
 * @tc.desc: IsSystemSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, IsSystemSpecificSession, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "IsSystemSpecificSession";
    info.bundleName_ = "IsSystemSpecificSession";

    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(sceneSession, nullptr);

    ASSERT_FALSE(sceneSession->IsSystemSpecificSession());
    sceneSession->SetIsSystemSpecificSession(true);
    ASSERT_TRUE(sceneSession->IsSystemSpecificSession());
}

/**
 * @tc.name: SetTemporarilyShowWhenLocked
 * @tc.desc:  * @tc.name: SetTemporarilyShowWhenLocked
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetTemporarilyShowWhenLocked, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetTemporarilyShowWhenLocked";
    info.bundleName_ = "SetTemporarilyShowWhenLocked";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    bool isTemporarilyShowWhenLocked = sceneSession->IsTemporarilyShowWhenLocked();
    ASSERT_EQ(isTemporarilyShowWhenLocked, false);
    sceneSession->SetTemporarilyShowWhenLocked(true);
    isTemporarilyShowWhenLocked = sceneSession->IsTemporarilyShowWhenLocked();
    ASSERT_EQ(isTemporarilyShowWhenLocked, true);
    sceneSession->SetTemporarilyShowWhenLocked(false);
    isTemporarilyShowWhenLocked = sceneSession->IsTemporarilyShowWhenLocked();
    ASSERT_EQ(isTemporarilyShowWhenLocked, false);
}

/**
 * @tc.name: GetShowWhenLockedFlagValue
 * @tc.desc:  * @tc.name: GetShowWhenLockedFlagValue
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, GetShowWhenLockedFlagValue, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetShowWhenLockedFlagValue";
    info.bundleName_ = "GetShowWhenLockedFlagValue";
    info.windowType_ = 1;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession;
    scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(scensession->GetShowWhenLockedFlagValue(), false);
    scensession->property_ = property;
    property->SetWindowFlags(4);
    ASSERT_EQ(scensession->GetShowWhenLockedFlagValue(), true);
}

/**
 * @tc.name: SetClientIdentityToken
 * @tc.desc: SetClientIdentityToken
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetClientIdentityToken, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetClientIdentityToken";
    info.bundleName_ = "SetClientIdentityToken";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    std::string token = "testToken";
    sceneSession->SetClientIdentityToken(token);
    ASSERT_EQ(sceneSession->GetClientIdentityToken(), token);
}

/**
 * @tc.name: SetSkipDraw
 * @tc.desc: SetSkipDraw
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetSkipDraw, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSkipDraw";
    info.bundleName_ = "SetSkipDraw";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->SetSkipDraw(true);
    sceneSession->SetSkipDraw(false);
}

/**
 * @tc.name: GetWindowDragHotAreaType
 * @tc.desc: GetWindowDragHotAreaType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, GetWindowDragHotAreaType, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HotAreaType";
    info.bundleName_ = "HotAreaType";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    WSRect rect = {0, 0, 10, 10};
    sceneSession->AddOrUpdateWindowDragHotArea(1, rect);
    sceneSession->AddOrUpdateWindowDragHotArea(1, rect);
    auto type = sceneSession->GetWindowDragHotAreaType(1, 2, 2);
    ASSERT_EQ(type, 1);
}

/**
 * @tc.name: GetSubWindowModalType
 * @tc.desc: GetSubWindowModalType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, GetSubWindowModalType, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ModalType";
    info.bundleName_ = "ModalType";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sceneSession->SetSessionProperty(nullptr);
    auto result = sceneSession->GetSubWindowModalType();
    ASSERT_EQ(result, SubWindowModalType::TYPE_UNDEFINED);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sceneSession->SetSessionProperty(property);
    result = sceneSession->GetSubWindowModalType();
    ASSERT_EQ(result, SubWindowModalType::TYPE_DIALOG);
}

/**
 * @tc.name: CheckGetAvoidAreaAvailable
 * @tc.desc: CheckGetAvoidAreaAvailable
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, CheckGetAvoidAreaAvailable, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "CheckGetAvoidAreaAvailable";
    info.bundleName_ = "CheckGetAvoidAreaAvailable";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    sceneSession->CheckGetAvoidAreaAvailable(AvoidAreaType::TYPE_SYSTEM);

    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sceneSession->SetSessionProperty(property);
    sceneSession->CheckGetAvoidAreaAvailable(AvoidAreaType::TYPE_SYSTEM);
}

/**
 * @tc.name: IsFullScreenMovable
 * @tc.desc: IsFullScreenMovable
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, IsFullScreenMovable, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "IsFullScreenMovable";
    info.bundleName_ = "IsFullScreenMovable";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sceneSession->SetSessionProperty(nullptr);
    auto result = sceneSession->IsFullScreenMovable();
    ASSERT_EQ(false, result);
}

/**
 * @tc.name: SetWindowAnimationFlag
 * @tc.desc: SetWindowAnimationFlag
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetWindowAnimationFlag, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetWindowAnimationFlag";
    info.bundleName_ = "SetWindowAnimationFlag";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    EXPECT_NE(sceneSession->sessionChangeCallback_, nullptr);
    sceneSession->sessionChangeCallback_->onWindowAnimationFlagChange_ = [](
        bool isNeedDefaultAnimationFlag) {};
    sceneSession->SetWindowAnimationFlag(true);
    ASSERT_EQ(true, sceneSession->needDefaultAnimationFlag_);
}
}
}
}