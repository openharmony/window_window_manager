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
#include "mock_message_parcel.h"
#include "proto.h"
#include "session/container/include/zidl/session_stage_proxy.h"
#include "string_wrapper.h"
#include "util.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionStageProxyRotationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionStageProxy> sessionStage_ = sptr<SessionStageProxy>::MakeSptr(iRemoteObjectMocker);
};

void SessionStageProxyRotationTest::SetUpTestCase() {}

void SessionStageProxyRotationTest::TearDownTestCase() {}

void SessionStageProxyRotationTest::SetUp() {}

void SessionStageProxyRotationTest::TearDown() {}

namespace {
/**
 * @tc.name: SetCurrentRotation
 * @tc.desc: test function : SetCurrentRotation
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyRotationTest, SetCurrentRotation, TestSize.Level1)
{
    int32_t currentRotation = 90;
    ASSERT_TRUE(sessionStage_ != nullptr);
    WSError res = sessionStage_->SetCurrentRotation(currentRotation);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: GetScreenNodeCount
 * @tc.desc: test function : GetScreenNodeCount
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyRotationTest, GetScreenNodeCount, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionStageProxyRotationTest: GetScreenNodeCount start";
    MockMessageParcel::ClearAllErrorFlag();
    sptr<MockIRemoteObject> remoteMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<SessionStageProxy> sessionStageProxy = sptr<SessionStageProxy>::MakeSptr(remoteMocker);

    // Case 1: Failed to write interface token
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    uint32_t nodeCount = 0;
    WSError errCode = sessionStageProxy->GetScreenNodeCount(nodeCount);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);

    // Case 2: remote is nullptr
    sptr<SessionStageProxy> nullProxy = sptr<SessionStageProxy>::MakeSptr(nullptr);
    errCode = nullProxy->GetScreenNodeCount(nodeCount);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);

    // Case 3: Failed to send request
    remoteMocker->SetRequestResult(ERR_TRANSACTION_FAILED);
    errCode = sessionStageProxy->GetScreenNodeCount(nodeCount);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    remoteMocker->SetRequestResult(ERR_NONE);

    // Case 4: Failed to read nodeCount
    MockMessageParcel::SetReadUint32ErrorFlag(true);
    errCode = sessionStageProxy->GetScreenNodeCount(nodeCount);
    EXPECT_EQ(errCode, WSError::WS_ERROR_IPC_FAILED);
    MockMessageParcel::SetReadUint32ErrorFlag(false);

    // Case 5: Success
    errCode = sessionStageProxy->GetScreenNodeCount(nodeCount);
    EXPECT_EQ(errCode, WSError::WS_OK);
    EXPECT_GE(nodeCount, 0);

    // Case 6: Verify nodeCount is updated on multiple calls
    uint32_t nodeCount2 = 0;
    errCode = sessionStageProxy->GetScreenNodeCount(nodeCount2);
    EXPECT_EQ(errCode, WSError::WS_OK);
    EXPECT_GE(nodeCount2, 0);

    MockMessageParcel::ClearAllErrorFlag();
    GTEST_LOG_(INFO) << "SessionStageProxyRotationTest: GetScreenNodeCount end";
}

/**
 * @tc.name: NotifyRotationChange
 * @tc.desc: test function : NotifyRotationChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyRotationTest, NotifyRotationChange, Function | SmallTest | Level1)
{
    ASSERT_TRUE(sessionStage_ != nullptr);
    RotationChangeInfo info = { RotationChangeType::WINDOW_WILL_ROTATE, 0, 0, { 0, 0, 2720, 1270 } };
    RotationChangeResult res = sessionStage_->NotifyRotationChange(info);
    ASSERT_EQ(0, res.windowRect_.width_);
}
} // namespace
} // namespace Rosen
} // namespace OHOS