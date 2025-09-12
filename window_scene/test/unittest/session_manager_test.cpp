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
#include "session_manager.h"

#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

#include "iremote_object_mocker.h"
#include "scene_board_judgement.h"
#include "session_manager_lite.h"
#include "session_manager_service_recover_interface.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
};

void SessionManagerTest::SetUpTestCase() {}

void SessionManagerTest::TearDownTestCase() {}

void SessionManagerTest::SetUp()
{
}

void SessionManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteRequest
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, OnRemoteRequest, TestSize.Level1)
{
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    IPCObjectStub iPCObjectStub;

    uint32_t code =
        static_cast<uint32_t>(OHOS::Rosen::ISessionManagerServiceRecoverListener::SessionManagerServiceRecoverMessage::
                                  TRANS_ID_ON_SESSION_MANAGER_SERVICE_RECOVER);
    auto ret = iPCObjectStub.OnRemoteRequest(code, data, reply, option);
    ASSERT_NE(ret, 0);

    code = static_cast<uint32_t>(OHOS::Rosen::ISessionManagerServiceRecoverListener::
                                     SessionManagerServiceRecoverMessage::TRANS_ID_ON_WMS_CONNECTION_CHANGED);
    ret = iPCObjectStub.OnRemoteRequest(code, data, reply, option);
    ASSERT_NE(ret, 0);

    code = 10;
    ret = iPCObjectStub.OnRemoteRequest(code, data, reply, option);
    ASSERT_NE(0, ret);
}

/**
 * @tc.name: OnRemoteDied1
 * @tc.desc: foundation died
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, OnRemoteDied1, TestSize.Level1)
{
    auto sm = SessionManager::GetInstance(-1);
    FoundationDeathRecipient foundationDeathRecipient;
    wptr<IRemoteObject> wptrDeath;
    foundationDeathRecipient.OnRemoteDied(wptrDeath);
    ASSERT_EQ(false, sm->isWMSConnected_);
    ASSERT_EQ(false, sm->isFoundationListenerRegistered_);
    ASSERT_EQ(false, sm->isRecoverListenerRegistered_);
    ASSERT_EQ(nullptr, sm->mockSessionManagerServiceProxy_);
    ASSERT_EQ(nullptr, sm->sessionManagerServiceProxy_);
    ASSERT_EQ(nullptr, sm->sceneSessionManagerProxy_);
}

/**
 * @tc.name: OnRemoteDied2
 * @tc.desc: scb died
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, OnRemoteDied2, TestSize.Level1)
{
    auto sm = SessionManager::GetInstance(-1);
    SSMDeathRecipient sSMDeathRecipient;
    wptr<IRemoteObject> wptrDeath;
    sSMDeathRecipient.OnRemoteDied(wptrDeath);
    ASSERT_EQ(nullptr, sm->sessionManagerServiceProxy_);
}

/**
 * @tc.name: OnFoundationDied
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, OnFoundationDied, TestSize.Level1)
{
    auto sm = SessionManager::GetInstance(-1);
    sm->OnFoundationDied();
    ASSERT_EQ(false, sm->isWMSConnected_);
    ASSERT_EQ(false, sm->isFoundationListenerRegistered_);
    ASSERT_EQ(false, sm->isRecoverListenerRegistered_);
    ASSERT_EQ(nullptr, sm->mockSessionManagerServiceProxy_);
    ASSERT_EQ(nullptr, sm->sessionManagerServiceProxy_);
    ASSERT_EQ(nullptr, sm->sceneSessionManagerProxy_);
}

/**
 * @tc.name: InitMockSMSProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, InitMockSMSProxy, TestSize.Level1)
{
    auto sm = SessionManager::GetInstance(-1);
    sm->InitMockSMSProxy();
    ASSERT_NE(sm->foundationDeath_, nullptr);
}

/**
 * @tc.name: RegisterWindowManagerRecoverCallbackFunc
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, RegisterWindowManagerRecoverCallbackFunc, TestSize.Level1)
{
    auto sm = SessionManager::GetInstance(-1);
    auto testFunc = []() { return; };
    sm->RegisterWindowManagerRecoverCallbackFunc(testFunc);
    ASSERT_NE(sm->windowManagerRecoverFunc_, nullptr);
}

/**
 * @tc.name: SessionManager::GetInstance(int32_t userId)
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, GetInstanceMulti, TestSize.Level1)
{
    sptr<SessionManager> instance = nullptr;
    int32_t userId = 101;
    instance = SessionManager::GetInstance(userId);
    ASSERT_NE(nullptr, instance);

    // branch overried
    instance = SessionManager::GetInstance(userId);
    ASSERT_NE(nullptr, instance);
}

/**
 * @tc.name: RemoveSSMDeathRecipient
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, RemoveSSMDeathRecipient, TestSize.Level1)
{
    sptr<SessionManager> sm = nullptr;
    sm = SessionManager::GetInstance(101);
    // branch 1
    sm->sceneSessionManagerProxy_ = nullptr;
    sm->RemoveSSMDeathRecipient();
 
    // branch 2
    auto sceneProxy = sm->GetSceneSessionManagerProxy();
    ASSERT_NE(nullptr, sceneProxy);
    sm->RemoveSSMDeathRecipient();
}

/**
 * @tc.name: InitSessionManagerServiceProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, InitSessionManagerServiceProxy, TestSize.Level1)
{
    sptr<SessionManager> instance = nullptr;
    instance = SessionManager::GetInstance(101);
    // branch 1
    sptr<IRemoteObject> object = new IRemoteObjectMocker();
    sptr<ISessionManagerService> smsProxy = iface_cast<ISessionManagerService>(object);
    instance->sessionManagerServiceProxy_ = smsProxy;
    instance->InitSessionManagerServiceProxy();
    ASSERT_NE(nullptr, instance);
 
    // branch 2
    instance->ClearSessionManagerProxy();
    instance->InitSessionManagerServiceProxy();
    ASSERT_NE(nullptr, instance);
}
 
/**
 * @tc.name: InitSceneSessionManagerProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, InitSceneSessionManagerProxy, TestSize.Level1)
{
    sptr<SessionManager> instance = nullptr;
    instance = SessionManager::GetInstance(101);
    // branch 1
    sptr<IRemoteObject> object = new IRemoteObjectMocker();
    sptr<ISceneSessionManager> sceneProxy = iface_cast<ISceneSessionManager>(object);
    instance->sceneSessionManagerProxy_ = sceneProxy;
    instance->InitSceneSessionManagerProxy();
    ASSERT_NE(nullptr, instance);
 
    // branch 2
    instance->ClearSessionManagerProxy();
    instance->InitSceneSessionManagerProxy();
    ASSERT_NE(nullptr, instance);
 
    // branch 3, fake sessionManagerServiceProxy_
    instance->ClearSessionManagerProxy();
    sptr<ISessionManagerService> sessionProxy = iface_cast<ISessionManagerService>(object);
    instance->sessionManagerServiceProxy_ = sessionProxy;
    instance->InitSceneSessionManagerProxy();
 
    // branch 4
    instance->ClearSessionManagerProxy();
    instance->InitSessionManagerServiceProxy();
    ASSERT_NE(nullptr, instance->sessionManagerServiceProxy_);
    instance->InitSceneSessionManagerProxy();
    ASSERT_NE(nullptr, instance->sceneSessionManagerProxy_);
}
} // namespace
} // namespace Rosen
} // namespace OHOS