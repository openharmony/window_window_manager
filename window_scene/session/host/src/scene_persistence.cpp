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

#include "session/host/include/scene_persistence.h"

#include <hitrace_meter.h>
#include <image_packer.h>
#include <parameters.h>

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, HILOG_DOMAIN_WINDOW, "ScenePersistence" };
constexpr const char* UNDERLINE_SEPARATOR = "_";
constexpr const char* ASTC_IMAGE_FORMAT = "image/astc/4*4";
constexpr const char* ASTC_IMAGE_SUFFIX = ".astc";
constexpr uint8_t ASTC_IMAGE_QUALITY = 20;

constexpr const char* IMAGE_FORMAT = "image/png";
constexpr const char* IMAGE_SUFFIX = ".png";
constexpr uint8_t IMAGE_QUALITY = 100;

constexpr uint8_t SUCCESS = 0;
const std::string SNAPSHOT_THREAD = "OS_SnapshotThread";
} // namespace

std::string ScenePersistence::snapshotDirectory_;
std::string ScenePersistence::updatedIconDirectory_;
std::shared_ptr<TaskScheduler> ScenePersistence::snapshotScheduler_;

bool ScenePersistence::CreateSnapshotDir(const std::string& directory)
{
    snapshotDirectory_ = directory + "/SceneSnapShot/";
    if (mkdir(snapshotDirectory_.c_str(), S_IRWXU)) {
        WLOGFD("mkdir failed or the directory already exists");
        return false;
    }
    return true;
}

bool ScenePersistence::CreateUpdatedIconDir(const std::string& directory)
{
    updatedIconDirectory_ = directory + "/UpdatedIcon/";
    if (mkdir(updatedIconDirectory_.c_str(), S_IRWXU)) {
        WLOGFD("mkdir failed or the directory already exists");
        return false;
    }
    return true;
}

ScenePersistence::ScenePersistence(const std::string& bundleName, const int32_t& persistentId)
{
    bundleName_ = bundleName;
    if (IsAstcEnabled()) {
        oldSnapshotPath_ = snapshotDirectory_ + bundleName + UNDERLINE_SEPARATOR +
            std::to_string(persistentId) + IMAGE_SUFFIX;
        snapshotPath_ = snapshotDirectory_ + bundleName + UNDERLINE_SEPARATOR +
            std::to_string(persistentId) + ASTC_IMAGE_SUFFIX;
    } else {
        snapshotPath_ = snapshotDirectory_ + bundleName + UNDERLINE_SEPARATOR +
            std::to_string(persistentId) + IMAGE_SUFFIX;
    }
    updatedIconPath_ = updatedIconDirectory_ + bundleName + IMAGE_SUFFIX;
    if (snapshotScheduler_ == nullptr) {
        snapshotScheduler_ = std::make_shared<TaskScheduler>(SNAPSHOT_THREAD);
    }
}

ScenePersistence::~ScenePersistence()
{
    remove(snapshotPath_.c_str());
    if (IsAstcEnabled()) {
        remove(oldSnapshotPath_.c_str());
    }
}

bool ScenePersistence::IsAstcEnabled()
{
    static bool isAstcEnabled = system::GetBoolParameter("persist.multimedia.image.astc.enabled", true);
    return isAstcEnabled;
}

