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
    property->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;

    sceneSession->SetSessionProperty(property);
    WSError result = sceneSession->RaiseAboveTarget(0);
    ASSERT_EQ(result, WSError::WS_OK);

    sptr<SceneSession> tempSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    int32_t persistentId = 1;
    tempSession->persistentId_ = persistentId;
    int32_t callingPid = 2;
    tempSession->callingPid_ = callingPid;
    sceneSession->subSession_.push_back(tempSession);
    int32_t subWindowId = 1;
    result = sceneSession->RaiseAboveTarget(subWindowId);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_CALLING);

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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    property->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    sceneSession->SetSessionProperty(property);

    sptr<SceneSession> sceneSession1 = nullptr;
    WSError result = sceneSession->BindDialogSessionTarget(sceneSession1);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    sptr<SceneSession> sceneSession2 = sceneSession;
    result = sceneSession->BindDialogSessionTarget(sceneSession2);
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    int ret = 1;
    std::string key = info.bundleName_ + info.moduleName_ + info.abilityName_;
    sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    sceneSession->NotifyPropertyWhenConnect();

    sptr<WindowSessionProperty> property = nullptr;
    sceneSession->NotifyPropertyWhenConnect();
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    int32_t p = 10;
    property->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));

    sceneSession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    AvoidArea avoidArea;
    sceneSession->GetSystemAvoidArea(rect, avoidArea);
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    property->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));

    sceneSession->SetSessionProperty(property);
    WSError result = sceneSession->HandleEnterWinwdowArea(-1, -1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);

    result = sceneSession->HandleEnterWinwdowArea(1, -1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);

    result = sceneSession->HandleEnterWinwdowArea(-1, 1);
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sceneSession->SetSessionProperty(property);
    WSError result = sceneSession->HandleEnterWinwdowArea(1, 1);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->sessionInfo_.isSystem_ = true;
    result = sceneSession->HandleEnterWinwdowArea(1, 1);
    result = sceneSession->HandleEnterWinwdowArea(1, 1);
    ASSERT_EQ(result, WSError::WS_OK);

    property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::ABOVE_APP_SYSTEM_WINDOW_END);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sceneSession->SetSessionProperty(property);
    result = sceneSession->HandleEnterWinwdowArea(1, 1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_TYPE);

    property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sceneSession->SetSessionProperty(property);
    result = sceneSession->HandleEnterWinwdowArea(1, 1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_TYPE);

    property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    property->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    sceneSession->SetSessionProperty(property);
    result = sceneSession->HandleEnterWinwdowArea(1, 1);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_TYPE);
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    ASSERT_EQ(sceneSession->TransferPointerEvent(pointerEvent), WSError::WS_ERROR_NULLPTR);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetPersistentId(11);
    sceneSession->property_ = property;

    std::shared_ptr<MMI::PointerEvent> pointerEvent_ = MMI::PointerEvent::Create();
    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW);
    ASSERT_EQ(sceneSession->TransferPointerEvent(pointerEvent_),
        WSError::WS_ERROR_INVALID_SESSION);

    sceneSession->sessionInfo_.isSystem_ = true;
    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    ASSERT_EQ(sceneSession->TransferPointerEvent(pointerEvent_),
    WSError::WS_ERROR_NULLPTR);

    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW);
    ASSERT_EQ(sceneSession->TransferPointerEvent(pointerEvent_),
    WSError::WS_ERROR_NULLPTR);

    sceneSession->sessionInfo_.isSystem_ = false;
    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
    ASSERT_EQ(sceneSession->TransferPointerEvent(pointerEvent_),
    WSError::WS_ERROR_INVALID_SESSION);

    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW);
    ASSERT_EQ(sceneSession->TransferPointerEvent(pointerEvent_),
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    ASSERT_EQ(sceneSession->TransferPointerEvent(pointerEvent), WSError::WS_ERROR_NULLPTR);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    property->SetPersistentId(11);
    sceneSession->property_ = property;

    std::shared_ptr<MMI::PointerEvent> pointerEvent_ = MMI::PointerEvent::Create();
    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
    ASSERT_EQ(sceneSession->TransferPointerEvent(pointerEvent_), WSError::WS_ERROR_INVALID_SESSION);
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    property->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    sceneSession->SetSessionProperty(property);

    WSError result = sceneSession->RequestSessionBack(true);
    ASSERT_EQ(result, WSError::WS_OK);

    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    EXPECT_NE(nullptr, surfaceNode);
    sceneSession->SetLeashWinSurfaceNode(surfaceNode);
    result = sceneSession->RequestSessionBack(true);
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sceneSession->SetParentPersistentId(0);
    int32_t result = sceneSession->GetParentPersistentId();
    ASSERT_EQ(result, 0);

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);

    sceneSession->SetSessionProperty(property);
    sceneSession->SetParentPersistentId(0);
    result = sceneSession->GetParentPersistentId();
    ASSERT_EQ(result, 0);

    sceneSession->SetSessionProperty(nullptr);
    sceneSession->SetParentPersistentId(0);
    ASSERT_EQ(0, sceneSession->GetParentPersistentId());
}

