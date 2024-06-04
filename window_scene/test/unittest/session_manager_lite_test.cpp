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
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <ipc_skeleton.h>

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

void SessionManagerLiteTest::SetUpTestCase()
{
}

void SessionManagerLiteTest::TearDownTestCase()
{
}

void SessionManagerLiteTest::SetUp()
{
}

void SessionManagerLiteTest::TearDown()
{
}

/**
 * @tc.name: OnWMSConnectionChangedCallback
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, OnWMSConnectionChangedCallback, Function | SmallTest | Level2)
{
    SessionManagerLite sessionManagerLite;
    sessionManagerLite.wmsConnectionChangedFunc_ = nullptr;
    sessionManagerLite.OnWMSConnectionChangedCallback(0, 0, true);

    int32_t userId = 2;
    int32_t screenId = 0;
    bool isConnected = true;
    sessionManagerLite.currentWMSUserId_ = SYSTEM_USERID;
    sessionManagerLite.OnWMSConnectionChanged(userId, screenId, isConnected, nullptr);

    sessionManagerLite.destroyed_ = true;
    sessionManagerLite.ClearSessionManagerProxy();

    sptr<ISessionManagerService> sessionManagerService;
    sessionManagerLite.RecoverSessionManagerService(sessionManagerService);
    sessionManagerLite.RegisterUserSwitchListener([]() {});
    sessionManagerLite.OnUserSwitch(sessionManagerService);
    sessionManagerLite.Clear();

    sessionManagerLite.isWMSConnected_ = true;
    sessionManagerLite.currentWMSUserId_ = SYSTEM_USERID;
    SessionManagerLite::WMSConnectionChangedCallbackFunc callbackFunc;
    auto ret = sessionManagerLite.RegisterWMSConnectionChangedListener(callbackFunc);
    ASSERT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: RegisterWMSConnectionChangedListener
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionManagerLiteTest, RegisterWMSConnectionChangedListener, Function | SmallTest | Level2)
{
    SessionManagerLite sessionManagerLite;
    sessionManagerLite.OnFoundationDied();
    FoundationDeathRecipientLite foundationDeathRecipientLite;
    wptr<IRemoteObject> wptrDeath = nullptr;
    foundationDeathRecipientLite.OnRemoteDied(wptrDeath);

    SSMDeathRecipient sSMDeathRecipient;
    sSMDeathRecipient.OnRemoteDied(wptrDeath);
    SessionManagerLite::WMSConnectionChangedCallbackFunc callbackFunc;
    auto ret = sessionManagerLite.RegisterWMSConnectionChangedListener(callbackFunc);
    ASSERT_EQ(WMError::WM_OK, ret);
}
}
}