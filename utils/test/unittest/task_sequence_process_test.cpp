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
    process.taskRunningFlag_.store(false);
    process.Notify();
    EXPECT_FALSE(process.taskRunningFlag_.load());
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
 * @tc.name: SetTaskRunningFlagTest01
 * @tc.desc: SetTaskRunningFlagTest01
 * @tc.type: FUNC
 */
HWTEST_F(TaskSequenceProcessTest, ATC_Finish01, TestSize.Level0)
{
    TaskSequenceProcess process = TaskSequenceProcess(1);
    process.taskRunningFlag_.store(true);
    process.Finish();
    EXPECT_EQ(process.taskRunningFlag_.load(), false);
}
} // namespace
} // namespace Rosen
} // namespace OHOS