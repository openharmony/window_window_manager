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

#ifndef OHOS_ROSEN_WINDOW_SCENE_TASK_SCHEDULER_H
#define OHOS_ROSEN_WINDOW_SCENE_TASK_SCHEDULER_H

#include <event_handler.h>

namespace OHOS::Rosen {
class TaskScheduler {
public:
    explicit TaskScheduler(const std::string& threadName);
    ~TaskScheduler() = default;

    std::shared_ptr<AppExecFwk::EventHandler> GetEventHandler();
    using Task = std::function<void()>;
    void PostAsyncTask(Task&& task, int64_t delayTime = 0);
    void PostVoidSyncTask(Task&& task);
    template<typename SyncTask, typename Return = std::invoke_result_t<SyncTask>>
    Return PostSyncTask(SyncTask&& task)
    {
        if (!handler_ || handler_->GetEventRunner()->IsCurrentRunnerThread()) {
            return task();
        }
        Return ret;
        auto syncTask = [&ret, &task]() { ret = task(); };
        handler_->PostSyncTask(std::move(syncTask), AppExecFwk::EventQueue::Priority::IMMEDIATE);
        return ret;
    }

private:
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_TASK_SCHEDULER_H