void ScenePersistence::SaveSnapshot(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const std::function<void()> resetSnapshotCallback)
{
    savingSnapshotSum_.fetch_add(1);
    isSavingSnapshot_.store(true);
    auto task = [weakThis = wptr(this), pixelMap, resetSnapshotCallback,
        savingSnapshotSum = savingSnapshotSum_.load()]() {
        auto scenePersistence = weakThis.promote();
        if (scenePersistence == nullptr || pixelMap == nullptr ||
            scenePersistence->snapshotPath_.find('/') == std::string::npos) {
            WLOGFE("scenePersistence is%{public}s nullptr, pixelMap is%{public}s nullptr",
                scenePersistence == nullptr ? "" : " not", pixelMap == nullptr ? "" : " not");
            resetSnapshotCallback();
            return;
        }

        WLOGFD("Save snapshot begin");
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "ScenePersistence:SaveSnapshot %s",
            scenePersistence->snapshotPath_.c_str());
        OHOS::Media::ImagePacker imagePacker;
        OHOS::Media::PackOption option;
        if (IsAstcEnabled()) {
            option.format = ASTC_IMAGE_FORMAT;
            option.quality = ASTC_IMAGE_QUALITY;
        } else {
            option.format = IMAGE_FORMAT;
            option.quality = IMAGE_QUALITY;
        }
        option.numberHint = 1;
        std::set<std::string> formats;
        if (imagePacker.GetSupportedFormats(formats)) {
            WLOGFE("Failed to get supported formats");
            resetSnapshotCallback();
            return;
        }

        remove(scenePersistence->snapshotPath_.c_str());
        if (IsAstcEnabled()) {
            remove(scenePersistence->oldSnapshotPath_.c_str());
        }
        scenePersistence->snapshotSize_ = { pixelMap->GetWidth(), pixelMap->GetHeight() };
        imagePacker.StartPacking(scenePersistence->snapshotPath_, option);
        imagePacker.AddImage(*pixelMap);
        int64_t packedSize = 0;
        imagePacker.FinalizePacking(packedSize);
        // If the current num is equals to the latest num, it is the last saveSnapshot task
        if (savingSnapshotSum == scenePersistence->savingSnapshotSum_.load()) {
            resetSnapshotCallback();
            scenePersistence->isSavingSnapshot_.store(false);
        }
        WLOGFD("Save snapshot end, packed size %{public}" PRIu64, packedSize);
    };
    snapshotScheduler_->RemoveTask("SaveSnapshot" + snapshotPath_);
    snapshotScheduler_->PostAsyncTask(task, "SaveSnapshot" + snapshotPath_);
}

bool ScenePersistence::IsSavingSnapshot()
{
    return isSavingSnapshot_.load();
}

void ScenePersistence::RenameSnapshotFromOldPersistentId(const int32_t &oldPersistentId)
{
    auto task = [weakThis = wptr(this), oldPersistentId]() {
        auto scenePersistence = weakThis.promote();
        if (scenePersistence == nullptr) {
            WLOGFE("scenePersistence is nullptr");
            return;
        }
        std::string oldSnapshotPath;
        if (IsAstcEnabled()) {
            oldSnapshotPath = snapshotDirectory_ + scenePersistence->bundleName_ + UNDERLINE_SEPARATOR +
                std::to_string(oldPersistentId) + ASTC_IMAGE_SUFFIX;
        } else {
            oldSnapshotPath = snapshotDirectory_ + scenePersistence->bundleName_ + UNDERLINE_SEPARATOR +
                std::to_string(oldPersistentId) + IMAGE_SUFFIX;
        }
        int ret = std::rename(oldSnapshotPath.c_str(), scenePersistence->snapshotPath_.c_str());
        if (ret == 0) {
            WLOGFI("Rename snapshot from %{public}s to %{public}s.",
                oldSnapshotPath.c_str(), scenePersistence->snapshotPath_.c_str());
        } else {
            WLOGFW("Failed to rename snapshot from %{public}s to %{public}s.",
                oldSnapshotPath.c_str(), scenePersistence->snapshotPath_.c_str());
        }
    };
    snapshotScheduler_->PostAsyncTask(task, "RenameSnapshotFromOldPersistentId");
}

std::string ScenePersistence::GetSnapshotFilePath()
{
    auto task = [weakThis = wptr(this)]() -> std::string {
        auto scenePersistence = weakThis.promote();
        if (scenePersistence == nullptr) {
            WLOGFE("scenePersistence is nullptr");
            return "";
        }
        return scenePersistence->snapshotPath_;
    };
    return snapshotScheduler_->PostSyncTask(task, "GetSnapshotFilePath");
}

