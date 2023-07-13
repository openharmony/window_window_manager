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

#include "anr_handler.h"

#include <cinttypes>
#include <functional>
#include <string>

#include "entrance_log.h"
#include "proto.h"
#include "util.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ANRHandler" };
constexpr int64_t MAX_MARK_PROCESS_DELAY_TIME = 3500000;
constexpr int64_t MIN_MARK_PROCESS_DELAY_TIME = 50000;
constexpr int32_t INVALID_OR_PROCESSED_ID = -1;
constexpr uint64_t INVALID_PERSISTENT_ID = -1;
constexpr int32_t TIME_TRANSITION = 1000;
const std::string ANR_HANDLER_RUNNER { "ANR_HANDLER" };
} // namespace

ANRHandler::ANRHandler()
{
    auto runner = AppExecFwk::EventRunner::Create(ANR_HANDLER_RUNNER);
    if (runner == nullptr) {
        WLOGFE("Create eventRunner failed");
        return;
    }
    eventHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
}

ANRHandler::~ANRHandler() {}

void ANRHandler::SetSessionStage(int32_t eventId, const wptr<ISessionStage> &sessionStage)
{
    std::lock_guard<std::mutex> guard(anrMtx_);
    sessionStageMap_[eventId] = sessionStage;
}

void ANRHandler::SetLastProcessedEventStatus(uint64_t persistentId, bool status)
{
    event_.sendStatus[persistentId] = status;
}

void ANRHandler::UpdateLastProcessedEventId(int32_t eventId)
{
    event_.lastEventId = eventId;
}

void ANRHandler::SetLastProcessedEventId(int32_t eventId, int64_t actionTime)
{
    std::lock_guard<std::mutex> guard(anrMtx_);
    if (event_.lastEventId > eventId) {
        WLOGFE("Event id %{public}d less then last processed lastEventId %{public}d", eventId, event_.lastEventId);
        return;
    }
    UpdateLastProcessedEventId(eventId);
    int64_t currentTime = GetSysClockTime();
    int64_t timeoutTime = ANRTimeOutTime::INPUT_UI_TIMEOUT_TIME * TIME_TRANSITION - (currentTime - actionTime);
    WLOGFD("Processed eventId:%{public}d, actionTime:%{public}" PRId64 ", "
        "currentTime:%{public}" PRId64 ", timeoutTime:%{public}" PRId64,
        eventId, actionTime, currentTime, timeoutTime);
    uint64_t persistentId = GetPersistentIdOfEvent(eventId);
    if (persistentId == INVALID_PERSISTENT_ID || event_.sendStatus[persistentId]) {
        return;
    }
    if (timeoutTime < MIN_MARK_PROCESS_DELAY_TIME) {
        SendEvent(eventId, 0);
    } else {
        int64_t delayTime = 0;
        if (timeoutTime >= MAX_MARK_PROCESS_DELAY_TIME) {
            delayTime = MAX_MARK_PROCESS_DELAY_TIME / TIME_TRANSITION;
        } else {
            delayTime = timeoutTime / TIME_TRANSITION;
        }
        SendEvent(eventId, delayTime);
    }
}

int32_t ANRHandler::GetLastProcessedEventId()
{
    if (event_.lastEventId == INVALID_OR_PROCESSED_ID || event_.lastEventId < event_.lastReportId) {
        WLOGFD("Invalid or processed, lastEventId:%{public}d, lastReportId:%{public}d",
            event_.lastEventId, event_.lastReportId);
        return INVALID_OR_PROCESSED_ID;
    }
    event_.lastReportId = event_.lastEventId;
    WLOGFD("Processed lastEventId:%{public}d, lastReportId:%{public}d", event_.lastEventId, event_.lastReportId);
    return event_.lastEventId;
}

void ANRHandler::MarkProcessed()
{
    int32_t eventId = GetLastProcessedEventId();
    uint64_t persistentId = GetPersistentIdOfEvent(eventId);
    if (eventId == INVALID_OR_PROCESSED_ID) {
        return;
    }
    WLOGFD("Processed eventId:%{public}d", eventId);
    if (sessionStageMap_.find(eventId) == sessionStageMap_.end()) {
        WLOGFE("sessionStage for eventId:%{public}d is not in sessionStageMap", eventId);
        return;
    }
    if (sessionStageMap_[eventId] == nullptr) {
        WLOGFE("sessionStage for eventId:%{public}d is nullptr", eventId);
        ClearExpiredEvents(persistentId, eventId);
        SetLastProcessedEventStatus(persistentId, false);
        return;
    }
    if (WSError ret = sessionStageMap_[eventId]->MarkProcessed(eventId); ret != WSError::WS_OK) {
        WLOGFE("Send to sceneBoard failed, ret:%{public}d", ret);
    }
    ClearExpiredEvents(persistentId, eventId);
    SetLastProcessedEventStatus(persistentId, false);
}

void ANRHandler::SendEvent(int32_t eventId, int64_t delayTime)
{
    uint64_t persistentId = GetPersistentIdOfEvent(eventId);
    SetLastProcessedEventStatus(persistentId, true);
    if (eventHandler_ == nullptr) {
        WLOGFE("eventHandler is nullptr");
        return;
    }
    if (!eventHandler_->PostHighPriorityTask(std::bind(&ANRHandler::MarkProcessed, this), delayTime)) {
        WLOGFE("Send dispatch event failed");
    }
}

void ANRHandler::ClearExpiredEvents(uint64_t persistentId, int32_t eventId)
{
    for (auto iter = sessionStageMap_.begin(); iter != sessionStageMap_.end();) {
        uint64_t currentPersistentId = GetPersistentIdOfEvent(iter->first);
        if (iter->first < eventId &&
            (currentPersistentId == persistentId || currentPersistentId == INVALID_PERSISTENT_ID)) {
            sessionStageMap_.erase(iter++);
        } else {
            iter++;
        }
    }
}

uint64_t ANRHandler::GetPersistentIdOfEvent(int32_t eventId)
{
    if (sessionStageMap_.find(eventId) == sessionStageMap_.end()) {
        WLOGFE("No sessionStage for eventId:%{public}d", eventId);
        return INVALID_PERSISTENT_ID;
    }
    auto sessionStage = sessionStageMap_[eventId];
    if (sessionStage == nullptr) {
        WLOGFE("SessionStage for eventId:%{public}d is nullptr", eventId);
        return INVALID_PERSISTENT_ID;
    }
    return sessionStage->GetPersistentId();
}

void ANRHandler::ClearDestroyedPersistentId(uint64_t persistentId)
{
    if (event_.sendStatus.find(persistentId) == event_.sendStatus.end()) {
        WLOGFE("PersistentId:%{public}" PRId64 " not in ANRHandler", persistentId);
        return;
    }
    event_.sendStatus.erase(persistentId);
}
} // namespace Rosen
} // namespace OHOS