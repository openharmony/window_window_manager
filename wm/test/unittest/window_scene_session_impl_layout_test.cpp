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
#include "window_adapter.h"
#include "window_scene_session_impl.h"
#include "window_session_impl.h"
#include "wm_common.h"

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
HWTEST_F(WindowSceneSessionImplLayoutTest, SetAspectRatio01, Function | SmallTest | Level3)
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
HWTEST_F(WindowSceneSessionImplLayoutTest, SetAspectRatio02, Function | SmallTest | Level3)
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
HWTEST_F(WindowSceneSessionImplLayoutTest, SetAspectRatio03, Function | SmallTest | Level2)
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
 * @tc.name: ResetAspectRatio01
 * @tc.desc: ResetAspectRatio test GetAvoidAreaByType
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, ResetAspectRatio01, Function | SmallTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("ResetAspectRatio01");
    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    ASSERT_EQ(WMError::WM_OK, window->ResetAspectRatio());
    ASSERT_EQ(0, session->GetAspectRatio());
}

/**
 * @tc.name: ResetAspectRatio02
 * @tc.desc: ResetAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, ResetAspectRatio02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("ResetAspectRatio02");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    windowSceneSessionImpl->hostSession_ = nullptr;
    auto ret = windowSceneSessionImpl->ResetAspectRatio();
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret);
    windowSceneSessionImpl->property_->SetPersistentId(1);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_CREATED;
    ret = windowSceneSessionImpl->ResetAspectRatio();
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetWindowLimits01
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetWindowLimits01, Function | SmallTest | Level2)
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

    WindowLimits windowLimits = { 2000, 2000, 2000, 2000, 0.0f, 0.0f };
    ASSERT_EQ(WMError::WM_OK, window->SetWindowLimits(windowLimits));
    WindowLimits windowSizeLimits = window->property_->GetWindowLimits();
    ASSERT_EQ(windowSizeLimits.maxWidth_, 2000);
    ASSERT_EQ(windowSizeLimits.maxHeight_, 2000);
    ASSERT_EQ(windowSizeLimits.minWidth_, 2000);
    ASSERT_EQ(windowSizeLimits.minHeight_, 2000);
}

/**
 * @tc.name: SetWindowLimits02
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetWindowLimits02, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetWindowLimits02");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    WindowLimits windowLimits = { 1000, 1000, 1000, 1000, 0.0f, 0.0f };

    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    auto ret = windowSceneSessionImpl->SetWindowLimits(windowLimits);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);
}

/**
 * @tc.name: SetWindowLimits03
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetWindowLimits03, Function | SmallTest | Level2)
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
    auto ret = windowSceneSessionImpl->SetWindowLimits(windowLimits);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetWindowLimits04
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetWindowLimits04, Function | SmallTest | Level2)
{
    sptr<WindowOption> subWindow = sptr<WindowOption>::MakeSptr();
    subWindow->SetWindowName("SetWindowLimits04");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(subWindow);
    WindowLimits windowLimits = { 1000, 1000, 1000, 1000, 0.0f, 0.0f };
    windowSceneSessionImpl->SetWindowLimits(windowLimits);
    windowSceneSessionImpl->property_->SetPersistentId(1004);
    windowSceneSessionImpl->property_->SetDisplayId(0);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSessionImpl->hostSession_ = session;
    windowSceneSessionImpl->state_ = WindowState::STATE_CREATED;
    windowSceneSessionImpl->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_LAUNCHING);
    auto ret = windowSceneSessionImpl->SetWindowLimits(windowLimits);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: SetWindowLimits05
 * @tc.desc: SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetWindowLimits05, Function | SmallTest | Level2)
{
    sptr<WindowOption> subWindow = sptr<WindowOption>::MakeSptr();
    subWindow->SetWindowName("SetWindowLimits05");
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(subWindow);
    WindowLimits windowLimits = { 1000, 1000, 1000, 1000, 0.0f, 0.0f };
    windowSceneSessionImpl->SetWindowLimits(windowLimits);
    windowSceneSessionImpl->property_->SetPersistentId(1005);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> subSession = sptr<SessionMocker>::MakeSptr(sessionInfo);
    windowSceneSessionImpl->hostSession_ = subSession;
    windowSceneSessionImpl->property_->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_CALLING, windowSceneSessionImpl->SetWindowLimits(windowLimits));
}

/**
 * @tc.name: CalculateNewLimitsByRatio01
 * @tc.desc: CalculateNewLimitsByRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, CalculateNewLimitsByRatio01, Function | SmallTest | Level2)
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
HWTEST_F(WindowSceneSessionImplLayoutTest, CalculateNewLimitsByRatio02, Function | SmallTest | Level2)
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
HWTEST_F(WindowSceneSessionImplLayoutTest, MoveToAsync01, Function | SmallTest | Level2)
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
HWTEST_F(WindowSceneSessionImplLayoutTest, MoveToAsync02, Function | SmallTest | Level2)
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
HWTEST_F(WindowSceneSessionImplLayoutTest, ResizeAsync01, Function | SmallTest | Level2)
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
HWTEST_F(WindowSceneSessionImplLayoutTest, ResizeAsync02, Function | SmallTest | Level2)
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
HWTEST_F(WindowSceneSessionImplLayoutTest, MoveTo01, Function | SmallTest | Level2)
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
HWTEST_F(WindowSceneSessionImplLayoutTest, MoveTo02, Function | SmallTest | Level2)
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
HWTEST_F(WindowSceneSessionImplLayoutTest, MoveTo03, Function | SmallTest | Level2)
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
 * @tc.name: moveWindowTo01
 * @tc.desc: test moveWindowTo for ALN/PC with windowMode: 102, windowType: 2107
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, moveWindowTo01, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("moveWindowTo01");
    option->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (window->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        window->property_->SetPersistentId(10001);
        Rect rect;
        WMError ret;
        ret = window->Create(abilityContext_, nullptr);
        EXPECT_EQ(WMError::WM_OK, ret);

        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);

        ret = window->MoveTo(-500, -500);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(-500, rect.posX_);
        EXPECT_EQ(-500, rect.posY_);

        ret = window->MoveTo(0, 0);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(0, rect.posX_);
        EXPECT_EQ(0, rect.posY_);

        ret = window->MoveTo(500, 500);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(500, rect.posX_);
        EXPECT_EQ(500, rect.posY_);

        ret = window->MoveTo(20000, 20000);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(20000, rect.posX_);
        EXPECT_EQ(20000, rect.posY_);
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(window->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: moveWindowTo02
 * @tc.desc: test moveWindowTo for ALN with windowMode: 102, windowType: 1001
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, moveWindowTo02, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("moveWindowTo02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (window->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";

        SessionInfo sessionInfo = { "bundleName_moveWindowTo02",
                                    "moduleName_moveWindowTo02",
                                    "abilityName_moveWindowTo02" };
        sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

        Rect rectOld;
        Rect rectNow;
        WMError ret;
        ret = window->Create(abilityContext_, sceneSession);
        EXPECT_EQ(WMError::WM_OK, ret);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);
        window->property_->SetPersistentId(10002);

        rectOld = window->property_->GetWindowRect();
        ret = window->MoveTo(-500, -500);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rectNow = window->property_->GetWindowRect();
        EXPECT_EQ(rectOld.posX_, rectNow.posX_);
        EXPECT_EQ(rectOld.posY_, rectNow.posY_);

        rectOld = window->property_->GetWindowRect();
        ret = window->MoveTo(0, 0);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rectNow = window->property_->GetWindowRect();
        EXPECT_EQ(rectOld.posX_, rectNow.posX_);
        EXPECT_EQ(rectOld.posY_, rectNow.posY_);

        rectOld = window->property_->GetWindowRect();
        ret = window->MoveTo(500, 500);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rectNow = window->property_->GetWindowRect();
        EXPECT_EQ(rectOld.posX_, rectNow.posX_);
        EXPECT_EQ(rectOld.posY_, rectNow.posY_);
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(window->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: moveWindowTo03
 * @tc.desc: test moveWindowTo for ALN with windowMode: 1, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, moveWindowTo03, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("moveWindowTo03");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (window->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        window->property_->SetPersistentId(10003);

        SessionInfo sessionInfo = { "bundleName_moveWindowTo03",
                                    "moduleName_moveWindowTo03",
                                    "abilityName_moveWindowTo03" };
        sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

        Rect rectOld;
        Rect rectNow;
        WMError ret;
        ret = window->Create(abilityContext_, sceneSession);
        EXPECT_EQ(WMError::WM_OK, ret);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);

        rectOld = window->property_->GetWindowRect();
        ret = window->MoveTo(-500, -500);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rectNow = window->property_->GetWindowRect();
        EXPECT_EQ(rectOld.posX_, rectNow.posX_);
        EXPECT_EQ(rectOld.posY_, rectNow.posY_);

        rectOld = window->property_->GetWindowRect();
        ret = window->MoveTo(0, 0);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rectNow = window->property_->GetWindowRect();
        EXPECT_EQ(rectOld.posX_, rectNow.posX_);
        EXPECT_EQ(rectOld.posY_, rectNow.posY_);

        rectOld = window->property_->GetWindowRect();
        ret = window->MoveTo(500, 500);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rectNow = window->property_->GetWindowRect();
        EXPECT_EQ(rectOld.posX_, rectNow.posX_);
        EXPECT_EQ(rectOld.posY_, rectNow.posY_);

        rectOld = window->property_->GetWindowRect();
        ret = window->MoveTo(20000, 20000);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rectNow = window->property_->GetWindowRect();
        EXPECT_EQ(rectOld.posX_, rectNow.posX_);
        EXPECT_EQ(rectOld.posY_, rectNow.posY_);
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(window->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: moveWindowTo04
 * @tc.desc: test moveWindowTo for ALN with windowMode: 100, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, moveWindowTo04, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("moveWindowTo04");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (window->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        window->property_->SetPersistentId(10004);

        SessionInfo sessionInfo = { "bundleName_moveWindowTo04",
                                    "moduleName_moveWindowTo04",
                                    "abilityName_moveWindowTo04" };
        sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

        Rect rectOld;
        Rect rectNow;
        WMError ret;
        ret = window->Create(abilityContext_, sceneSession);
        EXPECT_EQ(WMError::WM_OK, ret);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);

        rectOld = window->property_->GetWindowRect();
        ret = window->MoveTo(-500, -500);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rectNow = window->property_->GetWindowRect();
        EXPECT_EQ(rectOld.posX_, rectNow.posX_);
        EXPECT_EQ(rectOld.posY_, rectNow.posY_);

        rectOld = window->property_->GetWindowRect();
        ret = window->MoveTo(0, 0);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rectNow = window->property_->GetWindowRect();
        EXPECT_EQ(rectOld.posX_, rectNow.posX_);
        EXPECT_EQ(rectOld.posY_, rectNow.posY_);

        rectOld = window->property_->GetWindowRect();
        ret = window->MoveTo(500, 500);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rectNow = window->property_->GetWindowRect();
        EXPECT_EQ(rectOld.posX_, rectNow.posX_);
        EXPECT_EQ(rectOld.posY_, rectNow.posY_);

        rectOld = window->property_->GetWindowRect();
        ret = window->MoveTo(20000, 20000);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);
        rectNow = window->property_->GetWindowRect();
        EXPECT_EQ(rectOld.posX_, rectNow.posX_);
        EXPECT_EQ(rectOld.posY_, rectNow.posY_);
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(window->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: resize01
 * @tc.desc: test resize for ALN/PC with windowMode: 102, windowType: 2107
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, resize01, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize01");
    option->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (window->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        Rect rect;
        WMError ret;
        ret = window->Create(abilityContext_, nullptr);
        EXPECT_EQ(WMError::WM_OK, ret);
        window->property_->SetPersistentId(10008);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);

        WindowLimits windowLimits;
        ret = window->GetWindowLimits(windowLimits);
        EXPECT_EQ(WMError::WM_OK, ret);

        ret = window->Resize(-500, -500);
        EXPECT_EQ(WMError::WM_OK, ret);
        usleep(WAIT_SERVERAL_FRAMES);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(windowLimits.maxWidth_, rect.width_);
        EXPECT_EQ(windowLimits.maxHeight_, rect.height_);

        ret = window->Resize(500, 500);
        EXPECT_EQ(WMError::WM_OK, ret);
        usleep(WAIT_SERVERAL_FRAMES);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(500, rect.width_);
        EXPECT_EQ(500, rect.height_);

        ret = window->Resize(20000, 20000);
        EXPECT_EQ(WMError::WM_OK, ret);
        usleep(WAIT_SERVERAL_FRAMES);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(windowLimits.maxWidth_, rect.width_);
        EXPECT_EQ(windowLimits.maxHeight_, rect.height_);

        ret = window->Resize(0, 0);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret); // check parameter first
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(window->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: resize02
 * @tc.desc: test resize for ALN with windowMode: 1, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, resize02, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize02");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (window->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        SessionInfo sessionInfo = { "bundleName_resize02", "moduleName_resize02", "abilityName_resize02" };
        sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

        WMError ret;
        ret = window->Create(abilityContext_, sceneSession);
        EXPECT_EQ(WMError::WM_OK, ret);
        window->property_->SetPersistentId(10009);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);

        ret = window->Resize(500, 500);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

        ret = window->Resize(20000, 20000);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

        ret = window->Resize(0, 0);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret); // check parameter first
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(window->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: resize03
 * @tc.desc: test resize for PC with windowMode: 1, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, resize03, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize03");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (window->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        SessionInfo sessionInfo = { "bundleName_resize03", "moduleName_resize03", "abilityName_resize03" };
        sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

        WMError ret;
        ret = window->Create(abilityContext_, sceneSession);
        EXPECT_EQ(WMError::WM_OK, ret);
        window->property_->SetPersistentId(100010);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);

        ret = window->Resize(500, 500);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

        ret = window->Resize(20000, 20000);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

        ret = window->Resize(0, 0);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret); // check parameter first
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(window->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: resize04
 * @tc.desc: test resize for ALN/PC with windowMode: 100, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, resize04, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize04");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (window->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        SessionInfo sessionInfo = { "bundleName_resize04", "moduleName_resize04", "abilityName_resize04" };
        sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

        WMError ret;
        ret = window->Create(abilityContext_, sceneSession);
        EXPECT_EQ(WMError::WM_OK, ret);
        window->property_->SetPersistentId(100011);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);

        ret = window->Resize(500, 500);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

        ret = window->Resize(20000, 20000);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

        ret = window->Resize(0, 0);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_PARAM, ret); // check parameter first
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(window->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: resize05
 * @tc.desc: test resize for ALN/PC with windowMode: 102, windowType: 2107
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, resize05, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize05");
    option->SetWindowType(WindowType::WINDOW_TYPE_TOAST);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (window->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        Rect rect;
        WMError ret;
        ret = window->Create(abilityContext_, nullptr);
        EXPECT_EQ(WMError::WM_OK, ret);
        window->property_->SetPersistentId(10012);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);

        WindowLimits windowLimits;
        ret = window->GetWindowLimits(windowLimits);

        ret = window->Resize(windowLimits.maxWidth_ - 100, windowLimits.maxHeight_ - 100);
        EXPECT_EQ(WMError::WM_OK, ret);
        usleep(WAIT_SERVERAL_FRAMES);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(windowLimits.maxWidth_ - 100, rect.width_);
        EXPECT_EQ(windowLimits.maxHeight_ - 100, rect.height_);

        ret = window->Resize(windowLimits.maxWidth_ + 100, windowLimits.maxHeight_ + 100);
        EXPECT_EQ(WMError::WM_OK, ret);
        usleep(WAIT_SERVERAL_FRAMES);
        rect = window->property_->GetWindowRect();
        EXPECT_EQ(windowLimits.maxWidth_, rect.width_);
        EXPECT_EQ(windowLimits.maxHeight_, rect.height_);
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(window->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: resize06
 * @tc.desc: test resize for ALN with windowMode: 1, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, resize06, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize06");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (window->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        SessionInfo sessionInfo = { "bundleName_resize06", "moduleName_resize06", "abilityName_resize06" };
        sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

        WMError ret;
        ret = window->Create(abilityContext_, sceneSession);
        EXPECT_EQ(WMError::WM_OK, ret);
        window->property_->SetPersistentId(100013);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);

        WindowLimits windowLimits;
        ret = window->GetWindowLimits(windowLimits);

        ret = window->Resize(windowLimits.maxWidth_ - 100, windowLimits.maxHeight_ - 100);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

        ret = window->Resize(windowLimits.maxWidth_ + 100, windowLimits.maxHeight_ + 100);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(window->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: resize07
 * @tc.desc: test resize for PC with windowMode: 1, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, resize07, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize07");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (window->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        SessionInfo sessionInfo = { "bundleName_resize07", "moduleName_resize07", "abilityName_resize07" };
        sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

        WMError ret;
        ret = window->Create(abilityContext_, sceneSession);
        EXPECT_EQ(WMError::WM_OK, ret);
        window->property_->SetPersistentId(100014);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);

        WindowLimits windowLimits;
        ret = window->GetWindowLimits(windowLimits);

        ret = window->Resize(windowLimits.maxWidth_ - 100, windowLimits.maxHeight_ - 100);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);

        ret = window->Resize(windowLimits.maxWidth_ + 100, windowLimits.maxHeight_ + 100);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(window->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: resize08
 * @tc.desc: test resize for ALN/PC with windowMode: 100, windowType: 1
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, resize08, Function | MediumTest | Level3)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("resize08");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);

    sptr<WindowSceneSessionImpl> window = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (window->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        SessionInfo sessionInfo = { "bundleName_resize08", "moduleName_resize08", "abilityName_resize08" };
        sptr<SceneSession> sceneSession = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);

        WMError ret;
        ret = window->Create(abilityContext_, sceneSession);
        EXPECT_EQ(WMError::WM_OK, ret);
        window->property_->SetPersistentId(100015);
        ret = window->Show();
        EXPECT_EQ(WMError::WM_OK, ret);

        WindowLimits windowLimits;
        ret = window->GetWindowLimits(windowLimits);

        ret = window->Resize(windowLimits.maxWidth_ - 100, windowLimits.maxHeight_ - 100);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
        ret = window->Resize(windowLimits.maxWidth_ + 100, windowLimits.maxHeight_ + 100);
        EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, ret);
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(window->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: SetWindowLimitsDataRoute
 * @tc.desc: test data route for SetWindowLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetWindowLimitsDataRoute, Function | MediumTest | Level3)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "### begin ###");
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetWindowLimitsDataRoute");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (windowSceneSessionImpl->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        windowSceneSessionImpl->property_->SetPersistentId(1);
        windowSceneSessionImpl->property_->SetDisplayId(0);

        SessionInfo sessionInfo = { "CeateTestBundle", "CreateTestModule", "CreateTestAbility" };
        sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

        windowSceneSessionImpl->hostSession_ = session;
        windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;

        WindowLimits windowLimits1 = { 4000, 4000, 2000, 2000, 0.0f, 0.0f };
        WMError ret = windowSceneSessionImpl->SetWindowLimits(windowLimits1);
        EXPECT_EQ(WMError::WM_OK, ret);
        auto windowProperty = windowSceneSessionImpl->GetProperty();
        ASSERT_NE(nullptr, windowProperty);

        WindowLimits windowLimits2 = windowProperty->GetWindowLimits();
        EXPECT_EQ(windowLimits1.maxWidth_, windowLimits2.maxWidth_);
        EXPECT_EQ(windowLimits1.maxHeight_, windowLimits2.maxHeight_);
        EXPECT_EQ(windowLimits1.minWidth_, windowLimits2.minWidth_);
        EXPECT_EQ(windowLimits1.minHeight_, windowLimits2.minHeight_);
        TLOGI(WmsLogTag::WMS_LAYOUT, "### end ###");
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(windowSceneSessionImpl->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
}

/**
 * @tc.name: SetAspectRatioDataRoute
 * @tc.desc: test data route for SetAspectRatioDataRoute
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, SetAspectRatioDataRoute, Function | MediumTest | Level3)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "### begin ###");
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetAspectRatioDataRoute");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (windowSceneSessionImpl->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        windowSceneSessionImpl->property_->SetPersistentId(1);
        windowSceneSessionImpl->property_->SetDisplayId(0);

        SessionInfo sessionInfo = { "CeateTestBundle", "CreateTestModule", "CreateTestAbility" };
        sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

        windowSceneSessionImpl->hostSession_ = session;
        windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;

        WindowLimits windowLimits1 = { 4000, 4000, 2000, 2000, 0.0f, 0.0f };
        WMError ret = windowSceneSessionImpl->SetWindowLimits(windowLimits1);
        EXPECT_EQ(WMError::WM_OK, ret);
        const float ratio = 1.5;

        ret = windowSceneSessionImpl->SetAspectRatio(ratio);
        EXPECT_EQ(WMError::WM_OK, ret);
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(windowSceneSessionImpl->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "### end ###");
}

/**
 * @tc.name: moveToDataRoute
 * @tc.desc: test data route for moveTo
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, moveToDataRoute, Function | MediumTest | Level3)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "### begin ###");
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("moveToDataRoute");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (windowSceneSessionImpl->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        windowSceneSessionImpl->property_->SetPersistentId(1);
        windowSceneSessionImpl->property_->SetDisplayId(0);

        SessionInfo sessionInfo = { "CeateTestBundle", "CreateTestModule", "CreateTestAbility" };
        sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);

        windowSceneSessionImpl->hostSession_ = session;
        windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;

        WMError ret = windowSceneSessionImpl->MoveTo(500, 500);
        usleep(WAIT_SERVERAL_FRAMES);
        EXPECT_EQ(WMError::WM_OK, ret);

        Rect rect = windowSceneSessionImpl->property_->GetRequestRect();
        EXPECT_EQ(500, rect.posX_);
        EXPECT_EQ(500, rect.posY_);
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(windowSceneSessionImpl->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "### end ###");
}

/**
 * @tc.name: ResizeDataRoute
 * @tc.desc: test data route for Resize
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, ResizeDataRoute, Function | MediumTest | Level3)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "### begin ###");
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("ResizeDataRoute");
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (windowSceneSessionImpl->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        windowSceneSessionImpl->property_->SetPersistentId(1);
        windowSceneSessionImpl->property_->SetDisplayId(0);

        SessionInfo sessionInfo = { "CeateTestBundle", "CreateTestModule", "CreateTestAbility" };
        sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
        session->isActive_ = true;
        session->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
        session->SetSessionState(SessionState::STATE_FOREGROUND);
        windowSceneSessionImpl->hostSession_ = session;
        windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;

        WMError ret = windowSceneSessionImpl->Resize(500, 500);
        EXPECT_EQ(WMError::WM_OK, ret);

        Rect rect = windowSceneSessionImpl->property_->GetRequestRect();
        EXPECT_EQ(500, rect.width_);
        EXPECT_EQ(500, rect.height_);

        WSRect wsRect = { rect.posX_, rect.posY_, rect.width_, rect.height_ };
        WSError ret2 = session->UpdateSessionRect(wsRect, SizeChangeReason::RESIZE, false);
        EXPECT_EQ(WSError::WS_OK, ret2);

        usleep(WAIT_SERVERAL_FRAMES);
        Rect rect2 = session->property_->GetRequestRect();
        EXPECT_EQ(500, rect2.width_);
        EXPECT_EQ(500, rect2.height_);
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(windowSceneSessionImpl->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "### end ###");
}

/**
 * @tc.name: AdjustRectByAspectRatio
 * @tc.desc: test AdjustRectByAspectRatio
 * @tc.type: FUNC
 */
