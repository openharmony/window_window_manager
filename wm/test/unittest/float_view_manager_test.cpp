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
#include "float_view_controller.h"
#include "float_view_manager.h"
#include "float_view_option.h"
#include "window.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class FloatViewManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<FloatViewController> fvController_;
    sptr<FvOption> option_;
};

void FloatViewManagerTest::SetUpTestCase()
{
}

void FloatViewManagerTest::TearDownTestCase()
{
}

void FloatViewManagerTest::SetUp()
{
    option_ = sptr<FvOption>::MakeSptr();
    ASSERT_NE(nullptr, option_);
    fvController_ = sptr<FloatViewController>::MakeSptr(*option_, nullptr);
    ASSERT_NE(nullptr, fvController_);
}

void FloatViewManagerTest::TearDown()
{
    FloatViewManager::RemoveActiveController(fvController_);
    fvController_ = nullptr;
    option_ = nullptr;
}

namespace {
/**
 * @tc.name: HasActiveController
 * @tc.desc: Test HasActiveController with and without active controller
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewManagerTest, HasActiveController, TestSize.Level1)
{
    EXPECT_FALSE(FloatViewManager::HasActiveController());

    FloatViewManager::SetActiveController(fvController_);
    EXPECT_TRUE(FloatViewManager::HasActiveController());
    FloatViewManager::RemoveActiveController(fvController_);
}

/**
 * @tc.name: IsActiveController
 * @tc.desc: Test IsActiveController with various scenarios
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewManagerTest, IsActiveController, TestSize.Level1)
{
    EXPECT_FALSE(FloatViewManager::IsActiveController(fvController_));

    FloatViewManager::SetActiveController(fvController_);
    EXPECT_TRUE(FloatViewManager::IsActiveController(fvController_));

    auto otherController = sptr<FloatViewController>::MakeSptr(*option_, nullptr);
    EXPECT_FALSE(FloatViewManager::IsActiveController(otherController));

    FloatViewManager::RemoveActiveController(fvController_);
}

/**
 * @tc.name: SetActiveController
 * @tc.desc: Test SetActiveController with null and valid controller
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewManagerTest, SetActiveController, TestSize.Level1)
{
    wptr<FloatViewController> nullController;
    FloatViewManager::SetActiveController(nullController);
    EXPECT_FALSE(FloatViewManager::HasActiveController());

    FloatViewManager::SetActiveController(fvController_);
    EXPECT_TRUE(FloatViewManager::HasActiveController());
    EXPECT_TRUE(FloatViewManager::IsActiveController(fvController_));
    FloatViewManager::RemoveActiveController(fvController_);
}

/**
 * @tc.name: RemoveActiveController
 * @tc.desc: Test RemoveActiveController with various scenarios
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewManagerTest, RemoveActiveController, TestSize.Level1)
{
    FloatViewManager::RemoveActiveController(fvController_);
    EXPECT_FALSE(FloatViewManager::HasActiveController());

    FloatViewManager::SetActiveController(fvController_);
    EXPECT_TRUE(FloatViewManager::HasActiveController());
    FloatViewManager::RemoveActiveController(fvController_);
    EXPECT_FALSE(FloatViewManager::HasActiveController());

    FloatViewManager::SetActiveController(fvController_);
    auto otherController = sptr<FloatViewController>::MakeSptr(*option_, nullptr);
    FloatViewManager::RemoveActiveController(otherController);
    EXPECT_TRUE(FloatViewManager::HasActiveController());
    FloatViewManager::RemoveActiveController(fvController_);
}

/**
 * @tc.name: GetActiveController
 * @tc.desc: Test GetActiveController with and without active controller
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewManagerTest, GetActiveController, TestSize.Level1)
{
    auto controller = FloatViewManager::GetActiveController();
    EXPECT_EQ(nullptr, controller);

    FloatViewManager::SetActiveController(fvController_);
    controller = FloatViewManager::GetActiveController();
    EXPECT_NE(nullptr, controller);
    EXPECT_EQ(fvController_.GetRefPtr(), controller.GetRefPtr());
    FloatViewManager::RemoveActiveController(fvController_);
}

/**
 * @tc.name: DoActionHide
 * @tc.desc: Test DoActionHide with and without active controller
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewManagerTest, DoActionHide, TestSize.Level1)
{
    FloatViewManager::DoActionHide("test_reason");

    FloatViewManager::SetActiveController(fvController_);
    FloatViewManager::DoActionHide("test_reason");
    EXPECT_EQ(FvWindowState::FV_STATE_HIDDEN, fvController_->GetCurState());
    FloatViewManager::RemoveActiveController(fvController_);
}

/**
 * @tc.name: DoActionInSidebar
 * @tc.desc: Test DoActionInSidebar with and without active controller
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewManagerTest, DoActionInSidebar, TestSize.Level1)
{
    FloatViewManager::DoActionInSidebar("test_reason");

    FloatViewManager::SetActiveController(fvController_);
    FloatViewManager::DoActionInSidebar("test_reason");
    EXPECT_EQ(FvWindowState::FV_STATE_IN_SIDEBAR, fvController_->GetCurState());
    FloatViewManager::RemoveActiveController(fvController_);
}

/**
 * @tc.name: DoActionInFloatingBall
 * @tc.desc: Test DoActionInFloatingBall with and without active controller
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewManagerTest, DoActionInFloatingBall, TestSize.Level1)
{
    FloatViewManager::DoActionInFloatingBall("test_reason");

    FloatViewManager::SetActiveController(fvController_);
    FloatViewManager::DoActionInFloatingBall("test_reason");
    EXPECT_EQ(FvWindowState::FV_STATE_IN_FLOATING_BALL, fvController_->GetCurState());
    FloatViewManager::RemoveActiveController(fvController_);
}

/**
 * @tc.name: ControllerLifecycleTest
 * @tc.desc: Test controller lifecycle
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewManagerTest, ControllerLifecycleTest, TestSize.Level1)
{
    EXPECT_FALSE(FloatViewManager::HasActiveController());

    FloatViewManager::SetActiveController(fvController_);
    EXPECT_TRUE(FloatViewManager::HasActiveController());
    EXPECT_TRUE(FloatViewManager::IsActiveController(fvController_));

    FloatViewManager::RemoveActiveController(fvController_);
    EXPECT_FALSE(FloatViewManager::HasActiveController());
}

/**
 * @tc.name: RemoveActiveControllerBranchCoverage
 * @tc.desc: Test RemoveActiveController branch coverage
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewManagerTest, RemoveActiveControllerBranchCoverage, TestSize.Level1)
{
    auto otherController = sptr<FloatViewController>::MakeSptr(*option_, nullptr);
    FloatViewManager::RemoveActiveController(otherController);
    EXPECT_EQ(nullptr, FloatViewManager::activeController_);

    FloatViewManager::RemoveActiveController(fvController_);
    EXPECT_EQ(nullptr, FloatViewManager::activeController_);
}

}
}
}
