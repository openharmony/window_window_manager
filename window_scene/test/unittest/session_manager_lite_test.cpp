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
    sml_ = std::make_shared<SessionManagerLite>();
    ASSERT_NE(nullptr, sml_);
}

void SessionManagerLiteTest::TearDown()
{
    sml_ = nullptr;
}

namespace {
/**
 * @tc.name: RecoverSessionManagerService
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, RecoverSessionManagerService, TestSize.Level1)
{
    ASSERT_NE(nullptr, sml_);
    bool funcInvoked = false;
    sml_->RecoverSessionManagerService(nullptr);
    ASSERT_EQ(funcInvoked, false);

    sml_->userSwitchCallbackFunc_ = [&]() { funcInvoked = true; };
    sml_->RecoverSessionManagerService(nullptr);
    ASSERT_EQ(funcInvoked, true);
}

/**
 * @tc.name: ReregisterSessionListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, ReregisterSessionListener, TestSize.Level1)
{
    ASSERT_NE(nullptr, sml_);
    sml_->ReregisterSessionListener();
    ASSERT_EQ(nullptr, sml_->sceneSessionManagerLiteProxy_);

    sml_->recoverListenerRegistered_ = true;
    sml_->GetSceneSessionManagerLiteProxy();
    sml_->ReregisterSessionListener();
    ASSERT_NE(nullptr, sml_->sceneSessionManagerLiteProxy_);
}

/**
 * @tc.name: OnRemoteDied1
 * @tc.desc: foundation died
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnRemoteDied1, TestSize.Level1)
{
    ASSERT_NE(nullptr, sml_);
    FoundationDeathRecipient foundationDeathRecipient;
    wptr<IRemoteObject> wptrDeath;
    foundationDeathRecipient.OnRemoteDied(wptrDeath);
    ASSERT_EQ(false, sml_->isWMSConnected_);
    ASSERT_EQ(false, sml_->isFoundationListenerRegistered_);
    ASSERT_EQ(false, sml_->recoverListenerRegistered_);
    ASSERT_EQ(nullptr, sml_->mockSessionManagerServiceProxy_);
    ASSERT_EQ(nullptr, sml_->sessionManagerServiceProxy_);
    ASSERT_EQ(nullptr, sml_->sceneSessionManagerLiteProxy_);
}

/**
 * @tc.name: OnRemoteDied2
 * @tc.desc: scb died
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnRemoteDied2, TestSize.Level1)
{
    ASSERT_NE(nullptr, sml_);
    SSMDeathRecipient sSMDeathRecipient;
    wptr<IRemoteObject> wptrDeath;
    sSMDeathRecipient.OnRemoteDied(wptrDeath);
    ASSERT_EQ(nullptr, sml_->sessionManagerServiceProxy_);
}

/**
 * @tc.name: OnFoundationDied
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnFoundationDied, TestSize.Level1)
{
    ASSERT_NE(nullptr, sml_);
    sml_->OnFoundationDied();
    ASSERT_EQ(false, sml_->isWMSConnected_);
    ASSERT_EQ(false, sml_->isFoundationListenerRegistered_);
    ASSERT_EQ(false, sml_->recoverListenerRegistered_);
    ASSERT_EQ(nullptr, sml_->mockSessionManagerServiceProxy_);
    ASSERT_EQ(nullptr, sml_->sessionManagerServiceProxy_);
    ASSERT_EQ(nullptr, sml_->sceneSessionManagerLiteProxy_);
}

/**
 * @tc.name: RegisterSMSRecoverListener1
 * @tc.desc: mockSessionManagerServiceProxy_ is null
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, RegisterSMSRecoverListener1, TestSize.Level1)
{
    ASSERT_NE(nullptr, sml_);
    sml_->recoverListenerRegistered_ = false;
    sml_->mockSessionManagerServiceProxy_ = nullptr;
    sml_->RegisterSMSRecoverListener();
    ASSERT_EQ(sml_->recoverListenerRegistered_, false);
}

/**
 * @tc.name: RegisterSMSRecoverListener2
 * @tc.desc: normal test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, RegisterSMSRecoverListener2, TestSize.Level1)
{
    ASSERT_NE(nullptr, sml_);
    sml_->recoverListenerRegistered_ = false;
    sml_->InitMockSMSProxy();
    sml_->RegisterSMSRecoverListener();
    ASSERT_EQ(sml_->recoverListenerRegistered_, true);
}

/**
 * @tc.name: InitMockSMSProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, InitMockSMSProxy, TestSize.Level1)
{
    ASSERT_NE(nullptr, sml_);
    sml_->InitMockSMSProxy();
    sml_->InitMockSMSProxy();
    ASSERT_NE(sml_->foundationDeath_, nullptr);
}
} // namespace
} // namespace Rosen
} // namespace OHOS