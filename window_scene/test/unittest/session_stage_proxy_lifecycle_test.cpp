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

#include "session/container/include/zidl/session_stage_proxy.h"
#include "iremote_object_mocker.h"
#include <gtest/gtest.h>
#include "proto.h"
#include "string_wrapper.h"
#include "util.h"
#include "window_manager.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "mock_message_parcel.h"

using namespace testing;
using namespace testing::ext;

namespace {
    std::string logMsg;
    void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char* tag,
        const char* msg)
    {
        logMsg = msg;
    }
}

namespace OHOS {
namespace Rosen {
class SessionStageProxyLifecycleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<IRemoteObject> iRemoteObjectMocker = sptr<IRemoteObjectMocker>::MakeSptr();
    sptr<SessionStageProxy> sessionStage_ = sptr<SessionStageProxy>::MakeSptr(iRemoteObjectMocker);
};

void SessionStageProxyLifecycleTest::SetUpTestCase() {}

void SessionStageProxyLifecycleTest::TearDownTestCase() {}

void SessionStageProxyLifecycleTest::SetUp() {}

void SessionStageProxyLifecycleTest::TearDown() {}

namespace {
/**
 * @tc.name: SetActive
 * @tc.desc: test function : SetActive
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyLifecycleTest, SetActive, TestSize.Level1)
{
    bool active = false;
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->SetActive(active);
    ASSERT_EQ(WSError::WS_OK, res);
    active = true;
    ASSERT_TRUE((sessionStage_ != nullptr));
    res = sessionStage_->SetActive(active);
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifyDestroy
 * @tc.desc: test function : NotifyDestroy
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyLifecycleTest, NotifyDestroy, TestSize.Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    WSError res = sessionStage_->NotifyDestroy();
    ASSERT_EQ(WSError::WS_OK, res);
}

/**
 * @tc.name: NotifyForegroundInteractiveStatus
 * @tc.desc: test function : NotifyForegroundInteractiveStatus
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyLifecycleTest, NotifyForegroundInteractiveStatus, TestSize.Level1)
{
    bool interactive = true;
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->NotifyForegroundInteractiveStatus(interactive);
}

/**
 * @tc.name: NotifySessionForeground
 * @tc.desc: test function : NotifySessionForeground
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyLifecycleTest, NotifySessionForeground, TestSize.Level1)
{
    uint32_t reason = 1;
    bool withAnimation = true;
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->NotifySessionForeground(reason, withAnimation);
}

/**
 * @tc.name: NotifyLifecyclePausedStatus
 * @tc.desc: test function : NotifyLifecyclePausedStatus
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyLifecycleTest, NotifyLifecyclePausedStatus, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->NotifyLifecyclePausedStatus();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    sessionStage_->NotifyLifecyclePausedStatus();
    EXPECT_TRUE(logMsg.find("WriteInterfaceToken failed") != std::string::npos);

    sessionStage_->NotifyLifecyclePausedStatus();
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(false);
    EXPECT_TRUE(logMsg.find("WriteInterfaceToken failed") == std::string::npos);
}

/**
 * @tc.name: NotifySessionBackground
 * @tc.desc: test function : NotifySessionBackground
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyLifecycleTest, NotifySessionBackground, TestSize.Level1)
{
    uint32_t reason = 1;
    bool withAnimation = true;
    bool isFromInnerkits = true;
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->NotifySessionBackground(reason, withAnimation, isFromInnerkits);
}

/**
 * @tc.name: NotifyWindowVisibility
 * @tc.desc: test function : NotifyWindowVisibility
 * @tc.type: FUNC
 */
HWTEST_F(SessionStageProxyLifecycleTest, NotifyWindowVisibility, TestSize.Level1)
{
    ASSERT_TRUE((sessionStage_ != nullptr));
    sessionStage_->NotifyWindowVisibility(true);
}
} // namespace
} // namespace Rosen
} // namespace OHOS