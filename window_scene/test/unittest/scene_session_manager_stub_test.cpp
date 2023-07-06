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
#include <message_option.h>
#include <message_parcel.h>
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "window_manager_agent.h"
#include "zidl/scene_session_manager_stub.h"
#include "zidl/window_manager_agent_interface.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionManagerStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SceneSessionManagerStub> stub_;
};

void SceneSessionManagerStubTest::SetUpTestCase()
{
}

void SceneSessionManagerStubTest::TearDownTestCase()
{
}

void SceneSessionManagerStubTest::SetUp()
{
    stub_ = new SceneSessionManager();
}

void SceneSessionManagerStubTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnRemoteRequest01
 * @tc.desc: test TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, OnRemoteRequest01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(SceneSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        ISceneSessionManager::SceneSessionManagerMessage::TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name: OnRemoteRequest02
 * @tc.desc: test TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest, OnRemoteRequest02, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(SceneSessionManagerStub::GetDescriptor());
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_FOCUS;
    data.WriteUint32(static_cast<uint32_t>(type));
    sptr<IWindowManagerAgent> windowManagerAgent = new WindowManagerAgent();
    data.WriteRemoteObject(windowManagerAgent->AsObject());

    uint32_t code = static_cast<uint32_t>(
        ISceneSessionManager::SceneSessionManagerMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT);

    int res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, 0);
}

}
}
}