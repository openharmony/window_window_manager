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


#define private public
#define protected public
#include "mock_session_manager_service.h"
#undef private
#undef protected

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "iremote_object_mocker.h"

#include "scene_session_manager.h"
#include "session_manager_service_proxy.h"
#include "mock_accesstoken_kit.h"
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
    MOCK_METHOD(int32_t, GetUserIdByCallingUid, (), (override));
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
 * @tc.name: SetSessionManagerServiceFailed
 * @tc.desc: test the function of SetSessionManagerServiceFailed
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, SetSessionManagerServiceFailed, TestSize.Level1)
{
    MockMockSessionManagerService mockMockSms;
    sptr<IRemoteObject> nullSessionManagerService = nullptr;
    bool result = mockMockSms.SetSessionManagerService(nullSessionManagerService);
    EXPECT_EQ(result, false);

    sptr<IRemoteObject> mockSessionManagerService = sptr<IRemoteObjectMocker>::MakeSptr();
    // Mock GetUserIdByCallingUid() 返回非法用户id -1
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(-1));
    result = mockMockSms.SetSessionManagerService(mockSessionManagerService);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: GetSessionManagerService
 * @tc.desc: test the function of GetSessionManagerService
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, GetSessionManagerService, TestSize.Level1)
{
    sptr<IRemoteObject> sessionManagerService;
    MockMockSessionManagerService mockMockSms;
    // 1. Mock GetUserIdByCallingUid() 返回非法用户id -1
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(-1));
    ErrCode resultCode = mockMockSms.GetSessionManagerService(sessionManagerService);
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // 1. Mock GetUserIdByCallingUid() 返回系统用户id 0
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    resultCode = mockMockSms.GetSessionManagerService(sessionManagerService);
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
    MockMockSessionManagerService mockMockSms;
    sptr<IRemoteObject> sessionManagerService;
    // 1. Mock GetUserIdByCallingUid() 返回非法用户id -1
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(-1));
    ErrCode resultCode = mockMockSms.GetSessionManagerServiceByUserId(userId, sessionManagerService);
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // 2. Mock GetUserIdByCallingUid() 返回非系统用户id 200
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(200));
    resultCode = mockMockSms.GetSessionManagerServiceByUserId(userId, sessionManagerService);
    EXPECT_EQ(resultCode, ERR_WOULD_BLOCK);

    // 2. Mock GetUserIdByCallingUid() 返回系统用户id 0
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    resultCode = mockMockSms.GetSessionManagerServiceByUserId(userId, sessionManagerService);
    EXPECT_EQ(resultCode, ERR_OK);
}

