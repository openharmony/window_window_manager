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

#include "mission_listener_controller.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MissionListenerController"};
const std::string THREAD_NAME = "MissionListener";
}

MissionListenerController::MissionListenerController()
{
}

MissionListenerController::~MissionListenerController()
{}

void MissionListenerController::Init()
{
    if (!taskScheduler_) {
        taskScheduler_ = std::make_shared<TaskScheduler>(THREAD_NAME);
    }
}

WSError MissionListenerController::AddMissionListener(const sptr<AAFwk::IMissionListener>& listener)
{
    if (!listener) {
        WLOGFE("listener is invalid");
        return WSError::WS_ERROR_INVALID_PARAM;
    }

    std::lock_guard<ffrt::mutex> guard(listenerLock_);
    auto it = std::find_if(missionListeners_.begin(), missionListeners_.end(),
                           [&listener](const sptr<AAFwk::IMissionListener>& item) {
                               return (item && item->AsObject() == listener->AsObject());
                           });
    if (it != missionListeners_.end()) {
        WLOGFW("listener was already added, do not add again");
        return WSError::WS_OK;
    }

    if (!listenerDeathRecipient_) {
        std::weak_ptr<MissionListenerController> thisWeakPtr(shared_from_this());
        listenerDeathRecipient_ = new ListenerDeathRecipient([thisWeakPtr](const wptr<IRemoteObject>& remote) {
            auto controller = thisWeakPtr.lock();
            if (controller) {
                controller->OnListenerDied(remote);
            }
        });
    }
    auto listenerObject = listener->AsObject();
    if (listenerObject) {
        listenerObject->AddDeathRecipient(listenerDeathRecipient_);
    }
    missionListeners_.emplace_back(listener);

    return WSError::WS_OK;
}

void MissionListenerController::DelMissionListener(const sptr<AAFwk::IMissionListener>& listener)
{
    if (!listener) {
        WLOGFE("listener is invalid");
        return;
    }

    std::lock_guard<ffrt::mutex> guard(listenerLock_);
    auto it = std::find_if(missionListeners_.begin(), missionListeners_.end(),
                           [&listener](const sptr<AAFwk::IMissionListener> item) {
                               return (item && item->AsObject() == listener->AsObject());
                           });
    if (it != missionListeners_.end()) {
        missionListeners_.erase(it);
    }
}

void MissionListenerController::NotifyMissionCreated(int32_t missionId)
{
    if (missionId == -1) {
        return;
    }

    if (!taskScheduler_) {
        WLOGFE("taskScheduler is null");
        return;
    }
    auto task = [weak = weak_from_this(), missionId]() {
        auto self = weak.lock();
        if (self == nullptr) {
            WLOGFE("self is nullptr, NotifyMissionCreated failed");
            return;
        }
        WLOGFI("NotifyMissionCreated, missionId:%{public}d.", missionId);
        self->CallListeners(&AAFwk::IMissionListener::OnMissionCreated, missionId);
    };
    taskScheduler_->PostVoidSyncTask(task);
}

void MissionListenerController::NotifyMissionDestroyed(int32_t missionId)
{
    if (missionId == -1) {
        return;
    }

    if (!taskScheduler_) {
        WLOGFE("taskScheduler is null");
        return;
    }
    auto task = [weak = weak_from_this(), missionId]() {
        auto self = weak.lock();
        if (self == nullptr) {
            WLOGFE("self is nullptr, NotifyMissionDestroyed failed");
            return;
        }
        WLOGFI("NotifyMissionDestroyed, missionId:%{public}d.", missionId);
        self->CallListeners(&AAFwk::IMissionListener::OnMissionDestroyed, missionId);
    };
    taskScheduler_->PostVoidSyncTask(task);
}

void MissionListenerController::HandleUnInstallApp(const std::list<int32_t>& missions)
{
    if (missions.empty()) {
        return;
    }

    if (!taskScheduler_) {
        WLOGFE("taskScheduler is null");
        return;
    }
    auto task = [weak = weak_from_this(), missions]() {
        auto self = weak.lock();
        if (self == nullptr) {
            WLOGFE("self is nullptr, HandleUnInstallApp failed");
            return;
        }
        for (auto id : missions) {
            self->CallListeners(&AAFwk::IMissionListener::OnMissionDestroyed, id);
        }
    };
    taskScheduler_->PostVoidSyncTask(task);
}

void MissionListenerController::NotifyMissionSnapshotChanged(int32_t missionId)
{
    if (missionId == -1) {
        return;
    }

    if (!taskScheduler_) {
        WLOGFE("taskScheduler is null");
        return;
    }
    auto task = [weak = weak_from_this(), missionId]() {
        auto self = weak.lock();
        if (self == nullptr) {
            WLOGFE("self is nullptr, NotifyMissionSnapshotChanged failed");
            return;
        }
        WLOGFI("NotifyMissionSnapshotChanged, missionId:%{public}d.", missionId);
        self->CallListeners(&AAFwk::IMissionListener::OnMissionSnapshotChanged, missionId);
    };
    taskScheduler_->PostVoidSyncTask(task);
}

