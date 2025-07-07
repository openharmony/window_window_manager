/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "session/host/include/ws_ffrt_helper.h"

#include <limits>
#include <shared_mutex>
#include <unordered_map>

#include <unistd.h>

#include "c/executor_task.h"
#include "ffrt.h"
#include "ffrt_inner.h"
#include "hitrace_meter.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr int32_t FFRT_USER_INTERACTIVE_MAX_THREAD_NUM = 5;
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WSFFRTHelper"};
const std::unordered_map<TaskQos, ffrt::qos> FFRT_QOS_MAP = {
    { TaskQos::INHERIT, ffrt_qos_inherit },
    { TaskQos::BACKGROUND, ffrt_qos_background },
    { TaskQos::UTILITY, ffrt_qos_utility },
    { TaskQos::DEFAULT, ffrt_qos_default },
    { TaskQos::USER_INITIATED, ffrt_qos_user_initiated },
    { TaskQos::DEADLINE_REQUEST, ffrt_qos_deadline_request },
    { TaskQos::USER_INTERACTIVE, ffrt_qos_user_interactive },
};
}

class TaskHandleMap {
public:
    void SaveTask(const std::string& taskName, ffrt::task_handle handle)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        taskMap_[taskName] = std::move(handle);
    }
    
    void RemoveTask(const std::string& taskName, std::unique_ptr<ffrt::queue>& ffrtQueue)
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (auto iter = taskMap_.find(taskName); iter != taskMap_.end()) {
            if (iter->second != nullptr) {
                auto ret = ffrtQueue->cancel(iter->second);
                if (ret != 0) {
                    TLOGI(WmsLogTag::DEFAULT, "Failed, name=%{public}s, ret=%{public}d", taskName.c_str(), ret);
                }
            }
            taskMap_.erase(iter);
        } else {
            TLOGI(WmsLogTag::DEFAULT, "Task is not existed, name=%{public}s", taskName.c_str());
        }
    }

    bool IsTaskExisted(const std::string& taskName)
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto iter = taskMap_.find(taskName);
        return iter != taskMap_.end() && iter->second != nullptr;
    }

    std::size_t CountTask()
    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return taskMap_.size();
    }

private:
    std::unordered_map<std::string, ffrt::task_handle> taskMap_;
    std::shared_mutex mutex_;
};

WSFFRTHelper::WSFFRTHelper() : taskHandleMap_(std::make_unique<TaskHandleMap>())
{
    ffrtQueue_ = std::make_unique<ffrt::queue>(ffrt::queue_concurrent, "WSFFRTHelper",
        ffrt::queue_attr().qos(ffrt_qos_user_interactive).max_concurrency(FFRT_USER_INTERACTIVE_MAX_THREAD_NUM));
    TLOGI(WmsLogTag::WMS_MAIN, "FFRT user interactive qos max queue thread number: %{public}d",
        FFRT_USER_INTERACTIVE_MAX_THREAD_NUM);
}

WSFFRTHelper::~WSFFRTHelper() = default;

void WSFFRTHelper::SubmitTask(std::function<void()>&& task, const std::string& taskName, uint64_t delayTime,
    TaskQos qos)
{
    auto localTask = [task = std::move(task), taskName]() {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "f:%s", taskName.c_str());
        task();
    };
    if (delayTime == 0 && ffrt_get_cur_task() != nullptr) {
        localTask();
        return;
    }
    ffrt::task_handle handle = ffrtQueue_->submit_h(std::move(localTask));
    if (handle == nullptr) {
        WLOGE("Failed to post task, taskName=%{public}s", taskName.c_str());
        return;
    }
    taskHandleMap_->SaveTask(taskName, std::move(handle));
}

void WSFFRTHelper::CancelTask(const std::string& taskName)
{
    taskHandleMap_->RemoveTask(taskName, ffrtQueue_);
}

bool WSFFRTHelper::IsTaskExisted(const std::string& taskName) const
{
    return taskHandleMap_->IsTaskExisted(taskName);
}

std::size_t WSFFRTHelper::CountTask() const
{
    return taskHandleMap_->CountTask();
}
} // namespace OHOS::Rosen
