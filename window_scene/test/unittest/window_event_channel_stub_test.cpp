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

/**
 * @tc.name: HandleTransferSearchElementInfo01
 * @tc.desc: test function : HandleTransferSearchElementInfo01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfo01, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    
    data.WriteUint32(0);
    data.WriteUint32(0);
    data.WriteUint32(0);

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferSearchElementInfo(data, reply));
}

/**
 * @tc.name: HandleTransferSearchElementInfo02
 * @tc.desc: test function : HandleTransferSearchElementInfo02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfo02, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfo(data, reply));
}

/**
 * @tc.name: HandleTransferSearchElementInfo03
 * @tc.desc: test function : HandleTransferSearchElementInfo03
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfo03, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfo(data, reply));
}

/**
 * @tc.name: HandleTransferSearchElementInfo04
 * @tc.desc: test function : HandleTransferSearchElementInfo04
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfo04, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    
    data.WriteUint32(0);
    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfo(data, reply));
}

/**
 * @tc.name: HandleTransferSearchElementInfosByText01
 * @tc.desc: test function : HandleTransferSearchElementInfosByText01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfosByText01, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    data.WriteString("action");
    data.WriteUint32(0);
    
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferSearchElementInfosByText(data, reply));
}

/**
 * @tc.name: HandleTransferSearchElementInfosByText02
 * @tc.desc: test function : HandleTransferSearchElementInfosByText02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfosByText02, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfosByText(data, reply));
}

/**
 * @tc.name: HandleTransferSearchElementInfosByText03
 * @tc.desc: test function : HandleTransferSearchElementInfosByText02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfosByText03, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfosByText(data, reply));
}

/**
 * @tc.name: HandleTransferSearchElementInfosByText04
 * @tc.desc: test function : HandleTransferSearchElementInfosByText04
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfosByText04, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    data.WriteString("action");
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfosByText(data, reply));
}

/**
 * @tc.name: HandleTransferFindFocusedElementInfo01
 * @tc.desc: test function : HandleTransferFindFocusedElementInfo01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFindFocusedElementInfo01, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    data.WriteUint32(0);
    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferFindFocusedElementInfo(data, reply));
}

/**
 * @tc.name: HandleTransferFindFocusedElementInfo02
 * @tc.desc: test function : HandleTransferFindFocusedElementInfo02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFindFocusedElementInfo02, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFindFocusedElementInfo(data, reply));
}

/**
 * @tc.name: HandleTransferFindFocusedElementInfo03
 * @tc.desc: test function : HandleTransferFindFocusedElementInfo03
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFindFocusedElementInfo03, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFindFocusedElementInfo(data, reply));
}

/**
 * @tc.name: HandleTransferFindFocusedElementInfo04
 * @tc.desc: test function : HandleTransferFindFocusedElementInfo04
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFindFocusedElementInfo04, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFindFocusedElementInfo(data, reply));
}

/**
 * @tc.name: HandleTransferFocusMoveSearch01
 * @tc.desc: test function : HandleTransferFocusMoveSearch01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFocusMoveSearch01, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    data.WriteUint32(0);
    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferFocusMoveSearch(data, reply));
}

/**
 * @tc.name: HandleTransferFocusMoveSearch02
 * @tc.desc: test function : HandleTransferFocusMoveSearch02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFocusMoveSearch02, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFocusMoveSearch(data, reply));
}

/**
 * @tc.name: HandleTransferFocusMoveSearch03
 * @tc.desc: test function : HandleTransferFocusMoveSearch03
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFocusMoveSearch03, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFocusMoveSearch(data, reply));
}

/**
 * @tc.name: HandleTransferFocusMoveSearch04
 * @tc.desc: test function : HandleTransferFocusMoveSearch04
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFocusMoveSearch04, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint32(0);
    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFocusMoveSearch(data, reply));
}

/**
 * @tc.name: HandleTransferExecuteAction01
 * @tc.desc: test function : HandleTransferExecuteAction01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction01, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    std::vector<std::string> actionArgumentsKey {};
    std::vector<std::string> actionArgumentsValue {};

    data.WriteUint32(0);
    data.WriteUint32(0);
    data.WriteStringVector(actionArgumentsKey);
    data.WriteStringVector(actionArgumentsValue);
    data.WriteUint32(0);

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
}

/**
 * @tc.name: HandleTransferExecuteAction02
 * @tc.desc: test function : HandleTransferExecuteAction02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction02, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
}

/**
 * @tc.name: HandleTransferExecuteAction03
 * @tc.desc: test function : HandleTransferExecuteAction03
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction03, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
}

/**
 * @tc.name: HandleTransferExecuteAction04
 * @tc.desc: test function : HandleTransferExecuteAction04
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction04, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    data.WriteUint32(0);
    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
}

/**
 * @tc.name: HandleTransferExecuteAction05
 * @tc.desc: test function : HandleTransferExecuteAction05
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction05, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    
    std::vector<std::string> actionArgumentsKey {};

    data.WriteUint32(0);
    data.WriteUint32(0);
    data.WriteStringVector(actionArgumentsKey);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
}

/**
 * @tc.name: HandleTransferExecuteAction06
 * @tc.desc: test function : HandleTransferExecuteAction06
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction06, Function | SmallTest | Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    
    std::vector<std::string> actionArgumentsKey {};
    std::vector<std::string> actionArgumentsValue {};

    data.WriteUint32(0);
    data.WriteUint32(0);
    data.WriteStringVector(actionArgumentsKey);
    data.WriteStringVector(actionArgumentsValue);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
}
}
}
}