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
 * See the License for the specific language governing permissions permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include <gmock/gmock.h>
#include "float_view_controller.h"
#include "float_window_manager.h"
#include "floating_ball_controller.h"
#include "floating_ball_manager.h"
#include "picture_in_picture_manager.h"
#include "float_view_option.h"
#include "float_view_manager.h"
#include "floating_ball_option.h"
#include "window.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class MockFloatViewController : public FloatViewController {
public:
    MockFloatViewController(const FvOption& option, napi_env env) : FloatViewController(option, env) {}
    virtual ~MockFloatViewController() = default;

    MOCK_METHOD1(StartFloatViewSingle, WMError(bool showWhenCreate));
    MOCK_METHOD0(StopFloatViewFromClientSingle, WMError());
    MOCK_CONST_METHOD0(GetWindow, sptr<Window>());
    MOCK_METHOD0(GetCurState, FvWindowState());
    MOCK_METHOD0(IsBind, bool());
    MOCK_METHOD1(SetBindState, void(bool isBind));
    MOCK_METHOD1(SetBindWindowId, void(uint32_t windowId));
};

class MockFloatingBallController : public FloatingBallController {
public:
    MockFloatingBallController(const sptr<Window>& mainWindow, const uint32_t& mainWindowId, void* context)
        : FloatingBallController(mainWindow, mainWindowId, context) {}
    virtual ~MockFloatingBallController() = default;

    MOCK_METHOD2(StartFloatingBallSingle, WMError(const sptr<FbOption>& option, bool showWhenCreate));
    MOCK_METHOD0(StopFloatingBallFromClientSingle, WMError());
    MOCK_METHOD1(SetOption, void(const sptr<FbOption>& option));
    MOCK_METHOD1(SetBindState, void(bool isBind));
    MOCK_METHOD1(SetBindWindowId, void(uint32_t windowId));
    MOCK_CONST_METHOD0(GetFbWindow, sptr<Window>());
    MOCK_METHOD0(GetCurState, FbWindowState());
    MOCK_METHOD0(IsBind, bool());
};

class FloatWindowManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<MockFloatViewController> fvController_;
    sptr<MockFloatingBallController> fbController_;
    sptr<FvOption> fvOption_;
    sptr<FbOption> fbOption_;
};

void FloatWindowManagerTest::SetUpTestCase()
{
}

void FloatWindowManagerTest::TearDownTestCase()
{
}

void FloatWindowManagerTest::SetUp()
{
    fvOption_ = sptr<FvOption>::MakeSptr();
    fbOption_ = sptr<FbOption>::MakeSptr();
    ASSERT_NE(nullptr, fvOption_);
    ASSERT_NE(nullptr, fbOption_);
    
    fvController_ = sptr<MockFloatViewController>::MakeSptr(*fvOption_, nullptr);
    fbController_ = sptr<MockFloatingBallController>::MakeSptr(nullptr, 0, nullptr);
    
    ASSERT_NE(nullptr, fvController_);
    ASSERT_NE(nullptr, fbController_);
}

void FloatWindowManagerTest::TearDown()
{
    if (fvController_ != nullptr) {
        FloatViewManager::RemoveActiveController(fvController_);
    }
    fvController_ = nullptr;
    fbController_ = nullptr;
    fvOption_ = nullptr;
    fbOption_ = nullptr;
}

namespace {
HWTEST_F(FloatWindowManagerTest, GetControllerId, TestSize.Level1)
{
    std::string id1 = FloatWindowManager::GetControllerId();
    std::string id2 = FloatWindowManager::GetControllerId();
    EXPECT_NE(id1, id2);
    EXPECT_FALSE(id1.empty());
    EXPECT_FALSE(id2.empty());

    std::set<std::string> ids;
    for (int i = 0; i < 10; i++) {
        std::string id = FloatWindowManager::GetControllerId();
        EXPECT_TRUE(ids.insert(id).second);
    }
    EXPECT_EQ(10, ids.size());
}

HWTEST_F(FloatWindowManagerTest, Bind, TestSize.Level1)
{
    EXPECT_CALL(*fbController_, GetCurState()).WillRepeatedly(Return(FbWindowState::STATE_UNDEFINED));
    EXPECT_CALL(*fbController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fvController_, GetCurState()).WillRepeatedly(Return(FvWindowState::FV_STATE_UNDEFINED));
    EXPECT_CALL(*fvController_, IsBind()).WillRepeatedly(Return(false));
    
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::Bind(fvController_, fbController_, *fbOption_));
    EXPECT_TRUE(fvController_->IsBind());
    
