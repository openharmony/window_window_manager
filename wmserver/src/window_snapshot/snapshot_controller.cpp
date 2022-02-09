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
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_trace.h"

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
    std::shared_ptr<GetSurfaceCapture> callback = std::make_shared<GetSurfaceCapture>();
    rsInterface_.TakeSurfaceCapture(surfaceNode, callback);

    int counter = 0;
    while (!callback->IsPixelMapOk()) {
        usleep(10000); // 10000us equals to 10ms
        counter++;
        if (counter >= 200) { // wait for 200 * 10ms = 2s
            WLOGFE("Failed to get pixelmap, timeout");
            return WMError::WM_ERROR_NULLPTR;
        }
    }
    std::shared_ptr<Media::PixelMap> pixelMap = callback->GetPixelMap();

    if (pixelMap == nullptr) {
        WLOGFE("Failed to get pixelmap, return nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    snapshot.SetPixelMap(pixelMap);
    return WMError::WM_OK;
}

int32_t SnapshotController::GetSnapshot(const sptr<IRemoteObject> &token, Snapshot& snapshot)
{
    WM_SCOPED_TRACE("wms:GetSnapshot");
    if (token == nullptr) {
        WLOGFE("Get ailityToken failed!");
        return static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
    }
    std::shared_ptr<RSSurfaceNode> surfaceNode = windowRoot_->GetSurfaceNodeByAbilityToken(token);
    if (surfaceNode == nullptr) {
        WLOGFE("Get surfaceNode failed!");
        return static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
    }
    return static_cast<int32_t>(TakeSnapshot(surfaceNode, snapshot));
}
} // namespace Rosen
} // namespace OHOS