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
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "ability_context_impl.h"
#include "accessibility_event_info.h"
#include "color_parser.h"
#include "mock_session.h"
#include "window_helper.h"
#include "window_session_impl.h"
#include "wm_common.h"
#include "mock_uicontent.h"
#include "mock_window.h"
#include "parameters.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSessionImplTest4 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 50000;
};

void WindowSessionImplTest4::SetUpTestCase()
{
}

void WindowSessionImplTest4::TearDownTestCase()
{
}

void WindowSessionImplTest4::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowSessionImplTest4::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    abilityContext_ = nullptr;
}

namespace {
/**
 * @tc.name: GetRequestWindowStatetest01
 * @tc.desc: GetRequestWindowState
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, GetRequestWindowState, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetRequestWindowStatetest01 start";
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetRequestWindowState");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    auto ret = window->GetRequestWindowState();
    ASSERT_EQ(ret, WindowState::STATE_INITIAL);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetRequestWindowStatetest01 end";
}

/**
 * @tc.name: GetFocusabletest01
 * @tc.desc: GetFocusable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, GetFocusable, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetFocusabletest01 start";
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetFocusable");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    bool ret = window->GetFocusable();
    ASSERT_EQ(ret, true);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetFocusabletest01 end";
}


/**
 * @tc.name: TransferAccessibilityEvent
 * @tc.desc: TransferAccessibilityEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, TransferAccessibilityEvent, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: TransferAccessibilityEvent start";
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    Accessibility::AccessibilityEventInfo info;
    int64_t uiExtensionIdLevel = 0;
    ASSERT_EQ(WMError::WM_OK, window->TransferAccessibilityEvent(info, uiExtensionIdLevel));
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: TransferAccessibilityEvent end";
}

/**
 * @tc.name: SetSingleFrameComposerEnabled01
 * @tc.desc: SetSingleFrameComposerEnabled and check the retCode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetSingleFrameComposerEnabled01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetSingleFrameComposerEnabled01");
    sptr<WindowSessionImpl> window = new(std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    WMError retCode = window->SetSingleFrameComposerEnabled(false);
    ASSERT_EQ(retCode, WMError::WM_ERROR_INVALID_WINDOW);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_CREATED;
    retCode = window->SetSingleFrameComposerEnabled(false);
    ASSERT_EQ(retCode, WMError::WM_OK);

    window->surfaceNode_ = nullptr;
    retCode = window->SetSingleFrameComposerEnabled(false);
    ASSERT_EQ(retCode, WMError::WM_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: SetTopmost
 * @tc.desc: SetTopmost
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetTopmost, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTopmost");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    WMError res = window->SetTopmost(true);
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, res);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    res = window->SetTopmost(true);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, res);

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_CREATED;
    res = window->SetTopmost(true);
    ASSERT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: IsTopmost
 * @tc.desc: IsTopmost
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, IsTopmost, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("IsTopmost");
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    bool res = window->IsTopmost();
    ASSERT_FALSE(res);
}

/**
 * @tc.name: SetMainWindowTopmost
 * @tc.desc: SetMainWindowTopmost
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetMainWindowTopmost, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetMainWindowTopmost");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    WMError res = window->SetMainWindowTopmost(true);
    ASSERT_EQ(WMError::WM_ERROR_DEVICE_NOT_SUPPORT, res);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    res = window->SetMainWindowTopmost(true);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, res);

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_CREATED;
    res = window->SetMainWindowTopmost(true);
    ASSERT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: IsMainWindowTopmost
 * @tc.desc: IsMainWindowTopmost
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, IsMainWindowTopmost, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("IsMainWindowTopmost");
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    bool res = window->IsMainWindowTopmost();
    ASSERT_FALSE(res);
}

/**
 * @tc.name: SetDecorVisible
 * @tc.desc: SetDecorVisible and check the retCode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetDecorVisible, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetDecorVisibletest01 start";
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetDecorVisible");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    bool isVisible = true;
    WMError res = window->SetDecorVisible(isVisible);
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);

    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    res = window->SetDecorVisible(isVisible);
    ASSERT_EQ(res, WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetDecorVisibletest01 end";
}

/**
 * @tc.name: SetSubWindowModal
 * @tc.desc: SetSubWindowModal and check the retCode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetSubWindowModal, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetSubWindowModaltest01 start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetSubWindowModal");
    sptr<WindowSessionImpl> mainWindow = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, mainWindow);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    mainWindow->hostSession_ = session;
    ASSERT_NE(nullptr, mainWindow->property_);
    mainWindow->property_->SetPersistentId(1); // 1 is main window id
    mainWindow->state_ = WindowState::STATE_CREATED;
    WMError res = mainWindow->SetSubWindowModal(true); // main window is invalid
    ASSERT_EQ(WMError::WM_ERROR_INVALID_CALLING, res);

    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(window, nullptr);
    res = window->SetSubWindowModal(true); // sub window is valid
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, res); // window state is invalid

    window->hostSession_ = session;
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetPersistentId(2); // 2 is sub window id
    window->state_ = WindowState::STATE_CREATED;
    res = window->SetSubWindowModal(true); // sub window is valid
    ASSERT_EQ(WMError::WM_OK, res);
    res = window->SetSubWindowModal(false);
    ASSERT_EQ(WMError::WM_OK, res);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetSubWindowModaltest01 end";
}

/**
 * @tc.name: SetSubWindowModal02
 * @tc.desc: SetSubWindowModal and check the retCode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetSubWindowModal02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetSubWindowModaltest02 start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetSubWindowModal02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(window, nullptr);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    WMError res = window->SetSubWindowModal(true, ModalityType::WINDOW_MODALITY);
    ASSERT_EQ(res, WMError::WM_OK);
    res = window->SetSubWindowModal(true, ModalityType::APPLICATION_MODALITY);
    ASSERT_EQ(res, WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetSubWindowModaltest02 end";
}

/**
 * @tc.name: IsPcOrPadFreeMultiWindowMode
 * @tc.desc: IsPcOrPadFreeMultiWindowMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, IsPcOrPadFreeMultiWindowMode, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: IsPcOrPadFreeMultiWindowMode start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("IsPcOrPadFreeMultiWindowMode");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    ASSERT_EQ(true, window->IsPcOrPadFreeMultiWindowMode());
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    ASSERT_EQ(false, window->IsPcOrPadFreeMultiWindowMode());
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: IsPcOrPadFreeMultiWindowMode end";
}

/**
 * @tc.name: GetDecorHeight
 * @tc.desc: GetDecorHeight and check the retCode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, GetDecorHeight, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetDecorHeighttest01 start";
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetDecorHeight");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    int32_t height = 0;
    WMError res = window->GetDecorHeight(height);
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetDecorHeighttest01 end";
}

/**
 * @tc.name: GetTitleButtonArea
 * @tc.desc: GetTitleButtonArea and check the retCode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, GetTitleButtonArea, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetTitleButtonAreatest01 start";
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetTitleButtonArea");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    TitleButtonRect titleButtonRect;
    WMError res = window->GetTitleButtonArea(titleButtonRect);
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetDecorHeighttest01 end";
}

/**
 * @tc.name: GetUIContentRemoteObj
 * @tc.desc: GetUIContentRemoteObj and check the retCode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, GetUIContentRemoteObj, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetUIContentRemoteObj start";
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    sptr<IRemoteObject> remoteObj;
    WSError res = window->GetUIContentRemoteObj(remoteObj);
    ASSERT_EQ(res, WSError::WS_ERROR_NULLPTR);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    res = window->GetUIContentRemoteObj(remoteObj);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetUIContentRemoteObj end";
}

/**
 * @tc.name: RegisterExtensionAvoidAreaChangeListener
 * @tc.desc: RegisterExtensionAvoidAreaChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, RegisterExtensionAvoidAreaChangeListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: RegisterExtensionAvoidAreaChangeListener start";
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetTitleButtonArea");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    sptr<IAvoidAreaChangedListener> listener = nullptr;
    WMError res = window->RegisterExtensionAvoidAreaChangeListener(listener);
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);

    listener = sptr<IAvoidAreaChangedListener>::MakeSptr();
    vector<sptr<IAvoidAreaChangedListener>> holder;
    window->avoidAreaChangeListeners_[window->property_->GetPersistentId()] = holder;
    res = window->RegisterExtensionAvoidAreaChangeListener(listener);
    ASSERT_EQ(res, WMError::WM_OK);
    holder = window->avoidAreaChangeListeners_[window->property_->GetPersistentId()];
    auto existsListener = std::find(holder.begin(), holder.end(), listener);
    ASSERT_NE(existsListener, holder.end());

    // already registered
    res = window->RegisterExtensionAvoidAreaChangeListener(listener);
    ASSERT_EQ(res, WMError::WM_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: RegisterExtensionAvoidAreaChangeListener end";
}

/**
 * @tc.name: UnregisterExtensionAvoidAreaChangeListener
 * @tc.desc: UnregisterExtensionAvoidAreaChangeListener Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, UnregisterExtensionAvoidAreaChangeListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: UnregisterExtensionAvoidAreaChangeListener start";
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetTitleButtonArea");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    sptr<IAvoidAreaChangedListener> listener = nullptr;
    WMError res = window->UnregisterExtensionAvoidAreaChangeListener(listener);
    ASSERT_EQ(res, WMError::WM_ERROR_NULLPTR);

    listener = sptr<IAvoidAreaChangedListener>::MakeSptr();
    vector<sptr<IAvoidAreaChangedListener>> holder;
    window->avoidAreaChangeListeners_[window->property_->GetPersistentId()] = holder;
    window->RegisterExtensionAvoidAreaChangeListener(listener);

    res = window->UnregisterExtensionAvoidAreaChangeListener(listener);
    ASSERT_EQ(res, WMError::WM_OK);

    holder = window->avoidAreaChangeListeners_[window->property_->GetPersistentId()];
    auto existsListener = std::find(holder.begin(), holder.end(), listener);
    ASSERT_EQ(existsListener, holder.end());
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: UnregisterExtensionAvoidAreaChangeListener end";
}

/**
 * @tc.name: SetPipActionEvent
 * @tc.desc: SetPipActionEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetPipActionEvent, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetPipActionEvent start";
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetTitleButtonArea");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(nullptr, window->GetUIContentWithId(10000));
    window->property_->SetPersistentId(1);

    SessionInfo sessionInfo = { "CreateTestBundle", "TestGetUIContentWithId", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_EQ(window->FindWindowById(1), nullptr);
    ASSERT_EQ(nullptr, window->GetUIContentWithId(1));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy());
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetPipActionEvent end";
}

/**
 * @tc.name: SetPiPControlEvent
 * @tc.desc: SetPiPControlEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetPiPControlEvent, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetPiPControlEvent start";
    auto option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetTitleButtonArea");
    auto window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(window, nullptr);
    auto controlType = WsPiPControlType::VIDEO_PLAY_PAUSE;
    auto status = WsPiPControlStatus::PLAY;
    WSError res = window->SetPiPControlEvent(controlType, status);
    ASSERT_EQ(res, WSError::WS_OK);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetPiPControlEvent end";
}

/**
 * @tc.name: SetUIContentInner
 * @tc.desc: SetUIContentInner Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetUIContentInner, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetUIContentInner start";
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetUIContentInner");
    option->SetIsUIExtFirstSubWindow(true);
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetPersistentId(1);
    std::string url = "";
    EXPECT_TRUE(window->IsWindowSessionInvalid());
    WMError res1 = window->SetUIContentInner(url, nullptr, nullptr, WindowSetUIContentType::DEFAULT,
        BackupAndRestoreType::NONE, nullptr);
    ASSERT_EQ(res1, WMError::WM_ERROR_INVALID_WINDOW);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetUIContentInner end";
}

/**
 * @tc.name: TestGetUIContentWithId
 * @tc.desc: Get uicontent with id
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, TestGetUIContentWithId, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: TestGetUIContentWithId start";
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("TestGetUIContentWithId");
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(nullptr, window->GetUIContentWithId(10000));
    window->property_->SetPersistentId(1);

    SessionInfo sessionInfo = { "CreateTestBundle", "TestGetUIContentWithId", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(window->FindWindowById(1), nullptr);
        ASSERT_EQ(nullptr, window->GetUIContentWithId(1));
        ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy());
    }
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: TestGetUIContentWithId end";
}

/**
 * @tc.name: GetCallingWindowRect
 * @tc.desc: GetCallingWindowRect Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, GetCallingWindowRect, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("GetCallingWindowRect");
    sptr<WindowSessionImpl> window = new(std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    Rect rect = {0, 0, 0, 0};
    WMError retCode = window->GetCallingWindowRect(rect);
    ASSERT_EQ(retCode, WMError::WM_ERROR_INVALID_WINDOW);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_CREATED;
    window->GetCallingWindowRect(rect);
}


/**
 * @tc.name: EnableDrag
 * @tc.desc: EnableDrag Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, EnableDrag, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("EnableDrag");
    sptr<WindowSessionImpl> windowSession = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, windowSession);
    windowSession->property_->SetPersistentId(1);
    windowSession->property_->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    windowSession->hostSession_ = session;
    windowSession->EnableDrag(true);
}

/**
 * @tc.name: GetCallingWindowWindowStatus
 * @tc.desc: GetCallingWindowWindowStatus Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, GetCallingWindowWindowStatus, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("GetCallingWindowWindowStatus");
    sptr<WindowSessionImpl> window = new(std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    WindowStatus windowStatus = WindowStatus::WINDOW_STATUS_UNDEFINED;
    WMError retCode = window->GetCallingWindowWindowStatus(windowStatus);
    ASSERT_EQ(retCode, WMError::WM_ERROR_INVALID_WINDOW);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_CREATED;
    window->GetCallingWindowWindowStatus(windowStatus);
}

/**
 * @tc.name: GetParentId
 * @tc.desc: GetParentId Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, GetParentId, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);
    const int32_t res = window->GetParentId();
    ASSERT_EQ(res, 0);
    ASSERT_EQ(true, window->IsSupportWideGamut());
}

/**
 * @tc.name: PreNotifyKeyEvent
 * @tc.desc: PreNotifyKeyEvent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, PreNotifyKeyEvent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    std::shared_ptr<MMI::PointerEvent> pointerEvent;
    window->ConsumePointerEvent(pointerEvent);

    std::shared_ptr<MMI::KeyEvent> keyEvent;
    window->ConsumeKeyEvent(keyEvent);
    ASSERT_EQ(nullptr, window->GetUIContentSharedPtr());
    ASSERT_EQ(false, window->PreNotifyKeyEvent(keyEvent));
    ASSERT_EQ(false, window->NotifyOnKeyPreImeEvent(keyEvent));
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(nullptr, window->GetUIContentSharedPtr());
    ASSERT_EQ(false, window->PreNotifyKeyEvent(keyEvent));
    ASSERT_EQ(false, window->NotifyOnKeyPreImeEvent(keyEvent));
}

/**
 * @tc.name: CheckIfNeedCommitRsTransaction
 * @tc.desc: CheckIfNeedCommitRsTransaction
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, CheckIfNeedCommitRsTransaction, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);

    bool res = false;
    WindowSizeChangeReason wmReason = WindowSizeChangeReason::UNDEFINED;
    for (uint32_t i = static_cast<uint32_t>(WindowSizeChangeReason::UNDEFINED);
         i < static_cast<uint32_t>(WindowSizeChangeReason::END); i++) {
        wmReason = static_cast<WindowSizeChangeReason>(i);
        res = window->CheckIfNeedCommitRsTransaction(wmReason);
        if (wmReason == WindowSizeChangeReason::FULL_TO_SPLIT ||
            wmReason == WindowSizeChangeReason::FULL_TO_FLOATING || wmReason == WindowSizeChangeReason::RECOVER ||
            wmReason == WindowSizeChangeReason::MAXIMIZE) {
            ASSERT_EQ(res, false);
        } else {
            ASSERT_EQ(res, true);
        }
    }
    window->Destroy();
}

/**
 * @tc.name: UpdateRectForRotation
 * @tc.desc: UpdateRectForRotation Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, UpdateRectForRotation, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowSessionImpl> window =
        new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    Rect wmRect;
    wmRect.posX_ = 0;
    wmRect.posY_ = 0;
    wmRect.height_ = 50;
    wmRect.width_ = 50;

    WSRect rect;
    wmRect.posX_ = 0;
    wmRect.posY_ = 0;
    wmRect.height_ = 50;
    wmRect.width_ = 50;

    Rect preRect;
    preRect.posX_ = 0;
    preRect.posY_ = 0;
    preRect.height_ = 200;
    preRect.width_ = 200;

    window->property_->SetWindowRect(preRect);
    WindowSizeChangeReason wmReason = WindowSizeChangeReason{0};
    std::shared_ptr<RSTransaction> rsTransaction;
    SceneAnimationConfig config { .rsTransaction_ = rsTransaction };
    window->UpdateRectForRotation(wmRect, preRect, wmReason, config);

    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    auto res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);
}

/**
 * @tc.name: NotifyRotationAnimationEnd
 * @tc.desc: NotifyRotationAnimationEnd Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, NotifyRotationAnimationEnd, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->NotifyRotationAnimationEnd();

    OHOS::Ace::UIContentErrorCode aceRet = OHOS::Ace::UIContentErrorCode::NO_ERRORS;
    window->InitUIContent("", nullptr, nullptr, WindowSetUIContentType::BY_ABC, BackupAndRestoreType::NONE,
                          nullptr, aceRet);
    window->NotifyRotationAnimationEnd();
    ASSERT_NE(nullptr, window->uiContent_);
}

/**
 * @tc.name: SetTitleButtonVisible
 * @tc.desc: SetTitleButtonVisible and GetTitleButtonVisible
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetTitleButtonVisible, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    bool isMaximizeVisible = true;
    bool isMinimizeVisible = true;
    bool isSplitVisible = true;
    bool isCloseVisible = true;
    auto res = window->SetTitleButtonVisible(isMaximizeVisible, isMinimizeVisible,
        isSplitVisible, isCloseVisible);

    bool &hideMaximizeButton = isMaximizeVisible;
    bool &hideMinimizeButton = isMinimizeVisible;
    bool &hideSplitButton = isSplitVisible;
    bool &hideCloseButton = isCloseVisible;
    window->GetTitleButtonVisible(true, hideMaximizeButton, hideMinimizeButton,
        hideSplitButton, hideCloseButton);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: IsFocused
 * @tc.desc: IsFocused
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, IsFocused, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowSessionImpl> window =
        new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    bool res = window->IsFocused();
    ASSERT_EQ(res, false);

    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->RequestFocus());

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule",
                               "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));
    int32_t persistentId = window->GetPersistentId();
    if (persistentId == INVALID_SESSION_ID) {
        persistentId = 1;
        window->property_->SetPersistentId(persistentId);
    }
    if (window->state_ == WindowState::STATE_DESTROYED) {
        window->state_ = WindowState::STATE_SHOWN;
    }
    window->hostSession_ = session;
    window->RequestFocus();
    ASSERT_FALSE(window->IsWindowSessionInvalid());
    ASSERT_EQ(persistentId, window->GetPersistentId());
    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: NapiSetUIContent
 * @tc.desc: NapiSetUIContent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, NapiSetUIContent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("NapiSetUIContent");
    option->SetIsUIExtFirstSubWindow(true);
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetPersistentId(1);
    std::string url = "";
    AppExecFwk::Ability* ability = nullptr;

    window->SetUIContentByName(url, nullptr, nullptr, nullptr);
    window->SetUIContentByAbc(url, nullptr, nullptr, nullptr);
    WMError res1 = window->NapiSetUIContent(url, nullptr, nullptr, BackupAndRestoreType::CONTINUATION,
        nullptr, ability);
    ASSERT_EQ(res1, WMError::WM_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: GetAbcContent
 * @tc.desc: GetAbcContent Test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, GetAbcContent, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetAbcContent");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    std::string abcPath = "";
    std::shared_ptr<std::vector<uint8_t>> res = window->GetAbcContent(abcPath);
    std::filesystem::path abcFile{abcPath};
    ASSERT_TRUE(abcFile.empty());
    ASSERT_TRUE(!abcFile.is_absolute());
    ASSERT_TRUE(!std::filesystem::exists(abcFile));
    ASSERT_EQ(res, nullptr);

    abcPath = "/abc";
    res = window->GetAbcContent(abcPath);
    std::filesystem::path abcFile2{abcPath};
    ASSERT_FALSE(abcFile2.empty());
    ASSERT_FALSE(!abcFile2.is_absolute());
    ASSERT_TRUE(!std::filesystem::exists(abcFile2));
    ASSERT_EQ(res, nullptr);

    abcPath = "abc";
    res = window->GetAbcContent(abcPath);
    std::filesystem::path abcFile3{abcPath};
    ASSERT_FALSE(abcFile3.empty());
    ASSERT_TRUE(!abcFile3.is_absolute());
    ASSERT_TRUE(!std::filesystem::exists(abcFile3));
    ASSERT_EQ(res, nullptr);

    abcPath = "/log";
    res = window->GetAbcContent(abcPath);
    std::filesystem::path abcFile4{abcPath};
    ASSERT_FALSE(abcFile4.empty());
    ASSERT_FALSE(!abcFile4.is_absolute());
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_FALSE(!std::filesystem::exists(abcFile4));
        ASSERT_NE(res, nullptr);
        std::fstream file(abcFile, std::ios::in | std::ios::binary);
        ASSERT_FALSE(file);
    }
    window->Destroy();
}

/**
 * @tc.name: SetLandscapeMultiWindow
 * @tc.desc: SetLandscapeMultiWindow and check the retCode
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetLandscapeMultiWindow, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetLandscapeMultiWindow");
    sptr<WindowSessionImpl> window = new(std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, window);
    WMError retCode = window->SetLandscapeMultiWindow(false);
    ASSERT_EQ(retCode, WMError::WM_ERROR_INVALID_WINDOW);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_CREATED;
    retCode = window->SetLandscapeMultiWindow(false);
    ASSERT_EQ(retCode, WMError::WM_OK);
}

/**
 * @tc.name: GetTouchable
 * @tc.desc: GetTouchable
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, GetTouchable, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetTouchable");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->GetTouchable();
    window->GetBrightness();
    ASSERT_NE(window, nullptr);
}

/**
 * @tc.name: Notify03
 * @tc.desc: NotifyCloseExistPipWindow NotifyAfterResumed NotifyAfterPaused
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, Notify03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("Notify03");
    sptr<WindowSessionImpl> window = new WindowSessionImpl(option);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule",
                               "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, window->Create(nullptr, session));

    window->NotifyAfterResumed();
    window->NotifyAfterPaused();
    WSError res = window->NotifyCloseExistPipWindow();
    ASSERT_EQ(res, WSError::WS_OK);
    AAFwk::WantParams wantParams;
    WSError ret = window->NotifyTransferComponentData(wantParams);
    ASSERT_EQ(ret, WSError::WS_OK);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy());
}

/**
 * @tc.name: Filter
 * @tc.desc: Filter
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, Filter, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("Filter");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
    window->FilterKeyEvent(keyEvent);
    ASSERT_EQ(window->keyEventFilter_, nullptr);
    window->SetKeyEventFilter([](MMI::KeyEvent& keyEvent) {
        GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetKeyEventFilter";
        return true;
    });
    ASSERT_NE(window->keyEventFilter_, nullptr);
    window->FilterKeyEvent(keyEvent);
    auto ret = window->ClearKeyEventFilter();
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: UpdateOrientation
 * @tc.desc: UpdateOrientation
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, UpdateOrientation, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("UpdateOrientation");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    auto ret = window->UpdateOrientation();
    ASSERT_EQ(WSError::WS_OK, ret);
}

/**
 * @tc.name: SetTitleButtonVisible01
 * @tc.desc: SetTitleButtonVisible
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest4, SetTitleButtonVisible01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetTitleButtonVisible01 start";
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetTitleButtonVisible");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(1);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    WMError res = window->SetTitleButtonVisible(false, false, false, true);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_CALLING);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetTitleButtonVisible01 end";
}

/**
 * @tc.name: SetTitleButtonVisible02
 * @tc.desc: SetTitleButtonVisible
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest4, SetTitleButtonVisible02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetTitleButtonVisible02 start";
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetTitleButtonVisible");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    WMError res = window->SetTitleButtonVisible(false, false, false, true);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetTitleButtonVisible02 end";
}

/**
 * @tc.name: SetTitleButtonVisible03
 * @tc.desc: SetTitleButtonVisible
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest4, SetTitleButtonVisible03, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetTitleButtonVisible03 start";
    sptr option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetTitleButtonVisible");
    sptr window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window->windowSystemConfig_.isSystemDecorEnable_ = true;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    WMError res = window->SetTitleButtonVisible(false, false, false, true);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    res = window->SetTitleButtonVisible(false, false, false, true);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetTitleButtonVisible03 end";
}

/**
 * @tc.name: GetTitleButtonVisible01
 * @tc.desc: GetTitleButtonVisible
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest4, GetTitleButtonVisible01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetTitleButtonVisible01");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_NE(window->property_, nullptr);
    uint32_t modeSupportInfo = 1 | (1 << 1) | (1 << 2);
    window->property_->SetModeSupportInfo(modeSupportInfo);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    // show Maximize, Minimize, Split buttons.
    window->windowTitleVisibleFlags_ = { false, false, false, false };
    bool hideMaximizeButton = false;
    bool hideMinimizeButton = false;
    bool hideSplitButton = false;
    bool hideCloseButton = false;
    window->GetTitleButtonVisible(true, hideMaximizeButton, hideMinimizeButton, hideSplitButton,
        hideCloseButton);
    ASSERT_EQ(hideMaximizeButton, true);
    ASSERT_EQ(hideMinimizeButton, true);
    ASSERT_EQ(hideSplitButton, true);
    ASSERT_EQ(hideCloseButton, true);
}

/**
 * @tc.name: UpdateRect03
 * @tc.desc: UpdateRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, UpdateRect03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowSessionCreateCheck");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);

    WSRect rect;
    rect.posX_ = 0;
    rect.posY_ = 0;
    rect.height_ = 0;
    rect.width_ = 0;

    Rect rectW; // GetRect().IsUninitializedRect is true
    rectW.posX_ = 0;
    rectW.posY_ = 0;
    rectW.height_ = 0; // rectW - rect > 50
    rectW.width_ = 0;  // rectW - rect > 50

    window->property_->SetWindowRect(rectW);
    SizeChangeReason reason = SizeChangeReason::UNDEFINED;
    WSError res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);

    rect.height_ = 50;
    rect.width_ = 50;
    rectW.height_ = 50;
    rectW.width_ = 50;
    window->property_->SetWindowRect(rectW);
    res = window->UpdateRect(rect, reason);
    ASSERT_EQ(res, WSError::WS_OK);
}

/**
 * @tc.name: GetTitleButtonVisible02
 * @tc.desc: GetTitleButtonVisible
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest4, GetTitleButtonVisible02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetTitleButtonVisible02");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_NE(window->property_, nullptr);
    // only not support WINDOW_MODE_SUPPORT_SPLIT
    uint32_t modeSupportInfo = 1 | (1 << 1);
    window->property_->SetModeSupportInfo(modeSupportInfo);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    // show Maximize, Minimize, Split buttons.
    window->windowTitleVisibleFlags_ = { true, true, true, true };
    bool hideMaximizeButton = false;
    bool hideMinimizeButton = false;
    bool hideSplitButton = false;
    bool hideCloseButton = false;
    window->GetTitleButtonVisible(true, hideMaximizeButton, hideMinimizeButton, hideSplitButton, hideCloseButton);
    ASSERT_EQ(hideMaximizeButton, false);
    ASSERT_EQ(hideMinimizeButton, false);
    ASSERT_EQ(hideSplitButton, false);
    ASSERT_EQ(hideCloseButton, false);
}

/**
 * @tc.name: GetTitleButtonVisible03
 * @tc.desc: GetTitleButtonVisible
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest4, GetTitleButtonVisible03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetTitleButtonVisible03");
    option->SetDisplayId(1);
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_NE(window->property_, nullptr);
    ASSERT_EQ(1, window->GetDisplayId());
    // only not support WINDOW_MODE_SUPPORT_SPLIT
    uint32_t modeSupportInfo = 1 | (1 << 1) | (1 << 2);
    window->property_->SetModeSupportInfo(modeSupportInfo);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    // show Maximize, Minimize, Split buttons.
    window->windowTitleVisibleFlags_ = { false, false, false, false };
    bool hideMaximizeButton = true;
    bool hideMinimizeButton = true;
    bool hideSplitButton = true;
    bool hideCloseButton = true;
    window->GetTitleButtonVisible(false, hideMaximizeButton, hideMinimizeButton, hideSplitButton, hideCloseButton);
    ASSERT_EQ(hideMaximizeButton, true);
    ASSERT_EQ(hideMinimizeButton, true);
    ASSERT_EQ(hideSplitButton, true);
    ASSERT_EQ(hideCloseButton, true);
}

/**
 * @tc.name: GetAppForceLandscapeConfig01
 * @tc.desc: GetAppForceLandscapeConfig
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest4, GetAppForceLandscapeConfig01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("GetAppForceLandscapeConfig01");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    AppForceLandscapeConfig config = {};
    window->GetAppForceLandscapeConfig(config);
    window->hostSession_ = nullptr;
    WMError res = window->GetAppForceLandscapeConfig(config);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: UpdatePiPControlStatus01
 * @tc.desc: UpdatePiPControlStatus
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest4, UpdatePiPControlStatus01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("UpdatePiPControlStatus01");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    auto controlType = PiPControlType::VIDEO_PLAY_PAUSE;
    auto status = PiPControlStatus::ENABLED;
    window->UpdatePiPControlStatus(controlType, status);
    window->hostSession_ = nullptr;
    window->UpdatePiPControlStatus(controlType, status);
}

/**
 * @tc.name: SetAutoStartPiP
 * @tc.desc: SetAutoStartPiP
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest4, SetAutoStartPiP, Function | SmallTest | Level2)
{
    auto option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetAutoStartPiP");
    auto window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "SetAutoStartPiP", "SetAutoStartPiP", "SetAutoStartPiP" };
    auto session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    bool isAutoStart = true;
    window->SetAutoStartPiP(isAutoStart);
    window->hostSession_ = nullptr;
    window->SetAutoStartPiP(isAutoStart);
}

/**
 * @tc.name: NotifyWindowVisibility01
 * @tc.desc: NotifyWindowVisibility
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest4, NotifyWindowVisibility01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("NotifyWindowVisibility01");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new(std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->NotifyWindowVisibility(false);
    sptr<IWindowVisibilityChangedListener> listener = new IWindowVisibilityChangedListener();
    window->RegisterWindowVisibilityChangeListener(listener);
    window->NotifyWindowVisibility(false);
    window->UnregisterWindowVisibilityChangeListener(listener);
}

/**
 * @tc.name: NotifyMainWindowClose01
 * @tc.desc: NotifyMainWindowClose
 * @tc.type: FUNC
*/
HWTEST_F(WindowSessionImplTest4, NotifyMainWindowClose01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("NotifyMainWindowClose01");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(window, nullptr);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(1);

    bool terminateCloseProcess = false;
    WMError res = window->NotifyMainWindowClose(terminateCloseProcess);
    EXPECT_EQ(terminateCloseProcess, false);
    EXPECT_EQ(res, WMError::WM_ERROR_NULLPTR);
    sptr<IMainWindowCloseListener> listener = sptr<IMainWindowCloseListener>::MakeSptr();
    window->RegisterMainWindowCloseListeners(listener);
    res = window->NotifyMainWindowClose(terminateCloseProcess);
    EXPECT_EQ(terminateCloseProcess, false);
    EXPECT_EQ(res, WMError::WM_ERROR_NULLPTR);
    window->UnregisterMainWindowCloseListeners(listener);
}

/**
 * @tc.name: SetWindowContainerColor01
 * @tc.desc: SetWindowContainerColor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetWindowContainerColor01, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetWindowContainerColor01 start";
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetWindowContainerColor");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    std::string activeColor = "#00000000";
    std::string inactiveColor = "#00000000";
    WMError res = window->SetWindowContainerColor(activeColor, inactiveColor);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_CALLING);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetWindowContainerColor01 end";
}

/**
 * @tc.name: SetWindowContainerColor02
 * @tc.desc: SetWindowContainerColor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetWindowContainerColor02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetWindowContainerColor02 start";
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetWindowContainerColor");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    std::string activeColor = "#00000000";
    std::string inactiveColor = "#00000000";
    WMError res = window->SetWindowContainerColor(activeColor, inactiveColor);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetWindowContainerColor02 end";
}

/**
 * @tc.name: SetWindowContainerColor03
 * @tc.desc: SetWindowContainerColor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetWindowContainerColor03, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetWindowContainerColor03 start";
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetWindowContainerColor");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window->windowSystemConfig_.isSystemDecorEnable_ = true;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    std::string activeColor = "#00000000";
    std::string inactiveColor = "#00000000";
    WMError res = window->SetWindowContainerColor(activeColor, inactiveColor);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetWindowContainerColor03 end";
}

/**
 * @tc.name: SetWindowContainerColor04
 * @tc.desc: SetWindowContainerColor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, SetWindowContainerColor04, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetWindowContainerColor04 start";
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("SetWindowContainerColor");
    sptr<WindowSessionImpl> window = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(window, nullptr);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    window->windowSystemConfig_.freeMultiWindowSupport_ = true;
    window->windowSystemConfig_.isSystemDecorEnable_ = true;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    std::string activeColor = "color";
    std::string inactiveColor = "123";
    WMError res = window->SetWindowContainerColor(activeColor, inactiveColor);
    ASSERT_EQ(res, WMError::WM_ERROR_INVALID_WINDOW);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: SetWindowContainerColor04 end";
}

/**
 * @tc.name: IsPcOrPadCapabilityEnabled
 * @tc.desc: IsPcOrPadCapabilityEnabled test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, IsPcOrPadCapabilityEnabled, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: IsPcOrPadCapabilityEnabled start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("IsPcOrPadCapabilityEnabled");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(1);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    EXPECT_EQ(true, window->IsPcOrPadCapabilityEnabled());
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PHONE_WINDOW;
    EXPECT_EQ(false, window->IsPcOrPadCapabilityEnabled());
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PAD_WINDOW;
    EXPECT_EQ(false, window->IsPcOrPadCapabilityEnabled());
    window->property_->SetIsPcAppInPad(true);
    EXPECT_EQ(true, window->IsPcOrPadCapabilityEnabled());
    EXPECT_EQ(WMError::WM_OK, window->Destroy(true));
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: IsPcOrPadCapabilityEnabled end";
}

/**
 * @tc.name: DestroySubWindow
 * @tc.desc: DestroySubWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, DestroySubWindow, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: DestroySubWindow start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("DestroySubWindow");
    option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;

    sptr<WindowOption> subOption = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(subOption, nullptr);
    subOption->SetWindowName("DestroySubWindow01");
    subOption->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sptr<WindowSessionImpl> subWindow = sptr<WindowSessionImpl>::MakeSptr(subOption);
    ASSERT_NE(subWindow, nullptr);
    ASSERT_NE(subWindow->property_, nullptr);
    subWindow->property_->SetPersistentId(2);
    SessionInfo subSessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> subSession = sptr<SessionMocker>::MakeSptr(subSessionInfo);
    ASSERT_NE(nullptr, subSession);
    subWindow->hostSession_ = subSession;
    subWindow->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    std::vector<sptr<WindowSessionImpl>> vec;
    WindowSessionImpl::subWindowSessionMap_.insert(std::pair<int32_t,
        std::vector<sptr<WindowSessionImpl>>>(1, vec));
    WindowSessionImpl::subWindowSessionMap_[1].push_back(subWindow);
    window->DestroySubWindow();
    EXPECT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: UpdateSubWindowStateAndNotify01
 * @tc.desc: UpdateSubWindowStateAndNotify
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, UpdateSubWindowStateAndNotify01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("UpdateSubWindowStateAndNotify01");
    option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(window->property_, nullptr);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;

    sptr<WindowOption> subOption = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(subOption, nullptr);
    subOption->SetWindowName("UpdateSubWindowStateAndNotify011");
    subOption->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sptr<WindowSessionImpl> subWindow = sptr<WindowSessionImpl>::MakeSptr(subOption);
    ASSERT_NE(subWindow, nullptr);
    ASSERT_NE(subWindow->property_, nullptr);
    subWindow->property_->SetPersistentId(2);
    SessionInfo subSessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> subSession = sptr<SessionMocker>::MakeSptr(subSessionInfo);
    ASSERT_NE(nullptr, subSession);
    subWindow->hostSession_ = subSession;
    subWindow->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    std::vector<sptr<WindowSessionImpl>> vec;
    WindowSessionImpl::subWindowSessionMap_.insert(std::pair<int32_t,
        std::vector<sptr<WindowSessionImpl>>>(1, vec));
    subWindow->UpdateSubWindowStateAndNotify(1, WindowState::STATE_HIDDEN);
    WindowSessionImpl::subWindowSessionMap_[1].push_back(subWindow);
    subWindow->state_ = WindowState::STATE_SHOWN;
    window->UpdateSubWindowStateAndNotify(1, WindowState::STATE_HIDDEN);
    window->state_ = WindowState::STATE_HIDDEN;
    window->UpdateSubWindowStateAndNotify(1, WindowState::STATE_HIDDEN);
    window->state_ = WindowState::STATE_SHOWN;
    window->UpdateSubWindowStateAndNotify(1, WindowState::STATE_SHOWN);
    window->state_ = WindowState::STATE_SHOWN;
    window->UpdateSubWindowStateAndNotify(1, WindowState::STATE_SHOWN);
    EXPECT_EQ(WMError::WM_OK, subWindow->Destroy(true));
    EXPECT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: GetVirtualPixelRatio
 * @tc.desc: test GetVirtualPixelRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, GetVirtualPixelRatio, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetVirtualPixelRatio start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetVirtualPixelRatio");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    float vpr = -1.0f;
    window->property_->SetDisplayId(-1);
    vpr = window->GetVirtualPixelRatio();
    ASSERT_EQ(vpr, 0.0f);

    window->property_->SetDisplayId(0);
    vpr = window->GetVirtualPixelRatio();
    ASSERT_NE(vpr, 0.0f);
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetVirtualPixelRatio end";
}

/**
 * @tc.name: NotifyRotationAnimationEnd
 * @tc.desc: test NotifyRotationAnimationEnd
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, NotifyRotationAnimationEnd001, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    ASSERT_NE(option, nullptr);
    option->SetWindowName("NotifyRotationAnimationEnd001");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(window, nullptr);

    ASSERT_NE(window->handler_, nullptr);
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    ASSERT_NE(window->uiContent_, nullptr);
    window->NotifyRotationAnimationEnd();

    window->uiContent_ = nullptr;
    ASSERT_EQ(window->uiContent_, nullptr);
    window->NotifyRotationAnimationEnd();

    window->handler_ = nullptr;
    ASSERT_EQ(window->handler_, nullptr);
    window->NotifyRotationAnimationEnd();
}

/**
 * @tc.name: GetSubWindow
 * @tc.desc: test GetSubWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, GetSubWindow, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetSubWindow start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetSubWindow");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    window->property_->SetParentId(101); // this subWindow's parentId is 101
    std::vector<sptr<Window> > subWindows = window->GetSubWindow(101); // 101 is parentId
    ASSERT_EQ(0, subWindows.size());
    WindowSessionImpl::subWindowSessionMap_.insert(std::pair<int32_t,
        std::vector<sptr<WindowSessionImpl> > >(101, { window }));
    subWindows = window->GetSubWindow(101); // 101 is parentId
    ASSERT_EQ(1, subWindows.size());
    WindowSessionImpl::subWindowSessionMap_.erase(101); // 101
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: GetVirtualPixelRatio end";
}

/**
 * @tc.name: ClearListenersById_displayMoveListeners
 * @tc.desc: ClearListenersById_displayMoveListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_displayMoveListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_displayMoveListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_displayMoveListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IDisplayMoveListener> listener_ = new (std::nothrow) MockIDisplayMoveListener();
    window_->RegisterDisplayMoveListener(listener_);
    ASSERT_NE(window_->displayMoveListeners_.find(persistentId), window_->displayMoveListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->displayMoveListeners_.find(persistentId), window_->displayMoveListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_displayMoveListeners end";
}

/**
 * @tc.name: ClearListenersById_lifecycleListeners
 * @tc.desc: ClearListenersById_lifecycleListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_lifecycleListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_lifecycleListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_lifecycleListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IWindowLifeCycle> listener_ = new (std::nothrow) MockWindowLifeCycleListener();
    window_->RegisterLifeCycleListener(listener_);
    ASSERT_NE(window_->lifecycleListeners_.find(persistentId), window_->lifecycleListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->lifecycleListeners_.find(persistentId), window_->lifecycleListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_lifecycleListeners end";
}

/**
 * @tc.name: ClearListenersById_windowChangeListeners
 * @tc.desc: ClearListenersById_windowChangeListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_windowChangeListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_windowChangeListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_windowChangeListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IWindowChangeListener> listener_ = new (std::nothrow) MockWindowChangeListener();
    window_->RegisterWindowChangeListener(listener_);
    ASSERT_NE(window_->windowChangeListeners_.find(persistentId), window_->windowChangeListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->windowChangeListeners_.find(persistentId), window_->windowChangeListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_windowChangeListeners end";
}

/**
 * @tc.name: ClearListenersById_avoidAreaChangeListeners
 * @tc.desc: ClearListenersById_avoidAreaChangeListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_avoidAreaChangeListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_avoidAreaChangeListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_avoidAreaChangeListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IAvoidAreaChangedListener> listener_ = new (std::nothrow) MockAvoidAreaChangedListener();
    window_->RegisterExtensionAvoidAreaChangeListener(listener_);
    ASSERT_NE(window_->avoidAreaChangeListeners_.find(persistentId), window_->avoidAreaChangeListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->avoidAreaChangeListeners_.find(persistentId), window_->avoidAreaChangeListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_avoidAreaChangeListeners end";
}

/**
 * @tc.name: ClearListenersById_dialogDeathRecipientListeners
 * @tc.desc: ClearListenersById_dialogDeathRecipientListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_dialogDeathRecipientListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_dialogDeathRecipientListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_dialogDeathRecipientListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IDialogDeathRecipientListener> listener_ = new (std::nothrow) MockIDialogDeathRecipientListener();
    window_->RegisterDialogDeathRecipientListener(listener_);
    ASSERT_NE(window_->dialogDeathRecipientListeners_.find(persistentId),
              window_->dialogDeathRecipientListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->dialogDeathRecipientListeners_.find(persistentId),
              window_->dialogDeathRecipientListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_dialogDeathRecipientListeners end";
}

/**
 * @tc.name: ClearListenersById_dialogTargetTouchListener
 * @tc.desc: ClearListenersById_dialogTargetTouchListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_dialogTargetTouchListener, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_dialogTargetTouchListener start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_dialogTargetTouchListener");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IDialogTargetTouchListener> listener_ = new (std::nothrow) MockIDialogTargetTouchListener();
    window_->RegisterDialogTargetTouchListener(listener_);
    ASSERT_NE(window_->dialogTargetTouchListener_.find(persistentId), window_->dialogTargetTouchListener_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->dialogTargetTouchListener_.find(persistentId), window_->dialogTargetTouchListener_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_dialogTargetTouchListener end";
}

/**
 * @tc.name: ClearListenersById_screenshotListeners
 * @tc.desc: ClearListenersById_screenshotListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_screenshotListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_screenshotListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_screenshotListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IScreenshotListener> listener_ = new (std::nothrow) MockIScreenshotListener();
    window_->RegisterScreenshotListener(listener_);
    ASSERT_NE(window_->screenshotListeners_.find(persistentId), window_->screenshotListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->screenshotListeners_.find(persistentId), window_->screenshotListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_screenshotListeners end";
}

/**
 * @tc.name: ClearListenersById_windowStatusChangeListeners
 * @tc.desc: ClearListenersById_windowStatusChangeListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_windowStatusChangeListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_windowStatusChangeListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_windowStatusChangeListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IWindowStatusChangeListener> listener_ = new (std::nothrow) MockWindowStatusChangeListener();
    window_->RegisterWindowStatusChangeListener(listener_);
    ASSERT_NE(window_->windowStatusChangeListeners_.find(persistentId), window_->windowStatusChangeListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->windowStatusChangeListeners_.find(persistentId), window_->windowStatusChangeListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_windowStatusChangeListeners end";
}

/**
 * @tc.name: ClearListenersById_windowTitleButtonRectChangeListeners
 * @tc.desc: ClearListenersById_windowTitleButtonRectChangeListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_windowTitleButtonRectChangeListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_windowTitleButtonRectChangeListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_windowTitleButtonRectChangeListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IWindowTitleButtonRectChangedListener> listener_ =
        new (std::nothrow) MockWindowTitleButtonRectChangedListener();
    window_->windowTitleButtonRectChangeListeners_[persistentId].emplace_back(listener_);
    ASSERT_NE(window_->windowTitleButtonRectChangeListeners_.find(persistentId),
              window_->windowTitleButtonRectChangeListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->windowTitleButtonRectChangeListeners_.find(persistentId),
              window_->windowTitleButtonRectChangeListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_windowTitleButtonRectChangeListeners end";
}

/**
 * @tc.name: ClearListenersById_windowNoInteractionListeners
 * @tc.desc: ClearListenersById_windowNoInteractionListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_windowNoInteractionListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_windowNoInteractionListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_windowNoInteractionListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IWindowNoInteractionListener> listener_ = new (std::nothrow) MockWindowNoInteractionListener();
    window_->RegisterWindowNoInteractionListener(listener_);
    ASSERT_NE(window_->windowNoInteractionListeners_.find(persistentId), window_->windowNoInteractionListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->windowNoInteractionListeners_.find(persistentId), window_->windowNoInteractionListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_windowNoInteractionListeners end";
}

/**
 * @tc.name: ClearListenersById_windowRectChangeListeners
 * @tc.desc: ClearListenersById_windowRectChangeListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_windowRectChangeListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_windowRectChangeListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_windowRectChangeListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IWindowRectChangeListener> listener_ = new (std::nothrow) MockWindowRectChangeListener();
    window_->RegisterWindowRectChangeListener(listener_);
    ASSERT_NE(window_->windowRectChangeListeners_.find(persistentId), window_->windowRectChangeListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->windowRectChangeListeners_.find(persistentId), window_->windowRectChangeListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_windowRectChangeListeners end";
}

/**
 * @tc.name: ClearListenersById_subWindowCloseListeners
 * @tc.desc: ClearListenersById_subWindowCloseListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_subWindowCloseListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_subWindowCloseListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_subWindowCloseListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<ISubWindowCloseListener> listener_ = new (std::nothrow) MockISubWindowCloseListener();
    window_->subWindowCloseListeners_[persistentId] = listener_;
    ASSERT_NE(window_->subWindowCloseListeners_.find(persistentId), window_->subWindowCloseListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->subWindowCloseListeners_.find(persistentId), window_->subWindowCloseListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_subWindowCloseListeners end";
}

/**
 * @tc.name: ClearListenersById_mainWindowCloseListeners
 * @tc.desc: ClearListenersById_mainWindowCloseListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_mainWindowCloseListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_mainWindowCloseListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_mainWindowCloseListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IMainWindowCloseListener> listener_ = new (std::nothrow) MockIMainWindowCloseListener();
    window_->mainWindowCloseListeners_[persistentId] = listener_;
    ASSERT_NE(window_->mainWindowCloseListeners_.find(persistentId), window_->mainWindowCloseListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->mainWindowCloseListeners_.find(persistentId), window_->mainWindowCloseListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_mainWindowCloseListeners end";
}

/**
 * @tc.name: ClearListenersById_occupiedAreaChangeListeners
 * @tc.desc: ClearListenersById_occupiedAreaChangeListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_occupiedAreaChangeListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_occupiedAreaChangeListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_occupiedAreaChangeListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<IOccupiedAreaChangeListener> listener_ = new (std::nothrow) MockIOccupiedAreaChangeListener();
    window_->RegisterOccupiedAreaChangeListener(listener_);
    ASSERT_NE(window_->occupiedAreaChangeListeners_.find(persistentId), window_->occupiedAreaChangeListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->occupiedAreaChangeListeners_.find(persistentId), window_->occupiedAreaChangeListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_occupiedAreaChangeListeners end";
}

/**
 * @tc.name: ClearListenersById_switchFreeMultiWindowListeners
 * @tc.desc: ClearListenersById_switchFreeMultiWindowListeners
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest4, ClearListenersById_switchFreeMultiWindowListeners, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_switchFreeMultiWindowListeners start";
    sptr<WindowOption> option_ = sptr<WindowOption>::MakeSptr();
    option_->SetWindowName("ClearListenersById_switchFreeMultiWindowListeners");
    sptr<WindowSessionImpl> window_ = sptr<WindowSessionImpl>::MakeSptr(option_);
    ASSERT_NE(window_, nullptr);

    int persistentId = window_->GetPersistentId();
    window_->ClearListenersById(persistentId);

    sptr<ISwitchFreeMultiWindowListener> listener_ = new (std::nothrow) MockISwitchFreeMultiWindowListener();
    window_->RegisterSwitchFreeMultiWindowListener(listener_);
    ASSERT_NE(window_->switchFreeMultiWindowListeners_.find(persistentId),
              window_->switchFreeMultiWindowListeners_.end());

    window_->ClearListenersById(persistentId);
    ASSERT_EQ(window_->switchFreeMultiWindowListeners_.find(persistentId),
              window_->switchFreeMultiWindowListeners_.end());

    GTEST_LOG_(INFO) << "WindowSessionImplTest4: ClearListenersById_switchFreeMultiWindowListeners end";
}
}
} // namespace Rosen
} // namespace OHOS
