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
#include "fold_screen_controller/dual_display_policy.h"
#include "fold_screen_controller/fold_screen_sensor_manager.h"
#include "fold_screen_controller/sensor_fold_state_manager/single_display_sensor_fold_state_manager.h"
#include "fold_screen_controller/sensor_fold_state_manager/dual_display_sensor_fold_state_manager.h"
#include "fold_screen_state_internel.h"

#include "window_manager_hilog.h"

namespace OHOS::Rosen {
FoldScreenController::FoldScreenController(std::recursive_mutex& displayInfoMutex,
    std::shared_ptr<TaskScheduler> screenPowerTaskScheduler)
    : displayInfoMutex_(displayInfoMutex), screenPowerTaskScheduler_(screenPowerTaskScheduler)
{
    if (FoldScreenStateInternel::IsDualDisplayFoldDevice()) {
        foldScreenPolicy_ = GetFoldScreenPolicy(DisplayDeviceType::DOUBLE_DISPLAY_DEVICE);
        sensorFoldStateManager_ = new DualDisplaySensorFoldStateManager();
    } else if (FoldScreenStateInternel::IsSingleDisplayFoldDevice()) {
        foldScreenPolicy_ = GetFoldScreenPolicy(DisplayDeviceType::SINGLE_DISPLAY_DEVICE);
        sensorFoldStateManager_ = new SingleDisplaySensorFoldStateManager();
    }

    if (foldScreenPolicy_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "FoldScreenPolicy is null");
        return;
    }
#ifdef SENSOR_ENABLE
    FoldScreenSensorManager::GetInstance().SetFoldScreenPolicy(foldScreenPolicy_);
    FoldScreenSensorManager::GetInstance().SetSensorFoldStateManager(sensorFoldStateManager_);
#endif
}

FoldScreenController::~FoldScreenController()
{
    TLOGI(WmsLogTag::DMS, "FoldScreenController is destructed");
}

sptr<FoldScreenPolicy> FoldScreenController::GetFoldScreenPolicy(DisplayDeviceType productType)
{
    sptr<FoldScreenPolicy> tempPolicy = nullptr;
    switch (productType) {
        case DisplayDeviceType::SINGLE_DISPLAY_DEVICE: {
            tempPolicy = new DualDisplayDevicePolicy(displayInfoMutex_, screenPowerTaskScheduler_);
            break;
        }
        case DisplayDeviceType::DOUBLE_DISPLAY_DEVICE: {
            tempPolicy = new DualDisplayPolicy(displayInfoMutex_, screenPowerTaskScheduler_);
            break;
        }
        default: {
            TLOGE(WmsLogTag::DMS, "GetFoldScreenPolicy DisplayDeviceType is invalid");
            break;
        }
    }

    return tempPolicy;
}

void FoldScreenController::SetDisplayMode(const FoldDisplayMode displayMode)
{
    if (foldScreenPolicy_ == nullptr) {
        TLOGW(WmsLogTag::DMS, "SetDisplayMode: foldScreenPolicy_ is null");
        return;
    }
    foldScreenPolicy_->ChangeScreenDisplayMode(displayMode);
}

void FoldScreenController::LockDisplayStatus(bool locked)
{
    if (foldScreenPolicy_ == nullptr) {
        TLOGW(WmsLogTag::DMS, "LockDisplayStatus: foldScreenPolicy_ is null");
        return;
    }
    foldScreenPolicy_->LockDisplayStatus(locked);
}

FoldDisplayMode FoldScreenController::GetDisplayMode()
{
    if (foldScreenPolicy_ == nullptr) {
        TLOGW(WmsLogTag::DMS, "GetDisplayMode: foldScreenPolicy_ is null");
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
        TLOGW(WmsLogTag::DMS, "GetFoldStatus: foldScreenPolicy_ is null");
        return FoldStatus::UNKNOWN;
    }
    return foldScreenPolicy_->GetFoldStatus();
}

void FoldScreenController::SetFoldStatus(FoldStatus foldStatus)
{
    if (foldScreenPolicy_ == nullptr) {
        TLOGW(WmsLogTag::DMS, "SetFoldStatus: foldScreenPolicy_ is null");
        return;
    }
    foldScreenPolicy_->SetFoldStatus(foldStatus);
}

sptr<FoldCreaseRegion> FoldScreenController::GetCurrentFoldCreaseRegion()
{
    if (foldScreenPolicy_ == nullptr) {
        TLOGW(WmsLogTag::DMS, "GetFoldStatus: foldScreenPolicy_ is null");
        return nullptr;
    }
    return foldScreenPolicy_->GetCurrentFoldCreaseRegion();
}

ScreenId FoldScreenController::GetCurrentScreenId()
{
    if (foldScreenPolicy_ == nullptr) {
        TLOGW(WmsLogTag::DMS, "GetCurrentScreenId: foldScreenPolicy_ is null");
        return 0;
    }
    return foldScreenPolicy_->GetCurrentScreenId();
}

void FoldScreenController::SetOnBootAnimation(bool onBootAnimation)
{
    if (foldScreenPolicy_ == nullptr) {
        TLOGW(WmsLogTag::DMS, "SetOnBootAnimation: foldScreenPolicy_ is null");
        return;
    }
    foldScreenPolicy_->SetOnBootAnimation(onBootAnimation);
}

void FoldScreenController::UpdateForPhyScreenPropertyChange()
{
    if (foldScreenPolicy_ == nullptr) {
        TLOGW(WmsLogTag::DMS, "UpdateForPhyScreenPropertyChange: foldScreenPolicy_ is null");
        return;
    }
    foldScreenPolicy_->UpdateForPhyScreenPropertyChange();
}
} // namespace OHOS::Rosen