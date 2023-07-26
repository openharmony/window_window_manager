/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include "ability_context_impl.h"
#include "mock_session.h"
#include "window_session_impl.h"
#include "mock_uicontent.h"
#include "window_scene_session_impl.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class MockWindowChangeListener : public IWindowChangeListener {
public:
    MOCK_METHOD3(OnSizeChange,
        void(Rect rect, WindowSizeChangeReason reason, const std::shared_ptr<RSTransaction> &rsTransaction));
};

class MockWindowLifeCycleListener : public IWindowLifeCycle {
public:
    MOCK_METHOD0(AfterForeground, void(void));
    MOCK_METHOD0(AfterBackground, void(void));
    MOCK_METHOD0(AfterFocused, void(void));
    MOCK_METHOD0(AfterUnfocused, void(void));
    MOCK_METHOD1(ForegroundFailed, void(int32_t));
    MOCK_METHOD0(AfterActive, void(void));
    MOCK_METHOD0(AfterInactive, void(void));
};

class WindowSceneSessionImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_;
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
};

void WindowSceneSessionImplTest::SetUpTestCase() {}

void WindowSceneSessionImplTest::TearDownTestCase() {}

void WindowSceneSessionImplTest::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowSceneSessionImplTest::TearDown()
{
    abilityContext_ = nullptr;
}

RSSurfaceNode::SharedPtr WindowSceneSessionImplTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "startingWindowTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    return surfaceNode;
}

