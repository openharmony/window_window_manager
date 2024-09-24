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
#include "screen_session_manager_client/include/screen_session_manager_client.h"
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

    session->moveDragController_ = nullptr;
    session->isKeyboardPanelEnabled_ = false;

    session->Session::UpdateSizeChangeReason(SizeChangeReason::UNDEFINED);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask("SceneSessionTest5", nullptr));
    session->Session::UpdateSizeChangeReason(SizeChangeReason::MOVE);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask("SceneSessionTest5", nullptr));
    session->Session::UpdateSizeChangeReason(SizeChangeReason::RESIZE);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask("SceneSessionTest5", nullptr));
    session->Session::UpdateSizeChangeReason(SizeChangeReason::RECOVER);
    EXPECT_EQ(session->reason_, SizeChangeReason::RECOVER);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask("SceneSessionTest5", nullptr));

    session->moveDragController_ = sptr<MoveDragController>::MakeSptr(2024);
    session->moveDragController_->isStartDrag_ = true;
    session->moveDragController_->isStartMove_ = true;
    session->Session::UpdateSizeChangeReason(SizeChangeReason::MOVE);
    session->isKeyboardPanelEnabled_ = true;
    info.windowType_ = static_cast<uint32_t>(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask("SceneSessionTest5", nullptr));
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask("SceneSessionTest5", nullptr));

    session->Session::UpdateSizeChangeReason(SizeChangeReason::UNDEFINED);
    EXPECT_EQ(WSError::WS_ERROR_REPEAT_OPERATION, session->NotifyClientToUpdateRectTask("SceneSessionTest5", nullptr));

    session->Session::UpdateSizeChangeReason(SizeChangeReason::MOVE);
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_KEYBOARD_PANEL);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_SESSION, session->NotifyClientToUpdateRectTask("SceneSessionTest5", nullptr));
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
    ASSERT_NE(session->GetSessionProperty(), nullptr);
    EXPECT_EQ(WindowMode::WINDOW_MODE_UNDEFINED, session->GetSessionProperty()->GetWindowMode());

    session->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_END);
    session->isDisplayStatusBarTemporarily_.store(false);
    session->GetSystemAvoidArea(rect, avoidArea);

    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);
    SystemSessionConfig systemConfig;
    systemConfig.windowUIType_ = WindowUIType::PC_WINDOW;
    session->SetSystemConfig(systemConfig);
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    session->specificCallback_ = specificCallback;
    session->specificCallback_->onGetSceneSessionVectorByType_ = nullptr;
    session->GetSystemAvoidArea(rect, avoidArea);

    systemConfig.windowUIType_ = WindowUIType::PHONE_WINDOW;
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

    systemConfig.windowUIType_ = WindowUIType::PHONE_WINDOW;
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
    session->moveDragController_ = sptr<MoveDragController>::MakeSptr(2024);
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
    systemConfig.windowUIType_ = WindowUIType::PHONE_WINDOW;
    systemConfig.freeMultiWindowSupport_ = false;
    session->moveDragController_->isStartDrag_ = true;
    EXPECT_EQ(WSError::WS_ERROR_NULLPTR, session->TransferPointerEvent(pointerEvent, false));

    systemConfig.windowUIType_ = WindowUIType::PC_WINDOW;
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

