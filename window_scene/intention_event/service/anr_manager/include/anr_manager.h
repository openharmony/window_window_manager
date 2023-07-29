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

#ifndef ANR_MANAGER_H
#define ANR_MANAGER_H

#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>

#include "nocopyable.h"
#include "singleton.h"

#include "event_stage.h"
#include "ws_common.h"

namespace OHOS {
namespace Rosen {

class ANRManager final {
    DECLARE_DELAYED_SINGLETON(ANRManager);
public:
    DISALLOW_COPY_AND_MOVE(ANRManager);
    void Init();
    void SwitchAnr(bool status);
    void AddTimer(int32_t eventId, int32_t persistentId);
    void MarkProcessed(int32_t eventId, int32_t persistentId);
    bool IsANRTriggered(int32_t persistentId);
    void OnSessionLost(int32_t persistentId);
    void SetApplicationInfo(int32_t persistentId, int32_t applicationPid, std::string processName);
    void SetAnrObserver(std::function<void(int32_t)> anrObserver);
private:
    struct AppInfo {
        int32_t pid { -1 };
        std::string processName { "unknow" };
    };
    ANRManager::AppInfo GetAppInfoByPersistentId(int32_t persistentId);
    void RemoveTimers(int32_t persistentId);
    void RemovePersistentId(int32_t persistentId);
private:
    std::atomic_bool switcher_ {true};
    std::mutex mtx_;
    int32_t anrTimerCount_ { 0 };
    std::unordered_map<int32_t, AppInfo> applicationMap_;
    std::function<void(int32_t)> anrObserver_;
    EventStage eventStage_;
};
} // namespace Rosen
} // namespace OHOS
#endif // ANR_MANAGER_H
