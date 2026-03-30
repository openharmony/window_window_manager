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

#include "float_window_manager.h"
#include "float_view_controller.h"
#include "float_view_option.h"
#include "float_view_manager.h"
#include "floating_ball_controller.h"
#include "floating_ball_manager.h"
#include "floating_ball_option.h"
#include "picture_in_picture_manager.h"
#include "picture_in_picture_controller_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class FloatWindowManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FloatWindowManagerTest::SetUpTestCase() {}
void FloatWindowManagerTest::TearDownTestCase() {}

void FloatWindowManagerTest::SetUp()
{
    // Ensure clean static states for each test.
    FloatViewManager::RemoveActiveController(nullptr);
    FloatingBallManager::RemoveActiveController(nullptr);

    // Clear pip running list.
    PictureInPictureManager::RemoveRunningController(nullptr);
}

void FloatWindowManagerTest::TearDown()
{
    FloatViewManager::RemoveActiveController(nullptr);
    FloatingBallManager::RemoveActiveController(nullptr);
    PictureInPictureManager::RemoveRunningController(nullptr);
}

namespace {

class MockWindow : public Window {
public:
    MockWindow() {}
    ~MockWindow() {}
};

static sptr<FloatViewController> CreateTestFvController()
{
    FvOption opt;
    napi_env env = nullptr;
    return sptr<FloatViewController>::MakeSptr(opt, env);
}

static sptr<FloatingBallController> CreateTestFbController(uint32_t id)
{
    auto mainWindow = sptr<MockWindow>::MakeSptr();
    return sptr<FloatingBallController>::MakeSptr(mainWindow, id, nullptr);
}

/**
 * @tc.name: RelationGetters
 * @tc.desc: GetBoundFloatingBall/GetBoundFloatView null and not-found branches
 * @tc.type: FUNC
 */
HWTEST_F(FloatWindowManagerTest, RelationGetters, TestSize.Level1)
{
    EXPECT_EQ(nullptr, FloatWindowManager::GetBoundFloatingBall(nullptr));
    EXPECT_EQ(nullptr, FloatWindowManager::GetBoundFloatView(nullptr));

    auto fv = CreateTestFvController();
    auto fb = CreateTestFbController(100);
    ASSERT_NE(nullptr, fv);
    ASSERT_NE(nullptr, fb);

    // No relation yet.
    EXPECT_EQ(nullptr, FloatWindowManager::GetBoundFloatingBall(fv));
    EXPECT_EQ(nullptr, FloatWindowManager::GetBoundFloatView(fb));

    // Bind will create relation.
    FbOption fbOpt;
    auto ret = FloatWindowManager::Bind(fv, fb, fbOpt);
    EXPECT_EQ(WMError::WM_OK, ret);

    EXPECT_NE(nullptr, FloatWindowManager::GetBoundFloatingBall(fv));
    EXPECT_NE(nullptr, FloatWindowManager::GetBoundFloatView(fb));

    // UnBind removes relation.
    ret = FloatWindowManager::UnBind(fv, fb);
    EXPECT_EQ(WMError::WM_OK, ret);

    EXPECT_EQ(nullptr, FloatWindowManager::GetBoundFloatingBall(fv));
    EXPECT_EQ(nullptr, FloatWindowManager::GetBoundFloatView(fb));
}

/**
 * @tc.name: ConflictChecks
 * @tc.desc: IsFloatViewConflict/IsFloatingBallConflict/IsPipConflict branches
 * @tc.type: FUNC
 */
HWTEST_F(FloatWindowManagerTest, ConflictChecks, TestSize.Level1)
{
    auto fv = CreateTestFvController();
    auto fb = CreateTestFbController(101);
    ASSERT_NE(nullptr, fv);
    ASSERT_NE(nullptr, fb);

    wptr<FloatViewController> wFv(fv);
    wptr<FloatingBallController> wFb(fb);

    // No pip running, no active controller on other side => false.
    EXPECT_FALSE(FloatWindowManager::IsFloatViewConflict(wFv));
    EXPECT_FALSE(FloatWindowManager::IsFloatingBallConflict(wFb));
    EXPECT_FALSE(FloatWindowManager::IsPipConflict());

    // Make pip running => always true for both conflict checks.
    auto pip = sptr<PictureInPictureControllerBase>::MakeSptr();
    PictureInPictureManager::AddRunningController(pip);
    EXPECT_TRUE(FloatWindowManager::IsFloatViewConflict(wFv));
    EXPECT_TRUE(FloatWindowManager::IsFloatingBallConflict(wFb));
    PictureInPictureManager::RemoveRunningController(pip);

    // Activate floating ball => float view conflict depends on binding relationship.
    FloatingBallManager::SetActiveController(wFb);
    EXPECT_TRUE(FloatingBallManager::HasActiveController());

    // Not bound => true.
    EXPECT_TRUE(FloatWindowManager::IsFloatViewConflict(wFv));

    // Bound to same active FB => false.
    FbOption fbOpt;
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::Bind(fv, fb, fbOpt));
    EXPECT_FALSE(FloatWindowManager::IsFloatViewConflict(wFv));

