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

#include <vsync_station.h>
#include <gtest/gtest.h>

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

void VsyncStationTest::SetUpTestCase()
{
}

void VsyncStationTest::TearDownTestCase()
{
}

void VsyncStationTest::SetUp()
{
}

void VsyncStationTest::TearDown()
{
}

namespace {
/**
 * @tc.name: RequestVsyncOneWindow
 * @tc.desc: RequestVsyncOneWindow Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, RequestVsyncOneWindow, Function | SmallTest | Level3)
{
    NodeId nodeId = 0;
    std::shared_ptr<VsyncStation> vsyncStation = std::make_shared<VsyncStation>(nodeId);
    std::shared_ptr<VsyncCallback> vsyncCallback = std::make_shared<VsyncCallback>();
    vsyncStation->RequestVsync(vsyncCallback);
    ASSERT_NE(vsyncStation, nullptr);
}

/**
 * @tc.name: RequestVsyncMultiWindow
 * @tc.desc: RequestVsyncMultiWindow Test
 * @tc.type: FUNC
 */
HWTEST_F(VsyncStationTest, RequestVsyncMultiWindow, Function | SmallTest | Level3)
{
    NodeId nodeId0 = 0;
    std::shared_ptr<VsyncStation> vsyncStation0 = std::make_shared<VsyncStation>(nodeId0);
    std::shared_ptr<VsyncCallback> vsyncCallback0 = std::make_shared<VsyncCallback>();
    vsyncStation0->RequestVsync(vsyncCallback0);
    ASSERT_NE(vsyncStation0, nullptr);
    NodeId nodeId1 = 1;
    std::shared_ptr<VsyncStation> vsyncStation1 = std::make_shared<VsyncStation>(nodeId1);
    std::shared_ptr<VsyncCallback> vsyncCallback1 = std::make_shared<VsyncCallback>();
    vsyncStation1->RequestVsync(vsyncCallback1);
    ASSERT_NE(vsyncStation1, nullptr);
}

}
}
}