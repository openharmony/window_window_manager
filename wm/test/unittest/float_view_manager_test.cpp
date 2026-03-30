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

#include "float_view_manager.h"
#include "float_view_option.h"

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
};

void FloatViewManagerTest::SetUpTestCase() {}
void FloatViewManagerTest::TearDownTestCase() {}
void FloatViewManagerTest::SetUp() {}
void FloatViewManagerTest::TearDown() {}

namespace {

static sptr<FloatViewController> CreateTestController()
{
    FvOption opt;
    // Keep option minimal; controller methods we call from FloatViewManager
    // (ChangeState/SyncLimits/SyncWindowInfo/StopFloatView) do not require a real NAPI env.
    napi_env env = nullptr;
    return sptr<FloatViewController>::MakeSptr(opt, env);
}

/**
 * @tc.name: ActiveControllerAndActions
 * @tc.desc: Active controller bookkeeping + action dispatch branches
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewManagerTest, ActiveControllerAndActions, TestSize.Level1)
{
    // No active controller branches.
    EXPECT_FALSE(FloatViewManager::HasActiveController());
    EXPECT_FALSE(FloatViewManager::IsActiveController(nullptr));

    // Unsupported action name.
    FloatViewManager::DoActionEvent("unknown", "reason_unknown");

    // null controller should be ignored.
    FloatViewManager::SetActiveController(nullptr);
    EXPECT_FALSE(FloatViewManager::HasActiveController());

    auto controller = CreateTestController();
    ASSERT_NE(nullptr, controller);
    wptr<FloatViewController> wController(controller);

    // Activate controller and verify.
    FloatViewManager::SetActiveController(wController);
    EXPECT_TRUE(FloatViewManager::HasActiveController());
    EXPECT_TRUE(FloatViewManager::IsActiveController(wController));

    // Cover branch: active exists but RemoveActiveController(nullptr) should NOT clear.
    FloatViewManager::RemoveActiveController(nullptr);
    EXPECT_TRUE(FloatViewManager::HasActiveController());
    EXPECT_TRUE(FloatViewManager::IsActiveController(wController));

    // Drive all action branches.
    FloatViewManager::DoActionEvent("hide", "reason_hide");
    FloatViewManager::DoActionEvent("inSidebar", "reason_sidebar");
    FloatViewManager::DoActionEvent("inFloatingBall", "reason_floating_ball");
    FloatViewManager::DoActionEvent("close", "reason_close");

    // Remove with different controller should not clear.
    auto controller2 = CreateTestController();
    ASSERT_NE(nullptr, controller2);
    wptr<FloatViewController> wController2(controller2);
    FloatViewManager::RemoveActiveController(wController2);
    EXPECT_TRUE(FloatViewManager::HasActiveController());

    // Remove active controller.
    FloatViewManager::RemoveActiveController(wController);
    EXPECT_FALSE(FloatViewManager::HasActiveController());
}

/**
 * @tc.name: SyncInterfaces
 * @tc.desc: SyncFvWindowInfo and SyncFvLimits with/without controller
 * @tc.type: FUNC
 */
HWTEST_F(FloatViewManagerTest, SyncInterfaces, TestSize.Level1)
{
    FloatViewWindowInfo windowInfo;
    FloatViewLimits limits;

    // Without controller
    FloatViewManager::SyncFvWindowInfo(1, windowInfo, "reason_sync");
    FloatViewManager::SyncFvLimits(1, limits);

    // With controller
    auto controller = CreateTestController();
    ASSERT_NE(nullptr, controller);
    wptr<FloatViewController> wController(controller);
    FloatViewManager::SetActiveController(wController);

    FloatViewManager::SyncFvWindowInfo(2, windowInfo, "reason_sync2");
    FloatViewManager::SyncFvLimits(2, limits);

    FloatViewManager::RemoveActiveController(wController);
}

} // namespace
} // namespace Rosen
} // namespace OHOS
