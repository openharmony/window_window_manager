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
#include <parameters.h>

#include "common/include/session_permission.h"
#include "interfaces/include/ws_common.h"
#include "mock/mock_session_stage.h"
#include "session/host/include/session.h"
#include "session/host/include/scene_session.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "KeyboardSessionTest"};
}
class KeyboardSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    sptr<KeyboardSession> GetKeyboardSession(std::string &abilityName, std::string &bundleName);
};

void KeyboardSessionTest::SetUpTestCase()
{
}

void KeyboardSessionTest::TearDownTestCase()
{
}

void KeyboardSessionTest::SetUp()
{
}

void KeyboardSessionTest::TearDown()
{
}

sptr<KeyboardSession> KeyboardSessionTest::GetKeyboardSession(std::string &abilityName, std::string &bundleName)
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
    keyboardProperty->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->SetSessionProperty(keyboardProperty);

    return keyboardSession;
}

namespace {
/**
 * @tc.name: Show
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetKeyboardGravity, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetKeyboardGravity";
    info.bundleName_ = "GetKeyboardGravity";
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, nullptr);
    ASSERT_TRUE((keyboardSession != nullptr));
    ASSERT_EQ(SessionGravity::SESSION_GRAVITY_DEFAULT, keyboardSession->GetKeyboardGravity());

    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    sptr<WindowSessionProperty> windowSessionProperty = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(windowSessionProperty, nullptr);
    sceneSession->property_ = windowSessionProperty;
    keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, nullptr);
    ASSERT_EQ(SessionGravity::SESSION_GRAVITY_DEFAULT, keyboardSession->GetKeyboardGravity());
}

/**
 * @tc.name: Show
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, Show, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "Show";
    info.bundleName_ = "Show";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    ASSERT_TRUE((keyboardSession != nullptr));
    sptr<WindowSessionProperty> property = new WindowSessionProperty();
    ASSERT_NE(nullptr, property);

    keyboardSession->isKeyboardPanelEnabled_ = true;
    ASSERT_EQ(WSError::WS_OK, keyboardSession->Show(property));

    keyboardSession->isKeyboardPanelEnabled_ = false;
    ASSERT_EQ(WSError::WS_OK, keyboardSession->Show(property));
}

/**
 * @tc.name: Hide
 * @tc.desc: test function : Hide
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, Hide, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "Hide";
    info.bundleName_ = "Hide";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    ASSERT_TRUE((keyboardSession != nullptr));

    ASSERT_EQ(WSError::WS_OK, keyboardSession->Hide());
}

/**
 * @tc.name: Disconnect
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, Disconnect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Disconnect";
    info.bundleName_ = "Disconnect";
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, nullptr);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = new(std::nothrow) WindowSessionProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    keyboardSession->SetSessionProperty(property);
    keyboardSession->isActive_ = true;
    auto result = keyboardSession->Disconnect();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: NotifyClientToUpdateRect
 * @tc.desc: NotifyClientToUpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, NotifyClientToUpdateRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRect";
    info.bundleName_ = "NotifyClientToUpdateRect";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<SessionStageMocker> mockSessionStage = new (std::nothrow) SessionStageMocker();
    ASSERT_NE(mockSessionStage, nullptr);
    keyboardSession->dirtyFlags_ |= static_cast<uint32_t>(SessionUIDirtyFlag::RECT);
    keyboardSession->sessionStage_ = mockSessionStage;
    auto ret = keyboardSession->NotifyClientToUpdateRect(nullptr);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: SetKeyboardSessionGravity
 * @tc.desc: SetKeyboardSessionGravity
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, SetKeyboardSessionGravity, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "SetKeyboardSessionGravity";
    info.bundleName_ = "SetKeyboardSessionGravity";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    auto ret = keyboardSession->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, 0);
    ASSERT_EQ(ret, WSError::WS_OK);

    sptr<SceneSession::SessionChangeCallback> sessionChangeCb =
        new (std::nothrow) SceneSession::SessionChangeCallback();
    EXPECT_NE(sessionChangeCb, nullptr);
    keyboardSession->sessionChangeCallback_ = sessionChangeCb;
    ret = keyboardSession->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, 0);
    ASSERT_EQ(ret, WSError::WS_OK);

    keyboardSession->keyboardGravityChangeFunc_ = [](SessionGravity) {
        return 0;
    };
    ret = keyboardSession->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, 0);
    ASSERT_EQ(ret, WSError::WS_OK);

    keyboardSession->isKeyboardPanelEnabled_ = true;
    ret = keyboardSession->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, 0);
    ASSERT_EQ(ret, WSError::WS_OK);

    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    keyboardSession->isKeyboardPanelEnabled_ = false;
    ret = keyboardSession->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, 0);
    ASSERT_EQ(ret, WSError::WS_OK);

    keyboardSession->isKeyboardPanelEnabled_ = true;
    ret = keyboardSession->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_BOTTOM, 0);
    ASSERT_EQ(ret, WSError::WS_OK);

    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    ret = keyboardSession->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_FLOAT, 0);
    ASSERT_EQ(ret, WSError::WS_OK);

    keyboardSession->state_ = SessionState::STATE_DISCONNECT;
    ret = keyboardSession->SetKeyboardSessionGravity(SessionGravity::SESSION_GRAVITY_FLOAT, 0);
    ASSERT_EQ(ret, WSError::WS_OK);
}

/**
 * @tc.name: GetSceneSession
 * @tc.desc: GetSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetSceneSession, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetSceneSession";
    info.bundleName_ = "GetSceneSession";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);
    auto id = sceneSession->GetPersistentId();
    EXPECT_NE(id, 0);
    auto ret = keyboardSession->GetSceneSession(id);

    keyboardCb->onGetSceneSession_ = [](uint32_t) {
        return nullptr;
    };
    EXPECT_NE(keyboardCb->onGetSceneSession_, nullptr);
    ret = keyboardSession->GetSceneSession(id);
}

/**
 * @tc.name: NotifyOccupiedAreaChangeInfo
 * @tc.desc: NotifyOccupiedAreaChangeInfo
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, NotifyOccupiedAreaChangeInfo, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "NotifyOccupiedAreaChangeInfo";
    info.bundleName_ = "NotifyOccupiedAreaChangeInfo";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<SceneSession> callingSession = new (std::nothrow) SceneSession(info, nullptr);
    WSRect rect = { 0, 0, 0, 0 };
    WSRect occupiedArea = { 0, 0, 0, 0 };
    keyboardSession->NotifyOccupiedAreaChangeInfo(callingSession, rect, occupiedArea);

    WSRect lastSR = {1, 1, 1, 1};
    callingSession->lastSafeRect = lastSR;
    keyboardSession->NotifyOccupiedAreaChangeInfo(callingSession, rect, occupiedArea);

    sptr<WindowSessionProperty> windowSessionProperty = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    keyboardSession->NotifyOccupiedAreaChangeInfo(callingSession, rect, occupiedArea);
}

/**
 * @tc.name: RestoreCallingSession
 * @tc.desc: RestoreCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, RestoreCallingSession, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "RestoreCallingSession";
    info.bundleName_ = "RestoreCallingSession";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);
    auto id = sceneSession->GetPersistentId();
    EXPECT_NE(id, 0);

    keyboardSession->GetSessionProperty()->SetCallingSessionId(id);
    keyboardSession->RestoreCallingSession();
}

/**
 * @tc.name: UseFocusIdIfCallingSessionIdInvalid
 * @tc.desc: UseFocusIdIfCallingSessionIdInvalid
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, UseFocusIdIfCallingSessionIdInvalid, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "UseFocusIdIfCallingSessionIdInvalid";
    info.bundleName_ = "UseFocusIdIfCallingSessionIdInvalid";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);
    auto id = sceneSession->GetPersistentId();
    EXPECT_NE(id, 0);

    keyboardSession->GetSessionProperty()->SetCallingSessionId(id);
    keyboardSession->UseFocusIdIfCallingSessionIdInvalid();
}

/**
 * @tc.name: UpdateCallingSessionIdAndPosition
 * @tc.desc: UpdateCallingSessionIdAndPosition
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, UpdateCallingSessionIdAndPosition, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "UpdateCallingSessionIdAndPosition";
    info.bundleName_ = "UpdateCallingSessionIdAndPosition";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);
    auto id = sceneSession->GetPersistentId();
    EXPECT_NE(id, 0);

    keyboardSession->UpdateCallingSessionIdAndPosition(id);
}

/**
 * @tc.name: RelayoutKeyBoard
 * @tc.desc: RelayoutKeyBoard
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, RelayoutKeyBoard, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "RelayoutKeyBoard";
    info.bundleName_ = "RelayoutKeyBoard";
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, nullptr);
    EXPECT_NE(keyboardSession, nullptr);

    keyboardSession->RelayoutKeyBoard();
}

/**
 * @tc.name: GetFocusedSessionId
 * @tc.desc: GetFocusedSessionId
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetFocusedSessionId, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "RelayoutKeyBoard";
    info.bundleName_ = "RelayoutKeyBoard";
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    keyboardCb->onGetFocusedSessionId_ = []()
    {
        return 0;
    };
    EXPECT_NE(keyboardCb->onGetFocusedSessionId_, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    ASSERT_EQ(INVALID_WINDOW_ID, keyboardSession->GetFocusedSessionId());

    keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, nullptr);
    EXPECT_NE(keyboardSession, nullptr);

    ASSERT_EQ(INVALID_WINDOW_ID, keyboardSession->GetFocusedSessionId());
}

/**
 * @tc.name: GetStatusBarHeight
 * @tc.desc: GetStatusBarHeight
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetStatusBarHeight, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "RelayoutKeyBoard";
    info.bundleName_ = "RelayoutKeyBoard";
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, nullptr);
    EXPECT_NE(keyboardSession, nullptr);

    int32_t statusBarHeight = keyboardSession->GetStatusBarHeight();
    ASSERT_EQ(statusBarHeight, 0);

    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, nullptr);
    EXPECT_NE(keyboardSession, nullptr);
    statusBarHeight = keyboardSession->GetStatusBarHeight();
    ASSERT_EQ(statusBarHeight, 0);

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);
    WSRect rect({0, 0, 0, 0});
    sceneSession->winRect_ = rect;
    sceneSession->specificCallback_->onGetSceneSessionVectorByType_ =
        [&](WindowType, uint64_t)->std::vector<sptr<SceneSession>>
        {
            std::vector<sptr<SceneSession>> vec;
            vec.push_back(nullptr);
            return vec;
        };
    EXPECT_NE(specificCb->onGetSceneSessionVectorByType_, nullptr);
    statusBarHeight = keyboardSession->GetStatusBarHeight();
    ASSERT_EQ(statusBarHeight, 0);

    sptr<WindowSessionProperty> windowSessionProperty = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(windowSessionProperty, nullptr);
    sceneSession->property_ = windowSessionProperty;
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb=
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback;
    EXPECT_NE(keyboardCb, nullptr);
    keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    sceneSession->specificCallback_->onGetSceneSessionVectorByType_ =
        [&](WindowType, uint64_t)->std::vector<sptr<SceneSession>>
        {
            std::vector<sptr<SceneSession>> vec;
            vec.push_back(sceneSession);
            return vec;
        };
    statusBarHeight = keyboardSession->GetStatusBarHeight();
    ASSERT_EQ(statusBarHeight, 0);
}

/**
 * @tc.name: GetStatusBarHeight
 * @tc.desc: GetStatusBarHeight
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetStatusBarHeight02, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "RelayoutKeyBoard";
    info.bundleName_ = "RelayoutKeyBoard";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);
    sptr<WindowSessionProperty> windowSessionProperty = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(windowSessionProperty, nullptr);
    sceneSession->property_ = windowSessionProperty;
    WSRect rect3({0, 0, 0, 1});
    sceneSession->winRect_ = rect3;
    sceneSession->specificCallback_->onGetSceneSessionVectorByType_ =
        [&](WindowType, uint64_t)->std::vector<sptr<SceneSession>>
        {
            std::vector<sptr<SceneSession>> vec;
            vec.push_back(sceneSession);
            return vec;
        };
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback;
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    int32_t statusBarHeight = keyboardSession->GetStatusBarHeight();
    ASSERT_EQ(statusBarHeight, 1);
}

/**
 * @tc.name: OnKeyboardPanelUpdated
 * @tc.desc: OnKeyboardPanelUpdated
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, OnKeyboardPanelUpdated, Function | SmallTest | Level1)
{
    WLOGFI("OnKeyboardPanelUpdated begin!");
    int ret = 0;

    SessionInfo info;
    info.abilityName_ = "OnKeyboardPanelUpdated";
    info.bundleName_ = "OnKeyboardPanelUpdated";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    keyboardSession->isKeyboardPanelEnabled_ = false;
    keyboardSession->OnKeyboardPanelUpdated();

    keyboardSession->isKeyboardPanelEnabled_ = true;
    keyboardSession->specificCallback_ = nullptr;
    keyboardSession->OnKeyboardPanelUpdated();

    keyboardSession->specificCallback_ = specificCb;
    auto onUpdateAvoidArea = specificCb->onUpdateAvoidArea_;
    if (onUpdateAvoidArea == nullptr) {
        onUpdateAvoidArea = [](const int32_t& id){};
    }
    specificCb->onUpdateAvoidArea_ = nullptr;
    keyboardSession->OnKeyboardPanelUpdated();

    specificCb->onUpdateAvoidArea_ = onUpdateAvoidArea;
    keyboardSession->OnKeyboardPanelUpdated();

    ASSERT_EQ(ret, 0);
    WLOGFI("OnKeyboardPanelUpdated end!");
}

/**
 * @tc.name: SetCallingSessionId
 * @tc.desc: SetCallingSessionId
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, SetCallingSessionId, Function | SmallTest | Level1)
{
    WLOGFI("SetCallingSessionId begin!");
    SessionInfo info;
    info.abilityName_ = "SetCallingSessionId";
    info.bundleName_ = "SetCallingSessionId";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        new (std::nothrow) SceneSession::SpecificSessionCallback();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        new (std::nothrow) KeyboardSession::KeyboardSessionCallback();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    keyboardSession->keyboardCallback_ = nullptr;
    keyboardSession->SetCallingSessionId(0);

    keyboardSession->keyboardCallback_ = keyboardCb;
    auto onCallingSessionIdChange = keyboardSession->keyboardCallback_->onCallingSessionIdChange_;
    if (onCallingSessionIdChange == nullptr) {
        onCallingSessionIdChange = [](uint32_t Id){};
    }
    keyboardSession->keyboardCallback_->onCallingSessionIdChange_ = nullptr;
    keyboardSession->SetCallingSessionId(0);

    keyboardSession->keyboardCallback_->onCallingSessionIdChange_ = onCallingSessionIdChange;
    keyboardSession->SetCallingSessionId(0);

    WLOGFI("SetCallingSessionId end!");
}

/**
 * @tc.name: GetCallingSessionId
 * @tc.desc: GetCallingSessionId
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetCallingSessionId, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetCallingSessionId";
    info.bundleName_ = "GetCallingSessionId";
    sptr<KeyboardSession> keyboardSession = new (std::nothrow) KeyboardSession(info, nullptr, nullptr);
    EXPECT_NE(keyboardSession, nullptr);
    auto ret = keyboardSession->GetCallingSessionId();
    ASSERT_EQ(ret, INVALID_WINDOW_ID);
}

/**
 * @tc.name: AdjustKeyboardLayout
 * @tc.desc: AdjustKeyboardLayout
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, AdjustKeyboardLayout, Function | SmallTest | Level1)
{
    WLOGFI("AdjustKeyboardLayout begin!");
    SessionInfo info;
    info.abilityName_ = "AdjustKeyboardLayout";
    info.bundleName_ = "AdjustKeyboardLayout";
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

    WLOGFI("AdjustKeyboardLayout end!");
}

/**
 * @tc.name: CheckIfNeedRaiseCallingSession
 * @tc.desc: CheckIfNeedRaiseCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, CheckIfNeedRaiseCallingSession, Function | SmallTest | Level1)
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
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->SetSessionProperty(property);

    ASSERT_FALSE(keyboardSession->CheckIfNeedRaiseCallingSession(nullptr, true));

    sptr<SceneSession> sceneSession = new (std::nothrow) SceneSession(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);
    property->sessionGravity_ = SessionGravity::SESSION_GRAVITY_FLOAT;
    ASSERT_FALSE(keyboardSession->CheckIfNeedRaiseCallingSession(sceneSession, true));

    property->sessionGravity_ = SessionGravity::SESSION_GRAVITY_BOTTOM;
    ASSERT_TRUE(keyboardSession->CheckIfNeedRaiseCallingSession(sceneSession, false));

    property->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);

    keyboardSession->systemConfig_.uiType_ = "phone";
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
HWTEST_F(KeyboardSessionTest, OpenKeyboardSyncTransaction, Function | SmallTest | Level1)
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
HWTEST_F(KeyboardSessionTest, CloseKeyboardSyncTransaction1, Function | SmallTest | Level1)
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
HWTEST_F(KeyboardSessionTest, CloseKeyboardSyncTransaction2, Function | SmallTest | Level1)
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
HWTEST_F(KeyboardSessionTest, BindKeyboardPanelSession, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "BindKeyboardPanelSession";
    info.bundleName_ = "BindKeyboardPanelSession";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    sptr<SceneSession> panelSession = nullptr;
    keyboardSession->BindKeyboardPanelSession(panelSession);
    panelSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(panelSession, nullptr);
    keyboardSession->BindKeyboardPanelSession(panelSession);
    EXPECT_EQ(keyboardSession->keyboardPanelSession_, panelSession);
}

/**
 * @tc.name: GetKeyboardGravity01
 * @tc.desc: GetKeyboardGravity01
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetKeyboardGravity01, Function | SmallTest | Level1)
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
    keyboardSession->property_->sessionGravity_ = SessionGravity::SESSION_GRAVITY_BOTTOM;
    ASSERT_NE(keyboardSession, nullptr);
    ret = keyboardSession->GetKeyboardGravity();
    EXPECT_EQ(SessionGravity::SESSION_GRAVITY_BOTTOM, ret);
}

/**
 * @tc.name: GetCallingSessionId01
 * @tc.desc: GetCallingSessionId01
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetCallingSessionId01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetCallingSessionId";
    info.bundleName_ = "GetCallingSessionId";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->property_ = nullptr;
    auto ret = keyboardSession->GetCallingSessionId();
    EXPECT_EQ(ret, INVALID_SESSION_ID);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->SetCallingSessionId(1);
    ret = keyboardSession->GetCallingSessionId();
    EXPECT_EQ(ret, 1);
}

/**
 * @tc.name: NotifyKeyboardPanelInfoChange
 * @tc.desc: NotifyKeyboardPanelInfoChange
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, NotifyKeyboardPanelInfoChange, Function | SmallTest | Level1)
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
HWTEST_F(KeyboardSessionTest, CheckIfNeedRaiseCallingSession01, Function | SmallTest | Level1)
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
    keyboardSession->property_->sessionGravity_ = SessionGravity::SESSION_GRAVITY_BOTTOM;
    keyboardSession->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->systemConfig_.uiType_ = "phone";
    callingSession->systemConfig_.freeMultiWindowSupport_ = true;
    callingSession->systemConfig_.freeMultiWindowEnable_ = true;
    auto ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, false);
    callingSession->systemConfig_.freeMultiWindowEnable_ = false;
    ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, false);
    callingSession->systemConfig_.freeMultiWindowEnable_ = true;
    keyboardSession->systemConfig_.uiType_ = "pad";
    ret = keyboardSession->CheckIfNeedRaiseCallingSession(callingSession, true);
    EXPECT_EQ(ret, true);
    keyboardSession->systemConfig_.uiType_ = "pc";
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
HWTEST_F(KeyboardSessionTest, UpdateCallingSessionIdAndPosition01, Function | SmallTest | Level1)
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
HWTEST_F(KeyboardSessionTest, OpenKeyboardSyncTransaction01, Function | SmallTest | Level1)
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
HWTEST_F(KeyboardSessionTest, RelayoutKeyBoard01, Function | SmallTest | Level1)
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
    keyboardSession->property_->sessionGravity_ = SessionGravity::SESSION_GRAVITY_BOTTOM;
    keyboardSession->property_->sessionGravitySizePercent_ = 0;
    keyboardSession->RelayoutKeyBoard();
    keyboardSession->property_->sessionGravitySizePercent_ = 100;
    keyboardSession->RelayoutKeyBoard();
}

/**
 * @tc.name: Hide01
 * @tc.desc: test function : Hide
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, Hide01, Function | SmallTest | Level1)
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
    keyboardSession->state_ = SessionState::STATE_DISCONNECT;
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());
    keyboardSession->state_ = SessionState::STATE_CONNECT;
    keyboardSession->isActive_ = true;
    keyboardSession->systemConfig_.uiType_ = "phone";
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());
    keyboardSession->systemConfig_.uiType_ = "pc";
    keyboardSession->property_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(windowSessionProperty, nullptr);
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_NE(keyboardSession->property_, nullptr);
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());
}
}  // namespace
}  // namespace Rosen
}  // namespace OHOS
