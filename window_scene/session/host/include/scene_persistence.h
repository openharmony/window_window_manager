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

#include "common/include/task_scheduler.h"

namespace OHOS::Media {
class PixelMap;
} // namespace OHOS::Media

namespace OHOS::Rosen {
class ScenePersistence : public RefBase {
public:
    ScenePersistence(const std::string& bundleName, const int32_t& persistentId);
    ~ScenePersistence() = default;

    static bool CreateSnapshotDir(const std::string& directory);
    static bool CreateUpdatedIconDir(const std::string& directory);

    bool IsSnapshotExisted() const;
    std::string GetSnapshotFilePath();
    std::pair<uint32_t, uint32_t> GetSnapshotSize() const;
    void SaveSnapshot(const std::shared_ptr<Media::PixelMap>& pixelMap);

    void SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap>& pixelMap);
    std::string GetUpdatedIconPath() const;
    std::shared_ptr<Media::PixelMap> GetLocalSnapshotPixelMap() const;

private:
    static std::string snapshotDirectory_;
    std::string snapshotPath_;
    std::pair<uint32_t, uint32_t> snapshotSize_;

    static std::string updatedIconDirectory_;
    std::string updatedIconPath_;

    static std::shared_ptr<TaskScheduler> snapshotScheduler_;
};
} // namespace OHOS::Rosen

#endif // OHOS_ROSEN_WINDOW_SCENE_SCENE_PERSISTENCE_H