namespace {
/**
 * @tc.name: CreateWindowAndDestroy01
 * @tc.desc: Create window and destroy window
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateWindowAndDestroy01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("CreateWindow01");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    EXPECT_CALL(*(session), Connect(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, window->Create(abilityContext_, session));
    window->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));

    EXPECT_CALL(*(session), Connect(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_ERROR_INVALID_SESSION));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_SESSION, window->Create(abilityContext_, session));
}

/**
 * @tc.name: CreateAndConnectSpecificSession01
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateAndConnectSpecificSession01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("CreateAndConnectSpecificSession01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    windowscenesession->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    if (windowscenesession->CreateAndConnectSpecificSession() == WMError::WM_ERROR_NULLPTR)
    {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowscenesession->CreateAndConnectSpecificSession());
    }
    windowscenesession->property_->SetPersistentId(102);
    windowscenesession->property_->SetParentPersistentId(100);
    windowscenesession->property_->SetParentId(100);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    EXPECT_CALL(*(session), Connect(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Create(abilityContext_, session));
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), CreateAndConnectSpecificSession(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
}

/**
 * @tc.name: CreateAndConnectSpecificSession02
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateAndConnectSpecificSession02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option_ = new (std::nothrow) WindowOption();
    option_->SetWindowTag(WindowTag::SUB_WINDOW);
    option_->SetWindowName("ChildWindow0002");
    sptr<WindowSceneSessionImpl> parentscenesession_ = new (std::nothrow) WindowSceneSessionImpl(option_);
    ASSERT_NE(nullptr, parentscenesession_);
    
    SessionInfo sessionInfo_ = { "CreateTestBundle0", "CreateTestModule0", "CreateTestAbility0" };
    sptr<SessionMocker> session_ = new (std::nothrow) SessionMocker(sessionInfo_);
    ASSERT_NE(nullptr, session_);
    EXPECT_CALL(*(session_), Connect(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, parentscenesession_->Create(abilityContext_, session_));

    parentscenesession_->property_->SetParentPersistentId(102);
    parentscenesession_->property_->SetParentId(102);
    parentscenesession_->property_->type_ = WindowType::APP_MAIN_WINDOW_BASE;
    parentscenesession_->hostSession_ = session_;

    EXPECT_CALL(*(session_), CreateAndConnectSpecificSession(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
    parentscenesession_->property_->type_ = WindowType::APP_SUB_WINDOW_BASE;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, parentscenesession_->CreateAndConnectSpecificSession());
}

/**
 * @tc.name: IsValidSystemWindowType01
 * @tc.desc: IsValidSystemWindowType
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, IsValidSystemWindowType01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_DIALOG));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_FLOAT));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_SCREENSHOT));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_VOICE_INTERACTION));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_POINTER));
    ASSERT_FALSE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_TOAST));
    ASSERT_TRUE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE));
    ASSERT_TRUE(!windowscenesession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_APP_LAUNCHING));
}

/*
 * @tc.name: InvalidWindow
 * @tc.desc: InvalidWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, InvalidWindow, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->MoveTo(0, 0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Resize(0, 0));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetBackgroundColor(std::string("???")));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetTransparent(false));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Show(2, false));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Resize(2, 2));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Minimize());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Maximize());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->MaximizeFloating());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Recover());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->MaximizeFloating());
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetGlobalMaximizeMode(MaximizeMode::MODE_AVOID_SYSTEM_BAR));
}

/**
 * @tc.name: FindParentSessionByParentId01
 * @tc.desc: FindParentSessionByParentId
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, FindParentSessionByParentId01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowTag(WindowTag::MAIN_WINDOW);
    option->SetWindowName("FindParentSessionByParentId01");
    sptr<WindowSceneSessionImpl> parentscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, parentscenesession);
    
    parentscenesession->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_TRUE(parentscenesession->FindMainWindowWithContext() == nullptr);
    parentscenesession->SetWindowType(WindowType::ABOVE_APP_SYSTEM_WINDOW_END);
    ASSERT_TRUE(parentscenesession->FindMainWindowWithContext() == nullptr);

    parentscenesession->property_->SetPersistentId(1112);
    parentscenesession->property_->SetParentId(1000);
    parentscenesession->property_->SetParentPersistentId(1000);
    parentscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    EXPECT_CALL(*(session), Connect(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, parentscenesession->Create(abilityContext_, session));
    parentscenesession->hostSession_ = session;
    ASSERT_TRUE(nullptr != parentscenesession->FindParentSessionByParentId(1112));
}

/**
 * @tc.name: FindMainWindowWithContext01
 * @tc.desc: FindMainWindowWithContext
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, FindMainWindowWithContext01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowTag(WindowTag::MAIN_WINDOW);
    option->SetWindowName("FindMainWindowWithContext01");
    sptr<WindowSceneSessionImpl> parentscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, parentscenesession);
    
    parentscenesession->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_TRUE(parentscenesession->FindMainWindowWithContext() == nullptr);
    parentscenesession->SetWindowType(WindowType::ABOVE_APP_SYSTEM_WINDOW_END);
    ASSERT_TRUE(parentscenesession->FindMainWindowWithContext() == nullptr);

    parentscenesession->property_->SetPersistentId(1002);
    parentscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    EXPECT_CALL(*(session), Connect(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, parentscenesession->Create(abilityContext_, session));
    parentscenesession->hostSession_ = session;
    ASSERT_TRUE(nullptr != parentscenesession->FindParentSessionByParentId(1002));
    
    sptr<WindowOption> option_ = new (std::nothrow) WindowOption();
    option_->SetWindowTag(WindowTag::MAIN_WINDOW);
    option_->SetWindowName("FindMainWindowWithContext02");
    sptr<WindowSceneSessionImpl> parentscenesession_ = new (std::nothrow) WindowSceneSessionImpl(option_);
    ASSERT_NE(nullptr, parentscenesession_);

    SessionInfo sessionInfo_ = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session_ = new (std::nothrow) SessionMocker(sessionInfo_);
    ASSERT_NE(nullptr, session_);
    EXPECT_CALL(*(session_), Connect(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, parentscenesession_->Create(abilityContext_, session_));

    parentscenesession_->hostSession_ = session_;
    parentscenesession_->property_->type_ = WindowType::WINDOW_TYPE_DIALOG;
    ASSERT_FALSE(parentscenesession_->FindMainWindowWithContext() == nullptr);
}

/**
 * @tc.name: DisableAppWindowDecor01
 * @tc.desc: DisableAppWindowDecor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, DisableAppWindowDecor01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSessionImpl> windowession = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, windowession);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);

    ASSERT_NE(nullptr, session);
    std::shared_ptr<AbilityRuntime::Context> context;
    ASSERT_EQ(WMError::WM_OK, windowession->Create(context, session));

    windowession->UpdateDecorEnable(false);
    windowession->windowSystemConfig_.isSystemDecorEnable_ = false;

    windowession->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    windowession->DisableAppWindowDecor();
    ASSERT_FALSE(windowession->IsDecorEnable());
    windowession->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    windowession->DisableAppWindowDecor();
}

/**
 * @tc.name: HandleBackEvent01
 * @tc.desc: HandleBackEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, HandleBackEvent01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    std::shared_ptr<AbilityRuntime::AbilityContext> context = std::make_shared<AbilityRuntime::AbilityContextImpl>();
    windowscenesession->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    Ace::UIContentMocker *content = reinterpret_cast<Ace::UIContentMocker *>(windowscenesession->uiContent_.get());
    EXPECT_CALL(*content, ProcessBackPressed()).WillOnce(Return(false));
    ASSERT_EQ(WSError::WS_OK, windowscenesession->HandleBackEvent());
}

/**
 * @tc.name: RaiseToAppTop01
 * @tc.desc: RaiseToAppTop
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, RaiseToAppTop01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    windowscenesession->property_->SetPersistentId(6);
    windowscenesession->property_->SetParentPersistentId(6);
    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(WmErrorCode::WM_ERROR_INVALID_CALLING, windowscenesession->RaiseToAppTop());

    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    windowscenesession->state_ = WindowState::STATE_HIDDEN;
    ASSERT_EQ(WmErrorCode::WM_ERROR_STATE_ABNORMALLY, windowscenesession->RaiseToAppTop());

    windowscenesession->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), RaiseToAppTop()).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WmErrorCode::WM_OK, windowscenesession->RaiseToAppTop());
}

/**
 * @tc.name: Resize01
 * @tc.desc: Resize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Resize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Resize01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    windowscenesession->property_->SetPersistentId(888);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    Rect rect = {2, 2, 2, 2};
    windowscenesession->property_->SetWindowRect(rect);
    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    windowscenesession->state_ = WindowState::STATE_FROZEN;
    windowscenesession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Resize(1, 1));
}

/**
 * @tc.name: MoveTo01
 * @tc.desc: MoveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, MoveTo01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("MoveTo01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    windowscenesession->property_->SetPersistentId(1);
    windowscenesession->state_ = WindowState::STATE_HIDDEN;
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, windowscenesession->MoveTo(2, 2));
}

/**
 * @tc.name: Minimize01
 * @tc.desc: Minimize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Minimize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    windowscenesession->property_->SetPersistentId(1);
    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_MINIMIZE)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Minimize());
}

/**
 * @tc.name: StartMove01
 * @tc.desc: StartMove
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, StartMove01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    windowscenesession->property_->SetPersistentId(1);
    // show with null session

    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_START_MOVE)).WillOnce(Return(WSError::WS_OK));
    windowscenesession->StartMove();
}

/**
 * @tc.name: Close01
 * @tc.desc: Close
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Close01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    windowscenesession->property_->SetPersistentId(1);
    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_CLOSE)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Close());
}

/**
 * @tc.name: SetActive01
 * @tc.desc: SetActive
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetActive01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    windowscenesession->property_->SetPersistentId(1);
    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), UpdateActiveStatus(false)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WSError::WS_OK, windowscenesession->SetActive(false));
}

/**
 * @tc.name: Recover01
 * @tc.desc: Recover
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Recover01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Connect01");
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);

    windowscenesession->property_->SetPersistentId(1);
    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_RECOVER)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Recover());
}

/**
 * @tc.name: Maximize01
 * @tc.desc: Maximize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Maximize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Connect01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowscenesession);
    windowscenesession->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowscenesession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Maximize());
}

/**
 * @tc.name: Hide01
 * @tc.desc: Hide session
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Hide01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Hide01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    // show with null session
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Hide(2, false, false));

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, window->Hide(2, false, false));
    ASSERT_EQ(WMError::WM_OK, window->Hide(2, false, false));

    window->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WM_OK, window->Hide(2, false, false));
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->type_ = WindowType::APP_SUB_WINDOW_BASE;
    EXPECT_CALL(*(session), Background()).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->Hide(2, false, false));

    window->property_->type_ = WindowType::APP_SUB_WINDOW_BASE;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->Destroy(false));
}

/**
 * @tc.name: Show01
 * @tc.desc: Show session
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Show01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Show01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    window->hostSession_ = session;
    EXPECT_CALL(*(session), Foreground(_)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->Show(2, false));

    window->state_ = WindowState::STATE_CREATED;
    EXPECT_CALL(*(session), Foreground(_)).WillOnce(Return(WSError::WS_ERROR_INVALID_SESSION));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_SESSION, window->Show(2, false));
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));
}

/**
 * @tc.name: SetBackgroundColor01
 * @tc.desc: test SetBackgroundColor withow uiContent
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetBackgroundColor01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetBackgroundColor01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    std::shared_ptr<AbilityRuntime::Context> context;
    ASSERT_EQ(WMError::WM_OK, window->Create(context, session));

    window->property_->SetPersistentId(1);
    window->Show();
}

/*
 * @tc.name: SetTransparent
 * @tc.desc: SetTransparent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetTransparent, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetTransparent(true));
    window->property_->SetPersistentId(1);
    option->SetWindowName("SetTransparent");
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->SetBackgroundColor(333);
    ASSERT_EQ(WMError::WM_OK, window->SetTransparent(true));
}

/*
 * @tc.name: SetAspectRatio
 * @tc.desc: SetAspectRatio test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetAspectRatio, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetAspectRatio(0.1));

    window->property_->SetPersistentId(1);
    window->property_->SetDisplayId(3);
    WindowLimits windowLimits = { 3, 3, 3, 3, 2.0, 2.0 };
    window->property_->SetWindowLimits(windowLimits);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, window->SetAspectRatio(0.1));
}

/*
 * @tc.name: ResetAspectRatio
 * @tc.desc: ResetAspectRatio test GetAvoidAreaByType
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, ResetAspectRatio, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    EXPECT_CALL(*(session), SetAspectRatio(_)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->ResetAspectRatio());
}

/*
 * @tc.name: GetAvoidAreaByType
 * @tc.desc: GetAvoidAreaByType test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, GetAvoidAreaByType, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    AvoidArea avoidarea;
    ASSERT_EQ(WMError::WM_OK, window->GetAvoidAreaByType(AvoidAreaType::TYPE_CUTOUT, avoidarea));
}

/*
 * @tc.name: Immersive
 * @tc.desc: Immersive01 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Immersive, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);

    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetLayoutFullScreen(false));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetFullScreen(false));
    ASSERT_EQ(false, window->IsLayoutFullScreen());
    ASSERT_EQ(false, window->IsFullScreen());
}

/*
 * @tc.name: SystemBarProperty
 * @tc.desc: SystemBarProperty01 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SystemBarProperty, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);

    SystemBarProperty property = SystemBarProperty();
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW,
        window->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
}

/*
 * @tc.name: LimitCameraFloatWindowMininumSize
 * @tc.desc: LimitCameraFloatWindowMininumSize01 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, LimitCameraFloatWindowMininumSize, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    uint32_t width = 33;
    uint32_t height = 31;
    window->LimitCameraFloatWindowMininumSize(width, height);
}

/*
 * @tc.name: NotifyWindowNeedAvoid
 * @tc.desc: NotifyWindowNeedAvoid test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, NotifyWindowNeedAvoid, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->NotifyWindowNeedAvoid(false));

    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(190);
    window->hostSession_ = session;
    EXPECT_CALL(*(session), OnNeedAvoid(false)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->NotifyWindowNeedAvoid(false));
}

/*
 * @tc.name: SetLayoutFullScreenByApiVersion
 * @tc.desc: SetLayoutFullScreenByApiVersion test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetLayoutFullScreenByApiVersion, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetLayoutFullScreenByApiVersion(false));
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->SetPersistentId(190);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    EXPECT_CALL(*(session), OnSessionEvent(SessionEvent::EVENT_MAXIMIZE)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetLayoutFullScreenByApiVersion(false));
}

/*
 * @tc.name: SetGlobalMaximizeMode
 * @tc.desc: SetGlobalMaximizeMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetGlobalMaximizeMode, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetGlobalMaximizeMode(MaximizeMode::MODE_RECOVER));

    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(190);
    window->hostSession_ = session;
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetGlobalMaximizeMode(MaximizeMode::MODE_RECOVER));

    window->property_->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    EXPECT_CALL(*(session), SetGlobalMaximizeMode(MaximizeMode::MODE_RECOVER)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, window->SetGlobalMaximizeMode(MaximizeMode::MODE_RECOVER));
}

/*
 * @tc.name: CheckParmAndPermission
 * @tc.desc: CheckParmAndPermission test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CheckParmAndPermission, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("CheckParmAndPermission");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    } else {
        ASSERT_EQ(WMError::WM_OK, window->CheckParmAndPermission());
        window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
        ASSERT_EQ(WMError::WM_OK, window->CheckParmAndPermission());
    }
}

/*
 * @tc.name: SetBackdropBlurStyle
 * @tc.desc: SetBackdropBlurStyle test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetBackdropBlurStyle, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetBackdropBlurStyle");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    window->property_->SetDisplayId(3);

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_OFF));
    } else {
    ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_OFF));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBackdropBlurStyle(WindowBlurStyle::WINDOW_BLUR_THICK));
    }
}

/*
 * @tc.name: SetTurnScreenOn
 * @tc.desc: SetTurnScreenOn test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetTurnScreenOn, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetBackdropBlurStyle");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetTurnScreenOn(false));

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, window->SetTurnScreenOn(false));
}

/*
 * @tc.name: SetBlur
 * @tc.desc: SetBlur test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetBlur, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetBlur");

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr){
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    }else{
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBlur(-1.0));
    ASSERT_EQ(WMError::WM_OK, window->SetBlur(1.0));
    }
}

/*
 * @tc.name: SetKeepScreenOn
 * @tc.desc: SetKeepScreenOn test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetKeepScreenOn, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetKeepScreenOn");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetKeepScreenOn(false));

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, window->SetKeepScreenOn(false));
    ASSERT_FALSE(window->IsKeepScreenOn());
}

/*
 * @tc.name: SetPrivacyMode
 * @tc.desc: SetPrivacyMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetPrivacyMode, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetPrivacyMode");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetPrivacyMode(false));

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    if (WMError::WM_OK == window->SetPrivacyMode(false)) {
    ASSERT_EQ(WMError::WM_OK, window->SetPrivacyMode(false));
    ASSERT_EQ(false, window->IsPrivacyMode());
    }else if (WMError::WM_DO_NOTHING == window->SetPrivacyMode(false)) {
    ASSERT_EQ(WMError::WM_DO_NOTHING, window->SetPrivacyMode(false));
    }
}

/*
 * @tc.name: IsPrivacyMode
 * @tc.desc: IsPrivacyMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, IsPrivacyModec, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("IsPrivacyModec");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    window->SetPrivacyMode(false);
}

/*
 * @tc.name: SetSystemPrivacyMode
 * @tc.desc: SetSystemPrivacyMode test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetSystemPrivacyMode, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetSystemPrivacyMode");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    window->SetSystemPrivacyMode(false);
    ASSERT_EQ(false, window->property_->GetSystemPrivacyMode());
}

/*
 * @tc.name: SetSnapshotSkip
 * @tc.desc: SetSnapshotSkip test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetSnapshotSkip, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetSnapshotSkip");
    window->property_->SetWindowType(WindowType::ABOVE_APP_SYSTEM_WINDOW_BASE);
    window->property_->SetPersistentId(1);
    auto surfaceNode_mocker = CreateRSSurfaceNode();
    if (surfaceNode_mocker != nullptr) {
    ASSERT_NE(nullptr, surfaceNode_mocker);
    }

    window->surfaceNode_ = surfaceNode_mocker;
    auto surfaceNode = window->GetSurfaceNode();

    if (surfaceNode != nullptr) {
    ASSERT_EQ(WMError::WM_OK, window->SetSnapshotSkip(false));
    } else {
    ASSERT_EQ(nullptr, surfaceNode);
    }
}

/*
 * @tc.name: SetLayoutFullScreen
 * @tc.desc: SetLayoutFullScreen test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetLayoutFullScreen, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetLayoutFullScreen");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetLayoutFullScreen(false));

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, window->SetLayoutFullScreen(false));
    ASSERT_EQ(false, window->IsLayoutFullScreen());
}

/*
 * @tc.name: SetFullScreen
 * @tc.desc: SetFullScreen test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetFullScreen, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetFullScreen");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetFullScreen(false));
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetFullScreen(false));
    ASSERT_EQ(false, window->IsFullScreen());
}

/*
 * @tc.name: SetShadowOffsetX
 * @tc.desc: SetShadowOffsetX test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetShadowOffsetX, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetKeepScreenOn");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    } else {
        ASSERT_EQ(WMError::WM_OK, window->SetShadowOffsetX(1.0));
    }
}

/*
 * @tc.name: SetShadowOffsetY
 * @tc.desc: SetShadowOffsetY test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetShadowOffsetY, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetShadowOffsetY");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    } else {
        ASSERT_EQ(WMError::WM_OK, window->SetShadowOffsetY(1.0));
    }
}

/*
 * @tc.name: SetBackdropBlur
 * @tc.desc: SetBackdropBlur test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetBackdropBlur, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetBackdropBlur");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    } else {
        ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetBackdropBlur(-1.0));
        ASSERT_EQ(WMError::WM_OK, window->SetBackdropBlur(1.0));
    }
}

/*
 * @tc.name: SetShadowColor
 * @tc.desc: SetShadowColor test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetShadowColor, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetShadowColor");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr){
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    }else{
        ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowColor("111ff22ee44"));
        ASSERT_EQ(WMError::WM_OK, window->SetShadowColor("#ff22ee44"));
        ASSERT_EQ(WMError::WM_OK, window->SetShadowColor("#000999"));
    }
}

/*
 * @tc.name: SetCornerRadius
 * @tc.desc: SetCornerRadius test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetCornerRadius, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetCornerRadius");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr){
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetCornerRadius(1.0));
    } else {
        ASSERT_EQ(WMError::WM_OK, window->SetCornerRadius(1.0));
    }
}

/*
 * @tc.name: SetShadowRadius
 * @tc.desc: SetShadowRadius test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetShadowRadius, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowName("SetShadowRadius");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);

    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    } else {
        ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetShadowRadius(-1.0));
        ASSERT_EQ(WMError::WM_OK, window->SetShadowRadius(1.0));
    }
}

/**
 * @tc.name: SetTransform01
 * @tc.desc: set transform
 * @tc.type: FUNC
 * @tc.require:issueI7IJVV
 */
HWTEST_F(WindowSceneSessionImplTest, SetTransform01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetTransform01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    Transform trans_;
    window->SetTransform(trans_);
    ASSERT_TRUE(trans_ == window->GetTransform());
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));
}

