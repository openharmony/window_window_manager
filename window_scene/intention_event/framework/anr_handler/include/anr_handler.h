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

#ifndef ANR_HANDLER_H
#define ANR_HANDLER_H

#include <memory>
#include <mutex>
#include <list>
#include <unordered_map>

#include "event_handler.h"
#include "nocopyable.h"
#include "singleton.h"
#include "session_stage_interface.h"
namespace OHOS {
namespace Rosen {
class ANRHandler {
    DECLARE_DELAYED_SINGLETON(ANRHandler);

public:
    DISALLOW_COPY_AND_MOVE(ANRHandler);

    void SetSessionStage(int32_t eventId, const wptr<ISessionStage> &sessionStage);
    void HandleEventConsumed(int32_t eventId, int64_t actionTime);
    void OnWindowDestroyed(int32_t persistentId);
private:
    using Task = std::function<void()>;
    bool PostTask(Task&& task, int64_t delayTime = 0);
    void MarkProcessed();
    void SendEvent(int32_t eventId, int64_t delayTime);
    void SetAnrHandleState(int32_t eventId, bool status);
    void ClearExpiredEvents(int32_t eventId);
    int32_t GetPersistentIdOfEvent(int32_t eventId);
    bool IsOnEventHandler(int32_t persistentId);
    void UpdateLatestEventId(int32_t eventId);
private:
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ { nullptr };
    struct ANRHandlerState {
        std::unordered_map<int32_t, bool> sendStatus;
        int32_t currentEventIdToReceipt { -1 };
        std::list<int32_t> eventsToReceipt;
    };
    ANRHandlerState anrHandlerState_;
    struct SessionInfo {
        int32_t persistentId;
        wptr<ISessionStage> sessionStage;
    };
    std::unordered_map<int32_t, SessionInfo> sessionStageMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ANR_HANDLER_H