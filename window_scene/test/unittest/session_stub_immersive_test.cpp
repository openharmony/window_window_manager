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

#include "mock/mock_session_stub.h"
#include "session/host/include/zidl/session_ipc_interface_code.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class SessionStubImmersiveTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<SessionStubMocker> session_ = nullptr;
};

void SessionStubImmersiveTest::SetUpTestCase() {}

void SessionStubImmersiveTest::TearDownTestCase() {}

void SessionStubImmersiveTest::SetUp()
{
    session_ = sptr<SessionStubMocker>::MakeSptr();
    EXPECT_NE(nullptr, session_);

    EXPECT_CALL(*session_, OnRemoteRequest(_, _, _, _))
        .WillOnce(Invoke([&](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) -> int {
            return session_->SessionStub::OnRemoteRequest(code, data, reply, option);
        }));
}

void SessionStubImmersiveTest::TearDown()
{
    session_ = nullptr;
}

namespace {

/**
 * @tc.name: HandleGetAvoidAreaByTypeWithInvalidType
 * @tc.desc: GetAvoidAreaByType with invalid type
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubImmersiveTest, HandleGetAvoidAreaByTypeWithInvalidType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionStubImmersiveTest::HandleGetAvoidAreaByTypeWithInvalidType start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    data.WriteInterfaceToken(u"OHOS.ISession");
    data.WriteUint32(1111); // invalid type
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_AVOID_AREA);
    int ret = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, 5);
    GTEST_LOG_(INFO) << "SessionStubImmersiveTest::HandleGetAvoidAreaByTypeWithInvalidType end";
}

/**
 * @tc.name: HandleGetAvoidAreaByTypeWithSystemType
 * @tc.desc: GetAvoidAreaByType with system type
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubImmersiveTest, HandleGetAvoidAreaByTypeWithSystemType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionStubImmersiveTest::HandleGetAvoidAreaByTypeWithSystemType start";
    AvoidArea mockArea;
    mockArea.topRect_.width_ = 1200;
    mockArea.topRect_.height_ = 127;
    EXPECT_CALL(*session_, GetAvoidAreaByType(_, _, _)).WillOnce(Return(mockArea));

    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    data.WriteInterfaceToken(u"OHOS.ISession");
    data.WriteUint32(static_cast<uint32_t>(AvoidAreaType::TYPE_SYSTEM));
    WSRect rect = { 0, 0, 1200, 127 };
    data.WriteInt32(rect.posX_);
    data.WriteInt32(rect.posY_);
    data.WriteInt32(rect.width_);
    data.WriteInt32(rect.height_);
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_AVOID_AREA);
    int ret = session_->OnRemoteRequest(code, data, reply, option);

    ASSERT_EQ(ret, 0);
    sptr<AvoidArea> retArea = reply.ReadParcelable<AvoidArea>();
    ASSERT_TRUE(retArea != nullptr);
    ASSERT_EQ(retArea->topRect_.width_, 1200);
    ASSERT_EQ(retArea->topRect_.height_, 127);
    GTEST_LOG_(INFO) << "SessionStubImmersiveTest::HandleGetAvoidAreaByTypeWithSystemType end";
}

/**
 * @tc.name: HandleGetAllAvoidAreasNormal
 * @tc.desc: GetAllAvoidAreas return two Areas
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubImmersiveTest, HandleGetAllAvoidAreasNormal, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionStubImmersiveTest::HandleGetAllAvoidAreasNormal start";
    EXPECT_CALL(*session_, GetAllAvoidAreas(_))
        .WillOnce(Invoke([](std::map<AvoidAreaType, AvoidArea>& avoidAreas) -> WSError {
            AvoidArea mockArea;
            mockArea.topRect_.width_ = 1200;
            mockArea.topRect_.height_ = 127;
            avoidAreas[AvoidAreaType::TYPE_SYSTEM] = mockArea;

            AvoidArea indArea;
            indArea.bottomRect_.width_ = 500;
            indArea.bottomRect_.height_ = 10;
            avoidAreas[AvoidAreaType::TYPE_NAVIGATION_INDICATOR] = indArea;
            return WSError::WS_OK;
        }));

    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    data.WriteInterfaceToken(u"OHOS.ISession");
    data.WriteUint32(static_cast<uint32_t>(AvoidAreaType::TYPE_SYSTEM));
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_ALL_AVOID_AREAS);

    int ret = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, 0);

    uint32_t areasNum = reply.ReadUint32();
    ASSERT_EQ(areasNum, 2);
    for (uint32_t i = 0; i < 2; i++) {
        uint32_t type = reply.ReadUint32();
        ASSERT_TRUE((static_cast<AvoidAreaType>(type) == AvoidAreaType::TYPE_SYSTEM) ||
                    (static_cast<AvoidAreaType>(type) == AvoidAreaType::TYPE_NAVIGATION_INDICATOR));

        sptr<AvoidArea> area = reply.ReadParcelable<AvoidArea>();
        ASSERT_TRUE(area != nullptr);
    }
    uint32_t errCode = reply.ReadUint32();
    ASSERT_EQ(errCode, 0);
    GTEST_LOG_(INFO) << "SessionStubImmersiveTest::HandleGetAllAvoidAreasNormal end";
}

/**
 * @tc.name: HandleGetAllAvoidAreasEmpty
 * @tc.desc: GetAllAvoidAreas return empty
 * @tc.type: FUNC
 */
HWTEST_F(SessionStubImmersiveTest, HandleGetAllAvoidAreasEmpty, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionStubImmersiveTest::HandleGetAllAvoidAreasEmpty start";
    EXPECT_CALL(*session_, GetAllAvoidAreas(_)).WillOnce(Return(WSError::WS_OK));

    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    data.WriteInterfaceToken(u"OHOS.ISession");
    data.WriteUint32(static_cast<uint32_t>(AvoidAreaType::TYPE_SYSTEM));
    uint32_t code = static_cast<uint32_t>(SessionInterfaceCode::TRANS_ID_GET_ALL_AVOID_AREAS);

    int ret = session_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ret, 0);

    uint32_t areasNum = reply.ReadUint32();
    ASSERT_EQ(areasNum, 0);
    uint32_t errCode = reply.ReadUint32();
    ASSERT_EQ(errCode, 0);
    GTEST_LOG_(INFO) << "SessionStubImmersiveTest::HandleGetAllAvoidAreasEmpty end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS
