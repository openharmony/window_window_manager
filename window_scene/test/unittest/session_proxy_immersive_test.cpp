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

#include "iremote_object_mocker.h"
#include "session_ipc_interface_code.h"
#include "session_proxy.h"
#include "ws_common.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionProxyImmersiveTest : public testing::Test {
public:
    SessionProxyImmersiveTest() {}
    ~SessionProxyImmersiveTest() {}
};

namespace {
/**
 * @tc.name: GetAllAvoidAreasOnlyOne
 * @tc.desc: get one system avoid area
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyImmersiveTest, GetAllAvoidAreasOnlyOne, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAllAvoidAreasOnlyOne start";
    auto remoteObj = sptr<RemoteObjectMocker>::MakeSptr();
    EXPECT_CALL(*remoteObj, SendRequest(_, _, _, _))
        .WillOnce(Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) -> int {
            reply.WriteUint32(static_cast<uint32_t>(1)); // avoidArea num
            reply.WriteUint32(static_cast<uint32_t>(AvoidAreaType::TYPE_SYSTEM));
            AvoidArea tmpArea;
            tmpArea.topRect_ = { 0, 0, 1200, 127 };
            reply.WriteParcelable(&tmpArea);
            reply.WriteUint32(0); // error code
            return 0;
        }));

    auto sProxy = sptr<SessionProxy>::MakeSptr(remoteObj);
    std::map<AvoidAreaType, AvoidArea> avoidAreas;
    WSError ret = sProxy->GetAllAvoidAreas(avoidAreas);

    ASSERT_EQ(ret, WSError::WS_OK);
    ASSERT_EQ(1, avoidAreas.size());
    auto itrFind = avoidAreas.find(AvoidAreaType::TYPE_SYSTEM);
    ASSERT_TRUE(itrFind != avoidAreas.end());
    ASSERT_EQ(itrFind->second.topRect_.posX_, 0);
    ASSERT_EQ(itrFind->second.topRect_.posY_, 0);
    ASSERT_EQ(itrFind->second.topRect_.width_, 1200);
    ASSERT_EQ(itrFind->second.topRect_.height_, 127);
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAllAvoidAreasOnlyOne end";
}

/**
 * @tc.name: GetAllAvoidAreasRemoteErrorPara
 * @tc.desc: remote func return error code 1003
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyImmersiveTest, GetAllAvoidAreasRemoteErrorPara, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAllAvoidAreasRemoteErrorPara start";
    auto remoteObj = sptr<RemoteObjectMocker>::MakeSptr();
    EXPECT_CALL(*remoteObj, SendRequest(_, _, _, _)).WillOnce(Return(1003));

    auto sProxy = sptr<SessionProxy>::MakeSptr(remoteObj);
    std::map<AvoidAreaType, AvoidArea> avoidAreas;
    WSError ret = sProxy->GetAllAvoidAreas(avoidAreas);

    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);
    ASSERT_TRUE(avoidAreas.empty());
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAllAvoidAreasRemoteErrorPara end";
}

/**
 * @tc.name: GetAllAvoidAreasEmpty
 * @tc.desc: remote func return no avoid area
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyImmersiveTest, GetAllAvoidAreasEmpty, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAllAvoidAreasEmpty start";
    auto remoteObj = sptr<RemoteObjectMocker>::MakeSptr();
    EXPECT_CALL(*remoteObj, SendRequest(_, _, _, _))
        .WillOnce(Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) -> int {
            reply.WriteUint32(0); // avoidArea num
            reply.WriteUint32(0); // error code
            return 0;
        }));

    auto sProxy = sptr<SessionProxy>::MakeSptr(remoteObj);
    std::map<AvoidAreaType, AvoidArea> avoidAreas;
    WSError ret = sProxy->GetAllAvoidAreas(avoidAreas);

    ASSERT_EQ(ret, WSError::WS_OK);
    ASSERT_TRUE(avoidAreas.empty());
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAllAvoidAreasEmpty end";
}

/**
 * @tc.name: GetAllAvoidAreasWithInvalidAreaType
 * @tc.desc: remote func return invalid area type
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyImmersiveTest, GetAllAvoidAreasWithInvalidAreaType, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAllAvoidAreasWithInvalidAreaType start";
    auto remoteObj = sptr<RemoteObjectMocker>::MakeSptr();
    EXPECT_CALL(*remoteObj, SendRequest(_, _, _, _))
        .WillOnce(Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) -> int {
            reply.WriteUint32(1);    // avoidArea num
            reply.WriteUint32(1111); // invalid areaType
            AvoidArea tmpArea;
            tmpArea.topRect_ = { 0, 0, 1200, 127 };
            reply.WriteParcelable(&tmpArea);
            reply.WriteUint32(0); // error code
            return 0;
        }));

    auto sProxy = sptr<SessionProxy>::MakeSptr(remoteObj);
    std::map<AvoidAreaType, AvoidArea> avoidAreas;
    WSError ret = sProxy->GetAllAvoidAreas(avoidAreas);

    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAllAvoidAreasWithInvalidAreaType end";
}

/**
 * @tc.name: GetAllAvoidAreasWithInvalidArea
 * @tc.desc: remote func return invalid area type
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyImmersiveTest, GetAllAvoidAreasWithInvalidArea, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAllAvoidAreasWithInvalidArea start";
    auto remoteObj = sptr<RemoteObjectMocker>::MakeSptr();
    EXPECT_CALL(*remoteObj, SendRequest(_, _, _, _))
        .WillOnce(Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) -> int {
            reply.WriteUint32(1); // avoidArea num
            reply.WriteUint32(static_cast<uint32_t>(AvoidAreaType::TYPE_SYSTEM));
            reply.WriteUint32(2234); // invalid area
            reply.WriteUint32(0);    // error code
            return 0;
        }));

    auto sProxy = sptr<SessionProxy>::MakeSptr(remoteObj);
    std::map<AvoidAreaType, AvoidArea> avoidAreas;
    WSError ret = sProxy->GetAllAvoidAreas(avoidAreas);

    ASSERT_EQ(ret, WSError::WS_ERROR_IPC_FAILED);
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAllAvoidAreasWithInvalidArea end";
}

/**
 * @tc.name: GetAvoidAreaByTypeWithRemoteNull
 * @tc.desc: remote obj is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyImmersiveTest, GetAvoidAreaByTypeWithRemoteNull, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAvoidAreaByTypeWithRemoteNull start";
    auto sProxy = sptr<SessionProxy>::MakeSptr(nullptr);
    AvoidArea ret = sProxy->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM);
    ASSERT_TRUE(ret.isEmptyAvoidArea());
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAvoidAreaByTypeWithRemoteNull end";
}

/**
 * @tc.name: GetAvoidAreaByTypeWithRemoteErr
 * @tc.desc: remote func return error code
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyImmersiveTest, GetAvoidAreaByTypeWithRemoteErr, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAvoidAreaByTypeWithRemoteErr start";
    auto remoteObj = sptr<RemoteObjectMocker>::MakeSptr();
    EXPECT_CALL(*remoteObj, SendRequest(_, _, _, _)).WillOnce(Return(1003));

    auto sProxy = sptr<SessionProxy>::MakeSptr(remoteObj);
    AvoidArea ret = sProxy->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM);
    ASSERT_TRUE(ret.isEmptyAvoidArea());
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAvoidAreaByTypeWithRemoteErr end";
}

/**
 * @tc.name: GetAvoidAreaByTypeWithAreaInvalid
 * @tc.desc: remote func return invalid avoid area
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyImmersiveTest, GetAvoidAreaByTypeWithAreaInvalid, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAvoidAreaByTypeWithAreaInvalid start";
    auto remoteObj = sptr<RemoteObjectMocker>::MakeSptr();
    EXPECT_CALL(*remoteObj, SendRequest(_, _, _, _))
        .WillOnce(Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) -> int {
            reply.WriteUint32(2234); // invalid area
            return 0;
        }));

    auto sProxy = sptr<SessionProxy>::MakeSptr(remoteObj);
    AvoidArea ret = sProxy->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM);
    ASSERT_TRUE(ret.isEmptyAvoidArea());
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAvoidAreaByTypeWithAreaInvalid end";
}

/**
 * @tc.name: GetAvoidAreaByTypeWithAreaNormal
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyImmersiveTest, GetAvoidAreaByTypeWithAreaNormal, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAvoidAreaByTypeWithAreaNormal start";
    auto remoteObj = sptr<RemoteObjectMocker>::MakeSptr();
    EXPECT_CALL(*remoteObj, SendRequest(_, _, _, _))
        .WillOnce(Invoke([](uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) -> int {
            AvoidArea tmpArea;
            tmpArea.topRect_ = { 0, 0, 1200, 127 };
            reply.WriteParcelable(&tmpArea);
            return 0;
        }));

    auto sProxy = sptr<SessionProxy>::MakeSptr(remoteObj);
    AvoidArea ret = sProxy->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM);
    ASSERT_EQ(ret.topRect_.posX_, 0);
    ASSERT_EQ(ret.topRect_.posY_, 0);
    ASSERT_EQ(ret.topRect_.width_, 1200);
    ASSERT_EQ(ret.topRect_.height_, 127);
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAvoidAreaByTypeWithAreaNormal end";
}

/**
 * @tc.name: GetAvoidAreaByTypeEmpty
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyImmersiveTest, GetAvoidAreaByTypeEmpty, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAvoidAreaByTypeEmpty start";
    auto remoteObj = sptr<RemoteObjectMocker>::MakeSptr();
    auto sProxy = sptr<SessionProxy>::MakeSptr(remoteObj);
    AvoidArea res = sProxy->GetAvoidAreaByType(AvoidAreaType::TYPE_SYSTEM);
    AvoidArea area;
    ASSERT_EQ(res, area);
    GTEST_LOG_(INFO) << "SessionProxyImmersiveTest::GetAvoidAreaByTypeEmpty end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS
