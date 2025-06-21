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

#include <sys/stat.h>

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
} // namespace

std::string ScenePersistence::snapshotDirectory_;
std::string ScenePersistence::updatedIconDirectory_;
std::shared_ptr<WSFFRTHelper> ScenePersistence::snapshotFfrtHelper_;
bool ScenePersistence::isAstcEnabled_ = false;

bool ScenePersistence::CreateSnapshotDir(const std::string& directory)
{
    snapshotDirectory_ = directory + "/SceneSnapShot/";
    if (mkdir(snapshotDirectory_.c_str(), S_IRWXU)) {
        TLOGD(WmsLogTag::WMS_PATTERN, "mkdir failed or the directory already exists");
        return false;
    }
    return true;
}

bool ScenePersistence::CreateUpdatedIconDir(const std::string& directory)
{
    updatedIconDirectory_ = directory + "/UpdatedIcon/";
    if (mkdir(updatedIconDirectory_.c_str(), S_IRWXU)) {
        TLOGD(WmsLogTag::DEFAULT, "mkdir failed or the directory already exists");
        return false;
    }
    return true;
}

void ScenePersistence::SetSnapshotCapacity(SnapshotStatus capacity)
{
    capacity_ = capacity;
}

ScenePersistence::ScenePersistence(const std::string& bundleName, int32_t persistentId, SnapshotStatus capacity)
    : bundleName_(bundleName), persistentId_(persistentId), capacity_(capacity)
{
    for (uint32_t screen = SCREEN_UNKNOWN; screen < SCREEN_COUNT; screen++) {
        for (uint32_t orientation = SNAPSHOT_PORTRAIT; orientation < ORIENTATION_COUNT; orientation++) {
            if (isAstcEnabled_) {
                snapshotPath_[screen][orientation] = snapshotDirectory_ + bundleName + UNDERLINE_SEPARATOR +
                    std::to_string(persistentId) + std::to_string(screen) +
                    std::to_string(orientation) + ASTC_IMAGE_SUFFIX;
            } else {
                snapshotPath_[screen][orientation] = snapshotDirectory_ + bundleName + UNDERLINE_SEPARATOR +
                    std::to_string(persistentId) + std::to_string(screen) +
                    std::to_string(orientation) + IMAGE_SUFFIX;
            }
        }
    }
    updatedIconPath_ = updatedIconDirectory_ + bundleName + IMAGE_SUFFIX;
    if (snapshotFfrtHelper_ == nullptr) {
        snapshotFfrtHelper_ = std::make_shared<WSFFRTHelper>();
    }
}

ScenePersistence::~ScenePersistence()
{
    TLOGI(WmsLogTag::WMS_PATTERN, "destroyed, persistentId: %{public}d", persistentId_);
    for (const auto& row : snapshotPath_) {
        for (auto& snapshotPath : row) {
            remove(snapshotPath.c_str());
        }
    }
}

std::shared_ptr<WSFFRTHelper> ScenePersistence::GetSnapshotFfrtHelper() const
{
    return snapshotFfrtHelper_;
}

void ScenePersistence::InitAstcEnabled()
{
    static bool isAstcEnabled = system::GetBoolParameter("persist.multimedia.image.astc.enabled", true);
    isAstcEnabled_ = isAstcEnabled;
}

bool ScenePersistence::IsAstcEnabled()
{
    return isAstcEnabled_;
}

