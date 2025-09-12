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
#include "iremote_object_mocker.h"
#include "session_manager_lite.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionManagerLiteTest : public Test {
public:
    void SetUp() override;
    void TearDown() override;

private:
    sptr<SessionManagerLite> sml_;
};

void SessionManagerLiteTest::SetUp()
{
    sml_ = SessionManagerLite::GetInstance(-1);
}

void SessionManagerLiteTest::TearDown()
{
    sml_ = nullptr;
}

/**
 * @tc.name: GetSceneSessionManagerLiteProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, GetSceneSessionManagerLiteProxy, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sml_);
    sml_->Clear();
    sml_->ClearSessionManagerProxy();
    auto sceneSessionManagerLiteProxy = sml_->GetSceneSessionManagerLiteProxy();
    ASSERT_NE(nullptr, sceneSessionManagerLiteProxy);

    sml_->ClearSessionManagerProxy();
    sml_->GetSessionManagerServiceProxy();
    sceneSessionManagerLiteProxy = sml_->GetSceneSessionManagerLiteProxy();
    ASSERT_NE(nullptr, sceneSessionManagerLiteProxy);
}

/**
 * @tc.name: InitSceneSessionManagerLiteProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, InitSceneSessionManagerLiteProxy01, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sml_);
    sml_->InitSceneSessionManagerLiteProxy();
    ASSERT_NE(nullptr, sml_->sceneSessionManagerLiteProxy_);
}

/**
 * @tc.name: InitSceneSessionManagerLiteProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, InitSceneSessionManagerLiteProxy02, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sml_);
    sml_->GetSceneSessionManagerLiteProxy();
    sml_->InitSceneSessionManagerLiteProxy();
    ASSERT_NE(nullptr, sml_->sceneSessionManagerLiteProxy_);
}

/**
 * @tc.name: ClearSessionManagerProxy
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, ClearSessionManagerProxy, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sml_);
    sml_->ClearSessionManagerProxy();
    ASSERT_EQ(sml_->sessionManagerServiceProxy_, nullptr);

    sml_->recoverListenerRegistered_ = true;
    sml_->GetSessionManagerServiceProxy();
    sml_->ClearSessionManagerProxy();
    ASSERT_EQ(sml_->sessionManagerServiceProxy_, nullptr);
}

/**
 * @tc.name: OnWMSConnectionChangedCallback
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnWMSConnectionChangedCallback, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sml_);
    bool funcInvoked = false;
    sml_->wmsConnectionChangedFunc_ = nullptr;
    sml_->OnWMSConnectionChangedCallback(101, DEFAULT_SCREEN_ID, true, false);

    sml_->wmsConnectionChangedFunc_ = [&](int32_t userId, int32_t screenId, bool isConnected) { funcInvoked = true; };
    sml_->OnWMSConnectionChangedCallback(101, DEFAULT_SCREEN_ID, true, true);
    ASSERT_EQ(funcInvoked, true);
}

/**
 * @tc.name: OnWMSConnectionChanged1
 * @tc.desc: wms disconnected
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnWMSConnectionChanged1, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sml_);
    sptr<ISessionManagerService> sessionManagerService;
    sml_->isWMSConnected_ = true;
    sml_->currentWMSUserId_ = 100;
    sml_->OnWMSConnectionChanged(100, DEFAULT_SCREEN_ID, false, sessionManagerService);
    ASSERT_EQ(sml_->isWMSConnected_, false);

    sml_->currentWMSUserId_ = 101;
    sml_->isWMSConnected_ = true;
    sml_->OnWMSConnectionChanged(100, DEFAULT_SCREEN_ID, false, sessionManagerService);
    ASSERT_EQ(sml_->isWMSConnected_, true);
}

/**
 * @tc.name: OnWMSConnectionChanged2
 * @tc.desc: wms connected
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnWMSConnectionChanged2, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sml_);
    sptr<ISessionManagerService> sessionManagerService;
    sml_->isWMSConnected_ = false;
    sml_->currentWMSUserId_ = INVALID_USER_ID;
    sml_->OnWMSConnectionChanged(100, DEFAULT_SCREEN_ID, true, sessionManagerService);
    ASSERT_EQ(sml_->isWMSConnected_, true);

    // user switch
    sml_->currentWMSUserId_ = 100;
    sml_->isWMSConnected_ = true;
    sml_->OnWMSConnectionChanged(101, DEFAULT_SCREEN_ID, true, sessionManagerService);
    ASSERT_EQ(sml_->isWMSConnected_, true);
}

/**
 * @tc.name: OnUserSwitch
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnUserSwitch, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sml_);
    sml_->OnUserSwitch(nullptr);
    ASSERT_EQ(nullptr, sml_->sessionManagerServiceProxy_);

    sml_->recoverListenerRegistered_ = true;
    bool funInvoked = false;
    sml_->userSwitchCallbackFunc_ = [&]() { funInvoked = true; };
    auto sessionManagerService = sml_->GetSessionManagerServiceProxy();
    sml_->OnUserSwitch(sessionManagerService);
    ASSERT_EQ(funInvoked, true);
}

/**
 * @tc.name: RegisterWMSConnectionChangedListener
 * @tc.desc: WMSConnectionChangedCallbackFunc is null
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, RegisterWMSConnectionChangedListener, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sml_);
    auto ret = sml_->RegisterWMSConnectionChangedListener(nullptr);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, ret);
}

/**
 * @tc.name: RegisterWMSConnectionChangedListener1
 * @tc.desc: normal test
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, RegisterWMSConnectionChangedListener1, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sml_);
    sml_->recoverListenerRegistered_ = true;
    sml_->currentWMSUserId_ = 100;
    sml_->currentScreenId_ = 0;
    sml_->isWMSConnected_ = true;
    auto callbackFunc = [](int32_t userId, int32_t screenId, bool isConnected) {};
    auto ret = sml_->RegisterWMSConnectionChangedListener(callbackFunc);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: RegisterUserSwitchListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, RegisterUserSwitchListener, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sml_);
    sml_->RegisterUserSwitchListener(nullptr);
    ASSERT_EQ(sml_->userSwitchCallbackFunc_, nullptr);

    sml_->RegisterUserSwitchListener([]() {});
    ASSERT_NE(sml_->userSwitchCallbackFunc_, nullptr);
}

/**
 * @tc.name: UnregisterWMSConnectionChangedListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, UnregisterWMSConnectionChangedListener, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, sml_);
    sptr<IRemoteObject> remoteObject = sptr<IRemoteObjectMocker>::MakeSptr();
    sml_->mockSessionManagerServiceProxy_ = iface_cast<IMockSessionManagerInterface>(remoteObject);
    auto ret = sml_->UnregisterWMSConnectionChangedListener();
    ASSERT_EQ(WMError::WM_OK, ret);
}
} // namespace Rosen
} // namespace OHOS