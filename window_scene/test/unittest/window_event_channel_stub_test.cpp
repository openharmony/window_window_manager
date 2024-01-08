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
    constexpr int32_t MAX_ARGUMENTS_KEY_SIZE = 1000;
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
    WLOGI("HandleTransferSearchElementInfo01 begin");
    MessageParcel data;
    MessageParcel reply;
    
    data.WriteUint64(0);
    data.WriteUint32(0);
    data.WriteUint64(0);

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferSearchElementInfo(data, reply));
    WLOGI("HandleTransferSearchElementInfo01 end");
}

/**
 * @tc.name: HandleTransferSearchElementInfo02
 * @tc.desc: test function : HandleTransferSearchElementInfo02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfo02, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferSearchElementInfo02 begin");
    MessageParcel data;
    MessageParcel reply;
    
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfo(data, reply));
    WLOGI("HandleTransferSearchElementInfo02 end");
}

/**
 * @tc.name: HandleTransferSearchElementInfo03
 * @tc.desc: test function : HandleTransferSearchElementInfo03
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfo03, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferSearchElementInfo03 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint64(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfo(data, reply));
    WLOGI("HandleTransferSearchElementInfo03 end");
}

/**
 * @tc.name: HandleTransferSearchElementInfo04
 * @tc.desc: test function : HandleTransferSearchElementInfo04
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfo04, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferSearchElementInfo04 begin");
    MessageParcel data;
    MessageParcel reply;
    
    data.WriteUint64(0);
    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfo(data, reply));
    WLOGI("HandleTransferSearchElementInfo04 end");
}

/**
 * @tc.name: HandleTransferSearchElementInfosByText01
 * @tc.desc: test function : HandleTransferSearchElementInfosByText01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfosByText01, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferSearchElementInfosByText01 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint64(0);
    data.WriteString("action");
    data.WriteUint64(0);
    
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferSearchElementInfosByText(data, reply));
    WLOGI("HandleTransferSearchElementInfosByText01 end");
}

/**
 * @tc.name: HandleTransferSearchElementInfosByText02
 * @tc.desc: test function : HandleTransferSearchElementInfosByText02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfosByText02, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferSearchElementInfosByText02 begin");
    MessageParcel data;
    MessageParcel reply;
    
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfosByText(data, reply));
    WLOGI("HandleTransferSearchElementInfosByText02 end");
}

/**
 * @tc.name: HandleTransferSearchElementInfosByText03
 * @tc.desc: test function : HandleTransferSearchElementInfosByText02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfosByText03, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferSearchElementInfosByText03 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint64(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfosByText(data, reply));
    WLOGI("HandleTransferSearchElementInfosByText03 end");
}

/**
 * @tc.name: HandleTransferSearchElementInfosByText04
 * @tc.desc: test function : HandleTransferSearchElementInfosByText04
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferSearchElementInfosByText04, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferSearchElementInfosByText04 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint64(0);
    data.WriteString("action");
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferSearchElementInfosByText(data, reply));
    WLOGI("HandleTransferSearchElementInfosByText04 end");
}

/**
 * @tc.name: HandleTransferFindFocusedElementInfo01
 * @tc.desc: test function : HandleTransferFindFocusedElementInfo01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFindFocusedElementInfo01, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferFindFocusedElementInfo01 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint64(0);
    data.WriteUint32(0);
    data.WriteUint64(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferFindFocusedElementInfo(data, reply));
    WLOGI("HandleTransferFindFocusedElementInfo01 end");
}

/**
 * @tc.name: HandleTransferFindFocusedElementInfo02
 * @tc.desc: test function : HandleTransferFindFocusedElementInfo02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFindFocusedElementInfo02, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferFindFocusedElementInfo02 begin");
    MessageParcel data;
    MessageParcel reply;

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFindFocusedElementInfo(data, reply));
    WLOGI("HandleTransferFindFocusedElementInfo02 end");
}

/**
 * @tc.name: HandleTransferFindFocusedElementInfo03
 * @tc.desc: test function : HandleTransferFindFocusedElementInfo03
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFindFocusedElementInfo03, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferFindFocusedElementInfo03 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint64(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFindFocusedElementInfo(data, reply));
    WLOGI("HandleTransferFindFocusedElementInfo03 end");
}

/**
 * @tc.name: HandleTransferFindFocusedElementInfo04
 * @tc.desc: test function : HandleTransferFindFocusedElementInfo04
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFindFocusedElementInfo04, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferFindFocusedElementInfo04 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint64(0);
    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFindFocusedElementInfo(data, reply));
    WLOGI("HandleTransferFindFocusedElementInfo04 end");
}

/**
 * @tc.name: HandleTransferFocusMoveSearch01
 * @tc.desc: test function : HandleTransferFocusMoveSearch01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFocusMoveSearch01, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferFocusMoveSearch01 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint64(0);
    data.WriteUint32(0);
    data.WriteUint64(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferFocusMoveSearch(data, reply));
    WLOGI("HandleTransferFocusMoveSearch01 end");
}

/**
 * @tc.name: HandleTransferFocusMoveSearch02
 * @tc.desc: test function : HandleTransferFocusMoveSearch02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFocusMoveSearch02, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferFocusMoveSearch02 begin");
    MessageParcel data;
    MessageParcel reply;

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFocusMoveSearch(data, reply));
    WLOGI("HandleTransferFocusMoveSearch02 end");
}

/**
 * @tc.name: HandleTransferFocusMoveSearch03
 * @tc.desc: test function : HandleTransferFocusMoveSearch03
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFocusMoveSearch03, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferFocusMoveSearch03 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint64(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFocusMoveSearch(data, reply));
    WLOGI("HandleTransferFocusMoveSearch03 end");
}

/**
 * @tc.name: HandleTransferFocusMoveSearch04
 * @tc.desc: test function : HandleTransferFocusMoveSearch04
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferFocusMoveSearch04, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferFocusMoveSearch04 begin");
    MessageParcel data;
    MessageParcel reply;

    data.WriteUint64(0);
    data.WriteUint32(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferFocusMoveSearch(data, reply));
    WLOGI("HandleTransferFocusMoveSearch04 end");
}

/**
 * @tc.name: HandleTransferExecuteAction01
 * @tc.desc: test function : HandleTransferExecuteAction01
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction01, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferExecuteAction01 begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    std::vector<std::string> actionArgumentsKey {};
    std::vector<std::string> actionArgumentsValue {};

    data.WriteUint64(0);
    data.WriteUint32(0);
    data.WriteStringVector(actionArgumentsKey);
    data.WriteStringVector(actionArgumentsValue);
    data.WriteUint64(0);

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(0, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
    WLOGI("HandleTransferExecuteAction01 end");
}

/**
 * @tc.name: HandleTransferExecuteAction02
 * @tc.desc: test function : HandleTransferExecuteAction02
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction02, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferExecuteAction02 begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
    WLOGI("HandleTransferExecuteAction02 end");
}

/**
 * @tc.name: HandleTransferExecuteAction03
 * @tc.desc: test function : HandleTransferExecuteAction03
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction03, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferExecuteAction03 begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    data.WriteUint64(0);
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
    WLOGI("HandleTransferExecuteAction03 end");
}

/**
 * @tc.name: HandleTransferExecuteAction04
 * @tc.desc: test function : HandleTransferExecuteAction04
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction04, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferExecuteAction04 begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    data.WriteUint64(0);
    data.WriteUint32(0);
    std::vector<std::string> actionArgumentsKey {};
    std::vector<std::string> actionArgumentsValue {};
    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_DATA, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
    WLOGI("HandleTransferExecuteAction04 end");
}

/**
 * @tc.name: HandleTransferExecuteAction05
 * @tc.desc: test function : HandleTransferExecuteAction
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction05, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferExecuteAction05 begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    std::vector<std::string> actionArgumentsKey;
    std::vector<std::string> actionArgumentsValue;
    for (int i = 0; i <= MAX_ARGUMENTS_KEY_SIZE; i++) {
        actionArgumentsKey.push_back(std::to_string(i));
        actionArgumentsValue.push_back(std::to_string(i));
    }
    
    data.WriteUint64(0);
    data.WriteUint32(0);
    data.WriteStringVector(actionArgumentsKey);
    data.WriteStringVector(actionArgumentsValue);
    data.WriteUint64(0);

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_VALUE, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
    WLOGI("HandleTransferExecuteAction05 end");
}

/**
 * @tc.name: HandleTransferExecuteAction06
 * @tc.desc: test function : HandleTransferExecuteAction
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction06, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferExecuteAction06 begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    std::vector<std::string> actionArgumentsKey;
    std::vector<std::string> actionArgumentsValue;
    for (int i = 0; i <= 4; i++) {
        actionArgumentsKey.push_back(std::to_string(i));
    }
    
    for (int i = 0; i <= 5; i++) {
        actionArgumentsValue.push_back(std::to_string(i));
    }

    data.WriteUint64(0);
    data.WriteUint32(0);
    data.WriteStringVector(actionArgumentsKey);
    data.WriteStringVector(actionArgumentsValue);
    data.WriteUint64(0);

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_VALUE, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
    WLOGI("HandleTransferExecuteAction06 end");
}

/**
 * @tc.name: HandleTransferExecuteAction01
 * @tc.desc: test function : HandleTransferExecuteAction
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction07, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferExecuteAction07 begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    std::vector<std::string> actionArgumentsKey;
    actionArgumentsKey.push_back("action");
    std::vector<std::string> actionArgumentsValue;

    data.WriteUint64(0);
    data.WriteUint32(0);
    data.WriteStringVector(actionArgumentsKey);
    data.WriteStringVector(actionArgumentsValue);
    data.WriteUint64(0);

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_VALUE, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
    WLOGI("HandleTransferExecuteAction07 end");
}

/**
 * @tc.name: HandleTransferExecuteAction02
 * @tc.desc: test function : HandleTransferExecuteAction
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction08, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferExecuteAction08 begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    
    std::vector<std::string> actionArgumentsKey;
    std::vector<std::string> actionArgumentsValue;
    actionArgumentsValue.push_back("click");

    data.WriteUint64(0);
    data.WriteUint32(0);
    data.WriteStringVector(actionArgumentsKey);
    data.WriteStringVector(actionArgumentsValue);
    data.WriteUint64(0);

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_VALUE, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
    WLOGI("HandleTransferExecuteAction08 end");
}

/**
 * @tc.name: HandleTransferExecuteAction03
 * @tc.desc: test function : HandleTransferExecuteAction
 * @tc.type: FUNC
 */
HWTEST_F(WindowEventChannelStubTest, HandleTransferExecuteAction09, Function | SmallTest | Level1)
{
    WLOGI("HandleTransferExecuteAction09 begin");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    std::vector<std::string> actionArgumentsKey;
    actionArgumentsKey.push_back("action");
    actionArgumentsKey.push_back("action");

    std::vector<std::string> actionArgumentsValue;
    actionArgumentsValue.push_back("click");

    data.WriteUint64(0);
    data.WriteUint32(0);
    data.WriteStringVector(actionArgumentsKey);
    data.WriteStringVector(actionArgumentsValue);
    data.WriteUint64(0);

    ASSERT_TRUE((windowEventChannelStub_ != nullptr));
    ASSERT_EQ(ERR_INVALID_VALUE, windowEventChannelStub_->HandleTransferExecuteAction(data, reply));
    WLOGI("HandleTransferExecuteAction09 end");
}
}
}
}