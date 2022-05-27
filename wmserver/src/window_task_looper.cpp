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

#include "window_task_looper.h"

namespace OHOS {
namespace Rosen {
void WindowTaskLooper::PostTask(const Task& task)
{
    {
        std::lock_guard<std::mutex> lock(mtx_);
        taskQ_.emplace_back(task);
    }
    WakeUp();
}

void WindowTaskLooper::WaitTask()
{
    std::unique_lock<std::mutex> lock(mtx_);
    if (!wakeUp_ && taskQ_.empty()) {
        taskCond_.wait(
            lock,
            [this] () {
                return !taskQ_.empty() || wakeUp_;
            });
    }
    wakeUp_ = false;
}

void WindowTaskLooper::WakeUp()
{
    std::lock_guard<std::mutex> lock(mtx_);
    wakeUp_ = true;
    taskCond_.notify_one();
}

void WindowTaskLooper::Start() noexcept
{
    taskProcessThread_ = std::make_unique<std::thread>([this]() {
        TaskProcessThread();
    });
    taskProcessThread_->detach();
}

void WindowTaskLooper::TaskProcessThread()
{
    if (running_) {
        return;
    }
    running_ = true;
    while (running_) {
        WaitTask();
        ExecuteAllTask();
    }
}

void WindowTaskLooper::ExecuteAllTask()
{
    std::vector<Task> tasks;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        tasks.swap(taskQ_);
    }
    for (const auto& task : tasks) {
        if (task != nullptr) {
            task();
        }
    }
}

bool WindowTaskLooper::IsRunning()
{
    return running_;
}

WindowTaskLooper::~WindowTaskLooper()
{
    std::lock_guard<std::mutex> lock(mtx_);
    running_ = false;
    taskQ_.clear();
}
} // namespace Rosen
} // namespace OHOS