/**
 * @tc.name: GetMainSessionId
 * @tc.desc: GetMainSessionId Test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, GetMainSessionId, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetMainSessionId";
    info.moduleName_ = "GetMainSessionId";
    info.bundleName_ = "GetMainSessionId";

    sptr<Session> session = sptr<Session>::MakeSptr(info);
    EXPECT_NE(session, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetPersistentId(0);
    session->SetSessionProperty(property);

    sptr<Session> subSession = sptr<Session>::MakeSptr(info);
    EXPECT_NE(subSession, nullptr);
    subSession->SetParentSession(session);
    sptr<WindowSessionProperty> subProperty = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(subProperty, nullptr);
    subProperty->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    subProperty->SetPersistentId(1);
    subSession->SetSessionProperty(subProperty);

    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->SetParentSession(subSession);
    sptr<WindowSessionProperty> sceneProperty = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(sceneProperty, nullptr);
    sceneProperty->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    sceneProperty->SetPersistentId(2);
    sceneSession->SetSessionProperty(sceneProperty);
    auto result = sceneSession->GetMainSessionId();
    ASSERT_EQ(result, 0);
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    sceneSession->SetSessionProperty(property);

    std::string result = sceneSession->GetSessionSnapshotFilePath();
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    sceneSession->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
    sceneSession->sessionStage_ = mockSessionStage;
    auto ret = sceneSession->NotifyClientToUpdateRect("SceneSessionTest2", nullptr);
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    sceneSession->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
    sceneSession->sessionStage_ = mockSessionStage;
    sceneSession->UpdateSizeChangeReason(SizeChangeReason::ROTATION);
    ASSERT_EQ(sceneSession->reason_, SizeChangeReason::ROTATION);
    ASSERT_EQ(WSError::WS_OK, sceneSession->UpdateSizeChangeReason(SizeChangeReason::UNDEFINED));
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    sceneSession->SetSessionProperty(property);

    Rect rect = {0, 0, 800, 600};
    SizeChangeReason reason = SizeChangeReason::PIP_START;
    WSError result = sceneSession->UpdatePiPRect(rect, reason);
    ASSERT_EQ(result, WSError::WS_OK);

    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sceneSession->SetSessionProperty(property);
    result = sceneSession->UpdatePiPRect(rect, reason);
    ASSERT_EQ(result, WSError::WS_DO_NOTHING);

    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    sceneSession->SetSessionProperty(property);
    sceneSession->isTerminating_ = true;
    result = sceneSession->UpdatePiPRect(rect, reason);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->isTerminating_ = false;
    result = sceneSession->UpdatePiPRect(rect, reason);
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

    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sceneSession->SetSessionProperty(property);
    result = sceneSession->UpdatePiPControlStatus(controlType, status);
    ASSERT_EQ(result, WSError::WS_DO_NOTHING);
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->SetScale(1.0f, 1.0f, 0.0f, 0.0f);
    ASSERT_EQ(0, resultValue);
}

/**
 * @tc.name: RequestHideKeyboard
 * @tc.desc: RequestHideKeyboard
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->RequestHideKeyboard();
    ASSERT_EQ(0, resultValue);
}

/**
 * @tc.name: SetIsDisplayStatusBarTemporarily
 * @tc.desc: SetIsDisplayStatusBarTemporarily
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
 * @tc.desc: UpdateAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, UpdateAvoidArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateAvoidArea";
    info.bundleName_ = "UpdateAvoidArea";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSession->sessionStage_ = nullptr;
    WSError result = sceneSession->UpdateAvoidArea(nullptr, AvoidAreaType::TYPE_SYSTEM);
    EXPECT_EQ(WSError::WS_ERROR_NULLPTR, result);

    sceneSession->sessionStage_ = new SessionStageMocker();
    EXPECT_NE(nullptr, sceneSession->sessionStage_);
    result = sceneSession->UpdateAvoidArea(nullptr, AvoidAreaType::TYPE_SYSTEM);
    EXPECT_EQ(WSError::WS_OK, result);
}

/**
 * @tc.name: ChangeSessionVisibilityWithStatusBar
 * @tc.desc: normal function
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

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
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

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
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

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
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

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
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

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
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

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    EXPECT_EQ(sceneSession->combinedExtWindowFlags_.bitData, 0);
    sceneSession->UpdateExtWindowFlags(1234, ExtensionWindowFlags(3), ExtensionWindowFlags(3));
    sceneSession->UpdateExtWindowFlags(5678, ExtensionWindowFlags(4), ExtensionWindowFlags(4));
    sceneSession->CalculateCombinedExtWindowFlags();
    EXPECT_EQ(sceneSession->combinedExtWindowFlags_.bitData, 7);
}

/**
 * @tc.name: SaveUpdatedIcon
 * @tc.desc: SaveUpdatedIcon
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SaveUpdatedIcon, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SaveUpdatedIcon";
    info.bundleName_ = "SaveUpdatedIcon";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);

    sceneSession->scenePersistence_ = new ScenePersistence("OpenHarmony", 1);
    EXPECT_NE(nullptr, sceneSession->scenePersistence_);
    sceneSession->SaveUpdatedIcon(nullptr);
}

/**
 * @tc.name: NotifyTouchOutside
 * @tc.desc: NotifyTouchOutside
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, NotifyTouchOutside, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyTouchOutside";
    info.bundleName_ = "NotifyTouchOutside";
    sptr<SceneSession> sceneSession = new SceneSession(info, nullptr);

    sceneSession->sessionStage_ = new SessionStageMocker();
    EXPECT_NE(nullptr, sceneSession->sessionStage_);
    sceneSession->NotifyTouchOutside();

    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    EXPECT_NE(nullptr, sceneSession->sessionChangeCallback_);
    auto func = [sceneSession]() {
        sceneSession->SaveUpdatedIcon(nullptr);
    };
    sceneSession->onTouchOutside_ = func;
    EXPECT_NE(nullptr, &func);
    sceneSession->sessionStage_ = nullptr;
    sceneSession->NotifyTouchOutside();

    sceneSession->onTouchOutside_ = nullptr;
    sceneSession->sessionStage_ = nullptr;
    sceneSession->NotifyTouchOutside();
}

/**
 * @tc.name: CheckTouchOutsideCallbackRegistered
 * @tc.desc: CheckTouchOutsideCallbackRegistered
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, CheckTouchOutsideCallbackRegistered, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "CheckTouchOutsideCallbackRegistered";
    info.bundleName_ = "CheckTouchOutsideCallbackRegistered";
    sptr<SceneSession> sceneSession = new SceneSession(info, nullptr);

    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    EXPECT_NE(nullptr, sceneSession->sessionChangeCallback_);
    auto func = [sceneSession]() {
        sceneSession->NotifyWindowVisibility();
    };
    sceneSession->onTouchOutside_ = func;
    bool result = sceneSession->CheckTouchOutsideCallbackRegistered();
    EXPECT_EQ(true, result);

    sceneSession->onTouchOutside_ = nullptr;
    result = sceneSession->CheckTouchOutsideCallbackRegistered();
    EXPECT_EQ(false, result);

    sceneSession->sessionChangeCallback_ = nullptr;
    result = sceneSession->CheckTouchOutsideCallbackRegistered();
    EXPECT_EQ(false, result);
}

/**
 * @tc.name: RegisterTouchOutsideCallback
 * @tc.desc: test RegisterTouchOutsideCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, RegisterTouchOutsideCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "RegisterTouchOutsideCallback";
    info.bundleName_ = "RegisterTouchOutsideCallback";
    sptr<SceneSession> sceneSession = new SceneSession(info, nullptr);
    sceneSession->onTouchOutside_ = nullptr;
    NotifyTouchOutsideFunc func = []() {};
    sceneSession->RegisterTouchOutsideCallback(std::move(func));

    ASSERT_NE(sceneSession->onTouchOutside_, nullptr);
}

/**
 * @tc.name: UpdateRotationAvoidArea
 * @tc.desc: UpdateRotationAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, UpdateRotationAvoidArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRotationAvoidArea";
    info.bundleName_ = "UpdateRotationAvoidArea";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSession->specificCallback_ = new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, sceneSession->specificCallback_);
    auto func = [sceneSession](const int32_t& persistentId) {
        sceneSession->SetParentPersistentId(persistentId);
        sceneSession->GetParentPersistentId();
    };
    sceneSession->specificCallback_->onUpdateAvoidArea_ = func;
    sptr<Session> session;
    session = new (std::nothrow) Session(info);
    int result = session->GetPersistentId();
    EXPECT_EQ(0, result);
    sceneSession->UpdateRotationAvoidArea();

    sceneSession->specificCallback_ = nullptr;
    sceneSession->UpdateRotationAvoidArea();
}

/**
 * @tc.name: NotifyForceHideChange
 * @tc.desc: NotifyForceHideChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, NotifyForceHideChange, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyForceHideChange";
    info.bundleName_ = "NotifyForceHideChange";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSession->NotifyForceHideChange(true);

    sptr<Session> session = new (std::nothrow) Session(info);
    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    auto func = [sceneSession](bool hide) {
        sceneSession->SetPrivacyMode(hide);
    };
    sceneSession->onForceHideChangeFunc_ = func;
    EXPECT_NE(nullptr, &func);
    sceneSession->NotifyForceHideChange(true);

    sceneSession->SetSessionProperty(nullptr);
    sceneSession->NotifyForceHideChange(true);
    ASSERT_EQ(sceneSession->property_, nullptr);
}

/**
 * @tc.name: RegisterSessionChangeCallback
 * @tc.desc: RegisterSessionChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, RegisterSessionChangeCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "RegisterSessionChangeCallback";
    info.bundleName_ = "RegisterSessionChangeCallback";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sptr<SceneSession::SessionChangeCallback> callback = new SceneSession::SessionChangeCallback();
    EXPECT_NE(nullptr, callback);
    sceneSession->RegisterSessionChangeCallback(callback);
}

/**
 * @tc.name: ClearSpecificSessionCbMap
 * @tc.desc: ClearSpecificSessionCbMap
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, ClearSpecificSessionCbMap, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ClearSpecificSessionCbMap";
    info.bundleName_ = "ClearSpecificSessionCbMap";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, sceneSession);
    sceneSession->ClearSpecificSessionCbMap();
}

/**
 * @tc.name: SendPointerEventToUI
 * @tc.desc: SendPointerEventToUI
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SendPointerEventToUI, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SendPointerEventToUI";
    info.bundleName_ = "SendPointerEventToUI";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, sceneSession);
    auto pointerEventFunc = [sceneSession](std::shared_ptr<MMI::PointerEvent> pointerEvent) {
        sceneSession->NotifyOutsideDownEvent(pointerEvent);
    };
    sceneSession->systemSessionPointerEventFunc_ = pointerEventFunc;
    std::shared_ptr<MMI::PointerEvent> pointerEvent =
         MMI::PointerEvent::Create();
    sceneSession->SendPointerEventToUI(pointerEvent);
    EXPECT_NE(nullptr, pointerEvent);
}

/**
 * @tc.name: SetFloatingScale
 * @tc.desc: SetFloatingScale
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetFloatingScale, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetFloatingScale";
    info.bundleName_ = "SetFloatingScale";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSession->specificCallback_ = new SceneSession::SpecificSessionCallback();
    auto windowInfoUpdateFun = [sceneSession](int32_t persistentId, WindowUpdateType type) {
        if (WindowUpdateType::WINDOW_UPDATE_PROPERTY == type) {
            sceneSession->SetCollaboratorType(persistentId);
            return;
        } else {
            persistentId++;
            sceneSession->SetParentPersistentId(persistentId);
            return;
        }
    };
    auto updateAvoidAreaFun = [sceneSession](const int32_t& persistentId) {
        bool result = sceneSession->RemoveSubSession(persistentId);
        sceneSession->SetWindowAnimationFlag(result);
    };
    sceneSession->specificCallback_->onWindowInfoUpdate_ = windowInfoUpdateFun;
    sceneSession->specificCallback_->onUpdateAvoidArea_ = updateAvoidAreaFun;
    sceneSession->SetFloatingScale(3.14f);
    EXPECT_EQ(3.14f, sceneSession->floatingScale_);

    sceneSession->floatingScale_ = 3.0f;
    sceneSession->SetFloatingScale(3.0f);
    EXPECT_EQ(3.0f, sceneSession->floatingScale_);
}

/**
 * @tc.name: ProcessPointDownSession
 * @tc.desc: ProcessPointDownSession
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, ProcessPointDownSession, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ProcessPointDownSession";
    info.bundleName_ = "ProcessPointDownSession";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    sceneSession->specificCallback_ = new SceneSession::SpecificSessionCallback();
    EXPECT_NE(nullptr, sceneSession->specificCallback_);
    auto sessionTouchOutsideFun = [sceneSession](int32_t persistentId) {
        sceneSession->SetCollaboratorType(persistentId);
    };
    auto outsideDownEventFun = [sceneSession](int32_t x, int32_t y) {
        int z = x + y;
        sceneSession->SetCollaboratorType(z);
    };
    sceneSession->specificCallback_->onSessionTouchOutside_ = sessionTouchOutsideFun;
    sceneSession->specificCallback_->onOutsideDownEvent_ = outsideDownEventFun;
    EXPECT_EQ(WSError::WS_OK, sceneSession->ProcessPointDownSession(3, 4));

    sceneSession->specificCallback_->onSessionTouchOutside_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, sceneSession->ProcessPointDownSession(3, 4));

    sceneSession->sessionInfo_.bundleName_ = "SCBGestureBack";
    sceneSession->specificCallback_->onOutsideDownEvent_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, sceneSession->ProcessPointDownSession(3, 4));
}

/**
 * @tc.name: SetSelfToken
 * @tc.desc: SetSelfToken
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetSelfToken, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSelfToken";
    info.bundleName_ = "SetSelfToken";
    sptr<SceneSession> sceneSession;
    sptr<SceneSession::SpecificSessionCallback> specificSession =
        new SceneSession::SpecificSessionCallback();
    sceneSession = new (std::nothrow) SceneSession(info, specificSession);
    EXPECT_NE(nullptr, sceneSession);
    sceneSession->SetSessionState(SessionState::STATE_DISCONNECT);
    sceneSession->UpdateSessionState(SessionState::STATE_CONNECT);
    sceneSession->UpdateSessionState(SessionState::STATE_ACTIVE);
    sceneSession->isVisible_ = true;
    EXPECT_EQ(true, sceneSession->IsVisibleForAccessibility());
    sceneSession->SetSystemTouchable(false);
    EXPECT_EQ(false, sceneSession->IsVisibleForAccessibility());
    sceneSession->SetForegroundInteractiveStatus(true);
    sceneSession->NotifyAccessibilityVisibilityChange();
    auto windowInfoUpdateFun = [sceneSession](int32_t persistentId, WindowUpdateType type) {
        if (WindowUpdateType::WINDOW_UPDATE_PROPERTY == type) {
            sceneSession->SetCollaboratorType(persistentId);
            return;
        } else {
            persistentId++;
            sceneSession->SetParentPersistentId(persistentId);
            return;
        }
    };
    sceneSession->specificCallback_->onWindowInfoUpdate_ = windowInfoUpdateFun;
    sceneSession->NotifyAccessibilityVisibilityChange();
    EXPECT_NE(nullptr, sceneSession->specificCallback_);
}

/**
 * @tc.name: SetLastSafeRect
 * @tc.desc: SetLastSafeRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetLastSafeRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetLastSafeRect";
    info.bundleName_ = "SetLastSafeRect";
    sptr<SceneSession> sceneSession;
    sptr<SceneSession::SpecificSessionCallback> specificSession =
        new SceneSession::SpecificSessionCallback();
    sceneSession = new (std::nothrow) SceneSession(info, specificSession);
    WSRect rect = {3, 4, 5, 6};
    sceneSession->SetLastSafeRect(rect);
    WSRect result = sceneSession->GetLastSafeRect();
    EXPECT_EQ(3, result.posX_);
    EXPECT_EQ(4, result.posY_);
    EXPECT_EQ(5, result.width_);
    EXPECT_EQ(6, result.height_);
}

/**
 * @tc.name: GetSessionTargetRect
 * @tc.desc: GetSessionTargetRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, GetSessionTargetRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetSubSession";
    info.bundleName_ = "GetSessionTargetRect";
    sptr<SceneSession> sceneSession;
    sptr<SceneSession::SpecificSessionCallback> specificSession =
        new SceneSession::SpecificSessionCallback();
    sceneSession = new (std::nothrow) SceneSession(info, specificSession);
    bool res = sceneSession->AddSubSession(sceneSession);
    EXPECT_EQ(true, res);
    EXPECT_EQ(sceneSession, (sceneSession->GetSubSession())[0]);
    sceneSession->moveDragController_ = new MoveDragController(1024, WindowType::WINDOW_TYPE_FLOAT);
    WSRect rectResult = sceneSession->GetSessionTargetRect();
    EXPECT_EQ(0, rectResult.posX_);
    EXPECT_EQ(0, rectResult.width_);
    auto dragHotAreaFunc = [sceneSession](DisplayId displayId, int32_t type, SizeChangeReason reason) {
        if (SizeChangeReason::END == reason) {
            GTEST_LOG_(INFO) << "type = " << type;
        }
        return;
    };
    sceneSession->SetWindowDragHotAreaListener(dragHotAreaFunc);
    EXPECT_NE(nullptr,  sceneSession->moveDragController_);
    sceneSession->moveDragController_ = nullptr;
    rectResult = sceneSession->GetSessionTargetRect();
    sceneSession->SetWindowDragHotAreaListener(dragHotAreaFunc);
    ASSERT_EQ(0, rectResult.width_);
}

/**
 * @tc.name: SetPipActionEvent
 * @tc.desc: SetPipActionEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetPipActionEvent, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetPipActionEvent";
    info.bundleName_ = "SetPipActionEvent";
    auto sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    WSError res = sceneSession->SetPipActionEvent("close", 0);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
    auto mockSessionStage = sptr<SessionStageMocker>::MakeSptr();
    sceneSession->sessionStage_ = mockSessionStage;
    res = sceneSession->SetPipActionEvent("close", 0);
    ASSERT_EQ(res, WSError::WS_OK);
}

/**
 * @tc.name: SetPiPControlEvent
 * @tc.desc: SetPiPControlEvent
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
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
 * @tc.desc: ClearSpecificSessionCbMap
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, ClearSpecificSessionCbMap01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ClearSpecificSessionCbMap01";
    info.bundleName_ = "ClearSpecificSessionCbMap01";
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, sceneSession);
    sptr<Session> session;
    session = new (std::nothrow) Session(info);
    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    sceneSession->ClearSpecificSessionCbMap();

    bool isFromClient = true;
    sceneSession->needSnapshot_ = true;
    sceneSession->specificCallback_ = new (std::nothrow) SceneSession::SpecificSessionCallback();
    sceneSession->Disconnect(isFromClient);

    bool isActive = false;
    sceneSession->UpdateActiveStatus(isActive);
    isActive = true;
    Session ssession(info);
    ssession.state_ = SessionState::STATE_FOREGROUND;
    isActive = false;
    ssession.state_ = SessionState::STATE_ACTIVE;
}

/**
 * @tc.name: OnSessionEvent01
 * @tc.desc: OnSessionEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, OnSessionEvent01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnSessionEvent01";
    info.bundleName_ = "OnSessionEvent01";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    sceneSession->leashWinSurfaceNode_ = nullptr;
    SessionEvent event = SessionEvent::EVENT_START_MOVE;
    sceneSession->moveDragController_ = new MoveDragController(1, WindowType::WINDOW_TYPE_FLOAT);
    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    sceneSession->OnSessionEvent(event);

    sceneSession->moveDragController_->isStartDrag_ = true;
    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    EXPECT_NE(sceneSession->sessionChangeCallback_, nullptr);
    auto result = sceneSession->OnSessionEvent(event);
    ASSERT_EQ(result, WSError::WS_OK);
    event = SessionEvent::EVENT_DRAG_START;
    ASSERT_EQ(sceneSession->OnSessionEvent(event), WSError::WS_OK);
}

/**
 * @tc.name: SetSessionRectChangeCallback
 * @tc.desc: SetSessionRectChangeCallback
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
 * @tc.desc: SetSessionPiPControlStatusChangeCallback
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
 * @tc.name: SetAutoStartPiPStatusChangeCallback
 * @tc.desc: SetAutoStartPiPStatusChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetAutoStartPiPStatusChangeCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAutoStartPiPStatusChangeCallback";
    info.bundleName_ = "SetAutoStartPiPStatusChangeCallback";
    auto sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    NotifyAutoStartPiPStatusChangeFunc func;
    sceneSession->SetAutoStartPiPStatusChangeCallback(func);
}

/**
 * @tc.name: RaiseAppMainWindowToTop
 * @tc.desc: RaiseAppMainWindowToTop
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, RaiseAppMainWindowToTop, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "RaiseAppMainWindowToTop";
    info.bundleName_ = "RaiseAppMainWindowToTop";

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);

    WSError result = sceneSession->RaiseAppMainWindowToTop();
    EXPECT_EQ(WSError::WS_OK, result);
    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    bool status = true;
    sceneSession->OnNeedAvoid(status);

    bool showWhenLocked = true;
    sceneSession->OnShowWhenLocked(showWhenLocked);
    sceneSession->NotifyPropertyWhenConnect();

    sceneSession->focusedOnShow_ = false;
    result = sceneSession->RaiseAppMainWindowToTop();
    EXPECT_EQ(WSError::WS_OK, result);
}

/**
 * @tc.name: GetKeyboardAvoidArea
 * @tc.desc: GetKeyboardAvoidArea01
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
 * @tc.desc: GetCutoutAvoidArea
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
 * @tc.desc: GetAINavigationBarArea
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
    sceneSession->specificCallback_ = new SceneSession::SpecificSessionCallback();
    ASSERT_NE(nullptr, sceneSession->specificCallback_);
    sceneSession->specificCallback_->onGetAINavigationBarArea_ = [](uint64_t displayId) {
        WSRect rect = {1, 1, 1, 1};
        return rect;
    };
    sceneSession->GetAINavigationBarArea(rect, avoidArea);
}

/**
 * @tc.name: TransferPointerEvent
 * @tc.desc: TransferPointerEvent
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
    sceneSession->AdjustRectByLimits(limits, ratio, isDecor, vpr, rect);
    sceneSession->SetPipActionEvent("pointerEvent", 0);

    auto property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    sceneSession->SetSessionProperty(property);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, sceneSession->SetPipActionEvent("pointerEvent", 0));

    sceneSession->sessionStage_ = sptr<SessionStageMocker>::MakeSptr();
    property->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    property->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sceneSession->AdjustRectByLimits(limits, ratio, false, vpr, rect);
    ASSERT_EQ(WSError::WS_OK, sceneSession->SetPipActionEvent("pointerEvent", 0));
}

/**
 * @tc.name: OnMoveDragCallback
 * @tc.desc: OnMoveDragCallback
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
    sceneSession->SetSurfaceBounds(rect, false);
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
 * @tc.desc: OnMoveDragCallback
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
    sceneSession->NotifyIsCustomAnimationPlaying(isPlaying);

    sptr<AAFwk::SessionInfo> abilitySessionInfo = nullptr;
    bool visible = true;
    sceneSession->ChangeSessionVisibilityWithStatusBar(abilitySessionInfo, visible);
}

/**
 * @tc.name: OnMoveDragCallback
 * @tc.desc: OnMoveDragCallback
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
    session.sessionExceptionFunc_ = [](const SessionInfo& info, bool removeSession, bool startFail) {};
    session.jsSceneSessionExceptionFunc_ = [](const SessionInfo& info, bool removeSession, bool startFail) {};
    sceneSession->NotifySessionException(abilitySessionInfo, needRemoveSession);

    sceneSession->sessionChangeCallback_ = new SceneSession::SessionChangeCallback();
    sceneSession->NotifyPiPWindowPrepareClose();

    bool isLandscapeMultiWindow = true;
    sceneSession->SetLandscapeMultiWindow(isLandscapeMultiWindow);

    std::shared_ptr<MMI::KeyEvent> keyEvent = std::make_shared<MMI::KeyEvent>(MMI::KeyEvent::KEYCODE_BACK);
    bool isPreImeEvent = true;
    sceneSession->SendKeyEventToUI(keyEvent, isPreImeEvent);
    sceneSession->IsDirtyWindow();
    sceneSession->moveDragController_ = new MoveDragController(0, WindowType::WINDOW_TYPE_FLOAT);
    sceneSession->NotifyUILostFocus();
}

/**
 * @tc.name: IsStartMoving
 * @tc.desc: IsStartMoving
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
    DisplayId from = 0;
    DisplayId to = 0;
    sceneSession->NotifyDisplayMove(from, to);
    sceneSession->ClearExtWindowFlags();
    bool isRegister = true;
    sceneSession->UpdateRectChangeListenerRegistered(isRegister);
    
    sceneSession->sessionStage_ = new SessionStageMocker();
    EXPECT_NE(nullptr, sceneSession->sessionStage_);
    sceneSession->NotifyDisplayMove(from, to);
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
 * @tc.desc: SetTemporarilyShowWhenLocked
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

    sceneSession->isTemporarilyShowWhenLocked_.store(true);
    sceneSession->SetTemporarilyShowWhenLocked(true);
    ASSERT_EQ(sceneSession->IsTemporarilyShowWhenLocked(), true);
}

/**
 * @tc.name: GetShowWhenLockedFlagValue
 * @tc.desc: GetShowWhenLockedFlagValue
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
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(sceneSession->GetShowWhenLockedFlagValue(), false);
    sceneSession->property_ = property;
    property->SetWindowFlags(4);
    ASSERT_EQ(sceneSession->GetShowWhenLockedFlagValue(), true);
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
    sceneSession->AddOrUpdateWindowDragHotArea(0, 1, rect);
    sceneSession->AddOrUpdateWindowDragHotArea(0, 1, rect);
    auto type = sceneSession->GetWindowDragHotAreaType(0, 1, 2, 2);
    ASSERT_EQ(type, 1);
}

/**
 * @tc.name: SetSessionModalTypeChangeCallback
 * @tc.desc: SetSessionModalTypeChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetSessionModalTypeChangeCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSessionModalTypeChangeCallback";
    info.bundleName_ = "SetSessionModalTypeChangeCallback";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->SetSessionModalTypeChangeCallback([](SubWindowModalType subWindowModalType) {
        return;
    });
    EXPECT_NE(sceneSession->onSessionModalTypeChange_, nullptr);
}

/**
 * @tc.name: OnSessionModalTypeChange
 * @tc.desc: OnSessionModalTypeChange
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, OnSessionModalTypeChange, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnSessionModalTypeChange";
    info.bundleName_ = "OnSessionModalTypeChange";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->SetSessionModalTypeChangeCallback([](SubWindowModalType subWindowModalType) {
        return;
    });
    EXPECT_NE(sceneSession->onSessionModalTypeChange_, nullptr);
    EXPECT_EQ(sceneSession->OnSessionModalTypeChange(SubWindowModalType::TYPE_WINDOW_MODALITY), WSError::WS_OK);
}

/**
 * @tc.name: SetMainSessionModalTypeChangeCallback
 * @tc.desc: SetMainSessionModalTypeChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetMainSessionModalTypeChangeCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetMainSessionModalTypeChangeCallback";
    info.bundleName_ = "SetMainSessionModalTypeChangeCallback";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->SetMainSessionModalTypeChangeCallback([](bool isModal) {
        return;
    });
    EXPECT_NE(sceneSession->onMainSessionModalTypeChange_, nullptr);
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

    sceneSession->onWindowAnimationFlagChange_ = [](bool isNeedDefaultAnimationFlag) {};
    sceneSession->SetWindowAnimationFlag(true);
    ASSERT_EQ(true, sceneSession->needDefaultAnimationFlag_);
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
 * @tc.name: SetTitleAndDockHoverShowChangeCallback
 * @tc.desc: SetTitleAndDockHoverShowChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest2, SetTitleAndDockHoverShowChangeCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetTitleAndDockHoverShowChangeCallback";
    info.bundleName_ = "SetTitleAndDockHoverShowChangeCallback";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->SetTitleAndDockHoverShowChangeCallback([](bool isTitleHoverShown, bool isDockHoverShown) {
        return;
    });
    EXPECT_NE(sceneSession->onTitleAndDockHoverShowChangeFunc_, nullptr);
}
}
}
}