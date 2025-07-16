   /*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "mock_session_manager_service.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "iremote_object_mocker.h"

#include "scene_session_manager.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MockMockSessionManagerService : public MockSessionManagerService {
public:
    MOCK_METHOD(sptr<IRemoteObject>, GetSceneSessionManagerByUserId, (int32_t), (override));
    MOCK_METHOD(ErrCode,
                NotifySCBSnapshotSkipByUserIdAndBundleNames,
                (int32_t, const std::vector<std::string>&, const sptr<IRemoteObject>&),
                (override));
};

namespace {
/**
 * @tc.name: SetSnapshotSkipByUserIdAndBundleNamesInner
 * @tc.desc: test the core function of SetSnapshotSkipByUserIdAndBundleNames
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, SetSnapshotSkipByUserIdAndBundleNamesInner, TestSize.Level1)
{
    MockMockSessionManagerService mockMockSms;
    sptr<IRemoteObject> mockRemoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    EXPECT_CALL(mockMockSms, GetSceneSessionManagerByUserId(_))
        .Times(3)
        .WillOnce(Return(nullptr))
        .WillRepeatedly(Return(mockRemoteObject));
    EXPECT_CALL(mockMockSms, NotifySCBSnapshotSkipByUserIdAndBundleNames(_, _, _))
        .Times(2)
        .WillOnce(Return(ERR_INVALID_OPERATION))
        .WillRepeatedly(Return(ERR_OK));

    ErrCode ret = mockMockSms.SetSnapshotSkipByUserIdAndBundleNamesInner(100, { "notepad" });
    EXPECT_EQ(ERR_OK, ret);

    ret = mockMockSms.SetSnapshotSkipByUserIdAndBundleNamesInner(100, { "notepad" });
    EXPECT_EQ(ERR_INVALID_OPERATION, ret);

    ret = mockMockSms.SetSnapshotSkipByUserIdAndBundleNamesInner(100, { "notepad" });
    EXPECT_EQ(ERR_OK, ret);
}

/**
 * @tc.name: SetSnapshotSkipByIdNamesMapInner
 * @tc.desc: test the core function of SetSnapshotSkipByIdNamesMap
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, SetSnapshotSkipByIdNamesMapInner, TestSize.Level1)
{
    MockMockSessionManagerService mockMockSms;
    sptr<IRemoteObject> mockRemoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    EXPECT_CALL(mockMockSms, GetSceneSessionManagerByUserId(_))
        .Times(3)
        .WillOnce(Return(nullptr))
        .WillRepeatedly(Return(mockRemoteObject));
    EXPECT_CALL(mockMockSms, NotifySCBSnapshotSkipByUserIdAndBundleNames(_, _, _))
        .Times(2)
        .WillOnce(Return(ERR_INVALID_OPERATION))
        .WillRepeatedly(Return(ERR_OK));

    int32_t ret = mockMockSms.SetSnapshotSkipByIdNamesMapInner({ { 100, { "notepad" } } });
    EXPECT_EQ(ERR_OK, ret);

    ret = mockMockSms.SetSnapshotSkipByIdNamesMapInner({ { 100, { "notepad" } } });
    EXPECT_EQ(ERR_INVALID_OPERATION, ret);

    ret = mockMockSms.SetSnapshotSkipByIdNamesMapInner({ { 100, { "notepad" } } });
    EXPECT_EQ(ERR_OK, ret);
}

/**
 * @tc.name: RecoverSCBSnapshotSkipByUserId
 * @tc.desc: test the function of RecoverSCBSnapshotSkipByUserId
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, RecoverSCBSnapshotSkipByUserId, TestSize.Level1)
{
    MockMockSessionManagerService mockMockSms;
    sptr<IRemoteObject> mockRemoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    EXPECT_CALL(mockMockSms, GetSceneSessionManagerByUserId(_))
        .Times(4)
        .WillOnce(Return(mockRemoteObject))
        .WillOnce(Return(nullptr))
        .WillRepeatedly(Return(mockRemoteObject));
    EXPECT_CALL(mockMockSms, NotifySCBSnapshotSkipByUserIdAndBundleNames(_, _, _))
        .Times(3)
        .WillOnce(Return(ERR_OK))
        .WillOnce(Return(ERR_INVALID_OPERATION))
        .WillOnce(Return(ERR_OK));

    ErrCode ret = mockMockSms.SetSnapshotSkipByUserIdAndBundleNamesInner(100, { "notepad" });
    EXPECT_EQ(ERR_OK, ret);

    ret = mockMockSms.RecoverSCBSnapshotSkipByUserId(-1);
    EXPECT_EQ(ERR_INVALID_VALUE, ret);

    ret = mockMockSms.RecoverSCBSnapshotSkipByUserId(100);
    EXPECT_EQ(ERR_DEAD_OBJECT, ret);

    ret = mockMockSms.RecoverSCBSnapshotSkipByUserId(100);
    EXPECT_EQ(ERR_INVALID_OPERATION, ret);

    ret = mockMockSms.RecoverSCBSnapshotSkipByUserId(100);
    EXPECT_EQ(ERR_OK, ret);
}
} // namespace
} // namespace Rosen
} // namespace OHOS