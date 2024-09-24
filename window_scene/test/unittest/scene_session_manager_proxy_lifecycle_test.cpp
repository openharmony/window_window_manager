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
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_;
};

void sceneSessionManagerProxyLifecycleTest::SetUpTestCase()
{
}

void sceneSessionManagerProxyLifecycleTest::TearDownTestCase()
{
}

void sceneSessionManagerProxyLifecycleTest::SetUp()
{
}

void sceneSessionManagerProxyLifecycleTest::TearDown()
{
}

namespace {
/**
 * @tc.name: UpdateSessionWindowVisibilityListener001
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyLifecycleTest, UpdateSessionWindowVisibilityListener001, Function | SmallTest | Level2)
{
    int32_t persistentId = 0;
    bool haveListener = true;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    ASSERT_EQ(WSError::WS_OK, sceneSessionManagerProxy_->UpdateSessionWindowVisibilityListener(persistentId,
        haveListener));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: PendingSessionToForeground
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyLifecycleTest, PendingSessionToForeground, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> token = nullptr;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED, sceneSessionManagerProxy_->PendingSessionToForeground(token));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: PendingSessionToBackgroundForDelegator
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyLifecycleTest, PendingSessionToBackgroundForDelegator, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> token = nullptr;
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    ASSERT_EQ(WSError::WS_ERROR_IPC_FAILED,
        sceneSessionManagerProxy_->PendingSessionToBackgroundForDelegator(token, true));
    sceneSessionManagerProxy_ = nullptr;
}

/**
 * @tc.name: GetVisibilityWindowInfo
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(sceneSessionManagerProxyLifecycleTest, GetVisibilityWindowInfo, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new (std::nothrow) IRemoteObjectMocker();
    sptr<SceneSessionManagerProxy> sceneSessionManagerProxy_ =
        new (std::nothrow) SceneSessionManagerProxy(iRemoteObjectMocker);
    EXPECT_NE(sceneSessionManagerProxy_, nullptr);

    std::vector<sptr<WindowVisibilityInfo>> infos;
    ASSERT_EQ(WMError::WM_OK, sceneSessionManagerProxy_->GetVisibilityWindowInfo(infos));
    sceneSessionManagerProxy_ = nullptr;
}
}  // namespace
}
}
