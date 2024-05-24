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

#include "fold_screen_controller/fold_screen_policy.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
FoldScreenPolicy::FoldScreenPolicy() = default;
FoldScreenPolicy::~FoldScreenPolicy() = default;

void FoldScreenPolicy::ChangeScreenDisplayMode(FoldDisplayMode displayMode) {}
void FoldScreenPolicy::LockDisplayStatus(bool locked) { lockDisplayStatus_ = locked; }
void FoldScreenPolicy::SendSensorResult(FoldStatus foldStatus) {}
ScreenId FoldScreenPolicy::GetCurrentScreenId() { return screenId_; }
sptr<FoldCreaseRegion> FoldScreenPolicy::GetCurrentFoldCreaseRegion() { return currentFoldCreaseRegion_; }

void FoldScreenPolicy::SetOnBootAnimation(bool onBootAnimation)
{
    onBootAnimation_ = onBootAnimation;
}

void FoldScreenPolicy::UpdateForPhyScreenPropertyChange() {}

FoldDisplayMode FoldScreenPolicy::GetScreenDisplayMode()
{
    std::lock_guard<std::recursive_mutex> lock_mode(displayModeMutex_);
    return globalDisplayMode_;
}

FoldStatus FoldScreenPolicy::GetFoldStatus()
{
    return globalFoldStatus_;
}

void FoldScreenPolicy::SetFoldStatus(FoldStatus foldStatus)
{
    TLOGI(WmsLogTag::DMS, "SetFoldStatus FoldStatus: %{public}d", foldStatus);
    currentFoldStatus_ = foldStatus;
    globalFoldStatus_ = foldStatus;
}

void FoldScreenPolicy::ClearState()
{
    currentDisplayMode_ = FoldDisplayMode::UNKNOWN;
    currentFoldStatus_ = FoldStatus::UNKNOWN;
}
} // namespace OHOS::Rosen