    // Pip conflict: active FB => true.
    EXPECT_TRUE(FloatWindowManager::IsPipConflict());

    // Activate FV => floating ball conflict depends on binding relationship.
    FloatViewManager::SetActiveController(wFv);
    EXPECT_TRUE(FloatViewManager::HasActiveController());

    // Bound to same active FV => false.
    EXPECT_FALSE(FloatWindowManager::IsFloatingBallConflict(wFb));

    // Clear actives.
    FloatViewManager::RemoveActiveController(wFv);
    FloatingBallManager::RemoveActiveController(wFb);
    EXPECT_FALSE(FloatWindowManager::IsPipConflict());

    // Cleanup relation.
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::UnBind(fv, fb));
}

/**
 * @tc.name: BindUnBindInvalidBranches
 * @tc.desc: Cover Bind/UnBind invalid-state and already-bound branches
 * @tc.type: FUNC
 */
HWTEST_F(FloatWindowManagerTest, BindUnBindInvalidBranches, TestSize.Level1)
{
    auto fv = CreateTestFvController();
    auto fb = CreateTestFbController(102);
    ASSERT_NE(nullptr, fv);
    ASSERT_NE(nullptr, fb);

    FbOption fbOpt;

    // Force invalid state for FB: should fail Bind.
    fb->SetBindState(false);
    fb->SetBindWindowId(INVALID_WINDOW_ID);
    // Change to a state that is NOT UNDEFINED/STOPPED.
    // (STATE_STARTED exists in wm_common.h, used by controller.)
    // If enum differs on some products, this line may be adjusted.
    fb->StartFloatingBallSingle(sptr<FbOption>::MakeSptr(fbOpt), false);
    EXPECT_NE(WMError::WM_OK, FloatWindowManager::Bind(fv, fb, fbOpt));

    // Reset FB to a valid state by creating a fresh controller.
    fb = CreateTestFbController(103);
    ASSERT_NE(nullptr, fb);

    // Force invalid state for FV: ChangeState to non-allowed and expect failure.
    fv->ChangeState(FvWindowState::FV_STATE_STARTED);
    EXPECT_NE(WMError::WM_OK, FloatWindowManager::Bind(fv, fb, fbOpt));

    // Fresh FV to proceed.
    fv = CreateTestFvController();
    ASSERT_NE(nullptr, fv);

    // First bind OK.
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::Bind(fv, fb, fbOpt));

    // Already bound branches: fbController->IsBind() / fvController->IsBind().
    EXPECT_NE(WMError::WM_OK, FloatWindowManager::Bind(fv, fb, fbOpt));

    // UnBind should succeed.
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::UnBind(fv, fb));

    // UnBind again: not bound branch.
    EXPECT_NE(WMError::WM_OK, FloatWindowManager::UnBind(fv, fb));
}

/**
 * @tc.name: RemoveRelationBranches
 * @tc.desc: Cover RemoveRelation erase and not-found branches
 * @tc.type: FUNC
 */
