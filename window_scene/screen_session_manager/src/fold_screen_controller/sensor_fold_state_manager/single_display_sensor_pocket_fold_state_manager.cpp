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

#include <parameters.h>

#include "fold_screen_controller/fold_screen_policy.h"
#include "fold_screen_controller/sensor_fold_state_manager/single_display_sensor_pocket_fold_state_manager.h"
#include "fold_screen_controller/sensor_fold_state_manager/sensor_fold_state_manager.h"
#include "session/screen/include/screen_session.h"
#include "screen_scene_config.h"

#include "window_manager_hilog.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {
using OHOS::AppExecFwk::AppStateData;
using OHOS::AppExecFwk::ApplicationState;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SingleDisplaySensorPocketFoldStateManager"};
constexpr float ANGLE_MIN_VAL = 0.0F;
constexpr float ALTA_HALF_FOLDED_MAX_THRESHOLD = 140.0F;
constexpr float CLOSE_ALTA_HALF_FOLDED_MIN_THRESHOLD = 70.0F;
constexpr float OPEN_ALTA_HALF_FOLDED_MIN_THRESHOLD = 25.0F;
constexpr float ALTA_HALF_FOLDED_BUFFER = 10.0F;
constexpr float LARGER_BOUNDARY_FOR_ALTA_THRESHOLD = 90.0F;
constexpr float CAMERA_MAX_VAL = 100.0F;
constexpr int32_t LARGER_BOUNDARY_FLAG = 1;
constexpr int32_t SMALLER_BOUNDARY_FLAG = 0;
constexpr int32_t HALL_THRESHOLD = 1;
constexpr int32_t HALL_FOLDED_THRESHOLD = 0;
constexpr float TENT_MODE_EXIT_MIN_THRESHOLD = 5.0F;
constexpr float TENT_MODE_EXIT_MAX_THRESHOLD = 175.0F;
} // namespace

SingleDisplaySensorPocketFoldStateManager::SingleDisplaySensorPocketFoldStateManager()
{
    auto stringListConfig = ScreenSceneConfig::GetStringListConfig();
    if (stringListConfig.count("hallSwitchApp") != 0) {
        hallSwitchPackageNameList_ = stringListConfig["hallSwitchApp"];
    }
}
SingleDisplaySensorPocketFoldStateManager::~SingleDisplaySensorPocketFoldStateManager() {}

void SingleDisplaySensorPocketFoldStateManager::HandleAngleChange(float angle, int hall,
    sptr<FoldScreenPolicy> foldScreenPolicy)
{
    SetCameraFoldStrategy(angle);
    if (isInCameraFoldStrategy_) {
        HandleSensorChange(FoldStatus::FOLDED, angle, foldScreenPolicy);
        return;
    }
    if (IsTentMode()) {
        return TentModeHandleSensorChange(angle, hall, foldScreenPolicy);
    }
    FoldStatus nextState = GetNextFoldState(angle, hall);
    HandleSensorChange(nextState, angle, foldScreenPolicy);
}

void SingleDisplaySensorPocketFoldStateManager::HandleHallChange(float angle, int hall,
    sptr<FoldScreenPolicy> foldScreenPolicy)
{
    SetCameraFoldStrategy(angle);
    if (isInCameraFoldStrategy_) {
        HandleSensorChange(FoldStatus::FOLDED, angle, foldScreenPolicy);
        return;
    }
    if (IsTentMode()) {
        return TentModeHandleSensorChange(angle, hall, foldScreenPolicy);
    }
    FoldStatus nextState = GetNextFoldState(angle, hall);
    HandleSensorChange(nextState, angle, foldScreenPolicy);
}

void SingleDisplaySensorPocketFoldStateManager::UpdateSwitchScreenBoundaryForLargeFoldDevice(float angle, int hall)
{
    if (hall == HALL_FOLDED_THRESHOLD) {
        allowUserSensorForLargeFoldDevice = SMALLER_BOUNDARY_FLAG;
    } else if (angle >= LARGER_BOUNDARY_FOR_ALTA_THRESHOLD) {
        allowUserSensorForLargeFoldDevice = LARGER_BOUNDARY_FLAG;
    }
}

void SingleDisplaySensorPocketFoldStateManager::SetCameraFoldStrategy(float angle)
{
    FoldStatus currentState = GetCurrentState();
    std::string cameraApp = "com.huawei.hmos.camera";

    if (angle >= CAMERA_MAX_VAL) {
        if (isInCameraFoldStrategy_ != false) {
            isInCameraFoldStrategy_ = false;
            TLOGI(WmsLogTag::DMS, "Disable CameraFoldStrategy.");
        }
        return;
    }
    if (applicationStateObserver_ == nullptr) {
        return;
    }
    if (applicationStateObserver_->GetForegroundApp().empty()) {
        return;
    }
    if (applicationStateObserver_->GetForegroundApp() == cameraApp && currentState == FoldStatus::FOLDED) {
        if (isInCameraFoldStrategy_ != true) {
            isInCameraFoldStrategy_ = true;
            TLOGI(WmsLogTag::DMS, "Enable CameraFoldStrategy.");
        }
    }
}

