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
#include "parameters.h"
#include "floating_ball_controller.h"
#include "floating_ball_manager.h"
#include "window.h"
#include "wm_common.h"
#include "ability_context_impl.h"

using namespace testing;
using namespace testing::ext;
 
namespace OHOS {
namespace Rosen {

class MockWindow : public Window {
public:
    MockWindow() {};
    ~MockWindow() {};
    MOCK_METHOD3(Show, WMError(uint32_t reason, bool withAnimation, bool withFocus));
    MOCK_METHOD1(Destroy, WMError(uint32_t reason));
    MOCK_METHOD0(NotifyPrepareCloseFloatingBall, void());
    MOCK_METHOD2(UpdateFloatingBall, void(const FloatingBallTemplateBaseInfo& fbTemplateBaseInfo,
        const std::shared_ptr<Media::PixelMap>& icon));
    MOCK_METHOD1(RestoreFbMainWindow, WMError(const std::shared_ptr<AAFwk::Want>& want));
    MOCK_METHOD1(GetFloatingBallWindowId, WMError(uint32_t& windowId));
    uint32_t GetWindowId() const override
    {
        return 101;
    }
};

class MockLifeCycleListener : public IFbLifeCycle {
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

class MockClickListener : public IFbClickObserver {
public:

    void OnClickEvent() override
    {
        return;
    }
};

class FloatingBallControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FloatingBallControllerTest::SetUpTestCase()
{
}

void FloatingBallControllerTest::TearDownTestCase()
{
}

void FloatingBallControllerTest::SetUp()
{
}

void FloatingBallControllerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: CreateFloatingBallWindow
 * @tc.desc: CreateFloatingBallWindow
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerTest, CreateFloatingBallWindow01, TestSize.Level1)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<FbOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto fbController = sptr<FloatingBallController>::MakeSptr(mw, 100, nullptr);
    fbController->fbOption_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_FB_CREATE_FAILED, fbController->CreateFloatingBallWindow());
    fbController->fbOption_ = option;
    EXPECT_EQ(WMError::WM_ERROR_FB_CREATE_FAILED, fbController->CreateFloatingBallWindow());

    AbilityRuntime::AbilityContextImpl* contextPtr = new AbilityRuntime::AbilityContextImpl();
    fbController->contextPtr_ = contextPtr;
    fbController->CreateFloatingBallWindow();
    fbController->UpdateMainWindow(nullptr);
    EXPECT_EQ(100, fbController->mainWindowId_);
    fbController->UpdateMainWindow(mw);
    EXPECT_EQ(101, fbController->mainWindowId_);
    delete contextPtr;
}

