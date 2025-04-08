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
#include <gmock/gmock.h>
#include "iremote_object_mocker.h"
#include "mock_session_manager_service.h"
#include "wm_common.h"
#include "scene_session_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MockSessionManagerServiceTest : public Test {
public :
    void SetUp() override;
    void TearDown() override;
};

void MockSessionManagerServiceTest::SetUp() {}

void MockSessionManagerServiceTest::TearDown() {}

class MockTest : public MockSessionManagerServiceTest {
public:
    MOCK_METHOD(sptr<IRemoteObject>, GetSceneSessionManagerByUserId, (int32_t), (override));
    MOCK_METHOD(int32_t, NotifySCBSnapshotSkipByUserIdAndBundleNames,
        (int32_t, const std::vector<std::string>&, const sptr<IRemoteObject>&), (override));
};

namespace {
/**
 * @tc.name: SetSnapshotSkipByUserIdAndBundleNamesInner
 * @tc.desc: test the core function of SetSnapshotSkipByUserIdAndBundleNames
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, SetSnapshotSkipByUserIdAndBundleNamesInner, TestSize.Level1)
{
    MockTest mockTest;
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr<>();
    EXPECT_CALL(mockTest, GetSceneSessionManagerByUserId(_))
        .Times(3)
        .WillOnce(Return(nullptr))
        .WillRepeatedly(Return(iRemoteObjectMocker));
    EXPECT_CALL(mockTest, NotifySCBSnapshotSkipByUserIdAndBundleNames(_, _, _))
        .Times(2)
        .WillOnce(Return(ERR_TRANSACTION_FAILED))
        .WillRepeatedly(Return(ERR_NONE));
    int32_t ret = mockTest.SetSnapshotSkipByUserIdAndBundleNamesInner(100, {"com.huawei.hmos.notepad"});
    ASSERT_EQ(ERR_NONE, ret);

    ret = mockTest.SetSnapshotSkipByUserIdAndBundleNamesInner(100, {"com.huawei.hmos.notepad"});
    ASSERT_EQ(ERR_TRANSACTION_FAILED, ret);

    ret = mockTest.SetSnapshotSkipByUserIdAndBundleNamesInner(100, {"com.huawei.hmos.notepad"});
    ASSERT_EQ(ERR_NONE, ret);
}

/**
 * @tc.name: SetSnapshotSkipByIdNamesMapInner
 * @tc.desc: test the core function of SetSnapshotSkipByIdNamesMap
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, SetSnapshotSkipByIdNamesMapInner, TestSize.Level1)
{
    MockTest mockTest;
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr<>();
    EXPECT_CALL(mockTest, GetSceneSessionManagerByUserId(_))
        .Times(3)
        .WillOnce(Return(nullptr))
        .WillRepeatedly(Return(iRemoteObjectMocker));
    EXPECT_CALL(mockTest, NotifySCBSnapshotSkipByUserIdAndBundleNames(_, _, _))
        .Times(2)
        .WillOnce(Return(ERR_TRANSACTION_FAILED))
        .WillRepeatedly(Return(ERR_NONE));
    int32_t ret = mockTest.SetSnapshotSkipByIdNamesMapInner({{100, {"com.huawei.hmos.notepad"}}});
    ASSERT_EQ(ERR_NONE, ret);

    ret = mockTest.SetSnapshotSkipByIdNamesMapInner({{100, {"com.huawei.hmos.notepad"}}});
    ASSERT_EQ(ERR_TRANSACTION_FAILED, ret);

    ret = mockTest.SetSnapshotSkipByIdNamesMapInner({{100, {"com.huawei.hmos.notepad"}}});
    ASSERT_EQ(ERR_NONE, ret);
}

/**
 * @tc.name: RecoverSCBSnapshotSkipByUserId
 * @tc.desc: RecoverSCBSnapshotSkipByUserId
 * @tc.type: FUNC
 */
HWTEST_F(MockSessionManagerServiceTest, RecoverSCBSnapshotSkipByUserId, TestSize.Level1)
{
    MockTest mockTest;
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr<>();
    EXPECT_CALL(mockTest, GetSceneSessionManagerByUserId(_))
        .Times(4)
        .WillOnce(Return(iRemoteObjectMocker))
        .WillOnce(Return(nullptr))
        .WillRepeatedly(Return(iRemoteObjectMocker));
    EXPECT_CALL(mockTest, NotifySCBSnapshotSkipByUserIdAndBundleNames(_, _, _))
        .Times(3)
        .WillOnce(Return(ERR_NONE))
        .WillOnce(Return(ERR_TRANSACTION_FAILED))
        .WillOnce(Return(ERR_NONE));
    int32_t ret = mockTest.SetSnapshotSkipByUserIdAndBundleNamesInner({{100, {"com.huawei.hmos.notepad"}}});
    ASSERT_EQ(ERR_NONE, ret);

    ret = mockTest.RecoverSCBSnapshotSkipByUserId(-1);
    ASSERT_EQ(ERR_INVALID_VALUE, ret);

    ret = mockTest.RecoverSCBSnapshotSkipByUserId(100);
    ASSERT_EQ(ERR_NULL_OBJECT, ret);

    ret = mockTest.RecoverSCBSnapshotSkipByUserId(100);
    ASSERT_EQ(ERR_TRANSACTION_FAILED, ret);

    ret = mockTest.RecoverSCBSnapshotSkipByUserId(100);
    ASSERT_EQ(ERR_NONE, ret);
}
}
}
}