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
#include <vsync_station.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class VsyncStationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void VsyncStationTest::SetUpTestCase() {}

void VsyncStationTest::TearDownTestCase() {}

void VsyncStationTest::SetUp() {}

void VsyncStationTest::TearDown() {}

namespace {
/**
 * @tc.name: RequestVsyncOneWindow
 * @tc.desc: RequestVsyncOneWindow Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, RequestVsyncOneWindow, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::shared_ptr<VsyncStation> vsyncStation = std::make_shared<VsyncStation>(nodeId);
    std::shared_ptr<VsyncCallback> vsyncCallback = std::make_shared<VsyncCallback>();
    ASSERT_NE(vsyncStation, nullptr);
    vsyncStation->RequestVsync(vsyncCallback);
}

/**
 * @tc.name: RequestVsyncMultiWindow
 * @tc.desc: RequestVsyncMultiWindow Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, RequestVsyncMultiWindow, TestSize.Level1)
{
    NodeId nodeId0 = 0;
    std::shared_ptr<VsyncStation> vsyncStation0 = std::make_shared<VsyncStation>(nodeId0);
    std::shared_ptr<VsyncCallback> vsyncCallback0 = std::make_shared<VsyncCallback>();
    ASSERT_NE(vsyncStation0, nullptr);
    vsyncStation0->RequestVsync(vsyncCallback0);
    NodeId nodeId1 = 1;
    std::shared_ptr<VsyncStation> vsyncStation1 = std::make_shared<VsyncStation>(nodeId1);
    std::shared_ptr<VsyncCallback> vsyncCallback1 = std::make_shared<VsyncCallback>();
    ASSERT_NE(vsyncStation1, nullptr);
    vsyncStation1->RequestVsync(vsyncCallback1);
}

/**
 * @tc.name: GetFrameRateLinkerId
 * @tc.desc: GetFrameRateLinkerId Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, GetFrameRateLinkerId, TestSize.Level1)
{
    NodeId nodeId0 = 0;
    std::shared_ptr<VsyncStation> vsyncStation0 = std::make_shared<VsyncStation>(nodeId0);
    ASSERT_NE(vsyncStation0, nullptr);
    ASSERT_NE(-1, vsyncStation0->GetFrameRateLinkerId());
    NodeId nodeId1 = 1;
    std::shared_ptr<VsyncStation> vsyncStation1 = std::make_shared<VsyncStation>(nodeId1);
    ASSERT_NE(vsyncStation1, nullptr);
    ASSERT_NE(-1, vsyncStation1->GetFrameRateLinkerId());
}

/**
 * @tc.name: FlushFrameRate
 * @tc.desc: FlushFrameRate Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, FlushFrameRate, TestSize.Level1)
{
    NodeId nodeId0 = 0;
    std::shared_ptr<VsyncStation> vsyncStation0 = std::make_shared<VsyncStation>(nodeId0);
    ASSERT_NE(vsyncStation0, nullptr);
    uint32_t rate0 = 60;
    int32_t animatorExpectedFrameRate = -1;
    vsyncStation0->FlushFrameRate(nullptr, rate0, animatorExpectedFrameRate);
    NodeId nodeId1 = 1;
    std::shared_ptr<VsyncStation> vsyncStation1 = std::make_shared<VsyncStation>(nodeId1);
    ASSERT_NE(vsyncStation1, nullptr);
    uint32_t rate1 = 120;
    vsyncStation1->FlushFrameRate(nullptr, rate1, animatorExpectedFrameRate);
}

/**
 * @tc.name: SetFrameRateLinkerEnable
 * @tc.desc: SetFrameRateLinkerEnable Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, SetFrameRateLinkerEnable, TestSize.Level1)
{
    NodeId nodeId0 = 0;
    std::shared_ptr<VsyncStation> vsyncStation0 = std::make_shared<VsyncStation>(nodeId0);
    ASSERT_NE(vsyncStation0, nullptr);
    bool enable0 = false;
    vsyncStation0->SetFrameRateLinkerEnable(nullptr, enable0);
    NodeId nodeId1 = 1;
    std::shared_ptr<VsyncStation> vsyncStation1 = std::make_shared<VsyncStation>(nodeId1);
    ASSERT_NE(vsyncStation1, nullptr);
    bool enable1 = true;
    vsyncStation1->SetFrameRateLinkerEnable(nullptr, enable1);
}

/**
 * @tc.name: SetUiDvsyncSwitch
 * @tc.desc: SetUiDvsyncSwitch Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, SetUiDvsyncSwitch, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::shared_ptr<VsyncStation> vsyncStation = std::make_shared<VsyncStation>(nodeId);
    ASSERT_NE(vsyncStation, nullptr);
    vsyncStation->SetUiDvsyncSwitch(true);
    vsyncStation->SetUiDvsyncSwitch(false);
}

/**
 * @tc.name: DecreaseRequestVsyncTimes
 * @tc.desc: DecreaseRequestVsyncTimes Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, DecreaseRequestVsyncTimes, TestSize.Level1)
{
    NodeId nodeId = 0;
    std::shared_ptr<VsyncStation> vsyncStation = std::make_shared<VsyncStation>(nodeId);
    std::atomic<int32_t> requestVsyncTimes_{ 0 };
    int32_t current = requestVsyncTimes_.load();
    vsyncStation->DecreaseRequestVsyncTimes();
    int32_t desired = requestVsyncTimes_.load();
    if (current != 0) {
        EXPECT_EQ(current - 1, desired);
    }
}
} // namespace
} // namespace Rosen
} // namespace OHOS