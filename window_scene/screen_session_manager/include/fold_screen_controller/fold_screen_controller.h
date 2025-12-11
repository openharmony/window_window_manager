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

#ifndef OHOS_ROSEN_WINDOW_SCENE_FOLD_SCREEN_CONTROLLER_H
#define OHOS_ROSEN_WINDOW_SCENE_FOLD_SCREEN_CONTROLLER_H

#include <refbase.h>

#include "common/include/task_scheduler.h"
#include "fold_screen_controller/fold_screen_policy.h"
#include "fold_screen_controller/fold_screen_state_machine.h"
#include "fold_screen_controller/sensor_fold_state_manager/sensor_fold_state_manager.h"
#include "fold_screen_info.h"

namespace OHOS::Rosen {
enum class DisplayDeviceType :uint32_t {
    DISPLAY_DEVICE_UNKNOWN = 0,
    SINGLE_DISPLAY_DEVICE,
    DOUBLE_DISPLAY_DEVICE,
    SINGLE_DISPLAY_POCKET_DEVICE,
    SECONDARY_DISPLAY_DEVICE,
    SINGLE_DISPLAY_SUPER_DEVICE,
};

class FoldScreenController : public RefBase {
public:
    FoldScreenController(std::recursive_mutex& displayInfoMutex,
        std::shared_ptr<TaskScheduler> screenPowerTaskScheduler);
    virtual ~FoldScreenController();

    void BootAnimationFinishPowerInit();
    void SetDisplayMode(const FoldDisplayMode displayMode);
    void RecoverDisplayMode();
    FoldDisplayMode GetDisplayMode();
    bool IsFoldable();
    FoldStatus GetFoldStatus();
    bool GetTentMode();
    FoldDisplayMode GetModeMatchStatus();
    void SetFoldStatus(FoldStatus foldStatus);
    void OnTentModeChanged(int tentType, int32_t hall = -1);
    sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion();
    FoldCreaseRegion GetLiveCreaseRegion() const;
    ScreenId GetCurrentScreenId();
    void LockDisplayStatus(bool locked);
    void SetOnBootAnimation(bool onBootAnimation);
    void UpdateForPhyScreenPropertyChange();
    void ExitCoordination();
    Drawing::Rect GetScreenSnapshotRect();
    void SetMainScreenRegion(DMRect& mainScreenRegion);
    std::chrono::steady_clock::time_point GetStartTimePoint();
    bool GetIsFirstFrameCommitReported();
    void SetIsFirstFrameCommitReported(bool isFirstFrameCommitReported);
    /*
     *    Avoid fold to expand process queues public interface
     */
    bool GetModeChangeRunningStatus();
    void SetdisplayModeChangeStatus(bool status);
    bool GetdisplayModeRunningStatus();
    FoldDisplayMode GetLastCacheDisplayMode();
    void AddOrRemoveDisplayNodeToTree(ScreenId screenId, int32_t command);
    void SetIsClearingBootAnimation(bool isClearingBootAnimation);
    nlohmann::ordered_json GetFoldCreaseRegionJson();
    void NotifyRunSensorFoldStateManager();
private:
    sptr<FoldScreenPolicy> GetFoldScreenPolicy(DisplayDeviceType productType);
    sptr<FoldScreenPolicy> foldScreenPolicy_;
    sptr<SensorFoldStateManager> sensorFoldStateManager_;
    std::recursive_mutex& displayInfoMutex_;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler_;
    std::vector<FoldCreaseRegionItem> foldCreaseRegionItems_;
};
} // namespace OHOS::Rosen
#endif //OHOS_ROSEN_WINDOW_SCENE_FOLD_SCREEN_CONTROLLER_H