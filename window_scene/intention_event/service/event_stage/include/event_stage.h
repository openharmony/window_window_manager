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

#include <map>
#include <mutex>
#include <vector>

#include "noncopyable.h"
#include "singleton.h"

namespace OHOS {
namespace Rosen {
class EventStage final {
    DECLARE_DELAYED_SINGLETON(EventStage);
public:
    DISALLOW_COPY_AND_MOVE(EventStage);
    void SetAnrStatus(int32_t persistentId, bool status);
    bool CheckAnrStatus(int32_t persistentId);
    void SaveANREvent(int32_t persistentId, int32_t eventId, int32_t timerId);
    std::vector<int32_t> GetTimerIds(int32_t persistentId);
    std::vector<int32_t> DelEvents(int32_t persistentId, int32_t eventId);
    void OnSessionLost(int32_t persistentId);
private:
    struct EventTime {
        int32_t eventId { 0 };
        int32_t timerId { -1 };
    };
    std::mutex mutex_;
    std::map<int32_t, std::vector<EventTime>> events_;
    std::map<int32_t, bool> isAnrProcess_;
};
} // namespace Rosen
} // namespace OHOS
