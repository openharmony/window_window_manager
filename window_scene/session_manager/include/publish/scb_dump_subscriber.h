/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_ROSEN_SCB_DUMP_SUBSCRIBER_H
#define OHOS_ROSEN_SCB_DUMP_SUBSCRIBER_H

#include <chrono>
#include <condition_variable>
#include <want.h>

#include <common_event_data.h>
#include <common_event_publish_info.h>
#include "common_event_manager.h"
#include "common/include/task_scheduler.h"
#include "interfaces/include/ws_common.h"
#include "interfaces/include/ws_common_inner.h"

namespace OHOS::Rosen {
class ScbDumpSubscriber : public EventFwk::CommonEventSubscriber {
public:
    explicit ScbDumpSubscriber(const EventFwk::CommonEventSubscribeInfo& subscribeInfo)
        : CommonEventSubscriber(subscribeInfo)
    {}
    ~ScbDumpSubscriber() = default;

    void OnReceiveEvent(const EventFwk::CommonEventData &data);

    WSError Publish(std::string cmd);

    std::string GetDebugDumpInfo(std::chrono::milliseconds const &time);

    static void Subscribe(std::shared_ptr<ScbDumpSubscriber>& scbSubscriber);

    static void UnSubscribe(std::shared_ptr<ScbDumpSubscriber>& scbSubscriber);

    static std::string JoinCommands(const std::vector<std::string>& params, int size);

private:
    std::string dumpinfo_ = "";
    bool valueReady_ = false;
    std::mutex mutex_;
    std::condition_variable cv_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_SCB_DUMP_SUBSCRIBER_H