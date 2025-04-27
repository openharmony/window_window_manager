/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "color_parser.h"
#include "mock_session.h"
#include "mock_session_stub.h"
#include "mock_uicontent.h"
#include "mock_window.h"
#include "parameters.h"
#include "scene_board_judgement.h"
#include "window_helper.h"
#include "window_session_impl.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSessionImplTest5 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowSessionImplTest5::SetUpTestCase() {}

void WindowSessionImplTest5::TearDownTestCase() {}

void WindowSessionImplTest5::SetUp()
{
}

void WindowSessionImplTest5::TearDown()
{
}

namespace {
/**
 * @tc.name: GetSubWidnows
 * @tc.desc: GetSubWidnows
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest5, GetSubWidnows, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowSessionImplTest5: GetSubWidnows start";
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("GetSubWidnows");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->SetPersistentId(1);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    std::vector<sptr<WindowSessionImpl>> subWindows;
    window->GetSubWidnows(1, subWindows);
    EXPECT_EQ(subWindows.size(), 0);

    sptr<WindowOption> subWindowOption = sptr<WindowOption>::MakeSptr();
    subWindowOption->SetWindowName("GetSubWidnows_subWindow");
    sptr<WindowSessionImpl> subWindow = sptr<WindowSessionImpl>::MakeSptr(subWindowOption);
    subWindow->property_->SetPersistentId(2);
    subWindow->property_->SetParentPersistentId(1);
    subWindow->hostSession_ = session;
    subWindow->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    subWindow->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    WindowSessionImpl::subWindowSessionMap_.insert(std::pair<int32_t,
        std::vector<sptr<WindowSessionImpl>>>(1, { subWindow }));

    subWindows.clear();
    window->GetSubWidnows(1, subWindows);
    ASSERT_EQ(subWindows.size(), 1);
    EXPECT_EQ(subWindows[0], subWindow);
    GTEST_LOG_(INFO) << "WindowSessionImplTest5: GetSubWidnows end";
}

/**
 * @tc.name: RemoveSubWindow
 * @tc.desc: RemoveSubWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest5, RemoveSubWindow, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("RemoveSubWindow");
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    SessionInfo sessionInfo = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session = sptr<SessionMocker>::MakeSptr(sessionInfo);
    window->hostSession_ = session;
    window->property_->SetPersistentId(1);
    window->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;

    sptr<WindowOption> subWindowOption = sptr<WindowOption>::MakeSptr();
    subWindowOption->SetWindowName("RemoveSubWindow_subWindow");
    sptr<WindowSessionImpl> subWindow = sptr<WindowSessionImpl>::MakeSptr(subWindowOption);
    subWindow->property_->SetPersistentId(2);
    subWindow->property_->SetParentPersistentId(1);
    subWindow->hostSession_ = session;
    subWindow->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    subWindow->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    WindowSessionImpl::subWindowSessionMap_.insert(std::pair<int32_t,
        std::vector<sptr<WindowSessionImpl>>>(1, { subWindow }));

    sptr<WindowOption> subWindowOption2 = sptr<WindowOption>::MakeSptr();
    subWindowOption2->SetWindowName("RemoveSubWindow_subWindow2");
    sptr<WindowSessionImpl> subWindow2 = sptr<WindowSessionImpl>::MakeSptr(subWindowOption2);
    subWindow2->property_->SetPersistentId(3);
    subWindow2->property_->SetParentPersistentId(1);
    subWindow2->hostSession_ = session;
    subWindow2->windowSystemConfig_.windowUIType_ = WindowUIType::PC_WINDOW;
    subWindow2->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    WindowSessionImpl::subWindowSessionMap_[1].push_back(subWindow2);

    subWindow->RemoveSubWindow(2);
    std::vector<sptr<WindowSessionImpl>> subWindows;
    window->GetSubWidnows(1, subWindows);
    ASSERT_EQ(subWindows.size(), 2);
    EXPECT_EQ(subWindows[0], subWindow);
    subWindow->RemoveSubWindow(1);
    subWindows.clear();
    window->GetSubWidnows(1, subWindows);
    ASSERT_EQ(subWindows.size(), 1);
    EXPECT_NE(subWindows[0], subWindow);
}

/**
 * @tc.name: DestroySubWindow01
 * @tc.desc: DestroySubWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest5, DestroySubWindow01, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("DestroySubWindow01");
    option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);

    sptr<WindowOption> subOption01 = sptr<WindowOption>::MakeSptr();
    subOption01->SetWindowName("DestroySubWindow01_subwindow01");
    subOption01->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sptr<WindowSessionImpl> subWindow01 = sptr<WindowSessionImpl>::MakeSptr(subOption01);
    subWindow01->property_->SetPersistentId(2);
    subWindow01->property_->SetParentPersistentId(1);
    SessionInfo sessionInfo1 = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session1 = sptr<SessionMocker>::MakeSptr(sessionInfo1);
    subWindow01->hostSession_ = session1;
    subWindow01->state_ = WindowState::STATE_CREATED;
    WindowSessionImpl::subWindowSessionMap_.insert(std::pair<int32_t,
        std::vector<sptr<WindowSessionImpl>>>(1, { subWindow01 }));

    sptr<WindowOption> subOption02 = sptr<WindowOption>::MakeSptr();
    subOption02->SetWindowName("DestroySubWindow01_subwindow02");
    subOption02->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sptr<WindowSessionImpl> subWindow02 = sptr<WindowSessionImpl>::MakeSptr(subOption01);
    subWindow02->property_->SetPersistentId(3);
    subWindow02->property_->SetParentPersistentId(2);
    SessionInfo sessionInfo2 = { "CreateTestBundle", "CreateTestModule", "CreateTestAbility" };
    sptr<SessionMocker> session2 = sptr<SessionMocker>::MakeSptr(sessionInfo2);
    subWindow02->hostSession_ = session2;
    subWindow02->state_ = WindowState::STATE_CREATED;
    WindowSessionImpl::subWindowSessionMap_.insert(std::pair<int32_t,
        std::vector<sptr<WindowSessionImpl>>>(2, { subWindow02 }));

    std::vector<sptr<WindowSessionImpl>> subWindows;
    window->GetSubWidnows(1, subWindows);
    EXPECT_EQ(subWindows.size(), 1);
    subWindows.clear();
    window->GetSubWidnows(2, subWindows);
    EXPECT_EQ(subWindows.size(), 1);
    window->DestroySubWindow();
    EXPECT_EQ(subWindow01->state_, WindowState::STATE_DESTROYED);
    EXPECT_EQ(subWindow02->state_, WindowState::STATE_DESTROYED);
    EXPECT_EQ(subWindow01->hostSession_, nullptr);
    EXPECT_EQ(subWindow02->hostSession_, nullptr);
    subWindows.clear();
    window->GetSubWidnows(1, subWindows);
    EXPECT_EQ(subWindows.size(), 0);
    subWindows.clear();
    window->GetSubWidnows(2, subWindows);
    EXPECT_EQ(subWindows.size(), 0);
}

/**
 * @tc.name: SetUniqueVirtualPixelRatioForSub
 * @tc.desc: SetUniqueVirtualPixelRatioForSub test
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest5, SetUniqueVirtualPixelRatioForSub, TestSize.Level1)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetUniqueVirtualPixelRatioForSub");
    option->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(1);

    sptr<WindowOption> subOption01 = sptr<WindowOption>::MakeSptr();
    subOption01->SetWindowName("SetUniqueVirtualPixelRatioForSub_subwindow01");
    subOption01->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sptr<WindowSessionImpl> subWindow01 = sptr<WindowSessionImpl>::MakeSptr(subOption01);
    subWindow01->property_->SetPersistentId(2);
    subWindow01->property_->SetParentPersistentId(1);
    subWindow01->useUniqueDensity_ = false;
    subWindow01->virtualPixelRatio_ = 0.5f;
    WindowSessionImpl::subWindowSessionMap_.insert(std::pair<int32_t,
        std::vector<sptr<WindowSessionImpl>>>(1, { subWindow01 }));

    sptr<WindowOption> subOption02 = sptr<WindowOption>::MakeSptr();
    subOption02->SetWindowName("SetUniqueVirtualPixelRatioForSub_subwindow02");
    subOption02->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    sptr<WindowSessionImpl> subWindow02 = sptr<WindowSessionImpl>::MakeSptr(subOption01);
    subWindow02->property_->SetPersistentId(3);
    subWindow02->property_->SetParentPersistentId(2);
    subWindow01->useUniqueDensity_ = false;
    subWindow01->virtualPixelRatio_ = 0.3f;
    WindowSessionImpl::subWindowSessionMap_.insert(std::pair<int32_t,
        std::vector<sptr<WindowSessionImpl>>>(2, { subWindow02 }));

    bool useUniqueDensity = true;
    float virtualPixelRatio = 1.0f;
    window->SetUniqueVirtualPixelRatioForSub(useUniqueDensity, virtualPixelRatio);
    EXPECT_EQ(subWindow01->useUniqueDensity_, useUniqueDensity);
    EXPECT_EQ(subWindow02->useUniqueDensity_, useUniqueDensity);
    EXPECT_NEAR(subWindow01->virtualPixelRatio_, virtualPixelRatio, 0.00001f);
    EXPECT_NEAR(subWindow02->virtualPixelRatio_, virtualPixelRatio, 0.00001f);
}

/**
 * @tc.name: SetFollowScreenChange
 * @tc.desc: SetFollowScreenChange
 * @tc.type: FUNC
 */
HWTEST_F(WindowSessionImplTest5, SetFollowScreenChange, Function | SmallTest | Level2)
{
    sptr<WindowOption> option = sptr<WindowOption>::MakeSptr();
    option->SetWindowName("SetFollowScreenChange");

    sptr<WindowSessionImpl> window = sptr<WindowSessionImpl>::MakeSptr(option);
    window->property_->SetPersistentId(0);
    WMError ret = window->SetFollowScreenChange(true);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW, ret);

    window->property_->SetPersistentId(1);
    window->property_->SetWindowType(WindowType::APP_SUB_WINDOW_END);
    ret = window->SetFollowScreenChange(true);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE, ret);

    window->property_->SetWindowType(WindowType::WINDOW_TYPE_UI_EXTENSION);
    ret = window->SetFollowScreenChange(true);
    EXPECT_EQ(WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE, ret);

    window->property_->SetWindowType(WindowType::SYSTEM_WINDOW_BASE);
    ret = window->SetFollowScreenChange(true);
    EXPECT_EQ(WMError::WM_OK, ret);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
