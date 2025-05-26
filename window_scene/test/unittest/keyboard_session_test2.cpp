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
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "ui/rs_surface_node.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "KeyboardSessionTest2" };
std::string g_logMsg;
void KeyboardSessionTest2Callback(const LogType type, const LogLevel level,
    const unsigned int domain, const char *tag, const char *msg)
{
    g_logMsg = msg;
}
}

constexpr int WAIT_ASYNC_US = 1000000;
class KeyboardSessionTest2 : public testing::Test, public IScreenConnectionListener {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void OnScreenConnected(const sptr<ScreenSession>& screenSession) override;
    void OnScreenDisconnected(const sptr<ScreenSession>& screenSession) override;

private:
    sptr<KeyboardSession> GetKeyboardSession(const std::string& abilityName, const std::string& bundleName);
    sptr<SceneSession> GetSceneSession(const std::string& abilityName, const std::string& bundleName);
    sptr<KSSceneSessionMocker> GetSceneSessionMocker(const std::string& abilityName, const std::string& bundleName);
};

void KeyboardSessionTest2::SetUpTestCase() {}

void KeyboardSessionTest2::TearDownTestCase() {}

void KeyboardSessionTest2::SetUp() {}

void KeyboardSessionTest2::TearDown() {}

void KeyboardSessionTest2::OnScreenConnected(const sptr<ScreenSession>& screenSession) {}

void KeyboardSessionTest2::OnScreenDisconnected(const sptr<ScreenSession>& screenSession) {}

sptr<KeyboardSession> KeyboardSessionTest2::GetKeyboardSession(const std::string& abilityName,
                                                               const std::string& bundleName)
{
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    sptr<WindowSessionProperty> keyboardProperty = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(keyboardProperty, nullptr);
    keyboardProperty->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->SetSessionProperty(keyboardProperty);

    return keyboardSession;
}

sptr<SceneSession> KeyboardSessionTest2::GetSceneSession(const std::string& abilityName, const std::string& bundleName)
{
    SessionInfo info;
    info.abilityName_ = abilityName;
    info.bundleName_ = bundleName;
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
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
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
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
HWTEST_F(KeyboardSessionTest2, AdjustKeyboardLayout01, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "AdjustKeyboardLayout01";
    info.bundleName_ = "AdjustKeyboardLayout01";
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    keyboardSession->SetSessionProperty(property);

    KeyboardLayoutParams params;
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);

    keyboardSession->adjustKeyboardLayoutFunc_ = nullptr;
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);

    keyboardSession->adjustKeyboardLayoutFunc_ = [](const KeyboardLayoutParams& params) {};
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);
}

