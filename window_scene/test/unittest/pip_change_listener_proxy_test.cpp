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
#include "session_manager/include/zidl/pip_change_listener_proxy.h"
#include "mock_message_parcel.h"
#include "window_manager_hilog.h"

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
class PipChangeListenerProxyTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override;

private:
    sptr<MockIRemoteObject> iRemoteObjectMocker_;
    sptr<PipChangeListenerProxy> pipChgListenerProxy_;
};

void PipChangeListenerProxyTest::SetUp()
{
    iRemoteObjectMocker_ = sptr<MockIRemoteObject>::MakeSptr();
    ASSERT_NE(iRemoteObjectMocker_, nullptr);
    pipChgListenerProxy_ = sptr<PipChangeListenerProxy>::MakeSptr(iRemoteObjectMocker_);
    ASSERT_NE(pipChgListenerProxy_, nullptr);
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
}

void PipChangeListenerProxyTest::TearDown()
{
    MockMessageParcel::ClearAllErrorFlag();
    iRemoteObjectMocker_ = nullptr;
    pipChgListenerProxy_ = nullptr;
    LOG_SetCallback(nullptr);
}

namespace {
HWTEST_F(PipChangeListenerProxyTest, OnPipStart_ShouldFailed_WhenWriteInterfaceTokenFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    pipChgListenerProxy_->OnPipStart(1);
    GTEST_LOG_(INFO) << logMsg;
    EXPECT_TRUE(logMsg.find("Write interfaceToken failed") != std::string::npos);
}

HWTEST_F(PipChangeListenerProxyTest, OnPipStart_ShouldFailed_WhenWriteIntFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    pipChgListenerProxy_->OnPipStart(1);
    GTEST_LOG_(INFO) << logMsg;
    EXPECT_TRUE(logMsg.find("Write windowId failed") != std::string::npos);
}

HWTEST_F(PipChangeListenerProxyTest, OnPipStart_ShouldFailed_WhenRemoteObjIsNull, TestSize.Level1)
{
    sptr<PipChangeListenerProxy> pipChgListenerProxy = sptr<PipChangeListenerProxy>::MakeSptr(nullptr);
    ASSERT_NE(pipChgListenerProxy, nullptr);

    pipChgListenerProxy->OnPipStart(1);
    GTEST_LOG_(INFO) << logMsg;
    EXPECT_TRUE(logMsg.find("remote is null") != std::string::npos);
}

HWTEST_F(PipChangeListenerProxyTest, OnPipStart_ShouldFailed_WhenSendReqFailed, TestSize.Level1)
{
    iRemoteObjectMocker_->SetRequestResult(1);
    pipChgListenerProxy_->OnPipStart(1);
    GTEST_LOG_(INFO) << logMsg;
    EXPECT_TRUE(logMsg.find("SendRequest failed") != std::string::npos);
    iRemoteObjectMocker_->SetRequestResult(0);
}

HWTEST_F(PipChangeListenerProxyTest, OnPipStart_ShouldSuccess_WhenAllOperationsSucceed, TestSize.Level1)
{
    pipChgListenerProxy_->OnPipStart(1);
    GTEST_LOG_(INFO) << logMsg;
    EXPECT_TRUE(logMsg.empty());
}
} // namespace
} // Rosen
} // OHOS