/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include "ability_context_impl.h"
#include "float_view_controller.h"
#include "float_window_manager.h"
#include "native_engine/native_engine.h"
#include "window.h"
#include "wm_common.h"

#define FloatViewManager MockFloatViewManager
#define FloatWindowManager MockFloatWindowManager

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class MockWindow : public Window {
public:
    WindowState state_ = WindowState::STATE_INITIAL;
    const uint32_t mockWindowId_ = 101;
    MockWindow() {};
    ~MockWindow() {};
    MOCK_METHOD3(Show, WMError(uint32_t reason, bool withAnimation, bool withFocus));
    MOCK_METHOD1(Destroy, WMError(uint32_t reason));
    MOCK_METHOD0(NotifyPrepareCloseFloatView, void());
    MOCK_METHOD1(UpdateFloatView, WMError(const FloatViewTemplateInfo& fvTemplateInfo));
    MOCK_METHOD1(RestoreFloatViewMainWindow, WMError(const std::shared_ptr<AAFwk::WantParams>& wantParams));
    MOCK_METHOD6(NapiSetUIContent, WMError(const std::string& contentUrl, napi_env env, napi_value storage,
        BackupAndRestoreType type, sptr<IRemoteObject> token, AppExecFwk::Ability* ability));
    MOCK_METHOD1(RegisterLifeCycleListener, WMError(const sptr<IWindowLifeCycle>& listener));
    MOCK_METHOD1(UnregisterLifeCycleListener, WMError(const sptr<IWindowLifeCycle>& listener));
    uint32_t GetWindowId() const override
    {
        return mockWindowId_;
    }

    void SetWindowState(const WindowState& state)
    {
        state_ = state;
    }

    WindowState GetWindowState() const override
    {
        return state_;
    }
};

class MockStateChangeListener : public IFvStateChangeObserver {
public:
    void OnStateChange(const FloatViewState& state, const std::string& stopReason) override
    {
        return;
    }
};

class MockRectChangeListener : public IFvRectChangeObserver {
public:
    void OnRectangleChange(const Rect& rect, double scale, const std::string& reason) override
    {
        return;
    }
};

class MockLimitsChangeListener : public IFvLimitsChangeObserver {
public:
    void OnLimitsChange(const FloatViewLimits& limits) override
    {
        return;
    }
};

class MockFloatViewManager {
public:
    static bool HasActiveController() { return hasActiveController_; }
    static bool IsActiveController(const wptr<FloatViewController>& fvControllerWeak) { return isActiveController_; }
    static void SetActiveController(const wptr<FloatViewController>& fvControllerWeak) { hasActiveController_ = true; }
    static void RemoveActiveController(const wptr<FloatViewController>& fvControllerWeak) { hasActiveController_ = false; }
    static void DoActionClose(const std::string& reason) {}

    static bool hasActiveController_;
    static bool isActiveController_;
};

bool MockFloatViewManager::hasActiveController_ = false;
bool MockFloatViewManager::isActiveController_ = false;

class MockFloatWindowManager {
public:
    static std::string GetControllerId() { return "mock_controller_id"; }
    static uint64_t AcquireToken() { return token_; }
    static void ReleaseToken(uint64_t token) {}
    static WMError StartBindFloatView(const wptr<FloatViewController>& fvControllerWeak) { return WMError::WM_ERROR_INVALID_OPERATION; }
    static bool IsFloatViewConflict(const wptr<FloatViewController>& selfController) { return isFloatViewConflict_; }
    static WMError StopBindFloatView(const wptr<FloatViewController>& fvControllerWeak) { return WMError::WM_ERROR_INVALID_OPERATION; }

    static uint64_t token_;
    static bool isFloatViewConflict_;
};

uint64_t MockFloatWindowManager::token_ = 0;
bool MockFloatWindowManager::isFloatViewConflict_ = false;

class FloatViewControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<FloatViewController> fvController_;
    sptr<FvOption> option_;
    sptr<MockWindow> mw_;
};

void FloatViewControllerTest::SetUpTestCase()
{
}

void FloatViewControllerTest::TearDownTestCase()
{
}

void FloatViewControllerTest::SetUp()
{
    mw_ = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw_);
    option_ = sptr<FvOption>::MakeSptr();
    ASSERT_NE(nullptr, option_);
    fvController_ = sptr<FloatViewController>::MakeSptr(*option_, nullptr);
    ASSERT_NE(nullptr, fvController_);
}

void FloatViewControllerTest::TearDown()
{
    fvController_ = nullptr;
    option_ = nullptr;
    mw_ = nullptr;
}

