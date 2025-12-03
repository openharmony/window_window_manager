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

#include "ffrt_serial_queue_helper.h"

namespace OHOS::Rosen {

FfrtSerialQueueHelper::FfrtSerialQueueHelper()
{
    ffrtQueueHelper_ = std::make_shared<FfrtQueueHelper>();
}

void FfrtSerialQueueHelper::SubmitTask(std::function<void()>&& task)
{
    std::lock_guard<std::mutex> queueLock(taskQueueMutex_);
    taskQueue_.push(std::move(task));
    if (taskQueue_.size() == 1) {
        auto& firstTask = taskQueue_.front();
        SubmitCurrentTask(firstTask);
    }
}

void FfrtSerialQueueHelper::SubmitCurrentTask(std::function<void()>& task)
{
    auto taskWrapper = [this, task]() {
        task();
        {
            std::lock_guard<std::mutex> queueLock(taskQueueMutex_);
            if (taskQueue_.empty()) {
                return;
            }
            taskQueue_.pop();
            if (taskQueue_.empty()) {
                return;
            }
            auto& nextTask = taskQueue_.front();
            SubmitCurrentTask(nextTask);
        }
    };
    ffrtQueueHelper_->SubmitTask(taskWrapper);
}
} // namespace OHOS::Rosen