    EXPECT_CALL(*fbController_, IsBind()).WillOnce(Return(true));
    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, FloatWindowManager::Bind(fvController_, fbController_, *fbOption_));
    EXPECT_CALL(*fbController_, GetCurState()).WillOnce(Return(FbWindowState::STATE_STARTING));
    EXPECT_CALL(*fbController_, IsBind()).WillOnce(Return(false));
    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, FloatWindowManager::Bind(fvController_, fbController_, *fbOption_));
    EXPECT_CALL(*fbController_, GetCurState()).WillOnce(Return(FbWindowState::STATE_STARTING));
    EXPECT_CALL(*fbController_, IsBind()).WillOnce(Return(true));
    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, FloatWindowManager::Bind(fvController_, fbController_, *fbOption_));

    EXPECT_CALL(*fvController_, IsBind()).WillOnce(Return(true));
    EXPECT_EQ(WMError::WM_ERROR_FV_INVALID_STATE, FloatWindowManager::Bind(fvController_, fbController_, *fbOption_));
    EXPECT_CALL(*fvController_, GetCurState()).WillOnce(Return(FvWindowState::FV_STATE_STARTING));
    EXPECT_CALL(*fvController_, IsBind()).WillOnce(Return(false));
    EXPECT_EQ(WMError::WM_ERROR_FV_INVALID_STATE, FloatWindowManager::Bind(fvController_, fbController_, *fbOption_));
    EXPECT_CALL(*fvController_, GetCurState()).WillOnce(Return(FvWindowState::FV_STATE_STARTING));
    EXPECT_CALL(*fvController_, IsBind()).WillOnce(Return(true));
    EXPECT_EQ(WMError::WM_ERROR_FV_INVALID_STATE, FloatWindowManager::Bind(fvController_, fbController_, *fbOption_));
}

HWTEST_F(FloatWindowManagerTest, UnBind, TestSize.Level1)
{
    EXPECT_CALL(*fbController_, GetCurState()).WillRepeatedly(Return(FbWindowState::STATE_UNDEFINED));
    EXPECT_CALL(*fbController_, IsBind()).WillRepeatedly(Return(true));
    EXPECT_CALL(*fvController_, GetCurState()).WillRepeatedly(Return(FvWindowState::FV_STATE_UNDEFINED));
    EXPECT_CALL(*fvController_, IsBind()).WillRepeatedly(Return(true));

    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, FloatWindowManager::UnBind(fvController_, fbController_));

    EXPECT_CALL(*fbController_, IsBind()).WillOnce(Return(false));
    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, FloatWindowManager::UnBind(fvController_, fbController_));
    EXPECT_CALL(*fbController_, GetCurState()).WillOnce(Return(FbWindowState::STATE_STARTING));
    EXPECT_CALL(*fbController_, IsBind()).WillOnce(Return(true));
    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, FloatWindowManager::UnBind(fvController_, fbController_));
    EXPECT_CALL(*fbController_, GetCurState()).WillOnce(Return(FbWindowState::STATE_STARTING));
    EXPECT_CALL(*fbController_, IsBind()).WillOnce(Return(false));
    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, FloatWindowManager::UnBind(fvController_, fbController_));
    
    EXPECT_CALL(*fvController_, IsBind()).WillOnce(Return(false));
    EXPECT_EQ(WMError::WM_ERROR_FV_INVALID_STATE, FloatWindowManager::UnBind(fvController_, fbController_));
    EXPECT_CALL(*fvController_, GetCurState()).WillOnce(Return(FvWindowState::FV_STATE_STARTING));
    EXPECT_CALL(*fvController_, IsBind()).WillOnce(Return(true));
    EXPECT_EQ(WMError::WM_ERROR_FV_INVALID_STATE, FloatWindowManager::UnBind(fvController_, fbController_));
    EXPECT_CALL(*fvController_, GetCurState()).WillOnce(Return(FvWindowState::FV_STATE_STARTING));
    EXPECT_CALL(*fvController_, IsBind()).WillOnce(Return(false));
    EXPECT_EQ(WMError::WM_ERROR_FV_INVALID_STATE, FloatWindowManager::UnBind(fvController_, fbController_));

    EXPECT_EQ(WMError::WM_ERROR_FB_INVALID_STATE, FloatWindowManager::UnBind(fvController_, fbController_));
}

