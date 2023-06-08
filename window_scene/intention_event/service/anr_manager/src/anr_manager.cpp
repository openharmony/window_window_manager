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
#include "anr_manager.h"

#include <algorithm>
#include <vector>

// #include "ability_manager_client.h"

#include "event_stage.h"
#include "proto.h"
#include "timer_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ANRManager" };
const std::string FOUNDATION = "foundation";
constexpr int32_t MAX_ANR_TIMER_COUNT = 50;
} // namespace

void ANRManager::Init()
{
   // TODO
   // 该函数需要在SceneBoard启动的时候执行
   // 这里需要注册ANRManager::OnSessionLost 到每一个 session， 使得有session死亡之后执行此处的OnSessionLost
}

void ANRManager::AddTimer(int32_t id, int64_t currentTime, int32_t persistentId)
{
    if (anrTimerCount_ >= MAX_ANR_TIMER_COUNT) {
        WLOGFD("AddAnrTimer failed, anrTimerCount exceeded %{public}d", MAX_ANR_TIMER_COUNT);
        return;
    }
    int32_t timerId = TimerMgr->AddTimer(ANRTimeOutTime::INPUT_UI_TIMEOUT_TIME, 1, [this, id, persistentId]() {
        EVStage->SetAnrStatus(persistentId, true);
        WLOGFE("Application not responding. persistentId:%{public}d, eventId:%{public}d", persistentId, id);
        // int32_t pid = GetPidByPersistentId(persistentId);
        // if (int32_t ret = AAFwk::AbilityManagerClient::GetInstance()->SendANRProcessID(pid); ret != ERR_OK) {
        //     WLOGFE("SendANRProcessedId failed, ret:%{public}d", ret);
        //     return;
        // }
        std::vector<int32_t> timerIds = EVStage->GetTimerIds(persistentId);
        for (int32_t item : timerIds) {
            if (item != -1) {
                TimerMgr->RemoveTimer(item);
                anrTimerCount_--;
                WLOGFD("Clear anr timer, timer id:%{public}d, count:%{public}d", item, anrTimerCount_);
            }
        }
    });
    anrTimerCount_++;
    EVStage->SaveANREvent(persistentId, id, currentTime, timerId);
}

int32_t ANRManager::MarkProcessed(int32_t eventId, int32_t persistentId)
{
    WLOGFD("eventId:%{public}d, persistentId:%{public}d", eventId, persistentId);
    std::list<int32_t> timerIds = EVStage->DelEvents(persistentId, eventId);
    for (int32_t item : timerIds) {
        if (item != -1) {
            TimerMgr->RemoveTimer(item);
            anrTimerCount_--;
            WLOGFD("Remove anr timer, eventId:%{public}d, timer id:%{public}d,"
                "count:%{public}d", eventId, item, anrTimerCount_);
        }
    }
    return 0;
}

bool ANRManager::IsANRTriggered(int64_t time, int32_t persistentId)
{
    if (EVStage->CheckAnrStatus(persistentId)) {
        WLOGFD("Application not responding. persistentId:%{public}d", persistentId);
        return true;
    }
    WLOGFD("Event dispatch normal");
    return false;
}

void ANRManager::RemoveTimers(int32_t persistentId)
{
    std::vector<int32_t> timerIds = EVStage->GetTimerIds(persistentId);
    for (int32_t item : timerIds) {
        if (item != -1) {
            TimerMgr->RemoveTimer(item);
            anrTimerCount_--;
        }
    }
}

void ANRManager::OnSessionLost(int32_t persistentId)
{
    RemoveTimers(persistentId);
}

void ANRManager::SetApplicationPid(int32_t persistentId, int32_t applicationPid)
{
    applicationMap_[persistentId] = applicationPid;
}

int32_t ANRManager::GetPidByPersistentId(int32_t persistentId)
{
    if (applicationMap_.find(persistentId) != applicationMap_.end()) {
        return applicationMap_[persistentId];
    }
    WLOGFE("No application matches persistentId:%{public}d", persistentId);
    return -1;
}

} // namespace Rosen
} // namespace OHOS