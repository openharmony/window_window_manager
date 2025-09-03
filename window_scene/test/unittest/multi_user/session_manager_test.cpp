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

#include <gtest/gtest.h>

#include "session_manager.h"
#include "session_manager_lite.h"
#include "iremote_object_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionManagerTest : public Test {
public:
    void SetUp() override;
    void TearDown() override;

private:
    std::shared_ptr<SessionManager> sm_;
};

void SessionManagerTest::SetUp()
{
    sm_ = std::make_shared<SessionManager>();
}

void SessionManagerTest::TearDown()
{
    sm_ = nullptr;
}

/**
 * @tc.name: GetSceneSessionManagerProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, GetSceneSessionManagerProxy, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sm_);
    sm_->RemoveSSMDeathRecipient();
    sm_->ClearSessionManagerProxy();
    auto sceneSessionManagerProxy = sm_->GetSceneSessionManagerProxy();
    ASSERT_NE(nullptr, sceneSessionManagerProxy);

    sm_->ClearSessionManagerProxy();
    sm_->sessionManagerServiceProxy_ = SessionManagerLite::GetInstance().GetSessionManagerServiceProxy();
    sceneSessionManagerProxy = sm_->GetSceneSessionManagerProxy();
    ASSERT_NE(nullptr, sceneSessionManagerProxy);
}

/**
 * @tc.name: ClearSessionManagerProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, ClearSessionManagerProxy, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sm_);
    sm_->ClearSessionManagerProxy();
    ASSERT_EQ(sm_->sessionManagerServiceProxy_, nullptr);

    sm_->isRecoverListenerRegistered_ = true;
    sm_->sessionManagerServiceProxy_ = SessionManagerLite::GetInstance().GetSessionManagerServiceProxy();
    sm_->ClearSessionManagerProxy();
    ASSERT_EQ(sm_->sessionManagerServiceProxy_, nullptr);
}

/**
 * @tc.name: OnWMSConnectionChangedCallback
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, OnWMSConnectionChangedCallback, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sm_);
    bool funcInvoked = false;
    sm_->wmsConnectionChangedFunc_ = nullptr;
    sm_->OnWMSConnectionChangedCallback(101, DEFAULT_SCREEN_ID, true, false);

    sm_->wmsConnectionChangedFunc_ = [&](int32_t userId, int32_t screenId, bool isConnected) { funcInvoked = true; };
    sm_->OnWMSConnectionChangedCallback(101, DEFAULT_SCREEN_ID, true, true);
    ASSERT_EQ(funcInvoked, true);
}

/**
 * @tc.name: OnWMSConnectionChanged1
 * @tc.desc: wms disconnected
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, OnWMSConnectionChanged1, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sm_);
    sptr<ISessionManagerService> sessionManagerService;
    sm_->isWMSConnected_ = true;
    sm_->currentWMSUserId_ = 100;
    sm_->OnWMSConnectionChanged(100, DEFAULT_SCREEN_ID, false, sessionManagerService);
    ASSERT_EQ(sm_->isWMSConnected_, false);

    sm_->currentWMSUserId_ = 101;
    sm_->isWMSConnected_ = true;
    sm_->OnWMSConnectionChanged(100, DEFAULT_SCREEN_ID, false, sessionManagerService);
    ASSERT_EQ(sm_->isWMSConnected_, true);
}

/**
 * @tc.name: OnWMSConnectionChanged2
 * @tc.desc: wms connected
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, OnWMSConnectionChanged2, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sm_);
    sptr<ISessionManagerService> sessionManagerService;
    sm_->isWMSConnected_ = false;
    sm_->currentWMSUserId_ = INVALID_USER_ID;
    sm_->OnWMSConnectionChanged(100, DEFAULT_SCREEN_ID, true, sessionManagerService);
    ASSERT_EQ(sm_->isWMSConnected_, true);

    // user switch
    sm_->currentWMSUserId_ = 100;
    sm_->isWMSConnected_ = true;
    sm_->OnWMSConnectionChanged(101, DEFAULT_SCREEN_ID, true, sessionManagerService);
    ASSERT_EQ(sm_->isWMSConnected_, true);
}

/**
 * @tc.name: RegisterUserSwitchListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, RegisterUserSwitchListener, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sm_);
    sm_->RegisterUserSwitchListener(nullptr);
    ASSERT_EQ(sm_->userSwitchCallbackFunc_, nullptr);

    sm_->RegisterUserSwitchListener([]() {});
    ASSERT_NE(sm_->userSwitchCallbackFunc_, nullptr);
}

/**
 * @tc.name: OnUserSwitch
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, OnUserSwitch, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sm_);
    sm_->OnUserSwitch(nullptr);
    ASSERT_EQ(nullptr, sm_->sessionManagerServiceProxy_);

    bool funInvoked = false;
    sm_->userSwitchCallbackFunc_ = [&]() { funInvoked = true; };
    auto sessionManagerService = SessionManagerLite::GetInstance().GetSessionManagerServiceProxy();
    sm_->OnUserSwitch(sessionManagerService);
    ASSERT_EQ(funInvoked, true);
}

/**
 * @tc.name: RegisterWMSConnectionChangedListener
 * @tc.desc: WMSConnectionChangedCallbackFunc is null
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, RegisterWMSConnectionChangedListener, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sm_);
    auto ret = sm_->RegisterWMSConnectionChangedListener(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: RegisterWMSConnectionChangedListener1
 * @tc.desc: normal test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, RegisterWMSConnectionChangedListener1, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sm_);
    sm_->isRecoverListenerRegistered_ = true;
    sm_->currentWMSUserId_ = 100;
    sm_->currentScreenId_ = 0;
    sm_->isWMSConnected_ = true;
    auto callbackFunc = [](int32_t userId, int32_t screenId, bool isConnected) {};
    auto ret = sm_->RegisterWMSConnectionChangedListener(callbackFunc);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: UnregisterWMSConnectionChangedListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, UnregisterWMSConnectionChangedListener, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sm_);
    sptr<IRemoteObject> remoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    sm_->mockSessionManagerServiceProxy_ = iface_cast<IMockSessionManagerInterface>(remoteObject);
    auto ret = sm_->UnregisterWMSConnectionChangedListener();
    ASSERT_EQ(WMError::WM_OK, ret);
}
} // namespace Rosen
} // namespace OHOS
