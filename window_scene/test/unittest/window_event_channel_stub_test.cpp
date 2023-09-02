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
#include <gtest/gtest.h>
#include <axis_event.h>
#include <ipc_types.h>
#include <key_event.h>
#include <pointer_event.h>
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "window_manager.h"
#include "window_manager_agent.h"
#include "window_manager_hilog.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
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
}
}
}