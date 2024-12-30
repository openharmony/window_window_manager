/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ui_extension/provider_data_handler.h"

#include <gtest/gtest.h>
#include <message_parcel.h>
#include <want.h>
#include <memory>

#include "iremote_object_mocker.h"
#include "session/host/include/zidl/session_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen::Extension {
class ProviderDataHandlerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: ProviderDataHandlerSendData01
 * @tc.desc: Test send data
 * @tc.type: FUNC
 */
TEST_F(ProviderDataHandlerTest, ProviderDataHandlerSendData01)
{
    auto remoteObj = sptr<RemoteObjectMocker>::MakeSptr();
    auto sessionProxy = sptr<SessionProxy>::MakeSptr(remoteObj);

    // Mock the SendRequest behavior
    EXPECT_CALL(*remoteObj, SendRequest(_, _, _, _))
        .WillOnce(Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) -> int {
            // Verify the message option is sync
            EXPECT_TRUE(option.GetFlags() & MessageOption::TF_SYNC);

            // Write mock response
            reply.WriteUint32(static_cast<uint32_t>(DataHandlerErr::OK));  // Reply code

            // Create and write mock reply Want
            AAFwk::Want replyWant;
            replyWant.SetParam("test_key", std::string("test_value"));
            reply.WriteParcelable(&replyWant);

            return 0;
        }));

    // Create test data
    std::unique_ptr<ProviderDataHandler> handler = std::make_unique<ProviderDataHandler>();
    handler->SetRemoteProxyObject(sessionProxy->AsRemoteObject());

    AAFwk::Want sendWant;
    sendWant.SetParam("send_key", std::string("send_value"));
    AAFwk::Want reply;

    // Test sync send with reply
    auto result = handler->SendDataSync(SubSystemId::UI_EXTENSION, 1, sendWant, reply);

    // Verify results
    EXPECT_EQ(result, DataHandlerErr::OK);
    EXPECT_EQ(reply.GetStringParam("test_key"), "test_value");
}
}  // namespace OHOS::Rosen::Extension
