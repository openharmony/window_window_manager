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
#include "session/container/include/zidl/session_stage_proxy.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class SessionStageProxyLayoutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionStageProxy> sessionStage_ = sptr<SessionStageProxy>::MakeSptr(iRemoteObjectMocker);
};

void SessionStageProxyLayoutTest::SetUpTestCase() {}

void SessionStageProxyLayoutTest::TearDownTestCase() {}

void SessionStageProxyLayoutTest::SetUp() {}

void SessionStageProxyLayoutTest::TearDown() {}

namespace {
/**
 * @tc.name: NotifySingleHandTransformChange
 * @tc.desc: test function : NotifySingleHandTransformChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyLayoutTest, NotifySingleHandTransformChange, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SessionStageProxyLayoutTest: NotifySingleHandTransformChange start";
    SingleHandTransform singleHandTransform;
    sessionStage_->NotifySingleHandTransformChange(singleHandTransform);
    ASSERT_TRUE((sessionStage_ != nullptr));
    GTEST_LOG_(INFO) << "SessionStageProxyLayoutTest: NotifySingleHandTransformChange end";
}

/**
 * @tc.name: NotifyLayoutFinishAfterWindowModeChange
 * @tc.desc: test function : NotifyLayoutFinishAfterWindowModeChange
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyLayoutTest, NotifyLayoutFinishAfterWindowModeChange, TestSize.Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    GTEST_LOG_(INFO) << "SessionStageProxyLayoutTest: NotifyLayoutFinishAfterWindowModeChange start";
    MockMessageParcel::ClearAllErrorFlag();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    auto ret = sessionStage_->NotifyLayoutFinishAfterWindowModeChange(WindowMode::WINDOW_MODE_FULLSCREEN);
    EXPECT_EQ(WSError::WS_ERROR_IPC_FAILED, ret);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);

    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    ret = sessionStage_->NotifyLayoutFinishAfterWindowModeChange(WindowMode::WINDOW_MODE_FULLSCREEN);
    EXPECT_EQ(WSError::WS_ERROR_IPC_FAILED, ret);
    MockMessageParcel::SetWriteUint32ErrorFlag(false);

    sptr<SessionStageProxy> sessionStage = sptr<SessionStageProxy>::MakeSptr(nullptr);
    ret = sessionStage->NotifyLayoutFinishAfterWindowModeChange(WindowMode::WINDOW_MODE_FULLSCREEN);
    EXPECT_EQ(WSError::WS_ERROR_IPC_FAILED, ret);

    ret = sessionStage_->NotifyLayoutFinishAfterWindowModeChange(WindowMode::WINDOW_MODE_FULLSCREEN);
    EXPECT_EQ(WSError::WS_OK, ret);
    MockMessageParcel::ClearAllErrorFlag();
    GTEST_LOG_(INFO) << "SessionStageProxyLayoutTest: NotifyLayoutFinishAfterWindowModeChange end";
}
} // namespace
} // namespace Rosen
} // namespace OHOS