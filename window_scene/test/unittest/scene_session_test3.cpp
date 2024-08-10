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

#include "display_manager.h"
#include "input_event.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "pointer_event.h"

#include "session/host/include/main_session.h"
#include "session/host/include/keyboard_session.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/system_session.h"
#include "window_helper.h"
#include "wm_common.h"


using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionTest3 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionTest3::SetUpTestCase()
{
}

void SceneSessionTest3::TearDownTestCase()
{
}

void SceneSessionTest3::SetUp()
{
}

void SceneSessionTest3::TearDown()
{
}

namespace {

/**
 * @tc.name: SetAspectRatio11
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetAspectRatio1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio11";
    info.bundleName_ = "SetAspectRatio11";
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;
    scensession->property_ = nullptr;
    float ratio = 0.0001;
    auto result = scensession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);
}


/**
 * @tc.name: SetAspectRatio12
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetAspectRatio12, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio12";
    info.bundleName_ = "SetAspectRatio12";
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    float ratio = 0.0001;
    scensession->moveDragController_ = nullptr;
    auto result = scensession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
    scensession->moveDragController_ = new (std::nothrow) MoveDragController(0);
    result = scensession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    scensession->SetSessionProperty(property);
    result = scensession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: SetAspectRatio15
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetAspectRatio15, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio15";
    info.bundleName_ = "SetAspectRatio15";
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    scensession->SetSessionProperty(property);
    WindowLimits limits;
    limits.maxHeight_ = 10;
    limits.minWidth_ = 0;
    property->SetWindowLimits(limits);
    scensession->SetAspectRatio(ratio);

    limits.maxHeight_ = 0;
    limits.minWidth_ = 10;
    property->SetWindowLimits(limits);
    scensession->SetAspectRatio(ratio);
    EXPECT_NE(scensession, nullptr);
}

/**
 * @tc.name: SetAspectRatio8
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetAspectRatio8, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio8";
    info.bundleName_ = "SetAspectRatio8";
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    scensession->SetSessionProperty(property);

    float ratio = 0.1;
    WindowLimits limits;
    limits.maxHeight_ = 10;
    limits.minWidth_ = 10;
    property->SetWindowLimits(limits);
    auto result = scensession->SetAspectRatio(ratio);

    ratio = 10;
    result = scensession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: UpdateRect1
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, UpdateRect1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRect1";
    info.bundleName_ = "UpdateRect1";
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);

    scensession->SetSessionProperty(property);
    WSRect rect({1, 1, 1, 1});
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError result = scensession->UpdateRect(rect, reason);
    ASSERT_EQ(result, WSError::WS_OK);

    scensession->winRect_ = rect;
    result = scensession->UpdateRect(rect, reason);
    ASSERT_EQ(result, WSError::WS_OK);

    scensession->reason_ = SizeChangeReason::DRAG_END;
    result = scensession->UpdateRect(rect, reason);
    ASSERT_EQ(result, WSError::WS_OK);

    WSRect rect2({0, 0, 0, 0});
    result = scensession->UpdateRect(rect2, reason);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: FixKeyboardPositionByKeyboardPanel
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, FixKeyboardPositionByKeyboardPanel, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "FixKeyboardPositionByKeyboardPanel";
    info.bundleName_ = "FixKeyboardPositionByKeyboardPanel";
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);

    scensession->SetSessionProperty(property);
    scensession->FixKeyboardPositionByKeyboardPanel(nullptr, nullptr);
    ASSERT_NE(scensession, nullptr);
}

/**
 * @tc.name: FixKeyboardPositionByKeyboardPanel1
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, FixKeyboardPositionByKeyboardPanel1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "FixKeyboardPositionByKeyboardPanel1";
    info.bundleName_ = "FixKeyboardPositionByKeyboardPanel1";
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scenceSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scenceSession, nullptr);
    scenceSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);

    scenceSession->SetSessionProperty(property);
    scenceSession->FixKeyboardPositionByKeyboardPanel(scenceSession, scenceSession);
    ASSERT_NE(scenceSession, nullptr);

    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    property->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_FLOAT, 1);
    scenceSession->FixKeyboardPositionByKeyboardPanel(scenceSession, keyboardSession);
}

/**
 * @tc.name: NotifyClientToUpdateRectTask
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, NotifyClientToUpdateRectTask, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRectTask";
    info.bundleName_ = "NotifyClientToUpdateRectTask";
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);

    sceneSession->SetSessionProperty(property);
    sceneSession->SetSessionState(SessionState::STATE_ACTIVE);
    auto result = sceneSession->NotifyClientToUpdateRectTask(nullptr);
    ASSERT_EQ(result, WSError::WS_OK);

    property->SetWindowType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    sceneSession->SetSessionProperty(property);
    sceneSession->isKeyboardPanelEnabled_ = true;
    ASSERT_EQ(WSError::WS_OK, sceneSession->NotifyClientToUpdateRectTask(nullptr));

    property->SetWindowType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    sceneSession->SetSessionProperty(property);
    sceneSession->isKeyboardPanelEnabled_ = true;
    ASSERT_EQ(WSError::WS_OK, sceneSession->NotifyClientToUpdateRectTask(nullptr));

    std::shared_ptr<RSTransaction> rs;
    ASSERT_EQ(WSError::WS_OK, sceneSession->NotifyClientToUpdateRectTask(rs));
}

/**
 * @tc.name: BindDialogSessionTarget1
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, BindDialogSessionTarget1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "BindDialogSessionTarget1";
    info.bundleName_ = "BindDialogSessionTarget1";
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(scensession, nullptr);
    scensession->isActive_ = true;

    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    uint32_t p = 10;
    property->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, p);
    scensession->SetSessionProperty(property);

    sptr<SceneSession> sceneSession = nullptr;
    WSError result = scensession->BindDialogSessionTarget(sceneSession);
    ASSERT_EQ(result, WSError::WS_ERROR_NULLPTR);

    scensession->sessionChangeCallback_ = nullptr;
    sptr<SceneSession> sceneSession1 = scensession;
    result = scensession->BindDialogSessionTarget(sceneSession1);
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession1->sessionChangeCallback_ = new (std::nothrow) MainSession::SessionChangeCallback();
    EXPECT_NE(sceneSession1->sessionChangeCallback_, nullptr);
    sceneSession1->sessionChangeCallback_->onBindDialogTarget_ = [](const sptr<SceneSession>&) {};
    result = scensession->BindDialogSessionTarget(sceneSession1);
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: HandlePointerStyle1
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, HandlePointerStyle1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandlePointerStyle1";
    info.bundleName_ = "HandlePointerStyle1";
    info.windowType_ = 1;
    sptr<SceneSession::SpecificSessionCallback> specificCallback_ =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCallback_, nullptr);
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, specificCallback_);
    EXPECT_NE(scensession, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = nullptr;
    ASSERT_EQ(scensession->HandlePointerStyle(pointerEvent), WSError::WS_ERROR_NULLPTR);

    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetMaximizeMode(MaximizeMode::MODE_FULL_FILL);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetPersistentId(11);
    scensession->property_ = property;

    std::shared_ptr<MMI::PointerEvent> pointerEvent_ = MMI::PointerEvent::Create();
    pointerEvent_->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
    pointerEvent_->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
    pointerEvent_->SetButtonId(MMI::PointerEvent::POINTER_ACTION_MOVE);
    ASSERT_NE(scensession->HandlePointerStyle(pointerEvent_), WSError::WS_OK);
}

/**
 * @tc.name: ClearSpecificSessionCbMap1
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, ClearSpecificSessionCbMap1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ClearSpecificSessionCbMap1";
    info.bundleName_ = "ClearSpecificSessionCbMap1";
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, scensession);
    scensession->ClearSpecificSessionCbMap();

    sptr<MainSession::SessionChangeCallback> sessionChangeCallback =
        new (std::nothrow) MainSession::SessionChangeCallback();
    sessionChangeCallback->clearCallbackFunc_ = [](bool, int32_t) {};
    scensession->sessionChangeCallback_ = sessionChangeCallback;
    scensession->ClearSpecificSessionCbMap();
}

/**
 * @tc.name: IsMovableWindowType
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, IsMovableWindowType, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "IsMovableWindowType";
    info.bundleName_ = "IsMovableWindowType";
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, scensession);

    scensession->property_ = nullptr;
    EXPECT_EQ(scensession->IsMovableWindowType(), false);
}

/**
 * @tc.name: SetBlankFlag
 * @tc.desc: check func SetBlankFlag
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetBlankFlag, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetBlankFlag";
    info.bundleName_ = "SetBlankFlag";
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, scensession);

    bool isAddBlank = true;
    scensession->SetBlankFlag(isAddBlank);
    ASSERT_EQ(isAddBlank, scensession->GetBlankFlag());
}

/**
 * @tc.name: GetBlankFlag
 * @tc.desc: check func GetBlankFlag
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, GetBlankFlag, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetBlankFlag";
    info.bundleName_ = "GetBlankFlag";
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, scensession);

    bool isAddBlank = true;
    scensession->SetBlankFlag(isAddBlank);
    ASSERT_EQ(isAddBlank, scensession->GetBlankFlag());
}

/**
 * @tc.name: SetBufferAvailableCallbackEnable
 * @tc.desc: check func SetBufferAvailableCallbackEnable
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetBufferAvailableCallbackEnable, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetBufferAvailableCallbackEnable";
    info.bundleName_ = "SetBufferAvailableCallbackEnable";
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, scensession);

    bool enable = true;
    scensession->SetBufferAvailableCallbackEnable(enable);
    ASSERT_EQ(enable, scensession->GetBufferAvailableCallbackEnable());
}

/**
 * @tc.name: GetBufferAvailableCallbackEnable
 * @tc.desc: check func GetBufferAvailableCallbackEnable
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, GetBufferAvailableCallbackEnable, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetBufferAvailableCallbackEnable";
    info.bundleName_ = "GetBufferAvailableCallbackEnable";
    sptr<SceneSession> scensession = new (std::nothrow) SceneSession(info, nullptr);
    EXPECT_NE(nullptr, scensession);

    bool enable = true;
    scensession->SetBufferAvailableCallbackEnable(enable);
    ASSERT_EQ(enable, scensession->GetBufferAvailableCallbackEnable());
}

}
}
}