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
    if (!taskRunningFlag_.load() && !taskQueue_.empty()) {
        TaskSequenceEventInfo task = taskQueue_.front();
        taskQueue_.pop();
        taskRunningFlag_.store(true);
        Exec(task);
    } else if (taskRunningFlag_.load()) {
    TLOGI(WmsLogTag::DMS, "TaskSequenceProcess notify task but full");
    } else TLOGI(WmsLogTag::DMS, "TaskSequenceProcess queue is empty");
}

void TaskSequenceProcess::Push(const TaskSequenceEventInfo& eventInfo)
{
    TLOGI(WmsLogTag::DMS, "TaskSequenceProcess push");
    std::lock_guard<std::mutex> lock(queueMutex_);
    if (taskQueue_.size() >= maxQueueSize_) {
        taskQueue_.pop();
    }
    taskQueue_.push(eventInfo);
    Notify();
}

void TaskSequenceProcess::Finish()
{
    taskRunningFlag_.store(false);
    Notify();
}

void TaskSequenceProcess::Exec(const TaskSequenceEventInfo& task)
{
    task.task();
}
} // namespace OHOS::Rosen