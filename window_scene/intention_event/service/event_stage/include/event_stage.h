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

#include <list>
#include <map>
#include <vector>

#include "noncopyable.h"

namespace OHOS {
namespace Rosen {
class EventStage {
public:
    void SetAnrStatus(uint64_t persistentId, bool status);
    bool CheckAnrStatus(uint64_t persistentId);
    void SaveANREvent(uint64_t persistentId, int32_t id, int64_t time, int32_t timerId);
    std::vector<int32_t> GetTimerIds(uint64_t persistentId);
    std::list<int32_t> DelEvents(uint64_t persistentId, int32_t id);
    void OnSessionLost(uint64_t persistentId);
private:
    struct EventTime {
        int32_t id { 0 };
        int64_t eventTime { 0 };
        int32_t timerId { -1 };
    };
    std::map<uint64_t, std::vector<EventTime>> events_;
    std::map<uint64_t, bool> isAnrProcess_;
};
} // namespace Rosen
} // namespace OHOS
