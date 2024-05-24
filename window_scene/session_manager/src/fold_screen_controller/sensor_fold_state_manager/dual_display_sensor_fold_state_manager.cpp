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
#include "app_state_data.h"
#include "ability_state_data.h"
#include "process_data.h"
#include "fold_screen_controller/fold_screen_policy.h"

#include "fold_screen_controller/sensor_fold_state_manager/dual_display_sensor_fold_state_manager.h"
#include "fold_screen_controller/sensor_fold_state_manager/sensor_fold_state_manager.h"
#include "session/screen/include/screen_session.h"
#include "screen_scene_config.h"
#include "singleton.h"
#include "singleton_container.h"

#include "iremote_object.h"
#include "window_manager_hilog.h"

#ifdef POWER_MANAGER_ENABLE
#include <power_mgr_client.h>
#endif

namespace OHOS::Rosen {
using OHOS::AppExecFwk::AppStateData;
using OHOS::AppExecFwk::ApplicationState;
namespace {
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
} // namespace

DualDisplaySensorFoldStateManager::DualDisplaySensorFoldStateManager()
{
    auto stringListConfig = ScreenSceneConfig::GetStringListConfig();
    if (stringListConfig.count("hallSwitchApp") != 0) {
        packageNames_ = stringListConfig["hallSwitchApp"];
    }
}

DualDisplaySensorFoldStateManager::~DualDisplaySensorFoldStateManager() {}

void DualDisplaySensorFoldStateManager::HandleAngleChange(float angle, int hall,
    sptr<FoldScreenPolicy> foldScreenPolicy)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (std::islessequal(angle, INWARD_FOLDED_THRESHOLD) && hall == HALL_THRESHOLD) {
        return;
    }
    if (std::isless(angle, ANGLE_MIN_VAL)) {
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
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (applicationStateObserver_ != nullptr && hall == HALL_THRESHOLD
        && PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        if (std::count(packageNames_.begin(), packageNames_.end(), applicationStateObserver_->GetForegroundApp())) {
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
    if (std::isgreaterequal(angle, INWARD_EXPAND_THRESHOLD)) {
        isHallSwitchApp_ = true;
        return FoldStatus::EXPAND;
    }
    if (std::islessequal(angle, INWARD_FOLDED_LOWER_THRESHOLD)) {
        isHallSwitchApp_ = true;
        return FoldStatus::FOLDED;
    }
    if (isHallSwitchApp_) {
        if (std::isgreaterequal(angle, INWARD_FOLDED_UPPER_THRESHOLD)
            && std::islessequal(angle, INWARD_HALF_FOLDED_MAX_THRESHOLD)) {
            isHallSwitchApp_ = true;
            return FoldStatus::HALF_FOLD;
        }
    } else {
        if (std::isgreaterequal(angle, INWARD_HALF_FOLDED_MIN_THRESHOLD)
            && std::islessequal(angle, INWARD_HALF_FOLDED_MAX_THRESHOLD)) {
            isHallSwitchApp_ = true;
            return FoldStatus::HALF_FOLD;
        }
    }
    return state;
}

void DualDisplaySensorFoldStateManager::RegisterApplicationStateObserver()
{
    applicationStateObserver_ = new (std::nothrow) ApplicationStateObserver();
    auto appMgrClient_ = DelayedSingleton<AppExecFwk::AppMgrClient>::GetInstance();
    if (appMgrClient_ == nullptr) {
        TLOGE(WmsLogTag::DMS, "appMgrClient_ is nullptr.");
    } else {
        auto flag = static_cast<int32_t>(
            appMgrClient_->RegisterApplicationStateObserver(applicationStateObserver_, packageNames_));
        if (flag != ERR_OK) {
            TLOGE(WmsLogTag::DMS, "Register app debug listener failed.");
        } else {
            TLOGI(WmsLogTag::DMS, "Register app debug listener success.");
        }
    }
}

ApplicationStateObserver::ApplicationStateObserver() {}

void ApplicationStateObserver::OnForegroundApplicationChanged(const AppStateData &appStateData)
{
    if (appStateData.state == static_cast<int32_t>(ApplicationState::APP_STATE_FOREGROUND)) {
        foregroundBundleName_ = appStateData.bundleName;
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