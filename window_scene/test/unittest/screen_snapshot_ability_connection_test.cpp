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

#include <condition_variable>
#include <mutex>
#include <chrono>

#include "screen_snapshot_ability_connection.h"
#include "extension_manager_client.h"
#include "ipc_skeleton.h"
#include "scene_board_judgement.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int32_t DEFAULT_VALUE = -1;
constexpr uint32_t EXTENSION_CONNECT_OUT_TIME = 300; // ms
constexpr uint32_t TRANS_CMD_SEND_SNAPSHOT_RECT = 2;

class ScreenSnapshotAbilityConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::recursive_mutex mutex_;

private:
    std::mutex connectedMutex_;
    std::condition_variable connectedCv_;
};

void ScreenSnapshotAbilityConnectionTest::SetUpTestCase()
{
}

void ScreenSnapshotAbilityConnectionTest::TearDownTestCase()
{
}

void ScreenSnapshotAbilityConnectionTest::SetUp()
{
}

void ScreenSnapshotAbilityConnectionTest::TearDown()
{
}

namespace {

/**
 * @tc.name: OnAbilityConnectDone
 * @tc.desc: OnAbilityConnectDone func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotAbilityConnectionTest, OnAbilityConnectDone, Function | SmallTest | Level1)
{
    sptr<ScreenSnapshotAbilityConnection> abilityConnection(new(std::nothrow) ScreenSnapshotAbilityConnection());
    ASSERT_NE(abilityConnection, nullptr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard",
        "com.ohos.sceneboard.systemdialog");
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnection, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(resConnect, ERR_OK);
    } else {
        ASSERT_EQ(resConnect, ERR_OK);
        {
            std::unique_lock<std::mutex> lock(connectedMutex_);
            static_cast<void>(connectedCv_.wait_for(lock, std::chrono::milliseconds(EXTENSION_CONNECT_OUT_TIME)));
        }
        EXPECT_EQ(abilityConnection->IsAbilityConnected(), true);
        auto resDisconnect = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnection);
        ASSERT_EQ(resDisconnect, NO_ERROR);
        abilityConnection.clear();
        abilityConnection = nullptr;
    }
}

/**
 * @tc.name: OnAbilityDisconnectDone
 * @tc.desc: OnAbilityDisconnectDone func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotAbilityConnectionTest, OnAbilityDisconnectDone, Function | SmallTest | Level1)
{
    sptr<ScreenSnapshotAbilityConnection> abilityConnection(new(std::nothrow) ScreenSnapshotAbilityConnection());
    ASSERT_NE(abilityConnection, nullptr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard",
        "com.ohos.sceneboard.systemdialog");
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnection, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(resConnect, ERR_OK);
    } else {
        ASSERT_EQ(resConnect, ERR_OK);
        auto resDisconnect = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnection);
        ASSERT_EQ(resDisconnect, NO_ERROR);
        EXPECT_EQ(abilityConnection->IsAbilityConnected(), false);
        abilityConnection.clear();
        abilityConnection = nullptr;
    }
}

/**
 * @tc.name: IsAbilityConnected
 * @tc.desc: IsAbilityConnected func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotAbilityConnectionTest, IsAbilityConnected, Function | SmallTest | Level1)
{
    sptr<ScreenSnapshotAbilityConnection> abilityConnection(new(std::nothrow) ScreenSnapshotAbilityConnection());
    ASSERT_NE(abilityConnection, nullptr);
    EXPECT_EQ(abilityConnection->IsAbilityConnected(), false);
    abilityConnection.clear();
    abilityConnection = nullptr;
}

/**
 * @tc.name: SendMessageSync
 * @tc.desc: SendMessageSync func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotAbilityConnectionTest, SendMessageSync, Function | SmallTest | Level1)
{
    sptr<ScreenSnapshotAbilityConnection> abilityConnection(new(std::nothrow) ScreenSnapshotAbilityConnection());
    ASSERT_NE(abilityConnection, nullptr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard",
        "com.ohos.sceneboard.systemdialog");
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnection, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
        ASSERT_NE(resConnect, ERR_OK);
    } else {
        ASSERT_EQ(resConnect, ERR_OK);
        {
            std::unique_lock<std::mutex> lock(connectedMutex_);
            static_cast<void>(connectedCv_.wait_for(lock, std::chrono::milliseconds(EXTENSION_CONNECT_OUT_TIME)));
        }
        EXPECT_EQ(abilityConnection->IsAbilityConnected(), true);
        MessageParcel data;
        MessageParcel reply;
        data.WriteString16(Str8ToStr16("SA"));
        data.WriteString16(Str8ToStr16("ScreenSessionManager"));
        abilityConnection->SendMessageSync(TRANS_CMD_SEND_SNAPSHOT_RECT, data, reply);
        EXPECT_EQ(reply.ReadInt32(), 0);
        EXPECT_EQ(reply.ReadInt32(), 0);
        EXPECT_EQ(reply.ReadInt32(), 0);
        EXPECT_EQ(reply.ReadInt32(), 0);
        EXPECT_EQ(reply.ReadInt32(), 0);
        auto resDisconnect = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnection);
        ASSERT_EQ(resDisconnect, NO_ERROR);
        abilityConnection.clear();
        abilityConnection = nullptr;
    }
}

/**
 * @tc.name: OnRemoteDied
 * @tc.desc: OnRemoteDied func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSnapshotAbilityConnectionTest, OnRemoteDied, Function | SmallTest | Level1)
{
    sptr<ScreenSnapshotAbilityConnection> abilityConnection(new(std::nothrow) ScreenSnapshotAbilityConnection());
    ASSERT_NE(abilityConnection, nullptr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard",
        "com.ohos.sceneboard.systemdialog");
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnection, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    if (!SceneBoardJudgement::IsSceneBoardEnabled()) {
            ASSERT_NE(resConnect, ERR_OK);
    } else {
            ASSERT_EQ(resConnect, ERR_OK);
            auto resDisconnect = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnection);
            ASSERT_EQ(resDisconnect, NO_ERROR);
            EXPECT_EQ(abilityConnection->IsAbilityConnected(), false);
            abilityConnection.clear();
            abilityConnection = nullptr;
    }
}

}
} // namespace Rosen
} // namespace OHOS