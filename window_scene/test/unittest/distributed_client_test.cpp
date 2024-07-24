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

#include "ability_manager_errors.h"
#include "distributed_client.h"
#include "distributed_parcel_helper.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "window_manager_hilog.h"
#include "mock/mock_message_parcel.h"

using namespace testing;
using namespace testing::ext;

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
const std::string UNDEFINED = "undefined";
}

class DistributedClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    std::shared_ptr<DistributedClient> distributedClient_;
};

void DistributedClientTest::SetUpTestCase()
{
}

void DistributedClientTest::TearDownTestCase()
{
}

void DistributedClientTest::SetUp()
{
    distributedClient_ = std::make_shared<DistributedClient>();
    EXPECT_NE(nullptr, distributedClient_);
}

void DistributedClientTest::TearDown()
{
    distributedClient_ = nullptr;
}

namespace {
/**
 * @tc.name: GetMissionInfos
 * @tc.desc: GetMissionInfos test
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(DistributedClientTest, GetMissionInfos, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "DistributedClientTest GetMissionInfos start.";
    const std::u16string DMS_PROXY_INTERFACE_TOKEN = u"ohos.distributedschedule.accessToken";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::vector<AAFwk::MissionInfo> missionInfos;
    if (!data.WriteInterfaceToken(DMS_PROXY_INTERFACE_TOKEN)) {
        EXPECT_EQ(distributedClient_->GetMissionInfos("", 0, missionInfos), ERR_FLATTEN_OBJECT);
    }
    EXPECT_EQ(distributedClient_->GetMissionInfos("", 0, missionInfos), AAFwk::INVALID_PARAMETERS_ERR);
    GTEST_LOG_(INFO) << "DistributedClientTest GetMissionInfos end.";
}

/**
 * @tc.name: GetMissionInfos02
 * @tc.desc: GetMissionInfos test
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(DistributedClientTest, GetMissionInfos02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "DistributedClientTest GetMissionInfos02 start.";
    std::vector<AAFwk::MissionInfo> missionInfos;
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    EXPECT_NE(distributedClient_->GetMissionInfos("", 0, missionInfos), ERR_NONE);
    MockMessageParcel::ClearAllErrorFlag();
    GTEST_LOG_(INFO) << "DistributedClientTest GetMissionInfos02 end.";
}

/**
 * @tc.name: GetMissionInfos03
 * @tc.desc: GetMissionInfos test
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(DistributedClientTest, GetMissionInfos03, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "DistributedClientTest GetMissionInfos03 start.";
    std::vector<AAFwk::MissionInfo> missionInfos;
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    EXPECT_NE(distributedClient_->GetMissionInfos("", 0, missionInfos), ERR_NONE);
    MockMessageParcel::ClearAllErrorFlag();
    GTEST_LOG_(INFO) << "DistributedClientTest GetMissionInfos03 end.";
}
}

/**
 * @tc.name: GetRemoteMissionSnapshotInfo
 * @tc.desc: GetRemoteMissionSnapshotInfo test
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(DistributedClientTest, GetRemoteMissionSnapshotInfo, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "DistributedClientTest GetRemoteMissionSnapshotInfo start.";
    std::unique_ptr<AAFwk::MissionSnapshot> missionSnapshot;
    EXPECT_EQ(distributedClient_->GetRemoteMissionSnapshotInfo("", 0, missionSnapshot), ERR_NULL_OBJECT);
    GTEST_LOG_(INFO) << "DistributedClientTest GetRemoteMissionSnapshotInfo end.";
}

/**
 * @tc.name: GetRemoteMissionSnapshotInfo02
 * @tc.desc: GetRemoteMissionSnapshotInfo test
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(DistributedClientTest, GetRemoteMissionSnapshotInfo02, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "DistributedClientTest GetRemoteMissionSnapshotInfo02 start.";
    std::string deviceID = "123456789";
    std::unique_ptr<AAFwk::MissionSnapshot> missionSnapshot;
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    EXPECT_NE(distributedClient_->GetRemoteMissionSnapshotInfo(deviceID, 0, missionSnapshot), ERR_NONE);
    MockMessageParcel::ClearAllErrorFlag();
    GTEST_LOG_(INFO) << "DistributedClientTest GetRemoteMissionSnapshotInfo02 end.";
}

/**
 * @tc.name: GetRemoteMissionSnapshotInfo03
 * @tc.desc: GetRemoteMissionSnapshotInfo test
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(DistributedClientTest, GetRemoteMissionSnapshotInfo03, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "DistributedClientTest GetRemoteMissionSnapshotInfo03 start.";
    std::string deviceID = "123456789";
    std::unique_ptr<AAFwk::MissionSnapshot> missionSnapshot;
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    EXPECT_NE(distributedClient_->GetRemoteMissionSnapshotInfo(deviceID, 0, missionSnapshot), ERR_NONE);
    MockMessageParcel::ClearAllErrorFlag();
    GTEST_LOG_(INFO) << "DistributedClientTest GetRemoteMissionSnapshotInfo03 end.";
}

/**
 * @tc.name: ReadMissionInfosFromParcel
 * @tc.desc: ReadMissionInfosFromParcel test
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(DistributedClientTest, ReadMissionInfosFromParcel, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "DistributedClientTest ReadMissionInfosFromParcel start.";
    std::vector<AAFwk::MissionInfo> missionInfos;
    MessageParcel reply;
    EXPECT_EQ(distributedClient_->ReadMissionInfosFromParcel(reply, missionInfos), true);
    GTEST_LOG_(INFO) << "DistributedClientTest ReadMissionInfosFromParcel end.";
}

/**
 * @tc.name: SetMissionContinueState
 * @tc.desc: SetMissionContinueState test
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(DistributedClientTest, SetMissionContinueState, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "DistributedClientTest SetMissionContinueState start.";
    AAFwk::ContinueState state = AAFwk::ContinueState::CONTINUESTATE_ACTIVE;
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    EXPECT_NE(distributedClient_->SetMissionContinueState(0, state), ERR_NULL_OBJECT);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    EXPECT_NE(distributedClient_->SetMissionContinueState(0, state), ERR_NULL_OBJECT);
    MockMessageParcel::ClearAllErrorFlag();
    GTEST_LOG_(INFO) << "DistributedClientTest SetMissionContinueState end.";
}
}
}
