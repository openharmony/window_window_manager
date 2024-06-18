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

#include "common/include/session_permission.h"
#include "display_manager.h"
#include "input_event.h"
#include "key_event.h"

#include "mock/mock_session_stage.h"
#include "pointer_event.h"

#include "screen_manager.h"
#include "session/host/include/sub_session.h"
#include "session/host/include/main_session.h"
#include "session/host/include/scene_session.h"
#include "session/host/include/system_session.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager/include/screen_session_manager_client.h"
#include "wm_common.h"
#include "window_helper.h"
#include "ui/rs_surface_node.h"


using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class GetKeyboardGravitySceneSession : public SceneSession {
public:
    GetKeyboardGravitySceneSession(const SessionInfo& info, const sptr<SpecificSessionCallback>& specificCallback)
        : SceneSession(info, specificCallback)
    {
    }

    SessionGravity GetKeyboardGravity() const override
    {
        return SessionGravity::SESSION_GRAVITY_FLOAT;
    }
};

class SceneSessionTest5 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SceneSessionTest5::SetUpTestCase()
{
}

void SceneSessionTest5::TearDownTestCase()
{
}

void SceneSessionTest5::SetUp()
{
}

void SceneSessionTest5::TearDown()
{
}

namespace {
/**
 * @tc.name: Foreground01
 * @tc.desc: Foreground01 function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, Foreground01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Foreground01";
    info.bundleName_ = "Foreground01";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    sptr<WindowSessionProperty> property = nullptr;
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));

    info.windowType_ = static_cast<uint32_t>(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    sptr<SceneSession> session1 = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_EQ(WSError::WS_OK, session1->Foreground(property, true));
}

/**
 * @tc.name: Foreground02
 * @tc.desc: Foreground02 function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, Foreground02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Foreground02";
    info.bundleName_ = "Foreground02";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    sptr<WindowSessionProperty> property = nullptr;
    session->Session::SetSessionState(SessionState::STATE_CONNECT);
    session->Session::isActive_ = true;
    session->SetLeashWinSurfaceNode(nullptr);
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));

    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    session->specificCallback_ = specificCallback;
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));

    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->SetLeashWinSurfaceNode(surfaceNode);
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));

    session->SetSessionProperty(property);
    EXPECT_EQ(WSError::WS_OK, session->Foreground(property, false));
}

/**
 * @tc.name: BackgroundTask
 * @tc.desc: BackgroundTask function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, BackgroundTask, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "Foreground02";
    info.bundleName_ = "Foreground02";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    sptr<WindowSessionProperty> property = nullptr;
    session->SetSessionState(SessionState::STATE_BACKGROUND);
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(false));

    session->SetSessionState(SessionState::STATE_CONNECT);
    session->Session::SetSessionState(SessionState::STATE_CONNECT);
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(false));

    session->Session::SetSessionState(SessionState::STATE_INACTIVE);
    info.windowType_ = static_cast<uint32_t>(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(false));

    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    session->specificCallback_ = specificCallback;
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(false));

    session->scenePersistence_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(true));

    session->scenePersistence_ = new ScenePersistence("Foreground02", 1);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->surfaceNode_ = surfaceNode;
    EXPECT_EQ(WSError::WS_OK, session->BackgroundTask(true));
}

/**
 * @tc.name: FixKeyboardPositionByKeyboardPanel
 * @tc.desc: FixKeyboardPositionByKeyboardPanel function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, FixKeyboardPositionByKeyboardPanel, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "FixKeyboardPositionByKeyboardPanel";
    info.bundleName_ = "FixKeyboardPositionByKeyboardPanel";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);

    sptr<SceneSession> panelSession = nullptr;
    sptr<SceneSession> keyboardSession = nullptr;
    session->FixKeyboardPositionByKeyboardPanel(panelSession, keyboardSession);
    panelSession = session;
    session->FixKeyboardPositionByKeyboardPanel(panelSession, keyboardSession);
    keyboardSession = sptr<SceneSession>::MakeSptr(info, nullptr);
    session->FixKeyboardPositionByKeyboardPanel(panelSession, keyboardSession);

    keyboardSession = session;
    session->property_ = nullptr;
    session->FixKeyboardPositionByKeyboardPanel(panelSession, keyboardSession);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    session->SetSessionProperty(property);
    session->FixKeyboardPositionByKeyboardPanel(panelSession, keyboardSession);
    EXPECT_EQ(property, session->GetSessionProperty());
}

/**
 * @tc.name: NotifyClientToUpdateRectTask
 * @tc.desc: NotifyClientToUpdateRectTask function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, NotifyClientToUpdateRectTask, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRectTask";
    info.bundleName_ = "NotifyClientToUpdateRectTask";
    info.isSystem_ = true;

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    wptr<SceneSession> weakThis = session;

    session->moveDragController_ = nullptr;
    session->isKeyboardPanelEnabled_ = false;

    session->Session::UpdateSizeChangeReason(SizeChangeReason::UNDEFINED);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask(weakThis, nullptr));
    session->Session::UpdateSizeChangeReason(SizeChangeReason::MOVE);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask(weakThis, nullptr));
    session->Session::UpdateSizeChangeReason(SizeChangeReason::RESIZE);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask(weakThis, nullptr));
    session->Session::UpdateSizeChangeReason(SizeChangeReason::RECOVER);
    EXPECT_EQ(session->reason_, SizeChangeReason::RECOVER);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask(weakThis, nullptr));

    session->moveDragController_ = new MoveDragController(2024);
    session->moveDragController_->isStartDrag_ = true;
    session->moveDragController_->isStartMove_ = true;
    session->Session::UpdateSizeChangeReason(SizeChangeReason::MOVE);
    session->isKeyboardPanelEnabled_ = true;
    info.windowType_ = static_cast<uint32_t>(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask(weakThis, nullptr));
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask(weakThis, nullptr));

    session->Session::UpdateSizeChangeReason(SizeChangeReason::UNDEFINED);
    EXPECT_EQ(WSError::WS_ERROR_REPEAT_OPERATION, session->NotifyClientToUpdateRectTask(weakThis, nullptr));

    session->Session::UpdateSizeChangeReason(SizeChangeReason::MOVE);
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask(weakThis, nullptr));
}

/**
 * @tc.name: GetSystemAvoidArea
 * @tc.desc: GetSystemAvoidArea function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, GetSystemAvoidArea, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetSystemAvoidArea";
    info.bundleName_ = "GetSystemAvoidArea";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    WSRect rect;
    AvoidArea avoidArea;
    session->property_->SetWindowFlags(0);
    session->property_->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    session->isDisplayStatusBarTemporarily_.store(true);
    session->GetSystemAvoidArea(rect, avoidArea);
    EXPECT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, session->GetSessionProperty()->GetWindowMode());

    session->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_END);
    session->isDisplayStatusBarTemporarily_.store(false);
    session->GetSystemAvoidArea(rect, avoidArea);

    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);
    SystemSessionConfig systemConfig;
    systemConfig.uiType_ = "PC";
    session->SetSystemConfig(systemConfig);
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    session->specificCallback_ = specificCallback;
    session->specificCallback_->onGetSceneSessionVectorByType_ = nullptr;
    session->GetSystemAvoidArea(rect, avoidArea);

    systemConfig.uiType_ = "phone";
    GetSceneSessionVectorByTypeCallback func = [&session](WindowType type, uint64_t displayId) {
        std::vector<sptr<SceneSession>> vSession;
        vSession.push_back(session);
        return vSession;
    };
    session->property_->SetDisplayId(2024);
    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr();
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.clear();
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.insert(std::make_pair(2024, screenSession));
    session->specificCallback_->onGetSceneSessionVectorByType_ = func;
    session->GetSystemAvoidArea(rect, avoidArea);
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.clear();
}

/**
 * @tc.name: GetSystemAvoidArea01
 * @tc.desc: GetSystemAvoidArea01 function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, GetSystemAvoidArea01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetSystemAvoidArea01";
    info.bundleName_ = "GetSystemAvoidArea01";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    WSRect rect;
    AvoidArea avoidArea;
    session->property_->SetWindowFlags(0);
  
    session->isDisplayStatusBarTemporarily_.store(false);

    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);
    SystemSessionConfig systemConfig;
 
    session->SetSystemConfig(systemConfig);
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    session->specificCallback_ = specificCallback;

    systemConfig.uiType_ = "phone";
    GetSceneSessionVectorByTypeCallback func = [&session](WindowType type, uint64_t displayId) {
        std::vector<sptr<SceneSession>> vSession;
        vSession.push_back(session);
        return vSession;
    };
    session->property_->SetDisplayId(2024);
    sptr<ScreenSession> screenSession = sptr<ScreenSession>::MakeSptr();
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.clear();
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.insert(std::make_pair(2024, screenSession));
    session->specificCallback_->onGetSceneSessionVectorByType_ = func;
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.clear();

    session->property_->SetDisplayId(1024);
    session->Session::SetFloatingScale(0.0f);

    session->Session::SetFloatingScale(0.5f);
    EXPECT_EQ(0.5f, session->Session::GetFloatingScale());
    session->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    session->GetSystemAvoidArea(rect, avoidArea);
    session->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    session->GetSystemAvoidArea(rect, avoidArea);
    rect.height_ = 3;
    rect.width_ =4;
    session->GetSystemAvoidArea(rect, avoidArea);

    session->isVisible_ = true;
    session->property_->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    session->GetSystemAvoidArea(rect, avoidArea);
}

/**
 * @tc.name: HandlePointerStyle
 * @tc.desc: HandlePointerStyle function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, HandlePointerStyle, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandlePointerStyle";
    info.bundleName_ = "HandlePointerStyle";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    EXPECT_EQ(WSError::WS_ERROR_NULLPTR, session->HandlePointerStyle(nullptr));
    pointerEvent->SetSourceType(20);
    EXPECT_EQ(WSError::WS_DO_NOTHING, session->HandlePointerStyle(pointerEvent));
    pointerEvent->SetSourceType(1);
    pointerEvent->SetPointerAction(-3);
    EXPECT_EQ(WSError::WS_DO_NOTHING, session->HandlePointerStyle(pointerEvent));
    pointerEvent->SetPointerAction(3);
    pointerEvent->SetButtonId(1);
    EXPECT_EQ(WSError::WS_DO_NOTHING, session->HandlePointerStyle(pointerEvent));
    pointerEvent->SetButtonId(-1);
    pointerEvent->RemoveAllPointerItems();
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, session->HandlePointerStyle(pointerEvent));
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(2024);
    pointerEvent->AddPointerItem(pointerItem);
    pointerEvent->SetPointerId(2024);
    EXPECT_EQ(WSError::WS_OK, session->HandlePointerStyle(pointerEvent));
    pointerEvent->RemoveAllPointerItems();
}

/**
 * @tc.name: NotifyOutsideDownEvent
 * @tc.desc: NotifyOutsideDownEvent function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, NotifyOutsideDownEvent, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyOutsideDownEvent";
    info.bundleName_ = "NotifyOutsideDownEvent";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->SetPointerAction(2);
    pointerEvent->RemoveAllPointerItems();
    session->NotifyOutsideDownEvent(pointerEvent);
    pointerEvent->SetPointerAction(8);
    MMI::PointerEvent::PointerItem pointerItem;
    pointerItem.SetPointerId(2024);
    pointerEvent->AddPointerItem(pointerItem);
    pointerEvent->SetPointerId(2024);
    session->NotifyOutsideDownEvent(pointerEvent);
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    session->specificCallback_ = specificCallback;
    session->specificCallback_->onOutsideDownEvent_ = nullptr;
    session->NotifyOutsideDownEvent(pointerEvent);
    OnOutsideDownEvent func = [](int32_t x, int32_t y) {
        return;
    };
    session->specificCallback_->onOutsideDownEvent_ = func;
    session->NotifyOutsideDownEvent(pointerEvent);
    auto res = pointerEvent->GetPointerItem(2024, pointerItem);
    EXPECT_EQ(true, res);

    pointerEvent->SetPointerAction(5);
    session->NotifyOutsideDownEvent(pointerEvent);
    pointerEvent->RemoveAllPointerItems();
}

/**
 * @tc.name: TransferPointerEvent
 * @tc.desc: TransferPointerEvent function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, TransferPointerEvent, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "TransferPointerEvent";
    info.bundleName_ = "TransferPointerEvent";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();;

    SceneSession::enterSession_ = wptr<SceneSession>(session);
    info.isSystem_ = false;
    session->property_ = nullptr;
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->TransferPointerEvent(pointerEvent, false));

    info.isSystem_ = false;
    pointerEvent->SetPointerAction(9);

    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    session->specificCallback_ = specificCallback;
    session->specificCallback_->onSessionTouchOutside_ = nullptr;
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->TransferPointerEvent(pointerEvent, false));

    NotifySessionTouchOutsideCallback func = [](int32_t persistentId) {
        return;
    };
    session->specificCallback_->onSessionTouchOutside_ = func;
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->TransferPointerEvent(pointerEvent, false));
    pointerEvent->SetPointerAction(2);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->TransferPointerEvent(pointerEvent, false));
}

/**
 * @tc.name: TransferPointerEvent01
 * @tc.desc: TransferPointerEvent01 function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, TransferPointerEvent01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "TransferPointerEvent01";
    info.bundleName_ = "TransferPointerEvent01";
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();;
    session->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    session->property_->SetMaximizeMode(MaximizeMode::MODE_RECOVER);
    session->ClearDialogVector();
    session->moveDragController_ = new MoveDragController(2024);
    SystemSessionConfig systemConfig;
    systemConfig.isSystemDecorEnable_ = false;
    systemConfig.decorModeSupportInfo_ = 2;
    session->SetSystemConfig(systemConfig);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->TransferPointerEvent(pointerEvent, false));

    session->BindDialogToParentSession(session);
    session->SetSessionState(SessionState::STATE_ACTIVE);
    pointerEvent->SetPointerAction(5);
    session->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    session->property_->SetDragEnabled(false);
    systemConfig.isSystemDecorEnable_ = true;

    session->moveDragController_->isStartDrag_ = false;
    EXPECT_EQ(WSError::WS_ERROR_NULLPTR, session->TransferPointerEvent(pointerEvent, false));

    pointerEvent->SetPointerAction(2);
    EXPECT_EQ(WSError::WS_OK, session->TransferPointerEvent(pointerEvent, false));

    pointerEvent->SetPointerAction(5);
    session->property_->SetDragEnabled(true);
    systemConfig.uiType_ = "phone";
    systemConfig.freeMultiWindowSupport_ = false;
    session->moveDragController_->isStartDrag_ = true;
    EXPECT_EQ(WSError::WS_ERROR_NULLPTR, session->TransferPointerEvent(pointerEvent, false));

    systemConfig.uiType_ = "pc";
    EXPECT_EQ(WSError::WS_ERROR_NULLPTR, session->TransferPointerEvent(pointerEvent, false));
    session->ClearDialogVector();
}

/**
 * @tc.name: OnLayoutFullScreenChange
 * @tc.desc: OnLayoutFullScreenChange function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, OnLayoutFullScreenChange, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnLayoutFullScreenChange";
    info.bundleName_ = "OnLayoutFullScreenChange";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    EXPECT_EQ(WSError::WS_OK, session->OnLayoutFullScreenChange(true));

    sptr<SceneSession::SessionChangeCallback> sessionChangeCallback =
        sptr<SceneSession::SessionChangeCallback>::MakeSptr();
    session->RegisterSessionChangeCallback(sessionChangeCallback);
    sessionChangeCallback->onLayoutFullScreenChangeFunc_ = nullptr;
    EXPECT_EQ(WSError::WS_OK, session->OnLayoutFullScreenChange(true));

    NotifyLayoutFullScreenChangeFunc func = [](bool isLayoutFullScreen) {
        return;
    };
    sessionChangeCallback->onLayoutFullScreenChangeFunc_ = func;
    EXPECT_EQ(WSError::WS_OK, session->OnLayoutFullScreenChange(true));
}

/**
 * @tc.name: UpdateSessionPropertyByAction
 * @tc.desc: UpdateSessionPropertyByAction function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, UpdateSessionPropertyByAction, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateSessionPropertyByAction";
    info.bundleName_ = "UpdateSessionPropertyByAction";
    info.abilityInfo = std::make_shared<AppExecFwk::AbilityInfo>();
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, session->UpdateSessionPropertyByAction
        (nullptr, WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, session->UpdateSessionPropertyByAction
        (property, WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE));
    
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PERMISSION, session->UpdateSessionPropertyByAction
        (property, WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE));
}
}
}
}