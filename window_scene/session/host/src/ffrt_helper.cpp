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

#include "session/host/include/ffrt_helper.h"

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
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "FFRTHelper"};
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

struct FFRTHelperData {
    std::unordered_map<std::string, ffrt::task_handle> taskMap;
    std::shared_mutex mutex;
};

FFRTHelper::FFRTHelper()
{
    static bool firstInit = true;
    if (firstInit) {
        int ret = ffrt_set_cpu_worker_max_num(ffrt_qos_user_interactive, FFRT_USER_INTERACTIVE_MAX_THREAD_NUM);
        WLOGI("ffrt_set_cpu_worker_max_num(ffrt_qos_user_interactive), retcode = %{public}d", ret);
        firstInit = false;
    }
    data_ = std::make_shared<FFRTHelperData>();
}

void FFRTHelper::SubmitTask(const std::function<void()>& task, const std::string& taskName, uint64_t delayTime,
    TaskQos qos)
{
    pid_t tid = gettid();
    auto localTask = [task, taskName, tid] () {
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "%u:f:%s", tid, taskName.c_str());
        task();
    };
    if (delayTime == 0 && ffrt_get_cur_task() != nullptr) {
        localTask();
        return;
    }
    ffrt::task_handle handle = ffrt::submit_h(localTask, {}, {}, ffrt::task_attr().delay(delayTime).
        qos(FFRT_QOS_MAP.at(qos)));
    if (handle == nullptr) {
        WLOGE("Failed to post task, taskName = %{public}s", taskName.c_str());
        return;
    }
    std::unique_lock<std::shared_mutex> lock(data_->mutex);
    data_->taskMap[taskName] = std::move(handle);
}

void FFRTHelper::CancelTask(const std::string& taskName)
{
    std::unique_lock<std::shared_mutex> lock(data_->mutex);
    auto item = data_->taskMap.find(taskName);
    if (item == data_->taskMap.end()) {
        WLOGI("Task is not existed, taskName = %{public}s", taskName.c_str());
        return;
    }

    if (item->second != nullptr) {
        auto ret = ffrt::skip(item->second);
        if (ret != 0) {
            WLOGI("Failed to cancel task, taskName = %{public}s, retcode = %{public}d", taskName.c_str(), ret);
        }
    }
    data_->taskMap.erase(taskName);
}

bool FFRTHelper::IsTaskExisted(const std::string& taskName) const
{
    std::shared_lock<std::shared_mutex> lock(data_->mutex);
    auto item = data_->taskMap.find(taskName);
    if (item == data_->taskMap.end()) {
        return false;
    }
    return item->second != nullptr;
}

int FFRTHelper::CountTask() const
{
    std::shared_lock<std::shared_mutex> lock(data_->mutex);
    return data_->taskMap.size();
}

}