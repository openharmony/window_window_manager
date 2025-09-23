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

#ifndef WINDOW_WINDOW_MANAGER_UTILS_DMS_GLOBAL_MUTEX_H
#define WINDOW_WINDOW_MANAGER_UTILS_DMS_GLOBAL_MUTEX_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <thread>
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace DmUtils {
#define FREE_GLOBAL_LOCK_FOR_IPC() {DmUtils::DropLock ipcLock;}
class HoldLock {
public:
    HoldLock()
    {
        if (++counter() == 1) {
            while (!resMtx.try_lock_for(TRY_LOCK_TIMEOUT)) {
                if (CheckTimeout()) {
                    resMtx.unlock();
                }
            }
            lockTime = std::chrono::steady_clock::now();
            TLOGD(WmsLogTag::DMS, "timed_mutex:Lock acquired at time: %{public}lld",
                  std::chrono::duration_cast<std::chrono::milliseconds>(lockTime.time_since_epoch()).count());
        }
    }

    ~HoldLock()
    {
        if (--counter() == 0) {
            lockTime = std::chrono::steady_clock::now();
            TLOGD(WmsLogTag::DMS, "timed_mutex:Last instance released lock at: %{public}lld",
                  std::chrono::duration_cast<std::chrono::milliseconds>(lockTime.time_since_epoch()).count());
            resMtx.unlock();
        }
        TLOGD(WmsLogTag::DMS, "timed_mutex:~HoldLock: %{public}d", counter());
    }
    static bool ForceUnlock()
    {
        auto threadLocked = --counter() >= 0;
        if (threadLocked) {
            lockTime = std::chrono::steady_clock::now();
            resMtx.unlock();
        }
        return threadLocked;
    }

    static void ForceLock(bool hasLock)
    {
        ++counter();
        if (hasLock) {
            while (!resMtx.try_lock_for(TRY_LOCK_TIMEOUT)) {
                if (CheckTimeout()) {
                    resMtx.unlock();
                }
            }
        }
        lockTime = std::chrono::steady_clock::now();
    }

private:
    static std::chrono::steady_clock::time_point lockTime;
    static std::timed_mutex resMtx;
    static constexpr std::chrono::milliseconds TRY_LOCK_TIMEOUT{ 1000 };
    static constexpr std::chrono::milliseconds MAX_LOCK_INTERVAL{ 8 * TRY_LOCK_TIMEOUT };
    static int& counter()
    {
        static thread_local int _count = 0;
        return _count;
    }

    static bool CheckTimeout()
    {
        auto currentTime = std::chrono::steady_clock::now();
        auto currentMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch()).count();
        auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lockTime).count();
        if (interval > MAX_LOCK_INTERVAL.count()) {
            TLOGE(WmsLogTag::DMS, "timed_mutex:timeout currentTime:%{public}lld interval:%{public}lld",
                  currentMs, interval);
            return true;
        }
        return false;
    }
};

class DropLock {
public:
    DropLock()
    {
        hasLock = HoldLock::ForceUnlock();
    }

    ~DropLock()
    {
        HoldLock::ForceLock(hasLock);
    }

private:
    bool hasLock = true;
};

template <typename Func> auto wrap_callback(Func func)
{
    return [func](auto&&... args) {
        HoldLock callback_lock;
        func(std::forward<decltype(args)>(args)...);
    };
}

template <typename Rep, typename Period>
std::cv_status safe_wait_for(std::condition_variable& cv,
                             std::unique_lock<std::mutex>& lock,
                             const std::chrono::duration<Rep, Period>& rel_time)
{
    DropLock unlockGuard;
    auto status = cv.wait_for(lock, rel_time);
    return status;
}

template <typename Rep, typename Period, typename Predicate>
bool safe_wait_for(std::condition_variable& cv,
                   std::unique_lock<std::mutex>& lock,
                   const std::chrono::duration<Rep, Period>& rel_time,
                   Predicate pred)
{
    DropLock unlockGuard;
    bool result = cv.wait_for(lock, rel_time, pred);
    return result;
}

} // namespace DmUtils
} // namespace OHOS::Rosen

#define POST_LOCKED_TASK(handler, task, ...)              \
    handler->PostTask(                                    \
        [&]() {                                           \
            DmUtils::HoldLock postTask_lock;              \
            task();                                       \
        },                                                \
        ##__VA_ARGS__)
#endif // WINDOW_WINDOW_MANAGER_UTILS_DMS_GLOBAL_MUTEX_H