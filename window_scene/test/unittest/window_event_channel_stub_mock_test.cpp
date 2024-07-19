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

#include "session/container/include/zidl/window_event_channel_stub.h"
#include "session/container/include/zidl/session_stage_interface.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "iremote_object_mocker.h"
#include <message_option.h>
#include <message_parcel.h>
#include <string>
#include <gtest/gtest.h>
#include "mock_message_parcel.h"
#include "window_manager.h"
#include "window_manager_agent.h"
#include "window_manager_hilog.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventChannelProxyMockTest"};
}
class WindowEventChannelMockStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<ISessionStage> sessionStage = new SessionStageMocker();
    sptr<WindowEventChannelStub> windowEventChannelStub_ = new WindowEventChannelMocker(sessionStage);
};

void WindowEventChannelMockStubTest::SetUpTestCase()
{
}

void WindowEventChannelMockStubTest::TearDownTestCase()
{
}

void WindowEventChannelMockStubTest::SetUp()
{
}

void WindowEventChannelMockStubTest::TearDown()
{
}

/**
 * @tc.name: HandleTransferAccessibilityChildTreeRegister01
 * @tc.desc: test function : HandleTransferAccessibilityChildTreeRegister01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelMockStubTest, HandleTransferAccessibilityChildTreeRegister01, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferAccessibilityChildTreeRegister01 begin");
    MockMessageParcel::ClearAllErrorFlag();
    MessageParcel data;
    MessageParcel reply;
    
    data.WriteUint32(0);
    data.WriteInt32(0);
    data.WriteInt64(0);
    
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferAccessibilityChildTreeRegister(data, reply));
    WLOGI("HandleTransferAccessibilityChildTreeRegister01 end");
}

/**
 * @tc.name: HandleTransferAccessibilityChildTreeRegister02
 * @tc.desc: test function : HandleTransferAccessibilityChildTreeRegister02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelMockStubTest, HandleTransferAccessibilityChildTreeRegister02, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferAccessibilityChildTreeRegister02 begin");
    MockMessageParcel::ClearAllErrorFlag();
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    data.WriteInt32(0);
    data.WriteInt64(0);

    MockMessageParcel::SetWriteInt64ErrorFlag(true);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferAccessibilityChildTreeRegister(data, reply));
    WLOGI("HandleTransferAccessibilityChildTreeRegister02 end");
}

/**
 * @tc.name: HandleTransferAccessibilityChildTreeUnregister
 * @tc.desc: test function : HandleTransferAccessibilityChildTreeUnregister
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelMockStubTest, HandleTransferAccessibilityChildTreeUnregister, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferAccessibilityChildTreeUnregister begin");
    MockMessageParcel::ClearAllErrorFlag();
    MessageParcel data;
    MessageParcel reply;

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_NONE, windowEventChannelStub_->HandleTransferAccessibilityChildTreeUnregister(data, reply));
    WLOGI("HandleTransferAccessibilityChildTreeUnregister end");
}

/**
 * @tc.name: HandleTransferAccessibilityDumpChildInfo
 * @tc.desc: test function : HandleTransferAccessibilityDumpChildInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelMockStubTest, HandleTransferAccessibilityDumpChildInfo, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferAccessibilityDumpChildInfo begin");
    MockMessageParcel::ClearAllErrorFlag();
    MessageParcel data;
    MessageParcel reply;
    std::vector<std::string> params;
    data.WriteStringVector(params);

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_NONE, windowEventChannelStub_->HandleTransferAccessibilityDumpChildInfo(data, reply));
    WLOGI("HandleTransferAccessibilityDumpChildInfo end");
}
}
}