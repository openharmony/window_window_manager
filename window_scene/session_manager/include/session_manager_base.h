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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_BASE_H
#define OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_BASE_H

#include <unistd.h>

#include "common/include/message_scheduler.h"
#include "interfaces/include/ws_common.h"
namespace OHOS::Rosen {
class SessionManagerBase {
public:
    SessionManagerBase() = default;
    virtual ~SessionManagerBase() = default;
    uint64_t inline GeneratePersistentId()
    {
        return (((uint64_t)pid_ << 32) | (++sessionId_)); // 32: high bit for uint64
    }
protected:
    std::shared_ptr<MessageScheduler> msgScheduler_ = nullptr;
    std::atomic<bool> mmsSchedulerInit_ = false;
private:
    int pid_ = getpid();
    // shared by scene session and extension session once in same process
    std::atomic<uint32_t> sessionId_ = 0;
};
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_WINDOW_SCENE_SESSION_MANAGER_BASE_H
