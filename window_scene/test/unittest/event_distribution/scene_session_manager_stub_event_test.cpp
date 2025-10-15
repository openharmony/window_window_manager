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
#include <message_option.h>
#include <message_parcel.h>

#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "session_manager/include/zidl/scene_session_manager_stub.h"
#include "mock/mock_message_parcel.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionManagerStubEventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SceneSessionManagerStub> stub_;
};

void SceneSessionManagerStubEventTest::SetUpTestCase()
{
}

void SceneSessionManagerStubEventTest::TearDownTestCase()
{
}

void SceneSessionManagerStubEventTest::SetUp()
{
    stub_ = sptr<SceneSessionManager>::MakeSptr();
    SceneSessionManager::GetInstance().isUserBackground_ = true;
}

void SceneSessionManagerStubEventTest::TearDown()
{
}

namespace {
/**
 * @tc.name: ProcessRemoteRequest
 * @tc.desc: test ProcessRemoteRequest
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubEventTest, ProcessRemoteRequest, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = { MessageOption::TF_SYNC };
    // Failed to obtain the command.
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    int res = stub_->ProcessRemoteRequest(
        static_cast<uint32_t>(ISceneSessionManager::SceneSessionManagerMessage::TRANS_ID_SEND_COMMAND_EVENT),
        data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
    MockMessageParcel::ClearAllErrorFlag();
}

/**
 * @tc.name: HandleSendCommonEvent
 * @tc.desc: test HandleSendCommonEvent
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubEventTest, HandleSendCommonEvent, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    
    // Failed to obtain the command.
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    int res = stub_->HandleSendCommonEvent(data, reply);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Failed to obtain the length.
    MockMessageParcel::AddInt32Cache(static_cast<uint32_t>(CommonEventCommand::LOCK_CURSOR));
    res = stub_->HandleSendCommonEvent(data, reply);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Failed to obtain datas.
    MockMessageParcel::AddInt32Cache(static_cast<uint32_t>(CommonEventCommand::LOCK_CURSOR));
    MockMessageParcel::AddInt32Cache(LOCK_CURSOR_LENGTH);
    res = stub_->HandleSendCommonEvent(data, reply);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Failed to lock cursor.
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    MockMessageParcel::AddInt32Cache(static_cast<uint32_t>(CommonEventCommand::LOCK_CURSOR));
    MockMessageParcel::AddInt32Cache(LOCK_CURSOR_LENGTH);
    MockMessageParcel::AddInt32Cache(1);
    MockMessageParcel::AddInt32Cache(1);
    res = stub_->HandleSendCommonEvent(data, reply);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Successded to lock cursor.
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    MockMessageParcel::AddInt32Cache(static_cast<uint32_t>(CommonEventCommand::LOCK_CURSOR));
    MockMessageParcel::AddInt32Cache(LOCK_CURSOR_LENGTH);
    MockMessageParcel::AddInt32Cache(1);
    MockMessageParcel::AddInt32Cache(1);
    res = stub_->HandleSendCommonEvent(data, reply);
    EXPECT_EQ(res, ERR_NONE);

    // Successded to unlock cursor.
    MockMessageParcel::AddInt32Cache(static_cast<uint32_t>(CommonEventCommand::UNLOCK_CURSOR));
    MockMessageParcel::AddInt32Cache(UNLOCK_CURSOR_LENGTH);
    MockMessageParcel::AddInt32Cache(1);
    res = stub_->HandleSendCommonEvent(data, reply);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // Perform default behavior.
    MockMessageParcel::AddInt32Cache(10000);
    MockMessageParcel::AddInt32Cache(0);
    res = stub_->HandleSendCommonEvent(data, reply);
    EXPECT_EQ(res, ERR_NONE);
    MockMessageParcel::ClearAllErrorFlag();
}
} // namespace
} // namespace Rosen
} // namespace OHOS

