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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MockScreenManagerServiceTest"};
}

class MockScreenManagerServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MockScreenManagerServiceTest::SetUpTestCase()
{
}

void MockScreenManagerServiceTest::TearDownTestCase()
{
}

void MockScreenManagerServiceTest::SetUp()
{
}

void MockScreenManagerServiceTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnStart1
 * @tc.desc: on start
 * @tc.type: FUNC
 */
HWTEST_F(MockScreenManagerServiceTest, OnStart1, Function | SmallTest | Level2)
{
    WLOGI("OnStart01");
    auto res = WMError::WM_OK;
    MockScreenManagerService::GetInstance().OnStart();
    ASSERT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: OnStart2
 * @tc.desc: on start
 * @tc.type: FUNC
 */
HWTEST_F(MockScreenManagerServiceTest, OnStart2, Function | SmallTest | Level2)
{
    MockScreenManagerService* mService = new MockScreenManagerService();
    auto res = WMError::WM_OK;
    mService->OnStart();
    ASSERT_EQ(WMError::WM_OK, res);
}

/**
 * @tc.name: GetScreenDumpInfo
 * @tc.desc: get screen dump info
 * @tc.type: FUNC
 */
HWTEST_F(MockScreenManagerServiceTest, GetScreenDumpInfo, Function | SmallTest | Level2)
{
    std::vector<std::string> params;
    std::string info;
    auto ret = WMError::WM_OK;
    MockScreenManagerService::GetInstance().GetScreenDumpInfo(params, info);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: GetScreenDumpInfo
 * @tc.desc: get screen dump info
 * @tc.type: FUNC
 */
HWTEST_F(MockScreenManagerServiceTest, GetScreenDumpInfo2, Function | SmallTest | Level2)
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
HWTEST_F(MockScreenManagerServiceTest, Dump, Function | SmallTest | Level2)
{
    std::vector<std::u16string> args;
    auto ret = 0;
    ret = MockScreenManagerService::GetInstance().Dump(0, args);
    ASSERT_NE(0, ret);
}

/**
 * @tc.name: Dump
 * @tc.desc: Dump
 * @tc.type: FUNC
 */
HWTEST_F(MockScreenManagerServiceTest, Dump2, Function | SmallTest | Level2)
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
HWTEST_F(MockScreenManagerServiceTest, SetSessionManagerService1, Function | SmallTest | Level2)
{
    std::vector<std::u16string> args;
    auto ret = 0;
    sptr<IRemoteObject> sessionManagerService = nullptr;
    MockScreenManagerService::GetInstance().SetSessionManagerService(sessionManagerService);
    ASSERT_EQ(0, ret);
}

/**
 * @tc.name: SetSessionManagerService
 * @tc.desc: SetSessionManagerService
 * @tc.type: FUNC
 */
HWTEST_F(MockScreenManagerServiceTest, SetSessionManagerService2, Function | SmallTest | Level2)
{
    MockScreenManagerService* mService = new MockScreenManagerService();
    std::vector<std::u16string> args;
    auto ret = 0;
    sptr<IRemoteObject> sessionManagerService = nullptr;
    mService->SetSessionManagerService(sessionManagerService);
    ASSERT_EQ(0, ret);
}
}
}
}