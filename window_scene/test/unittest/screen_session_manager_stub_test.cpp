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
    data.WriteBool(false);
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
    data.WriteBool(false);
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
    data.WriteBool(false);
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
    data.WriteBool(false);
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
    data.WriteBool(false);
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
    data.WriteBool(false);
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
    data.WriteBool(false);
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
}
}
}