/**
 * @tc.name: StartFloatingBall
 * @tc.desc: StartFloatingBall
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerTest, StartFloatingBall01, TestSize.Level1)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<FbOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto fbController = sptr<FloatingBallController>::MakeSptr(mw, 100, nullptr);
    sptr<FbOption> nullOption = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_FB_CREATE_FAILED, fbController->StartFloatingBall(nullOption));
    fbController->curState_ = FbWindowState::STATE_STARTING;
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_OPERATION, fbController->StartFloatingBall(option));
    fbController->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_OPERATION, fbController->StartFloatingBall(option));

    fbController->curState_ = FbWindowState::STATE_UNDEFINED;
    auto activeFbController = sptr<FloatingBallController>::MakeSptr(mw, 100, nullptr);
    FloatingBallManager::SetActiveController(activeFbController);
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_CONTROLLER, fbController->StartFloatingBall(option));

    FloatingBallManager::RemoveActiveController(activeFbController);
    EXPECT_EQ(WMError::WM_ERROR_FB_CREATE_FAILED, fbController->StartFloatingBall(option));
    EXPECT_EQ(FbWindowState::STATE_UNDEFINED, fbController->curState_);

    AbilityRuntime::AbilityContextImpl* contextPtr = new AbilityRuntime::AbilityContextImpl();
    fbController->contextPtr_ = contextPtr;
    EXPECT_NE(WMError::WM_OK, fbController->StartFloatingBall(option));
    EXPECT_EQ(FbWindowState::STATE_UNDEFINED, fbController->GetControllerState());
    delete contextPtr;
}

/**
 * @tc.name: LifeCycleTest
 * @tc.desc: LifeCycleTest
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerTest, LifeCycleTest01, TestSize.Level1)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<FbOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto fbController = sptr<FloatingBallController>::MakeSptr(mw, 100, nullptr);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, fbController->RegisterFbLifecycle(nullptr));
    auto lifeListener = sptr<MockLifeCycleListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fbController->RegisterFbLifecycle(lifeListener));
    // repeat register
    EXPECT_EQ(WMError::WM_OK, fbController->RegisterFbLifecycle(lifeListener));
    fbController->fbLifeCycleListeners_.emplace_back(nullptr);
    fbController->OnFloatingBallStart();
    fbController->OnFloatingBallStop();
    // unregister
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, fbController->UnRegisterFbLifecycle(nullptr));
    EXPECT_EQ(WMError::WM_OK, fbController->UnRegisterFbLifecycle(lifeListener));
    fbController->fbLifeCycleListeners_.clear();
}

/**
 * @tc.name: ClickTest
 * @tc.desc: ClickTest
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerTest, ClickTest01, TestSize.Level1)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<FbOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto fbController = sptr<FloatingBallController>::MakeSptr(mw, 100, nullptr);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, fbController->RegisterFbClickObserver(nullptr));
    auto clickListener = sptr<MockClickListener>::MakeSptr();
    EXPECT_EQ(WMError::WM_OK, fbController->RegisterFbClickObserver(clickListener));
    // repeat register
    EXPECT_EQ(WMError::WM_OK, fbController->RegisterFbClickObserver(clickListener));
    fbController->fbClickObservers_.emplace_back(nullptr);
    fbController->OnFloatingBallClick();
    // unregister
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, fbController->UnRegisterFbClickObserver(nullptr));
    EXPECT_EQ(WMError::WM_OK, fbController->UnRegisterFbClickObserver(clickListener));
    fbController->fbClickObservers_.clear();
}

/**
 * @tc.name: UpdateFloatingBall
 * @tc.desc: UpdateFloatingBall
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerTest, UpdateFloatingBall01, TestSize.Level1)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<FbOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto fbController = sptr<FloatingBallController>::MakeSptr(mw, 100, nullptr);
    sptr<FbOption> nullOption = nullptr;
    fbController->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController->UpdateFloatingBall(nullOption));
    fbController->window_ = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController->UpdateFloatingBall(option));

    fbController->window_ = mw;
    fbController->curState_ = FbWindowState::STATE_STOPPED;
    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, fbController->UpdateFloatingBall(option));

    EXPECT_CALL(*(mw), UpdateFloatingBall(_, _)).Times(1);
    fbController->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_OK, fbController->UpdateFloatingBall(option));
}

/**
 * @tc.name: StopFloatingBall
 * @tc.desc: StopFloatingBall
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerTest, StopFloatingBall01, TestSize.Level1)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<FbOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto fbController = sptr<FloatingBallController>::MakeSptr(mw, 100, nullptr);
    fbController->curState_ = FbWindowState::STATE_STOPPED;
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_OPERATION, fbController->StopFloatingBallFromClient());
    fbController->curState_ = FbWindowState::STATE_STOPPING;
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_OPERATION, fbController->StopFloatingBallFromClient());
    fbController->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_CALL(*(mw), NotifyPrepareCloseFloatingBall()).Times(1);
    fbController->window_ = mw;
    EXPECT_EQ(WMError::WM_OK, fbController->StopFloatingBallFromClient());
    fbController->window_ = nullptr;
    fbController->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController->StopFloatingBallFromClient());
}

/**
 * @tc.name: StopFloatingBall
 * @tc.desc: StopFloatingBall
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerTest, StopFloatingBall02, TestSize.Level1)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<FbOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto fbController = sptr<FloatingBallController>::MakeSptr(mw, 100, nullptr);
    fbController->stopFromClient_ = false;
    fbController->curState_ = FbWindowState::STATE_STOPPING;
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_OPERATION, fbController->StopFloatingBall());
    fbController->curState_ = FbWindowState::STATE_STOPPED;
    EXPECT_EQ(WMError::WM_ERROR_FB_REPEAT_OPERATION, fbController->StopFloatingBall());

    fbController->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_CALL(*(mw), Destroy(_)).Times(1);
    fbController->window_ = mw;
    EXPECT_EQ(WMError::WM_OK, fbController->StopFloatingBall());
    EXPECT_EQ(FbWindowState::STATE_STOPPED, fbController->GetControllerState());

    fbController->window_ = nullptr;
    fbController->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController->StopFloatingBall());
    EXPECT_EQ(WMError::WM_ERROR_FB_INTERNAL_ERROR, fbController->DestroyFloatingBallWindow());
    EXPECT_EQ(nullptr, fbController->GetFbWindow());
}

/**
 * @tc.name: RestoreFloatingBallAbility
 * @tc.desc: RestoreFloatingBallAbility
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerTest, RestoreFloatingBallAbility, TestSize.Level1)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<FbOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto fbController = sptr<FloatingBallController>::MakeSptr(mw, 100, nullptr);
    std::shared_ptr<AAFwk::Want> want = std::make_shared<AAFwk::Want>();
    fbController->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_EQ(WMError::WM_ERROR_FB_STATE_ABNORMALLY, fbController->RestoreMainWindow(want));
    fbController->window_ = mw;
    fbController->curState_ = FbWindowState::STATE_STOPPING;
    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, fbController->RestoreMainWindow(want));
    fbController->curState_ = FbWindowState::STATE_STARTED;
    EXPECT_CALL(*(mw), RestoreFbMainWindow(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    EXPECT_EQ(WMError::WM_OK, fbController->RestoreMainWindow(want));
}

/**
 * @tc.name: GetFloatingBallInfo
 * @tc.desc: GetFloatingBallInfo
 * @tc.type: FUNC
 */
HWTEST_F(FloatingBallControllerTest, GetFloatingBallInfo, TestSize.Level1)
{
    auto mw = sptr<MockWindow>::MakeSptr();
    ASSERT_NE(nullptr, mw);
    auto option = sptr<FbOption>::MakeSptr();
    ASSERT_NE(nullptr, option);
    auto fbController = sptr<FloatingBallController>::MakeSptr(mw, 100, nullptr);
    EXPECT_CALL(*(mw), GetFloatingBallWindowId(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    fbController->window_ = mw;
    uint32_t mockId = 1;
    EXPECT_EQ(WMError::WM_OK, fbController->GetFloatingBallWindowInfo(mockId));
}
}
}
}