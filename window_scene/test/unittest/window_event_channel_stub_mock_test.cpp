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
 * @tc.name: HandleTransferSearchElementInfo
 * @tc.desc: test function : HandleTransferSearchElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelMockStubTest, HandleTransferSearchElementInfo, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferSearchElementInfo begin");
    MockMessageParcel::ClearAllErrorFlag();
    MessageParcel data;
    MessageParcel reply;
    
    data.WriteUint32(0);
    data.WriteUint32(0);
    data.WriteUint32(0);
    
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfo(data, reply));
    WLOGI("HandleTransferSearchElementInfo end");
}

/**
 * @tc.name: HandleTransferSearchElementInfosByText
 * @tc.desc: test function : HandleTransferSearchElementInfosByText
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelMockStubTest, HandleTransferSearchElementInfosByText, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferSearchElementInfosByText begin");
    MockMessageParcel::ClearAllErrorFlag();
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    data.WriteString("action");
    data.WriteUint32(0);
    
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfosByText(data, reply));
    WLOGI("HandleTransferSearchElementInfosByText end");
}

/**
 * @tc.name: HandleTransferFindFocusedElementInfo
 * @tc.desc: test function : HandleTransferFindFocusedElementInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelMockStubTest, HandleTransferFindFocusedElementInfo, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferFindFocusedElementInfo begin");
    MockMessageParcel::ClearAllErrorFlag();
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    data.WriteUint32(0);
    data.WriteUint32(0);

    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFindFocusedElementInfo(data, reply));
    WLOGI("HandleTransferFindFocusedElementInfo end");
}

/**
 * @tc.name: HandleTransferFocusMoveSearch
 * @tc.desc: test function : HandleTransferFocusMoveSearch
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelMockStubTest, HandleTransferFocusMoveSearch, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferFocusMoveSearch begin");
    MockMessageParcel::ClearAllErrorFlag();
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    data.WriteUint32(0);
    data.WriteUint32(0);

    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFocusMoveSearch(data, reply));
    WLOGI("HandleTransferFocusMoveSearch end");
}
}
}