HWTEST_F(FloatWindowManagerTest, GetBoundFloatingBall, TestSize.Level1)
{
    auto boundFb = FloatWindowManager::GetBoundFloatingBall(fvController_);
    EXPECT_EQ(nullptr, boundFb);

    sptr<MockFloatViewController> nullController;
    boundFb = FloatWindowManager::GetBoundFloatingBall(nullController);
    EXPECT_EQ(nullptr, boundFb);

    EXPECT_CALL(*fbController_, GetCurState()).WillRepeatedly(Return(FbWindowState::STATE_UNDEFINED));
    EXPECT_CALL(*fbController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fbController_, SetOption(_)).Times(1);
    EXPECT_CALL(*fbController_, SetBindState(true)).Times(1);
    EXPECT_CALL(*fvController_, GetCurState()).WillRepeatedly(Return(FvWindowState::FV_STATE_UNDEFINED));
    EXPECT_CALL(*fvController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fvController_, SetBindState(true)).Times(1);
    
    FloatWindowManager::Bind(fvController_, fbController_, *fbOption_);
    
    boundFb = FloatWindowManager::GetBoundFloatingBall(fvController_);
    EXPECT_NE(nullptr, boundFb);
    EXPECT_EQ(fbController_.GetRefPtr(), boundFb.GetRefPtr());
}

HWTEST_F(FloatWindowManagerTest, GetBoundFloatView, TestSize.Level1)
{
    auto boundFv = FloatWindowManager::GetBoundFloatView(fbController_);
    EXPECT_EQ(nullptr, boundFv);

    EXPECT_CALL(*fbController_, GetCurState()).WillRepeatedly(Return(FbWindowState::STATE_UNDEFINED));
    EXPECT_CALL(*fbController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fbController_, SetOption(_)).Times(1);
    EXPECT_CALL(*fbController_, SetBindState(true)).Times(1);
    EXPECT_CALL(*fvController_, GetCurState()).WillRepeatedly(Return(FvWindowState::FV_STATE_UNDEFINED));
    EXPECT_CALL(*fvController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fvController_, SetBindState(true)).Times(1);
    
    FloatWindowManager::Bind(fvController_, fbController_, *fbOption_);
    
    boundFv = FloatWindowManager::GetBoundFloatView(fbController_);
    EXPECT_NE(nullptr, boundFv);
    EXPECT_EQ(fvController_.GetRefPtr(), boundFv.GetRefPtr());

    EXPECT_CALL(*fbController_, IsBind()).WillRepeatedly(Return(true));
    EXPECT_CALL(*fbController_, SetBindState(false)).Times(1);
    EXPECT_CALL(*fbController_, SetBindWindowId(INVALID_WINDOW_ID)).Times(1);
    EXPECT_CALL(*fvController_, IsBind()).WillRepeatedly(Return(true));
    EXPECT_CALL(*fvController_, SetBindState(false)).Times(1);
    FloatWindowManager::UnBind(fvController_, fbController_);
    boundFv = FloatWindowManager::GetBoundFloatView(fbController_);
    EXPECT_EQ(nullptr, boundFv);

    sptr<FloatingBallController> nullController;
    boundFv = FloatWindowManager::GetBoundFloatView(nullController);
    EXPECT_EQ(nullptr, boundFv);
}

