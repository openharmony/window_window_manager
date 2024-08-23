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
#include <parameters.h>
#include "display_info.h"
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
        void(Rect rect, WindowSizeChangeReason reason, const std::shared_ptr<RSTransaction>& rsTransaction));
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
    MOCK_METHOD0(AfterResumed, void(void));
    MOCK_METHOD0(AfterPaused, void(void));
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
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

void WindowSceneSessionImplTest::SetUpTestCase() {}

void WindowSceneSessionImplTest::TearDownTestCase() {}

void WindowSceneSessionImplTest::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowSceneSessionImplTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
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
    ASSERT_NE(nullptr, option);
    option->SetWindowName("CreateWindowAndDestroy01");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy(false));
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, window->Create(abilityContext_, session));
    window->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: CreateWindowAndDestroy02
 * @tc.desc: Create window and destroy window
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateWindowAndDestroy02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("CreateWindowAndDestroy02");
    sptr<WindowSceneSessionImpl> window = new WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    std::string identityToken = "testToken";
    window->Create(abilityContext_, session);
    ASSERT_EQ(WMError::WM_ERROR_REPEAT_OPERATION, window->Create(abilityContext_, session, identityToken));
    window->property_->SetPersistentId(1);
    window->Destroy(false);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session, identityToken));
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));
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
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);

    windowSceneSession->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    if (windowSceneSession->CreateAndConnectSpecificSession() == WMError::WM_ERROR_NULLPTR) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowSceneSession->CreateAndConnectSpecificSession());
    }
    windowSceneSession->property_->SetPersistentId(102);
    windowSceneSession->property_->SetParentPersistentId(100);
    windowSceneSession->property_->SetParentId(100);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Destroy(true));
}

