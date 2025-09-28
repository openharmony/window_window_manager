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

#include <gtest/gtest.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "iremote_object_mocker.h"
#include "scene_board_judgement.h"
#include "session_manager.h"
#include "session_manager_lite.h"
#include "session_manager_service_recover_interface.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionManagerLiteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SessionManagerLiteTest::SetUpTestCase() {}

void SessionManagerLiteTest::TearDownTestCase() {}

void SessionManagerLiteTest::SetUp()
{
}

void SessionManagerLiteTest::TearDown()
{
}

namespace {
/**
 * @tc.name: RecoverSessionManagerService
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, RecoverSessionManagerService, TestSize.Level1)
{
    auto sml = SessionManagerLite::GetInstance(-1);
    bool funcInvoked = false;
    sml->RecoverSessionManagerService(nullptr);

    sml->userSwitchCallbackFunc_ = [&]() { funcInvoked = true; };
    sml->RecoverSessionManagerService(nullptr);
    EXPECT_NE(funcInvoked, false);
}

/**
 * @tc.name: ReregisterSessionListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, ReregisterSessionListener, TestSize.Level1)
{
    auto sml = SessionManagerLite::GetInstance(-1);
    sml->recoverListenerRegistered_ = true;
    sml->GetSceneSessionManagerLiteProxy();
    sml->ReregisterSessionListener();
    ASSERT_NE(nullptr, sml);
}

/**
 * @tc.name: FoundationDeathRecipientLite::OnRemoteDied
 * @tc.desc: foundation died
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnRemoteDied1, TestSize.Level1)
{
    auto sml = SessionManagerLite::GetInstance(-1);
    sptr<FoundationDeathRecipientLite> deathRecipient = new FoundationDeathRecipientLite();
    sptr<IRemoteObject> sptrDeath = sptr<IRemoteObjectMocker>::MakeSptr();
    wptr<IRemoteObject> wptrDeath = wptr(sptrDeath);
    deathRecipient->OnRemoteDied(wptrDeath);
    ASSERT_NE(nullptr, sml);
}

/**
 * @tc.name: OnRemoteDied2
 * @tc.desc: scb died
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnRemoteDied2, TestSize.Level1)
{
    auto sml = SessionManagerLite::GetInstance(-1);
    ASSERT_NE(nullptr, sml);
    sptr<SSMDeathRecipientLite> deathRecipient = new SSMDeathRecipientLite();
    sptr<IRemoteObject> sptrDeath = sptr<IRemoteObjectMocker>::MakeSptr();
    wptr<IRemoteObject> wptrDeath = wptr(sptrDeath);
    deathRecipient->OnRemoteDied(wptrDeath);
}

/**
 * @tc.name: OnFoundationDied
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnFoundationDied, TestSize.Level1)
{
    int32_t userId = -1;
    auto sml = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, sml);
    sml->OnFoundationDied();
    ASSERT_EQ(false, sml->isWMSConnected_);
    ASSERT_EQ(false, sml->isFoundationListenerRegistered_);
    ASSERT_EQ(false, sml->recoverListenerRegistered_);
    ASSERT_EQ(nullptr, sml->mockSessionManagerServiceProxy_);
    ASSERT_EQ(nullptr, sml->sessionManagerServiceProxy_);
    ASSERT_EQ(nullptr, sml->sceneSessionManagerLiteProxy_);
}

/**
 * @tc.name: RegisterSMSRecoverListener1
 * @tc.desc: mockSessionManagerServiceProxy_ is null
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, RegisterSMSRecoverListener1, TestSize.Level1)
{
    int32_t userId = -1;
    auto sml = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, sml);
    sml->recoverListenerRegistered_ = false;
    sml->mockSessionManagerServiceProxy_ = nullptr;
    sml->RegisterSMSRecoverListener();
    ASSERT_EQ(sml->recoverListenerRegistered_, false);
}

/**
 * @tc.name: RegisterSMSRecoverListener2
 * @tc.desc: normal test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, RegisterSMSRecoverListener2, TestSize.Level1)
{
    int32_t userId = -1;
    auto sml = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, sml);
    sml->recoverListenerRegistered_ = false;
    sml->InitMockSMSProxy();
    sml->RegisterSMSRecoverListener();
    ASSERT_EQ(sml->recoverListenerRegistered_, true);
}

/**
 * @tc.name: InitMockSMSProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, InitMockSMSProxy, TestSize.Level1)
{
    auto sml = SessionManagerLite::GetInstance(-1);
    sml->InitMockSMSProxy();
    ASSERT_NE(nullptr, sml);
}

/**
 * @tc.name: SessionManagerServiceLiteRecoverListener::OnSessionManagerServiceRecover
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, SessionManagerServiceLiteRecoverListener1, TestSize.Level1)
{
    auto sml = SessionManagerLite::GetInstance(-1);
    ASSERT_NE(nullptr, sml);
 
    sptr<IRemoteObject> service = sptr<IRemoteObjectMocker>::MakeSptr();
    // branch 1: nullptr
    sptr<SessionManagerServiceLiteRecoverListener> listener1 = new SessionManagerServiceLiteRecoverListener(nullptr);
    listener1->OnSessionManagerServiceRecover(service);
 
    // branch 2
    sptr<SessionManagerServiceLiteRecoverListener> listener2 = new SessionManagerServiceLiteRecoverListener(sml);
    listener2->OnSessionManagerServiceRecover(service);
    ASSERT_NE(nullptr, sml);
}

/**
 * @tc.name: SessionManagerServiceLiteRecoverListener::OnWMSConnectionChanged
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, SessionManagerServiceLiteRecoverListener2, TestSize.Level1)
{
    int32_t userId = 100;
    int32_t screenId = 101;
    bool isConnected = false;
    sptr<IRemoteObject> service = sptr<IRemoteObjectMocker>::MakeSptr();
 
    // branch 1
    sptr<SessionManagerServiceLiteRecoverListener> listener1 = new SessionManagerServiceLiteRecoverListener(nullptr);
    listener1->OnWMSConnectionChanged(userId, screenId, isConnected, service);
    ASSERT_NE(nullptr, listener1);
 
    // branch 2
    auto sml = SessionManagerLite::GetInstance(-1);
    sptr<SessionManagerServiceLiteRecoverListener> listener2 = new SessionManagerServiceLiteRecoverListener(sml);
    listener2->OnWMSConnectionChanged(userId, screenId, isConnected, service);
    ASSERT_NE(nullptr, sml);
}

/**
 * @tc.name: SessionManagerServiceLiteRecoverListener::OnRemoteRequest
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, SessionManagerServiceLiteRecoverListener3, TestSize.Level1)
{
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    IPCObjectStub iPCObjectStub;
    uint32_t code;
    auto sml = SessionManagerLite::GetInstance(-1);
    sptr<SessionManagerServiceLiteRecoverListener> listener = new SessionManagerServiceLiteRecoverListener(sml);
    code = static_cast<uint32_t>(OHOS::Rosen::ISessionManagerServiceRecoverListener::
                                     SessionManagerServiceRecoverMessage::TRANS_ID_ON_SESSION_MANAGER_SERVICE_RECOVER);
    listener->OnRemoteRequest(code, data, reply, option);
 
    code = static_cast<uint32_t>(OHOS::Rosen::ISessionManagerServiceRecoverListener::
                                     SessionManagerServiceRecoverMessage::TRANS_ID_ON_WMS_CONNECTION_CHANGED);
    listener->OnRemoteRequest(code, data, reply, option);
 
    code = 10;
    listener->OnRemoteRequest(code, data, reply, option);
    ASSERT_NE(nullptr, listener);
}

/**
 * @tc.name: SessionManagerLite::GetInstance(int32_t userId)
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, GetInstanceMulti, TestSize.Level1)
{
    sptr<SessionManagerLite> instance = nullptr;
    int32_t userId = 101;
 
    instance = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, instance);
 
    // branch overried
    instance = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, instance);
}

/**
 * @tc.name: SessionManagerLite::InitSessionManagerServiceProxy()
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, InitSessionManagerServiceProxy, TestSize.Level1)
{
    // branch 1
    auto instance1 = SessionManagerLite::GetInstance(-1);
    instance1->ClearSessionManagerProxy();
    ASSERT_EQ(nullptr, instance1->sessionManagerServiceProxy_);
    instance1->InitSessionManagerServiceProxy();
 
    // branch 2
    auto instance2 = SessionManagerLite::GetInstance(101);
    instance2->ClearSessionManagerProxy();
    ASSERT_EQ(nullptr, instance2->sessionManagerServiceProxy_);
    instance2->InitSessionManagerServiceProxy();
}
} // namespace
} // namespace Rosen
} // namespace OHOS