HWTEST_F(FloatWindowManagerTest, Token, TestSize.Level1)
{
    uint64_t token = FloatWindowManager::AcquireToken();
    EXPECT_NE(0, token);
    FloatWindowManager::ReleaseToken(token);

    uint64_t token1 = FloatWindowManager::AcquireToken();
    EXPECT_NE(0, token1);
    uint64_t token2 = FloatWindowManager::AcquireToken();
    EXPECT_EQ(0, token2);
    FloatWindowManager::ReleaseToken(token1);
    FloatWindowManager::ReleaseToken(token2);

    token2 = FloatWindowManager::AcquireToken();
    EXPECT_NE(0, token2);
    FloatWindowManager::ReleaseToken(token2);

    FloatWindowManager::tokenSeq_ = 0;
    uint64_t token3 = FloatWindowManager::AcquireToken();
    EXPECT_EQ(1, token3);
    FloatWindowManager::ReleaseToken(token3);

    uint64_t token4 = FloatWindowManager::AcquireToken();
    EXPECT_EQ(2, token4);
    FloatWindowManager::ReleaseToken(token4 + 1);
    FloatWindowManager::ReleaseToken(token4);
}

HWTEST_F(FloatWindowManagerTest, IsFloatViewConflict, TestSize.Level1)
{
    EXPECT_FALSE(FloatWindowManager::IsFloatViewConflict(fvController_));

    sptr<PictureInPictureControllerBase> mockController;
    PictureInPictureManager::runningControllers_.push_back(mockController);
    EXPECT_TRUE(FloatWindowManager::IsFloatViewConflict(fvController_));
    PictureInPictureManager::runningControllers_.erase(PictureInPictureManager::runningControllers_.end());

    sptr<MockFloatViewController> nullPtr = nullptr;
    wptr<MockFloatViewController> weakPtr = nullPtr;
    EXPECT_TRUE(FloatWindowManager::IsFloatViewConflict(weakPtr));

    auto mockFvController = sptr<MockFloatViewController>::MakeSptr(*fvOption_, nullptr);
    EXPECT_TRUE(FloatWindowManager::IsFloatViewConflict(mockFvController));
    FloatWindowManager::Bind(mockFvController, fbController_, *fbOption_);
    EXPECT_TRUE(FloatWindowManager::IsFloatViewConflict(mockFvController));
    FloatWindowManager::UnBind(mockFvController, fbController_);
    EXPECT_TRUE(FloatWindowManager::IsFloatViewConflict(mockFvController));
    FloatWindowManager::Bind(mockFvController, fbController_, *fbOption_);
    auto option = sptr<FbOption>::MakeSptr();
    auto fakeController = sptr<MockFloatingBallController>::MakeSptr(nullptr, 0, nullptr);
    FloatingBallManager::SetActiveController(fakeController);
    EXPECT_TRUE(FloatWindowManager::IsFloatViewConflict(mockFvController));
    FloatingBallManager::SetActiveController(fbController_);
    EXPECT_FALSE(FloatWindowManager::IsFloatViewConflict(mockFvController));
    
    FloatingBallManager::RemoveActiveController(fbController_);
    FloatWindowManager::UnBind(mockFvController, fbController_);
}

HWTEST_F(FloatWindowManagerTest, IsFloatingBallConflict, TestSize.Level1)
{
    EXPECT_FALSE(FloatWindowManager::IsFloatingBallConflict(fbController_));

    FloatViewManager::SetActiveController(fvController_);
    sptr<MockFloatingBallController> nullPtr = nullptr;
    wptr<MockFloatingBallController> weakPtr = nullPtr;
    EXPECT_TRUE(FloatWindowManager::IsFloatingBallConflict(weakPtr));

    auto mockFbController = sptr<MockFloatingBallController>::MakeSptr(nullptr, 0, nullptr);
    EXPECT_TRUE(FloatWindowManager::IsFloatingBallConflict(mockFbController));
    FloatWindowManager::Bind(fvController_, mockFbController, *fbOption_);
    EXPECT_TRUE(FloatWindowManager::IsFloatingBallConflict(mockFbController));
    FloatWindowManager::UnBind(fvController_, mockFbController);
    EXPECT_TRUE(FloatWindowManager::IsFloatingBallConflict(mockFbController));
    FloatWindowManager::Bind(fvController_, mockFbController, *fbOption_);
    auto option = sptr<FvOption>::MakeSptr();
    auto fakeController = sptr<MockFloatViewController>::MakeSptr(*option, nullptr);
    FloatViewManager::SetActiveController(fakeController);
    EXPECT_TRUE(FloatWindowManager::IsFloatingBallConflict(mockFbController));
    FloatViewManager::SetActiveController(fbController_);
    EXPECT_FALSE(FloatWindowManager::IsFloatingBallConflict(mockFbController));
}

