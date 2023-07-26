/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_SESSION_MANAGER_MISSION_LISTENER_CONTROLLER_H
#define OHOS_SESSION_MANAGER_MISSION_LISTENER_CONTROLLER_H

#include <mutex>
#include <vector>
#include <list>
#include "cpp/mutex.h"

#include "common/include/task_scheduler.h"
#include "mission_listener_interface.h"
#include "ws_common.h"

namespace OHOS {
namespace Rosen {

class MissionListenerController : public std::enable_shared_from_this<MissionListenerController> {
    public:
    MissionListenerController();

    ~MissionListenerController();

    void Init();

    WSError AddMissionListener(const sptr<AAFwk::IMissionListener>& listener);

    void DelMissionListener(const sptr<AAFwk::IMissionListener>& listener);

    void NotifyMissionCreated(int32_t missionId);

    void NotifyMissionDestroyed(int32_t missionId);

    void NotifyMissionSnapshotChanged(int32_t missionId);

    void NotifyMissionMovedToFront(int32_t missionId);

    void NotifyMissionFocused(int32_t missionId);

    void NotifyMissionUnfocused(int32_t missionId);

    void NotifyMissionIconChanged(int32_t missionId, const std::shared_ptr<OHOS::Media::PixelMap>& icon);

    void NotifyMissionClosed(int32_t missionId);

    void NotifyMissionLabelUpdated(int32_t missionId);

    void HandleUnInstallApp(const std::list<int32_t>& missions);

private:
    void OnListenerDied(const wptr<IRemoteObject>& remote);

    template<typename F, typename... Args>
    void CallListeners(F func, Args&& ... args)
    {
        std::lock_guard<ffrt::mutex> guard(listenerLock_);
        for (auto listener : missionListeners_) {
            if (listener) {
                (listener->*func)(std::forward<Args>(args)...);
            }
        }
    }

    class ListenerDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        using ListenerDiedHandler = std::function<void(const wptr<IRemoteObject>&)>;
        explicit ListenerDeathRecipient(ListenerDiedHandler handler);
        ~ListenerDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remote) final;

    private:
        ListenerDiedHandler diedHandler_;
    };

private:
    ffrt::mutex listenerLock_;
    std::shared_ptr<TaskScheduler> taskScheduler_;
    std::vector<sptr<AAFwk::IMissionListener>> missionListeners_;
    sptr<IRemoteObject::DeathRecipient> listenerDeathRecipient_;
};
}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_SESSION_MANAGER_MISSION_LISTENER_CONTROLLER_H
