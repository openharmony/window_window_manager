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
#include "common_event_stub.h"
#include "mock_screen_manager_service.h"
#include "mock_screen_manager_service_stub.h"
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

class MockScreenManagerServiceStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MockScreenManagerServiceStubTest::SetUpTestCase()
{
}

void MockScreenManagerServiceStubTest::TearDownTestCase()
{
}

void MockScreenManagerServiceStubTest::SetUp()
{
}

void MockScreenManagerServiceStubTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteRequest
 * @tc.desc: on start
 * @tc.type: FUNC
 */
HWTEST_F(MockScreenManagerServiceStubTest, OnRemoteRequest, Function | SmallTest | Level2)
{
    WLOGI("OnRemoteRequest");
    uint32_t code = static_cast<uint32_t>(
        IMockScreenManagerInterface::MockScreenManagerServiceMessage::TRANS_ID_GET_SCREEN_DUMP_INFO);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    sptr<MockScreenManagerService> mockScreenManagerService = new MockScreenManagerService();
    data.WriteInterfaceToken(EventFwk::CommonEventStub::GetDescriptor());
    int32_t res = mockScreenManagerService->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(-1, res);
    mockScreenManagerService.clear();
}

}
}
}