HWTEST_F(FloatWindowManagerTest, IsPipConflict, TestSize.Level1)
{
    auto mockFvController = sptr<MockFloatViewController>::MakeSptr(*fvOption_, nullptr);
    FloatViewManager::SetActiveController(mockFvController);
    EXPECT_TRUE(FloatWindowManager::IsPipConflict());
    FloatViewManager::RemoveActiveController(mockFvController);
    EXPECT_FALSE(FloatWindowManager::IsPipConflict());
}

HWTEST_F(FloatWindowManagerTest, StartBindFloatView, TestSize.Level1)
{
    auto mockFvController = sptr<MockFloatViewController>::MakeSptr(*fvOption_, nullptr);
    auto mockWindow = sptr<Window>::MakeSptr();
    auto mockFbWindow = sptr<Window>::MakeSptr();

    EXPECT_CALL(*fbController_, GetCurState()).WillRepeatedly(Return(FbWindowState::STATE_UNDEFINED));
    EXPECT_CALL(*fbController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fbController_, SetOption(_)).WillRepeatedly(Return());
    EXPECT_CALL(*fbController_, SetBindState(true)).WillRepeatedly(Return());
    EXPECT_CALL(*mockFvController, GetCurState()).WillRepeatedly(Return(FvWindowState::FV_STATE_UNDEFINED));
    EXPECT_CALL(*mockFvController, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*mockFvController, SetBindState(true)).WillRepeatedly(Return());
    FloatWindowManager::Bind(mockFvController, fbController_, *fbOption_);

    EXPECT_CALL(*mockFvController, StartFloatViewSingle(true)).WillOnce(Return(WMError::WM_ERROR_INVALID_OPERATION));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, FloatWindowManager::StartBindFloatView(mockFvController));

    EXPECT_CALL(*mockFvController, StartFloatViewSingle(true)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*mockFvController, GetWindow()).WillOnce(Return(nullptr));
    EXPECT_CALL(*mockFvController, StopFloatViewFromClientSingle()).WillOnce(Return(WMError::WM_OK));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, FloatWindowManager::StartBindFloatView(mockFvController));

    EXPECT_CALL(*mockFvController, StartFloatViewSingle(true)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*mockFvController, GetWindow()).WillOnce(Return(mockWindow));
    EXPECT_CALL(*fbController_, StartFloatingBallSingle(_, false)).WillOnce(Return(WMError::WM_ERROR_INVALID_OPERATION));
    EXPECT_CALL(*fbController_, SetBindWindowId(INVALID_WINDOW_ID)).WillOnce(Return());
    EXPECT_CALL(*mockFvController, StopFloatViewFromClientSingle()).WillOnce(Return(WMError::WM_OK));
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StartBindFloatView(mockFvController));

    EXPECT_CALL(*mockFvController, StartFloatViewSingle(true)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*mockFvController, GetWindow()).WillOnce(Return(mockWindow));
    EXPECT_CALL(*fbController_, StartFloatingBallSingle(_, false)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*fbController_, GetFbWindow()).WillOnce(Return(nullptr));
    EXPECT_CALL(*fbController_, SetBindWindowId(INVALID_WINDOW_ID)).WillOnce(Return());
    EXPECT_CALL(*mockFvController, StopFloatViewFromClientSingle()).WillOnce(Return(WMError::WM_OK));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, FloatWindowManager::StartBindFloatView(mockFvController));

    EXPECT_CALL(*mockFvController, StartFloatViewSingle(true)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*mockFvController, GetWindow()).WillOnce(Return(mockWindow));
    EXPECT_CALL(*fbController_, StartFloatingBallSingle(_, false)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*fbController_, GetFbWindow()).WillOnce(Return(mockFbWindow));
    EXPECT_CALL(*mockFvController, SetBindWindowId(_)).WillOnce(Return());
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::StartBindFloatView(mockFvController));

    FloatWindowManager::UnBind(mockFvController, fbController_);
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StartBindFloatView(mockFvController));

    wptr<MockFloatViewController> nullController;
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StartBindFloatView(nullController));
}

