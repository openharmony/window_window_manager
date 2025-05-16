/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include <iremote_broker.h>
#include <iremote_object.h>

#include "display_manager_agent_default.h"
#include "display_manager_stub.h"
#include "idisplay_manager.h"
#include "iremote_object_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
using RemoteMocker = MockIRemoteObject;
class DisplayManagerStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<DisplayManagerStub> stub_;
};

void DisplayManagerStubTest::SetUpTestCase()
{
}

void DisplayManagerStubTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void DisplayManagerStubTest::SetUp()
{
    stub_ = new DisplayManagerStub();
}

void DisplayManagerStubTest::TearDown()
{
}

/**
 * @tc.name: OnRemoteRequest01
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest01, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_DEFAULT_DISPLAY_INFO);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest02
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest02, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_DISPLAY_INFO_BY_ID);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest03
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest03, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_VISIBLE_AREA_DISPLAY_INFO_BY_ID);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest04
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest04, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_DISPLAY_INFO_BY_SCREEN);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest05
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest05, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_CREATE_VIRTUAL_SCREEN);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest06
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest06, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::
        COMMAND_CREATE_VIRTUAL_SCREEN_IN_DMVIRTUALSCREENOPTION_IN_IREMOTEOBJECT_OUT_UNSIGNED_LONG_IN_IBUFFERPRODUCER);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest07
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest07, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_DESTROY_VIRTUAL_SCREEN);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest08
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest08, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_VIRTUAL_SCREEN_SURFACE);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest09
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest09, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_ORIENTATION);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest10
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest10, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_DISPLAY_SNAPSHOT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest11
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest11, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_SCREEN_SUPPORTED_COLOR_GAMUTS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest12
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest12, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_SCREEN_COLOR_GAMUT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest13
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest13, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_SCREEN_COLOR_GAMUT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest14
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest14, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_SCREEN_GAMUT_MAP);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest15
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest15, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_SCREEN_GAMUT_MAP);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest16
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest16, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_SCREEN_COLOR_TRANSFORM);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest17
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest17, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_REGISTER_DISPLAY_MANAGER_AGENT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest18
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest18, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_UNREGISTER_DISPLAY_MANAGER_AGENT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest19
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest19, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_WAKE_UP_BEGIN);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest20
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest20, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_WAKE_UP_END);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest21
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest21, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SUSPEND_BEGIN);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest22
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest22, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SUSPEND_END);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest23
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest23, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_SCREEN_POWER_FOR_ALL);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest24
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest24, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_SPECIFIED_SCREEN_POWER);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest25
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest25, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_SCREEN_POWER);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest26
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest26, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_DISPLAY_STATE);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest27
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest27, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_DISPLAY_STATE);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest28
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest28, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_TRY_TO_CANCEL_SCREEN_OFF);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest29
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest29, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_ALL_DISPLAY_IDS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest30
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest30, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_CUTOUT_INFO);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest31
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest31, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_ADD_SURFACE_NODE_TO_DISPLAY);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest32
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest32, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_REMOVE_SURFACE_NODE_FROM_DISPLAY);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest33
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest33, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_HAS_PRIVATE_WINDOW);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest34
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest34, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_NOTIFY_DISPLAY_EVENT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest35
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest35, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_FREEZE);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest36
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest36, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_MAKE_MIRROR);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest37
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest37, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_STOP_MIRROR);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest38
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest38, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_SCREEN_INFO_BY_ID);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest39
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest39, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_SCREEN_GROUP_INFO_BY_ID);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest40
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest40, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_ALL_SCREEN_INFOS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest41
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest41, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_MAKE_EXPAND);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest42
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest42, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_STOP_EXPAND);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest43
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest43, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_REMOVE_VIRTUAL_SCREEN_FROM_GROUP);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest44
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest44, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_SCREEN_ACTIVE_MODE);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest45
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest45, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_VIRTUAL_PIXEL_RATIO);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest46
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest46, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_RESOLUTION);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest47
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest47, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_DENSITY_IN_CUR_RESOLUTION);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest48
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest48, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_IS_SCREEN_ROTATION_LOCKED);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest49
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest49, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_SCREEN_ROTATION_LOCKED);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest50
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest50, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_SET_SCREEN_ROTATION_LOCKED_FROM_JS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}


/**
 * @tc.name: OnRemoteRequest51
 * @tc.desc: test DisplayManagerStubTest::OnRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerStubTest, OnRemoteRequest51, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(DisplayManagerStub::GetDescriptor());

    auto code = static_cast<uint32_t>(IDisplayManagerIpcCode::COMMAND_GET_ALL_DISPLAY_PHYSICAL_RESOLUTION);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}
} // namespace OHOS::Rosen
