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
#include "float_view_manager.h"
#include "float_window_manager.h"
#include "window.h"
#include "wm_common.h"

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
    MOCK_METHOD4(NapiSetUIContent, WMError(const std::string& contentUrl, napi_env env, napi_value storage,
        BackupAndRestoreType type));
    MOCK_METHOD1(RegisterLifeCycleListener, void(const sptr<IWindowLifeCycle>& listener));
    MOCK_METHOD1(UnregisterLifeCycleListener, void(const sptr<IWindowLifeCycle>& listener));
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
    void OnStateChange(const FloatViewState& state) override
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
 * @tc.name: UpdateMainWindow
 * @tc.desc: UpdateMainWindow
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, UpdateMainWindow01, TestSize.Level1)
{
    fvController_->UpdateMainWindow(nullptr);
    fvController_->UpdateMainWindow(mw_);
}

/**
 * @tc.name: GetCurState
 * @tc.desc: GetCurState
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, GetCurState01, TestSize.Level1)
{
    FvWindowState state = fvController_->GetCurState();
    EXPECT_EQ(FvWindowState::FV_STATE_UNDEFINED, state);
}

/**
 * @tc.name: ChangeState
 * @tc.desc: ChangeState
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, ChangeState01, TestSize.Level1)
{
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(FvWindowState::FV_STATE_STARTED, fvController_->GetCurState());
    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPED);
    EXPECT_EQ(FvWindowState::FV_STATE_STOPPED, fvController_->GetCurState());
}

/**
 * @tc.name: ChangeState02
 * @tc.desc: ChangeState with all valid states
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, ChangeState02, TestSize.Level1)
{
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTING);
    EXPECT_EQ(FvWindowState::FV_STATE_STARTING, fvController_->GetCurState());
    fvController_->ChangeState(FvWindowState::FV_STATE_HIDDEN);
    EXPECT_EQ(FvWindowState::FV_STATE_HIDDEN, fvController_->GetCurState());
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
HWTEST_F(FloatViewControllerTest, IsStateWithWindow01, TestSize.Level1)
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
HWTEST_F(FloatViewControllerTest, SetBindState01, TestSize.Level1)
{
    fvController_->SetBindState(true);
    EXPECT_TRUE(fvController_->IsBind());
    fvController_->SetBindState(false);
    EXPECT_FALSE(fvController_->IsBind());
}

/**
 * @tc.name: SetBindWindowId
 * @tc.desc: SetBindWindowId
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SetBindWindowId01, TestSize.Level1)
{
    uint32_t windowId = 123;
    fvController_->SetBindWindowId(windowId);
}

/**
 * @tc.name: StartFloatView
 * @tc.desc: StartFloatView with bind state
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StartFloatView01, TestSize.Level1)
{
    fvController_->SetBindState(true);
    EXPECT_NE(WMError::WM_OK, fvController_->StartFloatView());
}

/**
 * @tc.name: StartFloatView02
 * @tc.desc: StartFloatView without bind state
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StartFloatView02, TestSize.Level1)
{
    fvController_->SetBindState(false);
    fvController_->UpdateMainWindow(mw_);
    EXPECT_NE(WMError::WM_OK, fvController_->StartFloatView());
}

/**
 * @tc.name: StartFloatViewSingle
 * @tc.desc: StartFloatViewSingle with invalid states
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StartFloatViewSingle01, TestSize.Level1)
{
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTING);
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StartFloatViewSingle());
    
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StartFloatViewSingle());
    
    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPING);
    EXPECT_EQ(WMError::WM_ERROR_FV_INVALID_STATE, fvController_->StartFloatViewSingle());
}

/**
 * @tc.name: StartFloatViewSingle02
 * @tc.desc: StartFloatViewSingle with showWhenCreate false
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StartFloatViewSingle02, TestSize.Level1)
{
    fvController_->UpdateMainWindow(mw_);
    EXPECT_NE(WMError::WM_OK, fvController_->StartFloatViewSingle(false));
}

/**
 * @tc.name: StopFloatViewFromClient
 * @tc.desc: StopFloatViewFromClient with bind state
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StopFloatViewFromClient01, TestSize.Level1)
{
    fvController_->SetBindState(true);
    EXPECT_NE(WMError::WM_OK, fvController_->StopFloatViewFromClient());
}

/**
 * @tc.name: StopFloatViewFromClient02
 * @tc.desc: StopFloatViewFromClient without bind state
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StopFloatViewFromClient02, TestSize.Level1)
{
    fvController_->SetBindState(false);
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->StopFloatViewFromClient());
}

/**
 * @tc.name: StopFloatViewFromClientSingle
 * @tc.desc: StopFloatViewFromClientSingle with invalid states
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StopFloatViewFromClientSingle01, TestSize.Level1)
{
    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPED);
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StopFloatViewFromClientSingle());
    
    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPING);
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StopFloatViewFromClientSingle());
}

/**
 * @tc.name: StopFloatViewFromClientSingle02
 * @tc.desc: StopFloatViewFromClientSingle with valid state but null window
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StopFloatViewFromClientSingle02, TestSize.Level1)
{
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->StopFloatViewFromClientSingle());
}

/**
 * @tc.name: StopFloatView
 * @tc.desc: StopFloatView with invalid states
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StopFloatView01, TestSize.Level1)
{
    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPED);
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StopFloatView("test"));

    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPING);
    EXPECT_EQ(WMError::WM_ERROR_FV_REPEAT_OPERATION, fvController_->StopFloatView("test"));
}

/**
 * @tc.name: StopFloatView02
 * @tc.desc: StopFloatView with null window
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StopFloatView02, TestSize.Level1)
{
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->StopFloatView(""));
}

/**
 * @tc.name: RestoreMainWindow
 * @tc.desc: RestoreMainWindow with invalid state
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, RestoreMainWindow01, TestSize.Level1)
{
    std::shared_ptr<AAFwk::WantParams> wantParams = std::make_shared<AAFwk::WantParams>();
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->RestoreMainWindow(wantParams));
}

/**
 * @tc.name: RestoreMainWindow02
 * @tc.desc: RestoreMainWindow with stopped state
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, RestoreMainWindow02, TestSize.Level1)
{
    std::shared_ptr<AAFwk::WantParams> wantParams = std::make_shared<AAFwk::WantParams>();
    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPED);
    EXPECT_EQ(WMError::WM_ERROR_FV_INVALID_STATE, fvController_->RestoreMainWindow(wantParams));
}

/**
 * @tc.name: SetUIContext
 * @tc.desc: SetUIContext with null window
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SetUIContext01, TestSize.Level1)
{
    std::string contextUrl = "test_url";
    std::shared_ptr<NativeReference> contentStorage = nullptr;
    EXPECT_EQ(WMError::WM_OK, fvController_->SetUIContext(contextUrl, contentStorage));
}

/**
 * @tc.name: SetUIContext02
 * @tc.desc: SetUIContext with state that has window but null window
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SetUIContext02, TestSize.Level1)
{
    std::string contextUrl = "test_url";
    std::shared_ptr<NativeReference> contentStorage = nullptr;
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->SetUIContext(contextUrl, contentStorage));
}

/**
 * @tc.name: SetVisibilityInApp
 * @tc.desc: SetVisibilityInApp with null window
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SetVisibilityInApp01, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_OK, fvController_->SetVisibilityInApp(true));
    EXPECT_EQ(WMError::WM_OK, fvController_->SetVisibilityInApp(false));
}

/**
 * @tc.name: SetVisibilityInApp02
 * @tc.desc: SetVisibilityInApp with state that has window but null window
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SetVisibilityInApp02, TestSize.Level1)
{
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->SetVisibilityInApp(true));
}

/**
 * @tc.name: SetWindowSize
 * @tc.desc: SetWindowSize with null window
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SetWindowSize01, TestSize.Level1)
{
    Rect rect {0, 0, 100, 100};
    EXPECT_EQ(WMError::WM_OK, fvController_->SetWindowSize(rect));
}

/**
 * @tc.name: SetWindowSize02
 * @tc.desc: SetWindowSize with state that has window but null window
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SetWindowSize02, TestSize.Level1)
{
    Rect rect {0, 0, 100, 100};
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, fvController_->SetWindowSize(rect));
}

/**
 * @tc.name: SyncWindowInfo
 * @tc.desc: SyncWindowInfo with null window
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SyncWindowInfo01, TestSize.Level1)
{
    uint32_t windowId = 101;
    FloatViewWindowInfo windowInfo;
    windowInfo.windowRect_ = {0, 0, 100, 100};
    windowInfo.scale_ = 1.0;
    fvController_->SyncWindowInfo(windowId, windowInfo, "test");
}

/**
 * @tc.name: SyncWindowInfo02
 * @tc.desc: SyncWindowInfo with mismatched window id
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SyncWindowInfo02, TestSize.Level1)
{
    uint32_t windowId = 999;
    FloatViewWindowInfo windowInfo;
    windowInfo.windowRect_ = {0, 0, 100, 100};
    windowInfo.scale_ = 1.0;
    fvController_->UpdateMainWindow(mw_);
    fvController_->SyncWindowInfo(windowId, windowInfo);
}

/**
 * @tc.name: SyncLimits
 * @tc.desc: SyncLimits with null window
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SyncLimits01, TestSize.Level1)
{
    uint32_t windowId = 101;
    FloatViewLimits limits;
    fvController_->SyncLimits(windowId, limits);
}

/**
 * @tc.name: SyncLimits02
 * @tc.desc: SyncLimits with mismatched window id
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, SyncLimits02, TestSize.Level1)
{
    uint32_t windowId = 999;
    FloatViewLimits limits;
    fvController_->UpdateMainWindow(mw_);
    fvController_->SyncLimits(windowId, limits);
}

/**
 * @tc.name: GetWindowInfo
 * @tc.desc: GetWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, GetWindowInfo01, TestSize.Level1)
{
    FloatViewWindowInfo windowInfo = fvController_->GetWindowInfo();
}

/**
 * @tc.name: GetWindow
 * @tc.desc: GetWindow
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, GetWindow01, TestSize.Level1)
{
    sptr<Window> window = fvController_->GetWindow();
    EXPECT_EQ(nullptr, window);
}

/**
 * @tc.name: RegisterStateChangeListener
 * @tc.desc: RegisterStateChangeListener
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, RegisterStateChangeListener01, TestSize.Level1)
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
HWTEST_F(FloatViewControllerTest, UnregisterStateChangeListener01, TestSize.Level1)
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
HWTEST_F(FloatViewControllerTest, RegisterRectChangeListener01, TestSize.Level1)
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
HWTEST_F(FloatViewControllerTest, UnregisterRectChangeListener01, TestSize.Level1)
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
HWTEST_F(FloatViewControllerTest, RegisterLimitsChangeListener01, TestSize.Level1)
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
HWTEST_F(FloatViewControllerTest, UnregisterLimitsChangeListener01, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, fvController_->UnregisterLimitsChangeListener(nullptr));
    auto listener = sptr<MockLimitsChangeListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fvController_->UnregisterLimitsChangeListener(listener));
}

/**
 * @tc.name: StateTransitionTest
 * @tc.desc: Test all state transitions
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewControllerTest, StateTransitionTest, TestSize.Level1)
{
    fvController_->ChangeState(FvWindowState::FV_STATE_UNDEFINED);
    EXPECT_EQ(FvWindowState::FV_STATE_UNDEFINED, fvController_->GetCurState());
    
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTING);
    EXPECT_EQ(FvWindowState::FV_STATE_STARTING, fvController_->GetCurState());
    
    fvController_->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_EQ(FvWindowState::FV_STATE_STARTED, fvController_->GetCurState());
    
    fvController_->ChangeState(FvWindowState::FV_STATE_HIDDEN);
    EXPECT_EQ(FvWindowState::FV_STATE_HIDDEN, fvController_->GetCurState());
    
    fvController_->ChangeState(FvWindowState::FV_STATE_IN_SIDEBAR);
    EXPECT_EQ(FvWindowState::FV_STATE_IN_SIDEBAR, fvController_->GetCurState());
    
    fvController_->ChangeState(FvWindowState::FV_STATE_IN_FLOATING_BALL);
    EXPECT_EQ(FvWindowState::FV_STATE_IN_FLOATING_BALL, fvController_->GetCurState());
    
    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPING);
    EXPECT_EQ(FvWindowState::FV_STATE_STOPPING, fvController_->GetCurState());
    
    fvController_->ChangeState(FvWindowState::FV_STATE_STOPPED);
    EXPECT_EQ(FvWindowState::FV_STATE_STOPPED, fvController_->GetCurState());
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

}
}
}