HWTEST_F(FloatWindowManagerTest, StartBindFloatingBall, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StartBindFloatingBall(fbController_, fbOption_));
    wptr<FloatingBallController> nullController;
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StartBindFloatingBall(nullController, fbOption_));

    auto mockFvController = sptr<MockFloatViewController>::MakeSptr(*fvOption_, nullptr);
    auto mockWindow = sptr<Window>::MakeSptr();
    auto mockFbWindow = sptr<Window>::MakeSptr();
    
    EXPECT_CALL(*fbController_, GetCurState()).WillRepeatedly(Return(FbWindowState::STATE_UNDEFINED));
    EXPECT_CALL(*fbController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fbController_, SetOption(_)).Times(1);
    EXPECT_CALL(*fbController_, SetBindState(true)).Times(1);
    
    EXPECT_CALL(*mockFvController, GetCurState()).WillRepeatedly(Return(FvWindowState::FV_STATE_UNDEFINED));
    EXPECT_CALL(*mockFvController, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*mockFvController, SetBindState(true)).Times(1);
    
    FloatWindowManager::Bind(mockFvController, fbController_, *fbOption_);
    FloatWindowManager::UnBind(mockFvController, fbController_);
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StartBindFloatingBall(fbController_, fbOption_));

    EXPECT_CALL(*fbController_, SetOption(_)).Times(1);
    EXPECT_CALL(*fbController_, SetBindState(true)).Times(1);
    EXPECT_CALL(*mockFvController, SetBindState(true)).Times(1);
    FloatWindowManager::Bind(mockFvController, fbController_, *fbOption_);

    EXPECT_CALL(*fbController_, StartFloatingBallSingle(_, true)).WillOnce(Return(WMError::WM_ERROR_INVALID_OPERATION));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, FloatWindowManager::StartBindFloatingBall(fbController_, fbOption_));

    EXPECT_CALL(*fbController_, StartFloatingBallSingle(_, true)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*fbController_, GetFbWindow()).WillOnce(Return(nullptr));
    EXPECT_CALL(*fbController_, StopFloatingBallFromClientSingle()).Times(1);
    EXPECT_CALL(*mockFvController, SetBindWindowId(_)).Times(1);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, FloatWindowManager::StartBindFloatingBall(fbController_, fbOption_));

    EXPECT_CALL(*fbController_, StartFloatingBallSingle(_, true)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*fbController_, GetFbWindow()).WillOnce(Return(mockFbWindow));
    EXPECT_CALL(*fbController_, SetBindWindowId(_)).Times(2);
    EXPECT_CALL(*mockFvController, StartFloatViewSingle(false)).WillOnce(Return(WMError::WM_ERROR_INVALID_OPERATION));
    EXPECT_CALL(*mockFvController, SetBindWindowId(_)).Times(2);
    EXPECT_CALL(*fbController_, StopFloatingBallFromClientSingle()).Times(1);
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StartBindFloatingBall(fbController_, fbOption_));

    EXPECT_CALL(*fbController_, StartFloatingBallSingle(_, true)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*fbController_, GetFbWindow()).WillOnce(Return(mockFbWindow));
    EXPECT_CALL(*fbController_, SetBindWindowId(_)).Times(2);
    EXPECT_CALL(*mockFvController, StartFloatViewSingle(false)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*mockFvController, GetWindow()).WillOnce(Return(nullptr));
    EXPECT_CALL(*mockFvController, SetBindWindowId(_)).Times(3);
    EXPECT_CALL(*fbController_, StopFloatingBallFromClientSingle()).Times(1);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, FloatWindowManager::StartBindFloatingBall(fbController_, fbOption_));

    EXPECT_CALL(*fbController_, StartFloatingBallSingle(_, true)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*fbController_, GetFbWindow()).WillOnce(Return(mockFbWindow));
    EXPECT_CALL(*fbController_, SetBindWindowId(_)).Times(2);
    EXPECT_CALL(*mockFvController, StartFloatViewSingle(false)).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*mockFvController, GetWindow()).WillOnce(Return(mockWindow));
    EXPECT_CALL(*mockFvController, SetBindWindowId(_)).Times(2);
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::StartBindFloatingBall(fbController_, fbOption_));
}

