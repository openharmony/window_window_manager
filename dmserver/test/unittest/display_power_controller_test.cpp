/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "display_power_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_US = 100000;
}
class DisplayPowerControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    std::recursive_mutex mutex_;
    sptr<DisplayPowerController> dpc_ = nullptr;
};

void DisplayPowerControllerTest::SetUpTestCase()
{
}

void DisplayPowerControllerTest::TearDownTestCase()
{
}

void DisplayPowerControllerTest::SetUp()
{
}

void DisplayPowerControllerTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {
/**
 * @tc.name: SuspendBegin
 * @tc.desc: test function : SuspendBegin
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerControllerTest, SuspendBegin, Function | SmallTest | Level1)
{
    dpc_ = new DisplayPowerController(mutex_, [](DisplayId, sptr<DisplayInfo>,
        const std::map<DisplayId, sptr<DisplayInfo>>&, DisplayStateChangeType) {});

    PowerStateChangeReason reason = PowerStateChangeReason{0};
    auto ret = dpc_->SuspendBegin(reason);
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: SetDisplayState01
 * @tc.desc: test function : SetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerControllerTest, SetDisplayState01, Function | SmallTest | Level1)
{
    dpc_ = new DisplayPowerController(mutex_, [](DisplayId, sptr<DisplayInfo>,
        const std::map<DisplayId, sptr<DisplayInfo>>&, DisplayStateChangeType) {});

    DisplayState state = DisplayState::UNKNOWN;
    auto ret = dpc_->SetDisplayState(state);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: SetDisplayState02
 * @tc.desc: test function : SetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerControllerTest, SetDisplayState02, Function | SmallTest | Level1)
{
    dpc_ = new DisplayPowerController(mutex_, [](DisplayId, sptr<DisplayInfo>,
        const std::map<DisplayId, sptr<DisplayInfo>>&, DisplayStateChangeType) {});

    DisplayState state = DisplayState::ON;
    auto ret = dpc_->SetDisplayState(state);
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: SetDisplayState03
 * @tc.desc: test function : SetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerControllerTest, SetDisplayState03, Function | SmallTest | Level1)
{
    dpc_ = new DisplayPowerController(mutex_, [](DisplayId, sptr<DisplayInfo>,
        const std::map<DisplayId, sptr<DisplayInfo>>&, DisplayStateChangeType) {});

    DisplayState state = DisplayState::OFF;
    auto ret = dpc_->SetDisplayState(state);
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: GetDisplayState
 * @tc.desc: test function : GetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerControllerTest, GetDisplayState, Function | SmallTest | Level1)
{
    dpc_ = new DisplayPowerController(mutex_, [](DisplayId, sptr<DisplayInfo>,
        const std::map<DisplayId, sptr<DisplayInfo>>&, DisplayStateChangeType) {});

    DisplayId displayId = 1;
    auto ret = dpc_->GetDisplayState(displayId);
    ASSERT_EQ(DisplayState::UNKNOWN, ret);
}

/**
 * @tc.name: NotifyDisplayEvent01
 * @tc.desc: test function : NotifyDisplayEvent
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerControllerTest, NotifyDisplayEvent01, Function | SmallTest | Level1)
{
    dpc_ = new DisplayPowerController(mutex_, [](DisplayId, sptr<DisplayInfo>,
        const std::map<DisplayId, sptr<DisplayInfo>>&, DisplayStateChangeType) {});

    DisplayEvent event = DisplayEvent::UNLOCK;
    dpc_->NotifyDisplayEvent(event);
    ASSERT_EQ(false, dpc_->isKeyguardDrawn_);
}

/**
 * @tc.name: NotifyDisplayEvent02
 * @tc.desc: test function : NotifyDisplayEvent
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerControllerTest, NotifyDisplayEvent02, Function | SmallTest | Level1)
{
    dpc_ = new DisplayPowerController(mutex_, [](DisplayId, sptr<DisplayInfo>,
        const std::map<DisplayId, sptr<DisplayInfo>>&, DisplayStateChangeType) {});

    DisplayEvent event = DisplayEvent::KEYGUARD_DRAWN;
    dpc_->NotifyDisplayEvent(event);
    ASSERT_EQ(true, dpc_->isKeyguardDrawn_);
}
}
}
}