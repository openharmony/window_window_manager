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

#include "screen_snapshot_ability_connection.h"

#include <chrono>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScreenSnapshotAbilityConnection" };
constexpr int32_t RES_FAILURE = -1;
constexpr int32_t RES_SUCCESS = 0;
constexpr uint32_t SEND_MESSAGE_SYNC_OUT_TIME = 300; // ms

void ScreenSnapshotAbilityConnection::OnAbilityConnectDone(
    const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    WLOGFD("OnAbilityConnectDone entry");
    if (resultCode != ERR_OK) {
        WLOGFE("ability connect failed, error code:%{public}d", resultCode);
        return;
    }
    WLOGFI("ability connect success, ability name %{public}s", element.GetAbilityName().c_str());
    if (remoteObject == nullptr) {
        WLOGFE("get remoteObject failed");
        return;
    }
    remoteObject_ = remoteObject;
    if (!AddObjectDeathRecipient()) {
        WLOGFE("AddObjectDeathRecipient failed");
        return;
    }

    isConnected_.store(true);
    connectedCv_.notify_all();
    WLOGFD("OnAbilityConnectDone exit");
}

void ScreenSnapshotAbilityConnection::OnAbilityDisconnectDone(
    const AppExecFwk::ElementName &element, int32_t resultCode)
{
    WLOGFD("OnAbilityDisconnectDone entry");
    if (resultCode != ERR_OK) {
        WLOGFE("ability disconnect failed, error code:%{public}d", resultCode);
    }

    WLOGFI("bundleName:%{public}s, abilityName:%{public}s, resultCode:%{public}d",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str(), resultCode);
    
    if (remoteObject_ == nullptr) {
        WLOGFE("remoteObject member is nullptr");
        return;
    }
    remoteObject_->RemoveDeathRecipient(deathRecipient_);
    remoteObject_ = nullptr;
    isConnected_.store(false);
    WLOGFD("OnAbilityDisconnectDone exit");
}

bool ScreenSnapshotAbilityConnection::AddObjectDeathRecipient()
{
    sptr<ScreenSnapshotAbilityDeathRecipient> deathRecipient(
        new(std::nothrow) ScreenSnapshotAbilityDeathRecipient([this] {
        WLOGFI("add death recipient handler");
        remoteObject_ = nullptr;
        isConnected_.store(false);
    }));

    if (deathRecipient == nullptr) {
        WLOGFE("create ScreenSnapshotAbilityDeathRecipient failed");
        return false;
    }
    deathRecipient_ = deathRecipient;
    if (remoteObject_ == nullptr) {
        WLOGFE("get the remoteObject failed");
        return false;
    }
    if (!remoteObject_->AddDeathRecipient(deathRecipient_)) {
        WLOGFE("AddDeathRecipient failed");
        return false;
    }
    return true;
}

bool ScreenSnapshotAbilityConnection::IsAbilityConnected()
{
    return isConnected_.load();
}

int32_t ScreenSnapshotAbilityConnection::SendMessageSync(int32_t transCode,
    MessageParcel &data, MessageParcel &reply)
{
    std::unique_lock<std::mutex> lock(connectedMutex_);
    connectedCv_.wait_for(lock, std::chrono::milliseconds(SEND_MESSAGE_SYNC_OUT_TIME));
    if (!IsAbilityConnected()) {
        WLOGFE("ability connection is not established");
        lock.unlock();
        return RES_FAILURE;
    }
    lock.unlock();
    MessageOption option;
    if (remoteObject_ == nullptr) {
        WLOGFE("remoteObject is nullptr");
        return RES_FAILURE;
    }
    int32_t ret = remoteObject_->SendRequest(transCode, data, reply, option);
    if (ret != ERR_OK) {
        WLOGFE("remoteObject send request failed");
        return RES_FAILURE;
    }
    return RES_SUCCESS;
}

void ScreenSnapshotAbilityDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remoteObject)
{
    WLOGFD("OnRemoteDied entry");
    if (deathHandler_ == nullptr) {
        WLOGFE("death handler is nullptr");
        return;
    }
    deathHandler_();
}

} // namespace OHOS::Rosen