HWTEST_F(FloatWindowManagerTest, RemoveRelationBranches, TestSize.Level1)
{
    auto fv = CreateTestFvController();
    auto fb = CreateTestFbController(104);
    ASSERT_NE(nullptr, fv);
    ASSERT_NE(nullptr, fb);

    // RemoveRelation when maps have no key (both not-found branches).
    FloatWindowManager::UnBind(fv, fb); // should be safe even if not bound

    // Bind to insert maps then UnBind to trigger erase branches.
    FbOption fbOpt;
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::Bind(fv, fb, fbOpt));
    EXPECT_EQ(WMError::WM_OK, FloatWindowManager::UnBind(fv, fb));
}

/**
 * @tc.name: ConflictSelfNullBranches
 * @tc.desc: Cover selfController promote nullptr branches in conflict checks
 * @tc.type: FUNC
 */
HWTEST_F(FloatWindowManagerTest, ConflictSelfNullBranches, TestSize.Level1)
{
    // Prepare one active controller on each side.
    auto fv = CreateTestFvController();
    auto fb = CreateTestFbController(105);
    ASSERT_NE(nullptr, fv);
    ASSERT_NE(nullptr, fb);

    wptr<FloatViewController> wFv(fv);
    wptr<FloatingBallController> wFb(fb);

    FloatingBallManager::SetActiveController(wFb);
    FloatViewManager::SetActiveController(wFv);

    // selfController is nullptr => promote nullptr, should fall through to true.
    EXPECT_TRUE(FloatWindowManager::IsFloatViewConflict(nullptr));
    EXPECT_TRUE(FloatWindowManager::IsFloatingBallConflict(nullptr));
}

/**
 * @tc.name: TokenAcquireReleaseBranches
 * @tc.desc: Cover AcquireToken/ReleaseToken branches (token==0, wrong owner, success path)
 * @tc.type: FUNC
 */
HWTEST_F(FloatWindowManagerTest, TokenAcquireReleaseBranches, TestSize.Level1)
{
    // Acquire should succeed when no owner.
    uint64_t token = FloatWindowManager::AcquireToken();
    EXPECT_NE(0u, token);

    // Release with wrong token should keep owner (subsequent Acquire should timeout and return 0).
    FloatWindowManager::ReleaseToken(token + 1);
    uint64_t token2 = FloatWindowManager::AcquireToken();
    EXPECT_EQ(0u, token2);

    // Release with 0 should do nothing.
    FloatWindowManager::ReleaseToken(0);

    // Release with correct token should free owner.
    FloatWindowManager::ReleaseToken(token);
    uint64_t token3 = FloatWindowManager::AcquireToken();
    EXPECT_NE(0u, token3);
    FloatWindowManager::ReleaseToken(token3);
}

/**
 * @tc.name: StartStopBindEarlyReturnBranches
 * @tc.desc: Cover Start/Stop bind functions early returns: null controller and missing relation
 * @tc.type: FUNC
 */
HWTEST_F(FloatWindowManagerTest, StartStopBindEarlyReturnBranches, TestSize.Level1)
{
    // null controller branches
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StartBindFloatView(nullptr));
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StartBindFloatingBall(nullptr, nullptr));
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StopBindFloatView(nullptr));
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StopBindFloatingBall(nullptr));

    // missing relation branches
    auto fv = CreateTestFvController();
    auto fb = CreateTestFbController(106);
    ASSERT_NE(nullptr, fv);
    ASSERT_NE(nullptr, fb);

    wptr<FloatViewController> wFv(fv);
    wptr<FloatingBallController> wFb(fb);

    // No relation inserted => should return WM_DO_NOTHING
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StartBindFloatView(wFv));
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StartBindFloatingBall(wFb, nullptr));
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StopBindFloatView(wFv));
    EXPECT_EQ(WMError::WM_DO_NOTHING, FloatWindowManager::StopBindFloatingBall(wFb));
}

} // namespace Rosen
} // namespace OHOS