FoldStatus SingleDisplaySensorPocketFoldStateManager::GetNextFoldState(float angle, int hall)
{
    UpdateSwitchScreenBoundaryForLargeFoldDevice(angle, hall);
    FoldStatus currentState = GetCurrentState();
    if (std::isless(angle, ANGLE_MIN_VAL)) {
        return currentState;
    }
    FoldStatus state;

    if (allowUserSensorForLargeFoldDevice == SMALLER_BOUNDARY_FLAG) {
        if (hall == HALL_FOLDED_THRESHOLD) {
            state = FoldStatus::FOLDED;
        } else if (std::islessequal(angle, ALTA_HALF_FOLDED_MAX_THRESHOLD - ALTA_HALF_FOLDED_BUFFER) &&
            hall == HALL_THRESHOLD) {
            state = FoldStatus::HALF_FOLD;
        } else if (std::isgreaterequal(angle, ALTA_HALF_FOLDED_MAX_THRESHOLD)) {
            state = FoldStatus::EXPAND;
        } else {
            state = currentState;
            if (state == FoldStatus::UNKNOWN) {
                state = FoldStatus::HALF_FOLD;
            }
        }
        return state;
    }

    if (hall == HALL_THRESHOLD && angle == OPEN_ALTA_HALF_FOLDED_MIN_THRESHOLD) {
        state = currentState;
    } else if (std::islessequal(angle, CLOSE_ALTA_HALF_FOLDED_MIN_THRESHOLD)) {
        state = FoldStatus::FOLDED;
    } else if (std::islessequal(angle, ALTA_HALF_FOLDED_MAX_THRESHOLD - ALTA_HALF_FOLDED_BUFFER) &&
        std::isgreater(angle, CLOSE_ALTA_HALF_FOLDED_MIN_THRESHOLD + ALTA_HALF_FOLDED_BUFFER)) {
        state = FoldStatus::HALF_FOLD;
    } else if (std::isgreaterequal(angle, ALTA_HALF_FOLDED_MAX_THRESHOLD)) {
        state = FoldStatus::EXPAND;
    } else {
        state = currentState;
        if (state == FoldStatus::UNKNOWN) {
            state = FoldStatus::HALF_FOLD;
        }
    }
    return state;
}

void SingleDisplaySensorPocketFoldStateManager::RegisterApplicationStateObserver()
{
    applicationStateObserver_ = new (std::nothrow) ApplicationStatePocketObserver();
    auto appMgrClient_ = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance();
    if (applicationStateObserver_ == nullptr) {
        TLOGI(WmsLogTag::DMS, "applicationStateObserver_ is nullptr.");
        return;
    }
    if (appMgrClient_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "appMgrClient_ is nullptr.");
    } else {
        auto flag = static_cast<int32_t>(
            appMgrClient_->RegisterApplicationStateObserver(applicationStateObserver_, hallSwitchPackageNameList_));
        if (flag != ERR_OK) {
            TLOGE(WmsLogTag::DMS, "Register app debug listener failed.");
        } else {
            TLOGI(WmsLogTag::DMS, "Register app debug listener success.");
        }
    }
}
void SingleDisplaySensorPocketFoldStateManager::HandleTentChange(bool isTent, sptr<FoldScreenPolicy> foldScreenPolicy)
{
    bool isNotRepeated = isTent ^ IsTentMode();
    if (!isNotRepeated) {
        WLOGI("Repeat reporting tent mode:%{public}s, no processing", (isTent == true) ? "on" : "off");
    }

    SetTentMode(isTent);
    if (foldScreenPolicy == nullptr) {
        WLOGE("foldScreenPolicy is nullptr");
        return;
    }

    if (isTent) {
        FoldStatus currentState = GetCurrentState();
        foldScreenPolicy->ChangeOnTentMode(currentState);
    } else {
        foldScreenPolicy->ChangeOffTentMode();
    }
}

bool SingleDisplaySensorPocketFoldStateManager::TriggerTentExit(float angle, int hall)
{
    if (hall == HALL_FOLDED_THRESHOLD) {
        WLOGI("Exit tent mode due to hall sensor report folded");
        return true;
    }
    
    if (std::isless(angle, TENT_MODE_EXIT_MIN_THRESHOLD) || std::isgreater(angle, TENT_MODE_EXIT_MAX_THRESHOLD)) {
        WLOGI("Exit tent mode due to angle sensor report angle:%{public}f", angle);
        return true;
    }

    return false;
}

void SingleDisplaySensorPocketFoldStateManager::TentModeHandleSensorChange(float angle, int hall,
    sptr<FoldScreenPolicy> foldScreenPolicy)
{
    if (TriggerTentExit(angle, hall)) {
        FoldStatus nextState = GetNextFoldState(angle, hall);
        HandleSensorChange(nextState, angle, foldScreenPolicy);
        SetTentMode(false);
    }
}

ApplicationStatePocketObserver::ApplicationStatePocketObserver() {}

void ApplicationStatePocketObserver::OnForegroundApplicationChanged(const AppStateData &appStateData)
{
    if (appStateData.state == static_cast<int32_t>(ApplicationState::APP_STATE_FOREGROUND)) {
        foregroundBundleName_ = appStateData.bundleName;
    }
    if (appStateData.state == static_cast<int32_t>(ApplicationState::APP_STATE_BACKGROUND)
        && foregroundBundleName_.compare(appStateData.bundleName) == 0) {
        foregroundBundleName_ = "" ;
    }
}

std::string ApplicationStatePocketObserver::GetForegroundApp()
{
    return foregroundBundleName_;
}
} // namespace OHOS::Rosen