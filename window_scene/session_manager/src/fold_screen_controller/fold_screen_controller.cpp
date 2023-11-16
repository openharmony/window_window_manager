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

#include "fold_screen_controller/fold_screen_controller.h"
#include "fold_screen_controller/dual_display_device_policy.h"
#include "fold_screen_controller/fold_screen_sensor_manager.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "FoldScreenController"};
} // namespace

FoldScreenController::FoldScreenController()
{
    foldScreenPolicy_ = GetFoldScreenPolicy(DisplayDeviceType::DOUBLE_DISPLAY_DEVICE);
    if (foldScreenPolicy_ == nullptr) {
        WLOGE("FoldScreenPolicy is null");
        return;
    }
    FoldScreenSensorManager::GetInstance().SetFoldScreenPolicy(foldScreenPolicy_);
}

FoldScreenController::~FoldScreenController()
{
    WLOGFI("FoldScreenController is destructed");
}

sptr<FoldScreenPolicy> FoldScreenController::GetFoldScreenPolicy(DisplayDeviceType productType)
{
    sptr<FoldScreenPolicy> tempPolicy = nullptr;
    switch (productType) {
        case DisplayDeviceType::DOUBLE_DISPLAY_DEVICE: {
            tempPolicy = new DualDisplayDevicePolicy();
            break;
        }
        default: {
            WLOGE("GetFoldScreenPolicy DisplayDeviceType is invalid");
            break;
        }
    }

    return tempPolicy;
}

void FoldScreenController::SetDisplayMode(const FoldDisplayMode displayMode)
{
    if (foldScreenPolicy_ == nullptr) {
        WLOGW("SetDisplayMode: foldScreenPolicy_ is null");
        return;
    }
    foldScreenPolicy_->ChangeScreenDisplayMode(displayMode);
}

FoldDisplayMode FoldScreenController::GetDisplayMode()
{
    if (foldScreenPolicy_ == nullptr) {
        WLOGW("GetDisplayMode: foldScreenPolicy_ is null");
        return FoldDisplayMode::UNKNOWN;
    }
    return foldScreenPolicy_->GetScreenDisplayMode();
}

bool FoldScreenController::IsFoldable()
{
    return true;
}

FoldStatus FoldScreenController::GetFoldStatus()
{
    if (foldScreenPolicy_ == nullptr) {
        WLOGW("GetFoldStatus: foldScreenPolicy_ is null");
        return FoldStatus::UNKNOWN;
    }
    return foldScreenPolicy_->GetFoldStatus();
}

sptr<FoldCreaseRegion> FoldScreenController::GetCurrentFoldCreaseRegion()
{
    if (foldScreenPolicy_ == nullptr) {
        WLOGW("GetFoldStatus: foldScreenPolicy_ is null");
        return nullptr;
    }
    return foldScreenPolicy_->GetCurrentFoldCreaseRegion();
}

ScreenId FoldScreenController::GetCurrentScreenId()
{
    if (foldScreenPolicy_ == nullptr) {
        WLOGW("GetCurrentScreenId: foldScreenPolicy_ is null");
        return 0;
    }
    return foldScreenPolicy_->GetCurrentScreenId();
}
} // namespace OHOS::Rosen