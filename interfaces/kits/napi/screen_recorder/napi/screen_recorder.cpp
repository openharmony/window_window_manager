/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "screen_recorder.h"

#include "event_runner.h"
#include "screen_info.h"
#include "snapshot_utils.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "ScreenRecorder"};
    constexpr int64_t RECORDER_INTERVAL = 16; // 16ms
    const std::string FILE_NAME = "storage/media/100/local/files/Pictures/pic";
}

ScreenRecorder::ScreenRecorder(sptr<Screen> screen) : screen_(screen)
{
    surfaceReaderHandler_ = new SurfaceReaderHandlerImpl();
    surfaceReader_.SetHandler(surfaceReaderHandler_);
    surfaceReader_.Init();
    auto runner = AppExecFwk::EventRunner::Create("ScreenRecorder_handler");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
}

ScreenRecorder::~ScreenRecorder()
{
}

ScreenId ScreenRecorder::GetId() const
{
    WLOGFE("GetId");
    if (screen_ == nullptr) {
        WLOGFE("GetId is invalid");
        return SCREEN_ID_INVALID;
    }
    return screen_->GetId();
}

sptr<Surface> ScreenRecorder::GetInputSurface() const
{
    WLOGFE("GetInputSurface");
    return surfaceReader_.GetSurface();
}

DMError ScreenRecorder::Start(std::vector<int> fds)
{
    WLOGFE("Start");
    if (screen_ == nullptr) {
        WLOGFE("Start is invalid");
        return DMError::DM_ERROR_NULLPTR;
    }
    if (isStarting_) {
        WLOGFE("Start failed, is Starting");
        return DMError::DM_ERROR_INVALID_MODE_ID;
    }
    isStarting_ = true;
    stop_ = false;
    fileIndex_ = 0;
    fds_ = fds;
    auto task = [this] {
        Record();
    };
    handler_->PostTask(task, AppExecFwk::EventQueue::Priority::HIGH);
    return DMError::DM_OK;
}

void ScreenRecorder::Record()
{
    WLOGFE("Record");
    if (stop_ || fileIndex_ >= fds_.size()) {
        isStarting_ = false;
        return;
    }
    auto task = [this] {
        Record();
    };
    if (surfaceReaderHandler_->IsImageOk()) {
        WLOGFE("WriteToPngWithPixelMap");
        auto pixelMap = surfaceReaderHandler_->GetPixelMap();
        if (pixelMap != nullptr) {
            auto start = std::chrono::steady_clock::now();
            bool ret = SnapShotUtils::WriteToPngWithPixelMap(fds_[fileIndex_], *pixelMap);
            if (ret) {
                WLOGFE("snapshot write to %{public}s as png", (FILE_NAME + std::to_string(fileIndex_)).c_str());
            } else {
                WLOGFE("snapshot write to %{public}s failed", (FILE_NAME + std::to_string(fileIndex_)).c_str());
            }
            auto end = std::chrono::steady_clock::now();
            int64_t cost = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            WLOGFE("WriteToPngWithPixelMap cost:%{public}" PRId64"", cost);
        }
        surfaceReaderHandler_->ResetFlag();
        fileIndex_++;
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::HIGH);
    } else {
        handler_->PostTask(task, RECORDER_INTERVAL, AppExecFwk::EventQueue::Priority::HIGH);
    }
}

DMError ScreenRecorder::Stop()
{
    WLOGFE("Stop");
    if (screen_ == nullptr) {
        WLOGFE("Stop is invalid");
        return DMError::DM_ERROR_NULLPTR;
    }
    stop_ = true;
    return DMError::DM_OK;
}
} // namespace OHOS::Rosen