namespace {
/**
 * @tc.name: GetCurState
 * @tc.desc: GetCurState
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, GetCurState, TestSize.Level1)
{
}

/**
 * @tc.name: ChangeState
 * @tc.desc: ChangeState with all valid states
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, ChangeState, TestSize.Level1)
{
    FvWindowState state = fvController_->GetCurState();
    EXPECT_EQ(FvWindowState::FV_STATE_UNDEFINED, state);
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(FvWindowState::FV_STATE_STARTED, fvController_->GetCurState());
    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPED);
    EXPECT_EQ(FvWindowState::FV_STATE_STOPPED, fvController_->GetCurState());
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTING);
    EXPECT_EQ(FvWindowState::FV_STATE_STARTING, fvController_->GetCurState());
    fvController_->ChangeState(FvWindowState::FV_STATE_IN_SIDEBAR);
    EXPECT_EQ(FvWindowState::FV_STATE_IN_SIDEBAR, fvController_->GetCurState());
    fvController_->ChangeState(FvWindowState::FV_STATE_IN_FLOATING_BALL);
    EXPECT_EQ(FvWindowState::FV_STATE_IN_FLOATING_BALL, fvController_->GetCurState());
}

/**
 * @tc.name: IsStateWithWindow
 * @tc.desc: IsStateWithWindow
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, IsStateWithWindow, TestSize.Level1)
{
    EXPECT_TRUE(fvController_->IsStateWithWindow(FvWindowState::FV_STATE_STARTED));
    EXPECT_TRUE(fvController_->IsStateWithWindow(FvWindowState::FV_STATE_HIDDEN));
    EXPECT_TRUE(fvController_->IsStateWithWindow(FvWindowState::FV_STATE_IN_SIDEBAR));
    EXPECT_TRUE(fvController_->IsStateWithWindow(FvWindowState::FV_STATE_IN_FLOATING_BALL));
    EXPECT_FALSE(fvController_->IsStateWithWindow(FvWindowState::FV_STATE_UNDEFINED));
    EXPECT_FALSE(fvController_->IsStateWithWindow(FvWindowState::FV_STATE_STOPPED));
}

/**
 * @tc.name: SetBindState
 * @tc.desc: SetBindState
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SetBindState, TestSize.Level1)
{
    fvController_->SetBindState(true);
    EXPECT_TRUE(fvController_->IsBind());
    fvController_->SetBindState(false);
    EXPECT_FALSE(fvController_->IsBind());
}

/**
 * @tc.name: StartFloatView
 * @tc.desc: StartFloatView with various states
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StartFloatView, TestSize.Level1)
{
    fvController_->SetBindState(true);
    EXPECT_NE(WMError::WM_OK, fvController_->StartFloatView());
    fvController_->UpdateMainWindow(nullptr);
    EXPECT_EQ(fvController_->mainWindow_, nullptr);

    fvController_->SetBindState(false);
    fvController_->UpdateMainWindow(mw_);
    EXPECT_NE(WMError::WM_OK, fvController_->StartFloatView());
}

/**
 * @tc.name: StartFloatViewSingle
 * @tc.desc: StartFloatViewSingle with various scenarios
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StartFloatViewSingle, TestSize.Level1)
{
    FloatWindowManager::token_ = 0;
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTING);
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StartFloatViewSingle());
    FloatWindowManager::token_ = 1;
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StartFloatViewSingle());

    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StartFloatViewSingle());

    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPING);
    EXPECT_EQ(WMError::WM_ERROR_FV_INVALID_STATE, fvController_->StartFloatViewSingle());

    FloatViewManager::hasActiveController_ = true;
    EXPECT_EQ(WMError::WM_ERROR_FV_START_FAILED, fvController_->StartFloatViewSingle());
    FloatViewManager::isActiveController_ = true;
    EXPECT_EQ(WMError::WM_ERROR_FV_INVALID_STATE, fvController_->StartFloatViewSingle());
    FloatViewManager::hasActiveController_ = false;
    FloatViewManager::isActiveController_ = false;

    FloatWindowManager::isFloatViewConflict_ = true;
    EXPECT_EQ(WMError::WM_ERROR_FLOAT_CONFLICT_WITH_OTHERS, fvController_->StartFloatViewSingle());
    FloatWindowManager::isFloatViewConflict_ = false;

    fvController_->UpdateMainWindow(mw_);
    EXPECT_NE(WMError::WM_OK, fvController_->StartFloatViewSingle(false));
    EXPECT_NE(WMError::WM_OK, fvController_->StartFloatViewSingle(true));
}

/**
 * @tc.name: CreateFloatViewWindow
 * @tc.desc: Test CreateFloatViewWindow with various scenarios
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, CreateFloatViewWindow, TestSize.Level1)
{
    void* invalidContext = nullptr;
    option_->SetContext(invalidContext);
    fvController_ = sptr<FloatViewController>::MakeSptr(*option_, nullptr);
    fvController_->UpdateMainWindow(mw_);
    mw_->SetWindowState(WindowState::STATE_SHOWN);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->CreateFloatViewWindow());

    fvController_ = sptr<FloatViewController>::MakeSptr(*option_, nullptr);
    fvController_->UpdateMainWindow(nullptr);
    mw_->SetWindowState(WindowState::STATE_SHOWN);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->CreateFloatViewWindow());

    fvController_ = sptr<FloatViewController>::MakeSptr(*option_, nullptr);
    fvController_->UpdateMainWindow(mw_);
    mw_->SetWindowState(WindowState::STATE_HIDDEN);
    EXPECT_EQ(WMError::WM_ERROR_FV_START_FAILED, fvController_->CreateFloatViewWindow());

    fvController_ = sptr<FloatViewController>::MakeSptr(*option_, nullptr);
    mw_->SetWindowState(WindowState::STATE_SHOWN);
    EXPECT_NE(WMError::WM_OK, fvController_->CreateFloatViewWindow());

    Rect rect {0, 0, 100, 100};
    option_->SetRect(rect);
    fvController_ = sptr<FloatViewController>::MakeSptr(*option_, nullptr);
    mw_->SetWindowState(WindowState::STATE_SHOWN);
    EXPECT_NE(WMError::WM_OK, fvController_->CreateFloatViewWindow());
}

/**
 * @tc.name: StopFloatViewFromClient
 * @tc.desc: StopFloatViewFromClient with various states
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StopFloatViewFromClient, TestSize.Level1)
{
    fvController_->SetBindState(true);
    EXPECT_NE(WMError::WM_OK, fvController_->StopFloatViewFromClient());

    fvController_->SetBindState(false);
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->StopFloatViewFromClient());
}

/**
 * @tc.name: StopFloatViewFromClientSingle
 * @tc.desc: StopFloatViewFromClientSingle with various states
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StopFloatViewFromClientSingle, TestSize.Level1)
{
    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPED);
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StopFloatViewFromClientSingle());

    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPING);
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StopFloatViewFromClientSingle());

    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->StopFloatViewFromClientSingle());
}

/**
 * @tc.name: StopFloatView
 * @tc.desc: StopFloatView with various states
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StopFloatView, TestSize.Level1)
{
    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPED);
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StopFloatView("test"));

    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPING);
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StopFloatView("test"));

    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->StopFloatView(""));
}

/**
 * @tc.name: RestoreMainWindow
 * @tc.desc: RestoreMainWindow with various states
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, RestoreMainWindow, TestSize.Level1)
{
    std::shared_ptr<AAFwk::WantParams> wantParams = std::make_shared<AAFwk::WantParams>();
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->RestoreMainWindow(wantParams));

    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPED);
    EXPECT_EQ(WMError::WM_ERROR_FV_INVALID_STATE, fvController_->RestoreMainWindow(wantParams));
}

/**
 * @tc.name: SetUIContext
 * @tc.desc: SetUIContext with various states
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SetUIContext, TestSize.Level1)
{
    std::string contextUrl = "test_url";
    std::shared_ptr<NativeReference> contentStorage = nullptr;
    EXPECT_EQ(WMError::WM_OK, fvController_->SetUIContext(contextUrl, contentStorage));

    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->SetUIContext(contextUrl, contentStorage));

    fvController_->ChangeState(FvWindowState::FV_STATE_HIDDEN);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->SetUIContext(contextUrl, contentStorage));

    fvController_->ChangeState(FvWindowState::FV_STATE_IN_SIDEBAR);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->SetUIContext(contextUrl, contentStorage));

    fvController_->ChangeState(FvWindowState::FV_STATE_IN_FLOATING_BALL);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->SetUIContext(contextUrl, contentStorage));
}

/**
 * @tc.name: SetVisibilityInApp
 * @tc.desc: SetVisibilityInApp with null window
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SetVisibilityInApp, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_OK, fvController_->SetVisibilityInApp(true));
    EXPECT_EQ(WMError::WM_OK, fvController_->SetVisibilityInApp(false));

    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->SetVisibilityInApp(true));
}

/**
 * @tc.name: SetWindowSize
 * @tc.desc: SetWindowSize with null window
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SetWindowSize, TestSize.Level1)
{
    Rect rect {0, 0, 100, 100};
    EXPECT_EQ(WMError::WM_OK, fvController_->SetWindowSize(rect));

    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->SetWindowSize(rect));

    fvController_->window_ = mw_;
    EXPECT_CALL(*mw_, UpdateFloatView(_)).Times(1).WillOnce(Return(WMError::WM_DO_NOTHING));
    EXPECT_EQ(WMError::WM_ERROR_SYSTEM_ABNORMALLY, fvController_->SetWindowSize(rect));

    EXPECT_CALL(*mw_, UpdateFloatView(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_EQ(WMError::WM_OK, fvController_->SetWindowSize(rect));
    fvController_->window_ = nullptr;
}

/**
 * @tc.name: GetWindow
 * @tc.desc: GetWindow
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, GetWindow, TestSize.Level1)
{
    fvController_->window_ = mw_;
    sptr<Window> window = fvController_->GetWindow();
    EXPECT_TRUE(fvController_->GetWindow() != nullptr);
    fvController_->window_ = nullptr;
    sptr<Window> window = fvController_->GetWindow();
    EXPECT_TRUE(fvController_->GetWindow() == nullptr);
}

/**
 * @tc.name: RegisterStateChangeListener
 * @tc.desc: RegisterStateChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, RegisterStateChangeListener, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->RegisterStateChangeListener(nullptr));
    auto listener = sptr<MockStateChangeListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fvController_->RegisterStateChangeListener(listener));
    EXPECT_EQ(WMError::WM_OK, fvController_->RegisterStateChangeListener(listener));
}

/**
 * @tc.name: UnregisterStateChangeListener
 * @tc.desc: UnregisterStateChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, UnregisterStateChangeListener, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->UnregisterStateChangeListener(nullptr));
    auto listener = sptr<MockStateChangeListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fvController_->UnregisterStateChangeListener(listener));
}

/**
 * @tc.name: RegisterRectChangeListener
 * @tc.desc: RegisterRectChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, RegisterRectChangeListener, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->RegisterRectChangeListener(nullptr));
    auto listener = sptr<MockRectChangeListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fvController_->RegisterRectChangeListener(listener));
    EXPECT_EQ(WMError::WM_OK, fvController_->RegisterRectChangeListener(listener));
}

/**
 * @tc.name: UnregisterRectChangeListener
 * @tc.desc: UnregisterRectChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, UnregisterRectChangeListener, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->UnregisterRectChangeListener(nullptr));
    auto listener = sptr<MockRectChangeListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fvController_->UnregisterRectChangeListener(listener));
}

/**
 * @tc.name: RegisterLimitsChangeListener
 * @tc.desc: RegisterLimitsChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, RegisterLimitsChangeListener, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->RegisterLimitsChangeListener(nullptr));
    auto listener = sptr<MockLimitsChangeListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fvController_->RegisterLimitsChangeListener(listener));
    EXPECT_EQ(WMError::WM_OK, fvController_->RegisterLimitsChangeListener(listener));
}

/**
 * @tc.name: UnregisterLimitsChangeListener
 * @tc.desc: UnregisterLimitsChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, UnregisterLimitsChangeListener, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->UnregisterLimitsChangeListener(nullptr));
    auto listener = sptr<MockLimitsChangeListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fvController_->UnregisterLimitsChangeListener(listener));
}

/**
 * @tc.name: ListenerNullTest
 * @tc.desc: Test all listener operations with null
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, ListenerNullTest, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->RegisterStateChangeListener(nullptr));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->UnregisterStateChangeListener(nullptr));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->RegisterRectChangeListener(nullptr));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->UnregisterRectChangeListener(nullptr));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->RegisterLimitsChangeListener(nullptr));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->UnregisterLimitsChangeListener(nullptr));
}

/**
 * @tc.name: StartFloatViewInner
 * @tc.desc: Test StartFloatViewInner with various scenarios
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StartFloatViewInner, TestSize.Level1)
{
    void* invalidContext = nullptr;
    option_->SetContext(invalidContext);
    fvController_ = sptr<FloatViewController>::MakeSptr(*option_, nullptr);
    fvController_->UpdateMainWindow(mw_);
    mw_->SetWindowState(WindowState::STATE_SHOWN);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->StartFloatViewInner());

    fvController_ = sptr<FloatViewController>::MakeSptr(*option_, nullptr);
    fvController_->UpdateMainWindow(mw_);
    mw_->SetWindowState(WindowState::STATE_SHOWN);
    EXPECT_NE(WMError::WM_OK, fvController_->StartFloatViewInner());
}
}
}
}
