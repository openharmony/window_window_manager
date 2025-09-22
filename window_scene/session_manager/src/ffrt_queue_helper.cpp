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

#include "ffrt_queue_helper.h"

#include <cstdint>

#include "ffrt_inner.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
constexpr int32_t FFRT_USER_INTERACTIVE_MAX_THREAD_NUM = 5;

FfrtQueueHelper::FfrtQueueHelper()
{
    ffrtQueue_ = std::make_unique<ffrt::queue>(ffrt::queue_concurrent, "FfrtQueueHelper",
        ffrt::queue_attr()
            .qos(ffrt_qos_user_interactive)
            .max_concurrency(FFRT_USER_INTERACTIVE_MAX_THREAD_NUM));
}

FfrtQueueHelper::~FfrtQueueHelper() = default;

// LCOV_EXCL_START
bool FfrtQueueHelper::SubmitTaskAndWait(std::function<void()>&& task, uint64_t timeout)
{
    auto timeoutFuture = std::make_shared<TimeoutFuture<bool>>();
    ffrtQueue_->submit([localTask = std::move(task), timeoutFuture, where = __func__] {
        if (localTask) {
            localTask();
        } else {
            TLOGNE(WmsLogTag::WMS_LIFE, "%{public}s: task function is empty", where);
        }
        timeoutFuture->FutureCall(true);
    }, __func__);
    bool isTimeout = false;
    timeoutFuture->GetResult(timeout, isTimeout);
    return isTimeout;
}

void FfrtQueueHelper::SubmitTask(std::function<void()>&& task)
{
    ffrtQueue_->submit(task);
}

void FfrtQueueHelper::SubmitTaskToHead(std::function<void()>&& task)
{
    ffrtQueue_->submit_head(task);
}
// LCOV_EXCL_STOP
} // namespace OHOS::Rosen
