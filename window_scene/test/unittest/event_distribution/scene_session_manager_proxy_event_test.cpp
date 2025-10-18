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
#include "iremote_object_mocker.h"
#include "mock/mock_message_parcel.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "session_manager/include/zidl/scene_session_manager_proxy.h"
#include "window_manager_agent.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class sceneSessionManagerProxyEventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObject> iRemoteObjectMocker;
};

void sceneSessionManagerProxyEventTest::SetUpTestCase() {}

void sceneSessionManagerProxyEventTest::TearDownTestCase() {}

void sceneSessionManagerProxyEventTest::SetUp() {}

void sceneSessionManagerProxyEventTest::TearDown() {}

namespace {
/**
 * @tc.name: SendCommonEvent
 * @tc.desc: SendCommonEvent
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyEventTest, SendCommonEvent, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = sptr<MockIRemoteObject>::MakeSptr();
    auto proxy = sptr<SceneSessionManagerProxy>::MakeSptr(remoteMocker);
    std::vector<int32_t> parameters;
    // Failed to test interface token.
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    auto ret = proxy->SendCommonEvent(1, parameters);
    EXPECT_EQ(ret, WMError::WM_ERROR_IPC_FAILED);

    // Failed to test write command.
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    ret = proxy->SendCommonEvent(1, parameters);
    EXPECT_EQ(ret, WMError::WM_ERROR_IPC_FAILED);

    // Failed to test write data.
    parameters.emplace_back(1);
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    MockMessageParcel::SetWriteInt32ErrorCount(1);
    ret = proxy->SendCommonEvent(1, parameters);
    EXPECT_EQ(ret, WMError::WM_ERROR_IPC_FAILED);

    // Failed to test remote.
    MockMessageParcel::ClearAllErrorFlag();
    auto ssmProxy = sptr<SceneSessionManagerProxy>::MakeSptr(nullptr);
    ret = ssmProxy->SendCommonEvent(1, parameters);
    EXPECT_EQ(ret, WMError::WM_ERROR_IPC_FAILED);

    // Failed to test the SetRequestResult execution.
    remoteMocker->SetRequestResult(ERR_INVALID_DATA);
    ret = proxy->SendCommonEvent(1, parameters);
    EXPECT_EQ(ret, WMError::WM_ERROR_IPC_FAILED);

    // Failed to test read ret.
    remoteMocker->SetRequestResult(ERR_NONE);
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    ret = proxy->SendCommonEvent(1, parameters);
    EXPECT_EQ(ret, WMError::WM_ERROR_IPC_FAILED);

    // The SetRequestResult test is successful.
    remoteMocker->SetRequestResult(ERR_NONE);
    MockMessageParcel::ClearAllErrorFlag();
    ret = proxy->SendCommonEvent(1, parameters);
    EXPECT_EQ(ret, static_cast<WMError>(0));
}
} // namespace
} // namespace Rosen
} // namespace OHOS
