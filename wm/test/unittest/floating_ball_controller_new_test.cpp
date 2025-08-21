/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "floating_ball_controller.h"
#include "floating_ball_manager.h"
#include "modifier_render_thread/rs_modifiers_draw_thread.h"
#include "parameters.h"
#include "window.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class MockNewWindow : public Window {
public:
    WindowState state_ = WindowState::STATE_INITIAL;
    const uint32_t mockWindowId_ = 101;
    MockNewWindow() {};
    ~MockNewWindow() {};
    MOCK_METHOD3(Show, WMError(uint32_t reason, bool withAnimation, bool withFocus));
    MOCK_METHOD1(Destroy, WMError(uint32_t reason));
    MOCK_METHOD0(NotifyPrepareCloseFloatingBall, void());
    MOCK_METHOD2(UpdateFloatingBall, WMError(const FloatingBallTemplateBaseInfo& fbTemplateBaseInfo,
        const std::shared_ptr<Media::PixelMap>& icon));
    MOCK_METHOD1(RestoreFbMainWindow, WMError(const std::shared_ptr<AAFwk::Want>& want));
    MOCK_METHOD1(GetFloatingBallWindowId, WMError(uint32_t& windowId));
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

class MockNewLifeCycleListener : public IFbLifeCycle {
public:
    void OnFloatingBallStart() override
    {
        return;
    }
    void OnFloatingBallStop() override
    {
        return;
    }
};

class MockNewClickListener : public IFbClickObserver {
public:

    void OnClickEvent() override
    {
        return;
    }
};

class FloatingBallControllerNewTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<FloatingBallController> fbController_;
    sptr<MockNewWindow> mw_;
};

void FloatingBallControllerNewTest::SetUpTestCase()
{
}

void FloatingBallControllerNewTest::TearDownTestCase()
{
#ifdef RS_ENABLE_VK
    RSModifiersDrawThread::Destroy();
#endif
}

void FloatingBallControllerNewTest::SetUp()
{
    int32_t windowId = 100;
    mw_ = sptr<MockNewWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw_);
    fbController_ = sptr<FloatingBallController>::MakeSptr(mw_, windowId, nullptr);
    ASSERT_NE(nullptr, fbController_);
}

void FloatingBallControllerNewTest::TearDown()
{
    fbController_ = nullptr;
    mw_ = nullptr;
}