/**
 * @tc.name: AdjustKeyboardLayout01
 * @tc.desc: AdjustKeyboardLayout
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, AdjustKeyboardLayout02, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "AdjustKeyboardLayout02";
    info.bundleName_ = "AdjustKeyboardLayout02";
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);

    KeyboardLayoutParams params;
    // params gravity is WINDOW_GRAVITY_BOTTOM
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);
    keyboardSession->state_ = SessionState::STATE_FOREGROUND;
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);

    // params gravity is WINDOW_GRAVITY_FLOAT
    params.gravity_ = WindowGravity::WINDOW_GRAVITY_FLOAT;
    ASSERT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);
}

/**
 * @tc.name: AdjustKeyboardLayout03
 * @tc.desc: AdjustKeyboardLayout test
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, AdjustKeyboardLayout03, Function | SmallTest | Level1)
{
    SessionInfo info;
    info.abilityName_ = "AdjustKeyboardLayout03";
    info.bundleName_ = "AdjustKeyboardLayout03";
    sptr<SceneSession::SpecificSessionCallback> specificCb =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    ASSERT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    ASSERT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    ASSERT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    keyboardSession->SetSessionProperty(property);

    KeyboardLayoutParams params;
    EXPECT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);

    keyboardSession->adjustKeyboardLayoutFunc_ = nullptr;
    EXPECT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);

    keyboardSession->adjustKeyboardLayoutFunc_ = [](const KeyboardLayoutParams& params){};
    EXPECT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);

    params.displayId_ = 100;
    EXPECT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);

    keyboardSession->adjustKeyboardLayoutFunc_ = nullptr;
    EXPECT_EQ(keyboardSession->AdjustKeyboardLayout(params), WSError::WS_OK);
}

/**
 * @tc.name: HandleMoveDragSurfaceNode01
 * @tc.desc: HandleMoveDragSurfaceNode test
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, HandleMoveDragSurfaceNode01, Function | SmallTest | Level1)
{
    g_logMsg.clear();
    LOG_SetCallback(KeyboardSessionTest2Callback);
    sptr<KeyboardSession> keyboardSession =
        GetKeyboardSession("HandleMoveDragSurfaceNode01", "HandleMoveDragSurfaceNode01");
    ASSERT_NE(keyboardSession, nullptr);

    ScreenSessionManagerClient::GetInstance().RegisterScreenConnectionListener(this);
    SessionOption option = {
        .rsId_ = 0,
        .isExtend_ = false,
        .screenId_ = 0,
    };
    ScreenEvent screenEvent = ScreenEvent::CONNECTED;
    ScreenSessionManagerClient::GetInstance().OnScreenConnectionChanged(option, screenEvent);

    sptr<MoveDragController> moveDragController = keyboardSession->GetMoveDragController();
    ASSERT_NE(moveDragController, nullptr);
    WSRect rect = { 0, 0, 200, 200 };
    moveDragController->SetTargetRect(rect);
    keyboardSession->HandleMoveDragSurfaceNode(SizeChangeReason::DRAG_MOVE);
    EXPECT_TRUE(g_logMsg.find("keyboard panel session is null") != std::string::npos);

    moveDragController->ResetCrossMoveDragProperty();
    moveDragController->SetTargetRect(rect);
    SessionInfo info;
    info.abilityName_ = "HandleMoveDragSurfaceNode01";
    info.bundleName_ = "HandleMoveDragSurfaceNode01";
    sptr<SceneSession> panelSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(panelSession, nullptr);
    keyboardSession->BindKeyboardPanelSession(panelSession);
    keyboardSession->HandleMoveDragSurfaceNode(SizeChangeReason::DRAG_MOVE);
    EXPECT_TRUE(g_logMsg.find("keyboard panel surface node is null") != std::string::npos);

    moveDragController->ResetCrossMoveDragProperty();
    moveDragController->SetTargetRect(rect);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    panelSession->SetSurfaceNode(surfaceNode);
    keyboardSession->HandleMoveDragSurfaceNode(SizeChangeReason::DRAG_MOVE);
    EXPECT_TRUE(g_logMsg.find("Add widow") != std::string::npos);

    keyboardSession->HandleMoveDragSurfaceNode(SizeChangeReason::DRAG_END);
    EXPECT_TRUE(g_logMsg.find("Remove window") != std::string::npos);
}

/**
 * @tc.name: CheckIfNeedRaiseCallingSession
 * @tc.desc: CheckIfNeedRaiseCallingSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, CheckIfNeedRaiseCallingSession, TestSize.Level1)
{
    WLOGFI("CheckIfNeedRaiseCallingSession begin!");
    SessionInfo info;
    info.abilityName_ = "CheckIfNeedRaiseCallingSession";
    info.bundleName_ = "CheckIfNeedRaiseCallingSession";
    sptr<SceneSession::SpecificSessionCallback> specificCb = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    EXPECT_NE(specificCb, nullptr);
    sptr<KeyboardSession::KeyboardSessionCallback> keyboardCb =
        sptr<KeyboardSession::KeyboardSessionCallback>::MakeSptr();
    EXPECT_NE(keyboardCb, nullptr);
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, specificCb, keyboardCb);
    EXPECT_NE(keyboardSession, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    keyboardSession->SetSessionProperty(property);

    ASSERT_FALSE(keyboardSession->CheckIfNeedRaiseCallingSession(nullptr, true));

    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, specificCb);
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
HWTEST_F(KeyboardSessionTest2, OpenKeyboardSyncTransaction, TestSize.Level1)
{
    std::string abilityName = "OpenKeyboardSyncTransaction";
    std::string bundleName = "OpenKeyboardSyncTransaction";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);

    WSRect keyboardPanelRect = { 0, 0, 0, 0 };
    bool isKeyboardShow = true;
    WindowAnimationInfo animationInfo;

    // isKeyBoardSyncTransactionOpen_ is false
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, isKeyboardShow, animationInfo);
    keyboardSession->OpenKeyboardSyncTransaction();

    // isKeyBoardSyncTransactionOpen_ is true
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, isKeyboardShow, animationInfo);
    keyboardSession->OpenKeyboardSyncTransaction();
    ASSERT_EQ(keyboardSession->isKeyboardSyncTransactionOpen_, true);
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, isKeyboardShow, animationInfo);
    ASSERT_EQ(keyboardSession->isKeyboardSyncTransactionOpen_, false);
}

/**
 * @tc.name: BindKeyboardPanelSession
 * @tc.desc: BindKeyboardPanelSession
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, BindKeyboardPanelSession, TestSize.Level0)
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
HWTEST_F(KeyboardSessionTest2, GetKeyboardGravity01, TestSize.Level0)
{
    SessionInfo info;
    info.abilityName_ = "GetKeyboardGravity";
    info.bundleName_ = "GetKeyboardGravity";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    sptr<WindowSessionProperty> windowSessionProperty = sptr<WindowSessionProperty>::MakeSptr();
    keyboardSession->property_ = windowSessionProperty;
    keyboardSession->property_->keyboardLayoutParams_.gravity_ = WindowGravity::WINDOW_GRAVITY_BOTTOM;
    auto ret = keyboardSession->GetKeyboardGravity();
    EXPECT_EQ(SessionGravity::SESSION_GRAVITY_BOTTOM, ret);
}

/**
 * @tc.name: NotifyKeyboardPanelInfoChange
 * @tc.desc: NotifyKeyboardPanelInfoChange
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, NotifyKeyboardPanelInfoChange, TestSize.Level1)
{
    WSRect rect = { 800, 800, 1200, 1200 };
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
HWTEST_F(KeyboardSessionTest2, CheckIfNeedRaiseCallingSession01, TestSize.Level1)
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
 * @tc.name: OpenKeyboardSyncTransaction01
 * @tc.desc: OpenKeyboardSyncTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, OpenKeyboardSyncTransaction01, TestSize.Level1)
{
    SessionInfo info;
    info.abilityName_ = "OpenKeyboardSyncTransaction01";
    info.bundleName_ = "OpenKeyboardSyncTransaction01";
    sptr<KeyboardSession> keyboardSession = sptr<KeyboardSession>::MakeSptr(info, nullptr, nullptr);
    ASSERT_NE(keyboardSession, nullptr);
    keyboardSession->isKeyboardSyncTransactionOpen_ = true;
    keyboardSession->OpenKeyboardSyncTransaction();
    keyboardSession->isKeyboardSyncTransactionOpen_ = false;
    keyboardSession->OpenKeyboardSyncTransaction();
    WSRect keyboardPanelRect = {0, 0, 0, 0};
    WindowAnimationInfo animationInfo;
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, true, animationInfo);
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, false, animationInfo);
}

/**
 * @tc.name: Hide01
 * @tc.desc: test function: Hide
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, Hide01, TestSize.Level0)
{
    SessionInfo info;
    info.abilityName_ = "Hide01";
    info.bundleName_ = "Hide01";
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
    keyboardSession->sessionStage_ = sptr<SessionStageMocker>::MakeSptr();
    ASSERT_NE(keyboardSession->sessionStage_, nullptr);
    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());

    // deviceType is pc and property is not nullptr
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->systemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    EXPECT_EQ(WSError::WS_OK, keyboardSession->Hide());

    // Hide system keyboard
    ASSERT_EQ(false, keyboardSession->IsSystemKeyboard());
    keyboardSession->SetIsSystemKeyboard(true);
    ASSERT_EQ(true, keyboardSession->IsSystemKeyboard());
    ASSERT_EQ(WSError::WS_OK, keyboardSession->Hide());
}

/**
 * @tc.name: CloseKeyBoardSyncTransaction3
 * @tc.desc: test function: CloseKeyBoardSyncTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, CloseKeyBoardSyncTransaction3, TestSize.Level1)
{
    std::string abilityName = "CloseKeyBoardSyncTransaction3";
    std::string bundleName = "CloseKeyBoardSyncTransaction3";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);
    ASSERT_NE(keyboardSession, nullptr);

    WSRect keyboardPanelRect;
    bool isKeyboardShow = true;
    WindowAnimationInfo animationInfo;

    keyboardSession->dirtyFlags_ = 0;
    keyboardSession->specificCallback_->onUpdateAvoidArea_ = [](uint32_t callingSessionId) {};
    keyboardSession->isKeyboardSyncTransactionOpen_ = true;
    // isKeyBoardSyncTransactionOpen_ is true
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, isKeyboardShow, animationInfo);
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(keyboardSession->isKeyboardSyncTransactionOpen_, false);
}

/**
 * @tc.name: CloseKeyboardSyncTransaction4
 * @tc.desc: test function: CloseKeyboardSyncTransaction
 * @tc.type: FUNC
 */
HWTEST_F(KeyboardSessionTest2, CloseKeyboardSyncTransaction4, TestSize.Level1)
{
    std::string abilityName = "CloseKeyboardSyncTransaction4";
    std::string bundleName = "CloseKeyboardSyncTransaction4";
    sptr<KeyboardSession> keyboardSession = GetKeyboardSession(abilityName, bundleName);
    ASSERT_NE(keyboardSession, nullptr);
    WSRect keyboardPanelRect = { 0, 0, 0, 0 };
    WindowAnimationInfo animationInfo;
    keyboardSession->dirtyFlags_ = 0;

    keyboardSession->isKeyboardSyncTransactionOpen_ = false;
    ASSERT_NE(keyboardSession->property_, nullptr);
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, false, animationInfo);
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(0, keyboardSession->dirtyFlags_);
    keyboardSession->isKeyboardSyncTransactionOpen_ = true;
    keyboardSession->property_->SetCallingSessionId(1);
    keyboardSession->CloseKeyboardSyncTransaction(keyboardPanelRect, false, animationInfo);
    usleep(WAIT_ASYNC_US);
    auto callingSessionId = keyboardSession->property_->GetCallingSessionId();
    ASSERT_EQ(callingSessionId, INVALID_WINDOW_ID);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
