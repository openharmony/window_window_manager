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
#include "session_manager/include/screen_session_manager_lite.h"
#include "dm_common.h"
#include "zidl/screen_session_manager_lite_stub.h"
#include "display_manager.h"
#include "display_manager_adapter.h"
#include "display_manager_agent_default.h"
#include "zidl/screen_session_manager_lite_interface.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t SLEEP_TIME_IN_US = 100000; // 100ms
}
class ScreenSessionManagerLiteStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<ScreenSessionManagerLiteStub> stub_;
};

void ScreenSessionManagerLiteStubTest::SetUpTestCase()
{
}

void ScreenSessionManagerLiteStubTest::TearDownTestCase()
{
    usleep(SLEEP_TIME_IN_US);
}

void ScreenSessionManagerLiteStubTest::SetUp()
{
    stub_ = new ScreenSessionManagerLiteStub();
}

void ScreenSessionManagerLiteStubTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteRequest01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteStubTest, OnRemoteRequest01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerLiteStub::GetDescriptor());
    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    data.WriteRemoteObject(displayManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IScreenSessionManagerLite::ScreenManagerLiteMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteStubTest, OnRemoteRequest02, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerLiteStub::GetDescriptor());
    DisplayManagerAgentType type = DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IDisplayManagerAgent> displayManagerAgent = new DisplayManagerAgentDefault();
    data.WriteRemoteObject(displayManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        IScreenSessionManagerLite::ScreenManagerLiteMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest03
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteStubTest, OnRemoteRequest03, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerLiteStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IScreenSessionManagerLite::ScreenManagerLiteMessage::TRANS_ID_SCENE_BOARD_GET_FOLD_DISPLAY_MODE);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest04
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteStubTest, OnRemoteRequest04, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerLiteStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IScreenSessionManagerLite::ScreenManagerLiteMessage::TRANS_ID_SCENE_BOARD_IS_FOLDABLE);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest05
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteStubTest, OnRemoteRequest05, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerLiteStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IScreenSessionManagerLite::ScreenManagerLiteMessage::TRANS_ID_SCENE_BOARD_GET_FOLD_STATUS);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest06
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteStubTest, OnRemoteRequest06, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerLiteStub::GetDescriptor());

    uint32_t code = static_cast<uint32_t>(
        IScreenSessionManagerLite::ScreenManagerLiteMessage::TRANS_ID_GET_DEFAULT_DISPLAY_INFO);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest07
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteStubTest, OnRemoteRequest07, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerLiteStub::GetDescriptor());
    DisplayId displayId = 0;
    data.WriteUint64(displayId);

    uint32_t code = static_cast<uint32_t>(
        IScreenSessionManagerLite::ScreenManagerLiteMessage::TRANS_ID_GET_DISPLAY_BY_ID);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest08
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(ScreenSessionManagerLiteStubTest, OnRemoteRequest08, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(ScreenSessionManagerLiteStub::GetDescriptor());
    DisplayId displayId = 0;
    data.WriteUint64(displayId);

    uint32_t code = static_cast<uint32_t>(
        IScreenSessionManagerLite::ScreenManagerLiteMessage::TRANS_ID_GET_CUTOUT_INFO);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}
}
}
}