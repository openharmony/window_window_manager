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

#include "event_stage.h"

#include <algorithm>

#include "entrance_log.h"
#include "proto.h"
#include "util.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "EventStage" };
} // namespace

EventStage::EventStage() {}
EventStage::~EventStage() {}

void EventStage::SetAnrStatus(int32_t persistentId, bool status)
{
    std::lock_guard<std::mutex> lock(mutex_);
    isAnrProcess_[persistentId] = status;
}

bool EventStage::CheckAnrStatus(int32_t persistentId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (isAnrProcess_.find(persistentId) != isAnrProcess_.end()) {
        return isAnrProcess_[persistentId];
    }
    WLOGFD("Current persistentId:%{public}d is not in event stage", persistentId);
    return false;
}

void EventStage::SaveANREvent(int32_t persistentId, int32_t eventId, int32_t timerId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    EventTime eventTime { eventId, timerId };
    events_[persistentId].push_back(eventTime);
}

std::vector<int32_t> EventStage::GetTimerIds(int32_t persistentId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (events_.find(persistentId) == events_.end()) {
        WLOGFD("Current events have no event for persistentId:%{public}d", persistentId);
        return {};
    }
    std::vector<int32_t> timers;
    for (const auto &item : events_[persistentId]) {
        timers.push_back(item.timerId);
    }
    return timers;
}

std::vector<int32_t> EventStage::DelEvents(int32_t persistentId, int32_t eventId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    WLOGFD("Delete events, persistentId:%{public}d, eventId:%{public}d", persistentId, eventId);
    if (events_.find(persistentId) == events_.end()) {
        WLOGFD("Current events have no event persistentId:%{public}d", persistentId);
        return {};
    }
    auto &events = events_[persistentId];
    auto fistMatchIter = find_if(events.begin(), events.end(), [eventId](const auto &item) {
        return item.eventId > eventId;
    });
    std::vector<int32_t> timerIds;
    for (auto iter = events.begin(); iter != fistMatchIter; iter++) {
        timerIds.push_back(iter->timerId);
    }
    events.erase(events.begin(), fistMatchIter);
    isAnrProcess_[persistentId] = false;
    return timerIds;
}

void EventStage::OnSessionLost(int32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mutex_);
    events_.erase(persistentId);
    isAnrProcess_.erase(persistentId);
}
} // namespace Rosen
} // namespace OHOS
