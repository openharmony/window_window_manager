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

#include "iremote_object_mocker.h"
#include "mock_message_parcel.h"
#include "session_ipc_interface_code.h"
#include "session_proxy.h"
#include "ws_common.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionProxyPropertyTest : public testing::Test {
public:
    SessionProxyPropertyTest() {}
    ~SessionProxyPropertyTest() {}
};

namespace {

/**
 * @tc.name: UpdateSessionPropertyByAction01
 * @tc.desc: UpdateSessionPropertyByAction
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyPropertyTest, UpdateSessionPropertyByAction01, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyPropertyTest: UpdateSessionPropertyByAction01 start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    WMError res = sProxy->UpdateSessionPropertyByAction(nullptr, WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON);
    ASSERT_EQ(res, WMError::WM_OK);
    GTEST_LOG_(INFO) << "SessionProxyPropertyTest: UpdateSessionPropertyByAction01 end";
}

/**
 * @tc.name: UpdateSessionPropertyByAction02
 * @tc.desc: UpdateSessionPropertyByAction
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyPropertyTest, UpdateSessionPropertyByAction02, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionProxyPropertyTest: UpdateSessionPropertyByAction02 start";
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker, nullptr);
    auto sProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sProxy, nullptr);
    WMError res = sProxy->UpdateSessionPropertyByAction(nullptr, WSPropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON);
    ASSERT_EQ(res, WMError::WM_OK);
    GTEST_LOG_(INFO) << "SessionProxyPropertyTest: UpdateSessionPropertyByAction02 end";
}

/**
 * @tc.name: UpdateSessionPropertyByAction
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(SessionProxyPropertyTest, UpdateSessionPropertyByAction, TestSize.Level1)
{
    MockMessageParcel::ClearAllErrorFlag();
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    auto sessionProxy = sptr<SessionProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(sessionProxy, nullptr);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    ASSERT_EQ(
        WMError::WM_ERROR_IPC_FAILED,
        sessionProxy->UpdateSessionPropertyByAction(nullptr, WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    MockMessageParcel::ClearAllErrorFlag();

    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    ASSERT_EQ(
        WMError::WM_ERROR_IPC_FAILED,
        sessionProxy->UpdateSessionPropertyByAction(nullptr, WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    MockMessageParcel::ClearAllErrorFlag();

    sptr<WindowSessionProperty> property = sptr<WindowSessionProperty>::MakeSptr();
    ASSERT_NE(property, nullptr);
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    ASSERT_EQ(
        WMError::WM_ERROR_IPC_FAILED,
        sessionProxy->UpdateSessionPropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    MockMessageParcel::ClearAllErrorFlag();

    MockMessageParcel::SetWriteBoolErrorFlag(false);
    ASSERT_EQ(
        WMError::WM_OK,
        sessionProxy->UpdateSessionPropertyByAction(property, WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    MockMessageParcel::ClearAllErrorFlag();

    MockMessageParcel::SetWriteBoolErrorFlag(true);
    ASSERT_EQ(
        WMError::WM_ERROR_IPC_FAILED,
        sessionProxy->UpdateSessionPropertyByAction(nullptr, WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    MockMessageParcel::ClearAllErrorFlag();

    MockMessageParcel::SetWriteBoolErrorFlag(false);
    ASSERT_EQ(
        WMError::WM_OK,
        sessionProxy->UpdateSessionPropertyByAction(nullptr, WSPropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON));
    MockMessageParcel::ClearAllErrorFlag();
}
} // namespace
} // namespace Rosen
} // namespace OHOS
