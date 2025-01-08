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
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "KeyboardSessionTest"};
}

class KeyboardSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<SceneSession> GetSceneSession(const std::string& abilityName, const std::string& bundleName);
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

sptr<SceneSession> KeyboardSessionTest::GetSceneSession(const std::string& abilityName,
    const std::string& bundleName)
{
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);

    return sceneSession;
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
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, nullptr);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    keyboardSession->property_ = windowSessionProperty;
    ASSERT_EQ(SessionGravity::SESSION_GRAVITY_BOTTOM, keyboardSession->GetKeyboardGravity());
}

/**
 * @tc.name: Show01
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, Show01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "Show01";
    info.bundleName_ = "Show01";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    ASSERT_TRUE((keyboardSession != nullptr));
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);

    keyboardSession->isKeyboardPanelEnabled_ = true;
    ASSERT_EQ(WSError::WS_OK, keyboardSession->Show(property));

    keyboardSession->isKeyboardPanelEnabled_ = false;
    ASSERT_EQ(WSError::WS_OK, keyboardSession->Show(property));

    ASSERT_NE(nullptr, keyboardSession->property_);
    KeyboardLayoutParams params;
    params.gravity_ = WindowGravity::WINDOW_GRAVITY_FLOAT;
    keyboardSession->property_->SetKeyboardLayoutParams(params);
    ASSERT_NE(SessionGravity::SESSION_GRAVITY_BOTTOM, keyboardSession->GetKeyboardGravity());
    ASSERT_EQ(WSError::WS_OK, keyboardSession->Show(property));
}

/**
 * @tc.name: Show02
 * @tc.desc: test function : Show
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, Show02, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "Show02";
    info.bundleName_ = "Show02";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    ASSERT_TRUE((keyboardSession != nullptr));
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, keyboardSession->Show(nullptr));
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
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
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
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    keyboardSession->SetSessionProperty(property);
    keyboardSession->isActive_ = true;
    auto result = keyboardSession->Disconnect();
    ASSERT_EQ(result, WSError::WS_OK);
}

/**
 * @tc.name: Disconnect01
 * @tc.desc: test system keyboard disconnect
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, DisConnect01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "DisConnect01";
    info.bundleName_ = "DisConnect01";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_EQ(WSError::WS_OK, keyboardSession->Disconnect());

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    ASSERT_EQ(WSError::WS_OK, keyboardSession->SetSessionProperty(property));
    keyboardSession->isActive_ = true;
    ASSERT_EQ(WSError::WS_OK, keyboardSession->Disconnect());

    SessionInfo panelInfo;
    panelInfo.abilityName_ = "systemKeyboardPanel";
    panelInfo.bundleName_ = "systemKeyboardPanel";
    sptr<SystemSession> panelSession = sptr<SystemSession>::MakeSptr(panelInfo, nullptr);
    keyboardSession->BindKeyboardPanelSession(panelSession);
    ASSERT_EQ(WSError::WS_OK, keyboardSession->Disconnect());
    panelSession->SetIsSystemKeyboard(true);
    ASSERT_EQ(WSError::WS_OK, keyboardSession->Disconnect());
}

/**
 * @tc.name: GetSceneSession01
 * @tc.desc: GetSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetSceneSession01, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetSceneSession01";
    info.bundleName_ = "GetSceneSession01";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);
    auto id = sceneSession->GetPersistentId();
    EXPECT_NE(id, 0);
    auto ret = keyboardSession->GetSceneSession(id);

    keyboardCb->onGetSceneSession = [](uint32_t) {
        return nullptr;
    };
    EXPECT_NE(keyboardCb->onGetSceneSession, nullptr);
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
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<SceneSession> callingSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    WSRect rect = { 0, 0, 1260, 2720 };
    WSRect occupiedArea = { 0, 1700, 1260, 1020 };
    keyboardSession->NotifyOccupiedAreaChangeInfo(callingSession, rect, occupiedArea);

    keyboardSession->NotifyOccupiedAreaChangeInfo(callingSession, rect, occupiedArea);

    callingSession->lastSafeRect = { 0, 0, 0, 0 };
    callingSession->GetSessionProperty()->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);
    keyboardSession->NotifyOccupiedAreaChangeInfo(callingSession, rect, occupiedArea);
}

/**
 * @tc.name: NotifyRootSceneOccupiedAreaChange
 * @tc.desc: NotifyRootSceneOccupiedAreaChange
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, NotifyRootSceneOccupiedAreaChange, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "NotifyRootSceneOccupiedAreaChange";
    info.bundleName_ = "NotifyRootSceneOccupiedAreaChange";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    auto occupiedInfo = sptr<OccupiedAreaChangeInfo>::MakeSptr();
    ASSERT_NE(nullptr, occupiedInfo);
    keyboardSession->NotifyRootSceneOccupiedAreaChange(occupiedInfo);
    keyboardSession->GetSessionProperty()->SetDisplayId(2025);
    keyboardSession->NotifyRootSceneOccupiedAreaChange(occupiedInfo);
    keyboardSession->GetSessionProperty()->SetDisplayId(0);
    keyboardSession->keyboardCallback_->onNotifyOccupiedAreaChange = nullptr;
    keyboardSession->NotifyRootSceneOccupiedAreaChange(occupiedInfo);
    keyboardSession->keyboardCallback_ = nullptr;
    keyboardSession->NotifyRootSceneOccupiedAreaChange(occupiedInfo);
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
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    // callingSession is nullptr
    keyboardSession->RestoreCallingSession();

    // callingsession is not nullptr
    info.windowType_ = 1; // 1 is main_window_type
    sptr<SceneSession> callingSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    EXPECT_NE(callingSession, nullptr);
    ASSERT_NE(keyboardSession->keyboardCallback_, nullptr);
    keyboardSession->keyboardCallback_->onGetSceneSession =
        [callingSession](int32_t persistentId)->sptr<SceneSession> {
        return callingSession;
    };
    keyboardSession->RestoreCallingSession();
    ASSERT_EQ(callingSession->GetOriPosYBeforeRaisedByKeyboard(), 0); // 0: default value

    callingSession->SetOriPosYBeforeRaisedByKeyboard(100); // 100 is not default
    ASSERT_NE(callingSession->property_, nullptr);
    callingSession->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    keyboardSession->RestoreCallingSession();
    ASSERT_EQ(callingSession->GetOriPosYBeforeRaisedByKeyboard(), 0); // 0: default value
}

/**
 * @tc.name: RestoreCallingSession02
 * @tc.desc: RestoreCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, RestoreCallingSession02, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "RestoreCallingSession02";
    info.bundleName_ = "RestoreCallingSession02";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    info.windowType_ = 1; // 1 is main_window_type
    sptr<SceneSession> callingSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    ASSERT_NE(keyboardSession->keyboardCallback_, nullptr);
    keyboardSession->keyboardCallback_->onGetSceneSession =
        [callingSession](int32_t persistentId)->sptr<SceneSession> {
        return callingSession;
    };

    keyboardSession->keyboardAvoidAreaActive_ = false;
    keyboardSession->RestoreCallingSession();
    ASSERT_EQ(callingSession->GetOriPosYBeforeRaisedByKeyboard(), 0); // 0: default value
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
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
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
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    info.windowType_ = 1;
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    EXPECT_NE(sceneSession, nullptr);
    auto id = sceneSession->GetPersistentId();
    EXPECT_NE(id, 0);

    keyboardSession->UpdateCallingSessionIdAndPosition(id);
}

/**
 * @tc.name: GetFocusedSessionId
 * @tc.desc: GetFocusedSessionId
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, GetFocusedSessionId, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "GetFocusedSessionId";
    info.bundleName_ = "GetFocusedSessionId";
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    keyboardCb->onGetFocusedSessionId = []()
    {
        return 0;
    };
    EXPECT_NE(keyboardCb->onGetFocusedSessionId, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    ASSERT_EQ(INVALID_WINDOW_ID, keyboardSession->GetFocusedSessionId());

    keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    EXPECT_NE(keyboardSession, nullptr);

    ASSERT_EQ(INVALID_WINDOW_ID, keyboardSession->GetFocusedSessionId());
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
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
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
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    // keyboardCallback_->onGetSceneSession is nullptr, getCallingSession is nullptr
    keyboardSession->SetCallingSessionId(0);
    ASSERT_EQ(keyboardSession->GetCallingSessionId(), INVALID_SESSION_ID);

    // getCallingSession is not nullptr
    info.windowType_ = 1; // 1 is main_window_type
    sptr<SceneSession> callingSession = sptr<SceneSession>::MakeSptr(info, specificCb);
    EXPECT_NE(callingSession, nullptr);
    ASSERT_NE(keyboardSession->keyboardCallback_, nullptr);
    keyboardSession->keyboardCallback_->onGetSceneSession =
        [callingSession](int32_t persistenId)->sptr<SceneSession> {
        if (persistenId != 100) { // callingSession's persistentId is 100
            return nullptr;
        }
        return callingSession;
    };
    keyboardSession->keyboardCallback_->onGetFocusedSessionId = []()->int32_t {
        return 100; // focusSession's persistentId is 100
    };
    keyboardSession->SetCallingSessionId(0);
    ASSERT_EQ(keyboardSession->GetCallingSessionId(), 100); // 100 is callingSessionId

    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->SetCallingSessionId(INVALID_SESSION_ID);
    keyboardSession->keyboardCallback_->onCallingSessionIdChange = [](int32_t callingSessionid){};
    keyboardSession->SetCallingSessionId(100);
    ASSERT_EQ(keyboardSession->GetCallingSessionId(), 100); // 100 is callingSessionId
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
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    EXPECT_NE(keyboardSession, nullptr);
    auto ret = keyboardSession->GetCallingSessionId();
    ASSERT_EQ(ret, INVALID_WINDOW_ID);
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
    auto ret = keyboardSession->GetCallingSessionId();
    EXPECT_EQ(ret, INVALID_SESSION_ID);
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->property_->SetCallingSessionId(1);
    ret = keyboardSession->GetCallingSessionId();
    EXPECT_EQ(ret, 1);
}

/**
 * @tc.name: NotifySystemKeyboardAvoidChange
 * @tc.desc: test NotifySystemKeyboardAvoidChange
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest, NotifySystemKeyboardAvoidChange, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "NotifySystemKeyboardAvoidChange";
    info.bundleName_ = "NotifySystemKeyboardAvoidChange";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);

    keyboardSession->NotifySystemKeyboardAvoidChange(SystemKeyboardAvoidChangeReason::KEYBOARD_CREATED);
    ASSERT_EQ(true, keyboardSession->keyboardAvoidAreaActive_);

    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    keyboardSession->NotifySystemKeyboardAvoidChange(SystemKeyboardAvoidChangeReason::KEYBOARD_CREATED);
    ASSERT_EQ(true, keyboardSession->keyboardAvoidAreaActive_);

    keyboardSession->SetIsSystemKeyboard(true);
    keyboardSession->NotifySystemKeyboardAvoidChange(SystemKeyboardAvoidChangeReason::KEYBOARD_CREATED);
    ASSERT_EQ(true, keyboardSession->keyboardAvoidAreaActive_);

    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCallback =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    keyboardSession->keyboardCallback_ = keyboardCallback;
    keyboardSession->NotifySystemKeyboardAvoidChange(SystemKeyboardAvoidChangeReason::KEYBOARD_CREATED);
    ASSERT_EQ(true, keyboardSession->keyboardAvoidAreaActive_);

    ASSERT_NE(nullptr, keyboardSession->keyboardCallback_);
    keyboardSession->keyboardCallback_->onSystemKeyboardAvoidChange =
        [](DisplayId displayId, SystemKeyboardAvoidChangeReason reason) {};
    keyboardSession->NotifySystemKeyboardAvoidChange(SystemKeyboardAvoidChangeReason::KEYBOARD_CREATED);
    ASSERT_EQ(true, keyboardSession->keyboardAvoidAreaActive_);
}
}  // namespace
}  // namespace Rosen
}  // namespace OHOS
