/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "future_callback_stub.h"
#include "mock/mock_future_callback_stub.h"
#include "window_session_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class FutureCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    sptr<FutureCallbackStub> futureCallback_ = nullptr;
};

void FutureCallbackStubTest::SetUpTestCase()
{
}

void FutureCallbackStubTest::TearDownTestCase()
{
}

void FutureCallbackStubTest::SetUp()
{
    futureCallback_ = new (std::nothrow) FutureCallbackStubMocker();
    EXPECT_NE(nullptr, futureCallback_);
}

void FutureCallbackStubTest::TearDown()
{
    futureCallback_ = nullptr;
}
namespace {
/**
 * @tc.name: OnRemoteRequest01
 * @tc.desc: sessionStub OnRemoteRequest01
 * @tc.type: FUNC
 * @tc.require: #I6JLSI
 */
HWTEST_F(FutureCallbackStubTest, OnRemoteRequest01, Function | SmallTest | Level2)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    auto res = futureCallback_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    data.WriteInterfaceToken(u"OHOS.IFutureCallback");
    res = futureCallback_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
    code = 1;
    res = futureCallback_->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(ERR_NONE, res);
}
} // namespace
} // namespace Rosen
} // namespace OHOS