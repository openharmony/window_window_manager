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

#include "connection/screen_session_ability_connection.h"
#include "extension_manager_client.h"
#include "ipc_skeleton.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int32_t DEFAULT_VALUE = -1;
constexpr uint32_t EXTENSION_CONNECT_OUT_TIME = 300; // ms
constexpr uint32_t TRANS_CMD_SEND_SNAPSHOT_RECT = 2;
namespace {
constexpr uint32_t SLEEP_TIME_US = 100000;
}

class ScreenSessionAbilityConnectionTest : public testing::Test {
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

void ScreenSessionAbilityConnectionTest::SetUpTestCase()
{
}

void ScreenSessionAbilityConnectionTest::TearDownTestCase()
{
}

void ScreenSessionAbilityConnectionTest::SetUp()
{
}

void ScreenSessionAbilityConnectionTest::TearDown()
{
    usleep(SLEEP_TIME_US);
}

namespace {

/**
 * @tc.name: OnAbilityConnectDone
 * @tc.desc: OnAbilityConnectDone func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, OnAbilityConnectDone, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard",
        "com.ohos.sceneboard.systemdialog");
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnectionStub, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    ASSERT_EQ(resConnect, ERR_OK);
    {
        std::unique_lock<std::mutex> lock(connectedMutex_);
        static_cast<void>(connectedCv_.wait_for(lock, std::chrono::milliseconds(EXTENSION_CONNECT_OUT_TIME)));
    }
    auto resDisconnect = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnectionStub);
    ASSERT_EQ(resDisconnect, NO_ERROR);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: OnAbilityDisconnectDone
 * @tc.desc: OnAbilityDisconnectDone func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, OnAbilityDisconnectDone, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard",
        "com.ohos.sceneboard.systemdialog");
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnectionStub, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    ASSERT_EQ(resConnect, ERR_OK);
    auto resDisconnect = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnectionStub);
    ASSERT_EQ(resDisconnect, NO_ERROR);
    EXPECT_EQ(abilityConnectionStub->IsAbilityConnected(), false);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: IsAbilityConnected
 * @tc.desc: IsAbilityConnected func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, IsAbilityConnected, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    EXPECT_EQ(abilityConnectionStub->IsAbilityConnected(), false);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: SendMessageSync
 * @tc.desc: SendMessageSync func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, SendMessageSync, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard",
        "com.ohos.sceneboard.systemdialog");
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnectionStub, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    ASSERT_EQ(resConnect, ERR_OK);
    {
        std::unique_lock<std::mutex> lock(connectedMutex_);
        static_cast<void>(connectedCv_.wait_for(lock, std::chrono::milliseconds(EXTENSION_CONNECT_OUT_TIME)));
    }
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(Str8ToStr16("SA"));
    data.WriteString16(Str8ToStr16("ScreenSessionManager"));
    abilityConnectionStub->SendMessageSync(TRANS_CMD_SEND_SNAPSHOT_RECT, data, reply);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    auto resDisconnect = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnectionStub);
    ASSERT_EQ(resDisconnect, NO_ERROR);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: OnRemoteDied
 * @tc.desc: OnRemoteDied func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, OnRemoteDied, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard",
        "com.ohos.sceneboard.systemdialog");
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnectionStub, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    ASSERT_EQ(resConnect, ERR_OK);
    auto resDisconnect = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnectionStub);
    ASSERT_EQ(resDisconnect, NO_ERROR);
    EXPECT_EQ(abilityConnectionStub->IsAbilityConnected(), false);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: IsAbilityConnectedSync
 * @tc.desc: IsAbilityConnectedSync func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, IsAbilityConnectedSync, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    EXPECT_EQ(abilityConnectionStub->IsAbilityConnectedSync(), false);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: ScreenSessionConnectExtension
 * @tc.desc: ScreenSessionConnectExtension func
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, ScreenSessionConnectExtension, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    AAFwk::Want want;
    const std::string bundleName = "com.ohos.sceneboard";
    const std::string abilityName = "com.ohos.sceneboard.systemdialog";
    want.SetElementName(bundleName, abilityName);
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnectionStub, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    ASSERT_EQ(resConnect, ERR_OK);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: ScreenSessionDisconnectExtension
 * @tc.desc: ScreenSessionDisconnectExtension func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, ScreenSessionDisconnectExtension, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard",
        "com.ohos.sceneboard.systemdialog");
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnectionStub, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    ASSERT_EQ(resConnect, ERR_OK);
    {
        std::unique_lock<std::mutex> lock(connectedMutex_);
        static_cast<void>(connectedCv_.wait_for(lock, std::chrono::milliseconds(EXTENSION_CONNECT_OUT_TIME)));
    }
    auto resDisconnect = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnectionStub);
    ASSERT_EQ(resDisconnect, NO_ERROR);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: SendMessage
 * @tc.desc: SendMessage func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, SendMessage, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard",
        "com.ohos.sceneboard.systemdialog");
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnectionStub, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    ASSERT_EQ(resConnect, ERR_OK);
    {
        std::unique_lock<std::mutex> lock(connectedMutex_);
        static_cast<void>(connectedCv_.wait_for(lock, std::chrono::milliseconds(EXTENSION_CONNECT_OUT_TIME)));
    }
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(Str8ToStr16("SA"));
    data.WriteString16(Str8ToStr16("ScreenSessionManager"));
    abilityConnectionStub->SendMessageSync(TRANS_CMD_SEND_SNAPSHOT_RECT, data, reply);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    auto resDisconnect = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnectionStub);
    ASSERT_EQ(resDisconnect, NO_ERROR);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: SendMessageBlock
 * @tc.desc: SendMessageBlock func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, SendMessageBlock, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard",
        "com.ohos.sceneboard.systemdialog");
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnectionStub, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    ASSERT_EQ(resConnect, ERR_OK);
    {
        std::unique_lock<std::mutex> lock(connectedMutex_);
        static_cast<void>(connectedCv_.wait_for(lock, std::chrono::milliseconds(EXTENSION_CONNECT_OUT_TIME)));
    }
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(Str8ToStr16("SA"));
    data.WriteString16(Str8ToStr16("ScreenSessionManager"));
    abilityConnectionStub->SendMessageSyncBlock(TRANS_CMD_SEND_SNAPSHOT_RECT, data, reply);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    auto resDisconnect = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnectionStub);
    ASSERT_EQ(resDisconnect, NO_ERROR);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: OnRemoteRequest
 * @tc.desc: OnRemoteRequest func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, OnRemoteRequest, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    AAFwk::Want want;
    want.SetElementName("com.ohos.sceneboard",
        "com.ohos.sceneboard.systemdialog");
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    auto resConnect = AAFwk::ExtensionManagerClient::GetInstance().ConnectServiceExtensionAbility(
        want, abilityConnectionStub, nullptr, DEFAULT_VALUE);
    IPCSkeleton::SetCallingIdentity(identity);
    ASSERT_EQ(resConnect, ERR_OK);
    {
        std::unique_lock<std::mutex> lock(connectedMutex_);
        static_cast<void>(connectedCv_.wait_for(lock, std::chrono::milliseconds(EXTENSION_CONNECT_OUT_TIME)));
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteString16(Str8ToStr16("SA"));
    data.WriteString16(Str8ToStr16("ScreenSessionManager"));
    abilityConnectionStub->OnRemoteRequest(TRANS_CMD_SEND_SNAPSHOT_RECT, data, reply, option);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    EXPECT_EQ(reply.ReadInt32(), 0);
    auto resDisconnect = AAFwk::ExtensionManagerClient::GetInstance().DisconnectAbility(abilityConnectionStub);
    ASSERT_EQ(resDisconnect, NO_ERROR);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: GetScreenId
 * @tc.desc: GetScreenId func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, GetScreenId, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    int32_t expection = 0;
    EXPECT_EQ(abilityConnectionStub->GetScreenId(), expection);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: GetLeft
 * @tc.desc: GetLeft func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, GetLeft, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    int32_t expection = 0;
    EXPECT_EQ(abilityConnectionStub->GetLeft(), expection);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: GetTop
 * @tc.desc: GetTop func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, GetTop, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    int32_t expection = 0;
    EXPECT_EQ(abilityConnectionStub->GetTop(), expection);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: GetWidth
 * @tc.desc: GetWidth func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, GetWidth, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    int32_t expection = 0;
    EXPECT_EQ(abilityConnectionStub->GetWidth(), expection);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: GetHeight
 * @tc.desc: GetHeight func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, GetHeight, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    int32_t expection = 0;
    EXPECT_EQ(abilityConnectionStub->GetHeight(), expection);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: GetErrCode
 * @tc.desc: GetErrCode func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, GetErrCode, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    uint32_t expection = 0;
    EXPECT_EQ(abilityConnectionStub->GetErrCode(), expection);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}

/**
 * @tc.name: EraseErrCode
 * @tc.desc: EraseErrCode func test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionAbilityConnectionTest, EraseErrCode, Function | SmallTest | Level1)
{
    sptr<ScreenSessionAbilityConnectionStub> abilityConnectionStub(
        new (std::nothrow) ScreenSessionAbilityConnectionStub());
    ASSERT_NE(abilityConnectionStub, nullptr);
    abilityConnectionStub->errCode_ = 1;
    uint32_t expection = 0;
    abilityConnectionStub->EraseErrCode();
    EXPECT_EQ(abilityConnectionStub->errCode_, expection);
    abilityConnectionStub.clear();
    abilityConnectionStub = nullptr;
}
}
} // namespace Rosen
} // namespace OHOS