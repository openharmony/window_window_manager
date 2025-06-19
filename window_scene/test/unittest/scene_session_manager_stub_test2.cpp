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

#include "iremote_object_mocker.h"
#include "screen_session_manager_client/include/screen_session_manager_client.h"
#include "session_manager/include/scene_session_manager.h"
#include "session_manager/include/zidl/scene_session_manager_interface.h"
#include "session/container/include/window_event_channel.h"
#include "window_manager_agent.h"
#include "zidl/scene_session_manager_stub.h"
#include "zidl/window_manager_agent_interface.h"
#include "pattern_detach_callback.h"
#include "test/mock/mock_session_stage.h"
#include "mock/mock_message_parcel.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class SceneSessionManagerStubTest2 : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<SceneSessionManagerStub> stub_;
private:
    static constexpr uint32_t WAIT_SYNC_IN_NS = 200000;
};

void SceneSessionManagerStubTest2::SetUpTestCase()
{
}

void SceneSessionManagerStubTest2::TearDownTestCase()
{
}

void SceneSessionManagerStubTest2::SetUp()
{}
    stub_ = sptr<SceneSessionManager>::MakeSptr();
}

void SceneSessionManagerStubTest2::TearDown()
{
    usleep(WAIT_SYNC_IN_NS);
}

namespace {

/**
 * @tc.name: HandleIsFreeMultiWindow
 * @tc.desc: test HandleIsFreeMultiWindow
 * @tc.type: FUNC
 */
HWTEST_F(SceneSessionManagerStubTest2, HandleIsFreeMultiWindow, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    
    // writeInt32 failed
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int res = stub_->HandleIsFreeMultiWindow(data, reply);
    EXPECT_EQ(res, ERR_INVALID_DATA);
    MockMessageParcel::ClearAllErrorFlag();
    
    // writeBool failed
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    res = stub_->HandleIsFreeMultiWindow(data, reply);
    EXPECT_EQ(res, ERR_INVALID_DATA);

    // interface success
    MockMessageParcel::ClearAllErrorFlag();
    res = stub_->HandleIsFreeMultiWindow(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}

} // namespace
} // namespace Rosen
} // namespace OHOS

