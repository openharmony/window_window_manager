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
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session/host/include/scene_session.h"
#include "window_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class KeyboardSessionTest3 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    sptr<KeyboardSession> GetKeyboardSession(const std::string& abilityName, const std::string& bundleName);
    sptr<SceneSession> GetSceneSession(const std::string& abilityName, const std::string& bundleName);
};

void KeyboardSessionTest3::SetUpTestCase()
{
}

void KeyboardSessionTest3::TearDownTestCase()
{
}

void KeyboardSessionTest3::SetUp()
{
}

void KeyboardSessionTest3::TearDown()
{
}

sptr<KeyboardSession> KeyboardSessionTest3::GetKeyboardSession(const std::string& abilityName,
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

sptr<SceneSession> KeyboardSessionTest3::GetSceneSession(const std::string& abilityName,
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

namespace {
/**
 * @tc.name: GetRSTransaction01
 * @tc.desc: test function : GetRSTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, GetRSTransaction01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("GetRSTransaction01",
        "GetRSTransaction01");
    ASSERT_NE(keyboardSession, nullptr);

    auto rsTransaction = keyboardSession->GetRSTransaction();
    ASSERT_EQ(rsTransaction, nullptr);
}

/**
 * @tc.name: GetSessionScreenName01
 * @tc.desc: test function : GetSessionScreenName
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, GetSessionScreenName01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("GetSessionScreenName01",
        "GetSessionScreenName01");
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->property_ = nullptr;
    auto resultStr = keyboardSession->GetSessionScreenName();
    ASSERT_EQ(resultStr, "");

    sptr<WindowSessionProperty> keyboardProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(keyboardProperty, nullptr);
    keyboardSession->property_ = keyboardProperty;
    keyboardSession->property_->displayId_ = 100;

    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr();
    ASSERT_NE(screenSession, nullptr);
    screenSession->name_ = "testScreenSession";
    screenSession->screenId_ = 100;
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.emplace(100, screenSession);

    resultStr = keyboardSession->GetSessionScreenName();
    ASSERT_EQ(resultStr, screenSession->name_);
}

/**
 * @tc.name: UseFocusIdIfCallingSessionIdInvalid01
 * @tc.desc: test function : UseFocusIdIfCallingSessionIdInvalid
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, UseFocusIdIfCallingSessionIdInvalid01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("UseFocusIdIfCallingSessionIdInvalid01",
        "UseFocusIdIfCallingSessionIdInvalid01");
    ASSERT_NE(keyboardSession, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCallback =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    ASSERT_NE(keyboardCallback, nullptr);
    keyboardSession->keyboardCallback_ = keyboardCallback;
    sptr<SceneSession> sceneSession = GetSceneSession("TestSceneSession", "TestSceneSession");
    ASSERT_NE(sceneSession, nullptr);
    sceneSession->persistentId_ = 100;
    keyboardSession->keyboardCallback_->onGetSceneSession_ =
        [sceneSession](uint32_t callingSessionId)->sptr<SceneSession> {
            if (sceneSession->persistentId_ != callingSessionId) {
                return nullptr;
            }
            return sceneSession;
        };

    keyboardSession->GetSessionProperty()->SetCallingSessionId(100);
    keyboardSession->UseFocusIdIfCallingSessionIdInvalid();
    auto resultId = keyboardSession->GetCallingSessionId();
    ASSERT_EQ(resultId, 100);

    keyboardSession->GetSessionProperty()->SetCallingSessionId(101);
    keyboardSession->keyboardCallback_->onGetFocusedSessionId_ = []()->int32_t {
        return 100;
    };
    keyboardSession->UseFocusIdIfCallingSessionIdInvalid();
    resultId = keyboardSession->GetCallingSessionId();
    ASSERT_EQ(resultId, 100);
}

/**
 * @tc.name: UpdateKeyboardAvoidArea01
 * @tc.desc: test function : UpdateKeyboardAvoidArea
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, UpdateKeyboardAvoidArea01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("UpdateKeyboardAvoidArea01",
        "UpdateKeyboardAvoidArea01");
    ASSERT_NE(keyboardSession, nullptr);

    // not foreground
    keyboardSession->dirtyFlags_ = 0;
    keyboardSession->state_ = SessionState::STATE_CONNECT;
    keyboardSession->UpdateKeyboardAvoidArea();
    ASSERT_EQ(keyboardSession->dirtyFlags_, 0);

    // has callback
    auto expectDirtyFlag = 0;
    keyboardSession->dirtyFlags_ = 0;
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->isVisible_ = true;
    keyboardSession->specificCallback_->onUpdateAvoidArea_ = [&expectDirtyFlag](const uint32_t& persistentId) {
        expectDirtyFlag = 1;
    };
    keyboardSession->UpdateKeyboardAvoidArea();
    if (Session::IsScbCoreEnabled()) {
        expectDirtyFlag = 0 | static_cast<uint32_t>(SessionUIDirtyFlag::AVOID_AREA);
        ASSERT_EQ(keyboardSession->dirtyFlags_, expectDirtyFlag);
    } else {
        ASSERT_EQ(expectDirtyFlag, 1);
    }

    // miss callback
    expectDirtyFlag = 0;
    keyboardSession->dirtyFlags_ = 1;
    keyboardSession->specificCallback_->onUpdateAvoidArea_ = nullptr;
    keyboardSession->UpdateKeyboardAvoidArea();
    if (Session::IsScbCoreEnabled()) {
        ASSERT_EQ(keyboardSession->dirtyFlags_, 1);
    } else {
        ASSERT_EQ(expectDirtyFlag, 0);
    }

    expectDirtyFlag = 0;
    keyboardSession->dirtyFlags_ = 2;
    keyboardSession->specificCallback_ = nullptr;
    keyboardSession->UpdateKeyboardAvoidArea();
    if (Session::IsScbCoreEnabled()) {
        ASSERT_EQ(keyboardSession->dirtyFlags_, 2);
    } else {
        ASSERT_EQ(expectDirtyFlag, 0);
    }
}

/**
 * @tc.name: MoveAndResizeKeyboard01
 * @tc.desc: test function : MoveAndResizeKeyboard
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, MoveAndResizeKeyboard01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("MoveAndResizeKeyboard01",
        "MoveAndResizeKeyboard01");
    ASSERT_NE(keyboardSession, nullptr);

    KeyboardLayoutParams param;
    param.LandscapeKeyboardRect_ = { 100, 100, 100, 200 };
    param.PortraitKeyboardRect_ = { 200, 200, 200, 100 };

    keyboardSession->isScreenAngleMismatch_ = true;
    keyboardSession->targetScreenWidth_ = 300;
    keyboardSession->targetScreenHeight_ = 400;

    // branch SESSION_GRAVITY_BOTTOM
    param.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    Rect expectRect = { 0, 300, 300, 100 };
    keyboardSession->MoveAndResizeKeyboard(param, nullptr, false);
    ASSERT_EQ(keyboardSession->property_->requestRect_, expectRect);

    //branch SESSION_GRAVITY_DEFAULT
    param.gravity_ = WindowGravity::WINDOW_GRAVITY_DEFAULT;
    expectRect = { 200, 300, 200, 100 };
    keyboardSession->MoveAndResizeKeyboard(param, nullptr, true);
    ASSERT_EQ(keyboardSession->property_->requestRect_, expectRect);
}

/**
 * @tc.name: MoveAndResizeKeyboard02
 * @tc.desc: test function : MoveAndResizeKeyboard
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, MoveAndResizeKeyboard02, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("MoveAndResizeKeyboard02",
        "MoveAndResizeKeyboard02");
    ASSERT_NE(keyboardSession, nullptr);

    KeyboardLayoutParams param;
    param.LandscapeKeyboardRect_ = { 100, 100, 100, 200 };
    param.PortraitKeyboardRect_ = { 200, 200, 200, 100 };

    keyboardSession->isScreenAngleMismatch_ = true;
    keyboardSession->targetScreenWidth_ = 300;
    keyboardSession->targetScreenHeight_ = 400;
    param.gravity_ = WindowGravity::WINDOW_GRAVITY_FLOAT;

    // branch else
    Rect expectRect = param.PortraitKeyboardRect_;
    keyboardSession->MoveAndResizeKeyboard(param, nullptr, true);
    ASSERT_EQ(keyboardSession->property_->requestRect_, expectRect);

    param.PortraitKeyboardRect_ = { 200, 200, 200, 0 };
    auto requestRect = keyboardSession->GetSessionRequestRect();
    expectRect = { requestRect.posX_, requestRect.posY_, requestRect.width_, requestRect.height_ };
    keyboardSession->MoveAndResizeKeyboard(param, nullptr, true);
    ASSERT_EQ(keyboardSession->property_->requestRect_, expectRect);

    param.PortraitKeyboardRect_ = { 200, 200, 0, 0 };
    requestRect = keyboardSession->GetSessionRequestRect();
    expectRect = { requestRect.posX_, requestRect.posY_, requestRect.width_, requestRect.height_ };
    keyboardSession->MoveAndResizeKeyboard(param, nullptr, true);
    ASSERT_EQ(keyboardSession->property_->requestRect_, expectRect);
}

/**
 * @tc.name: OnCallingSessionUpdated01
 * @tc.desc: test function : OnCallingSessionUpdated
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest3, OnCallingSessionUpdated01, Function | SmallTest | Level1)
{
    auto keyboardSession = GetKeyboardSession("OnCallingSessionUpdated01",
        "OnCallingSessionUpdated01");
    ASSERT_NE(keyboardSession, nullptr);

    // keyboardSession is not foreground
    keyboardSession->OnCallingSessionUpdated();
    ASSERT_EQ(keyboardSession->state_, SessionState::STATE_DISCONNECT);

    // keyboardSession's isVisible_ is false
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->OnCallingSessionUpdated();
    ASSERT_EQ(keyboardSession->state_, SessionState::STATE_FOREGROUND);

    // keyboardSession's isVisible_ is true
    keyboardSession->isVisible_ = true;
    keyboardSession->OnCallingSessionUpdated();
    ASSERT_EQ(keyboardSession->state_, SessionState::STATE_FOREGROUND);

    // callingsession is not nullptr
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    SessionInfo info;
    info.abilityName_ = "OnCallingSessionUpdated01";
    info.bundleName_ = "OnCallingSessionUpdated01";
    info.windowType_ = 1; // 1 is main_window_type
    sptr<SceneSession> callingSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(callingSession, nullptr);
    ASSERT_NE(keyboardSession->keyboardCallback_, nullptr);
    keyboardSession->keyboardCallback_->onGetSceneSession_ =
        [callingSession](int32_t persistentId)->sptr<SceneSession> {
        return callingSession;
    };
    // callingSession is fullScreen and isCallingSessionFloating is false
    // keyboardSession's gravity is SessionGravity::SESSION_GRAVITY_DEFAULT
    keyboardSession->OnCallingSessionUpdated();
    ASSERT_EQ(keyboardSession->state_, SessionState::STATE_FOREGROUND);

    // keyboardSession's gravity is WindowGravity::Window_GRAVITY_FLOAT
    EXPECT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_FLOAT;

    ASSERT_EQ(keyboardSession->GetKeyboardGravity(), SessionGravity::SESSION_GRAVITY_FLOAT);
    keyboardSession->OnCallingSessionUpdated();
    ASSERT_EQ(keyboardSession->state_, SessionState::STATE_FOREGROUND);
}
}  // namespace
}  // namespace Rosen
}  // namespace OHOS