void MissionListenerController::NotifyMissionMovedToFront(int32_t missionId)
{
    if (missionId == -1) {
        return;
    }

    if (!taskScheduler_) {
        WLOGFE("taskScheduler is null");
        return;
    }
    auto task = [weak = weak_from_this(), missionId]() {
        auto self = weak.lock();
        if (self == nullptr) {
            WLOGFE("self is nullptr, NotifyMissionMovedToFront failed");
            return;
        }
        WLOGFI("NotifyMissionMovedToFront, missionId:%{public}d.", missionId);
        self->CallListeners(&AAFwk::IMissionListener::OnMissionMovedToFront, missionId);
    };
    taskScheduler_->PostVoidSyncTask(task);
}

void MissionListenerController::NotifyMissionFocused(int32_t missionId)
{
    if (missionId == -1) {
        return;
    }

    if (!taskScheduler_) {
        WLOGFE("taskScheduler is null");
        return;
    }
    auto task = [weak = weak_from_this(), missionId]() {
        auto self = weak.lock();
        if (self == nullptr) {
            WLOGFE("self is nullptr, NotifyMissionFocused failed");
            return;
        }
        WLOGFI("NotifyMissionFocused, missionId:%{public}d.", missionId);
        self->CallListeners(&AAFwk::IMissionListener::OnMissionFocused, missionId);
    };
    taskScheduler_->PostVoidSyncTask(task);
}

void MissionListenerController::NotifyMissionUnfocused(int32_t missionId)
{
    if (missionId == -1) {
        return;
    }

    if (!taskScheduler_) {
        WLOGFE("taskScheduler is null");
        return;
    }
    auto task = [weak = weak_from_this(), missionId]() {
        auto self = weak.lock();
        if (self == nullptr) {
            WLOGFE("self is nullptr, NotifyMissionUnfocused failed");
            return;
        }
        WLOGFI("NotifyMissionUnfocused, missionId:%{public}d.", missionId);
        self->CallListeners(&AAFwk::IMissionListener::OnMissionUnfocused, missionId);
    };
    taskScheduler_->PostVoidSyncTask(task);
}

void MissionListenerController::NotifyMissionIconChanged(int32_t missionId,
                                                         const std::shared_ptr<OHOS::Media::PixelMap>& icon)
{
    if (missionId == -1) {
        return;
    }

    if (!taskScheduler_) {
        WLOGFE("taskScheduler is null");
        return;
    }
    auto task = [weak = weak_from_this(), missionId, icon]() {
        auto self = weak.lock();
        if (self == nullptr) {
            WLOGFE("self is nullptr, NotifyMissionIconChanged failed");
            return;
        }
        WLOGFI("NotifyMissionIconChanged, missionId:%{public}d.", missionId);
        self->CallListeners(&AAFwk::IMissionListener::OnMissionIconUpdated, missionId, icon);
    };
    taskScheduler_->PostVoidSyncTask(task);
}

void MissionListenerController::NotifyMissionClosed(int32_t missionId)
{
    if (missionId == -1) {
        return;
    }

    if (!taskScheduler_) {
        WLOGFE("taskScheduler is null");
        return;
    }
    auto task = [weak = weak_from_this(), missionId]() {
        auto self = weak.lock();
        if (self == nullptr) {
            WLOGFE("self is nullptr, NotifyMissionClosed failed");
            return;
        }
        WLOGFI("NotifyMissionClosed, missionId:%{public}d.", missionId);
        self->CallListeners(&AAFwk::IMissionListener::OnMissionClosed, missionId);
    };
    taskScheduler_->PostVoidSyncTask(task);
}

void MissionListenerController::NotifyMissionLabelUpdated(int32_t missionId)
{
    if (missionId == -1) {
        return;
    }

    if (!taskScheduler_) {
        WLOGFE("taskScheduler is null");
        return;
    }
    auto task = [weak = weak_from_this(), missionId]() {
        auto self = weak.lock();
        if (self == nullptr) {
            WLOGFE("self is nullptr, NotifyMissionLabelUpdated failed");
            return;
        }
        WLOGFI("NotifyMissionLabelUpdated, missionId:%{public}d.", missionId);
        self->CallListeners(&AAFwk::IMissionListener::OnMissionLabelUpdated, missionId);
    };
    taskScheduler_->PostVoidSyncTask(task);
}

void MissionListenerController::OnListenerDied(const wptr<IRemoteObject>& remote)
{
    WLOGFD("On mission listener died.");
    auto remoteObj = remote.promote();
    if (!remoteObj) {
        WLOGFD("invalid remote object.");
        return;
    }
    remoteObj->RemoveDeathRecipient(listenerDeathRecipient_);

    std::lock_guard<ffrt::mutex> guard(listenerLock_);
    auto it = std::find_if(missionListeners_.begin(), missionListeners_.end(),
                           [&remoteObj](const sptr<AAFwk::IMissionListener> item) {
                               return (item && item->AsObject() == remoteObj);
                           });
    if (it != missionListeners_.end()) {
        missionListeners_.erase(it);
    }
}

MissionListenerController::ListenerDeathRecipient::ListenerDeathRecipient(ListenerDiedHandler handler)
    : diedHandler_(handler)
{}

void MissionListenerController::ListenerDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (diedHandler_) {
        diedHandler_(remote);
    }
}
} // namespace Rosen
} // namespace OHOS
