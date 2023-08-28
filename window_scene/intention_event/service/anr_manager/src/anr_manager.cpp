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

#include "dfx_hisysevent.h"
#include "entrance_log.h"
#include "proto.h"
#include "timer_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ANRManager" };
constexpr int32_t MAX_ANR_TIMER_COUNT { 64 };
constexpr int32_t NONEXISTENT_TIMER_ID { -1 };
} // namespace

ANRManager::ANRManager() {}
ANRManager::~ANRManager() {}

void ANRManager::Init()
{
    CALL_DEBUG_ENTER;
    SwitchAnr(true);
    DelayedSingleton<TimerManager>::GetInstance()->Init();
}

void ANRManager::AddTimer(int32_t eventId, int32_t persistentId)
{
    std::lock_guard<std::mutex> guard(mtx_);
    if (!switcher_) {
        WLOGFD("Anr is off, dispatch event without timer");
        return;
    }
    if (anrTimerCount_ >= MAX_ANR_TIMER_COUNT) {
        WLOGFD("AddAnrTimer failed, anrTimerCount exceeded %{public}d", MAX_ANR_TIMER_COUNT);
        return;
    }
    int32_t timerId = DelayedSingleton<TimerManager>::GetInstance()->AddTimer(ANRTimeOutTime::INPUT_UI_TIMEOUT_TIME,
        [this, eventId, persistentId]() {
        WLOGFE("Anr callback enter. persistentId:%{public}d, eventId:%{public}d", persistentId, eventId);
        DelayedSingleton<EventStage>::GetInstance()->SetAnrStatus(persistentId, true);
        AppInfo appInfo = GetAppInfoByPersistentId(persistentId);
        DfxHisysevent::ApplicationBlockInput(eventId, appInfo.pid, appInfo.bundleName, persistentId);
        WLOGFE("Application not responding. persistentId:%{public}d, eventId:%{public}d, pid:%{public}d, "
            "bundleName:%{public}s", persistentId, eventId, appInfo.pid, appInfo.bundleName.c_str());
        ExecuteAnrObserver(appInfo.pid);
        std::vector<int32_t> timerIds = DelayedSingleton<EventStage>::GetInstance()->GetTimerIds(persistentId);
        for (int32_t item : timerIds) {
            DelayedSingleton<TimerManager>::GetInstance()->RemoveTimer(item);
            anrTimerCount_--;
        }
        WLOGFE("Anr callback leave. persistentId:%{public}d, eventId:%{public}d", persistentId, eventId);
    });
    if (timerId == NONEXISTENT_TIMER_ID) {
        WLOGFD("AddTimer for eventId:%{public}d, persistentId:%{public}d failed, result from timerManager",
            eventId, persistentId);
        return;
    }
    anrTimerCount_++;
    DelayedSingleton<EventStage>::GetInstance()->SaveANREvent(persistentId, eventId, timerId);
    WLOGFD("AddTimer for persistentId:%{public}d, timerId:%{public}d, eventId:%{public}d",
        persistentId, timerId, eventId);
}

void ANRManager::MarkProcessed(int32_t eventId, int32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    WLOGFI("Event: eventId:%{public}d, persistentId:%{public}d", eventId, persistentId);
    std::vector<int32_t> timerIds = DelayedSingleton<EventStage>::GetInstance()->DelEvents(persistentId, eventId);
    for (int32_t item : timerIds) {
        DelayedSingleton<TimerManager>::GetInstance()->RemoveTimer(item);
        anrTimerCount_--;
    }
}

bool ANRManager::IsANRTriggered(int32_t persistentId)
{
    std::lock_guard<std::mutex> guard(mtx_);
    if (DelayedSingleton<EventStage>::GetInstance()->CheckAnrStatus(persistentId)) {
        WLOGFE("Application not respond, persistentId:%{public}d -> pid:%{public}d, bundleName:%{public}s",
            persistentId, applicationMap_[persistentId].pid, applicationMap_[persistentId].bundleName.c_str());
        return true;
    }
    return false;
}

void ANRManager::OnSessionLost(int32_t persistentId)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    WLOGFD("Disconnect session, persistentId:%{public}d -> pid:%{public}d, bundleName:%{public}s",
        persistentId, applicationMap_[persistentId].pid, applicationMap_[persistentId].bundleName.c_str());
    RemoveTimers(persistentId);
    RemovePersistentId(persistentId);
}

void ANRManager::SetApplicationInfo(int32_t persistentId, int32_t pid, const std::string& bundleName)
{
    std::lock_guard<std::mutex> guard(mtx_);
    WLOGFD("PersistentId:%{public}d -> pid:%{public}d, bundleName:%{public}s",
        persistentId, pid, bundleName.c_str());
    applicationMap_[persistentId] = { pid, bundleName };
}

void ANRManager::SetAnrObserver(std::function<void(int32_t)> anrObserver)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    anrObserver_ = anrObserver;
}

ANRManager::AppInfo ANRManager::GetAppInfoByPersistentId(int32_t persistentId)
{
    if (applicationMap_.find(persistentId) != applicationMap_.end()) {
        WLOGFD("PersistentId:%{public}d -> pid:%{public}d, bundleName:%{public}s",
            persistentId, applicationMap_[persistentId].pid, applicationMap_[persistentId].bundleName.c_str());
        return applicationMap_[persistentId];
    }
    WLOGFD("No application matches persistentId:%{public}d", persistentId);
    return ANRManager::AppInfo();
}

void ANRManager::RemoveTimers(int32_t persistentId)
{
    WLOGFD("Remove timers for persistentId:%{public}d", persistentId);
    std::vector<int32_t> timerIds = DelayedSingleton<EventStage>::GetInstance()->GetTimerIds(persistentId);
    for (int32_t item : timerIds) {
        DelayedSingleton<TimerManager>::GetInstance()->RemoveTimer(item);
        anrTimerCount_--;
    }
}

void ANRManager::RemovePersistentId(int32_t persistentId)
{
    applicationMap_.erase(persistentId);
    DelayedSingleton<EventStage>::GetInstance()->OnSessionLost(persistentId);
}

void ANRManager::SwitchAnr(bool status)
{
    switcher_ = status;
    if (switcher_) {
        WLOGFI("Anr is on");
    } else {
        WLOGFI("Anr is off");
    }
}

void ANRManager::SetAppInfoGetter(std::function<void(int32_t, std::string&, int32_t)> callback)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    appInfoGetter_ = callback;
}

std::string ANRManager::GetBundleName(int32_t pid, int32_t uid)
{
    CALL_DEBUG_ENTER;
    std::lock_guard<std::mutex> guard(mtx_);
    std::string bundleName { "unknow" };
    if (appInfoGetter_ == nullptr) {
        WLOGFE("AppInfoGetter is nullptr");
        return bundleName;
    }
    appInfoGetter_(pid, bundleName, uid);
    return bundleName;
}

void ANRManager::ExecuteAnrObserver(int32_t pid)
{
    CALL_DEBUG_ENTER;
    if (anrObserver_ != nullptr) {
        anrObserver_(pid);
    } else {
        WLOGFE("AnrObserver is nullptr, do nothing");
    }
}
} // namespace Rosen
} // namespace OHOS
