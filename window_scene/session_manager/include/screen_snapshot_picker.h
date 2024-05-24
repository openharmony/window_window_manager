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

#ifndef OHOS_ROSEN_SCREEN_SNAPSHOT_PICKER_H
#define OHOS_ROSEN_SCREEN_SNAPSHOT_PICKER_H

#include <string>

#include "dm_common.h"
#include "image_type.h"
#include "ability_connection.h"
#include "screen_snapshot_ability_connection.h"

namespace OHOS::Rosen {
class ScreenSnapshotPicker {
public:
    static ScreenSnapshotPicker &GetInstance();

    bool SnapshotPickerConnectExtension();
    void SnapshotPickerDisconnectExtension();
    int32_t GetScreenSnapshotInfo(Media::Rect &rect, ScreenId &screenId);
    void SetScreenSnapshotBundleName(const std::string &bundleName);
    void SetScreenSnapshotAbilityName(const std::string &abilityName);

private:
    explicit ScreenSnapshotPicker() = default;
    ~ScreenSnapshotPicker() = default;

    bool SnapshotPickerConnectExtensionAbility(const AAFwk::Want &want);
private:
    sptr<ScreenSnapshotAbilityConnection> abilityConnection_;
    std::string screenSnapshotBundleName_{""};
    std::string screenSnapshotAbilityName_{""};
};
} // OHOS::Rosen

#endif // OHOS_ROSEN_SCREEN_SNAPTSHOT_PICKER_H