void ScenePersistence::SaveSnapshot(const std::shared_ptr<Media::PixelMap>& pixelMap,
    const std::function<void()> resetSnapshotCallback, SnapshotStatus key, DisplayOrientation rotate)
{
    savingSnapshotSum_.fetch_add(1);
    isSavingSnapshot_[key.first][key.second].store(true);
    auto task = [weakThis = wptr(this), pixelMap, resetSnapshotCallback,
        savingSnapshotSum = savingSnapshotSum_.load(), key, rotate]() {
        auto scenePersistence = weakThis.promote();
        if (scenePersistence == nullptr || pixelMap == nullptr ||
            scenePersistence->snapshotPath_[key.first][key.second].find('/') == std::string::npos) {
            TLOGNE(WmsLogTag::WMS_PATTERN, "scenePersistence %{public}s nullptr, pixelMap %{public}s nullptr",
                scenePersistence == nullptr ? "" : "not", pixelMap == nullptr ? "" : "not");
            resetSnapshotCallback();
            return;
        }

        TLOGNI(WmsLogTag::WMS_PATTERN, "Save snapshot begin");
        HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "SaveSnapshot %s",
            scenePersistence->snapshotPath_[key.first][key.second].c_str());
        OHOS::Media::ImagePacker imagePacker;
        OHOS::Media::PackOption option;
        option.format = IsAstcEnabled() ? ASTC_IMAGE_FORMAT : IMAGE_FORMAT;
        option.quality = IsAstcEnabled() ? ASTC_IMAGE_QUALITY : IMAGE_QUALITY;
        option.numberHint = 1;

        std::lock_guard lock(scenePersistence->savingSnapshotMutex_);
        remove(scenePersistence->snapshotPath_[key.first][key.second].c_str());
        scenePersistence->snapshotSize_[key.first][key.second] = { pixelMap->GetWidth(), pixelMap->GetHeight() };
        if (imagePacker.StartPacking(scenePersistence->snapshotPath_[key.first][key.second], option)) {
            TLOGNE(WmsLogTag::WMS_PATTERN, "Save snapshot failed, starting packing error");
            resetSnapshotCallback();
            return;
        }
        if (imagePacker.AddImage(*pixelMap)) {
            TLOGNE(WmsLogTag::WMS_PATTERN, "Save snapshot failed, adding image error");
            resetSnapshotCallback();
            return;
        }
        int64_t packedSize = 0;
        if (imagePacker.FinalizePacking(packedSize)) {
            TLOGNE(WmsLogTag::WMS_PATTERN, "Save snapshot failed, finalizing packing error");
            resetSnapshotCallback();
            return;
        }
        // If the current num is equals to the latest num, it is the last saveSnapshot task
        if (savingSnapshotSum == scenePersistence->savingSnapshotSum_.load()) {
            resetSnapshotCallback();
        }
        scenePersistence->rotate_[key.first][key.second] = rotate;
        TLOGNI(WmsLogTag::WMS_PATTERN, "Save snapshot end, packed size %{public}" PRIu64, packedSize);
    };
    snapshotFfrtHelper_->SubmitTask(std::move(task), "SaveSnapshot" + snapshotPath_[key.first][key.second]);
}

bool ScenePersistence::IsSavingSnapshot(SnapshotStatus key)
{
    return isSavingSnapshot_[key.first][key.second].load();
}

void ScenePersistence::ResetSnapshotCache()
{
    for (auto& row : isSavingSnapshot_) {
        for (auto& isSavingSnapshot : row) {
            isSavingSnapshot.store(false);
        }
    }
}

void ScenePersistence::RenameSnapshotFromOldPersistentId(const int32_t& oldPersistentId)
{
    auto task = [weakThis = wptr(this), oldPersistentId]() {
        auto scenePersistence = weakThis.promote();
        if (scenePersistence == nullptr) {
            TLOGNE(WmsLogTag::WMS_PATTERN, "scenePersistence is nullptr");
            return;
        }
        for (uint32_t screen = SCREEN_UNKNOWN; screen < SCREEN_COUNT; screen++) {
            for (uint32_t orientation = SNAPSHOT_PORTRAIT; orientation < ORIENTATION_COUNT; orientation++) {
                scenePersistence->RenameSnapshotFromOldPersistentId(oldPersistentId, { screen, orientation });
            }
        }
    };
    snapshotFfrtHelper_->SubmitTask(std::move(task), "RenameSnapshotFromOldPersistentId"
        + std::to_string(oldPersistentId));
}

void ScenePersistence::RenameSnapshotFromOldPersistentId(const int32_t& oldPersistentId, SnapshotStatus key)
{
    std::lock_guard lock(savingSnapshotMutex_);
    std::string oldSnapshotPath;
    auto& snapshotPath = snapshotPath_[key.first][key.second];
    if (isAstcEnabled_) {
        oldSnapshotPath = snapshotDirectory_ + bundleName_ + UNDERLINE_SEPARATOR +
            std::to_string(oldPersistentId) + std::to_string(key.first) +
            std::to_string(key.second) + ASTC_IMAGE_SUFFIX;
    } else {
        oldSnapshotPath = snapshotDirectory_ + bundleName_ + UNDERLINE_SEPARATOR +
            std::to_string(oldPersistentId) + std::to_string(key.first) +
            std::to_string(key.second) + IMAGE_SUFFIX;
    }
    int ret = std::rename(oldSnapshotPath.c_str(), snapshotPath.c_str());
    if (ret == 0) {
        TLOGNI(WmsLogTag::WMS_PATTERN, "Rename snapshot from %{public}s to %{public}s.",
            oldSnapshotPath.c_str(), snapshotPath.c_str());
    } else {
        TLOGNW(WmsLogTag::WMS_PATTERN, "Failed to rename snapshot from %{public}s to %{public}s.",
            oldSnapshotPath.c_str(), snapshotPath.c_str());
    }
}

