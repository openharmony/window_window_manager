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
#include "mock_screen_manager_service.h"
#include "display_manager.h"
#include "window_agent.h"
#include "window_impl.h"
#include "window_property.h"
#include "window_root.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MockSessionManagerServiceTest"};
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
HWTEST_F(MockSessionManagerServiceTest, OnRemoteDied, Function | SmallTest | Level2)
{
    WLOGI("OnRemoteDied");
    MockSessionManagerService::SMSDeathRecipient smsDeathRecipient;
    auto res = WMError::WM_OK;
    wptr<IRemoteObject> object = nullptr;
    smsDeathRecipient.OnRemoteDied(object);
    ASSERT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: OnRemoteDied
 * @tc.desc: OnRemoteDied
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, OnRemoteDied1, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "MockSessionManagerServiceTest: OnRemoteDied1 start";
    WLOGI("OnRemoteDied");
    MockSessionManagerService::SMSDeathRecipient* mService = new MockSessionManagerService::SMSDeathRecipient();
    auto res = WMError::WM_OK;
    wptr<IRemoteObject> object = nullptr;
    mService->OnRemoteDied(object);
    ASSERT_EQ(WMError::WM_OK, res);
    GTEST_LOG_(INFO) << "MockSessionManagerServiceTest: OnRemoteDied1 end";
}

/**
 * @tc.name: SetSessionManagerService
 * @tc.desc: set session manager service
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, SetSessionManagerService, Function | SmallTest | Level2)
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
HWTEST_F(MockSessionManagerServiceTest, GetSessionManagerService, Function | SmallTest | Level2)
{
    WLOGI("GetSessionManagerService");
    sptr<IRemoteObject> sessionManagerService =
    MockSessionManagerService::GetInstance().GetSessionManagerService();
    ASSERT_EQ(nullptr, sessionManagerService);
}

/**
 * @tc.name: onStart
 * @tc.desc: on start
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, OnStart, Function | SmallTest | Level2)
{
    WLOGI("onStart");
    auto ret = WMError::WM_OK;
    MockSessionManagerService::GetInstance().OnStart();
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: onStart
 * @tc.desc: on start
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, OnStart1, Function | SmallTest | Level2)
{
    GTEST_LOG_(INFO) << "MockSessionManagerServiceTest: OnStart1 start";
    MockSessionManagerService* mService = new MockSessionManagerService();
    WLOGI("onStart");
    auto ret = WMError::WM_OK;
    mService->OnStart();
    ASSERT_EQ(ret, WMError::WM_OK);
    GTEST_LOG_(INFO) << "MockSessionManagerServiceTest: OnStart1 end";
}

/**
 * @tc.name: Dump
 * @tc.desc: Dump
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, Dump, Function | SmallTest | Level2)
{
    MockSessionManagerService* mService = new MockSessionManagerService();
    WLOGI("onStart");
    std::vector<std::u16string> args;
    ASSERT_EQ(-1, mService->Dump(-2, args));
}

/**
 * @tc.name: OnStart2
 * @tc.desc: on start
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, OnStart2, Function | SmallTest | Level2)
{
    MockScreenManagerService* mService = new MockScreenManagerService();
    mService->OnStart();
    sptr<IRemoteObject> sessionManagerService = nullptr;
    ASSERT_EQ(false, MockSessionManagerService::GetInstance().SetSessionManagerService(sessionManagerService));
}

/**
 * @tc.name: GetScreenDumpInfo
 * @tc.desc: get screen dump info
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, GetScreenDumpInfo2, Function | SmallTest | Level2)
{
    MockScreenManagerService* mService = new MockScreenManagerService();
    std::vector<std::string> params;
    std::string info;
    auto ret = WMError::WM_OK;
    mService->GetScreenDumpInfo(params, info);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: Dump
 * @tc.desc: Dump
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, Dump2, Function | SmallTest | Level2)
{
    MockScreenManagerService* mService = new MockScreenManagerService();
    std::vector<std::u16string> args;
    auto ret = 0;
    ret = mService->Dump(0, args);
    ASSERT_NE(0, ret);
}

/**
 * @tc.name: SetSessionManagerService
 * @tc.desc: SetSessionManagerService
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, SetSessionManagerService2, Function | SmallTest | Level2)
{
    MockScreenManagerService* mService = new MockScreenManagerService();
    mService->SetSessionManagerService(MockScreenManagerService::GetInstance().screenSessionManager_);
    sptr<IRemoteObject> sessionManagerService = nullptr;
    ASSERT_EQ(false, MockSessionManagerService::GetInstance().SetSessionManagerService(sessionManagerService));
}

/**
 * @tc.name: GetSceneSessionManager
 * @tc.desc: GetSceneSessionManager
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, GetSceneSessionManager, Function | SmallTest | Level2)
{
    MockSessionManagerService* mService = new MockSessionManagerService();
    ASSERT_EQ(MockScreenManagerService::GetInstance().screenSessionManager_, mService->GetSceneSessionManager());
}
}
}
}