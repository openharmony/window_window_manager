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

#include "fold_screen_controller/sensor_fold_state_manager/sensor_fold_state_manager.h"
#include <hisysevent.h>
#include <chrono>

#include "fold_screen_controller/fold_screen_policy.h"
#include "fold_screen_state_internel.h"
#include "screen_session_manager/include/screen_session_manager.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SensorFoldStateManager"};
} // namespace

SensorFoldStateManager::SensorFoldStateManager() = default;
SensorFoldStateManager::~SensorFoldStateManager() = default;

void SensorFoldStateManager::HandleAngleChange(float angle, int hall, sptr<FoldScreenPolicy> foldScreenPolicy) {}

void SensorFoldStateManager::HandleHallChange(float angle, int hall, sptr<FoldScreenPolicy> foldScreenPolicy) {}

void SensorFoldStateManager::HandleSensorChange(FoldStatus nextState, float angle,
    sptr<FoldScreenPolicy> foldScreenPolicy)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (nextState == FoldStatus::UNKNOWN) {
        WLOGFW("fold state is UNKNOWN");
        return;
    }
    if (mState_ == nextState) {
        WLOGFI("fold state doesn't change, foldState = %{public}d.", mState_);
        return;
    }
    WLOGFI("current state: %{public}d, next state: %{public}d.", mState_, nextState);
    ReportNotifyFoldStatusChange(static_cast<int32_t>(mState_), static_cast<int32_t>(nextState), angle);

    NotifyReportFoldStatusToScb(mState_, nextState, angle);
    
    mState_ = nextState;
    if (foldScreenPolicy != nullptr) {
        foldScreenPolicy->SetFoldStatus(mState_);
    }
    ScreenSessionManager::GetInstance().NotifyFoldStatusChanged(mState_);
    if (foldScreenPolicy != nullptr && foldScreenPolicy->lockDisplayStatus_ != true) {
        foldScreenPolicy->SendSensorResult(mState_);
    }
}

FoldStatus SensorFoldStateManager::GetCurrentState()
{
    return mState_;
}

void SensorFoldStateManager::ReportNotifyFoldStatusChange(int32_t currentStatus, int32_t nextStatus,
    float postureAngle)
{
    WLOGI("ReportNotifyFoldStatusChange currentStatus: %{public}d, nextStatus: %{public}d, postureAngle: %{public}f",
        currentStatus, nextStatus, postureAngle);
    int32_t ret = HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "NOTIFY_FOLD_STATE_CHANGE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "CURRENT_FOLD_STATUS", currentStatus,
        "NEXT_FOLD_STATUS", nextStatus,
        "SENSOR_POSTURE", postureAngle);
    if (ret != 0) {
        WLOGE("ReportNotifyFoldStatusChange Write HiSysEvent error, ret: %{public}d", ret);
    }
}

void SensorFoldStateManager::ClearState(sptr<FoldScreenPolicy> foldScreenPolicy)
{
    mState_ = FoldStatus::UNKNOWN;
    foldScreenPolicy->ClearState();
}

void SensorFoldStateManager::RegisterApplicationStateObserver() {}


void SensorFoldStateManager::NotifyReportFoldStatusToScb(FoldStatus currentStatus, FoldStatus nextStatus,
    float postureAngle)
{
    std::chrono::time_point<std::chrono::system_clock> timeNow = std::chrono::system_clock::now();
    int32_t duration = static_cast<int32_t>(
        std::chrono::duration_cast<std::chrono::seconds>(timeNow - mLastStateClock_).count());
    mLastStateClock_ = timeNow;

    std::vector<std::string> screenFoldInfo {std::to_string(static_cast<int32_t>(currentStatus)),
        std::to_string(static_cast<int32_t>(nextStatus)), std::to_string(duration), std::to_string(postureAngle)};
    ScreenSessionManager::GetInstance().ReportFoldStatusToScb(screenFoldInfo);
}
} // namespace OHOS::Rosen