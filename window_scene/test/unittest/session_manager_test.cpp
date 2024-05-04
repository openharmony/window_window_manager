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

#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <ipc_skeleton.h>

#include "session_manager_service_recover_interface.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"
#include "session_manager_lite.h"

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
};

void SessionManagerTest::SetUpTestCase()
{
}

void SessionManagerTest::TearDownTestCase()
{
}

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
HWTEST_F(SessionManagerTest, OnRemoteRequest, Function | SmallTest | Level2)
{
    uint32_t code = static_cast<uint32_t>(OHOS::Rosen::ISessionManagerServiceRecoverListener::
        SessionManagerServiceRecoverMessage::TRANS_ID_ON_SESSION_MANAGER_SERVICE_RECOVER);
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    IPCObjectStub iPCObjectStub;
    iPCObjectStub.OnRemoteRequest(code, data, reply, option);
    code = static_cast<uint32_t>(OHOS::Rosen::ISessionManagerServiceRecoverListener::
        SessionManagerServiceRecoverMessage::TRANS_ID_ON_WMS_CONNECTION_CHANGED);
    auto ret = iPCObjectStub.OnRemoteRequest(code, data, reply, option);
    code = 10;
    iPCObjectStub.OnRemoteRequest(code, data, reply, option);
    ASSERT_NE(0, ret);
}

/**
 * @tc.name: OnWMSConnectionChangedCallback
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, OnWMSConnectionChangedCallback, Function | SmallTest | Level2)
{
    SessionManager sessionManager;
    sessionManager.wmsConnectionChangedFunc_ = nullptr;
    sessionManager.OnWMSConnectionChangedCallback(0, 0, true);

    int32_t userId = 2;
    int32_t screenId = 0;
    bool isConnected = true;
    sessionManager.currentWMSUserId_ = SYSTEM_USERID;
    sessionManager.OnWMSConnectionChanged(userId, screenId, isConnected);

    sessionManager.destroyed_ = true;
    sessionManager.ClearSessionManagerProxy();

    sptr<ISessionManagerService> sessionManagerService;
    sessionManager.RecoverSessionManagerService(sessionManagerService);
    sessionManager.OnUserSwitch();
    sessionManager.Clear();

    sessionManager.isWMSConnected_ = true;
    sessionManager.currentWMSUserId_ = SYSTEM_USERID;
    SessionManager::WMSConnectionChangedCallbackFunc callbackFunc;
    auto ret = sessionManager.RegisterWMSConnectionChangedListener(callbackFunc);
    ASSERT_EQ(WMError::WM_OK, ret);
}
/**
 * @tc.name: RegisterWMSConnectionChangedListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerTest, RegisterWMSConnectionChangedListener, Function | SmallTest | Level2)
{
    SessionManager sessionManager;
    sessionManager.OnFoundationDied();
    FoundationDeathRecipient foundationDeathRecipient;
    wptr<IRemoteObject> wptrDeath = nullptr;
    foundationDeathRecipient.OnRemoteDied(wptrDeath);

    SSMDeathRecipient sSMDeathRecipient;
    sSMDeathRecipient.OnRemoteDied(wptrDeath);
    SessionManager::WMSConnectionChangedCallbackFunc callbackFunc;
    auto ret = sessionManager.RegisterWMSConnectionChangedListener(callbackFunc);
    ASSERT_EQ(WMError::WM_OK, ret);
}
}
}
}