std::string ScenePersistence::GetSnapshotFilePathFromAce()
{
    if (IsAstcEnabled() && IsSnapshotExisted(oldSnapshotPath_)) {
        return oldSnapshotPath_;
    }
    return snapshotPath_;
}

void ScenePersistence::SaveUpdatedIcon(const std::shared_ptr<Media::PixelMap>& pixelMap)
{
    if (pixelMap == nullptr || updatedIconPath_.find('/') == std::string::npos) {
        return;
    }

    OHOS::Media::ImagePacker imagePacker;
    OHOS::Media::PackOption option;
    option.format = IMAGE_FORMAT;
    option.quality = IMAGE_QUALITY;
    option.numberHint = 1;
    std::set<std::string> formats;
    if (imagePacker.GetSupportedFormats(formats)) {
        WLOGFE("Failed to get supported formats");
        return;
    }

    if (remove(updatedIconPath_.c_str())) {
        WLOGFD("Failed to delete old file");
    }
    imagePacker.StartPacking(GetUpdatedIconPath(), option);
    imagePacker.AddImage(*pixelMap);
    int64_t packedSize = 0;
    imagePacker.FinalizePacking(packedSize);
    WLOGFD("SaveUpdatedIcon finished");
}

std::string ScenePersistence::GetUpdatedIconPath() const
{
    return updatedIconPath_;
}

std::pair<uint32_t, uint32_t> ScenePersistence::GetSnapshotSize() const
{
    return snapshotSize_;
}

bool ScenePersistence::IsSnapshotExisted() const
{
    if (IsAstcEnabled()) {
        return IsSnapshotExisted(snapshotPath_) || IsSnapshotExisted(oldSnapshotPath_);
    } else {
        return IsSnapshotExisted(snapshotPath_);
    }
}

bool ScenePersistence::IsSnapshotExisted(const std::string& path) const
{
    struct stat buf;
    if (stat(path.c_str(), &buf)) {
        WLOGFD("Snapshot file %{public}s does not exist", path.c_str());
        return false;
    }
    return S_ISREG(buf.st_mode);
}

std::shared_ptr<Media::PixelMap> ScenePersistence::GetLocalSnapshotPixelMap(const float oriScale,
    const float newScale) const
{
    if (!IsSnapshotExisted()) {
        WLOGE("local snapshot pic is not existed");
        return nullptr;
    }

    uint32_t errorCode = 0;
    Media::SourceOptions sourceOpts;
    sourceOpts.formatHint = IsAstcEnabled() ? ASTC_IMAGE_FORMAT : IMAGE_FORMAT;
    auto imageSource = Media::ImageSource::CreateImageSource(snapshotPath_, sourceOpts, errorCode);
    if (!imageSource) {
        WLOGE("create image source fail, errCode : %{public}d", errorCode);
        return nullptr;
    }

    Media::ImageInfo info;
    int32_t decoderWidth = 0;
    int32_t decoderHeight = 0;
    errorCode = imageSource->GetImageInfo(info);
    if (errorCode == SUCCESS) {
        decoderWidth = info.size.width;
        decoderHeight = info.size.height;
    }
    Media::DecodeOptions decodeOpts;
    decodeOpts.desiredPixelFormat = Media::PixelFormat::RGBA_8888;
    if (oriScale != 0 && decoderWidth > 0 && decoderHeight > 0) {
        auto isNeedToScale = newScale < oriScale;
        decodeOpts.desiredSize.width = isNeedToScale ?
            static_cast<int>(decoderWidth * newScale / oriScale) : decoderWidth;
        decodeOpts.desiredSize.height = isNeedToScale ?
            static_cast<int>(decoderHeight * newScale / oriScale) : decoderHeight;
    }
    return imageSource->CreatePixelMap(decodeOpts, errorCode);
}
} // namespace OHOS::Rosen
