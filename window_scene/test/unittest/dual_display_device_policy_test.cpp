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
#include <utility>

#include "fold_screen_controller/dual_display_device_policy.h"
#include "session/screen/include/screen_session.h"
#include "screen_session_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DualDisplayDevicePolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    sptr<DualDisplayDevicePolicy> dualDisplayDevicePolicy_ = nullptr;
};

void DualDisplayDevicePolicyTest::SetUpTestCase()
{
}

void DualDisplayDevicePolicyTest::TearDownTestCase()
{
}

void DualDisplayDevicePolicyTest::SetUp()
{
    dualDisplayDevicePolicy_ = new (std::nothrow) DualDisplayDevicePolicy();
    EXPECT_NE(nullptr, dualDisplayDevicePolicy_);
}

void DualDisplayDevicePolicyTest::TearDown()
{
    dualDisplayDevicePolicy_ = nullptr;
}

namespace {
/**
 * @tc.name: ChangeScreenDisplayMode
 * @tc.desc: ChangeScreenDisplayMode Test
 * @tc.type: FUNC
 */
HWTEST_F(DualDisplayDevicePolicyTest, ChangeScreenDisplayMode, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, dualDisplayDevicePolicy_);

    sptr<ScreenSessionManager> screenSessionManager_ = new ScreenSessionManager();
    sptr<ScreenSession> screenSession = new ScreenSession();
    pair<ScreenId, sptr<ScreenSession>> pir(0, screenSession);
    screenSessionManager_->screenSessionMap_.insert(pir);

    dualDisplayDevicePolicy_->ChangeScreenDisplayMode(FoldDisplayMode::MAIN);
    dualDisplayDevicePolicy_->ChangeScreenDisplayMode(FoldDisplayMode::FULL);
    dualDisplayDevicePolicy_->ChangeScreenDisplayMode(FoldDisplayMode::UNKNOWN);
    dualDisplayDevicePolicy_->ChangeScreenDisplayMode(FoldDisplayMode::SUB);

    sptr<FoldScreenPolicy> foldScreenPolicy_ = new FoldScreenPolicy();
    foldScreenPolicy_->currentDisplayMode_ = FoldDisplayMode::UNKNOWN;
    ASSERT_EQ(FoldDisplayMode::UNKNOWN, dualDisplayDevicePolicy_->GetScreenDisplayMode());
}

/**
 * @tc.name: GetScreenDisplayMode
 * @tc.desc: GetScreenDisplayMode Test
 * @tc.type: FUNC
 */
HWTEST_F(DualDisplayDevicePolicyTest, GetScreenDisplayMode, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, dualDisplayDevicePolicy_);

    sptr<FoldScreenPolicy> foldScreenPolicy_ = new FoldScreenPolicy();
    foldScreenPolicy_->currentDisplayMode_ = FoldDisplayMode::UNKNOWN;
    ASSERT_EQ(FoldDisplayMode::UNKNOWN, dualDisplayDevicePolicy_->GetScreenDisplayMode());
}

/**
 * @tc.name: SendSensorResult
 * @tc.desc: SendSensorResult Test
 * @tc.type: FUNC
 */
HWTEST_F(DualDisplayDevicePolicyTest, SendSensorResult, Function | SmallTest | Level1)
{
    ASSERT_NE(nullptr, dualDisplayDevicePolicy_);

    dualDisplayDevicePolicy_->SendSensorResult(FoldStatus::EXPAND);
    dualDisplayDevicePolicy_->SendSensorResult(FoldStatus::FOLDED);
    dualDisplayDevicePolicy_->SendSensorResult(FoldStatus::HALF_FOLD);
    dualDisplayDevicePolicy_->SendSensorResult(FoldStatus::HALF_FOLD);


    sptr<FoldScreenPolicy> foldScreenPolicy_ = new FoldScreenPolicy();
    foldScreenPolicy_->currentDisplayMode_ = FoldDisplayMode::UNKNOWN;
    ASSERT_EQ(FoldDisplayMode::UNKNOWN, dualDisplayDevicePolicy_->GetScreenDisplayMode());
}
}
}
}