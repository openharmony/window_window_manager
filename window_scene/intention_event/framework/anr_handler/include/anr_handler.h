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

    void SetSessionStage(const wptr<ISessionStage> &sessionStage);
    void SetLastProcessedEventId(int32_t eventId, int64_t actionTime);

private:
    void MarkProcessed();
    void UpdateLastProcessedEventId(int32_t eventId);
    void SetLastProcessedEventStatus(bool status);
    int32_t GetLastProcessedEventId();
    void SendEvent(int64_t delayTime);
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ { nullptr };
    wptr<ISessionStage> sessionStage_ = nullptr;

private:
    std::mutex anrMtx_;
    struct ANREvent {
        bool sendStatus { false };
        int32_t lastEventId { -1 };
        int32_t lastReportId { -1 };
    };
    ANREvent event_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ANR_HANDLER_H