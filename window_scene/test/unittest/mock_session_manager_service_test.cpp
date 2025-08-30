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

/**
 * @tc.name: SetSessionManagerService
 * @tc.desc: test the function of SetSessionManagerService
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, SetSessionManagerService, TestSize.Level1)
{
    MockMockSessionManagerService mockMockSms;
    sptr<IRemoteObject> nullSessionManagerService = nullptr;
    bool result = mockMockSms.SetSessionManagerService(nullSessionManagerService);
    EXPECT_EQ(result, false);

    sptr<IRemoteObject> mockSessionManagerService = sptr<IRemoteObjectMocker>::MakeSptr();
    // Mock GetUserIdByCallingUid() 返回非法用户id -1
    result = SetSessionManagerService(mockSessionManagerService);
    EXPECT_EQ(resultCode, false);

    // Mock GetUserIdByCallingUid() 返回合法用户id 100
    result = SetSessionManagerService(mockSessionManagerService);
    EXPECT_EQ(resultCode, true);
}

/**
 * @tc.name: GetSessionManagerService
 * @tc.desc: test the function of GetSessionManagerService
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, GetSessionManagerService, TestSize.Level1)
{
    sptr<IRemoteObject> sessionManagerService;
    // 1. Mock GetUserIdByCallingUid() 返回非法用户id -1
    ErrCode resultCode = GetSessionManagerService(sessionManagerService);
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // 1. Mock GetUserIdByCallingUid() 返回系统用户id 0
    ErrCode resultCode = GetSessionManagerService(sessionManagerService);
    EXPECT_EQ(resultCode, ERR_OK);
}

/**
 * @tc.name: GetSessionManagerServiceByUserId
 * @tc.desc: test the function of GetSessionManagerServiceByUserId
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, GetSessionManagerServiceByUserId, TestSize.Level1)
{
    int32_t userId = 100;
    sptr<IRemoteObject> sessionManagerService;
    // 1. Mock GetUserIdByCallingUid() 返回非法用户id -1
    ErrCode resultCode = GetSessionManagerServiceByUserId(userId, sessionManagerService);
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // 2. Mock GetUserIdByCallingUid() 返回非系统用户id 200
    ErrCode resultCode = GetSessionManagerServiceByUserId(userId, sessionManagerService);
    EXPECT_EQ(resultCode, ERR_WOULD_BLOCK);


    // 2. Mock GetUserIdByCallingUid() 返回系统用户id 0
    ErrCode resultCode = GetSessionManagerServiceByUserId(userId, sessionManagerService);
    EXPECT_EQ(resultCode, ERR_OK);
}

/**
 * @tc.name: RegisterSMSRecoverListener
 * @tc.desc: test the function of RegisterSMSRecoverListener
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, RegisterSMSRecoverListener, TestSize.Level1)
{
    sptr<IRemoteObject> listener = nullptr;
    int32_t userId = 100;
    // 1. listener 为 nullptr
    ErrCode resultCode = RegisterSMSRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // 2.Mock GetUserIdByCallingUid() 返回系统用户id 0
    SetMockUserId(0);
    listener = sptr<IRemoteObjectMocker>::MakeSptr();
    userId = INVALID_USER_ID;
    resultCode = RegisterSMSRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, ERR_OK);
    

    SetMockUserId(0);
    listener = sptr<IRemoteObjectMocker>::MakeSptr();
    userId = 100;
    resultCode = RegisterSMSRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, ERR_OK);
}

/**
 * @tc.name: UnregisterSMSRecoverListener
 * @tc.desc: test the function of UnregisterSMSRecoverListener
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, UnregisterSMSRecoverListener, TestSize.Level1)
{
    int32_t userId = INVALID_USER_ID;
    // 1.Mock GetUserIdByCallingUid() 返回系统用户id 0
    SetMockUserId(0);
    ErrCode resultCode = UnregisterSMSRecoverListener(userId);
    EXPECT_EQ(resultCode, ERR_OK);

    SetMockUserId(0);
    userId = 100;
    resultCode = UnregisterSMSRecoverListener(userId);
    EXPECT_EQ(resultCode, ERR_OK);
}

/**
 * @tc.name: RegisterSMSLiteRecoverListener
 * @tc.desc: test the function of RegisterSMSLiteRecoverListener
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, RegisterSMSLiteRecoverListener, TestSize.Level1)
{
    sptr<IRemoteObject> listener = nullptr;
    int32_t userId = 100;
    // 1. listener 为 nullptr
    ErrCode resultCode = RegisterSMSLiteRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // 2.Mock GetUserIdByCallingUid() 返回系统用户id 0
    SetMockUserId(0);
    listener = sptr<IRemoteObjectMocker>::MakeSptr();
    userId = INVALID_USER_ID;
    resultCode = RegisterSMSLiteRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, ERR_OK);
    

    SetMockUserId(0);
    listener = sptr<IRemoteObjectMocker>::MakeSptr();
    userId = 100;
    resultCode = RegisterSMSLiteRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, ERR_OK);
}

/**
 * @tc.name: UnregisterSMSLiteRecoverListener
 * @tc.desc: test the function of UnregisterSMSLiteRecoverListener
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, UnregisterSMSLiteRecoverListener, TestSize.Level1)
{
    int32_t userId = INVALID_USER_ID;
    // 1.Mock GetUserIdByCallingUid() 返回系统用户id 0
    SetMockUserId(0);
    ErrCode resultCode = UnregisterSMSLiteRecoverListener(userId);
    EXPECT_EQ(resultCode, ERR_OK);

    SetMockUserId(0);
    userId = 100;
    resultCode = UnregisterSMSLiteRecoverListener(userId);
    EXPECT_EQ(resultCode, ERR_OK);
}

/**
 * @tc.name: NotifySceneBoardAvailable
 * @tc.desc: test the function of NotifySceneBoardAvailable
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifySceneBoardAvailable, TestSize.Level1)
{
    // 1.Mock IsSystemCalling返回fasle
    ErrCode resultCode = NotifySceneBoardAvailable();
    EXPECT_EQ(resultCode, ERR_PERMISSION_DENIED);

    // 2. Mock IsSystemCalling返回true
    // Mock GetUserIdByCallingUid() 返回非法用户-1
    resultCode = NotifySceneBoardAvailable();
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // 2. Mock IsSystemCalling返回true
    // Mock GetUserIdByCallingUid() 返回合法用户100
    resultCode = NotifySceneBoardAvailable();
    EXPECT_EQ(resultCode, ERR_OK);
}
}

/**
 * @tc.name: NotifySceneBoardAvailableToSystemAppClient
 * @tc.desc: test the function of NotifySceneBoardAvailableToSystemAppClient
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifySceneBoardAvailableToSystemAppClient, TestSize.Level1)
{
    int32_t userId = 100;
    NotifySceneBoardAvailableToSystemAppClient(userId);
}

/**
 * @tc.name: NotifySceneBoardAvailableToClient
 * @tc.desc: test the function of NotifySceneBoardAvailableToClient
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifySceneBoardAvailableToClient, TestSize.Level1)
{
    int32_t userId = 100;
    NotifySceneBoardAvailableToClient(userId);
}

/**
 * @tc.name: NotifySceneBoardAvailableToSystemAppLiteClient
 * @tc.desc: test the function of NotifySceneBoardAvailableToSystemAppLiteClient
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifySceneBoardAvailableToSystemAppLiteClient, TestSize.Level1)
{
    int32_t userId = 100;
    NotifySceneBoardAvailableToSystemAppLiteClient(userId);
}

/**
 * @tc.name: NotifySceneBoardAvailableToLiteClient
 * @tc.desc: test the function of NotifySceneBoardAvailableToLiteClient
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifySceneBoardAvailableToLiteClient, TestSize.Level1)
{
    int32_t userId = 100;
    NotifySceneBoardAvailableToLiteClient(userId);
}

/**
 * @tc.name: NotifyWMSConnected
 * @tc.desc: test the function of NotifyWMSConnected
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifyWMSConnected, TestSize.Level1)
{
    int32_t userId = 100;
    int32_t screenId = 0;
    bool isColdStart = false;
    NotifyWMSConnected(userId, screenId, isColdStart);
}

/**
 * @tc.name: NotifyWMSConnectionChangedToClient
 * @tc.desc: test the function of NotifyWMSConnectionChangedToClient
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifyWMSConnectionChangedToClient, TestSize.Level1)
{
    int32_t wmsUserId = 100;
    int32_t screenId = 0;
    bool isConnnected = true;
    NotifyWMSConnectionChangedToClient(wmsUserId, screenId, isConnnected);
}

/**
 * @tc.name: NotifyWMSConnectionChangedToLiteClient
 * @tc.desc: test the function of NotifyWMSConnectionChangedToLiteClient
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifyWMSConnectionChangedToLiteClient, TestSize.Level1)
{
    int32_t wmsUserId = 100;
    int32_t screenId = 0;
    bool isConnnected = true;
    NotifyWMSConnectionChangedToLiteClient(wmsUserId, screenId, isConnnected);
}


/**
 * @tc.name: GetSceneSessionManagerCommon
 * @tc.desc: test the function of GetSceneSessionManagerCommon
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, GetSceneSessionManagerCommon, TestSize.Level1)
{
    sptr<IRemoteObject> result;
    bool isLite = false;
    // Mock GetUserIdByCallingUid() 返回非法用户id -1
    SetMockUserId(-1); 
    ErrCode resultCode = MockSessionManagerService::GetInstance().GetSceneSessionManagerCommon(100, result, isLite);
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // Mock GetUserIdByCallingUid() 返回非系统用户id 100
    SetMockUserId(100); 
    ErrCode resultCode = GetSceneSessionManagerCommon(200, result, isLite);
    EXPECT_EQ(resultCode, ERR_WOULD_BLOCK);

    // Mock GetUserIdByCallingUid() 返回系统用户id 0
    SetMockUserId(0); 
    // Mock GetSessionManagerServiceByUserId() 返回nullptr 或者 传入不存在userId 99999
    ErrCode resultCode = GetSceneSessionManagerCommon(99999, result, isLite);
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // Mock GetUserIdByCallingUid() 返回系统用户id 0
    SetMockUserId(0);
    // Mock GetSessionManagerServiceByUserId() 返回有效指针
    // Mock iface_cast 返回nullptr
    ErrCode resultCode = GetSceneSessionManagerCommon(100, result, isLite);
    EXPECT_EQ(resultCode, ERR_DEAD_OBJECT);

    isLite = false;
    // Mock GetUserIdByCallingUid() 返回系统用户id 0
    SetMockUserId(0);
    // Mock GetSessionManagerServiceByUserId() 返回有效指针
    // Mock iface_cast 返回有效代理
    ErrCode resultCode = GetSceneSessionManagerCommon(100, result, isLite);
    EXPECT_EQ(resultCode, ERR_OK);
    EXPECT_NQ(result, nullptr);

    isLite = true;
    // Mock GetUserIdByCallingUid() 返回系统用户id 0
    SetMockUserId(0);
    // Mock GetSessionManagerServiceByUserId() 返回有效指针
    // Mock iface_cast 返回有效代理
    ErrCode resultCode = GetSceneSessionManagerCommon(100, result, isLite);
    EXPECT_EQ(resultCode, ERR_OK);
    EXPECT_NQ(result, nullptr);

    isLite = false;
    // Mock GetUserIdByCallingUid() 返回系统用户id 0
    SetMockUserId(0);
    // Mock GetSessionManagerServiceByUserId() 返回有效指针
    // Mock iface_cast 返回有效代理
    // Mock GetSceneSessionManager() 返回nullptr
    ErrCode resultCode = GetSceneSessionManagerCommon(100, result, isLite);
    EXPECT_EQ(resultCode, ERR_DEAD_OBJECT);
}

} // namespace
} // namespace Rosen
} // namespace OHOSHOPEFU