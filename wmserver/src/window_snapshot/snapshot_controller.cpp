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

#include <chrono>
#include <hisysevent.h>
#include <hitrace_meter.h>
#include <sstream>

#include "surface_capture_future.h"
#include "surface_draw.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SnapshotController"};
    constexpr int REPORT_SHOW_WINDOW_TIMES = 50;
}

void SnapshotController::Init(sptr<WindowRoot>& root)
{
    windowRoot_ = root;
}

int32_t SnapshotController::GetSnapshot(const sptr<IRemoteObject> &token, Snapshot& snapshot)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:GetSnapshot");
    auto startTime = std::chrono::steady_clock::now();
    if (token == nullptr) {
        WLOGFE("Get snapshot failed, because token is null.");
        return static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
    }
    if (handler_ == nullptr) {
        WLOGFE("Get snapshot failed, because handler is null.");
        return static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
    }
    std::shared_ptr<Media::PixelMap> pixelMap;
    std::shared_ptr<RSSurfaceNode> surfaceNode;
    auto task = [this, &pixelMap, &surfaceNode, token] () {
        if (windowRoot_ == nullptr || token == nullptr) {
            surfaceNode = nullptr;
            WLOGFE("Get snapshot failed, because windowRoot is null.");
            return;
        }
        auto targetNode = windowRoot_->GetWindowNodeByAbilityToken(token);
        if (targetNode != nullptr) {
            pixelMap = targetNode->GetSnapshot();
            // reset window snapshot after use
            targetNode->SetSnapshot(nullptr);
            surfaceNode = targetNode->surfaceNode_;
        }
    };
    // post sync task to wms main handler
    if (handler_ == nullptr) {
        WLOGFE("Get window node failed, because window manager service handler is null");
        return static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
    }
    handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    if (surfaceNode == nullptr) {
        WLOGFE("Get window node failed, because surfaceNode is null");
        return static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
    }
    WMError res = WMError::WM_OK;
    if (pixelMap == nullptr) {
        // take surface snapshot, time out 2000ms
        res = SurfaceDraw::GetSurfaceSnapshot(surfaceNode, pixelMap, 2000) ? WMError::WM_OK : WMError::WM_ERROR_NULLPTR;
    }
    snapshot.SetPixelMap(pixelMap);
    if (res == WMError::WM_OK) {
        getSnapshotTimeConfig_.getSnapshotTimes_++;
        auto currentTime = std::chrono::steady_clock::now();
        int64_t costTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
        RecordGetSnapshotEvent(costTime);
    }
    return static_cast<int32_t>(res);
}

void SnapshotController::RecordGetSnapshotEvent(int64_t costTime)
{
    WLOGFI("get snapshot cost time(ms): %{public}" PRIu64", get snapshot times: %{public}u", costTime,
        getSnapshotTimeConfig_.getSnapshotTimes_.load());
    if (costTime <= 25) { // 20: means cost time is 25ms
        getSnapshotTimeConfig_.below25msTimes_++;
    } else if (costTime <= 35) { // 35: means cost time is 35ms
        getSnapshotTimeConfig_.below35msTimes_++;
    } else if (costTime <= 50) { // 50: means cost time is 50ms
        getSnapshotTimeConfig_.below50msTimes_++;
    } else if (costTime <= 200) { // 200: means cost time is 200ms
        getSnapshotTimeConfig_.below200msTimes_++;
    } else {
        getSnapshotTimeConfig_.above200msTimes_++;
    }
    if (getSnapshotTimeConfig_.getSnapshotTimes_ >= REPORT_SHOW_WINDOW_TIMES) {
        std::ostringstream oss;
        oss << "show window: " << "BELOW25(ms): " << getSnapshotTimeConfig_.below25msTimes_
            << ", BELOW35(ms):" << getSnapshotTimeConfig_.below35msTimes_
            << ", BELOW50(ms): " << getSnapshotTimeConfig_.below50msTimes_
            << ", BELOW200(ms): " << getSnapshotTimeConfig_.below200msTimes_
            << ", ABOVE50(ms): " << getSnapshotTimeConfig_.above200msTimes_ << ";";
        int32_t ret = OHOS::HiviewDFX::HiSysEvent::Write(
            OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
            "GET_SNAPSHOT_TIME",
            OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC,
            "MSG", oss.str());
        if (ret != 0) {
            WLOGFE("Write HiSysEvent error, ret:%{public}d", ret);
        } else {
            getSnapshotTimeConfig_.getSnapshotTimes_ = 0;
            getSnapshotTimeConfig_.below25msTimes_ = 0;
            getSnapshotTimeConfig_.below35msTimes_ = 0;
            getSnapshotTimeConfig_.below50msTimes_ = 0;
            getSnapshotTimeConfig_.below200msTimes_ = 0;
            getSnapshotTimeConfig_.above200msTimes_ = 0;
        }
    }
}
} // namespace Rosen
} // namespace OHOS