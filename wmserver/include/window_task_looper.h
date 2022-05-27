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


#ifndef OHOS_ROSEN_WINDOW_TASK_LOOPER_H
#define OHOS_ROSEN_WINDOW_TASK_LOOPER_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include <refbase.h>

namespace OHOS {
namespace Rosen {
template<typename SyncTask>
class ScheduledTask : public RefBase {
public:
    static auto Create(SyncTask&& task)
    {
        sptr<ScheduledTask<SyncTask>> t(new ScheduledTask(std::forward<SyncTask&&>(task)));
        return std::make_pair(t, t->task_.get_future());
    }

    void Run()
    {
        task_();
    }

private:
    explicit ScheduledTask(SyncTask&& task) : task_(std::move(task)) {}
    ~ScheduledTask() {}

    using Return = std::invoke_result_t<SyncTask>;
    std::packaged_task<Return()> task_;
};

class WindowTaskLooper {
public:
    using Task = std::function<void()>;
    WindowTaskLooper() = default;
    ~WindowTaskLooper();

    template<typename SyncTask, typename RetType = std::invoke_result_t<SyncTask>>
    std::future<RetType> ScheduleTask(SyncTask&& task)
    {
        auto [scheduledTask, taskFuture] = ScheduledTask<SyncTask>::Create(std::forward<SyncTask&&>(task));
        PostTask([t(std::move(scheduledTask))]() { t->Run(); });
        return std::move(taskFuture);
    }
    void PostTask(const Task& task);
    void WakeUp();
    void Start() noexcept;
    bool IsRunning();

private:
    void WaitTask();
    void TaskProcessThread();
    void ExecuteAllTask();

private:
    mutable std::mutex mtx_;
    bool wakeUp_ = false;
    std::condition_variable taskCond_;
    mutable std::atomic<bool> running_ { false };
    std::vector<Task> taskQ_;
    std::unique_ptr<std::thread> taskProcessThread_;
};
} // namespace Rosen
} // namespace OHOS
#endif