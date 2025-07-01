/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ROSEN_WINDOW_SCENE_SECONDARY_DISPLAY_FOLD_POLICY_H
#define OHOS_ROSEN_WINDOW_SCENE_SECONDARY_DISPLAY_FOLD_POLICY_H

#include <refbase.h>

#include "common/include/task_scheduler.h"
#include "fold_screen_controller/fold_screen_policy.h"
#include "fold_screen_info.h"
#include "session/screen/include/screen_session.h"

namespace OHOS::Rosen {
class SecondaryDisplayFoldPolicy : public FoldScreenPolicy {
public:
    SecondaryDisplayFoldPolicy(std::recursive_mutex& displayInfoMutex,
        std::shared_ptr<TaskScheduler> screenPowerTaskScheduler);
    ~SecondaryDisplayFoldPolicy() = default;
    void ChangeScreenDisplayMode(FoldDisplayMode displayMode,
        DisplayModeChangeReason reason = DisplayModeChangeReason::DEFAULT) override;
    void SendSensorResult(FoldStatus foldStatus) override;
    sptr<FoldCreaseRegion> GetCurrentFoldCreaseRegion() override;
    void SetOnBootAnimation(bool onBootAnimation) override;
    FoldDisplayMode GetModeMatchStatus() override;
    std::vector<uint32_t> GetScreenParams() override;
    Drawing::Rect GetScreenSnapshotRect() override;
    void SetMainScreenRegion(DMRect& mainScreenRegion) override;
    void SetSecondaryDisplayModeChangeStatus(bool status) override;
private:
    void ChangeSuperScreenDisplayMode(sptr<ScreenSession> screenSession,
        FoldDisplayMode displayMode);
    void RecoverWhenBootAnimationExit();
    void ReportFoldDisplayModeChange(FoldDisplayMode displayMode);
    void SendPropertyChangeResult(sptr<ScreenSession> screenSession, ScreenId screenId,
        ScreenPropertyChangeReason reason, FoldDisplayMode displayMode);
    void SetStatusFullActiveRectAndTpFeature();
    void SetStatusMainActiveRectAndTpFeature();
    void SetStatusGlobalFullActiveRectAndTpFeature();
    void InitScreenParams();
    std::recursive_mutex& displayInfoMutex_;
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler_;
    std::vector<uint32_t> screenParams_;
    bool isChangeScreenWhenBootCompleted = false;
};
} // namespace OHOS::Rosen
#endif //OHOS_ROSEN_WINDOW_SCENE_SECONDARY_DISPLAY_FOLD_POLICY_H