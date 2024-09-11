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

#include "fold_screen_controller/sensor_fold_state_manager/dual_display_sensor_fold_state_manager.h"
#include <iservice_registry.h>
#include <parameters.h>
#include <refbase.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "app_mgr_client.h"
#include "app_service_manager.h"
#include "app_mgr_constants.h"
#include "app_mgr_interface.h"

#include "fold_screen_controller/sensor_fold_state_manager/sensor_fold_state_manager.h"
#include "session/screen/include/screen_session.h"
#include "screen_scene_config.h"
#include "iremote_object.h"
#include "window_manager_hilog.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {
using OHOS::AppExecFwk::AppStateData;
using OHOS::AppExecFwk::ApplicationState;
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DualDisplaySensorFoldStateManager"};
const float INWARD_FOLDED_THRESHOLD = static_cast<float>(system::GetIntParameter<int32_t>
    ("const.fold.folded_threshold", 85));
const float INWARD_EXPAND_THRESHOLD = static_cast<float>(system::GetIntParameter<int32_t>
    ("const.fold.expand_threshold", 145));
const float INWARD_HALF_FOLDED_MAX_THRESHOLD = static_cast<float>(system::GetIntParameter<int32_t>
    ("const.half_folded_max_threshold", 135));
const float INWARD_HALF_FOLDED_MIN_THRESHOLD = static_cast<float>(system::GetIntParameter<int32_t>
    ("const.fold.half_folded_min_threshold", 85));
constexpr int32_t HALL_THRESHOLD = 1;
constexpr int32_t HALL_FOLDED_THRESHOLD = 0;
constexpr float ANGLE_MIN_VAL = 0.0F;
constexpr float INWARD_FOLDED_LOWER_THRESHOLD = 10.0F;
constexpr float INWARD_FOLDED_UPPER_THRESHOLD = 20.0F;
constexpr float ANGLE_BUFFER = 0.001F;
} // namespace

DualDisplaySensorFoldStateManager::DualDisplaySensorFoldStateManager()
{
    auto stringListConfig = ScreenSceneConfig::GetStringListConfig();
    if (stringListConfig.count("hallSwitchApp") != 0) {
        hallSwitchPackageNameList_ = stringListConfig["hallSwitchApp"];
    }
}

DualDisplaySensorFoldStateManager::~DualDisplaySensorFoldStateManager() {}

void DualDisplaySensorFoldStateManager::HandleAngleChange(float angle, int hall,
    sptr<FoldScreenPolicy> foldScreenPolicy)
{
    if (std::islessequal(angle, INWARD_FOLDED_THRESHOLD + ANGLE_BUFFER) && hall == HALL_THRESHOLD) {
        return;
    }
    if (std::isless(angle, ANGLE_MIN_VAL + ANGLE_BUFFER)) {
        return;
    }
    if (hall == HALL_FOLDED_THRESHOLD) {
        angle = ANGLE_MIN_VAL;
    }
    FoldStatus nextState = GetNextFoldState(angle, hall);
    HandleSensorChange(nextState, angle, foldScreenPolicy);
}

void DualDisplaySensorFoldStateManager::HandleHallChange(float angle, int hall,
    sptr<FoldScreenPolicy> foldScreenPolicy)
{
    if (applicationStateObserver_ != nullptr && hall == HALL_THRESHOLD
        && PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        if (std::count(hallSwitchPackageNameList_.begin(), hallSwitchPackageNameList_.end(),
            applicationStateObserver_->GetForegroundApp())) {
            isHallSwitchApp_ = false;
            return;
        }
    }
    if (hall == HALL_THRESHOLD) {
        angle = INWARD_HALF_FOLDED_MIN_THRESHOLD + 1.0f;
    }
    FoldStatus nextState = GetNextFoldState(angle, hall);
    HandleSensorChange(nextState, angle, foldScreenPolicy);
}

FoldStatus DualDisplaySensorFoldStateManager::GetNextFoldState(float angle, int hall)
{
    FoldStatus state = GetCurrentState();
    // EXPAND
    if (std::isgreaterequal(angle, INWARD_EXPAND_THRESHOLD + ANGLE_BUFFER)) {
        isHallSwitchApp_ = true;
        return FoldStatus::EXPAND;
    }
    // FOLDED
    if (std::islessequal(angle, INWARD_FOLDED_LOWER_THRESHOLD + ANGLE_BUFFER)) {
        isHallSwitchApp_ = true;
        return FoldStatus::FOLDED;
    }
    // HALF_FOLD
    if (isHallSwitchApp_) {
        ProcessHalfFoldState(state, angle, INWARD_FOLDED_UPPER_THRESHOLD, INWARD_HALF_FOLDED_MAX_THRESHOLD);
    } else {
        ProcessHalfFoldState(state, angle, INWARD_HALF_FOLDED_MIN_THRESHOLD, INWARD_HALF_FOLDED_MAX_THRESHOLD);
        isHallSwitchApp_ = true;
    }
    return state;
}
void DualDisplaySensorFoldStateManager::ProcessHalfFoldState(FoldStatus& state, float angle,
    float halfFoldMinThreshold, float halfFoldMaxThreshold)
{
    if (std::isgreaterequal(angle, halfFoldMinThreshold + ANGLE_BUFFER)
        && std::islessequal(angle, halfFoldMaxThreshold + ANGLE_BUFFER)) {
        state = FoldStatus::HALF_FOLD;
    }
}

void DualDisplaySensorFoldStateManager::RegisterApplicationStateObserver()
{
    applicationStateObserver_ = new (std::nothrow) ApplicationStateObserver();
    if (applicationStateObserver_ == nullptr) {
        WLOGFE("applicationStateObserver_ is nullptr.");
        return;
    }
    auto appMgrClient_ = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance();
    if (appMgrClient_ == nullptr) {
        WLOGFE("appMgrClient_ is nullptr.");
        return;
    }
    int32_t flag =
        appMgrClient_->RegisterApplicationStateObserver(applicationStateObserver_, hallSwitchPackageNameList_);
    if (flag != ERR_OK) {
        WLOGFE("Register app state listener failed, flag = %{public}d", flag);
    } else {
        WLOGFI("Register app state listener success.");
    }
}

ApplicationStateObserver::ApplicationStateObserver() {}

void ApplicationStateObserver::OnForegroundApplicationChanged(const AppStateData &appStateData)
{
    if (appStateData.state == static_cast<int32_t>(ApplicationState::APP_STATE_FOREGROUND)) {
        foregroundBundleName_ = appStateData.bundleName;
        WLOGFI("APP_STATE_FOREGROUND, packageName= %{public}s", foregroundBundleName_.c_str());
    }
    if (appStateData.state == static_cast<int32_t>(ApplicationState::APP_STATE_BACKGROUND)
        && foregroundBundleName_.compare(appStateData.bundleName) == 0) {
        foregroundBundleName_ = "" ;
    }
}

std::string ApplicationStateObserver::GetForegroundApp()
{
    return foregroundBundleName_;
}
} // namespace OHOS::Rosen