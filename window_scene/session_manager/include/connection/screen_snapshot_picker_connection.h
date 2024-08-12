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

#ifndef OHOS_ROSEN_SCREEN_SNAPSHOT_PICKER_CONNECTION_H
#define OHOS_ROSEN_SCREEN_SNAPSHOT_PICKER_CONNECTION_H

#include <string>
#include <memory>

#include "dm_common.h"
#include "image_type.h"
#include "ability_connection.h"
#include "screen_session_ability_connection.h"

namespace OHOS::Rosen {
class ScreenSnapshotPickerConnection {
public:
    static ScreenSnapshotPickerConnection &GetInstance();

    bool SnapshotPickerConnectExtension();
    void SnapshotPickerDisconnectExtension();
    int32_t GetScreenSnapshotInfo(Media::Rect &rect, ScreenId &screenId);
    void SetBundleName(const std::string &bundleName);
    void SetAbilityName(const std::string &abilityName);

private:
    explicit ScreenSnapshotPickerConnection() = default;
    ~ScreenSnapshotPickerConnection() = default;

private:
    std::unique_ptr<ScreenSessionAbilityConnection> abilityConnection_ = nullptr;
    std::string bundleName_{""};
    std::string abilityName_{""};
    sptr<ScreenSessionAbilityConnectionStub> GetScreenSessionAbilityConnectionStub();
};
} // OHOS::Rosen

#endif // OHOS_ROSEN_SCREEN_SNAPSHOT_PICKER_CONNECTION_H