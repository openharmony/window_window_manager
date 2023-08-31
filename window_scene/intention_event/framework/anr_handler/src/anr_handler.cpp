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

#include <algorithm>
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
constexpr int64_t MAX_MARK_PROCESS_DELAY_TIME_US { 3000000 };
constexpr int64_t MARK_PROCESS_DELAY_TIME_BIAS_US { 1500000 };
constexpr int32_t INVALID_EVENT_ID { -1 };
constexpr int32_t INVALID_PERSISTENT_ID { -1 };
constexpr int32_t TIME_TRANSITION { 1000 };
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
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (sessionStage == nullptr) {
        WLOGFE("SessionStage of eventId:%{public}d is nullptr", eventId);
        sessionStageMap_[eventId] = { INVALID_PERSISTENT_ID, nullptr };
        return;
    }
    int32_t persistentId = sessionStage->GetPersistentId();
    sessionStageMap_[eventId] = { persistentId, sessionStage };
    WLOGFD("SetSessionStage for eventId:%{public}d, persistentId:%{public}d", eventId, persistentId);
}

void ANRHandler::HandleEventConsumed(int32_t eventId, int64_t actionTime)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    int32_t currentPersistentId = GetPersistentIdOfEvent(eventId);
    WLOGFD("Processed eventId:%{public}d, persistentId:%{public}d", eventId, currentPersistentId);
    if (IsOnEventHandler(currentPersistentId)) {
        UpdateLatestEventId(eventId);
        return;
    }
    int64_t currentTime = GetSysClockTime();
    int64_t timeoutTime = ANRTimeOutTime::INPUT_UI_TIMEOUT_TIME * TIME_TRANSITION - (currentTime - actionTime);
    WLOGFD("Processed eventId:%{public}d, persistentId:%{public}d, actionTime:%{public}" PRId64 ", "
        "currentTime:%{public}" PRId64 ", timeoutTime:%{public}" PRId64,
        eventId, currentPersistentId, actionTime, currentTime, timeoutTime);
    if (timeoutTime >= MAX_MARK_PROCESS_DELAY_TIME_US) {
        int64_t delayTime = std::min(timeoutTime - MARK_PROCESS_DELAY_TIME_BIAS_US, MAX_MARK_PROCESS_DELAY_TIME_US);
        SendEvent(eventId, delayTime / TIME_TRANSITION);
    } else {
        SendEvent(eventId, 0);
    }
}

void ANRHandler::OnWindowDestroyed(int32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (anrHandlerState_.sendStatus.find(persistentId) == anrHandlerState_.sendStatus.end()) {
        WLOGFE("PersistentId:%{public}d not in ANRHandler", persistentId);
        return;
    }
    anrHandlerState_.sendStatus.erase(persistentId);
    if (anrHandlerState_.eventsIterMap.find(persistentId) == anrHandlerState_.eventsIterMap.end()) {
        WLOGFE("PersistentId:%{public}d not in ANRHandler", persistentId);
        return;
    }
    anrHandlerState_.eventsIterMap.erase(persistentId);
    WLOGFD("PersistentId:%{public}d erased in ANRHandler", persistentId);
    for (const auto &elem : sessionStageMap_) {
        if (elem.second.persistentId == persistentId) {
            sessionStageMap_.erase(elem.first);
        }
    }
}

void ANRHandler::SetAnrHandleState(int32_t eventId, bool status)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    int32_t persistentId = GetPersistentIdOfEvent(eventId);
    anrHandlerState_.sendStatus[persistentId] = status;
    if (status) {
        auto iter = anrHandlerState_.eventsToReceipt.insert(anrHandlerState_.eventsToReceipt.end(), eventId);
        anrHandlerState_.eventsIterMap[persistentId] = iter;
    } else {
        anrHandlerState_.eventsToReceipt.pop_front();
        anrHandlerState_.eventsIterMap[persistentId] = anrHandlerState_.eventsToReceipt.end();
        ClearExpiredEvents(eventId);
    }
}

void ANRHandler::MarkProcessed()
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (anrHandlerState_.eventsToReceipt.empty()) {
        WLOGFE("Events to receipt is empty");
        SetAnrHandleState(INVALID_EVENT_ID, false);
        return;
    }
    int32_t eventId = anrHandlerState_.eventsToReceipt.front();
    WLOGFI("MarkProcessed eventId:%{public}d, persistentId:%{public}d", eventId, GetPersistentIdOfEvent(eventId));
    if (sessionStageMap_.find(eventId) == sessionStageMap_.end()) {
        WLOGFE("SessionStage for eventId:%{public}d is not in sessionStageMap", eventId);
    } else if (sessionStageMap_[eventId].sessionStage == nullptr) {
        WLOGFE("SessionStage for eventId:%{public}d is nullptr", eventId);
    } else if (WSError ret = sessionStageMap_[eventId].sessionStage->MarkProcessed(eventId); ret != WSError::WS_OK) {
        WLOGFE("Send to sceneBoard failed, ret:%{public}d", ret);
    }
    SetAnrHandleState(eventId, false);
}

void ANRHandler::SendEvent(int32_t eventId, int64_t delayTime)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    SetAnrHandleState(eventId, true);
    if (eventHandler_ == nullptr) {
        WLOGFE("EventHandler is nullptr");
        SetAnrHandleState(eventId, false);
        return;
    }
    if (!eventHandler_->PostHighPriorityTask(std::bind(&ANRHandler::MarkProcessed, this), delayTime)) {
        WLOGFE("Post eventId:%{public}d, delayTime:%{public}" PRId64 " failed", eventId, delayTime);
    }
    WLOGFD("Post eventId:%{public}d, delayTime:%{%{public}" PRId64 " on eventHandler successfully", eventId, delayTime);
}

void ANRHandler::ClearExpiredEvents(int32_t eventId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    int32_t persistentId = GetPersistentIdOfEvent(eventId);
    for (auto iter = sessionStageMap_.begin(); iter != sessionStageMap_.end();) {
        auto currentPersistentId = GetPersistentIdOfEvent(iter->first);
        if (iter->first < eventId &&
            (currentPersistentId == persistentId || currentPersistentId == INVALID_PERSISTENT_ID)) {
            sessionStageMap_.erase(iter++);
        } else {
            iter++;
        }
    }
}

int32_t ANRHandler::GetPersistentIdOfEvent(int32_t eventId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (sessionStageMap_.find(eventId) != sessionStageMap_.end()) {
        return sessionStageMap_[eventId].persistentId;
    }
    WLOGFE("No sessionStage for eventId:%{public}d", eventId);
    return INVALID_PERSISTENT_ID;
}

bool ANRHandler::IsOnEventHandler(int32_t persistentId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (anrHandlerState_.sendStatus.find(persistentId) != anrHandlerState_.sendStatus.end() &&
        anrHandlerState_.sendStatus[persistentId]) {
        return true;
    }
    return false;
}

void ANRHandler::UpdateLatestEventId(int32_t eventId)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto currentPersistentId = GetPersistentIdOfEvent(eventId);
    auto pendingEventIter = anrHandlerState_.eventsIterMap[currentPersistentId];
    if (pendingEventIter == anrHandlerState_.eventsToReceipt.end()) {
        WLOGFE("No pending event on eventsToReceipt");
        return;
    }
    if (eventId > *pendingEventIter) {
        WLOGFD("Replace eventId:%{public}d by newer eventId:%{public}d", *pendingEventIter, eventId);
        *pendingEventIter = eventId;
    }
}

} // namespace Rosen
} // namespace OHOS
