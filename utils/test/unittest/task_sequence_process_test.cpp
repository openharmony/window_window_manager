/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "task_sequence_process.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class TaskSequenceProcessTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void TaskSequenceProcessTest::SetUpTestCase() {}

void TaskSequenceProcessTest::TearDownTestCase() {}

void TaskSequenceProcessTest::SetUp() {}

void TaskSequenceProcessTest::TearDown() {}

namespace {
/**
 * @tc.name: NotifyTest01
 * @tc.desc: NotifyTest01
 * @tc.type: FUNC
 */
HWTEST_F(TaskSequenceProcessTest, ATC_Notify01, TestSize.Level0)
{
    TaskSequenceProcess process = TaskSequenceProcess(10);
    process.SetTaskRunningFlag(false);
    TaskSequencEventInfo task;
    process.Push(task);
    process.Notify();
    EXPECT_TRUE(process.taskRunningFlag_.load());
}

/**
 * @tc.name: NotifyTest02
 * @tc.desc: NotifyTest02
 * @tc.type: FUNC
 */
HWTEST_F(TaskSequenceProcessTest, ATC_Notify02, TestSize.Level0)
{
    TaskSequenceProcess process = TaskSequenceProcess(10);
    process.SetTaskRunningFlag(false);
    process.Notify();
    EXPECT_FALSE(process.taskRunningFlag_.load());
}

/**
 * @tc.name: NotifyTest03
 * @tc.desc: NotifyTest03
 * @tc.type: FUNC
 */
HWTEST_F(TaskSequenceProcessTest, ATC_Notify03, TestSize.Level0)
{
    TaskSequenceProcess process = TaskSequenceProcess(10);
    process.SetTaskRunningFlag(true);
    TaskSequencEventInfo task;
    process.Push(task);
    process.Notify();
    EXPECT_TRUE(process.taskQueue_.size()==1);
}

/**
 * @tc.name: TaskSequenceProcessTest01
 * @tc.desc: TaskSequenceProcessTest01
 * @tc.type: FUNC
 */
HWTEST_F(TaskSequenceProcessTest, ATC_TaskSequenceProcess01, TestSize.Level0)
{
    TaskSequenceProcess process = TaskSequenceProcess(0);
    EXPECT_EQ(process.maxQueueSize_, 1);
}

/**
 * @tc.name: TaskSequenceProcessTest02
 * @tc.desc: TaskSequenceProcessTest02
 * @tc.type: FUNC
 */
HWTEST_F(TaskSequenceProcessTest, ATC_TaskSequenceProcess02, TestSize.Level0)
{
    TaskSequenceProcess process = TaskSequenceProcess(10);
    EXPECT_EQ(process.maxQueueSize_, 10);
    EXPECT_FALSE(process.taskRunningFlag_.load());
}

/**
 * @tc.name: ExecTest01
 * @tc.desc: ExecTest01
 * @tc.type: FUNC
 */
HWTEST_F(TaskSequenceProcessTest, ATC_Exec01, TestSize.Level0)
{
    TaskSequenceProcess process = TaskSequenceProcess(1);
    TaskSequencEventInfo task;
    bool taskInfoCalled = false;
    task.taskInfo = [&taskInfoCalled]() {
        taskInfoCalled =true;
    };
    process.Push(task);
    process.Notify();
    EXPECT_TRUE(taskInfoCalled);
}

/**
 * @tc.name: SetTaskRunningFlagTest01
 * @tc.desc: SetTaskRunningFlagTest01
 * @tc.type: FUNC
 */
HWTEST_F(TaskSequenceProcessTest, ATC_SetTaskRunningFlag01, TestSize.Level0)
{
    TaskSequenceProcess process = TaskSequenceProcess(1);
    bool flag = true;
    process.SetTaskRunningFlag(flag);
    EXPECT_EQ(process.taskRunningFlag_.load(), flag);
}

/**
 * @tc.name: PushTest01
 * @tc.desc: PushTest01
 * @tc.type: FUNC
 */
HWTEST_F(TaskSequenceProcessTest, ATC_Push01, TestSize.Level0)
{
    TaskSequenceProcess process = TaskSequenceProcess(1);
    TaskSequencEventInfo eventInfo1;
    TaskSequencEventInfo eventInfo2;
    process.Push(eventInfo1);
    process.Push(eventInfo2);
    EXPECT_EQ(process.taskQueue_.size(), 1);
}

/**
 * @tc.name: PushTest02
 * @tc.desc: PushTest02
 * @tc.type: FUNC
 */
HWTEST_F(TaskSequenceProcessTest, ATC_Push02, TestSize.Level0)
{
    TaskSequenceProcess process = TaskSequenceProcess(3);
    TaskSequencEventInfo eventInfo1;
    TaskSequencEventInfo eventInfo2;
    process.Push(eventInfo1);
    process.Push(eventInfo2);
    EXPECT_EQ(process.taskQueue_.size(), 2);
}
} // namespace
} // namespace Rosen
} // namespace OHOS