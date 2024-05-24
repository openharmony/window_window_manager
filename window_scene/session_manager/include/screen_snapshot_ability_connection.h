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

#ifndef OHOS_ROSEN_SCREEN_SNAPSHOT_ABILITY_CONNECTION_H
#define OHOS_ROSEN_SCREEN_SNAPSHOT_ABILITY_CONNECTION_H

#include <string>
#include <mutex>
#include <condition_variable>

#include "ability_connection.h"
#include "iremote_object.h"

namespace OHOS::Rosen {
class ScreenSnapshotAbilityDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit ScreenSnapshotAbilityDeathRecipient(
        std::function<void(void)> deathHandler) : deathHandler_(deathHandler) {}
    ~ScreenSnapshotAbilityDeathRecipient() = default;
    void OnRemoteDied(const wptr<IRemoteObject> &remoteObject) override;
private:
    std::function<void(void)> deathHandler_;
};

class ScreenSnapshotAbilityConnection : public AAFwk::AbilityConnectionStub {
public:
    explicit ScreenSnapshotAbilityConnection() = default;
    virtual ~ScreenSnapshotAbilityConnection() = default;

    void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
        const sptr<IRemoteObject> &remoteObject, int32_t resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;
    int32_t SendMessageSync(int32_t transCode, MessageParcel &data, MessageParcel &reply);
    bool IsAbilityConnected();

private:
    bool AddObjectDeathRecipient();

private:
    std::atomic<bool> isConnected_{false};
    sptr<IRemoteObject> remoteObject_;
    sptr<ScreenSnapshotAbilityDeathRecipient> deathRecipient_;
    std::mutex connectedMutex_;
    std::condition_variable connectedCv_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_SCREEN_SNAPTSHOT_ABILITY_CONNECTION_H