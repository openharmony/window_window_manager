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

#include <cstdint>
#include <gtest/gtest.h>
#include "mock_session_manager_service.h"
#include "display_manager.h"
#include "window_agent.h"
#include "window_impl.h"
#include "window_manager_hilog.h"
#include "window_property.h"
#include "window_root.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MockSessionManagerServiceTest"};

    std::string g_errLog;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
        const char *msg)
    {
        g_errLog = msg;
    }
}

class MockSessionManagerServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MockSessionManagerServiceTest::SetUpTestCase()
{
}

void MockSessionManagerServiceTest::TearDownTestCase()
{
}

void MockSessionManagerServiceTest::SetUp()
{
}

void MockSessionManagerServiceTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteDied
 * @tc.desc: OnRemoteDied
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, OnRemoteDied, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    WLOGI("OnRemoteDied");
    MockSessionManagerService::SMSDeathRecipient smsDeathRecipient(100);
    wptr<IRemoteObject> object = nullptr;
    smsDeathRecipient.OnRemoteDied(object);
    EXPECT_TRUE(g_errLog.find("sessionManagerService is null") != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: OnRemoteDied
 * @tc.desc: OnRemoteDied
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, OnRemoteDied1, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    GTEST_LOG_(INFO) << "MockSessionManagerServiceTest: OnRemoteDied1 start";
    WLOGI("OnRemoteDied");
    MockSessionManagerService::SMSDeathRecipient* mService = new MockSessionManagerService::SMSDeathRecipient(100);
    wptr<IRemoteObject> object = nullptr;
    mService->OnRemoteDied(object);
    EXPECT_FALSE(g_errLog.find("SessionManagerService died!") != std::string::npos);
    LOG_SetCallback(nullptr);
    delete mService;
    mService = nullptr;
    GTEST_LOG_(INFO) << "MockSessionManagerServiceTest: OnRemoteDied1 end";
}

/**
 * @tc.name: SetSessionManagerService
 * @tc.desc: set session manager service
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, SetSessionManagerService, TestSize.Level1)
{
    WLOGI("SetSessionManagerService");
    sptr<IRemoteObject> sessionManagerService = nullptr;
    MockSessionManagerService::GetInstance().SetSessionManagerService(sessionManagerService);
    ASSERT_EQ(false, MockSessionManagerService::GetInstance().SetSessionManagerService(sessionManagerService));
}

/**
 * @tc.name: GetSessionManagerService
 * @tc.desc: get session manager service
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, GetSessionManagerService, TestSize.Level1)
{
    WLOGI("GetSessionManagerService");
    sptr<IRemoteObject> sessionManagerService = nullptr;
    MockSessionManagerService::GetInstance().GetSessionManagerService(sessionManagerService);
    ASSERT_EQ(nullptr, sessionManagerService);
}

/**
 * @tc.name: onStart
 * @tc.desc: on start
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, OnStart, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    WLOGI("onStart");
    MockSessionManagerService::GetInstance().OnStart();
    EXPECT_FALSE(g_errLog.find("OnStart begin") != std::string::npos);
    LOG_SetCallback(nullptr);
}

/**
 * @tc.name: onStart
 * @tc.desc: on start
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, OnStart1, TestSize.Level1)
{
    g_errLog.clear();
    LOG_SetCallback(MyLogCallback);
    GTEST_LOG_(INFO) << "MockSessionManagerServiceTest: OnStart1 start";
    MockSessionManagerService* mService = new MockSessionManagerService();
    WLOGI("onStart");
    mService->OnStart();
    EXPECT_FALSE(g_errLog.find("OnStart begin") != std::string::npos);
    LOG_SetCallback(nullptr);
    delete mService;
    mService = nullptr;
    GTEST_LOG_(INFO) << "MockSessionManagerServiceTest: OnStart1 end";
}

/**
 * @tc.name: Dump
 * @tc.desc: Dump
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, Dump, TestSize.Level1)
{
    MockSessionManagerService* mService = new MockSessionManagerService();
    WLOGI("onStart");
    std::vector<std::u16string> args;
    ASSERT_EQ(-1, mService->Dump(-2, args));
}

/**
 * @tc.name: SetSessionManagerService
 * @tc.desc: SetSessionManagerService
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, SetSessionManagerService2, TestSize.Level1)
{
    sptr<IRemoteObject> sessionManagerService = nullptr;
    ASSERT_EQ(false, MockSessionManagerService::GetInstance().SetSessionManagerService(sessionManagerService));
    MockSessionManagerService::GetInstance().NotifyWMSConnected(100, 0, true);
    MockSessionManagerService::GetInstance().NotifyWMSConnected(100, 0, false);
}

/**
 * @tc.name: GetSMSDeathRecipientByUserId
 * @tc.desc: GetSMSDeathRecipientByUserId/RemoveSMSDeathRecipientByUserId
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, GetSMSDeathRecipientByUserId, TestSize.Level1)
{
    auto smsDeathRecipient = MockSessionManagerService::GetInstance().GetSMSDeathRecipientByUserId(100);
    ASSERT_EQ(nullptr, smsDeathRecipient);
    MockSessionManagerService::GetInstance().RemoveSMSDeathRecipientByUserId(100);
}

/**
 * @tc.name: GetSessionManagerServiceInner
 * @tc.desc: GetSessionManagerServiceInner/RemoveSessionManagerServiceByUserId
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, GetSessionManagerServiceInner, TestSize.Level1)
{
    auto sessionManagerService = MockSessionManagerService::GetInstance().GetSessionManagerServiceInner(100);
    ASSERT_EQ(nullptr, sessionManagerService);
    MockSessionManagerService::GetInstance().RemoveSessionManagerServiceByUserId(100);
}

/**
 * @tc.name: GetProcessSurfaceNodeIdByPersistentId
 * @tc.desc: GetProcessSurfaceNodeIdByPersistentId
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, GetProcessSurfaceNodeIdByPersistentId, TestSize.Level1)
{
    int32_t pid = 123;
    std::vector<uint64_t> persistentIds = {1, 2, 3};
    std::vector<uint64_t> surfaceNodeIds;
    MockSessionManagerService::GetInstance().GetProcessSurfaceNodeIdByPersistentId(
        pid, persistentIds, surfaceNodeIds);
    ASSERT_EQ(0, surfaceNodeIds.size());
}
}
}
}