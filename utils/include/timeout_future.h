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

#ifndef OHOS_ROSEN_WINDOW_SCENE_TIMEOUT_FUTURE_H
#define OHOS_ROSEN_WINDOW_SCENE_TIMEOUT_FUTURE_H

#include <condition_variable>
#include "dms_global_mutex.h"

namespace OHOS::Rosen {
template<class T>
class TimeoutFuture {
public:
    T GetResult(long timeout, bool &isTimeout)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        isTimeout = !DmUtils::safe_wait_for(conditionVariable_, lock,
            std::chrono::milliseconds(timeout), [this] { return IsReady(); });
        return result_;
    }

    void FutureCall(T t)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        result_ = t;
        isReady_ = true;
        conditionVariable_.notify_one();
    }

private:
    inline bool IsReady() { return isReady_; }

    std::condition_variable conditionVariable_;
    std::mutex mutex_;
    bool isReady_ = false;
    T result_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_TIMEOUT_FUTURE_H