std::string ScenePersistence::GetSnapshotFilePath(SnapshotStatus& key)
{
    if (hasSnapshot_[key.first][key.second]) {
        return snapshotPath_[key.first][key.second];
    }
    for (uint32_t orientation = SNAPSHOT_PORTRAIT; orientation < capacity_.second; orientation++) {
        if (hasSnapshot_[key.first][orientation]) {
            key.second = orientation;
            return snapshotPath_[key.first][orientation];
        }
    }
    for (uint32_t screen = SCREEN_UNKNOWN; screen < capacity_.first; screen++) {
        for (uint32_t orientation = SNAPSHOT_PORTRAIT; orientation < capacity_.second; orientation++) {
            if (hasSnapshot_[screen][orientation]) {
                key.second = orientation;
                return snapshotPath_[screen][orientation];
            }
        }
    }
    return snapshotPath_[SCREEN_UNKNOWN][SNAPSHOT_PORTRAIT];
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

    if (remove(updatedIconPath_.c_str())) {
        TLOGD(WmsLogTag::DEFAULT, "Failed to delete old file");
    }
    if (imagePacker.StartPacking(GetUpdatedIconPath(), option)) {
        TLOGE(WmsLogTag::DEFAULT, "Save updated icon failed, starting packing error");
        return;
    }
    if (imagePacker.AddImage(*pixelMap)) {
        TLOGE(WmsLogTag::DEFAULT, "Save updated icon failed, adding image error");
        return;
    }
    int64_t packedSize = 0;
    if (imagePacker.FinalizePacking(packedSize)) {
        TLOGE(WmsLogTag::DEFAULT, "Save updated icon failed, finalizing packing error");
        return;
    }
    TLOGD(WmsLogTag::DEFAULT, "SaveUpdatedIcon finished");
}

std::string ScenePersistence::GetUpdatedIconPath() const
{
    return updatedIconPath_;
}

std::pair<uint32_t, uint32_t> ScenePersistence::GetSnapshotSize(SnapshotStatus key) const
{
    return snapshotSize_[key.first][key.second];
}

void ScenePersistence::SetHasSnapshot(bool hasSnapshot, SnapshotStatus key)
{
    hasSnapshot_[key.first][key.second] = hasSnapshot;
}

bool ScenePersistence::HasSnapshot() const
{
    for (const auto& row : hasSnapshot_) {
        for (const auto& hasSnapshot : row) {
            if (hasSnapshot) {
                return true;
            }
        }
    }
    return false;
}

bool ScenePersistence::HasSnapshot(SnapshotStatus key) const
{
    return hasSnapshot_[key.first][key.second];
}

bool ScenePersistence::IsSnapshotExisted(SnapshotStatus key) const
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "IsSnapshotExisted");
    struct stat buf;
    if (stat(snapshotPath_[key.first][key.second].c_str(), &buf)) {
        TLOGD(WmsLogTag::WMS_PATTERN, "Snapshot file %{public}s does not exist",
            snapshotPath_[key.first][key.second].c_str());
        return false;
    }
    return S_ISREG(buf.st_mode);
}

std::shared_ptr<Media::PixelMap> ScenePersistence::GetLocalSnapshotPixelMap(const float oriScale,
    const float newScale, SnapshotStatus key) const
{
    if (!IsSnapshotExisted()) {
        TLOGE(WmsLogTag::WMS_PATTERN, "local snapshot pic is not existed");
        return nullptr;
    }

    uint32_t errorCode = 0;
    Media::SourceOptions sourceOpts;
    sourceOpts.formatHint = IsAstcEnabled() ? ASTC_IMAGE_FORMAT : IMAGE_FORMAT;
    std::lock_guard lock(savingSnapshotMutex_);
    auto imageSource = Media::ImageSource::CreateImageSource(snapshotPath_[key.first][key.second],
        sourceOpts, errorCode);
    if (!imageSource) {
        TLOGE(WmsLogTag::WMS_PATTERN, "create image source fail, errCode: %{public}u", errorCode);
        return nullptr;
    }

    Media::ImageInfo info;
    int32_t decoderWidth = 0;
    int32_t decoderHeight = 0;
    errorCode = imageSource->GetImageInfo(info);
    if (errorCode == Rosen::SUCCESS) {
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