/**
 * @tc.name: CreateAndConnectSpecificSession02
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateAndConnectSpecificSession02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowTag(WindowTag::SUB_WINDOW);
    option->SetWindowName("CreateAndConnectSpecificSession02");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    
    SessionInfo sessionInfo = { "CreateTestBundle0", "CreateTestModule0", "CreateTestAbility0" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Create(abilityContext_, session));
    windowSceneSession->property_->SetPersistentId(103);
    windowSceneSession->property_->SetParentPersistentId(102);
    windowSceneSession->property_->SetParentId(102);
    windowSceneSession->property_->type_ = WindowType::APP_MAIN_WINDOW_BASE;
    windowSceneSession->hostSession_ = session;

    windowSceneSession->property_->type_ = WindowType::APP_SUB_WINDOW_BASE;
    if (windowSceneSession->CreateAndConnectSpecificSession() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, windowSceneSession->CreateAndConnectSpecificSession());
    }
    windowSceneSession->Destroy(true);
}

/**
 * @tc.name: CreateAndConnectSpecificSession03
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateAndConnectSpecificSession03, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowTag(WindowTag::SUB_WINDOW);
    option->SetWindowName("CreateAndConnectSpecificSession03");
    option->SetExtensionTag(true);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    
    SessionInfo sessionInfo = { "CreateTestBundle0", "CreateTestModule0", "CreateTestAbility0" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Create(abilityContext_, session));

    windowSceneSession->property_->SetParentPersistentId(102);
    windowSceneSession->property_->SetParentId(102);
    windowSceneSession->property_->type_ = WindowType::APP_MAIN_WINDOW_BASE;
    windowSceneSession->hostSession_ = session;

    windowSceneSession->property_->type_ = WindowType::APP_SUB_WINDOW_BASE;
    if (windowSceneSession->CreateAndConnectSpecificSession() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, windowSceneSession->CreateAndConnectSpecificSession());
    }
    windowSceneSession->Destroy(true);
}

/**
 * @tc.name: CreateAndConnectSpecificSession04
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateAndConnectSpecificSession04, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowTag(WindowTag::SUB_WINDOW);
    option->SetWindowName("CreateAndConnectSpecificSession04");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    
    SessionInfo sessionInfo = { "CreateTestBundle4", "CreateTestModule4", "CreateTestAbility4" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Create(abilityContext_, session));

    windowSceneSession->property_->SetPersistentId(104);
    windowSceneSession->property_->SetParentPersistentId(103);
    windowSceneSession->property_->SetParentId(103);
    windowSceneSession->property_->type_ = WindowType::APP_MAIN_WINDOW_BASE;
    windowSceneSession->hostSession_ = session;

    windowSceneSession->property_->type_ = WindowType::APP_SUB_WINDOW_BASE;
    if (windowSceneSession->CreateAndConnectSpecificSession() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, windowSceneSession->CreateAndConnectSpecificSession());
    }
    windowSceneSession->Destroy(true);
}

/**
 * @tc.name: CreateAndConnectSpecificSession05
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateAndConnectSpecificSession05, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowTag(WindowTag::SUB_WINDOW);
    option->SetWindowName("CreateAndConnectSpecificSession05");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    
    SessionInfo sessionInfo = { "CreateTestBundle5", "CreateTestModule5", "CreateTestAbility5" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Create(abilityContext_, session));

    windowSceneSession->property_->SetParentPersistentId(104);
    windowSceneSession->property_->SetParentId(104);
    windowSceneSession->property_->type_ = WindowType::APP_MAIN_WINDOW_BASE;
    windowSceneSession->hostSession_ = session;

    windowSceneSession->property_->type_ = WindowType::APP_SUB_WINDOW_BASE;
    if (windowSceneSession->CreateAndConnectSpecificSession() == WMError::WM_OK) {
        ASSERT_NE(WMError::WM_OK, windowSceneSession->CreateAndConnectSpecificSession());
    }
    windowSceneSession->Destroy(true);
}

/**
 * @tc.name: CreateAndConnectSpecificSession06
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateAndConnectSpecificSession06, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowTag(WindowTag::SYSTEM_WINDOW);
    option->SetWindowName("CreateAndConnectSpecificSession06");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    
    SessionInfo sessionInfo = { "CreateTestBundle6", "CreateTestModule6", "CreateTestAbility6" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Create(abilityContext_, session));
    
    windowSceneSession->property_->SetPersistentId(105);
    windowSceneSession->property_->SetParentPersistentId(102);
    windowSceneSession->property_->SetParentId(102);
    windowSceneSession->property_->type_ = WindowType::APP_MAIN_WINDOW_BASE;
    windowSceneSession->hostSession_ = session;

    windowSceneSession->property_->type_ = WindowType::SYSTEM_WINDOW_BASE;
    if (windowSceneSession->CreateAndConnectSpecificSession() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, windowSceneSession->CreateAndConnectSpecificSession());
    }
    windowSceneSession->Destroy(true);
}

/**
 * @tc.name: CreateAndConnectSpecificSession07
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateAndConnectSpecificSession07, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowTag(WindowTag::SUB_WINDOW);
    option->SetWindowName("CreateAndConnectSpecificSession07");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    
    SessionInfo sessionInfo = { "CreateTestBundle7", "CreateTestModule7", "CreateTestAbility7" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Create(abilityContext_, session));
    
    windowSceneSession->property_->SetPersistentId(106);
    windowSceneSession->property_->SetParentPersistentId(105);
    windowSceneSession->property_->SetParentId(105);
    windowSceneSession->property_->type_ = WindowType::SYSTEM_WINDOW_BASE;
    windowSceneSession->hostSession_ = session;

    windowSceneSession->property_->type_ = WindowType::SYSTEM_SUB_WINDOW_BASE;
    if (windowSceneSession->CreateAndConnectSpecificSession() == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, windowSceneSession->CreateAndConnectSpecificSession());
    }
    windowSceneSession->Destroy(true);
}

/**
 * @tc.name: CreateAndConnectSpecificSession08
 * @tc.desc: CreateAndConnectSpecificSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, CreateAndConnectSpecificSession08, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowTag(WindowTag::SUB_WINDOW);
    option->SetWindowName("CreateAndConnectSpecificSession08");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    
    SessionInfo sessionInfo = { "CreateTestBundle8", "CreateTestModule8", "CreateTestAbility8" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Create(abilityContext_, session));
    
    windowSceneSession->property_->SetPersistentId(107);
    windowSceneSession->property_->SetParentPersistentId(106);
    windowSceneSession->property_->SetParentId(106);
    windowSceneSession->property_->type_ = WindowType::SYSTEM_SUB_WINDOW_BASE;
    windowSceneSession->hostSession_ = session;

    windowSceneSession->property_->type_ = WindowType::SYSTEM_SUB_WINDOW_BASE;
    if (windowSceneSession->CreateAndConnectSpecificSession() == WMError::WM_ERROR_INVALID_TYPE) {
        ASSERT_EQ(WMError::WM_ERROR_INVALID_TYPE, windowSceneSession->CreateAndConnectSpecificSession());
    }
    windowSceneSession->Destroy(true);
}

/**
 * @tc.name: RecoverAndReconnectSceneSession
 * @tc.desc: RecoverAndReconnectSceneSession
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, RecoverAndReconnectSceneSession, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("RecoverAndReconnectSceneSession");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowSceneSession->RecoverAndReconnectSceneSession());
}

/**
 * @tc.name: IsValidSystemWindowType01
 * @tc.desc: IsValidSystemWindowType
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, IsValidSystemWindowType01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("IsValidSystemWindowType01");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    ASSERT_FALSE(!windowSceneSession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW));
    ASSERT_FALSE(!windowSceneSession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT));
    ASSERT_FALSE(!windowSceneSession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA));
    ASSERT_FALSE(!windowSceneSession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_DIALOG));
    ASSERT_FALSE(!windowSceneSession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_FLOAT));
    ASSERT_FALSE(!windowSceneSession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_SCREENSHOT));
    ASSERT_FALSE(!windowSceneSession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH));
    ASSERT_FALSE(!windowSceneSession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_VOICE_INTERACTION));
    ASSERT_FALSE(!windowSceneSession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_POINTER));
    ASSERT_FALSE(!windowSceneSession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_TOAST));
    ASSERT_FALSE(!windowSceneSession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE));
    ASSERT_TRUE(!windowSceneSession->IsValidSystemWindowType(WindowType::WINDOW_TYPE_APP_LAUNCHING));
}

/*
 * @tc.name: InvalidWindow
 * @tc.desc: InvalidWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, InvalidWindow, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("InvalidWindow");
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
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    
    windowSceneSession->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    ASSERT_TRUE(windowSceneSession->FindMainWindowWithContext() == nullptr);
    windowSceneSession->SetWindowType(WindowType::ABOVE_APP_SYSTEM_WINDOW_END);
    ASSERT_TRUE(windowSceneSession->FindMainWindowWithContext() == nullptr);

    windowSceneSession->property_->SetPersistentId(1112);
    windowSceneSession->property_->SetParentId(1000);
    windowSceneSession->property_->SetParentPersistentId(1000);
    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Create(abilityContext_, session));
    windowSceneSession->hostSession_ = session;
    ASSERT_TRUE(nullptr != windowSceneSession->FindParentSessionByParentId(1112));
    windowSceneSession->Destroy(true);
}

/**
 * @tc.name: DisableAppWindowDecor01
 * @tc.desc: DisableAppWindowDecor
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, DisableAppWindowDecor01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("DisableAppWindowDecor01");
    sptr<WindowSessionImpl> windowSession = new (std::nothrow) WindowSessionImpl(option);
    ASSERT_NE(nullptr, windowSession);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);

    ASSERT_NE(nullptr, session);
    std::shared_ptr<AbilityRuntime::Context> context;
    ASSERT_EQ(WMError::WM_OK, windowSession->Create(context, session));

    windowSession->UpdateDecorEnable(false);
    windowSession->windowSystemConfig_.isSystemDecorEnable_ = false;

    windowSession->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    windowSession->DisableAppWindowDecor();
    ASSERT_FALSE(windowSession->IsDecorEnable());
    windowSession->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    windowSession->DisableAppWindowDecor();
    windowSession->Destroy(true);
}

/**
 * @tc.name: RaiseToAppTop01
 * @tc.desc: RaiseToAppTop
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, RaiseToAppTop01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("RaiseToAppTop01");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);

    windowSceneSession->property_->SetPersistentId(6);
    windowSceneSession->property_->SetParentPersistentId(6);
    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_CALLING, windowSceneSession->RaiseToAppTop());

    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    windowSceneSession->state_ = WindowState::STATE_HIDDEN;
    ASSERT_EQ(WMError::WM_DO_NOTHING, windowSceneSession->RaiseToAppTop());

    windowSceneSession->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->RaiseToAppTop());
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
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);

    windowSceneSession->property_->SetPersistentId(888);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSession->hostSession_ = session;
    Rect rect = {2, 2, 2, 2};
    windowSceneSession->property_->SetWindowRect(rect);
    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT_CAMERA);
    windowSceneSession->state_ = WindowState::STATE_FROZEN;
    windowSceneSession->hostSession_ = session;
    windowSceneSession->Resize(1, 1);
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
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);

    windowSceneSession->property_->SetPersistentId(1);
    windowSceneSession->state_ = WindowState::STATE_HIDDEN;
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->MoveTo(2, 2));
}

/**
 * @tc.name: Minimize01
 * @tc.desc: Minimize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Minimize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Minimize01");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);

    windowSceneSession->property_->SetPersistentId(1);
    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Minimize());
}

/**
 * @tc.name: StartMove01
 * @tc.desc: StartMove
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, StartMove01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("StartMove01");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    windowSceneSession->property_->SetPersistentId(1);
    // show with null session

    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    windowSceneSession->hostSession_ = session;
    windowSceneSession->StartMove();
    ASSERT_NE(nullptr, session);
}

/**
 * @tc.name: StartMoveSystemWindow01
 * @tc.desc: StartMoveSystemWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, StartMoveSystemWindow01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("StartMoveSystemWindow01");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    windowSceneSession->property_->SetPersistentId(1);
    // show with null session

    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_GLOBAL_SEARCH);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    windowSceneSession->hostSession_ = session;
    windowSceneSession->StartMoveSystemWindow();
    ASSERT_NE(nullptr, session);
}

/**
 * @tc.name: Close01
 * @tc.desc: Close
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Close01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Close01");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    windowSceneSession->property_->SetPersistentId(1);
    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Close());
}

/**
 * @tc.name: Close02
 * @tc.desc: Close
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Close02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Close02");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    windowSceneSession->property_->SetPersistentId(-1);
    windowSceneSession->property_->SetParentPersistentId(-1);
    windowSceneSession->property_->SetPersistentId(1);
    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
}

/**
 * @tc.name: SetActive01
 * @tc.desc: SetActive
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetActive01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetActive01");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    windowSceneSession->property_->SetPersistentId(1);
    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WSError::WS_OK, windowSceneSession->SetActive(false));
    ASSERT_EQ(WSError::WS_OK, windowSceneSession->SetActive(true));
}

/**
 * @tc.name: Recover01
 * @tc.desc: Recover
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Recover01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Recover01");
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);

    windowSceneSession->property_->SetPersistentId(1);
    windowSceneSession->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Recover());
}

/**
 * @tc.name: Maximize01
 * @tc.desc: Maximize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, Maximize01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("Maximize01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> windowSceneSession = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, windowSceneSession);
    windowSceneSession->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->Maximize());
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
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->Hide(2, false, false));

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
    ASSERT_EQ(WMError::WM_OK, window->Hide(2, false, false));

    window->property_->type_ = WindowType::APP_SUB_WINDOW_BASE;
    if (window->Destroy(false) == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->Destroy(false));
    }
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
    option->SetDisplayId(0);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->property_->SetPersistentId(1);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, window->Show(2, false));

    window->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WM_OK, window->Show(2, false));
    ASSERT_EQ(WMError::WM_OK, window->Destroy(false));
}

/**
 * @tc.name: NotifyDrawingCompleted
 * @tc.desc: NotifyDrawingCompleted session
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, NotifyDrawingCompleted, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("NotifyDrawingCompleted");
    option->SetDisplayId(0);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    ASSERT_NE(nullptr, window->property_);
    window->property_->SetPersistentId(1);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);

    window->hostSession_ = session;
    window->NotifyDrawingCompleted();
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
    window->Destroy(true);
}

/*
 * @tc.name: SetTransparent
 * @tc.desc: SetTransparent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetTransparent, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetTransparent");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetTransparent(true));
    window->property_->SetPersistentId(1);
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->uiContent_ = std::make_unique<Ace::UIContentMocker>();
    window->SetBackgroundColor(333);
    if (window->SetTransparent(true) == WMError::WM_OK) {
        ASSERT_EQ(WMError::WM_OK, window->SetTransparent(true));
    }
}

/*
 * @tc.name: SetAspectRatio
 * @tc.desc: SetAspectRatio test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetAspectRatio, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SetAspectRatio");
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
    option->SetWindowName("ResetAspectRatio");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
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
    option->SetWindowName("GetAvoidAreaByType");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    AvoidArea avoidarea;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->GetAvoidAreaByType(AvoidAreaType::TYPE_CUTOUT, avoidarea));
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
    option->SetWindowName("Immersive");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);


    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetLayoutFullScreen(false));
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetFullScreen(false));
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
 
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
    option->SetWindowName("SystemBarProperty");
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);

    SystemBarProperty property = SystemBarProperty();
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW,
        window->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
}

/*
 * @tc.name: SystemBarProperty02
 * @tc.desc: SystemBarProperty02 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SystemBarProperty02, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SystemBarProperty02");
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;

    SystemBarProperty property;
    ASSERT_EQ(WMError::WM_OK, window->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    ASSERT_FALSE(window->property_->GetSystemBarProperty()[WindowType::WINDOW_TYPE_STATUS_BAR].enableAnimation_);

    property.enableAnimation_ = false;
    ASSERT_EQ(WMError::WM_OK, window->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    ASSERT_FALSE(window->property_->GetSystemBarProperty()[WindowType::WINDOW_TYPE_STATUS_BAR].enableAnimation_);
}

/*
 * @tc.name: SystemBarProperty03
 * @tc.desc: SystemBarProperty03 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SystemBarProperty03, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SystemBarProperty03");
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;

    SystemBarProperty property;
    property.enableAnimation_ = true;
    ASSERT_EQ(WMError::WM_OK, window->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    ASSERT_TRUE(window->property_->GetSystemBarProperty()[WindowType::WINDOW_TYPE_STATUS_BAR].enableAnimation_);
}

/*
 * @tc.name: SystemBarProperty04
 * @tc.desc: SystemBarProperty04 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SystemBarProperty04, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SystemBarProperty04");
    ASSERT_NE(nullptr, option);
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    SystemBarProperty property;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW,
        window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
}

/*
 * @tc.name: SystemBarProperty05
 * @tc.desc: SystemBarProperty05 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SystemBarProperty05, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SystemBarProperty05");
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;

    SystemBarProperty property;
    ASSERT_EQ(WMError::WM_OK, window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    ASSERT_FALSE(window->property_->GetSystemBarProperty()[WindowType::WINDOW_TYPE_STATUS_BAR].enableAnimation_);

    property.enableAnimation_ = false;
    ASSERT_EQ(WMError::WM_OK, window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    ASSERT_FALSE(window->property_->GetSystemBarProperty()[WindowType::WINDOW_TYPE_STATUS_BAR].enableAnimation_);
}

/*
 * @tc.name: SystemBarProperty06
 * @tc.desc: SystemBarProperty06 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SystemBarProperty06, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SystemBarProperty06");
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;

    SystemBarProperty property;
    property.enableAnimation_ = true;
    ASSERT_EQ(WMError::WM_OK, window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    ASSERT_TRUE(window->property_->GetSystemBarProperty()[WindowType::WINDOW_TYPE_STATUS_BAR].enableAnimation_);
}

/*
 * @tc.name: SystemBarProperty07
 * @tc.desc: SystemBarProperty07 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SystemBarProperty07, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SystemBarProperty07");
    ASSERT_NE(nullptr, option);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);

    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    window->hostSession_ = session;

    SystemBarProperty property;
    ASSERT_EQ(WMError::WM_OK, window->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    ASSERT_EQ(SystemBarSettingFlag::DEFAULT_SETTING,
        window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR).settingFlag_);

    property.enable_ = false;
    property.settingFlag_ = SystemBarSettingFlag::ENABLE_SETTING;
    ASSERT_EQ(WMError::WM_OK, window->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    ASSERT_EQ(SystemBarSettingFlag::ENABLE_SETTING,
        window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR).settingFlag_);

    property.backgroundColor_ = 0xB3000000;
    property.settingFlag_ = SystemBarSettingFlag::COLOR_SETTING;
    ASSERT_EQ(WMError::WM_OK, window->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    ASSERT_EQ(SystemBarSettingFlag::COLOR_SETTING,
        window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR).settingFlag_);

    property.enable_ = true;
    property.backgroundColor_ = 0x4C000000;
    property.settingFlag_ = SystemBarSettingFlag::ALL_SETTING;
    ASSERT_EQ(WMError::WM_OK, window->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

/*
 * @tc.name: SetSystemBarProperties
 * @tc.desc: SetSystemBarProperties test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetSystemBarProperties, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    option->SetWindowName("SetSystemBarProperties");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(1);
    window->hostSession_ = session;
    std::map<WindowType, SystemBarProperty> properties;
    std::map<WindowType, SystemBarPropertyFlag> propertyFlags;
    SystemBarProperty current = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    SystemBarProperty property;
    properties[WindowType::WINDOW_TYPE_STATUS_BAR] = property;
    SystemBarPropertyFlag propertyFlag;
    propertyFlag.contentColorFlag = true;
    propertyFlags[WindowType::WINDOW_TYPE_STATUS_BAR] = propertyFlag;
    ASSERT_EQ(WMError::WM_OK, window->SetSystemBarProperties(properties, propertyFlags));
    if (property.contentColor_ != current.contentColor_) {
        std::map<WindowType, SystemBarProperty> currProperties;
        ASSERT_EQ(WMError::WM_OK, window->GetSystemBarProperties(currProperties));
        ASSERT_EQ(currProperties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_, property.contentColor_);
    }
}

/*
 * @tc.name: GetSystemBarProperties
 * @tc.desc: GetSystemBarProperties test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, GetSystemBarProperties, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    option->SetWindowName("GetSystemBarProperties");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(nullptr, window);
    std::map<WindowType, SystemBarProperty> properties;
    ASSERT_EQ(WMError::WM_OK, window->GetSystemBarProperties(properties));
}

/*
 * @tc.name: SpecificBarProperty
 * @tc.desc: SpecificBarProperty01 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SpecificBarProperty, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("SpecificBarProperty");
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);

    SystemBarProperty property = SystemBarProperty();
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW,
        window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    if (window->property_ == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR,
            window->SetSpecificBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    }
}

/*
 * @tc.name: NotifySpecificWindowSessionProperty
 * @tc.desc: NotifySpecificWindowSessionProperty01 test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, NotifySpecificWindowSessionProperty, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowMode(WindowMode::WINDOW_MODE_PIP);
    option->SetWindowName("NotifySpecificWindowSessionProperty");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);

    SystemBarProperty property = SystemBarProperty();
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW,
        window->NotifySpecificWindowSessionProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    window->property_->SetPersistentId(190);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_HIDDEN;
    ASSERT_EQ(WMError::WM_OK,
        window->NotifySpecificWindowSessionProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
    window->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_OK,
        window->NotifySpecificWindowSessionProperty(WindowType::WINDOW_TYPE_STATUS_BAR, property));
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
    option->SetWindowName("LimitCameraFloatWindowMininumSize");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    uint32_t width = 33;
    uint32_t height = 31;
    float vpr = 0.0f;
    window->LimitCameraFloatWindowMininumSize(width, height, vpr);
}

/*
 * @tc.name: NotifyWindowNeedAvoid
 * @tc.desc: NotifyWindowNeedAvoid test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, NotifyWindowNeedAvoid, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("NotifyWindowNeedAvoid");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->NotifyWindowNeedAvoid(false));

    window->state_ = WindowState::STATE_SHOWN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->property_->SetPersistentId(190);
    window->hostSession_ = session;
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
    option->SetWindowName("SetLayoutFullScreenByApiVersion");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetLayoutFullScreenByApiVersion(false));
    window->state_ = WindowState::STATE_SHOWN;
    window->property_->SetPersistentId(190);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
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
    option->SetWindowName("SetGlobalMaximizeMode");
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
    window->property_->SetWindowName("SetTurnScreenOn");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetTurnScreenOn(false));

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->SetTurnScreenOn(false);
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
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    } else {
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
    window->SetKeepScreenOn(false);
    ASSERT_FALSE(window->IsKeepScreenOn());
}

/*
 * @tc.name: DestoryInner01
 * @tc.desc: DestoryInner01
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, DestoryInner01, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("DestoryInner01");
    option->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(window, nullptr);
    SessionInfo sessionInfo = {"DestoryInnerBundle", "DestoryInnerModule", "DestoryInnerAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(session, nullptr);
    window->property_->SetPersistentId(123);
    window->property_->SetExtensionFlag(true);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->DestroyInner(true));
    window->property_->SetExtensionFlag(false);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->DestroyInner(true));
}

/*
 * @tc.name: DestoryInner02
 * @tc.desc: DestoryInner02
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, DestoryInner02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    ASSERT_NE(nullptr, option);
    option->SetWindowName("DestoryInner02");
    option->SetWindowType(WindowType::BELOW_APP_SYSTEM_WINDOW_BASE);
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_NE(window, nullptr);
    SessionInfo sessionInfo = {"DestoryInnerBundle", "DestoryInnerModule", "DestoryInnerAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(session, nullptr);
    window->property_->SetPersistentId(134);
    window->property_->SetExtensionFlag(true);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->DestroyInner(true));
    window->property_->SetExtensionFlag(false);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->DestroyInner(true));
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
    } else if (WMError::WM_DO_NOTHING == window->SetPrivacyMode(false)) {
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
        ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetSnapshotSkip(false));
    } else {
        ASSERT_EQ(nullptr, surfaceNode);
    }
}

/*
 * @tc.name: SetImmersiveModeEnabledState
 * @tc.desc: SetImmersiveModeEnabledState test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, SetImmersiveModeEnabledState, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->SetImmersiveModeEnabledState(false));

    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;
    window->property_->SetWindowName("SetImmersiveModeEnabledState");
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    window->state_ = WindowState::STATE_CREATED;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetImmersiveModeEnabledState(false));

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(WMError::WM_OK, window->SetImmersiveModeEnabledState(true));
    ASSERT_EQ(true, window->GetImmersiveModeEnabledState());
    ASSERT_EQ(WMError::WM_OK, window->SetImmersiveModeEnabledState(false));
    ASSERT_EQ(false, window->GetImmersiveModeEnabledState());

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_EQ(WMError::WM_OK, window->SetLayoutFullScreen(true));
    ASSERT_EQ(true, window->IsLayoutFullScreen());
    ASSERT_EQ(WMError::WM_OK, window->SetLayoutFullScreen(false));
    ASSERT_EQ(false, window->IsLayoutFullScreen());
    ASSERT_EQ(WMError::WM_OK, window->SetImmersiveModeEnabledState(true));
    ASSERT_EQ(true, window->IsLayoutFullScreen());
    ASSERT_EQ(WMError::WM_OK, window->SetImmersiveModeEnabledState(false));
    ASSERT_EQ(false, window->IsLayoutFullScreen());

    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ASSERT_EQ(WMError::WM_OK, window->SetImmersiveModeEnabledState(true));
    ASSERT_EQ(true, window->IsLayoutFullScreen());
    ASSERT_EQ(true, window->GetImmersiveModeEnabledState());
    ASSERT_EQ(WMError::WM_OK, window->MaximizeFloating());
    ASSERT_EQ(true, window->IsLayoutFullScreen());
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
    window->SetLayoutFullScreen(false);

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
    window->SetFullScreen(false);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = {"CreateTestBundle", "CreateTestModule", "CreateTestAbility"};
    sptr<SessionMocker> session = new (std::nothrow) SessionMocker(sessionInfo);
    ASSERT_NE(nullptr, session);
    window->hostSession_ = session;

    ASSERT_EQ(WMError::WM_OK, window->SetFullScreen(false));
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
    window->property_->SetWindowName("SetShadowOffsetX");
    window->property_->SetWindowType(WindowType::SYSTEM_SUB_WINDOW_BASE);
    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        ASSERT_EQ(WMError::WM_ERROR_NULLPTR, window->CheckParmAndPermission());
    } else {
        ASSERT_EQ(WMError::WM_OK, window->SetShadowOffsetX(1.0));
    }
}

/*
 * @tc.name: GetStatusBarHeight
 * @tc.desc: GetStatusBarHeight test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplTest, GetStatusBarHeight, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    option->SetWindowName("GetStatusBarHeight");
    sptr<WindowSceneSessionImpl> window = new (std::nothrow) WindowSceneSessionImpl(option);
    ASSERT_EQ(0, window->GetStatusBarHeight());
}
}
} // namespace Rosen
} // namespace OHOS
