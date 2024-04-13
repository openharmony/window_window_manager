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
#include <chrono>
#include <future>
#include <limits>

#include "pattern_detach_callback.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class PatternDetachCallbackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<PatternDetachCallback> patternDetachCallback_;
};

void PatternDetachCallbackTest::SetUpTestCase()
{
}

void PatternDetachCallbackTest::TearDownTestCase()
{
}

void PatternDetachCallbackTest::SetUp()
{
    patternDetachCallback_ = new (std::nothrow) PatternDetachCallback();
}

void PatternDetachCallbackTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetResult01
 * @tc.desc: GetResult Test
 * @tc.type: FUNC
 */
HWTEST_F(PatternDetachCallbackTest, GetResult01, Function | SmallTest | Level2)
{
    int32_t maxWaitTime = 300;
    auto startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    patternDetachCallback_->GetResult(maxWaitTime);
    auto endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    auto waitTime = endTime - startTime;
    GTEST_LOG_(INFO) << "GetResult waitTime:" << waitTime;
    ASSERT_TRUE(waitTime >= maxWaitTime);
}

/**
 * @tc.name: GetResult02
 * @tc.desc: GetResult Test
 * @tc.type: FUNC
 */
HWTEST_F(PatternDetachCallbackTest, GetResult02, Function | SmallTest | Level2)
{
    int32_t maxWaitTime = 300;
    int32_t sleepTime = 200;
    std::future<void> future = std::async(std::launch::async, [this, sleepTime]() -> void {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        patternDetachCallback_->OnPatternDetach(0);
    });
    auto startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    patternDetachCallback_->GetResult(maxWaitTime);
    auto endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    auto waitTime = endTime - startTime;
    GTEST_LOG_(INFO) << "GetResultAndOnPatternDetach waitTime:" << waitTime;
    ASSERT_TRUE(waitTime >= sleepTime && waitTime < maxWaitTime);
}

/**
 * @tc.name: GetResult03
 * @tc.desc: GetResult Test
 * @tc.type: FUNC
 */
HWTEST_F(PatternDetachCallbackTest, GetResult03, Function | SmallTest | Level2)
{
    int32_t maxWaitTime = 300;
    int32_t sleepTime = 400;
    std::future<void> future = std::async(std::launch::async, [this, sleepTime]() -> void {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        patternDetachCallback_->OnPatternDetach(0);
    });
    auto startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    patternDetachCallback_->GetResult(maxWaitTime);
    auto endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    auto waitTime = endTime - startTime;
    GTEST_LOG_(INFO) << "GetResultAndOnPatternDetach waitTime:" << waitTime;
    ASSERT_TRUE(waitTime >= maxWaitTime && waitTime < sleepTime);

    int resultValue = 0;
    std::function<void()> func = [this, &resultValue]() {
        patternDetachCallback_->GetResult(std::numeric_limits<int>::max());
        resultValue = 1;
    };
    func();
    ASSERT_EQ(resultValue, 1);
}

/**
 * @tc.name: OnRemoteRequest01
 * @tc.desc: OnRemoteRequest Test
 * @tc.type: FUNC
 */
HWTEST_F(PatternDetachCallbackTest, OnRemoteRequest01, Function | SmallTest | Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(u"error.GetDescriptor");

    uint32_t code = static_cast<uint32_t>(
        IPatternDetachCallback::PatternDetachCallbackMessage::TRANS_ID_PATTERN_ON_DETACH);
    int res = patternDetachCallback_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_STATE);
}

}
} // namespace Rosen
} // namespace OHOS