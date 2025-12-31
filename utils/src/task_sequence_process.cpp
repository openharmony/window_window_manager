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

#include "task_sequence_process.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
TaskSequenceProcess::TaskSequenceProcess(uint32_t maxQueueSize, uint64_t maxTimeInterval)
    : maxQueueSize_(maxQueueSize), maxTimeInterval_(maxTimeInterval)
{
    if (maxQueueSize_ <= 0) {
        maxQueueSize_ = 1;
    }
    if (! CreateSysTimer()) {
        TLOGI(WmsLogTag::DMS, "TaskSequenceProcess created fail, maxQueueSize: %{public}u", maxQueueSize_);
    }
}

TaskSequenceProcess::~TaskSequenceProcess()
{
    DestroySysTimer();
};

std::function<void()> TaskSequenceProcess::PopFromQueue()
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    TLOGI(WmsLogTag::DMS, "TaskSequenceProcess taskQueue_.size(): %{public}zu", taskQueue_.size());
    if (taskQueue_.empty() || taskRunningFlag_.load()) {
        TLOGI(WmsLogTag::DMS, "TaskSequenceProcess do not pop");
        return nullptr;
    }
    std::function<void()> task = taskQueue_.front();
    taskQueue_.pop();
    return task;
}

void TaskSequenceProcess::PushToQueue(const std::function<void()>& task)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    if (taskQueue_.size() >= maxQueueSize_) {
        taskQueue_.pop();
    }
    taskQueue_.push(task);
}

void TaskSequenceProcess::ExecTask()
{
    std::function<void()> task = PopFromQueue();
    if (!task) {
        TLOGI(WmsLogTag::DMS, "TaskSequenceProcess do not execute");
        return;
    }
    taskRunningFlag_.store(true);
    StartSysTimer();
    task();
}

void TaskSequenceProcess::AddTask(const std::function<void()>& task)
{
    PushToQueue(task);
    ExecTask();
}

void TaskSequenceProcess::FinishTask()
{
    taskRunningFlag_.store(false);
    StopSysTimer();
    ExecTask();
}

bool TaskSequenceProcess::CreateSysTimer()
{
    TLOGI(WmsLogTag::DMS, "TaskSequenceProcess StartSysTimer");

    std::lock_guard<std::mutex> lock(timerMutex_);
    if (taskTimerId_ != 0) {
        TLOGI(WmsLogTag::DMS, "TaskTimerId is not zero");
        return false;
    }
    std::shared_ptr<WindowSysTimer> taskSysTimer = std::make_unique<WindowSysTimer>(false, maxTimeInterval_, false);
    std::function<void()> callback = [this]() { taskRunningFlag_.store(false); };
    taskSysTimer->SetCallbackInfo(callback);
    taskSysTimer->SetName("task_system_timer");
    taskTimerId_ = MiscServices::TimeServiceClient::GetInstance()->CreateTimer(taskSysTimer);

    return taskTimerId_ ? true : false;
}

bool TaskSequenceProcess::DestroySysTimer()
{
    std::lock_guard<std::mutex> lock(timerMutex_);
    bool result = MiscServices::TimeServiceClient::GetInstance()->DestroyTimer(taskTimerId_);
    taskTimerId_ = 0;
    TLOGI(WmsLogTag::DMS, "stop TaskSequenceProcess timer result %{public}u", result);
    return result;
}

bool TaskSequenceProcess::StartSysTimer()
{
    std::lock_guard<std::mutex> lock(timerMutex_);
    if (taskTimerId_ == 0) {
        TLOGI(WmsLogTag::DMS, "TaskTimerId is zero");
        return false;
    }
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::milliseconds(maxTimeInterval_);
    auto expireTime = currentTime + duration;
    uint64_t triggerTime = std::chrono::duration_cast<std::chrono::milliseconds>(expireTime.time_since_epoch()).count();
    bool result = MiscServices::TimeServiceClient::GetInstance()->StartTimer(taskTimerId_, triggerTime);
    TLOGI(WmsLogTag::DMS, "TaskSequenceProcess timer result %{public}u", result);
    return result;
}

bool TaskSequenceProcess::StopSysTimer()
{
    std::lock_guard<std::mutex> lock(timerMutex_);
    if (taskTimerId_ == 0) {
        TLOGW(WmsLogTag::DMS, "TaskSequenceProcess TaskTimerId is zero");
        return false;
    }
    bool result = MiscServices::TimeServiceClient::GetInstance()->StopTimer(taskTimerId_);
    TLOGI(WmsLogTag::DMS, "stop TaskSequenceProcess timer result %{public}u", result);
    return result;
}
} // namespace OHOS::Rosen