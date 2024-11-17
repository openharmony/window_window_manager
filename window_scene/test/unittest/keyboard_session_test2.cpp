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

#include "session/host/include/keyboard_session.h"
#include <gtest/gtest.h>

#include "interfaces/include/ws_common.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_keyboard_session.h"
#include "session/host/include/session.h"
#include "session/host/include/scene_session.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "KeyboardSessionTest2"};
}

constexpr int WAIT_ASYNC_US = 1000000;
class KeyboardSessionTest2 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<KeyboardSession> GetKeyboardSession(const std::string& abilityName, const std::string& bundleName);
    sptr<SceneSession> GetSceneSession(const std::string& abilityName, const std::string& bundleName);
    sptr<KSSceneSessionMocker> GetSceneSessionMocker(const std::string& abilityName, const std::string& bundleName);
};

void KeyboardSessionTest2::SetUpTestCase()
{
}

void KeyboardSessionTest2::TearDownTestCase()
{
}

void KeyboardSessionTest2::SetUp()
{
}

void KeyboardSessionTest2::TearDown()
{
}

sptr<KeyboardSession> KeyboardSessionTest2::GetKeyboardSession(const std::string& abilityName,
    const std::string& bundleName)
{
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    sptr<WindowSessionProperty> keyboardProperty = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(keyboardProperty, nullptr);
    keyboardProperty->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->SetSessionProperty(keyboardProperty);

    return keyboardSession;
}

sptr<SceneSession> KeyboardSessionTest2::GetSceneSession(const std::string& abilityName,
    const std::string& bundleName)
{
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);

    return sceneSession;
}

sptr<KSSceneSessionMocker> KeyboardSessionTest2::GetSceneSessionMocker(const std::string& abilityName,
    const std::string& bundleName)
{
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KSSceneSessionMocker> mockSession = sptr<KSSceneSessionMocker>::MakeSptr(info, nullptr);

    return mockSession;
}

