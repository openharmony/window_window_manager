/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ability_context_impl.h"
#include "display_info.h"
#include "mock_session.h"
#include "mock_uicontent.h"
#include "mock_window_adapter.h"
#include "scene_board_judgement.h"
#include "session/host/include/scene_session.h"
#include "singleton_mocker.h"
#include "window_scene_session_impl.h"
#include "window_session_impl.h"

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

class WindowSceneSessionImplLayoutTest : public testing::Test {
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
    static constexpr uint32_t WAIT_SERVERAL_FRAMES = 36000;
};

void WindowSceneSessionImplLayoutTest::SetUpTestCase() {}

void WindowSceneSessionImplLayoutTest::TearDownTestCase() {}

void WindowSceneSessionImplLayoutTest::SetUp()
{
    abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();
}

void WindowSceneSessionImplLayoutTest::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
    abilityContext_ = nullptr;
}

RSSurfaceNode::SharedPtr WindowSceneSessionImplLayoutTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "startingWindowTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    return surfaceNode;
}

namespace {

/**
 * @tc.name: SetAspectRatio01
 * @tc.desc: SetAspectRatio test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetAspectRatio01, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetAspectRatio01");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetAspectRatio(0.1));
}

/**
 * @tc.name: SetAspectRatio02
 * @tc.desc: SetAspectRatio test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetAspectRatio02, TestSize.Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetAspectRatio02");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->SetAspectRatio(0.1));

    window->property_->SetPersistentId(1);
    window->property_->SetDisplayId(0);
    WindowLimits windowLimits = { 3000, 3000, 2000, 2000, 2.0, 2.0 };
    window->property_->SetWindowLimits(windowLimits);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
    window->hostSession_ = session;
    session->GetSessionProperty()->SetWindowLimits(windowLimits);
    const float ratio = 1.2;
    ASSERT_EQ(WMError::WM_OK, window->SetAspectRatio(ratio));
    ASSERT_EQ(ratio, session->GetAspectRatio());
}

/**
 * @tc.name: SetAspectRatio03
 * @tc.desc: SetAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetAspectRatio03, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetAspectRatio03");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSessionImpl->hostSession_ = session;
    auto ret = windowSceneSessionImpl->SetAspectRatio(MathHelper::INF);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    ret = windowSceneSessionImpl->SetAspectRatio(MathHelper::INF);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = windowSceneSessionImpl->SetAspectRatio(MathHelper::NAG_INF);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = windowSceneSessionImpl->SetAspectRatio(std::sqrt(-1.0));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = windowSceneSessionImpl->SetAspectRatio(0.0f);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret);
    ret = windowSceneSessionImpl->SetAspectRatio(1.0f);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: ResetAspectRatioTest
 * @tc.desc: Verify ResetAspectRatio behavior in different scenarios.
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, ResetAspectRatioTest, TestSize.Level1)
{
    auto option = sptr<WindowOption>::MakeSptr();
    auto window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetAspectRatio(1.5f);
    SessionInfo sessionInfo;
    auto mockHostSession = sptr<SessionMocker>::MakeSptr(sessionInfo);

    // Case 1: hostSession_ is nullptr
    {
        window->hostSession_ = nullptr;
        auto ret = window->ResetAspectRatio();
        EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
        EXPECT_FLOAT_EQ(1.5f, window->property_->GetAspectRatio());
    }

    // Case 2: hostSession_->SetAspectRatio fails
    {
        EXPECT_CALL(*mockHostSession, SetAspectRatio(_))
            .Times(1).WillOnce(Return(WSError::WS_ERROR_IPC_FAILED));
        window->hostSession_ = mockHostSession;
        auto ret = window->ResetAspectRatio();
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
        EXPECT_FLOAT_EQ(1.5f, window->property_->GetAspectRatio());
    }

    // Case 3: hostSession_->SetAspectRatio succeeds
    {
        EXPECT_CALL(*mockHostSession, SetAspectRatio(_))
            .Times(1).WillOnce(Return(WSError::WS_OK));
        auto ret = window->ResetAspectRatio();
        EXPECT_EQ(WMError::WM_OK, ret);
        EXPECT_FLOAT_EQ(0.0f, window->property_->GetAspectRatio());
    }
}

/**
 * @tc.name: SetWindowLimits01
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetWindowLimits01, TestSize.Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetWindowLimits01");
    option->SetDisplayId(0);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    window->property_->SetPersistentId(1);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->state_ = WindowState::STATE_FROZEN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;

    window->useUniqueDensity_ = true;
    window->virtualPixelRatio_ = 1.9;

    WindowLimits windowLimits = { 2000, 2000, 2000, 2000, 0.0f, 0.0f };
    EXPECT_EQ(WMError::WM_OK, window->SetWindowLimits(windowLimits, false));
    WindowLimits windowSizeLimits = window->property_->GetWindowLimits();
    EXPECT_EQ(windowSizeLimits.maxWidth_, 2000);
    EXPECT_EQ(windowSizeLimits.maxHeight_, 2000);
    EXPECT_EQ(windowSizeLimits.minWidth_, 2000);
    EXPECT_EQ(windowSizeLimits.minHeight_, 2000);

    windowLimits = { 2000, 2000, 100, 100, 0.0f, 0.0f };
    EXPECT_EQ(WMError::WM_OK, window->SetWindowLimits(windowLimits, false));
    windowSizeLimits = window->property_->GetWindowLimits();
    EXPECT_EQ(windowSizeLimits.maxWidth_, 2000);
    EXPECT_EQ(windowSizeLimits.maxHeight_, 2000);
    EXPECT_NE(windowSizeLimits.minWidth_, 100);
    EXPECT_NE(windowSizeLimits.minHeight_, 100);

    windowLimits = { 10000, 10000, 30, 30, 0.0f, 0.0f };
    EXPECT_EQ(WMError::WM_OK, window->SetWindowLimits(windowLimits, false));
    windowSizeLimits = window->property_->GetWindowLimits();
    EXPECT_NE(windowSizeLimits.maxWidth_, 10000);
    EXPECT_NE(windowSizeLimits.maxHeight_, 10000);
    EXPECT_NE(windowSizeLimits.minWidth_, 30);
    EXPECT_NE(windowSizeLimits.minHeight_, 30);
}

/**
 * @tc.name: SetWindowLimits06
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetWindowLimits06, TestSize.Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetWindowLimits06");
    option->SetDisplayId(0);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    window->property_->SetPersistentId(1);
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    window->state_ = WindowState::STATE_FROZEN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;

    window->useUniqueDensity_ = true;
    window->virtualPixelRatio_ = 1.9;

    WindowLimits windowLimits = { 2000, 2000, 2000, 2000, 0.0f, 0.0f };
    EXPECT_EQ(WMError::WM_OK, window->SetWindowLimits(windowLimits, true));
    WindowLimits windowSizeLimits = window->property_->GetWindowLimits();
    EXPECT_EQ(windowSizeLimits.maxWidth_, 2000);
    EXPECT_EQ(windowSizeLimits.maxHeight_, 2000);
    EXPECT_EQ(windowSizeLimits.minWidth_, 2000);
    EXPECT_EQ(windowSizeLimits.minHeight_, 2000);

    windowLimits = { 2000, 2000, 100, 100, 0.0f, 0.0f };
    EXPECT_EQ(WMError::WM_OK, window->SetWindowLimits(windowLimits, true));
    windowSizeLimits = window->property_->GetWindowLimits();
    EXPECT_EQ(windowSizeLimits.maxWidth_, 2000);
    EXPECT_EQ(windowSizeLimits.maxHeight_, 2000);
    EXPECT_NE(windowSizeLimits.minWidth_, 100);
    EXPECT_NE(windowSizeLimits.minHeight_, 100);

    windowLimits = { 2000, 2000, 100, 100, 0.0f, 0.0f };
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    EXPECT_EQ(WMError::WM_OK, window->SetWindowLimits(windowLimits, true));
    windowSizeLimits = window->property_->GetWindowLimits();
    EXPECT_EQ(windowSizeLimits.maxWidth_, 2000);
    EXPECT_EQ(windowSizeLimits.maxHeight_, 2000);
    EXPECT_EQ(windowSizeLimits.minWidth_, 100);
    EXPECT_EQ(windowSizeLimits.minHeight_, 100);

    windowLimits = { 10000, 10000, 30, 30, 0.0f, 0.0f };
    EXPECT_EQ(WMError::WM_OK, window->SetWindowLimits(windowLimits, true));
    windowSizeLimits = window->property_->GetWindowLimits();
    EXPECT_NE(windowSizeLimits.maxWidth_, 10000);
    EXPECT_NE(windowSizeLimits.maxHeight_, 10000);
    EXPECT_NE(windowSizeLimits.minWidth_, 30);
    EXPECT_NE(windowSizeLimits.minHeight_, 30);
}

/**
 * @tc.name: SetWindowLimits02
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetWindowLimits02, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetWindowLimits02");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    WindowLimits windowLimits = { 1000, 1000, 1000, 1000, 0.0f, 0.0f };

    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    auto ret = windowSceneSessionImpl->SetWindowLimits(windowLimits, false);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
    ret = windowSceneSessionImpl->SetWindowLimits(windowLimits, true);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: SetWindowLimits03
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetWindowLimits03, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetWindowLimits03");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    WindowLimits windowLimits = { 1000, 1000, 1000, 1000, 0.0f, 0.0f };
    windowSceneSessionImpl->property_->SetPersistentId(1);
    windowSceneSessionImpl->property_->SetDisplayId(0);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_CREATED;
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_DIALOG);
    auto ret = windowSceneSessionImpl->SetWindowLimits(windowLimits, false);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = windowSceneSessionImpl->SetWindowLimits(windowLimits, true);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetWindowLimits04
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetWindowLimits04, TestSize.Level1)
{
    sptr<WindowOption> subWindow = sptr<WindowOption>::MakeSptr();
    subWindow->SetWindowName("SetWindowLimits04");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(subWindow);
    WindowLimits windowLimits = { 1000, 1000, 1000, 1000, 0.0f, 0.0f };
    windowSceneSessionImpl->SetWindowLimits(windowLimits, false);
    windowSceneSessionImpl->property_->SetPersistentId(1004);
    windowSceneSessionImpl->property_->SetDisplayId(0);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_CREATED;
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_LAUNCHING);
    auto ret = windowSceneSessionImpl->SetWindowLimits(windowLimits, false);
    EXPECT_EQ(WMError::WM_OK, ret);
    ret = windowSceneSessionImpl->SetWindowLimits(windowLimits, true);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetWindowLimits05
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetWindowLimits05, TestSize.Level1)
{
    sptr<WindowOption> subWindow = sptr<WindowOption>::MakeSptr();
    subWindow->SetWindowName("SetWindowLimits05");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(subWindow);
    WindowLimits windowLimits = { 1000, 1000, 1000, 1000, 0.0f, 0.0f };
    windowSceneSessionImpl->property_->SetPersistentId(1005);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> subSession = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSessionImpl->hostSession_ = subSession;
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, windowSceneSessionImpl->SetWindowLimits(windowLimits, false));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, windowSceneSessionImpl->SetWindowLimits(windowLimits, true));

    windowSceneSessionImpl->property_->SetWindowType(WindowType::BELOW_APP_SYSTEM_WINDOW_BASE);
    EXPECT_EQ(WMError::WM_OK, windowSceneSessionImpl->SetWindowLimits(windowLimits, false));
    EXPECT_EQ(WMError::WM_OK, windowSceneSessionImpl->SetWindowLimits(windowLimits, true));

    windowSceneSessionImpl->property_->SetDragEnabled(false);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, windowSceneSessionImpl->SetWindowLimits(windowLimits, false));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, windowSceneSessionImpl->SetWindowLimits(windowLimits, true));
}

/**
 * @tc.name: CalculateNewLimitsByRatio01
 * @tc.desc: CalculateNewLimitsByRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, CalculateNewLimitsByRatio01, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("CalculateNewLimitsByRatio01");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    WindowLimits newLimits = { 0, 0, 0, 0, 0.0, 0.0 };
    WindowLimits customizedLimits = { 3, 3, 3, 3, 0.0, 0.0 };
    windowSceneSessionImpl->CalculateNewLimitsByRatio(newLimits, customizedLimits);
    newLimits = { 500, 500, 500, 500, 0.0, 0.0 };
    customizedLimits = { 3, 3, 3, 3, 2.0, 2.0 };
    windowSceneSessionImpl->CalculateNewLimitsByRatio(newLimits, customizedLimits);
    customizedLimits = { 3, 3, 3, 3, 1.0, 1.0 };
    windowSceneSessionImpl->CalculateNewLimitsByRatio(newLimits, customizedLimits);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::BELOW_APP_SYSTEM_WINDOW_END);
    auto ret = windowSceneSessionImpl->UpdateAnimationFlagProperty(true);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: CalculateNewLimitsByRatio02
 * @tc.desc: CalculateNewLimitsByRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, CalculateNewLimitsByRatio02, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("CalculateNewLimitsByRatio02");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    WindowLimits newLimits = { 0, 0, 0, 0, 0.0, 0.0 };
    WindowLimits customizedLimits = { 3, 3, 3, 3, 0.0, 0.0 };
    windowSceneSessionImpl->CalculateNewLimitsByRatio(newLimits, customizedLimits);
    newLimits = { 500, 500, 500, 500, 0.0, 0.0 };
    customizedLimits = { 3, 3, 3, 3, 2.0, 2.0 };
    windowSceneSessionImpl->CalculateNewLimitsByRatio(newLimits, customizedLimits);
    customizedLimits = { 3, 3, 3, 3, 1.0, 1.0 };
    windowSceneSessionImpl->CalculateNewLimitsByRatio(newLimits, customizedLimits);
    windowSceneSessionImpl->property_->SetWindowType(WindowType::BELOW_APP_SYSTEM_WINDOW_BASE);
    auto ret = windowSceneSessionImpl->UpdateAnimationFlagProperty(true);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: MoveToAsync01
 * @tc.desc: MoveToAsync
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, MoveToAsync01, TestSize.Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("MoveToAsync01");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    option->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, window->MoveToAsync(10, 10));

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OP_IN_CUR_STATUS, window->MoveToAsync(10, 10));
}

/**
 * @tc.name: MoveToAsync02
 * @tc.desc: MoveToAsync
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, MoveToAsync02, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("MoveToAsync02");
    option->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(10001);
    Rect rect;
    WMError ret;
    ret = window->Create(abilityContext_, nullptr);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(WMError::WM_OK, ret);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);
        window->state_ = WindowState::STATE_SHOWN;
        ret = window->MoveToAsync(500, 500);
        EXPECT_EQ(WMError::WM_OK, ret);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(500, rect.posX_);
        EXPECT_EQ(500, rect.posY_);
        window->state_ = WindowState::STATE_HIDDEN;
        ret = window->MoveToAsync(20000, 20000);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(20000, rect.posX_);
        EXPECT_EQ(20000, rect.posY_);
        ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
    } else {
        EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    }
}

/**
 * @tc.name: ResizeAsync01
 * @tc.desc: ResizeAsync
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, ResizeAsync01, TestSize.Level0)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("ResizeAsync01");
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    option->SetWindowType(WindowType::WINDOW_TYPE_PIP);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->SetPersistentId(1);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OP_IN_CUR_STATUS, window->ResizeAsync(500, 500));
}

/**
 * @tc.name: ResizeAsync02
 * @tc.desc: ResizeAsync
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, ResizeAsync02, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("ResizeAsync02");
    option->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(10012);
    Rect rect;
    WMError ret;
    ret = window->Create(abilityContext_, nullptr);
    if (SceneBoardJudgement::IsSceneBoardEnabled()) {
        EXPECT_EQ(WMError::WM_OK, ret);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);
        WindowLimits windowLimits;
        ret = window->GetWindowLimits(windowLimits);
        EXPECT_EQ(WMError::WM_OK, ret);
        window->state_ = WindowState::STATE_SHOWN;
        ret = window->ResizeAsync(windowLimits.maxWidth_ - 100, windowLimits.maxHeight_ - 100);
        EXPECT_EQ(WMError::WM_OK, ret);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(windowLimits.maxWidth_ - 100, rect.width_);
        EXPECT_EQ(windowLimits.maxHeight_ - 100, rect.height_);
        window->state_ = WindowState::STATE_HIDDEN;
        ret = window->ResizeAsync(windowLimits.maxWidth_ + 100, windowLimits.maxHeight_ + 100);
        EXPECT_EQ(WMError::WM_OK, ret);
        usleep(WAIT_SERVERAL_FRAMES);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(windowLimits.maxWidth_, rect.width_);
        EXPECT_EQ(windowLimits.maxHeight_, rect.height_);
        ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
    } else {
        EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    }
}

/**
 * @tc.name: MoveTo01
 * @tc.desc: MoveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, MoveTo01, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("MoveTo01");
    sptr<WindowSceneSessionImpl> windowSceneSession = sptr<WindowSceneSessionImpl>::MakeSptr(option);

    windowSceneSession->property_->SetPersistentId(1);
    windowSceneSession->state_ = WindowState::STATE_HIDDEN;
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSession->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, windowSceneSession->MoveTo(2, 2));
}

/**
 * @tc.name: MoveTo02
 * @tc.desc: MoveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, MoveTo02, TestSize.Level1)
{
    sptr<WindowOption> subOption = sptr<WindowOption>::MakeSptr();
    subOption->SetWindowName("MoveTo02SubWindow");
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = sptr<WindowSceneSessionImpl>::MakeSptr(subOption);
    subWindow->property_->SetPersistentId(1001);
    SessionInfo subSessionInfo = { "CreateSubTestBundle", "CreateSubTestModule", "CreateSubTestAbility" };
    sptr<SessionMocker> subSession = sptr<SessionMocker>::MakeSptr(subSessionInfo);
    subWindow->hostSession_ = subSession;
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(2, 2));

    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("MoveTo02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle2", "CreateTestModule2", "CreateTestAbility2" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->state_ = WindowState::STATE_SHOWN;
    ASSERT_EQ(WMError::WM_OK, window->Create(abilityContext_, session));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(3, 3));
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(3, 3));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(3, 4));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(4, 4));
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(4, 4));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(4, 5));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(5, 5));
    ASSERT_EQ(WMError::WM_OK, window->Destroy(true));
}

/**
 * @tc.name: MoveTo03
 * @tc.desc: MoveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, MoveTo03, TestSize.Level1)
{
    sptr<WindowOption> subOption = sptr<WindowOption>::MakeSptr();
    subOption->SetWindowName("MoveTo01SubWindow");
    subOption->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowSceneSessionImpl> subWindow = sptr<WindowSceneSessionImpl>::MakeSptr(subOption);
    subWindow->property_->SetPersistentId(1001);
    SessionInfo subSessionInfo = { "CreateSubTestBundle", "CreateSubTestModule", "CreateSubTestAbility" };
    sptr<SessionMocker> subSession = sptr<SessionMocker>::MakeSptr(subSessionInfo);
    subWindow->hostSession_ = subSession;
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(2, 2));

    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("MoveTo02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    WindowSceneSessionImpl::windowSessionMap_.insert(std::make_pair(
        window->GetWindowName(), std::pair<uint64_t, sptr<WindowSessionImpl>>(window->GetWindowId(), window)));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(3, 3));
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(4, 3));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(4, 4));
    window->property_->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(5, 4));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(5, 4));
    WindowSceneSessionImpl::windowSessionMap_.erase(window->GetWindowName());
}

/**
 * @tc.name: UpdateWindowModeForUITest01
 * @tc.desc: UpdateWindowModeForUITest
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, UpdateWindowModeForUITest01, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("UpdateWindowModeForUITest01");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    const int32_t windowId = 1;
    window->property_->SetPersistentId(windowId);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;

    WMError res = window->UpdateWindowModeForUITest(static_cast<int32_t>(WindowMode::WINDOW_MODE_UNDEFINED));
    EXPECT_EQ(WMError::WM_DO_NOTHING, res);
    res = window->UpdateWindowModeForUITest(static_cast<int32_t>(WindowMode::WINDOW_MODE_FULLSCREEN));
    EXPECT_EQ(WMError::WM_OK, res);
    res = window->UpdateWindowModeForUITest(static_cast<int32_t>(WindowMode::WINDOW_MODE_SPLIT_PRIMARY));
    EXPECT_EQ(WMError::WM_OK, res);
    res = window->UpdateWindowModeForUITest(static_cast<int32_t>(WindowMode::WINDOW_MODE_SPLIT_SECONDARY));
    EXPECT_EQ(WMError::WM_OK, res);
    res = window->UpdateWindowModeForUITest(static_cast<int32_t>(WindowMode::WINDOW_MODE_FLOATING));
    EXPECT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: GetAppHookWindowInfoFromServer
 * @tc.desc: GetAppHookWindowInfoFromServer
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, GetAppHookWindowInfoFromServer, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetAppHookWindowInfoFromServer");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    const int32_t windowId = 2025;
    window->property_->SetPersistentId(windowId);

    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    HookWindowInfo hookWindowInfo;
    WMError res = window->GetAppHookWindowInfoFromServer(hookWindowInfo);
    EXPECT_NE(res, WMError::WM_ERROR_INVALID_WINDOW);
}

/**
 * @tc.name: NotifyAppHookWindowInfoUpdated
 * @tc.desc: NotifyAppHookWindowInfoUpdated
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, NotifyAppHookWindowInfoUpdated, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("NotifyAppHookWindowInfoUpdated");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    const int32_t windowId = 2025;
    window->property_->SetPersistentId(windowId);

    // Case 1: GetAppHookWindowInfoFromServer failed
    window->hostSession_ = nullptr;
    WSError res = window->NotifyAppHookWindowInfoUpdated();
    EXPECT_EQ(res, WSError::WS_DO_NOTHING);

    // Case 2: success
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    res = window->NotifyAppHookWindowInfoUpdated();
    EXPECT_EQ(res, WSError::WS_OK);

    // Case 3: not mainWindow
    window->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    res = window->NotifyAppHookWindowInfoUpdated();
    EXPECT_EQ(res, WSError::WS_DO_NOTHING);
}

/**
 * @tc.name: GetGlobalScaledRect
 * @tc.desc: GetGlobalScaledRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, GetGlobalScaledRect, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetGlobalScaledRect");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    const int32_t windowId = 2025;
    window->property_->SetPersistentId(windowId);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    HookWindowInfo hookWindowInfo;
    hookWindowInfo.enableHookWindow = true;
    hookWindowInfo.widthHookRatio = 0.5f;
    window->SetAppHookWindowInfo(hookWindowInfo);
    Rect globalScaledRect = { 0, 0, 800, 800 };
    WMError res = window->GetGlobalScaledRect(globalScaledRect);
    EXPECT_EQ(res, WMError::WM_OK);
    EXPECT_NE(globalScaledRect.width_, 800);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