HWTEST_F(FloatWindowManagerTest, StopBindFloatView, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StopBindFloatView(fvController_));
    wptr<MockFloatViewController> nullController;
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StopBindFloatView(nullController));

    auto mockFvController = sptr<MockFloatViewController>::MakeSptr(*fvOption_, nullptr);
    
    EXPECT_CALL(*fbController_, GetCurState()).WillRepeatedly(Return(FbWindowState::STATE_UNDEFINED));
    EXPECT_CALL(*fbController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fbController_, SetOption(_)).Times(1);
    EXPECT_CALL(*fbController_, SetBindState(true)).Times(1);
    
    EXPECT_CALL(*mockFvController, GetCurState()).WillRepeatedly(Return(FvWindowState::FV_STATE_UNDEFINED));
    EXPECT_CALL(*mockFvController, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*mockFvController, SetBindState(true)).Times(1);
    
    FloatWindowManager::Bind(mockFvController, fbController_, *fbOption_);
    FloatWindowManager::UnBind(mockFvController, fbController_);
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StopBindFloatView(mockFvController));

    EXPECT_CALL(*fbController_, SetOption(_)).Times(1);
    EXPECT_CALL(*fbController_, SetBindState(true)).Times(1);
    EXPECT_CALL(*mockFvController, SetBindState(true)).Times(1);
    FloatWindowManager::Bind(mockFvController, fbController_, *fbOption_);

    EXPECT_CALL(*mockFvController, StopFloatViewFromClientSingle()).WillOnce(Return(WMError::WM_ERROR_INVALID_OPERATION));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, FloatWindowManager::StopBindFloatView(mockFvController));

    EXPECT_CALL(*mockFvController, StopFloatViewFromClientSingle()).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*fbController_, StopFloatingBallFromClientSingle()).WillOnce(Return(WMError::WM_ERROR_INVALID_OPERATION));
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StopBindFloatView(mockFvController));

    EXPECT_CALL(*mockFvController, StopFloatViewFromClientSingle()).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*fbController_, StopFloatingBallFromClientSingle()).WillOnce(Return(WMError::WM_OK));
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::StopBindFloatView(mockFvController));
}

HWTEST_F(FloatWindowManagerTest, StopBindFloatingBall, TestSize.Level1)
{
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StopBindFloatingBall(fbController_));
    wptr<FloatingBallController> nullController;
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StopBindFloatingBall(nullController));

    auto mockFvController = sptr<MockFloatViewController>::MakeSptr(*fvOption_, nullptr);
    
    EXPECT_CALL(*fbController_, GetCurState()).WillRepeatedly(Return(FbWindowState::STATE_UNDEFINED));
    EXPECT_CALL(*fbController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fbController_, SetOption(_)).Times(1);
    EXPECT_CALL(*fbController_, SetBindState(true)).Times(1);
    
    EXPECT_CALL(*mockFvController, GetCurState()).WillRepeatedly(Return(FvWindowState::FV_STATE_UNDEFINED));
    EXPECT_CALL(*mockFvController, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*mockFvController, SetBindState(true)).Times(1);
    
    FloatWindowManager::Bind(mockFvController, fbController_, *fbOption_);
    FloatWindowManager::UnBind(mockFvController, fbController_);
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StopBindFloatingBall(fbController_));

    EXPECT_CALL(*fbController_, SetOption(_)).Times(1);
    EXPECT_CALL(*fbController_, SetBindState(true)).Times(1);
    EXPECT_CALL(*mockFvController, SetBindState(true)).Times(1);
    FloatWindowManager::Bind(mockFvController, fbController_, *fbOption_);

    EXPECT_CALL(*fbController_, StopFloatingBallFromClientSingle()).WillOnce(Return(WMError::WM_ERROR_INVALID_OPERATION));
    EXPECT_EQ(WMError::WM_ERROR_INVALID_OPERATION, FloatWindowManager::StopBindFloatingBall(fbController_));

    EXPECT_CALL(*fbController_, StopFloatingBallFromClientSingle()).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*mockFvController, StopFloatViewFromClientSingle()).WillOnce(Return(WMError::WM_ERROR_INVALID_OPERATION));
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StopBindFloatingBall(fbController_));

    EXPECT_CALL(*fbController_, StopFloatingBallFromClientSingle()).WillOnce(Return(WMError::WM_OK));
    EXPECT_CALL(*mockFvController, StopFloatViewFromClientSingle()).WillOnce(Return(WMError::WM_OK));
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::StopBindFloatingBall(fbController_));
}