/**
 * @tc.name: SetSessionRectChangeCallback
 * @tc.desc: SetSessionRectChangeCallback function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, SetSessionRectChangeCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSessionRectChangeCallback";
    info.bundleName_ = "SetSessionRectChangeCallback";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    WSRect rec = { 1, 1, 1, 1 };
    NotifySessionRectChangeFunc func = [](const WSRect& rect, const SizeChangeReason reason, DisplayId newDisplayId) {
        return;
    };
    session->SetSessionRectChangeCallback(nullptr);
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    session->SetSessionRectChangeCallback(func);
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);
    session->SetSessionRectChangeCallback(func);
    rec.width_ = 0;
    session->SetSessionRectChangeCallback(func);
    rec.height_ = 0;
    session->SetSessionRectChangeCallback(func);
    EXPECT_EQ(WindowType::APP_MAIN_WINDOW_BASE, session->GetWindowType());
}

/**
 * @tc.name: SetSessionRectChangeCallback02
 * @tc.desc: SetSessionRectChangeCallback02 function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, SetSessionRectChangeCallback02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSessionRectChangeCallback02";
    info.bundleName_ = "SetSessionRectChangeCallback02";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    WSRect rec = { 1, 1, 1, 1 };
    NotifySessionRectChangeFunc func = [](const WSRect& rect, const SizeChangeReason reason, DisplayId displayId) {
        return;
    };
    session->SetSessionRectChangeCallback(nullptr);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    session->SetSessionProperty(property);
    session->SetSessionRectChangeCallback(func);

    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    session->SetSessionProperty(property);
    session->SetSessionRequestRect(rec);
    session->SetSessionRectChangeCallback(func);

    rec.width_ = 0;
    session->SetSessionRequestRect(rec);
    session->SetSessionRectChangeCallback(func);

    rec.height_ = 0;
    session->SetSessionRequestRect(rec);
    session->SetSessionRectChangeCallback(func);
    EXPECT_EQ(WindowType::APP_MAIN_WINDOW_BASE, session->GetWindowType());
}

/**
 * @tc.name: NotifyClientToUpdateRect
 * @tc.desc: NotifyClientToUpdateRect function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, NotifyClientToUpdateRect, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "NotifyClientToUpdateRect";
    info.bundleName_ = "NotifyClientToUpdateRect";
    info.isSystem_ = false;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->moveDragController_ = nullptr;
    session->isKeyboardPanelEnabled_ = false;
    session->reason_ = SizeChangeReason::UNDEFINED;
    session->Session::SetSessionState(SessionState::STATE_CONNECT);
    session->specificCallback_ = nullptr;
    session->reason_ = SizeChangeReason::DRAG;
    EXPECT_EQ(WSError::WS_OK, session->NotifyClientToUpdateRect("SceneSessionTest5", nullptr));

    UpdateAvoidAreaCallback func = [](const int32_t& persistentId) {
        return;
    };
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    specificCallback->onUpdateAvoidArea_ = func;
    session->specificCallback_ = specificCallback;
    session->reason_ = SizeChangeReason::RECOVER;
    EXPECT_EQ(WSError::WS_OK, session->NotifyClientToUpdateRect("SceneSessionTest5", nullptr));
}

/**
 * @tc.name: CheckAspectRatioValid
 * @tc.desc: CheckAspectRatioValid function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, CheckAspectRatioValid, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "CheckAspectRatioValid";
    info.bundleName_ = "CheckAspectRatioValid";
    info.isSystem_ = false;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    WindowLimits windowLimits;
    ASSERT_NE(session->GetSessionProperty(), nullptr);
    session->GetSessionProperty()->SetWindowLimits(windowLimits);

    SystemSessionConfig systemConfig;
    systemConfig.isSystemDecorEnable_ = false;
    session->SetSystemConfig(systemConfig);
    EXPECT_EQ(false, session->IsDecorEnable());

    windowLimits.minWidth_ = 0;
    windowLimits.minHeight_ = 0;
    EXPECT_EQ(WSError::WS_OK, session->SetAspectRatio(0.0f));

    windowLimits.minWidth_ = 1;
    windowLimits.maxHeight_ = 0;
    windowLimits.minHeight_ = 1;
    windowLimits.maxWidth_ = 0;
    EXPECT_EQ(WSError::WS_OK, session->SetAspectRatio(0.0f));

    windowLimits.maxHeight_ = 1;
    windowLimits.maxWidth_ = 1;
    EXPECT_EQ(WSError::WS_OK, session->SetAspectRatio(1.0f));

    windowLimits.maxHeight_ = 10000;
    windowLimits.minHeight_ = -10000;
    EXPECT_EQ(WSError::WS_OK, session->SetAspectRatio(0.0f));

    windowLimits.maxHeight_ = 10000;
    windowLimits.minHeight_ = -10000;
    EXPECT_EQ(WSError::WS_OK, session->SetAspectRatio(0.0f));

    session->SetSessionProperty(nullptr);
    EXPECT_EQ(WSError::WS_ERROR_NULLPTR, session->SetAspectRatio(0.0f));

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    WindowLimits limits = {8, 1, 6, 1, 1, 1.0f, 1.0f};
    property->SetWindowLimits(limits);
    session->SetSessionProperty(property);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, session->SetAspectRatio(0.1f));
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, session->SetAspectRatio(10.0f));
}

/**
 * @tc.name: GetSystemAvoidArea02
 * @tc.desc: GetSystemAvoidArea02 function
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, GetSystemAvoidArea02, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "GetSystemAvoidArea02";
    info.bundleName_ = "GetSystemAvoidArea02";

    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    ASSERT_NE(session->GetSessionProperty(), nullptr);
    session->GetSessionProperty()->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);

    SystemSessionConfig systemConfig;
    systemConfig.windowUIType_ = WindowUIType::PHONE_WINDOW;
    session->SetSystemConfig(systemConfig);
    ScreenSessionManagerClient::GetInstance().screenSessionMap_.clear();
    session->GetSessionProperty()->SetDisplayId(1664);
    session->Session::SetFloatingScale(0.02f);
    WSRect rect;
    AvoidArea avoidArea;
    session->GetSystemAvoidArea(rect, avoidArea);
    EXPECT_EQ(0.02f, session->Session::GetFloatingScale());

    session->Session::SetFloatingScale(0.5f);
    session->property_->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    session->GetSystemAvoidArea(rect, avoidArea);
    session->GetSessionProperty()->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    rect.height_ = 2;
    rect.width_ =1 ;
    session->GetSystemAvoidArea(rect, avoidArea);
    rect.height_ = 1;
    session->GetSystemAvoidArea(rect, avoidArea);
}

/**
 * @tc.name: FixRectByAspectRatio
 * @tc.desc: FixRectByAspectRatio function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, FixRectByAspectRatio, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "FixRectByAspectRatio";
    info.bundleName_ = "FixRectByAspectRatio";
    info.isSystem_ = false;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    session->SetSessionProperty(nullptr);
    WSRect rect;
    EXPECT_EQ(false, session->FixRectByAspectRatio(rect));
    session->SetSessionProperty(property);
    property->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    EXPECT_EQ(false, session->FixRectByAspectRatio(rect));
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    EXPECT_EQ(false, session->FixRectByAspectRatio(rect));
    property->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    EXPECT_EQ(true, session->FixRectByAspectRatio(rect));
}

/**
 * @tc.name: FixRectByAspectRatio01
 * @tc.desc: FixRectByAspectRatio function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, FixRectByAspectRatio01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "FixRectByAspectRatio01";
    info.bundleName_ = "FixRectByAspectRatio01";
    info.isSystem_ = false;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    WSRect rect;
    session->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);
    session->aspectRatio_ = 0.5f;
    EXPECT_NE(nullptr, DisplayManager::GetInstance().GetDefaultDisplay());

    SystemSessionConfig systemConfig;
    systemConfig.isSystemDecorEnable_ = true;
    systemConfig.decorModeSupportInfo_ = 2;
    session->SetSystemConfig(systemConfig);
    EXPECT_EQ(true, session->FixRectByAspectRatio(rect));

    systemConfig.isSystemDecorEnable_ = false;
    EXPECT_EQ(false, session->FixRectByAspectRatio(rect));

    systemConfig.isSystemDecorEnable_ = true;
    session->SetSessionProperty(nullptr);
    EXPECT_EQ(false, session->FixRectByAspectRatio(rect));
}

/**
 * @tc.name: OnMoveDragCallback
 * @tc.desc: OnMoveDragCallback function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, OnMoveDragCallback, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "OnMoveDragCallback";
    info.bundleName_ = "OnMoveDragCallback";
    info.isSystem_ = false;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->moveDragController_ = nullptr;
    SizeChangeReason reason = { SizeChangeReason::DRAG };
    session->OnMoveDragCallback(reason);
    session->moveDragController_ = sptr<MoveDragController>::MakeSptr(2024);
    session->OnMoveDragCallback(reason);

    reason = SizeChangeReason::DRAG_END;
    session->OnMoveDragCallback(reason);

    reason = SizeChangeReason::MOVE;
    session->OnMoveDragCallback(reason);

    reason = SizeChangeReason::DRAG_START;
    session->OnMoveDragCallback(reason);
    EXPECT_EQ(WSError::WS_OK, session->UpdateSizeChangeReason(reason));

    session->moveDragController_ = sptr<MoveDragController>::MakeSptr(2024);
    EXPECT_NE(session->moveDragController_, nullptr);
    session->SetSessionProperty(nullptr);
    session->OnMoveDragCallback(reason);
    EXPECT_EQ(WSError::WS_OK, session->UpdateSizeChangeReason(reason));

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    ASSERT_NE(nullptr, property);
    session->SetSessionProperty(property);
    property->compatibleModeInPc_ = true;
    session->OnMoveDragCallback(reason);
    EXPECT_EQ(WSError::WS_OK, session->UpdateSizeChangeReason(reason));
}

/**
 * @tc.name: UpdateWinRectForSystemBar
 * @tc.desc: UpdateWinRectForSystemBar function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, UpdateWinRectForSystemBar, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateWinRectForSystemBar";
    info.bundleName_ = "UpdateWinRectForSystemBar";
    info.isSystem_ = false;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    specificCallback->onGetSceneSessionVectorByType_ = nullptr;
    session->specificCallback_ = specificCallback;
    WSRect rect = { 1, 10, 3, 4 };
    session->UpdateWinRectForSystemBar(rect);
    GetSceneSessionVectorByTypeCallback func = [session](WindowType type, uint64_t displayId)->
        std::vector<sptr<SceneSession>>
    {
        std::vector<sptr<SceneSession>> vSession;
        vSession.push_back(session);
        return vSession;
    };
    specificCallback->onGetSceneSessionVectorByType_ = func;
    session->UpdateWinRectForSystemBar(rect);

    session->isVisible_ = true;
    session->winRect_ = rect;
    session->UpdateWinRectForSystemBar(rect);

    WSRect rect1 = { 1, 2, 10, 4 };
    session->winRect_ = rect1;
    session->UpdateWinRectForSystemBar(rect);
    EXPECT_EQ(4, session->GetSessionRect().height_);

    WSRect rect2 = { 1, 2, 10, 8 };
    session->winRect_ = rect2;
    session->UpdateWinRectForSystemBar(rect);

    session->property_ = nullptr;
    session->UpdateWinRectForSystemBar(rect);
}

/**
 * @tc.name: UpdateNativeVisibility
 * @tc.desc: UpdateNativeVisibility function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, UpdateNativeVisibility, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateNativeVisibility";
    info.bundleName_ = "UpdateNativeVisibility";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    sptr<SceneSession::SpecificSessionCallback> specificCallback =
        sptr<SceneSession::SpecificSessionCallback>::MakeSptr();
    NotifyWindowInfoUpdateCallback dateFunc = [](int32_t persistentId, WindowUpdateType type) {
        return;
    };
    UpdateAvoidAreaCallback areaFunc = [](const int32_t persistentId) {
        return;
    };
    specificCallback->onWindowInfoUpdate_ = dateFunc;
    specificCallback->onUpdateAvoidArea_ = areaFunc;
    session->specificCallback_ = specificCallback;
    session->UpdateNativeVisibility(true);
    session->SetSessionProperty(nullptr);
    session->UpdateNativeVisibility(false);
    EXPECT_EQ(nullptr, session->property_);
}

/**
 * @tc.name: SetPrivacyMode
 * @tc.desc: SetPrivacyMode function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, SetPrivacyMode, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetPrivacyMode";
    info.bundleName_ = "SetPrivacyMode";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->surfaceNode_ = surfaceNode;
    ASSERT_NE(session->GetSessionProperty(), nullptr);
    session->GetSessionProperty()->SetPrivacyMode(true);
    EXPECT_EQ(true, session->GetSessionProperty()->GetPrivacyMode());
    session->leashWinSurfaceNode_ = nullptr;
    session->SetPrivacyMode(false);
    session->leashWinSurfaceNode_ = surfaceNode;
    session->SetPrivacyMode(true);
    session->SetSessionProperty(nullptr);
    session->SetPrivacyMode(true);

    sptr<WindowSessionProperty> property = new (std::nothrow) WindowSessionProperty();
    EXPECT_NE(property, nullptr);
    property->SetPrivacyMode(true);
    session->SetSessionProperty(property);
    session->SetPrivacyMode(true);
}

/**
 * @tc.name: SetSnapshotSkip
 * @tc.desc: SetSnapshotSkip function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, SetSnapshotSkip, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSnapshotSkip";
    info.bundleName_ = "SetSnapshotSkip";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->surfaceNode_ = surfaceNode;
    ASSERT_NE(session->GetSessionProperty(), nullptr);
    session->GetSessionProperty()->SetSnapshotSkip(true);
    EXPECT_EQ(true, session->GetSessionProperty()->GetSnapshotSkip());
    session->leashWinSurfaceNode_ = nullptr;
    session->SetSnapshotSkip(false);
    session->leashWinSurfaceNode_ = surfaceNode;
    session->SetSnapshotSkip(false);
    session->SetSnapshotSkip(true);
    session->surfaceNode_ = nullptr;
    EXPECT_NE(nullptr, session->GetLeashWinSurfaceNode());
    session->SetSnapshotSkip(true);
    session->SetSessionProperty(nullptr);
    session->SetSnapshotSkip(true);
    EXPECT_EQ(nullptr, session->GetSessionProperty());
}

/**
 * @tc.name: SetWatermarkEnabled
 * @tc.desc: SetWatermarkEnabled function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, SetWatermarkEnabled, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetWatermarkEnabled";
    info.bundleName_ = "SetWatermarkEnabled";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);

    std::string watermarkName = "watermarkNameTest";
    session->SetWatermarkEnabled(watermarkName, true);

    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->surfaceNode_ = surfaceNode;
    session->SetWatermarkEnabled(watermarkName, true);
}

/**
 * @tc.name: UIExtSurfaceNodeIdCache
 * @tc.desc: UIExtSurfaceNodeIdCache
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, UIExtSurfaceNodeIdCache, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UIExtSurfaceNodeIdCache";
    info.bundleName_ = "UIExtSurfaceNodeIdCache";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);

    session->AddUIExtSurfaceNodeId(1, 2);
    EXPECT_EQ(session->GetUIExtPersistentIdBySurfaceNodeId(1), 2);

    session->RemoveUIExtSurfaceNodeId(2);
    EXPECT_EQ(session->GetUIExtPersistentIdBySurfaceNodeId(1), 0);
}

/**
 * @tc.name: SetSystemSceneOcclusionAlpha
 * @tc.desc: SetSystemSceneOcclusionAlpha function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, SetSystemSceneOcclusionAlpha, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSystemSceneOcclusionAlpha";
    info.bundleName_ = "SetSystemSceneOcclusionAlpha";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->SetSystemSceneOcclusionAlpha(-0.325);
    session->SetSystemSceneOcclusionAlpha(3.14125);

    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->surfaceNode_ = surfaceNode;
    session->leashWinSurfaceNode_ = nullptr;
    session->SetSystemSceneOcclusionAlpha(0.14125);
    session->leashWinSurfaceNode_ = surfaceNode;
    session->SetSystemSceneOcclusionAlpha(0.14125);
    EXPECT_NE(nullptr, session->GetLeashWinSurfaceNode());
}

/**
 * @tc.name: SetSystemSceneForceUIFirst
 * @tc.desc: SetSystemSceneForceUIFirst function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, SetSystemSceneForceUIFirst, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSystemSceneForceUIFirst";
    info.bundleName_ = "SetSystemSceneForceUIFirst";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->SetSystemSceneForceUIFirst(true);

    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->surfaceNode_ = surfaceNode;
    session->leashWinSurfaceNode_ = nullptr;
    session->SetSystemSceneForceUIFirst(true);
    session->leashWinSurfaceNode_ = surfaceNode;
    session->SetSystemSceneForceUIFirst(true);
    EXPECT_NE(nullptr, session->GetLeashWinSurfaceNode());
}

/**
 * @tc.name: UpdateWindowAnimationFlag
 * @tc.desc: UpdateWindowAnimationFlag function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, UpdateWindowAnimationFlag, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateWindowAnimationFlag";
    info.bundleName_ = "UpdateWindowAnimationFlag";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);

    sptr<SceneSession::SessionChangeCallback> sessionChangeCallback =
        sptr<SceneSession::SessionChangeCallback>::MakeSptr();
    session->RegisterSessionChangeCallback(nullptr);
    EXPECT_EQ(WSError::WS_OK, session->UpdateWindowAnimationFlag(true));

    sessionChangeCallback->onWindowAnimationFlagChange_ = nullptr;
    session->RegisterSessionChangeCallback(sessionChangeCallback);
    EXPECT_EQ(WSError::WS_OK, session->UpdateWindowAnimationFlag(true));

    NotifyWindowAnimationFlagChangeFunc func = [](const bool flag) {
        return;
    };
    sessionChangeCallback->onWindowAnimationFlagChange_ = func;
    EXPECT_EQ(WSError::WS_OK, session->UpdateWindowAnimationFlag(true));
}

/**
 * @tc.name: SetForegroundInteractiveStatus
 * @tc.desc: SetForegroundInteractiveStatus function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, SetForegroundInteractiveStatus, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetForegroundInteractiveStatus";
    info.bundleName_ = "SetForegroundInteractiveStatus";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    session->SetSessionState(SessionState::STATE_FOREGROUND);
    EXPECT_NE(session, nullptr);
    session->toastSession_.clear();
    session->toastSession_.push_back(session);
    session->SetForegroundInteractiveStatus(false);
    session->toastSession_.clear();
    session->SetSessionState(SessionState::STATE_ACTIVE);
    session->toastSession_.push_back(session);
    session->SetForegroundInteractiveStatus(false);
    session->toastSession_.clear();
    session->SetSessionState(SessionState::STATE_CONNECT);
    session->toastSession_.push_back(session);
    session->SetForegroundInteractiveStatus(false);
    session->toastSession_.clear();
    session->SetSessionState(SessionState::STATE_ACTIVE);
    session->toastSession_.push_back(nullptr);
    session->SetForegroundInteractiveStatus(false);
    session->toastSession_.clear();
}

/**
 * @tc.name: HandleUpdatePropertyByAction
 * @tc.desc: HandleUpdatePropertyByAction function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, HandleUpdatePropertyByAction, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleUpdatePropertyByAction";
    info.bundleName_ = "HandleUpdatePropertyByAction";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, session);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_RECT;
    auto res = session->HandleUpdatePropertyByAction(nullptr, action);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, res);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    res = session->HandleUpdatePropertyByAction(property, action);
    EXPECT_EQ(WMError::WM_DO_NOTHING, res);
    action = WSPropertyChangeAction::ACTION_UPDATE_FLAGS;
    res = session->HandleUpdatePropertyByAction(property, action);
    EXPECT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: SetSystemWindowEnableDrag
 * @tc.desc: SetSystemWindowEnableDrag function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, SetSystemWindowEnableDrag, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetSystemWindowEnableDrag";
    info.bundleName_ = "SetSystemWindowEnableDrag";
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_DESKTOP);
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    auto ret = session->SetSystemWindowEnableDrag(true);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: HandleActionUpdateSetBrightness
 * @tc.desc: HandleActionUpdateSetBrightness function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, HandleActionUpdateSetBrightness, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleActionUpdateSetBrightness";
    info.bundleName_ = "HandleActionUpdateSetBrightness";
    info.windowType_ = static_cast<uint32_t>(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    info.isSystem_ = true;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(nullptr, session);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(nullptr, property);
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_RECT;
    auto res = session->HandleActionUpdateSetBrightness(property, action);
    EXPECT_EQ(WMError::WM_OK, res);

    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<SceneSession> session1 = sptr<SceneSession>::MakeSptr(info, nullptr);
    res = session1->HandleActionUpdateSetBrightness(property, action);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_SESSION, res);

    info.isSystem_ = false;
    sptr<SceneSession> session2 = sptr<SceneSession>::MakeSptr(info, nullptr);
    session2->SetSessionState(SessionState::STATE_CONNECT);
    res = session2->HandleActionUpdateSetBrightness(property, action);
    EXPECT_EQ(WMError::WM_OK, res);

    sptr<SceneSession> session3 = sptr<SceneSession>::MakeSptr(info, nullptr);
    session3->SetSessionState(SessionState::STATE_CONNECT);
    float brightness = 1.0;
    property->SetBrightness(brightness);
    EXPECT_EQ(brightness, property->GetBrightness());
    EXPECT_EQ(WSError::WS_OK, session3->SetBrightness(brightness));
    res = session3->HandleActionUpdateSetBrightness(property, action);
    EXPECT_EQ(WMError::WM_OK, res);
    EXPECT_EQ(brightness, session3->GetBrightness());

    sptr<SceneSession> session4 = sptr<SceneSession>::MakeSptr(info, nullptr);
    session4->SetSessionState(SessionState::STATE_CONNECT);
    brightness = 0.8;
    property->SetBrightness(brightness);
    EXPECT_EQ(brightness, property->GetBrightness());
    res = session4->HandleActionUpdateSetBrightness(property, action);
    EXPECT_EQ(WMError::WM_OK, res);
    EXPECT_EQ(brightness, session4->GetBrightness());
    brightness = 1.0;
    EXPECT_EQ(WSError::WS_OK, session4->SetBrightness(brightness));
    EXPECT_EQ(brightness, session4->GetBrightness());
}

/**
 * @tc.name: HandleActionUpdateMaximizeState
 * @tc.desc: HandleActionUpdateMaximizeState function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, HandleActionUpdateMaximizeState, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleActionUpdateMaximizeState";
    info.bundleName_ = "HandleActionUpdateMaximizeState";
    info.windowType_ = static_cast<uint32_t>(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_RECT;

    auto res = session->HandleActionUpdateMaximizeState(property, action);
    EXPECT_EQ(WMError::WM_OK, res);
    res = session->HandleActionUpdateMode(property, action);
    EXPECT_EQ(WMError::WM_OK, res);
    res = session->HandleActionUpdateAnimationFlag(property, action);
    EXPECT_EQ(WMError::WM_OK, res);

    session->SetSessionProperty(nullptr);
    res = session->HandleActionUpdateMaximizeState(property, action);
    EXPECT_EQ(WMError::WM_OK, res);
    res = session->HandleActionUpdateMode(property, action);
    EXPECT_EQ(WMError::WM_OK, res);
    res = session->HandleActionUpdateAnimationFlag(property, action);
    EXPECT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: SetUniqueDensityDpi
 * @tc.desc: SetUniqueDensityDpi function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, SetUniqueDensityDpi, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "SetUniqueDensityDpi";
    info.bundleName_ = "SetUniqueDensityDpi";
    info.windowType_ = static_cast<uint32_t>(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    info.isSystem_ = true;
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    session->sessionStage_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_INVALID_SESSION, session->SetUniqueDensityDpi(true, 520));
    session->sessionInfo_.isSystem_ = false;
    session->state_ = SessionState::STATE_DISCONNECT;
    EXPECT_EQ(WMError::WM_ERROR_INVALID_SESSION, session->SetUniqueDensityDpi(true, 520));
    session->state_ = SessionState::STATE_CONNECT;
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, session->SetUniqueDensityDpi(true, 520));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, session->SetUniqueDensityDpi(true, 79));
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, session->SetUniqueDensityDpi(false, 79));

    session->sessionStage_ = new SessionStageMocker();
    EXPECT_NE(nullptr, session->sessionStage_);
}

/**
 * @tc.name: HandleActionUpdateModeSupportInfo
 * @tc.desc: HandleActionUpdateModeSupportInfo function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, HandleActionUpdateModeSupportInfo, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleActionUpdateModeSupportInfo";
    info.bundleName_ = "HandleActionUpdateModeSupportInfo";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(session, nullptr);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->isSystemCalling_ = true;
    ASSERT_NE(session, nullptr);
    session->SetSessionProperty(nullptr);
    ASSERT_EQ(WMError::WM_OK, session->HandleActionUpdateModeSupportInfo(property,
        WSPropertyChangeAction::ACTION_UPDATE_RECT));

    property->isSystemCalling_ = false;
    session->SetSessionProperty(property);
    ASSERT_EQ(WMError::WM_ERROR_NOT_SYSTEM_APP, session->HandleActionUpdateModeSupportInfo(property,
        WSPropertyChangeAction::ACTION_UPDATE_RECT));
}

/**
 * @tc.name: UpdateUIParam
 * @tc.desc: UpdateUIParam function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, UpdateUIParam, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateUIParam";
    info.bundleName_ = "UpdateUIParam";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(session, nullptr);
    session->isFocused_ = true;
    session->isVisible_ = true;
    uint32_t res = session->UpdateUIParam();
    ASSERT_EQ(1, res);
}

/**
 * @tc.name: UpdateVisibilityInner
 * @tc.desc: UpdateVisibilityInner function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, UpdateVisibilityInner, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateVisibilityInner";
    info.bundleName_ = "UpdateVisibilityInner";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(session, nullptr);
    session->isVisible_ = true;
    ASSERT_EQ(false, session->UpdateVisibilityInner(true));
}

/**
 * @tc.name: UpdateInteractiveInner
 * @tc.desc: UpdateInteractiveInner function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, UpdateInteractiveInner, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateInteractiveInner";
    info.bundleName_ = "UpdateInteractiveInner";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    ASSERT_NE(session, nullptr);
    session->foregroundInteractiveStatus_.store(true);
    ASSERT_EQ(false, session->UpdateInteractiveInner(true));
    ASSERT_EQ(true, session->UpdateInteractiveInner(false));
}

/**
 * @tc.name: IsAnco
 * @tc.desc: IsAnco function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, IsAnco, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "IsAnco";
    info.bundleName_ = "IsAnco";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    bool res = session->IsAnco();
    EXPECT_EQ(res, false);

    session->collaboratorType_ = CollaboratorType::RESERVE_TYPE;
    res = session->IsAnco();
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: ProcessUpdatePropertyByAction
 * @tc.desc: ProcessUpdatePropertyByAction function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, ProcessUpdatePropertyByAction, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "ProcessUpdatePropertyByAction";
    info.bundleName_ = "ProcessUpdatePropertyByAction";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(property, nullptr);

    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_FOCUSABLE);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_TOUCHABLE);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_ORIENTATION);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE);
    session->ProcessUpdatePropertyByAction(
        property, WSPropertyChangeAction::ACTION_UPDATE_SYSTEM_PRIVACY_MODE);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_SNAPSHOT_SKIP);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_MAXIMIZE_STATE);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_OTHER_PROPS);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_STATUS_PROPS);
    session->ProcessUpdatePropertyByAction(
        property, WSPropertyChangeAction::ACTION_UPDATE_NAVIGATION_INDICATOR_PROPS);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_FLAGS);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_MODE);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_DECOR_ENABLE);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_DRAGENABLED);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_RAISEENABLED);
    session->ProcessUpdatePropertyByAction(
        property, WSPropertyChangeAction::ACTION_UPDATE_HIDE_NON_SYSTEM_FLOATING_WINDOWS);
    session->ProcessUpdatePropertyByAction(
        property, WSPropertyChangeAction::ACTION_UPDATE_TEXTFIELD_AVOID_INFO);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_WINDOW_MASK);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_TOPMOST);
    session->ProcessUpdatePropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO);
}

/**
 * @tc.name: HandleActionUpdateTurnScreenOn
 * @tc.desc: HandleActionUpdateTurnScreenOn function01
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, HandleActionUpdateTurnScreenOn, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleActionUpdateTurnScreenOn";
    info.bundleName_ = "HandleActionUpdateTurnScreenOn";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(property, nullptr);

    auto res = session->HandleActionUpdateTurnScreenOn(
        property, WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON);
    EXPECT_EQ(res, WMError::WM_OK);

    property->SetTurnScreenOn(true);
    res = session->HandleActionUpdateTurnScreenOn(
        property, WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON);
    EXPECT_EQ(res, WMError::WM_OK);
}

/**
 * @tc.name: HandleActionUpdatePrivacyMode1
 * @tc.desc: HandleActionUpdatePrivacyMode1
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, HandleActionUpdatePrivacyMode1, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleActionUpdatePrivacyMode1";
    info.bundleName_ = "HandleActionUpdatePrivacyMode1";
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    EXPECT_NE(session->property_, nullptr);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->surfaceNode_ = surfaceNode;
    session->property_->SetPrivacyMode(false);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetPrivacyMode(true);
    auto res =
        session->HandleActionUpdatePrivacyMode(property, WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE);
    EXPECT_EQ(WMError::WM_OK, res);
    EXPECT_EQ(true, session->property_->GetPrivacyMode());
}

/**
 * @tc.name: HandleActionUpdatePrivacyMode2
 * @tc.desc: HandleActionUpdatePrivacyMode2
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, HandleActionUpdatePrivacyMode2, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleActionUpdatePrivacyMode2";
    info.bundleName_ = "HandleActionUpdatePrivacyMode2";
    info.windowType_ = static_cast<uint32_t>(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);
    EXPECT_NE(session->property_, nullptr);
    struct RSSurfaceNodeConfig config;
    std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Create(config);
    session->surfaceNode_ = surfaceNode;
    session->property_->SetPrivacyMode(true);
    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    property->SetPrivacyMode(false);
    auto res =
        session->HandleActionUpdatePrivacyMode(property, WSPropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE);
    EXPECT_EQ(WMError::WM_OK, res);
    EXPECT_EQ(false, session->property_->GetPrivacyMode());
}

/**
 * @tc.name: UpdateClientRect01
 * @tc.desc: UpdateClientRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, UpdateClientRect01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateClientRect01";
    info.bundleName_ = "UpdateClientRect01";
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);

    WSRect rect = { 0, 0, 0, 0 };
    EXPECT_EQ(session->UpdateClientRect(rect), WSError::WS_OK);

    session->SetClientRect(rect);
    rect.posX_ = 100;
    rect.posY_ = 100;
    rect.width_ = 800;
    rect.height_ = 800;
    EXPECT_EQ(session->UpdateClientRect(rect), WSError::WS_OK);

    session->SetClientRect(rect);
    EXPECT_EQ(session->UpdateClientRect(rect), WSError::WS_OK);
}

/**
 * @tc.name: UpdateRect01
 * @tc.desc: UpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, UpdateRect01, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "UpdateRect01";
    info.bundleName_ = "UpdateRect01";
    info.windowType_ = static_cast<uint32_t>(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session->property_, nullptr);
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;

    WSRect rect = { 200, 200, 200, 200 };
    session->winRect_ = rect;
    session->SetClientRect(rect);
    EXPECT_EQ(session->UpdateRect(rect, reason, "SceneSessionTest5"), WSError::WS_OK);

    rect.posX_ = 100;
    rect.posY_ = 100;
    rect.width_ = 800;
    rect.height_ = 800;
    session->winRect_ = rect;
    EXPECT_EQ(session->UpdateRect(rect, reason, "SceneSessionTest5"), WSError::WS_OK);
}

/**
 * @tc.name: HandleMoveDragSurfaceNode
 * @tc.desc: HandleMoveDragSurfaceNode Test
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionTest5, HandleMoveDragSurfaceNode, Function | SmallTest | Level2)
{
    SessionInfo info;
    info.abilityName_ = "HandleMoveDragSurfaceNode";
    info.bundleName_ = "HandleMoveDragSurfaceNode";
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(info, nullptr);
    EXPECT_NE(session, nullptr);

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    EXPECT_NE(property, nullptr);

    session->moveDragController_ = sptr<MoveDragController>::MakeSptr(2024);
    EXPECT_NE(session->moveDragController_, nullptr);

    session->HandleMoveDragSurfaceNode(SizeChangeReason::DRAG_START);
    session->HandleMoveDragSurfaceNode(SizeChangeReason::DRAG);
    session->HandleMoveDragSurfaceNode(SizeChangeReason::MOVE);
    session->HandleMoveDragSurfaceNode(SizeChangeReason::DRAG_END);
}
}
}
}