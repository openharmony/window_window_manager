/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "task_sequence_process.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
TaskSequenceProcess::TaskSequenceProcess(uint32_t maxQueueSize)
    : maxQueueSize_(maxQueueSize)
    {
        if (maxQueueSize_ <= 0) {
            maxQueueSize_ = 1;
        }
        taskRunningFlag_.store(false);
        TLOGI(WmsLogTag::DMS, "TaskSequenceProcess created with maxQueueSize: %{public}u", maxQueueSize_);
    }
TaskSequenceProcess::~TaskSequenceProcess() = default;

void TaskSequenceProcess::Notify()
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    TLOGI(WmsLogTag::DMS, "TaskSequenceProcess taskRunningFlag_: %{public}d, taskQueue.empty(): %{public}d",
        taskRunningFlag_.load(), taskQueue_.empty());
    if (!taskRunningFlag_.load() && !taskQueue_.empty()) {
        TaskSequenceEventInfo task = taskQueue_.front();
        taskQueue_.pop();
        taskRunningFlag_.store(true);
        try {
            Exec(task);
        } catch (const std::exception& e) {
            TLOGE(WmsLogTag::DMS, "TaskSequenceProcess exec failded:%{public}s", e.what());
        } catch (...) {
            TLOGE(WmsLogTag::DMS, "TaskSequenceProcess exec failded with unknown exception");
        }
        taskRunningFlag_.store(false);
    } else if (taskRunningFlag_.load()) {
    TLOGI(WmsLogTag::DMS, "TaskSequenceProcess notify task but full");
    } else TLOGI(WmsLogTag::DMS, "TaskSequenceProcess queue is empty");
}

void TaskSequenceProcess::Push(const TaskSequenceEventInfo& eventInfo)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    TLOGI(WmsLogTag::DMS, "TaskSequenceProcess push");
    if (taskQueue_.size() >= maxQueueSize_) {
        taskQueue_.pop();
    }
    taskQueue_.push(eventInfo);
}

void TaskSequenceProcess::SetTaskRunningFlag(bool flag)
{
    taskRunningFlag_.store(flag);
}

void TaskSequenceProcess::Exec(const TaskSequenceEventInfo& task)
{
    if (task.taskInfo) {
        task.taskInfo();
    }
    TLOGI(WmsLogTag::DMS, "TaskSequenceProcess execute task finish");
}
} // namespace OHOS::Rosen