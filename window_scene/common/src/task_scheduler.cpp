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
#include "hitrace_meter.h"

namespace OHOS::Rosen {
TaskScheduler::TaskScheduler(const std::string& threadName)
{
    auto runner = AppExecFwk::EventRunner::Create(threadName);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
}

std::shared_ptr<AppExecFwk::EventHandler> TaskScheduler::GetEventHandler()
{
    return handler_;
}

void TaskScheduler::PostAsyncTask(Task&& task, const std::string& name, int64_t delayTime)
{
    auto localTask = [task, name] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:%s", name.c_str());
        task();
    };
    if (!handler_ || (delayTime == 0 && handler_->GetEventRunner()->IsCurrentRunnerThread())) {
        return localTask();
    }
    handler_->PostTask(std::move(localTask), "wms:" + name, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void TaskScheduler::PostVoidSyncTask(Task&& task, const std::string& name)
{
    auto localTask = [task, name] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:%s", name.c_str());
        task();
    };
    if (!handler_ || handler_->GetEventRunner()->IsCurrentRunnerThread()) {
        return localTask();
    }
    handler_->PostSyncTask(std::move(localTask), "wms:" + name, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void TaskScheduler::PostTask(Task&& task, const std::string& name, int64_t delayTime)
{
    auto localTask = [task, name] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ssm:%s", name.c_str());
        task();
    };
    if (!handler_ || (delayTime == 0 && handler_->GetEventRunner()->IsCurrentRunnerThread())) {
        return localTask();
    }
    handler_->PostTask(std::move(localTask), "wms:" + name, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
}

void TaskScheduler::RemoveTask(const std::string& name)
{
    if (handler_) {
        handler_->RemoveTask("wms:" + name);
    }
}

void StartTraceForSyncTask(std::string name)
{
    StartTraceArgs(HITRACE_TAG_WINDOW_MANAGER, "ssm:%s", name.c_str());
}
void FinishTraceForSyncTask()
{
    FinishTrace(HITRACE_TAG_WINDOW_MANAGER);
}
} // namespace OHOS::Rosen
