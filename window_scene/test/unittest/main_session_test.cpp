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
#include "session/host/include/main_session.h"

#include "common/include/session_permission.h"
#include "key_event.h"
#include "mock/mock_session_stage.h"
#include "session/host/include/session.h"
#include "session/host/include/system_session.h"
#include <ui/rs_surface_node.h>
#include "window_event_channel_base.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MainSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    SessionInfo info;
    sptr<MainSession::SpecificSessionCallback> specificCallback = nullptr;
    sptr<MainSession> mainSession_;
private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
};

void MainSessionTest::SetUpTestCase()
{
}

void MainSessionTest::TearDownTestCase()
{
}

void MainSessionTest::SetUp()
{
    SessionInfo info;
    info.abilityName_ = "testMainSession1";
    info.moduleName_ = "testMainSession2";
    info.bundleName_ = "testMainSession3";
    mainSession_ = sptr<MainSession>::MakeSptr(info, specificCallback);
    EXPECT_NE(nullptr, mainSession_);
}

void MainSessionTest::TearDown()
{
    mainSession_ = nullptr;
}

RSSurfaceNode::SharedPtr MainSessionTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "WindowSessionTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    return surfaceNode;
}

namespace {

/**
 * @tc.name: MainSession01
 * @tc.desc: check func MainSession
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, MainSession01, Function | SmallTest | Level1)
{
    MainSession* pMainSession = nullptr;
    sptr<MainSession::SpecificSessionCallback> pSpecificCallback = nullptr;

    SessionInfo info;
    info.persistentId_ = -1;
    info.abilityName_ = "";
    info.moduleName_ = "";
    info.bundleName_ = "";
    pMainSession = sptr<MainSession>::MakeSptr(info, pSpecificCallback);
    EXPECT_NE(nullptr, pMainSession);

    info.persistentId_ = 0;
    pMainSession = sptr<MainSession>::MakeSptr(info, pSpecificCallback);
    EXPECT_NE(nullptr, pMainSession);

    info.persistentId_ = -1;
    info.abilityName_ = "MainSession01";
    info.moduleName_ = "MainSession02";
    info.bundleName_ = "MainSession03";
    pSpecificCallback = new(std::nothrow) MainSession::SpecificSessionCallback;
    pMainSession = sptr<MainSession>::MakeSptr(info, pSpecificCallback);
    EXPECT_NE(nullptr, pMainSession);

    info.persistentId_ = 0;
    pMainSession = sptr<MainSession>::MakeSptr(info, pSpecificCallback);
    EXPECT_NE(nullptr, pMainSession);
}

/**
 * @tc.name: TransferKeyEvent01
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, TransferKeyEvent01, Function | SmallTest | Level1)
{
    mainSession_->state_ = SessionState::STATE_END;

    ASSERT_EQ(WSError::WS_ERROR_INVALID_SESSION, mainSession_->TransferKeyEvent(nullptr));
}

/**
 * @tc.name: TransferKeyEvent02
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, TransferKeyEvent02, Function | SmallTest | Level1)
{
    mainSession_->state_ = SessionState::STATE_CONNECT;
    std::shared_ptr<MMI::KeyEvent> keyEvent = nullptr;
    mainSession_->ClearDialogVector();
    ASSERT_EQ(WSError::WS_ERROR_NULLPTR, mainSession_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: TransferKeyEvent03
 * @tc.desc: check func TransferKeyEvent
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, TransferKeyEvent03, Function | SmallTest | Level1)
{
    mainSession_->state_ = SessionState::STATE_CONNECT;
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    ASSERT_NE(keyEvent, nullptr);
    SessionInfo info;
    info.abilityName_ = "testDialogSession1";
    info.moduleName_ = "testDialogSession2";
    info.bundleName_ = "testDialogSession3";
    sptr<Session> dialogSession = sptr<SystemSession>::MakeSptr(info, nullptr);
    dialogSession->SetSessionState(SessionState::STATE_ACTIVE);
    mainSession_->BindDialogToParentSession(dialogSession);

    ASSERT_EQ(WSError::WS_ERROR_INVALID_PERMISSION, mainSession_->TransferKeyEvent(keyEvent));
}

/**
 * @tc.name: ProcessPointDownSession01
 * @tc.desc: check func ProcessPointDownSession
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, ProcessPointDownSession01, Function | SmallTest | Level1)
{
    EXPECT_EQ(WSError::WS_OK, mainSession_->ProcessPointDownSession(100, 200));
    mainSession_->ClearDialogVector();
    EXPECT_EQ(WSError::WS_OK, mainSession_->ProcessPointDownSession(10, 20));
}

/**
 * @tc.name: ProcessPointDownSession02
 * @tc.desc: check func ProcessPointDownSession
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, ProcessPointDownSession02, Function | SmallTest | Level1)
{
    mainSession_->BindDialogToParentSession(mainSession_);
    EXPECT_EQ(WSError::WS_OK, mainSession_->ProcessPointDownSession(10, 20));
}

/**
 * @tc.name: SetTopmost01
 * @tc.desc: check func SetTopmost
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, SetTopmost01, Function | SmallTest | Level1)
{
    EXPECT_EQ(WSError::WS_OK, mainSession_->SetTopmost(true));
    EXPECT_EQ(WSError::WS_OK, mainSession_->SetTopmost(false));
}

/**
 * @tc.name: SetTopmost02
 * @tc.desc: check func SetTopmost
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, SetTopmost02, Function | SmallTest | Level1)
{
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    mainSession_->SetSessionProperty(property);
    ASSERT_TRUE(mainSession_->GetSessionProperty() != nullptr);
    EXPECT_EQ(WSError::WS_OK, mainSession_->SetTopmost(true));
}

/**
 * @tc.name: UpdatePointerArea
 * @tc.desc: check func UpdatePointerArea
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, UpdatePointerArea, Function | SmallTest | Level1)
{
    WSRect Rect={0, 0, 50, 50};
    mainSession_->UpdateWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    mainSession_->UpdatePointerArea(Rect);
    mainSession_->UpdateWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    mainSession_->UpdatePointerArea(Rect);
    ASSERT_EQ(Rect, mainSession_->preRect_);
}

/**
 * @tc.name: CheckPointerEventDispatch
 * @tc.desc: check func CheckPointerEventDispatch
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, CheckPointerEventDispatch, Function | SmallTest | Level1)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();

    mainSession_->SetSessionState(SessionState::STATE_FOREGROUND);
    bool res = mainSession_->CheckPointerEventDispatch(pointerEvent);
    ASSERT_EQ(res, true);

    mainSession_->SetSessionState(SessionState::STATE_ACTIVE);
    res = mainSession_->CheckPointerEventDispatch(pointerEvent);
    ASSERT_EQ(res, true);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW);
    mainSession_->SetSessionState(SessionState::STATE_DISCONNECT);
    res = mainSession_->CheckPointerEventDispatch(pointerEvent);
    ASSERT_EQ(res, true);

    pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_PULL_DOWN);
    mainSession_->SetSessionState(SessionState::STATE_DISCONNECT);
    res = mainSession_->CheckPointerEventDispatch(pointerEvent);
    ASSERT_EQ(res, false);
}

/**
 * @tc.name: RectCheck03
 * @tc.desc: check func RectCheck
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, RectCheck03, Function | SmallTest | Level1)
{
    ASSERT_NE(mainSession_, nullptr);
    SessionInfo info;
    info.abilityName_ = "testMainSessionRectCheck";
    info.moduleName_ = "testMainSessionRectCheck";
    info.bundleName_ = "testMainSessionRectCheck";
    sptr<Session> session = sptr<Session>::MakeSptr(info);
    EXPECT_NE(nullptr, session);
    mainSession_->parentSession_ = session;
    uint32_t curWidth = 100;
    uint32_t curHeight = 200;
    mainSession_->RectCheck(curWidth, curHeight);

    curWidth = 300;
    curHeight = 200;
    mainSession_->RectCheck(curWidth, curHeight);

    curWidth = 1930;
    curHeight = 200;
    mainSession_->RectCheck(curWidth, curHeight);

    curWidth = 330;
    curHeight = 200;
    mainSession_->RectCheck(curWidth, curHeight);

    curWidth = 330;
    curHeight = 1930;
    mainSession_->RectCheck(curWidth, curHeight);
}

/**
 * @tc.name: SetExitSplitOnBackground
 * @tc.desc: check func SetExitSplitOnBackground
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, SetExitSplitOnBackground, Function | SmallTest | Level1)
{
    bool isExitSplitOnBackground = true;
    mainSession_->SetExitSplitOnBackground(isExitSplitOnBackground);
    ASSERT_EQ(true, isExitSplitOnBackground);
}

/**
 * @tc.name: IsExitSplitOnBackground01
 * @tc.desc: check func IsExitSplitOnBackground
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, IsExitSplitOnBackground01, Function | SmallTest | Level1)
{
    bool isExitSplitOnBackground = true;
    mainSession_->SetExitSplitOnBackground(isExitSplitOnBackground);
    bool ret = mainSession_->IsExitSplitOnBackground();
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: IsExitSplitOnBackground02
 * @tc.desc: check func IsExitSplitOnBackground
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, IsExitSplitOnBackground02, Function | SmallTest | Level1)
{
    bool isExitSplitOnBackground = false;
    mainSession_->SetExitSplitOnBackground(isExitSplitOnBackground);
    bool ret = mainSession_->IsExitSplitOnBackground();
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: OnTitleAndDockHoverShowChange
 * @tc.desc: OnTitleAndDockHoverShowChange
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, OnTitleAndDockHoverShowChange, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnTitleAndDockHoverShowChange";
    info.bundleName_ = "OnTitleAndDockHoverShowChange";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sceneSession->SetTitleAndDockHoverShowChangeCallback([](bool isTitleHoverShown, bool isDockHoverShown) {
        return;
    });
    EXPECT_NE(sceneSession->onTitleAndDockHoverShowChangeFunc_, nullptr);
    EXPECT_EQ(sceneSession->OnTitleAndDockHoverShowChange(true, true), WSError::WS_OK);
}

/**
 * @tc.name: OnRestoreMainWindow
 * @tc.desc: OnRestoreMainWindow function01
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, OnRestoreMainWindow, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnRestoreMainWindow";
    info.bundleName_ = "OnRestoreMainWindow";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    EXPECT_EQ(WSError::WS_OK, session->OnRestoreMainWindow());

    session->onRestoreMainWindowFunc_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, session->OnRestoreMainWindow());

    NotifyRestoreMainWindowFunc func = [] {
        return;
    };
    session->onRestoreMainWindowFunc_ = func;
    EXPECT_EQ(WSError::WS_OK, session->OnRestoreMainWindow());
}

/**
 * @tc.name: OnSetWindowRectAutoSave
 * @tc.desc: OnSetWindowRectAutoSave
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, OnSetWindowRectAutoSave, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnSetWindowRectAutoSave";
    info.bundleName_ = "OnSetWindowRectAutoSave";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);

    session->onSetWindowRectAutoSaveFunc_ = nullptr;
    EXPECT_EQ(nullptr, session->onSetWindowRectAutoSaveFunc_);

    NotifySetWindowRectAutoSaveFunc func = [](bool enabled, bool isSaveBySpecifiedFlag) {
        return;
    };
    session->onSetWindowRectAutoSaveFunc_ = func;
    EXPECT_NE(nullptr, session->onSetWindowRectAutoSaveFunc_);
    EXPECT_EQ(false, session->GetSessionProperty()->GetIsSaveBySpecifiedFlag());
}

/**
 * @tc.name: NotifyMainModalTypeChange
 * @tc.desc: NotifyMainModalTypeChange function01
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, NotifyMainModalTypeChange, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyMainModalTypeChange";
    info.bundleName_ = "NotifyMainModalTypeChange";
    sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    sceneSession->RegisterMainModalTypeChangeCallback([](bool isModal) {
        return;
    });
    EXPECT_NE(sceneSession->onMainModalTypeChange_, nullptr);
    EXPECT_EQ(WSError::WS_OK, sceneSession->NotifyMainModalTypeChange(true));
}

/**
 * @tc.name: IsModal
 * @tc.desc: IsModal function01
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, IsModal, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "IsModal";
    info.bundleName_ = "IsModal";
    sptr<MainSession> sceneSession = sptr<MainSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    EXPECT_EQ(sceneSession->IsModal(), false);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->AddWindowFlag(WindowFlag::WINDOW_FLAG_IS_MODAL);
    sceneSession->SetSessionProperty(property);
    EXPECT_EQ(sceneSession->IsModal(), true);
}

/**
 * @tc.name: IsApplicationModal
 * @tc.desc: IsApplicationModal function01
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, IsApplicationModal, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "IsApplicationModal";
    info.bundleName_ = "IsApplicationModal";
    sptr<MainSession> sceneSession = sptr<MainSession>::MakeSptr(info, nullptr);
    EXPECT_NE(sceneSession, nullptr);
    EXPECT_EQ(sceneSession->IsApplicationModal(), false);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->AddWindowFlag(WindowFlag::WINDOW_FLAG_IS_MODAL);
    sceneSession->SetSessionProperty(property);
    EXPECT_EQ(sceneSession->IsApplicationModal(), true);
}

/**
 * @tc.name: NotifySupportWindowModesChange
 * @tc.desc: NotifySupportWindowModesChange
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, NotifySupportWindowModesChange, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifySupportWindowModesChange";
    info.bundleName_ = "NotifySupportWindowModesChange";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);

    std::vector<AppExecFwk::SupportWindowMode> supportedWindowModes = {
        AppExecFwk::SupportWindowMode::FULLSCREEN,
        AppExecFwk::SupportWindowMode::SPLIT,
        AppExecFwk::SupportWindowMode::FLOATING
    };

    EXPECT_EQ(WSError::WS_OK, session->NotifySupportWindowModesChange(supportedWindowModes));

    session->onSetSupportedWindowModesFunc_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, session->NotifySupportWindowModesChange(supportedWindowModes));

    session->onSetSupportedWindowModesFunc_ = [](
        std::vector<AppExecFwk::SupportWindowMode>&& supportedWindowModes) {
        return;
    };

    EXPECT_EQ(WSError::WS_OK, session->NotifySupportWindowModesChange(supportedWindowModes));
}

/**
 * @tc.name: NotifySessionLockStateChange
 * @tc.desc: NotifySessionLockStateChange
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, NotifySessionLockStateChange, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.bundleName_ = "NotifySessionLockStateChangeBundle";
    info.moduleName_ = "NotifySessionLockStateChangeModule";
    info.abilityName_ = "NotifySessionLockStateChangeAbility";
    info.appIndex_ = 0;

    sptr<MainSession> session = sptr<MainSession>::MakeSptr(info, nullptr);

    session->NotifySessionLockStateChange(true);
    EXPECT_EQ(session->GetSessionLockState(), true);
}

/**
 * @tc.name: UpdateFlag
 * @tc.desc: UpdateFlag
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, UpdateFlag, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateFlag";
    info.bundleName_ = "UpdateFlag";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);

    session->onUpdateFlagFunc_ = nullptr;
    EXPECT_EQ(nullptr, session->onUpdateFlagFunc_);

    NotifyUpdateFlagFunc func = [](const std::string& flag) {
        return;
    };
    session->onUpdateFlagFunc_ = func;
    EXPECT_NE(nullptr, session->onUpdateFlagFunc_);
}

/**
 * @tc.name: NotifySubAndDialogFollowRectChange
 * @tc.desc: NotifySubAndDialogFollowRectChange
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, NotifySubAndDialogFollowRectChange01, Function | SmallTest | Level2)
{
    SessionInfo info;
    sptr<SceneSession> subSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<MainSession> mainSession = sptr<MainSession>::MakeSptr(info, nullptr);
    
    bool isCall = false;
    auto task = [&isCall](const WSRect& rect, bool isGlobal, bool needFlush) {
        isCall = true;
    };
    mainSession->RegisterNotifySurfaceBoundsChangeFunc(subSession->GetPersistentId(), std::move(task));
    ASSERT_NE(nullptr, mainSession->notifySurfaceBoundsChangeFuncMap_[subSession->GetPersistentId()]);

    WSRect rect;
    subSession->isFollowParentLayout_ = false;
    mainSession->NotifySubAndDialogFollowRectChange(rect, false, false);
    ASSERT_EQ(false, isCall);

    subSession->isFollowParentLayout_ = true;
    sptr<SceneSession::SpecificSessionCallback> callBack = sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    mainSession->specificCallback_ = callBack;
    auto getSessionCallBack = [&subSession](int32_t persistentId) {
        return subSession;
    };
    callBack->onGetSceneSessionByIdCallback_ = getSessionCallBack;
    mainSession->NotifySubAndDialogFollowRectChange(rect, false, false);
    ASSERT_EQ(true, isCall);
}

/**
 * @tc.name: NotifyIsFullScreenInForceSplitMode
 * @tc.desc: NotifyIsFullScreenInForceSplitMode test
 * @tc.type: FUNC
 */
HWTEST_F(MainSessionTest, NotifyIsFullScreenInForceSplitMode, TestSize.Level3)
{
    SessionInfo info;
    info.abilityName_ = "NotifyIsFullScreenInForceSplitMode";
    info.bundleName_ = "NotifyIsFullScreenInForceSplitMode";
    sptr<MainSession> testSession = sptr<MainSession>::MakeSptr(info, nullptr);
    auto ret = testSession->NotifyIsFullScreenInForceSplitMode(true);
    EXPECT_EQ(ret, WSError::WS_OK);
    testSession->RegisterForceSplitFullScreenChangeCallback([](uint32_t uid, bool isFullScreen) {});
    ret = testSession->NotifyIsFullScreenInForceSplitMode(true);
    EXPECT_EQ(ret, WSError::WS_OK);
}
}
}
}