/**
 * @tc.name: RegisterAnimationTransitionController01
 * @tc.desc: RegisterAnimationTransitionController
 * @tc.type: FUNC
 * @tc.require:issueI7IJVV
 */
HWTEST_F(WindowSceneSessionImplTest, RegisterAnimationTransitionController01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("RegisterAnimationTransitionController01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->RegisterAnimationTransitionController(nullptr));
}

/**
 * @tc.name: SetNeedDefaultAnimation01
 * @tc.desc: SetNeedDefaultAnimation
 * @tc.type: FUNC
 * @tc.require:issueI7IJVV
 */
HWTEST_F(WindowSceneSessionImplTest, SetNeedDefaultAnimation01, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("SetNeedDefaultAnimation01");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    auto ret = true;
    window->property_->SetPersistentId(1);

    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->SetNeedDefaultAnimation(false);
    ASSERT_TRUE(ret);
}

/**
 * @tc.desc: UpdateSurfaceNodeAfterCustomAnimation01
 * @tc.desc: UpdateSurfaceNodeAfterCustomAnimation
 * @tc.type: FUNC
 * @tc.require:issueI7IJVV
 */
HWTEST_F(WindowSceneSessionImplTest, UpdateSurfaceNodeAfterCustomAnimation, Function | SmallTest | Level3)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("UpdateSurfaceNodeAfterCustomAnimation");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->property_->SetPersistentId(1);

    window->UpdateSurfaceNodeAfterCustomAnimation(false);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->UpdateSurfaceNodeAfterCustomAnimation(false));
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, window->UpdateSurfaceNodeAfterCustomAnimation(false));
}

/**
 * @tc.name: SetAlpha01
 * @tc.desc: SetAlpha
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetAlpha01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetAlpha01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    sptr<WindowSceneSessionImpl> windowscenesession = new (std::nothrow) WindowSceneSessionImpl(option);

    ASSERT_NE(nullptr, windowscenesession);
    windowscenesession->property_->SetPersistentId(11);
    windowscenesession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowscenesession->SetAlpha(1.0));
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    EXPECT_CALL(*(session), Connect(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
    EXPECT_CALL(*(session), CreateAndConnectSpecificSession(_, _, _, _, _, _)).WillOnce(Return(WSError::WS_OK));
    ASSERT_EQ(WMError::WM_OK, windowscenesession->Create(abilityContext_, session));
    windowscenesession->hostSession_ = session;

    auto surfaceNode = windowscenesession->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowscenesession->CheckParmAndPermission());
    } else {
        ASSERT_EQ(WMError::WM_OK, windowscenesession->SetAlpha(1.0));
    }
}
}
} // namespace Rosen
} // namespace OHOS
