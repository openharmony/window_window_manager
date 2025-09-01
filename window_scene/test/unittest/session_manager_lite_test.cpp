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
#include "session_manager.h"
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <ipc_skeleton.h>
#include "scene_board_judgement.h"
#include "session_manager_lite.h"
#include "session_manager_service_recover_interface.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"
#include "iremote_object_mocker.h"

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

private:
    std::shared_ptr<SessionManagerLite> sml_;
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
    int32_t userId = -1;
    auto sml = SessionManagerLite::GetInstance(userId);
    bool funcInvoked = false;
    sml->RecoverSessionManagerService(nullptr);
    ASSERT_EQ(funcInvoked, false);

    sml->userSwitchCallbackFunc_ = [&]() { funcInvoked = true; };
    sml->RecoverSessionManagerService(nullptr);
    ASSERT_EQ(funcInvoked, true);
}

/**
 * @tc.name: ReregisterSessionListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, ReregisterSessionListener, TestSize.Level1)
{
    int32_t userId = -1;
    auto sml = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, sml);
    sml->ReregisterSessionListener();
    ASSERT_EQ(nullptr, sml->sceneSessionManagerLiteProxy_);

    sml->recoverListenerRegistered_ = true;
    sml->GetSceneSessionManagerLiteProxy();
    sml->ReregisterSessionListener();
    ASSERT_NE(nullptr, sml->sceneSessionManagerLiteProxy_);
}

/**
 * @tc.name: OnRemoteDied1
 * @tc.desc: foundation died
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnRemoteDied1, TestSize.Level1)
{
    int32_t userId = -1;
    auto sml = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, sml);
    FoundationDeathRecipient foundationDeathRecipient;
    wptr<IRemoteObject> wptrDeath;
    foundationDeathRecipient.OnRemoteDied(wptrDeath);
    ASSERT_EQ(false, sml->isWMSConnected_);
    ASSERT_EQ(false, sml->isFoundationListenerRegistered_);
    ASSERT_EQ(false, sml->recoverListenerRegistered_);
    ASSERT_EQ(nullptr, sml->mockSessionManagerServiceProxy_);
    ASSERT_EQ(nullptr, sml->sessionManagerServiceProxy_);
    ASSERT_EQ(nullptr, sml->sceneSessionManagerLiteProxy_);
}

/**
 * @tc.name: OnRemoteDied2
 * @tc.desc: scb died
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnRemoteDied2, TestSize.Level1)
{
    int32_t userId = -1;
    auto sml = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, sml);
    SSMDeathRecipient sSMDeathRecipient;
    wptr<IRemoteObject> wptrDeath;
    sSMDeathRecipient.OnRemoteDied(wptrDeath);
    ASSERT_EQ(nullptr, sml->sessionManagerServiceProxy_);
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
    int32_t userId = -1;
    auto sml = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, sml);
    sml->InitMockSMSProxy();
    sml->InitMockSMSProxy();
    ASSERT_NE(sml->foundationDeath_, nullptr);
}

/**
 * @tc.name: GetInstance
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, GetInstance, TestSize.Level1)
{
    sptr<SessionManagerLite> instance = nullptr;
    int32_t userId;

    userId = -1;
    instance = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, instance);

    userId = 101;
    instance = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, instance);

    // branch overried
    instance = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, instance);
}

/**
 * @tc.name: SessionManagerServiceLiteRecoverListener::OnSessionManagerServiceRecover
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, SessionManagerServiceLiteRecoverListener1, TestSize.Level1)
{
    int32_t userId = 101;
    sptr<SessionManagerLite> sml = SessionManagerLite::GetInstance(userId);
    ASSERT_NE(nullptr, sml);

    sptr<SessionManagerServiceLiteRecoverListener> listener = new SessionManagerServiceLiteRecoverListener(sml);
    sptr<IRemoteObject> service = sptr<IRemoteObjectMocker>::MakeSptr();
    listener->OnSessionManagerServiceRecover(service);
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
    sptr<SessionManagerServiceLiteRecoverListener> listener1 = new SessionManagerServiceLiteRecoverListener(nullptr);
    ASSERT_NE(nullptr, listener1);
    listener1->OnWMSConnectionChanged(userId, screenId, isConnected, service);

    sptr<SessionManagerLite> sml = SessionManagerLite::GetInstance(userId);
    sptr<SessionManagerServiceLiteRecoverListener> listener2 = new SessionManagerServiceLiteRecoverListener(sml);
    ASSERT_NE(nullptr, listener2);
    listener2->OnWMSConnectionChanged(userId, screenId, isConnected, service);
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
    int32_t userId = 101;

    sptr<SessionManagerLite> sml = SessionManagerLite::GetInstance(userId);
    sptr<SessionManagerServiceLiteRecoverListener> listener = new SessionManagerServiceLiteRecoverListener(sml);
    ASSERT_NE(nullptr, listener);

    code = static_cast<uint32_t>(OHOS::Rosen::ISessionManagerServiceRecoverListener::
                                     SessionManagerServiceRecoverMessage::TRANS_ID_ON_SESSION_MANAGER_SERVICE_RECOVER);
    auto ret = listener->OnRemoteRequest(code, data, reply, option);
    ASSERT_NE(ret, 0);

    code = static_cast<uint32_t>(OHOS::Rosen::ISessionManagerServiceRecoverListener::
                                     SessionManagerServiceRecoverMessage::TRANS_ID_ON_WMS_CONNECTION_CHANGED);
    ret = listener->OnRemoteRequest(code, data, reply, option);
    ASSERT_NE(ret, 0);

    code = 10;
    ret = listener->OnRemoteRequest(code, data, reply, option);
    ASSERT_NE(ret, 0);
}

/**
 * @tc.name: SSMDeathRecipientLite::OnRemoteDied
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnRemoteDied3, TestSize.Level1)
{
    sptr<SSMDeathRecipientLite> recipient = new SSMDeathRecipientLite();
    ASSERT_NE(nullptr, recipient);
    wptr<IRemoteObject> wptrDeath;
    recipient->OnRemoteDied(wptrDeath);
}
} // namespace
} // namespace Rosen
} // namespace OHOS