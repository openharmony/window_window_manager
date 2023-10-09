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
    PostTask([this, eventId, sessionStage]() {
        sptr<ISessionStage> session = sessionStage.promote();
        if (session == nullptr) {
            WLOGFE("SessionStage for eventId:%{public}d is nullptr", eventId);
            sessionStageMap_[eventId] = { INVALID_PERSISTENT_ID, nullptr };
            return;
        }
        int32_t persistentId = session->GetPersistentId();
        sessionStageMap_[eventId] = { persistentId, sessionStage };
        WLOGFD("SetSessionStage for eventId:%{public}d, persistentId:%{public}d", eventId, persistentId);
    });
}

void ANRHandler::HandleEventConsumed(int32_t eventId, int64_t actionTime)
{
    PostTask([this, eventId, actionTime]() {
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
    });
}

void ANRHandler::OnWindowDestroyed(int32_t persistentId)
{
    PostTask([this, persistentId]() {
        anrHandlerState_.sendStatus.erase(persistentId);
        anrHandlerState_.eventsIterMap.erase(persistentId);
        for (auto iter = sessionStageMap_.begin(); iter != sessionStageMap_.end();) {
            if (iter->second.persistentId == persistentId) {
                iter = sessionStageMap_.erase(iter);
            } else {
                ++iter;
            }
        }
        WLOGFD("PersistentId:%{public}d and its events erased in ANRHandler", persistentId);
    });
}

bool ANRHandler::PostTask(Task&& task, int64_t delayTime)
{
    if (eventHandler_ == nullptr) {
        WLOGFE("EventHandler is nullptr");
        return false;
    }
    if (eventHandler_->GetEventRunner()->IsCurrentRunnerThread()) {
        WLOGFE("Already running on eventHandler");
        task();
        return true;
    }
    if (!eventHandler_->PostTask(std::move(task), delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE)) {
        WLOGFE("PostTask failed");
        return false;
    }
    return true;
}

void ANRHandler::SetAnrHandleState(int32_t eventId, bool status)
{
    CALL_DEBUG_ENTER;
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
    if (anrHandlerState_.eventsToReceipt.empty()) {
        WLOGFE("Events to receipt is empty");
        SetAnrHandleState(INVALID_EVENT_ID, false);
        return;
    }
    int32_t eventId = anrHandlerState_.eventsToReceipt.front();
    WLOGFI("MarkProcessed eventId:%{public}d, persistentId:%{public}d", eventId, GetPersistentIdOfEvent(eventId));
    if (sessionStageMap_.find(eventId) == sessionStageMap_.end()) {
        WLOGFE("SessionStage for eventId:%{public}d is not in sessionStageMap", eventId);
    } else {
        sptr<ISessionStage> session = sessionStageMap_[eventId].sessionStage.promote();
        if (session == nullptr) {
            WLOGFE("SessionStage for eventId:%{public}d is nullptr", eventId);
        } else if (WSError ret = session->MarkProcessed(eventId); ret != WSError::WS_OK) {
            WLOGFE("Send to sceneBoard failed, ret:%{public}d", ret);
        }
    }
    SetAnrHandleState(eventId, false);
}

void ANRHandler::SendEvent(int32_t eventId, int64_t delayTime)
{
    CALL_DEBUG_ENTER;
    SetAnrHandleState(eventId, true);
    if (eventHandler_ != nullptr &&
        eventHandler_->PostHighPriorityTask([this]() {
            MarkProcessed();
        }, delayTime)) {
        WLOGFD("Post eventId:%{public}d, delayTime:%{public}" PRId64 " successfully", eventId, delayTime);
    } else {
        WLOGFE("Post eventId:%{public}d, delayTime:%{public}" PRId64 " failed", eventId, delayTime);
        SetAnrHandleState(eventId, false);
    }
}

void ANRHandler::ClearExpiredEvents(int32_t eventId)
{
    CALL_DEBUG_ENTER;
    int32_t persistentId = GetPersistentIdOfEvent(eventId);
    for (auto iter = sessionStageMap_.begin(); iter != sessionStageMap_.end();) {
        auto currentPersistentId = GetPersistentIdOfEvent(iter->first);
        if (iter->first < eventId &&
            (currentPersistentId == persistentId || currentPersistentId == INVALID_PERSISTENT_ID)) {
            iter = sessionStageMap_.erase(iter);
        } else {
            ++iter;
        }
    }
}

int32_t ANRHandler::GetPersistentIdOfEvent(int32_t eventId)
{
    if (sessionStageMap_.find(eventId) != sessionStageMap_.end()) {
        return sessionStageMap_[eventId].persistentId;
    }
    WLOGFE("No sessionStage for eventId:%{public}d", eventId);
    return INVALID_PERSISTENT_ID;
}

bool ANRHandler::IsOnEventHandler(int32_t persistentId)
{
    if (anrHandlerState_.sendStatus.find(persistentId) != anrHandlerState_.sendStatus.end() &&
        anrHandlerState_.sendStatus[persistentId]) {
        return true;
    }
    return false;
}

void ANRHandler::UpdateLatestEventId(int32_t eventId)
{
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
