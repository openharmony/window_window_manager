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

#include "ws_ffrt_helper.h"

using namespace testing::ext;
using namespace testing::mt;

namespace OHOS::Rosen {
static std::atomic<int> g_taskId(0);
static WSFFRTHelper* g_wsFfrtHelper = nullptr;
static constexpr size_t TASK_NUM = 1000;

class WSFFRTHelperTest : public testing::Test {
protected:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    WSFFRTHelper* wsFfrtHelper_;
};

void WSFFRTHelperTest::SetUpTestCase()
{
}

void WSFFRTHelperTest::TearDownTestCase()
{
}

void WSFFRTHelperTest::SetUp()
{
    wsFfrtHelper_ = new WSFFRTHelper();
}

void WSFFRTHelperTest::TearDown()
{
    delete wsFfrtHelper_;
    wsFfrtHelper_ = nullptr;
}

/**
 * @tc.name: SubmitTask001
 * @tc.desc: test function SubmitTask
 * @tc.type: FUNC
 */
HWTEST_F(WSFFRTHelperTest, SubmitTask001, Function | SmallTest | Level2)
{
    ASSERT_NE(wsFfrtHelper_, nullptr);

    auto mockTask = []{};
    std::string taskName = "testTask";
    uint64_t delayTime = 0;
    TaskQos qos = TaskQos::USER_INTERACTIVE;

    wsFfrtHelper_->SubmitTask(mockTask, taskName, delayTime, qos);
    bool result = wsFfrtHelper_->IsTaskExisted(taskName);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: SubmitTask002
 * @tc.desc: test function SubmitTask in a multithreading case
 * @tc.type: FUNC
 */
HWTEST_F(WSFFRTHelperTest, SubmitTask002, Function | SmallTest | Level2)
{
    g_wsFfrtHelper = wsFfrtHelper_;
    ASSERT_NE(g_wsFfrtHelper, nullptr);

    constexpr int threadNum = 100;
    g_taskId.store(0);
    SET_THREAD_NUM(threadNum);
    auto submitTask = [] {
        for (size_t i = 0; i < TASK_NUM; ++i) {
            auto mockTask = []{};
            int id = g_taskId.fetch_add(1);
            std::string taskName = "testTask" + std::to_string(id);
            uint64_t delayTime = 0;
            TaskQos qos = TaskQos::USER_INTERACTIVE;
            g_wsFfrtHelper->SubmitTask(mockTask, taskName, delayTime, qos);
        }
    };
    GTEST_RUN_TASK(submitTask);

    int result = g_wsFfrtHelper->CountTask();
    ASSERT_EQ(result, TASK_NUM * threadNum);
    g_wsFfrtHelper = nullptr;
}

/**
 * @tc.name: CancelTask001
 * @tc.desc: test function CancelTask
 * @tc.type: FUNC
 */
HWTEST_F(WSFFRTHelperTest, CancelTask001, Function | SmallTest | Level2)
{
    ASSERT_NE(wsFfrtHelper_, nullptr);

    auto mockTask = []{};
    std::string taskName = "testTask";
    uint64_t delayTime = 0;
    TaskQos qos = TaskQos::USER_INTERACTIVE;

    wsFfrtHelper_->SubmitTask(mockTask, taskName, delayTime, qos);
    wsFfrtHelper_->CancelTask(taskName);
    bool result = wsFfrtHelper_->IsTaskExisted(taskName);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: CancelTask002
 * @tc.desc: test function CancelTask in a multithreading case
 * @tc.type: FUNC
 */
HWTEST_F(WSFFRTHelperTest, CancelTask002, Function | SmallTest | Level2)
{
    g_wsFfrtHelper = wsFfrtHelper_;
    ASSERT_NE(g_wsFfrtHelper, nullptr);

    constexpr int totalTaskNum = 50000;
    auto mockTask = []{};
    for (size_t i = 0; i < totalTaskNum; ++i) {
        wsFfrtHelper_->SubmitTask(mockTask, "testTask" + std::to_string(i), 0, TaskQos::USER_INTERACTIVE);
    }

    constexpr int threadNum = 10;
    g_taskId.store(0);
    SET_THREAD_NUM(threadNum);
    auto cancelTask = [] {
        for (size_t i = 0; i < TASK_NUM; ++i) {
            int id = g_taskId.fetch_add(1);
            std::string taskName = "testTask" + std::to_string(id);
            g_wsFfrtHelper->CancelTask(taskName);
        }
    };
    GTEST_RUN_TASK(cancelTask);

    int result = g_wsFfrtHelper->CountTask();
    ASSERT_EQ(result, totalTaskNum - threadNum * TASK_NUM);
    g_wsFfrtHelper = nullptr;
}

}