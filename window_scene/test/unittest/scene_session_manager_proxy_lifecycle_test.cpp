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
#include "mock/mock_session.h"
#include "mock/mock_session_stage.h"
#include "mock/mock_window_event_channel.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "session_manager/include/zidl/scene_session_manager_proxy.h"
#include "window_manager_agent.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class sceneSessionManagerProxyLifecycleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObject> iRemoteObjectMocker;
};

void sceneSessionManagerProxyLifecycleTest::SetUpTestCase() {}

void sceneSessionManagerProxyLifecycleTest::TearDownTestCase() {}

void sceneSessionManagerProxyLifecycleTest::SetUp() {}

void sceneSessionManagerProxyLifecycleTest::TearDown() {}

namespace {
/**
 * @tc.name: UpdateSessionWindowVisibilityListener001
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyLifecycleTest, UpdateSessionWindowVisibilityListener001, TestSize.Level1)
{
    int32_t persistentId = 0;
    bool haveListener = true;
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy =
        sptr<SceneSessionManagerProxy>::MakeSptr(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy, nullptr);

    ASSERT_EQ(WSError::WS_OK,
              sceneSessionManagerProxy->UpdateSessionWindowVisibilityListener(persistentId, haveListener));
}

/**
 * @tc.name: GetAllMainWindowInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyLifecycleTest, GetAllMainWindowInfo, TestSize.Level1)
{
    sptr<MockIRemoteObject> iRemoteObjectMocker = nullptr;
    sptr<SceneSessionManagerProxy> ssManagerProxy = sptr<SceneSessionManagerProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(ssManagerProxy, nullptr);
    DisplayId displayId = 0;
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    EXPECT_EQ(WMError::WM_ERROR_IPC_FAILED, ssManagerProxy->MinimizeAllAppWindows(displayId));
 
    iRemoteObjectMocker = sptr<MockIRemoteObject>::MakeSptr();
    ssManagerProxy = sptr<SceneSessionManagerProxy>::MakeSptr(iRemoteObjectMocker);
    ASSERT_NE(ssManagerProxy, nullptr);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    EXPECT_EQ(WMError::WM_ERROR_IPC_FAILED, ssManagerProxy->MinimizeAllAppWindows(displayId));
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    MockMessageParcel::SetWriteUint64ErrorFlag(true);
    EXPECT_EQ(WMError::WM_ERROR_IPC_FAILED, ssManagerProxy->MinimizeAllAppWindows(displayId));
    MockMessageParcel::SetWriteUint64ErrorFlag(false);
    EXPECT_EQ(WMError::WM_ERROR_IPC_FAILED, ssManagerProxy->MinimizeAllAppWindows(displayId));
 
    iRemoteObjectMocker->SetRequestResult(ERR_INVALID_DATA);
    EXPECT_EQ(WMError::WM_ERROR_IPC_FAILED, ssManagerProxy->MinimizeAllAppWindows(displayId));
    iRemoteObjectMocker->SetRequestResult(ERR_NONE);
    EXPECT_EQ(WMError::WM_ERROR_IPC_FAILED, ssManagerProxy->MinimizeAllAppWindows(displayId));
    MockMessageParcel::ClearAllErrorFlag();
}

/**
 * @tc.name: PendingSessionToForeground
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyLifecycleTest, PendingSessionToForeground, TestSize.Level1)
{
    sptr<IRemoteObject> token = nullptr;
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy =
        sptr<SceneSessionManagerProxy>::MakeSptr(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy, nullptr);

    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, sceneSessionManagerProxy->PendingSessionToForeground(token));
}

/**
 * @tc.name: PendingSessionToBackgroundForDelegator
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyLifecycleTest, PendingSessionToBackgroundForDelegator, TestSize.Level1)
{
    sptr<IRemoteObject> token = nullptr;
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy =
        sptr<SceneSessionManagerProxy>::MakeSptr(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy, nullptr);

    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED,
              sceneSessionManagerProxy->PendingSessionToBackgroundForDelegator(token, true));
}

/**
 * @tc.name: GetVisibilityWindowInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyLifecycleTest, GetVisibilityWindowInfo, TestSize.Level1)
{
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy =
        sptr<SceneSessionManagerProxy>::MakeSptr(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy, nullptr);

    std::vector<sptr<WindowVisibilityInfo>> infos;
    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy->GetVisibilityWindowInfo(infos));
}
} // namespace
} // namespace Rosen
} // namespace OHOS