HWTEST_F(FloatWindowManagerTest, BindUnBindCycle, TestSize.Level1)
{
    EXPECT_CALL(*fbController_, GetCurState()).WillRepeatedly(Return(FbWindowState::STATE_UNDEFINED));
    EXPECT_CALL(*fbController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fbController_, SetOption(_)).Times(1);
    EXPECT_CALL(*fbController_, SetBindState(true)).Times(1);
    EXPECT_CALL(*fvController_, GetCurState()).WillRepeatedly(Return(FvWindowState::FV_STATE_UNDEFINED));
    EXPECT_CALL(*fvController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fvController_, SetBindState(true)).Times(1);
    
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::Bind(fvController_, fbController_, *fbOption_));
    EXPECT_TRUE(fvController_->IsBind());
    
    EXPECT_CALL(*fbController_, IsBind()).WillRepeatedly(Return(true));
    EXPECT_CALL(*fbController_, SetBindState(false)).Times(1);
    EXPECT_CALL(*fbController_, SetBindWindowId(INVALID_WINDOW_ID)).Times(1);
    EXPECT_CALL(*fvController_, IsBind()).WillRepeatedly(Return(true));
    EXPECT_CALL(*fvController_, SetBindState(false)).Times(1);
    
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::UnBind(fvController_, fbController_));
    EXPECT_FALSE(fvController_->IsBind());
}

HWTEST_F(FloatWindowManagerTest, MultiplePairsTest, TestSize.Level1)
{
    auto fvController2 = sptr<MockFloatViewController>::MakeSptr(*fvOption_, nullptr);
    auto fbController2 = sptr<MockFloatingBallController>::MakeSptr(nullptr, 0, nullptr);
    
    EXPECT_CALL(*fbController_, GetCurState()).WillRepeatedly(Return(FbWindowState::STATE_UNDEFINED));
    EXPECT_CALL(*fbController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fbController_, SetOption(_)).Times(1);
    EXPECT_CALL(*fbController_, SetBindState(true)).Times(1);
    EXPECT_CALL(*fvController_, GetCurState()).WillRepeatedly(Return(FvWindowState::FV_STATE_UNDEFINED));
    EXPECT_CALL(*fvController_, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fvController_, SetBindState(true)).Times(1);
    
    EXPECT_CALL(*fbController2, GetCurState()).WillRepeatedly(Return(FbWindowState::STATE_UNDEFINED));
    EXPECT_CALL(*fbController2, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fbController2, SetOption(_)).Times(1);
    EXPECT_CALL(*fbController2, SetBindState(true)).Times(1);
    EXPECT_CALL(*fvController2, GetCurState()).WillRepeatedly(Return(FvWindowState::FV_STATE_UNDEFINED));
    EXPECT_CALL(*fvController2, IsBind()).WillRepeatedly(Return(false));
    EXPECT_CALL(*fvController2, SetBindState(true)).Times(1);
    
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::Bind(fvController_, fbController_, *fbOption_));
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::Bind(fvController2, fbController2, *fbOption_));
    
    auto boundFb1 = FloatWindowManager::GetBoundFloatingBall(fvController_);
    auto boundFb2 = FloatWindowManager::GetBoundFloatingBall(fvController2);
    
    EXPECT_NE(nullptr, boundFb1);
    EXPECT_NE(nullptr, boundFb2);
    EXPECT_EQ(fbController_.GetRefPtr(), boundFb1.GetRefPtr());
    EXPECT_EQ(fbController2.GetRefPtr(), boundFb2.GetRefPtr());
}
}
}
}
