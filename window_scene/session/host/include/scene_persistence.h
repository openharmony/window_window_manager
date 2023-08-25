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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SCENE_PERSISTENCE_H
#define OHOS_ROSEN_WINDOW_SCENE_SCENE_PERSISTENCE_H

#include <string>
#include <sys/stat.h>

#include <refbase.h>

#include "interfaces/include/ws_common.h"

namespace OHOS::Media {
class PixelMap;
} // namespace OHOS::Media

namespace OHOS::Rosen {
class ScenePersistence : public RefBase {
public:
    ScenePersistence(const SessionInfo& info, const int32_t& persistentId);
    ~ScenePersistence() = default;

    static inline bool CreateSnapshotDir(const std::string& strFilesDir)
    {
        strPersistPath_ = strFilesDir + "/SceneSnapShot/";
        constexpr mode_t MKDIR_MODE = 0740;
        if (mkdir(strPersistPath_.c_str(), MKDIR_MODE) != 0) {
            return false;
        }
        return true;
    }

    static inline bool CreateUpdatedIconDir(const std::string& strFilesDir)
    {
        strPersistUpdatedIconPath_ = strFilesDir + "/UpdatedIcon/";
        constexpr mode_t MKDIR_MODE = 0740;
        if (mkdir(strPersistUpdatedIconPath_.c_str(), MKDIR_MODE) != 0) {
            return false;
        }
        return true;
    }

    bool IsSnapshotExisted() const;
    std::string GetSnapshotFilePath() const;
    std::pair<uint32_t, uint32_t> GetSnapshotSize() const;
    void SaveSnapshot(const std::shared_ptr<Media::PixelMap>& pixelMap);
    void SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap>& pixelMap);
    std::string GetUpdatedIconPath() const;

private:
    static std::string strPersistPath_;
    static std::string strPersistUpdatedIconPath_;
    std::pair<uint32_t, uint32_t> snapshotSize_;
    std::string strSnapshotFile_;
    std::string strUpdatedIconPath_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_PERSISTENCE_H
