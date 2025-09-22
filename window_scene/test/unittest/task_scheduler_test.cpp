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

#include "common/include/task_scheduler.h"
#include <gtest/gtest.h>

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class TaskSchedulerTest : public testing::Test {
public:
    TaskSchedulerTest() {}
    ~TaskSchedulerTest() {}
};

namespace {
/**
 * @tc.name: task_scheduler_test001
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(TaskSchedulerTest, task_scheduler_test001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "TaskSchedulerTest: task_scheduler_test001 start";
    std::string threadName = "threadName";
    std::string name = "name";
    TaskScheduler* taskScheduler = new (std::nothrow) TaskScheduler(threadName);
    ASSERT_NE(taskScheduler, nullptr);
    taskScheduler->RemoveTask(name);
    delete taskScheduler;
    GTEST_LOG_(INFO) << "TaskSchedulerTest: task_scheduler_test001 end";
}

/**
 * @tc.name: GetEventHandler
 * @tc.desc: GetEventHandler function
 * @tc.type: FUNC
 */
HWTEST_F(TaskSchedulerTest, GetEventHandler, TestSize.Level1)
{
    std::string threadName = "threadName";
    std::shared_ptr<TaskScheduler> taskScheduler = std::make_shared<TaskScheduler>(threadName);
    ASSERT_NE(taskScheduler, nullptr);
    EXPECT_NE(taskScheduler->GetEventHandler(), nullptr);
}

/**
 * @tc.name: PostTask
 * @tc.desc: PostTask function
 * @tc.type: FUNC
 */
HWTEST_F(TaskSchedulerTest, PostTask, TestSize.Level1)
{
    std::string threadName = "threadName";
    std::shared_ptr<TaskScheduler> taskScheduler = std::make_shared<TaskScheduler>(threadName);
    ASSERT_NE(taskScheduler, nullptr);
    int resultValue = 0;
    auto taskFunc = [&resultValue]() {
        GTEST_LOG_(INFO) << "START_TASK";
        resultValue = 1;
    };
    taskScheduler->PostAsyncTask(taskFunc, "ssmTask");
    EXPECT_NE(taskScheduler->handler_, nullptr);
    EXPECT_EQ(resultValue, 0);

    int64_t delayTime = 1;
    taskScheduler->PostAsyncTask(taskFunc, "ssmTask", delayTime);
    EXPECT_EQ(resultValue, 0);
}

HWTEST_F(TaskSchedulerTest, AddExportTask1, TestSize.Level1)
{
    std::string threadName = "threadName";
    std::string funcName = "funcName";
    std::shared_ptr<TaskScheduler> taskScheduler = std::make_shared<TaskScheduler>(threadName);
    pid_t taskTid = 0;
    auto taskFunc = [&taskTid]() {
        GTEST_LOG_(INFO) << "START_TASK";
        taskTid = gettid();
    };
    ASSERT_NE(taskScheduler, nullptr);
    ASSERT_EQ(taskScheduler->exportFuncMap_.size(), 0);
    taskScheduler->AddExportTask(funcName, taskFunc);
    ASSERT_EQ(taskTid, gettid());
    ASSERT_EQ(taskScheduler->exportFuncMap_.size(), 0);
}

HWTEST_F(TaskSchedulerTest, AddExportTask2, TestSize.Level1)
{
    std::string threadName = "threadName";
    std::string funcName = "funcName";
    std::shared_ptr<TaskScheduler> taskScheduler = std::make_shared<TaskScheduler>(threadName);
    pid_t taskTid = 0;
    auto taskFunc = [&taskTid]() {
        GTEST_LOG_(INFO) << "START_TASK";
        taskTid = gettid();
    };
    ASSERT_NE(taskScheduler, nullptr);
    ASSERT_EQ(taskScheduler->exportFuncMap_.size(), 0);
    auto testTask = [taskScheduler, funcName, taskFunc] {
        taskScheduler->AddExportTask(funcName, taskFunc);
        return 0;
    };
    taskScheduler->PostSyncTask(testTask);
    ASSERT_EQ(taskTid, 0);
    ASSERT_NE(taskScheduler->exportFuncMap_.size(), 0);
}

HWTEST_F(TaskSchedulerTest, SetExportHandler, TestSize.Level1)
{
    std::string exportThreadName = "exportThread";
    auto eventRunner = AppExecFwk::EventRunner::Create(exportThreadName);
    auto eventHandler = std::make_shared<AppExecFwk::EventHandler>(eventRunner);
    std::string threadName = "threadName";
    std::shared_ptr<TaskScheduler> taskScheduler = std::make_shared<TaskScheduler>(threadName);
    taskScheduler->SetExportHandler(eventHandler);
    ASSERT_EQ(eventHandler.get(), taskScheduler->exportHandler_.get());
}

HWTEST_F(TaskSchedulerTest, ExecuteExportTask, TestSize.Level1)
{
    std::string threadName = "threadName";
    std::shared_ptr<TaskScheduler> taskScheduler = std::make_shared<TaskScheduler>(threadName);
    bool executed = false;
    std::string funcName = "funcName";
    auto taskFunc = [&executed]() {
        GTEST_LOG_(INFO) << "START_TASK";
        executed = true;
    };
    ASSERT_EQ(taskScheduler->exportFuncMap_.size(), 0);
    taskScheduler->ExecuteExportTask();
    ASSERT_EQ(taskScheduler->exportFuncMap_.size(), 0);
    ASSERT_EQ(executed, false);
    taskScheduler->exportFuncMap_["taskFunc"] = taskFunc;
    ASSERT_EQ(taskScheduler->exportFuncMap_.size(), 1);
    taskScheduler->ExecuteExportTask();

    std::string exportThreadName = "exportThread";
    auto eventRunner = AppExecFwk::EventRunner::Create(exportThreadName);
    auto eventHandler = std::make_shared<AppExecFwk::EventHandler>(eventRunner);
    taskScheduler->SetExportHandler(eventHandler);
    taskScheduler->ExecuteExportTask();
    ASSERT_EQ(taskScheduler->exportFuncMap_.size(), 0);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
