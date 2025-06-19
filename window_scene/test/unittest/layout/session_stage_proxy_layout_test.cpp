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
 * @tc.name: UpdateWindowModeForUITest01
 * @tc.desc: test function : UpdateWindowModeForUITest
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyLayoutTest, UpdateWindowModeForUITest01, TestSize.Level1)
{
    TLOGI(WmsLogTag::WMS_LAYOUT, "SessionStageProxyLayoutTest: UpdateWindowModeForUITest01 start");
    GTEST_LOG_(INFO) << "SessionStageProxyLayoutTest: UpdateWindowModeForUITest01 start";
    const int32_t updateMode = 1;
    sptr<MockIRemoteObject> remoteMocker = sptr<MockIRemoteObject>::MakeSptr();
    sptr<SessionStageProxy> sessionStageProxy = sptr<SessionStageProxy>::MakeSptr(remoteMocker);

    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    WMError errCode = sessionStageProxy->UpdateWindowModeForUITest(updateMode);
    EXPECT_EQ(errCode, WMError::WM_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);

    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    errCode = sessionStageProxy->UpdateWindowModeForUITest(updateMode);
    EXPECT_EQ(errCode, WMError::WM_ERROR_IPC_FAILED);
    MockMessageParcel::SetWriteInt32ErrorFlag(false);

    remoteMocker->SetRequestResult(1);
    errCode = sessionStageProxy->UpdateWindowModeForUITest(updateMode);
    EXPECT_EQ(errCode, WMError::WM_ERROR_IPC_FAILED);
    remoteMocker->SetRequestResult(0);

    MockMessageParcel::SetReadInt32ErrorFlag(true);
    errCode = sessionStageProxy->UpdateWindowModeForUITest(updateMode);
    MockMessageParcel::SetReadInt32ErrorFlag(false);
    EXPECT_EQ(errCode, WMError::WM_OK);

    sptr<SessionStageProxy> sessionStageProxy2 = sptr<SessionStageProxy>::MakeSptr(nullptr);
    errCode = sessionStageProxy2->UpdateWindowModeForUITest(updateMode);
    EXPECT_EQ(errCode, WMError::WM_ERROR_IPC_FAILED);

    MockMessageParcel::ClearAllErrorFlag();
    TLOGI(WmsLogTag::WMS_LAYOUT, "SessionStageProxyLayoutTest: UpdateWindowModeForUITest01 end");
}
} // namespace
} // namespace Rosen
} // namespace OHOS