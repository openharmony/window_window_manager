/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "snapshot_controller.h"

#include <hitrace_meter.h>

#include "surface_capture_future.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SnapshotController"};
}

void SnapshotController::Init(sptr<WindowRoot>& root)
{
    windowRoot_ = root;
}

WMError SnapshotController::TakeSnapshot(const std::shared_ptr<RSSurfaceNode>& surfaceNode, Snapshot& snapshot)
{
    std::shared_ptr<SurfaceCaptureFuture> callback = std::make_shared<SurfaceCaptureFuture>();
    rsInterface_.TakeSurfaceCapture(surfaceNode, callback, scaleW, scaleH);
    std::shared_ptr<Media::PixelMap> pixelMap = callback->GetResult(2000); // wait for <= 2000ms
    if (pixelMap == nullptr) {
        WLOGFE("Failed to get pixelmap, return nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    snapshot.SetPixelMap(pixelMap);
    return WMError::WM_OK;
}

int32_t SnapshotController::GetSnapshot(const sptr<IRemoteObject> &token, Snapshot& snapshot)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:GetSnapshot");
    if (token == nullptr) {
        WLOGFE("Get snapshot failed, because token is null.");
        return static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
    }
    if (handler_ == nullptr) {
        WLOGFE("Get snapshot failed, because handler is null.");
        return static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
    }
    std::shared_ptr<RSSurfaceNode> surfaceNode;
    auto task = [this, &surfaceNode, token] () {
        if (windowRoot_ == nullptr || token == nullptr) {
            surfaceNode = nullptr;
            WLOGFE("Get snapshot failed, because windowRoot is null.");
            return;
        }
        surfaceNode = windowRoot_->GetSurfaceNodeByAbilityToken(token);
    };
    // post sync task to wms main handler
    handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    if (surfaceNode == nullptr) {
        WLOGFE("Get surfaceNode failed, because surfaceNode is null");
        return static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
    }
    return static_cast<int32_t>(TakeSnapshot(surfaceNode, snapshot));
}
} // namespace Rosen
} // namespace OHOS