namespace {
/**
 * @tc.name: CreateFloatingBallWindowAllNull
 * @tc.desc: CreateFloatingBallWindowAllNull
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, CreateFloatingBallWindowAllNull, TestSize.Level1)
{
    sptr<FbOption> nullOption = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController_->CreateFloatingBallWindow(nullOption));
}

/**
 * @tc.name: CreateFloatingBallWindowAllNull
 * @tc.desc: CreateFloatingBallWindowAllNull
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, CreateFloatingBallWindowAllNull001, TestSize.Level1)
{
    fbController_->contextPtr_ = nullptr;
    sptr<FbOption> option = sptr<FbOption>::MakeSptr();
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController_->CreateFloatingBallWindow(option));
    option = nullptr;
}

/**
 * @tc.name: CreateFloatingBallWindowAllNull
 * @tc.desc: CreateFloatingBallWindowAllNull
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, CreateFloatingBallWindowAllNull002, TestSize.Level1)
{
    std::shared_ptr<AbilityRuntime::AbilityContextImpl> contextPtr =
        std::make_shared<AbilityRuntime::AbilityContextImpl>();
    fbController_->contextPtr_ = &contextPtr;
    fbController_->mainWindow_ = nullptr;
    sptr<FbOption> option = sptr<FbOption>::MakeSptr();
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController_->CreateFloatingBallWindow(option));
    option = nullptr;
    fbController_->contextPtr_ = nullptr;
}

/**
 * @tc.name: CreateFloatingBallWindowNotNullOption
 * @tc.desc: CreateFloatingBallWindowNotNullOption
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, CreateFloatingBallWindowNotNullOption, TestSize.Level1)
{
    std::shared_ptr<AbilityRuntime::AbilityContextImpl> contextPtr =
        std::make_shared<AbilityRuntime::AbilityContextImpl>();
    fbController_->contextPtr_ = &contextPtr;
    sptr<FbOption> option = sptr<FbOption>::MakeSptr();
    mw_->SetWindowState(WindowState::STATE_INITIAL);
    EXPECT_EQ(WMError::WM_ERROR_FB_CREATE_FAILED, fbController_->CreateFloatingBallWindow(option));
    option = nullptr;
    fbController_->contextPtr_ = nullptr;
}

/**
 * @tc.name: CreateFloatingBallWindowNotNullOption
 * @tc.desc: CreateFloatingBallWindowNotNullOption
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, CreateFloatingBallWindowNotNullOption001, TestSize.Level1)
{
    std::shared_ptr<AbilityRuntime::AbilityContextImpl> contextPtr =
        std::make_shared<AbilityRuntime::AbilityContextImpl>();
    fbController_->contextPtr_ = &contextPtr;
    sptr<FbOption> option = sptr<FbOption>::MakeSptr();
    mw_->SetWindowState(WindowState::STATE_SHOWN);
    EXPECT_EQ(WMError::WM_ERROR_FB_CREATE_FAILED, fbController_->CreateFloatingBallWindow(option));
    option = nullptr;
    fbController_->contextPtr_ = nullptr;
}

/**
 * @tc.name: CreateFloatingBallWindowUpdateNull
 * @tc.desc: CreateFloatingBallWindowUpdateNull
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, CreateFloatingBallWindowUpdateNull, TestSize.Level1)
{
    std::shared_ptr<AbilityRuntime::AbilityContextImpl> contextPtr =
        std::make_shared<AbilityRuntime::AbilityContextImpl>();
    fbController_->contextPtr_ = &contextPtr;
    mw_->SetWindowState(WindowState::STATE_SHOWN);
    sptr<FbOption> option = sptr<FbOption>::MakeSptr();
    auto res = fbController_->CreateFloatingBallWindow(option);
    EXPECT_EQ(WMError::WM_ERROR_FB_CREATE_FAILED, res);
    fbController_->UpdateMainWindow(nullptr);
    EXPECT_EQ(100, fbController_->mainWindowId_);
    option = nullptr;
    fbController_->contextPtr_ = nullptr;
}

/**
 * @tc.name: CreateFloatingBallWindowUpdateNotNull
 * @tc.desc: CreateFloatingBallWindowUpdateNotNull
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, CreateFloatingBallWindowUpdateNotNull, TestSize.Level1)
{
    std::shared_ptr<AbilityRuntime::AbilityContextImpl> contextPtr =
        std::make_shared<AbilityRuntime::AbilityContextImpl>();
    fbController_->contextPtr_ = &contextPtr;
    sptr<FbOption> option = sptr<FbOption>::MakeSptr();
    auto res = fbController_->CreateFloatingBallWindow(option);
    EXPECT_EQ(WMError::WM_ERROR_FB_CREATE_FAILED, res);
    fbController_->UpdateMainWindow(mw_);
    EXPECT_EQ(101, fbController_->mainWindowId_);
    option = nullptr;
    fbController_->contextPtr_ = nullptr;
}

/**
 * @tc.name: StartFloatingBallNull
 * @tc.desc: StartFloatingBallNull
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, StartFloatingBallNull, TestSize.Level1)
{
    sptr<FbOption> nullOption = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController_->StartFloatingBall(nullOption));
}

/**
 * @tc.name: StartFloatingBallNotNull
 * @tc.desc: StartFloatingBallNotNull
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, StartFloatingBallNotNull, TestSize.Level1)
{
    fbController_->curState_ = FbWindowState::STATE_STARTING;
    sptr<FbOption> option = sptr<FbOption>::MakeSptr();
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_OPERATION, fbController_->StartFloatingBall(option));
    option = nullptr;
}

/**
 * @tc.name: StartFloatingBallNotNull
 * @tc.desc: StartFloatingBallNotNull
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, StartFloatingBallNotNull01, TestSize.Level1)
{
    fbController_->curState_ = FbWindowState::STATE_STARTED;
    sptr<FbOption> option = sptr<FbOption>::MakeSptr();
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_OPERATION, fbController_->StartFloatingBall(option));
    option = nullptr;
}

/**
 * @tc.name: StartFloatingBallSetActiveController
 * @tc.desc: StartFloatingBallSetActiveController
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, StartFloatingBallSetActiveController, TestSize.Level1)
{
    fbController_->curState_ = FbWindowState::STATE_UNDEFINED;
    sptr<FbOption> option = sptr<FbOption>::MakeSptr();
    FloatingBallManager::SetActiveController(fbController_);
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController_->StartFloatingBall(option));
    FloatingBallManager::RemoveActiveController(fbController_);
    option = nullptr;
}

/**
 * @tc.name: StartFloatingBallSetActiveController
 * @tc.desc: StartFloatingBallSetActiveController
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, StartFloatingBallSetActiveController01, TestSize.Level1)
{
    auto activeFbController = sptr<FloatingBallController>::MakeSptr(mw_, 100, nullptr);
    sptr<FbOption> option = sptr<FbOption>::MakeSptr();
    FloatingBallManager::SetActiveController(activeFbController);
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_CONTROLLER, fbController_->StartFloatingBall(option));

    FloatingBallManager::RemoveActiveController(activeFbController);
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController_->StartFloatingBall(option));
    option = nullptr;
}

/**
 * @tc.name: StartFloatingBallGetControllerState
 * @tc.desc: StartFloatingBallGetControllerState
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, StartFloatingBallGetControllerState, TestSize.Level1)
{
    std::unique_ptr<AbilityRuntime::AbilityContextImpl> contextPtr =
        std::make_unique<AbilityRuntime::AbilityContextImpl>();
    fbController_->contextPtr_ = contextPtr.get();
    sptr<FbOption> option = sptr<FbOption>::MakeSptr();
    EXPECT_NE(WMError::WM_OK, fbController_->StartFloatingBall(option));
    EXPECT_EQ(FbWindowState::STATE_UNDEFINED, fbController_->GetControllerState());
    option = nullptr;
    fbController_->contextPtr_ = nullptr;
}

/**
 * @tc.name: LifeCycleTestNull
 * @tc.desc: LifeCycleTestNull
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, LifeCycleTestNull, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_ERROR_FB_INTERNAL_ERROR, fbController_->RegisterFbLifecycle(nullptr));
}

/**
 * @tc.name: LifeCycleTestNotNull
 * @tc.desc: LifeCycleTestNotNull
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, LifeCycleTestNotNull, TestSize.Level1)
{
    auto lifeListener = sptr<MockNewLifeCycleListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fbController_->RegisterFbLifecycle(lifeListener));
    fbController_->fbLifeCycleListeners_.clear();
}

/**
 * @tc.name: LifeCycleTestRepeatRegister
 * @tc.desc: LifeCycleTestRepeatRegister
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, LifeCycleTestRepeatRegister, TestSize.Level1)
{
    auto lifeListener = sptr<MockNewLifeCycleListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fbController_->RegisterFbLifecycle(lifeListener));
    // repeat register
    EXPECT_EQ(WMError::WM_OK, fbController_->RegisterFbLifecycle(lifeListener));
    fbController_->fbLifeCycleListeners_.emplace_back(nullptr);
    fbController_->OnFloatingBallStart();
    fbController_->OnFloatingBallStop();
    fbController_->fbLifeCycleListeners_.clear();
}

/**
 * @tc.name: LifeCycleTestUnregister
 * @tc.desc: LifeCycleTestUnregister
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, LifeCycleTestUnregister, TestSize.Level1)
{
    auto lifeListener = sptr<MockNewLifeCycleListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_ERROR_FB_INTERNAL_ERROR, fbController_->UnRegisterFbLifecycle(nullptr));
    EXPECT_EQ(WMError::WM_OK, fbController_->UnRegisterFbLifecycle(lifeListener));
    fbController_->fbLifeCycleListeners_.clear();
}

/**
 * @tc.name: ClickTestNull
 * @tc.desc: ClickTestNull
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, ClickTestNull, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_ERROR_FB_INTERNAL_ERROR, fbController_->RegisterFbClickObserver(nullptr));
}

/**
 * @tc.name: ClickTestNotNull
 * @tc.desc: ClickTestNotNull
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, ClickTestNotNull, TestSize.Level1)
{
    auto clickListener = sptr<MockNewClickListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fbController_->RegisterFbClickObserver(clickListener));
    fbController_->fbClickObservers_.clear();
}

/**
 * @tc.name: ClickTestRepeatRegister
 * @tc.desc: ClickTestRepeatRegister
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, ClickTestRepeatRegister, TestSize.Level1)
{
    auto clickListener = sptr<MockNewClickListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fbController_->RegisterFbClickObserver(clickListener));
    // repeat register
    EXPECT_EQ(WMError::WM_OK, fbController_->RegisterFbClickObserver(clickListener));
    fbController_->fbClickObservers_.emplace_back(nullptr);
    fbController_->OnFloatingBallClick();
    fbController_->fbClickObservers_.clear();
}

/**
 * @tc.name: ClickTestUnregister
 * @tc.desc: ClickTestUnregister
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, ClickTestUnregister, TestSize.Level1)
{
    auto clickListener = sptr<MockNewClickListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_ERROR_FB_INTERNAL_ERROR, fbController_->UnRegisterFbClickObserver(nullptr));
    EXPECT_EQ(WMError::WM_OK, fbController_->UnRegisterFbClickObserver(clickListener));
    fbController_->fbClickObservers_.clear();
}

/**
 * @tc.name: UpdateFloatingBall
 * @tc.desc: UpdateFloatingBall
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, UpdateFloatingBall01, TestSize.Level1)
{
    fbController_->curState_ = FbWindowState::STATE_STOPPED;
    sptr<FbOption> option = sptr<FbOption>::MakeSptr();
    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, fbController_->UpdateFloatingBall(option));

    fbController_->curState_ = FbWindowState::STATE_STARTED;
    sptr<FbOption> nullOption = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController_->UpdateFloatingBall(nullOption));
    fbController_->window_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController_->UpdateFloatingBall(option));

    fbController_->window_ = mw_;
    fbController_->curState_ = FbWindowState::STATE_STOPPED;
    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, fbController_->UpdateFloatingBall(option));
    EXPECT_CALL(*(mw_), UpdateFloatingBall(_, _)).Times(1).WillOnce(Return(WMError::WM_OK));
    fbController_->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_OK, fbController_->UpdateFloatingBall(option));
    option = nullptr;
}

/**
 * @tc.name: StopFloatingBall
 * @tc.desc: StopFloatingBall
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, StopFloatingBall01, TestSize.Level1)
{
    fbController_->curState_ = FbWindowState::STATE_STOPPED;
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_OPERATION, fbController_->StopFloatingBallFromClient());
    fbController_->curState_ = FbWindowState::STATE_STOPPING;
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_OPERATION, fbController_->StopFloatingBallFromClient());
    fbController_->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_CALL(*(mw_), NotifyPrepareCloseFloatingBall()).Times(1);
    fbController_->window_ = mw_;
    EXPECT_EQ(WMError::WM_OK, fbController_->StopFloatingBallFromClient());
    fbController_->window_ = nullptr;
    fbController_->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController_->StopFloatingBallFromClient());
}

/**
 * @tc.name: StopFloatingBall
 * @tc.desc: StopFloatingBall
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, StopFloatingBall02, TestSize.Level1)
{
    fbController_->stopFromClient_ = false;
    fbController_->curState_ = FbWindowState::STATE_STOPPING;
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_OPERATION, fbController_->StopFloatingBall());
    fbController_->curState_ = FbWindowState::STATE_STOPPED;
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_OPERATION, fbController_->StopFloatingBall());

    fbController_->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_CALL(*(mw_), Destroy(_)).Times(1);
    fbController_->window_ = mw_;
    EXPECT_EQ(WMError::WM_OK, fbController_->StopFloatingBall());
    EXPECT_EQ(FbWindowState::STATE_STOPPED, fbController_->GetControllerState());

    fbController_->window_ = nullptr;
    fbController_->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController_->StopFloatingBall());
    EXPECT_EQ(WMError::WM_ERROR_FB_INTERNAL_ERROR, fbController_->DestroyFloatingBallWindow());
    EXPECT_EQ(nullptr, fbController_->GetFbWindow());
}

/**
 * @tc.name: RestoreFloatingBallAbility
 * @tc.desc: RestoreFloatingBallAbility
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, RestoreFloatingBallAbility, TestSize.Level1)
{
    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    fbController_->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController_->RestoreMainWindow(want));
    fbController_->window_ = mw_;
    fbController_->curState_ = FbWindowState::STATE_STOPPING;
    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, fbController_->RestoreMainWindow(want));
    fbController_->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_CALL(*(mw_), RestoreFbMainWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_EQ(WMError::WM_OK, fbController_->RestoreMainWindow(want));
}

/**
 * @tc.name: GetFloatingBallInfo
 * @tc.desc: GetFloatingBallInfo
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerNewTest, GetFloatingBallInfo, TestSize.Level1)
{
    EXPECT_CALL(*(mw_), GetFloatingBallWindowId(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    fbController_->window_ = mw_;
    uint32_t mockId = 1;
    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, fbController_->GetFloatingBallWindowInfo(mockId));
    fbController_->curState_ = FbWindowState::STATE_STARTED;
    fbController_->window_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController_->GetFloatingBallWindowInfo(mockId));
    fbController_->window_ = mw_;
    EXPECT_EQ(WMError::WM_OK, fbController_->GetFloatingBallWindowInfo(mockId));
}
}
}
}