HWTEST_F(WindowSceneSessionImplLayoutTest, AdjustRectByAspectRatio, Function | MediumTest | Level0)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "### begin ###");
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("AdjustRectByAspectRatio");
    option->SetWindowType(WindowType::APP_WINDOW_BASE);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowSceneSessionImpl> windowSceneSessionImpl = sptr<WindowSceneSessionImpl>::MakeSptr(option);
    if (windowSceneSessionImpl->windowSystemConfig_.windowUIType_ == WindowUIType::PHONE_WINDOW) {
        GTEST_LOG_(INFO) << "windowUIType = WindowUIType::PHONE_WINDOW";
        windowSceneSessionImpl->property_->SetPersistentId(1);
        windowSceneSessionImpl->property_->SetDisplayId(0);

        SessionInfo sessionInfo = { "CeateTestBundle", "CreateTestModule", "CreateTestAbility" };
        sptr<SceneSession> session = sptr<SceneSession>::MakeSptr(sessionInfo, nullptr);
        session->isActive_ = true;
        session->property_->SetWindowType(WindowType::APP_WINDOW_BASE);
        session->property_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
        session->SetSessionState(SessionState::STATE_FOREGROUND);

        windowSceneSessionImpl->hostSession_ = session;
        windowSceneSessionImpl->state_ = WindowState::STATE_SHOWN;

        WindowLimits windowLimits1 = { 3000, 3000, 1200, 1200, 0.0f, 0.0f };
        WMError wmRet1 = windowSceneSessionImpl->SetWindowLimits(windowLimits1);
        EXPECT_EQ(WMError::WM_OK, wmRet1);

        WMError wmRet2 = windowSceneSessionImpl->Resize(1600, 1600);
        EXPECT_EQ(WMError::WM_OK, wmRet2);

        float ratio = 0.8;
        WMError wmRet3 = windowSceneSessionImpl->SetAspectRatio(ratio);
        EXPECT_EQ(WMError::WM_OK, wmRet3);

        // 服务端执行相同的逻辑
        auto property = windowSceneSessionImpl->property_;
        WSPropertyChangeAction action = WSPropertyChangeAction::ACTION_UPDATE_WINDOW_LIMITS;
        WMError wsRet1 = session->UpdateSessionPropertyByAction(property, action);
        EXPECT_EQ(WMError::WM_OK, wsRet1);

        Rect rect = windowSceneSessionImpl->property_->GetRequestRect();
        WSRect wsRect = { rect.posX_, rect.posY_, rect.width_, rect.height_ };
        // 异步接口加一个延迟
        WSError wsRet2 = session->UpdateSessionRect(wsRect, SizeChangeReason::RESIZE, false);
        EXPECT_EQ(WSError::WS_OK, wsRet2);
        usleep(WAIT_SERVERAL_FRAMES);
        WSError wsRet3 = session->UpdateRect(wsRect, SizeChangeReason::RESIZE, "AdjustRectByAspectRatio", nullptr);
        EXPECT_EQ(WSError::WS_OK, wsRet3);
        usleep(WAIT_SERVERAL_FRAMES);
        WSError wsRet4 = session->SetAspectRatio(ratio);
        EXPECT_EQ(WSError::WS_OK, wsRet4);
    } else {
        GTEST_LOG_(INFO) << "windowUIType != WindowUIType::PHONE_WINDOW";
        ASSERT_NE(windowSceneSessionImpl->windowSystemConfig_.windowUIType_, WindowUIType::PHONE_WINDOW);
    }
    TLOGI(WmsLogTag::WMS_LAYOUT, "### end ###");
}
} // namespace
} // namespace Rosen
} // namespace OHOS
