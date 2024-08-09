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

#include <gtest/gtest.h>
#include <message_option.h>
#include <message_parcel.h>
#include <common/rs_rect.h>
#include <transaction/rs_marshalling_helper.h>

#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "window_manager_agent.h"
#include "zidl/screen_session_manager_stub.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class ScreenSessionManagerStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<ScreenSessionManagerStub> stub_;
};

void ScreenSessionManagerStubTest::SetUpTestCase()
{
}

void ScreenSessionManagerStubTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void ScreenSessionManagerStubTest::SetUp()
{
    stub_ = new ScreenSessionManagerStub();
}

void ScreenSessionManagerStubTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteRequest01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_DEFAULT_DISPLAY_INFO);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest02, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, -1);
}

/**
 * @tc.name: OnRemoteRequest03
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest03, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, -1);
}

/**
 * @tc.name: OnRemoteRequest04
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest04, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_WAKE_UP_BEGIN);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest05
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest05, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_WAKE_UP_END);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest06
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest06, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SUSPEND_BEGIN);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest07
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest07, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SUSPEND_END);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest08
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest08, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_DISPLAY_STATE);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest09
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest09, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest10
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest10, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest11
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest11, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_EVENT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest12
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest12, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(1009);

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, -1);
}

/**
 * @tc.name: OnRemoteRequest13
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest13, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_ID);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest14
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest14, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_SCREEN);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest15
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest15, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_ALL_DISPLAYIDS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest16
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest16, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest17
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest17, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_ALL_SCREEN_INFOS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest18
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest18, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_GAMUTS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest19
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest19, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_CREATE_VIRTUAL_SCREEN);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest20
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest20, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_DESTROY_VIRTUAL_SCREEN);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest21
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest21, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest22
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest22, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_DESTROY_VIRTUAL_SCREEN);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest23
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest23, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_MIRROR);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest24
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest24, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_STOP_MIRROR);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest25
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest25, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_EXPAND);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest26
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest26, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_STOP_EXPAND);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest27
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest27, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest28
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest28, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_REMOVE_VIRTUAL_SCREEN_FROM_SCREEN_GROUP);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest29
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest29, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_SNAPSHOT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest30
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest30, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_SCREEN_ACTIVE_MODE);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest32
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest32, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_DISABLE_MIRROR);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest33
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest33, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteRemoteObject(nullptr);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_CLIENT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest34
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest34, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(0);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_SCREEN_PROPERTY);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest35
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest35, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(0);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_NODE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest36
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest36, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(0);
    RRect bounds;
    RSMarshallingHelper::Marshalling(data, bounds);
    data.WriteFloat(0);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_UPDATE_SCREEN_ROTATION_PROPERTY);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest37
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest37, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_CURVED_SCREEN_COMPRESSION_AREA);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest38
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest38, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(0);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_PHY_SCREEN_PROPERTY);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest39
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest39, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_SCREEN_PRIVACY_STATE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest40
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest40, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(0);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_SPACE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest41
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest41, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(0);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_SPACE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest42
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest42, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_SET_HDR_FORMAT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest43
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest43, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(0);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_HDR_FORMAT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest44
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest44, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(0);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_HDR_FORMAT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest45
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest45, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_SET_PIXEL_FORMAT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest46
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest46, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(0);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_PIXEL_FORMAT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest47
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest47, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_TRANSFORM);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest48
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest48, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_SET_GAMUT_MAP);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest49
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest49, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(0);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_GAMUT_MAP);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest050
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest050, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_GAMUT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest051
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest051, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(0);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_GAMUT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest052
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest052, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_DENSITY_IN_CURRENT_RESOLUTION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest053
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest053, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_RESOLUTION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest054
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest054, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_SPACE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest055
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest055, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_CANVAS_ROTATION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest056
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest056, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, -1);
}

/**
 * @tc.name: OnRemoteRequest057
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest057, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_DEVICE_SCREEN_CONFIG);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest58
 * @tc.desc: normal function, TRANS_ID_SET_SCREENID_PRIVACY_STATE test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest58, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    DisplayId displayId = 0;
    data.WriteUint64(displayId);
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_SCREENID_PRIVACY_STATE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest59
 * @tc.desc: normal function, TRANS_ID_SET_SCREEN_PRIVACY_WINDOW_LIST test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest59, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    DisplayId displayId = 0;
    data.WriteUint64(displayId);
    std::vector<std::string> privacyWindowList{"win0", "win1"};
    data.WriteStringVector(privacyWindowList);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_SCREEN_PRIVACY_WINDOW_LIST);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest60
 * @tc.desc: normal function, TRANS_ID_SET_SCREEN_OFF_DELAY_TIME test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest60, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteInt32(2000);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_SCREEN_OFF_DELAY_TIME);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest61
 * @tc.desc: normal function, TRANS_ID_SET_VIRTUAL_SCREEN_BLACK_LIST test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest61, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    ScreenId id = 1001;
    std::vector<uint64_t> windowIdList{10, 20, 30};
    data.WriteUint64(static_cast<uint64_t>(id));
    data.WriteUInt64Vector(windowIdList);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_BLACK_LIST);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest62
 * @tc.desc: normal function, TRANS_ID_NOTIFY_DISPLAY_HOOK_INFO test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest62, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteInt32(200);
    data.WriteBool(true);
    data.WriteUint32(1062);
    data.WriteUint32(1062);
    data.WriteFloat(1.5);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_HOOK_INFO);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest63
 * @tc.desc: normal function, TRANS_ID_RESET_ALL_FREEZE_STATUS test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest63, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_RESET_ALL_FREEZE_STATUS);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest64
 * @tc.desc: normal function, TRANS_ID_PROXY_FOR_FREEZE test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest64, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    int32_t size = 10;
    data.WriteInt32(size);
    for (int32_t i = 0; i < size; i++) {
        data.WriteInt32(i);
    }
    data.WriteBool(true);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_PROXY_FOR_FREEZE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest65
 * @tc.desc: normal function, TRANS_ID_DISABLE_POWEROFF_RENDER_CONTROL test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest65, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1065);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_DISABLE_POWEROFF_RENDER_CONTROL);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest66
 * @tc.desc: normal function, TRANS_ID_SWITCH_USER test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest66, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SWITCH_USER);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest67
 * @tc.desc: normal function, TRANS_ID_SET_VIRTUAL_SCREEN_REFRESH_RATE test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest67, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1067);
    data.WriteUint32(120);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_REFRESH_RATE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest68
 * @tc.desc: normal function, TRANS_ID_SET_VIRTUAL_SCREEN_FLAG test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest68, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1068);
    data.WriteUint32(100);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_FLAG);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest69
 * @tc.desc: normal function, TRANS_ID_GET_VIRTUAL_SCREEN_FLAG test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest69, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1069);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_VIRTUAL_SCREEN_FLAG);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest70
 * @tc.desc: normal function, TRANS_ID_NOTIFY_FOLD_TO_EXPAND_COMPLETION test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest70, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteBool(true);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_NOTIFY_FOLD_TO_EXPAND_COMPLETION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest71
 * @tc.desc: normal function, TRANS_ID_GET_AVAILABLE_AREA test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest71, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1071);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_AVAILABLE_AREA);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest72
 * @tc.desc: normal function, TRANS_ID_UPDATE_AVAILABLE_AREA test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest72, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1072);
    data.WriteInt32(100);
    data.WriteInt32(200);
    data.WriteUint32(150);
    data.WriteInt32(250);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_UPDATE_AVAILABLE_AREA);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest73
 * @tc.desc: normal function, TRANS_ID_RESIZE_VIRTUAL_SCREEN test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest73, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1073);
    data.WriteUint32(1080);
    data.WriteUint32(1440);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_RESIZE_VIRTUAL_SCREEN);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest74
 * @tc.desc: normal function, TRANS_ID_NOTIFY_DISPLAY_CHANGE_INFO test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest74, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    sptr<DisplayChangeInfo> info = new DisplayChangeInfo();
    info->action_ = ActionType::SINGLE_START;
    ScreenId toScreenId = SCREEN_ID_INVALID;
    info->toScreenId_ = toScreenId;
    ScreenId fromScreenId = SCREEN_ID_INVALID;
    info->fromScreenId_ = fromScreenId;
    std::string abilityName = "11";
    std::string bundleName = "22";
    info->abilityName_ = abilityName;
    info->bundleName_ = bundleName;
    info->Marshalling(data);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_CHANGE_INFO);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest75
 * @tc.desc: normal function, TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest75, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    std::vector<ScreenId> uniqueScreenIds = {1008, 2008, 3008};
    data.WriteUint32(uniqueScreenIds.size());
    data.WriteUInt64Vector(uniqueScreenIds);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest76
 * @tc.desc: normal function, TRANS_ID_SCENE_BOARD_GET_CURRENT_FOLD_CREASE_REGION test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest76, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_CURRENT_FOLD_CREASE_REGION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest77
 * @tc.desc: normal function, TRANS_ID_SCENE_BOARD_GET_FOLD_STATUS test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest77, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_FOLD_STATUS);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest78
 * @tc.desc: normal function, TRANS_ID_SCENE_BOARD_IS_FOLDABLE test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest78, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_IS_FOLDABLE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest79
 * @tc.desc: normal function, TRANS_ID_SCENE_BOARD_GET_FOLD_DISPLAY_MODE test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest79, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_GET_FOLD_DISPLAY_MODE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest80
 * @tc.desc: normal function, TRANS_ID_SCENE_BOARD_LOCK_FOLD_DISPLAY_STATUS test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest80, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_LOCK_FOLD_DISPLAY_STATUS);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest81
 * @tc.desc: normal function, TRANS_ID_SCENE_BOARD_SET_FOLD_DISPLAY_MODE test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest81, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_SET_FOLD_DISPLAY_MODE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest82
 * @tc.desc: normal function, TRANS_ID_DEVICE_IS_CAPTURE test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest82, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_DEVICE_IS_CAPTURE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest83
 * @tc.desc: normal function, TRANS_ID_SCENE_BOARD_DUMP_SPECIAL_SCREEN test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest83, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_DUMP_SPECIAL_SCREEN);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest84
 * @tc.desc: normal function, TRANS_ID_SCENE_BOARD_DUMP_ALL_SCREEN test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest84, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_DUMP_ALL_SCREEN);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest85
 * @tc.desc: normal function, TRANS_ID_HAS_IMMERSIVE_WINDOW test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest85, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_HAS_IMMERSIVE_WINDOW);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest86
 * @tc.desc: normal function, TRANS_ID_CONVERT_SCREENID_TO_RSSCREENID test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest86, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1086);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_CONVERT_SCREENID_TO_RSSCREENID);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest87
 * @tc.desc: normal function, TRANS_ID_HAS_PRIVATE_WINDOW test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest87, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1087);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_HAS_PRIVATE_WINDOW);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest88
 * @tc.desc: normal function, TRANS_ID_GET_CUTOUT_INFO test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest88, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1088);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_CUTOUT_INFO);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest89
 * @tc.desc: normal function, TRANS_ID_IS_SCREEN_ROTATION_LOCKED test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest89, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_IS_SCREEN_ROTATION_LOCKED);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest90
 * @tc.desc: normal function, TRANS_ID_SET_SCREEN_ROTATION_LOCKED_FROM_JS test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest90, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteBool(true);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED_FROM_JS);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest91
 * @tc.desc: normal function, TRANS_ID_SET_SCREEN_ROTATION_LOCKED test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest91, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteBool(true);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest92
 * @tc.desc: normal function, TRANS_ID_SET_ORIENTATION test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest92, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1092);
    data.WriteUint32(270);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_ORIENTATION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest93
 * @tc.desc: normal function, TRANS_ID_SET_VIRTUAL_PIXEL_RATIO_SYSTEM test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest93, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1093);
    data.WriteFloat(2.5);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO_SYSTEM);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest94
 * @tc.desc: normal function, TRANS_ID_GET_SNAPSHOT_BY_PICKER test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest94, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_SNAPSHOT_BY_PICKER);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest95
 * @tc.desc: normal function, TRANS_ID_SET_VIRTUAL_SCREEN_SCALE_MODE test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest95, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint64(1095);
    data.WriteUint32(25);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SCALE_MODE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest96
 * @tc.desc: normal function, TRANS_ID_SET_VIRTUAL_SCREEN_SURFACE test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest96, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SURFACE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest97
 * @tc.desc: normal function, TRANS_ID_SET_SPECIFIED_SCREEN_POWER test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest97, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    data.WriteUint32(1097);
    data.WriteUint32(1);
    data.WriteUint32(12);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_SPECIFIED_SCREEN_POWER);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest98
 * @tc.desc: normal function, TRANS_ID_PROXY_FOR_FREEZE test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest98, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    int32_t size = 200;
    data.WriteInt32(size);
    for (int32_t i = 0; i < size; i++) {
        data.WriteInt32(i);
    }
    data.WriteBool(true);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_PROXY_FOR_FREEZE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest99
 * @tc.desc: normal function, invalid TRANS_ID test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest99, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(3500);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_NE(res, 0);
}

/**
 * @tc.name: OnRemoteRequest100
 * @tc.desc: normal function, TRANS_ID_SET_VIRTUAL_SCREEN_BLACK_LIST test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest100, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    ScreenId id = 1100;
    std::vector<uint64_t> windowIdList{};
    data.WriteUint64(static_cast<uint64_t>(id));
    data.WriteUInt64Vector(windowIdList);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_BLACK_LIST);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest101
 * @tc.desc: normal function, TRANS_ID_NOTIFY_DISPLAY_CHANGE_INFO test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest101, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    sptr<DisplayChangeInfo> info = new DisplayChangeInfo();
    info->Marshalling(data);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_CHANGE_INFO);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest102
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest102, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(1102);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_PHY_SCREEN_PROPERTY);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest103
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest103, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(1103);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_DISPLAY_NODE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest104
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest104, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(1104);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_SCREEN_PROPERTY);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest105
 * @tc.desc: normal function, TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest105, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t size = 50;
    std::vector<ScreenId> uniqueScreenIds;
    for (uint32_t i = 0; i < size; i++) {
        uniqueScreenIds.push_back(i);
    }
    data.WriteUint32(size);
    data.WriteUInt64Vector(uniqueScreenIds);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest106
 * @tc.desc: normal function, TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest106, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());

    uint32_t size = 0;
    std::vector<ScreenId> uniqueScreenIds {};
    data.WriteUint32(size);
    data.WriteUInt64Vector(uniqueScreenIds);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_MAKE_UNIQUE_SCREEN);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest107
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest107, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(1107);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_SPACE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest108
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest108, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(1108);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_SPACE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest109
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest109, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(1109);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_HDR_FORMAT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest110
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest110, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(1100);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_HDR_FORMAT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest111
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest111, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(1111);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_PIXEL_FORMAT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest112
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest112, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(1112);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_GAMUT_MAP);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest113
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest113, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    data.WriteUint64(1113);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_GAMUT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest114
 * @tc.desc: normal function, TRANS_ID_GET_ALL_PHYSICAL_DISPLAY_RESOLUTION test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest114, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_ALL_PHYSICAL_DISPLAY_RESOLUTION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest115
 * @tc.desc: normal function, TRANS_ID_GET_SCREEN_POWER
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest115, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    ScreenId testScreenId = 1115;
    data.WriteUint64(static_cast<uint64_t>(testScreenId));

    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest116
 * @tc.desc: normal function, TRANS_ID_SET_VIRTUAL_SCREEN_CANVAS_ROTATION
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest116, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    ScreenId testScreenId = 1116;
    data.WriteUint64(static_cast<uint64_t>(testScreenId));
    data.WriteBool(false);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_CANVAS_ROTATION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest117
 * @tc.desc: normal function, TRANS_ID_SET_RESOLUTION
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest117, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    ScreenId testScreenId = 1117;
    data.WriteUint64(static_cast<uint64_t>(testScreenId));
    uint32_t width = 1080;
    data.WriteUint32(width);
    uint32_t height = 2560;
    data.WriteUint32(height);
    float virtualPixelRatio = 64;
    data.WriteFloat(virtualPixelRatio);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SET_RESOLUTION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest118
 * @tc.desc: normal function, TRANS_ID_GET_DENSITY_IN_CURRENT_RESOLUTION
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest118, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    ScreenId testScreenId = 1118;
    data.WriteUint64(static_cast<uint64_t>(testScreenId));
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_GET_DENSITY_IN_CURRENT_RESOLUTION);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest119
 * @tc.desc: normal function, TRANS_ID_SCREEN_SET_COLOR_GAMUT
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest119, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    ScreenId testScreenId = 1119;
    data.WriteUint64(static_cast<uint64_t>(testScreenId));
    int32_t colorGamutIdx = 64;
    data.WriteInt32(colorGamutIdx);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_GAMUT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest120
 * @tc.desc: normal function, TRANS_ID_SCREEN_SET_GAMUT_MAP
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest120, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    ScreenId testScreenId = 1119;
    data.WriteUint64(static_cast<uint64_t>(testScreenId));
    uint32_t colorGamutIdx = 64;
    data.WriteUint32(colorGamutIdx);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_SET_GAMUT_MAP);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest121
 * @tc.desc: normal function, TRANS_ID_SCREEN_SET_COLOR_TRANSFORM
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest121, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    ScreenId testScreenId = 1121;
    data.WriteUint64(static_cast<uint64_t>(testScreenId));
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_TRANSFORM);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest122
 * @tc.desc: normal function, TRANS_ID_SCREEN_SET_PIXEL_FORMAT
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest122, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    ScreenId testScreenId = 1122;
    data.WriteUint64(static_cast<uint64_t>(testScreenId));
    int32_t pixelFormat = 64;
    data.WriteInt32(pixelFormat);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_SET_PIXEL_FORMAT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest123
 * @tc.desc: normal function, TRANS_ID_SCREEN_SET_HDR_FORMAT
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest123, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    ScreenId testScreenId = 1123;
    data.WriteUint64(static_cast<uint64_t>(testScreenId));
    int32_t modeIdx = 120;
    data.WriteInt32(modeIdx);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_SET_HDR_FORMAT);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest124
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest124, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor());
    ScreenId testScreenId = 1123;
    data.WriteUint64(static_cast<uint64_t>(testScreenId));
    int32_t colorSpace = 32;
    data.WriteInt32(colorSpace);
    uint32_t code = static_cast<uint32_t>(
        IDisplayManager::DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_SPACE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest125
 * @tc.desc: TRANS_ID_SCENE_BOARD_SET_DISPLAY_SCALE test
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerStubTest, OnRemoteRequest125, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    ScreenId screenId = 0;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float pivotX = 0.5f;
    float pivotY = 0.5f;
    ASSERT_TRUE(data.WriteInterfaceToken(ScreenSessionManagerStub::GetDescriptor()) &&
                data.WriteUint64(static_cast<uint64_t>(screenId)) && data.WriteFloat(scaleX) &&
                data.WriteFloat(scaleY) && data.WriteFloat(pivotX) && data.WriteFloat(pivotY));
    uint32_t code =
        static_cast<uint32_t>(IDisplayManager::DisplayManagerMessage::TRANS_ID_SCENE_BOARD_SET_DISPLAY_SCALE);
    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}
}
}
}