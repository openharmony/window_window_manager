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
#include "ability_context_impl.h"
#include "display_info.h"
#include "mock_session.h"
#include "mock_uicontent.h"
#include "mock_window.h"
#include "parameters.h"
#include "scene_board_judgement.h"
#include "window_session_impl.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSessionImplTest3 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<WindowSessionImpl> window_;
};

void WindowSessionImplTest3::SetUpTestCase() {}

void WindowSessionImplTest3::TearDownTestCase() {}

void WindowSessionImplTest3::SetUp() {}

void WindowSessionImplTest3::TearDown()
{
    if (window_ != nullptr) {
        window_->Destroy();
    }
}

namespace {
sptr<WindowSessionImpl> GetTestWindowImpl(const std::string& name)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    if (option == nullptr) {
        return nullptr;
    }
    option->SetWindowName(name);
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    if (window == nullptr) {
        return nullptr;
    }
    SessionInfo sessionInfo = { name, name, name };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    if (session == nullptr) {
        return nullptr;
    }
    window->hostSession_ = session;
    return window;
}

/**
 * @tc.name: SetInputEventConsumer
 * @tc.desc: SetInputEventConsumer01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetInputEventConsumer01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: SetInputEventConsumer01 start";
    window_ = GetTestWindowImpl("SetInputEventConsumer01");
    ASSERT_NE(window_, nullptr);
    window_->inputEventConsumer_ = nullptr;
    std::shared_ptr<IInputEventConsumer> inputEventConsumer = std::make_shared<MockInputEventConsumer>();
    window_->SetInputEventConsumer(inputEventConsumer);
    ASSERT_NE(window_->inputEventConsumer_, nullptr);
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: SetInputEventConsumer01 end";
}

/**
 * @tc.name: SetContinueState
 * @tc.desc: SetContinueState test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetContinueState, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: SetContinueState start";
    window_ = GetTestWindowImpl("SetContinueState");
    ASSERT_NE(window_, nullptr);
    WMError ret = window_->SetContinueState(static_cast<int32_t>(ContinueState::CONTINUESTATE_INACTIVE));
    ASSERT_EQ(ret, WMError::WM_OK);
    ret = window_->SetContinueState(-100);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_PARAM);
        ret = window_->SetContinueState(3);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_PARAM);
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: SetContinueState end";
}

/**
 * @tc.name: GetListeners
 * @tc.desc: GetListeners01 IDisplayMoveListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, GetListeners01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: GetListeners01 start";
    window_ = GetTestWindowImpl("GetListeners01");
    ASSERT_NE(window_, nullptr);
    window_->displayMoveListeners_.clear();
    window_->NotifyDisplayMove(0, 100);
    ASSERT_TRUE(window_->displayMoveListeners_[window_->GetPersistentId()].empty());

    sptr<IDisplayMoveListener> displayMoveListener = new (std::nothrow) MockIDisplayMoveListener();
    ASSERT_EQ(window_->RegisterDisplayMoveListener(displayMoveListener), WMError::WM_OK);
    window_->NotifyDisplayMove(0, 100);
    ASSERT_FALSE(window_->displayMoveListeners_[window_->GetPersistentId()].empty());
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: GetListeners01 end";
}

/**
 * @tc.name: RegisterWindowNoInteractionListener
 * @tc.desc: RegisterWindowNoInteractionListener01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, RegisterWindowNoInteractionListener01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: RegisterWindowNoInteractionListener01 start";
    window_ = GetTestWindowImpl("RegisterWindowNoInteractionListener01");
    ASSERT_NE(window_, nullptr);
    ASSERT_EQ(window_->RegisterWindowNoInteractionListener(nullptr), WMError::WM_ERROR_NULLPTR);
    ASSERT_EQ(window_->UnregisterWindowNoInteractionListener(nullptr), WMError::WM_ERROR_NULLPTR);

    sptr<IWindowNoInteractionListener> windowNoInteractionListenerSptr =
        new (std::nothrow) MockIWindowNoInteractionListener();
    ASSERT_EQ(window_->RegisterWindowNoInteractionListener(windowNoInteractionListenerSptr), WMError::WM_OK);
    ASSERT_EQ(window_->UnregisterWindowNoInteractionListener(windowNoInteractionListenerSptr), WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest3: RegisterWindowNoInteractionListener01 end";
}

/**
 * @tc.name: SetForceSplitEnable
 * @tc.desc: SetForceSplitEnable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetForceSplitEnable, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetForceSplitEnable start";
    window_ = GetTestWindowImpl("SetForceSplitEnable");
    ASSERT_NE(window_, nullptr);

    bool isForceSplit = false;
    std::string homePage = "MainPage";
    int32_t res = 0;
    window_->SetForceSplitEnable(isForceSplit, homePage);
    ASSERT_EQ(res, 0);

    isForceSplit = true;
    window_->SetForceSplitEnable(isForceSplit, homePage);
    ASSERT_EQ(res, 0);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetForceSplitEnable end";
}

/**
 * @tc.name: GetAppForceLandscapeConfig01
 * @tc.desc: GetAppForceLandscapeConfig
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, GetAppForceLandscapeConfig01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetAppForceLandscapeConfig start";
    window_ = GetTestWindowImpl("GetAppForceLandscapeConfig01");
    ASSERT_NE(window_, nullptr);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window_->hostSession_ = session;
    window_->property_->SetPersistentId(1);
    window_->state_ = WindowState::STATE_CREATED;
    AppForceLandscapeConfig config = {};
    auto res = window_->GetAppForceLandscapeConfig(config);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(res, WMError::WM_OK);
        ASSERT_EQ(config.mode_, 0);
        ASSERT_EQ(config.homePage_, "");
    }
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetAppForceLandscapeConfig end";
}

/**
 * @tc.name: GetAppForceLandscapeConfig02
 * @tc.desc: GetAppForceLandscapeConfig
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, GetAppForceLandscapeConfig02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetAppForceLandscapeConfig start";
    window_ = GetTestWindowImpl("GetAppForceLandscapeConfig");
    ASSERT_NE(window_, nullptr);

    AppForceLandscapeConfig config = {};
    window_->hostSession_ = nullptr;
    auto res = window_->GetAppForceLandscapeConfig(config);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
        ASSERT_EQ(config.mode_, 0);
        ASSERT_EQ(config.homePage_, "");
    }
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetAppForceLandscapeConfig end";
}

/**
 * @tc.name: IsFocused
 * @tc.desc: IsFocused
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, IsFocused, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: IsFocused start";
    window_ = GetTestWindowImpl("IsFocused");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(INVALID_SESSION_ID);
    auto ret = window_->IsFocused();
    ASSERT_EQ(ret, false);

    window_->property_->SetPersistentId(1);
    window_->state_ = WindowState::STATE_CREATED;
    window_->UpdateFocus(true);
    ret = window_->IsFocused();
    ASSERT_EQ(ret, true);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: IsFocused end";
}

/**
 * @tc.name: IsNotifyInteractiveDuplicative
 * @tc.desc: IsNotifyInteractiveDuplicative
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, IsNotifyInteractiveDuplicative, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: IsNotifyInteractiveDuplicative start";
    window_ = GetTestWindowImpl("IsNotifyInteractiveDuplicative");
    ASSERT_NE(window_, nullptr);
    window_->hasFirstNotifyInteractive_ = true;
    window_->interactive_ = true;
    window_->NotifyForegroundInteractiveStatus(true);
    auto ret = window_->IsNotifyInteractiveDuplicative(true);
    ASSERT_EQ(ret, true);
    GTEST_LOG_(INFO) << "IsNotifyInteractiveDuplicative: IsNotifyInteractiveDuplicative end";
}

/**
 * @tc.name: SetMainWindowTopmost
 * @tc.desc: SetMainWindowTopmost
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetMainWindowTopmost, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetMainWindowTopmost start";
    window_ = GetTestWindowImpl("SetMainWindowTopmost");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(INVALID_SESSION_ID);
    auto ret = window_->SetMainWindowTopmost(true);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetMainWindowTopmost end";
}

/**
 * @tc.name: GetRequestedOrientation
 * @tc.desc: GetRequestedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, GetRequestedOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetRequestedOrientation start";
    window_ = GetTestWindowImpl("GetRequestedOrientation");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(INVALID_SESSION_ID);
    auto ret = window_->GetRequestedOrientation();
    ASSERT_EQ(ret, Orientation::UNSPECIFIED);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetRequestedOrientation end";
}

/**
 * @tc.name: SetDecorVisible
 * @tc.desc: SetDecorVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetDecorVisible, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetDecorVisible start";
    window_ = GetTestWindowImpl("SetDecorVisible");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(INVALID_SESSION_ID);
    auto ret = window_->SetDecorVisible(true);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetDecorVisible end";
}

/**
 * @tc.name: SetWindowModal
 * @tc.desc: SetWindowModal
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetWindowModal, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetWindowModal start";
    window_ = GetTestWindowImpl("SetWindowModal");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(INVALID_SESSION_ID);
    auto ret = window_->SetWindowModal(true);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);
    ret = window_->SetWindowModal(false);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);

    window_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window_->property_->SetPersistentId(1);
    window_->state_ = WindowState::STATE_CREATED;
    window_->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ret = window_->SetWindowModal(true);
    ASSERT_EQ(WMError::WM_OK, ret);
    ret = window_->SetWindowModal(false);
    ASSERT_EQ(WMError::WM_OK, ret);

    window_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = window_->SetWindowModal(true);
    ASSERT_EQ(WMError::WM_OK, ret);
    ret = window_->SetWindowModal(false);
    ASSERT_EQ(WMError::WM_OK, ret);
    
    window_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    ret = window_->SetWindowModal(true);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_CALLING);
    ret = window_->SetWindowModal(false);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_CALLING);
    
    window_->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ret = window_->SetWindowModal(false);
    ASSERT_EQ(ret, WMError::WM_ERROR_DEVICE_NOT_SUPPORT);
    ret = window_->SetWindowModal(true);
    ASSERT_EQ(ret, WMError::WM_ERROR_DEVICE_NOT_SUPPORT);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetWindowModal end";
}

/**
 * @tc.name: SetDecorButtonStyle
 * @tc.desc: SetDecorButtonStyle
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetDecorButtonStyle, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetDecorButtonStyle start";
    window_ = GetTestWindowImpl("SetDecorButtonStyle");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(INVALID_SESSION_ID);
    DecorButtonStyle decorButtonStyle;
    auto ret = window_->SetDecorButtonStyle(decorButtonStyle);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetDecorButtonStyle end";
}

/**
 * @tc.name: GetDecorButtonStyle
 * @tc.desc: GetDecorButtonStyle
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, GetDecorButtonStyle, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetDecorButtonStyle start";
    window_ = GetTestWindowImpl("GetDecorButtonStyle");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(INVALID_SESSION_ID);
    DecorButtonStyle decorButtonStyle;
    auto ret = window_->GetDecorButtonStyle(decorButtonStyle);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetDecorButtonStyle end";
}

/**
 * @tc.name: RegisterMainWindowCloseListeners
 * @tc.desc: RegisterMainWindowCloseListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, RegisterMainWindowCloseListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: RegisterMainWindowCloseListeners start";
    window_ = GetTestWindowImpl("RegisterMainWindowCloseListeners");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(INVALID_SESSION_ID);
    sptr<IMainWindowCloseListener> listener = sptr<IMainWindowCloseListener>::MakeSptr();
    ASSERT_NE(listener, nullptr);
    auto ret = window_->RegisterMainWindowCloseListeners(listener);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);

    window_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    window_->property_->SetPersistentId(1);
    window_->state_ = WindowState::STATE_CREATED;
    ret = window_->RegisterMainWindowCloseListeners(listener);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_CALLING);

    window_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window_->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ret = window_->RegisterMainWindowCloseListeners(listener);
    ASSERT_EQ(ret, WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: RegisterMainWindowCloseListeners end";
}

/**
 * @tc.name: UnregisterMainWindowCloseListeners
 * @tc.desc: UnregisterMainWindowCloseListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, UnregisterMainWindowCloseListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UnregisterMainWindowCloseListeners start";
    window_ = GetTestWindowImpl("UnregisterMainWindowCloseListeners");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(INVALID_SESSION_ID);
    sptr<IMainWindowCloseListener> listener = sptr<IMainWindowCloseListener>::MakeSptr();
    ASSERT_NE(listener, nullptr);
    auto ret = window_->UnregisterMainWindowCloseListeners(listener);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_WINDOW);

    window_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    window_->property_->SetPersistentId(1);
    window_->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    window_->state_ = WindowState::STATE_CREATED;
    ret = window_->UnregisterMainWindowCloseListeners(listener);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_CALLING);

    window_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ret = window_->UnregisterMainWindowCloseListeners(listener);
    ASSERT_EQ(ret, WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UnregisterMainWindowCloseListeners end";
}

/**
 * @tc.name: RegisterSwitchFreeMultiWindowListener
 * @tc.desc: RegisterSwitchFreeMultiWindowListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, RegisterSwitchFreeMultiWindowListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: RegisterSwitchFreeMultiWindowListener start";
    window_ = GetTestWindowImpl("RegisterSwitchFreeMultiWindowListener");
    ASSERT_NE(window_, nullptr);
    sptr<ISwitchFreeMultiWindowListener> listener = sptr<ISwitchFreeMultiWindowListener>::MakeSptr();
    ASSERT_NE(listener, nullptr);
    window_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    auto ret = window_->RegisterSwitchFreeMultiWindowListener(listener);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_CALLING);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: RegisterSwitchFreeMultiWindowListener end";
}

/**
 * @tc.name: UnregisterSwitchFreeMultiWindowListener
 * @tc.desc: UnregisterSwitchFreeMultiWindowListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, UnregisterSwitchFreeMultiWindowListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UnregisterSwitchFreeMultiWindowListener start";
    window_ = GetTestWindowImpl("UnregisterSwitchFreeMultiWindowListener");
    ASSERT_NE(window_, nullptr);
    sptr<ISwitchFreeMultiWindowListener> listener = sptr<ISwitchFreeMultiWindowListener>::MakeSptr();
    ASSERT_NE(listener, nullptr);
    window_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_END);
    auto ret = window_->UnregisterSwitchFreeMultiWindowListener(listener);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_CALLING);
    window_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    ret = window_->UnregisterSwitchFreeMultiWindowListener(listener);
    ASSERT_EQ(ret, WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UnregisterSwitchFreeMultiWindowListener end";
}

/**
 * @tc.name: SetSplitButtonVisible
 * @tc.desc: SetSplitButtonVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetSplitButtonVisible, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetSplitButtonVisible start";
    window_ = GetTestWindowImpl("SetSplitButtonVisible");
    ASSERT_NE(window_, nullptr);
    auto ret = window_->SetSplitButtonVisible(true);
    ASSERT_EQ(ret, WSError::WS_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetSplitButtonVisible end";
}

/**
 * @tc.name: NotifyNoInteractionTimeout
 * @tc.desc: NotifyNoInteractionTimeout
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, NotifyNoInteractionTimeout, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: NotifyNoInteractionTimeout start";
    window_ = GetTestWindowImpl("NotifyNoInteractionTimeout");
    ASSERT_NE(window_, nullptr);
    IWindowNoInteractionListenerSptr listener = nullptr;
    auto ret = window_->NotifyNoInteractionTimeout(listener);
    ASSERT_EQ(ret, WSError::WS_ERROR_NULLPTR);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: NotifyNoInteractionTimeout end";
}

/**
 * @tc.name: IsVerticalOrientation
 * @tc.desc: IsVerticalOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, IsVerticalOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: IsVerticalOrientation start";
    window_ = GetTestWindowImpl("IsVerticalOrientation");
    ASSERT_NE(window_, nullptr);
    Orientation orientation = Orientation::VERTICAL;
    auto ret = window_->IsVerticalOrientation(orientation);
    ASSERT_EQ(ret, true);
    orientation = Orientation::REVERSE_VERTICAL;
    ret = window_->IsVerticalOrientation(orientation);
    ASSERT_EQ(ret, true);
    orientation = Orientation::SENSOR_VERTICAL;
    ret = window_->IsVerticalOrientation(orientation);
    ASSERT_EQ(ret, true);
    orientation = Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED;
    ret = window_->IsVerticalOrientation(orientation);
    ASSERT_EQ(ret, true);
    orientation = Orientation::USER_ROTATION_PORTRAIT;
    ret = window_->IsVerticalOrientation(orientation);
    ASSERT_EQ(ret, true);
    orientation = Orientation::USER_ROTATION_PORTRAIT_INVERTED;
    ret = window_->IsVerticalOrientation(orientation);
    ASSERT_EQ(ret, true);
    orientation = Orientation::UNSPECIFIED;
    ret = window_->IsVerticalOrientation(orientation);
    ASSERT_EQ(ret, false);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: IsVerticalOrientation end";
}

/**
 * @tc.name: MarkProcessed
 * @tc.desc: MarkProcessed
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, MarkProcessed, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: MarkProcessed start";
    window_ = GetTestWindowImpl("MarkProcessed");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(1);
    window_->state_ = WindowState::STATE_CREATED;
    auto ret = window_->MarkProcessed(1);
    ASSERT_EQ(ret, WSError::WS_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: MarkProcessed end";
}

/**
 * @tc.name: UpdateRectForOtherReasonTask
 * @tc.desc: UpdateRectForOtherReasonTask
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, UpdateRectForOtherReasonTask, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateRectForOtherReasonTask start";
    window_ = GetTestWindowImpl("UpdateRectForOtherReasonTask");
    ASSERT_NE(window_, nullptr);
    Rect wmRect = { 0, 0, 0, 0 };
    Rect preRect = { 0, 0, 0, 0 };
    WindowSizeChangeReason wmReason = WindowSizeChangeReason::UNDEFINED;
    std::shared_ptr<RSTransaction> rsTransaction = nullptr;
    window_->lastSizeChangeReason_ = WindowSizeChangeReason::UNDEFINED;
    window_->postTaskDone_ = false;
    window_->UpdateRectForOtherReasonTask(wmRect, preRect, wmReason, rsTransaction);
    ASSERT_EQ(window_->postTaskDone_, true);
    window_->UpdateRectForOtherReasonTask(wmRect, preRect, wmReason, rsTransaction);
    window_->postTaskDone_ = false;
    wmRect.posX_ = 1;
    window_->UpdateRectForOtherReasonTask(wmRect, preRect, wmReason, rsTransaction);
    ASSERT_EQ(window_->postTaskDone_, true);

    window_->handler_ = nullptr;
    window_->UpdateRectForOtherReason(wmRect, preRect, wmReason, rsTransaction);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateRectForOtherReasonTask end";
}

/**
 * @tc.name: CopyUniqueDensityParameter
 * @tc.desc: CopyUniqueDensityParameter
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, CopyUniqueDensityParameter, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: CopyUniqueDensityParameter start";
    window_ = GetTestWindowImpl("CopyUniqueDensityParameter");
    ASSERT_NE(window_, nullptr);
    sptr<WindowSessionImpl> parentWindow = GetTestWindowImpl("CopyUniqueDensityParameter01");
    ASSERT_NE(parentWindow, nullptr);
    window_->useUniqueDensity_ = false;
    window_->virtualPixelRatio_ = 1.0f;
    parentWindow->useUniqueDensity_ = true;
    parentWindow->virtualPixelRatio_ = 1.0f;
    window_->CopyUniqueDensityParameter(parentWindow);
    ASSERT_EQ(window_->useUniqueDensity_, true);
    parentWindow = nullptr;
    window_->CopyUniqueDensityParameter(parentWindow);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: CopyUniqueDensityParameter end";
}

/**
 * @tc.name: SetRaiseByClickEnabled
 * @tc.desc: SetRaiseByClickEnabled
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetRaiseByClickEnabled, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetRaiseByClickEnabled start";
    window_ = GetTestWindowImpl("SetRaiseByClickEnabled");
    ASSERT_NE(window_, nullptr);
    window_->property_->parentPersistentId_ = 2;
    window_->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    window_->state_ = WindowState::STATE_SHOWN;
    window_->property_->SetPersistentId(1);
    auto ret = window_->SetRaiseByClickEnabled(true);
    ASSERT_EQ(ret, WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetRaiseByClickEnabled end";
}

/**
 * @tc.name: SetSubWindowModal
 * @tc.desc: SetSubWindowModal
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetSubWindowModal, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetSubWindowModal start";
    window_ = GetTestWindowImpl("SetSubWindowModal");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(1);
    window_->state_ = WindowState::STATE_CREATED;
    window_->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ModalityType modalityType = ModalityType::APPLICATION_MODALITY;
    window_->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    window_->windowSystemConfig_.freeMultiWindowEnable_ = false;
    window_->windowSystemConfig_.freeMultiWindowSupport_ = false;
    auto ret = window_->SetSubWindowModal(true, modalityType);
    ASSERT_EQ(ret, WMError::WM_ERROR_DEVICE_NOT_SUPPORT);

    window_->vsyncStation_ = nullptr;
    window_->ClearVsyncStation();
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetSubWindowModal end";
}

/**
 * @tc.name: UpdateFrameLayoutCallbackIfNeeded
 * @tc.desc: UpdateFrameLayoutCallbackIfNeeded
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, UpdateFrameLayoutCallbackIfNeeded, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateFrameLayoutCallbackIfNeeded start";
    window_ = GetTestWindowImpl("UpdateFrameLayoutCallbackIfNeeded");
    ASSERT_NE(window_, nullptr);
    window_->enableFrameLayoutFinishCb_ = false;
    WindowSizeChangeReason wmReason = WindowSizeChangeReason::FULL_TO_SPLIT;
    window_->UpdateFrameLayoutCallbackIfNeeded(wmReason);
    ASSERT_EQ(window_->enableFrameLayoutFinishCb_, true);

    window_->enableFrameLayoutFinishCb_ = false;
    wmReason = WindowSizeChangeReason::SPLIT_TO_FULL;
    window_->UpdateFrameLayoutCallbackIfNeeded(wmReason);
    ASSERT_EQ(window_->enableFrameLayoutFinishCb_, true);

    window_->enableFrameLayoutFinishCb_ = false;
    wmReason = WindowSizeChangeReason::FULL_TO_FLOATING;
    window_->UpdateFrameLayoutCallbackIfNeeded(wmReason);
    ASSERT_EQ(window_->enableFrameLayoutFinishCb_, true);

    window_->enableFrameLayoutFinishCb_ = false;
    wmReason = WindowSizeChangeReason::FLOATING_TO_FULL;
    window_->UpdateFrameLayoutCallbackIfNeeded(wmReason);
    ASSERT_EQ(window_->enableFrameLayoutFinishCb_, true);

    window_->enableFrameLayoutFinishCb_ = false;
    wmReason = WindowSizeChangeReason::DRAG_END;
    window_->windowSystemConfig_.freeMultiWindowEnable_ = true;
    window_->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window_->UpdateFrameLayoutCallbackIfNeeded(wmReason);
    ASSERT_EQ(window_->enableFrameLayoutFinishCb_, true);

    window_->windowSystemConfig_.freeMultiWindowSupport_ = false;
    window_->UpdateFrameLayoutCallbackIfNeeded(wmReason);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UpdateFrameLayoutCallbackIfNeeded end";
}

/**
 * @tc.name: SetRequestedOrientation
 * @tc.desc: SetRequestedOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetRequestedOrientation, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetRequestedOrientation start";
    window_ = GetTestWindowImpl("SetRequestedOrientation");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(1);
    window_->state_ = WindowState::STATE_CREATED;
    Orientation orientation = Orientation::VERTICAL;
    window_->property_->requestedOrientation_ = Orientation::VERTICAL;
    window_->SetRequestedOrientation(orientation);
    orientation = Orientation::USER_ROTATION_PORTRAIT;
    window_->SetRequestedOrientation(orientation);
    auto ret = window_->GetRequestedOrientation();
    ASSERT_EQ(ret, orientation);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetRequestedOrientation end";
}

/**
 * @tc.name: SetTargetAPIVersion
 * @tc.desc: SetTargetAPIVersion
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetTargetAPIVersion, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetTargetAPIVersion start";
    window_ = GetTestWindowImpl("SetAPPWindowIcon");
    ASSERT_NE(window_, nullptr);
    uint32_t version = 14;
    window_->SetTargetAPIVersion(version);
    EXPECT_EQ(version, window_->GetTargetAPIVersion());
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetTargetAPIVersion end";
}

/**
 * @tc.name: SetAPPWindowIcon
 * @tc.desc: SetAPPWindowIcon
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, GetTargetAPIVersion, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetTargetAPIVersion start";
    window_ = GetTestWindowImpl("GetTargetAPIVersion");
    ASSERT_NE(window_, nullptr);
    EXPECT_EQ(0, window_->GetTargetAPIVersion());
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetTargetAPIVersion end";
}

/**
 * @tc.name: SetAPPWindowIcon
 * @tc.desc: SetAPPWindowIcon
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetAPPWindowIcon, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetAPPWindowIcon start";
    window_ = GetTestWindowImpl("SetAPPWindowIcon");
    ASSERT_NE(window_, nullptr);
    window_->uiContent_ = nullptr;
    std::shared_ptr<Media::PixelMap> icon = std::make_shared<Media::PixelMap>();
    ASSERT_NE(icon, nullptr);
    auto ret = window_->SetAPPWindowIcon(icon);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetAPPWindowIcon end";
}

/**
 * @tc.name: SetBackgroundColor
 * @tc.desc: SetBackgroundColor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetBackgroundColor, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetBackgroundColor start";
    window_ = GetTestWindowImpl("SetBackgroundColor");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(1);
    window_->state_ = WindowState::STATE_CREATED;
    std::string color = "";
    auto ret = window_->SetBackgroundColor(color);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_PARAM);

    color = "#FF0000";
    ret = window_->SetBackgroundColor(color);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_OPERATION);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetBackgroundColor end";
}

/**
 * @tc.name: Find
 * @tc.desc: Find
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, Find, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: Find start";
    window_ = GetTestWindowImpl("Find");
    ASSERT_NE(window_, nullptr);
    window_->windowSessionMap_.clear();
    std::string name = "Find";
    sptr<WindowSessionImpl> window1 = GetTestWindowImpl("Find1");
    ASSERT_NE(window1, nullptr);
    window_->windowSessionMap_.insert(std::make_pair(name, std::make_pair(1, window1)));
    auto ret = window_->Find(name);
    ASSERT_NE(ret, nullptr);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: Find end";
}

/**
 * @tc.name: RegisterWindowTitleButtonRectChangeListener
 * @tc.desc: RegisterWindowTitleButtonRectChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, RegisterWindowTitleButtonRectChangeListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: RegisterWindowTitleButtonRectChangeListener start";
    window_ = GetTestWindowImpl("RegisterWindowTitleButtonRectChangeListener");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(1);
    window_->state_ = WindowState::STATE_SHOWN;
    window_->interactive_ = true;
    window_->hasFirstNotifyInteractive_ = true;
    window_->NotifyForegroundInteractiveStatus(true);

    sptr<IWindowTitleButtonRectChangedListener> listener = nullptr;
    auto ret = window_->RegisterWindowTitleButtonRectChangeListener(listener);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: RegisterWindowTitleButtonRectChangeListener end";
}

/**
 * @tc.name: GetUIContentWithId
 * @tc.desc: GetUIContentWithId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, GetUIContentWithId, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetUIContentWithId start";
    window_ = GetTestWindowImpl("GetUIContentWithId");
    ASSERT_NE(window_, nullptr);
    window_->windowSessionMap_.clear();
    std::string name = "GetUIContentWithId";
    sptr<WindowSessionImpl> window1 = GetTestWindowImpl("GetUIContentWithId1");
    ASSERT_NE(window1, nullptr);
    window_->windowSessionMap_.insert(std::make_pair(name, std::make_pair(1, window1)));
    auto ret = window_->GetUIContentWithId(1);
    ASSERT_EQ(ret, nullptr);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: GetUIContentWithId end";
}

/**
 * @tc.name: UnregisterWindowRectChangeListener
 * @tc.desc: UnregisterWindowRectChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, UnregisterWindowRectChangeListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UnregisterWindowRectChangeListener start";
    window_ = GetTestWindowImpl("UnregisterWindowRectChangeListener");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(1);
    window_->state_ = WindowState::STATE_SHOWN;
    window_->windowRectChangeListeners_.clear();
    sptr<IWindowRectChangeListener> listener = nullptr;
    auto ret = window_->UnregisterWindowRectChangeListener(listener);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: UnregisterWindowRectChangeListener end";
}

/**
 * @tc.name: IsFloatingWindowAppType
 * @tc.desc: IsFloatingWindowAppType
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, IsFloatingWindowAppType, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: IsFloatingWindowAppType start";
    window_ = GetTestWindowImpl("IsFloatingWindowAppType");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetPersistentId(1);
    window_->property_->isFloatingWindowAppType_ = true;
    window_->state_ = WindowState::STATE_CREATED;
    auto ret = window_->IsFloatingWindowAppType();
    ASSERT_EQ(ret, true);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: IsFloatingWindowAppType end";
}

/**
 * @tc.name: SetWindowContainerColor
 * @tc.desc: SetWindowContainerColor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetWindowContainerColor, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetWindowContainerColor start";
    window_ = GetTestWindowImpl("SetWindowContainerColor");
    ASSERT_NE(window_, nullptr);
    window_->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window_->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window_->windowSystemConfig_.isSystemDecorEnable_ = true;
    window_->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    std::string activeColor = "";
    std::string inactiveColor = "";
    auto ret = window_->SetWindowContainerColor(activeColor, inactiveColor);
    ASSERT_NE(ret, WMError::WM_ERROR_DEVICE_NOT_SUPPORT);
    GTEST_LOG_(INFO) << "WindowSessionImplTest: SetWindowContainerColor end";
}

/**
 * @tc.name: SetAvoidAreaOption
 * @tc.desc: SetAvoidAreaOption
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetAvoidAreaOption, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetAvoidAreaOption");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    window->hostSession_ = session;
    window->state_ = WindowState::STATE_CREATED;
    WMError res = window->SetAvoidAreaOption(3);
    ASSERT_EQ(res, WMError::WM_OK);
}

/**
 * @tc.name: GetAvoidAreaOption
 * @tc.desc: GetAvoidAreaOption
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, GetAvoidAreaOption, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetAvoidAreaOption");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    window->hostSession_ = session;
    window->state_ = WindowState::STATE_CREATED;
    uint32_t avoidAreaOption = 0;
    WMError res = window->GetAvoidAreaOption(avoidAreaOption);
    ASSERT_EQ(res, WMError::WM_OK);
}

/**
 * @tc.name: SetWatchGestureConsumed
 * @tc.desc: SetWatchGestureConsumed test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetWatchGestureConsumed, Function | SmallTest | Level2)
{
    window_ = GetTestWindowImpl("SetWatchGestureConsumed");
    ASSERT_NE(window_, nullptr);
    bool isWatchGestureConsumed = false;
    window_->SetWatchGestureConsumed(isWatchGestureConsumed);
    ASSERT_EQ(window_->GetWatchGestureConsumed(), false);
}

/**
 * @tc.name: NotifyConsumeResultToFloatWindow
 * @tc.desc: NotifyConsumeResultToFloatWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, NotifyConsumeResultToFloatWindow, Function | SmallTest | Level2)
{
    window_ = GetTestWindowImpl("NotifyConsumeResultToFloatWindow");
    ASSERT_NE(window_, nullptr);
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    keyEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_TAB);
    keyEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    window_->SetWatchGestureConsumed(false);
    bool isConsumed = false;
    window_->NotifyConsumeResultToFloatWindow(keyEvent, isConsumed);
    ASSERT_EQ(window_->GetWatchGestureConsumed(), false);
}

/**
 * @tc.name: IsSystemWindow
 * @tc.desc: IsSystemWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, IsSystemWindow, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("IsSystemWindow");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    window->hostSession_ = session;
    window->state_ = WindowState::STATE_CREATED;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_FLOAT);
    bool res = window->IsSystemWindow();
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: IsAppWindow
 * @tc.desc: IsAppWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, IsAppWindow, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("IsAppWindow");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

    window->hostSession_ = session;
    window->state_ = WindowState::STATE_CREATED;
    window->property_->SetWindowType(WindowType::APP_WINDOW_BASE);
    bool res = window->IsAppWindow();
    ASSERT_EQ(res, true);
}

/**
 * @tc.name: SetMouseEventFilter
 * @tc.desc: SetMouseEventFilter
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetMouseEventFilter, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetMouseEventFilter");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    window->hostSession_ = session;
    window->property_->SetPersistentId(1);
    WMError res = window->SetMouseEventFilter([](const OHOS::MMI::PointerEvent& event) {
        return true;
    });
    ASSERT_EQ(res, WMError::WM_OK);
}

/**
 * @tc.name: ClearMouseEventFilter
 * @tc.desc: ClearMouseEventFilter
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, ClearMouseEventFilter, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("ClearMouseEventFilter");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    window->hostSession_ = session;
    window->property_->SetPersistentId(1);
    WMError res = window->ClearMouseEventFilter();
    ASSERT_EQ(res, WMError::WM_OK);
}

/**
 * @tc.name: SetTouchEventFilter
 * @tc.desc: SetTouchEventFilter
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, SetTouchEventFilter, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetTouchEventFilter");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    window->hostSession_ = session;
    window->property_->SetPersistentId(1);
    WMError res = window->SetTouchEventFilter([](const OHOS::MMI::PointerEvent& event) {
        return true;
    });
    ASSERT_EQ(res, WMError::WM_OK);
}

/**
 * @tc.name: ClearTouchEventFilter
 * @tc.desc: ClearTouchEventFilter
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest3, ClearTouchEventFilter, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("ClearTouchEventFilter");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    window->hostSession_ = session;
    window->property_->SetPersistentId(1);
    WMError res = window->ClearTouchEventFilter();
    ASSERT_EQ(res, WMError::WM_OK);
}
} // namespace
} // namespace Rosen
} // namespace OHOS