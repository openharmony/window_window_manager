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

void ANRHandler::SetSessionStage(const wptr<ISessionStage> &sessionStage)
{
    std::lock_guard<std::mutex> guard(anrMtx_);
    sessionStage_ = sessionStage;
}

void ANRHandler::SetLastProcessedEventStatus(bool status)
{
    event_.sendStatus = status;
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
    if (event_.sendStatus) {
        return;
    }
    if (timeoutTime < MIN_MARK_PROCESS_DELAY_TIME) {
        SendEvent(0);
    } else {
        int64_t delayTime = 0;
        if (timeoutTime >= MAX_MARK_PROCESS_DELAY_TIME) {
            delayTime = MAX_MARK_PROCESS_DELAY_TIME / TIME_TRANSITION;
        } else {
            delayTime = timeoutTime / TIME_TRANSITION;
        }
        SendEvent(delayTime);
    }
}

int32_t ANRHandler::GetLastProcessedEventId()
{
    if (event_.lastEventId == INVALID_OR_PROCESSED_ID
        || event_.lastEventId < event_.lastReportId) {
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
    CALL_DEBUG_ENTER;
    int32_t eventId = GetLastProcessedEventId();
    if (eventId == INVALID_OR_PROCESSED_ID) {
        return;
    }
    WLOGFD("Processed eventId:%{public}d", eventId);
    if (sessionStage_ == nullptr) {
        WLOGFE("sessionStage is nullptr");
        return;
    }
    if (WSError ret = sessionStage_->MarkProcessed(eventId); ret != WSError::WS_OK) {
        WLOGFE("Send to sceneBoard failed, ret:%{public}d", ret);
    }
    SetLastProcessedEventStatus(false);
}

void ANRHandler::SendEvent(int64_t delayTime)
{
    SetLastProcessedEventStatus(true);
    if (eventHandler_ == nullptr) {
        WLOGFE("eventHandler is nullptr");
        return;
    }
    std::function<void()> eventFunc = std::bind(&ANRHandler::MarkProcessed, this);
    if (!eventHandler_->PostHighPriorityTask(eventFunc, delayTime)) {
        WLOGFE("Send dispatch event failed");
    }
}
} // namespace Rosen
} // namespace OHOS