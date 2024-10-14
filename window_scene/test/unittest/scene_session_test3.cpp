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
HWTEST_F(SceneSessionTest3, SetAspectRatio11, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio11";
    info.bundleName_ = "SetAspectRatio11";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;
    sceneSession->property_ = nullptr;
    float ratio = 0.0001;
    auto result = sceneSession->SetAspectRatio(ratio);
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
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 0.0001;
    sceneSession->moveDragController_ = nullptr;
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_OK);
    ASSERT_EQ(sceneSession->GetAspectRatio(), ratio);

    sceneSession->moveDragController_ = sptr<MoveDragController>::MakeSptr(0);
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
 * @tc.name: SetAspectRatio15
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetAspectRatio15, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAspectRatio15";
    info.bundleName_ = "SetAspectRatio15";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->isActive_ = true;

    float ratio = 0.1;
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sceneSession->SetSessionProperty(property);
    WindowLimits limits;
    limits.maxHeight_ = 10;
    limits.minWidth_ = 0;
    property->SetWindowLimits(limits);
    auto result = sceneSession->SetAspectRatio(ratio);
    ASSERT_EQ(result, WSError::WS_ERROR_INVALID_PARAM);
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
 * @tc.name: UpdateRect1
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, UpdateRect1, Function | SmallTest | Level2)
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
    WSError result = sceneSession->UpdateRect(rect, reason, "SceneSessionTest3");
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->winRect_ = rect;
    result = sceneSession->UpdateRect(rect, reason, "SceneSessionTest3");
    ASSERT_EQ(result, WSError::WS_OK);

    sceneSession->reason_ = SizeChangeReason::DRAG_END;
    result = sceneSession->UpdateRect(rect, reason, "SceneSessionTest3");
    ASSERT_EQ(result, WSError::WS_OK);

    WSRect rect2({0, 0, 0, 0});
    result = sceneSession->UpdateRect(rect2, reason, "SceneSessionTest3");
    ASSERT_EQ(result, WSError::WS_OK);
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
    auto result = sceneSession->NotifyClientToUpdateRectTask("SceneSessionTest3", nullptr);
    ASSERT_EQ(result, WSError::WS_OK);

    property->SetWindowType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    sceneSession->SetSessionProperty(property);
    sceneSession->isKeyboardPanelEnabled_ = true;
    ASSERT_EQ(WSError::WS_OK, sceneSession->NotifyClientToUpdateRectTask("SceneSessionTest3", nullptr));

    property->SetWindowType(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    sceneSession->SetSessionProperty(property);
    sceneSession->isKeyboardPanelEnabled_ = true;
    ASSERT_EQ(WSError::WS_OK, sceneSession->NotifyClientToUpdateRectTask("SceneSessionTest3", nullptr));

    std::shared_ptr<RSTransaction> rs;
    ASSERT_EQ(WSError::WS_OK, sceneSession->NotifyClientToUpdateRectTask("SceneSessionTest3", rs));
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
    sessionChangeCallback->clearCallbackFunc_ = [](bool) {};
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

/**
 * @tc.name: NotifyClientToUpdateAvoidArea
 * @tc.desc: check func NotifyClientToUpdateAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, NotifyClientToUpdateAvoidArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateAvoidArea";
    info.bundleName_ = "NotifyClientToUpdateAvoidArea";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(nullptr, sceneSession);

    sceneSession->NotifyClientToUpdateAvoidArea();
    EXPECT_EQ(nullptr, sceneSession->specificCallback_);

    sptr<SceneSession::SpecificSessionCallback> callback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    sceneSession = sptr<SceneSession>::MakeSptr(info, callback);
    EXPECT_NE(nullptr, sceneSession);
    sceneSession->persistentId_ = 6;
    callback->onUpdateAvoidArea_ = nullptr;
    sceneSession->NotifyClientToUpdateAvoidArea();

    UpdateAvoidAreaCallback callbackFun = [&sceneSession](int32_t persistentId) {
        sceneSession->RemoveToastSession(persistentId);
        return;
    };
    callback->onUpdateAvoidArea_ = callbackFun;

    callback->onUpdateOccupiedAreaIfNeed_ = nullptr;
    UpdateOccupiedAreaIfNeedCallback updateCallbackFun = [&sceneSession](int32_t persistentId) {
        sceneSession->RemoveToastSession(persistentId);
        return;
    };
    callback->onUpdateOccupiedAreaIfNeed_ = updateCallbackFun;
    sceneSession->NotifyClientToUpdateAvoidArea();
    EXPECT_EQ(6, sceneSession->GetPersistentId());
}

/**
 * @tc.name: UpdateScaleInner
 * @tc.desc: check func UpdateScaleInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, UpdateScaleInner, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateScaleInner";
    info.bundleName_ = "UpdateScaleInner";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(nullptr, sceneSession);

    sceneSession->sessionStage_ = nullptr;
    bool res = sceneSession->UpdateScaleInner(10.0f, 10.0f, 10.0f, 10.0f);
    EXPECT_EQ(true, res);
    res = sceneSession->UpdateScaleInner(10.0f, 9.0f, 10.0f, 10.0f);
    res = sceneSession->UpdateScaleInner(10.0f, 9.0f, 9.0f, 10.0f);
    res = sceneSession->UpdateScaleInner(10.0f, 9.0f, 9.0f, 9.0f);
    EXPECT_EQ(true, res);
    res = sceneSession->UpdateScaleInner(10.0f, 9.0f, 9.0f, 9.0f);
    EXPECT_EQ(false, res);
    sptr<SessionStageMocker> mockSessionStage = sptr<SessionStageMocker>::MakeSptr();
    ASSERT_NE(mockSessionStage, nullptr);
    sceneSession->sessionStage_ = mockSessionStage;
    res = sceneSession->UpdateScaleInner(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_EQ(true, res);
}

/**
 * @tc.name: UpdateZOrderInner
 * @tc.desc: check func UpdateZOrderInner
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, UpdateZOrderInner, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateZOrderInner";
    info.bundleName_ = "UpdateZOrderInner";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(nullptr, sceneSession);

    int res = sceneSession->UpdateZOrderInner(10);
    EXPECT_EQ(true, res);

    res = sceneSession->UpdateZOrderInner(10);
    EXPECT_EQ(false, res);
}

/**
 * @tc.name: GetSubWindowModalType02
 * @tc.desc: GetSubWindowModalType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, GetSubWindowModalType02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ModalType2";
    info.bundleName_ = "ModalType2";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    property->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_IS_TOAST));
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->GetSubWindowModalType();
    ASSERT_EQ(result, SubWindowModalType::TYPE_TOAST);
}

/**
 * @tc.name: GetSubWindowModalType03
 * @tc.desc: GetSubWindowModalType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, GetSubWindowModalType03, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ModalType3";
    info.bundleName_ = "ModalType3";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    property->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_IS_MODAL));
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->GetSubWindowModalType();
    ASSERT_EQ(result, SubWindowModalType::TYPE_WINDOW_MODALITY);
}

/**
 * @tc.name: GetSubWindowModalType04
 * @tc.desc: GetSubWindowModalType
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, GetSubWindowModalType04, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ModalType4";
    info.bundleName_ = "ModalType4";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    property->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    sceneSession->SetSessionProperty(property);
    auto result = sceneSession->GetSubWindowModalType();
    ASSERT_EQ(result, SubWindowModalType::TYPE_NORMAL);
}

/**
 * @tc.name: RegisterDefaultAnimationFlagChangeCallback
 * @tc.desc: RegisterDefaultAnimationFlagChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, RegisterDefaultAnimationFlagChangeCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "RegisterDefaultAnimationFlagChangeCallback";
    info.bundleName_ = "RegisterDefaultAnimationFlagChangeCallback";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->RegisterDefaultAnimationFlagChangeCallback([sceneSession](const bool flag) {
        return;
    });
    sptr<SceneSession::SessionChangeCallback> sessionChangeCallback =
        sptr<SceneSession::SessionChangeCallback>::MakeSptr();
    sceneSession->RegisterSessionChangeCallback(nullptr);
    ASSERT_EQ(WSError::WS_OK, sceneSession->UpdateWindowAnimationFlag(true));

    sceneSession->RegisterSessionChangeCallback(sessionChangeCallback);
    sceneSession->RegisterDefaultAnimationFlagChangeCallback([sceneSession](const bool flag) {
        return;
    });
    ASSERT_EQ(WSError::WS_OK, sceneSession->UpdateWindowAnimationFlag(true));
}

/**
 * @tc.name: SetMainWindowTopmostChangeCallback
 * @tc.desc: SetMainWindowTopmostChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetMainWindowTopmostChangeCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetMainWindowTopmostChangeCallback";
    info.bundleName_ = "SetMainWindowTopmostChangeCallback";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);

    NotifyMainWindowTopmostChangeFunc func;
    sceneSession->SetMainWindowTopmostChangeCallback(func);

    NotifyMainWindowTopmostChangeFunc func1 = [sceneSession](bool isTopmost) {
        return;
    };
    sceneSession->SetMainWindowTopmostChangeCallback(func1);
    ASSERT_NE(nullptr, sceneSession->mainWindowTopmostChangeFunc_);
}

/**
 * @tc.name: SetKeyboardGravityChangeCallback
 * @tc.desc: SetKeyboardGravityChangeCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetKeyboardGravityChangeCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetKeyboardGravityChangeCallback";
    info.bundleName_ = "SetKeyboardGravityChangeCallback";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    sceneSession->property_ = windowSessionProperty;

    NotifyKeyboardGravityChangeFunc func;
    sceneSession->SetKeyboardGravityChangeCallback(func);

    NotifyKeyboardGravityChangeFunc func1 = [sceneSession](SessionGravity gravity) {
        return;
    };
    sceneSession->SetKeyboardGravityChangeCallback(func1);
    ASSERT_EQ(SessionGravity::SESSION_GRAVITY_DEFAULT, sceneSession->GetKeyboardGravity());
}

/**
 * @tc.name: SetRestoreMainWindowCallback
 * @tc.desc: SetRestoreMainWindowCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetRestoreMainWindowCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetRestoreMainWindowCallback";
    info.bundleName_ = "SetRestoreMainWindowCallback";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    sceneSession->property_ = windowSessionProperty;

    NotifyRestoreMainWindowFunc func;
    sceneSession->SetRestoreMainWindowCallback(func);

    NotifyRestoreMainWindowFunc func1 = [sceneSession]() {
        return;
    };
    sceneSession->SetRestoreMainWindowCallback(func1);
    ASSERT_EQ(nullptr, sceneSession->sessionChangeCallback_);

    auto sessionchangeCallback = sptr<SceneSession::SessionChangeCallback>::MakeSptr();
    sceneSession->RegisterSessionChangeCallback(sessionchangeCallback);
    sceneSession->SetRestoreMainWindowCallback(func1);
    ASSERT_NE(nullptr, sceneSession->sessionChangeCallback_);
}

/**
 * @tc.name: SetAdjustKeyboardLayoutCallback
 * @tc.desc: SetAdjustKeyboardLayoutCallback
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetAdjustKeyboardLayoutCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAdjustKeyboardLayoutCallback";
    info.bundleName_ = "SetAdjustKeyboardLayoutCallback";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    sceneSession->property_ = windowSessionProperty;

    NotifyKeyboardLayoutAdjustFunc func;
    sceneSession->SetAdjustKeyboardLayoutCallback(func);

    NotifyKeyboardLayoutAdjustFunc func1 = [sceneSession](const KeyboardLayoutParams& params) {
        return;
    };
    sceneSession->SetAdjustKeyboardLayoutCallback(func1);
    ASSERT_NE(nullptr, sceneSession->adjustKeyboardLayoutFunc_);
}

/**
 * @tc.name: GetStartMoveFlag
 * @tc.desc: GetStartMoveFlag
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, GetStartMoveFlag, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetStartMoveFlag";
    info.bundleName_ = "GetStartMoveFlag";

    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->moveDragController = new MoveDragController(1);
    bool isMoving = false;
    ASSERT_EQ(WSError::WS_OK, sceneSession->GetStartMoveFlag(isMoving));

    sceneSession->moveDragController_ = new MoveDragController(1024);
    ASSERT_EQ(WSError::WS_OK, sceneSession->GetStartMoveFlag(isMoving));
}

/**
 * @tc.name: SetCompatibleWindowSizeInPc
 * @tc.desc: SetCompatibleWindowSizeInPc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetCompatibleWindowSizeInPc, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetCompatibleWindowSizeInPc";
    info.bundleName_ = "SetCompatibleWindowSizeInPc";

    int32_t portraitWidth = 10;
    int32_t portraitHeight = 20;
    int32_t landscapeWidth = 10;
    int32_t landscapeHeight = 20;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->SetSessionProperty(nullptr);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR,
        sceneSession->SetCompatibleWindowSizeInPc(portraitWidth, portraitHeight, landscapeWidth, landscapeHeight));

    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    sceneSession->property_ = windowSessionProperty;
    ASSERT_EQ(WSError::WS_OK,
        sceneSession->SetCompatibleWindowSizeInPc(portraitWidth, portraitHeight, landscapeWidth, landscapeHeight));
}

/**
 * @tc.name: SetAppSupportPhoneInPc
 * @tc.desc: SetAppSupportPhoneInPc
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetAppSupportPhoneInPc, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetAppSupportPhoneInPc";
    info.bundleName_ = "SetAppSupportPhoneInPc";

    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->SetSessionProperty(nullptr);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, sceneSession->SetAppSupportPhoneInPc(false));

    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    sceneSession->property_ = windowSessionProperty;
    ASSERT_EQ(WSError::WS_OK, sceneSession->SetAppSupportPhoneInPc(false));
}

/**
 * @tc.name: CompatibleFullScreenRecover
 * @tc.desc: CompatibleFullScreenRecover
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, CompatibleFullScreenRecover, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "CompatibleFullScreenRecover";
    info.bundleName_ = "CompatibleFullScreenRecover";

    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->SetSessionProperty(nullptr);
    sptr<SessionStageMocker> mockSessionStage = sptr<SessionStageMocker>::MakeSptr();
    sceneSession->sessionStage_ = mockSessionStage;
    sceneSession->SetSessionState(SessionState::STATE_DISCONNECT);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, sceneSession->CompatibleFullScreenRecover());

    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    sceneSession->property_ = windowSessionProperty;
    sceneSession->SetSessionState(SessionState::STATE_CONNECT);
    ASSERT_EQ(WSError::WS_OK, sceneSession->CompatibleFullScreenRecover());
}

/**
 * @tc.name: SetIsPcAppInPad
 * @tc.desc: SetIsPcAppInPad
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, SetIsPcAppInPad, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetIsPcAppInPad";
    info.bundleName_ = "SetIsPcAppInPad";

    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->SetSessionProperty(nullptr);
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, sceneSession->SetIsPcAppInPad(false));

    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    sceneSession->property_ = windowSessionProperty;
    ASSERT_EQ(WSError::WS_OK, sceneSession->SetIsPcAppInPad(false));
}

/**
 * @tc.name: CompatibleFullScreenClose
 * @tc.desc: CompatibleFullScreenClose
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest3, CompatibleFullScreenClose, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "CompatibleFullScreenClose";
    info.bundleName_ = "CompatibleFullScreenClose";

    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->SetSessionProperty(nullptr);
    sptr<SessionStageMocker> mockSessionStage = sptr<SessionStageMocker>::MakeSptr();
    sceneSession->sessionStage_ = mockSessionStage;
    sceneSession->SetSessionState(SessionState::STATE_DISCONNECT);
    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, sceneSession->CompatibleFullScreenClose());

    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    sceneSession->property_ = windowSessionProperty;
    sceneSession->SetSessionState(SessionState::STATE_CONNECT);
    ASSERT_EQ(WSError::WS_OK, sceneSession->CompatibleFullScreenClose());
}
}
}
}