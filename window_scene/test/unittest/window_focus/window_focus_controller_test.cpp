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
#include <gtest/gtest.h>
#include "session/host/include/scene_session.h"
#include "session_manager/include/scene_session_manager.h"
#include "window_helper.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class WindowFocusControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static sptr<SceneSessionManager> ssm_;
};
sptr<SceneSessionManager> WindowFocusControllerTest::ssm_ = nullptr;

void WindowFocusControllerTest::SetUpTestCase()
{
    ssm_ = &SceneSessionManager::GetInstance();
}

void WindowFocusControllerTest::TearDownTestCase()
{
    ssm_ = nullptr;
}

void WindowFocusControllerTest::SetUp() {}

void WindowFocusControllerTest::TearDown() {}

namespace {

/**
 * @tc.name: AddFocusGroup
 * @tc.desc: AddFocusGroup
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusControllerTest, AddFocusGroup, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowFocusControllerTest::AddFocusGroup start";
    EXPECT_NE(ssm_, nullptr);
    WSError res = ssm_->windowFocusController_->AddFocusGroup(0, DISPLAY_ID_INVALID);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, res);

    res = ssm_->windowFocusController_->AddFocusGroup(0, 0);
    EXPECT_EQ(WSError::WS_OK, res);

    res = ssm_->windowFocusController_->AddFocusGroup(1, 1);
    EXPECT_EQ(WSError::WS_OK, res);
    GTEST_LOG_(INFO) << "WindowFocusControllerTest::AddFocusGroup end";
}

/**
 * @tc.name: RemoveFocusGroup
 * @tc.desc: RemoveFocusGroup
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusControllerTest, RemoveFocusGroup, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowFocusControllerTest::RemoveFocusGroup start";
    EXPECT_NE(ssm_, nullptr);
    WSError res = ssm_->windowFocusController_->RemoveFocusGroup(0, DISPLAY_ID_INVALID);
    EXPECT_EQ(WSError::WS_ERROR_INVALID_PARAM, res);

    res = ssm_->windowFocusController_->RemoveFocusGroup(0, 0);
    EXPECT_EQ(WSError::WS_OK, res);

    ssm_->windowFocusController_->AddFocusGroup(0, 0);
    res = ssm_->windowFocusController_->RemoveFocusGroup(0, 0);
    EXPECT_EQ(WSError::WS_OK, res);
    GTEST_LOG_(INFO) << "WindowFocusControllerTest::RemoveFocusGroup end";
}

/**
 * @tc.name: GetDisplayGroupId
 * @tc.desc: GetDisplayGroupId
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusControllerTest, GetDisplayGroupId, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "WindowFocusControllerTest::GetDisplayGroupId start";
    EXPECT_NE(ssm_, nullptr);
    DisplayId res = ssm_->windowFocusController_->GetDisplayGroupId(DEFAULT_DISPLAY_ID);
    EXPECT_EQ(DEFAULT_DISPLAY_ID, res);

    ssm_->windowFocusController_->displayId2GroupIdMap_.clear();
    res = ssm_->windowFocusController_->GetDisplayGroupId(1);
    EXPECT_EQ(DEFAULT_DISPLAY_ID, res);

    ssm_->windowFocusController_->deletedDisplayId2GroupIdMap_.insert({ 1, 1 });
    res = ssm_->windowFocusController_->GetDisplayGroupId(1);
    EXPECT_EQ(DISPLAY_ID_INVALID, res);
    ssm_->windowFocusController_->deletedDisplayId2GroupIdMap_.clear();
    
    ssm_->windowFocusController_->displayId2GroupIdMap_.insert({ 1, 1 });
    res = ssm_->windowFocusController_->GetDisplayGroupId(1);
    EXPECT_EQ(1, res);
    GTEST_LOG_(INFO) << "WindowFocusControllerTest::GetDisplayGroupId end";
}

} // namespace
} // namespace Rosen
} // namespace OHOS