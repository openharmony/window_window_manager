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

#include "persistent_storage.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "PersistentStorage"};
}
std::map<PersistentStorageType, std::string> PersistentStorage::storagePath_ = {
    { PersistentStorageType::ASPECT_RATIO, "/data/service/el1/public/window/window_aspect_ratio.xml" },
};

bool PersistentStorage::HasKey(const std::string& key, PersistentStorageType storageType)
{
    bool res = false;
    auto pref = GetPreference(storageType);
    if (!pref) {
        WLOGE("[PersistentStorage] Preferences is nullptr");
        return res;
    }
    res = pref->HasKey(key);
    WLOGD("[PersistentStorage] %{public}s %{public}s", key.c_str(),
        (res ? "Has persisted key:" : "Don't have persisted key:"));
    return res;
}

void PersistentStorage::Delete(const std::string& key, PersistentStorageType storageType)
{
    auto pref = GetPreference(storageType);
    if (!pref) {
        WLOGE("[PersistentStorage] Preferences is nullptr");
        return;
    }
    pref->Delete(key);
    pref->Flush();
    WLOGD("[PersistentStorage] Delete key %{public}s", key.c_str());
}

std::shared_ptr<PersistentPerference> PersistentStorage::GetPreference(PersistentStorageType storageType)
{
    auto iter = storagePath_.find(storageType);
    if (iter == storagePath_.end()) {
        return nullptr;
    }
    auto fileName = storagePath_[storageType];
    int errCode;
    auto pref = NativePreferences::PreferencesHelper::GetPreferences(fileName, errCode);
    WLOGD("[PersistentStorage] GetPreference fileName: %{public}s, errCode: %{public}d", fileName.c_str(), errCode);
    return pref;
}
} // namespace Rosen
} // namespace OHOS