/**
 * @tc.name: RegisterSMSRecoverListener
 * @tc.desc: test the function of RegisterSMSRecoverListener
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, RegisterSMSRecoverListener, TestSize.Level1)
{
    MockMockSessionManagerService mockMockSms;
    sptr<IRemoteObject> listener = nullptr;
    int32_t userId = 100;
    // 1. listener 为 nullptr
    ErrCode resultCode = mockMockSms.RegisterSMSRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // 2.Mock GetUserIdByCallingUid() 返回系统用户id 0
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    listener = sptr<IRemoteObjectMocker>::MakeSptr();
    userId = INVALID_USER_ID;
    resultCode = mockMockSms.RegisterSMSRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, ERR_OK);
    
    // 3.Mock GetUserIdByCallingUid() 返回系统用户id 0
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    listener = sptr<IRemoteObjectMocker>::MakeSptr();
    userId = 100;
    resultCode = mockMockSms.RegisterSMSRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, 4194312);

    // 4.Mock GetUserIdByCallingUid() 返回非系统用户id 100
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(100));
    listener = sptr<IRemoteObjectMocker>::MakeSptr();
    userId = INVALID_USER_ID;
    resultCode = mockMockSms.RegisterSMSRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, ERR_WOULD_BLOCK);
}

/**
 * @tc.name: UnregisterSMSRecoverListener
 * @tc.desc: test the function of UnregisterSMSRecoverListener
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, UnregisterSMSRecoverListener, TestSize.Level1)
{
    MockMockSessionManagerService mockMockSms;
    // 1.Mock GetUserIdByCallingUid() 返回系统用户id 0
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    int32_t userId = INVALID_USER_ID;
    ErrCode resultCode = mockMockSms.UnregisterSMSRecoverListener(userId);
    EXPECT_EQ(resultCode, ERR_OK);

    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>> systemAppSmsRecoverListenerMap;
    systemAppSmsRecoverListenerMap[999090] = nullptr;
    mockMockSms.systemAppSmsRecoverListenerMap_[0] = systemAppSmsRecoverListenerMap;
    resultCode = mockMockSms.UnregisterSMSRecoverListener(userId);
    EXPECT_EQ(resultCode, ERR_OK);

    // 2.Mock GetUserIdByCallingUid() 返回系统用户id 0
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    userId = 100;
    resultCode = mockMockSms.UnregisterSMSRecoverListener(userId);
    EXPECT_EQ(resultCode, 4194312);

    // 3.Mock GetUserIdByCallingUid() 返回非系统用户id 100
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(100));
    userId = INVALID_USER_ID;
    resultCode = mockMockSms.UnregisterSMSRecoverListener(userId);
    EXPECT_EQ(resultCode, ERR_OK);
}

/**
 * @tc.name: RegisterSMSLiteRecoverListener
 * @tc.desc: test the function of RegisterSMSLiteRecoverListener
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, RegisterSMSLiteRecoverListener, TestSize.Level1)
{
    MockMockSessionManagerService mockMockSms;
    sptr<IRemoteObject> listener = nullptr;
    int32_t userId = 100;
    // 1. listener 为 nullptr
    ErrCode resultCode = mockMockSms.RegisterSMSLiteRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // 2.Mock GetUserIdByCallingUid() 返回系统用户id 0
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    listener = sptr<IRemoteObjectMocker>::MakeSptr();
    userId = INVALID_USER_ID;
    resultCode = mockMockSms.RegisterSMSLiteRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, ERR_OK);
    
    // 3.Mock GetUserIdByCallingUid() 返回系统用户id 0
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    listener = sptr<IRemoteObjectMocker>::MakeSptr();
    userId = 100;
    resultCode = mockMockSms.RegisterSMSLiteRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, 4194312);

    // 4.Mock GetUserIdByCallingUid() 返回非系统用户id 100
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(100));
    listener = sptr<IRemoteObjectMocker>::MakeSptr();
    userId = INVALID_USER_ID;
    resultCode = mockMockSms.RegisterSMSLiteRecoverListener(listener, userId);
    EXPECT_EQ(resultCode, ERR_WOULD_BLOCK);
}

/**
 * @tc.name: UnregisterSMSLiteRecoverListener
 * @tc.desc: test the function of UnregisterSMSLiteRecoverListener
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, UnregisterSMSLiteRecoverListener, TestSize.Level1)
{
    MockMockSessionManagerService mockMockSms;
    // 1.Mock GetUserIdByCallingUid() 返回系统用户id 0
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    int32_t userId = INVALID_USER_ID;
    ErrCode resultCode = mockMockSms.UnregisterSMSLiteRecoverListener(userId);
    EXPECT_EQ(resultCode, ERR_OK);

    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>> systemAppSmsLiteRecoverListenerMap;
    systemAppSmsLiteRecoverListenerMap[999090] = nullptr;
    mockMockSms.systemAppSmsLiteRecoverListenerMap_[0] = systemAppSmsLiteRecoverListenerMap;
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    resultCode = mockMockSms.UnregisterSMSLiteRecoverListener(userId);
    EXPECT_EQ(resultCode, ERR_OK);

    // 2.Mock GetUserIdByCallingUid() 返回系统用户id 0
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    userId = 100;
    resultCode = mockMockSms.UnregisterSMSLiteRecoverListener(userId);
    EXPECT_EQ(resultCode, 4194312);

    // 3.Mock GetUserIdByCallingUid() 返回非系统用户id 100
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(100));
    userId = INVALID_USER_ID;
    resultCode = mockMockSms.UnregisterSMSLiteRecoverListener(userId);
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
    MockMockSessionManagerService mockMockSms;
    MockAccesstokenKit::MockIsSystemApp(false);
    MockAccesstokenKit::MockIsSACalling(false);
    ErrCode resultCode = mockMockSms.NotifySceneBoardAvailable();
    EXPECT_EQ(resultCode, ERR_PERMISSION_DENIED);

    // 2. Mock IsSystemCalling返回true
    MockAccesstokenKit::MockIsSystemApp(true);
    MockAccesstokenKit::MockIsSACalling(true);
    // Mock GetUserIdByCallingUid() 返回非法用户-1
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(-1));
    resultCode = mockMockSms.NotifySceneBoardAvailable();
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // 2. Mock IsSystemCalling返回true
    MockAccesstokenKit::MockIsSystemApp(true);
    MockAccesstokenKit::MockIsSACalling(true);
    // Mock GetUserIdByCallingUid() 返回合法用户100
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(100));
    resultCode = mockMockSms.NotifySceneBoardAvailable();
    EXPECT_EQ(resultCode, ERR_OK);
}

/**
 * @tc.name: NotifySceneBoardAvailableToSystemAppClient
 * @tc.desc: test the function of NotifySceneBoardAvailableToSystemAppClient
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifySceneBoardAvailableToSystemAppClient, TestSize.Level1)
{
    int32_t userId = 100;
    MockMockSessionManagerService mockMockSms;
    mockMockSms.NotifySceneBoardAvailableToSystemAppClient(userId);

    mockMockSms.userId2ScreenIdMap_[100] = 0;
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>> systemAppSmsRecoverListenerMap;
    systemAppSmsRecoverListenerMap[999090] = nullptr;
    mockMockSms.systemAppSmsRecoverListenerMap_[0] = systemAppSmsRecoverListenerMap;
    mockMockSms.NotifySceneBoardAvailableToSystemAppClient(userId);

    mockMockSms.sessionManagerServiceMap_[100] = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<IRemoteObject> mockRemoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    systemAppSmsRecoverListenerMap[999090] = iface_cast<ISessionManagerServiceRecoverListener>(mockRemoteObject);
    mockMockSms.systemAppSmsRecoverListenerMap_[0] = systemAppSmsRecoverListenerMap;
    mockMockSms.NotifySceneBoardAvailableToSystemAppClient(userId);
}

/**
 * @tc.name: NotifySceneBoardAvailableToClient
 * @tc.desc: test the function of NotifySceneBoardAvailableToClient
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifySceneBoardAvailableToClient, TestSize.Level1)
{
    int32_t userId = 100;
    MockMockSessionManagerService mockMockSms;
    mockMockSms.NotifySceneBoardAvailableToClient(userId);

    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>> smsRecoverListenerMap;
    smsRecoverListenerMap[999090] = nullptr;
    mockMockSms.smsRecoverListenerMap_[userId] = smsRecoverListenerMap;
    mockMockSms.NotifySceneBoardAvailableToClient(userId);

    mockMockSms.sessionManagerServiceMap_[100] = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<IRemoteObject> mockRemoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    smsRecoverListenerMap[999090] = iface_cast<ISessionManagerServiceRecoverListener>(mockRemoteObject);
    mockMockSms.smsRecoverListenerMap_[userId] = smsRecoverListenerMap;
    mockMockSms.NotifySceneBoardAvailableToClient(userId);
}

/**
 * @tc.name: NotifySceneBoardAvailableToSystemAppLiteClient
 * @tc.desc: test the function of NotifySceneBoardAvailableToSystemAppLiteClient
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifySceneBoardAvailableToSystemAppLiteClient, TestSize.Level1)
{
    int32_t userId = 100;
    MockMockSessionManagerService mockMockSms;
    mockMockSms.NotifySceneBoardAvailableToSystemAppLiteClient(userId);

    mockMockSms.userId2ScreenIdMap_[100] = 0;
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>> systemAppSmsLiteRecoverListenerMap;
    systemAppSmsLiteRecoverListenerMap[999090] = nullptr;
    mockMockSms.systemAppSmsLiteRecoverListenerMap_[0] = systemAppSmsLiteRecoverListenerMap;
    mockMockSms.NotifySceneBoardAvailableToSystemAppLiteClient(userId);

    mockMockSms.sessionManagerServiceMap_[100] = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<IRemoteObject> mockRemoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    systemAppSmsLiteRecoverListenerMap[999090] = iface_cast<ISessionManagerServiceRecoverListener>(mockRemoteObject);
    mockMockSms.systemAppSmsLiteRecoverListenerMap_[0] = systemAppSmsLiteRecoverListenerMap;
    mockMockSms.NotifySceneBoardAvailableToSystemAppLiteClient(userId);
}

/**
 * @tc.name: NotifySceneBoardAvailableToLiteClient
 * @tc.desc: test the function of NotifySceneBoardAvailableToLiteClient
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifySceneBoardAvailableToLiteClient, TestSize.Level1)
{
    int32_t userId = 100;
    MockMockSessionManagerService mockMockSms;
    mockMockSms.NotifySceneBoardAvailableToLiteClient(userId);

    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>> smsLiteRecoverListenerMap;
    smsLiteRecoverListenerMap[999090] = nullptr;
    mockMockSms.smsLiteRecoverListenerMap_[userId] = smsLiteRecoverListenerMap;
    mockMockSms.NotifySceneBoardAvailableToLiteClient(userId);

    mockMockSms.sessionManagerServiceMap_[100] = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<IRemoteObject> mockRemoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    smsLiteRecoverListenerMap[999090] = iface_cast<ISessionManagerServiceRecoverListener>(mockRemoteObject);
    mockMockSms.smsLiteRecoverListenerMap_[userId] = smsLiteRecoverListenerMap;
    mockMockSms.NotifySceneBoardAvailableToLiteClient(userId);
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
    MockMockSessionManagerService mockMockSms;
    mockMockSms.NotifyWMSConnected(userId, screenId, isColdStart);
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
    MockMockSessionManagerService mockMockSms;
    mockMockSms.NotifyWMSConnectionChangedToClient(wmsUserId, screenId, isConnnected);

    mockMockSms.userId2ScreenIdMap_[100] = 0;
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>> systemAppSmsRecoverListenerMap;
    systemAppSmsRecoverListenerMap[999090] = nullptr;
    mockMockSms.systemAppSmsRecoverListenerMap_[0] = systemAppSmsRecoverListenerMap;
    mockMockSms.NotifyWMSConnectionChangedToClient(wmsUserId, screenId, isConnnected);

    mockMockSms.sessionManagerServiceMap_[100] = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<IRemoteObject> mockRemoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    systemAppSmsRecoverListenerMap[999090] = iface_cast<ISessionManagerServiceRecoverListener>(mockRemoteObject);
    mockMockSms.systemAppSmsRecoverListenerMap_[0] = systemAppSmsRecoverListenerMap;
    mockMockSms.NotifyWMSConnectionChangedToClient(wmsUserId, screenId, isConnnected);
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
    MockMockSessionManagerService mockMockSms;
    mockMockSms.NotifyWMSConnectionChangedToLiteClient(wmsUserId, screenId, isConnnected);

    mockMockSms.userId2ScreenIdMap_[100] = 0;
    std::map<int32_t, sptr<ISessionManagerServiceRecoverListener>> systemAppSmsLiteRecoverListenerMap;
    systemAppSmsLiteRecoverListenerMap[999090] = nullptr;
    mockMockSms.systemAppSmsLiteRecoverListenerMap_[0] = systemAppSmsLiteRecoverListenerMap;
    mockMockSms.NotifyWMSConnectionChangedToLiteClient(wmsUserId, screenId, isConnnected);

    mockMockSms.sessionManagerServiceMap_[100] = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<IRemoteObject> mockRemoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    systemAppSmsLiteRecoverListenerMap[999090] = iface_cast<ISessionManagerServiceRecoverListener>(mockRemoteObject);
    mockMockSms.systemAppSmsLiteRecoverListenerMap_[0] = systemAppSmsLiteRecoverListenerMap;
    mockMockSms.NotifyWMSConnectionChangedToLiteClient(wmsUserId, screenId, isConnnected);
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
    MockMockSessionManagerService mockMockSms;
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(-1));
    ErrCode resultCode = mockMockSms.GetSceneSessionManagerCommon(100, result, isLite);
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);

    // Mock GetUserIdByCallingUid() 返回非系统用户id 100
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(100));
    resultCode = mockMockSms.GetSceneSessionManagerCommon(200, result, isLite);
    EXPECT_EQ(resultCode, ERR_WOULD_BLOCK);

    // Mock GetUserIdByCallingUid() 返回系统用户id 0
    EXPECT_CALL(mockMockSms, GetUserIdByCallingUid())
        .Times(1)
        .WillOnce(Return(0));
    // Mock GetSessionManagerServiceByUserId() 返回nullptr 或者 传入不存在userId 99999
    resultCode = mockMockSms.GetSceneSessionManagerCommon(99999, result, isLite);
    EXPECT_EQ(resultCode, ERR_INVALID_VALUE);
}

/**
 * @tc.name: DefaultSceneSessionManager
 * @tc.desc: test the function of DefaultSceneSessionManager
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, DefaultSceneSessionManager, TestSize.Level1)
{
    MockMockSessionManagerService mockMockSms;
    mockMockSms.defaultWMSUserId_ = 100;
    mockMockSms.sessionManagerServiceMap_[100] = sptr<IRemoteObjectMocker>::MakeSptr();
    mockMockSms.defaultSceneSessionManager_ = sptr<IRemoteObjectMocker>::MakeSptr();

    std::vector<std::string> args = {"arg1"};
    std::string info = "info";
    std::string& dumpInfo = info;
    mockMockSms.DumpSessionInfo(args, dumpInfo);
    std::vector<uint64_t> windowIdList = {111, 222};
    std::vector<uint64_t> surfaceNodeIds = {111, 222};
    std::vector<int32_t> persistentIds = {111, 222};
    std::vector<string> privacyWindowTags = {"tag1", "tag2"};
    mockMockSms.GetProcessSurfaceNodeIdByPersistentId(123, windowIdList, surfaceNodeIds);
    mockMockSms.AddSkipSelfWhenShowOnVirtualScreenList(persistentIds, 100);
    mockMockSms.RemoveSkipSelfWhenShowOnVirtualScreenList(persistentIds, 100);
    mockMockSms.SetScreenPrivacyWindowTagSwitch(0, privacyWindowTags, false);
}

/**
 * @tc.name: NotifyWMSConnectionStatus
 * @tc.desc: test the function of NotifyWMSConnectionStatus
 * @tc.type: FUNC
 */
HWTEST(MockSessionManagerServiceTest, NotifyWMSConnectionStatus, TestSize.Level1)
{
    MockMockSessionManagerService mockMockSms;
    int32_t userId = 100;
    mockMockSms.wmsConnectionStatusMap_[userId] = true;

    sptr<IRemoteObject> mockRemoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<ISessionManagerServiceRecoverListener> smsListener =
        iface_cast<ISessionManagerServiceRecoverListener>(mockRemoteObject);
    ErrCode errCode = mockMockSms.NotifyWMSConnectionStatus(userId, smsListener);
    EXPECT_EQ(errCode, ERR_DEAD_OBJECT);

    mockMockSms.sessionManagerServiceMap_[userId] = sptr<IRemoteObjectMocker>::MakeSptr();
    mockMockSms.userId2ScreenIdMap_[userId] = 0;
    errCode = mockMockSms.NotifyWMSConnectionStatus(userId, smsListener);
    EXPECT_EQ(errCode, ERR_OK);
}
} // namespace
} // namespace Rosen
} // namespace OHOS