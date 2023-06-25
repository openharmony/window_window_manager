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

#include "session/host/include/scene_persistent_storage.h"

#include <filesystem>

namespace OHOS {
namespace Rosen {

std::string ScenePersistentStorage::saveDir_;
std::map<ScenePersistentStorageType, std::string> ScenePersistentStorage::storagePath_;

bool ScenePersistentStorage::HasKey(const std::string& key, ScenePersistentStorageType storageType)
{
    bool res = false;
    auto pref = GetPreference(storageType);
    if (!pref) {
        WLOGE("[ScenePersistentStorage] Preferences is nullptr");
        return res;
    }
    res = pref->HasKey(key);
    WLOGD("[ScenePersistentStorage] %{public}s %{public}s", key.c_str(),
        (res ? "Has persisted key:" : "Don't have persisted key:"));
    return res;
}

void ScenePersistentStorage::Delete(const std::string& key, ScenePersistentStorageType storageType)
{
    auto pref = GetPreference(storageType);
    if (!pref) {
        WLOGE("[ScenePersistentStorage] Preferences is nullptr");
        return;
    }
    pref->Delete(key);
    pref->Flush();
    WLOGD("[ScenePersistentStorage] Delete key %{public}s", key.c_str());
}

std::shared_ptr<PersistentPerference> ScenePersistentStorage::GetPreference(ScenePersistentStorageType storageType)
{
    auto iter = storagePath_.find(storageType);
    if (iter == storagePath_.end()) {
        return nullptr;
    }
    auto fileName = storagePath_[storageType];
    int errCode;
    auto pref = NativePreferences::PreferencesHelper::GetPreferences(fileName, errCode);
    WLOGD("[ScenePersistentStorage] GetPreference file: %{public}s, errCode: %{public}d", fileName.c_str(), errCode);
    return pref;
}

void ScenePersistentStorage::InitDir(std::string dir)
{
    saveDir_ = dir + "/persistent_storage/";
    std::filesystem::path fileDir {saveDir_};
    if (!std::filesystem::exists(fileDir)) {
        std::filesystem::create_directories(fileDir);
        std::filesystem::permissions(fileDir, std::filesystem::perms::owner_all | std::filesystem::perms::group_read);
    }
    storagePath_ = {
        { ScenePersistentStorageType::ASPECT_RATIO, saveDir_ + "session_window_aspect_ratio.xml" },
        { ScenePersistentStorageType::MAXIMIZE_STATE, saveDir_ + "session_window_maximize_state.xml" },
    };
}

} // namespace Rosen
} // namespace OHOS
