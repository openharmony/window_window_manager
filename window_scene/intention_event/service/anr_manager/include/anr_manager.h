/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANR_MANAGER_H
#define ANR_MANAGER_H

#include <functional>
#include <mutex>
#include <unordered_map>

#include "nocopyable.h"
#include "singleton.h"

#include "event_stage.h"
#include "ws_common.h"

namespace OHOS {
namespace Rosen {

class ANRManager final {
    DECLARE_DELAYED_SINGLETON(ANRManager);
public:
    DISALLOW_COPY_AND_MOVE(ANRManager);
    void Init();
    void AddTimer(int32_t id, int64_t currentTime, uint64_t persistentId);
    void MarkProcessed(int32_t eventId, uint64_t persistentId);
    bool IsANRTriggered(int64_t time, uint64_t persistentId);
    void OnSessionLost(uint64_t persistentId);
    void SetApplicationPid(uint64_t persistentId, int32_t applicationPid);
    void SetAnrObserver(std::function<void(int32_t)> anrObserver);
private:
    int32_t GetPidByPersistentId(uint64_t persistentId);
    void RemoveTimers(uint64_t persistentId);
    void RemovePersistentId(uint64_t persistentId);
private:
    std::mutex mtx_;
    int32_t anrTimerCount_ { 0 };
    std::unordered_map<uint64_t, int32_t> applicationMap_;
    std::function<void(int32_t)> anrObserver_;
    EventStage eventStage_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ANR_MANAGER_H