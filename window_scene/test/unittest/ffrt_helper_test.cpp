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
#include <gtest/hwext/gtest-multithread.h>

#include <atomic>
#include <string>

#include "ffrt_helper.h"

using namespace testing::ext;
using namespace testing::mt;

namespace OHOS::Rosen {
static std::atomic<int> g_taskId(0);
static FFRTHelper* g_ffrtHelper = nullptr;
static constexpr size_t g_taskNum = 1000;

class FFRTHelperTest : public testing::Test {
protected:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    FFRTHelper* ffrtHelper;
};

void FFRTHelperTest::SetUpTestCase()
{
}

void FFRTHelperTest::TearDownTestCase()
{
}

void FFRTHelperTest::SetUp()
{
    ffrtHelper = new FFRTHelper();
}

void FFRTHelperTest::TearDown()
{
    delete ffrtHelper;
    ffrtHelper = nullptr;
}

/**
 * @tc.name: SubmitTask001
 * @tc.desc: test function SubmitTask
 * @tc.type: FUNC
 */
HWTEST_F(FFRTHelperTest, SubmitTask001, Function | SmallTest | Level2)
{
    ASSERT_NE(ffrtHelper, nullptr);

    auto mockTask = []() {};
    std::string taskName = "testTask";
    uint64_t delayTime = 0;
    TaskQos qos = TaskQos::USER_INTERACTIVE;

    ffrtHelper->SubmitTask(mockTask, taskName, delayTime, qos);
    bool result = ffrtHelper->IsTaskExisted(taskName);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: SubmitTask002
 * @tc.desc: test function SubmitTask in a multithreading case
 * @tc.type: FUNC
 */
HWTEST_F(FFRTHelperTest, SubmitTask002, Function | SmallTest | Level2)
{
    g_ffrtHelper = ffrtHelper;
    ASSERT_NE(g_ffrtHelper, nullptr);

    const int threadNum = 100;
    g_taskId.store(0);
    SET_THREAD_NUM(threadNum);
    auto submitTask = []() {
        for (size_t i = 0; i < g_taskNum; ++i) {
            auto mockTask = [](){};
            int id = g_taskId.fetch_add(1);
            std::string taskName = "testTask" + std::to_string(id);
            uint64_t delayTime = 0;
            TaskQos qos = TaskQos::USER_INTERACTIVE;
            g_ffrtHelper->SubmitTask(mockTask, taskName, delayTime, qos);
        }
    };
    GTEST_RUN_TASK(submitTask);

    int result = g_ffrtHelper->CountTask();
    ASSERT_EQ(result, g_taskNum * threadNum);
    g_ffrtHelper = nullptr;
}

/**
 * @tc.name: CancelTask001
 * @tc.desc: test function CancelTask
 * @tc.type: FUNC
 */
HWTEST_F(FFRTHelperTest, CancelTask001, Function | SmallTest | Level2)
{
    ASSERT_NE(ffrtHelper, nullptr);

    auto mockTask = [](){};
    std::string taskName = "testTask";
    uint64_t delayTime = 0;
    TaskQos qos = TaskQos::USER_INTERACTIVE;

    ffrtHelper->SubmitTask(mockTask, taskName, delayTime, qos);
    ffrtHelper->CancelTask(taskName);
    bool result = ffrtHelper->IsTaskExisted(taskName);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CancelTask002
 * @tc.desc: test function CancelTask in a multithreading case
 * @tc.type: FUNC
 */
HWTEST_F(FFRTHelperTest, CancelTask002, Function | SmallTest | Level2)
{
    g_ffrtHelper = ffrtHelper;
    ASSERT_NE(g_ffrtHelper, nullptr);

    const int totalTaskNum = 500000;
    for (size_t i = 0; i < totalTaskNum; ++i) {
        ffrtHelper->SubmitTask([](){}, "testTask" + std::to_string(i), 0, TaskQos::USER_INTERACTIVE);
    }
    
    const int threadNum = 10;
    g_taskId.store(0);
    SET_THREAD_NUM(threadNum);
    auto cancelTask = []() {
        for (size_t i = 0; i < g_taskNum; ++i) {
            int id = g_taskId.fetch_add(1);
            std::string taskName = "testTask" + std::to_string(id);
            g_ffrtHelper->CancelTask(taskName);
        }
    };
    GTEST_RUN_TASK(cancelTask);

    int result = g_ffrtHelper->CountTask();
    ASSERT_EQ(result, totalTaskNum - threadNum * g_taskNum);
    g_ffrtHelper = nullptr;
}

}