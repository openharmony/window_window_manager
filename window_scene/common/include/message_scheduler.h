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

#ifndef OHOS_ROSEN_WINDOW_SCENE_MESSAGE_SCHEDULER_H
#define OHOS_ROSEN_WINDOW_SCENE_MESSAGE_SCHEDULER_H
#include "event_handler.h"
#include "window_manager_hilog.h"
namespace OHOS::Rosen {
#ifndef WS_CHECK_NULL_SCHE_VOID
#define WS_CHECK_NULL_SCHE_VOID(ptr, task)                                \
    do {                                                                  \
        if (!(ptr)) {                                                     \
            task();                                                       \
            return;                                                       \
        }                                                                 \
    } while (0)
#endif

#ifndef WS_CHECK_NULL_SCHE_RETURN
#define WS_CHECK_NULL_SCHE_RETURN(ptr, task)                              \
    do {                                                                  \
        if (!(ptr)) {                                                     \
            return task();                                                \
        }                                                                 \
    } while (0)
#endif

class MessageScheduler {
public:
    using Task = std::function<void()>;
    MessageScheduler(const std::string& threadName);
    MessageScheduler(const std::shared_ptr<AppExecFwk::EventHandler>& handler) : handler_(handler) {};
    ~MessageScheduler();
    void PostVoidSyncTask(Task task);
    void PostAsyncTask(Task task, int64_t delayTime = 0);
    template<typename SyncTask, typename Return = std::invoke_result_t<SyncTask>>
    Return PostSyncTask(SyncTask&& task)
    {
        Return ret;
        std::function<void()> syncTask([&ret, &task]() {ret = task();});
        if (handler_) {
            handler_->PostSyncTask(syncTask, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        }
        return ret;
    }
private:
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_MESSAGE_SCHEDULER_H