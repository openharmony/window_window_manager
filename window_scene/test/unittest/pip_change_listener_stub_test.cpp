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
#include "session_manager/include/zidl/pip_change_listener_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MockPipChangeListenerStub : public PipChangeListenerStub {
public:
    void OnPipStart(int32_t windowId) override {}
};

class PipChangeListenerStubTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
private:
    sptr<PipChangeListenerStub> stub_;
};

void PipChangeListenerStubTest::SetUp()
{
    stub_ = sptr<MockPipChangeListenerStub>::MakeSptr();
}

void PipChangeListenerStubTest::TearDown()
{
    stub_ = nullptr;
}

namespace {
HWTEST_F(PipChangeListenerStubTest, OnRemoteRequest_ShouldReturnFailed_WhenReadInterfaceTokenFails, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    uint32_t code =
        static_cast<uint32_t>(MockPipChangeListenerStub::IPipChangeListenerMessage::TRANS_ON_PIP_CHANGE_EVENT);
    auto res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_TRANSACTION_FAILED);
}

HWTEST_F(PipChangeListenerStubTest, OnRemoteRequest_ShouldReturnInvalidData_WhenReadIntFails, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    data.WriteInterfaceToken(PipChangeListenerStub::GetDescriptor());
    uint32_t code =
        static_cast<uint32_t>(MockPipChangeListenerStub::IPipChangeListenerMessage::TRANS_ON_PIP_CHANGE_EVENT);
    auto res = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_DATA);
}

HWTEST_F(PipChangeListenerStubTest, HandleOnPipChange_ShouldReturnErrNone_WhenAllOperationsSucceed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;

    data.WriteInt32(1);
    auto res = stub_->HandleOnPipChange(data, reply);
    EXPECT_EQ(res, ERR_NONE);
}
} // namespace
} // Rosen
} // OHOS