namespace {
/**
 * @tc.name: AdjustKeyboardLayout01
 * @tc.desc: AdjustKeyboardLayout
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, AdjustKeyboardLayout01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "AdjustKeyboardLayout01";
    info.bundleName_ = "AdjustKeyboardLayout01";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    keyboardSession->SetSessionProperty(property);
    keyboardSession->RegisterSessionChangeCallback(nullptr);

    KeyboardLayoutParams params;
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);

    sptr<SceneSession::SessionChangeCallback> sessionChangeCallback =
        new (std::nothrow) SceneSession::SessionChangeCallback();
    EXPECT_NE(sessionChangeCallback, nullptr);
    keyboardSession->adjustKeyboardLayoutFunc_ = nullptr;
    keyboardSession->RegisterSessionChangeCallback(sessionChangeCallback);
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);

    keyboardSession->adjustKeyboardLayoutFunc_ = [](const KeyboardLayoutParams& params){};
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);
}

/**
 * @tc.name: AdjustKeyboardLayout01
 * @tc.desc: AdjustKeyboardLayout
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, AdjustKeyboardLayout02, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "AdjustKeyboardLayout02";
    info.bundleName_ = "AdjustKeyboardLayout02";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    keyboardSession->SetSessionProperty(nullptr);

    KeyboardLayoutParams params;
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);
}

/**
 * @tc.name: CheckIfNeedRaiseCallingSession
 * @tc.desc: CheckIfNeedRaiseCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, CheckIfNeedRaiseCallingSession, Function | SmallTest | Level1)
{
    WLOGFI("CheckIfNeedRaiseCallingSession begin!");
    SessionInfo info;
    info.abilityName_ = "CheckIfNeedRaiseCallingSession";
    info.bundleName_ = "CheckIfNeedRaiseCallingSession";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->SetSessionProperty(property);

    ASSERT_FALSE(keyboardSession->CheckIfNeedRaiseCallingSession(nullptr, true));

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);

    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_FLOAT;
    ASSERT_FALSE(keyboardSession->CheckIfNeedRaiseCallingSession(sceneSession, true));

    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    ASSERT_TRUE(keyboardSession->CheckIfNeedRaiseCallingSession(sceneSession, false));

    property->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);

    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ASSERT_FALSE(keyboardSession->CheckIfNeedRaiseCallingSession(sceneSession, true));

    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->CheckIfNeedRaiseCallingSession(sceneSession, true);

    WLOGFI("CheckIfNeedRaiseCallingSession end!");
}

/**
 * @tc.name: OpenKeyboardSyncTransaction
 * @tc.desc: OpenKeyboardSyncTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, OpenKeyboardSyncTransaction, Function | SmallTest | Level1)
{
    std::string abilityName = "OpenKeyboardSyncTransaction";
    std::string bundleName = "OpenKeyboardSyncTransaction";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);

    // isKeyBoardSyncTransactionOpen_ is false
    keyboardSession->OpenKeyboardSyncTransaction();

    // isKeyBoardSyncTransactionOpen_ is true
    keyboardSession->OpenKeyboardSyncTransaction();
}

/**
 * @tc.name: CloseKeyboardSyncTransaction1
 * @tc.desc: CloseKeyboardSyncTransaction1
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, CloseKeyboardSyncTransaction1, Function | SmallTest | Level1)
{
    std::string abilityName = "CloseKeyboardSyncTransaction1";
    std::string bundleName = "CloseKeyboardSyncTransaction1";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);

    WSRect keyboardPanelRect = { 0, 0, 0, 0 };
    bool isKeyboardShow = true;
    bool isRotating = false;

    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, isKeyboardShow, isRotating);
}

/**
 * @tc.name: CloseKeyboardSyncTransaction2
 * @tc.desc: CloseKeyboardSyncTransaction2
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, CloseKeyboardSyncTransaction2, Function | SmallTest | Level1)
{
    std::string abilityName = "CloseKeyboardSyncTransaction2";
    std::string bundleName = "CloseKeyboardSyncTransaction2";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);

    WSRect keyboardPanelRect = { 0, 0, 0, 0 };
    bool isKeyboardShow = true;
    bool isRotating = false;

    // isKeyBoardSyncTransactionOpen_ is true
    keyboardSession->OpenKeyboardSyncTransaction();
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, isKeyboardShow, isRotating);
}

/**
 * @tc.name: BindKeyboardPanelSession
 * @tc.desc: BindKeyboardPanelSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, BindKeyboardPanelSession, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "BindKeyboardPanelSession";
    info.bundleName_ = "BindKeyboardPanelSession";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    sptr<SceneSession> panelSession = nullptr;
    keyboardSession->BindKeyboardPanelSession(panelSession);
    sptr<SceneSession> getPanelSession = keyboardSession->GetKeyboardPanelSession();
    ASSERT_EQ(getPanelSession, nullptr);
    panelSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(panelSession, nullptr);
    keyboardSession->BindKeyboardPanelSession(panelSession);
    getPanelSession = keyboardSession->GetKeyboardPanelSession();
    EXPECT_EQ(getPanelSession, panelSession);
}

/**
 * @tc.name: GetKeyboardGravity01
 * @tc.desc: GetKeyboardGravity01
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, GetKeyboardGravity01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetKeyboardGravity";
    info.bundleName_ = "GetKeyboardGravity";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->property_ = nullptr;
    auto ret = keyboardSession->GetKeyboardGravity();
    EXPECT_EQ(SessionGravity::SESSION_GRAVITY_DEFAULT, ret);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    ASSERT_NE(keyboardSession, nullptr);
    ret = keyboardSession->GetKeyboardGravity();
    EXPECT_EQ(SessionGravity::SESSION_GRAVITY_BOTTOM, ret);
}

/**
 * @tc.name: NotifyKeyboardPanelInfoChange
 * @tc.desc: NotifyKeyboardPanelInfoChange
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, NotifyKeyboardPanelInfoChange, Function | SmallTest | Level1)
{
    WSRect rect = {800, 800, 1200, 1200};
    SessionInfo info;
    info.abilityName_ = "NotifyKeyboardPanelInfoChange";
    info.bundleName_ = "NotifyKeyboardPanelInfoChange";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->isKeyboardPanelEnabled_ = false;
    keyboardSession->NotifyKeyboardPanelInfoChange(rect, true);
    keyboardSession->isKeyboardPanelEnabled_ = true;
    keyboardSession->sessionStage_ = nullptr;
    keyboardSession->NotifyKeyboardPanelInfoChange(rect, true);
    sptr<SessionStageMocker> mockSessionStage = sptr<SessionStageMocker>::MakeSptr();
    ASSERT_NE(mockSessionStage, nullptr);
    keyboardSession->sessionStage_ = mockSessionStage;
    ASSERT_NE(keyboardSession->sessionStage_, nullptr);
    keyboardSession->NotifyKeyboardPanelInfoChange(rect, true);
}

/**
 * @tc.name: CheckIfNeedRaiseCallingSession01
 * @tc.desc: CheckIfNeedRaiseCallingSession01
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, CheckIfNeedRaiseCallingSession01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "CheckIfNeedRaiseCallingSession";
    info.bundleName_ = "CheckIfNeedRaiseCallingSession";
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    ASSERT_NE(specificCb, nullptr);
    sptr<SceneSession> callingSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    ASSERT_NE(callingSession, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    keyboardSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    callingSession->systemConfig_.freeMultiWindowSupport_ = true;
    callingSession->systemConfig_.freeMultiWindowEnable_ = true;
    auto ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, false);
    callingSession->systemConfig_.freeMultiWindowEnable_ = false;
    ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, false);
    callingSession->systemConfig_.freeMultiWindowEnable_ = true;
    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, true);
    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    callingSession->systemConfig_.freeMultiWindowEnable_ = false;
    ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, true);
    keyboardSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, true);
    ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, false);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: UpdateCallingSessionIdAndPosition01
 * @tc.desc: UpdateCallingSessionIdAndPosition01
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, UpdateCallingSessionIdAndPosition01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "UpdateCallingSessionIdAndPosition";
    info.bundleName_ = "UpdateCallingSessionIdAndPosition";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->property_ = nullptr;
    keyboardSession->UpdateCallingSessionIdAndPosition(0);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->SetCallingSessionId(-1);
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->UpdateCallingSessionIdAndPosition(0);
    keyboardSession->state_ = SessionState::STATE_CONNECT;
    keyboardSession->UpdateCallingSessionIdAndPosition(0);
    keyboardSession->UpdateCallingSessionIdAndPosition(-1);
    keyboardSession->property_->SetCallingSessionId(0);
    keyboardSession->UpdateCallingSessionIdAndPosition(0);
}

/**
 * @tc.name: OpenKeyboardSyncTransaction01
 * @tc.desc: OpenKeyboardSyncTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, OpenKeyboardSyncTransaction01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "UpdateCallingSessionIdAndPosition";
    info.bundleName_ = "UpdateCallingSessionIdAndPosition";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->isKeyboardSyncTransactionOpen_ = true;
    keyboardSession->OpenKeyboardSyncTransaction();
    keyboardSession->isKeyboardSyncTransactionOpen_ = false;
    keyboardSession->OpenKeyboardSyncTransaction();
    WSRect keyboardPanelRect = {0, 0, 0, 0};
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, true, true);
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, false, false);
}

/**
 * @tc.name: RelayoutKeyBoard01
 * @tc.desc: RelayoutKeyBoard01
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, RelayoutKeyBoard01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "RelayoutKeyBoard";
    info.bundleName_ = "RelayoutKeyBoard";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->property_ = nullptr;
    keyboardSession->RelayoutKeyBoard();
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    keyboardSession->RelayoutKeyBoard();
}

/**
 * @tc.name: Hide01
 * @tc.desc: test function : Hide
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, Hide01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "Hide";
    info.bundleName_ = "Hide";
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    ASSERT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    ASSERT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    ASSERT_NE(keyboardSession, nullptr);

    // setActive false return not ok
    keyboardSession->state_ = SessionState::STATE_DISCONNECT;
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());

    // setActive false return ok and deviceType is phone
    keyboardSession->state_ = SessionState::STATE_CONNECT;
    keyboardSession->isActive_ = true;
    keyboardSession->sessionStage_ = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(keyboardSession->sessionStage_, nullptr);
    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());

    // deviceType is pc and property is not nullptr
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());

    // deviceType is pc and property is nullptr
    keyboardSession->property_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());
}

/**
 * @tc.name: RaiseCallingSession01
 * @tc.desc: test function : RaiseCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, RaiseCallingSession01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("RaiseCallingSession01",
        "RaiseCallingSession01");
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->isVisible_ = true;

    Rosen::WSRect resultRect{ 0, 0, 0, 0 };
    sptr<KSSceneSessionMocker> callingSession = GetSceneSessionMocker("callingSession", "callingSession");
    ASSERT_NE(callingSession, nullptr);

    callingSession->updateRectCallback_ = [&resultRect](const WSRect& rect, SizeChangeReason reason) {
        resultRect.posX_ = rect.posX_;
        resultRect.posY_ = rect.posY_;
        resultRect.width_ = rect.width_;
        resultRect.height_ = rect.height_;
    };
    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    Rosen::WSRect keyboardPanelRect{ 0, 0, 0, 0 };
    Rosen::WSRect emptyRect{ 0, 0, 0, 0 };
    std::shared_ptr<RSTransaction> rsTransaction = nullptr;
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, rsTransaction);
    ASSERT_EQ(resultRect, emptyRect);

    // for cover GetSceneSession
    keyboardSession->keyboardCallback_->onGetSceneSession_ =
        [callingSession](int32_t persistentId)->sptr<SceneSession> {
        return callingSession;
    };
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, rsTransaction);
    // for cover CheckIfNeedRaiseCallingSession
    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;

    // for empty rect check;
    keyboardSession->winRect_.posX_ = 1;
    keyboardSession->winRect_.posY_ = 1;
    keyboardSession->winRect_.posX_ = 1;
    keyboardSession->winRect_.posX_ = 1;

    // for cover oriPosYBeforeRaisedBykeyboard == 0
    callingSession->SetOriPosYBeforeRaisedByKeyboard(0);
    ASSERT_EQ(resultRect, emptyRect);
}

/**
 * @tc.name: RaiseCallingSession02
 * @tc.desc: test function : RaiseCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, RaiseCallingSession02, Function | SmallTest | Level1)
{
    Rosen::WSRect keyboardPanelRect{ 1, 1, 1, 1 };
    auto keyboardSession = GetKeyboardSession("RaiseCallingSession02",
        "RaiseCallingSession02");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<KSSceneSessionMocker> callingSession = GetSceneSessionMocker("callingSession", "callingSession");
    ASSERT_NE(callingSession, nullptr);
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    callingSession->winRect_ = { 1, 1, 1, 1 };
    keyboardSession->keyboardCallback_->onGetSceneSession_ = [callingSession](int32_t persistentId) {
        return callingSession;
    };
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->isVisible_ = true;
    callingSession->oriPosYBeforeRaisedByKeyboard_ = 0;
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->winRect_.posY_, 1);

    callingSession->oriPosYBeforeRaisedByKeyboard_ = 10;
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->winRect_.posY_, 1);

    keyboardPanelRect = { 0, 0, 0, 0 };
    callingSession->oriPosYBeforeRaisedByKeyboard_ = 10;
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->winRect_.posY_, 1);
}

/**
 * @tc.name: RaiseCallingSession03
 * @tc.desc: test function : RaiseCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, RaiseCallingSession03, Function | SmallTest | Level1)
{
    Rosen::WSRect keyboardPanelRect{ 1, 1, 1, 1 };
    auto keyboardSession = GetKeyboardSession("RaiseCallingSession03",
        "RaiseCallingSession03");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<KSSceneSessionMocker> callingSession = GetSceneSessionMocker("callingSession", "callingSession");
    ASSERT_NE(callingSession, nullptr);
    callingSession->winRect_ = { 1, 1, 1, 1 };
    callingSession->oriPosYBeforeRaisedByKeyboard_ = 0;
    callingSession->updateRectCallback_ = [](const WSRect& rect, SizeChangeReason reason) {};
    keyboardSession->keyboardCallback_->onGetSceneSession_ = [callingSession](int32_t persistentId) {
        return callingSession;
    };
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->isVisible_ = true;
    auto callingOriPosY = 0;
    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    callingOriPosY = callingSession->oriPosYBeforeRaisedByKeyboard_;
    ASSERT_EQ(callingOriPosY, 0);

    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    callingSession->winRect_.posY_ = 200;
    keyboardPanelRect.posY_ = 200;
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    callingOriPosY = callingSession->oriPosYBeforeRaisedByKeyboard_;
    ASSERT_EQ(callingOriPosY, 200);

    callingSession->oriPosYBeforeRaisedByKeyboard_ = 10;
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    callingOriPosY = callingSession->oriPosYBeforeRaisedByKeyboard_;
    ASSERT_EQ(callingOriPosY, 10);
}

/**
 * @tc.name: IsCallingSessionSplitMode01
 * @tc.desc: test function : IsCallingSessionSplitMode
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, IsCallingSessionSplitMode01, Function | SmallTest | Level1)
{
    Rosen::WSRect keyboardPanelRect{ 0, 0, 0, 0 };
    auto keyboardSession = GetKeyboardSession("IsCallingSessionSplitMode01",
        "IsCallingSessionSplitMode01");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<KSSceneSessionMocker> callingSession = GetSceneSessionMocker("callingSession", "callingSession");
    ASSERT_NE(callingSession, nullptr);
    callingSession->oriPosYBeforeRaisedByKeyboard_ = 0;
    callingSession->winRect_ = { 0, 0, 0, 0 };
    callingSession->updateRectCallback_ = [](const WSRect& rect, SizeChangeReason reason) {};
    keyboardSession->keyboardCallback_->onGetSceneSession_ = [callingSession](int32_t persistentId) {
        return callingSession;
    };
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;

    auto callingParentSession = GetSceneSession("callingParentSession", "callingParentSession");
    ASSERT_NE(callingSession, nullptr);

    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->oriPosYBeforeRaisedByKeyboard_, 0);

    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->oriPosYBeforeRaisedByKeyboard_, 0);

    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->oriPosYBeforeRaisedByKeyboard_, 0);

    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->oriPosYBeforeRaisedByKeyboard_, 0);

    callingSession->parentSession_ = callingParentSession;
    callingSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    callingParentSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->oriPosYBeforeRaisedByKeyboard_, 0);

    callingParentSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    keyboardSession->RaiseCallingSession(keyboardPanelRect, true, nullptr);
    ASSERT_EQ(callingSession->oriPosYBeforeRaisedByKeyboard_, 0);
}

/**
 * @tc.name: CloseKeyBoardSyncTransaction3
 * @tc.desc: test function : CloseKeyBoardSyncTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, CloseKeyBoardSyncTransaction3, Function | SmallTest | Level1)
{
    std::string abilityName = "CloseKeyBoardSyncTransaction3";
    std::string bundleName = "CloseKeyBoardSyncTransaction3";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);
    ASSERT_NE(keyboardSession, nullptr);

    WSRect keyboardPanelRect = { 0, 0, 0, 0 };
    bool isKeyboardShow = true;
    bool isRotating = false;

    keyboardSession->dirtyFlags_ = 0;
    keyboardSession->specificCallback_->onUpdateAvoidArea_ = [](uint32_t callingSessionId) {};
    keyboardSession->isKeyboardSyncTransactionOpen_ = true;
    // isKeyBoardSyncTransactionOpen_ is true
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, isKeyboardShow, isRotating);
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(keyboardSession->isKeyboardSyncTransactionOpen_, false);
}

/**
 * @tc.name: CloseKeyboardSyncTransaction4
 * @tc.desc: test function : CloseKeyboardSyncTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, CloseKeyboardSyncTransaction4, Function | SmallTest | Level1)
{
    std::string abilityName = "CloseKeyboardSyncTransaction4";
    std::string bundleName = "CloseKeyboardSyncTransaction4";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);
    ASSERT_NE(keyboardSession, nullptr);
    WSRect keyboardPanelRect = { 0, 0, 0, 0 };
    keyboardSession->dirtyFlags_ = 0;

    keyboardSession->isKeyboardSyncTransactionOpen_ = false;
    keyboardSession->property_ = nullptr;
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, false, false);
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(0, keyboardSession->dirtyFlags_);

    sptr<WindowSessionProperty> keyboardProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(keyboardProperty, nullptr);
    keyboardSession->property_ = keyboardProperty;
    keyboardSession->isKeyboardSyncTransactionOpen_ = true;
    keyboardSession->property_->SetCallingSessionId(1);
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, false, false);
    usleep(WAIT_ASYNC_US);
    auto callingSessionId = keyboardSession->property_->GetCallingSessionId();
    ASSERT_EQ(callingSessionId, INVALID_WINDOW_ID);
}

}  // namespace
}  // namespace Rosen
}  // namespace OHOS
