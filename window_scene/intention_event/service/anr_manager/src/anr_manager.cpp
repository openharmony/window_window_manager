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

#include <vector>

#include "event_stage.h"
#include "timer_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ANRManager" };
const std::string FOUNDATION = "foundation";
constexpr int32_t MAX_ANR_TIMER_COUNT = 50;
} // namespace

void ANRManager::Init()
{
    // 在服务启动的时候执行，主要是初始化用于通信的session
    /**
     * SetAnrObserver 传下来的pid是用于向上通知ANR消息的，但是MarkProcessed传下来的pid则是消费者对应的pid，不是同一个东西
     * 通过MarkProcessed 传递下来的pid能够将该pid对应进程的对应类型的事件删除
     * AddTimer 在sceneBoard从多模拿到事件之后就执行
     * MarkProcessed应该绑定到inputEvent->MarkProcessed 中，通过SetProcessedCallback
     * IsANRTriggered 在事件上报过程中调用，如果该类型事件已经发生ANR，就不上报该类型事件
    */
   // TODO
   // 该函数需要在SceneBoard启动的时候执行
   // 这里需要注册OnSessionLost 到SessionManager， 使得有session死亡之后执行此处的OnSessionLost

}

void ANRManager::AddTimer(int32_t id, int64_t currentTime, int32_t persistentId)
{
    if (anrTimerCount_ >= MAX_ANR_TIMER_COUNT) {
        WLOGFD("Add anr timer failed, anrtimer count reached the maximum number:%{public}d", MAX_ANR_TIMER_COUNT);
        return;
    }
    int32_t timerId = TimerMgr->AddTimer(INPUT_UI_TIMEOUT_TIME, 1, [this, id, persistentId]() {
        EventStageSingleton->SetAnrStatus(persistentId, true);
        WLOGFE("Application not responding. persistentId:%{public}d, eventId:%{public}d", persistentId, id);
        // TODO 
        // 通过 anrNoticedPid_ 发送消息向上通知ANR，说是这里会提供一个接口
        std::vector<int32_t> timerIds = EventStageSingleton->GetTimerIds(persistentId);
        for (auto id : timerIds) {
            if (id != -1) {
                TimerMgr->RemoveTimer(id);
                anrTimerCount_--;
                WLOGFD("Clear anr timer, timer id:%{public}d, count:%{public}d", id, anrTimerCount_);
            }
        }
    });
    anrTimerCount_++;
    EventStageSingleton->SaveANREvent(persistentId, id, currentTime, timerId);
}

int32_t ANRManager::MarkProcessed(int32_t eventId, int32_t persistentId)
{
    // 这个在事件被正常消费之后通过客户端的 MarkProcessed 调用下来，内部执行对应定时器清理的工作
    WLOGFD("eventId:%{public}d, persistentId:%{public}d", eventId, persistentId);
    std::list<int32_t> timerIds = EventStageSingleton->DelEvents(persistentId, eventId);
    for (int32_t item : timerIds) {
        if (item != -1) {
            TimerMgr->RemoveTimer(item);
            anrTimerCount_--;
            WLOGFD("Remove anr timer, eventId:%{public}d, timer id:%{public}d,"
                "count:%{public}d", eventId, item, anrTimerCount_);
        }
    }
    return RET_OK;
}

bool ANRManager::IsANRTriggered(int64_t time, int32_t persistentId)
{
    // 在事件上报的时候呗调用，如果当前该类型的事件在某进程中已经发生了ANR，则不将该类型事件继续上报
    if (EventStageSingleton->CheckAnrStatus(persistentId)) {
        WLOGFD("Application not responding. persistentId:%{public}d", persistentId);
        return true;
    }
    WLOGFD("Event dispatch normal");
    return false;
}

void ANRManager::RemoveTimers(int32_t persistentId)
{
    // 在OnSessionLost的时候被调用，删除当前session对应的所有ANR定时器
    std::vector<int32_t> timerIds = EventStageSingleton->GetTimerIds(persistentId);
    for (int32_t item : timerIds) {
        if (item != -1) {
            TimerMgr->RemoveTimer(item);
            anrTimerCount_--;
        }
    }
}

void ANRManager::OnSessionLost(int32_t persistentId)
{
    // 当某个session断开之后执行定时器清理逻辑，通常都是通过死亡监听实现的
    RemoveTimers(persistentId);
}

} // namespace Rosen
} // namespace OHOS