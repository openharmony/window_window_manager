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
#include "session/container/include/zidl/window_event_ipc_interface_code.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "iremote_object_mocker.h"
#include <message_option.h>
#include <message_parcel.h>
#include <string>
#include <gtest/gtest.h>
#include <axis_event.h>
#include <ipc_types.h>
#include <key_event.h>
#include <pointer_event.h>
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "window_manager.h"
#include "window_manager_agent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowEventChannelStubTest"};
}
class WindowEventChannelStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<ISessionStage> sessionStage = new SessionStageMocker();
    sptr<WindowEventChannelStub> windowEventChannelStub_ = new WindowEventChannelMocker(sessionStage);
};

void WindowEventChannelStubTest::SetUpTestCase()
{
}

void WindowEventChannelStubTest::TearDownTestCase()
{
}

void WindowEventChannelStubTest::SetUp()
{
}

void WindowEventChannelStubTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteRequest
 * @tc.desc: test function : OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, OnRemoteRequest, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(WindowEventChannelStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        ISceneSessionManager::SceneSessionManagerMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->OnRemoteRequest(code, data, reply, option));
}

/**
 * @tc.name: HandleTransferKeyEvent01
 * @tc.desc: test function : HandleTransferKeyEvent01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferKeyEvent01, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(1);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(5, windowEventChannelStub_->HandleTransferKeyEvent(data, reply));
}

/**
 * @tc.name: HandleTransferKeyEvent02
 * @tc.desc: test function : HandleTransferKeyEvent02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferKeyEvent02, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(5, windowEventChannelStub_->HandleTransferKeyEvent(data, reply));
}

/**
 * @tc.name: HandleTransferKeyEventAsync
 * @tc.desc: test function : HandleTransferKeyEventAsync
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferKeyEventAsync, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(true);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(5, windowEventChannelStub_->HandleTransferKeyEventAsync(data, reply));
}

/**
 * @tc.name: HandleTransferPointerEvent01
 * @tc.desc: test function : HandleTransferPointerEvent01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferPointerEvent01, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(5, windowEventChannelStub_->HandleTransferPointerEvent(data, reply));
}

/**
 * @tc.name: HandleTransferPointerEvent02
 * @tc.desc: test function : HandleTransferPointerEvent02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferPointerEvent02, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(5, windowEventChannelStub_->HandleTransferPointerEvent(data, reply));
}

/**
 * @tc.name: HandleTransferFocusActiveEvent
 * @tc.desc: test function : HandleTransferFocusActiveEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFocusActiveEvent, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(false);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferFocusActiveEvent(data, reply));
}

/**
 * @tc.name: HandleTransferFocusStateEvent
 * @tc.desc: test function : HandleTransferFocusStateEvent
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFocusStateEvent, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteBool(false);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferFocusStateEvent(data, reply));
}

/**
 * @tc.name: HandleTransferAccessibilityChildTreeRegister01
 * @tc.desc: test function : HandleTransferAccessibilityChildTreeRegister01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferAccessibilityChildTreeRegister01, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferAccessibilityChildTreeRegister01 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    data.WriteInt32(0);
    data.WriteInt64(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferAccessibilityChildTreeRegister(data, reply));
    WLOGI("HandleTransferAccessibilityChildTreeRegister01 end");
}

/**
 * @tc.name: HandleTransferAccessibilityChildTreeRegister02
 * @tc.desc: test function : HandleTransferAccessibilityChildTreeRegister02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferAccessibilityChildTreeRegister02, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferAccessibilityChildTreeRegister02 begin");
    MessageParcel data;
    MessageParcel reply;
    
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferAccessibilityChildTreeRegister(data, reply));
    WLOGI("HandleTransferAccessibilityChildTreeRegister02 end");
}

/**
 * @tc.name: HandleTransferAccessibilityChildTreeRegister03
 * @tc.desc: test function : HandleTransferAccessibilityChildTreeRegister03
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferAccessibilityChildTreeRegister03, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferAccessibilityChildTreeRegister03 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferAccessibilityChildTreeRegister(data, reply));
    WLOGI("HandleTransferAccessibilityChildTreeRegister03 end");
}

/**
 * @tc.name: HandleTransferAccessibilityChildTreeRegister04
 * @tc.desc: test function : HandleTransferAccessibilityChildTreeRegister04
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferAccessibilityChildTreeRegister04, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferAccessibilityChildTreeRegister04 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    data.WriteInt32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferAccessibilityChildTreeRegister(data, reply));
    WLOGI("HandleTransferAccessibilityChildTreeRegister04 end");
}

/**
 * @tc.name: HandleTransferAccessibilityChildTreeUnregister01
 * @tc.desc: test function : HandleTransferAccessibilityChildTreeUnregister01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferAccessibilityChildTreeUnregister01, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferAccessibilityChildTreeUnregister01 begin");
    MessageParcel data;
    MessageParcel reply;
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferAccessibilityChildTreeUnregister(data, reply));
    WLOGI("HandleTransferAccessibilityChildTreeUnregister01 end");
}

/**
 * @tc.name: HandleTransferAccessibilityDumpChildInfo01
 * @tc.desc: test function : HandleTransferAccessibilityDumpChildInfo01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferAccessibilityDumpChildInfo01, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferAccessibilityDumpChildInfo01 begin");
    MessageParcel data;
    MessageParcel reply;

    std::vector<std::string> params;
    data.WriteStringVector(params);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferAccessibilityDumpChildInfo(data, reply));
    WLOGI("HandleTransferAccessibilityDumpChildInfo01 end");
}
}
}
}