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

#include "common/include/extension_data_handler.h"

#include <gtest/gtest.h>
#include <message_parcel.h>
#include <want.h>

#include "extension_data_handler_mock.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen::Extension {
class ExtensionDataHandlerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: DataTransferConfigMarshalling01
 * @tc.desc: Test DataTransferConfig Marshalling with valid data
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionDataHandlerTest, DataTransferConfigMarshalling01, TestSize.Level1)
{
    DataTransferConfig config;
    config.subSystemId = SubSystemId::WM_UIEXT;
    config.customId = 123;
    config.needReply = true;
    config.needSyncSend = false;

    MessageParcel parcel;
    ASSERT_TRUE(config.Marshalling(parcel));
}

/**
 * @tc.name: DataTransferConfigUnmarshalling01
 * @tc.desc: Test DataTransferConfig Unmarshalling with valid data
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionDataHandlerTest, DataTransferConfigUnmarshalling01, TestSize.Level1)
{
    MessageParcel parcel;
    parcel.WriteUint8(static_cast<uint8_t>(SubSystemId::WM_UIEXT));
    parcel.WriteUint32(123);
    parcel.WriteBool(true);
    parcel.WriteBool(false);

    auto config = DataTransferConfig::Unmarshalling(parcel);
    ASSERT_NE(nullptr, config);
    ASSERT_EQ(SubSystemId::WM_UIEXT, config->subSystemId);
    ASSERT_EQ(123u, config->customId);
    ASSERT_TRUE(config->needReply);
    ASSERT_FALSE(config->needSyncSend);
    delete config;
}

/**
 * @tc.name: DataTransferConfigUnmarshalling02
 * @tc.desc: Test DataTransferConfig Unmarshalling with invalid subSystemId
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionDataHandlerTest, DataTransferConfigUnmarshalling02, TestSize.Level1)
{
    MessageParcel parcel;
    parcel.WriteUint8(static_cast<uint8_t>(SubSystemId::INVALID));
    parcel.WriteUint32(123);
    parcel.WriteBool(true);
    parcel.WriteBool(false);

    auto config = DataTransferConfig::Unmarshalling(parcel);
    ASSERT_EQ(nullptr, config);
}

/**
 * @tc.name: RegisterDataConsumer01
 * @tc.desc: Test RegisterDataConsumer with valid callback
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionDataHandlerTest, RegisterDataConsumer01, TestSize.Level1)
{
    MockDataHandler handler;
    auto callback =
        [](SubSystemId id, uint32_t customId, AAFwk::Want&& data, std::optional<AAFwk::Want>& reply) -> int32_t {
        return 0;
    };

    auto ret = handler.RegisterDataConsumer(SubSystemId::WM_UIEXT, std::move(callback));
    ASSERT_EQ(DataHandlerErr::OK, ret);
}

/**
 * @tc.name: RegisterDataConsumer02
 * @tc.desc: Test RegisterDataConsumer with duplicate registration
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionDataHandlerTest, RegisterDataConsumer02, TestSize.Level1)
{
    MockDataHandler handler;
    auto callback =
        [](SubSystemId id, uint32_t customId, AAFwk::Want&& data, std::optional<AAFwk::Want>& reply) -> int32_t {
        return 0;
    };
    auto callback1 = callback;
    auto ret = handler.RegisterDataConsumer(SubSystemId::WM_UIEXT, std::move(callback));
    ASSERT_EQ(ret, DataHandlerErr::OK);

    ret = handler.RegisterDataConsumer(SubSystemId::WM_UIEXT, std::move(callback1));
    ASSERT_EQ(ret, DataHandlerErr::DUPLICATE_REGISTRATION);
}

/**
 * @tc.name: UnregisterDataConsumer01
 * @tc.desc: Test UnregisterDataConsumer with registered consumer
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionDataHandlerTest, UnregisterDataConsumer01, TestSize.Level1)
{
    MockDataHandler handler;
    auto callback =
        [](SubSystemId id, uint32_t customId, AAFwk::Want&& data, std::optional<AAFwk::Want>& reply) -> int32_t {
        return 0;
    };
    auto callback1 = callback;

    ASSERT_EQ(DataHandlerErr::OK, handler.RegisterDataConsumer(SubSystemId::WM_UIEXT, std::move(callback)));
    handler.UnregisterDataConsumer(SubSystemId::WM_UIEXT);

    // Verify unregistration by trying to register again
    ASSERT_EQ(DataHandlerErr::OK, handler.RegisterDataConsumer(SubSystemId::WM_UIEXT, std::move(callback1)));
}

/**
 * @tc.name: SendDataTest01
 * @tc.desc: Test SendDataSync with mocked behavior
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionDataHandlerTest, SendDataTest01, TestSize.Level1)
{
    MockDataHandler handler;
    AAFwk::Want data;
    AAFwk::Want reply;

    // Set up expectations
    EXPECT_CALL(handler, SendData(testing::_, testing::_, testing::_)).WillOnce(testing::Return(DataHandlerErr::OK));

    // Test SendDataSync
    ASSERT_EQ(DataHandlerErr::OK, handler.SendDataSync(SubSystemId::WM_UIEXT, 123, data, reply));
}

/**
 * @tc.name: NotifyDataConsumer01
 * @tc.desc: Test NotifyDataConsumer with sync mode and valid callback
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionDataHandlerTest, NotifyDataConsumer01, TestSize.Level1)
{
    MockDataHandler handler;
    bool callbackCalled = false;

    auto callback = [&callbackCalled](SubSystemId id,
                                      uint32_t customId,
                                      AAFwk::Want&& data,
                                      std::optional<AAFwk::Want>& reply) -> int32_t {
        callbackCalled = true;
        EXPECT_EQ(SubSystemId::WM_UIEXT, id);
        EXPECT_EQ(123u, customId);
        if (reply.has_value()) {
            reply->SetParam("test", 1);
        }
        return 0;
    };

    // Register consumer
    auto ret = handler.RegisterDataConsumer(SubSystemId::WM_UIEXT, std::move(callback));
    ASSERT_EQ(DataHandlerErr::OK, ret);

    // Prepare test data
    AAFwk::Want data;
    std::optional<AAFwk::Want> reply = std::make_optional<AAFwk::Want>();
    DataTransferConfig config;
    config.needSyncSend = true;
    config.needReply = true;
    config.subSystemId = SubSystemId::WM_UIEXT;
    config.customId = 123;

    // Test NotifyDataConsumer
    ret = handler.NotifyDataConsumer(std::move(data), reply, config);
    ASSERT_EQ(DataHandlerErr::OK, ret);
    ASSERT_TRUE(callbackCalled);
    ASSERT_TRUE(reply.has_value());
    ASSERT_EQ(1, reply->GetIntParam("test", 0));
}

/**
 * @tc.name: NotifyDataConsumer02
 * @tc.desc: Test NotifyDataConsumer with async mode
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionDataHandlerTest, NotifyDataConsumer02, TestSize.Level1)
{
    MockDataHandler handler;
    bool callbackCalled = false;

    auto callback = [&callbackCalled](SubSystemId id,
                                      uint32_t customId,
                                      AAFwk::Want&& data,
                                      std::optional<AAFwk::Want>& reply) -> int32_t {
        callbackCalled = true;
        EXPECT_EQ(SubSystemId::WM_UIEXT, id);
        EXPECT_EQ(123u, customId);
        return 0;
    };

    // Register consumer
    ASSERT_EQ(DataHandlerErr::OK, handler.RegisterDataConsumer(SubSystemId::WM_UIEXT, std::move(callback)));

    // Prepare test data
    AAFwk::Want data;
    std::optional<AAFwk::Want> reply;
    DataTransferConfig config;
    config.needSyncSend = false;
    config.subSystemId = SubSystemId::WM_UIEXT;
    config.customId = 123;

    // Test NotifyDataConsumer
    auto ret = handler.NotifyDataConsumer(std::move(data), reply, config);
    ASSERT_EQ(DataHandlerErr::OK, ret);
}

/**
 * @tc.name: NotifyDataConsumer03
 * @tc.desc: Test NotifyDataConsumer with unregistered consumer
 * @tc.type: FUNC
 */
HWTEST_F(ExtensionDataHandlerTest, NotifyDataConsumer03, TestSize.Level1)
{
    MockDataHandler handler;
    AAFwk::Want data;
    std::optional<AAFwk::Want> reply;
    DataTransferConfig config;
    config.subSystemId = SubSystemId::WM_UIEXT;
    config.customId = 123;

    // Test NotifyDataConsumer without registering consumer
    auto ret = handler.NotifyDataConsumer(std::move(data), reply, config);
    ASSERT_EQ(DataHandlerErr::NO_CONSUME_CALLBACK, ret);
}
} // namespace OHOS::Rosen::Extension
